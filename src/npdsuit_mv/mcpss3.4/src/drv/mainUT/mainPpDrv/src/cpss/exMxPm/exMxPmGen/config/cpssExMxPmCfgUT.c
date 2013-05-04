/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmCfgUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmCfg, that provides
*       CPSS EXMXPM initialization of PPs and shadow data structures, and
*       declarations of global variables.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/exMxPm/exMxPmGen/config/cpssExMxPmCfg.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Invalid enum */
#define CFG_INIT_INVALID_ENUM_CNS    0x5AAAAAA5

extern GT_U32 prvExMxPmTestMemReadWrite;
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmCfgDevEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmCfgDevEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmCfgDevEnableGet with non-NULL enablePtr.
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

        st = cpssExMxPmCfgDevEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmCfgDevEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmCfgDevEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmCfgDevEnableGet: %d", dev);

        /* Verifying frameType */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxPmCfgDevEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmCfgDevEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmCfgDevEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmCfgDevEnableGet: %d", dev);

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
        st = cpssExMxPmCfgDevEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmCfgDevEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmCfgDevEnableGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmCfgDevEnableGet)
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
        st = cpssExMxPmCfgDevEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmCfgDevEnableGet(dev, NULL);
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
        st = cpssExMxPmCfgDevEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmCfgDevEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmCfgTableNumEntriesGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_EXMXPM_CFG_TABLES_ENT  table,
    OUT GT_U32                      *numEntriesPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmCfgTableNumEntriesGet)
{
/*
    ITERATE_DEVICE (EXMXPM)
    1.1. Call with table [CPSS_EXMXPM_CFG_TABLE_VLAN_E /
                          CPSS_EXMXPM_CFG_TABLE_FDB_E /
                          CPSS_EXMXPM_CFG_TABLE_INTERNAL_ACTION_E],
              not NULL numEntriesPtr.
    Expected: GT_OK.
    1.2. Call with table [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
    1.3. Call with numEntriesPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_CFG_TABLES_ENT  table       = CPSS_EXMXPM_CFG_TABLE_VLAN_E;
    GT_U32                      numEntries  = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with table [CPSS_EXMXPM_CFG_TABLE_VLAN_E /
                                  CPSS_EXMXPM_CFG_TABLE_FDB_E /
                                  CPSS_EXMXPM_CFG_TABLE_INTERNAL_ACTION_E], 
                           not NULL numEntriesPtr.
            Expected: GT_OK.
        */
        /* iterate with table = CPSS_EXMXPM_CFG_TABLE_VLAN_E */
        table = CPSS_EXMXPM_CFG_TABLE_VLAN_E;

        st = cpssExMxPmCfgTableNumEntriesGet(dev, table, &numEntries);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, table);

        /* iterate with table = CPSS_EXMXPM_CFG_TABLE_FDB_E */
        table = CPSS_EXMXPM_CFG_TABLE_FDB_E;

        st = cpssExMxPmCfgTableNumEntriesGet(dev, table, &numEntries);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, table);

        /* iterate with table = CPSS_EXMXPM_CFG_TABLE_INTERNAL_ACTION_E */
        table = CPSS_EXMXPM_CFG_TABLE_INTERNAL_ACTION_E;

        st = cpssExMxPmCfgTableNumEntriesGet(dev, table, &numEntries);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, table);

        /*
            1.2. Call with table [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        table = CFG_INIT_INVALID_ENUM_CNS;

        st = cpssExMxPmCfgTableNumEntriesGet(dev, table, &numEntries);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, table);

        table = CPSS_EXMXPM_CFG_TABLE_INTERNAL_ACTION_E;

        /*
            1.3. Call with numEntriesPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmCfgTableNumEntriesGet(dev, table, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, numEntriesPtr = NULL", dev, table);
    }

    table = CPSS_EXMXPM_CFG_TABLE_VLAN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmCfgTableNumEntriesGet(dev, table, &numEntries);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmCfgTableNumEntriesGet(dev, table, &numEntries);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmCfg suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmCfg)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmCfgDevEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmCfgDevEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmCfgTableNumEntriesGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmCfg)

