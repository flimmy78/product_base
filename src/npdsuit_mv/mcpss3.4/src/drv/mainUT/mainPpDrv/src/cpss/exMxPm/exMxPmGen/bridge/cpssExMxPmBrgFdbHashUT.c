/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmBrgFdbHashUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmBrgFdbHash , that provides
*       Hash calculate for MAC address table implementation for ExMxPm.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdbHash.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
#include <gtOs/gtOsTimer.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/*#define UT_FDB_DEBUG*/

#ifdef UT_FDB_DEBUG
#define UT_FDB_DUMP(_x) cpssOsPrintf _x
#else
#define UT_FDB_DUMP(_x)
#endif

/* Invalid enum */
#define BRG_FDB_HASH_INVALID_ENUM_CNS    0x5AAAAAA5

/* Tests use this vlan id for testing VLAN functions */
#define BRG_FDB_HASH_TESTED_VLAN_ID_CNS  100

/* the Fid supports 16 bits */
#define UT_PRV_CPSS_MAX_NUM_FIDS_CNS    BIT_16

static GT_STATUS prvUtTestFdbHashCases(IN GT_U8 dev);

static GT_STATUS prvUtTestFdbHash(IN GT_U8 dev,
                                  IN CPSS_EXMXPM_FDB_ENTRY_KEY_STC *macKeyPtr);

static void prvUtBuildMacAddrKeyFromBitArray(IN GT_U32 bitArray[3],
                                             INOUT CPSS_EXMXPM_FDB_ENTRY_KEY_STC *macKeyPtr);

static GT_STATUS prvUtCheckMacEntry(IN GT_BOOL valid, IN GT_BOOL skip,
                                    IN CPSS_EXMXPM_FDB_ENTRY_STC *entryPtr,
                                    IN CPSS_EXMXPM_FDB_ENTRY_KEY_STC *macKeyPtr);
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbInternalHashCalc
(
    IN  GT_U8                           devNum,
    IN  CPSS_EXMXPM_FDB_ENTRY_KEY_STC   *fdbEntryKeyPtr,
    OUT GT_U32                          *fdbHashIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbInternalHashCalc)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with valid fdbEntryKeyPtr {entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                         macFid->fId[100],
                                         macFid->macAddr[00:1A:FF:FF:FF:FF]}
                   and non-NULL fdbHashIndexPtr.
    Expected: GT_OK.
    1.2. Call with valid fdbEntryKeyPtr {entryType [CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E],
                                         ipMcast->fId[100],
                                         ipMcast->sip[123.23.34.5],
                                         ipMacst [123.45.67.8]}
                   and non-NULL fdbHashIndexPtr.
    Expected: GT_OK.
    1.3. Call with out of range fdbEntryKeyPtr->entryType [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range fdbEntryKeyPtr->key->ipMcast->fId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and other parameters the same as in 1.2.
    Expected: NOT GT_OK.
    1.5. Call with out of range fdbEntryKeyPtr->key->macFid->fId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.6. Call with null fdbEntryKeyPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.7. Call with null fdbHashIndexPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_ENTRY_KEY_STC  fdbEntryKey;
    GT_U32                         fdbHashIndex = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with valid fdbEntryKeyPtr {entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                                 macFid->fId[100],
                                                 macFid->macAddr[00:1A:FF:FF:FF:FF]}
                           and non-NULL fdbHashIndexPtr.
            Expected: GT_OK.
        */
        fdbEntryKey.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E;

        fdbEntryKey.key.macFid.fId = BRG_FDB_HASH_TESTED_VLAN_ID_CNS;

        fdbEntryKey.key.macFid.macAddr.arEther[0] = 0x0;
        fdbEntryKey.key.macFid.macAddr.arEther[1] = 0x1A;
        fdbEntryKey.key.macFid.macAddr.arEther[2] = 0xFF;
        fdbEntryKey.key.macFid.macAddr.arEther[3] = 0xFF;
        fdbEntryKey.key.macFid.macAddr.arEther[4] = 0xFF;
        fdbEntryKey.key.macFid.macAddr.arEther[5] = 0xFF;

        if((PRV_CPSS_EXMXPM_PP_MAC(dev)->moduleCfg.bridgeCfg.fdbMemoryMode)
         == CPSS_EXMXPM_PP_CONFIG_FDB_TABLE_MODE_EXTERNAL_E)
        {
            /* device not use internal memory for FDB */
            continue;
        }

        st = cpssExMxPmBrgFdbInternalHashCalc(dev, &fdbEntryKey, &fdbHashIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with valid fdbEntryKeyPtr {entryType [CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E],
                                                 ipMcast->fId[100],
                                                 ipMcast->sip[123.23.34.5],
                                                 ipMacst [123.45.67.8]}
                           and non-NULL fdbHashIndexPtr.
            Expected: GT_OK.
        */
        fdbEntryKey.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E;

        fdbEntryKey.key.ipMcast.fId = BRG_FDB_HASH_TESTED_VLAN_ID_CNS;

        fdbEntryKey.key.ipMcast.sip[0] = 123;
        fdbEntryKey.key.ipMcast.sip[1] = 23;
        fdbEntryKey.key.ipMcast.sip[2] = 34;
        fdbEntryKey.key.ipMcast.sip[3] = 5;

        fdbEntryKey.key.ipMcast.dip[0] = 123;
        fdbEntryKey.key.ipMcast.dip[1] = 45;
        fdbEntryKey.key.ipMcast.dip[2] = 67;
        fdbEntryKey.key.ipMcast.dip[3] = 8;

        st = cpssExMxPmBrgFdbInternalHashCalc(dev, &fdbEntryKey, &fdbHashIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.3. Call with out of range fdbEntryKeyPtr->entryType [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        if (PRV_CPSS_EXMXPM_PP_MAC(dev)->fdbInfo.hashMode != CPSS_EXMXPM_FDB_HASH_FUNC_LEGACY_E)
        {
            fdbEntryKey.entryType = BRG_FDB_HASH_INVALID_ENUM_CNS;

            st = cpssExMxPmBrgFdbInternalHashCalc(dev, &fdbEntryKey, &fdbHashIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryKeyPtr->entryType = %d",
                                         dev, fdbEntryKey.entryType);

            fdbEntryKey.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
        }

        /*
            1.4. Call with out of range fdbEntryKeyPtr->key->ipMcast->fId [UT_PRV_CPSS_MAX_NUM_FIDS_CNS]
                           and other parameters the same as in 1.2.
            Expected: NOT GT_OK.
        */
        fdbEntryKey.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E;
        fdbEntryKey.key.ipMcast.fId = UT_PRV_CPSS_MAX_NUM_FIDS_CNS;

        st = cpssExMxPmBrgFdbInternalHashCalc(dev, &fdbEntryKey, &fdbHashIndex);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, fdbEntryKeyPtr->entryType = %d, fdbEntryKeyPtr->key->ipMcast->fId = %d",
                                     dev, fdbEntryKey.entryType, fdbEntryKey.key.ipMcast.fId);

        fdbEntryKey.key.ipMcast.fId = 0;

        /*
            1.5. Call with out of range fdbEntryKeyPtr->key->macFid->fId [UT_PRV_CPSS_MAX_NUM_FIDS_CNS]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        fdbEntryKey.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E;
        fdbEntryKey.key.macFid.fId = UT_PRV_CPSS_MAX_NUM_FIDS_CNS;

        st = cpssExMxPmBrgFdbInternalHashCalc(dev, &fdbEntryKey, &fdbHashIndex);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, fdbEntryKeyPtr->entryType = %d, fdbEntryKeyPtr->key->macFid->fId = %d",
                                     dev, fdbEntryKey.entryType, fdbEntryKey.key.macFid.fId);

        fdbEntryKey.key.macFid.fId = 0;

        /*
            1.6. Call with null fdbEntryKeyPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbInternalHashCalc(dev, NULL, &fdbHashIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fdbEntryKeyPtr = NULL", dev);

        /*
            1.7. Call with null fdbHashIndexPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbInternalHashCalc(dev, &fdbEntryKey, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fdbEntryKeyPtr = NULL", dev);
    }

    fdbEntryKey.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E;

    fdbEntryKey.key.macFid.fId = BRG_FDB_HASH_TESTED_VLAN_ID_CNS;

    fdbEntryKey.key.macFid.macAddr.arEther[0] = 0x0;
    fdbEntryKey.key.macFid.macAddr.arEther[1] = 0x1A;
    fdbEntryKey.key.macFid.macAddr.arEther[2] = 0xFF;
    fdbEntryKey.key.macFid.macAddr.arEther[3] = 0xFF;
    fdbEntryKey.key.macFid.macAddr.arEther[4] = 0xFF;
    fdbEntryKey.key.macFid.macAddr.arEther[5] = 0xFF;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbInternalHashCalc(dev, &fdbEntryKey, &fdbHashIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbInternalHashCalc(dev, &fdbEntryKey, &fdbHashIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgFdbExternalHashCalc
(
    IN  GT_U8                           devNum,
    IN  CPSS_EXMXPM_FDB_ENTRY_KEY_STC   *fdbEntryKeyPtr,
    OUT GT_U32                          *lutHashIndexPtr,
    OUT GT_U32                          *lutSecHashValuePtr,
    OUT GT_U32                          *fdbHashIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgFdbExternalHashCalc)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with valid fdbEntryKeyPtr {entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                         macFid->fId[100],
                                         macFid->macAddr[00:1A:FF:FF:FF:FF]}
                   and non-NULL lutHashIndexPtr,
                   non-NULL lutSecHashValuePtr,
                   non-NULL fdbHashIndexPtr.
    Expected: GT_OK.
    1.2. Call with valid fdbEntryKeyPtr {entryType [CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E],
                                         ipMcast->fId[100],
                                         ipMcast->sip[123.23.34.5],
                                         ipMacst [123.45.67.8]}
                   and non-NULL lutHashIndexPtr,
                   non-NULL lutSecHashValuePtr,
                   non-NULL fdbHashIndexPtr.
    Expected: GT_OK.
    1.3. Call with out of range fdbEntryKeyPtr->entryType [0x5AAAAAA5]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range fdbEntryKeyPtr->key->macFid->fId [UT_PRV_CPSS_MAX_NUM_FIDS_CNS]
                   and other parameters the same as in 1.1.
    Expected: GT_OUT_OF_RANGE.
    1.5. Call with out of range fdbEntryKeyPtr->key->ipMcast->fId [UT_PRV_CPSS_MAX_NUM_FIDS_CNS]
                   and other parameters the same as in 1.2.
    Expected: GT_OUT_OF_RANGE.
    1.6. Call with null lutHashIndexPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.7. Call with null lutSecHashValuePtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.8. Call with null fdbHashIndexPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.9. Call with null fdbEntryKeyPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_FDB_ENTRY_KEY_STC   fdbEntryKey;
    GT_U32                          lutHashIndex    = 0;
    GT_U32                          lutSecHashValue = 0;
    GT_U32                          fdbHashIndex    = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with valid fdbEntryKeyPtr {entryType [CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E],
                                                 macFid->fId[100],
                                                 macFid->macAddr[00:1A:FF:FF:FF:FF]}
                           and non-NULL lutHashIndexPtr,
                           non-NULL lutSecHashValuePtr,
                           non-NULL fdbHashIndexPtr.
            Expected: GT_OK.
        */

        if((PRV_CPSS_EXMXPM_PP_MAC(dev)->moduleCfg.bridgeCfg.fdbMemoryMode)
         != CPSS_EXMXPM_PP_CONFIG_FDB_TABLE_MODE_EXTERNAL_E)
        {
            /* device not use external memory for FDB */
            continue;
        }


        fdbEntryKey.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

        fdbEntryKey.key.macFid.fId = BRG_FDB_HASH_TESTED_VLAN_ID_CNS;

        fdbEntryKey.key.macFid.macAddr.arEther[0] = 0x0;
        fdbEntryKey.key.macFid.macAddr.arEther[1] = 0x1A;
        fdbEntryKey.key.macFid.macAddr.arEther[2] = 0xFF;
        fdbEntryKey.key.macFid.macAddr.arEther[3] = 0xFF;
        fdbEntryKey.key.macFid.macAddr.arEther[4] = 0xFF;
        fdbEntryKey.key.macFid.macAddr.arEther[5] = 0xFF;

        st = cpssExMxPmBrgFdbExternalHashCalc(dev, &fdbEntryKey, &lutHashIndex, &lutSecHashValue, &fdbHashIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with valid fdbEntryKeyPtr {entryType [CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E],
                                                 ipMcast->fId[100],
                                                 ipMcast->sip[123.23.34.5],
                                                 ipMacst [123.45.67.8]}
                           and non-NULL lutHashIndexPtr,
                           non-NULL lutSecHashValuePtr,
                           non-NULL fdbHashIndexPtr.
            Expected: GT_OK.
        */
        fdbEntryKey.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;

        fdbEntryKey.key.ipMcast.fId = BRG_FDB_HASH_TESTED_VLAN_ID_CNS;

        fdbEntryKey.key.ipMcast.sip[0] = 123;
        fdbEntryKey.key.ipMcast.sip[1] = 23;
        fdbEntryKey.key.ipMcast.sip[2] = 34;
        fdbEntryKey.key.ipMcast.sip[3] = 5;

        fdbEntryKey.key.ipMcast.dip[0] = 123;
        fdbEntryKey.key.ipMcast.dip[1] = 45;
        fdbEntryKey.key.ipMcast.dip[2] = 67;
        fdbEntryKey.key.ipMcast.dip[3] = 8;

        st = cpssExMxPmBrgFdbExternalHashCalc(dev, &fdbEntryKey, &lutHashIndex, &lutSecHashValue, &fdbHashIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.3. Call with out of range fdbEntryKeyPtr->entryType [0x5AAAAAA5]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        fdbEntryKey.entryType = BRG_FDB_HASH_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgFdbExternalHashCalc(dev, &fdbEntryKey, &lutHashIndex, &lutSecHashValue, &fdbHashIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryKeyPtr->entryType = %d",
                                     dev, fdbEntryKey.entryType);

        fdbEntryKey.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

        /*
            1.4. Call with out of range fdbEntryKeyPtr->key->macFid->fId [UT_PRV_CPSS_MAX_NUM_FIDS_CNS]
                           and other parameters the same as in 1.1.
            Expected: GT_OUT_OF_RANGE.
        */
        fdbEntryKey.key.macFid.fId = UT_PRV_CPSS_MAX_NUM_FIDS_CNS;

        st = cpssExMxPmBrgFdbExternalHashCalc(dev, &fdbEntryKey, &lutHashIndex, &lutSecHashValue, &fdbHashIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryKeyPtr->entryType = %d, fdbEntryKeyPtr->key->macFid->fId = %d",
                                     dev, fdbEntryKey.entryType, fdbEntryKey.key.macFid.fId);

        fdbEntryKey.key.macFid.fId = BRG_FDB_HASH_TESTED_VLAN_ID_CNS;

        /*
            1.5. Call with out of range fdbEntryKeyPtr->key->ipMcast->fId [UT_PRV_CPSS_MAX_NUM_FIDS_CNS]
                           and other parameters the same as in 1.2.
            Expected: GT_OUT_OF_RANGE.
        */
        fdbEntryKey.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
        fdbEntryKey.key.ipMcast.fId = UT_PRV_CPSS_MAX_NUM_FIDS_CNS;

        st = cpssExMxPmBrgFdbExternalHashCalc(dev, &fdbEntryKey, &lutHashIndex, &lutSecHashValue, &fdbHashIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, fdbEntryKeyPtr->entryType = %d, fdbEntryKeyPtr->key->ipMcast->fId = %d",
                                     dev, fdbEntryKey.entryType, fdbEntryKey.key.ipMcast.fId);

        fdbEntryKey.key.ipMcast.fId = BRG_FDB_HASH_TESTED_VLAN_ID_CNS;

        /*
            1.6. Call with null lutHashIndexPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbExternalHashCalc(dev, &fdbEntryKey, NULL, &lutSecHashValue, &fdbHashIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lutHashIndexPtr = NULL", dev);

        /*
            1.7. Call with null lutSecHashValuePtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbExternalHashCalc(dev, &fdbEntryKey, &lutHashIndex, NULL, &fdbHashIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lutSecHashValuePtr = NULL", dev);

        /*
            1.8. Call with null fdbHashIndexPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbExternalHashCalc(dev, &fdbEntryKey, &lutHashIndex, &lutSecHashValue, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fdbHashIndexPtr = NULL", dev);

        /*
            1.9. Call with null fdbEntryKeyPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgFdbExternalHashCalc(dev, NULL, &lutHashIndex, &lutSecHashValue, &fdbHashIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fdbEntryKeyPtr = NULL", dev);
    }

    fdbEntryKey.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;

    fdbEntryKey.key.macFid.fId= BRG_FDB_HASH_TESTED_VLAN_ID_CNS;

    fdbEntryKey.key.macFid.macAddr.arEther[0] = 0x0;
    fdbEntryKey.key.macFid.macAddr.arEther[1] = 0x1A;
    fdbEntryKey.key.macFid.macAddr.arEther[2] = 0xFF;
    fdbEntryKey.key.macFid.macAddr.arEther[3] = 0xFF;
    fdbEntryKey.key.macFid.macAddr.arEther[4] = 0xFF;
    fdbEntryKey.key.macFid.macAddr.arEther[5] = 0xFF;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgFdbExternalHashCalc(dev, &fdbEntryKey, &lutHashIndex, &lutSecHashValue, &fdbHashIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgFdbExternalHashCalc(dev, &fdbEntryKey, &lutHashIndex, &lutSecHashValue, &fdbHashIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*******************************************************************************
* prvUtCheckMacEntry
*
* DESCRIPTION:
*       This routine checks if the entry exists in FDB in the index created
*       by CPSS, if yes check that there a match between entries keys.
*
* INPUTS:
*       valid       -  entry validity
*       skip        -  entry skip bit
*       entry       - mac entry
*       macKeyPtr   - mac entry key
*
* OUTPUTS:
*       macKeyPtr   - mac entry key
*
*       GT_OK           - the test was ok
*       GT_FAIL         - on fail
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtCheckMacEntry
(
    IN GT_BOOL                      valid,
    IN GT_BOOL                      skip,
    IN CPSS_EXMXPM_FDB_ENTRY_STC    *entryPtr,
    IN CPSS_EXMXPM_FDB_ENTRY_KEY_STC *macKeyPtr
)
{

    if (valid == GT_FALSE)
    {
        UT_FDB_DUMP(("prvUtCheckMacEntry: valid == GT_FALSE \n"));
        return GT_FAIL;
    }

    if (skip == GT_TRUE)
    {
        UT_FDB_DUMP(("prvUtCheckMacEntry: skip == GT_TRUE \n"));
        return GT_FAIL;
    }

    if (macKeyPtr->entryType == CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E)
    {
        /* compare MAC Address entries */
        if ((entryPtr->key.key.macFid.macAddr.arEther[0] !=
            macKeyPtr->key.macFid.macAddr.arEther[0]) ||
            (entryPtr->key.key.macFid.macAddr.arEther[1] !=
            macKeyPtr->key.macFid.macAddr.arEther[1]) ||
            (entryPtr->key.key.macFid.macAddr.arEther[2] !=
            macKeyPtr->key.macFid.macAddr.arEther[2]) ||
            (entryPtr->key.key.macFid.macAddr.arEther[3] !=
            macKeyPtr->key.macFid.macAddr.arEther[3]) ||
            (entryPtr->key.key.macFid.macAddr.arEther[4] !=
            macKeyPtr->key.macFid.macAddr.arEther[4]) ||
            (entryPtr->key.key.macFid.macAddr.arEther[5] !=
            macKeyPtr->key.macFid.macAddr.arEther[5]) ||
            (entryPtr->key.key.macFid.fId !=
             macKeyPtr->key.macFid.fId))
        {
            UT_FDB_DUMP(("prvUtCheckMacEntry: no match in macKeyPtr->entryType == CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E \n"
                    "entryPtr->mac[%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x] ,entryPtr->vlan[%d]\n"
                    "macKeyPtr->mac[%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x],macKeyPtr->vlan[%d]\n"
                    ,entryPtr->key.key.macFid.macAddr.arEther[0]
                    ,entryPtr->key.key.macFid.macAddr.arEther[1]
                    ,entryPtr->key.key.macFid.macAddr.arEther[2]
                    ,entryPtr->key.key.macFid.macAddr.arEther[3]
                    ,entryPtr->key.key.macFid.macAddr.arEther[4]
                    ,entryPtr->key.key.macFid.macAddr.arEther[5]
                    ,entryPtr->key.key.macFid.fId
                    ,macKeyPtr->key.macFid.macAddr.arEther[0]
                    ,macKeyPtr->key.macFid.macAddr.arEther[1]
                    ,macKeyPtr->key.macFid.macAddr.arEther[2]
                    ,macKeyPtr->key.macFid.macAddr.arEther[3]
                    ,macKeyPtr->key.macFid.macAddr.arEther[4]
                    ,macKeyPtr->key.macFid.macAddr.arEther[5]
                    ,macKeyPtr->key.macFid.fId
                    ));
            return GT_FAIL;
        }
    }
    else
    {
        /* compare MAC Address entries */
        if ((entryPtr->key.key.ipMcast.sip[0] != macKeyPtr->key.ipMcast.sip[0]) ||
            (entryPtr->key.key.ipMcast.sip[1] != macKeyPtr->key.ipMcast.sip[1]) ||
            (entryPtr->key.key.ipMcast.sip[2] != macKeyPtr->key.ipMcast.sip[2]) ||
            (entryPtr->key.key.ipMcast.sip[3] != macKeyPtr->key.ipMcast.sip[3]) ||
            (entryPtr->key.key.ipMcast.dip[0] != macKeyPtr->key.ipMcast.dip[0]) ||
            (entryPtr->key.key.ipMcast.dip[1] != macKeyPtr->key.ipMcast.dip[1]) ||
            (entryPtr->key.key.ipMcast.dip[2] != macKeyPtr->key.ipMcast.dip[2]) ||
            (entryPtr->key.key.ipMcast.dip[3] != macKeyPtr->key.ipMcast.dip[3]) ||
            (entryPtr->key.key.ipMcast.fId != macKeyPtr->key.ipMcast.fId))
        {
            UT_FDB_DUMP(("prvUtCheckMacEntry: no match in macKeyPtr->entryType == CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E \n"
                    "entryPtr->sip[%2.2x:%2.2x:%2.2x:%2.2x] ,entryPtr->sip[%2.2x:%2.2x:%2.2x:%2.2x] ,entryPtr->vlan[%d]\n"
                    "macKeyPtr->sip[%2.2x:%2.2x:%2.2x:%2.2x],macKeyPtr->sip[%2.2x:%2.2x:%2.2x:%2.2x],macKeyPtr->vlan[%d]\n"
                    ,entryPtr->key.key.ipMcast.sip[0]
                    ,entryPtr->key.key.ipMcast.sip[1]
                    ,entryPtr->key.key.ipMcast.sip[2]
                    ,entryPtr->key.key.ipMcast.sip[3]
                    ,entryPtr->key.key.ipMcast.dip[0]
                    ,entryPtr->key.key.ipMcast.dip[1]
                    ,entryPtr->key.key.ipMcast.dip[2]
                    ,entryPtr->key.key.ipMcast.dip[3]
                    ,entryPtr->key.key.ipMcast.fId
                    ,macKeyPtr->key.ipMcast.sip[0]
                    ,macKeyPtr->key.ipMcast.sip[1]
                    ,macKeyPtr->key.ipMcast.sip[2]
                    ,macKeyPtr->key.ipMcast.sip[3]
                    ,macKeyPtr->key.ipMcast.dip[0]
                    ,macKeyPtr->key.ipMcast.dip[1]
                    ,macKeyPtr->key.ipMcast.dip[2]
                    ,macKeyPtr->key.ipMcast.dip[3]
                    ,macKeyPtr->key.ipMcast.fId
                    ));
            return GT_FAIL;
        }
    }

    return GT_OK;
}
/*******************************************************************************
* prvUtBuildMacAddrKeyFromBitArray
*
* DESCRIPTION:
*       This routine build MAC entry key from bit array, according to entry type.
*
* INPUTS:
*       dev      -  device id
*       bitArray - bit array
*       macKeyPtr   - mac entry key (entry type)
*
* OUTPUTS:
*       macKeyPtr   - mac entry key
*
* COMMENTS:
*       None.
*******************************************************************************/
static void prvUtBuildMacAddrKeyFromBitArray
(
    IN  GT_U32                              bitArray[3],
    INOUT CPSS_EXMXPM_FDB_ENTRY_KEY_STC    *macKeyPtr
)
{
    if (macKeyPtr->entryType == CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E)
    {
        /* build MAC Address */
        macKeyPtr->key.macFid.macAddr.arEther[0] = (GT_U8)(bitArray[0] & 0xFF);
        macKeyPtr->key.macFid.macAddr.arEther[1] = (GT_U8)((bitArray[0] >> 8) & 0xFF);
        macKeyPtr->key.macFid.macAddr.arEther[2] = (GT_U8)((bitArray[0] >> 16) & 0xFF);
        macKeyPtr->key.macFid.macAddr.arEther[3] = (GT_U8)((bitArray[0] >> 24) & 0xFF);
        macKeyPtr->key.macFid.macAddr.arEther[4] = (GT_U8)(bitArray[1] & 0xFF);
        macKeyPtr->key.macFid.macAddr.arEther[5] = (GT_U8)((bitArray[1] >> 8) & 0xFF);
        /* Forwarding ID */
        macKeyPtr->key.macFid.fId = (GT_U16)((bitArray[1] >> 16) & 0xFFFF);
    }
    else
    {
        /* build IP Address*/
        /* Source IP */
        macKeyPtr->key.ipMcast.sip[0] = (GT_U8)(bitArray[0] & 0xFF);
        macKeyPtr->key.ipMcast.sip[1] = (GT_U8)((bitArray[0] >> 8) & 0xFF);
        macKeyPtr->key.ipMcast.sip[2] = (GT_U8)((bitArray[0] >> 16) & 0xFF);
        macKeyPtr->key.ipMcast.sip[3] = (GT_U8)((bitArray[0] >> 24) & 0xFF);
        /* Destination IP */
        macKeyPtr->key.ipMcast.dip[0] = (GT_U8)(bitArray[1] & 0xFF);
        macKeyPtr->key.ipMcast.dip[1] = (GT_U8)((bitArray[1] >> 8) & 0xFF);
        macKeyPtr->key.ipMcast.dip[2] = (GT_U8)((bitArray[1] >> 16) & 0xFF);
        macKeyPtr->key.ipMcast.dip[3] = (GT_U8)((bitArray[1] >> 24) & 0xFF);
        /* Forwarding ID */
        macKeyPtr->key.ipMcast.fId = (GT_U16)(bitArray[2] & 0xFFFF);
    }
}

/*******************************************************************************
* prvUtTestFdbHash
*
* DESCRIPTION:
*       This routine test CPSS FDB Hash function, according to entry type.
*
* INPUTS:
*       dev      -  device id
*       macKeyPtr   - pointer to mac entry key (entry type)
*
* OUTPUTS:
*       GT_OK           - the test was ok
*       GT_BAD_PARAM    - Invalid device id
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtTestFdbHash
(
    IN GT_U8 dev,
    IN CPSS_EXMXPM_FDB_ENTRY_KEY_STC *macKeyPtr
)
{
    GT_U32                      hashIndex;
    GT_BOOL                     valid;
    GT_BOOL                     skip;
    GT_BOOL                     aged;
    GT_BOOL                     completed, succeeded;
    GT_U8                       associatedDevNum;
    CPSS_EXMXPM_FDB_ENTRY_STC   entry;
    GT_U32                      bitArray[3];
    GT_U32                      deltaWord0 = 0x10624d;
    GT_U32                      deltaWord1 = 0x10624;
    GT_U32                      deltaWord2 = 0x1;
    GT_U32                      i;
    GT_STATUS   st;
    GT_U32                  tableLength = _32K;

    if((PRV_CPSS_EXMXPM_PP_MAC(dev)->moduleCfg.bridgeCfg.fdbMemoryMode)
     == CPSS_EXMXPM_PP_CONFIG_FDB_TABLE_MODE_EXTERNAL_E)
    {
        /* device not use internal memory for FDB */
        return GT_OK;
    }

    /* initialize bit array */
    bitArray[0] = 0;
    bitArray[1] = 0;
    bitArray[2] = 0;

    /* initialize mac entry */
    cpssOsMemSet(&entry,0,sizeof(CPSS_EXMXPM_FDB_ENTRY_STC));

    /* build MAC Address from bit array */
    for (i = 0; i < tableLength; i++)
    {
        prvUtBuildMacAddrKeyFromBitArray(bitArray, macKeyPtr);

        /* calculate the entry hash index by CPSS hash algorithm */
        st = cpssExMxPmBrgFdbInternalHashCalc(dev, macKeyPtr, &hashIndex);
        if (st != GT_OK)
        {
            UT_FDB_DUMP(("ERROR! cpssExMxPmBrgFdbInternalHashCalc: entry index = %d\n", i));
            return st;
        }

        entry.key = *macKeyPtr;

        /* Set destination interface type to be VID */
        entry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_VID_E;

        /* add the entry to FDB table */
        st = cpssExMxPmBrgFdbEntrySet(dev, &entry);
        if (st != GT_OK)
        {
            UT_FDB_DUMP(("ERROR! cpssExMxPmBrgFdbEntrySet: entry index = %d\n", i));
            return st;
        }

        /* check that PP finished AU processing */
        do
        {
            /* Check that the AU message processing has completed */
            st = cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(dev, CPSS_NA_E,
                                                       &completed, &succeeded);
            if (st != GT_OK)
            {
                UT_FDB_DUMP(("ERROR! cpssExMxPmBrgFdbFromCpuAuMsgStatusGet: entry index = %d\n", i));
                return st;
            }
        }while (completed != GT_TRUE);


        /* Read the entry by previously calculated hash index */
        st = cpssExMxPmBrgFdbEntryRead(dev, hashIndex, &valid, &skip, &aged,
                                        &associatedDevNum, &entry);
        if (st != GT_OK)
        {
            UT_FDB_DUMP(("ERROR! cpssExMxPmBrgFdbEntryRead: entry index = %d\n", i));
            return st;
        }

        /* check if the entry exists, if yes check that there is match between the keys */
        st = prvUtCheckMacEntry(valid, skip, &entry, macKeyPtr);
        if (st != GT_OK)
        {
            UT_FDB_DUMP(("prvUtCheckMacEntry: check fail i %d hashIndex %d \n",
                         i, hashIndex));

#ifdef UT_FDB_DEBUG
            {
                /* look for the entry in the table */
                GT_U32  jj;
                GT_STATUS   rc;

                for (jj=0;jj<tableLength;jj++)
                {
                    /* Read by index the FDB */
                    rc = cpssExMxPmBrgFdbEntryRead(dev, jj, &valid, &skip, &aged,
                                                    &associatedDevNum, &entry);
                    if(rc == GT_OK && valid == GT_TRUE && skip == GT_FALSE)
                    {
                        rc = prvUtCheckMacEntry(valid, skip, &entry, macKeyPtr);
                        if(rc == GT_OK)
                        {
                            /*we found the entry */
                            UT_FDB_DUMP(("prvUtTestFdbHash: the entry FOUND in the FDB in index[%d] in PP \n",jj));
                            break;
                        }
                    }
                }

                if(jj == tableLength)
                {
                    UT_FDB_DUMP(("prvUtTestFdbHash: the entry not found in the FDB in PP \n"));
                }
            }
#endif /*UT_FDB_DEBUG*/

            return st;
        }

        /* remove the entry from FDB */
        st = cpssExMxPmBrgFdbEntryInvalidate(dev, hashIndex);
        if (st != GT_OK)
        {
            UT_FDB_DUMP(("ERROR! cpssExMxPmBrgFdbEntryInvalidate: entry index = %d\n", i));
            return st;
        }

        /* increase bit array */
        bitArray[0] += deltaWord0;
        bitArray[1] += deltaWord1;
        bitArray[2] += deltaWord2;

        #ifdef ASIC_SIMULATION
        /* test take about 90 minutes without this skipping */
            i += 128;
        #endif
    }

    return GT_OK;
}

/*******************************************************************************
* prvUtTestFdbHashCases
*
* DESCRIPTION:
*       This routine test all cases of CPSS FDB Hash function, according
*       to entry type.
*
* INPUTS:
*       dev      -  device id
*       macKeyPtr   - pointer to mac entry key (entry type)
*
* OUTPUTS:
*       GT_OK           - the test was ok
*       GT_BAD_PARAM    - Invalid device id
*       GT_BAD_PTR      - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtTestFdbHashCases
(
    IN GT_U8 dev
)
{
    GT_STATUS   st;
    CPSS_MAC_ENTRY_EXT_TYPE_ENT         entryType;
    CPSS_EXMXPM_FDB_HASH_FUNC_MODE_ENT hashMode;
    CPSS_MAC_VL_ENT macVlanMode;
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC   macKey;
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC *macKeyPtr = &macKey;
#ifdef UT_FDB_DEBUG
    char*       entryTypeStr[2]=
        {"MAC","IPV4"};
    char*       hashModeStr[2]=
        {"CRC","legacy"};
    char*       macVlanModeStr[2]=
        {"IVL","SVL"};
#endif /*UT_FDB_DEBUG*/

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(macKeyPtr);

    /* check if dev active and from Puma family */
    PRV_CPSS_EXMXPM_DEV_CHECK_MAC(dev);

    for(entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E ;
        entryType <= CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E ;
        entryType++)
    {
        macKeyPtr->entryType = entryType;

        for(hashMode = CPSS_EXMXPM_FDB_HASH_FUNC_CRC_E ;
            hashMode <= CPSS_EXMXPM_FDB_HASH_FUNC_LEGACY_E;
            hashMode++)
        {

            if(hashMode == CPSS_EXMXPM_FDB_HASH_FUNC_LEGACY_E)
            {
#ifdef ASIC_SIMULATION
                /*the GM has bug calculating the hash index in 'Legacy' mode */
                utfFailureMsgLog("GM has bug calculating the hash index in 'Legacy' mode - see CQ# 104991 \n",NULL,0);
                continue;
#endif /*!ASIC_SIMULATION*/
            }

            /* set hash mode */
            st = cpssExMxPmBrgFdbHashModeSet(dev, hashMode);
            if(st != GT_OK)
            {
                return st;
            }

            for(macVlanMode = CPSS_IVL_E ; macVlanMode <= CPSS_SVL_E ; macVlanMode++)
            {
                /* set vlan mode */
                st = cpssExMxPmBrgFdbMacVlanLookupModeSet(dev, CPSS_SVL_E);
                if(st != GT_OK)
                {
                    return st;
                }
                /* test FDB Hash for entry mode , hash mode , vlan mode */
                UT_FDB_DUMP(("prvUtTestFdbHashCases: start [%s] [%s] [%s] check\n",
                        entryTypeStr[entryType],
                        hashModeStr[hashMode],
                        macVlanModeStr[macVlanMode]));

                st = prvUtTestFdbHash(dev, macKeyPtr);
                if(st != GT_OK)
                {
                    return st;
                }
            }
        }
    }

    return GT_OK;
}


UTF_TEST_CASE_MAC(prvCpssExMxPmTestFdbHash)
{
    GT_STATUS   st;
    GT_U8       dev = 0;

    st = prvUtTestFdbHashCases(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}


/*******************************************************************************
* prvUtTestFdbHashExt
*
* DESCRIPTION:
*       This routine test CPSS FDB Hash function, according to entry type.
*
* INPUTS:
*       dev         - device id
*       macKeyPtr   - pointer to mac entry key (entry type)
*
* OUTPUTS:
*       GT_OK           - the test was ok
*       GT_BAD_PARAM    - invalid device id
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtTestFdbHashExt
(
    IN GT_U8 dev,
    IN CPSS_EXMXPM_FDB_ENTRY_KEY_STC *macKeyPtr
)
{
    GT_U32                         hashIndex;
    GT_U32                         lutHashIndex;
    GT_U32                         lutSecHashValue;
    GT_BOOL                        valid;
    GT_BOOL                        skip;
    GT_BOOL                        aged;
    GT_BOOL                        completed, succeeded;
    GT_U8                          associatedDevNum;
    CPSS_EXMXPM_FDB_ENTRY_STC      entry;
    GT_U32                         bitArray[3];
    GT_U32                         deltaWord0 = 0x10624d;
    GT_U32                         deltaWord1 = 0x10624;
    GT_U32                         deltaWord2 = 0x1;
    GT_U32                         i;
    GT_STATUS                      st;
    GT_U32                         tableLength = _32K;
    CPSS_EXMXPM_FDB_LUT_ENTRY_STC  lutEntry; 
    GT_U8                          lutSubCount;

    if((PRV_CPSS_EXMXPM_PP_MAC(dev)->moduleCfg.bridgeCfg.fdbMemoryMode)
     != CPSS_EXMXPM_PP_CONFIG_FDB_TABLE_MODE_EXTERNAL_E)
    {
        /* device not use external memory for FDB */
        return GT_BAD_STATE;                                                                            
    }

    /* initialize bit array */
    bitArray[0] = 0;
    bitArray[1] = 0;
    bitArray[2] = 0;

    /* initialize mac entry */
    cpssOsMemSet(&entry,0,sizeof(CPSS_EXMXPM_FDB_ENTRY_STC));

    /* build MAC Address from bit array */
    for (i = 0; i < tableLength; i++)
    {
        prvUtBuildMacAddrKeyFromBitArray(bitArray, macKeyPtr);

        /* calculate the entry hash index by CPSS hash algorithm */
        st = cpssExMxPmBrgFdbExternalHashCalc(dev, macKeyPtr, &lutHashIndex, &lutSecHashValue, &hashIndex);

        if (st != GT_OK)
        {
            UT_FDB_DUMP(("ERROR! cpssExMxPmBrgFdbExternalHashCalc: entry index = %d\n", i));
            return st;
        }

        entry.key = *macKeyPtr;

        /* Set destination interface type to be VID */
        entry.dstOutlif.interfaceInfo.type = CPSS_INTERFACE_VID_E;

        /* add the entry to FDB table */
        st = cpssExMxPmBrgFdbEntrySet(dev, &entry);
        if (st != GT_OK)
        {
            UT_FDB_DUMP(("ERROR! cpssExMxPmBrgFdbEntrySet: entry index = %d\n", i));
            return st;
        }

        /* check that PP finished AU processing */
        do
        {
            /* Check that the AU message processing has completed */
            st = cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(dev, CPSS_NA_E,
                                                       &completed, &succeeded);
            if (st != GT_OK)
            {
                UT_FDB_DUMP(("ERROR! cpssExMxPmBrgFdbFromCpuAuMsgStatusGet: entry index = %d\n", i));
                return st;
            }
        }while (completed != GT_TRUE);
        
        /* Read the LUT table */
        st = cpssExMxPmBrgFdbLutEntryRead(dev, lutHashIndex, &lutEntry); 
        if (st != GT_OK)
        {
            UT_FDB_DUMP(("ERROR!  cpssExMxPmBrgFdbLutEntryRead: entry index = %d\n", i));
            return st;
        }

        /* Checking all sub entries */
        for (lutSubCount = 0; lutSubCount < CPSS_EXMXPM_FDB_LUT_SUB_ENTRIES_NUM_CNS; lutSubCount++)
        {
            if(lutEntry.subEntry[lutSubCount].isValid  == GT_TRUE)
            {
               if (lutEntry.subEntry[lutSubCount].lutSecHashValue == lutSecHashValue)
               {
                   break;
               }

               UT_FDB_DUMP(("ERROR! wrong lutSecHashValue: lutSubCount = %d\n", lutSubCount));
               return GT_FAIL;
            }
        }

        /* If there are null sub entries, than return GT_FAIL */
        if (lutSubCount== CPSS_EXMXPM_FDB_LUT_SUB_ENTRIES_NUM_CNS)
        {
            UT_FDB_DUMP("ERROR! cpssExMxPmBrgFdbLutEntryRead: lutSubCount = 0\n");
            return GT_FAIL;
        }

        /* Read the entry by previously calculated hash index */
        st = cpssExMxPmBrgFdbEntryRead(dev, hashIndex, &valid, &skip, &aged,
                                        &associatedDevNum, &entry);
        if (st != GT_OK)
        {
            UT_FDB_DUMP(("ERROR! cpssExMxPmBrgFdbEntryRead: entry index = %d\n", i));
            return st;
        }

        /* check if the entry exists, if yes check that there is match between the keys */
        st = prvUtCheckMacEntry(valid, skip, &entry, macKeyPtr);
        if (st != GT_OK)
        {
            UT_FDB_DUMP(("prvUtCheckMacEntry: check fail i %d hashIndex %d \n",
                         i, hashIndex));

#ifdef UT_FDB_DEBUG
            {
                /* look for the entry in the table */
                GT_U32  jj;
                GT_STATUS   rc;

                for (jj=0;jj<tableLength;jj++)
                {
                    /* Read by index the FDB */
                    rc = cpssExMxPmBrgFdbEntryRead(dev, jj, &valid, &skip, &aged,
                                                    &associatedDevNum, &entry);
                    if(rc == GT_OK && valid == GT_TRUE && skip == GT_FALSE)
                    {
                        rc = prvUtCheckMacEntry(valid, skip, &entry, macKeyPtr);
                        if(rc == GT_OK)
                        {
                            /*we found the entry */
                            UT_FDB_DUMP(("prvUtTestFdbHashExt: the entry FOUND in the FDB in index[%d] in PP \n",jj));
                            break;
                        }
                    }
                }

                if(jj == tableLength)
                {
                    UT_FDB_DUMP(("prvUtTestFdbHashExt: the entry not found in the FDB in PP \n"));
                }
            }
#endif /*UT_FDB_DEBUG*/

            return st;
        }

        /* remove the entry from FDB */
        st = cpssExMxPmBrgFdbEntryInvalidate(dev, hashIndex);
        if (st != GT_OK)
        {
            UT_FDB_DUMP(("ERROR! cpssExMxPmBrgFdbEntryInvalidate: entry index = %d\n", i));
            return st;
        }

        /* invalidate lut entry */

        for (lutSubCount = 0; lutSubCount < CPSS_EXMXPM_FDB_LUT_SUB_ENTRIES_NUM_CNS; lutSubCount++)
        {
            lutEntry.subEntry[lutSubCount].isValid = GT_FALSE;
        }
        cpssExMxPmBrgFdbLutEntryWrite(dev, lutHashIndex, &lutEntry); 

        /* increase bit array */
        bitArray[0] += deltaWord0;
        bitArray[1] += deltaWord1;
        bitArray[2] += deltaWord2;

        #ifdef ASIC_SIMULATION
        /* test take about 90 minutes without this skipping */
            i += 16;
        #endif
    }

    return GT_OK;
}

/*******************************************************************************
* prvUtTestFdbHashCasesExt
*
* DESCRIPTION:
*       This routine test all cases of CPSS FDB Hash function, according
*       to entry type.
*
* INPUTS:
*       dev      -  device id
*       macKeyPtr   - pointer to mac entry key (entry type)
*
* OUTPUTS:
*       GT_OK           - the test was ok
*       GT_BAD_PARAM    - Invalid device id
*       GT_BAD_PTR      - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtTestFdbHashCasesExt
(
    IN GT_U8 dev
)
{
    GT_STATUS   st;
    CPSS_MAC_ENTRY_EXT_TYPE_ENT         entryType;
    CPSS_EXMXPM_FDB_HASH_FUNC_MODE_ENT  hashMode;
    CPSS_MAC_VL_ENT macVlanMode;
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC   macKey;
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC  *macKeyPtr = &macKey;
#ifdef UT_FDB_DEBUG
    char*       entryTypeStr[2]=
        {"MAC","IPV4"};
    char*       hashModeStr[2]=
        {"CRC","legacy"};
    char*       macVlanModeStr[2]=
        {"IVL","SVL"};
#endif /*UT_FDB_DEBUG*/

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(macKeyPtr);

    /* check if dev active and from Puma family */
    PRV_CPSS_EXMXPM_DEV_CHECK_MAC(dev);

    for(entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E ;
        entryType <= CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E ;
        entryType++)
    {
        macKeyPtr->entryType = entryType;

        /* set hash mode */
        hashMode = CPSS_EXMXPM_FDB_HASH_FUNC_CRC_E;
        st = cpssExMxPmBrgFdbHashModeSet(dev, hashMode);
        if(st != GT_OK)
        {
            return st;
        }

        for(macVlanMode = CPSS_IVL_E ; macVlanMode <= CPSS_SVL_E ; macVlanMode++)
        {
            /* set vlan mode */
            st = cpssExMxPmBrgFdbMacVlanLookupModeSet(dev, CPSS_SVL_E);
            if(st != GT_OK)
            {
                return st;
            }
            /* test FDB Hash for entry mode , hash mode , vlan mode */
            UT_FDB_DUMP(("prvUtTestFdbHashCasesExt: start [%s] [%s] [%s] check\n",
                    entryTypeStr[entryType],
                    hashModeStr[hashMode],
                    macVlanModeStr[macVlanMode]));

            st = prvUtTestFdbHashExt(dev, macKeyPtr);
            if(st != GT_OK)
            {
                return st;
            }
        }
    }

    return GT_OK;
}


UTF_TEST_CASE_MAC(prvCpssExMxPmTestFdbHashExt)
{
    GT_STATUS   st;
    GT_U8       dev = 0;

    st = prvUtTestFdbHashCasesExt(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
}

/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmBrgFdbHash suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmBrgFdbHash)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbInternalHashCalc)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgFdbExternalHashCalc)
    /* FDB hash test */
    UTF_SUIT_DECLARE_TEST_MAC(prvCpssExMxPmTestFdbHash)
    UTF_SUIT_DECLARE_TEST_MAC(prvCpssExMxPmTestFdbHashExt)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmBrgFdbHash)

