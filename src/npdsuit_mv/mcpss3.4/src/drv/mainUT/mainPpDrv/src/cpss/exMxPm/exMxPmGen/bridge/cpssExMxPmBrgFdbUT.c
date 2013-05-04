/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmBrgFdbUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmBrgFdb, that provides
*       FDB tables facility CPSS ExMx Puma implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdb.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

#include <gtOs/gtOsMem.h>

/* defines */

/* Default valid value for port id */
#define BRG_FDB_VALID_PHY_PORT_CNS  0

/* Invalid enum */
#define BRG_FDB_INVALID_ENUM_CNS    0x5AAAAAA5

/* Tests use this vlan id for testing VLAN functions */
#define BRG_FDB_TESTED_VLAN_ID_CNS  100

/* Maximum value for trunk id. */
#define MAX_TRUNK_ID_CNS            256
#define MAX_NUM_OF_AU_MSG  128

/* Internal functions forward declaration */
static GT_STATUS prvUtfCoreClockGet(IN GT_U8 dev, OUT GT_U32* coreClockPtr);
static GT_STATUS prvUtfFdbTcamTableSize(IN GT_U8 dev, OUT GT_U32* tableSizePtr);
static GT_STATUS prvUtfFdbMacTableSize(IN GT_U8 dev, OUT GT_U32* tableSizePtr);
#ifdef FDB_EXTERN_MEMORY_USED
static GT_STATUS prvUtfLutTableSize(IN GT_U8 dev, OUT GT_U32* tableSizePtr);
#endif

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbHashModeSet
(
    IN GT_U8                                devNum,
    IN CPSS_EXMXPM_FDB_HASH_FUNC_MODE_ENT   mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbHashModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with mode [CPSS_EXMXPM_FDB_HASH_FUNC_CRC_E /
                                  CPSS_EXMXPM_FDB_HASH_FUNC_LEGACY_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbHashModeGet.
    Expected: GT_OK and the same modePtr.
    1.3. Call function with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_HASH_FUNC_MODE_ENT mode    = CPSS_EXMXPM_FDB_HASH_FUNC_CRC_E;
    CPSS_EXMXPM_FDB_HASH_FUNC_MODE_ENT modeGet = CPSS_EXMXPM_FDB_HASH_FUNC_CRC_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_EXMXPM_FDB_HASH_FUNC_CRC_E /
                                          CPSS_EXMXPM_FDB_HASH_FUNC_LEGACY_E].
            Expected: GT_OK.
        */

        /* 1.1. Call with mode [CPSS_EXMXPM_FDB_HASH_FUNC_CRC_E] */
        mode = CPSS_EXMXPM_FDB_HASH_FUNC_CRC_E;

        st = cpssExMxPmBrgFdbHashModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbHashModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbHashModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbHashModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /* 1.1. Call with mode [CPSS_EXMXPM_FDB_HASH_FUNC_LEGACY_E] */
        mode = CPSS_EXMXPM_FDB_HASH_FUNC_LEGACY_E;

        st = cpssExMxPmBrgFdbHashModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbHashModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbHashModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbHashModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /*
            1.3. Call function with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbHashModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_FDB_HASH_FUNC_CRC_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbHashModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbHashModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbHashModeGet
(
    IN  GT_U8                                 devNum,
    OUT CPSS_EXMXPM_FDB_HASH_FUNC_MODE_ENT   *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbHashModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null modePtr.
    Expected: GT_OK.
    1.2. Call with null modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_HASH_FUNC_MODE_ENT  mode = CPSS_EXMXPM_FDB_HASH_FUNC_CRC_E;

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
        st = cpssExMxPmBrgFdbHashModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbHashModeGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbHashModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbHashModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbNumOfEntriesInLookupSet
(
    IN GT_U8    devNum,
    IN GT_U32   lookupLen
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbNumOfEntriesInLookupSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with lookupLen [4 / 32].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbNumOfEntriesInLookupGet
    Expected: GT_OK and the same lookupLenPtr.
    1.3. Call function with out of range lookupLen [1 / 36].
    Expected: NOT GT_OK.
    1.4. Call function with out of range lookupLen [5].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32  len    = 0;
    GT_U32  lenGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with lookupLen [4 / 32].
            Expected: GT_OK.
        */

        /* Call with lookupLen [4] */
        len = 4;

        st = cpssExMxPmBrgFdbNumOfEntriesInLookupSet(dev, len);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, len);

        /*
            1.2. Call cpssExMxPmBrgFdbNumOfEntriesInLookupGet
            Expected: GT_OK and the same lookupLenPtr.
        */
        st = cpssExMxPmBrgFdbNumOfEntriesInLookupGet(dev, &lenGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbNumOfEntriesInLookupGet: %d", dev);

        /* Verify values */
        UTF_VERIFY_EQUAL1_STRING_MAC(len, lenGet,
                       "get another lookupLenPtr than was set: %d", dev);

        /* Call with lookupLen [32] */
        len = 32;

        st = cpssExMxPmBrgFdbNumOfEntriesInLookupSet(dev, len);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, len);

        /*
            1.2. Call cpssExMxPmBrgFdbNumOfEntriesInLookupGet
            Expected: GT_OK and the same lookupLenPtr.
        */
        st = cpssExMxPmBrgFdbNumOfEntriesInLookupGet(dev, &lenGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbNumOfEntriesInLookupGet: %d", dev);

        /* Verify values */
        UTF_VERIFY_EQUAL1_STRING_MAC(len, lenGet,
                       "get another lookupLenPtr than was set: %d", dev);

        /*
            1.3. Call function with out of range lookupLen [1 / 36].
            Expected: NOT GT_OK.
        */

        /* Call with lookupLen [1] */
        len = 1;

        st = cpssExMxPmBrgFdbNumOfEntriesInLookupSet(dev, len);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, len);

        /* Call with lookupLen [36] */
        len = 36;

        st = cpssExMxPmBrgFdbNumOfEntriesInLookupSet(dev, len);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, len);

        /*
            1.4. Call function with out of range lookupLen [5].
            Expected: NOT GT_OK.
        */
        len = 5;

        st = cpssExMxPmBrgFdbNumOfEntriesInLookupSet(dev, len);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, len);
    }

    len = 4;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbNumOfEntriesInLookupSet(dev, len);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbNumOfEntriesInLookupSet(dev, len);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbNumOfEntriesInLookupGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *lookupLenPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbNumOfEntriesInLookupGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null lookupLenPtr.
    Expected: GT_OK.
    1.2. Call with null lookupLenPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32  len = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null lookupLenPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbNumOfEntriesInLookupGet(dev, &len);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null lookupLenPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbNumOfEntriesInLookupGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lookupLenPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbNumOfEntriesInLookupGet(dev, &len);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbNumOfEntriesInLookupGet(dev, &len);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbMacVlanLookupModeSet
(
    IN GT_U8            devNum,
    IN CPSS_MAC_VL_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbMacVlanLookupModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with mode [CPSS_IVL_E /
                                  CPSS_SVL_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbMacVlanLookupModeGet
    Expected: GT_OK and the same modePtr.
    1.3. Call function with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_MAC_VL_ENT mode    = CPSS_IVL_E;
    CPSS_MAC_VL_ENT modeGet = CPSS_IVL_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_IVL_EE / CPSS_SVL_E].
            Expected: GT_OK.
        */

        /* 1.1. Call with mode [CPSS_IVL_EE] */
        mode = CPSS_IVL_E;

        st = cpssExMxPmBrgFdbMacVlanLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbMacVlanLookupModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbMacVlanLookupModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbMacVlanLookupModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /* 1.1. Call with mode [CPSS_SVL_E] */
        mode = CPSS_SVL_E;

        st = cpssExMxPmBrgFdbMacVlanLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbMacVlanLookupModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbMacVlanLookupModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbMacVlanLookupModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /*
            1.3. Call function with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbMacVlanLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_IVL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbMacVlanLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbMacVlanLookupModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbMacVlanLookupModeGet
(
    IN  GT_U8            devNum,
    OUT CPSS_MAC_VL_ENT  *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbMacVlanLookupModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null modePtr.
    Expected: GT_OK.
    1.2. Call with null modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_MAC_VL_ENT mode = CPSS_IVL_E;

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
        st = cpssExMxPmBrgFdbMacVlanLookupModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbMacVlanLookupModeGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbMacVlanLookupModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbMacVlanLookupModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbExtLookupOffsetSizeSet
(
    IN GT_U8                                    devNum,
    IN GT_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_ENT  offset
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbExtLookupOffsetSizeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with offset [CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_5_BITS_E /
                                    CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_6_BITS_E /
                                    CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_7_BITS_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbExtLookupOffsetSizeGet
    Expected: GT_OK and the same offsetPtr.
    1.3. Call function with out of range offset [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_ENT offset    = CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_5_BITS_E;
    GT_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_ENT offsetGet = CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_5_BITS_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with offset [CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_5_BITS_E /
                                            CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_6_BITS_E /
                                            CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_7_BITS_E].
            Expected: GT_OK.
        */

        /* Call with offset [CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_5_BITS_E] */
        offset = CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_5_BITS_E;

        st = cpssExMxPmBrgFdbExtLookupOffsetSizeSet(dev, offset);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, offset);

        /*
            1.2. Call cpssExMxPmBrgFdbExtLookupOffsetSizeGet
            Expected: GT_OK and the same offsetPtr.
        */
        st = cpssExMxPmBrgFdbExtLookupOffsetSizeGet(dev, &offsetGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbExtLookupOffsetSizeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(offset, offsetGet,
                       "get another offset than was set: %d", dev);

        /* Call with offset [CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_6_BITS_E] */
        offset = CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_6_BITS_E;

        st = cpssExMxPmBrgFdbExtLookupOffsetSizeSet(dev, offset);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, offset);

        /*
            1.2. Call cpssExMxPmBrgFdbExtLookupOffsetSizeGet
            Expected: GT_OK and the same offsetPtr.
        */
        st = cpssExMxPmBrgFdbExtLookupOffsetSizeGet(dev, &offsetGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbExtLookupOffsetSizeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(offset, offsetGet,
                       "get another offset than was set: %d", dev);

        /* Call with offset [CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_7_BITS_E] */
        offset = CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_7_BITS_E;

        st = cpssExMxPmBrgFdbExtLookupOffsetSizeSet(dev, offset);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, offset);

        /*
            1.2. Call cpssExMxPmBrgFdbExtLookupOffsetSizeGet
            Expected: GT_OK and the same offsetPtr.
        */
        st = cpssExMxPmBrgFdbExtLookupOffsetSizeGet(dev, &offsetGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbExtLookupOffsetSizeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(offset, offsetGet,
                       "get another offset than was set: %d", dev);

        /*
            1.3. Call function with out of range offset [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        offset = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbExtLookupOffsetSizeSet(dev, offset);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, offset);
    }

    offset = CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_5_BITS_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbExtLookupOffsetSizeSet(dev, offset);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbExtLookupOffsetSizeSet(dev, offset);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbExtLookupOffsetSizeGet
(
    IN  GT_U8                                    devNum,
    OUT GT_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_ENT  *offsetPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbExtLookupOffsetSizeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null offsetPtr.
    Expected: GT_OK.
    1.2. Call with null offsetPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_ENT offset = CPSS_EXMXPM_FDB_LUT_FIELD_OFFSET_SIZE_5_BITS_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null offsetPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbExtLookupOffsetSizeGet(dev, &offset);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null offsetPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbExtLookupOffsetSizeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, offsetPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbExtLookupOffsetSizeGet(dev, &offset);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbExtLookupOffsetSizeGet(dev, &offset);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbEntrySet
(
    IN GT_U8                        devNum,
    IN CPSS_EXMXPM_FDB_ENTRY_STC    *fdbEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
-> CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E
    1.1. Call function with valid fdbEntryPtr {key {entryType[CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                                    macfId->fId[100],
                                                    macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                               dstOutlif { outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                           interfaceInfo{ set to trunc default }
                                                           outlifPointer{arpPtr[0]} }
                                               isStatic [GT_FALSE],
                                               daCommand [CPSS_PACKET_CMD_FORWARD_E],
                                               saCommand [CPSS_PACKET_CMD_FORWARD_E],
                                               daRoute [GT_FALSE],
                                               mirrorToRxAnalyzerPortEn [GT_FALSE],
                                               sourceID [0],
                                               userDefined [0],
                                               daQosIndex [0],
                                               saQosIndex [0],
                                               daSecurityLevel [0],
                                               saSecurityLevel [0],
                                               appSpecificCpuCode [GT_FALSE],
                                               pwId[0] }.
    Expected: GT_OK.
    1.2. Call function with valid fdbEntryPtr {key {entryType[ CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E ],
                                                    macfId->fId[100],
                                                    macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                               dstOutlif { outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                           interfaceInfo{ set to port default }
                                                           outlifPointer{arpPtr[0]} }
                                               isStatic [GT_TRUE],
                                               daCommand [CPSS_PACKET_CMD_DROP_HARD_E],
                                               saCommand [CPSS_PACKET_CMD_DROP_HARD_E],
                                               daRoute [GT_TRUE],
                                               mirrorToRxAnalyzerPortEn [GT_TRUE],
                                               sourceID [0x7],
                                               userDefined [0xF],
                                               daQosIndex [0x7],
                                               saQosIndex [0x7],
                                               daSecurityLevel [0x7],
                                               saSecurityLevel [0x7],
                                               appSpecificCpuCode [GT_TRUE],
                                               pwId[0] }.
    Expected: GT_OK.

-> CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E
    1.3. Call function with valid fdbEntryPtr {key {entryType[ CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E ],
                                                    ipMcast->fId[100],
                                                    ipMcast->sip[00:1A:FF:FF], ipMcast->dip[00:1A:FF:FF]},
                                               dstOutlif { outlifType[ CPSS_EXMXPM_OUTLIF_TYPE_LL_E ],
                                                           interfaceInfo{type[ CPSS_INTERFACE_VIDX_E ],
                                                                        vidx[100],
                                                           outlifPointer{arpPtr[0],
                                               daCommand [ CPSS_PACKET_CMD_DROP_SOFT_E ],
                                               saCommand [ CPSS_PACKET_CMD_DROP_SOFT_E ],
                                               sourceID [ 0],
                                               userDefined [0xF],
                                               daQosIndex [3],
                                               saQosIndex [5],
                                               daSecurityLevel [1],
                                               saSecurityLevel [4],
                                               appSpecificCpuCode [ GT_FALSE],
                                               pwId[10] }.
    Expected: GT_OK.

-> CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E
    1.4. Call function with valid fdbEntryPtr {key {entryType[ CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E],
                                                    ipMcast->fId[100],
                                                    ipMcast->sip[00:1A:FF:FF], ipMcast->dip[00:1A:FF:FF]},
                                               dstOutlif { outlifType[ CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E],
                                                           interfaceInfo{type[ CPSS_INTERFACE_VID_E],
                                                                        vlanId[100]},
                                                                   outlifPointer{arpPtr[0],
                                               daCommand [ CPSS_PACKET_CMD_DROP_SOFT_E],
                                               saCommand [ CPSS_PACKET_CMD_DROP_SOFT_E],
                                               sourceID [ 0],
                                               userDefined [0xF],
                                               daQosIndex [0x7],
                                               saQosIndex [0x7],
                                               daSecurityLevel [0x7],
                                               saSecurityLevel [0x7],
                                               appSpecificCpuCode [ GT_TRUE],
                                               pwId[10] }.
    Expected: GT_OK.

    1.5. Call function with out of range fdbEntryPtr->key->entryType [0x5AAAAAA5]
                            and other parameters the same as in 1.7.
    Expected: GT_BAD_PARAM.

    1.6. Call with interfaceInfo.type [0x5AAAAAA5] out of range
                  and other valid params.
    Expected: GT_BAD_PARAM.

[ CPSS_INTERFACE_TRUNK_E ]
    -> out of range
    1.7. Call with interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                    interfaceInfo.trunkId [4096]
                    and other valid params.
    Expected: NOT GT_OK.

   -> not relevant
    1.8. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                    interfaceInfo.trunkId [4096]
                    and other valid params.
    Expected: GT_OK.

[ CPSS_INTERFACE_PORT_E ]
    -> out of range
    1.9. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                    interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other valid params.
    Expected: NOT GT_OK.
    1.10. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                    interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                    and other valid params.
    Expected: NOT GT_OK.

   -> not relevant
    1.11. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                    interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other valid params.
    Expected: NOT GT_OK.
    1.12. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                    interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                    and other valid params.
    Expected: NOT GT_OK.

[ CPSS_INTERFACE_VIDX_E ]
    -> out of range
    1.13. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                    interfaceInfo.vidx [4096]
                    and other valid params.
    Expected: NOT GT_OK.

   -> not relevant
    1.14. Call with interfaceInfo.type [CPSS_INTERFACE_VID_E]
                    interfaceInfo.vidx [4096]
                    and other valid params.
    Expected: GT_OK.

    -> not relevant
    1.16. Call with interfaceInfo.type [CPSS_INTERFACE_DEVICE_E]
                    interfaceInfo.vlanId [4096]
                    and other valid params.
    Expected: GT_OK.

[ CPSS_INTERFACE_DEVICE_E ]
    -> out of range
    1.17. Call with interfaceInfo.type [CPSS_INTERFACE_DEVICE_E]
                    interfaceInfo.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other valid params.
    Expected: NOT GT_OK.

[ CPSS_INTERFACE_FABRIC_VIDX_E ]
    -> out of range
    1.19. Call with interfaceInfo.type [CPSS_INTERFACE_FABRIC_VIDX_E]
                    interfaceInfo.fabricVidx [4096]
                   and other valid params.
    Expected: NOT GT_OK.

    -> not relevant
    1.20. Call with interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                    interfaceInfo.fabricVidx [4096]
                   and other valid params.
    Expected: GT_OK.
    1.21. Call function with out of range fdbEntryPtr->daCommand [0x5AAAAAA5]
                             and other parameters the same as in 1.7.
    Expected: GT_BAD_PARAM.
    1.22. Call function with out of range fdbEntryPtr->saCommand [0x5AAAAAA5],
                             key->entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E] (saCommand is not important for IPv4 and IPv6 MC).
                             and other parameters the same as in 1.7.
    Expected: GT_BAD_PARAM.
    1.23. Call function with out of range fdbEntryPtr->userDefined [0x10]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.24. Call function with out of range fdbEntryPtr->daQosIndex [0x8]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.25. Call function with out of range fdbEntryPtr->saQosIndex [0x8]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.26. Call function with out of range fdbEntryPtr->daSecurityLevel [0x8]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.27. Call function with out of range fdbEntryPtr->saSecurityLevel [0x8]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.28. Call with fdbEntryPtr->dstOutlif.outlifType [0x5AAAAAA5] (out of range)
    Expected: GT_BAD_PARAM.
    1.29. Call function with null fdbEntryPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_ENTRY_STC  fdbEntry;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            -> CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E
            1.1. Call function with valid fdbEntryPtr {key {entryType[CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                                            macfId->fId[100],
                                                            macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                                       dstOutlif { outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                   interfaceInfo{ set to trunc default }
                                                                   outlifPointer{arpPtr[0]} }
                                                       isStatic [GT_FALSE],
                                                       daCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                       saCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                       daRoute [GT_FALSE],
                                                       mirrorToRxAnalyzerPortEn [GT_FALSE],
                                                       sourceID [0],
                                                       userDefined [0],
                                                       daQosIndex [0],
                                                       saQosIndex [0],
                                                       daSecurityLevel [0],
                                                       saSecurityLevel [0],
                                                       appSpecificCpuCode [GT_FALSE],
                                                       pwId[0] }.
            Expected: GT_OK.
        */
        fdbEntry.key.entryType      = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        fdbEntry.key.key.macFid.fId = 100;
        fdbEntry.key.key.macFid.macAddr.arEther[0] = 0x00;
        fdbEntry.key.key.macFid.macAddr.arEther[1] = 0x1A;
        fdbEntry.key.key.macFid.macAddr.arEther[2] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[3] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[4] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[5] = 0xFF;

        fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
        fdbEntry.dstOutlif.interfaceInfo.trunkId = 100;
        fdbEntry.dstOutlif.outlifPointer.arpPtr  = 0;

        fdbEntry.isStatic                 = GT_FALSE;
        fdbEntry.daCommand                = CPSS_PACKET_CMD_FORWARD_E;
        fdbEntry.saCommand                = CPSS_PACKET_CMD_FORWARD_E;
        fdbEntry.daRoute                  = GT_FALSE;
        fdbEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
        fdbEntry.sourceId                 = 0;
        fdbEntry.userDefined              = 0;
        fdbEntry.daQosIndex               = 0;
        fdbEntry.saQosIndex               = 0;
        fdbEntry.daSecurityLevel          = 0;
        fdbEntry.saSecurityLevel          = 0;
        fdbEntry.appSpecificCpuCode       = GT_FALSE;
        fdbEntry.spUnknown                = GT_FALSE;

        fdbEntry.pwId   = 0;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
        -> CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E
            1.2. Call function with valid fdbEntryPtr {key {entryType[CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E],
                                                            macfId->fId[100],
                                                            macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                                       dstOutlif { outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                   interfaceInfo{ set to port default }
                                                                   outlifPointer{arpPtr[0]} }
                                                       isStatic [GT_TRUE],
                                                       daCommand [CPSS_PACKET_CMD_DROP_HARD_E],
                                                       saCommand [CPSS_PACKET_CMD_DROP_HARD_E],
                                                       daRoute [GT_TRUE],
                                                       mirrorToRxAnalyzerPortEn [GT_TRUE],
                                                       sourceID [0x7],
                                                       userDefined [0xF],
                                                       daQosIndex [0x7],
                                                       saQosIndex [0x7],
                                                       daSecurityLevel [0x7],
                                                       saSecurityLevel [0x7],
                                                       appSpecificCpuCode [GT_TRUE],
                                                       pwId[0] }.
            Expected: GT_OK.
        */
        fdbEntry.key.entryType      = CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E;
        fdbEntry.key.key.macFid.fId = 100;
        fdbEntry.key.key.macFid.macAddr.arEther[0] = 0x00;
        fdbEntry.key.key.macFid.macAddr.arEther[1] = 0x1A;
        fdbEntry.key.key.macFid.macAddr.arEther[2] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[3] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[4] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[5] = 0xFF;

        fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_PORT_E;
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum  = dev;
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = 0;
        fdbEntry.dstOutlif.outlifPointer.arpPtr  = 0;

        fdbEntry.isStatic                 = GT_TRUE;
        fdbEntry.daCommand                = CPSS_PACKET_CMD_DROP_HARD_E;
        fdbEntry.saCommand                = CPSS_PACKET_CMD_DROP_HARD_E;
        fdbEntry.daRoute                  = GT_TRUE;
        fdbEntry.mirrorToRxAnalyzerPortEn = GT_TRUE;
        fdbEntry.sourceId                 = 7;
        fdbEntry.userDefined              = 0xF;
        fdbEntry.daQosIndex               = 7;
        fdbEntry.saQosIndex               = 7;
        fdbEntry.daSecurityLevel          = 7;
        fdbEntry.saSecurityLevel          = 7;
        fdbEntry.appSpecificCpuCode       = GT_TRUE;
        fdbEntry.spUnknown                = GT_FALSE;

        fdbEntry.pwId   = 0;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
        -> CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E
            1.3. Call function with valid fdbEntryPtr {key {entryType[ CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E ],
                                                            ipMcast->fId[100],
                                                            ipMcast->sip[00:1A:FF:FF], ipMcast->dip[00:1A:FF:FF]},
                                                       dstOutlif { outlifType[ CPSS_EXMXPM_OUTLIF_TYPE_LL_E ],
                                                                   interfaceInfo{type[ CPSS_INTERFACE_VIDX_E ],
                                                                                vidx[100],
                                                                   outlifPointer{arpPtr[0],
                                                       daCommand [ CPSS_PACKET_CMD_DROP_SOFT_E ],
                                                       saCommand [ CPSS_PACKET_CMD_DROP_SOFT_E ],
                                                       sourceID [ 0],
                                                       userDefined [0xF],
                                                       daQosIndex [3],
                                                       saQosIndex [5],
                                                       daSecurityLevel [1],
                                                       saSecurityLevel [4],
                                                       appSpecificCpuCode [ GT_TRUE],
                                                       pwId[10] }.
                                 and other params from 1.1.
            Expected: GT_OK.
        */
        fdbEntry.key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E;
        fdbEntry.key.key.ipMcast.fId = 100;
        fdbEntry.key.key.ipMcast.sip[0] = 0x00;
        fdbEntry.key.key.ipMcast.sip[1] = 0x1A;
        fdbEntry.key.key.ipMcast.sip[2] = 0xFF;
        fdbEntry.key.key.ipMcast.sip[3] = 0xFF;
        fdbEntry.key.key.ipMcast.dip[0] = 0x00;
        fdbEntry.key.key.ipMcast.dip[1] = 0x1A;
        fdbEntry.key.key.ipMcast.dip[2] = 0xFF;
        fdbEntry.key.key.ipMcast.dip[3] = 0xFF;

        fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        fdbEntry.dstOutlif.interfaceInfo.type   = CPSS_INTERFACE_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.vidx   = 100;
        fdbEntry.dstOutlif.outlifPointer.arpPtr = 0;

        fdbEntry.daCommand                = CPSS_PACKET_CMD_DROP_SOFT_E;
        fdbEntry.saCommand                = CPSS_PACKET_CMD_DROP_SOFT_E;
        fdbEntry.sourceId                 = 0;
        fdbEntry.userDefined              = 0xF;
        fdbEntry.daQosIndex               = 3;
        fdbEntry.saQosIndex               = 5;
        fdbEntry.daSecurityLevel          = 1;
        fdbEntry.saSecurityLevel          = 4;
        fdbEntry.appSpecificCpuCode       = GT_FALSE;

        fdbEntry.pwId = 10;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
        -> CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E
            1.4. Call function with valid fdbEntryPtr {key {entryType[ CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E],
                                                            ipMcast->fId[100],
                                                            ipMcast->sip[00:1A:FF:FF], ipMcast->dip[00:1A:FF:FF]},
                                                       dstOutlif { outlifType[ CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E],
                                                                   interfaceInfo{type[ CPSS_INTERFACE_VID_E],
                                                                                vlanId[100]},
                                                                           outlifPointer{arpPtr[0],
                                                       daCommand [ CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                                       saCommand [ CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                                       sourceID [ 0],
                                                       userDefined [0xF],
                                                       daQosIndex [0x7],
                                                       saQosIndex [0x7],
                                                       daSecurityLevel [0x7],
                                                       saSecurityLevel [0x7],
                                                       appSpecificCpuCode [ GT_TRUE],
                                                       pwId[10] }.
            Expected: GT_OK.
        */
        fdbEntry.key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E;
        fdbEntry.key.key.ipMcast.fId = 100;
        fdbEntry.key.key.ipMcast.sip[0] = 0x00;
        fdbEntry.key.key.ipMcast.sip[1] = 0x1A;
        fdbEntry.key.key.ipMcast.sip[2] = 0xFF;
        fdbEntry.key.key.ipMcast.sip[3] = 0xFF;
        fdbEntry.key.key.ipMcast.dip[0] = 0x00;
        fdbEntry.key.key.ipMcast.dip[1] = 0x1A;
        fdbEntry.key.key.ipMcast.dip[2] = 0xFF;
        fdbEntry.key.key.ipMcast.dip[3] = 0xFF;

        fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_VID_E;
        fdbEntry.dstOutlif.interfaceInfo.vlanId  = 100;
        fdbEntry.dstOutlif.outlifPointer.arpPtr = 0;

        fdbEntry.daCommand                = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        fdbEntry.saCommand                = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        fdbEntry.sourceId                 = 0;
        fdbEntry.userDefined              = 0xF;
        fdbEntry.daQosIndex               = 7;
        fdbEntry.saQosIndex               = 7;
        fdbEntry.daSecurityLevel          = 7;
        fdbEntry.saSecurityLevel          = 7;
        fdbEntry.appSpecificCpuCode       = GT_TRUE;

        fdbEntry.pwId = 10;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.5. Call function with out of range fdbEntryPtr->key->entryType [0x5AAAAAA5]
                                    and other parameters the same as in 1.7.
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.key.entryType = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryPtr->key->entryType = %d",
                                     dev, fdbEntry.key.entryType);

        fdbEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

        /*
            1.6. Call with interfaceInfo.type [0x5AAAAAA5] out of range
                          and other valid params.
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryPtr->dstOutlif.interfaceInfo.type = %d",
                                     dev, fdbEntry.dstOutlif.interfaceInfo.type);

        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;

        /*
        [ CPSS_INTERFACE_TRUNK_E ]
            -> out of range
            1.7. Call with interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                            interfaceInfo.trunkId [BIT_8]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
        fdbEntry.dstOutlif.interfaceInfo.trunkId = BIT_8;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.trunkId = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.trunkId);

        fdbEntry.dstOutlif.interfaceInfo.trunkId = 100;
        /*
           -> not relevant
            1.8. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                            interfaceInfo.trunkId [BIT_8]
                            and other valid params.
            Expected: GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_PORT_E;
        fdbEntry.dstOutlif.interfaceInfo.trunkId = BIT_8;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.trunkId = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.trunkId);

        fdbEntry.dstOutlif.interfaceInfo.trunkId = 100;

        /*
        [ CPSS_INTERFACE_PORT_E ]
            -> out of range
            1.9. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                            interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = 0;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devPort.devNum = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.devPort.devNum);

        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = dev;

        /*
            1.10. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                            interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = dev;
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devPort.portNum = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.devPort.portNum);

        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = 0;

        /*
           -> not relevant
            1.11. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                            interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other valid params.
            Expected: GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = 0;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devPort.devNum = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.devPort.devNum);

        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = dev;

        /*
            1.12. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                            interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                            and other valid params.
            Expected: GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = dev;
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devPort.portNum = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.devPort.portNum);

        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = 0;
        /*
        [ CPSS_INTERFACE_VIDX_E ]
            -> out of range
            1.13. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                            interfaceInfo.vidx [4096]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.vidx = 4096;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.vidx = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.vidx);

        fdbEntry.dstOutlif.interfaceInfo.vidx = 100;

        /*
           -> not relevant
            1.14. Call with interfaceInfo.type [CPSS_INTERFACE_VID_E]
                            interfaceInfo.vidx [4096]
                            and other valid params.
            Expected: GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_VID_E;
        fdbEntry.dstOutlif.interfaceInfo.vidx = 4096;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.vidx = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.vidx);

        fdbEntry.dstOutlif.interfaceInfo.vidx = 100;

        /*
        [ CPSS_INTERFACE_DEVICE_E ]
            -> out of range
            1.17. Call with interfaceInfo.type [CPSS_INTERFACE_DEVICE_E]
                            interfaceInfo.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_DEVICE_E;
        fdbEntry.dstOutlif.interfaceInfo.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devNum = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.devNum);

        fdbEntry.dstOutlif.interfaceInfo.devNum = dev;

        /*
        [ CPSS_INTERFACE_FABRIC_VIDX_E ]
            -> out of range
            1.19. Call with interfaceInfo.type [CPSS_INTERFACE_FABRIC_VIDX_E]
                            interfaceInfo.fabricVidx [4096]
                           and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_FABRIC_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.fabricVidx = 4096;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.fabricVidx = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.fabricVidx);

        fdbEntry.dstOutlif.interfaceInfo.fabricVidx = 100;
        /*
            -> not relevant
            1.20. Call with interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                            interfaceInfo.fabricVidx [4096]
                           and other valid params.
            Expected: GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        fdbEntry.dstOutlif.interfaceInfo.fabricVidx = 4096;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.fabricVidx = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.fabricVidx);

        fdbEntry.dstOutlif.interfaceInfo.fabricVidx = 100;

        /*
            1.21. Call function with out of range fdbEntryPtr->daCommand [0x5AAAAAA5]
                                     and other parameters the same as in 1.7.
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.daCommand = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryPtr->daCommand = %d",
                                     dev, fdbEntry.daCommand);

        fdbEntry.daCommand = CPSS_PACKET_CMD_FORWARD_E;

        /*
            1.22. Call function with out of range fdbEntryPtr->saCommand [0x5AAAAAA5],
                                     key->entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E] (saCommand is not important for IPv4 and IPv6 MC).
                                     and other parameters the same as in 1.7.
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.saCommand     = BRG_FDB_INVALID_ENUM_CNS;
        fdbEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryPtr->saCommand = %d",
                                     dev, fdbEntry.saCommand);

        fdbEntry.saCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        /*
            1.23. Call function with out of range fdbEntryPtr->userDefined [0x10]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.userDefined = 0x10;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->userDefined = %d",
                                     dev, fdbEntry.userDefined);

        fdbEntry.userDefined = 0x10;

        /*
            1.24. Call function with out of range fdbEntryPtr->daQosIndex [0x8]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.daQosIndex = 0x8;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->daQosIndex = %d",
                                     dev, fdbEntry.daQosIndex);

        fdbEntry.daQosIndex = 0x8;

        /*
            1.25. Call function with out of range fdbEntryPtr->saQosIndex [0x8]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.saQosIndex = 0x8;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->saQosIndex = %d",
                                     dev, fdbEntry.saQosIndex);

        fdbEntry.saQosIndex = 0x8;

        /*
            1.26. Call function with out of range fdbEntryPtr->daSecurityLevel [0x8]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.daSecurityLevel = 0x8;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->daSecurityLevel = %d",
                                     dev, fdbEntry.daSecurityLevel);

        fdbEntry.daSecurityLevel = 0x8;

        /*
            1.27. Call function with out of range fdbEntryPtr->saSecurityLevel [0x8]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.saSecurityLevel = 0x8;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->saSecurityLevel = %d",
                                     dev, fdbEntry.saSecurityLevel);

        fdbEntry.saSecurityLevel = 0x8;

        /*
            1.28. Call with fdbEntryPtr->dstOutlif.outlifType [0x5AAAAAA5] (out of range)
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.dstOutlif.outlifType = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntry.dstOutlif.outlifType = %d",
                                     dev, fdbEntry.dstOutlif.outlifType);

        fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_DIT_E;

        /*
            1.29. Call function with null fdbEntryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbEntrySet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fdbEntryPtr = NULL", dev);
    }

    fdbEntry.key.entryType      = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    fdbEntry.key.key.macFid.fId = 100;
    fdbEntry.key.key.macFid.macAddr.arEther[0] = 0x00;
    fdbEntry.key.key.macFid.macAddr.arEther[1] = 0x1A;
    fdbEntry.key.key.macFid.macAddr.arEther[2] = 0xFF;
    fdbEntry.key.key.macFid.macAddr.arEther[3] = 0xFF;
    fdbEntry.key.key.macFid.macAddr.arEther[4] = 0xFF;
    fdbEntry.key.key.macFid.macAddr.arEther[5] = 0xFF;

    fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
    fdbEntry.dstOutlif.interfaceInfo.trunkId = 100;
    fdbEntry.dstOutlif.outlifPointer.arpPtr  = 0;

    fdbEntry.isStatic                 = GT_FALSE;
    fdbEntry.daCommand                = CPSS_PACKET_CMD_FORWARD_E;
    fdbEntry.saCommand                = CPSS_PACKET_CMD_FORWARD_E;
    fdbEntry.daRoute                  = GT_FALSE;
    fdbEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
    fdbEntry.sourceId                 = 0;
    fdbEntry.userDefined              = 0;
    fdbEntry.daQosIndex               = 0;
    fdbEntry.saQosIndex               = 0;
    fdbEntry.daSecurityLevel          = 0;
    fdbEntry.saSecurityLevel          = 0;
    fdbEntry.appSpecificCpuCode       = GT_FALSE;
    fdbEntry.spUnknown                = GT_FALSE;

    fdbEntry.pwId   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbEntrySet(dev, &fdbEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbQaSend
(
    IN  GT_U8                          devNum,
    IN  CPSS_EXMXPM_FDB_ENTRY_KEY_STC  *fdbEntryKeyPtr
)
*/

/* 1. GM doesn't support the functionality of QA messaging
   2. After sending QA message, GM is stucked when trying to write FDB entry
      cpssExMxPmBrgFdbEntryWrite */
#if 0
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbQaSend)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with valid fdbEntryKeyPtr {entryType [CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E],
                            macFid->fId[100],
                            macFid->macAddr[00:1A:1B:1C:1D:1E]}
    Expected: GT_OK.
    1.2. Call function with valid fdbEntryKeyPtr {entryType [CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E],
                            ipMcast->sip[123.45.67.8],
                            ipMcast->dip[123.23.34.5],
                            ipMacst->fId [100]}
    Expected: GT_OK.
    1.3. Call function with null fdbEntryKeyPtr [NULL].
    Expected: GT_BAD_PTR.
    1.4. Call function with out of range fdbEntryKeyPtr->entryType [0x5AAAAAA5]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_ENTRY_KEY_STC fdbEntry;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with valid fdbEntryKeyPtr {entryType [CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E],
                                    macFid->fId[100],
                                    macFid->macAddr[00:1A:1B:1C:1D:1E]}
            Expected: GT_OK.
        */
        fdbEntry.entryType      = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        fdbEntry.key.macFid.fId = 100;

        fdbEntry.key.macFid.macAddr.arEther[0] = 0x0;
        fdbEntry.key.macFid.macAddr.arEther[1] = 0x1A;
        fdbEntry.key.macFid.macAddr.arEther[2] = 0x1B;
        fdbEntry.key.macFid.macAddr.arEther[3] = 0x1C;
        fdbEntry.key.macFid.macAddr.arEther[4] = 0x1D;
        fdbEntry.key.macFid.macAddr.arEther[5] = 0x1E;

        st = cpssExMxPmBrgFdbQaSend(dev, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with valid fdbEntryKeyPtr {entryType [CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E],
                                    ipMcast->sip[123.45.67.8],
                                    ipMcast->dip[123.23.34.5],
                                    ipMacst->fId [100]}
            Expected: GT_OK.
        */
        fdbEntry.entryType       = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
        fdbEntry.key.ipMcast.fId = 100;

        fdbEntry.key.ipMcast.sip[0] = 123;
        fdbEntry.key.ipMcast.sip[1] = 45;
        fdbEntry.key.ipMcast.sip[2] = 67;
        fdbEntry.key.ipMcast.sip[3] = 8;

        fdbEntry.key.ipMcast.dip[0] = 123;
        fdbEntry.key.ipMcast.dip[1] = 23;
        fdbEntry.key.ipMcast.dip[2] = 34;
        fdbEntry.key.ipMcast.dip[3] = 5;

        st = cpssExMxPmBrgFdbQaSend(dev, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.3. Call function with null fdbEntryKeyPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbQaSend(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fdbEntryKeyPtr = NULL", dev);

        /*
            1.4. Call function with out of range fdbEntryKeyPtr->entryType [0x5AAAAAA5]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.entryType = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbQaSend(dev, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryPtr->entryType = %d",
                                     dev, fdbEntry.entryType);
    }

    fdbEntry.entryType      = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    fdbEntry.key.macFid.fId = 100;

    fdbEntry.key.macFid.macAddr.arEther[0] = 0x0;
    fdbEntry.key.macFid.macAddr.arEther[1] = 0x1A;
    fdbEntry.key.macFid.macAddr.arEther[2] = 0x1B;
    fdbEntry.key.macFid.macAddr.arEther[3] = 0x1C;
    fdbEntry.key.macFid.macAddr.arEther[4] = 0x1D;
    fdbEntry.key.macFid.macAddr.arEther[5] = 0x1E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbQaSend(dev, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbQaSend(dev, &fdbEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
#endif /* if 0*/
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbEntryDelete
(
    IN GT_U8                           devNum,
    IN CPSS_EXMXPM_FDB_ENTRY_KEY_STC   *fdbEntryKeyPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbEntryDelete)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with valid fdbEntryKeyPtr {entryType [CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E],
                            macFid->fId[100],
                            macFid->macAddr[00:1A:1B:1C:1D:1E]}
    Expected: GT_OK.
    1.2. Call with fdbEntry.key.macFid->fId [BIT_16] (out of range)
    Expected: NOT GT_OK.
    1.3. Call function with valid fdbEntryKeyPtr {entryType [CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E],
                            ipMcast->sip[123.45.67.8],
                            ipMcast->dip[123.23.34.5],
                            ipMacst->fId [100]}
    Expected: GT_OK.
    1.4. Call function with null fdbEntryKeyPtr [NULL].
    Expected: GT_BAD_PTR.
    1.5. Call function with out of range fdbEntryKeyPtr->entryType [0x5AAAAAA5]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with fdbEntry.key.ipMcast.fId [BIT_16] (out of range)
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_ENTRY_KEY_STC fdbEntry;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with valid fdbEntryKeyPtr {entryType [CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E],
                                    macFid->fId[100],
                                    macFid->macAddr[00:1A:1B:1C:1D:1E]}
            Expected: GT_OK.
        */
        fdbEntry.entryType      = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        fdbEntry.key.macFid.fId = 100;

        fdbEntry.key.macFid.macAddr.arEther[0] = 0x0;
        fdbEntry.key.macFid.macAddr.arEther[1] = 0x1A;
        fdbEntry.key.macFid.macAddr.arEther[2] = 0x1B;
        fdbEntry.key.macFid.macAddr.arEther[3] = 0x1C;
        fdbEntry.key.macFid.macAddr.arEther[4] = 0x1D;
        fdbEntry.key.macFid.macAddr.arEther[5] = 0x1E;

        st = cpssExMxPmBrgFdbEntryDelete(dev, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with fdbEntry.key.macFid->fId [BIT_16] (out of range)
            Expected: NOT GT_OK.
        */
        fdbEntry.key.macFid.fId = 0x10000;

        st = cpssExMxPmBrgFdbEntryDelete(dev, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntry.key.macFid.fId = %d",
                                         dev, fdbEntry.key.macFid.fId);

        fdbEntry.key.macFid.fId = 100;

        /*
            1.3. Call function with valid fdbEntryKeyPtr {entryType [CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E],
                                    ipMcast->sip[123.45.67.8],
                                    ipMcast->dip[123.23.34.5],
                                    ipMacst->fId [100]}
            Expected: GT_OK.
        */
        fdbEntry.entryType       = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
        fdbEntry.key.ipMcast.fId = 100;

        fdbEntry.key.ipMcast.sip[0] = 123;
        fdbEntry.key.ipMcast.sip[1] = 45;
        fdbEntry.key.ipMcast.sip[2] = 67;
        fdbEntry.key.ipMcast.sip[3] = 8;

        fdbEntry.key.ipMcast.dip[0] = 123;
        fdbEntry.key.ipMcast.dip[1] = 23;
        fdbEntry.key.ipMcast.dip[2] = 34;
        fdbEntry.key.ipMcast.dip[3] = 5;

        st = cpssExMxPmBrgFdbEntryDelete(dev, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.4. Call function with null fdbEntryKeyPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbEntryDelete(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fdbEntryKeyPtr = NULL", dev);

        /*
            1.5. Call function with out of range fdbEntryKeyPtr->entryType [0x5AAAAAA5]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.entryType = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbEntryDelete(dev, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryPtr->key->entryType = %d",
                                     dev, fdbEntry.entryType);

        fdbEntry.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E;

        /*
            1.6. Call with fdbEntry.key.ipMcast.fId [BIT_16] (out of range)
            Expected: NOT GT_OK.
        */
        fdbEntry.key.ipMcast.fId = 0x10000;

        st = cpssExMxPmBrgFdbEntryDelete(dev, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntry.key.ipMcast.fId = %d",
                                         dev, fdbEntry.key.ipMcast.fId);
    }

    fdbEntry.entryType      = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    fdbEntry.key.macFid.fId = 100;

    fdbEntry.key.macFid.macAddr.arEther[0] = 0x0;
    fdbEntry.key.macFid.macAddr.arEther[1] = 0x1A;
    fdbEntry.key.macFid.macAddr.arEther[2] = 0x1B;
    fdbEntry.key.macFid.macAddr.arEther[3] = 0x1C;
    fdbEntry.key.macFid.macAddr.arEther[4] = 0x1D;
    fdbEntry.key.macFid.macAddr.arEther[5] = 0x1E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbEntryDelete(dev, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbEntryDelete(dev, &fdbEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U32                       index,
    IN GT_BOOL                      skip,
    IN CPSS_EXMXPM_FDB_ENTRY_STC    *fdbEntryPtr

)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbEntryWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with index [0],
                            skip [GT_FALSE]
                            fdbEntryPtr {key {entryType[CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                                    macfId->fId[100],
                                                    macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                               dstOutlif { outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                           interfaceInfo{type[CPSS_INTERFACE_TRUNK_E],
                                                                        trunkId[100],
                                                           outlifPointer{arpPtr[0],
                                               isStatic [GT_FALSE],
                                               daCommand [CPSS_PACKET_CMD_FORWARD_E],
                                               saCommand [CPSS_PACKET_CMD_FORWARD_E],
                                               daRoute [GT_FALSE],
                                               mirrorToRxAnalyzerPortEn [GT_FALSE],
                                               sourceID [0],
                                               userDefined [0],
                                               daQosIndex [0],
                                               saQosIndex [0],
                                               daSecurityLevel [0],
                                               saSecurityLevel [0],
                                               appSpecificCpuCode [GT_FALSE],
                                               pwId[0] }.
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbEntryRead with index [0] and non-NULL pointers.
    Expected: GT_OK and the same parameters as input was - check by fields (only valid).
    1.3. Call function with index [1],
                            skip [GT_FALSE]
                            and valid fdbEntryPtr {key {entryType[ CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E ],
                                                    ipMcast->fId[100],
                                                    ipMcast->sip[00:1A:FF:FF], ipMcast->dip[00:1A:FF:FF]},
                                               dstOutlif { outlifType[ CPSS_EXMXPM_OUTLIF_TYPE_LL_E ],
                                                           interfaceInfo{type[ CPSS_INTERFACE_VIDX_E ],
                                                                        vidx[100],
                                                           outlifPointer{arpPtr[0],
                                               daCommand [ CPSS_PACKET_CMD_DROP_SOFT_E ],
                                               sourceID [ 0],
                                               userDefined [0],
                                               daQosIndex [3],
                                               saQosIndex [5],
                                               daSecurityLevel [1],
                                               saSecurityLevel [4],
                                               appSpecificCpuCode [ GT_TRUE],
                                               pwId[10]
                                               and other params from 1.1.}
    Expected: GT_OK.
    1.4. Call cpssExMxPmBrgFdbEntryRead with index [1] and non-NULL pointers.
    Expected: GT_OK and the same parameters as input. (don't check fdbEntry.saCommand, fdbEntry.sourceId)
    1.5. Call function with index [2],
                            skip [GT_FALSE]
                            and valid fdbEntryPtr {key {entryType[ CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E],
                                                    macfId->fId[100],
                                                    macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                               dstOutlif { outlifType[ CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                           interfaceInfo{type[ CPSS_INTERFACE_VIDX_E],
                                                                        vidx[100]},
                                                           outlifPointer{arpPtr[0],
                                               daCommand [ CPSS_PACKET_CMD_DROP_HARD_E],
                                               and other params from 1.1.}
    Expected: GT_OK.
    1.6. Call cpssExMxPmBrgFdbEntryRead with index [2] and non-NULL pointers.
    Expected: GT_OK and the same parameters as input (don't check macEntry.saCommand, macEntry.sourceId)

    1.7. Call function with out of range index [macTableSize(dev)]
                             and other params from 1.10.
    Expected: NOT GT_OK.
    1.8. Call function with null fdbEntryPtr [NULL].
    Expected: GT_BAD_PTR.
    1.9. Call function with out of range key->entryType [0x5AAAAAA5]
                             and other parameters the same as in 1.10.
    Expected: GT_BAD_PARAM.

    1.10. Call with interfaceInfo.type [0x5AAAAAA5] out of range
                  and other valid params.
    Expected: GT_BAD_PARAM.
[ CPSS_INTERFACE_TRUNK_E ]
    -> out of range
    1.10. Call with interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                    interfaceInfo.trunkId [4096]
                    and other valid params.
    Expected: NOT GT_OK.

   -> not relevant
    1.11. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                    interfaceInfo.trunkId [4096]
                    and other valid params.
    Expected: GT_OK.

[ CPSS_INTERFACE_PORT_E ]
    -> out of range
    1.12. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                    interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other valid params.
    Expected: NOT GT_OK.
    1.13. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                    interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                    and other valid params.
    Expected: NOT GT_OK.

   -> not relevant
    1.14. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                    interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other valid params.
    Expected: NOT GT_OK.
    1.15. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                    interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                    and other valid params.
    Expected: NOT GT_OK.

[ CPSS_INTERFACE_VIDX_E ]
    -> out of range
    1.16. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                    interfaceInfo.vidx [4096]
                    and other valid params.
    Expected: NOT GT_OK.

   -> not relevant
    1.17. Call with interfaceInfo.type [CPSS_INTERFACE_VID_E]
                    interfaceInfo.vidx [4096]
                    and other valid params.
    Expected: GT_OK.

[ CPSS_INTERFACE_DEVICE_E ]
    -> out of range
    1.20. Call with interfaceInfo.type [CPSS_INTERFACE_DEVICE_E]
                    interfaceInfo.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other valid params.
    Expected: NOT GT_OK.

[ CPSS_INTERFACE_FABRIC_VIDX_E ]
    -> out of range
    1.22. Call with interfaceInfo.type [CPSS_INTERFACE_FABRIC_VIDX_E]
                    interfaceInfo.fabricVidx [4096]
                   and other valid params.
    Expected: NOT GT_OK.

    -> not relevant
    1.23. Call with interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                    interfaceInfo.fabricVidx [4096]
                   and other valid params.
    Expected: GT_OK.

    1.24. Call function with out of range fdbEntryPtr->daCommand [0x5AAAAAA5]
                             and other parameters the same as in 1.10.
    Expected: GT_BAD_PARAM.
    1.25. Call function with out of range fdbEntryPtr->saCommand [0x5AAAAAA5]
                             and other parameters the same as in 1.10.
    Expected: GT_BAD_PARAM.
    1.26. Call function with out of range fdbEntryPtr->userDefined [0x10]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.27. Call function with out of range fdbEntryPtr->daQosIndex [0x8]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.28. Call function with out of range fdbEntryPtr->saQosIndex [0x8]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.29. Call function with out of range fdbEntryPtr->daSecurityLevel [0x8]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.30. Call function with out of range fdbEntryPtr->saSecurityLevel [0x8]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.31. Call cpssExMxPmBrgFdbEntryInvalidate with index [0/ 1/ 2] to invalidate all changes
    Expected: GT_OK.
*/
    GT_STATUS                 st          = GT_OK;
    GT_U8                     dev;

    GT_U32                    index       = 0;
    GT_BOOL                   skip        = GT_FALSE;
    CPSS_EXMXPM_FDB_ENTRY_STC fdbEntry;
    GT_BOOL                   validGet    = GT_FALSE;
    GT_BOOL                   skipGet     = GT_FALSE;
    GT_BOOL                   agedGet     = GT_FALSE;
    GT_U8                     devNumGet   = 0;
    CPSS_EXMXPM_FDB_ENTRY_STC entryGet;
    GT_BOOL                   isEqual     = GT_FALSE;


    cpssOsBzero((GT_VOID*) &fdbEntry, sizeof(fdbEntry));
    cpssOsBzero((GT_VOID*) &entryGet, sizeof(entryGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with index [0],
                                    skip [GT_FALSE]
                                    fdbEntryPtr {key {entryType[CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                                            macfId->fId[100],
                                                            macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                                       dstOutlif { outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                   interfaceInfo{type[CPSS_INTERFACE_TRUNK_E],
                                                                                trunkId[100],
                                                                   outlifPointer{arpPtr[0],
                                                       isStatic [GT_FALSE],
                                                       daCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                       saCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                       daRoute [GT_FALSE],
                                                       mirrorToRxAnalyzerPortEn [GT_FALSE],
                                                       sourceID [0],
                                                       userDefined [0],
                                                       daQosIndex [0],
                                                       saQosIndex [0],
                                                       daSecurityLevel [0],
                                                       saSecurityLevel [0],
                                                       appSpecificCpuCode [GT_FALSE],
                                                       pwId[0] }.
            Expected: GT_OK.
        */
        index = 0;
        skip  = GT_FALSE;

        cpssOsBzero((GT_VOID*) &fdbEntry, sizeof(fdbEntry));
        fdbEntry.key.entryType      = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        fdbEntry.key.key.macFid.fId = 100;
        fdbEntry.key.key.macFid.macAddr.arEther[0] = 0x00;
        fdbEntry.key.key.macFid.macAddr.arEther[1] = 0x1A;
        fdbEntry.key.key.macFid.macAddr.arEther[2] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[3] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[4] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[5] = 0xFF;

        fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
        fdbEntry.dstOutlif.interfaceInfo.trunkId = 100;
        fdbEntry.dstOutlif.outlifPointer.arpPtr  = 0;

        fdbEntry.isStatic                 = GT_FALSE;
        fdbEntry.daCommand                = CPSS_PACKET_CMD_FORWARD_E;
        fdbEntry.saCommand                = CPSS_PACKET_CMD_FORWARD_E;
        fdbEntry.daRoute                  = GT_FALSE;
        fdbEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
        fdbEntry.sourceId                 = 0;
        fdbEntry.userDefined              = 0;
        fdbEntry.daQosIndex               = 0;
        fdbEntry.saQosIndex               = 0;
        fdbEntry.daSecurityLevel          = 0;
        fdbEntry.saSecurityLevel          = 0;
        fdbEntry.appSpecificCpuCode       = GT_FALSE;
        fdbEntry.spUnknown                = GT_FALSE;

        fdbEntry.pwId   = 0;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        /*
            1.2. Call cpssExMxPmBrgFdbEntryRead with index [0] and non-NULL pointers.
            Expected: GT_OK and the same parameters as input was - check by fields (only valid).
        */
        index = 0;
        cpssOsBzero((GT_VOID*) &entryGet, sizeof(entryGet));

        st = cpssExMxPmBrgFdbEntryRead(dev, index, &validGet, &skipGet, &agedGet, &devNumGet, &entryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgFdbEntryRead: %d, %d", dev, index);

        /* Verifying struct fields */
        UTF_VERIFY_EQUAL1_STRING_MAC(skip, skipGet,
                   "get another skipPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.key.entryType, entryGet.key.entryType,
                   "get another fdbEntryPtr->key.entryType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.key.key.macFid.fId, entryGet.key.key.macFid.fId,
                   "get another fdbEntryPtr->key.key.macFid.fId than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &fdbEntry.key.key.macFid.macAddr,
                               (GT_VOID*) &entryGet.key.key.macFid.macAddr,
                                sizeof(fdbEntry.key.key.macFid.macAddr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another fdbEntryPtr->key.key.macFid.macAddr than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &fdbEntry.dstOutlif,
                               (GT_VOID*) &entryGet.dstOutlif,
                                sizeof(fdbEntry.dstOutlif))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another fdbEntryPtr->.dstOutlif than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.isStatic, entryGet.isStatic,
                   "get another fdbEntryPtr->isStatic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daCommand, entryGet.daCommand,
                   "get another fdbEntryPtr->daCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.saCommand, entryGet.saCommand,
                   "get another fdbEntryPtr->saCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.mirrorToRxAnalyzerPortEn, entryGet.mirrorToRxAnalyzerPortEn,
                   "get another fdbEntryPtr->mirrorToRxAnalyzerPortEn than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.sourceId, entryGet.sourceId,
                   "get another fdbEntryPtr->sourceId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daQosIndex, entryGet.daQosIndex,
                   "get another fdbEntryPtr->daQosIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daSecurityLevel, entryGet.daSecurityLevel,
                   "get another fdbEntryPtr->daSecurityLevel than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.appSpecificCpuCode, entryGet.appSpecificCpuCode,
                   "get another fdbEntryPtr->appSpecificCpuCode than was set: %d", dev);

        /*
            1.3. Call function with index [1],
                                    skip [GT_FALSE]
                                    and valid fdbEntryPtr {key {entryType[ CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E ],
                                                            macfId->fId[100],
                                                            macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                                       dstOutlif { outlifType[ CPSS_EXMXPM_OUTLIF_TYPE_LL_E ],
                                                                   interfaceInfo{type[ CPSS_INTERFACE_VIDX_E ],
                                                                                vidx[100],
                                                                   outlifPointer{arpPtr[0],
                                                       daCommand [ CPSS_PACKET_CMD_DROP_SOFT_E ],
                                                       sourceID [ 0],
                                                       userDefined [0],
                                                       daQosIndex [3],
                                                       saQosIndex [5],
                                                       daSecurityLevel [1],
                                                       saSecurityLevel [4],
                                                       appSpecificCpuCode [ GT_TRUE],
                                                       pwId[10]
                                                       and other params from 1.1.}
            Expected: GT_OK.
        */
        index = 1;
        skip  = GT_FALSE;

        cpssOsBzero((GT_VOID*) &fdbEntry, sizeof(fdbEntry));
        fdbEntry.key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E;
        fdbEntry.key.key.macFid.fId = 100;
        fdbEntry.key.key.macFid.macAddr.arEther[0] = 0x00;
        fdbEntry.key.key.macFid.macAddr.arEther[1] = 0x1A;
        fdbEntry.key.key.macFid.macAddr.arEther[2] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[3] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[4] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[5] = 0xFF;

        fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        fdbEntry.dstOutlif.interfaceInfo.type   = CPSS_INTERFACE_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.vidx   = 100;
        fdbEntry.dstOutlif.outlifPointer.arpPtr = 0;

        fdbEntry.daCommand                = CPSS_PACKET_CMD_DROP_SOFT_E;
        fdbEntry.sourceId                 = 0;
        fdbEntry.userDefined              = 0;
        fdbEntry.daQosIndex               = 3;
        fdbEntry.saQosIndex               = 5;
        fdbEntry.daSecurityLevel          = 1;
        fdbEntry.saSecurityLevel          = 4;
        fdbEntry.appSpecificCpuCode       = GT_TRUE;

        fdbEntry.pwId = 10;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        /*
            1.4. Call cpssExMxPmBrgFdbEntryRead with index [1] and non-NULL pointers.
            Expected: GT_OK and the same parameters as input. (don't check fdbEntry.saCommand, fdbEntry.sourceId)
        */
        index = 1;
        cpssOsBzero((GT_VOID*) &entryGet, sizeof(entryGet));

        st = cpssExMxPmBrgFdbEntryRead(dev, index, &validGet, &skipGet, &agedGet, &devNumGet, &entryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgFdbEntryRead: %d, %d", dev, index);

        /* Verifying struct fields */
        UTF_VERIFY_EQUAL1_STRING_MAC(skip, skipGet,
                   "get another skipPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.key.entryType, entryGet.key.entryType,
                   "get another fdbEntryPtr->key.entryType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.key.key.macFid.fId, entryGet.key.key.macFid.fId,
                   "get another fdbEntryPtr->key.key.macFid.fId than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &fdbEntry.key.key.macFid.macAddr,
                               (GT_VOID*) &entryGet.key.key.macFid.macAddr,
                                sizeof(fdbEntry.key.key.macFid.macAddr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another fdbEntryPtr->key.key.macFid.macAddr than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &fdbEntry.dstOutlif,
                               (GT_VOID*) &entryGet.dstOutlif,
                                sizeof(fdbEntry.dstOutlif))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another fdbEntryPtr->.dstOutlif than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.isStatic, entryGet.isStatic,
                   "get another fdbEntryPtr->isStatic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daCommand, entryGet.daCommand,
                   "get another fdbEntryPtr->daCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.mirrorToRxAnalyzerPortEn, entryGet.mirrorToRxAnalyzerPortEn,
                   "get another fdbEntryPtr->mirrorToRxAnalyzerPortEn than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daQosIndex, entryGet.daQosIndex,
                   "get another fdbEntryPtr->daQosIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daSecurityLevel, entryGet.daSecurityLevel,
                   "get another fdbEntryPtr->daSecurityLevel than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.appSpecificCpuCode, entryGet.appSpecificCpuCode,
                   "get another fdbEntryPtr->appSpecificCpuCode than was set: %d", dev);

        /*
            1.5. Call function with index [2],
                                    skip [GT_FALSE]
                                    and valid fdbEntryPtr {key {entryType[ CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E],
                                                            macfId->fId[100],
                                                            macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                                       dstOutlif { outlifType[ CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                   interfaceInfo{type[ CPSS_INTERFACE_VIDX_E],
                                                                                vidx[100]},
                                                                   outlifPointer{arpPtr[0]},
                                                       daCommand [ CPSS_PACKET_CMD_DROP_HARD_E],
                                                       saCommand [ CPSS_PACKET_CMD_DROP_HARD_E]
                                                       and other params from 1.1.}
            Expected: GT_OK.
        */
        index = 2;
        skip  = GT_FALSE;

        cpssOsBzero((GT_VOID*) &fdbEntry, sizeof(fdbEntry));
        fdbEntry.key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E;
        fdbEntry.key.key.macFid.fId = 100;
        fdbEntry.key.key.macFid.macAddr.arEther[0] = 0x00;
        fdbEntry.key.key.macFid.macAddr.arEther[1] = 0x1A;
        fdbEntry.key.key.macFid.macAddr.arEther[2] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[3] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[4] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[5] = 0xFF;

        fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        fdbEntry.dstOutlif.interfaceInfo.type   = CPSS_INTERFACE_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.vidx   = 100;
        fdbEntry.dstOutlif.outlifPointer.arpPtr  = 0;

        fdbEntry.daCommand = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        /*
            1.6. Call cpssExMxPmBrgFdbEntryRead with index [2] and non-NULL pointers.
            Expected: GT_OK and the same parameters as input (don't check macEntry.saCommand, macEntry.sourceId)
        */
        index = 2;
        cpssOsBzero((GT_VOID*) &entryGet, sizeof(entryGet));

        st = cpssExMxPmBrgFdbEntryRead(dev, index, &validGet, &skipGet, &agedGet, &devNumGet, &entryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgFdbEntryRead: %d, %d", dev, index);

        /* Verifying struct fields */
        UTF_VERIFY_EQUAL1_STRING_MAC(skip, skipGet,
                   "get another skipPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.key.entryType, entryGet.key.entryType,
                   "get another fdbEntryPtr->key.entryType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.key.key.macFid.fId, entryGet.key.key.macFid.fId,
                   "get another fdbEntryPtr->key.key.macFid.fId than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &fdbEntry.key.key.macFid.macAddr,
                               (GT_VOID*) &entryGet.key.key.macFid.macAddr,
                                sizeof(fdbEntry.key.key.macFid.macAddr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another fdbEntryPtr->key.key.macFid.macAddr than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &fdbEntry.dstOutlif,
                               (GT_VOID*) &entryGet.dstOutlif,
                                sizeof(fdbEntry.dstOutlif))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another fdbEntryPtr->.dstOutlif than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.isStatic, entryGet.isStatic,
                   "get another fdbEntryPtr->isStatic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daCommand, entryGet.daCommand,
                   "get another fdbEntryPtr->daCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.mirrorToRxAnalyzerPortEn, entryGet.mirrorToRxAnalyzerPortEn,
                   "get another fdbEntryPtr->mirrorToRxAnalyzerPortEn than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.userDefined, entryGet.userDefined,
                   "get another fdbEntryPtr->userDefined than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.saQosIndex, entryGet.saQosIndex,
                   "get another fdbEntryPtr->saQosIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daSecurityLevel, entryGet.daSecurityLevel,
                   "get another fdbEntryPtr->daSecurityLevel than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.appSpecificCpuCode, entryGet.appSpecificCpuCode,
                   "get another fdbEntryPtr->appSpecificCpuCode than was set: %d", dev);

        /*
            1.7. Call function with out of range index [macTableSize(dev)]
                                     and other params from 1.10.
            Expected: NOT GT_OK.
        */
        st = prvUtfFdbMacTableSize(dev, &index);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFdbMacTableSize: %d", dev);

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.8. Call function with null fdbEntryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fdbEntryPtr = NULL", dev);

        /*
            1.9. Call function with out of range key->entryType [0x5AAAAAA5]
                                     and other parameters the same as in 1.10.
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.key.entryType = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryPtr->key->entryType = %d",
                                     dev, fdbEntry.key.entryType);

        fdbEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

        /*
            1.10. Call with interfaceInfo.type [0x5AAAAAA5] out of range
                          and other valid params.
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryPtr->dstOutlif.interfaceInfo.type = %d",
                                     dev, fdbEntry.dstOutlif.interfaceInfo.type);

        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;

        /*
        [ CPSS_INTERFACE_TRUNK_E ]
            -> out of range
            1.10. Call with interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                            interfaceInfo.trunkId [4096]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
        fdbEntry.dstOutlif.interfaceInfo.trunkId = BIT_8;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.trunkId = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.trunkId);

        fdbEntry.dstOutlif.interfaceInfo.trunkId = 100;

        /*
           -> not relevant
            1.11. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                            interfaceInfo.trunkId [4096]
                            and other valid params.
            Expected: GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_PORT_E;
        fdbEntry.dstOutlif.interfaceInfo.trunkId = BIT_8;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.trunkId = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.trunkId);

        fdbEntry.dstOutlif.interfaceInfo.trunkId = 100;

        /*
        [ CPSS_INTERFACE_PORT_E ]
            -> out of range
            1.12. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                            interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = 0;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devPort.devNum = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.devPort.devNum);

        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = dev;

        /*
            1.13. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                            interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = dev;
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devPort.portNum = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.devPort.portNum);

        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = 0;

        /*
           -> not relevant
            1.14. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                            interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = 0;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devPort.devNum = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.devPort.devNum);

        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = dev;

        /*
            1.15. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                            interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = dev;
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devPort.portNum = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.devPort.portNum);

        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = 0;

        /*
        [ CPSS_INTERFACE_VIDX_E ]
            -> out of range
            1.16. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                            interfaceInfo.vidx [4096]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.vidx = 4096;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.vidx = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.vidx);

        fdbEntry.dstOutlif.interfaceInfo.vidx = 100;

        /*
           -> not relevant
            1.17. Call with interfaceInfo.type [CPSS_INTERFACE_VID_E]
                            interfaceInfo.vidx [4096]
                            and other valid params.
            Expected: GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_VID_E;
        fdbEntry.dstOutlif.interfaceInfo.vidx = 4096;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.vidx = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.vidx);

        fdbEntry.dstOutlif.interfaceInfo.vidx = 100;

        /*
        [ CPSS_INTERFACE_DEVICE_E ]
            -> out of range
            1.20. Call with interfaceInfo.type [CPSS_INTERFACE_DEVICE_E]
                            interfaceInfo.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_DEVICE_E;
        fdbEntry.dstOutlif.interfaceInfo.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devNum = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.devNum);

        fdbEntry.dstOutlif.interfaceInfo.devNum = dev;

        /*
        [ CPSS_INTERFACE_FABRIC_VIDX_E ]
            -> out of range
            1.22. Call with interfaceInfo.type [CPSS_INTERFACE_FABRIC_VIDX_E]
                            interfaceInfo.fabricVidx [4096]
                           and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_FABRIC_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.fabricVidx = 4096;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.fabricVidx = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.fabricVidx);

        fdbEntry.dstOutlif.interfaceInfo.fabricVidx = 100;

        /*
            1.23. Call with interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                            interfaceInfo.fabricVidx [4096]
                           and other valid params.
            Expected: GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        fdbEntry.dstOutlif.interfaceInfo.fabricVidx = 4096;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.fabricVidx = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.fabricVidx);

        fdbEntry.dstOutlif.interfaceInfo.fabricVidx = 100;

        /*
            1.24. Call function with out of range fdbEntryPtr->daCommand [0x5AAAAAA5]
                                     and other parameters the same as in 1.10.
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.daCommand = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryPtr->daCommand = %d",
                                     dev, fdbEntry.daCommand);

        fdbEntry.daCommand = CPSS_MAC_TABLE_INTERV_E;

        /*
            1.25. Call function with out of range fdbEntryPtr->saCommand [0x5AAAAAA5]
                                     and other parameters the same as in 1.10.
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.saCommand = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryPtr->saCommand = %d",
                                     dev, fdbEntry.saCommand);

        fdbEntry.saCommand = CPSS_MAC_TABLE_INTERV_E;

        /*
            1.26. Call function with out of range fdbEntryPtr->userDefined [0x10]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.userDefined = 0x10;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->userDefined = %d",
                                     dev, fdbEntry.userDefined);

        fdbEntry.userDefined = 0x10;

        /*
            1.27. Call function with out of range fdbEntryPtr->daQosIndex [0x8]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.daQosIndex = 0x8;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->daQosIndex = %d",
                                     dev, fdbEntry.daQosIndex);

        fdbEntry.daQosIndex = 0x8;

        /*
            1.28. Call function with out of range fdbEntryPtr->saQosIndex [0x8]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.saQosIndex = 0x8;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->saQosIndex = %d",
                                     dev, fdbEntry.saQosIndex);

        fdbEntry.saQosIndex = 0x8;

        /*
            1.29. Call function with out of range fdbEntryPtr->daSecurityLevel [0x8]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.daSecurityLevel = 0x8;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->daSecurityLevel = %d",
                                     dev, fdbEntry.daSecurityLevel);

        fdbEntry.daSecurityLevel = 0x8;

        /*
            1.30. Call function with out of range fdbEntryPtr->saSecurityLevel [0x8]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.saSecurityLevel = 0x8;

        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->saSecurityLevel = %d",
                                     dev, fdbEntry.saSecurityLevel);

        fdbEntry.saSecurityLevel = 0x8;

        /*
            1.31. Call cpssExMxPmBrgFdbEntryInvalidate with index [0/ 1/ 2] to invalidate all changes
            Expected: GT_OK.
        */
        /* Call with index [0] */
        index = 0;
        st = cpssExMxPmBrgFdbEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbEntryInvalidate: %d, %d", dev, index);

        /* Call with index [1] */
        index = 1;

        st = cpssExMxPmBrgFdbEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbEntryInvalidate: %d, %d", dev, index);

        /* Call with index [2] */
        index = 2;

        st = cpssExMxPmBrgFdbEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbEntryInvalidate: %d, %d", dev, index);
    }

    index = 0;
    skip  = GT_FALSE;

    cpssOsBzero((GT_VOID*) &fdbEntry, sizeof(fdbEntry));
    fdbEntry.key.entryType      = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    fdbEntry.key.key.macFid.fId = 100;
    fdbEntry.key.key.macFid.macAddr.arEther[0] = 0x00;
    fdbEntry.key.key.macFid.macAddr.arEther[1] = 0x1A;
    fdbEntry.key.key.macFid.macAddr.arEther[2] = 0xFF;
    fdbEntry.key.key.macFid.macAddr.arEther[3] = 0xFF;
    fdbEntry.key.key.macFid.macAddr.arEther[4] = 0xFF;
    fdbEntry.key.key.macFid.macAddr.arEther[5] = 0xFF;

    fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
    fdbEntry.dstOutlif.interfaceInfo.trunkId = 100;
    fdbEntry.dstOutlif.outlifPointer.arpPtr  = 0;

    fdbEntry.isStatic                 = GT_FALSE;
    fdbEntry.daCommand                = CPSS_PACKET_CMD_FORWARD_E;
    fdbEntry.saCommand                = CPSS_PACKET_CMD_FORWARD_E;
    fdbEntry.daRoute                  = GT_FALSE;
    fdbEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
    fdbEntry.sourceId                 = 0;
    fdbEntry.userDefined              = 0;
    fdbEntry.daQosIndex               = 0;
    fdbEntry.saQosIndex               = 0;
    fdbEntry.daSecurityLevel          = 0;
    fdbEntry.saSecurityLevel          = 0;
    fdbEntry.appSpecificCpuCode       = GT_FALSE;
    fdbEntry.spUnknown                = GT_FALSE;

    fdbEntry.pwId   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbEntryWrite(dev, index, skip, &fdbEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbEntryRead
(
    IN  GT_U8                      devNum,
    IN  GT_U32                     index,
    OUT GT_BOOL                    *validPtr,
    OUT GT_BOOL                    *skipPtr,
    OUT GT_BOOL                    *agedPtr,
    OUT GT_U8                      *associatedDevNumPtr,
    OUT CPSS_EXMXPM_FDB_ENTRY_STC  *fdbEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbEntryRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0],
                   non-null validPtr,
                   non-null skipPtr,
                   non-null agedPtr,
                   non-null associatedDevNumPtr
                   and non-null fdbEntryPtr.
    Expected: GT_OK.
    1.2. Call function with out of range index [macTableSize(dev)].
    Expected: NOT GT_OK.
    1.3. Call with index [0],
                   null validPtr [NULL],
                   non-null skipPtr,
                   non-null agedPtr,
                   non-null associatedDevNumPtr
                   and non-null fdbEntryPtr.
    Expected: GT_BAD_PTR.
    1.4. Call with index [0],
                   non-null validPtr,
                   null skipPtr [NULL],
                   non-null agedPtr,
                   non-null associatedDevNumPtr
                   and non-null fdbEntryPtr.
    Expected: GT_BAD_PTR.
    1.5. Call with index [0],
                   non-null validPtr,
                   non-null skipPtr,
                   null agedPtr [NULL],
                   non-null associatedDevNumPtr
                   and non-null fdbEntryPtr.
    Expected: GT_BAD_PTR.
    1.6. Call with index [0],
                   non-null validPtr,
                   non-null skipPtr,
                   non-null agedPtr,
                   null associatedDevNumPtr [NULL]
                   and non-null fdbEntryPtr.
    Expected: GT_BAD_PTR.
    1.7. Call with index [0],
                   non-null validPtr,
                   non-null skipPtr,
                   non-null agedPtr,
                   non-null associatedDevNumPtr
                   and null fdbEntryPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    index     = 0;
    GT_U32                    tableSize = 0;
    GT_BOOL                   valid     = GT_FALSE;
    GT_BOOL                   skip      = GT_FALSE;
    GT_BOOL                   aged      = GT_FALSE;
    GT_U8                     devNum    = 0;
    CPSS_EXMXPM_FDB_ENTRY_STC fdbEntry;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0],
                           non-null validPtr,
                           non-null skipPtr,
                           non-null agedPtr,
                           non-null associatedDevNumPtr
                           and non-null entryPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbEntryRead(dev, index, &valid, &skip,
                                       &aged, &devNum, &fdbEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Getting table size */
        st = prvUtfFdbMacTableSize(dev, &tableSize);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFdbMacTableSize: %d", dev);

        /*
            1.2. Call function with out of range index [macTableSize(dev)].
            Expected: NOT GT_OK.
        */
        index = tableSize;

        st = cpssExMxPmBrgFdbEntryRead(dev, index, &valid, &skip,
                                       &aged, &devNum, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call with index [0],
                           null validPtr [NULL],
                           non-null skipPtr,
                           non-null agedPtr,
                           non-null associatedDevNumPtr
                           and non-null entryPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbEntryRead(dev, index, NULL, &skip,
                                       &aged, &devNum, &fdbEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);

        /*
            1.4. Call with index [0],
                           non-null validPtr,
                           null skipPtr [NULL],
                           non-null agedPtr,
                           non-null associatedDevNumPtr
                           and non-null entryPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbEntryRead(dev, index, &valid, NULL,
                                       &aged, &devNum, &fdbEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, skipPtr = NULL", dev);

        /*
            1.5. Call with index [0],
                           non-null validPtr,
                           non-null skipPtr,
                           null agedPtr [NULL],
                           non-null associatedDevNumPtr
                           and non-null entryPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbEntryRead(dev, index, &valid, &skip,
                                       NULL, &devNum, &fdbEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, agedPtr = NULL", dev);

        /*
            1.6. Call with index [0],
                           non-null validPtr,
                           non-null skipPtr,
                           non-null agedPtr,
                           null associatedDevNumPtr [NULL]
                           and non-null entryPtr.
            Expected: GT_BAD_PTR.
        */
        index = 0;

        st = cpssExMxPmBrgFdbEntryRead(dev, index, &valid, &skip,
                                       &aged, NULL, &fdbEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, associatedDevNumPtr = NULL", dev);

        /*
            1.7. Call with index [0],
                           non-null validPtr,
                           non-null skipPtr,
                           non-null agedPtr,
                           non-null associatedDevNumPtr
                           and null entryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbEntryRead(dev, index, &valid, &skip,
                                       &aged, &devNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryPtr = NULL", dev);
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
        st = cpssExMxPmBrgFdbEntryRead(dev, index, &valid, &skip,
                                        &aged, &devNum, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbEntryRead(dev, index, &valid, &skip,
                                        &aged, &devNum, &fdbEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbEntryInvalidate
(
    IN GT_U8         devNum,
    IN GT_U32        index
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbEntryInvalidate)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with index [0].
    Expected: GT_OK.
    1.2. Call with out of range index [macTableSize(dev)].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32     index     = 0;
    GT_U32     tableSize = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with index [0].
            Expected: GT_OK.
        */
        index = 0;

        st = cpssExMxPmBrgFdbEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Getting table size */
        st = prvUtfFdbMacTableSize(dev, &tableSize);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFdbMacTableSize: %d", dev);

        /*
            1.2. Call with out of range index [macTableSize(dev)].
            Expected: NOT GT_OK.
        */
        index = tableSize;

        st = cpssExMxPmBrgFdbEntryInvalidate(dev, index);
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
        st = cpssExMxPmBrgFdbEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbEntryInvalidate(dev, index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
#ifdef FDB_EXTERN_MEMORY_USED
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbLutEntryWrite
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    IN CPSS_EXMXPM_FDB_LUT_ENTRY_STC    *lutEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbLutEntryWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with index [0 / 1],
                            lutEntryPtr->{subEntry[].{lutSecHashValue[0x0 / 0xFFFF],
                                                      fdbEntryOffset[0],
                                                      isValid [GT_TRUE]}}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbLutEntryRead with the same index and non-null lutEntryPtr.
    Expected: GT_OK and the same lutEntryPtr.
    1.3. Call function with out of range index [maxLutEntry]
                            and other parameters from 1.1.
    Expected: not GT_OK.
    1.4. Call function with index [0],
                            lutEntryPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                        index     = 0;
    GT_BOOL                       isEqual   = GT_FALSE;
    CPSS_EXMXPM_FDB_LUT_ENTRY_STC lutEntry;
    CPSS_EXMXPM_FDB_LUT_ENTRY_STC lutEntryGet;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with index [0 / 1],
                                    lutEntryPtr->{subEntry[].{lutSecHashValue[0x0 / 0xFFFF],
                                                              fdbEntryOffset[0],
                                                              isValid [GT_TRUE]}}.
            Expected: GT_OK.
        */
        lutEntry.subEntry[0].lutSecHashValue = 0;
        lutEntry.subEntry[0].isValid         = GT_TRUE;
        lutEntry.subEntry[1].lutSecHashValue = 0;
        lutEntry.subEntry[1].isValid         = GT_TRUE;
        lutEntry.subEntry[2].lutSecHashValue = 0;
        lutEntry.subEntry[2].isValid         = GT_TRUE;
        lutEntry.subEntry[3].lutSecHashValue = 0;
        lutEntry.subEntry[3].isValid         = GT_TRUE;

        /* Call with index [0] */
        index = 0;

        lutEntry.subEntry[0].lutSecHashValue = 0;
        lutEntry.subEntry[1].lutSecHashValue = 0;
        lutEntry.subEntry[2].lutSecHashValue = 0;
        lutEntry.subEntry[3].lutSecHashValue = 0;

        st = cpssExMxPmBrgFdbLutEntryWrite(dev, index, &lutEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call cpssExMxPmBrgFdbLutEntryRead with the same index and non-null lutEntryPtr.
            Expected: GT_OK and the same lutEntryPtr.
        */
        st = cpssExMxPmBrgFdbLutEntryRead(dev, index, &lutEntryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgFdbLutEntryRead: %d, %d", dev, index);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &lutEntry,
                                     (GT_VOID*) &lutEntryGet,
                                     sizeof(lutEntry))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another lutEntry than was set: %d", dev);

        /* Call with index [1] */
        index = 1;

        lutEntry.subEntry[0].lutSecHashValue = 0xFFFF;
        lutEntry.subEntry[1].lutSecHashValue = 0xFFFF;
        lutEntry.subEntry[2].lutSecHashValue = 0xFFFF;
        lutEntry.subEntry[3].lutSecHashValue = 0xFFFF;

        st = cpssExMxPmBrgFdbLutEntryWrite(dev, index, &lutEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call cpssExMxPmBrgFdbLutEntryRead with the same index and non-null lutEntryPtr.
            Expected: GT_OK and the same lutEntryPtr.
        */
        st = cpssExMxPmBrgFdbLutEntryRead(dev, index, &lutEntryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgFdbLutEntryRead: %d, %d", dev, index);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &lutEntry,
                                     (GT_VOID*) &lutEntryGet,
                                     sizeof(lutEntry))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another lutEntry than was set: %d", dev);

        /*
            1.3. Call function with out of range index [maxLutEntry]
                                    and other parameters from 1.1.
            Expected: not GT_OK.
        */

        /* Getting max table size */
        st = prvUtfLutTableSize(dev, &index);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfLutTableSize: %d", dev);

        st = cpssExMxPmBrgFdbLutEntryWrite(dev, index, &lutEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.4. Call function with index [0],
                                    lutEntryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbLutEntryWrite(dev, index, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lutEntryPtr = NULL", dev);
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
        st = cpssExMxPmBrgFdbLutEntryWrite(dev, index, &lutEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbLutEntryWrite(dev, index, &lutEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbLutEntryRead
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,
    OUT CPSS_EXMXPM_FDB_LUT_ENTRY_STC   *lutEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbLutEntryRead)
{
/*
    1.1. Call function with index [0 / 1]
                            and non-null lutEntryPtr.
    Expected: GT_OK.
    1.2. Call function with out of range index [maxLutEntry]
                            and other parameters from 1.1.
    Expected: not GT_OK.
    1.3. Call function with index [0],
                            lutEntryPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                        index     = 0;
    CPSS_EXMXPM_FDB_LUT_ENTRY_STC lutEntry;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with index [0 / 1]
                                    and non-null lutEntryPtr.
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index = 0;

        st = cpssExMxPmBrgFdbLutEntryRead(dev, index, &lutEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with index [1] */
        index = 1;

        st = cpssExMxPmBrgFdbLutEntryRead(dev, index, &lutEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.2. Call function with out of range index [maxLutEntry]
                                    and other parameters from 1.1.
            Expected: not GT_OK.
        */

        /* Getting max table size */
        st = prvUtfLutTableSize(dev, &index);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfLutTableSize: %d", dev);

        st = cpssExMxPmBrgFdbLutEntryRead(dev, index, &lutEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.4. Call function with index [0],
                                    lutEntryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbLutEntryRead(dev, index, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lutEntryPtr = NULL", dev);
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
        st = cpssExMxPmBrgFdbLutEntryRead(dev, index, &lutEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbLutEntryRead(dev, index, &lutEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
#endif
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbAaAndTaToCpuSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbAaAndTaToCpuSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbAaAndTaToCpuGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbAaAndTaToCpuSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbAaAndTaToCpuGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbAaAndTaToCpuGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbAaAndTaToCpuGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbAaAndTaToCpuSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbAaAndTaToCpuGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbAaAndTaToCpuGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbAaAndTaToCpuGet: %d", dev);

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
        st = cpssExMxPmBrgFdbAaAndTaToCpuSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbAaAndTaToCpuSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbAaAndTaToCpuGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbAaAndTaToCpuGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbAaAndTaToCpuGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbAaAndTaToCpuGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbAaAndTaToCpuGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbAaAndTaToCpuGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbNaToCpuSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbNaToCpuSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbNaToCpuGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbNaToCpuSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbNaToCpuGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbNaToCpuGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbNaToCpuGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbNaToCpuSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbNaToCpuGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbNaToCpuGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbNaToCpuGet: %d", dev);

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
        st = cpssExMxPmBrgFdbNaToCpuSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbNaToCpuSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbNaToCpuGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbNaToCpuGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbNaToCpuGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbNaToCpuGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbNaToCpuGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbNaToCpuGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbQaUnicastToCpuSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbQaUnicastToCpuSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbQaUnicastToCpuGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbQaUnicastToCpuSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbQaUnicastToCpuGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbQaUnicastToCpuGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbQaUnicastToCpuGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbQaUnicastToCpuSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbQaUnicastToCpuGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbQaUnicastToCpuGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbQaUnicastToCpuGet: %d", dev);

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
        st = cpssExMxPmBrgFdbQaUnicastToCpuSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbQaUnicastToCpuSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbQaUnicastToCpuGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbQaUnicastToCpuGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbQaUnicastToCpuGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbQaUnicastToCpuGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbQaUnicastToCpuGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbQaUnicastToCpuGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbQaMulticastToCpuSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbQaMulticastToCpuSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbQaMulticastToCpuGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbQaMulticastToCpuSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbQaMulticastToCpuGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbQaMulticastToCpuGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbQaMulticastToCpuGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbQaMulticastToCpuSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbQaMulticastToCpuGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbQaMulticastToCpuGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbQaMulticastToCpuGet: %d", dev);

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
        st = cpssExMxPmBrgFdbQaMulticastToCpuSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbQaMulticastToCpuSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbQaMulticastToCpuGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbQaMulticastToCpuGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbQaMulticastToCpuGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbQaMulticastToCpuGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbQaMulticastToCpuGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbQaMulticastToCpuGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbSpAaMsgToCpuSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbSpAaMsgToCpuSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbSpAaMsgToCpuGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbSpAaMsgToCpuSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbSpAaMsgToCpuGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbSpAaMsgToCpuGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbSpAaMsgToCpuGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbSpAaMsgToCpuSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbSpAaMsgToCpuGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbSpAaMsgToCpuGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbSpAaMsgToCpuGet: %d", dev);

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
        st = cpssExMxPmBrgFdbSpAaMsgToCpuSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbSpAaMsgToCpuSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbSpAaMsgToCpuGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbSpAaMsgToCpuGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbSpAaMsgToCpuGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbSpAaMsgToCpuGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbSpAaMsgToCpuGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbSpAaMsgToCpuGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbSpEntriesDelEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbSpEntriesDelEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbSpEntriesDelEnableGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbSpEntriesDelEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbSpEntriesDelEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbSpEntriesDelEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbSpEntriesDelEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbSpEntriesDelEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbSpEntriesDelEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbSpEntriesDelEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbSpEntriesDelEnableGet: %d", dev);

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
        st = cpssExMxPmBrgFdbSpEntriesDelEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbSpEntriesDelEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbSpEntriesDelEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbSpEntriesDelEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbSpEntriesDelEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbSpEntriesDelEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbSpEntriesDelEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbSpEntriesDelEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbNaMsgOnFailedLearningSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbNaMsgOnFailedLearningSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbNaMsgOnFailedLearningGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbNaMsgOnFailedLearningSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbNaMsgOnFailedLearningGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbNaMsgOnFailedLearningGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbNaMsgOnFailedLearningGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbNaMsgOnFailedLearningSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbNaMsgOnFailedLearningGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbNaMsgOnFailedLearningGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbNaMsgOnFailedLearningGet: %d", dev);

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
        st = cpssExMxPmBrgFdbNaMsgOnFailedLearningSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbNaMsgOnFailedLearningSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbNaMsgOnFailedLearningGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbNaMsgOnFailedLearningGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbNaMsgOnFailedLearningGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbNaMsgOnFailedLearningGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbNaMsgOnFailedLearningGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbNaMsgOnFailedLearningGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbFromCpuAuMsgStatusGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_UPD_MSG_TYPE_ENT   msgType,
    OUT GT_BOOL                 *completedPtr,
    OUT GT_BOOL                 *succeededPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbFromCpuAuMsgStatusGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with msgType[CPSS_NA_E / CPSS_QA_E], non-null completedPtr and non-null succeededPtr.
    Expected: GT_OK.
    1.2. Call with null msgType [0x5AAAAAA5]and not NULL pointers.
    Expected: GT_BAD_PARAM.
    1.3. Call with null completedPtr [NULL] and non-null succeededPtr.
    Expected: GT_BAD_PTR.
    1.4. Call with non-null completedPtr and null succeededPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_UPD_MSG_TYPE_ENT   msgType   = CPSS_NA_E;
    GT_BOOL                 completed = GT_FALSE;
    GT_BOOL                 succeeded = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with msgType[CPSS_NA_E / CPSS_QA_E],
                           non-null completedPtr and non-null succeededPtr.
            Expected: GT_OK.
        */
        /* iterate with msgType = CPSS_NA_E */
        msgType = CPSS_NA_E;

        st = cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(dev, msgType, &completed, &succeeded);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* iterate with msgType = CPSS_QA_E */
        msgType = CPSS_QA_E;

        st = cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(dev, msgType, &completed, &succeeded);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null msgType [0x5AAAAAA5]and not NULL pointers.
            Expected: GT_BAD_PARAM.
        */
        msgType = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(dev, msgType, &completed, &succeeded);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, msgType = %d", dev, msgType);

        msgType = CPSS_NA_E;

        /*
            1.3. Call with null completedPtr [NULL] and non-null succeededPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(dev, msgType, NULL, &succeeded);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, completedPtr = NULL", dev);

        /*
            1.4. Call with non-null completedPtr and null succeededPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(dev, msgType, &completed, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, completedPtr = NULL", dev);
    }

    msgType = CPSS_NA_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(dev, msgType, &completed, &succeeded);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(dev, msgType, &completed, &succeeded);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbAuMsgBlockGet
(
    IN      GT_U8                                   devNum,
    INOUT   GT_U32                                  *numOfAuPtr,
    OUT     CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC    *auMessagesPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbAuMsgBlockGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null auMessagesPtr, numOfAuPtr [2].
    Expected: GT_OK.
    1.2. Call with null numOfAuPtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with null auMessagesPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                               numOfAu = 0;
    CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC auMessages[2];

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null auMessagesPtr, numOfAuPtr [2].
            Expected: GT_OK.
        */
        numOfAu = 2;

        st = cpssExMxPmBrgFdbAuMsgBlockGet(dev, &numOfAu, auMessages);
        if (GT_OK != st)
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NO_MORE, st, dev, numOfAu);
        }


        /*
            1.2. Call with null numOfAuPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbAuMsgBlockGet(dev, NULL, auMessages);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfAuPtr = NULL", dev);

        /*
            1.3. Call with null auMessagesPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbAuMsgBlockGet(dev, &numOfAu, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, auMessagesPtr = NULL", dev);
    }

    numOfAu = 2;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbAuMsgBlockGet(dev, &numOfAu, auMessages);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbAuMsgBlockGet(dev, &numOfAu, auMessages);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbSecondaryAuMsgBlockGet
(
    IN      GT_U8                                   devNum,
    INOUT   GT_U32                                  *numOfAuPtr,
    OUT     CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC    *auMessagesPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbSecondaryAuMsgBlockGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null auMessagesPtr, numOfAuPtr [0 / 2 / MAX_NUM_OF_AU_MSG].
    Expected: GT_OK.
    1.2. Call with null numOfAuPtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with null auMessagesPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                               numOfAu = 0;
    CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC auMessages[MAX_NUM_OF_AU_MSG];

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null auMessagesPtr, numOfAuPtr [ 0 ].
            Expected: GT_OK.
        */
        numOfAu = 0;
        st = cpssExMxPmBrgFdbSecondaryAuMsgBlockGet(dev, &numOfAu, auMessages);
        if (GT_OK != st)
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NO_MORE, st, dev, numOfAu);
        }

        /*
            1.1. Call with non-null auMessagesPtr, numOfAuPtr [2].
            Expected: GT_OK.
        */
        numOfAu = 2;
        st = cpssExMxPmBrgFdbSecondaryAuMsgBlockGet(dev, &numOfAu, auMessages);
        if (GT_OK != st)
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NO_MORE, st, dev, numOfAu);
        }
        /*
            1.1. Call with non-null auMessagesPtr, numOfAuPtr [MAX_NUM_OF_AU_MSG].
            Expected: GT_OK.
        */
        numOfAu = MAX_NUM_OF_AU_MSG;
        st = cpssExMxPmBrgFdbSecondaryAuMsgBlockGet(dev, &numOfAu, auMessages);
        if (GT_OK != st)
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NO_MORE, st, dev, numOfAu);
        }


        /*
            1.2. Call with null numOfAuPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbSecondaryAuMsgBlockGet(dev, NULL, auMessages);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfAuPtr = NULL", dev);

        /*
            1.3. Call with null auMessagesPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        numOfAu = 2;

        st = cpssExMxPmBrgFdbSecondaryAuMsgBlockGet(dev, &numOfAu, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, auMessagesPtr = NULL", dev);
    }

    numOfAu = 2;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbSecondaryAuMsgBlockGet(dev, &numOfAu, auMessages);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    numOfAu = 2;

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbSecondaryAuMsgBlockGet(dev, &numOfAu, auMessages);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbFuMsgBlockGet
(
    IN     GT_U8                                 devNum,
    INOUT  GT_U32                                *numOfFuPtr,
    OUT    CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC  *fuMessagesPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbFuMsgBlockGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null fuMessagesPtr, numOfFuPtr [2].
    Expected: GT_OK.
    1.2. Call with null numOfFuPtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with null fuMessagesPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                               numOfFu;
    CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC fuMessages[2];

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null pointers, numOfFuPtr [2].
            Expected: GT_OK.
        */
        numOfFu = 2;

        st = cpssExMxPmBrgFdbFuMsgBlockGet(dev, &numOfFu, fuMessages);
        if (GT_OK != st)
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NO_MORE, st, dev, numOfFu);
        }
        /*
            1.2. Call with null numOfFuPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbFuMsgBlockGet(dev, NULL, fuMessages);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfFuPtr = NULL", dev);

        /*
            1.3. Call with null fuMessagesPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbFuMsgBlockGet(dev, &numOfFu, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fuMessagesPtr = NULL", dev);
    }

    numOfFu = 2;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbFuMsgBlockGet(dev, &numOfFu, fuMessages);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbFuMsgBlockGet(dev, &numOfFu, fuMessages);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbDropAuEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbDropAuEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbDropAuEnableGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbDropAuEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbDropAuEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbDropAuEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbDropAuEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbDropAuEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbDropAuEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbDropAuEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbDropAuEnableGet: %d", dev);

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
        st = cpssExMxPmBrgFdbDropAuEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbDropAuEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbDropAuEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbDropAuEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbDropAuEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbDropAuEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbDropAuEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbDropAuEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbAuMsgRateLimitSet
(
    IN GT_U8                       devNum,
    IN GT_U32                      msgRate,
    IN GT_BOOL                     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbAuMsgRateLimitSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with msgRate [9 * coreClock],
                   enable [GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbAuMsgRateLimitGet
    Expected: GT_OK and the same msgRatePtr and enablePtr.
    1.3. Call with out of range msgRate [(204000/200 + 1) * (200 * coreClock / 200)],
                   enable [GT_TRUE].
    Expected: NOT GT_OK.
    1.4. Call with out of range msgRate [(204000/200 + 1) * (200 * coreClock / 200)],
                   enable [GT_FALSE].
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      rate      = 0;
    GT_BOOL     enable    = GT_FALSE;
    GT_U32      rateGet   = 0;
    GT_BOOL     enableGet = GT_FALSE;
    GT_U32      coreClock = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting core clock */
        st = prvUtfCoreClockGet(dev, &coreClock);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCoreClockGet: %d", dev);

        /*
            1.1. Call with msgRate [9 * coreClock],
                           enable [GT_TRUE].
            Expected: GT_OK.
        */
        rate   = 9 * coreClock;
        enable = GT_TRUE;

        st = cpssExMxPmBrgFdbAuMsgRateLimitSet(dev, rate, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, rate, enable);

        /*
            1.2. Call cpssExMxPmBrgFdbAuMsgRateLimitGet
            Expected: GT_OK and the same msgRatePtr and enablePtr.
        */
        st = cpssExMxPmBrgFdbAuMsgRateLimitGet(dev, &rateGet, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbAuMsgRateLimitGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /*
            1.3. Call with out of range msgRate [(204000/200 + 1) * (200 * coreClock / 200)],
                           enable [GT_TRUE].
            Expected: NOT GT_OK.
        */
        rate   = (204000 / 200 + 1) * (200 * coreClock / 200);
        enable = GT_TRUE;

        st = cpssExMxPmBrgFdbAuMsgRateLimitSet(dev, rate, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, rate, enable);

        rate   = 2 * (200 * coreClock / 200);

        /*
            1.4. Call with out of range msgRate [(204000/200 + 1) * (200 * coreClock / 200)],
                           enable [GT_FALSE].
            Expected: GT_OK.
        */
        rate   = (204000 / 200 + 1) * (200 * coreClock / 200);
        enable = GT_FALSE;

        st = cpssExMxPmBrgFdbAuMsgRateLimitSet(dev, rate, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, rate, enable);
    }

    rate   = 200;
    enable = GT_TRUE;


    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbAuMsgRateLimitSet(dev, rate, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbAuMsgRateLimitSet(dev, rate, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbAuMsgRateLimitGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *msgRatePtr,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbAuMsgRateLimitGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null msgRatePtr and non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null msgRatePtr [NULL] and non-null enablePtr.
    Expected: GT_BAD_PTR.
    1.3. Call with non-null msgRatePtr and null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      rate   = 0;
    GT_BOOL     enable = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null msgRatePtr
                           and non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbAuMsgRateLimitGet(dev, &rate, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null msgRatePtr [NULL]
                           and non-null enablePtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbAuMsgRateLimitGet(dev, NULL, &enable);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, msgRatePtr = NULL", dev);

        /*
            1.3. Call with non-null msgRatePtr
                           and null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbAuMsgRateLimitGet(dev, &rate, NULL);
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
        st = cpssExMxPmBrgFdbAuMsgRateLimitGet(dev, &rate, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbAuMsgRateLimitGet(dev, &rate, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbSecureAutoLearnSet
(
    IN GT_U8                                devNum,
    IN CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbSecureAutoLearnSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with mode [CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E /
                                  CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E /
                                  CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E /
                                  CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbSecureAutoLearnGet
    Expected: GT_OK and the same modePtr.
    1.3. Call function with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT mode    = CPSS_IVL_E;
    CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT modeGet = CPSS_IVL_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E /
                                  CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E /
                                  CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E /
                                  CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E].
            Expected: GT_OK.
        */

        /* Call with mode [CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E] */
        mode = CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E;

        st = cpssExMxPmBrgFdbSecureAutoLearnSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbSecureAutoLearnGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbSecureAutoLearnGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbSecureAutoLearnGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E] */
        mode = CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E;

        st = cpssExMxPmBrgFdbSecureAutoLearnSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbSecureAutoLearnGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbSecureAutoLearnGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbSecureAutoLearnGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E] */
        mode = CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E;

        st = cpssExMxPmBrgFdbSecureAutoLearnSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbSecureAutoLearnGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbSecureAutoLearnGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbSecureAutoLearnGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /* Call with mode [CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E] */
        mode = CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E;

        st = cpssExMxPmBrgFdbSecureAutoLearnSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbSecureAutoLearnGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbSecureAutoLearnGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbSecureAutoLearnGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /*
            1.3. Call function with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbSecureAutoLearnSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbSecureAutoLearnSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbSecureAutoLearnSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbSecureAutoLearnGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbSecureAutoLearnGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null modePtr.
    Expected: GT_OK.
    1.2. Call with null modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT mode = CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E;

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
        st = cpssExMxPmBrgFdbSecureAutoLearnGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbSecureAutoLearnGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbSecureAutoLearnGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbSecureAutoLearnGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbSecureAgingSet
(
    IN GT_U8          devNum,
    IN GT_BOOL        enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbSecureAgingSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbSecureAgingGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbSecureAgingSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbSecureAgingGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbSecureAgingGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbSecureAgingGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbSecureAgingSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbSecureAgingGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbSecureAgingGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbSecureAgingGet: %d", dev);

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
        st = cpssExMxPmBrgFdbSecureAgingSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbSecureAgingSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbSecureAgingGet
(
    IN  GT_U8          devNum,
    OUT GT_BOOL        *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbSecureAgingGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbSecureAgingGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbSecureAgingGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbSecureAgingGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbSecureAgingGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbDeviceTableSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  devTable[CPSS_EXMXPM_FDB_DEV_TABLE_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbDeviceTableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with devTable [0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbDeviceTableGet
    Expected: GT_OK and the same devTable.
    1.3. Call with devTable [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL    isEqual = GT_FALSE;
    GT_U32     devTable[CPSS_EXMXPM_FDB_DEV_TABLE_SIZE_CNS];
    GT_U32     devTableGet[CPSS_EXMXPM_FDB_DEV_TABLE_SIZE_CNS];

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with devTable [0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF].
            Expected: GT_OK.
        */
        devTable[0] = 0xFFFF;
        devTable[1] = 0xFFFF;
        devTable[2] = 0xFFFF;
        devTable[3] = 0xFFFF;

        st = cpssExMxPmBrgFdbDeviceTableSet(dev, devTable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgFdbDeviceTableGet
            Expected: GT_OK and the same devTable.
        */
        st = cpssExMxPmBrgFdbDeviceTableGet(dev, devTableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbDeviceTableGet: %d", dev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) devTable,
                                     (GT_VOID*) devTableGet,
                                     sizeof(devTable))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another devTable than was set: %d", dev);

        /*
            1.3. Call with devTable [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbDeviceTableSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, devTable = NULL", dev);
    }

    devTable[0] = 0xFFFF;
    devTable[1] = 0xFFFF;
    devTable[2] = 0xFFFF;
    devTable[3] = 0xFFFF;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbDeviceTableSet(dev, devTable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbDeviceTableSet(dev, devTable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbDeviceTableGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  devTable[CPSS_EXMXPM_FDB_DEV_TABLE_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbDeviceTableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL devTable.
    Expected: GT_OK.
    1.2. Call with devTable [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32  devTable[CPSS_EXMXPM_FDB_DEV_TABLE_SIZE_CNS];

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null devTable.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbDeviceTableGet(dev, devTable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null devTable [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbDeviceTableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, devTable = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbDeviceTableGet(dev, devTable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbDeviceTableGet(dev, devTable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbActionsEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbActionsEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbActionsEnableGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbActionsEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbActionsEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbActionsEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbActionsEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbActionsEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbActionsEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbActionsEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbActionsEnableGet: %d", dev);

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
        st = cpssExMxPmBrgFdbActionsEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbActionsEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbActionsEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbActionsEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbActionsEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionsEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbActionsEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbActionsEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbActionModeSet
(
    IN GT_U8                            devNum,
    IN CPSS_EXMXPM_FDB_ACTION_MODE_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbActionModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with mode [CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E /
                                  CPSS_EXMXPM_FDB_ACTION_AGE_WITHOUT_REMOVAL_E /
                                  CPSS_EXMXPM_FDB_ACTION_DELETING_E /
                                  CPSS_EXMXPM_FDB_ACTION_TRANSPLANTING_E /
                                  CPSS_EXMXPM_FDB_ACTION_FDB_UPLOAD_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbActionModeGet
    Expected: GT_OK and the same modePtr.
    1.3. Call function with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_ACTION_MODE_ENT mode    = CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E;
    CPSS_EXMXPM_FDB_ACTION_MODE_ENT modeGet = CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E /
                                          CPSS_EXMXPM_FDB_ACTION_AGE_WITHOUT_REMOVAL_E /
                                          CPSS_EXMXPM_FDB_ACTION_DELETING_E /
                                          CPSS_EXMXPM_FDB_ACTION_TRANSPLANTING_E /
                                          CPSS_EXMXPM_FDB_ACTION_FDB_UPLOAD_E].
            Expected: GT_OK.
        */

        /* Call with mode [CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E] */
        mode = CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E;

        st = cpssExMxPmBrgFdbActionModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbActionModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbActionModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbActionModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_FDB_ACTION_AGE_WITHOUT_REMOVAL_E] */
        mode = CPSS_EXMXPM_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;

        st = cpssExMxPmBrgFdbActionModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbActionModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbActionModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbActionModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_FDB_ACTION_DELETING_E] */
        mode = CPSS_EXMXPM_FDB_ACTION_DELETING_E;

        st = cpssExMxPmBrgFdbActionModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbActionModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbActionModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbActionModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_FDB_ACTION_TRANSPLANTING_E] */
        mode = CPSS_EXMXPM_FDB_ACTION_TRANSPLANTING_E;

        st = cpssExMxPmBrgFdbActionModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbActionModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbActionModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbActionModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_FDB_ACTION_FDB_UPLOAD_E] */
        mode = CPSS_EXMXPM_FDB_ACTION_FDB_UPLOAD_E;

        st = cpssExMxPmBrgFdbActionModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbActionModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbActionModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbActionModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /*
            1.3. Call function with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbActionModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbActionModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbActionModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbActionModeGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_EXMXPM_FDB_ACTION_MODE_ENT *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbActionModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null modePtr.
    Expected: GT_OK.
    1.2. Call with null modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_ACTION_MODE_ENT mode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;

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
        st = cpssExMxPmBrgFdbActionModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionModeGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbActionModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbActionModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbTriggerModeSet
(
    IN GT_U8                            devNum,
    IN CPSS_EXMXPM_FDB_TRIGGER_MODE_ENT mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbTriggerModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with mode [CPSS_EXMXPM_FDB_TRIGGER_TRIG_E /
                                  CPSS_EXMXPM_FDB_TRIGGER_AUTO_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbTriggerModeGet
    Expected: GT_OK and the same modePtr.
    1.3. Call function with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_TRIGGER_MODE_ENT mode    = CPSS_EXMXPM_FDB_TRIGGER_AUTO_E;
    CPSS_EXMXPM_FDB_TRIGGER_MODE_ENT modeGet = CPSS_EXMXPM_FDB_TRIGGER_AUTO_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_EXMXPM_FDB_TRIGGER_TRIG_E/
                                          CPSS_EXMXPM_FDB_TRIGGER_AUTO_E].
            Expected: GT_OK.
        */

        /* Call with mode [CPSS_EXMXPM_FDB_TRIGGER_TRIG_E] */
        mode = CPSS_EXMXPM_FDB_TRIGGER_TRIG_E;

        st = cpssExMxPmBrgFdbTriggerModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbTriggerModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbTriggerModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbTriggerModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_FDB_TRIGGER_AUTO_E] */
        mode = CPSS_EXMXPM_FDB_TRIGGER_AUTO_E;

        st = cpssExMxPmBrgFdbTriggerModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmBrgFdbTriggerModeGet
            Expected: GT_OK and the same modePtr.
        */
        st = cpssExMxPmBrgFdbTriggerModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbTriggerModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another modePtr than was set: %d", dev);

        /*
            1.3. Call function with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbTriggerModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_FDB_TRIGGER_AUTO_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbTriggerModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbTriggerModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbTriggerModeGet
(
    IN  GT_U8                       devNum,
    OUT CPSS_EXMXPM_FDB_TRIGGER_MODE_ENT   *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbTriggerModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null modePtr.
    Expected: GT_OK.
    1.2. Call with null modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_TRIGGER_MODE_ENT mode = CPSS_EXMXPM_FDB_TRIGGER_AUTO_E;

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
        st = cpssExMxPmBrgFdbTriggerModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbTriggerModeGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbTriggerModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbTriggerModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbAgingTriggerSet
(
    IN GT_U8  devNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbAgingTriggerSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function to set FDB action trigger.
    Expected: GT_OK.
    1.2. Call  cpssExMxPmBrgFdbAgingTriggerGet with non-null actFinishedPtr
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL actFinished = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function to set FDB action trigger.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbAgingTriggerSet(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call  cpssExMxPmBrgFdbAgingTriggerGet with non-null actFinishedPtr
            Expected: GT_OK and actFinishedPtr [GT_TRUE].
        */
        st = cpssExMxPmBrgFdbAgingTriggerGet(dev, &actFinished);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbAgingTriggerGet: %d", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbAgingTriggerSet(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbAgingTriggerSet(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbAgingTriggerGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *actFinishedPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbAgingTriggerGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null actFinishedPtr.
    Expected: GT_OK.
    1.2. Call with null actFinishedPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     actFinished = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null actFinishedPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbAgingTriggerGet(dev, &actFinished);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null actFinishedPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbAgingTriggerGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actFinishedPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbAgingTriggerGet(dev, &actFinished);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbAgingTriggerGet(dev, &actFinished);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

#ifndef _WIN32
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbTriggeredActionStart
(
    IN GT_U8                            devNum,
    IN CPSS_EXMXPM_FDB_ACTION_MODE_ENT  mode
)
*/
/* cpssExMxPmBrgFdbTriggeredActionStart - function HW/GM dependent */
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbTriggeredActionStart)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with mode [CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E /
                                  CPSS_EXMXPM_FDB_ACTION_AGE_WITHOUT_REMOVAL_E /
                                  CPSS_EXMXPM_FDB_ACTION_DELETING_E /
                                  CPSS_EXMXPM_FDB_ACTION_TRANSPLANTING_E /
                                  CPSS_EXMXPM_FDB_ACTION_FDB_UPLOAD_E].
    Expected: GT_OK.
    1.2. Call function with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_ACTION_MODE_ENT mode = CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E /
                                          CPSS_EXMXPM_FDB_ACTION_AGE_WITHOUT_REMOVAL_E /
                                          CPSS_EXMXPM_FDB_ACTION_DELETING_E /
                                          CPSS_EXMXPM_FDB_ACTION_TRANSPLANTING_E /
                                          CPSS_EXMXPM_FDB_ACTION_FDB_UPLOAD_E].
            Expected: GT_OK.
        */

        /* Call with mode [CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E] */
        mode = CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E;

        st = cpssExMxPmBrgFdbTriggeredActionStart(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /* Call with mode [CPSS_EXMXPM_FDB_ACTION_AGE_WITHOUT_REMOVAL_E] */
        mode = CPSS_EXMXPM_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;

        st = cpssExMxPmBrgFdbTriggeredActionStart(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /* Call with mode [CPSS_EXMXPM_FDB_ACTION_DELETING_E] */
        mode = CPSS_EXMXPM_FDB_ACTION_DELETING_E;

        st = cpssExMxPmBrgFdbTriggeredActionStart(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /* Call with mode [CPSS_EXMXPM_FDB_ACTION_TRANSPLANTING_E] */
        mode = CPSS_EXMXPM_FDB_ACTION_TRANSPLANTING_E;

        st = cpssExMxPmBrgFdbTriggeredActionStart(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /* Call with mode [CPSS_EXMXPM_FDB_ACTION_FDB_UPLOAD_E] */
        mode = CPSS_EXMXPM_FDB_ACTION_FDB_UPLOAD_E;

        st = cpssExMxPmBrgFdbTriggeredActionStart(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call function with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbTriggeredActionStart(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbTriggeredActionStart(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbTriggeredActionStart(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
#endif
/* cpssExMxPmBrgFdbTriggeredActionStart - function HW/GM dependent */

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbAgingTimeoutSet
(
    IN GT_U8  devNum,
    IN GT_U32 timeout
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbAgingTimeoutSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with timeout [0 / ((300 * 200) / coreClock)].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbAgingTimeoutGet
    Expected: GT_OK and same timeoutPtr.
    1.3. Call function with timeout [4096].
    Expected: NOT GT_OK for device with clock higher or equal 200 MHZ and for other clock GT_OK.
    1.4. Call function with timeout [5687].
    Expected: NOT GT_OK for device with clock higher or eual 144 MHZ and for other clock GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32  timeout    = 0;
    GT_U32  timeoutGet = 0;
    GT_U32  coreClock  = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting core clock */
        st = prvUtfCoreClockGet(dev, &coreClock);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCoreClockGet: %d", dev);

        /*
            1.1. Call function with timeout [0 / 300].
            Expected: GT_OK.
        */

        /* Call with timeout [0] */
        timeout = 0;

        st = cpssExMxPmBrgFdbAgingTimeoutSet(dev, timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);

        /*
            1.2. Call cpssExMxPmBrgFdbAgingTimeoutGet
            Expected: GT_OK and same timeoutPtr.
        */
        st = cpssExMxPmBrgFdbAgingTimeoutGet(dev, &timeoutGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbAgingTimeoutGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(timeout, timeoutGet,
                       "get another timeoutPtr than was set: %d", dev);

        /* Call with timeout [(300 * 200) / coreClock] */
        timeout = (300 * 200) / coreClock;

        st = cpssExMxPmBrgFdbAgingTimeoutSet(dev, timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);

        /*
            1.2. Call cpssExMxPmBrgFdbAgingTimeoutGet
            Expected: GT_OK and same timeoutPtr.
        */
        st = cpssExMxPmBrgFdbAgingTimeoutGet(dev, &timeoutGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbAgingTimeoutGet: %d", dev);

        /*
            1.3. Call function with timeout [4096].
            Expected: NOT GT_OK for device with clock higher or equal 200 MHZ and for other clock GT_OK.
        */
        timeout = 4096;

        st = cpssExMxPmBrgFdbAgingTimeoutSet(dev, timeout);
        if (coreClock >= 200)
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);
        }

        timeout = 300;

        /*
            1.4. Call function with timeout [5687].
            Expected: NOT GT_OK for device with clock higher or eual 144 MHZ and for other clock GT_OK.
        */
        timeout = 5687;

        st = cpssExMxPmBrgFdbAgingTimeoutSet(dev, timeout);
        if (coreClock > 144)
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);
        }
    }

    timeout = 300;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbAgingTimeoutSet(dev, timeout);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbAgingTimeoutSet(dev, timeout);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbAgingTimeoutGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *timeoutPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbAgingTimeoutGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null timeoutPtr.
    Expected: GT_OK.
    1.2. Call with null timeoutPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32  timeout = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null timeoutPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbAgingTimeoutGet(dev, &timeout);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null timeoutPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbAgingTimeoutGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, timeoutPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbAgingTimeoutGet(dev, &timeout);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbAgingTimeoutGet(dev, &timeout);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbActionActiveFidSet
(
    IN GT_U8    devNum,
    IN GT_U32   fId,
    IN GT_U32   fIdMask
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbActionActiveFidSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with fId [1 / 0]
                            and fIdMask [0x7F / 0].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbActionActiveFidGet
    Expected: GT_OK and the same fIdPtr and fIdMaskPtr.
    1.3. Call function with out of range fId [BIT_16] (out of range)
                            and fIdMask [1].
    Expected: NOT GT_OK.
    1.4. Call function with fId [2]
                            and out of range fIdMask [BIT_16] (out of range).
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      fId        = 0;
    GT_U32      fIdMask    = 0;
    GT_U32      fIdGet     = 0;
    GT_U32      fIdMaskGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with actDev [1 / 0]
                            and actDevMask [0x7F / 0].
            Expected: GT_OK.
        */

        /* Call with actDev [1] and actDevMask [0x7F] */
        fId     = 1;
        fIdMask = 0x7F;

        st = cpssExMxPmBrgFdbActionActiveFidSet(dev, fId, fIdMask);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fId, fIdMask);

        /*
            1.2. Call cpssExMxPmBrgFdbActionActiveFidGet
            Expected: GT_OK and the same actDevPtr and actDevMaskPtr.
        */
        st = cpssExMxPmBrgFdbActionActiveFidGet(dev, &fIdGet, &fIdMaskGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbActionActiveFidGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fId, fIdGet,
                       "get another fIdPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fIdMask, fIdMaskGet,
                       "get another fIdMaskPtr than was set: %d", dev);

        /* Call with fId [0] and fIdMask [0] */
        fId     = 0;
        fIdMask = 0;

        st = cpssExMxPmBrgFdbActionActiveFidSet(dev, fId, fIdMask);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fId, fIdMask);

        /*
            1.2. Call cpssExMxPmBrgFdbActionActiveFidGet
            Expected: GT_OK and the same fIdPtr and fIdMaskPtr.
        */
        st = cpssExMxPmBrgFdbActionActiveFidGet(dev, &fIdGet, &fIdMaskGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbActionActiveFidGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(fId, fIdGet,
                       "get another fIdPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fIdMask, fIdMaskGet,
                       "get another fIdMaskPtr than was set: %d", dev);

        /*
            1.3. Call function with out of range fId [BIT_16] (out of range)
                                    and fIdMask [1].
            Expected: NOT GT_OK.
        */
        fId     = BIT_16;
        fIdMask = 1;

        st = cpssExMxPmBrgFdbActionActiveFidSet(dev, fIdGet, fIdMaskGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fId);

        fId = 2;

        /*
            1.4. Call function with fId [2]
                                    and out of range fIdMask [BIT_16] (out of range).
            Expected: NOT GT_OK.
        */
        fIdMask = BIT_16;

        st = cpssExMxPmBrgFdbActionActiveFidSet(dev, fIdGet, fIdMaskGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, fIdMask = %d", dev, fIdMask);
    }

    fId     = 0;
    fIdMask = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbActionActiveFidSet(dev, fId, fIdMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbActionActiveFidSet(dev, fId, fIdMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbActionActiveFidGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *fIdPtr,
    OUT GT_U32   *fIdMaskPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbActionActiveFidGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null fIdPtr and non-null fIdMaskPtr.
    Expected: GT_OK.
    1.2. Call with null fIdPtr [NULL] and non-null fIdMaskPtr.
    Expected: GT_BAD_PTR.
    1.3. Call with non-null fIdPtr and null fIdMaskPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      fId     = 0;
    GT_U32      fIdMask = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null fIdPtr
                           and non-null fIdMaskPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbActionActiveFidGet(dev, &fId, &fIdMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null fIdPtr [NULL]
                           and non-null fIdMaskPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionActiveFidGet(dev, NULL, &fIdMask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fIdPtr = NULL", dev);

        /*
            1.3. Call with non-null fIdPtr
                           and null fIdMaskPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionActiveFidGet(dev, &fId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fIdMaskPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbActionActiveFidGet(dev, &fId, &fIdMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbActionActiveFidGet(dev, &fId, &fIdMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbActionActiveOutlifSet
(
    IN GT_U8                            devNum,
    IN CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC  actOutlifPtr,
    IN CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC  actOutlifMaskPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbActionActiveOutlifSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with actOutlif {isMultiple[0 / 1],
                                          isTrunk[0],
                                          vidx[0 / 100],
                                          portTrunk[0],
                                          devNum[dev / 0]}
                            and actOutlifMask { isMultiple[0 / 1],
                                                isTrunk[0],
                                                vidx[0 / 4095],
                                                portTrunk[63 / 0],
                                                devNum[127 / 0]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbActionActiveOutlifGet
    Expected: GT_OK and the same actOutlifPtr and actOutlifMaskPtr.
    1.3 Call with actOutlifPtr->isMultiple[2] (out of range)
    Expected: NOT GT_OK
    1.4 Call with actOutlifPtr->isTrunk[2] (out of range)
    Expected: NOT GT_OK
    1.5. Call with actOutlifPtr->isMultiple[0],
                   and actOutlifPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] out of range
    Expected: NOT GT_OK.
    1.6. Call with actOutlifPtr->isMultiple[1],
                   and actOutlifPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] not relevant
    Expected: GT_OK.
    1.7. Call with actOutlifPtr->isMultiple[1],
                   and actOutlifPtr->vidx [4096] out of range
    Expected: NOT GT_OK.
    1.8. Call with actOutlifPtr->isMultiple[0],
                   and actOutlifPtr->vidx [4096] not relevant
    Expected: GT_OK.
    1.9. Call with actOutlifPtr->isMultiple[0],
                   actOutlifPtr->isTrunk[0],
                   actOutlifPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] out of range
    Expected: NOT GT_OK.
    1.10. Call with actOutlifPtr->isMultiple[1],
                    actOutlifPtr->isTrunk[0],
                    actOutlifPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant
    Expected: GT_OK.
    1.11. Call with actOutlifPtr->isMultiple[0],
                    actOutlifPtr->isTrunk[1],
                    actOutlifPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant
    Expected: GT_OK.
    1.12 Call with actOutlifMaskPtr->isMultiple[2] (out of range)
    Expected: NOT GT_OK
    1.13 Call with actOutlifMaskPtr->isTrunk[2] (out of range)
    Expected: NOT GT_OK
    1.14. Call with actOutlifPtr->isMultiple[0],
                   and actOutlifMaskPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] out of range
    Expected: NOT GT_OK.
    1.15. Call with actOutlifPtr->isMultiple[1],
                   and actOutlifMaskPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] not relevant
    Expected: GT_OK.
    1.16. Call with actOutlifPtr->isMultiple[1],
                   and actOutlifMaskPtr->vidx [4096] out of range
    Expected: NOT GT_OK.
    1.17. Call with actOutlifPtr->isMultiple[0],
                   and actOutlifMaskPtr->vidx [4096] not relevant
    Expected: GT_OK.
    1.18. Call with actOutlifPtr->isMultiple[0],
                   actOutlifPtr->isTrunk[0],
                   actOutlifMaskPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] out of range
    Expected: NOT GT_OK.
    1.19. Call with actOutlifPtr->isMultiple[1],
                    actOutlifPtr->isTrunk[0],
                    actOutlifMaskPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant
    Expected: GT_OK.
    1.20. Call with actOutlifPtr->isMultiple[0],
                    actOutlifPtr->isTrunk[1],
                    actOutlifMaskPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant
    Expected: GT_OK.
    1.21. Call function with actOutlifPtr [NULL]
    Expected: GT_BAD_PTR.
    1.22. Call function with actOutlifMaskPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st      = GT_OK;
    GT_U8       dev;
    GT_BOOL     isEqual = GT_FALSE;

    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC      actOutlif;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC      actOutlifMask;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC      actOutlifGet;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC      actOutlifMaskGet;


    cpssOsBzero((GT_VOID*) &actOutlif, sizeof(actOutlif));
    cpssOsBzero((GT_VOID*) &actOutlifMask, sizeof(actOutlifMask));
    cpssOsBzero((GT_VOID*) &actOutlifGet, sizeof(actOutlifGet));
    cpssOsBzero((GT_VOID*) &actOutlifMaskGet, sizeof(actOutlifMaskGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with actOutlif {isMultiple[0 / 1],
                                                  isTrunk[0],
                                                  vidx[0 / 100],
                                                  portTrunk[0],
                                                  devNum[dev / 0]}
                                    and actOutlifMask { isMultiple[0 / 1],
                                                        isTrunk[0],
                                                        vidx[0 / 4095],
                                                        portTrunk[63 / 0],
                                                        devNum[127 / 0]}.
            Expected: GT_OK.
        */
        /* iterate with actOutlif.isMultiple = 0 */
        actOutlif.isMultiple = 0;
        actOutlif.isTrunk    = 0;
        actOutlif.vidx       = 0;
        actOutlif.portTrunk  = 0;
        actOutlif.devNum     = dev;

        actOutlifMask.isMultiple = 0;
        actOutlifMask.isTrunk    = 0;
        actOutlifMask.vidx       = 0;
        actOutlif.portTrunk      = 0;
        actOutlif.devNum         = 0;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgFdbActionActiveOutlifGet
            Expected: GT_OK and the same actOutlifPtr and actOutlifMaskPtr.
        */
        cpssOsBzero((GT_VOID*) &actOutlifGet, sizeof(actOutlifGet));
        cpssOsBzero((GT_VOID*) &actOutlifMaskGet, sizeof(actOutlifMaskGet));

        st = cpssExMxPmBrgFdbActionActiveOutlifGet(dev, &actOutlifGet, &actOutlifMaskGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbActionActiveOutlifGet: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &actOutlif, (GT_VOID*) &actOutlifGet, sizeof(actOutlif) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another actOutlif then was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &actOutlifMask, (GT_VOID*) &actOutlifMaskGet, sizeof(actOutlifMask) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another actOutlifMask then was set: %d", dev);

        /* iterate with actOutlif.isMultiple = 1 */
        actOutlif.isMultiple = 1;
        actOutlif.vidx       = 100;

        actOutlifMask.isMultiple = 1;
        actOutlifMask.vidx       = 0xFFF;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgFdbActionActiveOutlifGet
            Expected: GT_OK and the same actOutlifPtr and actOutlifMaskPtr.
        */
        cpssOsBzero((GT_VOID*) &actOutlifGet, sizeof(actOutlifGet));
        cpssOsBzero((GT_VOID*) &actOutlifMaskGet, sizeof(actOutlifMaskGet));

        st = cpssExMxPmBrgFdbActionActiveOutlifGet(dev, &actOutlifGet, &actOutlifMaskGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbActionActiveOutlifGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(actOutlif.isMultiple, actOutlifGet.isMultiple,
                       "get another actOutlifPtr->isMultiplethan was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(actOutlif.vidx, actOutlifGet.vidx,
                       "get another actOutlifPtr->vidx was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(actOutlifMask.isMultiple, actOutlifMaskGet.isMultiple,
                       "get another actOutlifMaskPtr->isMultiplethan was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(actOutlifMask.vidx, actOutlifMaskGet.vidx,
                       "get another actOutlifMaskPtr->vidx was set: %d", dev);

        /*
            1.3 Call with actOutlifPtr->isMultiple[2] (out of range)
            Expected: NOT GT_OK
        */
        actOutlif.isMultiple = 2;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d",
                                         dev, actOutlif.isMultiple);

        actOutlif.isMultiple = 0;

        actOutlifMask.isTrunk = 1;
        actOutlifMask.portTrunk = 63;
        actOutlifMask.devNum    = 127;

        /*
            1.4 Call with actOutlifPtr->isTrunk[2] (out of range)
            Expected: NOT GT_OK
        */
        actOutlif.isTrunk = 2;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isTrunk = %d",
                                         dev, actOutlif.isTrunk);

        actOutlif.isTrunk = 0;

        /*
            1.5. Call with actOutlifPtr->isMultiple[0],
                           and actOutlifPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] out of range
            Expected: NOT GT_OK.
        */
        actOutlif.isMultiple = 0;
        actOutlif.devNum     = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d, actOutlifPtr->devNum = %d",
                                         dev, actOutlif.isMultiple, actOutlif.devNum);

        actOutlif.devNum = 0;

        /*
            1.6. Call with actOutlifPtr->isMultiple[1],
                           and actOutlifPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] not relevant
            Expected: GT_OK.
        */
        actOutlif.isMultiple = 1;
        actOutlif.devNum     = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d, actOutlifPtr->devNum = %d",
                                     dev, actOutlif.isMultiple, actOutlif.devNum);

        actOutlif.devNum = 0;

        /*
            1.7. Call with actOutlifPtr->isMultiple[1],
                           and actOutlifPtr->vidx [4096] out of range
            Expected: NOT GT_OK.
        */
        actOutlif.isMultiple = 1;
        actOutlif.vidx       = 4096;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d, actOutlifPtr->vidx = %d",
                                         dev, actOutlif.isMultiple, actOutlif.vidx);

        actOutlif.vidx = 100;

        /*
            1.8. Call with actOutlifPtr->isMultiple[0],
                           and actOutlifPtr->vidx [4096] not relevant
            Expected: GT_OK.
        */
        actOutlif.isMultiple = 0;
        actOutlif.vidx       = 4096;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d, actOutlifPtr->vidx = %d",
                                     dev, actOutlif.isMultiple, actOutlif.vidx);

        actOutlif.vidx = 100;

        /*
            1.9. Call with actOutlifPtr->isMultiple[0],
                           actOutlifPtr->isTrunk[0],
                           actOutlifPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] out of range
            Expected: NOT GT_OK.
        */
        actOutlif.isMultiple = 0;
        actOutlif.isTrunk    = 0;
        actOutlif.portTrunk  = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d, actOutlifPtr->isTrunk = %d, actOutlifPtr->portTrunk = %d",
                                         dev, actOutlif.isMultiple, actOutlif.isTrunk, actOutlif.portTrunk);

        actOutlif.portTrunk  = 0;

        /*
            1.10. Call with actOutlifPtr->isMultiple[1],
                            actOutlifPtr->isTrunk[0],
                            actOutlifPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant
            Expected: GT_OK.
        */
        actOutlif.isMultiple = 1;
        actOutlif.isTrunk    = 0;
        actOutlif.portTrunk  = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d, actOutlifPtr->isTrunk = %d, actOutlifPtr->portTrunk = %d",
                                     dev, actOutlif.isMultiple, actOutlif.isTrunk, actOutlif.portTrunk);

        actOutlif.portTrunk  = 0;

        /*
            1.11. Call with actOutlifPtr->isMultiple[0],
                            actOutlifPtr->isTrunk[1],
                            actOutlifPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant
            Expected: GT_OK.
        */
        actOutlif.isMultiple = 0;
        actOutlif.isTrunk    = 1;
        actOutlif.portTrunk  = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d, actOutlifPtr->isTrunk = %d, actOutlifPtr->portTrunk = %d",
                                     dev, actOutlif.isMultiple, actOutlif.isTrunk, actOutlif.portTrunk);

        actOutlif.portTrunk  = 0;

        /*
            1.12 Call with actOutlifMaskPtr->isMultiple[2] (out of range)
            Expected: NOT GT_OK
        */
        actOutlifMask.isMultiple = 2;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, actOutlifMaskPtr->isMultiple = %d",
                                         dev, actOutlifMask.isMultiple);

        actOutlifMask.isMultiple = 1;

        /*
            1.13 Call with actOutlifMaskPtr->isTrunk[2] (out of range)
            Expected: NOT GT_OK
        */
        actOutlifMask.isTrunk = 2;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, actOutlifMaskPtr->isTrunk = %d",
                                         dev, actOutlifMask.isTrunk);

        actOutlifMask.isTrunk = 1;

        /*
            1.14. Call with actOutlifPtr->isMultiple[0],
                           and actOutlifMaskPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] out of range
            Expected: NOT GT_OK.
        */
        actOutlif.isMultiple = 0;
        actOutlifMask.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d, actOutlifMaskPtr->devNum = %d",
                                         dev, actOutlif.isMultiple, actOutlifMask.devNum);

        actOutlifMask.devNum = 127;

        /*
            1.15. Call with actOutlifPtr->isMultiple[1],
                           and actOutlifMaskPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] not relevant
            Expected: GT_OK.
        */
        actOutlif.isMultiple = 1;
        actOutlifMask.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d, actOutlifMaskPtr->devNum = %d",
                                     dev, actOutlif.isMultiple, actOutlifMask.devNum);

        actOutlifMask.devNum = 127;

        /*
            1.16. Call with actOutlifPtr->isMultiple[1],
                           and actOutlifMaskPtr->vidx [4096] out of range
            Expected: NOT GT_OK.
        */
        actOutlif.isMultiple = 1;
        actOutlifMask.vidx   = 4096;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d, actOutlifMaskPtr->vidx = %d",
                                         dev, actOutlif.isMultiple, actOutlifMask.vidx);

        actOutlifMask.vidx = 4095;

        /*
            1.17. Call with actOutlifPtr->isMultiple[0],
                           and actOutlifMaskPtr->vidx [4096] not relevant
            Expected: GT_OK.
        */
        actOutlif.isMultiple = 0;
        actOutlifMask.vidx   = 4096;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d, actOutlifMaskPtr->vidx = %d",
                                     dev, actOutlif.isMultiple, actOutlifMask.vidx);

        actOutlifMask.vidx = 4095;

        /*
            1.18. Call with actOutlifPtr->isMultiple[0],
                           actOutlifPtr->isTrunk[0],
                           actOutlifMaskPtr->portTrunk [PRV_CPSS_MAX_PP_DEVICES_CNS] out of range
            Expected: NOT GT_OK.
        */
        actOutlif.isMultiple    = 0;
        actOutlif.isTrunk       = 0;
        actOutlifMask.portTrunk = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d, actOutlifMaskPtr->portTrunk = %d",
                                         dev, actOutlif.isMultiple, actOutlifMask.portTrunk);

        actOutlifMask.portTrunk = 63;

        /*
            1.19. Call with actOutlifPtr->isMultiple[1],
                            actOutlifPtr->isTrunk[0],
                            actOutlifMaskPtr->portTrunk [PRV_CPSS_MAX_PP_DEVICES_CNS] not relevant
            Expected: GT_OK.
        */
        actOutlif.isMultiple    = 1;
        actOutlif.isTrunk       = 0;
        actOutlifMask.portTrunk = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d, actOutlifMaskPtr->portTrunk = %d",
                                     dev, actOutlif.isMultiple, actOutlifMask.portTrunk);

        actOutlifMask.portTrunk = 63;

        /*
            1.20. Call with actOutlifPtr->isMultiple[0],
                            actOutlifPtr->isTrunk[1],
                            actOutlifMaskPtr->portTrunk [PRV_CPSS_MAX_PP_DEVICES_CNS] not relevant
            Expected: GT_OK.
        */
        actOutlif.isMultiple    = 0;
        actOutlif.isTrunk       = 1;
        actOutlifMask.portTrunk = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, actOutlifPtr->isMultiple = %d, actOutlifMaskPtr->portTrunk = %d",
                                     dev, actOutlif.isMultiple, actOutlifMask.portTrunk);

        actOutlifMask.portTrunk = 63;

        /*
            1.21. Call function with actOutlifPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, NULL, &actOutlifMask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actOutlifPtr = NULL", dev);

        /*
            1.22. Call function with actOutlifMaskPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actOutlifMask = NULL", dev);
    }

    actOutlif.isMultiple = 0;
    actOutlif.isTrunk    = 0;
    actOutlif.vidx       = 0;
    actOutlif.portTrunk  = 0;
    actOutlif.devNum     = dev;

    actOutlifMask.isMultiple = 0;
    actOutlifMask.isTrunk    = 0;
    actOutlifMask.vidx       = 0;
    actOutlifMask.portTrunk  = 63;
    actOutlifMask.devNum     = 127;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbActionActiveOutlifSet(dev, &actOutlif, &actOutlifMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbActionActiveOutlifGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC *actOutlifPtr,
    OUT CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC *actOutlifMaskPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbActionActiveOutlifGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null actOutlifPtr and non-null actOutlifMaskPtr.
    Expected: GT_OK.
    1.2. Call with null actOutlifPtr [NULL] and non-null actOutlifMaskPtr.
    Expected: GT_BAD_PTR.
    1.3. Call with non-null actOutlifPtr and null actOutlifMaskPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC actOutlif;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC actOutlifMask;


    cpssOsBzero((GT_VOID*) &actOutlif, sizeof(actOutlif));
    cpssOsBzero((GT_VOID*) &actOutlifMask, sizeof(actOutlifMask));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null actOutlifPtr
                           and non-null actOutlifMaskPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbActionActiveOutlifGet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null actOutlifPtr [NULL]
                                and non-null actOutlifMaskPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionActiveOutlifGet(dev, NULL, &actOutlifMask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actOutlifPtr = NULL", dev);

        /*
            1.3. Call with non-null actOutlifPtr
                           and null actOutlifMaskPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionActiveOutlifGet(dev, &actOutlif, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actOutlifMaskPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbActionActiveOutlifGet(dev, &actOutlif, &actOutlifMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbActionActiveOutlifGet(dev, &actOutlif, &actOutlifMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbActionTransplantOutlifSet
(
    IN GT_U8                                devNum,
    IN CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC     *newOutlifPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbActionTransplantOutlifSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with newOutlifPtr {isMultiple[0 / 1],
                                          isTrunk[0 / 1],
                                          vidx[0 / 4095],
                                          portTrunk[0],
                                          devNum[0]}
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbActionTransplantOutlifGet
    Expected: GT_OK and the same newOutlifPtr.
    1.3 Call with newOutlifPtr->isTrunk[2] (out of range)
    Expected: NOT GT_OK
    1.4. Call with newOutlifPtr->isMultiple[0],
                   and newOutlifPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] out of range
    Expected: NOT GT_OK.
    1.5. Call with newOutlifPtr->isMultiple[1],
                   and newOutlifPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] not relevant
    Expected: GT_OK.
    1.6. Call with newOutlifPtr->isMultiple[1],
                   and newOutlifPtr->vidx [4096] out of range
    Expected: NOT GT_OK.
    1.7. Call with newOutlifPtr->isMultiple[0],
                   and newOutlifPtr->vidx [4096] not relevant
    Expected: GT_OK.
    1.8. Call with newOutlifPtr->isMultiple[0],
                   newOutlifPtr->isTrunk[0],
                   newOutlifPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] out of range
    Expected: NOT GT_OK.
    1.9. Call with newOutlifPtr->isMultiple[1],
                    newOutlifPtr->isTrunk[0],
                    newOutlifPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant
    Expected: GT_OK.
    1.10. Call with newOutlifPtr->isMultiple[0],
                    newOutlifPtr->isTrunk[1],
                    newOutlifPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant
    Expected: GT_OK.
    1.11. Call with newOutlifPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st      = GT_OK;
    GT_U8       dev;
    GT_BOOL     isEqual = GT_FALSE;

    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC newOutlif;
    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC newOutlifGet;


    cpssOsBzero((GT_VOID*) &newOutlif, sizeof(newOutlif));
    cpssOsBzero((GT_VOID*) &newOutlifGet, sizeof(newOutlifGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with newOutlifPtr {isMultiple[0 / 1],
                                                  isTrunk[0 / 1],
                                                  vidx[0 / 4095],
                                                  portTrunk[0],
                                                  devNum[0]}
            Expected: GT_OK.
        */
        /* iterate with newOutlif.isMultiple = 0 */
        newOutlif.isMultiple = 0;
        newOutlif.isTrunk    = 0;
        newOutlif.vidx       = 0;
        newOutlif.portTrunk  = 0;
        newOutlif.devNum     = 0;

        st = cpssExMxPmBrgFdbActionTransplantOutlifSet(dev, &newOutlif);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgFdbActionTransplantOutlifGet
            Expected: GT_OK and the same newOutlifPtr.
        */
        st = cpssExMxPmBrgFdbActionTransplantOutlifGet(dev, &newOutlifGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbActionTransplantOutlifGet: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &newOutlif, (GT_VOID*) &newOutlifGet, sizeof(newOutlif) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another newOutlif then was set: %d", dev);

        /* iterate with newOutlif.isMultiple = 1 */
        newOutlif.isMultiple = 1;
        newOutlif.vidx       = 4095;

        st = cpssExMxPmBrgFdbActionTransplantOutlifSet(dev, &newOutlif);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgFdbActionTransplantOutlifGet
            Expected: GT_OK and the same newOutlifPtr.
        */
        st = cpssExMxPmBrgFdbActionTransplantOutlifGet(dev, &newOutlifGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbActionTransplantOutlifGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(newOutlif.isMultiple, newOutlifGet.isMultiple,
                       "get another actOutlifPtr->isMultiplethan was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(newOutlif.vidx, newOutlifGet.vidx,
                       "get another actOutlifPtr->vidx was set: %d", dev);

        /*
            1.3 Call with newOutlifPtr->isMultiple[0] and newOutlifPtr->isTrunk[2] (out of range)
            Expected: NOT GT_OK
        */
        newOutlif.isMultiple = 0;
        newOutlif.isTrunk = 2;

        st = cpssExMxPmBrgFdbActionTransplantOutlifSet(dev, &newOutlif);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, newOutlif.isTrunk = %d",
                                        dev, newOutlif.isTrunk);

        newOutlif.isTrunk = 0;

        /*
            1.4. Call with newOutlifPtr->isMultiple[0],
                           and newOutlifPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] out of range
            Expected: NOT GT_OK.
        */
        newOutlif.isMultiple = 0;
        newOutlif.devNum     = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmBrgFdbActionTransplantOutlifSet(dev, &newOutlif);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, newOutlif.isMultiple = %d, newOutlif.devNum = %d",
                                        dev, newOutlif.isMultiple, newOutlif.devNum);

        newOutlif.devNum = 0;

        /*
            1.5. Call with newOutlifPtr->isMultiple[1],
                           and newOutlifPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] not relevant
            Expected: GT_OK.
        */
        newOutlif.isMultiple = 1;
        newOutlif.devNum     = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmBrgFdbActionTransplantOutlifSet(dev, &newOutlif);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, newOutlif.isMultiple = %d, newOutlif.devNum = %d",
                                    dev, newOutlif.isMultiple, newOutlif.devNum);

        newOutlif.devNum = 0;

        /*
            1.6. Call with newOutlifPtr->isMultiple[1],
                           and newOutlifPtr->vidx [4096] out of range
            Expected: NOT GT_OK.
        */
        newOutlif.isMultiple = 1;
        newOutlif.vidx       = 4096;

        st = cpssExMxPmBrgFdbActionTransplantOutlifSet(dev, &newOutlif);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, newOutlif.isMultiple = %d, newOutlif.vidx = %d",
                                        dev, newOutlif.isMultiple, newOutlif.vidx);

        newOutlif.vidx = 100;

        /*
            1.7. Call with newOutlifPtr->isMultiple[0],
                           and newOutlifPtr->vidx [4096] not relevant
            Expected: GT_OK.
        */
        newOutlif.isMultiple = 0;
        newOutlif.vidx       = 4096;

        st = cpssExMxPmBrgFdbActionTransplantOutlifSet(dev, &newOutlif);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, newOutlif.isMultiple = %d, newOutlif.vidx = %d",
                                    dev, newOutlif.isMultiple, newOutlif.vidx);

        newOutlif.vidx = 100;

        /*
            1.8. Call with newOutlifPtr->isMultiple[0],
                           newOutlifPtr->isTrunk[0],
                           newOutlifPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] out of range
            Expected: NOT GT_OK.
        */
        newOutlif.isMultiple = 0;
        newOutlif.isTrunk    = 0;
        newOutlif.portTrunk  = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgFdbActionTransplantOutlifSet(dev, &newOutlif);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, newOutlif.isMultiple = %d, newOutlif.isTrunk = %d, newOutlif.portTrunk = %d",
                                        dev, newOutlif.isMultiple, newOutlif.isTrunk, newOutlif.portTrunk);

        newOutlif.portTrunk  = 0;

        /*
            1.9. Call with newOutlifPtr->isMultiple[1],
                            newOutlifPtr->isTrunk[0],
                            newOutlifPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant
            Expected: GT_OK.
        */
        newOutlif.isMultiple = 1;
        newOutlif.isTrunk    = 0;
        newOutlif.portTrunk  = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgFdbActionTransplantOutlifSet(dev, &newOutlif);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, newOutlif.isMultiple = %d, newOutlif.isTrunk = %d, newOutlif.portTrunk = %d",
                                    dev, newOutlif.isMultiple, newOutlif.isTrunk, newOutlif.portTrunk);

        newOutlif.portTrunk  = 0;

        /*
            1.10. Call with newOutlifPtr->isMultiple[0],
                            newOutlifPtr->isTrunk[1],
                            newOutlifPtr->portTrunk [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant
            Expected: GT_OK.
        */
        newOutlif.isMultiple = 0;
        newOutlif.isTrunk    = 1;
        newOutlif.portTrunk  = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgFdbActionTransplantOutlifSet(dev, &newOutlif);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, newOutlif.isMultiple = %d, newOutlif.isTrunk = %d, newOutlif.portTrunk = %d",
                                    dev, newOutlif.isMultiple, newOutlif.isTrunk, newOutlif.portTrunk);

        newOutlif.portTrunk  = 0;

        /*
            1.11. Call with newOutlifPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionTransplantOutlifSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, newOutlifPtr = NULL", dev);
    }

    newOutlif.isMultiple = 0;
    newOutlif.isTrunk    = 0;
    newOutlif.vidx       = 0;
    newOutlif.portTrunk  = 0;
    newOutlif.devNum     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbActionTransplantOutlifSet(dev, &newOutlif);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbActionTransplantOutlifSet(dev, &newOutlif);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbActionTransplantOutlifGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC *newOutlifPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbActionTransplantOutlifGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL newOutlifPtr
    Expected: GT_OK.
    1.2. Call with not newOutlifPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_SCAN_OUTLIF_STC newOutlif;


    cpssOsBzero((GT_VOID*) &newOutlif, sizeof(newOutlif));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL newOutlifPtr
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbActionTransplantOutlifGet(dev, &newOutlif);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with not newOutlifPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionTransplantOutlifGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, newOutlifPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbActionTransplantOutlifGet(dev, &newOutlif);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbActionTransplantOutlifGet(dev, &newOutlif);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbActionActiveL2VpnSet
(
    IN GT_U8                               devNum,
    IN CPSS_EXMXPM_FDB_SCAN_L2VPN_STC      *l2VpnPtr,
    IN CPSS_EXMXPM_FDB_SCAN_L2VPN_STC      *l2VpnMaskPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbActionActiveL2VpnSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with l2VpnPtr {isDit[0 / 1],
                             ditPtr[0 / 0xFFFF],
                             pwId[0 / 127] },
                   l2VpnMaskPtr{isDit[1],
                                ditPtr[0xFFFF],
                                pwId[127] },
    Expected: GT_OK.

    1.2. Call cpssExMxPmBrgFdbActionActiveL2VpnGet with not NULL l2VpnPtr and l2VpnMaskPtr.
    Expected: GT_OK and the same l2VpnPtr and l2VpnMaskPtr as was set.
    1.3. Call with l2VpnPtr->isDit [2] out of range.
    Expected: NOT GT_OK
    1.4. Call with l2VpnPtr->ditPtr [0x10000] out of range.
    Expected: NOT GT_OK
    1.5. Call with l2VpnPtr->pwId [128] out of range.
    Expected: NOT GT_OK
    1.6. Call with l2VpnMaskPtr->isDit [2] out of range.
    Expected: NOT GT_OK
    1.7. Call with l2VpnMaskPtr->ditPtr [0x10000] out of range.
    Expected: NOT GT_OK
    1.8. Call with l2VpnMaskPtr->pwId [128] out of range.
    Expected: NOT GT_OK
    1.9. Call with l2VpnPtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.10. Call with l2VpnMaskPtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_BOOL     isEqual = GT_FALSE;

    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC      l2Vpn;
    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC      l2VpnMask;
    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC      l2VpnGet;
    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC      l2VpnMaskGet;


    cpssOsBzero((GT_VOID*) &l2Vpn, sizeof(l2Vpn));
    cpssOsBzero((GT_VOID*) &l2VpnGet, sizeof(l2VpnGet));
    cpssOsBzero((GT_VOID*) &l2VpnMask, sizeof(l2VpnMask));
    cpssOsBzero((GT_VOID*) &l2VpnMaskGet, sizeof(l2VpnMaskGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with l2VpnPtr {isDit[0 / 1],
                                     ditPtr[0 / 0xFFFF],
                                     pwId[0 / 127] },
                           l2VpnMaskPtr{isDit[1],
                                        ditPtr[0xFFFF],
                                        pwId[127] },
            Expected: GT_OK.
        */
        /* iterate with l2Vpn.isDit = 0 */
        l2Vpn.isDit  = 0;
        l2Vpn.ditPtr = 0;
        l2Vpn.pwId   = 0;

        l2VpnMask.isDit  = 0;
        l2VpnMask.ditPtr = 0;
        l2VpnMask.pwId   = 0;

        st = cpssExMxPmBrgFdbActionActiveL2VpnSet(dev, &l2Vpn, &l2VpnMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgFdbActionActiveL2VpnGet with not NULL l2VpnPtr and l2VpnMaskPtr.
            Expected: GT_OK and the same l2VpnPtr and l2VpnMaskPtr as was set.
        */
        st = cpssExMxPmBrgFdbActionActiveL2VpnGet(dev, &l2VpnGet, &l2VpnMaskGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbActionActiveL2VpnGet: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &l2VpnMask, (GT_VOID*) &l2VpnMaskGet, sizeof(l2VpnMask) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another l2VpnMask then was set: %d", dev);

        /* iterate with l2Vpn.isDit = 1 */
        l2Vpn.isDit  = 1;
        l2Vpn.ditPtr = 0xFFFF;
        l2Vpn.pwId   = 127;

        l2VpnMask.isDit  = 1;
        l2VpnMask.ditPtr = 0xFFFF;
        l2VpnMask.pwId   = 127;

        st = cpssExMxPmBrgFdbActionActiveL2VpnSet(dev, &l2Vpn, &l2VpnMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgFdbActionActiveL2VpnGet with not NULL l2VpnPtr and l2VpnMaskPtr.
            Expected: GT_OK and the same l2VpnPtr and l2VpnMaskPtr as was set.
        */
        st = cpssExMxPmBrgFdbActionActiveL2VpnGet(dev, &l2VpnGet, &l2VpnMaskGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbActionActiveL2VpnGet: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &l2Vpn, (GT_VOID*) &l2VpnGet, sizeof(l2Vpn) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another l2Vpn then was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &l2VpnMask, (GT_VOID*) &l2VpnMaskGet, sizeof(l2VpnMask) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another l2VpnMask then was set: %d", dev);

        /*
            1.3. Call with l2VpnPtr->isDit [2] out of range.
            Expected: NOT GT_OK
        */
        l2Vpn.isDit = 2;

        st = cpssExMxPmBrgFdbActionActiveL2VpnSet(dev, &l2Vpn, &l2VpnMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, l2VpnPtr->isDit = %d", dev, l2Vpn.isDit);

        l2Vpn.isDit = 0;

        /*
            1.4. Call with l2VpnPtr->ditPtr [0x10000] out of range.
            Expected: NOT GT_OK
        */
        l2Vpn.ditPtr = 0x10000;

        st = cpssExMxPmBrgFdbActionActiveL2VpnSet(dev, &l2Vpn, &l2VpnMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, l2VpnPtr->ditPtr = %d", dev, l2Vpn.ditPtr);

        l2Vpn.ditPtr = 0;

        /*
            1.5. Call with l2VpnPtr->pwId [128] out of range.
            Expected: NOT GT_OK
        */
        l2Vpn.pwId = 128;

        st = cpssExMxPmBrgFdbActionActiveL2VpnSet(dev, &l2Vpn, &l2VpnMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, l2VpnPtr->pwId = %d", dev, l2Vpn.pwId);

        l2Vpn.pwId = 0;

        /*
            1.6. Call with l2VpnMaskPtr->isDit [2] out of range.
            Expected: NOT GT_OK
        */
        l2VpnMask.isDit = 2;

        st = cpssExMxPmBrgFdbActionActiveL2VpnSet(dev, &l2Vpn, &l2VpnMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, l2VpnMaskPtr->isDit = %d", dev, l2VpnMask.isDit);

        l2VpnMask.isDit = 0;

        /*
            1.7. Call with l2VpnMaskPtr->ditPtr [0x10000] out of range.
            Expected: NOT GT_OK
        */
        l2VpnMask.ditPtr = 0x10000;

        st = cpssExMxPmBrgFdbActionActiveL2VpnSet(dev, &l2Vpn, &l2VpnMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, l2VpnMaskPtr->ditPtr = %d", dev, l2VpnMask.ditPtr);

        l2VpnMask.ditPtr = 0;

        /*
            1.8. Call with l2VpnMaskPtr->pwId [128] out of range.
            Expected: NOT GT_OK
        */
        l2VpnMask.pwId = 128;

        st = cpssExMxPmBrgFdbActionActiveL2VpnSet(dev, &l2Vpn, &l2VpnMask);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, l2VpnMaskPtr->pwId = %d", dev, l2VpnMask.pwId);

        l2VpnMask.pwId = 0;

        /*
            1.9. Call with l2VpnPtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionActiveL2VpnSet(dev, NULL, &l2VpnMask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, l2VpnPtr = null", dev);

        /*
            1.10. Call with l2VpnMaskPtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionActiveL2VpnSet(dev, &l2Vpn, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, l2VpnMaskPtr = null", dev);
    }

    l2Vpn.isDit  = 0;
    l2Vpn.ditPtr = 0;
    l2Vpn.pwId   = 0;

    l2VpnMask.isDit  = 1;
    l2VpnMask.ditPtr = 0xFFFF;
    l2VpnMask.pwId   = 127;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbActionActiveL2VpnSet(dev, &l2Vpn, &l2VpnMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbActionActiveL2VpnSet(dev, &l2Vpn, &l2VpnMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbActionActiveL2VpnGet
(
    IN  GT_U8                              devNum,
    OUT CPSS_EXMXPM_FDB_SCAN_L2VPN_STC     *l2VpnPtr,
    OUT CPSS_EXMXPM_FDB_SCAN_L2VPN_STC     *l2VpnMaskPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbActionActiveL2VpnGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not-NULL l2VpnPtr and l2VpnMaskPtr.
    Expected: GT_OK.
    1.2. Call with l2VpnPtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with l2VpnMaskPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC      l2Vpn;
    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC      l2VpnMask;


    cpssOsBzero((GT_VOID*) &l2Vpn, sizeof(l2Vpn));
    cpssOsBzero((GT_VOID*) &l2VpnMask, sizeof(l2VpnMask));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not-NULL l2VpnPtr and l2VpnMaskPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbActionActiveL2VpnGet(dev, &l2Vpn, &l2VpnMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with l2VpnPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionActiveL2VpnGet(dev, NULL, &l2VpnMask);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, l2VpnPtr = null", dev);

        /*
            1.3. Call with l2VpnMaskPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionActiveL2VpnGet(dev, &l2Vpn, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, l2VpnMaskPtr = null", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbActionActiveL2VpnGet(dev, &l2Vpn, &l2VpnMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbActionActiveL2VpnGet(dev, &l2Vpn, &l2VpnMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbActionTransplantL2VpnSet
(
    IN GT_U8                              devNum,
    IN CPSS_EXMXPM_FDB_SCAN_L2VPN_STC    *newL2VpnPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbActionTransplantL2VpnSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with newl2VpnPtr {isDit[0 / 1],
                             ditPtr[0 / 0xFFFF],
                             pwId[0 / 127] },
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbActionTransplantL2VpnGet with not NULL l2VpnPtr and l2VpnMaskPtr.
    Expected: GT_OK and the same newl2VpnPtr as was set.
    1.3. Call with newl2VpnPtr->isDit [2] out of range.
    Expected: NOT GT_OK
    1.4. Call with newl2VpnPtr->ditPtr [0x10000] out of range.
    Expected: NOT GT_OK
    1.5. Call with newl2VpnPtr->pwId [128] out of range.
    Expected: NOT GT_OK
    1.6. Call with newl2VpnPtr [NULL]
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_BOOL     isEqual = GT_FALSE;

    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC    newL2Vpn;
    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC    newL2VpnGet;


    cpssOsBzero((GT_VOID*) &newL2Vpn, sizeof(newL2Vpn));
    cpssOsBzero((GT_VOID*) &newL2VpnGet, sizeof(newL2VpnGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with newl2VpnPtr {isDit[0 / 1],
                                     ditPtr[0 / 0xFFFF],
                                     pwId[0 / 127] },
            Expected: GT_OK.
        */
        /* iterate with newL2Vpn.isDit = 0 */
        newL2Vpn.isDit  = 0;
        newL2Vpn.ditPtr = 0;
        newL2Vpn.pwId   = 0;

        st = cpssExMxPmBrgFdbActionTransplantL2VpnSet(dev, &newL2Vpn);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgFdbActionTransplantL2VpnGet with not NULL l2VpnPtr and l2VpnMaskPtr.
            Expected: GT_OK and the same newl2VpnPtr as was set.
        */
        st = cpssExMxPmBrgFdbActionTransplantL2VpnGet(dev, &newL2VpnGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbActionTransplantL2VpnGet: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &newL2Vpn, (GT_VOID*) &newL2VpnGet, sizeof(newL2Vpn) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another newL2Vpn then was set: %d", dev);

        /* iterate with newL2Vpn.isDit = 1 */
        newL2Vpn.isDit  = 1;
        newL2Vpn.ditPtr = 0xFFFF;
        newL2Vpn.pwId   = 127;

        st = cpssExMxPmBrgFdbActionTransplantL2VpnSet(dev, &newL2Vpn);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgFdbActionTransplantL2VpnGet with not NULL l2VpnPtr and l2VpnMaskPtr.
            Expected: GT_OK and the same newl2VpnPtr as was set.
        */
        st = cpssExMxPmBrgFdbActionTransplantL2VpnGet(dev, &newL2VpnGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbActionTransplantL2VpnGet: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &newL2Vpn, (GT_VOID*) &newL2VpnGet, sizeof(newL2Vpn) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another newL2Vpn then was set: %d", dev);

        /*
            1.3. Call with newl2VpnPtr->isDit [2] out of range.
            Expected: NOT GT_OK
        */
        newL2Vpn.isDit = 2;

        st = cpssExMxPmBrgFdbActionTransplantL2VpnSet(dev, &newL2Vpn);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, newl2VpnPtr->isDit = %d", dev, newL2Vpn.isDit);

        newL2Vpn.isDit = 0;

        /*
            1.4. Call with newl2VpnPtr->ditPtr [0x10000] out of range.
            Expected: NOT GT_OK
        */
        newL2Vpn.ditPtr = 0x10000;

        st = cpssExMxPmBrgFdbActionTransplantL2VpnSet(dev, &newL2Vpn);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, newl2VpnPtr->ditPtr = %d", dev, newL2Vpn.ditPtr);

        newL2Vpn.ditPtr = 0;

        /*
            1.5. Call with newl2VpnPtr->pwId [128] out of range.
            Expected: NOT GT_OK
        */
        newL2Vpn.pwId = 128;

        st = cpssExMxPmBrgFdbActionTransplantL2VpnSet(dev, &newL2Vpn);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, newl2VpnPtr->pwId = %d", dev, newL2Vpn.pwId);

        newL2Vpn.pwId = 0;

        /*
            1.6. Call with newl2VpnPtr [NULL]
            Expected: GT_BAD_PTR
        */
        st = cpssExMxPmBrgFdbActionTransplantL2VpnSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, newl2VpnPtr = NULL", dev);
    }

    newL2Vpn.isDit  = 0;
    newL2Vpn.ditPtr = 0;
    newL2Vpn.pwId   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbActionTransplantL2VpnSet(dev, &newL2Vpn);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbActionTransplantL2VpnSet(dev, &newL2Vpn);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbActionTransplantL2VpnGet
(
    IN  GT_U8                             devNum,
    OUT CPSS_EXMXPM_FDB_SCAN_L2VPN_STC    *newL2VpnPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbActionTransplantL2VpnGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL newL2VpnPtr.
    Expected: GT_OK.
    1.2. Call with newL2VpnPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_SCAN_L2VPN_STC    newL2Vpn;


    cpssOsBzero((GT_VOID*) &newL2Vpn, sizeof(newL2Vpn));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL newL2VpnPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbActionTransplantL2VpnGet(dev, &newL2Vpn);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with newL2VpnPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbActionTransplantL2VpnGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, newl2VpnPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbActionTransplantL2VpnGet(dev, &newL2Vpn);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbActionTransplantL2VpnGet(dev, &newL2Vpn);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  ageOutAllDevOnTrunkEnable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with ageOutAllDevOnTrunkEnable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet
    Expected: GT_OK and the same ageOutAllDevOnTrunkEnablePtr.
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
            1.1. Call function with ageOutAllDevOnTrunkEnable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with ageOutAllDevOnTrunkEnable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet
            Expected: GT_OK and the same ageOutAllDevOnTrunkEnablePtr.
        */
        st = cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another ageOutAllDevOnTrunkEnablePtr than was set: %d", dev);

        /* Call with ageOutAllDevOnTrunkEnable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet
            Expected: GT_OK and the same ageOutAllDevOnTrunkEnablePtr.
        */
        st = cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another ageOutAllDevOnTrunkEnablePtr than was set: %d", dev);
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
        st = cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *ageOutAllDevOnTrunkEnablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null ageOutAllDevOnTrunkEnablePtr.
    Expected: GT_OK.
    1.2. Call with null ageOutAllDevOnTrunkEnablePtr [NULL].
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
            1.1. Call with non-null ageOutAllDevOnTrunkEnablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null ageOutAllDevOnTrunkEnablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ageOutAllDevOnTrunkEnablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableSet
(
    IN GT_U8                       devNum,
    IN GT_BOOL                     deleteStatic
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with deleteStatic [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet
    Expected: GT_OK and the same deleteStaticPtr.
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
            1.1. Call function with deleteStatic [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with deleteStatic [GT_FALSE] */
        state = GT_FALSE;

        st = cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet
            Expected: GT_OK and the same deleteStaticPtr.
        */
        st = cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another deleteStaticPtr than was set: %d", dev);

        /* Call with deleteStatic [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet
            Expected: GT_OK and the same deleteStaticPtr.
        */
        st = cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another deleteStaticPtr than was set: %d", dev);
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
        st = cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *deleteStaticPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null deleteStaticPtr.
    Expected: GT_OK.
    1.2. Call with null deleteStaticPtr [NULL].
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
            1.1. Call with non-null deleteStaticPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null deleteStaticPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, deleteStaticPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbStaticDelEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbStaticDelEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbStaticDelEnableGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbStaticDelEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbStaticDelEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbStaticDelEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbStaticDelEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbStaticDelEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbStaticDelEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbStaticDelEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbStaticDelEnableGet: %d", dev);

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
        st = cpssExMxPmBrgFdbStaticDelEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbStaticDelEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbStaticDelEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbStaticDelEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbStaticDelEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbStaticDelEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbStaticDelEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbStaticDelEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbStaticTransEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbStaticTransEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbStaticTransEnableGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbStaticTransEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbStaticTransEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbStaticTransEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbStaticTransEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbStaticTransEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbStaticTransEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbStaticTransEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbStaticTransEnableGet: %d", dev);

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
        st = cpssExMxPmBrgFdbStaticTransEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbStaticTransEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbStaticTransEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbStaticTransEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbStaticTransEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbStaticTransEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbStaticTransEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbStaticTransEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbLsrEnableSet
(
    IN GT_U8         devNum,
    IN GT_BOOL       enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbLsrEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbLsrEnableGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbLsrEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbLsrEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbLsrEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbLsrEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbLsrEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbLsrEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbLsrEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbLsrEnableGet: %d", dev);

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
        st = cpssExMxPmBrgFdbLsrEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbLsrEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbLsrEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbLsrEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbLsrEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbLsrEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbLsrEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbLsrEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbLsrStoppedGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbLsrStoppedGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


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
        st = cpssExMxPmBrgFdbLsrStoppedGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbLsrStoppedGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbLsrStoppedGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbLsrStoppedGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbAgeRefreshEnableSet
(
    IN GT_U8         devNum,
    IN GT_BOOL       enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbAgeRefreshEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE / GT_TRUE]
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbAgeRefreshEnableGet with not NULL enablePtr
    Expected: GT_OK and the same enable as was set.
*/

    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL       enable    = GT_FALSE;
    GT_BOOL       enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_FALSE / GT_TRUE]
            Expected: GT_OK.
        */
        /* iterate with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssExMxPmBrgFdbAgeRefreshEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmBrgFdbAgeRefreshEnableGet with not NULL enablePtr
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssExMxPmBrgFdbAgeRefreshEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbAgeRefreshEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);

        /* iterate with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssExMxPmBrgFdbAgeRefreshEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmBrgFdbAgeRefreshEnableGet with not NULL enablePtr
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssExMxPmBrgFdbAgeRefreshEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbAgeRefreshEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbAgeRefreshEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbAgeRefreshEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbAgeRefreshEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbAgeRefreshEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbAgeRefreshEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR
        */
        st = cpssExMxPmBrgFdbAgeRefreshEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbAgeRefreshEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbAgeRefreshEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbEccErrGet
(
    IN  GT_U8                          devNum,
    OUT GT_U32                         *fdbEccErrCntPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbEccErrGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL fdbEccErrPtr and fdbEccErrCntPtr.
    Expected: GT_OK.
    1.2. Call with fdbEccErrPtr [NULL].
    Expected: GT_BAD_PTR
    1.3. Call with fdbEccErrCntPtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                         fdbEccErrCnt;

    fdbEccErrCnt = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL fdbEccErrPtr and fdbEccErrCntPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbEccErrGet(dev, &fdbEccErrCnt);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.3. Call with fdbEccErrCntPtr [NULL].
            Expected: GT_BAD_PTR
        */
        st = cpssExMxPmBrgFdbEccErrGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fdbEccErrCntPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbEccErrGet(dev, &fdbEccErrCnt);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbEccErrGet(dev, &fdbEccErrCnt);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbPwIdTransplantEnableSet
(
    IN GT_U8         devNum,
    IN GT_BOOL       enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbPwIdTransplantEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbPwIdTransplantEnableGet with not NULL enablePtr
    Expected: GT_OK
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL       enable    = GT_FALSE;
    GT_BOOL       enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* iterate with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssExMxPmBrgFdbPwIdTransplantEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmBrgFdbPwIdTransplantEnableGet with not NULL enablePtr
            Expected: GT_OK
        */
        st = cpssExMxPmBrgFdbPwIdTransplantEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbPwIdTransplantEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);

        /* iterate with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssExMxPmBrgFdbPwIdTransplantEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmBrgFdbPwIdTransplantEnableGet with not NULL enablePtr
            Expected: GT_OK
        */
        st = cpssExMxPmBrgFdbPwIdTransplantEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbPwIdTransplantEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbPwIdTransplantEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbPwIdTransplantEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbPwIdTransplantEnableGet
(
    IN GT_U8         devNum,
    OUT GT_BOOL       *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbPwIdTransplantEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enable [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbPwIdTransplantEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enable [NULL].
            Expected: GT_BAD_PTR
        */
        st = cpssExMxPmBrgFdbPwIdTransplantEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbPwIdTransplantEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbPwIdTransplantEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbPortAutoLearnEnableSet
(
    IN GT_U8        devNum,
    IN GT_U8        port,
    IN GT_BOOL      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbPortAutoLearnEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgFdbPortAutoLearnEnableGet with not NULL enablePtr
    Expected: GT_OK
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_BOOL      enable    = GT_FALSE;
    GT_BOOL      enableGet = GT_FALSE;


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
                1.1.1. Call with enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* iterate with enable = GT_FALSE */
            enable = GT_FALSE;

            st = cpssExMxPmBrgFdbPortAutoLearnEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgFdbPortAutoLearnEnableGet with not NULL enablePtr
                Expected: GT_OK
            */
            st = cpssExMxPmBrgFdbPortAutoLearnEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbPortAutoLearnEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable than was set: %d, %d", dev, port);

            /* iterate with enable = GT_TRUE */
            enable = GT_TRUE;

            st = cpssExMxPmBrgFdbPortAutoLearnEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgFdbPortAutoLearnEnableGet with not NULL enablePtr
                Expected: GT_OK
            */
            st = cpssExMxPmBrgFdbPortAutoLearnEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbPortAutoLearnEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable than was set: %d, %d", dev, port);
        }

        /* set correct values*/
        enable = GT_FALSE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgFdbPortAutoLearnEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgFdbPortAutoLearnEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgFdbPortAutoLearnEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_FALSE;
    port   = BRG_FDB_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbPortAutoLearnEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbPortAutoLearnEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbPortAutoLearnEnableGet
(
    IN  GT_U8        dev,
    IN  GT_U8        port,
    OUT GT_BOOL      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbPortAutoLearnEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with not NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_BOOL     enable = GT_FALSE;


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
                1.1.1. Call with not NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgFdbPortAutoLearnEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgFdbPortAutoLearnEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgFdbPortAutoLearnEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgFdbPortAutoLearnEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgFdbPortAutoLearnEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    port = BRG_FDB_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbPortAutoLearnEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbPortAutoLearnEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbPortNaToCpuEnableSet
(
    IN GT_U8        devNum,
    IN GT_U8        port,
    IN GT_BOOL      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbPortNaToCpuEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgFdbPortNaToCpuEnableGet with not NULL enablePtr
    Expected: GT_OK
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_BOOL      enable    = GT_FALSE;
    GT_BOOL      enableGet = GT_FALSE;


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
                1.1.1. Call with enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* iterate with enable = GT_FALSE */
            enable = GT_FALSE;

            st = cpssExMxPmBrgFdbPortNaToCpuEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgFdbPortNaToCpuEnableGet with not NULL enablePtr
                Expected: GT_OK
            */
            st = cpssExMxPmBrgFdbPortNaToCpuEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbPortNaToCpuEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable than was set: %d, %d", dev, port);

            /* iterate with enable = GT_TRUE */
            enable = GT_TRUE;

            st = cpssExMxPmBrgFdbPortNaToCpuEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgFdbPortNaToCpuEnableGet with not NULL enablePtr
                Expected: GT_OK
            */
            st = cpssExMxPmBrgFdbPortNaToCpuEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbPortNaToCpuEnableGet: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet, "got another enable than was set: %d, %d", dev, port);
        }

        /* set correct values*/
        enable = GT_FALSE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgFdbPortNaToCpuEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgFdbPortNaToCpuEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgFdbPortNaToCpuEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_FALSE;
    port   = BRG_FDB_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbPortNaToCpuEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbPortNaToCpuEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbPortNaToCpuEnableGet
(
    IN  GT_U8        devNum,
    IN  GT_U8        port,
    OUT GT_BOOL      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbPortNaToCpuEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with not NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_BOOL     enable = GT_FALSE;


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
                1.1.1. Call with not NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgFdbPortNaToCpuEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgFdbPortNaToCpuEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgFdbPortNaToCpuEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgFdbPortNaToCpuEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgFdbPortNaToCpuEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    port = BRG_FDB_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbPortNaToCpuEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbPortNaToCpuEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbTcamEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbTcamEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbTcamEnableGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbTcamEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbTcamEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbTcamEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbTcamEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbTcamEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbTcamEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbTcamEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbTcamEnableGet: %d", dev);

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
        st = cpssExMxPmBrgFdbTcamEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbTcamEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbTcamEnableGet
(
    IN GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbTcamEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbTcamEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbTcamEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbTcamEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbTcamEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet
(
    IN GT_U8     devNum,
    IN GT_U8     portNum,
    IN CPSS_PACKET_CMD_ENT    command
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with command [CPSS_PACKET_CMD_FORWARD_E /
                              CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                              CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                              CPSS_PACKET_CMD_DROP_HARD_E /
                              CPSS_PACKET_CMD_DROP_SOFT_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet with not NULL commandPtr
    Expected: GT_OK.
    1.1.3. Call with command [CPSS_PACKET_CMD_ROUTE_E /
                              CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E /
                              CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E /
                              CPSS_PACKET_CMD_BRIDGE_E /
                              CPSS_PACKET_CMD_NONE_E] (not supported).
    Expected: NOT GT_OK.
    1.1.4. Call with out of range command [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    CPSS_PACKET_CMD_ENT command    = CPSS_PACKET_CMD_FORWARD_E;
    CPSS_PACKET_CMD_ENT commandGet = CPSS_PACKET_CMD_FORWARD_E;


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
                1.1.1. Call with command [CPSS_PACKET_CMD_FORWARD_E /
                                          CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                                          CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                          CPSS_PACKET_CMD_DROP_HARD_E /
                                          CPSS_PACKET_CMD_DROP_SOFT_E].
                Expected: GT_OK.
            */
            /* iterate with command = CPSS_PACKET_CMD_FORWARD_E */
            command = CPSS_PACKET_CMD_FORWARD_E;

            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, command);

            /*
                1.1.2. Call cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet with not NULL commandPtr
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet(dev, port, &commandGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(command, commandGet,
                       "got another command than was set: %d, %d", dev, port);

            /* iterate with command = CPSS_PACKET_CMD_MIRROR_TO_CPU_E */
            command = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, command);

            /*
                1.1.2. Call cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet with not NULL commandPtr
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet(dev, port, &commandGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(command, commandGet,
                       "got another command than was set: %d, %d", dev, port);

            /* iterate with command = CPSS_PACKET_CMD_TRAP_TO_CPU_E */
            command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, command);

            /*
                1.1.2. Call cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet with not NULL commandPtr
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet(dev, port, &commandGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(command, commandGet,
                       "got another command than was set: %d, %d", dev, port);

            /* iterate with command = CPSS_PACKET_CMD_DROP_HARD_E */
            command = CPSS_PACKET_CMD_DROP_HARD_E;

            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, command);

            /*
                1.1.2. Call cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet with not NULL commandPtr
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet(dev, port, &commandGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(command, commandGet,
                       "got another command than was set: %d, %d", dev, port);

            /* iterate with command = CPSS_PACKET_CMD_DROP_SOFT_E */
            command = CPSS_PACKET_CMD_DROP_SOFT_E;

            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, command);

            /*
                1.1.2. Call cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet with not NULL commandPtr
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet(dev, port, &commandGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(command, commandGet,
                       "got another command than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with command [CPSS_PACKET_CMD_ROUTE_E /
                                          CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E /
                                          CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E /
                                          CPSS_PACKET_CMD_BRIDGE_E /
                                          CPSS_PACKET_CMD_NONE_E] (not supported).
                Expected: NOT GT_OK.
            */
            /* iterate with command = CPSS_PACKET_CMD_ROUTE_E */
            command = CPSS_PACKET_CMD_ROUTE_E;

            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, command);

            /* iterate with command = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E */
            command = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, command);

            /* iterate with command = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E */
            command = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;

            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, command);

            /* iterate with command = CPSS_PACKET_CMD_BRIDGE_E */
            command = CPSS_PACKET_CMD_BRIDGE_E;

            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, command);

            /* iterate with command = CPSS_PACKET_CMD_NONE_E */
            command = CPSS_PACKET_CMD_NONE_E;

            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, command);

            /*
                1.1.4. Call with out of range command [0x5AAAAAA5].
                Expected: GT_BAD_PARAM.
            */
            command = BRG_FDB_INVALID_ENUM_CNS;

            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, command);
        }

        /* set correct values*/
        command = CPSS_PACKET_CMD_FORWARD_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    command = CPSS_PACKET_CMD_FORWARD_E;
    port    = BRG_FDB_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet(dev, port, command);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet
(
    IN GT_U8     devNum,
    IN GT_U8     portNum,
    OUT CPSS_PACKET_CMD_ENT    *commandPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with not NULL commandPtr.
    Expected: GT_OK.
    1.1.2. Call with commandPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    CPSS_PACKET_CMD_ENT command = CPSS_PACKET_CMD_FORWARD_E;


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
                1.1.1. Call with not NULL commandPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet(dev, port, &command);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with commandPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, commandPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet(dev, port, &command);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet(dev, port, &command);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet(dev, port, &command);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    port = BRG_FDB_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet(dev, port, &command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet(dev, port, &command);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbTcamEntryWrite
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    IN GT_BOOL                          skip,
    IN CPSS_EXMXPM_FDB_ENTRY_STC        *fdbEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbTcamEntryWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    Stop LSR
    1.1. Call function with index [0],
                            skip [GT_FALSE]
                            fdbEntryPtr {key {entryType[CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                                    macfId->fId[100],
                                                    macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                               dstOutlif { outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                           interfaceInfo{type[CPSS_INTERFACE_TRUNK_E],
                                                                        trunkId[100],
                                                           outlifPointer{arpPtr[0],
                                               isStatic [GT_FALSE],
                                               daCommand [CPSS_PACKET_CMD_FORWARD_E],
                                               saCommand [CPSS_PACKET_CMD_FORWARD_E],
                                               daRoute [GT_FALSE],
                                               mirrorToRxAnalyzerPortEn [GT_FALSE],
                                               sourceID [0],
                                               userDefined [0],
                                               daQosIndex [0],
                                               saQosIndex [0],
                                               daSecurityLevel [0],
                                               saSecurityLevel [0],
                                               appSpecificCpuCode [GT_FALSE],
                                               pwId[0] }.
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbTcamEntryRead with index [0] and non-NULL pointers.
    Expected: GT_OK and the same parameters as input was - check by fields (only valid).
    1.3. Call function with index [1],
                            skip [GT_FALSE]
                            and valid fdbEntryPtr {key {entryType[ CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E ],
                                                    ipMcast->fId[100],
                                                    ipMcast->sip[00:1A:FF:FF], ipMcast->dip[00:1A:FF:FF]},
                                               dstOutlif { outlifType[ CPSS_EXMXPM_OUTLIF_TYPE_LL_E ],
                                                           interfaceInfo{type[ CPSS_INTERFACE_VIDX_E ],
                                                                        vidx[100],
                                                           outlifPointer{arpPtr[0],
                                               daCommand [ CPSS_PACKET_CMD_DROP_SOFT_E ],
                                               sourceID [ 0],
                                               userDefined [0],
                                               daQosIndex [3],
                                               saQosIndex [5],
                                               daSecurityLevel [1],
                                               saSecurityLevel [4],
                                               appSpecificCpuCode [ GT_TRUE],
                                               pwId[10]
                                               and other params from 1.1.}
    Expected: GT_OK.
    1.4. Call cpssExMxPmBrgFdbTcamEntryRead with index [1] and non-NULL pointers.
    Expected: GT_OK and the same parameters as input. (don't check fdbEntry.saCommand, fdbEntry.sourceId)
    1.5. Call function with index [2],
                            skip [GT_FALSE]
                            and valid fdbEntryPtr {key {entryType[ CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E],
                                                    macfId->fId[100],
                                                    macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                               dstOutlif { outlifType[ CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                           interfaceInfo{type[ CPSS_INTERFACE_VIDX_E],
                                                                        vidx[100]},
                                                           outlifPointer{arpPtr[0],
                                               daCommand [ CPSS_PACKET_CMD_DROP_HARD_E],
                                               and other params from 1.1.}
    Expected: GT_OK.
    1.6. Call cpssExMxPmBrgFdbTcamEntryRead with index [2] and non-NULL pointers.
    Expected: GT_OK and the same parameters as input (don't check macEntry.saCommand, macEntry.sourceId)

    1.7. Call function with out of range index [macTableSize(dev)]
                             and other params from 1.10.
    Expected: NOT GT_OK.
    1.8. Call function with null fdbEntryPtr [NULL].
    Expected: GT_BAD_PTR.
    1.9. Call function with out of range key->entryType [0x5AAAAAA5]
                             and other parameters the same as in 1.10.
    Expected: GT_BAD_PARAM.

    1.10. Call with interfaceInfo.type [0x5AAAAAA5] out of range
                  and other valid params.
    Expected: GT_BAD_PARAM.
[ CPSS_INTERFACE_TRUNK_E ]
    -> out of range
    1.10. Call with interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                    interfaceInfo.trunkId [4096]
                    and other valid params.
    Expected: NOT GT_OK.

   -> not relevant
    1.11. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                    interfaceInfo.trunkId [4096]
                    and other valid params.
    Expected: GT_OK.

[ CPSS_INTERFACE_PORT_E ]
    -> out of range
    1.12. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                    interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other valid params.
    Expected: NOT GT_OK.
    1.13. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                    interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                    and other valid params.
    Expected: NOT GT_OK.

   -> not relevant
    1.14. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                    interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other valid params.
    Expected: NOT GT_OK.
    1.15. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                    interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                    and other valid params.
    Expected: NOT GT_OK.

[ CPSS_INTERFACE_VIDX_E ]
    -> out of range
    1.16. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                    interfaceInfo.vidx [4096]
                    and other valid params.
    Expected: NOT GT_OK.

   -> not relevant
    1.17. Call with interfaceInfo.type [CPSS_INTERFACE_VID_E]
                    interfaceInfo.vidx [4096]
                    and other valid params.
    Expected: GT_OK.

[ CPSS_INTERFACE_DEVICE_E ]
    -> out of range
    1.20. Call with interfaceInfo.type [CPSS_INTERFACE_DEVICE_E]
                    interfaceInfo.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other valid params.
    Expected: NOT GT_OK.

[ CPSS_INTERFACE_FABRIC_VIDX_E ]
    -> out of range
    1.22. Call with interfaceInfo.type [CPSS_INTERFACE_FABRIC_VIDX_E]
                    interfaceInfo.fabricVidx [4096]
                   and other valid params.
    Expected: NOT GT_OK.

    -> not relevant
    1.23. Call with interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                    interfaceInfo.fabricVidx [4096]
                   and other valid params.
    Expected: GT_OK.

    1.24. Call function with out of range fdbEntryPtr->daCommand [0x5AAAAAA5]
                             and other parameters the same as in 1.10.
    Expected: GT_BAD_PARAM.
    1.25. Call function with out of range fdbEntryPtr->saCommand [0x5AAAAAA5]
                             and other parameters the same as in 1.10.
    Expected: GT_BAD_PARAM.
    1.26. Call function with out of range fdbEntryPtr->userDefined [0x10]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.27. Call function with out of range fdbEntryPtr->daQosIndex [0x8]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.28. Call function with out of range fdbEntryPtr->saQosIndex [0x8]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.29. Call function with out of range fdbEntryPtr->daSecurityLevel [0x8]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.30. Call function with out of range fdbEntryPtr->saSecurityLevel [0x8]
                             and other parameters the same as in 1.7.
    Expected: NOT GT_OK.
    1.31. Call cpssExMxPmBrgFdbTcamEntryInvalidate with index [0/ 1/ 2] to invalidate all changes
    Expected: GT_OK.
*/
    GT_STATUS                 st          = GT_OK;
    GT_U8                     dev;

    GT_U32                    index       = 0;
    GT_BOOL                   skip        = GT_FALSE;
    CPSS_EXMXPM_FDB_ENTRY_STC fdbEntry;
    GT_BOOL                   validGet    = GT_FALSE;
    GT_BOOL                   skipGet     = GT_FALSE;
    GT_BOOL                   agedGet     = GT_FALSE;
    GT_BOOL                   status      = GT_FALSE;
    GT_U8                     devNumGet   = 0;
    CPSS_EXMXPM_FDB_ENTRY_STC entryGet;
    GT_BOOL                   isEqual     = GT_FALSE;


    cpssOsBzero((GT_VOID*) &fdbEntry, sizeof(fdbEntry));
    cpssOsBzero((GT_VOID*) &entryGet, sizeof(entryGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Stop LSR */
        st = cpssExMxPmBrgFdbLsrEnableSet(dev, GT_FALSE);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.1. Call function with index [0],
                                    skip [GT_FALSE]
                                    fdbEntryPtr {key {entryType[CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                                            macfId->fId[100],
                                                            macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                                       dstOutlif { outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                   interfaceInfo{type[CPSS_INTERFACE_TRUNK_E],
                                                                                trunkId[100],
                                                                   outlifPointer{arpPtr[0],
                                                       isStatic [GT_FALSE],
                                                       daCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                       saCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                       daRoute [GT_FALSE],
                                                       mirrorToRxAnalyzerPortEn [GT_FALSE],
                                                       sourceID [0],
                                                       userDefined [0],
                                                       daQosIndex [0],
                                                       saQosIndex [0],
                                                       daSecurityLevel [0],
                                                       saSecurityLevel [0],
                                                       appSpecificCpuCode [GT_FALSE],
                                                       pwId[0] }.
            Expected: GT_OK.
        */
        index = 0;
        skip  = GT_FALSE;

        cpssOsBzero((GT_VOID*) &fdbEntry, sizeof(fdbEntry));
        fdbEntry.key.entryType      = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        fdbEntry.key.key.macFid.fId = 100;
        fdbEntry.key.key.macFid.macAddr.arEther[0] = 0x00;
        fdbEntry.key.key.macFid.macAddr.arEther[1] = 0x1A;
        fdbEntry.key.key.macFid.macAddr.arEther[2] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[3] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[4] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[5] = 0xFF;

        fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
        fdbEntry.dstOutlif.interfaceInfo.trunkId = 100;
        fdbEntry.dstOutlif.outlifPointer.arpPtr  = 0;

        fdbEntry.isStatic                 = GT_FALSE;
        fdbEntry.daCommand                = CPSS_PACKET_CMD_FORWARD_E;
        fdbEntry.saCommand                = CPSS_PACKET_CMD_FORWARD_E;
        fdbEntry.daRoute                  = GT_FALSE;
        fdbEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
        fdbEntry.sourceId                 = 0;
        fdbEntry.userDefined              = 0;
        fdbEntry.daQosIndex               = 0;
        fdbEntry.saQosIndex               = 0;
        fdbEntry.daSecurityLevel          = 0;
        fdbEntry.saSecurityLevel          = 0;
        fdbEntry.appSpecificCpuCode       = GT_FALSE;

        fdbEntry.pwId   = 0;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        /*
            1.2. Call cpssExMxPmBrgFdbEntryRead with index [0] and non-NULL pointers.
            Expected: GT_OK and the same parameters as input was - check by fields (only valid).
        */
        index = 0;
        cpssOsBzero((GT_VOID*) &entryGet, sizeof(entryGet));

        st = cpssExMxPmBrgFdbTcamEntryRead(dev, index, &validGet, &skipGet, &agedGet, &devNumGet, &entryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgFdbEntryRead: %d, %d", dev, index);

        /* Verifying struct fields */
        UTF_VERIFY_EQUAL1_STRING_MAC(skip, skipGet,
                   "get another skipPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.key.entryType, entryGet.key.entryType,
                   "get another fdbEntryPtr->key.entryType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.key.key.macFid.fId, entryGet.key.key.macFid.fId,
                   "get another fdbEntryPtr->key.key.macFid.fId than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &fdbEntry.key.key.macFid.macAddr,
                               (GT_VOID*) &entryGet.key.key.macFid.macAddr,
                                sizeof(fdbEntry.key.key.macFid.macAddr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another fdbEntryPtr->key.key.macFid.macAddr than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &fdbEntry.dstOutlif,
                               (GT_VOID*) &entryGet.dstOutlif,
                                sizeof(fdbEntry.dstOutlif))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another fdbEntryPtr->.dstOutlif than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.isStatic, entryGet.isStatic,
                   "get another fdbEntryPtr->isStatic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daCommand, entryGet.daCommand,
                   "get another fdbEntryPtr->daCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.saCommand, entryGet.saCommand,
                   "get another fdbEntryPtr->saCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.mirrorToRxAnalyzerPortEn, entryGet.mirrorToRxAnalyzerPortEn,
                   "get another fdbEntryPtr->mirrorToRxAnalyzerPortEn than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.sourceId, entryGet.sourceId,
                   "get another fdbEntryPtr->sourceId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daQosIndex, entryGet.daQosIndex,
                   "get another fdbEntryPtr->daQosIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daSecurityLevel, entryGet.daSecurityLevel,
                   "get another fdbEntryPtr->daSecurityLevel than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.appSpecificCpuCode, entryGet.appSpecificCpuCode,
                   "get another fdbEntryPtr->appSpecificCpuCode than was set: %d", dev);

        /* waiting till the end of the previous operation */
        status = GT_FALSE;
        while (GT_FALSE == status)
        {
            st = cpssExMxPmBrgFdbTcamStatusGet(dev, &status);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbTcamStatusGet: %d", dev);
        }

        /*
            1.3. Call function with index [1],
                                    skip [GT_FALSE]
                                    and valid fdbEntryPtr {key {entryType[ CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E ],
                                                            macfId->fId[100],
                                                            macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                                       dstOutlif { outlifType[ CPSS_EXMXPM_OUTLIF_TYPE_LL_E ],
                                                                   interfaceInfo{type[ CPSS_INTERFACE_VIDX_E ],
                                                                                vidx[100],
                                                                   outlifPointer{arpPtr[0],
                                                       daCommand [ CPSS_PACKET_CMD_DROP_SOFT_E ],
                                                       sourceID [ 0],
                                                       userDefined [0],
                                                       daQosIndex [3],
                                                       saQosIndex [5],
                                                       daSecurityLevel [1],
                                                       saSecurityLevel [4],
                                                       appSpecificCpuCode [ GT_TRUE],
                                                       pwId[10]
                                                       and other params from 1.1.}
            Expected: GT_OK.
        */
        index = 1;
        skip  = GT_FALSE;

        cpssOsBzero((GT_VOID*) &fdbEntry, sizeof(fdbEntry));
        fdbEntry.key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E;
        fdbEntry.key.key.macFid.fId = 100;
        fdbEntry.key.key.macFid.macAddr.arEther[0] = 0x00;
        fdbEntry.key.key.macFid.macAddr.arEther[1] = 0x1A;
        fdbEntry.key.key.macFid.macAddr.arEther[2] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[3] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[4] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[5] = 0xFF;

        fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        fdbEntry.dstOutlif.interfaceInfo.type   = CPSS_INTERFACE_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.vidx   = 100;
        fdbEntry.dstOutlif.outlifPointer.arpPtr = 0;

        fdbEntry.daCommand                = CPSS_PACKET_CMD_DROP_SOFT_E;
        fdbEntry.sourceId                 = 0;
        fdbEntry.userDefined              = 0;
        fdbEntry.daQosIndex               = 3;
        fdbEntry.saQosIndex               = 5;
        fdbEntry.daSecurityLevel          = 1;
        fdbEntry.saSecurityLevel          = 4;
        fdbEntry.appSpecificCpuCode       = GT_TRUE;

        fdbEntry.pwId = 10;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        /*
            1.4. Call cpssExMxPmBrgFdbEntryRead with index [1] and non-NULL pointers.
            Expected: GT_OK and the same parameters as input. (don't check fdbEntry.saCommand, fdbEntry.sourceId)
        */
        index = 1;
        cpssOsBzero((GT_VOID*) &entryGet, sizeof(entryGet));

        st = cpssExMxPmBrgFdbTcamEntryRead(dev, index, &validGet, &skipGet, &agedGet, &devNumGet, &entryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgFdbEntryRead: %d, %d", dev, index);

        /* Verifying struct fields */
        UTF_VERIFY_EQUAL1_STRING_MAC(skip, skipGet,
                   "get another skipPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.key.entryType, entryGet.key.entryType,
                   "get another fdbEntryPtr->key.entryType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.key.key.macFid.fId, entryGet.key.key.macFid.fId,
                   "get another fdbEntryPtr->key.key.macFid.fId than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &fdbEntry.key.key.macFid.macAddr,
                               (GT_VOID*) &entryGet.key.key.macFid.macAddr,
                                sizeof(fdbEntry.key.key.macFid.macAddr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another fdbEntryPtr->key.key.macFid.macAddr than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &fdbEntry.dstOutlif,
                               (GT_VOID*) &entryGet.dstOutlif,
                                sizeof(fdbEntry.dstOutlif))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another fdbEntryPtr->.dstOutlif than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.isStatic, entryGet.isStatic,
                   "get another fdbEntryPtr->isStatic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daCommand, entryGet.daCommand,
                   "get another fdbEntryPtr->daCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.mirrorToRxAnalyzerPortEn, entryGet.mirrorToRxAnalyzerPortEn,
                   "get another fdbEntryPtr->mirrorToRxAnalyzerPortEn than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daQosIndex, entryGet.daQosIndex,
                   "get another fdbEntryPtr->daQosIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daSecurityLevel, entryGet.daSecurityLevel,
                   "get another fdbEntryPtr->daSecurityLevel than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.appSpecificCpuCode, entryGet.appSpecificCpuCode,
                   "get another fdbEntryPtr->appSpecificCpuCode than was set: %d", dev);

        /* waiting till the end of the previous operation */
        status = GT_FALSE;
        while (GT_FALSE == status)
        {
            st = cpssExMxPmBrgFdbTcamStatusGet(dev, &status);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbTcamStatusGet: %d", dev);
        }

        /*
            1.5. Call function with index [2],
                                    skip [GT_FALSE]
                                    and valid fdbEntryPtr {key {entryType[ CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E],
                                                            macfId->fId[100],
                                                            macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                                       dstOutlif { outlifType[ CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                   interfaceInfo{type[ CPSS_INTERFACE_VIDX_E],
                                                                                vidx[100]},
                                                                   outlifPointer{arpPtr[0]},
                                                       daCommand [ CPSS_PACKET_CMD_DROP_HARD_E],
                                                       saCommand [ CPSS_PACKET_CMD_DROP_HARD_E]
                                                       and other params from 1.1.}
            Expected: GT_OK.
        */
        index = 2;
        skip  = GT_FALSE;

        cpssOsBzero((GT_VOID*) &fdbEntry, sizeof(fdbEntry));
        fdbEntry.key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E;
        fdbEntry.key.key.macFid.fId = 100;
        fdbEntry.key.key.macFid.macAddr.arEther[0] = 0x00;
        fdbEntry.key.key.macFid.macAddr.arEther[1] = 0x1A;
        fdbEntry.key.key.macFid.macAddr.arEther[2] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[3] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[4] = 0xFF;
        fdbEntry.key.key.macFid.macAddr.arEther[5] = 0xFF;

        fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        fdbEntry.dstOutlif.interfaceInfo.type   = CPSS_INTERFACE_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.vidx   = 100;
        fdbEntry.dstOutlif.outlifPointer.arpPtr  = 0;

        fdbEntry.daCommand = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        /*
            1.6. Call cpssExMxPmBrgFdbEntryRead with index [2] and non-NULL pointers.
            Expected: GT_OK and the same parameters as input (don't check macEntry.saCommand, macEntry.sourceId)
        */
        index = 2;
        cpssOsBzero((GT_VOID*) &entryGet, sizeof(entryGet));

        st = cpssExMxPmBrgFdbTcamEntryRead(dev, index, &validGet, &skipGet, &agedGet, &devNumGet, &entryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgFdbEntryRead: %d, %d", dev, index);

        /* Verifying struct fields */
        UTF_VERIFY_EQUAL1_STRING_MAC(skip, skipGet,
                   "get another skipPtr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.key.entryType, entryGet.key.entryType,
                   "get another fdbEntryPtr->key.entryType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.key.key.macFid.fId, entryGet.key.key.macFid.fId,
                   "get another fdbEntryPtr->key.key.macFid.fId than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &fdbEntry.key.key.macFid.macAddr,
                               (GT_VOID*) &entryGet.key.key.macFid.macAddr,
                                sizeof(fdbEntry.key.key.macFid.macAddr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another fdbEntryPtr->key.key.macFid.macAddr than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &fdbEntry.dstOutlif,
                               (GT_VOID*) &entryGet.dstOutlif,
                                sizeof(fdbEntry.dstOutlif))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another fdbEntryPtr->.dstOutlif than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.isStatic, entryGet.isStatic,
                   "get another fdbEntryPtr->isStatic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daCommand, entryGet.daCommand,
                   "get another fdbEntryPtr->daCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.mirrorToRxAnalyzerPortEn, entryGet.mirrorToRxAnalyzerPortEn,
                   "get another fdbEntryPtr->mirrorToRxAnalyzerPortEn than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.userDefined, entryGet.userDefined,
                   "get another fdbEntryPtr->userDefined than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.saQosIndex, entryGet.saQosIndex,
                   "get another fdbEntryPtr->saQosIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.daSecurityLevel, entryGet.daSecurityLevel,
                   "get another fdbEntryPtr->daSecurityLevel than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(fdbEntry.appSpecificCpuCode, entryGet.appSpecificCpuCode,
                   "get another fdbEntryPtr->appSpecificCpuCode than was set: %d", dev);

        /* waiting till the end of the previous operation */
        status = GT_FALSE;
        while (GT_FALSE == status)
        {
            st = cpssExMxPmBrgFdbTcamStatusGet(dev, &status);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbTcamStatusGet: %d", dev);
        }

        /*
            1.7. Call function with out of range index [macTableSize(dev)]
                                     and other params from 1.10.
            Expected: NOT GT_OK.
        */
        st = prvUtfFdbTcamTableSize(dev, &index);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFdbMacTableSize: %d", dev);

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.8. Call function with null fdbEntryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fdbEntryPtr = NULL", dev);

        /*
            1.9. Call function with out of range key->entryType [0x5AAAAAA5]
                                     and other parameters the same as in 1.10.
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.key.entryType = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryPtr->key->entryType = %d",
                                     dev, fdbEntry.key.entryType);

        fdbEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

        /*
            1.10. Call with interfaceInfo.type [0x5AAAAAA5] out of range
                          and other valid params.
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryPtr->dstOutlif.interfaceInfo.type = %d",
                                     dev, fdbEntry.dstOutlif.interfaceInfo.type);

        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;

        /*
        [ CPSS_INTERFACE_TRUNK_E ]
            -> out of range
            1.10. Call with interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                            interfaceInfo.trunkId [4096]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
        fdbEntry.dstOutlif.interfaceInfo.trunkId = BIT_8;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.trunkId = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.trunkId);

        fdbEntry.dstOutlif.interfaceInfo.trunkId = 100;

        /*
           -> not relevant
            1.11. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                            interfaceInfo.trunkId [4096]
                            and other valid params.
            Expected: GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_PORT_E;
        fdbEntry.dstOutlif.interfaceInfo.trunkId = BIT_8;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.trunkId = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.trunkId);

        fdbEntry.dstOutlif.interfaceInfo.trunkId = 100;

        /*
        [ CPSS_INTERFACE_PORT_E ]
            -> out of range
            1.12. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                            interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = 0;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devPort.devNum = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.devPort.devNum);

        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = dev;

        /*
            1.13. Call with interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                            interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = dev;
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devPort.portNum = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.devPort.portNum);

        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = 0;

        /*
           -> not relevant
            1.14. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                            interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = 0;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devPort.devNum = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.devPort.devNum);

        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = dev;

        /*
            1.15. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                            interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.devPort.devNum = dev;
        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devPort.portNum = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.devPort.portNum);

        fdbEntry.dstOutlif.interfaceInfo.devPort.portNum = 0;

        /*
        [ CPSS_INTERFACE_VIDX_E ]
            -> out of range
            1.16. Call with interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                            interfaceInfo.vidx [4096]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.vidx = 4096;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.vidx = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.vidx);

        fdbEntry.dstOutlif.interfaceInfo.vidx = 100;

        /*
           -> not relevant
            1.17. Call with interfaceInfo.type [CPSS_INTERFACE_VID_E]
                            interfaceInfo.vidx [4096]
                            and other valid params.
            Expected: GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_VID_E;
        fdbEntry.dstOutlif.interfaceInfo.vidx = 4096;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.vidx = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.vidx);

        fdbEntry.dstOutlif.interfaceInfo.vidx = 100;

        /*
        [ CPSS_INTERFACE_DEVICE_E ]
            -> out of range
            1.20. Call with interfaceInfo.type [CPSS_INTERFACE_DEVICE_E]
                            interfaceInfo.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_DEVICE_E;
        fdbEntry.dstOutlif.interfaceInfo.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.devNum = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.devNum);

        fdbEntry.dstOutlif.interfaceInfo.devNum = dev;

        /*
        [ CPSS_INTERFACE_FABRIC_VIDX_E ]
            -> out of range
            1.22. Call with interfaceInfo.type [CPSS_INTERFACE_FABRIC_VIDX_E]
                            interfaceInfo.fabricVidx [4096]
                           and other valid params.
            Expected: NOT GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_FABRIC_VIDX_E;
        fdbEntry.dstOutlif.interfaceInfo.fabricVidx = 4096;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.fabricVidx = %d",
                                         dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                         fdbEntry.dstOutlif.interfaceInfo.fabricVidx);

        fdbEntry.dstOutlif.interfaceInfo.fabricVidx = 100;

        /*
            1.23. Call with interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                            interfaceInfo.fabricVidx [4096]
                           and other valid params.
            Expected: GT_OK.
        */
        fdbEntry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        fdbEntry.dstOutlif.interfaceInfo.fabricVidx = 4096;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstOutlif.interfaceInfo.type = %d, ->dstOutlif.interfaceInfo.fabricVidx = %d",
                                     dev,fdbEntry.dstOutlif.interfaceInfo.type,
                                     fdbEntry.dstOutlif.interfaceInfo.fabricVidx);

        fdbEntry.dstOutlif.interfaceInfo.fabricVidx = 100;

        /*
            1.24. Call function with out of range fdbEntryPtr->daCommand [0x5AAAAAA5]
                                     and other parameters the same as in 1.10.
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.daCommand = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryPtr->daCommand = %d",
                                     dev, fdbEntry.daCommand);

        fdbEntry.daCommand = CPSS_MAC_TABLE_INTERV_E;

        /*
            1.25. Call function with out of range fdbEntryPtr->saCommand [0x5AAAAAA5]
                                     and other parameters the same as in 1.10.
            Expected: GT_BAD_PARAM.
        */
        fdbEntry.saCommand = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryPtr->saCommand = %d",
                                     dev, fdbEntry.saCommand);

        fdbEntry.saCommand = CPSS_MAC_TABLE_INTERV_E;

        /*
            1.26. Call function with out of range fdbEntryPtr->userDefined [0x10]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.userDefined = 0x10;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->userDefined = %d",
                                     dev, fdbEntry.userDefined);

        fdbEntry.userDefined = 0x10;

        /*
            1.27. Call function with out of range fdbEntryPtr->daQosIndex [0x8]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.daQosIndex = 0x8;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->daQosIndex = %d",
                                     dev, fdbEntry.daQosIndex);

        fdbEntry.daQosIndex = 0x8;

        /*
            1.28. Call function with out of range fdbEntryPtr->saQosIndex [0x8]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.saQosIndex = 0x8;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->saQosIndex = %d",
                                     dev, fdbEntry.saQosIndex);

        fdbEntry.saQosIndex = 0x8;

        /*
            1.29. Call function with out of range fdbEntryPtr->daSecurityLevel [0x8]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.daSecurityLevel = 0x8;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->daSecurityLevel = %d",
                                     dev, fdbEntry.daSecurityLevel);

        fdbEntry.daSecurityLevel = 0x8;

        /*
            1.30. Call function with out of range fdbEntryPtr->saSecurityLevel [0x8]
                                     and other parameters the same as in 1.7.
            Expected: NOT GT_OK.
        */
        fdbEntry.saSecurityLevel = 0x8;

        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, fdbEntryPtr->saSecurityLevel = %d",
                                     dev, fdbEntry.saSecurityLevel);

        fdbEntry.saSecurityLevel = 0x8;

        /*
            1.31. Call cpssExMxPmBrgFdbTcamEntryInvalidate with index [0/ 1/ 2] to invalidate all changes
            Expected: GT_OK.
        */
        /* Call with index [0] */
        index = 0;
        st = cpssExMxPmBrgFdbTcamEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbEntryInvalidate: %d, %d", dev, index);

        /* Call with index [1] */
        index = 1;

        st = cpssExMxPmBrgFdbTcamEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbEntryInvalidate: %d, %d", dev, index);

        /* Call with index [2] */
        index = 2;

        st = cpssExMxPmBrgFdbTcamEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbEntryInvalidate: %d, %d", dev, index);
    }

    index = 0;
    skip  = GT_FALSE;

    cpssOsBzero((GT_VOID*) &fdbEntry, sizeof(fdbEntry));
    fdbEntry.key.entryType      = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    fdbEntry.key.key.macFid.fId = 100;
    fdbEntry.key.key.macFid.macAddr.arEther[0] = 0x00;
    fdbEntry.key.key.macFid.macAddr.arEther[1] = 0x1A;
    fdbEntry.key.key.macFid.macAddr.arEther[2] = 0xFF;
    fdbEntry.key.key.macFid.macAddr.arEther[3] = 0xFF;
    fdbEntry.key.key.macFid.macAddr.arEther[4] = 0xFF;
    fdbEntry.key.key.macFid.macAddr.arEther[5] = 0xFF;

    fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
    fdbEntry.dstOutlif.interfaceInfo.trunkId = 100;
    fdbEntry.dstOutlif.outlifPointer.arpPtr  = 0;

    fdbEntry.isStatic                 = GT_FALSE;
    fdbEntry.daCommand                = CPSS_PACKET_CMD_FORWARD_E;
    fdbEntry.saCommand                = CPSS_PACKET_CMD_FORWARD_E;
    fdbEntry.daRoute                  = GT_FALSE;
    fdbEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
    fdbEntry.sourceId                 = 0;
    fdbEntry.userDefined              = 0;
    fdbEntry.daQosIndex               = 0;
    fdbEntry.saQosIndex               = 0;
    fdbEntry.daSecurityLevel          = 0;
    fdbEntry.saSecurityLevel          = 0;
    fdbEntry.appSpecificCpuCode       = GT_FALSE;

    fdbEntry.pwId   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbTcamEntryWrite(dev, index, skip, &fdbEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbTcamEntryInvalidate
(
    IN GT_U8                            devNum,
    IN GT_U32                           index
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbTcamEntryInvalidate)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with index [0].
    Expected: GT_OK.
    1.2. Call with out of range index [macTableSize(dev)].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32     index     = 0;
    GT_U32     tableSize = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with index [0].
            Expected: GT_OK.
        */
        index = 0;

        st = cpssExMxPmBrgFdbTcamEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Getting table size */
        st = prvUtfFdbTcamTableSize(dev, &tableSize);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFdbMacTableSize: %d", dev);

        /*
            1.2. Call with out of range index [macTableSize(dev)].
            Expected: NOT GT_OK.
        */
        index = tableSize;

        st = cpssExMxPmBrgFdbTcamEntryInvalidate(dev, index);
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
        st = cpssExMxPmBrgFdbTcamEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbTcamEntryInvalidate(dev, index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbTcamEntryRead
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      index,
    OUT GT_BOOL                     *validPtr,
    OUT GT_BOOL                     *skipPtr,
    OUT GT_BOOL                     *agedPtr,
    OUT GT_U8                       *associatedDevNumPtr,
    OUT CPSS_EXMXPM_FDB_ENTRY_STC   *fdbEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbTcamEntryRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0],
                   non-null validPtr,
                   non-null skipPtr,
                   non-null agedPtr,
                   non-null associatedDevNumPtr
                   and non-null fdbEntryPtr.
    Expected: GT_OK.
    1.2. Call function with out of range index [macTableSize(dev)].
    Expected: NOT GT_OK.
    1.3. Call with index [0],
                   null validPtr [NULL],
                   non-null skipPtr,
                   non-null agedPtr,
                   non-null associatedDevNumPtr
                   and non-null fdbEntryPtr.
    Expected: GT_BAD_PTR.
    1.4. Call with index [0],
                   non-null validPtr,
                   null skipPtr [NULL],
                   non-null agedPtr,
                   non-null associatedDevNumPtr
                   and non-null fdbEntryPtr.
    Expected: GT_BAD_PTR.
    1.5. Call with index [0],
                   non-null validPtr,
                   non-null skipPtr,
                   null agedPtr [NULL],
                   non-null associatedDevNumPtr
                   and non-null fdbEntryPtr.
    Expected: GT_BAD_PTR.
    1.6. Call with index [0],
                   non-null validPtr,
                   non-null skipPtr,
                   non-null agedPtr,
                   null associatedDevNumPtr [NULL]
                   and non-null fdbEntryPtr.
    Expected: GT_BAD_PTR.
    1.7. Call with index [0],
                   non-null validPtr,
                   non-null skipPtr,
                   non-null agedPtr,
                   non-null associatedDevNumPtr
                   and null fdbEntryPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    index     = 0;
    GT_U32                    tableSize = 0;
    GT_BOOL                   valid     = GT_FALSE;
    GT_BOOL                   skip      = GT_FALSE;
    GT_BOOL                   aged      = GT_FALSE;
    GT_U8                     devNum    = 0;
    CPSS_EXMXPM_FDB_ENTRY_STC fdbEntry;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0],
                           non-null validPtr,
                           non-null skipPtr,
                           non-null agedPtr,
                           non-null associatedDevNumPtr
                           and non-null entryPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbTcamEntryRead(dev, index, &valid, &skip,
                                           &aged, &devNum, &fdbEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Getting table size */
        st = prvUtfFdbTcamTableSize(dev, &tableSize);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFdbMacTableSize: %d", dev);

        /*
            1.2. Call function with out of range index [macTableSize(dev)].
            Expected: NOT GT_OK.
        */
        index = tableSize;

        st = cpssExMxPmBrgFdbTcamEntryRead(dev, index, &valid, &skip,
                                           &aged, &devNum, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call with index [0],
                           null validPtr [NULL],
                           non-null skipPtr,
                           non-null agedPtr,
                           non-null associatedDevNumPtr
                           and non-null entryPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbTcamEntryRead(dev, index, NULL, &skip,
                                           &aged, &devNum, &fdbEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);

        /*
            1.4. Call with index [0],
                           non-null validPtr,
                           null skipPtr [NULL],
                           non-null agedPtr,
                           non-null associatedDevNumPtr
                           and non-null entryPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbTcamEntryRead(dev, index, &valid, NULL,
                                           &aged, &devNum, &fdbEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, skipPtr = NULL", dev);

        /*
            1.5. Call with index [0],
                           non-null validPtr,
                           non-null skipPtr,
                           null agedPtr [NULL],
                           non-null associatedDevNumPtr
                           and non-null entryPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbTcamEntryRead(dev, index, &valid, &skip,
                                           NULL, &devNum, &fdbEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, agedPtr = NULL", dev);

        /*
            1.6. Call with index [0],
                           non-null validPtr,
                           non-null skipPtr,
                           non-null agedPtr,
                           null associatedDevNumPtr [NULL]
                           and non-null entryPtr.
            Expected: GT_BAD_PTR.
        */
        index = 0;

        st = cpssExMxPmBrgFdbTcamEntryRead(dev, index, &valid, &skip,
                                           &aged, NULL, &fdbEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, associatedDevNumPtr = NULL", dev);

        /*
            1.7. Call with index [0],
                           non-null validPtr,
                           non-null skipPtr,
                           non-null agedPtr,
                           non-null associatedDevNumPtr
                           and null entryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbTcamEntryRead(dev, index, &valid, &skip,
                                           &aged, &devNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryPtr = NULL", dev);
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
        st = cpssExMxPmBrgFdbTcamEntryRead(dev, index, &valid, &skip,
                                           &aged, &devNum, &fdbEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbTcamEntryRead(dev, index, &valid, &skip,
                                       &aged, &devNum, &fdbEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbTcamStatusGet
(
    IN GT_U8                            devNum,
    OUT GT_BOOL                         *completedPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbTcamStatusGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbTcamStatusGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbTcamStatusGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbTcamStatusGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbTcamStatusGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbTcamMemoryRead
(
    IN GT_U8                         devNum,
    IN GT_U32                        index,
    IN CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_ENT         readFormat,
    OUT CPSS_EXMXPM_BRG_FDB_TCAM_MEMORY_INFO_UNT    *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbTcamMemoryRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0],
                   readFormat [CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E /
                               CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E]
                   non-null entryPtr.
    Expected: GT_OK.
    1.2. Call function with out of range index [macTableSize(dev)].
    Expected: NOT GT_OK.
    1.3. Call with out of range readFormat [0x5AAAAAA5],
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with null entryPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                   index      = 0;
    GT_U32                                   tableSize  = 0;
    CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_ENT     readFormat = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;
    CPSS_EXMXPM_BRG_FDB_TCAM_MEMORY_INFO_UNT entry;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0],
                           readFormat [CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E /
                                       CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E]
                           non-null entryPtr.
            Expected: GT_OK.
        */
        /* iterate with readFormat = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E */
        readFormat = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;

        st = cpssExMxPmBrgFdbTcamMemoryRead(dev, index, readFormat, &entry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, readFormat);

        /* Getting table size */
        st = prvUtfFdbTcamTableSize(dev, &tableSize);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFdbTcamTableSize: %d", dev);

        /* iterate with readFormat = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E */
        readFormat = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_PATTERN_MASK_E;

        st = cpssExMxPmBrgFdbTcamMemoryRead(dev, index, readFormat, &entry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, readFormat);

        /* Getting table size */
        st = prvUtfFdbTcamTableSize(dev, &tableSize);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfFdbTcamTableSize: %d", dev);

        /*
            1.2. Call function with out of range index [macTableSize(dev)].
            Expected: NOT GT_OK.
        */
        index = tableSize;

        st = cpssExMxPmBrgFdbTcamMemoryRead(dev, index, readFormat, &entry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call with out of range readFormat [0x5AAAAAA5],
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        readFormat = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbTcamMemoryRead(dev, index, readFormat, &entry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, index, readFormat);

        readFormat = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;

        /*
            1.4. Call with null entryPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbTcamMemoryRead(dev, index, readFormat, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryPtr = NULL", dev);
    }

    readFormat = CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbTcamMemoryRead(dev, index, readFormat, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbTcamMemoryRead(dev, index, readFormat, &entry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Lookup table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbFillLookupTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in Lookup table.
         Call cpssExMxPmBrgFdbLutEntryWrite with index [0..numEntries - 1],
                                                        lutEntryPtr->{subEntry[].{lutSecHashValue[0x0],
                                                                                  fdbEntryOffset[0],
                                                                                  isValid [GT_TRUE]}}.
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmBrgFdbLutEntryWrite with index [numEntries] and other params from  1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in Lookup table and compare with original.
         Call cpssExMxPmBrgFdbLutEntryRead with the same index and non-null lutEntryPtr.
    Expected: GT_OK and the same lutEntryPtr.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmBrgFdbLutEntryRead with index [numEntries] and other params from  1.4.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_EXMXPM_FDB_LUT_ENTRY_STC lutEntry;
    CPSS_EXMXPM_FDB_LUT_ENTRY_STC lutEntryGet;


    cpssOsBzero((GT_VOID*) &lutEntry, sizeof(lutEntry));
    cpssOsBzero((GT_VOID*) &lutEntryGet, sizeof(lutEntryGet));

    /* Fill the entry for Lookup table */
    lutEntry.subEntry[0].lutSecHashValue = 0;
    lutEntry.subEntry[0].fdbEntryOffset  = 0;
    lutEntry.subEntry[0].isValid         = GT_TRUE;

    lutEntry.subEntry[1].lutSecHashValue = 0;
    lutEntry.subEntry[1].fdbEntryOffset  = 0;
    lutEntry.subEntry[1].isValid         = GT_TRUE;

    lutEntry.subEntry[2].lutSecHashValue = 0;
    lutEntry.subEntry[2].fdbEntryOffset  = 0;
    lutEntry.subEntry[2].isValid         = GT_TRUE;

    lutEntry.subEntry[3].lutSecHashValue = 0;
    lutEntry.subEntry[3].fdbEntryOffset  = 0;
    lutEntry.subEntry[3].isValid         = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = PRV_CPSS_EXMXPM_PP_MAC(dev)->moduleCfg.bridgeCfg.externLutNumOfEntries;

        /* 1.2. Fill all entries in Lookup table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            lutEntry.subEntry[0].fdbEntryOffset = iTemp % 0x7D;
            lutEntry.subEntry[1].fdbEntryOffset = iTemp % 0x7D;
            lutEntry.subEntry[2].fdbEntryOffset = iTemp % 0x7D;
            lutEntry.subEntry[3].fdbEntryOffset = iTemp % 0x7D;

            st = cpssExMxPmBrgFdbLutEntryWrite(dev, iTemp, &lutEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbLutEntryWrite: %d, %d", dev, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmBrgFdbLutEntryWrite(dev, numEntries, &lutEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbLutEntryWrite: %d, %d", dev, iTemp);

        /* 1.4. Read all entries in Lookup table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            lutEntry.subEntry[0].fdbEntryOffset = iTemp % 0x7D;
            lutEntry.subEntry[1].fdbEntryOffset = iTemp % 0x7D;
            lutEntry.subEntry[2].fdbEntryOffset = iTemp % 0x7D;
            lutEntry.subEntry[3].fdbEntryOffset = iTemp % 0x7D;

            st = cpssExMxPmBrgFdbLutEntryRead(dev, iTemp, &lutEntryGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbLutEntryRead: %d, %d", dev, iTemp);

            /* Verifying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &lutEntry,
                                         (GT_VOID*) &lutEntryGet,
                                         sizeof(lutEntry))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another lutEntry than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmBrgFdbLutEntryRead(dev, numEntries, &lutEntryGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbLutEntryRead: %d, %d", dev, iTemp);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Hardware FDB table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbFillHardwareFDBTable)
{
/*
    1. Call osMemStartHeapAllocationCounter set the current value of
                        heap allocated bytes to the allocation counter
    ITERATE_DEVICE (ExMxPm)
    2.1. Get table Size.
    Expected: GT_OK.
    2.2. Fill all entries in Hardware FDB table.
         Call cpssExMxPmBrgFdbEntryWrite with index [0..numEntries - 1],
                                                skip [GT_FALSE]
                                                fdbEntryPtr {key {entryType[CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                                                        macfId->fId[100],
                                                                        macfId->macAddr[00:1A:FF:FF:FF:FF]},
                                                                   dstOutlif { outlifType[CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                               interfaceInfo{type[CPSS_INTERFACE_TRUNK_E],
                                                                                            trunkId[100],
                                                                               outlifPointer{arpPtr[0],
                                                                   isStatic [GT_FALSE],
                                                                   daCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                                   saCommand [CPSS_PACKET_CMD_FORWARD_E],
                                                                   daRoute [GT_FALSE],
                                                                   mirrorToRxAnalyzerPortEn [GT_FALSE],
                                                                   sourceID [0],
                                                                   userDefined [0],
                                                                   daQosIndex [0],
                                                                   saQosIndex [0],
                                                                   daSecurityLevel [0],
                                                                   saSecurityLevel [0],
                                                                   appSpecificCpuCode [GT_FALSE],
                                                                   pwId[0] }.
    Expected: GT_OK.
    2.3. Try to write entry with index out of range.
         Call cpssExMxPmBrgFdbEntryWrite with index [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    2.4. Read all entries in Hardware FDB table and compare with original.
         Call cpssExMxPmBrgFdbEntryRead with index and non-NULL pointers.
    Expected: GT_OK and the same parameters as input was - check by fields (only valid).
    2.5. Try to read entry with index out of range.
         Call cpssExMxPmBrgFdbEntryRead with index [numEntries] and other params from 1.4.
    Expected: NOT GT_OK.
    2.6. Delete all entries in FDB table.
         Call cpssExMxPmBrgFdbEntryInvalidate with index from 1.2.
    Expected: GT_OK
    2.7. Try to delete entry with index out of range.
         Call cpssExMxPmBrgFdbEntryInvalidate with index [numEntries]/
    Expected: NOT GT_OK.
    3. Call osMemGetHeapAllocationCounter to returns the delta of current allocated
                            bytes number and the value of allocation counter.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;
    GT_BOOL     skip       = GT_FALSE;
    GT_BOOL     validGet   = GT_FALSE;
    GT_BOOL     skipGet    = GT_FALSE;
    GT_BOOL     agedGet    = GT_FALSE;
    GT_U8       devNumGet  = 0;
    CPSS_EXMXPM_FDB_ENTRY_STC fdbEntry;
    CPSS_EXMXPM_FDB_ENTRY_STC entryGet;


    cpssOsBzero((GT_VOID*) &fdbEntry, sizeof(fdbEntry));
    cpssOsBzero((GT_VOID*) &entryGet, sizeof(entryGet));

    /* Fill the entry for FDB table */
    skip  = GT_FALSE;

    fdbEntry.key.entryType      = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    fdbEntry.key.key.macFid.fId = 100;
    fdbEntry.key.key.macFid.macAddr.arEther[0] = 0x00;
    fdbEntry.key.key.macFid.macAddr.arEther[1] = 0x1A;
    fdbEntry.key.key.macFid.macAddr.arEther[2] = 0xFF;
    fdbEntry.key.key.macFid.macAddr.arEther[3] = 0xFF;
    fdbEntry.key.key.macFid.macAddr.arEther[4] = 0xFF;
    fdbEntry.key.key.macFid.macAddr.arEther[5] = 0xFF;

    fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    fdbEntry.dstOutlif.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
    fdbEntry.dstOutlif.interfaceInfo.trunkId = 100;
    fdbEntry.dstOutlif.outlifPointer.arpPtr  = 0;

    fdbEntry.isStatic                 = GT_TRUE;
    fdbEntry.daCommand                = CPSS_PACKET_CMD_DROP_SOFT_E;
    fdbEntry.saCommand                = CPSS_PACKET_CMD_FORWARD_E;
    fdbEntry.daRoute                  = GT_FALSE;
    fdbEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
    fdbEntry.sourceId                 = 0;
    fdbEntry.userDefined              = 0;
    fdbEntry.daQosIndex               = 0;
    fdbEntry.saQosIndex               = 0;
    fdbEntry.daSecurityLevel          = 0;
    fdbEntry.saSecurityLevel          = 0;
    fdbEntry.appSpecificCpuCode       = GT_FALSE;
    fdbEntry.spUnknown                = GT_FALSE;

    fdbEntry.pwId   = 0;

    /*
        1. Call osMemStartHeapAllocationCounter set the current value of
                            heap allocated bytes to the allocation counter
    */
    osMemStartHeapAllocationCounter();

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 2. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 2.1. Get table Size */
        numEntries = PRV_CPSS_EXMXPM_PP_MAC(dev)->moduleCfg.bridgeCfg.fdbNumOfEntries;

        /* 2.2. Fill all entries in FDB table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            fdbEntry.dstOutlif.interfaceInfo.trunkId = (GT_U16)(1 + iTemp % 255);

            fdbEntry.userDefined     = iTemp % 15;
            fdbEntry.daQosIndex      = iTemp % 7;
            fdbEntry.saQosIndex      = iTemp % 7;
            fdbEntry.daSecurityLevel = iTemp % 7;
            fdbEntry.saSecurityLevel = iTemp % 7;

            st = cpssExMxPmBrgFdbEntryWrite(dev, iTemp, skip, &fdbEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbEntryWrite: %d, %d, %d", dev, iTemp, skip);
        }

        /* 2.3. Try to write entry with index out of range. */
        st = cpssExMxPmBrgFdbEntryWrite(dev, iTemp, skip, &fdbEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbEntryWrite: %d, %d, %d", dev, iTemp, skip);

        /* 2.4. Read all entries in FDB table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            fdbEntry.dstOutlif.interfaceInfo.trunkId = (GT_U16)(1 + iTemp % 255);

            fdbEntry.userDefined     = iTemp % 15;
            fdbEntry.daQosIndex      = iTemp % 7;
            fdbEntry.saQosIndex      = iTemp % 7;
            fdbEntry.daSecurityLevel = iTemp % 7;
            fdbEntry.saSecurityLevel = iTemp % 7;

            cpssOsBzero((GT_VOID*) &entryGet, sizeof(entryGet));

            st = cpssExMxPmBrgFdbEntryRead(dev, iTemp, &validGet, &skipGet, &agedGet, &devNumGet, &entryGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbEntryRead: %d, %d", dev, iTemp);

            /* Verifying struct fields */
            UTF_VERIFY_EQUAL2_STRING_MAC(skip, skipGet,
                       "get another skipPtr than was set: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(fdbEntry.key.entryType, entryGet.key.entryType,
                       "get another fdbEntryPtr->key.entryType than was set: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(fdbEntry.key.key.macFid.fId, entryGet.key.key.macFid.fId,
                       "get another fdbEntryPtr->key.key.macFid.fId than was set: %d, %d", dev, iTemp);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &fdbEntry.key.key.macFid.macAddr,
                                   (GT_VOID*) &entryGet.key.key.macFid.macAddr,
                                    sizeof(fdbEntry.key.key.macFid.macAddr))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                       "get another fdbEntryPtr->key.key.macFid.macAddr than was set: %d, %d", dev, iTemp);

            UTF_VERIFY_EQUAL2_STRING_MAC(fdbEntry.dstOutlif.outlifType, entryGet.dstOutlif.outlifType,
                       "get another fdbEntryPtr->dstOutlif.outlifType than was set: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(fdbEntry.dstOutlif.interfaceInfo.type, entryGet.dstOutlif.interfaceInfo.type,
                       "get another fdbEntryPtr->dstOutlif.interfaceInfo.type than was set: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(fdbEntry.dstOutlif.interfaceInfo.trunkId, entryGet.dstOutlif.interfaceInfo.trunkId,
                       "get another fdbEntryPtr->dstOutlif.interfaceInfo.trunkId than was set: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(fdbEntry.dstOutlif.outlifPointer.arpPtr, entryGet.dstOutlif.outlifPointer.arpPtr,
                       "get another fdbEntryPtr->dstOutlif.outlifPointer.arpPtr than was set: %d, %d", dev, iTemp);

            UTF_VERIFY_EQUAL2_STRING_MAC(fdbEntry.isStatic, entryGet.isStatic,
                       "get another fdbEntryPtr->isStatic than was set: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(fdbEntry.daCommand, entryGet.daCommand,
                       "get another fdbEntryPtr->daCommand than was set: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(fdbEntry.saCommand, entryGet.saCommand,
                       "get another fdbEntryPtr->saCommand than was set: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(fdbEntry.mirrorToRxAnalyzerPortEn, entryGet.mirrorToRxAnalyzerPortEn,
                       "get another fdbEntryPtr->mirrorToRxAnalyzerPortEn than was set: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(fdbEntry.sourceId, entryGet.sourceId,
                       "get another fdbEntryPtr->sourceId than was set: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(fdbEntry.daQosIndex, entryGet.daQosIndex,
                       "get another fdbEntryPtr->daQosIndex than was set: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(fdbEntry.daSecurityLevel, entryGet.daSecurityLevel,
                       "get another fdbEntryPtr->daSecurityLevel than was set: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(fdbEntry.appSpecificCpuCode, entryGet.appSpecificCpuCode,
                       "get another fdbEntryPtr->appSpecificCpuCode than was set: %d, %d", dev, iTemp);
        }

        /* 2.5. Try to read entry with index out of range. */
        st = cpssExMxPmBrgFdbEntryRead(dev, numEntries, &validGet, &skipGet, &agedGet, &devNumGet, &entryGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbEntryRead: %d, %d", dev, iTemp);

        /* 2.6. Delete all entries in Hardware FDB table. */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmBrgFdbEntryInvalidate(dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbEntryInvalidate: %d, %d", dev, iTemp);
        }

        /* 2.7. Try to delete entry with index out of range. */
        st = cpssExMxPmBrgFdbEntryInvalidate(dev, numEntries);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgFdbEntryInvalidate: %d, %d", dev, iTemp);
    }

    /*
        3. Call osMemGetHeapAllocationCounter to returns the delta of current allocated
                                bytes number and the value of allocation counter.
    */
    iTemp = osMemGetHeapAllocationCounter();
    UTF_VERIFY_EQUAL0_STRING_MAC(0, iTemp, "size of heap changed (heap_bytes_allocated)");
}
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbTriggerAuQueueWa
(
    IN GT_U8  devNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbTriggerAuQueueWa)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function cpssExMxPmBrgFdbTriggerAuQueueWa.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function to set FDB action trigger.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgFdbTriggerAuQueueWa(dev);
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
        st = cpssExMxPmBrgFdbTriggerAuQueueWa(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbTriggerAuQueueWa(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbVplsDevNumEnableGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbVplsDevNumEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
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
        st = cpssExMxPmBrgFdbVplsDevNumEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbVplsDevNumEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgFdbVplsDevNumEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbVplsDevNumEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbVplsDevNumEnableSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbVplsDevNumEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgFdbVplsDevNumEnableGet
    Expected: GT_OK and the same enablePtr.
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

        st = cpssExMxPmBrgFdbVplsDevNumEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbVplsDevNumEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbVplsDevNumEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbVplsDevNumEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgFdbVplsDevNumEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgFdbVplsDevNumEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssExMxPmBrgFdbVplsDevNumEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgFdbVplsDevNumEnableGet: %d", dev);

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
        st = cpssExMxPmBrgFdbVplsDevNumEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    state = GT_TRUE;

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbVplsDevNumEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbQueueFullGet
(
    IN GT_U8    devNum,
    IN CPSS_EXMXPM_FDB_QUEUE_TYPE_ENT  queueType
    OUT GT_BOOL *isFullPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbQueueFullGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with not NULL isFullPtr and queueType
            [CPSS_EXMXPM_FDB_QUEUE_TYPE_AU_E / CPSS_EXMXPM_FDB_QUEUE_TYPE_FU_E].
    Expected: GT_OK.
    1.2 Call function with not NULL isFullPtr and queueType [ BRG_FDB_INVALID_ENUM_CNS ]
            (out of range)
    Expected: GT_BAD_PARAM.
    1.3. Call with null isFullPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL isFull = GT_FALSE;

    CPSS_EXMXPM_FDB_QUEUE_TYPE_ENT queueType = CPSS_EXMXPM_FDB_QUEUE_TYPE_AU_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with queueType [CPSS_EXMXPM_FDB_QUEUE_TYPE_AU_E /
                                               CPSS_EXMXPM_FDB_QUEUE_TYPE_FU_E].
            Expected: GT_OK.
        */

        /* Call with queueType [CPSS_EXMXPM_FDB_QUEUE_TYPE_AU_E] */
        st = cpssExMxPmBrgFdbQueueFullGet(dev, queueType, &isFull);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, isFull);

        /* Call with queueType [CPSS_EXMXPM_FDB_QUEUE_TYPE_FU_E] */
        queueType    = CPSS_EXMXPM_FDB_QUEUE_TYPE_FU_E;

        st = cpssExMxPmBrgFdbQueueFullGet(dev, queueType, &isFull);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, isFull);
        queueType    = CPSS_EXMXPM_FDB_QUEUE_TYPE_AU_E;
        /*
            1.2 Call function with not NULL isFullPtr and queueType [ BRG_FDB_INVALID_ENUM_CNS ]
                    (out of range)
            Expected: GT_BAD_PARAM.
        */
        queueType    = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbQueueFullGet(dev, queueType, &isFull);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, isFull);

        /*
            1.2. Call with null isFull [NULL].
            Expected: GT_BAD_PTR.
        */
            st = cpssExMxPmBrgFdbQueueFullGet(dev, queueType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, isFullPtr = NULL", dev);
    }

    queueType    = CPSS_EXMXPM_FDB_QUEUE_TYPE_AU_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbQueueFullGet(dev, queueType, &isFull);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbQueueFullGet(dev, queueType, &isFull);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmBrgFdb suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmBrgFdb)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbHashModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbHashModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbNumOfEntriesInLookupSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbNumOfEntriesInLookupGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbMacVlanLookupModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbMacVlanLookupModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbExtLookupOffsetSizeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbExtLookupOffsetSizeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbEntrySet)
    /* 1. GM doesn't support the functionality of QA messaging
       2. After sending QA message, GM is stucked when trying to write FDB entry
          cpssExMxPmBrgFdbEntryWrite */
    /* UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbQaSend) */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbEntryDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbEntryInvalidate)
#ifdef FDB_EXTERN_MEMORY_USED
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbLutEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbLutEntryRead)
#endif
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbAaAndTaToCpuSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbAaAndTaToCpuGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbNaToCpuSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbNaToCpuGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbQaUnicastToCpuSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbQaUnicastToCpuGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbQaMulticastToCpuSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbQaMulticastToCpuGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbSpAaMsgToCpuSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbSpAaMsgToCpuGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbSpEntriesDelEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbSpEntriesDelEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbNaMsgOnFailedLearningSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbNaMsgOnFailedLearningGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbFromCpuAuMsgStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbAuMsgBlockGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbSecondaryAuMsgBlockGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbFuMsgBlockGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbDropAuEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbDropAuEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbAuMsgRateLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbAuMsgRateLimitGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbSecureAutoLearnSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbSecureAutoLearnGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbSecureAgingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbSecureAgingGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbDeviceTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbDeviceTableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbActionsEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbActionsEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbActionModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbActionModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbTriggerModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbTriggerModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbAgingTriggerSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbAgingTriggerGet)
#ifndef _WIN32
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbTriggeredActionStart)
#endif
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbAgingTimeoutSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbAgingTimeoutGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbActionActiveFidSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbActionActiveFidGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbActionActiveOutlifSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbActionActiveOutlifGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbActionTransplantOutlifSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbActionTransplantOutlifGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbActionActiveL2VpnSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbActionActiveL2VpnGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbActionTransplantL2VpnSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbActionTransplantL2VpnGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbAgeOutAllDevOnTrunkEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbStaticOfNonExistDevRemoveEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbStaticDelEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbStaticDelEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbStaticTransEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbStaticTransEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbLsrEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbLsrEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbLsrStoppedGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbAgeRefreshEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbAgeRefreshEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbEccErrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbPwIdTransplantEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbPwIdTransplantEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbPortAutoLearnEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbPortAutoLearnEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbPortNaToCpuEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbPortNaToCpuEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbTcamEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbTcamEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbPortUnkUcMacSaCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbPortUnkUcMacSaCmdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbTcamEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbTcamEntryInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbTcamEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbTcamStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbTcamMemoryRead)
    /* Tests for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbFillLookupTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbFillHardwareFDBTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbTriggerAuQueueWa)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbVplsDevNumEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbVplsDevNumEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbQueueFullGet)



UTF_SUIT_END_TESTS_MAC(cpssExMxPmBrgFdb)


/*******************************************************************************
* prvUtfCoreClockGet
*
* DESCRIPTION:
*       This routine returns core clock per device.
*
* INPUTS:
*       dev      -  device id
*
* OUTPUTS:
*       coreClockPtr    -  (pointer to) core clock
*
*       GT_OK           -  Get core clock was OK
*       GT_BAD_PARAM    -  Invalid device id
*       GT_BAD_PTR      -  Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfCoreClockGet
(
    IN  GT_U8       dev,
    OUT GT_U32      *coreClockPtr
)
{
    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(coreClockPtr);

    /* check if dev active and from ExMxPm family */
    PRV_CPSS_EXMXPM_DEV_CHECK_MAC(dev);

    *coreClockPtr = PRV_CPSS_PP_MAC(dev)->coreClock;

    return(GT_OK);
}

/*******************************************************************************
* prvUtfFdbMacTableSize
*
* DESCRIPTION:
*       This routine returns mac table size per device.
*
* INPUTS:
*       dev      -  device id
*
* OUTPUTS:
*       tableSizePtr    - (pointer to) mac table size
*
*       GT_OK           - Get table size was OK
*       GT_BAD_PARAM    - Invalid device id
*       GT_BAD_PTR      - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfFdbMacTableSize
(
    IN  GT_U8       dev,
    OUT GT_U32      *tableSizePtr
)
{
    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(tableSizePtr);

    /* check if dev active and from ExMxPm family */
    PRV_CPSS_EXMXPM_DEV_CHECK_MAC(dev);

    *tableSizePtr = PRV_CPSS_EXMXPM_PP_MAC(dev)->moduleCfg.bridgeCfg.fdbNumOfEntries;

    return(GT_OK);
}

/*******************************************************************************
* prvUtfFdbMacTableSize
*
* DESCRIPTION:
*       This routine returns max fdb TCAM index.
*
* INPUTS:
*       dev      -  device id
*
* OUTPUTS:
*       tableSizePtr    - (pointer to) mac table size
*
*       GT_OK           - Get table size was OK
*       GT_BAD_PARAM    - Invalid device id
*       GT_BAD_PTR      - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfFdbTcamTableSize
(
    IN  GT_U8       dev,
    OUT GT_U32      *tableSizePtr
)
{
    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(tableSizePtr);

    /* check if dev active and from ExMxPm family */
    PRV_CPSS_EXMXPM_DEV_CHECK_MAC(dev);

    *tableSizePtr = PRV_CPSS_EXMXPM_PP_MAC(dev)->moduleCfg.bridgeCfg.fdbTcamNumOfEntries;

    return(GT_OK);
}

#ifdef FDB_EXTERN_MEMORY_USED
/*******************************************************************************
* prvUtfLutTableSize
*
* DESCRIPTION:
*       This routine returns LUT table size per device.
*
* INPUTS:
*       dev      -  device id
*
* OUTPUTS:
*       tableSizePtr    - (pointer to) LUT table size
*
*       GT_OK           - Get table size was OK
*       GT_BAD_PARAM    - Invalid device id
*       GT_BAD_PTR      - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfLutTableSize
(
    IN  GT_U8       dev,
    OUT GT_U32      *tableSizePtr
)
{
    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(tableSizePtr);

    /* check if dev active and from ExMxPm family */
    PRV_CPSS_EXMXPM_DEV_CHECK_MAC(dev);

    *tableSizePtr = PRV_CPSS_EXMXPM_PP_MAC(dev)->moduleCfg.bridgeCfg.externLutNumOfEntries;

    return(GT_OK);
}
#endif
