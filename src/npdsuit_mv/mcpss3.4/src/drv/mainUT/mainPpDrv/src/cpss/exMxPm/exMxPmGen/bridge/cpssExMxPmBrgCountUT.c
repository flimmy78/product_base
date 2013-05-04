/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmBrgCountUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmBrgCount, that provides
*       CPSS ExMxPm Bridge Counters facility API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgCount.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define BRG_COUNT_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define BRG_COUNT_INVALID_ENUM_CNS    0x5AAAAAA5

/* Tests use this vlan id for testing VLAN functions */
#define BRG_COUNT_TESTED_VLAN_ID_CNS  100

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntLearnedEntryDiscardGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *countValuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntLearnedEntryDiscardGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null countValuePtr. 
    Expected: GT_OK.
    1.2. Call with countValuePtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      countValue = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-null countValuePtr.
            Expected: GT_OK.
        */
        /* The function is not supported for EXMXPM */
        st = cpssExMxPmBrgCntLearnedEntryDiscardGet(dev, &countValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null countValuePtr [NULL]. 
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgCntLearnedEntryDiscardGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, countValuePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntLearnedEntryDiscardGet(dev, &countValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntLearnedEntryDiscardGet(dev, &countValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntDropCntrModeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMXPM_BRG_DROP_CNTR_MODE_ENT  dropMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntDropCntrModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with dropMode [CPSS_EXMXPM_BRG_DROP_CNTR_VLAN_RANGE_E /
                             CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_IPV6_IPM_E /
                             CPSS_EXMXPM_BRG_DROP_CNTR_SECURE_LEARNING_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgCntDropCntrModeGet with non-NULL dropModePtr.
    Expected: GT_OK and the same dropMode.
    1.3. Call with out of range dropMode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_DROP_CNTR_MODE_ENT  dropMode    = CPSS_EXMXPM_BRG_DROP_CNTR_VLAN_RANGE_E;
    CPSS_EXMXPM_BRG_DROP_CNTR_MODE_ENT  dropModeGet = CPSS_EXMXPM_BRG_DROP_CNTR_VLAN_RANGE_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with dropMode [CPSS_EXMXPM_BRG_DROP_CNTR_VLAN_RANGE_E /
                                     CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_IPV6_IPM_E /
                                     CPSS_EXMXPM_BRG_DROP_CNTR_SECURE_LEARNING_E].
            Expected: GT_OK.
        */

        /* Call with dropMode [CPSS_EXMXPM_BRG_DROP_CNTR_VLAN_RANGE_E] */
        dropMode = CPSS_EXMXPM_BRG_DROP_CNTR_VLAN_RANGE_E;

        st = cpssExMxPmBrgCntDropCntrModeSet(dev, dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropMode);

        /*
            1.2. Call cpssExMxPmBrgCntDropCntrModeGet with non-NULL dropModePtr.
            Expected: GT_OK and the same dropMode.
        */
        st = cpssExMxPmBrgCntDropCntrModeGet(dev, &dropModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgCntDropCntrModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropMode, dropModeGet,
                       "get another dropMode than was set: %d", dev);

        /* Call with dropMode [CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_IPV6_IPM_E] */
        dropMode = CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_IPV6_IPM_E;

        st = cpssExMxPmBrgCntDropCntrModeSet(dev, dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropMode);

        /*
            1.2. Call cpssExMxPmBrgCntDropCntrModeGet with non-NULL dropModePtr.
            Expected: GT_OK and the same dropMode.
        */
        st = cpssExMxPmBrgCntDropCntrModeGet(dev, &dropModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgCntDropCntrModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropMode, dropModeGet,
                       "get another dropMode than was set: %d", dev);

        /* Call with dropMode [CPSS_EXMXPM_BRG_DROP_CNTR_SECURE_LEARNING_E] */
        dropMode = CPSS_EXMXPM_BRG_DROP_CNTR_SECURE_LEARNING_E;

        st = cpssExMxPmBrgCntDropCntrModeSet(dev, dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropMode);

        /*
            1.2. Call cpssExMxPmBrgCntDropCntrModeGet with non-NULL dropModePtr.
            Expected: GT_OK and the same dropMode.
        */
        st = cpssExMxPmBrgCntDropCntrModeGet(dev, &dropModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgCntDropCntrModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropMode, dropModeGet,
                       "get another dropMode than was set: %d", dev);

        /*
            1.3. Call function with out of range dropMode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        dropMode = BRG_COUNT_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgCntDropCntrModeSet(dev, dropMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, dropMode);
    }

    dropMode = CPSS_EXMXPM_BRG_DROP_CNTR_VLAN_RANGE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntDropCntrModeSet(dev, dropMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntDropCntrModeSet(dev, dropMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntDropCntrModeGet
(
    IN   GT_U8                                  devNum,
    OUT  CPSS_EXMXPM_BRG_DROP_CNTR_MODE_ENT     *dropModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntDropCntrModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null dropModePtr. 
    Expected: GT_OK.
    1.2. Call with dropModePtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_DROP_CNTR_MODE_ENT  dropMode = CPSS_EXMXPM_BRG_DROP_CNTR_VLAN_RANGE_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-null dropModePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgCntDropCntrModeGet(dev, &dropMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null dropModePtr [NULL]. 
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgCntDropCntrModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntDropCntrModeGet(dev, &dropMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntDropCntrModeGet(dev, &dropMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntDropCntrSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  dropCntr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntDropCntrSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with dropCntr [0 / 0xFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgCntDropCntrGet with non-NULL dropCntrPtr.
    Expected: GT_OK and the same dropCntr.
    1.3. Call with dropCntr [0xFFFFFFFF].
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      dropCntr    = 0;
    GT_U32      dropCntrGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with dropCntr [0 / 0xFFFF].
            Expected: GT_OK.
        */

        /* Call with dropCntr [0] */
        dropCntr = 0;

        st = cpssExMxPmBrgCntDropCntrSet(dev, dropCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropCntr);

        /*
            1.2. Call cpssExMxPmBrgCntDropCntrGet with non-NULL dropCntrPtr.
            Expected: GT_OK and the same dropCntr.
        */
        st = cpssExMxPmBrgCntDropCntrGet(dev, &dropCntrGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgCntDropCntrGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropCntr, dropCntrGet,
                       "get another dropCntr than was set: %d", dev);

        /* Call with dropCntr [0xFFFF] */
        dropCntr = 0xFFFF;

        st = cpssExMxPmBrgCntDropCntrSet(dev, dropCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropCntr);

        /*
            1.2. Call cpssExMxPmBrgCntDropCntrGet with non-NULL dropCntrPtr.
            Expected: GT_OK and the same dropCntr.
        */
        st = cpssExMxPmBrgCntDropCntrGet(dev, &dropCntrGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgCntDropCntrGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(dropCntr, dropCntrGet,
                       "get another dropCntr than was set: %d", dev);

        /*
            1.3. Call with dropCntr [0xFFFFFFFF].
            Expected: GT_OK.
        */
        dropCntr = 0xFFFFFFFF;

        st = cpssExMxPmBrgCntDropCntrSet(dev, dropCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dropCntr);
    }

    dropCntr = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntDropCntrSet(dev, dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntDropCntrSet(dev, dropCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntDropCntrGet
(
    IN   GT_U8   devNum,
    OUT  GT_U32  *dropCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntDropCntrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null dropCntrPtr. 
    Expected: GT_OK.
    1.2. Call with dropCntrPtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      dropCntr = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-null dropCntrPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgCntDropCntrGet(dev, &dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null dropCntrPtr [NULL]. 
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgCntDropCntrModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dropCntrPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntDropCntrGet(dev, &dropCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntDropCntrGet(dev, &dropCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntMacDaSaSet 
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *macSaPtr,
    IN  GT_ETHERADDR    *macDaPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntMacDaSaSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with macSaPtr [AB:CD:EF:00:00:01],
                            macDaPtr [AB:CD:EF:00:00:02].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgCntMacDaSaGet with non-NULL macSaPtr and macDaPtr.
    Expected: GT_OK and same values as written.
    1.3. Call function with macSaPtr [NULL],
                            other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call function with macDaPtr [NULL],
                            other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_ETHERADDR    macSa;
    GT_ETHERADDR    macDa;
    GT_ETHERADDR    macSaGet;
    GT_ETHERADDR    macDaGet;
    GT_BOOL         isEqual = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with macSaPtr [AB:CD:EF:00:00:01],
                                    macDaPtr [AB:CD:EF:00:00:02].
            Expected: GT_OK.
        */
        macSa.arEther[0] = 0xAB;
        macSa.arEther[1] = 0xCD;
        macSa.arEther[2] = 0xEF;
        macSa.arEther[3] = 0x0;
        macSa.arEther[4] = 0x0;
        macSa.arEther[5] = 0x1;

        macDa.arEther[0] = 0xAB;
        macDa.arEther[1] = 0xCD;
        macDa.arEther[2] = 0xEF;
        macDa.arEther[3] = 0x0;
        macDa.arEther[4] = 0x0;
        macDa.arEther[5] = 0x2;

        st = cpssExMxPmBrgCntMacDaSaSet(dev, &macSa, &macDa);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgCntMacDaSaGet with non-NULL macSaPtr and macDaPtr.
            Expected: GT_OK and same values as written.
        */
        st = cpssExMxPmBrgCntMacDaSaGet(dev, &macSaGet, &macDaGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgCntMacDaSaGet: %d", dev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macSa,
                                     (GT_VOID*) &macSaGet,
                                     sizeof(macSa))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another macSaPtr than was set: %d", dev);
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macDa,
                                     (GT_VOID*) &macDaGet,
                                     sizeof(macDa))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another macDaPtr than was set: %d", dev);

        /*
            1.3. Call function with macSaPtr [NULL],
                                    other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgCntMacDaSaSet(dev, NULL, &macDa);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macSaPtr = NULL", dev);

        /*
            1.4. Call function with macDaPtr [NULL],
                                    other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgCntMacDaSaSet(dev, &macSa, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macDaPtr = NULL", dev);
    }

    macSa.arEther[0] = 0xAB;
    macSa.arEther[1] = 0xCD;
    macSa.arEther[2] = 0xEF;
    macSa.arEther[3] = 0x0;
    macSa.arEther[4] = 0x0;
    macSa.arEther[5] = 0x1;

    macDa.arEther[0] = 0xAB;
    macDa.arEther[1] = 0xCD;
    macDa.arEther[2] = 0xEF;
    macDa.arEther[3] = 0x0;
    macDa.arEther[4] = 0x0;
    macDa.arEther[5] = 0x2;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntMacDaSaSet(dev, &macSa, &macDa);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntMacDaSaSet(dev, &macSa, &macDa);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntMacDaSaGet 
(
    IN   GT_U8          devNum,
    OUT  GT_ETHERADDR   *macSaPtr,
    OUT  GT_ETHERADDR   *macDaPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntMacDaSaGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with non-NULL macSaPtr,
                            macDaPtr.
    Expected: GT_OK.
    1.2. Call function with macSaPtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call function with macDaPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_ETHERADDR    macSa;
    GT_ETHERADDR    macDa;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with non-NULL macSaPtr,
                                    macDaPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgCntMacDaSaGet(dev, &macSa, &macDa);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with macSaPtr [NULL],
                                    other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgCntMacDaSaGet(dev, NULL, &macDa);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macSaPtr = NULL", dev);

        /*
            1.3. Call function with macDaPtr [NULL],
                                    other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgCntMacDaSaGet(dev, &macSa, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macDaPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntMacDaSaGet(dev, &macSa, &macDa);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntMacDaSaGet(dev, &macSa, &macDa);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntHostGroupCntrsGet
(
    IN   GT_U8                              devNum,
    OUT  CPSS_EXMXPM_BRG_HOST_CNTR_STC   *hostGroupCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntHostGroupCntrsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null hostGroupCntrPtr. 
    Expected: GT_OK.
    1.2. Call with hostGroupCntrPtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_HOST_CNTR_STC   hostGroupCntr;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-null hostGroupCntrPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgCntHostGroupCntrsGet(dev, &hostGroupCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null hostGroupCntrPtr [NULL]. 
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgCntHostGroupCntrsGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, hostGroupCntrPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntHostGroupCntrsGet(dev, &hostGroupCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntHostGroupCntrsGet(dev, &hostGroupCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntMatrixGroupCntrsGet
(
    IN   GT_U8  devNum,
    OUT  GT_U32 *matrixCntrSaDaPktsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntMatrixGroupCntrsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null matrixCntrSaDaPktsPtr. 
    Expected: GT_OK.
    1.2. Call with matrixCntrSaDaPktsPtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      matrixCntrSaDaPkts = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-null matrixCntrSaDaPktsPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgCntMatrixGroupCntrsGet(dev, &matrixCntrSaDaPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null matrixCntrSaDaPktsPtr [NULL]. 
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgCntMatrixGroupCntrsGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, matrixCntrSaDaPktsPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntMatrixGroupCntrsGet(dev, &matrixCntrSaDaPkts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntMatrixGroupCntrsGet(dev, &matrixCntrSaDaPkts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntBridgeIngressCntrModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT         cntrSetId,
    IN  CPSS_EXMXPM_BRG_INGRESS_CNTR_MODE_ENT   setMode,
    IN  GT_U8                                   port,
    IN  GT_U16                                  vlan
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntBridgeIngressCntrModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E /
                              CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E],
                   setMode [CPSS_EXMXPM_BRG_INGRESS_CNTR_VID_MODE_E /
                            CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E],
                   port [0 / 12],
                   vlan [100 / 4095]. 
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgCntBridgeIngressCntrModeGet with the same cntrSetId and non-NULL setModePtr, portPtr, vlanPtr.
    Expected: GT_OK and same values as written.
    1.3. Call with out of range cntrSetId [0x5AAAAAA5]
                   and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range setMode [0x5AAAAAA5]
                   and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with setMode [CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_MODE_E /
                            CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E]
                   and out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. 
    Expected: GT_BAD_PARAM.
    1.6. Call with setMode [CPSS_EXMXPM_BRG_INGRESS_CNTR_ALL_MODE_E]
                   and out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant). 
    Expected: GT_OK.
    1.7. Call with setMode [CPSS_EXMXPM_BRG_INGRESS_CNTR_VID_MODE_E /
                            CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E]
                   and out of range vlan [PRV_CPSS_MAX_NUM_VLANS_CNS]. 
    Expected: GT_BAD_PARAM.
    1.8. Call with setMode [CPSS_EXMXPM_BRG_INGRESS_CNTR_ALL_MODE_E]
                   and out of range vlan [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant). 
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT         cntrSetId  = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
    CPSS_EXMXPM_BRG_INGRESS_CNTR_MODE_ENT   setMode    = CPSS_EXMXPM_BRG_INGRESS_CNTR_ALL_MODE_E;
    GT_U8                                   port       = BRG_COUNT_VALID_VIRT_PORT_CNS;
    GT_U16                                  vlan       = 0;
    CPSS_EXMXPM_BRG_INGRESS_CNTR_MODE_ENT   setModeGet = CPSS_EXMXPM_BRG_INGRESS_CNTR_ALL_MODE_E;
    GT_U8                                   portGet    = BRG_COUNT_VALID_VIRT_PORT_CNS;
    GT_U16                                  vlanGet    = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E /
                                      CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E],
                           setMode [CPSS_EXMXPM_BRG_INGRESS_CNTR_VID_MODE_E /
                                    CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E],
                           port [0 / 12],
                           vlan [100 / 4095]. 
            Expected: GT_OK.
        */

        /* Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E] */
        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
        setMode   = CPSS_EXMXPM_BRG_INGRESS_CNTR_VID_MODE_E;
        port      = BRG_COUNT_VALID_VIRT_PORT_CNS;
        vlan      = BRG_COUNT_TESTED_VLAN_ID_CNS;

        st = cpssExMxPmBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, cntrSetId, setMode, port, vlan);

        /*
            1.2. Call cpssExMxPmBrgCntBridgeIngressCntrModeGet with the same cntrSetId and non-NULL setModePtr, portPtr, vlanPtr.
            Expected: GT_OK and same values as written.
        */
        st = cpssExMxPmBrgCntBridgeIngressCntrModeGet(dev, cntrSetId, &setModeGet, &portGet, &vlanGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgCntBridgeIngressCntrModeGet: %d, %d", dev, cntrSetId);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(setMode, setModeGet,
                       "get another setMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(port, portGet,
                       "get another port than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vlan, vlanGet,
                       "get another vlan than was set: %d", dev);

        /* Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E] */
        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E;
        setMode   = CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E;
        port      = 12;
        vlan      = 4095;

        st = cpssExMxPmBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, cntrSetId, setMode, port, vlan);

        /*
            1.2. Call cpssExMxPmBrgCntBridgeIngressCntrModeGet with the same cntrSetId and non-NULL setModePtr, portPtr, vlanPtr.
            Expected: GT_OK and same values as written.
        */
        st = cpssExMxPmBrgCntBridgeIngressCntrModeGet(dev, cntrSetId, &setModeGet, &portGet, &vlanGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgCntBridgeIngressCntrModeGet: %d, %d", dev, cntrSetId);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(setMode, setModeGet,
                       "get another setMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(port, portGet,
                       "get another port than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vlan, vlanGet,
                       "get another vlan than was set: %d", dev);

        /*
            1.3. Call with out of range cntrSetId [0x5AAAAAA5]
                           and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cntrSetId = BRG_COUNT_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cntrSetId);

        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;

        /*
            1.4. Call with out of range setMode [0x5AAAAAA5]
                           and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        setMode = BRG_COUNT_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, setMode = %d", dev, setMode);

        setMode = CPSS_EXMXPM_BRG_INGRESS_CNTR_VID_MODE_E;

        /*
            1.5. Call with setMode [CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_MODE_E /
                                    CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E]
                           and out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. 
            Expected: GT_BAD_PARAM.
        */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        /* Call with setMode [CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_MODE_E] */
        setMode = CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_MODE_E;

        st = cpssExMxPmBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, setMode = %d, port = %d", dev, setMode, port);

        /* Call with setMode [CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E] */
        setMode = CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E;

        st = cpssExMxPmBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, setMode = %d, port = %d", dev, setMode, port);

        port = BRG_COUNT_VALID_VIRT_PORT_CNS;

        /*
            1.6. Call with setMode [CPSS_EXMXPM_BRG_INGRESS_CNTR_ALL_MODE_E]
                           and out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant). 
            Expected: GT_OK.
        */
        port    = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        setMode = CPSS_EXMXPM_BRG_INGRESS_CNTR_ALL_MODE_E;

        st = cpssExMxPmBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, setMode = %d, port = %d", dev, setMode, port);

        port = BRG_COUNT_VALID_VIRT_PORT_CNS;

        /*
            1.7. Call with setMode [CPSS_EXMXPM_BRG_INGRESS_CNTR_VID_MODE_E /
                                    CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E]
                           and out of range vlan [PRV_CPSS_MAX_NUM_VLANS_CNS]. 
            Expected: GT_BAD_PARAM.
        */
        vlan = PRV_CPSS_MAX_NUM_VLANS_CNS;

        /* Call with setMode [CPSS_EXMXPM_BRG_INGRESS_CNTR_VID_MODE_E] */
        setMode = CPSS_EXMXPM_BRG_INGRESS_CNTR_VID_MODE_E;

        st = cpssExMxPmBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, setMode = %d, vlan = %d", dev, setMode, vlan);

        /* Call with setMode [CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E] */
        setMode = CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E;

        st = cpssExMxPmBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, setMode = %d, vlan = %d", dev, setMode, vlan);

        vlan = BRG_COUNT_TESTED_VLAN_ID_CNS;

        /*
            1.8. Call with setMode [CPSS_EXMXPM_BRG_INGRESS_CNTR_ALL_MODE_E]
                           and out of range vlan [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant). 
            Expected: GT_OK.
        */
        vlan    = PRV_CPSS_MAX_NUM_VLANS_CNS;
        setMode = CPSS_EXMXPM_BRG_INGRESS_CNTR_ALL_MODE_E;

        st = cpssExMxPmBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, setMode = %d, vlan = %d", dev, setMode, vlan);
    }

    cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
    setMode   = CPSS_EXMXPM_BRG_INGRESS_CNTR_VID_MODE_E;
    port      = BRG_COUNT_VALID_VIRT_PORT_CNS;
    vlan      = BRG_COUNT_TESTED_VLAN_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntBridgeIngressCntrModeGet
(
    IN   GT_U8                                  devNum,
    IN   CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT        cntrSetId,
    OUT  CPSS_EXMXPM_BRG_INGRESS_CNTR_MODE_ENT  *setModePtr,
    OUT  GT_U8                                  *portPtr,
    OUT  GT_U16                                 *vlanPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntBridgeIngressCntrModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E /
                              CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E],
                   non-NULL setModePtr, portPtr, vlanPtr.
    Expected: GT_OK.
    1.2. Call with out of range cntrSetId [0x5AAAAAA5]
                   and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with setModePtr [NULL]
                   and other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with portPtr [NULL] (can be NULL)
                   and other params same as in 1.1.
    Expected: GT_OK.
    1.5. Call with vlanPtr [NULL] (can be NULL)
                   and other params same as in 1.1.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT         cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
    CPSS_EXMXPM_BRG_INGRESS_CNTR_MODE_ENT   setMode   = CPSS_EXMXPM_BRG_INGRESS_CNTR_ALL_MODE_E;
    GT_U8                                   port      = BRG_COUNT_VALID_VIRT_PORT_CNS;
    GT_U16                                  vlan      = 0;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E /
                                      CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E],
                           non-NULL setModePtr, portPtr, vlanPtr.
            Expected: GT_OK.
        */

        /* Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E] */
        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
        
        st = cpssExMxPmBrgCntBridgeIngressCntrModeGet(dev, cntrSetId, &setMode, &port, &vlan);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetId);

        /* Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E] */
        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E;
        
        st = cpssExMxPmBrgCntBridgeIngressCntrModeGet(dev, cntrSetId, &setMode, &port, &vlan);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetId);

        /*
            1.2. Call with out of range cntrSetId [0x5AAAAAA5]
                           and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cntrSetId = BRG_COUNT_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgCntBridgeIngressCntrModeGet(dev, cntrSetId, &setMode, &port, &vlan);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cntrSetId);

        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;

        /*
            1.3. Call with setModePtr [NULL]
                           and other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgCntBridgeIngressCntrModeGet(dev, cntrSetId, NULL, &port, &vlan);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, setModePtr = NULL", dev);

        /*
            1.4. Call with portPtr [NULL] (can be NULL)
                           and other params same as in 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgCntBridgeIngressCntrModeGet(dev, cntrSetId, &setMode, NULL, &vlan);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, portPtr = NULL", dev);

        /*
            1.5. Call with vlanPtr [NULL] (can be NULL)
                           and other params same as in 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgCntBridgeIngressCntrModeGet(dev, cntrSetId, &setMode, &port, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, vlanPtr = NULL", dev);
    }

    cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntBridgeIngressCntrModeGet(dev, cntrSetId, &setMode, &port, &vlan);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntBridgeIngressCntrModeGet(dev, cntrSetId, &setMode, &port, &vlan);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntBridgeIngressCntrsGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT     cntrSetId,
    OUT CPSS_BRIDGE_INGRESS_CNTR_STC        *ingressCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntBridgeIngressCntrsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E /
                              CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E],
                   non-NULL ingressCntrPtr.
    Expected: GT_OK.
    1.2. Call with out of range cntrSetId [0x5AAAAAA5]
                   and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with ingressCntrPtr [NULL]
                   and other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT     cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
    CPSS_BRIDGE_INGRESS_CNTR_STC        ingressCntr;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E /
                                      CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E],
                           non-NULL ingressCntrPtr.
            Expected: GT_OK.
        */

        /* Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E] */
        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
        
        st = cpssExMxPmBrgCntBridgeIngressCntrsGet(dev, cntrSetId, &ingressCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetId);

        /* Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E] */
        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E;
        
        st = cpssExMxPmBrgCntBridgeIngressCntrsGet(dev, cntrSetId, &ingressCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetId);

        /*
            1.2. Call with out of range cntrSetId [0x5AAAAAA5]
                           and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cntrSetId = BRG_COUNT_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgCntBridgeIngressCntrsGet(dev, cntrSetId, &ingressCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cntrSetId);

        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;

        /*
            1.3. Call with ingressCntrPtr [NULL]
                           and other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgCntBridgeIngressCntrsGet(dev, cntrSetId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ingressCntrPtr = NULL", dev);
    }

    cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntBridgeIngressCntrsGet(dev, cntrSetId, &ingressCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntBridgeIngressCntrsGet(dev, cntrSetId, &ingressCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntBridgeEgressCntrModeSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT     cntrSetId,
    IN  CPSS_PORT_EGRESS_CNT_MODE_ENT       cntrModeBmp,
    IN  GT_U8                               port,
    IN  GT_U16                              vlan,
    IN  GT_U8                               tc,
    IN  CPSS_DP_LEVEL_ENT                   dpLevel
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntBridgeEgressCntrModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E /
                              CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E],
                   cntrModeBmp[CPSS_EGRESS_CNT_PORT_E /
                               CPSS_EGRESS_CNT_VLAN_E],
                   port [0 / 12],
                   vlan [100 / 4095],
                   tc [0 / 5],
                   dpLevel [CPSS_DP_GREEN_E / CPSS_DP_RED_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgCntBridgeEgressCntrModeGet with the same cntrSetId
              and non-NULL cntrModeBmpPtr, portPtr, vlanPtr, tcPtr, dpLevelPtr.
    Expected: GT_OK and same values as written.
    1.3. Call with out of range cntrSetId [0x5AAAAAA5]
                   and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range cntrModeBmp [0x5AAAAAA5]
                   and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with cntrModeBmp [CPSS_EGRESS_CNT_PORT_E]
                   and out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. 
    Expected: GT_BAD_PARAM.
    1.6. Call with cntrModeBmp [CPSS_EGRESS_CNT_VLAN_E]
                   and out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant).
    Expected: GT_OK.
    1.7. Call with cntrModeBmp [CPSS_EGRESS_CNT_VLAN_E]
                   and out of range vlan [PRV_CPSS_MAX_NUM_VLANS_CNS]. 
    Expected: GT_BAD_PARAM.
    1.8. Call with cntrModeBmp [CPSS_EGRESS_CNT_TC_E]
                   and out of range vlan [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant).
    Expected: GT_OK.
    1.9. Call with cntrModeBmp [CPSS_EGRESS_CNT_TC_E]
                   and out of range tc [CPSS_TC_RANGE_CNS]. 
    Expected: GT_BAD_PARAM.
    1.10. Call with cntrModeBmp [CPSS_EGRESS_CNT_DP_E]
                    and out of range tc [CPSS_TC_RANGE_CNS] (not relevant).
    Expected: GT_OK.
    1.11. Call with cntrModeBmp [CPSS_EGRESS_CNT_DP_E]
                    and out of range dpLevel [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
    1.13. Call with cntrModeBmp [CPSS_EGRESS_CNT_PORT_E]
                    and out of range dpLevel [0x5AAAAAA5] (not relevant).
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT     cntrSetId   = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
    CPSS_PORT_EGRESS_CNT_MODE_ENT       cntrModeBmp = CPSS_EGRESS_CNT_PORT_E;
    GT_U8                               port        = BRG_COUNT_VALID_VIRT_PORT_CNS;
    GT_U16                              vlan        = 0;
    GT_U8                               tc          = 0;
    CPSS_DP_LEVEL_ENT                   dpLevel     = CPSS_DP_GREEN_E;
    CPSS_PORT_EGRESS_CNT_MODE_ENT       cntrModeGet = CPSS_EGRESS_CNT_PORT_E;
    GT_U8                               portGet     = BRG_COUNT_VALID_VIRT_PORT_CNS;
    GT_U16                              vlanGet     = 0;
    GT_U8                               tcGet       = 0;
    CPSS_DP_LEVEL_ENT                   dpLevelGet  = CPSS_DP_GREEN_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E /
                                      CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E],
                           cntrModeBmp[CPSS_EGRESS_CNT_PORT_E /
                                       CPSS_EGRESS_CNT_VLAN_E],
                           port [0 / 12],
                           vlan [100 / 4095],
                           tc [0 / 5],
                           dpLevel [CPSS_DP_GREEN_E / CPSS_DP_RED_E].
            Expected: GT_OK.
        */

        /* Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E] */
        cntrSetId   = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
        cntrModeBmp = CPSS_EGRESS_CNT_PORT_E;
        port        = BRG_COUNT_VALID_VIRT_PORT_CNS;
        vlan        = BRG_COUNT_TESTED_VLAN_ID_CNS;
        tc          = 0;
        dpLevel     = CPSS_DP_GREEN_E;

        st = cpssExMxPmBrgCntBridgeEgressCntrModeSet(dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL7_PARAM_MAC(GT_OK, st, dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);

        /*
            1.2. Call cpssExMxPmBrgCntBridgeEgressCntrModeGet with the same cntrSetId
                      and non-NULL cntrModeBmpPtr, portPtr, vlanPtr, tcPtr, dpLevelPtr.
            Expected: GT_OK and same values as written.
        */
        st = cpssExMxPmBrgCntBridgeEgressCntrModeGet(dev, cntrSetId, &cntrModeGet, &portGet, &vlanGet, &tcGet, &dpLevelGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgCntBridgeEgressCntrModeGet: %d, %d", dev, cntrSetId);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cntrModeBmp, cntrModeGet,
                       "get another cntrModeBmp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(port, portGet,
                       "get another port than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(tc, tcGet,
                       "get another tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(dpLevel, dpLevelGet,
                       "get another dpLevel than was set: %d", dev);

        /* Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E] */
        cntrSetId   = CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E;
        cntrModeBmp = CPSS_EGRESS_CNT_PORT_E;
        port        = BRG_COUNT_VALID_VIRT_PORT_CNS;
        vlan        = BRG_COUNT_TESTED_VLAN_ID_CNS;
        tc          = 0;
        dpLevel     = CPSS_DP_GREEN_E;

        st = cpssExMxPmBrgCntBridgeEgressCntrModeSet(dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL7_PARAM_MAC(GT_OK, st, dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);

        /*
            1.2. Call cpssExMxPmBrgCntBridgeEgressCntrModeGet with the same cntrSetId
                      and non-NULL cntrModeBmpPtr, portPtr, vlanPtr, tcPtr, dpLevelPtr.
            Expected: GT_OK and same values as written.
        */
        st = cpssExMxPmBrgCntBridgeEgressCntrModeGet(dev, cntrSetId, &cntrModeGet, &portGet, &vlanGet, &tcGet, &dpLevelGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgCntBridgeEgressCntrModeGet: %d, %d", dev, cntrSetId);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cntrModeBmp, cntrModeGet,
                       "get another cntrModeBmp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(port, portGet,
                       "get another port than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(tc, tcGet,
                       "get another tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(dpLevel, dpLevelGet,
                       "get another dpLevel than was set: %d", dev);

        /*
            1.3. Call with out of range cntrSetId [0x5AAAAAA5]
                           and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cntrSetId = BRG_COUNT_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgCntBridgeEgressCntrModeSet(dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cntrSetId);

        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;

        /*
            1.4. Call with out of range cntrModeBmp [0x5AAAAAA5]
                           and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cntrModeBmp = BRG_COUNT_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgCntBridgeEgressCntrModeSet(dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, cntrModeBmp = %d", dev, cntrModeBmp);

        cntrModeBmp = CPSS_EGRESS_CNT_PORT_E;

        /*
            1.5. Call with cntrModeBmp [CPSS_EGRESS_CNT_PORT_E]
                           and out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]. 
            Expected: GT_BAD_PARAM.
        */
        cntrModeBmp = CPSS_EGRESS_CNT_PORT_E;
        port        = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgCntBridgeEgressCntrModeSet(dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, cntrModeBmp = %d, port = %d", dev, cntrModeBmp, port);

        /*
            1.6. Call with cntrModeBmp [CPSS_EGRESS_CNT_VLAN_E]
                           and out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant).
            Expected: GT_OK.
        */
        cntrModeBmp = CPSS_EGRESS_CNT_VLAN_E;
        
        st = cpssExMxPmBrgCntBridgeEgressCntrModeSet(dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, cntrModeBmp = %d, port = %d", dev, cntrModeBmp, port);

        port = BRG_COUNT_VALID_VIRT_PORT_CNS;

        /*
            1.7. Call with cntrModeBmp [CPSS_EGRESS_CNT_VLAN_E]
                           and out of range vlan [PRV_CPSS_MAX_NUM_VLANS_CNS]. 
            Expected: GT_BAD_PARAM.
        */
        cntrModeBmp = CPSS_EGRESS_CNT_VLAN_E;
        vlan        = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmBrgCntBridgeEgressCntrModeSet(dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, cntrModeBmp = %d, vlan = %d", dev, cntrModeBmp, vlan);

        /*
            1.8. Call with cntrModeBmp [CPSS_EGRESS_CNT_TC_E]
                           and out of range vlan [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant).
            Expected: GT_OK.
        */
        cntrModeBmp = CPSS_EGRESS_CNT_TC_E;
        
        st = cpssExMxPmBrgCntBridgeEgressCntrModeSet(dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, cntrModeBmp = %d, vlan = %d", dev, cntrModeBmp, vlan);

        vlan = BRG_COUNT_TESTED_VLAN_ID_CNS;

        /*
            1.9. Call with cntrModeBmp [CPSS_EGRESS_CNT_TC_E]
                           and out of range tc [CPSS_TC_RANGE_CNS]. 
            Expected: GT_BAD_PARAM.
        */
        cntrModeBmp = CPSS_EGRESS_CNT_TC_E;
        tc          = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmBrgCntBridgeEgressCntrModeSet(dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, cntrModeBmp = %d, tc = %d", dev, cntrModeBmp, tc);

        /*
            1.10. Call with cntrModeBmp [CPSS_EGRESS_CNT_DP_E]
                            and out of range tc [CPSS_TC_RANGE_CNS] (not relevant).
            Expected: GT_OK.
        */
        cntrModeBmp = CPSS_EGRESS_CNT_DP_E;
        
        st = cpssExMxPmBrgCntBridgeEgressCntrModeSet(dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, cntrModeBmp = %d, tc = %d", dev, cntrModeBmp, tc);

        tc = 0;

        /*
            1.11. Call with cntrModeBmp [CPSS_EGRESS_CNT_DP_E]
                            and out of range dpLevel [0x5AAAAAA5]. 
            Expected: GT_BAD_PARAM.
        */
        cntrModeBmp = CPSS_EGRESS_CNT_DP_E;
        dpLevel     = BRG_COUNT_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgCntBridgeEgressCntrModeSet(dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, cntrModeBmp = %d, dpLevel = %d", dev, cntrModeBmp, dpLevel);

        dpLevel = CPSS_DP_YELLOW_E;

        /*
            1.13. Call with cntrModeBmp [CPSS_EGRESS_CNT_PORT_E]
                            and out of range dpLevel [0x5AAAAAA5] (not relevant).
            Expected: GT_OK.
        */
        cntrModeBmp = CPSS_EGRESS_CNT_PORT_E;
        dpLevel     = BRG_COUNT_INVALID_ENUM_CNS;
        
        st = cpssExMxPmBrgCntBridgeEgressCntrModeSet(dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, cntrModeBmp = %d, dpLevel = %d", dev, cntrModeBmp, dpLevel);
    }

    cntrSetId   = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
    cntrModeBmp = CPSS_EGRESS_CNT_PORT_E;
    port        = BRG_COUNT_VALID_VIRT_PORT_CNS;
    vlan        = BRG_COUNT_TESTED_VLAN_ID_CNS;
    tc          = 0;
    dpLevel     = CPSS_DP_GREEN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntBridgeEgressCntrModeSet(dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntBridgeEgressCntrModeSet(dev, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntBridgeEgressCntrModeGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT     cntrSetId,
    OUT CPSS_PORT_EGRESS_CNT_MODE_ENT       *cntrModeBmpPtr,
    OUT GT_U8                               *portPtr,
    OUT GT_U16                              *vlanPtr,
    OUT GT_U8                               *tcPtr,
    OUT CPSS_DP_LEVEL_ENT                   *dpLevelPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntBridgeEgressCntrModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E /
                              CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E],
                   non-NULL cntrModeBmpPtr,
                   non-NULL portPtr,
                   non-NULL vlanPtr,
                   non-NULL tcPtr,
                   non-NULL dpLevelPtr.
    Expected: GT_OK.
    1.2. Call with out of range cntrSetId [0x5AAAAAA5]
                   and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with cntrModeBmpPtr [NULL]
                   and other params same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with portPtr [NULL] (can be NULL)
                   and other params same as in 1.1.
    Expected: GT_OK.
    1.5. Call with vlanPtr [NULL] (can be NULL)
                   and other params same as in 1.1.
    Expected: GT_OK.
    1.6. Call with tcPtr [NULL] (can be NULL)
                   and other params same as in 1.1.
    Expected: GT_OK.
    1.7. Call with dpLevelPtr [NULL] (can be NULL)
                   and other params same as in 1.1.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT     cntrSetId   = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
    CPSS_PORT_EGRESS_CNT_MODE_ENT       cntrModeBmp = CPSS_EGRESS_CNT_PORT_E;
    GT_U8                               port        = BRG_COUNT_VALID_VIRT_PORT_CNS;
    GT_U16                              vlan        = 0;
    GT_U8                               tc          = 0;
    CPSS_DP_LEVEL_ENT                   dpLevel     = CPSS_DP_GREEN_E;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E /
                                      CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E],
                           non-NULL cntrModeBmpPtr,
                           non-NULL portPtr,
                           non-NULL vlanPtr,
                           non-NULL tcPtr,
                           non-NULL dpLevelPtr.
            Expected: GT_OK.
        */

        /* Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E] */
        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
        
        st = cpssExMxPmBrgCntBridgeEgressCntrModeGet(dev, cntrSetId, &cntrModeBmp, &port, &vlan, &tc, &dpLevel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetId);

        /* Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E] */
        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E;
        
        st = cpssExMxPmBrgCntBridgeEgressCntrModeGet(dev, cntrSetId, &cntrModeBmp, &port, &vlan, &tc, &dpLevel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetId);

        /*
            1.2. Call with out of range cntrSetId [0x5AAAAAA5]
                           and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cntrSetId = BRG_COUNT_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgCntBridgeEgressCntrModeGet(dev, cntrSetId, &cntrModeBmp, &port, &vlan, &tc, &dpLevel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cntrSetId);

        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;

        /*
            1.3. Call with cntrModeBmpPtr [NULL]
                           and other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgCntBridgeEgressCntrModeGet(dev, cntrSetId, NULL, &port, &vlan, &tc, &dpLevel);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cntrModeBmpPtr = NULL", dev);

        /*
            1.4. Call with portPtr [NULL] (can be NULL)
                           and other params same as in 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgCntBridgeEgressCntrModeGet(dev, cntrSetId, &cntrModeBmp, NULL, &vlan, &tc, &dpLevel);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, portPtr = NULL", dev);

        /*
            1.5. Call with vlanPtr [NULL] (can be NULL)
                           and other params same as in 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgCntBridgeEgressCntrModeGet(dev, cntrSetId, &cntrModeBmp, &port, NULL, &tc, &dpLevel);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, vlanPtr = NULL", dev);

        /*
            1.6. Call with tcPtr [NULL] (can be NULL)
                           and other params same as in 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgCntBridgeEgressCntrModeGet(dev, cntrSetId, &cntrModeBmp, &port, &vlan, NULL, &dpLevel);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, tcPtr = NULL", dev);

        /*
            1.7. Call with dpLevelPtr [NULL] (can be NULL)
                           and other params same as in 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgCntBridgeEgressCntrModeGet(dev, cntrSetId, &cntrModeBmp, &port, &vlan, &tc, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, dpLevelPtr = NULL", dev);
    }

    cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntBridgeEgressCntrModeGet(dev, cntrSetId, &cntrModeBmp, &port, &vlan, &tc, &dpLevel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntBridgeEgressCntrModeGet(dev, cntrSetId, &cntrModeBmp, &port, &vlan, &tc, &dpLevel);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgCntBridgeEgressCntrsGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT     cntrSetId,
    OUT CPSS_EXMXPM_BRG_EGRESS_CNTR_STC     *egressCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgCntBridgeEgressCntrsGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E /
                              CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E],
                   non-NULL egressCntrPtr.
    Expected: GT_OK.
    1.2. Call with out of range cntrSetId [0x5AAAAAA5]
                   and other params same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with egressCntrPtr [NULL]
                   and other params same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT     cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
    CPSS_EXMXPM_BRG_EGRESS_CNTR_STC     egressCntr;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E /
                                      CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E],
                           non-NULL egressCntrPtr.
            Expected: GT_OK.
        */

        /* Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E] */
        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
        
        st = cpssExMxPmBrgCntBridgeEgressCntrsGet(dev, cntrSetId, &egressCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetId);

        /* Call with cntrSetId [CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E] */
        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_1_E;
        
        st = cpssExMxPmBrgCntBridgeEgressCntrsGet(dev, cntrSetId, &egressCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetId);

        /*
            1.2. Call with out of range cntrSetId [0x5AAAAAA5]
                           and other params same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        cntrSetId = BRG_COUNT_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgCntBridgeEgressCntrsGet(dev, cntrSetId, &egressCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cntrSetId);

        cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;

        /*
            1.3. Call with egressCntrPtr [NULL]
                           and other params same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgCntBridgeEgressCntrsGet(dev, cntrSetId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, egressCntrPtr = NULL", dev);
    }

    cntrSetId = CPSS_EXMXPM_BRG_CNTR_SET_ID_0_E;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgCntBridgeEgressCntrsGet(dev, cntrSetId, &egressCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgCntBridgeEgressCntrsGet(dev, cntrSetId, &egressCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmBrgCount suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmBrgCount)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntLearnedEntryDiscardGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntDropCntrModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntDropCntrModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntDropCntrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntDropCntrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntMacDaSaSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntMacDaSaGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntHostGroupCntrsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntMatrixGroupCntrsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntBridgeIngressCntrModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntBridgeIngressCntrModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntBridgeIngressCntrsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntBridgeEgressCntrModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntBridgeEgressCntrModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgCntBridgeEgressCntrsGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmBrgCount)

