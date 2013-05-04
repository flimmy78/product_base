/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpLpmUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChIpLpm, that provides
*       the CPSS DXCH LPM Hierarchy manager.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Defines */

/* Tests use this vlan id for testing VLAN functions */
#define IP_LPM_TESTED_VLAN_ID_CNS  100

/* Internal functions forward declaration */

/* Creates two LpmDBs */
static GT_STATUS prvUtfCreateDefaultLpmDB(GT_U8 dev);

/* Creates LpmDB for multicast */
static GT_STATUS prvUtfCreateMcLpmDB(GT_U8 dev);

/* Creates two LpmDBs and Virtual Router */
static GT_STATUS prvUtfCreateLpmDBAndVirtualRouterAdd(IN GT_U8 dev);

/* Compare PclIpUcActionEntries. */
static void prvUtfComparePclIpUcActionEntries(IN GT_U32                    lpmDBId,
                                              IN GT_U32                    vrId,
                                              IN CPSS_DXCH_PCL_ACTION_STC* pclActionReceivePtr,
                                              IN CPSS_DXCH_PCL_ACTION_STC* pclActionExpectedPtr);

/* Compare IpLttEntries. */
static void prvUtfCompareIpLttEntries(IN GT_U32                      lpmDBId,
                                      IN GT_U32                      vrId,
                                      IN CPSS_DXCH_IP_LTT_ENTRY_STC* IpLttEntryReceivePtr,
                                      IN CPSS_DXCH_IP_LTT_ENTRY_STC* IpLttEntryExpectedPtr);

/* Fills returns PclIpUcActionEntry with default values. */
static void prvUtfSetDefaultActionEntry(IN CPSS_DXCH_PCL_ACTION_STC* pclActionPtr);

/* Fills returns IpLttEntry with default values. */
static void prvUtfSetDefaultIpLttEntry(IN CPSS_DXCH_IP_LTT_ENTRY_STC* ipLttEntry);

/* get the shadow type of a given device*/
static GT_STATUS prvUtfGetDevShadow(GT_U8 dev,CPSS_DXCH_IP_TCAM_SHADOW_TYPE_ENT *shadowType);


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmDBCreate
(
    IN GT_U32                                       lpmDBId,
    IN CPSS_DXCH_IP_TCAM_SHADOW_TYPE_ENT            shadowType,
    IN CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    IN GT_BOOL                                      partitionEnable,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *tcamLpmManagerCapcityCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmDBCreate)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{firstIndex [0], lastIndex [100]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10],
        numOfIpv4McSourcePrefixes [5], numOfIpv6Prefixes [10]}
    Expected: GT_OK.
    1.2. Call with lpmDBId [1], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [200], lastIndex [1000]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]}
    Expected: GT_OK.
    1.3. Call with the same lpmDBId [0] (already created) and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with lpmDBId [4], wrong enum values shadowType
        and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with lpmDBId [2], wrong enum values protocolStack
        and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with lpmDBId [5], indexesRangePtr{firstIndex [0],
        lastIndex[0xFFFFFFFF]} (index should be in range 0-1023)
    Expected: NON GT_OK.
    1.7. Call with lpmDBId [6], indexesRangePtr{firstIndex [10], lastIndex[10] }
    Expected: NON GT_OK.
    1.8. Call with lpmDBId [7], indexesRangePtr{firstIndex [10], lastIndex[0] }
    Expected: NOT GT_OK.
    1.10. Call with lpmDBId [9], partitionEnable [GT_TRUE] and
        tcamLpmManagerCapcityCfgPtr[NULL] (relevant only if partitionEnable = GT_TRUE)
        and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.11. Call with lpmDBId [10], indexesRangePtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.12. Call with lpmDBId [11], partitionEnable [GT_TRUE] and
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [0xFFFFFFFF],
        numOfIpv4McSourcePrefixes [5], numOfIpv6Prefixes [10]}.
    Expected: NON GT_OK.
    1.13. Call with lpmDBId [12], partitionEnable [GT_TRUE] and
        tcamLpmManagerCapcityCfgPtr {numOfIpv4Prefixes [10],
        numOfIpv4McSourcePrefixes [0xFFFFFFFF], numOfIpv6Prefixes [10]}.
    Expected: NON GT_OK.
    1.14. Call with lpmDBId [13], partitionEnable [GT_TRUE] and
        tcamLpmManagerCapcityCfgPtr {numOfIpv4Prefixes [10],
        numOfIpv4McSourcePrefixes [5], numOfIpv6Prefixes [0xFFFFFFFF]}.
    Expected: NON GT_OK.
*/
    GT_STATUS st        = GT_OK;
    GT_U8     dev;

    GT_U32                                       lpmDBId         = 0;
    GT_U32                                       maxTcamIndex    = 0;
    CPSS_DXCH_IP_TCAM_SHADOW_TYPE_ENT            shadowType      = CPSS_DXCH_IP_TCAM_CHEETAH_SHADOW_E;
    CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack   = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC indexesRange;
    GT_BOOL                                      partitionEnable = GT_FALSE;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC tcamLpmManagerCapcityCfg;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfGetDevShadow(dev, &shadowType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, shadowType);

        /* get max Tcam index */
        maxTcamIndex = (CPSS_DXCH_IP_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E == shadowType) ?
                       (PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.policyTcamRaws) :
                       (PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.routerAndTunnelTermTcam);

        /*
            1.1. Call with lpmDBId [0],
                           protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
                           indexesRangePtr{firstIndex [0], lastIndex [100]},
                           partitionEnable[GT_TRUE],
                           tcamLpmManagerCapcityCfgPtr{numOfIpv4Prefixes [10],
                                                       numOfIpv4McSourcePrefixes [5],
                                                       numOfIpv6Prefixes [10]}
            Expected: GT_OK.
        */
        lpmDBId         = 0;
        protocolStack   = CPSS_IP_PROTOCOL_IPV4V6_E;
        partitionEnable = GT_TRUE;

        indexesRange.firstIndex = 0;
        indexesRange.lastIndex  = 100;

        tcamLpmManagerCapcityCfg.numOfIpv4Prefixes         = 10;
        tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 5;
        tcamLpmManagerCapcityCfg.numOfIpv6Prefixes         = 10;

        st = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);
        st = GT_ALREADY_EXIST == st ? GT_OK : st;
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, partitionEnable = %d",
                                     lpmDBId, shadowType, protocolStack, partitionEnable);

        /*
            1.2. Call with lpmDBId [1],
                           protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
                           indexesRangePtr{ firstIndex [200], lastIndex [1000]},
                           partitionEnable[GT_TRUE],
                           tcamLpmManagerCapcityCfgPtr{numOfIpv4Prefixes [10],
                                                       numOfIpv4McSourcePrefixes [5],
                                                       numOfIpv6Prefixes [10]}
            Expected: GT_OK.
        */
        lpmDBId    = 1;

        indexesRange.firstIndex = 200;
        indexesRange.lastIndex  = maxTcamIndex - 1;

        st = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, partitionEnable = %d",
                                     lpmDBId, shadowType, protocolStack, partitionEnable);

        /*
            1.3. Call with the same lpmDBId [0] (already created)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        lpmDBId    = 0;

        indexesRange.firstIndex = 0;
        indexesRange.lastIndex  = 100;

        st = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, shadowType);

        /*
            1.4. Call with lpmDBId [4], wrong enum values shadowType
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        lpmDBId = 4;

        UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmDBCreate
                            (lpmDBId, shadowType, protocolStack, &indexesRange,
                             partitionEnable, &tcamLpmManagerCapcityCfg,NULL),
                            shadowType);

        st = prvUtfGetDevShadow(dev, &shadowType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, shadowType);

        /*
            1.5. Call with lpmDBId [2], wrong enum values protocolStack
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        lpmDBId = 2;

        UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmDBCreate
                            (lpmDBId, shadowType, protocolStack, &indexesRange,
                             partitionEnable, &tcamLpmManagerCapcityCfg,NULL),
                            protocolStack);

        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        /*
            1.6. Call with lpmDBId [5], indexesRangePtr{firstIndex [0],
                      lastIndex[0xFFFFFFFF]} (index should be in range 0-1023)
            Expected: NON GT_OK.
        */
        lpmDBId = 5;

        indexesRange.firstIndex = 0;
        indexesRange.lastIndex  = 0xFFFFFFFF;

        st = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, indexesRangePtr->lastIndex = %d",
                                                      lpmDBId, indexesRange.lastIndex);

        indexesRange.lastIndex = 100;

        /*
            1.7. Call with lpmDBId [6], indexesRangePtr{firstIndex [10], lastIndex[10] }
            Expected: NON GT_OK.
        */
        lpmDBId = 6;

        indexesRange.firstIndex = 10;
        indexesRange.lastIndex  = 10;

        st = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, indexesRangePtr->firstIndex = %d, indexesRangePtr->lastIndex = %d",
                    lpmDBId, indexesRange.firstIndex, indexesRange.lastIndex);

        indexesRange.firstIndex = 0;
        indexesRange.lastIndex  = 100;

        /*
            1.8. Call with lpmDBId [7], indexesRangePtr{firstIndex [10], lastIndex[0] }
            Expected: NOT GT_OK.
        */
        lpmDBId = 7;

        indexesRange.firstIndex = 10;
        indexesRange.lastIndex  = 0;

        st = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, indexesRangePtr->firstIndex = %d, indexesRangePtr->lastIndex = %d",
                             lpmDBId, indexesRange.firstIndex, indexesRange.lastIndex);

        indexesRange.firstIndex = 0;
        indexesRange.lastIndex  = 100;

        /*
            1.10. Call with lpmDBId [9], partitionEnable [GT_TRUE]
                            and tcamLpmManagerCapcityCfgPtr[NULL]
                            (relevant only if partitionEnable = GT_TRUE.)
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        lpmDBId         = 9;
        partitionEnable = GT_TRUE;

        st = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable,NULL, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                      "%d, partitionEnable = %d, tcamLpmManagerCapcityCfgPtr = NULL",
                                                      lpmDBId, partitionEnable);

        /*
            1.11. Call with lpmDBId [10], indexesRangePtr [NULL]
                  and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, NULL,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, indexesRangePtr = NULL", lpmDBId);

        /*
            1.12. Call with lpmDBId [11], partitionEnable [GT_TRUE]
                  and tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [0xFFFFFFFF],
                  numOfIpv4McSourcePrefixes [5], numOfIpv6Prefixes [10]}.
            Expected: NON GT_OK.
        */
        lpmDBId = 11;


        tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = 0xFFFFFFFF;

        st = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                            "%d, tcamLpmManagerCapcityCfgPtr->numOfIpv4Prefixes = %d",
                            lpmDBId, tcamLpmManagerCapcityCfg.numOfIpv4Prefixes);

        tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = 10;

        /*
            1.13. Call with lpmDBId [12], partitionEnable [GT_TRUE]
                    and tcamLpmManagerCapcityCfgPtr {numOfIpv4Prefixes [10],
                                                     numOfIpv4McSourcePrefixes [0xFFFFFFFF],
                                                     numOfIpv6Prefixes [10]}.
            Expected: NON GT_OK.
        */
        lpmDBId = 12;
        tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 0xFFFFFFFF;

        st = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);
        /* numOfIpv4McSourcePrefixes is not used for CPSS_DXCH_IP_TCAM_CHEETAH_SHADOW_E*/
        if(PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                        "%d, tcamLpmManagerCapcityCfgPtr->numOfIpv4McSourcePrefixes = %d",
                            lpmDBId, tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                        "%d, tcamLpmManagerCapcityCfgPtr->numOfIpv4McSourcePrefixes = %d",
                             lpmDBId, tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes);
        }


        tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 5;

        /*
            1.14. Call with lpmDBId [13],
                        partitionEnable [GT_TRUE]
                        and tcamLpmManagerCapcityCfgPtr {numOfIpv4Prefixes [10],
                                                         numOfIpv4McSourcePrefixes [5],
                                                         numOfIpv6Prefixes [0xFFFFFFFF]}.
            Expected: NON GT_OK.
        */
        lpmDBId = 13;
        tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = 0xFFFFFFFF;

        st = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                "%d, tcamLpmManagerCapcityCfgPtr->numOfIpv6Prefixes = %d",
                                  lpmDBId, tcamLpmManagerCapcityCfg.numOfIpv6Prefixes);

        tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = 10;
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmDBDevListAdd
(
    IN GT_U32   lpmDBId,
    IN GT_U8    devList[],
    IN GT_U32   numOfDevs
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmDBDevListAdd)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0 / 1],
                     protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
                     indexesRangePtr{ firstIndex [0/ 200],
                                      lastIndex [100/ 1000]},
                     partitionEnable[GT_TRUE],
                     tcamLpmManagerCapcityCfgPtr{numOfIpv4Prefixes [10],
                                                 numOfIpv4McSourcePrefixes [5],
                                                 numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call with lpmDBId [0], devList [dev1, dev2 ... devN], numOfDevs[numOfDevs].
    Expected: GT_OK.
    1.3. Call with not valid LPM DB id lpmDBId [10], devList [10], numOfDevs[1].
    Expected: NOT GT_OK.
    1.4. Call with lpmDBId [1], devList [{PRV_CPSS_MAX_PP_DEVICES_CNS}], numOfDevs[1].
    Expected: NOT GT_OK.
    1.5. Call with lpmDBId [1], devList [NULL], numOfDevs[1]
    Expected: GT_BAD_PTR
*/
    GT_STATUS st        = GT_OK;
    GT_U8     dev, devNum;

    GT_U32    lpmDBId   = 0;
    GT_U8     devList[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U32    numOfDevs = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0 / 1],
                         protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
                         indexesRangePtr{ firstIndex [0/ 200],
                                          lastIndex [100/ 1000]},
                          partitionEnable[GT_TRUE],
                          tcamLpmManagerCapcityCfgPtr{numOfIpv4Prefixes [10],
                                                      numOfIpv4McSourcePrefixes [5],
                                                      numOfIpv6Prefixes [10]} to create LPM DB.
            Expected: GT_OK.
        */
        st = prvUtfCreateDefaultLpmDB(dev);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

        /*
            1.2. Call with lpmDBId [0], devList [dev1, dev2 ... devN], numOfDevs[numOfDevs].
            Expected: GT_OK.
        */
        lpmDBId   = 0;

        /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&devNum, UTF_NONE_FAMILY_E);

        /* 1. Go over all active devices. */
        while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
        {
            devList[numOfDevs++] = devNum;
        }

        st = cpssDxChIpLpmDBDevListAdd(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfDevs = %d", lpmDBId, numOfDevs);

        /*
            1.3. Call with not valid LPM DB id lpmDBId [10], devList [10], numOfDevs[1].
            Expected: NOT GT_OK.
        */
        lpmDBId    = 10;
        devList[0] = 10;
        numOfDevs  = 1;

        st = cpssDxChIpLpmDBDevListAdd(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.4. Call with lpmDBId [1], devList [{PRV_CPSS_MAX_PP_DEVICES_CNS}], numOfDevs[1].
            Expected: NOT GT_OK.
        */
        lpmDBId    = 1;
        devList[0] = PRV_CPSS_MAX_PP_DEVICES_CNS;
        numOfDevs  = 1;

        st = cpssDxChIpLpmDBDevListAdd(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, devList[0] = %d",
                                                     lpmDBId, devList[0]);

        devList[0] = 0;

        /*
            1.5. Call with lpmDBId [1], devList [NULL], numOfDevs[1]
            Expected: GT_BAD_PTR
        */
        st = cpssDxChIpLpmDBDevListAdd(lpmDBId, NULL, numOfDevs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, devList[] = NULL", lpmDBId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmDBDevsListRemove
(
    IN GT_U32   lpmDBId,
    IN GT_U8    devList[],
    IN GT_U32   numOfDevs
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmDBDevsListRemove)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0 / 1],
             protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
             indexesRangePtr{ firstIndex [0/ 200],
                              lastIndex [100/ 1000]},
             partitionEnable[GT_TRUE],
             tcamLpmManagerCapcityCfgPtr{numOfIpv4Prefixes [10],
                                         numOfIpv4McSourcePrefixes [5],
                                         numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmDBDevListAdd with lpmDBId [0 / 1],
                         devList [0, 1, 2, 3, 4] / [5, 6, 7],
                         numOfDevs[5] / [3] to adds devices to an existing LPM DB.
    Expected: GT_OK.
    1.3. Call with lpmDBId [0 / 1], devList [0, 1, 2, 3, 4] / [5, 6, 7], numOfDevs[3].
    Expected: GT_OK.
    1.4. Call with lpmDBId [0], devList [3, 4], numOfDevs[2].
    Expected: GT_OK.
    1.5. Call with lpmDBId [0], devList [0, 1, 2, 3, 4], numOfDevs[5] (already removed).
    Expected: NOT GT_OK.
    1.6. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.7. Call with lpmDBId [1], devList [{PRV_CPSS_MAX_PP_DEVICES_CNS}], numOfDevs[1].
    Expected: NOT GT_OK.
    1.8. Call with lpmDBId [1], devList [NULL], numOfDevs[1]
    Expected: GT_BAD_PTR
*/
    GT_STATUS st        = GT_OK;
    GT_U8     dev, devNum;

    GT_U32    lpmDBId   = 0;
    GT_U8     devList[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U32    numOfDevs = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0 / 1],
                     protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
                     indexesRangePtr{ firstIndex [0/ 200],
                                      lastIndex [100/ 1000]},
                      partitionEnable[GT_TRUE],
                      tcamLpmManagerCapcityCfgPtr{numOfIpv4Prefixes [10],
                                                  numOfIpv4McSourcePrefixes [5],
                                                  numOfIpv6Prefixes [10]} to create LPM DB.
            Expected: GT_OK.
        */
        st = prvUtfCreateDefaultLpmDB(dev);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

        /*
            1.2. Call cpssDxChIpLpmDBDevListAdd with lpmDBId [0 / 1],
                                 devList [0, 1, 2, 3, 4] / [5, 6, 7],
                                 numOfDevs[5] / [3] to adds devices to an existing LPM DB.
            Expected: GT_OK.
        */
        lpmDBId = 0;

        /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&devNum, UTF_NONE_FAMILY_E);

        /* 1. Go over all active devices. */
        while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
        {
            devList[numOfDevs++] = devNum;
        }

        /* for lpmDBId = 0 dev 0 was already added in the init phase,
           so we will get GT_OK but an actual add is not done */
        st = cpssDxChIpLpmDBDevListAdd(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
            "cpssDxChIpLpmDBDevListAdd: %d, numOfDevs = %d", lpmDBId, numOfDevs);

        lpmDBId = 1;

        st = cpssDxChIpLpmDBDevListAdd(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
            "cpssDxChIpLpmDBDevListAdd: %d, numOfDevs = %d", lpmDBId, numOfDevs);

        /*
            1.3. Call with lpmDBId [0 / 1], devList [0, 1, 2, 3, 4] / [5, 6, 7],
                           numOfDevs[3].
            Expected: GT_OK.
        */
        lpmDBId   = 0;

        st = cpssDxChIpLpmDBDevsListRemove(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfDevs = %d", lpmDBId, numOfDevs);

        lpmDBId = 1;

        st = cpssDxChIpLpmDBDevsListRemove(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfDevs = %d", lpmDBId, numOfDevs);

        /*
            1.5. Call with lpmDBId [0],
                           devList [0, 1, 2, 3, 4],
                           numOfDevs[5] (already removed).
            Expected: NOT GT_OK.
        */
        st = cpssDxChIpLpmDBDevsListRemove(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                        "%d, numOfDevs = %d", lpmDBId, numOfDevs);

        /*
            1.6. Call with not valid LPM DB id lpmDBId [10]
                           and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmDBDevsListRemove(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.7. Call with lpmDBId [1], devList [{PRV_CPSS_MAX_PP_DEVICES_CNS}],
                           numOfDevs[1].
            Expected: NOT GT_OK.
        */
        lpmDBId    = 1;
        devList[0] = PRV_CPSS_MAX_PP_DEVICES_CNS;
        numOfDevs  = 1;

        st = cpssDxChIpLpmDBDevsListRemove(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                    "%d, devList[0] = %d", lpmDBId, devList[0]);

        devList[0] = 0;

        /*
            1.8. Call with lpmDBId [1], devList [NULL], numOfDevs[1]
            Expected: GT_BAD_PTR
        */
        st = cpssDxChIpLpmDBDevsListRemove(lpmDBId, NULL, numOfDevs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, devList[] = NULL", lpmDBId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmVirtualRouterAdd
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT *defUcNextHopInfoPtr,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC             *defMcRouteLttEntryPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT             protocolStack
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmVirtualRouterAdd)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate to create default LPM DB configuration.
         See cpssDxChIpLpmDBCreate description.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmDBDevListAdd with lpmDBId [0 / 1],
         devList [0, 1, 2, 3, 4] / [5, 6, 7],
         numOfDevs[5] / [3] to adds devices to an existing LPM DB.
    Expected: GT_OK
    1.3. Call with lpmDBId [0], vrId[0], defUcNextHopInfo filled with
         default configuration, defMcRouteLttEntry [NULL] (support only unicast),
         protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_ALREADY_EXIST.
    1.4. Call with lpmDBId [1], vrId[0], defUcNextHopInfo filled with
         default configuration, defMcRouteLttEntry filled with default
         configuration (support both: unicast and multicast) and
         protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK .
    1.5. Call cpssDxChIpLpmVirtualRouterGet with the same params.
    Expected: GT_OK and the same values.
    1.6. Call with lpmDBId [3], vrId[0], defUcNextHopInfo filled with
         default configuration, defMcRouteLttEntry [NULL]
         (support only unicast) and protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK .
    1.7. Call cpssDxChIpLpmVirtualRouterGet with the same params.
    Expected: GT_OK and the same values.
    1.8. Call with lpmDBId [4], vrId[0], defUcNextHopInfo [NULL]
         defMcRouteLttEntry filled with default configuration
         (support only multicast) and
         protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK .
    1.9. Call cpssDxChIpLpmVirtualRouterGet with the same params.
    Expected: GT_OK and the same values.
    1.10. Call with lpmDBId [0], vrId[0],
             wrong enum values defUcNextHopInfoPtr->pclIpUcAction.pktCmd,
             defMcRouteLttEntry[NULL] (support only UC),
             protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_BAD_PARAM.
    1.11. Call with lpmDBId [1], vrId[0],
            wrong enum values defUcNextHopInfoPtr->ipLttEntry.routeType
            defMcRouteLttEntry {routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
            numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
            sipSaCheckMismatchEnable[GT_TRUE],
            ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
            protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_ALREADY_EXIST.
    1.12. Call with correct numOfPaths [5/7] ipv4/ipv6 for unicast and multicast.
    Expected: GT_OK.
    1.13. Call with correct numOfPaths [8/63] ipv4/ipv6 for unicast and multicast.
    Expected: GT_OK for lion and above and not GT_OK for other.
    1.14. Call with wrong numOfPaths [64] (out of range).
    Check ipv4/ipv6 for unicast and multicast.
    Expected: NOT GT_OK
    1.15. Call with only supportIpv4Mc enabled (other disabled).
            vrConfigInfo.supportIpv4Uc = GT_FALSE;
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            vrConfigInfo.supportIpv6Uc = GT_FALSE;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;
    Expected: GT_OK.
    1.16. Call with all types disabled.
            vrConfigInfo.supportIpv4Uc = GT_FALSE;
            vrConfigInfo.supportIpv4Mc = GT_FALSE;
            vrConfigInfo.supportIpv6Uc = GT_FALSE;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;
    Expected: NOT GT_OK.
    1.17. Call with lpmDBId [1], vrId[0],
            out of range defUcNextHopInfoPtr->ipLttEntry.routeEntryBaseIndex [4096]
            defMcRouteLttEntry {routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                               numOfPaths[0],
                               routeEntryBaseIndex[0],
                               ucRPFCheckEnable[GT_TRUE],
                               sipSaCheckMismatchEnable[GT_TRUE],
                               ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
            protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: NOT GT_OK.
    1.18. Call with lpmDBId [1], vrId[0],
            out of range defUcNextHopInfoPtr->ipLttEntry.
            ipv6MCGroupScopeLevel [wrong enum values]
            defMcRouteLttEntry {routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                                   numOfPaths[0],
                                   routeEntryBaseIndex[0],
                                   ucRPFCheckEnable[GT_TRUE],
                                   sipSaCheckMismatchEnable[GT_TRUE],
                                   ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
            protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_BAD_PARAM.
    1.19. Call with not valid LPM DB id lpmDBId [10].
    Expected: NOT GT_OK.
    1.20. Call with lpmDBId [0],
                invalid defUcNextHopInfoPtr->pclIpUcAction.mirror.cpuCode
                [CPSS_NET_MIRROR_PCL_RST_FIN_E] (Tiger only),
                and other parameters the same as in 1.2.
    Expected: NON GT_OK.
    1.21. Call with lpmDBId [0],
                     wrong enum values defUcNextHopInfoPtr->pclIpUcAction.mirror.cpuCode
                     and other parameters the same as in 1.2.
    Expected: GT_BAD_PARAM.
    1.22. Call with lpmDBId [0],
             defUcNextHopInfoPtr->pclIpUcAction.matchCounter
             [{enableMatchCount = GT_TRUE; matchCounterIndex = 33 (out of range)}]
             and other parameters the same as in 1.2.
    Expected: NON GT_OK.
    1.23. Call with lpmDBId [0],
              defUcNextHopInfoPtr->pclIpUcAction.qos.modifyDscp [wrong enum values]
              and other parameters the same as in 1.2.
    Expected: GT_BAD_PARAM.
    1.24. Call with lpmDBId [0],
                     defUcNextHopInfoPtr->pclIpUcAction.qos.modifyUp [wrong enum values]
                     and other parameters the same as in 1.2.
    Expected: GT_BAD_PARAM.
    1.25. Call with lpmDBId [0],
             defUcNextHopInfoPtr->pclIpUcAction.qos.ingress.profilePrecedence
             [wrong enum values]
             and other parameters the same as in 1.2.
    Expected: GT_BAD_PARAM.
    1.26. Call with lpmDBId [0],
             defUcNextHopInfoPtr->pclIpUcAction.redirect.redirectCmd [wrong enum values]
             and other parameters the same as in 1.2.
    Expected: GT_BAD_PARAM.
    1.27. Call with lpmDBId [0],
          defUcNextHopInfoPtr->pclIpUcAction.policer[{policerEnable = GT_TRUE;
                                                      policerId = 0xFFFFFFFF}]
                     and other parameters the same as in 1.2.
    Expected: NON GT_OK.
    1.28. Call with lpmDBId [0],
             defUcNextHopInfoPtr->pclIpUcAction.vlan
                        [{egressTaggedModify = GT_TRUE;
                          modifyVlan = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E,
                          vlanId = 4096}]
             and other parameters the same as in 1.2.
    Expected: GT_BAD_PARAM.
    1.29. Call with lpmDBId [0],
             defUcNextHopInfoPtr->pclIpUcAction.vlan[{egressTaggedModify = GT_FALSE;
                                                      modifyVlan = wrong enum values}]
             and other parameters the same as in 1.2.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                              st            = GT_OK;
    GT_U8                                  dev, devNum;

    GT_U32                                 lpmDBId       = 0;
    GT_U32                                 vrId          = 0;

    GT_U8                                  devList[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U32                                 numOfDevs = 0;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC         vrConfigInfo;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC         vrConfigInfoGet;
    GT_BOOL                                isEqual = GT_FALSE;

    cpssOsBzero((GT_VOID*) &vrConfigInfo, sizeof(CPSS_DXCH_IP_LPM_VR_CONFIG_STC));
    cpssOsBzero((GT_VOID*) &vrConfigInfoGet, sizeof(CPSS_DXCH_IP_LPM_VR_CONFIG_STC));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. Call cpssDxChIpLpmDBCreate to create default LPM DB configuration.
                 See cpssDxChIpLpmDBCreate description.
            Expected: GT_OK.
        */
        st = prvUtfCreateDefaultLpmDB(dev);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

        /*
            1.2. Call cpssDxChIpLpmDBDevListAdd with lpmDBId [0 / 1],
                 devList [0, 1, 2, 3, 4] / [5, 6, 7],
                 numOfDevs[5] / [3] to adds devices to an existing LPM DB.
            Expected: GT_OK.
        */
        lpmDBId = 0;

        /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&devNum, UTF_NONE_FAMILY_E);

        /* 1. Go over all active devices. */
        while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
        {
            devList[numOfDevs++] = devNum;
        }

        /* for lpmDBId = 0 dev 0 was already added in the init phase,
           so we will get GT_OK but an actual add is not done */
        st = cpssDxChIpLpmDBDevListAdd(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChIpLpmDBDevListAdd: %d, numOfDevs = %d", lpmDBId, numOfDevs);

        lpmDBId = 1;

        st = cpssDxChIpLpmDBDevListAdd(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChIpLpmDBDevListAdd: %d, numOfDevs = %d", lpmDBId, numOfDevs);

        /*
            1.3. Call with lpmDBId [0], vrId[0], defUcNextHopInfo filled with
                 default configuration, defMcRouteLttEntry [NULL] (support only unicast),
                 protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
            Expected: GT_ALREADY_EXIST.
        */
        lpmDBId = 0;
        vrId    = 0;
        vrConfigInfo.supportIpv4Mc = GT_FALSE;
        vrConfigInfo.supportIpv6Mc = GT_FALSE;
        vrConfigInfo.supportIpv4Uc = GT_TRUE;
        vrConfigInfo.supportIpv6Uc = GT_TRUE;

        /* Fill defUcNextHopInfoPtr structure */
        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction);
            prvUtfSetDefaultActionEntry(&vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry);
            prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry);
        }

        st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_ALREADY_EXIST, st,"%d, %d", lpmDBId, vrId);

        /*
            1.4. Call with lpmDBId [1], vrId[0], defUcNextHopInfo filled with
                 default configuration, defMcRouteLttEntry filled with default
                 configuration (support both: unicast and multicast) and
                 protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
            Expected: GT_OK .
        */
        lpmDBId = 1;
        vrConfigInfo.supportIpv4Mc = !(prvUtfIsPbrModeUsed());
        vrConfigInfo.supportIpv6Mc = !(prvUtfIsPbrModeUsed());
        vrConfigInfo.supportIpv4Uc = GT_TRUE;
        vrConfigInfo.supportIpv6Uc = GT_TRUE;

        prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv4McRouteLttEntry);
        prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv6McRouteLttEntry);

        st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,"%d, %d", lpmDBId, vrId);

        /*
            1.5. Call cpssDxChIpLpmVirtualRouterGet with the same params.
            Expected: GT_OK and the same values.
        */
        st = cpssDxChIpLpmVirtualRouterGet (lpmDBId, vrId, &vrConfigInfoGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,"%d, %d", lpmDBId, vrId);
        if (GT_OK == st)
        {
            /*check supportIpv4Uc*/
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, vrConfigInfoGet.supportIpv4Uc,
                                "supportIpv4Uc don't equal GT_TRUE, %d", vrConfigInfoGet.supportIpv4Uc);

            /*check vrConfigInfoGet.defIpv4UcNextHopInfo.ipLttEntry*/
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry,
                                         (GT_VOID*)&vrConfigInfoGet.defIpv4UcNextHopInfo.ipLttEntry,
                       sizeof (CPSS_DXCH_IP_LTT_ENTRY_STC))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                    "get another defIpv4UcNextHopInfo than was set: %d", dev);
            /*check supportIpv6Uc*/
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, vrConfigInfoGet.supportIpv6Uc,
                                "supportIpv6Uc don't equal GT_TRUE, %d", vrConfigInfoGet.supportIpv6Uc);

            /*check vrConfigInfo.defIpv6UcNextHopInfoGet.ipLttEntry*/
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry,
                                         (GT_VOID*)&vrConfigInfoGet.defIpv6UcNextHopInfo.ipLttEntry,
                                         sizeof (CPSS_DXCH_IP_LTT_ENTRY_STC))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                    "get another defIpv6UcNextHopInfo than was set: %d", dev);

            /*check supportIpv4Mc*/
            UTF_VERIFY_EQUAL1_STRING_MAC(!prvUtfIsPbrModeUsed(), vrConfigInfoGet.supportIpv4Mc,
                                "supportIpv4Mc don't equal GT_TRUE, %d", vrConfigInfoGet.supportIpv4Mc);

            if (GT_FALSE == prvUtfIsPbrModeUsed())
            {
                /*check vrConfigInfoGet.defIpv4McRouteLttEntry*/
                isEqual = (0 == cpssOsMemCmp((GT_VOID*)&vrConfigInfo.defIpv4McRouteLttEntry,
                                             (GT_VOID*)&vrConfigInfoGet.defIpv4McRouteLttEntry,
                                             sizeof (CPSS_DXCH_IP_LTT_ENTRY_STC))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                        "get another defIpv4McRouteLttEntry than was set: %d", dev);
            }

            /*check supportIpv6Mc*/
            UTF_VERIFY_EQUAL1_STRING_MAC(!prvUtfIsPbrModeUsed(), vrConfigInfoGet.supportIpv6Mc,
                                "supportIpv6Mc don't equal GT_TRUE, %d", vrConfigInfoGet.supportIpv6Mc);

            if (GT_FALSE == prvUtfIsPbrModeUsed())
            {
                /*check vrConfigInfoGet.defIpv4McRouteLttEntry*/
                isEqual = (0 == cpssOsMemCmp((GT_VOID*)&vrConfigInfo.defIpv6McRouteLttEntry,
                                             (GT_VOID*)&vrConfigInfoGet.defIpv6McRouteLttEntry,
                                             sizeof (CPSS_DXCH_IP_LTT_ENTRY_STC))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                        "get another defIpv6McRouteLttEntry than was set: %d", dev);
            }
        }

        /*
            1.6. Call with lpmDBId [3], vrId[0], defUcNextHopInfo filled with
                 default configuration, defMcRouteLttEntry [NULL]
                 (support only unicast) and protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
            Expected: GT_OK .
        */
        lpmDBId = 3;
        vrConfigInfo.supportIpv4Mc = GT_FALSE;
        vrConfigInfo.supportIpv6Mc = GT_FALSE;

        st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,"%d, %d", lpmDBId, vrId);

        /*
            1.7. Call cpssDxChIpLpmVirtualRouterGet with the same params.
            Expected: GT_OK and the same values.
        */
        st = cpssDxChIpLpmVirtualRouterGet (lpmDBId, vrId, &vrConfigInfoGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,"%d, %d", lpmDBId, vrId);

        if (GT_OK == st)
        {
            /*check supportIpv4Uc*/
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, vrConfigInfoGet.supportIpv4Uc,
                                "supportIpv4Uc don't equal GT_TRUE, %d", vrConfigInfoGet.supportIpv4Uc);

            /*check vrConfigInfoGet.defIpv4UcNextHopInfo.ipLttEntry*/
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry,
                                         (GT_VOID*)&vrConfigInfoGet.defIpv4UcNextHopInfo.ipLttEntry,
                       sizeof (CPSS_DXCH_IP_LTT_ENTRY_STC))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                    "get another defIpv4UcNextHopInfo than was set: %d", dev);
            /*check supportIpv6Uc*/
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, vrConfigInfoGet.supportIpv6Uc,
                                "supportIpv6Uc don't equal GT_TRUE, %d", vrConfigInfoGet.supportIpv6Uc);

            /*check vrConfigInfo.defIpv6UcNextHopInfoGet.ipLttEntry*/
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry,
                                         (GT_VOID*)&vrConfigInfoGet.defIpv6UcNextHopInfo.ipLttEntry,
                                         sizeof (CPSS_DXCH_IP_LTT_ENTRY_STC))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                    "get another defIpv6UcNextHopInfo than was set: %d", dev);

            /*check supportIpv4Mc*/
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, vrConfigInfoGet.supportIpv4Mc,
                                "supportIpv4Mc don't equal GT_TRUE, %d", vrConfigInfoGet.supportIpv4Mc);

            /*check supportIpv6Mc*/
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, vrConfigInfoGet.supportIpv6Mc,
                                "supportIpv6Mc don't equal GT_TRUE, %d", vrConfigInfoGet.supportIpv6Mc);
        }

        /* MC is not supported in PBR mode */
        if (GT_FALSE == prvUtfIsPbrModeUsed())
        {
            /*
                1.8. Call with lpmDBId [4], vrId[0], defUcNextHopInfo [NULL]
                     defMcRouteLttEntry filled with default configuration
                     (support only multicast) and
                     protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
                Expected: GT_OK .
            */
            lpmDBId = 4;
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            vrConfigInfo.supportIpv6Mc = GT_TRUE;
            vrConfigInfo.supportIpv4Uc = GT_FALSE;
            vrConfigInfo.supportIpv6Uc = GT_FALSE;

            prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv4McRouteLttEntry);
            prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv6McRouteLttEntry);

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,"%d, %d", lpmDBId, vrId);

            /*
                1.9. Call cpssDxChIpLpmVirtualRouterGet with the same params.
                Expected: GT_OK and the same values.
            */
            st = cpssDxChIpLpmVirtualRouterGet (lpmDBId, vrId, &vrConfigInfoGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,"%d, %d", lpmDBId, vrId);

            if (GT_OK == st)
            {
                /*check supportIpv4Uc*/
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, vrConfigInfoGet.supportIpv4Uc,
                                    "supportIpv4Uc don't equal GT_TRUE, %d", vrConfigInfoGet.supportIpv4Uc);

                /*check supportIpv6Uc*/
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, vrConfigInfoGet.supportIpv6Uc,
                                    "supportIpv6Uc don't equal GT_TRUE, %d", vrConfigInfoGet.supportIpv6Uc);

                /*check supportIpv4Mc*/
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, vrConfigInfoGet.supportIpv4Mc,
                                    "supportIpv4Mc don't equal GT_TRUE, %d", vrConfigInfoGet.supportIpv4Mc);

                /*check vrConfigInfoGet.defIpv4McRouteLttEntry*/
                isEqual = (0 == cpssOsMemCmp((GT_VOID*)&vrConfigInfo.defIpv4McRouteLttEntry,
                                             (GT_VOID*)&vrConfigInfoGet.defIpv4McRouteLttEntry,
                                             sizeof (CPSS_DXCH_IP_LTT_ENTRY_STC))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                        "get another defIpv4McRouteLttEntry than was set: %d", dev);
                /*check supportIpv6Mc*/
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, vrConfigInfoGet.supportIpv6Mc,
                                    "supportIpv6Mc don't equal GT_TRUE, %d", vrConfigInfoGet.supportIpv6Mc);

                /*check vrConfigInfoGet.defIpv4McRouteLttEntry*/
                isEqual = (0 == cpssOsMemCmp((GT_VOID*)&vrConfigInfo.defIpv6McRouteLttEntry,
                                             (GT_VOID*)&vrConfigInfoGet.defIpv6McRouteLttEntry,
                                             sizeof (CPSS_DXCH_IP_LTT_ENTRY_STC))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                        "get another defIpv6McRouteLttEntry than was set: %d", dev);
            }
        }

        /*
            1.10. Call with lpmDBId [0], vrId[0],
                     wrong enum values defUcNextHopInfoPtr->pclIpUcAction.pktCmd,
                     defMcRouteLttEntry[NULL] (support only UC),
                     protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
            Expected: GT_BAD_PARAM.
        */
        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            lpmDBId = 0;
            vrConfigInfo.supportIpv4Mc = GT_FALSE;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;
            vrConfigInfo.supportIpv4Uc = GT_TRUE;
            vrConfigInfo.supportIpv6Uc = GT_TRUE;

            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                                (lpmDBId, vrId, &vrConfigInfo),
                                vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.pktCmd);

            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                                (lpmDBId, vrId, &vrConfigInfo),
                                vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.pktCmd);
        }

        /*
            1.11. Call with lpmDBId [1], vrId[0],
                    wrong enum values defUcNextHopInfoPtr->ipLttEntry.routeType
                    defMcRouteLttEntry {routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                    numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
                    sipSaCheckMismatchEnable[GT_TRUE],
                    ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
                    protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
            Expected: GT_ALREADY_EXIST.
        */

        lpmDBId = 1;
        vrConfigInfo.supportIpv4Mc = !(prvUtfIsPbrModeUsed());;
        vrConfigInfo.supportIpv6Mc = !(prvUtfIsPbrModeUsed());;
        vrConfigInfo.supportIpv4Uc = GT_TRUE;
        vrConfigInfo.supportIpv6Uc = GT_TRUE;

        vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths     = 0;
        vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths     = 0;

        vrConfigInfo.defIpv4McRouteLttEntry.routeEntryBaseIndex      = 0;
        vrConfigInfo.defIpv4McRouteLttEntry.ucRPFCheckEnable         = GT_TRUE;
        vrConfigInfo.defIpv4McRouteLttEntry.sipSaCheckMismatchEnable = GT_TRUE;
        vrConfigInfo.defIpv4McRouteLttEntry.ipv6MCGroupScopeLevel    = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;

        vrConfigInfo.defIpv6McRouteLttEntry.routeEntryBaseIndex      = 0;
        vrConfigInfo.defIpv6McRouteLttEntry.ucRPFCheckEnable         = GT_TRUE;
        vrConfigInfo.defIpv6McRouteLttEntry.sipSaCheckMismatchEnable = GT_TRUE;
        vrConfigInfo.defIpv6McRouteLttEntry.ipv6MCGroupScopeLevel    = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;

        for(enumsIndex = 0; enumsIndex < utfInvalidEnumArrSize; enumsIndex++)
        {
            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeType =
                                                        utfInvalidEnumArr[enumsIndex];
            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_ALREADY_EXIST, st,
                  "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.routeType = %d",
                  lpmDBId, vrId,  vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeType);
        }

        vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeType
                                                = CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E;

        for(enumsIndex = 0; enumsIndex < utfInvalidEnumArrSize; enumsIndex++)
        {
            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.routeType =
                                                        utfInvalidEnumArr[enumsIndex];
            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_ALREADY_EXIST, st,
                   "%d, %d, defIpv6UcNextHopInfoPtr->ipLttEntry.routeType = %d",
                   lpmDBId, vrId,  vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.routeType);
        }
        vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.routeType
                                                   = CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E;

        vrConfigInfo.defIpv4McRouteLttEntry.routeType = CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E;
        vrConfigInfo.defIpv6McRouteLttEntry.routeType = CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E;

        st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);

        /*
            1.12. Call with correct numOfPaths [5/7] ipv4/ipv6 for unicast and multicast.
            Expected: GT_OK.
        */
        /* call with numOfPaths [5] for ipv4 Uc */
        vrConfigInfo.supportIpv4Uc = GT_TRUE;

        vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 5;

        st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
               lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);

        /* restore configuration */
        vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 0;

        st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);

        vrConfigInfo.supportIpv4Uc = GT_FALSE;

        /* call with numOfPaths [7] for ipv4 Uc */
        vrConfigInfo.supportIpv4Uc = GT_TRUE;

        vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 7;

        st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
               lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);

        /* restore configuration */
        vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 0;

        st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);

        vrConfigInfo.supportIpv4Uc = GT_FALSE;

        /* call with numOfPaths [5] for ipv6 Uc */
        vrConfigInfo.supportIpv6Uc = GT_TRUE;

        vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 5;

        st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "%d, %d, defIpv6UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
               lpmDBId, vrId, vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths);

        /* restore configuration */
        vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 0;

        st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);

        vrConfigInfo.supportIpv6Uc = GT_FALSE;

        /* call with numOfPaths [7] for ipv4 Uc */
        vrConfigInfo.supportIpv6Uc = GT_TRUE;

        vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 7;

        st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "%d, %d, defIpv6UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
               lpmDBId, vrId, vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths);

        /* restore configuration */
        vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 0;

        st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);

        vrConfigInfo.supportIpv6Uc = GT_FALSE;

        /* MC is not supported in PBR mode */
        if (GT_FALSE == prvUtfIsPbrModeUsed())
        {
            /* call with numOfPaths [5] for ipv4 Mc */
            vrConfigInfo.supportIpv4Mc = GT_TRUE;

            vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths = 5;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4McRouteLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths);

            /* restore configuration */
            vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths = 0;

            st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);

            vrConfigInfo.supportIpv4Mc = GT_FALSE;

            /* call with numOfPaths [7] for ipv4 Mc */
            vrConfigInfo.supportIpv4Mc = GT_TRUE;

            vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths = 7;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4McRouteLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths);

            /* restore configuration */
            vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths = 0;

            st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);

            vrConfigInfo.supportIpv4Mc = GT_FALSE;

            /* call with numOfPaths [5] for ipv6 Mc */
            vrConfigInfo.supportIpv6Mc = GT_TRUE;

            vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths = 5;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv6McRouteLttEntryPtr.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths);

            /* restore configuration */
            vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths = 0;

            st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);

            vrConfigInfo.supportIpv6Mc = GT_FALSE;

            /* call with numOfPaths [7] for ipv4 Mc */
            vrConfigInfo.supportIpv6Mc = GT_TRUE;

            vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths = 7;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv6McRouteLttEntryPtr.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths);

            /* restore configuration */
            vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths = 0;

            st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);

            vrConfigInfo.supportIpv6Mc = GT_FALSE;
        }

        /*
            1.13. Call with correct numOfPaths [8/63] ipv4/ipv6 for unicast and multicast.
            Expected: GT_OK for lion and above and not GT_OK for other.
        */
        /* call with numOfPaths [8] for ipv4 Uc */
        vrConfigInfo.supportIpv4Uc = GT_TRUE;

        vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 8;

        st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
        if ((CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily) || prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);

            /* restore configuration */
            st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
        }

        /* restore configuration */
        vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 0;
        vrConfigInfo.supportIpv4Uc = GT_FALSE;

        /* call with numOfPaths [63] for ipv4 Uc */
        vrConfigInfo.supportIpv4Uc = GT_TRUE;

        vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 63;

        st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
        if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);

            /* restore configuration */
            st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
        }

        /* restore configuration */
        vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 0;
        vrConfigInfo.supportIpv4Uc = GT_FALSE;

        /* call with numOfPaths [8] for ipv6 Uc */
        vrConfigInfo.supportIpv6Uc = GT_TRUE;

        vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 8;

        st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
        if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);

            /* restore configuration */
            st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
        }

        /* restore configuration */
        vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 0;
        vrConfigInfo.supportIpv6Uc = GT_FALSE;

        /* call with numOfPaths [63] for ipv4 Uc */
        vrConfigInfo.supportIpv6Uc = GT_TRUE;

        vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 63;

        st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
        if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);

            /* restore configuration */
            st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
        }

        /* restore configuration */
        vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 0;
        vrConfigInfo.supportIpv6Uc = GT_FALSE;

        /* MC is not supported in PBR mode */
        if (GT_FALSE == prvUtfIsPbrModeUsed())
        {
            /* call with numOfPaths [8] for ipv4 Mc */
            vrConfigInfo.supportIpv4Mc = GT_TRUE;

            vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths = 8;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);

                /* restore configuration */
                st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
            }

            /* restore configuration */
            vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths = 0;
            vrConfigInfo.supportIpv4Mc = GT_FALSE;

            /* call with numOfPaths [63] for ipv4 Mc */
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths = 63;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);

                /* restore configuration */
                st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
            }

            /* restore configuration */
            vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths = 0;
            vrConfigInfo.supportIpv4Mc = GT_FALSE;

            /* call with numOfPaths [8] for ipv6 Mc */
            vrConfigInfo.supportIpv6Mc = GT_TRUE;
            vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths = 8;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);

                /* restore configuration */
                st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
            }

            /* restore configuration */
            vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths = 0;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;

            /* call with numOfPaths [63] for ipv4 Mc */
            vrConfigInfo.supportIpv6Mc = GT_TRUE;

            vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths = 63;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);

                /* restore configuration */
                st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
            }

            /* restore configuration */
            vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths = 0;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;
        }

        /*
            1.14. Call with wrong numOfPaths [64] (out of range).
            Check ipv4/ipv6 for unicast and multicast.
            Expected: NOT GT_OK
        */
        /* LTT Entry is not supported in PBR mode */
        if (GT_FALSE == prvUtfIsPbrModeUsed())
        {
            /* call with numOfPaths [64] for ipv4 Uc */
            vrConfigInfo.supportIpv4Uc = GT_TRUE;

            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 64;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);

            /* restore configuration */
            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 0;
            vrConfigInfo.supportIpv4Uc = GT_FALSE;

            /* call with numOfPaths [64] for ipv6 Uc */
            vrConfigInfo.supportIpv6Uc = GT_TRUE;
            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 64;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv6UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths);

            /* restore configuration */
            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 0;
            vrConfigInfo.supportIpv6Uc = GT_FALSE;

            /* call with numOfPaths [64] for ipv4 Mc */
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths = 64;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4McRouteLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths);

            /* restore configuration */
            vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths = 0;
            vrConfigInfo.supportIpv4Mc = GT_FALSE;

            /* call with numOfPaths [64] for ipv6 Mc */
            vrConfigInfo.supportIpv6Mc = GT_TRUE;
            vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths = 64;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv6McRouteLttEntryPtr.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths);

            /* restore configuration */
            vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths = 0;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;
        }

        /*
            1.15. Call with only supportIpv4Mc enabled (other disabled).
                    vrConfigInfo.supportIpv4Uc = GT_FALSE;
                    vrConfigInfo.supportIpv4Mc = GT_TRUE;
                    vrConfigInfo.supportIpv6Uc = GT_FALSE;
                    vrConfigInfo.supportIpv6Mc = GT_FALSE;
            Expected: GT_OK.
        */
        /* MC is not supported in PBR mode */
        if (GT_FALSE == prvUtfIsPbrModeUsed())
        {
            vrConfigInfo.supportIpv4Uc = GT_FALSE;
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            vrConfigInfo.supportIpv6Uc = GT_FALSE;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "%d, %d, call with enabled only supportIpv4Mc", lpmDBId, vrId);

            if(GT_OK == st)
            {
                /*
                    1.16. Call with all types disabled.
                            vrConfigInfo.supportIpv4Uc = GT_FALSE;
                            vrConfigInfo.supportIpv4Mc = GT_FALSE;
                            vrConfigInfo.supportIpv6Uc = GT_FALSE;
                            vrConfigInfo.supportIpv6Mc = GT_FALSE;
                    Expected: NOT GT_OK.
                */
                vrConfigInfo.supportIpv4Uc = GT_FALSE;
                vrConfigInfo.supportIpv4Mc = GT_FALSE;
                vrConfigInfo.supportIpv6Uc = GT_FALSE;
                vrConfigInfo.supportIpv6Mc = GT_FALSE;

                st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
                UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                       "%d, %d, call with disabled all features", lpmDBId, vrId);

                /* restore configuration */
                st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);

                vrConfigInfo.supportIpv4Mc = GT_TRUE;
            }
        }

        /*
            1.17. Call with lpmDBId [1], vrId[0],
                    out of range defUcNextHopInfoPtr->ipLttEntry.routeEntryBaseIndex [4096]
                    defMcRouteLttEntry {routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                                       numOfPaths[0],
                                       routeEntryBaseIndex[0],
                                       ucRPFCheckEnable[GT_TRUE],
                                       sipSaCheckMismatchEnable[GT_TRUE],
                                       ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
                    protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
            Expected: NOT GT_OK.
        */
        /* LTT Entry is not supported in PBR mode */
        if (GT_FALSE == prvUtfIsPbrModeUsed())
        {
            /* check ipv4 */
            vrConfigInfo.supportIpv4Uc = GT_TRUE;
            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeEntryBaseIndex =
                                PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.routerNextHop;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.routeEntryBaseIndex = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeEntryBaseIndex);

            if(GT_OK == st)
            {
                /* restore configuration */
                st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
            }

            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeEntryBaseIndex = 0;

            /* check ipv6 */
            vrConfigInfo.supportIpv6Uc = GT_TRUE;
            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.routeEntryBaseIndex =
                               PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.routerNextHop;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, defIpv6UcNextHopInfoPtr->ipLttEntry.routeEntryBaseIndex = %d",
                    lpmDBId, vrId, vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.routeEntryBaseIndex);

            if(GT_OK == st)
            {
                /* restore configuration */
                st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
            }

            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.routeEntryBaseIndex = 0;
        }

        /*
            1.18. Call with lpmDBId [1], vrId[0],
                    out of range defUcNextHopInfoPtr->ipLttEntry.
                    ipv6MCGroupScopeLevel [wrong enum values]
                    defMcRouteLttEntry {routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                                           numOfPaths[0],
                                           routeEntryBaseIndex[0],
                                           ucRPFCheckEnable[GT_TRUE],
                                           sipSaCheckMismatchEnable[GT_TRUE],
                                           ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
                    protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                            (lpmDBId, vrId, &vrConfigInfo),
                            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel);

        UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                            (lpmDBId, vrId, &vrConfigInfo),
                            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel);

        /*
            1.19. Call with not valid lpmDBId [10].
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /* Test cases for Cheetah+ devices */
        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            /*
                1.20. Call with lpmDBId [0],
                            invalid defUcNextHopInfoPtr->pclIpUcAction.mirror.cpuCode
                            [CPSS_NET_MIRROR_PCL_RST_FIN_E] (Tiger only),
                            and other parameters the same as in 1.2.
                Expected: NON GT_OK.
            */
            lpmDBId = 0;
            vrConfigInfo.supportIpv4Uc = GT_TRUE;
            vrConfigInfo.supportIpv6Uc = GT_TRUE;
            vrConfigInfo.supportIpv4Mc = GT_FALSE;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;

            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.mirror.cpuCode = CPSS_NET_MIRROR_PCL_RST_FIN_E;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                            "%d, %d, defIpv4UcNextHopInfoPtr->pclIpUcAction.mirror.cpuCode = %d",
                            lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.mirror.cpuCode);

            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.mirror.cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E;

            vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.mirror.cpuCode = CPSS_NET_MIRROR_PCL_RST_FIN_E;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                            "%d, %d, defIpv6UcNextHopInfoPtr->pclIpUcAction.mirror.cpuCode = %d",
                            lpmDBId, vrId, vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.mirror.cpuCode);

            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.mirror.cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E;

            /*
                1.21. Call with lpmDBId [0],
                                 wrong enum values defUcNextHopInfoPtr->pclIpUcAction.mirror.cpuCode
                                 and other parameters the same as in 1.2.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                                (lpmDBId, vrId, &vrConfigInfo),
                                vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.mirror.cpuCode);

            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                                (lpmDBId, vrId, &vrConfigInfo),
                                vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.mirror.cpuCode);

            /*
                1.22. Call with lpmDBId [0],
                         defUcNextHopInfoPtr->pclIpUcAction.matchCounter
                         [{enableMatchCount = GT_TRUE; matchCounterIndex = 33 (out of range)}]
                         and other parameters the same as in 1.2.
                Expected: NON GT_OK.
            */
            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.matchCounter.enableMatchCount  = GT_TRUE;
            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex = 33;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, %d, defIpv4UcNextHopInfoPtr->pclIpUcAction.matchCounter.matchCounterIndex = %d",
                 lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex);

            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.matchCounter.enableMatchCount  = GT_FALSE;
            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex = 0;

            vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.matchCounter.enableMatchCount  = GT_TRUE;
            vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex = 33;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, %d, defIpv6UcNextHopInfoPtr->pclIpUcAction.matchCounter.matchCounterIndex = %d",
                 lpmDBId, vrId, vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex);

            vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.matchCounter.enableMatchCount  = GT_FALSE;
            vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex = 0;

            /*
                1.23. Call with lpmDBId [0],
                          defUcNextHopInfoPtr->pclIpUcAction.qos.modifyDscp [wrong enum values]
                          and other parameters the same as in 1.2.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                                (lpmDBId, vrId, &vrConfigInfo),
                                vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.qos.modifyDscp);

            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                                (lpmDBId, vrId, &vrConfigInfo),
                                vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.qos.modifyDscp);

            /*
                1.24. Call with lpmDBId [0],
                                 defUcNextHopInfoPtr->pclIpUcAction.qos.modifyUp [wrong enum values]
                                 and other parameters the same as in 1.2.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                                (lpmDBId, vrId, &vrConfigInfo),
                                vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.qos.modifyUp);

            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                                (lpmDBId, vrId, &vrConfigInfo),
                                vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.qos.modifyUp);

            /*
                1.25. Call with lpmDBId [0],
                         defUcNextHopInfoPtr->pclIpUcAction.qos.ingress.profilePrecedence
                         [wrong enum values]
                         and other parameters the same as in 1.2.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                                (lpmDBId, vrId, &vrConfigInfo),
              vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.qos.qos.ingress.profilePrecedence);

            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                                (lpmDBId, vrId, &vrConfigInfo),
              vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.qos.qos.ingress.profilePrecedence);

            /*
                1.26. Call with lpmDBId [0],
                         defUcNextHopInfoPtr->pclIpUcAction.redirect.redirectCmd [wrong enum values]
                         and other parameters the same as in 1.2.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                                (lpmDBId, vrId, &vrConfigInfo),
              vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.redirect.redirectCmd);

            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                                (lpmDBId, vrId, &vrConfigInfo),
              vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.redirect.redirectCmd);

            /*
                1.27. Call with lpmDBId [0],
                      defUcNextHopInfoPtr->pclIpUcAction.policer[{policerEnable = GT_TRUE;
                                                                  policerId = 0xFFFFFFFF}]
                                 and other parameters the same as in 1.2.
                Expected: NON GT_OK.
            */
            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.policer.policerEnable = GT_TRUE;
            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.policer.policerId     = 0xFFFFFFFF;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                            "%d, %d, defIpv4UcNextHopInfoPtr->pclIpUcAction.policer.policerId = %d",
                             lpmDBId, vrId,  vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.policer.policerId);

            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.policer.policerEnable = GT_FALSE;
            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.policer.policerId     = 0;

            vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.policer.policerEnable = GT_TRUE;
            vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.policer.policerId     = 0xFFFFFFFF;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                            "%d, %d, defIpv6UcNextHopInfoPtr->pclIpUcAction.policer.policerId = %d",
                             lpmDBId, vrId,  vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.policer.policerId);

            vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.policer.policerEnable = GT_FALSE;
            vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.policer.policerId     = 0;

            /*
                1.28. Call with lpmDBId [0],
                         defUcNextHopInfoPtr->pclIpUcAction.vlan
                                    [{egressTaggedModify = GT_TRUE;
                                      modifyVlan = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E,
                                      vlanId = 4096}]
                         and other parameters the same as in 1.2.
                Expected: GT_BAD_PARAM.
            */
            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.vlan.modifyVlan = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.vlan.vlanId     = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
                            "%d, %d, defIpv4UcNextHopInfoPtr->pclIpUcAction.vlan.vlanId = %d",
                            lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.vlan.vlanId);

            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.vlan.modifyVlan = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
            vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.vlan.vlanId     = IP_LPM_TESTED_VLAN_ID_CNS;

            vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.vlan.modifyVlan = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
            vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.vlan.vlanId     = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
                            "%d, %d, defIpv6UcNextHopInfoPtr->pclIpUcAction.vlan.vlanId = %d",
                            lpmDBId, vrId, vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.vlan.vlanId);

            vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.vlan.modifyVlan = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
            vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.vlan.vlanId     = IP_LPM_TESTED_VLAN_ID_CNS;

            /*
                1.29. Call with lpmDBId [0],
                         defUcNextHopInfoPtr->pclIpUcAction.vlan[{egressTaggedModify = GT_FALSE;
                                                                  modifyVlan = wrong enum values}]
                         and other parameters the same as in 1.2.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                                (lpmDBId, vrId, &vrConfigInfo),
                                vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction.vlan.modifyVlan);

            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmVirtualRouterAdd
                                (lpmDBId, vrId, &vrConfigInfo),
                                vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction.vlan.modifyVlan);
        }
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmVirtualRouterDel
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmVirtualRouterDel)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Create default LPM configuration and add virtual routers
         with prvUtfCreateLpmDBAndVirtualRouterAdd.
    Expected: GT_OK.
    1.2. Call with lpmDBId [0], vrId[0].
    Expected: GT_OK.
    1.3. Call with lpmDBId [0] and already removed vrId[0].
    Expected: NOT GT_OK.
    1.4. Call with lpmDBId [0] and out of range vrId[1].
    Expected: NOT GT_OK.
    1.5. Call with not valid LPM DB id lpmDBId [10], and vrId [0].
    Expected: NOT GT_OK.
*/
    GT_STATUS st      = GT_OK;
    GT_U8     dev;

    GT_U32    lpmDBId = 0;
    GT_U32    vrId    = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Create default LPM configuration and add virtual routers
                 with prvUtfCreateLpmDBAndVirtualRouterAdd.
            Expected: GT_OK.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.2. Call with lpmDBId [0], vrId[0].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /*
            1.3. Call with lpmDBId [0] and already removed vrId[0].
            Expected: NOT GT_OK.
        */
        st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /*
            1.4. Call with lpmDBId [0] and out of range vrId[10].
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.5. Call with not valid LPM DB id lpmDBId [10], and vrId [0].
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAdd
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPADDR                                ipAddr,
    IN GT_U32                                   prefixLen,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *nextHopInfoPtr,
    IN GT_BOOL                                  override
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv4UcPrefixAdd)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Create default LPM configuration and add virtual routers
         with prvUtfCreateLpmDBAndVirtualRouterAdd.
    Expected: GT_OK.
    1.2. Call with lpmDBId [1], vrId[0], ipAddr[10.16.0.1], prefixLen[32],
        nextHopInfoPtr->ipLttEntry{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
        numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
        sipSaCheckMismatchEnable[GT_TRUE], ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
    Expected: GT_OK.
    1.3. Call with lpmDBId [1], vrId[0], ipAddr[10.16.0.1], prefixLen[32],
           nextHopInfoPtr->ipLttEntry{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                                      numOfPaths[0],
                                      routeEntryBaseIndex[0],
                                      ucRPFCheckEnable[GT_TRUE],
                                      sipSaCheckMismatchEnable[GT_TRUE],
                                      ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
    Expected: GT_OK
    1.4. Call with override [GT_FALSE] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.5. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.6. Call with not supported vrId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.7. Call with out of range prefixLen [33] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.8. Call with lpmDBId [1] wrong enum values nextHopInfoPtr->ipLttEntry.routeType
        and other parameters from 1.5.
    Expected: GT_BAD_PARAM.
    1.9. Call with lpmDBId [1], correct nextHopInfoPtr->ipLttEntry.numOfPaths
                     ([5 / 7] - for lion and above and [49 / 63] for other)
                     and other parameters from 1.5.
    Expected: GT_OK.
    1.10. Call with lpmDBId [1],
                     out of range nextHopInfoPtr->ipLttEntry.numOfPaths
                     ([64 / 65] - for lion and above and [8 / 9] for other)
                     and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.11. Call with lpmDBId [1] out of range nextHopInfoPtr->ipLttEntry.
        numOfPaths[routeEntryBaseIndex] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.12. Call with lpmDBId [1] out of range nextHopInfoPtr->ipLttEntry.
        ipv6MCGroupScopeLevel [wrong enum values] and other parameters from 1.5.
    Expected: GT_BAD_PARAM.
    1.13. Call with lpmDBId [1] nextHopInfoPtr[NULL] and other parameters from 1.5.
    Expected: GT_BAD_PTR.
    1.14. Call with lpmDBId [0] wrong enum values nextHopInfoPtr->pclIpUcAction.pktCmdp
        and other parameters from 1.4.
    Expected: GT_BAD_PARAM.
    1.15. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0 / 1] and vrId[0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                                 lpmDBId     = 0;
    GT_U32                                 vrId        = 0;
    GT_IPADDR                              ipAddr;
    GT_U32                                 prefixLen   = 0;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT nextHopInfo;
    GT_BOOL                                override    = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. Create default LPM configuration and add virtual routers
                 with prvUtfCreateLpmDBAndVirtualRouterAdd.
            Expected: GT_OK.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.2. Call with lpmDBId [0], vrId[0], ipAddr[10.15.0.1], prefixLen[32],
                   nextHopInfoPtr->pclIpUcAction{pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                             mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                                    mirrorToRxAnalyzerPort = GT_FALSE},
                             matchCounter{enableMatchCount = GT_FALSE,
                                          matchCounterIndex = 0},
                             qos{egressPolicy = GT_FALSE, modifyDscp = GT_FALSE, modifyUp = GT_FALSE,
                                 qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                                        profilePrecedence=GT_FALSE] ] },
                             redirect {CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                             data[routerLttIndex=0] },
                             policer { policerEnable=GT_FALSE, policerId=0 },
                             vlan {egressTaggedModify=GT_FALSE,
                                   modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                                   nestedVlan=GT_FALSE, vlanId=100,
                                   precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                              ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
                           override [GT_TRUE].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipAddr.arIP[0] = 10;
        ipAddr.arIP[1] = 15;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 1;

        prefixLen = 32;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&nextHopInfo.ipLttEntry);
        }

        override  = GT_TRUE;

        st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, override = %d",
                                        lpmDBId, vrId, prefixLen, override);

        /*
            1.3. Call with lpmDBId [1], vrId[0], ipAddr[10.16.0.1], prefixLen[32],
                   nextHopInfoPtr->ipLttEntry{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                                              numOfPaths[0],
                                              routeEntryBaseIndex[0],
                                              ucRPFCheckEnable[GT_TRUE],
                                              sipSaCheckMismatchEnable[GT_TRUE],
                                              ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
            Expected: GT_OK
        */
        lpmDBId = 1;

        ipAddr.arIP[0] = 10;
        ipAddr.arIP[1] = 16;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 1;

        st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, override = %d",
                                        lpmDBId, vrId, prefixLen, override);

        /*
            1.4. Call with override [GT_FALSE] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        override = GT_FALSE;

        st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, override = %d", lpmDBId, vrId, override);

        override = GT_TRUE;

        /*
            1.5. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.6. Call with not supported vrId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.7. Call with out of range prefixLen [33] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        prefixLen = 33;

        st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, prefixLen = %d", lpmDBId, prefixLen);

        prefixLen = 32;

        if (CPSS_PP_FAMILY_CHEETAH_E != PRV_CPSS_PP_MAC(dev)->devFamily && !prvUtfIsPbrModeUsed())
        {
            /*
                1.8. Call with lpmDBId [1]
                                wrong enum values nextHopInfoPtr->ipLttEntry.routeType
                                and other parameters from 1.5.
                Expected: GT_BAD_PARAM.
            */
            lpmDBId = 1;
            prvUtfSetDefaultIpLttEntry(&nextHopInfo.ipLttEntry);

            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmIpv4UcPrefixAdd
                                (lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override),
                                nextHopInfo.ipLttEntry.routeType);

            /*
                1.9. Call with lpmDBId [1], correct nextHopInfoPtr->ipLttEntry.numOfPaths
                                 ([5 / 7] - for lion and above and [49 / 63] for other)
                                 and other parameters from 1.5.
                Expected: GT_OK.
            */
            /*call with 49 or 5 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                nextHopInfo.ipLttEntry.numOfPaths = 49;
            }
            else
            {
                nextHopInfo.ipLttEntry.numOfPaths = 5;
            }

            st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                                    &nextHopInfo, override);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, nextHopInfo.ipLttEntry.numOfPaths);

            /*call with 63 or 7 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                nextHopInfo.ipLttEntry.numOfPaths = 63;
            }
            else
            {
                nextHopInfo.ipLttEntry.numOfPaths = 7;
            }

            st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                                    &nextHopInfo, override);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, nextHopInfo.ipLttEntry.numOfPaths);


            /*
                1.10. Call with lpmDBId [1],
                                 out of range nextHopInfoPtr->ipLttEntry.numOfPaths
                                 ([64 / 65] - for lion and above and [8 / 9] for other)
                                 and other parameters from 1.5.
                Expected: NOT GT_OK.
            */
            /*call with 64 or 8 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                nextHopInfo.ipLttEntry.numOfPaths = 64;
            }
            else
            {
                nextHopInfo.ipLttEntry.numOfPaths = 8;
            }

            st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                                    &nextHopInfo, override);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, nextHopInfo.ipLttEntry.numOfPaths);

            /*call with 65 or 9 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                nextHopInfo.ipLttEntry.numOfPaths = 65;
            }
            else
            {
                nextHopInfo.ipLttEntry.numOfPaths = 9;
            }

            st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                                    &nextHopInfo, override);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, nextHopInfo.ipLttEntry.numOfPaths);

            nextHopInfo.ipLttEntry.numOfPaths = 0;

            /*
                1.11. Call with lpmDBId [1]
                                 out of range nextHopInfoPtr->ipLttEntry.routeEntryBaseIndex[4096]
                                 and other parameters from 1.5.
                Expected: NOT GT_OK.
            */
            nextHopInfo.ipLttEntry.routeEntryBaseIndex =
                                PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.routerNextHop;

            st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr,
                                              prefixLen, &nextHopInfo, override);

            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                        "%d, %d, nextHopInfoPtr->ipLttEntry.routeEntryBaseIndex",
                        lpmDBId, vrId, nextHopInfo.ipLttEntry.routeEntryBaseIndex);

            nextHopInfo.ipLttEntry.routeEntryBaseIndex = 0;

            /*
                1.12. Call with lpmDBId [1]
                                 wrong enum values nextHopInfoPtr->ipLttEntry.ipv6MCGroupScopeLevel
                                 and other parameters from 1.5.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmIpv4UcPrefixAdd
                                (lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override),
                                nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel);

            /*
                1.13. Call with lpmDBId [1] nextHopInfoPtr[NULL] and other parameters from 1.5.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, NULL, override);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, %d, nextHopInfoPtr = NULL", lpmDBId, vrId);
        }

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            /*
                1.14. Call with lpmDBId [0]
                         wrong enum values nextHopInfoPtr->pclIpUcAction.pktCmdp
                         and other parameters from 1.4.
                Expected: GT_BAD_PARAM.
            */
            lpmDBId = 0;
            prvUtfSetDefaultActionEntry(&nextHopInfo.pclIpUcAction);

            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmIpv4UcPrefixAdd
                                (lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override),
                                nextHopInfo.pclIpUcAction.pktCmd);
        }

        /*
            1.15. Call cpssDxChIpLpmIpv4UcPrefixesFlush with
                  lpmDBId [0 / 1] and vrId[0] to invalidate changes.
            Expected: GT_OK.
        */
        vrId = 0;

        /* Call with lpmDBId[0] */
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /* Call with lpmDBId[1] */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAddBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArray
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv4UcPrefixAddBulk)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
         indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
         tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
         numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0],
         defUcNextHopInfoPtr{ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
         mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
         matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
         qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
         qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
         redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] },
         policer { policerEnable=GT_FALSE, policerId=0 },
         vlan { egressTaggedModify=GT_FALSE, modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
         nestedVlan=GT_FALSE, vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
         ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
         defMcRouteLttEntryPtr [NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK Cheetha else GT_BAD_PRT
    1.3. Call with lpmDBId[0], ipv4PrefixArrayLen[1], ipv4PrefixArray{vrId[0],
         ipAddr[10.15.0.1], prefixLen[32], nextHopInfoPtr{ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
         mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
         matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
         qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
         qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ]},
         redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] },
         policer { policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
         modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
         precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
         ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
         numOfEcmpWaRouteEntries[1], override[GT_TRUE], returnStatus[0]}
    Expected: GT_OK.
    1.4. Call with not supported ipv4PrefixArray->vrId[10] and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.5. Call with out of range ipv4PrefixArray->prefixLen [33] and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.6. Call with lpmDBId [1], correct nextHopInfoPtr->ipLttEntry.numOfPaths
                     ([5 / 7] - for lion and above and [49 / 63] for other)
                     and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.7. Call with lpmDBId [1],
                     out of range nextHopInfoPtr->ipLttEntry.numOfPaths
                     ([64 / 65] - for lion and above and [8 / 9] for other)
                     and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.9. Call with ipv4PrefixArray[NULL] and other parameters from 1.3.
    Expected: GT_BAD_PTR.
    1.10. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0] and vrId[0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                              lpmDBId   = 0;
    GT_U32                              prefixLen = 0;
    CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC ipv4PrefixArray;
    GT_U32                              vrId      = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call with lpmDBId[0], ipv4PrefixArrayLen[1], ipv4PrefixArray{vrId[0],
                   ipAddr[10.15.0.1], prefixLen[32],
                   nextHopInfoPtr{ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                   mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                   mirrorToRxAnalyzerPort = GT_FALSE},
                   matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
                   qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
                   qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                   profilePrecedence=GT_FALSE] ] },
                   redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] },
                   policer { policerEnable=GT_FALSE, policerId=0 },
                   vlan { egressTaggedModify=GT_FALSE,
                   modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                   vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                   ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
                   numOfEcmpWaRouteEntries[1], override[GT_TRUE], returnStatus[0]}
            Expected: GT_OK.
        */
        lpmDBId   = 0;
        prefixLen = 1;

        ipv4PrefixArray.ipAddr.arIP[0] = 10;
        ipv4PrefixArray.ipAddr.arIP[1] = 15;
        ipv4PrefixArray.ipAddr.arIP[2] = 0;
        ipv4PrefixArray.ipAddr.arIP[3] = 1;

        ipv4PrefixArray.prefixLen = 32;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&ipv4PrefixArray.nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&ipv4PrefixArray.nextHopInfo.ipLttEntry);
        }

        ipv4PrefixArray.numOfEcmpWaRouteEntries = 1;
        ipv4PrefixArray.override                = GT_TRUE;
        ipv4PrefixArray.returnStatus            = GT_OK;
        ipv4PrefixArray.vrId = 0;

        st = cpssDxChIpLpmIpv4UcPrefixAddBulk(lpmDBId, prefixLen, &ipv4PrefixArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, prefixLen);

        /*
            1.4. Call with not supported ipv4PrefixArray->vrId[10]
                 and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        ipv4PrefixArray.vrId = 10;

        st = cpssDxChIpLpmIpv4UcPrefixAddBulk(lpmDBId, prefixLen, &ipv4PrefixArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipv4PrefixArray->vrId = %d",
                                         lpmDBId, ipv4PrefixArray.vrId);

        ipv4PrefixArray.vrId = 0;

        /*
            1.5. Call with out of range ipv4PrefixArray->prefixLen [33]
                 and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        ipv4PrefixArray.prefixLen = 33;

        st = cpssDxChIpLpmIpv4UcPrefixAddBulk(lpmDBId, prefixLen, &ipv4PrefixArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipv4PrefixArray->prefixLen = %d",
                                         lpmDBId, ipv4PrefixArray.prefixLen);

        ipv4PrefixArray.prefixLen = 32;

        if (CPSS_PP_FAMILY_CHEETAH_E != PRV_CPSS_PP_MAC(dev)->devFamily && !prvUtfIsPbrModeUsed())
        {
            /*
                1.6. Call with lpmDBId [1], correct nextHopInfoPtr->ipLttEntry.numOfPaths
                                 ([5 / 7] - for lion and above and [49 / 63] for other)
                                 and other parameters from 1.3.
                Expected: GT_OK.
            */
            /*call with 49 or 5 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                ipv4PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 49;
            }
            else
            {
                ipv4PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 5;
            }

            st = cpssDxChIpLpmIpv4UcPrefixAddBulk(lpmDBId, prefixLen, &ipv4PrefixArray);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, ipv4PrefixArray.nextHopInfo.ipLttEntry.numOfPaths);

            /*call with 63 or 7 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                ipv4PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 63;
            }
            else
            {
                ipv4PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 7;
            }

            st = cpssDxChIpLpmIpv4UcPrefixAddBulk(lpmDBId, prefixLen, &ipv4PrefixArray);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, ipv4PrefixArray.nextHopInfo.ipLttEntry.numOfPaths);

            /*
                1.7. Call with lpmDBId [1],
                                 out of range nextHopInfoPtr->ipLttEntry.numOfPaths
                                 ([64 / 65] - for lion and above and [8 / 9] for other)
                                 and other parameters from 1.3.
                Expected: NOT GT_OK.
            */
            /*call with 64 or 8 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                ipv4PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 64;
            }
            else
            {
                ipv4PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 8;
            }

            st = cpssDxChIpLpmIpv4UcPrefixAddBulk(lpmDBId, prefixLen, &ipv4PrefixArray);

            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, ipv4PrefixArray.nextHopInfo.ipLttEntry.numOfPaths);

            /*call with 65 or 9 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                ipv4PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 65;
            }
            else
            {
                ipv4PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 9;
            }

            st = cpssDxChIpLpmIpv4UcPrefixAddBulk(lpmDBId, prefixLen, &ipv4PrefixArray);

            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, ipv4PrefixArray.nextHopInfo.ipLttEntry.numOfPaths);

            ipv4PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 0;
        }

        /*
            1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv4UcPrefixAddBulk(lpmDBId, prefixLen, &ipv4PrefixArray);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.9. Call with ipv4PrefixArray[NULL] and other parameters from 1.3.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv4UcPrefixAddBulk(lpmDBId, prefixLen, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipv4PrefixArray = NULL", lpmDBId);

        /*
            1.10. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0 / 1]
                 and vrId[0] to invalidate changes.
            Expected: GT_OK.
        */
        vrId = 0;

        /* Call with lpmDBId[0] */
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /* Call with lpmDBId[1] */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv4UcPrefixDel
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               vrId,
    IN GT_IPADDR                            ipAddr,
    IN GT_U32                               prefixLen
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv4UcPrefixDel)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0], defUcNextHopInfoPtr{
        pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
        mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount = GT_FALSE,
        matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
        modifyUp=GT_FALSE , qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
        profilePrecedence=GT_FALSE] ] }, redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
        data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
        egressTaggedModify=GT_FALSE, modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
        nestedVlan=GT_FALSE, vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
        }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
        defMcRouteLttEntryPtr [NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK Cheetah else GT_BAD_PTR
    1.3. Call cpssDxChIpLpmIpv4UcPrefixAdd with lpmDBId [0], vrId[0], ipAddr[10.15.0.1],
        prefixLen[32], nextHopInfoPtr->pclIpUcAction { pktCmd = CPSS_PACKET_CMD_FORWARD_E,
        mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
        matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
        egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
        ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
        redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
        policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
        modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
        precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
        doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } }, override [GT_TRUE].
    Expected: GT_OK.
    1.4. Call with lpmDBId [0], vrId[0], ipAddr[10.15.0.1], prefixLen[32].
    Expected: GT_OK.
    1.5. For Cheetah+ call with lpmDBId [0], vrId[0], ipAddr[10.15.0.1] (already deleted), prefixLen[32].
    Expected: NOT GT_OK.
    1.6. For Cheetah+ call with lpmDBId [0], vrId[0], ipAddr[10.15.0.2] (wrong IP address), prefixLen[32].
    Expected: NOT GT_OK.
    1.7. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.8. Call with not supported vrId [10] and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.9. Call with out of range prefixLen [33] and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.10. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0] and vrId[0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                                 lpmDBId   = 0;
    GT_U32                                 vrId      = 0;
    GT_IPADDR                              ipAddr;
    GT_U32                                 prefixLen = 0;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT nextHopInfo;
    GT_BOOL                                override  = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv4UcPrefixAdd with lpmDBId [0], vrId[0], ipAddr[10.15.0.1],
                            prefixLen[32],
                            nextHopInfoPtr->pclIpUcAction { pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                            mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                            mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount
                            = GT_FALSE, matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE,
                            modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
                            ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                            profilePrecedence=GT_FALSE] ] }, redirect {
                            CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] },
                            policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
                            egressTaggedModify=GT_FALSE,
                            modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                            nestedVlan=GT_FALSE, vlanId=100,
                            precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                            ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
                            override [GT_TRUE].
            Expected: GT_OK.
        */

        lpmDBId = 0;
        vrId    = 0;

        ipAddr.arIP[0] = 10;
        ipAddr.arIP[1] = 15;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 1;

        prefixLen = 32;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&nextHopInfo.ipLttEntry);
        }

        override  = GT_TRUE;

        st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                     "cpssDxChIpLpmIpv4UcPrefixAdd: %d, %d, %d, override = %d",
                                     lpmDBId, vrId, prefixLen, override);

        /*
            1.4. Call with lpmDBId [0], vrId[0], ipAddr[10.15.0.1], prefixLen[32].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipAddr.arIP[0] = 10;
        ipAddr.arIP[1] = 15;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 1;

        prefixLen = 32;

        st = cpssDxChIpLpmIpv4UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            /*
                1.5. For Cheetah+ call with lpmDBId [0], vrId[0], ipAddr[10.15.0.1] (already
                deleted), prefixLen[32].
                Expected: NOT GT_OK.
            */
            st = cpssDxChIpLpmIpv4UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

            /*
                1.6. For Cheetah+ call with lpmDBId [0], vrId[0], ipAddr[10.15.0.2] (wrong IP
                address), prefixLen[32].
                Expected: NOT GT_OK.
            */
            ipAddr.arIP[0] = 10;
            ipAddr.arIP[1] = 15;
            ipAddr.arIP[2] = 0;
            ipAddr.arIP[3] = 2;

            st = cpssDxChIpLpmIpv4UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        /*
            1.7. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv4UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.8. Call with not supported vrId [10] and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv4UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.9. Call with out of range prefixLen [33] and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        prefixLen = 33;

        st = cpssDxChIpLpmIpv4UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, prefixLen = %d", lpmDBId, prefixLen);

        prefixLen = 32;

        /*
            1.10. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0 / 1] and vrId[0] to
            invalidate changes.
            Expected: GT_OK.
        */
        vrId = 0;

        /* Call with lpmDBId[0] */
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /* Call with lpmDBId[1] */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv4UcPrefixDelBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArray
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv4UcPrefixDelBulk)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. - 1.2.
    1.3. Call cpssDxChIpLpmIpv4UcPrefixAddBulk with lpmDBId[0],
           ipv4PrefixArrayLen[1],
           ipv4PrefixArray{vrId[0],
                           ipAddr[10.15.0.1],
                           prefixLen[32],
                           nextHopInfoPtr{ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                           mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                           mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter {
                           enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
                           egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
                           modifyUp=GT_FALSE , qos [ ingress[profileIndex=0,
                           profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ]
                           }, redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                           data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE,
                           policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
                           modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                           nestedVlan=GT_FALSE, vlanId=100,
                           precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                           ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE,
                           GT_FALSE } },
                           numOfEcmpWaRouteEntries[1],
                           override[GT_TRUE],
                           returnStatus[0]}
    Expected: GT_OK.
    1.4. Call with lpmDBId[0],
                   ipv4PrefixArrayLen[1],
                   ipv4PrefixArray{vrId[0],
                                   ipAddr[10.15.0.1],
                                   prefixLen[32],
                                   nextHopInfoPtr{pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                                   mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                                   mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter {
                                   enableMatchCount = GT_FALSE, matchCounterIndex = 0
                                   }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
                                   modifyUp=GT_FALSE , qos [ ingress[profileIndex=0,
                                   profileAssignIndex=GT_FALSE,
                                   profilePrecedence=GT_FALSE] ] }, redirect {
                                   CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                                   data[routerLttIndex=0] }, policer {
                                   policerEnable=GT_FALSE, policerId=0 }, vlan {
                                   egressTaggedModify=GT_FALSE,
                                   modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                                   nestedVlan=GT_FALSE, vlanId=100,
                                   precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
                                   }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE,
                                   GT_FALSE, GT_FALSE } },
                   numOfEcmpWaRouteEntries[1],
                   override[GT_TRUE],
                   returnStatus[0]}
    Expected: GT_OK.
        1.5. For Cheetah+ call with lpmDBId[0],
                ipv4PrefixArrayLen[1], ipv4PrefixArray{vrId[0], ipAddr[10.15.0.1],
                prefixLen[32], nextHopInfoPtr{ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount =
                GT_FALSE, matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE,
                modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [ ingress[profileIndex=0,
                profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] }, redirect
                { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] },
                policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
                egressTaggedModify=GT_FALSE,
                modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                nestedVlan=GT_FALSE, vlanId=100,
                precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute
                { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
                numOfEcmpWaRouteEntries[1], override[GT_TRUE], returnStatus[0]}
                (already deleted)
        Expected: NOT GT_OK.
    1.6. Call with not supported ipv4PrefixArray->vrId[10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.7. Call with out of range ipv4PrefixArray->prefixLen [33]
         and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.9. Call with ipv4PrefixArray[NULL] and other parameters from 1.4.
    Expected: GT_BAD_PTR.
    1.10. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0 / 1]
        and vrId[0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                              lpmDBId     = 0;
    GT_U32                              arrayLen    = 0;
    CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC ipv4Prefix;
    GT_U32                              vrId        = 0;
    CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC ipv4PrefixArray;
    GT_U32                              prefixLen   = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv4UcPrefixAddBulk with lpmDBId[0],
                   ipv4PrefixArrayLen[1],
                   ipv4PrefixArray{vrId[0],
                                   ipAddr[10.15.0.1],
                                   prefixLen[32],
                                   nextHopInfoPtr{ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                                   mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                                   mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter {
                                   enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
                                   egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
                                   modifyUp=GT_FALSE , qos [ ingress[profileIndex=0,
                                   profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ]
                                   }, redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                                   data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE,
                                   policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
                                   modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                                   nestedVlan=GT_FALSE, vlanId=100,
                                   precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                                   ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE,
                                   GT_FALSE } },
                                   numOfEcmpWaRouteEntries[1],
                                   override[GT_TRUE],
                                   returnStatus[0]}
            Expected: GT_OK.
        */
        lpmDBId   = 0;
        prefixLen = 1;

        ipv4PrefixArray.ipAddr.arIP[0] = 10;
        ipv4PrefixArray.ipAddr.arIP[1] = 15;
        ipv4PrefixArray.ipAddr.arIP[2] = 0;
        ipv4PrefixArray.ipAddr.arIP[3] = 1;

        ipv4PrefixArray.prefixLen = 32;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&ipv4PrefixArray.nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&ipv4PrefixArray.nextHopInfo.ipLttEntry);
        }

        ipv4PrefixArray.numOfEcmpWaRouteEntries = 1;
        ipv4PrefixArray.override                = GT_TRUE;
        ipv4PrefixArray.returnStatus            = GT_OK;
        ipv4PrefixArray.vrId = 0;

        st = cpssDxChIpLpmIpv4UcPrefixAddBulk(lpmDBId, prefixLen, &ipv4PrefixArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, prefixLen);

        /*
            1.4. Call with lpmDBId[0],
                           ipv4PrefixArrayLen[1],
                           ipv4PrefixArray{vrId[0],
                                           ipAddr[10.15.0.1],
                                           prefixLen[32],
                                           nextHopInfoPtr{pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                                           mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                                           mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter {
                                           enableMatchCount = GT_FALSE, matchCounterIndex = 0
                                           }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
                                           modifyUp=GT_FALSE , qos [ ingress[profileIndex=0,
                                           profileAssignIndex=GT_FALSE,
                                           profilePrecedence=GT_FALSE] ] }, redirect {
                                           CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                                           data[routerLttIndex=0] }, policer {
                                           policerEnable=GT_FALSE, policerId=0 }, vlan {
                                           egressTaggedModify=GT_FALSE,
                                           modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                                           nestedVlan=GT_FALSE, vlanId=100,
                                           precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
                                           }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE,
                                           GT_FALSE, GT_FALSE } },
                           numOfEcmpWaRouteEntries[1],
                           override[GT_TRUE],
                           returnStatus[0]}
            Expected: GT_OK.
        */
        lpmDBId  = 0;
        arrayLen = 1;

        ipv4Prefix.vrId = 0;

        ipv4Prefix.ipAddr.arIP[0] = 10;
        ipv4Prefix.ipAddr.arIP[1] = 15;
        ipv4Prefix.ipAddr.arIP[2] = 0;
        ipv4Prefix.ipAddr.arIP[3] = 1;

        ipv4Prefix.prefixLen = 32;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&ipv4PrefixArray.nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&ipv4PrefixArray.nextHopInfo.ipLttEntry);
        }

        ipv4Prefix.numOfEcmpWaRouteEntries = 1;
        ipv4Prefix.override                = GT_TRUE;
        ipv4Prefix.returnStatus            = GT_OK;

        st = cpssDxChIpLpmIpv4UcPrefixDelBulk(lpmDBId, arrayLen, &ipv4Prefix);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, arrayLen);

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            /*
                1.5. For Cheetah+ call with lpmDBId[0],
                        ipv4PrefixArrayLen[1], ipv4PrefixArray{vrId[0], ipAddr[10.15.0.1],
                        prefixLen[32], nextHopInfoPtr{ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                        mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                        mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount =
                        GT_FALSE, matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE,
                        modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [ ingress[profileIndex=0,
                        profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] }, redirect
                        { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] },
                        policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
                        egressTaggedModify=GT_FALSE,
                        modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                        nestedVlan=GT_FALSE, vlanId=100,
                        precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute
                        { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
                        numOfEcmpWaRouteEntries[1], override[GT_TRUE], returnStatus[0]}
                        (already deleted)
                Expected: NOT GT_OK.
            */
            st = cpssDxChIpLpmIpv4UcPrefixDelBulk(lpmDBId, arrayLen, &ipv4Prefix);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, arrayLen);
        }

        /*
            1.6. Call with not supported ipv4PrefixArray->vrId[10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        ipv4Prefix.vrId = 10;

        st = cpssDxChIpLpmIpv4UcPrefixDelBulk(lpmDBId, arrayLen, &ipv4Prefix);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                "%d, ipv4PrefixArray->vrId = %d", lpmDBId, ipv4Prefix.vrId);

        ipv4Prefix.vrId = 0;

        /*
            1.7. Call with out of range ipv4PrefixArray->prefixLen [33]
                 and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        ipv4Prefix.prefixLen = 33;

        st = cpssDxChIpLpmIpv4UcPrefixDelBulk(lpmDBId, arrayLen, &ipv4Prefix);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                    "%d, ipv4PrefixArray->prefixLen = %d", lpmDBId, ipv4Prefix.prefixLen);

        ipv4Prefix.prefixLen = 32;

        /*
            1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv4UcPrefixDelBulk(lpmDBId, arrayLen, &ipv4Prefix);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.9. Call with ipv4PrefixArray[NULL] and other parameters from 1.4.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv4UcPrefixDelBulk(lpmDBId, arrayLen, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, ipv4PrefixArray = NULL", lpmDBId);

        /*
            1.10. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0 / 1]
                and vrId[0] to invalidate changes.
            Expected: GT_OK.
        */
        vrId = 0;

        /* Call with lpmDBId[0] */
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /* Call with lpmDBId[1] */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv4UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv4UcPrefixesFlush)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0], defUcNextHopInfoPtr{
        pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
        mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount = GT_FALSE,
        matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
        modifyUp=GT_FALSE , qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
        profilePrecedence=GT_FALSE] ] }, redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
        data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
        egressTaggedModify=GT_FALSE, modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
        nestedVlan=GT_FALSE, vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
        }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
        defMcRouteLttEntryPtr [NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK Cheetha else GT_BAD_PRT
    1.3. Call with lpmDBId [0], vrId[0].
    Expected: GT_OK.
    1.4. Call with not supported vrId [1] and other parameters from 1.3.
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                   lpmDBId = 0;
    GT_U32                   vrId    = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call with lpmDBId [0], vrId[0].
            Expected: GT_OK.
        */
        lpmDBId = 1;
        vrId    = 0;

        st = cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId, vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        /*
            1.4. Call with not supported vrId [1] and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        vrId = 0;
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId, vrId);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv4UcPrefixSearch
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    IN  GT_U32                                  prefixLen,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *rowIndexPtr,
    OUT GT_U32                                  *columnIndexPtr

)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv4UcPrefixSearch)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0], defUcNextHopInfoPtr{
        pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
        mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount = GT_FALSE,
        matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
        modifyUp=GT_FALSE , qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
        profilePrecedence=GT_FALSE] ] }, redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
        data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
        egressTaggedModify=GT_FALSE, modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
        nestedVlan=GT_FALSE, vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
        }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
        defMcRouteLttEntryPtr [NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK Cheetha else GT_BAD_PRT
    1.3. Call cpssDxChIpLpmIpv4UcPrefixAdd with lpmDBId [0], vrId[0], ipAddr[10.15.0.1],
        prefixLen[32], nextHopInfoPtr->pclIpUcAction { pktCmd = CPSS_PACKET_CMD_FORWARD_E,
        mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
        matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
        egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
        ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
        redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
        policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
        modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
        precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
        doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } }, override [GT_TRUE].
    Expected: GT_OK.
    1.4. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.1], prefixLen[32] and non-null nextHopInfoPtr
    Expected: GT_OK and the same nextHopInfoPtr.
    1.5. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.1], prefixLen[24] and non-null nextHopInfoPtr
    Expected: GT_OK for Cheetah+ and NOT GT_OK for others and the same nextHopInfoPtr.
    1.6. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.255], prefixLen[24] and non-null nextHopInfoPtr
    Expected: GT_OK for Cheetah+ and NOT GT_OK for others and the same nextHopInfoPtr.
    1.7. For Cheetah+ call with lpmDBId[0], vrId[0], ipAddr[10.15.0.255], prefixLen[32] and
        non-null nextHopInfoPtr
    Expected: NOT GT_OK.
    1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.9. Call with not supported vrId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.10. Call with out of range prefixLen [33] and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.11. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.1], prefixLen[32] and nextHopInfoPtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                                 lpmDBId     = 0;
    GT_U32                                 vrId        = 0;
    GT_IPADDR                              ipAddr;
    GT_U32                                 prefixLen   = 0;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT nextHopInfo;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT nextHopInfoGet;
    GT_BOOL                                override    = GT_FALSE;
    GT_U32                                 rowIndex;
    GT_U32                                 columnIndex;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv4UcPrefixAdd with lpmDBId [0], vrId[0], ipAddr[10.15.0.1],
                      prefixLen[32], nextHopInfoPtr->pclIpUcAction { pktCmd =
                      CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode =
                      CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
                      matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos
                      { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
                      ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                      profilePrecedence=GT_FALSE] ] }, redirect {
                      CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] },
                      policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
                      egressTaggedModify=GT_FALSE,
                      modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                      vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                      ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
                      override [GT_TRUE].
            Expected: GT_OK.
        */

        lpmDBId = 0;
        vrId    = 0;

        ipAddr.arIP[0] = 10;
        ipAddr.arIP[1] = 15;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 1;

        prefixLen = 32;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&nextHopInfo.ipLttEntry);
        }

        override = GT_TRUE;

        st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChIpLpmIpv4UcPrefixAdd: %d, %d, %d, override = %d",
                                     lpmDBId, vrId, prefixLen, override);

        /*
            1.4. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.1], prefixLen[32] and non-null nextHopInfoPtr
            Expected: GT_OK and the same nextHopInfoPtr.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipAddr.arIP[0] = 10;
        ipAddr.arIP[1] = 15;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 1;

        prefixLen = 32;

        st = cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                             &nextHopInfoGet,&rowIndex,&columnIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDBId, vrId, prefixLen);

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfComparePclIpUcActionEntries(lpmDBId, vrId, &nextHopInfoGet.pclIpUcAction,
                                                                &nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfCompareIpLttEntries(lpmDBId, vrId, &nextHopInfoGet.ipLttEntry, &nextHopInfo.ipLttEntry);
        }

        /*
            1.5. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.1], prefixLen[24] and non-null
                nextHopInfoPtr
            Expected: GT_OK for Cheetah+ and NOT GT_OK for others and the same nextHopInfoPtr.
        */
        prefixLen = 24;

        st = cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                             &nextHopInfoGet,&rowIndex,&columnIndex);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDBId, vrId, prefixLen);

        /*
            1.6. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.255], prefixLen[24] and non-null nextHopInfoPtr
            Expected: GT_OK for Cheetah+ and NOT GT_OK for others and the same nextHopInfoPtr.
        */
        prefixLen = 24;

        ipAddr.arIP[0] = 10;
        ipAddr.arIP[1] = 15;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 255;

        st = cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                             &nextHopInfoGet,&rowIndex,&columnIndex);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDBId, vrId, prefixLen);

        /*
            1.7. For Cheetah+ call with lpmDBId[0], vrId[0], ipAddr[10.15.0.255], prefixLen[32]
                and non-null nextHopInfoPtr
            Expected: NOT GT_OK.
        */
        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prefixLen = 32;

            st = cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                                  &nextHopInfoGet,&rowIndex,&columnIndex);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDBId, vrId, prefixLen);
        }

        /*
            1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                             &nextHopInfoGet,&rowIndex,&columnIndex);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.9. Call with not supported vrId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */

        vrId = 10;

        st = cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                             &nextHopInfoGet,&rowIndex,&columnIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.10. Call with out of range prefixLen [33] and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        prefixLen = 33;

        st = cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                             &nextHopInfoGet,&rowIndex,&columnIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, prefixLen = %d", lpmDBId, prefixLen);

        prefixLen = 32;

        /*
            1.10. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.1], prefixLen[32] and nextHopInfoPtr[NULL].
            Expected: GT_BAD_PTR.
        */
        prefixLen = 32;

        ipAddr.arIP[0] = 10;
        ipAddr.arIP[1] = 15;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 1;

        st = cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                             NULL,&rowIndex,&columnIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nextHopInfoPtr = NULL", lpmDBId, vrId);

        /*
            1.11. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0] and vrId[0] to invalidate changes.
            Expected: GT_OK.
        */
        vrId    = 0;
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv4UcPrefixGetNext
(
    IN    GT_U32                                    lpmDBId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPADDR                                 *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr,
    OUT GT_U32                                      *rowIndexPtr,
    OUT GT_U32                                      *columnIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv4UcPrefixGetNext)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0], defUcNextHopInfoPtr{
        pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
        mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount = GT_FALSE,
        matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
        modifyUp=GT_FALSE , qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
        profilePrecedence=GT_FALSE] ] }, redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
        data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
        egressTaggedModify=GT_FALSE, modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
        nestedVlan=GT_FALSE, vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
        }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
        defMcRouteLttEntryPtr [NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK Cheetha else GT_BAD_PRT
    1.3. Call cpssDxChIpLpmIpv4UcPrefixAdd with lpmDBId [0], vrId[0], ipAddr[10.15.0.1],
        prefixLen[32], nextHopInfoPtr->pclIpUcAction { pktCmd = CPSS_PACKET_CMD_FORWARD_E,
        mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
        matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
        egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
        ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
        redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
        policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
        modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
        precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
        doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } }, override [GT_TRUE].
    Expected: GT_OK.
    1.4. Call cpssDxChIpLpmIpv4UcPrefixAdd with lpmDBId [0], vrId[0], ipAddr[10.15.0.2],
        prefixLen[32], nextHopInfoPtr->pclIpUcAction { pktCmd = CPSS_PACKET_CMD_FORWARD_E,
        mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
        matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
        egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
        ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
        redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
        policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
        modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
        precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
        doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } }, override [GT_TRUE].
    Expected: GT_OK.
    1.5. Call with lpmDBId[0], vrId[0], ipAddrPtr [10.15.0.1], prefixLenPtr [32] and non-null
        nextHopInfoPtr
    Expected: GT_OK and the same prefixLenPtr  and nextHopInfoPtr.
    1.6. For Cheetah+ call with lpmDBId[0], vrId[0], ipAddrPtr [10.15.0.2], prefixLenPtr [32]
    and non-null nextHopInfoPtr
    Expected: NOT GT_OK (the last prefix reached).
    1.7. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.8. Call with not supported vrId [10] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.9. Call with out of range prefixLenPtr [33] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.10. Call with lpmDBId[0], vrId[0], ipAddrPtr [NULL] and other parameters from 1.5.
    Expected: GT_BAD_PTR.
    1.11. Call with lpmDBId[0], vrId[0], prefixLenPtr [NULL] and other parameters from 1.5.
    Expected: GT_BAD_PTR.
    1.12. Call with lpmDBId[0], vrId[0], nextHopInfoPtr [NULL] and other parameters from 1.5.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                                 lpmDBId     = 0;
    GT_U32                                 vrId        = 0;
    GT_IPADDR                              ipAddr;
    GT_IPADDR                              ipAddrTmp;
    GT_U32                                 prefixLen   = 0;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT nextHopInfo;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT nextHopInfoGet;
    GT_BOOL                                override    = GT_FALSE;
    GT_U32                                 rowIndex;
    GT_U32                                 columnIndex;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv4UcPrefixAdd with lpmDBId [0],
                 vrId[0], ipAddr[10.15.0.1], prefixLen[32], nextHopInfoPtr->pclIpUcAction {
                 pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode =
                 CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
                 matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
                 egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
                 ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                 profilePrecedence=GT_FALSE] ] }, redirect {
                 CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
                 policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
                 modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                 vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                 ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
                 override [GT_TRUE].
            Expected: GT_OK.
        */

        lpmDBId = 0;
        vrId    = 0;

        ipAddr.arIP[0] = 10;
        ipAddr.arIP[1] = 15;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 1;

        prefixLen = 32;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&nextHopInfo.ipLttEntry);
        }

        override  = GT_TRUE;

        st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChIpLpmIpv4UcPrefixAdd: %d, %d, %d, override = %d",
                                     lpmDBId, vrId, prefixLen, override);

        /*
            1.4. Call cpssDxChIpLpmIpv4UcPrefixAdd with lpmDBId [0],
                 vrId[0], ipAddr[10.15.0.2], prefixLen[32], nextHopInfoPtr->pclIpUcAction {
                 pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode =
                 CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
                 matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
                 egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
                 ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                 profilePrecedence=GT_FALSE] ] }, redirect {
                 CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
                 policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
                 modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                 vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                 ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
                 override [GT_TRUE].
            Expected: GT_OK.
        */

        lpmDBId = 0;
        vrId    = 0;

        ipAddr.arIP[0] = 10;
        ipAddr.arIP[1] = 15;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 2;

        prefixLen = 32;

        override  = GT_TRUE;

        st = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChIpLpmIpv4UcPrefixAdd: %d, %d, %d, override = %d",
                                     lpmDBId, vrId, prefixLen, override);

        /*
            1.5. Call with lpmDBId[0], vrId[0], ipAddrPtr [10.15.0.1],
                 prefixLenPtr [32] and non-null nextHopInfoPtr
            Expected: GT_OK and the same prefixLenPtr  and nextHopInfoPtr.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipAddr.arIP[0] = 10;
        ipAddr.arIP[1] = 15;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 1;

        ipAddrTmp.arIP[0] = 10;
        ipAddrTmp.arIP[1] = 15;
        ipAddrTmp.arIP[2] = 0;
        ipAddrTmp.arIP[3] = 2;

        prefixLen = 32;

        st = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, &ipAddr, &prefixLen,
                                               &nextHopInfoGet, &rowIndex, &columnIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfComparePclIpUcActionEntries(lpmDBId, vrId,
                                    &nextHopInfoGet.pclIpUcAction, &nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfCompareIpLttEntries(lpmDBId, vrId, &nextHopInfoGet.ipLttEntry,
                                                        &nextHopInfo.ipLttEntry);
        }

        /* Verifying ipAddr */
        UTF_VERIFY_EQUAL2_STRING_MAC(ipAddr.arIP[0], ipAddrTmp.arIP[0],
                   "get another ipAddrPtr[0] than was set: %d, %d", lpmDBId, vrId);
        UTF_VERIFY_EQUAL2_STRING_MAC(ipAddr.arIP[1], ipAddrTmp.arIP[1],
                   "get another ipAddrPtr[1] than was set: %d, %d", lpmDBId, vrId);
        UTF_VERIFY_EQUAL2_STRING_MAC(ipAddr.arIP[2], ipAddrTmp.arIP[2],
                   "get another ipAddrPtr[2] than was set: %d, %d", lpmDBId, vrId);
        UTF_VERIFY_EQUAL2_STRING_MAC(ipAddr.arIP[3], ipAddrTmp.arIP[3],
                   "get another ipAddrPtr[3] than was set: %d, %d", lpmDBId, vrId);

        /*
            1.6. For Cheetah+ call with lpmDBId[0], vrId[0], ipAddrPtr [10.15.0.2],
            prefixLenPtr [32] and non-null nextHopInfoPtr
            Expected: NOT GT_OK (the last prefix reached).
        */
        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            ipAddr.arIP[0] = 10;
            ipAddr.arIP[1] = 15;
            ipAddr.arIP[2] = 0;
            ipAddr.arIP[3] = 2;

            st = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, &ipAddr, &prefixLen,
                                                   &nextHopInfoGet, &rowIndex, &columnIndex);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        /*
            1.7. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, &ipAddr, &prefixLen,
                                              &nextHopInfoGet, &rowIndex, &columnIndex);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.8. Call with not supported vrId [10] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */

        vrId = 10;

        st = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, &ipAddr, &prefixLen,
                                               &nextHopInfoGet, &rowIndex, &columnIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.9. Call with out of range prefixLenPtr [33] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        prefixLen = 33;

        st = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, &ipAddr, &prefixLen,
                                               &nextHopInfoGet, &rowIndex, &columnIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, prefixLen = %d", lpmDBId, prefixLen);

        prefixLen = 32;

        /*
            1.10. Call with lpmDBId[0], vrId[0], ipAddrPtr [NULL] and other parameters from 1.5.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, NULL, &prefixLen,
                                              &nextHopInfoGet, &rowIndex, &columnIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ipAddrPtr = NULL", lpmDBId, vrId);

        /*
            1.11. Call with lpmDBId[0], vrId[0], prefixLenPtr [NULL] and other parameters from 1.5.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, &ipAddr, NULL,
                                              &nextHopInfoGet, &rowIndex, &columnIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, prefixLenPtr = NULL", lpmDBId, vrId);

        /*
            1.12. Call with lpmDBId[0], vrId[0], nextHopInfoPtr [NULL] and other parameters from 1.5.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, &ipAddr, &prefixLen,
                                               NULL, &rowIndex, &columnIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nextHopInfoPtr = NULL", lpmDBId, vrId);

        /*
            1.13. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0]
                  and vrId[0] to invalidate changes.
            Expected: GT_OK.
        */
        vrId    = 0;
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv4McEntryAdd
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPADDR                    ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPADDR                    ipSrc,
    IN GT_U32                       ipSrcPrefixLen,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC   *mcRouteLttEntryPtr,
    IN GT_BOOL                      override,
    IN GT_BOOL                      defragmentationEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv4McEntryAdd)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. - 1.3.
    1.4. Call with lpmDBId[0 / 1], vrId[0], ipGroup[224.15.0.1], ipGroupPrefixLen[32],
        ipSrc[224.16.0.1], ipSrcPrefixLen[32],
        mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0],
        routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
        sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, override[GT_TRUE],
        defragmentationEnable[GT_FALSE].
    Expected: GT_OK (no any constraints for any devices).
    1.5. Call with lpmDBId [1],
                                vrId[0],
                                wrong enum values mcRouteLttEntryPtr->routeType
                                and othe parameters from 1.4.
    Expected: GT_BAD_PARAM.
    1.6. Call with lpmDBId [1], vrId[0],
                                out of range mcRouteLttEntryPtr->numOfPaths [8 / 63]
                                and othe parameters from 1.4.
    Expected: GT_OK for Lion and above and not GT_OK for other.
    1.7. Call with lpmDBId [1], vrId[0],
                                out of range mcRouteLttEntryPtr->numOfPaths [64]
                                and othe parameters from 1.4.
    Expected: NOT GT_OK
    1.8. Call with lpmDBId [1], vrId[0],
                                out of range mcRouteLttEntryPtr->routeEntryBaseIndex [4096]
                                and othe parameters from 1.4.
    Expected: NOT GT_OK.
    1.9. Call with lpmDBId [1], vrId[0],
                                wrong enum values mcRouteLttEntryPtr->ipv6MCGroupScopeLevel
                                and othe parameters from 1.4.
    Expected: GT_BAD_PARAM.
    1.10. Call with lpmDBId [1], out of range vrId[10] and other valid parameters.
    Expected: NOT GT_OK.
    1.10. Call with lpmDBId [0], out of range vrId[10] and other valid parameters.
    Expected: NOT GT_OK.
    1.11. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.12. Call with out of range ipGroupPrefixLen [33] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.13. Call with out of range ipSrcPrefixLen [33] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.14. Call with lpmDBId [1], vrId[0], mcRouteLttEntryPtr [NULL] and other valid parameters.
    Expected: GT_BAD_PTR.
    1.15. Call cpssDxChIpLpmIpv4McEntriesFlush with with lpmDBId [0 / 1],
          vrId[0] to invalidate entries.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;



    GT_U32                     lpmDBId          = 0;
    GT_U32                     vrId             = 0;
    GT_IPADDR                  ipGroup;
    GT_U32                     ipGroupPrefixLen = 0;
    GT_IPADDR                  ipSrc;
    GT_U32                     ipSrcPrefixLen   = 0;
    CPSS_DXCH_IP_LTT_ENTRY_STC mcRouteLttEntry;
    GT_BOOL                    override         = GT_FALSE;
    GT_BOOL                    defragEnable     = GT_FALSE;

    GT_U8                      devList[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U32                     numOfDevs = 1;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        devList[0] = dev;


        /*
            1.1. - 1.3.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.4. Call with lpmDBId[0 / 1], vrId[0], ipGroup[224.15.0.1], ipGroupPrefixLen[32],
                ipSrc[224.16.0.1], ipSrcPrefixLen[32],
                mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0],
                routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
                sipSaCheckMismatchEnable[GT_TRUE],
                ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, override[GT_TRUE],
                defragmentationEnable[GT_FALSE].
            Expected: GT_OK (no any constraints for any devices).
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 224;
        ipGroup.arIP[1] = 15;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 1;

        ipGroupPrefixLen = 32;

        ipSrc.arIP[0] = 224;
        ipSrc.arIP[1] = 15;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 0;

        prvUtfSetDefaultIpLttEntry(&mcRouteLttEntry);

        override     = GT_TRUE;
        defragEnable = GT_FALSE;

        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                         ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, override = %d, defragmentationEnable = %d",
                                             lpmDBId, vrId, override, defragEnable);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, override = %d, defragmentationEnable = %d",
                                         lpmDBId, vrId, override, defragEnable);
        }

        /*
            1.5. Call with lpmDBId [1], vrId[0],
                                        wrong enum values mcRouteLttEntryPtr->routeType
                                        and othe parameters from 1.4.
            Expected: GT_BAD_PARAM.
        */
        lpmDBId = 1;
        vrId    = 0;

        if (!prvUtfIsPbrModeUsed())
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmIpv4McEntryAdd
                                (lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                 ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable),
                                mcRouteLttEntry.routeType);
        }

        /*
            1.6. Call with lpmDBId [1], vrId[0],
                                        out of range mcRouteLttEntryPtr->numOfPaths [8 / 63]
                                        and othe parameters from 1.4.
            Expected: GT_OK for Lion and above and not GT_OK for other.
        */
        /* call with mcRouteLttEntry.numOfPaths = 8 */
        mcRouteLttEntry.numOfPaths = 8;

        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                     ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);

        if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, mcRouteLttEntry.numOfPaths = %d",
                                         lpmDBId, vrId, mcRouteLttEntry.numOfPaths);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, mcRouteLttEntry.numOfPaths = %d",
                                           lpmDBId, vrId, mcRouteLttEntry.numOfPaths);
        }

        /* call with mcRouteLttEntry.numOfPaths = 63 */
        mcRouteLttEntry.numOfPaths = 63;

        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                     ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);

        if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, mcRouteLttEntry.numOfPaths = %d",
                                         lpmDBId, vrId, mcRouteLttEntry.numOfPaths);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, mcRouteLttEntry.numOfPaths = %d",
                                           lpmDBId, vrId, mcRouteLttEntry.numOfPaths);
        }

        mcRouteLttEntry.numOfPaths = 0;

        /*
            1.7. Call with lpmDBId [1], vrId[0],
                                        out of range mcRouteLttEntryPtr->numOfPaths [64]
                                        and othe parameters from 1.4.
            Expected: NOT GT_OK
        */
        mcRouteLttEntry.numOfPaths = 64;

        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                     ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);

        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, mcRouteLttEntry.numOfPaths = %d",
                                       lpmDBId, vrId, mcRouteLttEntry.numOfPaths);

        mcRouteLttEntry.numOfPaths = 0;

        /*
            1.8. Call with lpmDBId [1], vrId[0],
                           out of range mcRouteLttEntryPtr->routeEntryBaseIndex [4096]
                                        and othe parameters from 1.4.
            Expected: NOT GT_OK.
        */
        mcRouteLttEntry.routeEntryBaseIndex =
                            PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.routerNextHop;


        st = cpssDxChIpLpmDBDevListAdd(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfDevs = %d", lpmDBId, numOfDevs);

        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                     ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                        "%d, %d, mcRouteLttEntryPtr->routeEntryBaseIndex = %d",
                                         lpmDBId, vrId, mcRouteLttEntry.routeEntryBaseIndex);

        mcRouteLttEntry.routeEntryBaseIndex = 0;

        /*
            1.9. Call with lpmDBId [1], vrId[0],
                                        wrong enum values mcRouteLttEntryPtr->ipv6MCGroupScopeLevel
                                        and othe parameters from 1.4.
            Expected: GT_BAD_PARAM.
        */
        if (!prvUtfIsPbrModeUsed())
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmIpv4McEntryAdd
                                (lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                 ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable),
                                mcRouteLttEntry.ipv6MCGroupScopeLevel);
        }

        /*
            1.10. Call with lpmDBId [1], out of range vrId[10] and other valid parameters.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                     ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /*
            1.10. Call with lpmDBId [0], out of range vrId[10] and other valid parameters.
            Expected: NOT GT_OK.
        */
        lpmDBId = 0;
        vrId    = 10;

        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                         ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.11. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                         ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.12. Call with out of range ipGroupPrefixLen [33] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        ipGroupPrefixLen = 33;

        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                              ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipGroupPrefixLen = %d",
                                        lpmDBId, ipGroupPrefixLen);

        ipGroupPrefixLen = 32;

        /*
            1.13. Call with out of range ipSrcPrefixLen [33] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        ipSrcPrefixLen = 33;

        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                        "%d, ipSrcPrefixLen = %d", lpmDBId, ipSrcPrefixLen);

        ipSrcPrefixLen = 32;

        /*
            1.14. Call with lpmDBId [1], vrId[0], mcRouteLttEntryPtr [NULL]
                  and other valid parameters.
            Expected: GT_BAD_PTR.
        */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                         ipSrcPrefixLen, NULL, override, defragEnable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                            "%d, %d, mcRouteLttEntryPtr = NULL", lpmDBId, vrId);

        /*
            1.15. Call cpssDxChIpLpmIpv4McEntriesFlush with with lpmDBId [0 / 1],
                  vrId[0] to invalidate entries.
            Expected: GT_OK.
        */
        vrId = 0;

        /* Call with lpmDBId [0] */
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        /* Call with lpmDBId [1] */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv4McEntryDel
(
    IN GT_U32       lpmDBId,
    IN GT_U32       vrId,
    IN GT_IPADDR    ipGroup,
    IN GT_U32       ipGroupPrefixLen,
    IN GT_IPADDR    ipSrc,
    IN GT_U32       ipSrcPrefixLen
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv4McEntryDel)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [1], vrId[0],
        defUcNextHopInfoPtr->ipLttEntry{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
        numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
        sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, defMcRouteLttEntryPtr {
        routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0], routeEntryBaseIndex[0],
        ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
    Expected: GT_OK .
    1.3. Call cpssDxChIpLpmIpv4McEntryAdd with lpmDBId[0], vrId[0], ipGroup[224.15.0.1],
        ipGroupPrefixLen[32], ipSrc[224.16.0.1], ipSrcPrefixLen[32],
        mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0],
        routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, override[GT_TRUE],
        defragmentationEnable[GT_FALSE].
    Expected: GT_OK.
    1.4. Call with lpmDBId[0], vrId[0], ipGroup[10.15.0.1], ipGroupPrefixLen[32],
        ipSrc[10.16.0.1], ipSrcPrefixLen[32],.
    Expected: GT_OK.
    1.5. Call with lpmDBId[0], vrId[0], ipGroup[10.15.0.1], ipGroupPrefixLen[32],
        ipSrc[10.16.0.1], ipSrcPrefixLen[32] (already deleted).
    Expected: NOT GT_OK.
    1.6. Call with lpmDBId [0 or 1], out of range vrId[10] and other valid parameters.
    Expected: NOT GT_OK.
    1.7. Call with out of range ipGroupPrefixLen [33] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.8. Call with out of range ipSrcPrefixLen [33] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.9. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.10. Call cpssDxChIpLpmIpv4McEntriesFlush with with lpmDBId [1], vrId[0] to invalidate
        entries.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                     lpmDBId          = 0;
    GT_U32                     vrId             = 0;
    GT_IPADDR                  ipGroup;
    GT_U32                     ipGroupPrefixLen = 0;
    GT_IPADDR                  ipSrc;
    GT_U32                     ipSrcPrefixLen   = 0;
    CPSS_DXCH_IP_LTT_ENTRY_STC mcRouteLttEntry;
    GT_BOOL                    override         = GT_FALSE;
    GT_BOOL                    defragEnable     = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv4McEntryAdd with lpmDBId[1],
                 vrId[0], ipGroup[224.15.0.1], ipGroupPrefixLen[32], ipSrc[224.16.0.1],
                 ipSrcPrefixLen[32],
                 mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                 numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
                 sipSaCheckMismatchEnable[GT_TRUE],
                 ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]},
                 override[GT_TRUE], defragmentationEnable[GT_FALSE].
            Expected: GT_OK
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 224;
        ipGroup.arIP[1] = 15;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 1;

        ipGroupPrefixLen = 32;

        ipSrc.arIP[0] = 224;
        ipSrc.arIP[1] = 15;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 32;

        prvUtfSetDefaultIpLttEntry(&mcRouteLttEntry);

        override     = GT_TRUE;
        defragEnable = GT_FALSE;

        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                         ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChIpLpmIpv4McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                         lpmDBId, vrId, override, defragEnable);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                "cpssDxChIpLpmIpv4McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                     lpmDBId, vrId, override, defragEnable);
        }

        /*
            1.4. Call with lpmDBId[0], vrId[0], ipGroup[224.15.0.1], ipGroupPrefixLen[32],
                           ipSrc[224.16.0.1], ipSrcPrefixLen[32].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 224;
        ipGroup.arIP[1] = 15;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 1;

        ipGroupPrefixLen = 32;

        ipSrc.arIP[0] = 224;
        ipSrc.arIP[1] = 15;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 32;

        st = cpssDxChIpLpmIpv4McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                                                          ipSrcPrefixLen);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        /*
            1.6. Call with lpmDBId [0], out of range vrId[10] and other valid parameters.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv4McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                                                          ipSrcPrefixLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.6. Call with lpmDBId [1], out of range vrId[10] and other valid parameters.
                Expected: NOT GT_OK.
        */
        lpmDBId = 1;
        vrId    = 10;

        st = cpssDxChIpLpmIpv4McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.7. Call with out of range ipGroupPrefixLen [33] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        ipGroupPrefixLen = 33;

        st = cpssDxChIpLpmIpv4McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipGroupPrefixLen = %d", lpmDBId, ipGroupPrefixLen);

        ipGroupPrefixLen = 32;

        /*
            1.8. Call with out of range ipSrcPrefixLen [33] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        ipSrcPrefixLen = 33;

        st = cpssDxChIpLpmIpv4McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipSrcPrefixLen = %d", lpmDBId, ipSrcPrefixLen);

        ipSrcPrefixLen = 32;

        /*
            1.9. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv4McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        /*
            1.10. Call cpssDxChIpLpmIpv4McEntriesFlush with with lpmDBId [1], vrId[0] to invalidate entries.
            Expected: GT_OK.
        */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv4McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv4McEntriesFlush)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0],
        defUcNextHopInfoPtr{ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
        mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
        matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
        egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
        ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
        redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer
        { policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
        modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
        precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
        doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },  defMcRouteLttEntryPtr
        [NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK for not Cheetha devices else GT_BAD_PRT.
    1.3. Call with lpmDBId [0], vrId[0].
    Expected: GT_OK.
    1.4. Call with not valid LPM DB id lpmDBId [10] and other parameters from 3.
    Expected: NOT GT_OK.
    1.5. Call with lpmDBId [0 or 1], out of range vrId[10] and other valid parameters.
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                   lpmDBId = 0;
    GT_U32                   vrId    = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call with lpmDBId [0],
                           vrId[0].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        st = cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        /*
            1.4. Call with not valid LPM DB id lpmDBId [10]
                           and other parameters from 3.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.5. Call with lpmDBId [0],
                           out of range vrId[10] and other valid parameters.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.5. Call with lpmDBId [1],
                               out of range vrId[10]
                               and other valid parameters.
                Expected: NOT GT_OK.
        */
        lpmDBId = 1;
        vrId    = 10;

        st = cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv4McEntryGetNext
(
    IN    GT_U32                        lpmDBId,
    IN    GT_U32                        vrId,
    INOUT GT_IPADDR                     *ipGroupPtr,
    INOUT GT_U32                        *ipGroupPrefixLenPtr,
    INOUT GT_IPADDR                     *ipSrcPtr,
    INOUT GT_U32                        *ipSrcPrefixLenPtr,
    OUT   CPSS_DXCH_IP_LTT_ENTRY_STC    *mcRouteLttEntryPtr,
    OUT GT_U32                          *gRowIndexPtr,
    OUT GT_U32                          *gColumnIndexPtr,
    OUT GT_U32                          *sRowIndexPtr,
    OUT GT_U32                          *sColumnIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv4McEntryGetNext)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0],
        defUcNextHopInfoPtr->ipLttEntry{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
        numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
        sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, defMcRouteLttEntryPtr {
        routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0], routeEntryBaseIndex[0],
        ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
    Expected: GT_OK .
    1.3. Call cpssDxChIpLpmIpv4McEntryAdd for 10 times with lpmDBId[0], vrId[0],
        ipGroup[224.15.0.1], ipGroupPrefixLen[32], ipSrc[224.16.0.1], ipSrcPrefixLen[32],
        mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0],
        routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, override[GT_TRUE],
        defragmentationEnable[GT_FALSE].
    Expected: GT_OK.
    1.4. Call with lpmDBId[0], vrId[0], ipGroupPtr[224.15.0.1...9], ipGroupPrefixLenPtr [32],
        ipSrcPtr[224.16.0.1...9], ipSrcPrefixLenPtr[32] and non-null mcRouteLttEntryPtr.
    Expected: GT_OK and ipGroupPtr[224.15.0.2...10], ipGroupPrefixLenPtr[32],
        ipSrcPtr[224.16.0.2...10], ipSrcPrefixLenPtr[32] and the same mcRouteLttEntryPtr.
    1.5. Call with lpmDBId [0 or 1], out of range vrId[10] and other valid parameters.
    Expected: NOT GT_OK.
    1.6. Call with out of range ipGroupPrefixLenPtr [33] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.7. Call with out of range ipSrcPrefixLenPtr [33] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.9. Call with lpmDBId [0], vrId[0], ipGroupPtr[NULL] and other valid parameters.
    Expected: GT_BAD_PTR.
    1.10. Call with lpmDBId [0], vrId[0], ipGroupPrefixLenPtr [NULL] and other valid parameters.
    Expected: GT_BAD_PTR.
    1.11. Call with lpmDBId [0], vrId[0], ipSrcPtr [NULL] and other valid parameters.
    Expected: GT_BAD_PTR.
    1.12. Call with lpmDBId [0], vrId[0], ipSrcPrefixLenPtr [NULL] and other valid parameters.
    Expected: GT_BAD_PTR.
    1.13. Call with lpmDBId [0], vrId[0], mcRouteLttEntryPtr [NULL] and other valid parameters.
    Expected: GT_BAD_PTR.
    1.14. Call cpssDxChIpLpmIpv4McEntriesFlush with with lpmDBId [0 / 1], vrId[0] to invalidate entries.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                     lpmDBId          = 0;
    GT_U32                     vrId             = 0;
    GT_IPADDR                  ipGroup;
    GT_U32                     ipGroupPrefixLen = 0;
    GT_IPADDR                  ipSrc;
    GT_U32                     ipSrcPrefixLen   = 0;
    CPSS_DXCH_IP_LTT_ENTRY_STC mcRouteLttEntry;
    CPSS_DXCH_IP_LTT_ENTRY_STC mcRouteLttEntryGet;
    GT_BOOL                    override         = GT_FALSE;
    GT_BOOL                    defragEnable     = GT_FALSE;
    GT_U8                      iter             = 0;
    GT_U32                          gRowIndex;
    GT_U32                          gColumnIndex;
    GT_U32                          sRowIndex;
    GT_U32                          sColumnIndex;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv4McEntryAdd for 10 times with lpmDBId[0],
                 vrId[0], ipGroup[224.15.0.1..10], ipGroupPrefixLen[32], ipSrc[224.16.0.1..10],
                 ipSrcPrefixLen[32],
                 mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                 numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
                 sipSaCheckMismatchEnable[GT_TRUE],
                 ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]},
                 override[GT_TRUE], defragmentationEnable[GT_FALSE].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 224;
        ipGroup.arIP[1] = 15;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 1;

        ipGroupPrefixLen = 32;

        ipSrc.arIP[0] = 224;
        ipSrc.arIP[1] = 16;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 32;

        prvUtfSetDefaultIpLttEntry(&mcRouteLttEntry);

        override     = GT_TRUE;
        defragEnable = GT_FALSE;

        /* add ipGroup[224.15.0.1], ipSrc[224.16.0.1] */
        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                             ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChIpLpmIpv4McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                             lpmDBId, vrId, override, defragEnable);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChIpLpmIpv4McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                             lpmDBId, vrId, override, defragEnable);
        }


        for (iter = 0; iter < 10; iter++)
        {
            ipGroup.arIP[3] = (GT_U8)(ipGroup.arIP[3] + 1);

            ipSrc.arIP[3] = (GT_U8)(ipSrc.arIP[3] + 1);

            st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                             ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
            if (prvUtfIsPbrModeUsed())
            {
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "cpssDxChIpLpmIpv4McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                             lpmDBId, vrId, override, defragEnable);
            }
            else
            {
                    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                        "cpssDxChIpLpmIpv4McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                             lpmDBId, vrId, override, defragEnable);
            }
        }

        /*
            1.4. Call with lpmDBId[0], vrId[0], ipGroupPtr[224.15.0.1], ipGroupPrefixLenPtr
                [32], ipSrcPtr[224.16.0.1], ipSrcPrefixLenPtr[32] and non-null mcRouteLttEntryPtr.
            Expected: GT_OK and ipGroupPtr[10.15.0.2...10], ipGroupPrefixLenPtr[32],
                ipSrcPtr[10.16.0.2...10], ipSrcPrefixLenPtr[32] and the same mcRouteLttEntryPtr.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 224;
        ipGroup.arIP[1] = 15;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 1;

        ipGroupPrefixLen = 32;

        ipSrc.arIP[0] = 224;
        ipSrc.arIP[1] = 16;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 32;

        for (iter = 0; iter < 10; iter++)
        {
            st = cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                                 &ipSrcPrefixLen, &mcRouteLttEntryGet,
                                                 &gRowIndex,&gColumnIndex, &sRowIndex,&sColumnIndex);
            if (prvUtfIsPbrModeUsed())
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

                if (GT_OK == st)
                {
                    /* Verifying values */
                    prvUtfCompareIpLttEntries(lpmDBId, vrId, &mcRouteLttEntryGet, &mcRouteLttEntry);

                    UTF_VERIFY_EQUAL2_STRING_MAC(iter+2 , ipGroup.arIP[3],
                               "get another ipGroup.arIP[3] than was set: %d, %d", lpmDBId, vrId);
                    UTF_VERIFY_EQUAL2_STRING_MAC(iter+2, ipSrc.arIP[3],
                               "get another ipSrc.arIP[3] than was set: %d, %d", lpmDBId, vrId);
                }
            }
        }

        /*
            1.5. Call with lpmDBId [0], out of range vrId[10] and other valid parameters.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                         &ipSrcPrefixLen, &mcRouteLttEntryGet,&gRowIndex,
                                         &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.5. Call with lpmDBId [1], out of range vrId[10] and other valid parameters.
            Expected: NOT GT_OK.
        */
        lpmDBId = 1;
        vrId    = 10;

        st = cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                             &ipSrcPrefixLen, &mcRouteLttEntryGet,&gRowIndex,
                                             &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.6. Call with out of range ipGroupPrefixLenPtr [33] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        ipGroupPrefixLen = 33;

        st = cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                             &ipSrcPrefixLen, &mcRouteLttEntryGet,&gRowIndex,
                                             &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipGroupPrefixLenPtr = %d",
                                                        lpmDBId, ipGroupPrefixLen);

        ipGroupPrefixLen = 32;

        /*
            1.7. Call with out of range ipSrcPrefixLenPtr [33] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        ipSrcPrefixLen = 33;

        st = cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                             &ipSrcPrefixLen, &mcRouteLttEntryGet,&gRowIndex,
                                             &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipSrcPrefixLenPtr = %d",
                                                        lpmDBId, ipSrcPrefixLen);

        ipSrcPrefixLen = 32;

        /*
            1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                             &ipSrcPrefixLen, &mcRouteLttEntryGet,&gRowIndex,
                                             &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.9. Call with lpmDBId [0], vrId[0], ipGroupPtr[NULL] and other valid parameters.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId, vrId, NULL, &ipGroupPrefixLen, &ipSrc,
                                             &ipSrcPrefixLen, &mcRouteLttEntryGet,&gRowIndex,
                                             &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ipGroupPtr = NULL", lpmDBId, vrId);

        /*
            1.10. Call with lpmDBId [0], vrId[0], ipGroupPrefixLenPtr [NULL]
                  and other valid parameters.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId, vrId, &ipGroup, NULL, &ipSrc,
                                             &ipSrcPrefixLen, &mcRouteLttEntryGet,&gRowIndex,
                                             &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ipGroupPrefixLenPtr = NULL",
                                                                            lpmDBId, vrId);

        /*
            1.11. Call with lpmDBId [0], vrId[0], ipSrcPtr [NULL] and other valid parameters.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, NULL,
                                             &ipSrcPrefixLen, &mcRouteLttEntryGet,&gRowIndex,
                                             &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ipSrcPtr = NULL", lpmDBId, vrId);

        /*
            1.12. Call with lpmDBId [0], vrId[0], ipSrcPrefixLenPtr [NULL] and other valid parameters.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                             NULL, &mcRouteLttEntryGet,&gRowIndex,
                                             &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ipSrcPrefixLenPtr = NULL", lpmDBId, vrId);

        /*
            1.13. Call with lpmDBId [0], vrId[0], mcRouteLttEntryPtr [NULL] and other valid parameters.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                             &ipSrcPrefixLen, NULL,&gRowIndex,
                                             &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, mcRouteLttEntryPtr = NULL", lpmDBId, vrId);

        /*
            1.14. Call cpssDxChIpLpmIpv4McEntriesFlush with with lpmDBId [0 / 1],
                                                                 vrId[0] to invalidate entries.
            Expected: GT_OK.
        */
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        /* Call with lpmDBId [1] */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv4McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPADDR                   ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPADDR                   ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                      *gRowIndexPtr,
    OUT GT_U32                      *gColumnIndexPtr,
    OUT GT_U32                      *sRowIndexPtr,
    OUT GT_U32                      *sColumnIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv4McEntrySearch)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0],
        defUcNextHopInfoPtr->ipLttEntry{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
        numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
        sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, defMcRouteLttEntryPtr {
        routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0], routeEntryBaseIndex[0],
        ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
    Expected: GT_OK .
    1.4. Call cpssDxChIpLpmIpv4McEntryAdd for 10 times with lpmDBId[0], vrId[0],
        ipGroup[224.15.0.1], ipGroupPrefixLen[32], ipSrc[224.16.0.1], ipSrcPrefixLen[32],
        mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0],
        routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, override[GT_TRUE],
        defragmentationEnable[GT_FALSE].
    Expected: GT_OK.
    1.5. Call with lpmDBId[0], vrId[0], ipGroup[224.15.0.1], ipGroupPrefixLen [32],
        ipSrc[224.16.0.1], ipSrcPrefixLen[32] and non-null mcRouteLttEntry.
    Expected: GT_OK and the same mcRouteLttEntryPtr.
    1.6. Call with lpmDBId [0 or 1], out of range vrId[10] and other valid parameters.
    Expected: NOT GT_OK.
    1.7. Call with out of range ipGroupPrefixLen [33] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.8. Call with out of range ipSrcPrefixLen [33] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.9. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.10. Call with lpmDBId [0], vrId[0], mcRouteLttEntryPtr [NULL] and other valid parameters.
    Expected: GT_BAD_PTR.
    1.11. Call cpssDxChIpLpmIpv4McEntriesFlush with with lpmDBId [0/1], vrId[0] to invalidate entries.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                     lpmDBId          = 0;
    GT_U32                     vrId             = 0;
    GT_IPADDR                  ipGroup;
    GT_U32                     ipGroupPrefixLen = 0;
    GT_IPADDR                  ipSrc;
    GT_U32                     ipSrcPrefixLen   = 0;
    CPSS_DXCH_IP_LTT_ENTRY_STC mcRouteLttEntry;
    CPSS_DXCH_IP_LTT_ENTRY_STC mcRouteLttEntryGet;
    GT_BOOL                    override         = GT_FALSE;
    GT_BOOL                    defragEnable     = GT_FALSE;
    GT_U8                      iter             = 0;
    OUT GT_U32                 gRowIndex;
    OUT GT_U32                 gColumnIndex;
    OUT GT_U32                 sRowIndex;
    OUT GT_U32                 sColumnIndex;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv4McEntryAdd for 10 times with lpmDBId[0],
                 vrId[0], ipGroup[224.15.0.1..10], ipGroupPrefixLen[32], ipSrc[224.16.0.1..10],
                 ipSrcPrefixLen[32],
                 mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                 numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
                 sipSaCheckMismatchEnable[GT_TRUE],
                 ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]},
                 override[GT_TRUE], defragmentationEnable[GT_FALSE].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 224;
        ipGroup.arIP[1] = 15;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 1;

        ipGroupPrefixLen = 32;

        ipSrc.arIP[0] = 224;
        ipSrc.arIP[1] = 16;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 32;

        prvUtfSetDefaultIpLttEntry(&mcRouteLttEntry);

        override     = GT_TRUE;
        defragEnable = GT_FALSE;

        /* add ipGroup[224.15.0.1],ipSrc[224.16.0.1] */
        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                         ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChIpLpmIpv4McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                         lpmDBId, vrId, override, defragEnable);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChIpLpmIpv4McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                         lpmDBId, vrId, override, defragEnable);
        }

        for (iter = 0; iter < 10; iter++)
        {
            ipGroup.arIP[3] = (GT_U8) (ipGroup.arIP[3] + 1);

            ipSrc.arIP[3] = (GT_U8) (ipSrc.arIP[3] + 1);

            st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                             ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
            if (prvUtfIsPbrModeUsed())
            {
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "cpssDxChIpLpmIpv4McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                             lpmDBId, vrId, override, defragEnable);
            }
            else
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                        "cpssDxChIpLpmIpv4McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                             lpmDBId, vrId, override, defragEnable);
            }
        }

        /*
            1.5. Call with lpmDBId[0], vrId[0], ipGroup[224.15.0.1], ipGroupPrefixLen [32],
                ipSrc[224.16.0.1], ipSrcPrefixLen[32] and non-null mcRouteLttEntry.
            Expected: GT_OK and the same mcRouteLttEntryPtr.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 224;
        ipGroup.arIP[1] = 15;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 1;

        ipGroupPrefixLen = 32;

        ipSrc.arIP[0] = 224;
        ipSrc.arIP[1] = 16;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 32;

        for (iter = 0; iter < 10; iter++)
        {
            st = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                                ipSrcPrefixLen, &mcRouteLttEntryGet,&gRowIndex,
                                                &gColumnIndex, &sRowIndex,&sColumnIndex);
            if (prvUtfIsPbrModeUsed())
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

                prvUtfCompareIpLttEntries(lpmDBId,vrId, &mcRouteLttEntryGet, &mcRouteLttEntry);
            }
        }

        /*
            1.6. Call with lpmDBId [0], out of range vrId[10] and other valid parameters.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                        ipSrcPrefixLen, &mcRouteLttEntry,&gRowIndex,
                                         &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.7. Call with lpmDBId [1], out of range vrId[10] and other valid parameters.
            Expected: NOT GT_OK.
        */
        lpmDBId = 1;
        vrId    = 10;

        st = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                            ipSrcPrefixLen, &mcRouteLttEntry,&gRowIndex,
                                            &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.8. Call with out of range ipGroupPrefixLen [33] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        ipGroupPrefixLen = 33;

        st = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                            ipSrcPrefixLen, &mcRouteLttEntry,&gRowIndex,
                                            &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipGroupPrefixLen = %d", lpmDBId, ipGroupPrefixLen);

        ipGroupPrefixLen = 32;

        /*
            1.9. Call with out of range ipSrcPrefixLen [33] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        ipSrcPrefixLen = 33;

        st = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                            ipSrcPrefixLen, &mcRouteLttEntry,&gRowIndex,
                                            &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipSrcPrefixLen = %d", lpmDBId, ipSrcPrefixLen);

        ipSrcPrefixLen = 32;

        /*
            1.10. Call with lpmDBId [0], vrId[0], mcRouteLttEntryPtr [NULL] and other valid parameters.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                            ipSrcPrefixLen, NULL,&gRowIndex,
                                             &gColumnIndex, &sRowIndex,&sColumnIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, mcRouteLttEntryPtr = NULL", lpmDBId, vrId);

        /*
            1.11. Call cpssDxChIpLpmIpv4McEntriesFlush with with lpmDBId [0/1], vrId[0] to invalidate entries.
            Expected: GT_OK.
        */
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        lpmDBId = 1;

        st = cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv4McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPADDR                   ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPADDR                   ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                      *gRowIndexPtr,
    OUT GT_U32                      *gColumnIndexPtr,
    OUT GT_U32                      *sRowIndexPtr,
    OUT GT_U32                      *sColumnIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv4McEntryDelSearch)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0],
        defUcNextHopInfoPtr->ipLttEntry{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
        numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
        sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, defMcRouteLttEntryPtr {
        routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0], routeEntryBaseIndex[0],
        ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
    Expected: GT_OK .
    1.3. Call cpssDxChIpLpmIpv4McEntryAdd with lpmDBId[0], vrId[0],
                 ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                 ipSrc[149.218.213.102], ipSrcPrefixLen[32],
                 mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                                    numOfPaths[0], routeEntryBaseIndex[0],
                                    ucRPFCheckEnable[GT_TRUE],
                                    sipSaCheckMismatchEnable[GT_TRUE],
                                    ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]},
                 override[GT_TRUE], defragmentationEnable[GT_FALSE].
    Expected: GT_OK.
    1.4. Call cpssDxChIpLpmIpv4McEntryAdd with lpmDBId[0], vrId[0],
                 ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                 ipSrc[10.99.0.102], ipSrcPrefixLen[32],
                 mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                                    numOfPaths[0], routeEntryBaseIndex[0],
                                    ucRPFCheckEnable[GT_TRUE],
                                    sipSaCheckMismatchEnable[GT_TRUE],
                                    ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]},
                 override[GT_TRUE], defragmentationEnable[GT_FALSE].
    Expected: GT_OK.
    1.5. Call cpssDxChIpLpmIpv4McEntrySearch with lpmDBId[0], vrId[0],
                 ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                 ipSrc[149.218.213.102], ipSrcPrefixLen[32],
                 and non-null mcRouteLttEntry.
    Expected: GT_OK and the same mcRouteLttEntryPtr.
    1.6. Call cpssDxChIpLpmIpv4McEntrySearch with lpmDBId[0], vrId[0],
                 ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                 ipSrc[10.99.0.102], ipSrcPrefixLen[32],
                 and non-null mcRouteLttEntry.
    Expected: GT_OK and the same mcRouteLttEntryPtr.
    1.7. Call cpssDxChIpLpmIpv4McEntryDel with lpmDBId[0], vrId[0],
                 ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                 ipSrc[10.99.0.102], ipSrcPrefixLen[32].
    Expected: GT_OK.
    1.8. Call cpssDxChIpLpmIpv4McEntrySearch with lpmDBId[0], vrId[0],
                 ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                 ipSrc[149.218.213.102], ipSrcPrefixLen[32],
                 and non-null mcRouteLttEntry.
    Expected: GT_OK and the same mcRouteLttEntryPtr.
    1.9. Call cpssDxChIpLpmIpv4McEntrySearch with lpmDBId[0], vrId[0],
                 ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                 ipSrc[10.99.0.102], ipSrcPrefixLen[32],
                 and non-null mcRouteLttEntry.
    Expected: NOT GT_OK.
    1.10. Call cpssDxChIpLpmIpv4McEntryAdd with lpmDBId[0], vrId[0],
                  ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                  ipSrc[10.99.0.102], ipSrcPrefixLen[32],
                  mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                                     numOfPaths[0], routeEntryBaseIndex[0],
                                     ucRPFCheckEnable[GT_TRUE],
                                     sipSaCheckMismatchEnable[GT_TRUE],
                                     ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]},
                  override[GT_TRUE], defragmentationEnable[GT_FALSE].
    Expected: GT_OK.
    1.11. Call cpssDxChIpLpmIpv4McEntrySearch with lpmDBId[0], vrId[0],
                  ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                  ipSrc[149.218.213.102], ipSrcPrefixLen[32],
                  and non-null mcRouteLttEntry.
    Expected: GT_OK and the same mcRouteLttEntryPtr.
    1.12. Call cpssDxChIpLpmIpv4McEntrySearch with lpmDBId[0], vrId[0],
                  ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                  ipSrc[10.99.0.102], ipSrcPrefixLen[32],
                  and non-null mcRouteLttEntry.
    Expected: GT_OK and the same mcRouteLttEntryPtr.
    1.13. Call cpssDxChIpLpmIpv4McEntriesFlush with with lpmDBId [0/1], vrId[0] to invalidate entries.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                     lpmDBId          = 0;
    GT_U32                     vrId             = 0;
    GT_U32                     ipGroupPrefixLen = 0;
    GT_U32                     ipSrcPrefixLen   = 0;
    GT_BOOL                    override         = GT_FALSE;
    GT_BOOL                    defragEnable     = GT_FALSE;
    GT_U32                     gRowIndex        = 0;
    GT_U32                     gColumnIndex     = 0;
    GT_U32                     sRowIndex        = 0;
    GT_U32                     sColumnIndex     = 0;
    GT_IPADDR                  ipGroup = {0};
    GT_IPADDR                  ipSrc0  = {0};
    GT_IPADDR                  ipSrc1  = {0};
    CPSS_DXCH_IP_LTT_ENTRY_STC mcRouteLttEntry    = {0};
    CPSS_DXCH_IP_LTT_ENTRY_STC mcRouteLttEntryGet = {0};


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv4McEntryAdd with lpmDBId[0], vrId[0],
                 ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                 ipSrc[149.218.213.102], ipSrcPrefixLen[32],
                 mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                                    numOfPaths[0], routeEntryBaseIndex[0],
                                    ucRPFCheckEnable[GT_TRUE],
                                    sipSaCheckMismatchEnable[GT_TRUE],
                                    ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]},
                 override[GT_TRUE], defragmentationEnable[GT_FALSE].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 225;
        ipGroup.arIP[1] = 1;
        ipGroup.arIP[2] = 1;
        ipGroup.arIP[3] = 17;

        ipGroupPrefixLen = 32;

        ipSrc0.arIP[0] = 149;
        ipSrc0.arIP[1] = 218;
        ipSrc0.arIP[2] = 213;
        ipSrc0.arIP[3] = 102;

        ipSrcPrefixLen = 32;

        prvUtfSetDefaultIpLttEntry(&mcRouteLttEntry);

        override     = GT_TRUE;
        defragEnable = GT_FALSE;

        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc0,
                                         ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, lpmDBId, vrId, override, defragEnable);
        }
        else
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, lpmDBId, vrId, override, defragEnable);
        }

        /*
            1.4. Call cpssDxChIpLpmIpv4McEntryAdd with lpmDBId[0], vrId[0],
                 ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                 ipSrc[10.99.0.102], ipSrcPrefixLen[32],
                 mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                                    numOfPaths[0], routeEntryBaseIndex[0],
                                    ucRPFCheckEnable[GT_TRUE],
                                    sipSaCheckMismatchEnable[GT_TRUE],
                                    ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]},
                 override[GT_TRUE], defragmentationEnable[GT_FALSE].
            Expected: GT_OK.
        */
        ipSrc1.arIP[0] = 10;
        ipSrc1.arIP[1] = 99;
        ipSrc1.arIP[2] = 0;
        ipSrc1.arIP[3] = 102;

        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc1,
                                         ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, lpmDBId, vrId, override, defragEnable);
        }
        else
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, lpmDBId, vrId, override, defragEnable);
        }

        /*
            1.5. Call cpssDxChIpLpmIpv4McEntrySearch with lpmDBId[0], vrId[0],
                 ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                 ipSrc[149.218.213.102], ipSrcPrefixLen[32],
                 and non-null mcRouteLttEntry.
            Expected: GT_OK and the same mcRouteLttEntryPtr.
        */
        st = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc0,
                                            ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex,
                                            &gColumnIndex, &sRowIndex, &sColumnIndex);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        /*
            1.6. Call cpssDxChIpLpmIpv4McEntrySearch with lpmDBId[0], vrId[0],
                 ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                 ipSrc[10.99.0.102], ipSrcPrefixLen[32],
                 and non-null mcRouteLttEntry.
            Expected: GT_OK and the same mcRouteLttEntryPtr.
        */
        st = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc1,
                                            ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex,
                                            &gColumnIndex, &sRowIndex, &sColumnIndex);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        /*
            1.7. Call cpssDxChIpLpmIpv4McEntryDel with lpmDBId[0], vrId[0],
                 ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                 ipSrc[10.99.0.102], ipSrcPrefixLen[32].
            Expected: GT_OK.
        */
        st = cpssDxChIpLpmIpv4McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen,
                                         ipSrc1, ipSrcPrefixLen);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        /*
            1.8. Call cpssDxChIpLpmIpv4McEntrySearch with lpmDBId[0], vrId[0],
                 ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                 ipSrc[149.218.213.102], ipSrcPrefixLen[32],
                 and non-null mcRouteLttEntry.
            Expected: GT_OK and the same mcRouteLttEntryPtr.
        */
        st = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc0,
                                            ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex,
                                            &gColumnIndex, &sRowIndex, &sColumnIndex);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

            prvUtfCompareIpLttEntries(lpmDBId,vrId, &mcRouteLttEntryGet, &mcRouteLttEntry);
        }

        /*
            1.9. Call cpssDxChIpLpmIpv4McEntrySearch with lpmDBId[0], vrId[0],
                 ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                 ipSrc[10.99.0.102], ipSrcPrefixLen[32],
                 and non-null mcRouteLttEntry.
            Expected: NOT GT_OK.
        */
        st = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc1,
                                            ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex,
                                            &gColumnIndex, &sRowIndex, &sColumnIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /*
            1.10. Call cpssDxChIpLpmIpv4McEntryAdd with lpmDBId[0], vrId[0],
                  ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                  ipSrc[10.99.0.102], ipSrcPrefixLen[32],
                  mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                                     numOfPaths[0], routeEntryBaseIndex[0],
                                     ucRPFCheckEnable[GT_TRUE],
                                     sipSaCheckMismatchEnable[GT_TRUE],
                                     ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]},
                  override[GT_TRUE], defragmentationEnable[GT_FALSE].
            Expected: GT_OK.
        */
        st = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc1,
                                         ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, lpmDBId, vrId, override, defragEnable);
        }
        else
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, lpmDBId, vrId, override, defragEnable);
        }

        /*
            1.11. Call cpssDxChIpLpmIpv4McEntrySearch with lpmDBId[0], vrId[0],
                  ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                  ipSrc[149.218.213.102], ipSrcPrefixLen[32],
                  and non-null mcRouteLttEntry.
            Expected: GT_OK and the same mcRouteLttEntryPtr.
        */
        st = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc0,
                                            ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex,
                                            &gColumnIndex, &sRowIndex, &sColumnIndex);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

            prvUtfCompareIpLttEntries(lpmDBId,vrId, &mcRouteLttEntryGet, &mcRouteLttEntry);
        }

        /*
            1.12. Call cpssDxChIpLpmIpv4McEntrySearch with lpmDBId[0], vrId[0],
                  ipGroup[225.1.1.17], ipGroupPrefixLen[32],
                  ipSrc[10.99.0.102], ipSrcPrefixLen[32],
                  and non-null mcRouteLttEntry.
            Expected: GT_OK and the same mcRouteLttEntryPtr.
        */
        st = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc1,
                                            ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex,
                                            &gColumnIndex, &sRowIndex, &sColumnIndex);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

            prvUtfCompareIpLttEntries(lpmDBId,vrId, &mcRouteLttEntryGet, &mcRouteLttEntry);
        }

        /*
            1.13. Call cpssDxChIpLpmIpv4McEntriesFlush with with lpmDBId [0/1], vrId[0] to invalidate entries.
            Expected: GT_OK.
        */
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        lpmDBId = 1;

        st = cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv6UcPrefixAdd
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPV6ADDR                              ipAddr,
    IN GT_U32                                   prefixLen,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *nextHopInfoPtr,
    IN GT_BOOL                                  override,
    IN GT_BOOL                                  defragmentationEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv6UcPrefixAdd)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0 / 1],
        protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E], indexesRangePtr{ firstIndex [0], lastIndex
        [1024]}, partitionEnable[GT_TRUE], tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10],
        numOfIpv4McSourcePrefixes [5], numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0], defUcNextHopInfoPtr{
        pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
        mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount = GT_FALSE,
        matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
        modifyUp=GT_FALSE , qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
        profilePrecedence=GT_FALSE] ] }, redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
        data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
        egressTaggedModify=GT_FALSE, modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
        nestedVlan=GT_FALSE, vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
        }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
        defMcRouteLttEntryPtr [NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK for Cheetha devices else GT_BAD_PRT
    1.3. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [1], vrId[0],
        defUcNextHopInfoPtr->ipLttEntry{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
        numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
        sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, defMcRouteLttEntryPtr {
        routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0], routeEntryBaseIndex[0],
        ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
    Expected: GT_OK .
    1.4. Call with lpmDBId [0], vrId[0], ipAddr[10.15.0.1], prefixLen[128],
        nextHopInfoPtr->pclIpUcAction { pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode =
        CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter {
        enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE,
        modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [ ingress[profileIndex=0,
        profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] }, redirect {
        CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
        policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
        modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
        precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
        doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } }, override [GT_TRUE].
    Expected: GT_OK.
    1.5. Call with lpmDBId [1], vrId[0], ipAddr[10.16.0.1], prefixLen[128],
        nextHopInfoPtr->ipLttEntry{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0],
        routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
    Expected: GT_OK.
    1.6. Call with override [GT_FALSE] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.7. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.8. Call with not supported vrId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.9. Call with out of range prefixLen [129] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.10. For not Cheetah+ call with lpmDBId [1] out of range
        nextHopInfoPtr->ipLttEntry.routeType [wrong enum values] and other parameters from 1.5.
    Expected: GT_BAD_PARAM.
    1.11. Call with lpmDBId [1], correct nextHopInfoPtr->ipLttEntry.numOfPaths
                     ([5 / 7] - for lion and above and [49 / 63] for other)
                     and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.12. Call with lpmDBId [1],
                     out of range nextHopInfoPtr->ipLttEntry.numOfPaths
                     ([64 / 65] - for lion and above and [8 / 9] for other)
                     and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.13. Call with lpmDBId [1] out of range nextHopInfoPtr->ipLttEntry.
        numOfPaths[routeEntryBaseIndex] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.14. Call with lpmDBId [1] out of range nextHopInfoPtr->ipLttEntry. ipv6MCGroupScopeLevel
        [wrong enum values] and other parameters from 1.5.
    Expected: GT_BAD_PARAM
    1.15. Call with lpmDBId [1] nextHopInfoPtr[NULL] and other parameters from 1.5.
    Expected: GT_BAD_PTR.
    1.16. For Cheetah+ call with lpmDBId [0] out of range
        nextHopInfoPtr->pclIpUcAction.pktCmdp[wrong enum values] and other parameters from 1.4.
    Expected: GT_BAD_PARAM.
    1.17. Call cpssDxChIpLpmIpv6UcPrefixesFlush with lpmDBId [0 / 1] and vrId[0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                                 lpmDBId      = 0;
    GT_U32                                 vrId         = 0;
    GT_IPV6ADDR                            ipAddr;
    GT_U32                                 prefixLen    = 0;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT nextHopInfo;
    GT_BOOL                                override     = GT_FALSE;
    GT_BOOL                                defragEnable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. - 1.3.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.4. Call with lpmDBId [0], vrId[0], ipAddr[10.15.0.1], prefixLen[32],
               nextHopInfoPtr->pclIpUcAction{pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                                             mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                                                    mirrorToRxAnalyzerPort = GT_FALSE},
                                             matchCounter{enableMatchCount = GT_FALSE,
                                                          matchCounterIndex = 0},
                                             qos{egressPolicy = GT_FALSE,
                                                 modifyDscp = GT_FALSE,
                                                 modifyUp = GT_FALSE,
                                                 qos [ ingress[profileIndex=0,
                                                 profileAssignIndex=GT_FALSE,
                                                 profilePrecedence=GT_FALSE] ] },
                                             redirect {CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                                             data[routerLttIndex=0] },
                                             policer { policerEnable=GT_FALSE, policerId=0 },
                                             vlan {egressTaggedModify=GT_FALSE,
                                                   modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                                                   nestedVlan=GT_FALSE,
                                                   vlanId=100,
                                                   precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                                              ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
               override [GT_TRUE].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipAddr.u32Ip[0] = 10;
        ipAddr.u32Ip[1] = 15;
        ipAddr.u32Ip[2] = 0;
        ipAddr.u32Ip[3] = 1;

        prefixLen = 128;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&nextHopInfo.ipLttEntry);
        }

        override     = GT_TRUE;
        defragEnable = GT_FALSE;

        st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override, defragEnable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "%d, %d, %d, override = %d, defragmentationEnable = %d",
                                     lpmDBId, vrId, prefixLen, override, defragEnable);

        /*
            1.5. Call with lpmDBId [1], vrId[0], ipAddr[10.16.0.1], prefixLen[128],
                   nextHopInfoPtr->ipLttEntry{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                                              numOfPaths[0],
                                              routeEntryBaseIndex[0],
                                              ucRPFCheckEnable[GT_TRUE],
                                              sipSaCheckMismatchEnable[GT_TRUE],
                                              ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
            Expected: GT_OK.
        */
        lpmDBId = 1;

        ipAddr.u32Ip[0] = 10;
        ipAddr.u32Ip[1] = 16;
        ipAddr.u32Ip[2] = 0;
        ipAddr.u32Ip[3] = 1;

        prvUtfSetDefaultIpLttEntry(&nextHopInfo.ipLttEntry);

        st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo,
                                                                    override, defragEnable);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "%d, %d, %d, override = %d, defragmentationEnable = %d",
                                     lpmDBId, vrId, prefixLen, override, defragEnable);

        /*
            1.6. Call with override [GT_FALSE] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        override = GT_FALSE;

        st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo,
                                                                    override, defragEnable);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, override = %d", lpmDBId, vrId, override);

        override = GT_TRUE;

        /*
            1.7. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo,
                                                                    override, defragEnable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.8. Call with not supported vrId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */

        vrId = 10;

        st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo,
                                                                    override, defragEnable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.9. Call with out of range prefixLen [129] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        prefixLen = 129;

        st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo,
                                                                    override, defragEnable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, prefixLen = %d", lpmDBId, prefixLen);

        prefixLen = 128;

        if (CPSS_PP_FAMILY_CHEETAH_E != PRV_CPSS_PP_MAC(dev)->devFamily && !prvUtfIsPbrModeUsed())
        {
            /*
                1.10. For not Cheetah+ call with lpmDBId [1]
                                wrong enum values nextHopInfoPtr->ipLttEntry.routeType
                                and other parameters from 1.5.
                Expected: GT_BAD_PARAM.
            */
            lpmDBId = 1;

            prvUtfSetDefaultIpLttEntry(&nextHopInfo.ipLttEntry);

            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmIpv6UcPrefixAdd
                                (lpmDBId, vrId, ipAddr, prefixLen,
                                 &nextHopInfo, override, defragEnable),
                                nextHopInfo.ipLttEntry.routeType);

            /*
                1.11. Call with lpmDBId [1], correct nextHopInfoPtr->ipLttEntry.numOfPaths
                     ([5 / 7] - for lion and above and [49 / 63] for other)
                                 and other parameters from 1.5.
                Expected: GT_OK.
            */
            /*call with 49 or 5 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                nextHopInfo.ipLttEntry.numOfPaths = 49;
            }
            else
            {
                nextHopInfo.ipLttEntry.numOfPaths = 5;
            }

            st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo,
                                                                     override, defragEnable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, nextHopInfo.ipLttEntry.numOfPaths);

            /*call with 63 or 7 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                nextHopInfo.ipLttEntry.numOfPaths = 63;
            }
            else
            {
                nextHopInfo.ipLttEntry.numOfPaths = 7;
            }

            st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo,
                                                                     override, defragEnable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, nextHopInfo.ipLttEntry.numOfPaths);

            /*
                1.12. Call with lpmDBId [1],
                                 out of range nextHopInfoPtr->ipLttEntry.numOfPaths
                                 ([64 / 65] - for lion and above and [8 / 9] for other)
                                 and other parameters from 1.5.
                Expected: NOT GT_OK.
            */
            /*call with 64 or 8 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                nextHopInfo.ipLttEntry.numOfPaths = 64;
            }
            else
            {
                nextHopInfo.ipLttEntry.numOfPaths = 8;
            }

            st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo,
                                                                     override, defragEnable);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, nextHopInfo.ipLttEntry.numOfPaths);

            /*call with 65 or 9 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                nextHopInfo.ipLttEntry.numOfPaths = 65;
            }
            else
            {
                nextHopInfo.ipLttEntry.numOfPaths = 9;
            }

            st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo,
                                                                     override, defragEnable);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, nextHopInfo.ipLttEntry.numOfPaths);

            nextHopInfo.ipLttEntry.numOfPaths = 0;

            /*
                1.13. Call with lpmDBId [1] out of range
                    nextHopInfoPtr->ipLttEntry.routeEntryBaseIndex[4096] and other parameters from 1.5.
                Expected: NOT GT_OK.
            */
            nextHopInfo.ipLttEntry.routeEntryBaseIndex = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.routerNextHop;

            st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo,
                                                                        override, defragEnable);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "%d, %d, nextHopInfoPtr->ipLttEntry.routeEntryBaseIndex",
                                             lpmDBId, vrId, nextHopInfo.ipLttEntry.routeEntryBaseIndex);

            nextHopInfo.ipLttEntry.routeEntryBaseIndex = 0;

            /*
                1.14. Call with lpmDBId [1]
                             wrong enum values nextHopInfoPtr->ipLttEntry.ipv6MCGroupScopeLevel
                             and other parameters from 1.5.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmIpv6UcPrefixAdd
                                (lpmDBId, vrId, ipAddr, prefixLen,
                                 &nextHopInfo, override, defragEnable),
                                nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel);

            /*
                1.15. Call with lpmDBId [1]
                                             nextHopInfoPtr[NULL]
                                             and other parameters from 1.5.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, NULL, override, defragEnable);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nextHopInfoPtr = NULL", lpmDBId, vrId);
        }

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            /*
                1.16. For Cheetah+ call with lpmDBId [0]
                                             wrong enum values nextHopInfoPtr->pclIpUcAction.pktCmdp
                                             and other parameters from 1.4.
                Expected: GT_BAD_PARAM.
            */
            lpmDBId = 0;
            prvUtfSetDefaultActionEntry(&nextHopInfo.pclIpUcAction);

            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmIpv6UcPrefixAdd
                                (lpmDBId, vrId, ipAddr, prefixLen,
                                 &nextHopInfo, override, defragEnable),
                                nextHopInfo.pclIpUcAction.pktCmd);
        }

        /*
            1.17. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0 / 1] and vrId[0] to invalidate changes.
            Expected: GT_OK.
        */
        vrId = 0;

        /* Call with lpmDBId[0] */
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /* Call with lpmDBId[1] */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv6UcPrefixAddBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC  *ipv6PrefixArray,
    IN GT_BOOL                              defragmentationEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv6UcPrefixAddBulk)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0], defUcNextHopInfoPtr{
        pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
        mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount = GT_FALSE,
        matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
        modifyUp=GT_FALSE , qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
        profilePrecedence=GT_FALSE] ] }, redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
        data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
        egressTaggedModify=GT_FALSE, modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
        nestedVlan=GT_FALSE, vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
        }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
        defMcRouteLttEntryPtr [NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK for Cheetha devices else GT_BAD_PRT
    1.3. Call with lpmDBId[0], ipv6PrefixArrayLen [1], ipv6PrefixArray{vrId[0],
        ipAddr[10.15.0.1], prefixLen[128], nextHopInfoPtr{ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
        mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
        matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
        egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
        ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
        redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
        policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
        modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
        precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
        doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } }, numOfEcmpWaRouteEntries[1],
        override[GT_TRUE], returnStatus[0]}
    Expected: GT_OK.
    1.4. Call with not supported ipv6PrefixArray->vrId[1] and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.5. Call with out of range ipv6PrefixArray->prefixLen [129] and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.6. Call with lpmDBId [1], correct nextHopInfoPtr->ipLttEntry.numOfPaths
                     ([5 / 7] - for lion and above and [49 / 63] for other)
                     and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.7. Call with lpmDBId [1], out of range nextHopInfoPtr->ipLttEntry.numOfPaths
                     ([64 / 65] - for lion and above and [8 / 9] for other)
                     and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.9. Call with ipv6PrefixArray [NULL] and other parameters from 1.3.
    Expected: GT_BAD_PTR.
    1.10. Call cpssDxChIpLpmIpv6UcPrefixesFlush with lpmDBId [0] and vrId[0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                              lpmDBId      = 0;
    GT_U32                              prefixLen    = 0;
    CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC ipv6PrefixArray;
    GT_U32                              vrId         = 0;
    GT_BOOL                             defragEnable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call with lpmDBId[0], ipv4PrefixArrayLen[1], ipv4PrefixArray{vrId[0],
                       ipAddr[10.15.0.1], prefixLen[128],
                       nextHopInfoPtr{ pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode =
                       CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
                       matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos
                       { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
                       ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                       profilePrecedence=GT_FALSE] ] }, redirect {
                       CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] },
                       policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
                       egressTaggedModify=GT_FALSE,
                       modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                       vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                       ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
                       numOfEcmpWaRouteEntries[1], override[GT_TRUE], returnStatus[0]}
            Expected: GT_OK.
        */
        lpmDBId   = 0;
        prefixLen = 1;

        ipv6PrefixArray.ipAddr.u32Ip[0] = 10;
        ipv6PrefixArray.ipAddr.u32Ip[1] = 15;
        ipv6PrefixArray.ipAddr.u32Ip[2] = 0;
        ipv6PrefixArray.ipAddr.u32Ip[3] = 1;

        ipv6PrefixArray.prefixLen = 128;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&ipv6PrefixArray.nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&ipv6PrefixArray.nextHopInfo.ipLttEntry);
        }

        ipv6PrefixArray.numOfEcmpWaRouteEntries = 1;
        ipv6PrefixArray.override                = GT_TRUE;
        ipv6PrefixArray.returnStatus            = GT_OK;
        ipv6PrefixArray.vrId = 0;

        defragEnable = GT_FALSE;

        st = cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId, prefixLen, &ipv6PrefixArray, defragEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, prefixLen);

        /*
            1.4. Call with not supported ipv6PrefixArray->vrId[10] and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        ipv6PrefixArray.vrId = 10;

        st = cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId, prefixLen, &ipv6PrefixArray, defragEnable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipv6PrefixArray->vrId = %d",
                                         lpmDBId, ipv6PrefixArray.vrId);

        ipv6PrefixArray.vrId = 0;

        /*
            1.5. Call with out of range ipv6PrefixArray->prefixLen [129] and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        ipv6PrefixArray.prefixLen = 129;

        st = cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId, prefixLen, &ipv6PrefixArray, defragEnable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipv6PrefixArray->prefixLen = %d",
                                         lpmDBId, ipv6PrefixArray.prefixLen);

        ipv6PrefixArray.prefixLen = 128;


        if (CPSS_PP_FAMILY_CHEETAH_E != PRV_CPSS_PP_MAC(dev)->devFamily && !prvUtfIsPbrModeUsed())
        {
            /*
                1.6. Call with lpmDBId [1], correct nextHopInfoPtr->ipLttEntry.numOfPaths
                     ([5 / 7] - for lion and above and [49 / 63] for other)
                                 and other parameters from 1.5.
                Expected: GT_OK.
            */
            /*call with 49 or 5 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                ipv6PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 49;
            }
            else
            {
                ipv6PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 5;
            }

            st = cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId, prefixLen,
                                        &ipv6PrefixArray, defragEnable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, ipv6PrefixArray.nextHopInfo.ipLttEntry.numOfPaths);

            /*call with 63 or 7 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                ipv6PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 63;
            }
            else
            {
                ipv6PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 7;
            }

            st = cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId, prefixLen,
                                        &ipv6PrefixArray, defragEnable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, ipv6PrefixArray.nextHopInfo.ipLttEntry.numOfPaths);

            /*
                1.7. Call with lpmDBId [1],
                                 out of range nextHopInfoPtr->ipLttEntry.numOfPaths
                                 ([64 / 65] - for lion and above and [8 / 9] for other)
                                 and other parameters from 1.5.
                Expected: NOT GT_OK.
            */
            /*call with 64 or 8 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                ipv6PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 64;
            }
            else
            {
                ipv6PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 8;
            }

            st = cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId, prefixLen,
                                        &ipv6PrefixArray, defragEnable);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, ipv6PrefixArray.nextHopInfo.ipLttEntry.numOfPaths);

            /*call with 65 or 9 values */
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                ipv6PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 65;
            }
            else
            {
                ipv6PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 9;
            }

            st = cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId, prefixLen,
                                        &ipv6PrefixArray, defragEnable);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, ipv6PrefixArray.nextHopInfo.ipLttEntry.numOfPaths);

            ipv6PrefixArray.nextHopInfo.ipLttEntry.numOfPaths = 0;


        }

        /*
            1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId, prefixLen, &ipv6PrefixArray, defragEnable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.9. Call with ipv6PrefixArray[NULL] and other parameters from 1.3.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId, prefixLen, NULL, defragEnable);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipv6PrefixArray = NULL", lpmDBId);

        /*
            1.10. Call cpssDxChIpLpmIpv6UcPrefixesFlush with lpmDBId [0 / 1]
                and vrId[0] to invalidate changes.
            Expected: GT_OK.
        */
        vrId = 0;

        /* Call with lpmDBId[0] */
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /* Call with lpmDBId[1] */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv6UcPrefixDel
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               vrId,
    IN GT_IPV6ADDR                          ipAddr,
    IN GT_U32                               prefixLen
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv6UcPrefixDel)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0], defUcNextHopInfoPtr{
        pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
        mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount = GT_FALSE,
        matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
        modifyUp=GT_FALSE , qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
        profilePrecedence=GT_FALSE] ] }, redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
        data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
        egressTaggedModify=GT_FALSE, modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
        nestedVlan=GT_FALSE, vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
        }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
        defMcRouteLttEntryPtr [NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK.
    1.3. Call cpssDxChIpLpmIpv6UcPrefixAdd with lpmDBId [0], vrId[0], ipAddr[10.15.0.1],
        prefixLen[128], nextHopInfoPtr->pclIpUcAction { pktCmd = CPSS_PACKET_CMD_FORWARD_E,
        mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
        matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
        egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
        ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
        redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
        policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
        modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
        precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
        doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } }, override [GT_TRUE].
    Expected: GT_OK.
    1.4. Call with lpmDBId [0], vrId[0], ipAddr[10.15.0.1], prefixLen[128].
    Expected: GT_OK.
    1.5. Call with lpmDBId [0], vrId[0], ipAddr[10.15.0.1] (already deleted), prefixLen[128].
    Expected: NOT GT_OK.
    1.6. Call with lpmDBId [0], vrId[0], ipAddr[10.15.0.2] (wrong IP address), prefixLen[128].
    Expected: NOT GT_OK.
    1.7. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.8. Call with not supported vrId [10] and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.9. Call with out of range prefixLen [129] and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.10. Call cpssDxChIpLpmIpv6UcPrefixesFlush with lpmDBId [0] and vrId[0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                                 lpmDBId      = 0;
    GT_U32                                 vrId         = 0;
    GT_IPV6ADDR                            ipAddr;
    GT_U32                                 prefixLen    = 0;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT nextHopInfo;
    GT_BOOL                                override     = GT_FALSE;
    GT_BOOL                                defragEnable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv6UcPrefixAdd with lpmDBId [0],
                 vrId[0], ipAddr[10.15.0.1], prefixLen[128], nextHopInfoPtr->pclIpUcAction {
                 pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode =
                 CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
                 matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
                 egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
                 ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                 profilePrecedence=GT_FALSE] ] }, redirect {
                 CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
                 policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
                 modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                 vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                 ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
                 override [GT_TRUE].
            Expected: GT_OK.
        */

        lpmDBId = 0;
        vrId    = 0;

        ipAddr.u32Ip[0] = 10;
        ipAddr.u32Ip[1] = 15;
        ipAddr.u32Ip[2] = 0;
        ipAddr.u32Ip[3] = 1;

        prefixLen = 128;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&nextHopInfo.ipLttEntry);
        }

        override     = GT_TRUE;
        defragEnable = GT_FALSE;

        st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo,
                                                                override, defragEnable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                    "cpssDxChIpLpmIpv6UcPrefixAdd: %d, %d, %d, override = %d",
                                     lpmDBId, vrId, prefixLen, override);

        /*
            1.4. Call with lpmDBId [0], vrId[0], ipAddr[10.15.0.1], prefixLen[128].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipAddr.u32Ip[0] = 10;
        ipAddr.u32Ip[1] = 15;
        ipAddr.u32Ip[2] = 0;
        ipAddr.u32Ip[3] = 1;

        prefixLen = 128;

        st = cpssDxChIpLpmIpv6UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /*
            1.5. Call with lpmDBId [0], vrId[0], ipAddr[10.15.0.1] (already deleted), prefixLen[128].
            Expected: NOT GT_OK.
        */
        st = cpssDxChIpLpmIpv6UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /*
            1.6. Call with lpmDBId [0], vrId[0], ipAddr[10.15.0.2] (wrong IP address), prefixLen[128].
            Expected: NOT GT_OK.
        */
        ipAddr.u32Ip[0] = 10;
        ipAddr.u32Ip[1] = 15;
        ipAddr.u32Ip[2] = 0;
        ipAddr.u32Ip[3] = 2;

        st = cpssDxChIpLpmIpv6UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /*
            1.7. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv6UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.8. Call with not supported vrId [10] and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv6UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.9. Call with out of range prefixLen [129] and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        prefixLen = 129;

        st = cpssDxChIpLpmIpv6UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, prefixLen = %d", lpmDBId, prefixLen);

        prefixLen = 128;

        /*
            1.10. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0 / 1]
                  and vrId[0] to invalidate changes.
            Expected: GT_OK.
        */
        vrId = 0;

        /* Call with lpmDBId[0] */
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /* Call with lpmDBId[1] */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv6UcPrefixDelBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC  *ipv6PrefixArray
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv6UcPrefixDelBulk)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. - 1.2.
    1.3. Call cpssDxChIpLpmIpv6UcPrefixAddBulk with lpmDBId[0],
       ipv4PrefixArrayLen[1],
       ipv4PrefixArray{vrId[0],
                       ipAddr[10.15.0.1],
                       prefixLen[128],
                       nextHopInfoPtr{ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                       mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                       mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter {
                       enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
                       egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
                       qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                       profilePrecedence=GT_FALSE] ] }, redirect {
                       CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0]
                       }, policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
                       egressTaggedModify=GT_FALSE,
                       modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                       nestedVlan=GT_FALSE, vlanId=100,
                       precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                       ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE,
                       GT_FALSE } },
                       numOfEcmpWaRouteEntries[1],
                       override[GT_TRUE],
                       returnStatus[0]}
    Expected: GT_OK.
    1.4. Call with lpmDBId[0],
                   ipv4PrefixArrayLen[1],
                   ipv4PrefixArray{vrId[0],
                                   ipAddr[10.15.0.1],
                                   prefixLen[128],
                                   nextHopInfoPtr{pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                                   mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                                   mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter {
                                   enableMatchCount = GT_FALSE, matchCounterIndex = 0
                                   }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
                                   modifyUp=GT_FALSE , qos [ ingress[profileIndex=0,
                                   profileAssignIndex=GT_FALSE,
                                   profilePrecedence=GT_FALSE] ] }, redirect {
                                   CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                                   data[routerLttIndex=0] }, policer {
                                   policerEnable=GT_FALSE, policerId=0 }, vlan {
                                   egressTaggedModify=GT_FALSE,
                                   modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                                   nestedVlan=GT_FALSE, vlanId=100,
                                   precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
                                   }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE,
                                   GT_FALSE, GT_FALSE } },
                   numOfEcmpWaRouteEntries[1],
                   override[GT_TRUE],
                   returnStatus[0]}
    Expected: GT_OK.
    1.5. Call with lpmDBId[0],
                                ipv4PrefixArrayLen[1],
                                ipv4PrefixArray{vrId[0], ipAddr[10.15.0.1],
                                prefixLen[128], nextHopInfoPtr{ pktCmd =
                                CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode =
                                CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                                mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter {
                                enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
                                qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
                                modifyUp=GT_FALSE , qos [ ingress[profileIndex=0,
                                profileAssignIndex=GT_FALSE,
                                profilePrecedence=GT_FALSE] ] }, redirect {
                                CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                                data[routerLttIndex=0] }, policer {
                                policerEnable=GT_FALSE, policerId=0 }, vlan {
                                egressTaggedModify=GT_FALSE,
                                modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                                nestedVlan=GT_FALSE, vlanId=100,
                                precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
                                }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE,
                                GT_FALSE, GT_FALSE } },
                                numOfEcmpWaRouteEntries[1],
                                override[GT_TRUE],
                                returnStatus[0]} (already deleted)
    Expected: NOT GT_OK.
    1.6. Call with not supported ipv4PrefixArray->vrId[10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.7. Call with out of range ipv4PrefixArray->prefixLen [129] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.9. Call with ipv4PrefixArray[NULL] and other parameters from 1.4.
    Expected: GT_BAD_PTR.
    1.10. Call with correct nextHopInfoPtr->ipLttEntry.numOfPaths
                     ([5 / 7] - for lion and above and [49 / 63] for other)
                     and other parameters from 1.3.
    Expected: GT_OK.
    1.11. Call with lpmDBId [1],
                     out of range nextHopInfoPtr->ipLttEntry.numOfPaths
                     ([64 / 65] - for lion and above and [8 / 9] for other)
                     and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.12. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0 / 1]
          and vrId[0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                              lpmDBId      = 0;
    GT_U32                              arrayLen     = 0;
    CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC ipv6Prefix;
    GT_U32                              vrId         = 0;
    CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC ipv6PrefixArray;
    GT_U32                              prefixLen    = 0;
    GT_BOOL                             defragEnable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv6UcPrefixAddBulk with lpmDBId[0],
               ipv4PrefixArrayLen[1],
               ipv4PrefixArray{vrId[0],
                               ipAddr[10.15.0.1],
                               prefixLen[128],
                               nextHopInfoPtr{ pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                               mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                               mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter {
                               enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
                               egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE ,
                               qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                               profilePrecedence=GT_FALSE] ] }, redirect {
                               CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0]
                               }, policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
                               egressTaggedModify=GT_FALSE,
                               modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                               nestedVlan=GT_FALSE, vlanId=100,
                               precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                               ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE,
                               GT_FALSE } },
                               numOfEcmpWaRouteEntries[1],
                               override[GT_TRUE],
                               returnStatus[0]}
            Expected: GT_OK.
        */
        lpmDBId   = 0;
        prefixLen = 1;

        ipv6PrefixArray.ipAddr.u32Ip[0] = 10;
        ipv6PrefixArray.ipAddr.u32Ip[1] = 15;
        ipv6PrefixArray.ipAddr.u32Ip[2] = 0;
        ipv6PrefixArray.ipAddr.u32Ip[3] = 1;

        ipv6PrefixArray.prefixLen = 128;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&ipv6PrefixArray.nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&ipv6PrefixArray.nextHopInfo.ipLttEntry);
        }

        ipv6PrefixArray.numOfEcmpWaRouteEntries = 1;
        ipv6PrefixArray.override                = GT_TRUE;
        ipv6PrefixArray.returnStatus            = GT_OK;
        ipv6PrefixArray.vrId = 0;

        defragEnable = GT_FALSE;

        st = cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId, prefixLen, &ipv6PrefixArray, defragEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, prefixLen);

        /*
            1.4. Call with lpmDBId[0],
                           ipv4PrefixArrayLen[1],
                           ipv4PrefixArray{vrId[0],
                                           ipAddr[10.15.0.1],
                                           prefixLen[128],
                                           nextHopInfoPtr{pktCmd = CPSS_PACKET_CMD_FORWARD_E,
                                           mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                                           mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter {
                                           enableMatchCount = GT_FALSE, matchCounterIndex = 0
                                           }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
                                           modifyUp=GT_FALSE , qos [ ingress[profileIndex=0,
                                           profileAssignIndex=GT_FALSE,
                                           profilePrecedence=GT_FALSE] ] }, redirect {
                                           CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                                           data[routerLttIndex=0] }, policer {
                                           policerEnable=GT_FALSE, policerId=0 }, vlan {
                                           egressTaggedModify=GT_FALSE,
                                           modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                                           nestedVlan=GT_FALSE, vlanId=100,
                                           precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
                                           }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE,
                                           GT_FALSE, GT_FALSE } },
                           numOfEcmpWaRouteEntries[1],
                           override[GT_TRUE],
                           returnStatus[0]}
            Expected: GT_OK.
        */
        lpmDBId  = 0;
        arrayLen = 1;

        ipv6Prefix.vrId = 0;

        ipv6Prefix.ipAddr.u32Ip[0] = 10;
        ipv6Prefix.ipAddr.u32Ip[1] = 15;
        ipv6Prefix.ipAddr.u32Ip[2] = 0;
        ipv6Prefix.ipAddr.u32Ip[3] = 1;

        ipv6Prefix.prefixLen = 128;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&ipv6PrefixArray.nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&ipv6PrefixArray.nextHopInfo.ipLttEntry);
        }

        ipv6Prefix.numOfEcmpWaRouteEntries = 1;
        ipv6Prefix.override                = GT_TRUE;
        ipv6Prefix.returnStatus            = GT_OK;

        st = cpssDxChIpLpmIpv6UcPrefixDelBulk(lpmDBId, arrayLen, &ipv6Prefix);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, arrayLen);

        /*
            1.5. Call with lpmDBId[0],
                                        ipv4PrefixArrayLen[1],
                                        ipv4PrefixArray{vrId[0], ipAddr[10.15.0.1],
                                        prefixLen[128], nextHopInfoPtr{ pktCmd =
                                        CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode =
                                        CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
                                        mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter {
                                        enableMatchCount = GT_FALSE, matchCounterIndex = 0 },
                                        qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
                                        modifyUp=GT_FALSE , qos [ ingress[profileIndex=0,
                                        profileAssignIndex=GT_FALSE,
                                        profilePrecedence=GT_FALSE] ] }, redirect {
                                        CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
                                        data[routerLttIndex=0] }, policer {
                                        policerEnable=GT_FALSE, policerId=0 }, vlan {
                                        egressTaggedModify=GT_FALSE,
                                        modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                                        nestedVlan=GT_FALSE, vlanId=100,
                                        precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
                                        }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE,
                                        GT_FALSE, GT_FALSE } },
                                        numOfEcmpWaRouteEntries[1],
                                        override[GT_TRUE],
                                        returnStatus[0]} (already deleted)
            Expected: NOT GT_OK.
        */
        st = cpssDxChIpLpmIpv6UcPrefixDelBulk(lpmDBId, arrayLen, &ipv6Prefix);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, arrayLen);

        /*
            1.6. Call with not supported ipv4PrefixArray->vrId[10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        ipv6Prefix.vrId = 10;

        st = cpssDxChIpLpmIpv6UcPrefixDelBulk(lpmDBId, arrayLen, &ipv6Prefix);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipv6PrefixArray->vrId = %d",
                                                                lpmDBId, ipv6Prefix.vrId);

        ipv6Prefix.vrId = 0;

        /*
            1.7. Call with out of range ipv4PrefixArray->prefixLen [129] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        ipv6Prefix.prefixLen = 129;

        st = cpssDxChIpLpmIpv6UcPrefixDelBulk(lpmDBId, arrayLen, &ipv6Prefix);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipv6PrefixArray->prefixLen = %d",
                                                            lpmDBId, ipv6Prefix.prefixLen);

        ipv6Prefix.prefixLen = 128;

        /*
            1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */

        lpmDBId = 10;

        st = cpssDxChIpLpmIpv6UcPrefixDelBulk(lpmDBId, arrayLen, &ipv6Prefix);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.9. Call with ipv4PrefixArray[NULL] and other parameters from 1.4.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv6UcPrefixDelBulk(lpmDBId, arrayLen, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, ipv6PrefixArray = NULL", lpmDBId);

        /*
            1.10. Call with correct nextHopInfoPtr->ipLttEntry.numOfPaths
                             ([5 / 7] - for lion and above and [49 / 63] for other)
                             and other parameters from 1.3.
            Expected: GT_OK.
        */
        /*call with 49 or 5 values */
        if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
        {
            ipv6Prefix.nextHopInfo.ipLttEntry.numOfPaths = 49;
        }
        else
        {
            ipv6Prefix.nextHopInfo.ipLttEntry.numOfPaths = 5;
        }
        /* add prefixes first */
        st = cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId, prefixLen, &ipv6PrefixArray, defragEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, prefixLen);

        st = cpssDxChIpLpmIpv6UcPrefixDelBulk(lpmDBId, arrayLen, &ipv6Prefix);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                      lpmDBId, vrId, ipv6Prefix.nextHopInfo.ipLttEntry.numOfPaths);

        /*call with 63 or 7 values */
        if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
        {
            ipv6Prefix.nextHopInfo.ipLttEntry.numOfPaths = 63;
        }
        else
        {
            ipv6Prefix.nextHopInfo.ipLttEntry.numOfPaths = 7;
        }

        /* add prefixes first */
        st = cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId, prefixLen, &ipv6PrefixArray, defragEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, prefixLen);

        st = cpssDxChIpLpmIpv6UcPrefixDelBulk(lpmDBId, arrayLen, &ipv6Prefix);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                  lpmDBId, vrId, ipv6Prefix.nextHopInfo.ipLttEntry.numOfPaths);

        /*
            1.11. Call with lpmDBId [1],
                             out of range nextHopInfoPtr->ipLttEntry.numOfPaths
                             ([64 / 65] - for lion and above and [8 / 9] for other)
                             and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        /*call with 64 or 8 values */
        if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
        {
            ipv6Prefix.nextHopInfo.ipLttEntry.numOfPaths = 64;
        }
        else
        {
            ipv6Prefix.nextHopInfo.ipLttEntry.numOfPaths = 8;
        }

        st = cpssDxChIpLpmIpv6UcPrefixDelBulk(lpmDBId, arrayLen, &ipv6Prefix);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                  lpmDBId, vrId, ipv6Prefix.nextHopInfo.ipLttEntry.numOfPaths);

        /*call with 65 or 9 values */
        if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
        {
            ipv6Prefix.nextHopInfo.ipLttEntry.numOfPaths = 65;
        }
        else
        {
            ipv6Prefix.nextHopInfo.ipLttEntry.numOfPaths = 9;
        }

        st = cpssDxChIpLpmIpv6UcPrefixDelBulk(lpmDBId, arrayLen, &ipv6Prefix);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, %d, nextHopInfoPtr->ipLttEntry.numOfPaths %d",
                  lpmDBId, vrId, ipv6Prefix.nextHopInfo.ipLttEntry.numOfPaths);

        ipv6Prefix.nextHopInfo.ipLttEntry.numOfPaths = 0;


        /*
            1.12. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0 / 1]
                  and vrId[0] to invalidate changes.
            Expected: GT_OK.
        */
        vrId = 0;

        /* Call with lpmDBId[0] */
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /* Call with lpmDBId[1] */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv6UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv6UcPrefixesFlush)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0], defUcNextHopInfoPtr{
        pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
        mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount = GT_FALSE,
        matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
        modifyUp=GT_FALSE , qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
        profilePrecedence=GT_FALSE] ] }, redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
        data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
        egressTaggedModify=GT_FALSE, modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
        nestedVlan=GT_FALSE, vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
        }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
        defMcRouteLttEntryPtr [NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK.
    1.3. Call with lpmDBId [0], vrId[0].
    Expected: GT_OK.
    1.4. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.5. Call with not supported vrId [1] and other parameters from 1.3.
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                   lpmDBId = 0;
    GT_U32                   vrId    = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call with lpmDBId [0], vrId[0].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        st = cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId, vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /*
            1.4. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId, vrId);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.5. Call with not supported vrId [1] and other parameters from 1.3.
            Expected: NOT GT_OK.
        */

        lpmDBId = 10;

        st = cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId, vrId);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv6UcPrefixSearch
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPV6ADDR                             ipAddr,
    IN  GT_U32                                  prefixLen,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv6UcPrefixSearch)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0], defUcNextHopInfoPtr{
        pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
        mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount = GT_FALSE,
        matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
        modifyUp=GT_FALSE , qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
        profilePrecedence=GT_FALSE] ] }, redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
        data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
        egressTaggedModify=GT_FALSE, modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
        nestedVlan=GT_FALSE, vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
        }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
        defMcRouteLttEntryPtr [NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK.
    1.3. Call cpssDxChIpLpmIpv4UcPrefixAdd with lpmDBId [0], vrId[0], ipAddr[10.15.0.1],
        prefixLen[128], nextHopInfoPtr->pclIpUcAction { pktCmd = CPSS_PACKET_CMD_FORWARD_E,
        mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
        matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
        egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
        ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
        redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
        policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
        modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
        precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
        doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } }, override [GT_TRUE].
    Expected: GT_OK.
    1.4. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.1], prefixLen[128] and non-null
        nextHopInfoPtr
    Expected: GT_OK and the same nextHopInfoPtr.
    1.5. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.1], prefixLen[124] and non-null
        nextHopInfoPtr
    Expected: GT_OK for Cheetah+ and NOT GT_OK for others and the same nextHopInfoPtr.
    1.6. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.255], prefixLen[124] and non-null
        nextHopInfoPtr
    Expected: GT_OK for Cheetah+ and NOT GT_OK for others and the same nextHopInfoPtr.
    1.7. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.255], prefixLen[128] and non-null
        nextHopInfoPtr
    Expected: NOT GT_OK.
    1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.9. Call with not supported vrId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.10. Call with out of range prefixLen [129] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.11. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.1], prefixLen[128] and nextHopInfoPtr[NULL].
    Expected: GT_BAD_PTR.
    1.12. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0] and vrId[0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                                 lpmDBId      = 0;
    GT_U32                                 vrId         = 0;
    GT_IPV6ADDR                            ipAddr;
    GT_U32                                 prefixLen    = 0;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT nextHopInfo;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT nextHopInfoGet;
    GT_BOOL                                override     = GT_FALSE;
    GT_BOOL                                defragEnable = GT_FALSE;
    GT_U32                                 tcamRowIndex;
    GT_U32                                 tcamColumIndex;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv6UcPrefixAdd with lpmDBId [0],
                 vrId[0], ipAddr[10.15.0.1], prefixLen[128], nextHopInfoPtr->pclIpUcAction {
                 pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode =
                 CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
                 matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
                 egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
                 ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                 profilePrecedence=GT_FALSE] ] }, redirect {
                 CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
                 policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
                 modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                 vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                 ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
                 override [GT_TRUE].
            Expected: GT_OK.
        */

        lpmDBId = 0;
        vrId    = 0;

        ipAddr.u32Ip[0] = 10;
        ipAddr.u32Ip[1] = 15;
        ipAddr.u32Ip[2] = 0;
        ipAddr.u32Ip[3] = 1;

        prefixLen = 128;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&nextHopInfo.ipLttEntry);
        }

        override     = GT_TRUE;
        defragEnable = GT_FALSE;

        st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                          &nextHopInfo, override, defragEnable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                "cpssDxChIpLpmIpv6UcPrefixAdd: %d, %d, %d, override = %d",
                                     lpmDBId, vrId, prefixLen, override);

        /*
            1.4. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.1],
                 prefixLen[128] and non-null nextHopInfoPtr
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipAddr.u32Ip[0] = 10;
        ipAddr.u32Ip[1] = 15;
        ipAddr.u32Ip[2] = 0;
        ipAddr.u32Ip[3] = 1;

        prefixLen = 128;

        st = cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                              &nextHopInfoGet, &tcamRowIndex,
                                              &tcamColumIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDBId, vrId, prefixLen);

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfComparePclIpUcActionEntries(lpmDBId, vrId, &nextHopInfoGet.pclIpUcAction,
                                                                &nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfCompareIpLttEntries(lpmDBId, vrId, &nextHopInfoGet.ipLttEntry, &nextHopInfo.ipLttEntry);
        }

        /*
            1.5. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.1], prefixLen[124] and non-null nextHopInfoPtr
            Expected: GT_OK for Cheetah+ and NOT GT_OK for others and the same nextHopInfoPtr.

        */
        prefixLen = 124;

        st = cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                             &nextHopInfoGet, &tcamRowIndex,
                                             &tcamColumIndex);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDBId, vrId, prefixLen);

        /*
            1.6. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.255], prefixLen[124] and non-null nextHopInfoPtr
            Expected: GT_OK for Cheetah+ and NOT GT_OK for others and the same nextHopInfoPtr.

        */
        prefixLen = 124;

        ipAddr.u32Ip[0] = 10;
        ipAddr.u32Ip[1] = 15;
        ipAddr.u32Ip[2] = 0;
        ipAddr.u32Ip[3] = 255;

        st = cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                             &nextHopInfoGet, &tcamRowIndex,
                                             &tcamColumIndex);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDBId, vrId, prefixLen);

        /*
            1.7. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.255], prefixLen[128] and non-null nextHopInfoPtr
            Expected: NOT GT_OK.
        */
        prefixLen = 128;

        st = cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                             &nextHopInfoGet, &tcamRowIndex,
                                             &tcamColumIndex);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDBId, vrId, prefixLen);

        /*
            1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                              &nextHopInfoGet, &tcamRowIndex,
                                              &tcamColumIndex);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.9. Call with not supported vrId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                              &nextHopInfoGet, &tcamRowIndex,
                                              &tcamColumIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.10. Call with out of range prefixLen [129] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        prefixLen = 129;

        st = cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                              &nextHopInfoGet, &tcamRowIndex,
                                              &tcamColumIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, prefixLen = %d", lpmDBId, prefixLen);

        prefixLen = 128;

        /*
            1.11. Call with lpmDBId[0], vrId[0], ipAddr[10.15.0.1], prefixLen[32] and nextHopInfoPtr[NULL].
            Expected: GT_BAD_PTR.
        */
        prefixLen = 32;

        ipAddr.u32Ip[0] = 10;
        ipAddr.u32Ip[1] = 15;
        ipAddr.u32Ip[2] = 0;
        ipAddr.u32Ip[3] = 1;

        st = cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                              NULL, &tcamRowIndex,&tcamColumIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nextHopInfoPtr = NULL", lpmDBId, vrId);

        /*
            1.12. Call cpssDxChIpLpmIpv6UcPrefixesFlush with lpmDBId [0] and vrId[0] to invalidate changes.
            Expected: GT_OK.
        */
        vrId    = 0;
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv6UcPrefixGetNext
(
    IN    GT_U32                                    lpmDBId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPV6ADDR                               *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr,
    OUT GT_U32                                      *tcamRowIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv6UcPrefixGetNext)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0], defUcNextHopInfoPtr{
        pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
        mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount = GT_FALSE,
        matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
        modifyUp=GT_FALSE , qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
        profilePrecedence=GT_FALSE] ] }, redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
        data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
        egressTaggedModify=GT_FALSE, modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
        nestedVlan=GT_FALSE, vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
        }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
        defMcRouteLttEntryPtr [NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK.
    1.3. Call cpssDxChIpLpmIpv4UcPrefixAdd with lpmDBId [0], vrId[0], ipAddr[10.15.0.1],
        prefixLen[128], nextHopInfoPtr->pclIpUcAction { pktCmd = CPSS_PACKET_CMD_FORWARD_E,
        mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
        matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
        egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
        ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
        redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
        policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
        modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
        precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
        doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } }, override [GT_TRUE].
    Expected: GT_OK.
    1.4. Call cpssDxChIpLpmIpv4UcPrefixAdd with lpmDBId [0], vrId[0], ipAddr[10.15.0.2],
        prefixLen[128], nextHopInfoPtr->pclIpUcAction { pktCmd = CPSS_PACKET_CMD_FORWARD_E,
        mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
        matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
        egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
        ingress[profileIndex=0, profileAssignIndex=GT_FALSE, profilePrecedence=GT_FALSE] ] },
        redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
        policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
        modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE, vlanId=100,
        precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E }, ipUcRoute {
        doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } }, override [GT_TRUE].
    Expected: GT_OK.
    1.5. Call with lpmDBId[0], vrId[0], ipAddrPtr [10.15.0.1], prefixLenPtr [128] and non-null nextHopInfoPtr
    Expected: GT_OK and the same prefixLenPtr  and nextHopInfoPtr.
    1.6. Call with lpmDBId[0], vrId[0], ipAddrPtr [10.15.0.2], prefixLenPtr [128] and non-null nextHopInfoPtr
    Expected: NOT GT_OK (the last prefix reached).
    1.7. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.8. Call with not supported vrId [10] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.9. Call with out of range prefixLenPtr [129] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.10. Call with lpmDBId[0], vrId[0], ipAddrPtr [NULL] and other parameters from 1.5.
    Expected: GT_BAD_PTR.
    1.11. Call with lpmDBId[0], vrId[0], prefixLenPtr [NULL] and other parameters from 1.5.
    Expected: GT_BAD_PTR.
    1.12. Call with lpmDBId[0], vrId[0], nextHopInfoPtr [NULL] and other parameters from 1.5.
    Expected: GT_BAD_PTR.
    1.13. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0] and vrId[0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                                 lpmDBId      = 0;
    GT_U32                                 vrId         = 0;
    GT_IPV6ADDR                            ipAddr;
    GT_IPV6ADDR                            ipAddrTmp;
    GT_U32                                 prefixLen    = 0;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT nextHopInfo;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT nextHopInfoGet;
    GT_BOOL                                override     = GT_FALSE;
    GT_BOOL                                defragEnable = GT_FALSE;
    GT_U32                                 tcamRowIndex;
    GT_U32                                 tcamColumnIndex;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv4UcPrefixAdd with lpmDBId [0],
                 vrId[0], ipAddr[10.15.0.1], prefixLen[128], nextHopInfoPtr->pclIpUcAction {
                 pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode =
                 CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
                 matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
                 egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
                 ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                 profilePrecedence=GT_FALSE] ] }, redirect {
                 CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
                 policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
                 modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                 vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                 ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
                 override [GT_TRUE].
            Expected: GT_OK.
        */

        lpmDBId = 0;
        vrId    = 0;

        ipAddr.u32Ip[0] = 10;
        ipAddr.u32Ip[1] = 15;
        ipAddr.u32Ip[2] = 0;
        ipAddr.u32Ip[3] = 1;

        prefixLen = 128;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&nextHopInfo.ipLttEntry);
        }

        override     = GT_TRUE;
        defragEnable = GT_FALSE;

        st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override, defragEnable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChIpLpmIpv6UcPrefixAdd: %d, %d, %d, override = %d",
                                     lpmDBId, vrId, prefixLen, override);

        /*
            1.4. Call cpssDxChIpLpmIpv6UcPrefixAdd with lpmDBId [0],
                 vrId[0], ipAddr[10.15.0.2], prefixLen[128], nextHopInfoPtr->pclIpUcAction {
                 pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode =
                 CPSS_NET_CONTROL_DEST_MAC_TRAP_E, mirrorToRxAnalyzerPort = GT_FALSE},
                 matchCounter { enableMatchCount = GT_FALSE, matchCounterIndex = 0 }, qos {
                 egressPolicy=GT_FALSE, modifyDscp=GT_FALSE, modifyUp=GT_FALSE , qos [
                 ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
                 profilePrecedence=GT_FALSE] ] }, redirect {
                 CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E, data[routerLttIndex=0] }, policer {
                 policerEnable=GT_FALSE, policerId=0 }, vlan { egressTaggedModify=GT_FALSE,
                 modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E, nestedVlan=GT_FALSE,
                 vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E },
                 ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
                 override [GT_TRUE].
            Expected: GT_OK.
        */

        lpmDBId = 0;
        vrId    = 0;

        ipAddr.u32Ip[0] = 10;
        ipAddr.u32Ip[1] = 15;
        ipAddr.u32Ip[2] = 0;
        ipAddr.u32Ip[3] = 2;

        prefixLen = 128;

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfSetDefaultActionEntry(&nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfSetDefaultIpLttEntry(&nextHopInfo.ipLttEntry);
        }

        override     = GT_TRUE;
        defragEnable = GT_FALSE;

        st = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &nextHopInfo, override, defragEnable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChIpLpmIpv6UcPrefixAdd: %d, %d, %d, override = %d",
                                     lpmDBId, vrId, prefixLen, override);

        /*
            1.5. Call with lpmDBId[0], vrId[0], ipAddrPtr [10.15.0.1],
                    prefixLenPtr [128] and non-null nextHopInfoPtr
            Expected: GT_OK and the same prefixLenPtr  and nextHopInfoPtr.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipAddr.u32Ip[0] = 10;
        ipAddr.u32Ip[1] = 15;
        ipAddr.u32Ip[2] = 0;
        ipAddr.u32Ip[3] = 1;

        ipAddrTmp.u32Ip[0] = 10;
        ipAddrTmp.u32Ip[1] = 15;
        ipAddrTmp.u32Ip[2] = 0;
        ipAddrTmp.u32Ip[3] = 1;

        prefixLen = 128;

        st = cpssDxChIpLpmIpv6UcPrefixGetNext(lpmDBId, vrId, &ipAddr, &prefixLen,
                                               &nextHopInfoGet, &tcamRowIndex,
                                               &tcamColumnIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        if (CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily || prvUtfIsPbrModeUsed())
        {
            prvUtfComparePclIpUcActionEntries(lpmDBId, vrId, &nextHopInfoGet.pclIpUcAction,
                                                                &nextHopInfo.pclIpUcAction);
        }
        else
        {
            prvUtfCompareIpLttEntries(lpmDBId, vrId, &nextHopInfoGet.ipLttEntry,
                                                                &nextHopInfo.ipLttEntry);
        }

        /* Verifying ipAddr */
        UTF_VERIFY_EQUAL2_STRING_MAC(ipAddr.arIP[0], ipAddrTmp.arIP[0],
                   "get another ipAddrPtr[0] than was set: %d, %d", lpmDBId, vrId);
        UTF_VERIFY_EQUAL2_STRING_MAC(ipAddr.arIP[1], ipAddrTmp.arIP[1],
                   "get another ipAddrPtr[1] than was set: %d, %d", lpmDBId, vrId);
        UTF_VERIFY_EQUAL2_STRING_MAC(ipAddr.arIP[2], ipAddrTmp.arIP[2],
                   "get another ipAddrPtr[2] than was set: %d, %d", lpmDBId, vrId);
        UTF_VERIFY_EQUAL2_STRING_MAC(ipAddr.arIP[3], ipAddrTmp.arIP[3],
                   "get another ipAddrPtr[3] than was set: %d, %d", lpmDBId, vrId);

        /*
            1.6. Call with lpmDBId[0], vrId[0], ipAddrPtr [10.15.0.2],
                 prefixLenPtr [128] and non-null nextHopInfoPtr
            Expected: NOT GT_OK (the last prefix reached).
        */
        ipAddr.u32Ip[0] = 10;
        ipAddr.u32Ip[1] = 15;
        ipAddr.u32Ip[2] = 0;
        ipAddr.u32Ip[3] = 2;

        st = cpssDxChIpLpmIpv6UcPrefixGetNext(lpmDBId, vrId, &ipAddr, &prefixLen,
                                              &nextHopInfoGet, &tcamRowIndex,&tcamColumnIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /*
            1.7. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv6UcPrefixGetNext(lpmDBId, vrId, &ipAddr, &prefixLen,
                                               &nextHopInfoGet, &tcamRowIndex,
                                               &tcamColumnIndex);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.8. Call with not supported vrId [10] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv6UcPrefixGetNext(lpmDBId, vrId, &ipAddr, &prefixLen,
                                               &nextHopInfoGet, &tcamRowIndex,
                                               &tcamColumnIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.9. Call with out of range prefixLenPtr [129] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        prefixLen = 129;

        st = cpssDxChIpLpmIpv6UcPrefixGetNext(lpmDBId, vrId, &ipAddr, &prefixLen,
                                               &nextHopInfoGet, &tcamRowIndex,
                                               &tcamColumnIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, prefixLenPtr = %d", lpmDBId, prefixLen);

        prefixLen = 128;

        /*
            1.10. Call with lpmDBId[0], vrId[0], ipAddrPtr [NULL] and other parameters from 1.5.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv6UcPrefixGetNext(lpmDBId, vrId, NULL, &prefixLen,
                                              &nextHopInfoGet, &tcamRowIndex,
                                              &tcamColumnIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ipAddrPtr = NULL", lpmDBId, vrId);

        /*
            1.11. Call with lpmDBId[0], vrId[0], prefixLenPtr [NULL] and other parameters from 1.5.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv6UcPrefixGetNext(lpmDBId, vrId, &ipAddr, NULL,
                                              &nextHopInfoGet, &tcamRowIndex,
                                              &tcamColumnIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, prefixLenPtr = NULL", lpmDBId, vrId);

        /*
            1.12. Call with lpmDBId[0], vrId[0], nextHopInfoPtr [NULL] and other parameters from 1.5.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv6UcPrefixGetNext(lpmDBId, vrId, &ipAddr, &prefixLen,
                                              NULL, &tcamRowIndex,
                                              &tcamColumnIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nextHopInfoPtr = NULL", lpmDBId, vrId);

        /*
            1.13. Call cpssDxChIpLpmIpv4UcPrefixesFlush with lpmDBId [0] and vrId[0] to invalidate changes.
            Expected: GT_OK.
        */
        vrId    = 0;
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId , vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv6McEntryAdd
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPV6ADDR                  ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPV6ADDR                  ipSrc,
    IN GT_U32                       ipSrcPrefixLen,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC   *mcRouteLttEntryPtr,
    IN GT_BOOL                      override,
    IN GT_BOOL                      defragmentationEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv6McEntryAdd)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. - 1.3.
    1.4. Call with lpmDBId[0 / 1], vrId[0], ipGroup[255.15.0.1], ipGroupPrefixLen[32],
        ipSrc[255.16.0.1], ipSrcPrefixLen[32],
        mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0],
        routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
        sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, override[GT_TRUE],
        defragmentationEnable[GT_FALSE].
    Expected: GT_OK (no any constraints for any devices).
    1.5. Call with lpmDBId [1], vrId[0],
         wrong enum values mcRouteLttEntryPtr->routeType and other parameters from 1.4.
    Expected: GT_BAD_PARAM.
    1.6. Call with lpmDBId [1], vrId[0],
                                out of range mcRouteLttEntryPtr->numOfPaths [8 / 63]
                                and othe parameters from 1.4.
    Expected: GT_OK for Lion and above and not GT_OK for other.
    1.7. Call with lpmDBId [1], vrId[0],
                                out of range mcRouteLttEntryPtr->numOfPaths [64]
                                and othe parameters from 1.4.
    Expected: NOT GT_OK
    1.8. Call with lpmDBId [1], vrId[0],
                    out of range mcRouteLttEntryPtr->routeEntryBaseIndex [4096]
                    and othe parameters from 1.4.
    Expected: NOT GT_OK.
    1.9. Call with lpmDBId [1], vrId[0],
                    wrong enum values mcRouteLttEntryPtr->ipv6MCGroupScopeLevel
                    and othe parameters from 1.4.
    Expected: GT_BAD_PARAM.
    1.10. Call with lpmDBId [1], out of range vrId[10] and other valid parameters.
    Expected: NOT GT_OK.
    1.11. Call with out of range ipGroupPrefixLen [129] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.12. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.13. Call with lpmDBId [1], vrId[0], mcRouteLttEntryPtr [NULL]
    and other valid parameters.
    Expected: GT_BAD_PTR.
    1.14. Call cpssDxChIpLpmIpv6McEntriesFlush with with lpmDBId [0 / 1],
          vrId[0] to invalidate entries.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;



    GT_U32                     lpmDBId          = 0;
    GT_U32                     vrId             = 0;
    GT_IPV6ADDR                ipGroup;
    GT_U32                     ipGroupPrefixLen = 0;
    GT_IPV6ADDR                ipSrc;
    GT_U32                     ipSrcPrefixLen   = 0;
    CPSS_DXCH_IP_LTT_ENTRY_STC mcRouteLttEntry;
    GT_BOOL                    override         = GT_FALSE;
    GT_BOOL                    defragEnable     = GT_FALSE;
    GT_U8                      devList[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U32                     numOfDevs = 1;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        devList[0] = dev;
        /*
            1.1. - 1.3.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.4. Call with lpmDBId[0 / 1], vrId[0], ipGroup[255.15.0.1], ipGroupPrefixLen[32],
                ipSrc[255.16.0.1], ipSrcPrefixLen[32],
                mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0],
                routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
                sipSaCheckMismatchEnable[GT_TRUE],
                ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, override[GT_TRUE],
                defragmentationEnable[GT_FALSE].
            Expected: GT_OK (no any constraints for any devices).
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 255;
        ipGroup.arIP[1] = 15;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 1;

        ipGroupPrefixLen = 32;

        ipSrc.arIP[0] = 255;
        ipSrc.arIP[1] = 15;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 0;

        prvUtfSetDefaultIpLttEntry(&mcRouteLttEntry);

        override     = GT_TRUE;
        defragEnable = GT_FALSE;

        st = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                         ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, override = %d, defragmentationEnable = %d",
                                             lpmDBId, vrId, override, defragEnable);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, override = %d, defragmentationEnable = %d",
                                         lpmDBId, vrId, override, defragEnable);
        }

        /*
            1.5. Call with lpmDBId [1], vrId[0],
                 wrong enum values mcRouteLttEntryPtr->routeType and other parameters from 1.4.
            Expected: GT_BAD_PARAM.
        */
        lpmDBId = 1;
        vrId    = 0;

        if (!prvUtfIsPbrModeUsed())
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmIpv6McEntryAdd
                                (lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                 ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable),
                                mcRouteLttEntry.routeType);
        }

        /*
            1.6. Call with lpmDBId [1], vrId[0],
                                        out of range mcRouteLttEntryPtr->numOfPaths [8 / 63]
                                        and othe parameters from 1.4.
            Expected: GT_OK for Lion and above and not GT_OK for other.
        */
        /* call with mcRouteLttEntry.numOfPaths = 8 */
        mcRouteLttEntry.numOfPaths = 8;

        st = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                     ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);

        if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, mcRouteLttEntry.numOfPaths = %d",
                                           lpmDBId, vrId, mcRouteLttEntry.numOfPaths);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, mcRouteLttEntry.numOfPaths = %d",
                                           lpmDBId, vrId, mcRouteLttEntry.numOfPaths);
        }

        /* call with mcRouteLttEntry.numOfPaths = 63 */
        mcRouteLttEntry.numOfPaths = 63;

        st = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                     ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);

        if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, mcRouteLttEntry.numOfPaths = %d",
                                           lpmDBId, vrId, mcRouteLttEntry.numOfPaths);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, mcRouteLttEntry.numOfPaths = %d",
                                           lpmDBId, vrId, mcRouteLttEntry.numOfPaths);
        }

        mcRouteLttEntry.numOfPaths = 0;

        /*
            1.7. Call with lpmDBId [1], vrId[0],
                                        out of range mcRouteLttEntryPtr->numOfPaths [64]
                                        and othe parameters from 1.4.
            Expected: NOT GT_OK
        */
        mcRouteLttEntry.numOfPaths = 64;

        st = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                     ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);

        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, mcRouteLttEntry.numOfPaths = %d",
                                       lpmDBId, vrId, mcRouteLttEntry.numOfPaths);

        mcRouteLttEntry.numOfPaths = 0;


        /*
            1.8. Call with lpmDBId [1], vrId[0],
                            out of range mcRouteLttEntryPtr->routeEntryBaseIndex [4096]
                            and othe parameters from 1.4.
            Expected: NOT GT_OK.
        */
        mcRouteLttEntry.routeEntryBaseIndex = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.routerNextHop;

        st = cpssDxChIpLpmDBDevListAdd(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfDevs = %d", lpmDBId, numOfDevs);

        st = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                     ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, mcRouteLttEntryPtr->routeEntryBaseIndex = %d",
                                         lpmDBId, vrId, mcRouteLttEntry.routeEntryBaseIndex);

        mcRouteLttEntry.routeEntryBaseIndex = 0;

        /*
            1.9. Call with lpmDBId [1], vrId[0],
                            wrong enum values mcRouteLttEntryPtr->ipv6MCGroupScopeLevel
                            and othe parameters from 1.4.
            Expected: GT_BAD_PARAM.
        */
        if (!prvUtfIsPbrModeUsed())
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmIpv6McEntryAdd
                                (lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                 ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable),
                                mcRouteLttEntry.ipv6MCGroupScopeLevel);
        }

        /*
            1.10. Call with lpmDBId [1], out of range vrId[10] and other valid parameters.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                     ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.11. Call with out of range ipGroupPrefixLen [129] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        ipGroupPrefixLen = 129;

        st = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                            ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                            "%d, ipGroupPrefixLen = %d", lpmDBId, ipGroupPrefixLen);

        ipGroupPrefixLen = 128;

        /*
            1.12. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                            ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.13. Call with lpmDBId [1], vrId[0], mcRouteLttEntryPtr [NULL]
            and other valid parameters.
            Expected: GT_BAD_PTR.
        */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                         ipSrcPrefixLen, NULL, override, defragEnable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                    "%d, %d, mcRouteLttEntryPtr = NULL", lpmDBId, vrId);

        /*
            1.14. Call cpssDxChIpLpmIpv6McEntriesFlush with with lpmDBId [0 / 1],
                  vrId[0] to invalidate entries.
            Expected: GT_OK.
        */
        vrId = 0;

        /* Call with lpmDBId [0] */
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        /* Call with lpmDBId [1] */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv6McEntryDel
(
    IN GT_U32       lpmDBId,
    IN GT_U32       vrId,
    IN GT_IPV6ADDR  ipGroup,
    IN GT_U32       ipGroupPrefixLen,
    IN GT_IPV6ADDR  ipSrc,
    IN GT_U32       ipSrcPrefixLen
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv6McEntryDel)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [1], vrId[0],
        defUcNextHopInfoPtr->ipLttEntry{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
        numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
        sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, defMcRouteLttEntryPtr {
        routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0], routeEntryBaseIndex[0],
        ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
    Expected: GT_OK .
    1.3. Call cpssDxChIpLpmIpv4McEntryAdd with lpmDBId[0], vrId[0], ipGroup[255.15.0.1],
        ipGroupPrefixLen[32], ipSrc[255.16.0.1], ipSrcPrefixLen[32],
        mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0],
        routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, override[GT_TRUE],
        defragmentationEnable[GT_FALSE].
    Expected: GT_OK.
    1.4. Call with lpmDBId[0], vrId[0], ipGroup[255.15.0.1], ipGroupPrefixLen[32],
        ipSrc[255.16.0.1], ipSrcPrefixLen[32],.
    Expected: GT_OK.
    1.5. Call with lpmDBId[0], vrId[0], ipGroup[255.15.0.1], ipGroupPrefixLen[32],
        ipSrc[255.16.0.1], ipSrcPrefixLen[32] (already deleted).
    Expected: NOT GT_OK.
    1.6. Call with lpmDBId [0 or 1], out of range vrId[10] and other valid parameters.
    Expected: NOT GT_OK.
    1.7. Call with out of range ipGroupPrefixLen [129] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.8. Call with out of range ipSrcPrefixLen [129] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.9. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.10. Call cpssDxChIpLpmIpv6McEntriesFlush with with lpmDBId [1], vrId[0] to invalidate entries.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                     lpmDBId          = 0;
    GT_U32                     vrId             = 0;
    GT_IPV6ADDR                ipGroup;
    GT_U32                     ipGroupPrefixLen = 0;
    GT_IPV6ADDR                ipSrc;
    GT_U32                     ipSrcPrefixLen   = 0;
    CPSS_DXCH_IP_LTT_ENTRY_STC mcRouteLttEntry;
    GT_BOOL                    override         = GT_FALSE;
    GT_BOOL                    defragEnable     = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv6McEntryAdd with lpmDBId[1],
                 vrId[0], ipGroup[255.15.0.1], ipGroupPrefixLen[32], ipSrc[255.16.0.1],
                 ipSrcPrefixLen[32],
                 mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                 numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
                 sipSaCheckMismatchEnable[GT_TRUE],
                 ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]},
                 override[GT_TRUE], defragmentationEnable[GT_FALSE].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 255;
        ipGroup.arIP[1] = 15;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 1;

        ipGroupPrefixLen = 32;

        ipSrc.arIP[0] = 255;
        ipSrc.arIP[1] = 15;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 32;

        prvUtfSetDefaultIpLttEntry(&mcRouteLttEntry);

        override     = GT_TRUE;
        defragEnable = GT_FALSE;

        st = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                         ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChIpLpmIpv6McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                         lpmDBId, vrId, override, defragEnable);
        }
        else
        {
               UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChIpLpmIpv6McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                         lpmDBId, vrId, override, defragEnable);
        }

        /*
            1.4. Call with lpmDBId[0], vrId[0], ipGroup[255.15.0.1], ipGroupPrefixLen[32],
                    ipSrc[255.16.0.1], ipSrcPrefixLen[32].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 255;
        ipGroup.arIP[1] = 15;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 1;

        ipGroupPrefixLen = 32;

        ipSrc.arIP[0] = 255;
        ipSrc.arIP[1] = 15;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 32;

        st = cpssDxChIpLpmIpv6McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen,
                                                            ipSrc, ipSrcPrefixLen);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        /*
            1.5. Call with lpmDBId[0],
                                        vrId[0],
                                        ipGroup[255.15.0.1],
                                        ipGroupPrefixLen[32],
                                        ipSrc[255.16.0.1],
                                        ipSrcPrefixLen[32] (already deleted).
            Expected: NOT GT_OK.
        */
        st = cpssDxChIpLpmIpv6McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen,
                                                            ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /*
            1.6. Call with lpmDBId [0],
                           out of range vrId[10]
                           and other valid parameters.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv6McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen,
                                                            ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /*
            1.6. Call with lpmDBId [1],
                               out of range vrId[10]
                               and other valid parameters.
                Expected: NOT GT_OK.
        */
        lpmDBId = 1;
        vrId    = 10;

        st = cpssDxChIpLpmIpv6McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen,
                                                            ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.7. Call with out of range ipGroupPrefixLen [129] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        ipGroupPrefixLen = 129;

        st = cpssDxChIpLpmIpv6McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen,
                                                            ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipGroupPrefixLen = %d",
                                                    lpmDBId, ipGroupPrefixLen);

        ipGroupPrefixLen = 128;

        /*
            1.8. Call with out of range ipSrcPrefixLen [129] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        ipSrcPrefixLen = 129;

        st = cpssDxChIpLpmIpv6McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen,
                                                            ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipSrcPrefixLen = %d",
                                                          lpmDBId, ipSrcPrefixLen);

        ipSrcPrefixLen = 128;

        /*
            1.9. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.4.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv6McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen,
                                                            ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        /*
            1.10. Call cpssDxChIpLpmIpv6McEntriesFlush with with lpmDBId [1],
                  vrId[0] to invalidate entries.
            Expected: GT_OK.
        */
        lpmDBId = 1;

        st = cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv6McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv6McEntriesFlush)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0], defUcNextHopInfoPtr{
        pktCmd = CPSS_PACKET_CMD_FORWARD_E, mirror{cpuCode = CPSS_NET_CONTROL_DEST_MAC_TRAP_E,
        mirrorToRxAnalyzerPort = GT_FALSE}, matchCounter { enableMatchCount = GT_FALSE,
        matchCounterIndex = 0 }, qos { egressPolicy=GT_FALSE, modifyDscp=GT_FALSE,
        modifyUp=GT_FALSE , qos [ ingress[profileIndex=0, profileAssignIndex=GT_FALSE,
        profilePrecedence=GT_FALSE] ] }, redirect { CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E,
        data[routerLttIndex=0] }, policer { policerEnable=GT_FALSE, policerId=0 }, vlan {
        egressTaggedModify=GT_FALSE, modifyVlan=CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
        nestedVlan=GT_FALSE, vlanId=100, precedence=CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E
        }, ipUcRoute { doIpUcRoute=GT_FALSE, 0, GT_FALSE, GT_FALSE, GT_FALSE } },
        defMcRouteLttEntryPtr [NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK.
    1.3. Call with lpmDBId [0], vrId[0].
    Expected: GT_OK.
    1.4. Call with not valid LPM DB id lpmDBId [10] and other parameters from 3.
    Expected: NOT GT_OK.
    1.5. Call with lpmDBId [0 or 1], out of range vrId[10] and other valid parameters.
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                   lpmDBId = 0;
    GT_U32                   vrId    = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call with lpmDBId [0],
                           vrId[0].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        st = cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        /*
            1.4. Call with not valid LPM DB id lpmDBId [10]
                           and other parameters from 3.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.5. Call with lpmDBId [0],
                           out of range vrId[10] and other valid parameters.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        /*
            1.5. Call with lpmDBId [1],
                               out of range vrId[10]
                               and other valid parameters.
                Expected: NOT GT_OK.
        */
        lpmDBId = 1;
        vrId    = 10;

        st = cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv6McEntryGetNext
(
    IN    GT_U32                      lpmDBId,
    IN    GT_U32                      vrId,
    INOUT GT_IPV6ADDR                 *ipGroupPtr,
    INOUT GT_U32                      *ipGroupPrefixLenPtr,
    INOUT GT_IPV6ADDR                 *ipSrcPtr,
    INOUT GT_U32                      *ipSrcPrefixLenPtr,
    OUT   CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT   GT_U32                      *gRowIndexPtr,
    OUT   GT_U32                      *sRowIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv6McEntryGetNext)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0],
        defUcNextHopInfoPtr->ipLttEntry{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
        numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
        sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, defMcRouteLttEntryPtr {
        routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0], routeEntryBaseIndex[0],
        ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
    Expected: GT_OK .
    1.3. Call cpssDxChIpLpmIpv4McEntryAdd for 10 times with lpmDBId[0], vrId[0],
        ipGroup[255.15.0.1..10], ipGroupPrefixLen[32], ipSrc[255.16.0.1..10], ipSrcPrefixLen[32],
        mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0],
        routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, override[GT_TRUE],
        defragmentationEnable[GT_FALSE].
    Expected: GT_OK.
    1.4. Call with lpmDBId[0], vrId[0], ipGroupPtr[255.15.0.1..9], ipGroupPrefixLenPtr [32],
        ipSrcPtr[255.16.0.1..9], ipSrcPrefixLenPtr[32] and non-null mcRouteLttEntryPtr.
    Expected: GT_OK and ipGroupPtr[255.15.0.2..10], ipGroupPrefixLenPtr[32],
        ipSrcPtr[255.16.0.2..10], ipSrcPrefixLenPtr[32] and the same mcRouteLttEntryPtr.
    1.5. Call with lpmDBId [0 or 1], out of range vrId[10] and other valid parameters.
    Expected: NOT GT_OK.
    1.6. Call with out of range ipGroupPrefixLenPtr [129] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.7. Call with out of range ipSrcPrefixLenPtr [129] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.8. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.9. Call with lpmDBId [0], vrId[0], ipGroupPtr[NULL] and other valid parameters.
    Expected: GT_BAD_PTR.
    1.10. Call with lpmDBId [0], vrId[0], ipGroupPrefixLenPtr [NULL] and other valid parameters.
    Expected: GT_BAD_PTR.
    1.11. Call with lpmDBId [0], vrId[0], ipSrcPtr [NULL] and other valid parameters.
    Expected: GT_BAD_PTR.
    1.12. Call with lpmDBId [0], vrId[0], ipSrcPrefixLenPtr [NULL] and other valid parameters.
    Expected: GT_BAD_PTR.
    1.13. Call with lpmDBId [0], vrId[0], mcRouteLttEntryPtr [NULL] and other valid parameters.
    Expected: GT_BAD_PTR.
    1.14. Call cpssDxChIpLpmIpv6McEntriesFlush with with lpmDBId [0/1], vrId[0] to invalidate entries.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                     lpmDBId          = 0;
    GT_U32                     vrId             = 0;
    GT_IPV6ADDR                ipGroup;
    GT_U32                     ipGroupPrefixLen = 0;
    GT_IPV6ADDR                ipSrc;
    GT_U32                     ipSrcPrefixLen   = 0;
    CPSS_DXCH_IP_LTT_ENTRY_STC mcRouteLttEntry;
    CPSS_DXCH_IP_LTT_ENTRY_STC mcRouteLttEntryGet;
    GT_BOOL                    override         = GT_FALSE;
    GT_BOOL                    defragEnable     = GT_FALSE;
    GT_U8                      iter             = 0;
    GT_U32                     gRowIndex;
    GT_U32                     sRowIndex;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv4McEntryAdd for 10 times with lpmDBId[0],
                 vrId[0], ipGroup[255.15.0.1..10], ipGroupPrefixLen[32], ipSrc[255.16.0.1..10],
                 ipSrcPrefixLen[32],
                 mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                 numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
                 sipSaCheckMismatchEnable[GT_TRUE],
                 ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]},
                 override[GT_TRUE], defragmentationEnable[GT_FALSE].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 255;
        ipGroup.arIP[1] = 15;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 1;

        ipGroupPrefixLen = 32;

        ipSrc.arIP[0] = 255;
        ipSrc.arIP[1] = 16;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 32;

        prvUtfSetDefaultIpLttEntry(&mcRouteLttEntry);

        override     = GT_TRUE;
        defragEnable = GT_FALSE;

        /* add ipGroup[255.15.0.1], ipSrc[255.16.0.1] */
        st = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                             ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChIpLpmIpv6McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                         lpmDBId, vrId, override, defragEnable);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChIpLpmIpv6McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                         lpmDBId, vrId, override, defragEnable);
        }

        for (iter = 0; iter < 10; iter++)
        {
            ipGroup.arIP[3] = (GT_U8) (ipGroup.arIP[3] + 1);

            ipSrc.arIP[3] = (GT_U8) (ipSrc.arIP[3] + 1);

            st = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                             ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
            if (prvUtfIsPbrModeUsed())
            {
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "cpssDxChIpLpmIpv6McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                             lpmDBId, vrId, override, defragEnable);
            }
            else
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                        "cpssDxChIpLpmIpv6McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                             lpmDBId, vrId, override, defragEnable);
            }
        }

        /*
            1.4. Call with lpmDBId[0], vrId[0], ipGroupPtr[255.15.0.1], ipGroupPrefixLenPtr
                [32], ipSrcPtr[255.16.0.1], ipSrcPrefixLenPtr[32] and non-null mcRouteLttEntryPtr.
            Expected: GT_OK and ipGroupPtr[255.15.0.2..10], ipGroupPrefixLenPtr[32],
                ipSrcPtr[255.16.0.2..10], ipSrcPrefixLenPtr[32] and the same mcRouteLttEntryPtr.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 255;
        ipGroup.arIP[1] = 15;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 1;

        ipGroupPrefixLen = 32;

        ipSrc.arIP[0] = 255;
        ipSrc.arIP[1] = 16;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 32;

        for (iter = 0; iter < 10; iter++)
        {
           st = cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                   &ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex, &sRowIndex);
           if (prvUtfIsPbrModeUsed())
           {
               UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
           }
           else
           {
               UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

               /* Verifying values */
               prvUtfCompareIpLttEntries(lpmDBId, vrId, &mcRouteLttEntryGet, &mcRouteLttEntry);

               UTF_VERIFY_EQUAL2_STRING_MAC(iter + 2, ipGroup.arIP[3],
                           "get another ipGroup.arIP[3] than was set: %d, %d", lpmDBId, vrId);
               UTF_VERIFY_EQUAL2_STRING_MAC(iter + 2, ipSrc.arIP[3],
                           "get another ipSrc.arIP[3] than was set: %d, %d", lpmDBId, vrId);
           }
        }

        /*
            1.5. Call with lpmDBId [0],
                           out of range vrId[10] and other valid parameters.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                         &ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex, &sRowIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);


        /*
            1.5. Call with lpmDBId [1],
                               out of range vrId[10]
                               and other valid parameters.
            Expected: NOT GT_OK.
        */
        lpmDBId = 1;
        vrId    = 10;

        st = cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                   &ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex, &sRowIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.6. Call with out of range ipGroupPrefixLenPtr [129] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        ipGroupPrefixLen = 129;

        st = cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                   &ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex, &sRowIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipGroupPrefixLenPtr = %d",
                                                        lpmDBId, ipGroupPrefixLen);

        ipGroupPrefixLen = 128;

        /*
            1.7. Call with out of range ipSrcPrefixLenPtr [129] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        ipSrcPrefixLen = 129;

        st = cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                             &ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex, &sRowIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipSrcPrefixLenPtr = %d", lpmDBId, ipSrcPrefixLen);

        ipSrcPrefixLen = 128;

        /*
            1.8. Call with not valid LPM DB id lpmDBId [10]
                           and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                              &ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex, &sRowIndex);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.9. Call with lpmDBId [0],
                           vrId[0],
                           ipGroupPtr[NULL]
                           and other valid parameters.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId, vrId, NULL, &ipGroupPrefixLen, &ipSrc,
                                   &ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex, &sRowIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ipGroupPtr = NULL", lpmDBId, vrId);

        /*
            1.10. Call with lpmDBId [0],
                           vrId[0],
                           ipGroupPrefixLenPtr [NULL]
                           and other valid parameters.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, NULL, &ipSrc,
                                   &ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex, &sRowIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ipGroupPrefixLenPtr = NULL", lpmDBId, vrId);

        /*
            1.11. Call with lpmDBId [0], vrId[0], ipSrcPtr [NULL] and other valid parameters.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, NULL,
                                             &ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex, &sRowIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ipSrcPtr = NULL", lpmDBId, vrId);

        /*
            1.12. Call with lpmDBId [0],
                            vrId[0],
                            ipSrcPrefixLenPtr [NULL]
                            and other valid parameters.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                             NULL, &mcRouteLttEntryGet, &gRowIndex, &sRowIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ipSrcPrefixLenPtr = NULL", lpmDBId, vrId);

        /*
            1.13. Call with lpmDBId [0],
                            vrId[0],
                            mcRouteLttEntryPtr [NULL]
                            and other valid parameters.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipGroupPrefixLen, &ipSrc,
                                             &ipSrcPrefixLen, NULL, &gRowIndex, &sRowIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, mcRouteLttEntryPtr = NULL",
                                                                            lpmDBId, vrId);

        /*
            1.14. Call cpssDxChIpLpmIpv6McEntriesFlush with with lpmDBId [0/1], vrId[0]
                    to invalidate entries.
            Expected: GT_OK.
        */
        lpmDBId = 0;

        st = cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        lpmDBId = 1;

        st = cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmIpv6McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPV6ADDR                 ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPV6ADDR                 ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                      *gRowIndexPtr,
    OUT GT_U32                      *sRowIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmIpv6McEntrySearch)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmVirtualRouterAdd with lpmDBId [0], vrId[0],
        defUcNextHopInfoPtr->ipLttEntry{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
        numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
        sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, defMcRouteLttEntryPtr {
        routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0], routeEntryBaseIndex[0],
        ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}
    Expected: GT_OK .
    1.4. Call cpssDxChIpLpmIpv4McEntryAdd for 10 times with lpmDBId[0], vrId[0],
        ipGroup[255.15.0.1..10], ipGroupPrefixLen[32], ipSrc[255.16.0.1..10], ipSrcPrefixLen[32],
        mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E], numOfPaths[0],
        routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE], sipSaCheckMismatchEnable[GT_TRUE],
        ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]}, override[GT_TRUE],
        defragmentationEnable[GT_FALSE].
    Expected: GT_OK.
    1.5. Call with lpmDBId[0], vrId[0], ipGroup[255.15.0.1], ipGroupPrefixLen [32],
    ipSrc[255.16.0.1], ipSrcPrefixLen[32] and non-null mcRouteLttEntry.
    Expected: GT_OK and the same mcRouteLttEntryPtr.
    1.6. Call with lpmDBId [0 or 1], out of range vrId[10] and other valid parameters.
    Expected: NOT GT_OK.
    1.7. Call with out of range ipGroupPrefixLen [129] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.8. Call with out of range ipSrcPrefixLen [129] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.9. Call with not valid LPM DB id lpmDBId [10] and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.10. Call with lpmDBId [0], vrId[0], mcRouteLttEntryPtr [NULL] and other valid parameters.
    Expected: GT_BAD_PTR.
    1.11. Call cpssDxChIpLpmIpv6McEntriesFlush with with lpmDBId [0/1], vrId[0] to invalidate entries.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_U32                     lpmDBId          = 0;
    GT_U32                     vrId             = 0;
    GT_IPV6ADDR                ipGroup;
    GT_U32                     ipGroupPrefixLen = 0;
    GT_IPV6ADDR                ipSrc;
    GT_U32                     ipSrcPrefixLen   = 0;
    CPSS_DXCH_IP_LTT_ENTRY_STC mcRouteLttEntry;
    CPSS_DXCH_IP_LTT_ENTRY_STC mcRouteLttEntryGet;
    GT_BOOL                    override         = GT_FALSE;
    GT_BOOL                    defragEnable     = GT_FALSE;
    GT_U8                      iter             = 0;
    GT_U32                     gRowIndex;
    GT_U32                     sRowIndex;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. - 1.2.
        */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd: %d", dev);

        /*
            1.3. Call cpssDxChIpLpmIpv4McEntryAdd for 10 times with lpmDBId[0],
                 vrId[0], ipGroup[255.15.0.1..10], ipGroupPrefixLen[32], ipSrc[255.16.0.1..10],
                 ipSrcPrefixLen[32],
                 mcRouteLttEntryPtr{routeType[CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                 numOfPaths[0], routeEntryBaseIndex[0], ucRPFCheckEnable[GT_TRUE],
                 sipSaCheckMismatchEnable[GT_TRUE],
                 ipv6MCGroupScopeLevel[CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E]},
                 override[GT_TRUE], defragmentationEnable[GT_FALSE].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 255;
        ipGroup.arIP[1] = 15;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 1;

        ipGroupPrefixLen = 32;

        ipSrc.arIP[0] = 255;
        ipSrc.arIP[1] = 16;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 32;

        prvUtfSetDefaultIpLttEntry(&mcRouteLttEntry);

        override     = GT_TRUE;
        defragEnable = GT_FALSE;

        /* add ipGroup[255.15.0.1],ipSrc[255.16.0.1] */
        st = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                             ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChIpLpmIpv6McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                             lpmDBId, vrId, override, defragEnable);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChIpLpmIpv6McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                             lpmDBId, vrId, override, defragEnable);
        }

        for (iter = 0; iter < 10; iter++)
        {
            ipGroup.arIP[3] = (GT_U8) (ipGroup.arIP[3] + 1);

            ipSrc.arIP[3] = (GT_U8) (ipSrc.arIP[3] + 1);

            st = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                             ipSrcPrefixLen, &mcRouteLttEntry, override, defragEnable);
            if (prvUtfIsPbrModeUsed())
            {
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                        "cpssDxChIpLpmIpv6McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                             lpmDBId, vrId, override, defragEnable);
            }
            else
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChIpLpmIpv6McEntryAdd: %d, %d, override = %d, defragmentationEnable = %d",
                                         lpmDBId, vrId, override, defragEnable);
            }
        }

        /*
            1.5. Call with lpmDBId[0], vrId[0], ipGroup[255.15.0.1], ipGroupPrefixLen [32],
                ipSrc[255.16.0.1], ipSrcPrefixLen[32] and non-null mcRouteLttEntry.
            Expected: GT_OK and the same mcRouteLttEntryPtr.
        */
        lpmDBId = 0;
        vrId    = 0;

        ipGroup.arIP[0] = 255;
        ipGroup.arIP[1] = 15;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 1;

        ipGroupPrefixLen = 32;

        ipSrc.arIP[0] = 255;
        ipSrc.arIP[1] = 16;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 32;

        for (iter = 0; iter < 10; iter++)
        {
            st = cpssDxChIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                    ipSrcPrefixLen, &mcRouteLttEntryGet, &gRowIndex, &sRowIndex);
            if (prvUtfIsPbrModeUsed())
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

                prvUtfCompareIpLttEntries(lpmDBId,vrId, &mcRouteLttEntryGet, &mcRouteLttEntry);
            }
        }

        /*
            1.6. Call with lpmDBId [0], out of range vrId[10] and other valid parameters.
            Expected: NOT GT_OK.
        */
        vrId = 10;

        st = cpssDxChIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                        ipSrcPrefixLen, &mcRouteLttEntry, &gRowIndex, &sRowIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.7. Call with lpmDBId [1], out of range vrId[10] and other valid parameters.
            Expected: NOT GT_OK.
        */
        lpmDBId = 1;
        vrId    = 10;

        st = cpssDxChIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                            ipSrcPrefixLen, &mcRouteLttEntry, &gRowIndex, &sRowIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

        vrId = 0;

        /*
            1.8. Call with out of range ipGroupPrefixLen [129] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        ipGroupPrefixLen = 129;

        st = cpssDxChIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                            ipSrcPrefixLen, &mcRouteLttEntry, &gRowIndex, &sRowIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipGroupPrefixLen = %d", lpmDBId, ipGroupPrefixLen);

        ipGroupPrefixLen = 128;

        /*
            1.9. Call with out of range ipSrcPrefixLen [129] and other parameters from 1.5.
            Expected: NOT GT_OK.
        */
        ipSrcPrefixLen = 129;

        st = cpssDxChIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                            ipSrcPrefixLen, &mcRouteLttEntry, &gRowIndex, &sRowIndex);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipSrcPrefixLen = %d", lpmDBId, ipSrcPrefixLen);

        ipSrcPrefixLen = 128;

        /*
            1.10. Call with lpmDBId [0], vrId[0], mcRouteLttEntryPtr [NULL] and other valid parameters.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, ipGroupPrefixLen, ipSrc,
                                            ipSrcPrefixLen, NULL, &gRowIndex, &sRowIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, mcRouteLttEntryPtr = NULL", lpmDBId, vrId);

        /*
            1.11. Call cpssDxChIpLpmIpv6McEntriesFlush with with lpmDBId [0/1], vrId[0] to invalidate entries.
            Expected: GT_OK.
        */

        lpmDBId = 0;

        st = cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

        lpmDBId = 1;

        st = cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
        if (prvUtfIsPbrModeUsed())
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmDBMemSizeGet
(
    IN    GT_U32 lpmDBId,
    INOUT GT_U32 *lpmDbSizePtr,
    IN    GT_U32 numOfEntriesInStep,
    INOUT GT_U32 *iterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmDBMemSizeGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
         indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
         tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
         numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call with lpmDBId [0], lpmDbSizePtr[0], numOfEntriesInStep[1], iterPtr[0].
    Expected: GT_OK.
    1.3. Call with lpmDbSizePtr[NULL] and other parameters from 1.2.
    Expected: GT_BAD_PTR.
    1.4. Call with iterPtr [NULL] and other parameters from 1.2.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st        = GT_OK;

    GT_U32    lpmDBId   = 0;
    GT_U32    lpmDbSize = 0;
    GT_U32    numInStep = 0;
    GT_U32    iter      = 0;
    GT_U8     dev;


    /*
        1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0 / 1],
                         protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
                         indexesRangePtr{ firstIndex [0/ 200],
                                          lastIndex [100/ 1000]},
                          partitionEnable[GT_TRUE],
                          tcamLpmManagerCapcityCfgPtr{numOfIpv4Prefixes [10],
                                                      numOfIpv4McSourcePrefixes [5],
                                                      numOfIpv6Prefixes [10]} to create LPM DB.
        Expected: GT_OK.
    */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfCreateDefaultLpmDB(dev);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");
    }
    /*
        1.2. Call with lpmDBId [0], lpmDbSizePtr[0], numOfEntriesInStep[1], iterPtr[0].
        Expected: GT_OK.
    */
    lpmDBId   = 0;
    lpmDbSize = 0;
    numInStep = 1;
    iter      = 0;

    st = cpssDxChIpLpmDBMemSizeGet(lpmDBId, &lpmDbSize, numInStep, &iter);
    UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, lpmDBId, lpmDbSize, numInStep, iter);

    /*
        1.3. Call with lpmDbSizePtr[NULL]
                       and other parameters from 1.2.
        Expected: GT_BAD_PTR.
    */
    st = cpssDxChIpLpmDBMemSizeGet(lpmDBId, NULL, numInStep, &iter);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lpmDbSizePtr = NULL", lpmDBId);

    /*
        1.4. Call with iterPtr [NULL]
                       and other parameters from 1.2.
        Expected: GT_BAD_PTR.
    */
    st = cpssDxChIpLpmDBMemSizeGet(lpmDBId, &lpmDbSize, numInStep, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, iterPtr = NULL", lpmDBId);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmDBExport
(
    IN    GT_U32 lpmDBId,
    OUT   GT_U32 *lpmDbPtr,
    IN    GT_U32 numOfEntriesInStep,
    INOUT GT_U32 *iterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmDBExport)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
        indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
        tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
        numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call with lpmDBId [0], numOfEntriesInStep[1], iterPtr[0].
    Expected: GT_OK.
    1.3. Call cpssDxChIpLpmDBImport with the same lpmDbPtr to import exported DB.
    Expected: GT_OK.
    1.4. Call with lpmDbPtr [NULL] and other parameters from 1.2.
    Expected: GT_BAD_PTR.
    1.5. Call with iterPtr [NULL] and other parameters from 1.2.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st        = GT_OK;

    GT_U32                   lpmDBId   = 0;
    /* lpmDb type should be enough to store PRV_CPSS_DXCH_IP_VR_INFO_STC
     * otherwise buffer overflow causes to segfault
     */
    struct { char x[1024]; } lpmDb;

    GT_U32                   numInStep = 0;
    GT_U32                   iter      = 0;
    GT_U8                    dev;


    /*
        1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0 / 1],
                         protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
                         indexesRangePtr{ firstIndex [0/ 200],
                                          lastIndex [100/ 1000]},
                          partitionEnable[GT_TRUE],
                          tcamLpmManagerCapcityCfgPtr{numOfIpv4Prefixes [10],
                                                      numOfIpv4McSourcePrefixes [5],
                                                      numOfIpv6Prefixes [10]} to create LPM DB.
        Expected: GT_OK.
    */

        /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfCreateDefaultLpmDB(dev);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

        /*
            1.2. Call with lpmDBId [0],
                           numOfEntriesInStep[1],
                           iterPtr[0].
            Expected: GT_OK.
        */
        lpmDBId   = 0;
        numInStep = 1;
        iter      = 0;

        st = cpssDxChIpLpmDBExport(lpmDBId, (GT_U32*) &lpmDb, numInStep, &iter);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfEntriesInStep = %d", lpmDBId, numInStep);

        /*
            1.3. Call cpssDxChIpLpmDBImport with the same lpmDbPtr to import exported DB.
            Expected: GT_OK.
        */
        st = cpssDxChIpLpmDBImport(lpmDBId, (GT_U32*) &lpmDb, numInStep, &iter);
        /*UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChIpLpmDBImport: %d, lpmDbPtr = %d", lpmDBId, lpmDb);*/

        /*
            1.4. Call with lpmDbPtr [NULL] and other parameters from 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmDBExport(lpmDBId, NULL, numInStep, &iter);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lpmDbPtr = NULL", lpmDBId);

        /*
            1.5. Call with iterPtr [NULL] and other parameters from 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmDBExport(lpmDBId, (GT_U32*) &lpmDb, numInStep, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, iterPtr = NULL", lpmDBId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmDBImport
(
    IN    GT_U32 lpmDBId,
    IN    GT_U32 *lpmDbPtr,
    IN    GT_U32 numOfEntriesInStep,
    INOUT GT_U32 *iterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmDBImport)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
            indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
            tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10], numOfIpv4McSourcePrefixes [5],
            numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call with lpmDBId [0], non-null lpmDbPtr , numOfEntriesInStep[1], iterPtr[0].
    Expected: GT_OK.
    1.3. Call with lpmDbPtr [NULL] and other parameters from 1.2.
    Expected: GT_BAD_PTR.
    1.4. Call with iterPtr [NULL] and other parameters from 1.2.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                st        = GT_OK;

    GT_U32                   lpmDBId   = 0;
        /* lpmDb type should be enough to store PRV_CPSS_DXCH_IP_VR_INFO_STC
     * otherwise buffer overflow causes to segfault
     */
    struct { char x[1024]; } lpmDb;
    GT_U32                   numInStep = 0;
    GT_U32                   iter      = 0;
    GT_U8                    dev       = 0;

    /*
        1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0 / 1],
                 protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
                 indexesRangePtr{ firstIndex [0/ 200],
                                  lastIndex [100/ 1000]},
                  partitionEnable[GT_TRUE],
                  tcamLpmManagerCapcityCfgPtr{numOfIpv4Prefixes [10],
                                              numOfIpv4McSourcePrefixes [5],
                                              numOfIpv6Prefixes [10]} to create LPM DB.
        Expected: GT_OK.
    */
        /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfCreateDefaultLpmDB(dev);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");


        /*
            1.2. Call with lpmDBId [0],
                           non-null lpmDbPtr ,
                           numOfEntriesInStep[1], iterPtr[0].
            Expected: GT_OK.
        */
        lpmDBId   = 0;
        numInStep = 1;
        iter      = 0;

        st = cpssDxChIpLpmDBExport(lpmDBId, (GT_U32*) &lpmDb, numInStep, &iter);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfEntriesInStep = %d", lpmDBId, numInStep);

        /* need do reset before importing back the data */
        st = cpssDxChIpLpmDBImport(lpmDBId,  (GT_U32*) &lpmDb, numInStep, &iter);
        /*UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfEntriesInStep = %d", lpmDBId, numInStep);*/
        /*
            1.3. Call with lpmDbPtr [NULL] and other parameters from 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmDBImport(lpmDBId, NULL, numInStep, &iter);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lpmDbPtr = NULL", lpmDBId);

        /*
            1.4. Call with iterPtr [NULL] and other parameters from 1.2.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmDBImport(lpmDBId,  (GT_U32*) &lpmDb, numInStep, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, iterPtr = NULL", lpmDBId);

    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmDBCheetah2VrSupportCreate
(
    IN GT_U32                                                   lpmDBId,
    IN CPSS_IP_PROTOCOL_STACK_ENT                               protocolStack,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *pclTcamCfgPtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *routerTcamCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmDBCheetah2VrSupportCreate)
{
/*
    ITERATE_DEVICES (DxCh2)
    1.1. Call with lpmDBId [40/50/6],
                    protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                   CPSS_IP_PROTOCOL_IPV6_E /
                                   CPSS_IP_PROTOCOL_IPV4V6_E],
                pclTcamCfgPtr.tcamRange.firstIndex = 0;
                pclTcamCfgPtr.tcamRange.lastIndex = 500;
                pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

                routerTcamCfgPtr.tcamRange.firstIndex = 600;
                routerTcamCfgPtr.tcamRange.lastIndex = 900;
                routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;
    Expected: GT_OK.
    1.2. Call with lpmDBId [7],
                    protocolStack [CPSS_IP_PROTOCOL_IPV4_E]
                pclTcamCfgPtr.tcamRange.firstIndex = 0;
                pclTcamCfgPtr.tcamRange.lastIndex = 300;
                pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 700;
                pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 700;

                routerTcamCfgPtr.tcamRange.firstIndex = 1600;
                routerTcamCfgPtr.tcamRange.lastIndex = 2600;
                routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 700;
                routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 700;
    Expected: GT_OK.
    1.3. Call with lpmDBId [8],
                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E]
                pclTcamCfgPtr.tcamRange.firstIndex = 0;
                pclTcamCfgPtr.tcamRange.lastIndex = 5000;
                pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;
                routerTcamCfgPtr.tcamRange.firstIndex = 600;
                routerTcamCfgPtr.tcamRange.lastIndex = 900;
                routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;
    Expected: GT_BAD_PARAM
                routerTcamCfgPtr->tcamRange.lastIndex -
                routerTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
    1.4. Call with lpmDBId [9],
                protocolStack [CPSS_IP_PROTOCOL_IPV4_E]
                pclTcamCfgPtr.tcamRange.firstIndex = 1000;
                pclTcamCfgPtr.tcamRange.lastIndex = 900;
                pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;
                routerTcamCfgPtr.tcamRange.firstIndex = 600;
                routerTcamCfgPtr.tcamRange.lastIndex = 900;
                routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;
    Expected: GT_BAD_PARAM
                pclTcamCfgPtr.tcamRange.firstIndex bigger than
                pclTcamCfgPtr.tcamRange.lastIndex
    1.5. Call with lpmDBId [10],
                protocolStack [CPSS_IP_PROTOCOL_IPV4_E]
                pclTcamCfgPtr.tcamRange.firstIndex = 100;
                pclTcamCfgPtr.tcamRange.lastIndex = 900;
                pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

                routerTcamCfgPtr.tcamRange.firstIndex = 600;
                routerTcamCfgPtr.tcamRange.lastIndex = 500;
                routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;
    Expected: GT_BAD_PARAM
                routerTcamCfgPtr.tcamRange.firstIndex more than
                routerTcamCfgPtr.tcamRange.lastIndex
    1.6. Call with lpmDBId [11],
                protocolStack [CPSS_IP_PROTOCOL_IPV4_E]
                pclTcamCfgPtr.tcamRange.firstIndex = 0;
                pclTcamCfgPtr.tcamRange.lastIndex = 500;
                pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

                routerTcamCfgPtr.tcamRange.firstIndex = 600;
                routerTcamCfgPtr.tcamRange.lastIndex = 2000;
                routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;
    Expected: GT_BAD_PARAM
                pclTcamCfgPtr->tcamRange.lastIndex -
                pclTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
    1.7. Call with lpmDBId [12],
                protocolStack [CPSS_IP_PROTOCOL_IPV4_E]
                pclTcamCfgPtr.tcamRange.firstIndex = 0;
                pclTcamCfgPtr.tcamRange.lastIndex = 500;
                pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 200;
                pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 200;

                routerTcamCfgPtr.tcamRange.firstIndex = 600;
                routerTcamCfgPtr.tcamRange.lastIndex = 700;
                routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 200;
                routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 200;
    Expected: GT_BAD_PARAM
                (routerTcamCfgPtr->tcamRange.lastIndex -
                routerTcamCfgPtr->tcamRange.firstIndex + 1) * 5
                must be larger or equal to
                pclTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
                pclTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes +
                routerTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
                routerTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes
    1.8. Call with wrong enum values protocolStack ,
          and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with pclTcamCfgPtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.10. Call with routerTcamCfgPtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_U8      dev       = 0;
    GT_STATUS  st        = GT_OK;
    GT_U32     lpmDBId   = 0;

    CPSS_IP_PROTOCOL_STACK_ENT                               protocolStack = 0;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC pclTcamCfgPtr;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC routerTcamCfgPtr;

    cpssOsBzero((GT_VOID*) &pclTcamCfgPtr, sizeof(pclTcamCfgPtr));
    cpssOsBzero((GT_VOID*) &routerTcamCfgPtr, sizeof(routerTcamCfgPtr));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with lpmDBId [40/50/6],
                            protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                           CPSS_IP_PROTOCOL_IPV6_E /
                                           CPSS_IP_PROTOCOL_IPV4V6_E],
                        pclTcamCfgPtr.tcamRange.firstIndex = 0;
                        pclTcamCfgPtr.tcamRange.lastIndex = 500;
                        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

                        routerTcamCfgPtr.tcamRange.firstIndex = 600;
                        routerTcamCfgPtr.tcamRange.lastIndex = 900;
                        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;
            Expected: GT_OK.
        */

        pclTcamCfgPtr.tcamRange.firstIndex = 0;
        pclTcamCfgPtr.tcamRange.lastIndex = 500;
        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

        routerTcamCfgPtr.tcamRange.firstIndex = 600;
        routerTcamCfgPtr.tcamRange.lastIndex = 900;
        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

        /* call with lpmDBId [40] and protocolStack[CPSS_IP_PROTOCOL_IPV4_E] */
        lpmDBId  = 40;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDBId, protocolStack,
                                                    &pclTcamCfgPtr, &routerTcamCfgPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d lpmDbPtr", lpmDBId);


        /* call with lpmDBId [50] and protocolStack[CPSS_IP_PROTOCOL_IPV6_E] */
        lpmDBId  = 50;
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDBId, protocolStack,
                                                    &pclTcamCfgPtr, &routerTcamCfgPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d lpmDbPtr", lpmDBId);

        /* call with lpmDBId [6] and protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E] */
        lpmDBId  = 6;
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDBId, protocolStack,
                                                    &pclTcamCfgPtr, &routerTcamCfgPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.2. Call with lpmDBId [7],
                            protocolStack [CPSS_IP_PROTOCOL_IPV4_E]
                        pclTcamCfgPtr.tcamRange.firstIndex = 0;
                        pclTcamCfgPtr.tcamRange.lastIndex = 300;
                        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 700;
                        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 700;

                        routerTcamCfgPtr.tcamRange.firstIndex = 1600;
                        routerTcamCfgPtr.tcamRange.lastIndex = 2600;
                        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 700;
                        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 700;
            Expected: GT_OK.
        */
        lpmDBId  = 7;

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        pclTcamCfgPtr.tcamRange.firstIndex = 0;
        pclTcamCfgPtr.tcamRange.lastIndex = 300;
        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 700;
        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 700;

        routerTcamCfgPtr.tcamRange.firstIndex = 1600;
        routerTcamCfgPtr.tcamRange.lastIndex = 2600;
        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 700;
        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 700;

        st = cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDBId, protocolStack,
                                                    &pclTcamCfgPtr, &routerTcamCfgPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.3. Call with lpmDBId [8],
                           protocolStack [CPSS_IP_PROTOCOL_IPV4_E]
                        pclTcamCfgPtr.tcamRange.firstIndex = 0;
                        pclTcamCfgPtr.tcamRange.lastIndex = 5000;
                        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;
                        routerTcamCfgPtr.tcamRange.firstIndex = 600;
                        routerTcamCfgPtr.tcamRange.lastIndex = 900;
                        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;
            Expected: GT_BAD_PARAM
                        routerTcamCfgPtr->tcamRange.lastIndex -
                        routerTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
        */
        lpmDBId  = 8;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        pclTcamCfgPtr.tcamRange.firstIndex = 0;
        pclTcamCfgPtr.tcamRange.lastIndex = 5000;
        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

        routerTcamCfgPtr.tcamRange.firstIndex = 600;
        routerTcamCfgPtr.tcamRange.lastIndex = 900;
        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

        st = cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDBId, protocolStack,
                                                    &pclTcamCfgPtr, &routerTcamCfgPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PARAM, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.4. Call with lpmDBId [9],
                        protocolStack [CPSS_IP_PROTOCOL_IPV4_E]
                        pclTcamCfgPtr.tcamRange.firstIndex = 1000;
                        pclTcamCfgPtr.tcamRange.lastIndex = 900;
                        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;
                        routerTcamCfgPtr.tcamRange.firstIndex = 600;
                        routerTcamCfgPtr.tcamRange.lastIndex = 900;
                        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;
            Expected: GT_BAD_PARAM
                        pclTcamCfgPtr.tcamRange.firstIndex bigger than
                        pclTcamCfgPtr.tcamRange.lastIndex
        */
        lpmDBId  = 9;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        pclTcamCfgPtr.tcamRange.firstIndex = 1000;
        pclTcamCfgPtr.tcamRange.lastIndex = 900;
        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

        routerTcamCfgPtr.tcamRange.firstIndex = 600;
        routerTcamCfgPtr.tcamRange.lastIndex = 900;
        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

        st = cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDBId, protocolStack,
                                                    &pclTcamCfgPtr, &routerTcamCfgPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PARAM, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.5. Call with lpmDBId [10],
                        protocolStack [CPSS_IP_PROTOCOL_IPV4_E]
                        pclTcamCfgPtr.tcamRange.firstIndex = 100;
                        pclTcamCfgPtr.tcamRange.lastIndex = 900;
                        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

                        routerTcamCfgPtr.tcamRange.firstIndex = 600;
                        routerTcamCfgPtr.tcamRange.lastIndex = 500;
                        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;
            Expected: GT_BAD_PARAM
                        routerTcamCfgPtr.tcamRange.firstIndex more than
                        routerTcamCfgPtr.tcamRange.lastIndex
        */
        lpmDBId  = 10;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        pclTcamCfgPtr.tcamRange.firstIndex = 100;
        pclTcamCfgPtr.tcamRange.lastIndex = 900;
        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

        routerTcamCfgPtr.tcamRange.firstIndex = 600;
        routerTcamCfgPtr.tcamRange.lastIndex = 500;
        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

        st = cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDBId, protocolStack,
                                                    &pclTcamCfgPtr, &routerTcamCfgPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PARAM, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.6. Call with lpmDBId [11],
                        protocolStack [CPSS_IP_PROTOCOL_IPV4_E]
                        pclTcamCfgPtr.tcamRange.firstIndex = 0;
                        pclTcamCfgPtr.tcamRange.lastIndex = 500;
                        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

                        routerTcamCfgPtr.tcamRange.firstIndex = 600;
                        routerTcamCfgPtr.tcamRange.lastIndex = 2000;
                        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
                        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;
            Expected: GT_BAD_PARAM
                        pclTcamCfgPtr->tcamRange.lastIndex -
                        pclTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
        */
        lpmDBId  = 11;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        pclTcamCfgPtr.tcamRange.firstIndex = 0;
        pclTcamCfgPtr.tcamRange.lastIndex = 500;
        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

        routerTcamCfgPtr.tcamRange.firstIndex = 600;
        routerTcamCfgPtr.tcamRange.lastIndex = 2000;
        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 10;
        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 10;

        st = cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDBId, protocolStack,
                                                    &pclTcamCfgPtr, &routerTcamCfgPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PARAM, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.7. Call with lpmDBId [12],
                        protocolStack [CPSS_IP_PROTOCOL_IPV4_E]
                        pclTcamCfgPtr.tcamRange.firstIndex = 0;
                        pclTcamCfgPtr.tcamRange.lastIndex = 500;
                        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 200;
                        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 200;

                        routerTcamCfgPtr.tcamRange.firstIndex = 600;
                        routerTcamCfgPtr.tcamRange.lastIndex = 700;
                        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 200;
                        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 200;
            Expected: GT_BAD_PARAM
                        (routerTcamCfgPtr->tcamRange.lastIndex -
                        routerTcamCfgPtr->tcamRange.firstIndex + 1) * 5
                        must be larger or equal to
                        pclTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
                        pclTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes +
                        routerTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
                        routerTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes
        */
        lpmDBId  = 12;
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        pclTcamCfgPtr.tcamRange.firstIndex = 0;
        pclTcamCfgPtr.tcamRange.lastIndex = 500;
        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 200;
        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 200;

        routerTcamCfgPtr.tcamRange.firstIndex = 600;
        routerTcamCfgPtr.tcamRange.lastIndex = 700;
        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 200;
        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 200;

        st = cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDBId, protocolStack,
                                                    &pclTcamCfgPtr, &routerTcamCfgPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PARAM, st, "%d lpmDbPtr", lpmDBId);


        /*
            1.8. Call with wrong enum values protocolStack, and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        lpmDBId  = 13;

        pclTcamCfgPtr.tcamRange.firstIndex = 0;
        pclTcamCfgPtr.tcamRange.lastIndex = 300;
        pclTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 700;
        pclTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 700;

        routerTcamCfgPtr.tcamRange.firstIndex = 1600;
        routerTcamCfgPtr.tcamRange.lastIndex = 2600;
        routerTcamCfgPtr.prefixesCfg.numOfIpv4Prefixes = 700;
        routerTcamCfgPtr.prefixesCfg.numOfIpv6Prefixes = 700;

        UTF_ENUMS_CHECK_MAC(cpssDxChIpLpmDBCheetah2VrSupportCreate
                            (lpmDBId, protocolStack, &pclTcamCfgPtr, &routerTcamCfgPtr),
                            protocolStack);

        /*
            1.9. Call with pclTcamCfgPtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        lpmDBId  = 14;

        st = cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDBId, protocolStack,
                                                    NULL, &routerTcamCfgPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d", lpmDBId);

        /*
            1.10. Call with routerTcamCfgPtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        lpmDBId  = 15;

        st = cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDBId, protocolStack,
                                                    &pclTcamCfgPtr, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d", lpmDBId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
(
    IN  GT_U32  lpmDBId,
    IN  GT_U32  pclIdArrayLen,
    IN  GT_U32  pclIdArray[]
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [2],
                 protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
                 indexesRangePtr{ firstIndex [200],
                                  lastIndex [1000]},
                 partitionEnable[GT_TRUE],
                 tcamLpmManagerCapcityCfgPtr{numOfIpv4Prefixes [10],
                                             numOfIpv4McSourcePrefixes [5],
                                             numOfIpv6Prefixes [10]}
                 to create LPM DB.
    Expected: GT_OK.
    1.2. Call with multicast lpmDb configuration
         (lpmDB without virtual routers).
    Expected: GT_OK.
    1.3. Call cpssDxChIpLpmDBCreate with lpmDBId [0 / 1],
         protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
         indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
         tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10],
         numOfIpv4McSourcePrefixes [5], numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.4. Call with lpmDb with Vr. The LPM DB must be empty when
         using this API (can't contain any prefixes or virtual routers).
    Expected: GT_BAD_STATE.
    1.5. Call with lpmDb [4] (not exists).
    Expected: NOT GT_OK.
*/
    GT_U8      dev       = 0;
    GT_STATUS  st        = GT_OK;

    GT_U32     lpmDBId   = 0;
    GT_U32     pclIdArray[1];

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [2],
                         protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
                         indexesRangePtr{ firstIndex [200],
                                          lastIndex [1000]},
                         partitionEnable[GT_TRUE],
                         tcamLpmManagerCapcityCfgPtr{numOfIpv4Prefixes [10],
                                                     numOfIpv4McSourcePrefixes [5],
                                                     numOfIpv6Prefixes [10]}
                         to create LPM DB.
            Expected: GT_OK.
        */

        /* create lpmDBId [2] for policy based routing*/
        st = prvUtfCreateMcLpmDB(dev);

        switch (PRV_CPSS_PP_MAC(dev)->devFamily)
        {
            case CPSS_PP_FAMILY_CHEETAH_E:
            case CPSS_PP_FAMILY_DXCH_XCAT_E:
            case CPSS_PP_FAMILY_DXCH_LION_E:
            case CPSS_PP_FAMILY_DXCH_XCAT2_E:
                UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateMcLpmDB");
                break;
            default:
                UTF_VERIFY_EQUAL0_STRING_MAC(GT_NOT_IMPLEMENTED, st, "prvUtfCreateMcLpmDB");
        }

        if (GT_OK == st)
        {
            /*
                1.2. Call with multicast lpmDb configuration
                     (lpmDB without virtual routers).
                Expected: GT_OK.
            */
            lpmDBId  = 2;
            pclIdArray[0] = 0;

            st = cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet(lpmDBId,1,pclIdArray);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d", lpmDBId);
        }

        /*
            1.3. Call cpssDxChIpLpmDBCreate with lpmDBId [0 / 1],
                 protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
                 indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
                 tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10],
                 numOfIpv4McSourcePrefixes [5], numOfIpv6Prefixes [10]} to create LPM DB.
            Expected: GT_OK.
        */

        /* create lpmDBId [0] for policy based routing*/
        st = prvUtfCreateDefaultLpmDB(dev);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

        if (GT_OK == st)
        {
            /*
                1.4. Call with lpmDb with Vr. The LPM DB must be empty when
                     using this API (can't contain any prefixes or virtual routers).
                Expected: GT_BAD_STATE.
            */

            lpmDBId  = 0;
            pclIdArray[0] = 0;

            st = cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet(lpmDBId,1,pclIdArray);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_STATE, st, "%d", lpmDBId);

        }

        /*
            1.5. Call with lpmDb [4] (not exists).
            Expected: NOT GT_OK.
        */
        lpmDBId  = 6;
        pclIdArray[0] = 0;

        st = cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet(lpmDBId,1,pclIdArray);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d", lpmDBId);

    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet
(
    IN      GT_U32                          lpmDBId,
    OUT     GT_BOOL                         *defaultMcUsedPtr,
    OUT     CPSS_IP_PROTOCOL_STACK_ENT      *protocolStackPtr,
    OUT     GT_U32                          defaultIpv4RuleIndexArray[],
    INOUT   GT_U32                          *defaultIpv4RuleIndexArrayLenPtr,
    OUT     GT_U32                          defaultIpv6RuleIndexArray[],
    INOUT   GT_U32                          *defaultIpv6RuleIndexArrayLenPtr,
    OUT     GT_U32                          pclIdArray[],
    INOUT   GT_U32                          *pclIdArrayLenPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet)
{
/*
    ITERATE_DEVICES (DxChx and above)
    1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0],
         protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
         indexesRangePtr{ firstIndex [0], lastIndex [1024]}, partitionEnable[GT_TRUE],
         tcamLpmManagerCapcityCfgPtr{ numOfIpv4Prefixes [10],
         numOfIpv4McSourcePrefixes [5], numOfIpv6Prefixes [10]} to create LPM DB.
    Expected: GT_OK.
    1.2. Call with lpmDBId [0].
    Expected: GT_OK.
    1.3. Call with defaultMcUsedPtr [NULL].
    Expected: GT_BAD_PTR.
    1.4. Call with defaultIpv4RuleIndexArray [NULL].
    Expected: GT_BAD_PTR.
    1.5. Call with defaultIpv6RuleIndexArray [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_U8      dev       = 0;
    GT_STATUS  st        = GT_OK;
    GT_U32     lpmDBId   = 0;

    GT_BOOL                     defaultMcUsedPtr;
    CPSS_IP_PROTOCOL_STACK_ENT  protocolStackPtr;
    GT_U32                      defaultIpv4RuleIndexArray[1];
    GT_U32                      defaultIpv4RuleIndexArrayLen = 1;
    GT_U32                      defaultIpv6RuleIndexArray[1];
    GT_U32                      defaultIpv6RuleIndexArrayLen = 1;
    GT_U32                      pclIdArray[1];
    GT_U32                      pclIdArrayLen = 1;

    /*
        1.1. Call cpssDxChIpLpmDBCreate with lpmDBId [0 / 1],
                     protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E],
                     indexesRangePtr{ firstIndex [0/ 200],
                                      lastIndex [100/ 1000]},
                     partitionEnable[GT_TRUE],
                     tcamLpmManagerCapcityCfgPtr{numOfIpv4Prefixes [10],
                                                 numOfIpv4McSourcePrefixes [5],
                                                 numOfIpv6Prefixes [10]}
                     to create LPM DB.
        Expected: GT_OK.
    */
        /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfCreateDefaultLpmDB(dev);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");
    }

    /*
        1.2. Call with default  lpmDb configuration [0].
        Expected: GT_OK.
    */

    lpmDBId  = 0;

    st = cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet(lpmDBId, &defaultMcUsedPtr,&protocolStackPtr,
            defaultIpv4RuleIndexArray, &defaultIpv4RuleIndexArrayLen,
            defaultIpv6RuleIndexArray, &defaultIpv6RuleIndexArrayLen,
            pclIdArray, &pclIdArrayLen);

    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d lpmDbPtr", lpmDBId);

    /*
        1.3. Call with defaultMcUsedPtr [NULL].
        Expected: GT_BAD_PTR.
    */
    st = cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet(lpmDBId, NULL,&protocolStackPtr,
            defaultIpv4RuleIndexArray, &defaultIpv4RuleIndexArrayLen,
            defaultIpv6RuleIndexArray, &defaultIpv6RuleIndexArrayLen,
            pclIdArray, &pclIdArrayLen);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d", lpmDBId);

    /*
        1.4. Call with defaultIpv4RuleIndexArray [NULL].
        Expected: GT_BAD_PTR.
    */
    st = cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet(lpmDBId, &defaultMcUsedPtr,&protocolStackPtr,
            NULL, &defaultIpv4RuleIndexArrayLen,
            defaultIpv6RuleIndexArray, &defaultIpv6RuleIndexArrayLen,
            pclIdArray, &pclIdArrayLen);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d", lpmDBId);

    /*
        1.5. Call with defaultIpv6RuleIndexArray [NULL].
        Expected: GT_BAD_PTR.
    */
    st = cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet(lpmDBId, &defaultMcUsedPtr,&protocolStackPtr,
            defaultIpv4RuleIndexArray, &defaultIpv4RuleIndexArrayLen,
            NULL, &defaultIpv6RuleIndexArrayLen,
            pclIdArray, &pclIdArrayLen);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d", lpmDBId);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmTcamLinesReserve
(
    IN  GT_U32                                          lpmDBId,
    IN  GT_U32                                          linesToReserveArray[],
    IN  GT_U32                                          numberOfLinesToReserve,
    IN  CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    allocateReserveLinesAs
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmTcamLinesReserve)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
        1.1. Call with empty lpmDb [5],  numberOfLinesToReserve = 10;
        Expected: GT_OK.
        1.2. Call with empty lpmDb [5],  numberOfLinesToReserve = 10;
                allocateReserveLinesAs.numOfIpv4Prefixes = 20;
                allocateReserveLinesAs.numOfIpv4McSourcePrefixes = 40;
                allocateReserveLinesAs.numOfIpv6Prefixes = 40;
        Expected: GT_OK.
        1.3. Call with empty lpmDb [5],  numberOfLinesToReserve = 10;
                allocateReserveLinesAs.numOfIpv4Prefixes = 10;
                allocateReserveLinesAs.numOfIpv4McSourcePrefixes = 0;
                allocateReserveLinesAs.numOfIpv6Prefixes = 0;
        Expected: GT_OK.
        1.4. Call with non-existent lpmDb [20].
        Expected: GT_NOT_FOUND.
        1.5. Call with non-empty lpmDb [0].
        Expected: GT_BAD_STATE.
        1.6. Call with empty lpmDb [5],  numberOfLinesToReserve = 10;
                allocateReserveLinesAs.numOfIpv4Prefixes = 20;
                allocateReserveLinesAs.numOfIpv4McSourcePrefixes = 40;
                allocateReserveLinesAs.numOfIpv6Prefixes = 40;
        Expected: GT_BAD_PARAM.
                  - all lines to reserve must reside in the TCAM
                  section allocated to the LPM DB
        1.7. Call with empty lpmDb [5],  numberOfLinesToReserve = 10;
                allocateReserveLinesAs.numOfIpv4Prefixes = 10;
                allocateReserveLinesAs.numOfIpv4McSourcePrefixes = 0;
                allocateReserveLinesAs.numOfIpv6Prefixes = 0;
        Expected: NOT GT_OK.
                             - all lines to reserve must be unique
*/
    GT_U8      dev    = 0;
    GT_STATUS  st     = GT_OK;
    GT_U8      i      = 0;

    GT_U32     lpmDBId = 0;
    GT_U32                                          linesToReserveArray[10];
    GT_U32                                          numberOfLinesToReserve = 10;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    allocateReserveLinesAs;

    cpssOsBzero((GT_VOID*) &allocateReserveLinesAs,
                sizeof(CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfCreateDefaultLpmDB(dev);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

        /*
            1.1. Call with empty lpmDb [5],  numberOfLinesToReserve = 10;
            Expected: GT_OK.
        */
        lpmDBId   = 5;
        numberOfLinesToReserve = 10;

        for (i = 0; i < numberOfLinesToReserve; i++)
        {
            linesToReserveArray[i] = 1001 + i;
        }

        st = cpssDxChIpLpmTcamLinesReserve(lpmDBId, linesToReserveArray,
                        numberOfLinesToReserve, allocateReserveLinesAs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.2. Call with empty lpmDb [5],  numberOfLinesToReserve = 1;
                    allocateReserveLinesAs.numOfIpv4Prefixes = 20;
                    allocateReserveLinesAs.numOfIpv4McSourcePrefixes = 40;
                    allocateReserveLinesAs.numOfIpv6Prefixes = 40;
            Expected: GT_OK.
        */
        lpmDBId   = 5;
        numberOfLinesToReserve = 1;
        allocateReserveLinesAs.numOfIpv4Prefixes = 20;
        allocateReserveLinesAs.numOfIpv4McSourcePrefixes = 40;
        allocateReserveLinesAs.numOfIpv6Prefixes = 40;


        for (i = 0; i < numberOfLinesToReserve; i++)
        {
            linesToReserveArray[i] = 1012 + i;
        }

        st = cpssDxChIpLpmTcamLinesReserve(lpmDBId, linesToReserveArray,
                        numberOfLinesToReserve, allocateReserveLinesAs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.3. Call with empty lpmDb [5],  numberOfLinesToReserve = 1;
                    allocateReserveLinesAs.numOfIpv4Prefixes = 10;
                    allocateReserveLinesAs.numOfIpv4McSourcePrefixes = 0;
                    allocateReserveLinesAs.numOfIpv6Prefixes = 0;
            Expected: GT_OK.
        */
        lpmDBId   = 5;
        allocateReserveLinesAs.numOfIpv4Prefixes = 10;
        allocateReserveLinesAs.numOfIpv4McSourcePrefixes = 0;
        allocateReserveLinesAs.numOfIpv6Prefixes = 0;

        for (i = 0; i < numberOfLinesToReserve; i++)
        {
            linesToReserveArray[i] = 1014 + i;
        }

        st = cpssDxChIpLpmTcamLinesReserve(lpmDBId, linesToReserveArray,
                        numberOfLinesToReserve, allocateReserveLinesAs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.4. Call with non-existent lpmDb [20].
            Expected: GT_NOT_FOUND.
        */
        lpmDBId   = 20;

        st = cpssDxChIpLpmTcamLinesReserve(lpmDBId, linesToReserveArray,
                        numberOfLinesToReserve, allocateReserveLinesAs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_FOUND, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.5. Call with non-empty lpmDb [0].
            Expected: GT_BAD_STATE.
        */
        lpmDBId   = 0;

        st = cpssDxChIpLpmTcamLinesReserve(lpmDBId, linesToReserveArray,
                        numberOfLinesToReserve, allocateReserveLinesAs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_STATE, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.6. Call with empty lpmDb [5],  numberOfLinesToReserve = 10;
                    allocateReserveLinesAs.numOfIpv4Prefixes = 20;
                    allocateReserveLinesAs.numOfIpv4McSourcePrefixes = 40;
                    allocateReserveLinesAs.numOfIpv6Prefixes = 40;
            Expected: GT_BAD_PARAM.
                      - all lines to reserve must reside in the TCAM
                      section allocated to the LPM DB
        */
        lpmDBId   = 5;
        allocateReserveLinesAs.numOfIpv4Prefixes = 20;
        allocateReserveLinesAs.numOfIpv4McSourcePrefixes = 40;
        allocateReserveLinesAs.numOfIpv6Prefixes = 40;


        for (i = 0; i < numberOfLinesToReserve; i++)
        {
            linesToReserveArray[i] = 120 + i;
        }

        st = cpssDxChIpLpmTcamLinesReserve(lpmDBId, linesToReserveArray,
                        numberOfLinesToReserve, allocateReserveLinesAs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PARAM, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.7. Call with empty lpmDb [5],  numberOfLinesToReserve = 10;
                    allocateReserveLinesAs.numOfIpv4Prefixes = 10;
                    allocateReserveLinesAs.numOfIpv4McSourcePrefixes = 0;
                    allocateReserveLinesAs.numOfIpv6Prefixes = 0;
            Expected: NOT GT_OK.
                                 - all lines to reserve must be unique
        */
        lpmDBId   = 5;
        allocateReserveLinesAs.numOfIpv4Prefixes = 10;
        allocateReserveLinesAs.numOfIpv4McSourcePrefixes = 0;
        allocateReserveLinesAs.numOfIpv6Prefixes = 0;

        for (i = 0; i < numberOfLinesToReserve; i++)
        {
            linesToReserveArray[i] = 1360 + i;
        }

        st = cpssDxChIpLpmTcamLinesReserve(lpmDBId, linesToReserveArray,
                        numberOfLinesToReserve, allocateReserveLinesAs);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d lpmDbPtr", lpmDBId);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmVirtualRouterGet
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    OUT GT_BOOL                                 *supportUcPtr,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *defUcNextHopInfoPtr,
    OUT GT_BOOL                                 *supportMcPtr,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC              *defMcRouteLttEntryPtr,
    OUT CPSS_IP_PROTOCOL_STACK_ENT              *protocolStackPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmVirtualRouterGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with lpmDb [0] and vrId.
    Expected: GT_OK.
    1.2. Call with lpmDBId [3] and vrId [1] (support only Uc routing).
    Expected: GT_OK and supportUc [GT_TRUE], supportMc [GT_FALSE].
    1.3. Call with lpmDBId [1] and vrId [1] (support only Mc routing).
    Expected: GT_OK and supportUc [GT_FALSE], supportMc [GT_TRUE].
    1.4. Call with lpmDb [0] and non-existent vrId [2].
    Expected: GT_NOT_FOUND.
    1.5. Call with empty lpmDb [1].
    Expected: GT_NOT_FOUND.
    1.6. Call with wrong &vrConfigInfoGet [NULL].
    Expected: GT_BAD_PTR.

*/
    GT_U8      dev       = 0;
    GT_STATUS  st        = GT_OK;

    GT_U32     lpmDBId   = 0;
    GT_U32     vrId      = 0;

    CPSS_DXCH_IP_LPM_VR_CONFIG_STC         vrConfigInfoGet = {0};

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfCreateLpmDBAndVirtualRouterAdd(dev);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

        /*
            1.1. Call with lpmDBId [0] and vrId [0].
            Expected: GT_OK.
        */
        lpmDBId = 0;
        vrId    = 0;

        st = cpssDxChIpLpmVirtualRouterGet(lpmDBId, vrId, &vrConfigInfoGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.2. Call with lpmDBId [3] and vrId [1] (support only Uc routing).
            Expected: GT_OK and supportUc [GT_TRUE], supportMc [GT_FALSE].
        */
        lpmDBId = prvUtfIsPbrModeUsed() ? 1 : 3;
        vrId    = prvUtfIsPbrModeUsed() ? 0 : 1;

        st = cpssDxChIpLpmVirtualRouterGet(lpmDBId, vrId, &vrConfigInfoGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d lpmDbPtr", lpmDBId);
        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(vrConfigInfoGet.supportIpv4Uc, GT_TRUE, "lpmDbPtr = %d, vrId = %d",
                                                                            lpmDBId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vrConfigInfoGet.supportIpv6Uc, GT_TRUE, "lpmDbPtr = %d, vrId = %d",
                                                                            lpmDBId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vrConfigInfoGet.supportIpv4Mc, GT_FALSE, "lpmDbPtr = %d, vrId = %d",
                                                                            lpmDBId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vrConfigInfoGet.supportIpv6Mc, GT_FALSE, "lpmDbPtr = %d, vrId = %d",
                                                                            lpmDBId, vrId);
        }

        /*
            1.3. Call with lpmDBId [1] and vrId [1] (support only Mc routing).
            Expected: GT_OK and supportUc [GT_FALSE], supportMc [GT_TRUE].
        */
        /* MC is not supported in PBR mode */
        if (GT_FALSE == prvUtfIsPbrModeUsed())
        {
            lpmDBId = 4;
            vrId    = 1;

            st = cpssDxChIpLpmVirtualRouterGet(lpmDBId, vrId, &vrConfigInfoGet);

            if (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_CHEETAH2_E)
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d lpmDbPtr", lpmDBId);
                if (GT_OK == st)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(vrConfigInfoGet.supportIpv4Uc, GT_FALSE, "lpmDbPtr = %d, vrId = %d",
                                                                                    lpmDBId, vrId);
                    UTF_VERIFY_EQUAL2_STRING_MAC(vrConfigInfoGet.supportIpv6Uc, GT_FALSE, "lpmDbPtr = %d, vrId = %d",
                                                                                    lpmDBId, vrId);
                    UTF_VERIFY_EQUAL2_STRING_MAC(vrConfigInfoGet.supportIpv4Mc, GT_TRUE, "lpmDbPtr = %d, vrId = %d",
                                                                                    lpmDBId, vrId);
                    UTF_VERIFY_EQUAL2_STRING_MAC(vrConfigInfoGet.supportIpv6Mc, GT_TRUE, "lpmDbPtr = %d, vrId = %d",
                                                                                    lpmDBId, vrId);
                }
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d lpmDbPtr", lpmDBId);
            }
        }

        /*
            1.4. Call with lpmDb [0] and non-existent vrId [2].
            Expected: GT_NOT_FOUND.
        */
        lpmDBId = 0;
        vrId    = 2;

        st = cpssDxChIpLpmVirtualRouterGet(lpmDBId, vrId, &vrConfigInfoGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_FOUND, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.5. Call with empty lpmDb [1].
            Expected: GT_NOT_FOUND.
        */
        lpmDBId = 1;

        st = cpssDxChIpLpmVirtualRouterGet(lpmDBId, vrId, &vrConfigInfoGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_FOUND, st, "%d lpmDbPtr", lpmDBId);

        /*
            1.6. Call with wrong vrConfigInfoGet [NULL].
            Expected: GT_BAD_PTR.
        */
        lpmDBId = 0;

        st = cpssDxChIpLpmVirtualRouterGet(lpmDBId, vrId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d lpmDbPtr", lpmDBId);

    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmVirtualRouterSharedAdd
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT *defUcNextHopInfoPtr,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC             *defMcRouteLttEntryPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT             protocolStack
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmVirtualRouterSharedAdd)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call cpssDxChIpLpmDBCreate to create default LPM DB configuration.
         See cpssDxChIpLpmDBCreate description.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmDBDevListAdd with lpmDBId [0 / 1],
         devList [0, 1, 2, 3, 4] / [5, 6, 7],
         numOfDevs[5] / [3] to adds devices to an existing LPM DB.
    Expected: GT_OK.
    1.3. Call with lpmDBId [1], vrId[0], defUcNextHopInfo filled with
         default configuration, defMcRouteLttEntry filled with default
         configuration (support both: unicast and multicast) and
         protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_NOT_SUPPORTED (multicast not supported currently).
    1.4. Call with lpmDBId [3], vrId[0], defUcNextHopInfo filled with
         default configuration, defMcRouteLttEntry [NULL]
         (support only unicast) and protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_OK .
        1.5. Call cpssDxChIpLpmVirtualRouterGet with the same params.
        Expected: GT_OK and the same values.
    1.6. Call with lpmDBId [4], vrId[0], defUcNextHopInfo [NULL]
         defMcRouteLttEntry filled with default configuration
         (support only multicast) and
         protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: GT_NOT_SUPPORTED (multicast not supported yet) .
    1.7. Call with lpmDBId [1], out of range vrId [10] and other valid parameters.
    Expected: NOT GT_OK.
    1.8. Call with not valid lpmDBId [10].
    Expected: NOT GT_OK.
    1.9. Call with lpmDBId [0], vrId[0], defUcNextHopInfoPtr[NULL],
          defMcRouteLttEntry[NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
    Expected: NOT GT_OK.
    1.10. Call with correct numOfPaths [5/7] ipv4/ipv6 for unicast and multicast.
    Expected: GT_OK.
    1.11. Call with correct numOfPaths [8/63] ipv4/ipv6 for unicast and multicast.
    Expected: GT_OK for lion and above and not GT_OK for other.
    1.12. Call with wrong numOfPaths [64] (out of range).
    Check ipv4/ipv6 for unicast and multicast.
    Expected: NOT GT_OK
    1.13. Call with wrong dev family.
    Expected: GT_NOT_SUPPORTED (only cheetah3 and above supported).
*/
    GT_STATUS                              st            = GT_OK;
    GT_U8                                  dev, devNum;

    GT_U32                                 lpmDBId       = 0;
    GT_U32                                 vrId          = 0;

    GT_U8                                  devList[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U32                                 numOfDevs = 0;
    GT_BOOL                                isEqual = GT_FALSE;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC         vrConfigInfo;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC         vrConfigInfoGet;

    cpssOsBzero((GT_VOID*) &vrConfigInfo, sizeof(CPSS_DXCH_IP_LPM_VR_CONFIG_STC));
    cpssOsBzero((GT_VOID*) &vrConfigInfoGet, sizeof(CPSS_DXCH_IP_LPM_VR_CONFIG_STC));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /*
            1.1. Call cpssDxChIpLpmDBCreate to create default LPM DB configuration.
                 See cpssDxChIpLpmDBCreate description.
            Expected: GT_OK.
        */
        st = prvUtfCreateDefaultLpmDB(dev);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

        /*
            1.2. Call cpssDxChIpLpmDBDevListAdd with lpmDBId [0 / 1],
                 devList [0, 1, 2, 3, 4] / [5, 6, 7],
                 numOfDevs[5] / [3] to adds devices to an existing LPM DB.
            Expected: GT_OK.
        */
        lpmDBId = 0;

        /* prepare device iterator */
        PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&devNum, UTF_NONE_FAMILY_E);

        /* 1. Go over all active devices. */
        while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
        {
            devList[numOfDevs++] = devNum;
        }

        /* for lpmDBId = 0 dev 0 was already added in the init phase,
           so we will get GT_OK but an actual add is not done */
        st = cpssDxChIpLpmDBDevListAdd(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChIpLpmDBDevListAdd: %d, numOfDevs = %d", lpmDBId, numOfDevs);

        lpmDBId = 1;

        st = cpssDxChIpLpmDBDevListAdd(lpmDBId, devList, numOfDevs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChIpLpmDBDevListAdd: %d, numOfDevs = %d", lpmDBId, numOfDevs);

        /* DxCh3 shadow type and above are supported. PBR shadow types are not supported */
        if ((CPSS_PP_FAMILY_CHEETAH3_E <= PRV_CPSS_PP_MAC(dev)->devFamily) &&
            (GT_FALSE == prvUtfIsPbrModeUsed()))
        {
            /*
                1.3. Call with lpmDBId [1], vrId[0], defUcNextHopInfo filled with
                     default configuration, defMcRouteLttEntry filled with default
                     configuration (support both: unicast and multicast) and
                     protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
                Expected: GT_NOT_SUPPORTED (multicast not supported currently).
            */
            lpmDBId = 1;
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            vrConfigInfo.supportIpv6Mc = GT_TRUE;
            vrConfigInfo.supportIpv4Uc = GT_TRUE;
            vrConfigInfo.supportIpv6Uc = GT_TRUE;
            prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry);
            prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry);
            prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv4McRouteLttEntry);
            prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv6McRouteLttEntry);

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_NOT_SUPPORTED, st,"%d, %d", lpmDBId, vrId);

            /*
                1.4. Call with lpmDBId [3], vrId[0], defUcNextHopInfo filled with
                     default configuration, defMcRouteLttEntry [NULL]
                     (support only unicast) and protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
                Expected: GT_OK .
            */
            vrConfigInfo.supportIpv4Mc = GT_FALSE;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;
            lpmDBId = 3;

            /*remove any virtual routers for prevent GT_BAD_PARAM by prefixes conflict
              when run current LPM suit*/
            for(vrId = 0; vrId < 100; vrId++)
            {
                st = cpssDxChIpLpmVirtualRouterGet(lpmDBId, vrId, &vrConfigInfoGet);
                if (GT_OK == st)
                {
                    st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d", lpmDBId, vrId);
                }
            }

            vrId = 2;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,"%d, %d", lpmDBId, vrId);

            if (GT_OK == st)
            {
                /*
                    1.5. Call cpssDxChIpLpmVirtualRouterGet with the same params.
                    Expected: GT_OK and the same values.
                */
                st = cpssDxChIpLpmVirtualRouterGet(lpmDBId, vrId, &vrConfigInfoGet);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d", lpmDBId, vrId);

                /*check supportIpv4Uc*/
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, vrConfigInfoGet.supportIpv4Uc,
                                    "SupportIpv4Uc don't equal GT_TRUE, %d", vrConfigInfoGet.supportIpv4Uc);

                /*check defIpv4UcNextHopInfoGet.ipLttEntry*/
                isEqual = (0 == cpssOsMemCmp((GT_VOID*)&vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry,
                                             (GT_VOID*)&vrConfigInfoGet.defIpv4UcNextHopInfo.ipLttEntry,
                           sizeof (CPSS_DXCH_IP_LTT_ENTRY_STC))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                        "get another defIpv4UcNextHopInfo than was set: %d", dev);

                /*check supportIpv6Uc*/
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, vrConfigInfoGet.supportIpv6Uc,
                                    "SupportIpv6Uc don't equal GT_TRUE, %d", vrConfigInfoGet.supportIpv6Uc);

                /*check defIpv6UcNextHopInfoGet.ipLttEntry*/
                isEqual = (0 == cpssOsMemCmp((GT_VOID*)&vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry,
                                             (GT_VOID*)&vrConfigInfoGet.defIpv6UcNextHopInfo.ipLttEntry,
                           sizeof (CPSS_DXCH_IP_LTT_ENTRY_STC))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                        "get another defIpv6UcNextHopInfo than was set: %d", dev);


                /*check supportIpv4Mc*/
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, vrConfigInfoGet.supportIpv4Mc,
                                    "SupportIpv4Mc don't equal GT_FALSE, %d", vrConfigInfoGet.supportIpv4Mc);
                /*check supportIpv6Mc*/
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, vrConfigInfoGet.supportIpv6Mc,
                                    "SupportIpv6Mc don't equal GT_FALSE, %d", vrConfigInfoGet.supportIpv6Mc);

            }

            /*
                1.6. Call with lpmDBId [4], vrId[0], defUcNextHopInfo [NULL]
                     defMcRouteLttEntry filled with default configuration
                     (support only multicast) and
                     protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
                Expected: GT_NOT_SUPPORTED (multicast not supported yet) .
            */
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            vrConfigInfo.supportIpv6Mc = GT_TRUE;
            vrConfigInfo.supportIpv4Uc = GT_FALSE;
            vrConfigInfo.supportIpv6Uc = GT_FALSE;

            lpmDBId = 4;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_NOT_SUPPORTED, st,"%d, %d", lpmDBId, vrId);

            vrConfigInfo.supportIpv4Mc = GT_FALSE;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;

            /*
                1.7. Call with lpmDBId [1], out of range vrId [10] and other valid parameters.
                Expected: NOT GT_OK.
            */
            lpmDBId = 1;
            vrId = 10;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

            vrId = 0;

            /*
                1.8. Call with not valid lpmDBId [10].
                Expected: NOT GT_OK.
            */
            lpmDBId = 10;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

            lpmDBId = 0;

            /*
                1.9. Call with lpmDBId [0], vrId[0], defUcNextHopInfoPtr[NULL],
                      defMcRouteLttEntry[NULL], protocolStack[CPSS_IP_PROTOCOL_IPV4V6_E].
                Expected: NOT GT_OK.
            */
            vrConfigInfo.supportIpv4Mc = GT_FALSE;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;
            vrConfigInfo.supportIpv4Uc = GT_FALSE;
            vrConfigInfo.supportIpv6Uc = GT_FALSE;
            lpmDBId = 0;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
             "%d, %d, supportIpv4_Ipv6Uc = GT_FALSE, supportIpv4_Ipv6Mc = GT_FALSE", lpmDBId, vrId);


            /*
                1.10. Call with correct numOfPaths [5/7] ipv4/ipv6 for unicast and multicast.
                Expected: GT_OK.
            */
            lpmDBId = 1;

            /*remove any virtual routers for prevent GT_BAD_PARAM by prefixes conflict
              when run current LPM suit*/
            for(vrId = 0; vrId < 100; vrId++)
            {
                st = cpssDxChIpLpmVirtualRouterGet(lpmDBId, vrId, &vrConfigInfoGet);
                if (GT_OK == st)
                {
                    st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d", lpmDBId, vrId);
                }
            }

            vrConfigInfo.supportIpv4Uc = GT_TRUE;
            vrConfigInfo.supportIpv4Mc = GT_FALSE;
            vrConfigInfo.supportIpv6Uc = GT_TRUE;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;

            /* call with numOfPaths [5] for ipv4 Uc */
            vrConfigInfo.supportIpv4Uc = GT_TRUE;

            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 5;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);

            /* restore configuration */
            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 0;
            if (GT_OK == st)
            {
                st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
            }

            vrConfigInfo.supportIpv4Uc = GT_FALSE;

            /* call with numOfPaths [7] for ipv4 Uc */
            vrConfigInfo.supportIpv4Uc = GT_TRUE;

            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 7;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);

            /* restore configuration */
            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 0;

            if (GT_OK == st)
            {
                st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
            }

            vrConfigInfo.supportIpv4Uc = GT_FALSE;

            /* call with numOfPaths [5] for ipv6 Uc */
            vrConfigInfo.supportIpv6Uc = GT_TRUE;

            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 5;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv6UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths);

            /* restore configuration */
            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 0;

            if (GT_OK == st)
            {
                st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
            }

            vrConfigInfo.supportIpv6Uc = GT_FALSE;

            /* call with numOfPaths [7] for ipv4 Uc */
            vrConfigInfo.supportIpv6Uc = GT_TRUE;

            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 7;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv6UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths);

            /* restore configuration */
            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 0;

            if (GT_OK == st)
            {
                st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
            }

            vrConfigInfo.supportIpv6Uc = GT_FALSE;

            /*
                1.11. Call with correct numOfPaths [8/63] ipv4/ipv6 for unicast and multicast.
                Expected: GT_OK for lion and above and not GT_OK for other.
            */
            /* call with numOfPaths [8] for ipv4 Uc */
            vrConfigInfo.supportIpv4Uc = GT_TRUE;

            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 8;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
            }

            /* restore configuration */
            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 0;
            st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);

            /* GT_NOT_FOUND is possible result for not LION family -  in case virtual
               router was not really added before */
            if(st != GT_NOT_FOUND)
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);

            vrConfigInfo.supportIpv4Uc = GT_FALSE;

            /* call with numOfPaths [63] for ipv4 Uc */
            vrConfigInfo.supportIpv4Uc = GT_TRUE;

            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 63;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
            }

            /* restore configuration */
            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 0;
            st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);

            /* GT_NOT_FOUND is possible result for not LION family -  in case virtual
               router was not really added before */
            if(st != GT_NOT_FOUND)
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
            vrConfigInfo.supportIpv4Uc = GT_FALSE;

            /* call with numOfPaths [8] for ipv6 Uc */
            vrConfigInfo.supportIpv6Uc = GT_TRUE;

            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 8;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
            }

            /* restore configuration */
            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 0;
            st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
            /* GT_NOT_FOUND is possible result for not LION family -  in case virtual
               router was not really added before */
            if(st != GT_NOT_FOUND)
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);

            vrConfigInfo.supportIpv6Uc = GT_FALSE;
            /* call with numOfPaths [63] for ipv4 Uc */
            vrConfigInfo.supportIpv6Uc = GT_TRUE;

            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 63;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            if (CPSS_PP_FAMILY_DXCH_LION_E <= PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                       "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                       lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);
            }

            /* restore configuration */
            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 0;
            st = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
            /* GT_NOT_FOUND is possible result for not LION family -  in case virtual
               router was not really added before */
            if(st != GT_NOT_FOUND)
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",lpmDBId, vrId);
            vrConfigInfo.supportIpv6Uc = GT_FALSE;

            /*
                1.12. Call with wrong numOfPaths [64] (out of range).
                Check ipv4/ipv6 for unicast and multicast.
                Expected: NOT GT_OK
            */
            /* call with numOfPaths [64] for ipv4 Uc */
            vrConfigInfo.supportIpv4Uc = GT_TRUE;

            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 64;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths);

            /* restore configuration */
            vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = 0;
            vrConfigInfo.supportIpv4Uc = GT_FALSE;

            /* call with numOfPaths [64] for ipv6 Uc */
            vrConfigInfo.supportIpv6Uc = GT_TRUE;

            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 64;

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                   "%d, %d, defIpv6UcNextHopInfoPtr->ipLttEntry.numOfPaths = %d",
                   lpmDBId, vrId, vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths);

            /* restore configuration */
            vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = 0;
            vrConfigInfo.supportIpv6Uc = GT_FALSE;

        }
        else
        {
            /*
                1.13. Call with wrong dev family.
                Expected: GT_NOT_SUPPORTED (only cheetah3 and above supported).
            */
            vrConfigInfo.supportIpv4Mc = GT_FALSE;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;
            vrConfigInfo.supportIpv4Uc = GT_TRUE;
            vrConfigInfo.supportIpv6Uc = GT_TRUE;

            lpmDBId = 1;
            prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry);
            prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry);

            st = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfigInfo);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_NOT_SUPPORTED, st,"%d, %d", lpmDBId, vrId);
        }

    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmDBCapacityGet
(
    IN GT_U32                                 lpmDBId,
    OUT GT_BOOL                                         *partitionEnablePtr,
    OUT CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC    *indexesRangePtr,
    OUT CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    *tcamLpmManagerCapcityCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpLpmDBCapacityGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call cpssDxChIpLpmDBCreate to create default LPM DB configuration.
         See cpssDxChIpLpmDBCreate description.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmDBCapacityGet with lpmDBId [1].
    Expected: GT_OK.
    1.3. Call cpssDxChIpLpmDBCapacityGet with lpmDBId [1] and
        wrong partitionEnablePtr [NULL].
    Expected: GT_BAD_PTR.
    1.4. Call cpssDxChIpLpmDBCapacityGet with lpmDBId [1] and
        wrong indexesRangePtr [NULL].
    Expected: GT_BAD_PTR.
    1.5. Call cpssDxChIpLpmDBCapacityGet with lpmDBId [1] and
        wrong tcamLpmManagerCapcityCfgPtr [NULL].
    Expected: GT_BAD_PTR.
    1.6. Call with not valid LPM DB id lpmDBId [10].
    Expected: NOT GT_OK.
*/
    GT_STATUS  st  = GT_OK;
    GT_U8      dev;

    GT_U32     lpmDBId = 0;
    GT_BOOL                                      partitionEnable;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC indexesRange;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC tcamLpmManagerCapcityCfg;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssDxChIpLpmDBCreate to create default LPM DB configuration.
                 See cpssDxChIpLpmDBCreate description.
            Expected: GT_OK.
        */
        st = prvUtfCreateDefaultLpmDB(dev);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

        /*
            1.2. Call cpssDxChIpLpmDBCapacityGet with lpmDBId [0].
            Expected: GT_OK.
        */
        st = cpssDxChIpLpmDBCapacityGet(lpmDBId, &partitionEnable,
                                        &indexesRange, &tcamLpmManagerCapcityCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        /*
            1.3. Call cpssDxChIpLpmDBCapacityGet with lpmDBId [0] and
                wrong partitionEnablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmDBCapacityGet(lpmDBId, NULL,
                                        &indexesRange, &tcamLpmManagerCapcityCfg);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, partitionEnablePtr = NULL", lpmDBId);

        /*
            1.4. Call cpssDxChIpLpmDBCapacityGet with lpmDBId [0] and
                wrong indexesRangePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmDBCapacityGet(lpmDBId, &partitionEnable,
                                        NULL, &tcamLpmManagerCapcityCfg);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, indexesRangePtr = NULL", lpmDBId);

        /*
            1.5. Call cpssDxChIpLpmDBCapacityGet with lpmDBId [0] and
                wrong tcamLpmManagerCapcityCfgPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmDBCapacityGet(lpmDBId, &partitionEnable, &indexesRange, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, tcamLpmManagerCapcityCfgPtr = NULL", lpmDBId);

        /*
            1.6. Call with not valid LPM DB id lpmDBId [10].
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmDBCapacityGet(lpmDBId, &partitionEnable,
                                        &indexesRange, &tcamLpmManagerCapcityCfg);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpLpmDBCapacityUpdate
(
    IN GT_U32                                       lpmDBId,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *tcamLpmManagerCapcityCfgPtr
)
*/
#if 0
UTF_TEST_CASE_MAC(cpssDxChIpLpmDBCapacityUpdate)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call cpssDxChIpLpmDBCreate to create default LPM DB configuration.
         See cpssDxChIpLpmDBCreate description.
    Expected: GT_OK.
    1.2. Call cpssDxChIpLpmDBCapacityUpdate with lpmDBId [1]
        and indexesRange.firstIndex = 200;
            indexesRange.lastIndex = 1000;
            tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = 11;
            tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 22;
            tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = 33;
    Expected: GT_OK for PBR and non-PBR modes (fit in range).
    1.3. Call cpssDxChIpLpmDBCapacityGet with the same params.
    Expected: GT_OK and the same values.
    1.4. Call cpssDxChIpLpmDBCapacityUpdate with lpmDBId [1]
        and indexesRange.firstIndex = 0;
            indexesRange.lastIndex = 10;
            tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = 11;
            tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 22;
            tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = 33;
    Expected: GT_OK for PBR and NOT GT_OK for non-PBR mode (not fit in range).
    1.5. Call cpssDxChIpLpmDBCapacityUpdate with lpmDBId [0]
        and indexesRange.firstIndex = 0;
            indexesRange.lastIndex  = 5;
            tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = 11;
            tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 22;
            tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = 33;
    Expected: NOT GT_OK in PBR and non-PBR modes (sum of prefixes not fit in range).
    1.6. Call cpssDxChIpLpmDBCapacityUpdate with lpmDBId [0]
        and indexesRange.firstIndex = 10;
            indexesRange.lastIndex  = 0;
    Expected: NOT GT_OK (firstIndex should be less than last).
    1.7. Call with not valid lpmDBId [10].
    Expected: NOT GT_OK.
    1.8. Call cpssDxChIpLpmDBCapacityUpdate with lpmDBId [0] and
        wrong indexesRangePtr [NULL].
    Expected: GT_BAD_PTR.
    1.9. Call cpssDxChIpLpmDBCapacityUpdate with lpmDBId [0] and
        wrong tcamLpmManagerCapcityCfgPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS  st  = GT_OK;
    GT_U8      dev;

    GT_U32     lpmDBId = 0;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC indexesRange;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC tcamLpmManagerCapcityCfg;

    GT_BOOL                                      partitionEnableGet;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC indexesRangeGet;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC tcamLpmManagerCapcityCfgGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssDxChIpLpmDBCreate to create default LPM DB configuration.
                 See cpssDxChIpLpmDBCreate description.
            Expected: GT_OK.
        */
        st = prvUtfCreateDefaultLpmDB(dev);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

        /*
            1.2. Call cpssDxChIpLpmDBCapacityUpdate with lpmDBId [1]
                and indexesRange.firstIndex = 0;
                    indexesRange.lastIndex = 1000;
                    tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = 11;
                    tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 22;
                    tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = 33;
            Expected: GT_OK for PBR and non-PBR modes (fit in range).
        */
        lpmDBId = 1;

        indexesRange.firstIndex = 200;
        indexesRange.lastIndex = 1000;

        tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = 11;
        tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 22;
        tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = 33;

        st = cpssDxChIpLpmDBCapacityUpdate(lpmDBId, &indexesRange, &tcamLpmManagerCapcityCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        if(GT_OK == st)
        {
            /*
                1.3. Call cpssDxChIpLpmDBCapacityGet with the same params.
                Expected: GT_OK and the same values.
            */
            st = cpssDxChIpLpmDBCapacityGet(lpmDBId, &partitionEnableGet,
                                            &indexesRangeGet, &tcamLpmManagerCapcityCfgGet);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

            /*check partitionEnable*/
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, partitionEnableGet,
                   "partitionEnable not equal GT_TRUE, %d", partitionEnableGet);

            /*check indexesRange.firstIndex*/
            UTF_VERIFY_EQUAL2_STRING_MAC(indexesRange.firstIndex, indexesRangeGet.firstIndex,
                                          "indexesRange.firstIndex were not updated, %d, %d",
                                         indexesRange.firstIndex, indexesRangeGet.firstIndex);

            /*check indexesRange.lastIndex*/
            UTF_VERIFY_EQUAL2_STRING_MAC(indexesRange.lastIndex, indexesRangeGet.lastIndex,
                                          "indexesRange.lastIndex were not updated, %d, %d",
                                         indexesRange.lastIndex, indexesRangeGet.lastIndex);

            /*check tcamLpmManagerCapcityCfg.numOfIpv4Prefixes*/
            UTF_VERIFY_EQUAL2_STRING_MAC(tcamLpmManagerCapcityCfg.numOfIpv4Prefixes,
                                         tcamLpmManagerCapcityCfgGet.numOfIpv4Prefixes,
                   "tcamLpmManagerCapcityCfg.numOfIpv4Prefixes were not updated, %d, %d",
                                         tcamLpmManagerCapcityCfg.numOfIpv4Prefixes,
                                         tcamLpmManagerCapcityCfgGet.numOfIpv4Prefixes);

            /*check tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes*/
            UTF_VERIFY_EQUAL2_STRING_MAC(tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes,
                                         tcamLpmManagerCapcityCfgGet.numOfIpv4McSourcePrefixes,
                   "tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes were not updated, %d, %d",
                                         tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes,
                                         tcamLpmManagerCapcityCfgGet.numOfIpv4McSourcePrefixes);

            /*check tcamLpmManagerCapcityCfg.numOfIpv6Prefixes*/
            UTF_VERIFY_EQUAL2_STRING_MAC(tcamLpmManagerCapcityCfg.numOfIpv6Prefixes,
                                         tcamLpmManagerCapcityCfgGet.numOfIpv6Prefixes,
                   "tcamLpmManagerCapcityCfg.numOfIpv6Prefixes were not updated, %d, %d",
                                         tcamLpmManagerCapcityCfg.numOfIpv6Prefixes,
                                         tcamLpmManagerCapcityCfgGet.numOfIpv6Prefixes);
        }

        /*
            1.4. Call cpssDxChIpLpmDBCapacityUpdate with lpmDBId [1]
                and indexesRange.firstIndex = 0;
                    indexesRange.lastIndex = 10;
                    tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = 11;
                    tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 22;
                    tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = 33;
            Expected: GT_OK for PBR and NOT GT_OK for non-PBR mode (not fit in range).
        */
        lpmDBId = 1;
        indexesRange.firstIndex = 0;
        indexesRange.lastIndex = 10;
        tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = 11;
        tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 22;
        tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = 33;

        st = cpssDxChIpLpmDBCapacityUpdate(lpmDBId, &indexesRange, &tcamLpmManagerCapcityCfg);

        if( prvUtfIsPbrModeUsed() )
        {
            /* 11 lines * 4 entries >= 11 + 33  */
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);
        }
        else
        {
            /* 11 lines * 4 entries <= 11 + (33 * 4)  */
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);
        }

        /*
            1.5. Call cpssDxChIpLpmDBCapacityUpdate with lpmDBId [0]
                and indexesRange.firstIndex = 0;
                    indexesRange.lastIndex  = 5;
                    tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = 11;
                    tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 22;
                    tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = 33;
            Expected: NOT GT_OK in PBR and non-PBR modes (sum of prefixes not fit in range).
        */
        indexesRange.lastIndex = 5;

        st = cpssDxChIpLpmDBCapacityUpdate(lpmDBId, &indexesRange, &tcamLpmManagerCapcityCfg);

        /* 6 lines * 4 entries < 11 + 33  */
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        indexesRange.lastIndex = 1000;

        /*
            1.6. Call cpssDxChIpLpmDBCapacityUpdate with lpmDBId [0]
                and indexesRange.firstIndex = 10;
                    indexesRange.lastIndex  = 0;
            Expected: NOT GT_OK (firstIndex should be less than last).
        */
        indexesRange.firstIndex = 10;
        indexesRange.lastIndex  = 0;

        st = cpssDxChIpLpmDBCapacityUpdate(lpmDBId, &indexesRange, &tcamLpmManagerCapcityCfg);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        indexesRange.firstIndex = 0;
        indexesRange.lastIndex  = 1000;

        /*
            1.7. Call with not valid lpmDBId [10].
            Expected: NOT GT_OK.
        */
        lpmDBId = 10;

        st = cpssDxChIpLpmDBCapacityUpdate(lpmDBId, &indexesRange, &tcamLpmManagerCapcityCfg);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

        lpmDBId = 0;

        /*
            1.8. Call cpssDxChIpLpmDBCapacityUpdate with lpmDBId [0] and
                wrong indexesRangePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmDBCapacityUpdate(lpmDBId, NULL, &tcamLpmManagerCapcityCfg);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, indexesRangePtr = NULL", lpmDBId);

        /*
            1.9. Call cpssDxChIpLpmDBCapacityUpdate with lpmDBId [0] and
                wrong tcamLpmManagerCapcityCfgPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChIpLpmDBCapacityUpdate(lpmDBId, &indexesRange, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, tcamLpmManagerCapcityCfgPtr = NULL", lpmDBId);
    }
}
#endif

/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChIpLpm suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChIpLpm)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmDBCreate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmDBDevListAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmDBDevsListRemove)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmVirtualRouterAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmVirtualRouterDel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv4UcPrefixAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv4UcPrefixAddBulk)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv4UcPrefixDel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv4UcPrefixDelBulk)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv4UcPrefixesFlush)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv4UcPrefixSearch)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv4UcPrefixGetNext)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv4McEntryAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv4McEntryDel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv4McEntriesFlush)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv4McEntryGetNext)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv4McEntrySearch)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv4McEntryDelSearch)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv6UcPrefixAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv6UcPrefixAddBulk)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv6UcPrefixDel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv6UcPrefixDelBulk)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv6UcPrefixesFlush)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv6UcPrefixSearch)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv6UcPrefixGetNext)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv6McEntryAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv6McEntryDel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv6McEntriesFlush)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv6McEntryGetNext)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmIpv6McEntrySearch)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmDBMemSizeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmDBExport)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmDBImport)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmDBCheetah2VrSupportCreate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmTcamLinesReserve)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmVirtualRouterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmVirtualRouterSharedAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmDBCapacityGet)
/*    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpLpmDBCapacityUpdate)*/
UTF_SUIT_END_TESTS_MAC(cpssDxChIpLpm)


/*******************************************************************************
* prvUtfComparePclIpUcActionEntries
*
* DESCRIPTION:
*       This routine compare PclIpUcActionEntries.
*
* INPUTS:
*       lpmDBId              - The LPM DB id.
*       vrId                 - The virtual router Id.
*       pclActionReceivePtr  - (pointer to) Received value.
*       pclActionExpectedPtr - (pointer to) Expected value.
*
* OUTPUTS:
*
* COMMENTS:
*       None.
*******************************************************************************/
static void prvUtfComparePclIpUcActionEntries
(
    IN GT_U32                      lpmDBId,
    IN GT_U32                      vrId,
    IN CPSS_DXCH_PCL_ACTION_STC*   pclActionReceivePtr,
    IN CPSS_DXCH_PCL_ACTION_STC*   pclActionExpectedPtr
)
{
    if ((NULL == pclActionReceivePtr) || (NULL == pclActionExpectedPtr))
    {
        return;
    }

    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->pktCmd, pclActionReceivePtr->pktCmd,
     "get another pclActionExpectedPtr->pktCmd than was set: %d, %d", lpmDBId, vrId);

    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->mirror.cpuCode,
                                 pclActionReceivePtr->mirror.cpuCode,
     "get another pclActionExpectedPtr->mirror.cpuCode than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->mirror.mirrorToRxAnalyzerPort,
                                 pclActionReceivePtr->mirror.mirrorToRxAnalyzerPort,
     "get another pclActionExpectedPtr->mirror.mirrorToRxAnalyzerPort than was set: %d, %d", lpmDBId, vrId);

    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->matchCounter.enableMatchCount,
                                 pclActionReceivePtr->matchCounter.enableMatchCount,
     "get another pclActionExpectedPtr->matchCounter.enableMatchCount than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->matchCounter.matchCounterIndex,
                                 pclActionReceivePtr->matchCounter.matchCounterIndex,
     "get another pclActionExpectedPtr->matchCounter.matchCounterIndex than was set: %d, %d", lpmDBId, vrId);

    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->egressPolicy,
                                 pclActionReceivePtr->egressPolicy,
     "get another pclActionExpectedPtr->egressPolicy than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->qos.modifyDscp,
                                 pclActionReceivePtr->qos.modifyDscp,
     "get another pclActionExpectedPtr->qos.modifyDscp than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->qos.modifyUp,
                                 pclActionReceivePtr->qos.modifyUp,
     "get another pclActionExpectedPtr->qos.modifyUp than was set: %d, %d", lpmDBId, vrId);

    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->qos.qos.ingress.profileIndex,
                                 pclActionReceivePtr->qos.qos.ingress.profileIndex,
     "get another pclActionExpectedPtr->qos.qos.ingress.profileIndex than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->qos.qos.ingress.profileAssignIndex,
                                 pclActionReceivePtr->qos.qos.ingress.profileAssignIndex,
     "get another pclActionExpectedPtr->qos.qos.ingress.profileAssignIndex than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->qos.qos.ingress.profilePrecedence,
                                 pclActionReceivePtr->qos.qos.ingress.profilePrecedence,
     "get another pclActionExpectedPtr->qos.qos.ingress.profilePrecedence than was set: %d, %d", lpmDBId, vrId);

    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->redirect.redirectCmd,
                                 pclActionReceivePtr->redirect.redirectCmd,
     "get another pclActionExpectedPtr->redirect.redirectCmd than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->redirect.data.routerLttIndex,
                                 pclActionReceivePtr->redirect.data.routerLttIndex,
     "get another pclActionExpectedPtr->redirect.data.routerLttIndex than was set: %d, %d", lpmDBId, vrId);

    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->policer.policerEnable,
                                 pclActionReceivePtr->policer.policerEnable,
     "get another pclActionExpectedPtr->policer.policerEnable than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->policer.policerId,
                                 pclActionReceivePtr->policer.policerId,
     "get another pclActionExpectedPtr->policer.policerId than was set: %d, %d", lpmDBId, vrId);

    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->vlan.modifyVlan,
                                 pclActionReceivePtr->vlan.modifyVlan,
     "get another pclActionExpectedPtr->vlan.modifyVlan than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->vlan.vlanId,
                                 pclActionReceivePtr->vlan.vlanId,
     "get another pclActionExpectedPtr->vlan.vlanId than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->vlan.precedence,
                                 pclActionReceivePtr->vlan.precedence,
     "get another pclActionExpectedPtr->vlan.precedence than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->vlan.nestedVlan,
                                 pclActionReceivePtr->vlan.nestedVlan,
     "get another pclActionExpectedPtr->vlan.nestedVlan than was set: %d, %d", lpmDBId, vrId);

    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->ipUcRoute.doIpUcRoute,
                                 pclActionReceivePtr->ipUcRoute.doIpUcRoute,
     "get another pclActionExpectedPtr->ipUcRoute.doIpUcRoute than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->ipUcRoute.arpDaIndex,
                                 pclActionReceivePtr->ipUcRoute.arpDaIndex,
     "get another pclActionExpectedPtr->ipUcRoute.arpDaIndex than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->ipUcRoute.decrementTTL,
                                 pclActionReceivePtr->ipUcRoute.decrementTTL,
     "get another pclActionExpectedPtr->ipUcRoute.decrementTTL than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->ipUcRoute.bypassTTLCheck,
                                 pclActionReceivePtr->ipUcRoute.bypassTTLCheck,
     "get another pclActionExpectedPtr->ipUcRoute.bypassTTLCheck than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(pclActionExpectedPtr->ipUcRoute.icmpRedirectCheck,
                                 pclActionReceivePtr->ipUcRoute.icmpRedirectCheck,
     "get another pclActionExpectedPtr->ipUcRoute.icmpRedirectCheck than was set: %d, %d", lpmDBId, vrId);

    return;
}

/*******************************************************************************
* prvUtfCompareIpLttEntries
*
* DESCRIPTION:
*       This routine compare IpLttEntries.
*
* INPUTS:
*       lpmDBId               - The LPM DB id.
*       vrId                  - The virtual router Id.
*       IpLttEntryReceivePtr  - (pointer to) Received value.
*       IpLttEntryExpectedPtr - (pointer to) Expected value.
*
* OUTPUTS:
*
* COMMENTS:
*       None.
*******************************************************************************/
static void prvUtfCompareIpLttEntries
(
    IN GT_U32                        lpmDBId,
    IN GT_U32                        vrId,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC*   IpLttEntryReceivePtr,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC*   IpLttEntryExpectedPtr
)
{
    if ((NULL == IpLttEntryReceivePtr) || (NULL == IpLttEntryExpectedPtr))
    {
        return;
    }

    UTF_VERIFY_EQUAL2_STRING_MAC(IpLttEntryExpectedPtr->routeType, IpLttEntryReceivePtr->routeType,
     "get another IpLttEntryExpectedPtr->routeType than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(IpLttEntryExpectedPtr->numOfPaths, IpLttEntryReceivePtr->numOfPaths,
     "get another IpLttEntryExpectedPtr->numOfPaths than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(IpLttEntryExpectedPtr->routeEntryBaseIndex,
                                 IpLttEntryReceivePtr->routeEntryBaseIndex,
     "get another IpLttEntryExpectedPtr->routeEntryBaseIndex than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(IpLttEntryExpectedPtr->ucRPFCheckEnable,
                                 IpLttEntryReceivePtr->ucRPFCheckEnable,
     "get another IpLttEntryExpectedPtr->ucRPFCheckEnable than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(IpLttEntryExpectedPtr->sipSaCheckMismatchEnable,
                                 IpLttEntryReceivePtr->sipSaCheckMismatchEnable,
     "get another IpLttEntryExpectedPtr->sipSaCheckMismatchEnable than was set: %d, %d", lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(IpLttEntryExpectedPtr->ipv6MCGroupScopeLevel,
                                 IpLttEntryReceivePtr->ipv6MCGroupScopeLevel,
     "get another IpLttEntryExpectedPtr->ipv6MCGroupScopeLevel than was set: %d, %d", lpmDBId, vrId);

    return;
}


/*******************************************************************************
* prvUtfSetDefaultActionEntry
*
* DESCRIPTION:
*       This routine fills returns PclIpUcActionEntry with default values.
*
* INPUTS:
*       pclActionPtr - (pointer to) PclIpUcActionEntry.
*
* OUTPUTS:
*
* COMMENTS:
*       None.
*******************************************************************************/
static void prvUtfSetDefaultActionEntry
(
    IN CPSS_DXCH_PCL_ACTION_STC*   pclActionPtr
)
{
    if (NULL == pclActionPtr)
    {
        return;
    }

    cpssOsBzero((GT_VOID*) pclActionPtr, sizeof(CPSS_DXCH_PCL_ACTION_STC));

    pclActionPtr->pktCmd = CPSS_PACKET_CMD_FORWARD_E;

    pclActionPtr->mirror.cpuCode                = CPSS_NET_CONTROL_DEST_MAC_TRAP_E;
    pclActionPtr->mirror.mirrorToRxAnalyzerPort = GT_FALSE;

    pclActionPtr->matchCounter.enableMatchCount  = GT_FALSE;
    pclActionPtr->matchCounter.matchCounterIndex = 0;

    pclActionPtr->egressPolicy     = GT_FALSE;
    pclActionPtr->qos.modifyDscp   = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
    pclActionPtr->qos.modifyUp     = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;

    pclActionPtr->qos.qos.ingress.profileIndex       = 0;
    pclActionPtr->qos.qos.ingress.profileAssignIndex = GT_FALSE;
    pclActionPtr->qos.qos.ingress.profilePrecedence  = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    pclActionPtr->redirect.redirectCmd         = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E;
    pclActionPtr->redirect.data.routerLttIndex = 0;

    pclActionPtr->policer.policerEnable = GT_FALSE;
    pclActionPtr->policer.policerId     = 0;

    /*pclActionPtr->vlan.egressTaggedModify = GT_FALSE;*/
    pclActionPtr->vlan.modifyVlan         = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
    pclActionPtr->vlan.vlanId             = IP_LPM_TESTED_VLAN_ID_CNS;
    pclActionPtr->vlan.precedence         = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    pclActionPtr->ipUcRoute.doIpUcRoute       = GT_FALSE;
    pclActionPtr->ipUcRoute.arpDaIndex        = 0;
    pclActionPtr->ipUcRoute.decrementTTL      = GT_FALSE;
    pclActionPtr->ipUcRoute.bypassTTLCheck    = GT_FALSE;
    pclActionPtr->ipUcRoute.icmpRedirectCheck = GT_FALSE;

    return;
}

/*******************************************************************************
* prvUtfSetDefaultIpLttEntry
*
* DESCRIPTION:
*       This routine fills returns IpLttEntry with default values.
*
* INPUTS:
*       pclActionPtr - (pointer to) IpLttEntry.
*
* OUTPUTS:
*
* COMMENTS:
*       None.
*******************************************************************************/
static void prvUtfSetDefaultIpLttEntry
(
    IN CPSS_DXCH_IP_LTT_ENTRY_STC*   ipLttEntry
)
{
    if (NULL == ipLttEntry)
    {
        return;
    }

    cpssOsBzero((GT_VOID*) ipLttEntry, sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));

    ipLttEntry->routeType                = CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    ipLttEntry->numOfPaths               = 0;
    ipLttEntry->routeEntryBaseIndex      = 0;
    ipLttEntry->ucRPFCheckEnable         = GT_TRUE;
    ipLttEntry->sipSaCheckMismatchEnable = GT_TRUE;
    ipLttEntry->ipv6MCGroupScopeLevel    = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;
}

/*******************************************************************************
* prvUtfCreateDefaultLpmDB
*
* DESCRIPTION:
*       This routine creates two LpmDBs
*
* INPUTS:
*
* OUTPUTS:
*
*       GT_OK           - Operation OK
*       GT_BAD_PARAM    - Internal error
*       GT_BAD_PTR      - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfCreateDefaultLpmDB(GT_U8 dev)
{
    GT_STATUS                                    retVal          = GT_OK;
    GT_U32                                       lpmDBId         = 0;
    GT_U32                                       maxTcamIndex    = 0;
    CPSS_DXCH_IP_TCAM_SHADOW_TYPE_ENT            shadowType      = CPSS_DXCH_IP_TCAM_CHEETAH_SHADOW_E;
    CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack   = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC indexesRange;
    GT_BOOL                                      partitionEnable = GT_FALSE;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC tcamLpmManagerCapcityCfg;


    /* set shadow type according to device family */
    switch (PRV_CPSS_PP_MAC(dev)->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            shadowType = CPSS_DXCH_IP_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E;
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
            shadowType = prvUtfIsPbrModeUsed() ? CPSS_DXCH_IP_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E
                                               : CPSS_DXCH_IP_TCAM_XCAT_SHADOW_E;
            break;
        case CPSS_PP_FAMILY_DXCH_LION_E:
            shadowType = CPSS_DXCH_IP_TCAM_XCAT_SHADOW_E;
            break;
        case CPSS_PP_FAMILY_CHEETAH3_E:
            shadowType = CPSS_DXCH_IP_TCAM_CHEETAH3_SHADOW_E;
            break;
        case CPSS_PP_FAMILY_CHEETAH2_E:
            shadowType = CPSS_DXCH_IP_TCAM_CHEETAH2_SHADOW_E;
            break;
        case CPSS_PP_FAMILY_CHEETAH_E:
            shadowType = CPSS_DXCH_IP_TCAM_CHEETAH_SHADOW_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
    partitionEnable = GT_TRUE;

    tcamLpmManagerCapcityCfg.numOfIpv4Prefixes         = 10;
    tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 5;
    tcamLpmManagerCapcityCfg.numOfIpv6Prefixes         = 10;

    /* get max Tcam index */
    maxTcamIndex = (CPSS_DXCH_IP_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E == shadowType) ?
                   (PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.policyTcamRaws) :
                   (PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.routerAndTunnelTermTcam);

    /* Create LpmDB [0] */
    lpmDBId = 0;

    indexesRange.firstIndex = 0;
    indexesRange.lastIndex  = 100;

    retVal = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);

    if((retVal != GT_OK)&&(retVal!=GT_ALREADY_EXIST))
        return retVal;

    /* Create LpmDB [1] */
    lpmDBId = 1;

    indexesRange.firstIndex = 200;
    indexesRange.lastIndex  = maxTcamIndex - 1;

    retVal = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);

    if((retVal != GT_OK)&&(retVal!=GT_ALREADY_EXIST))
        return retVal;


    /* LpmDB [2] is used in prvUtfCreateMcLpmDB for multicast */


    /* Create LpmDB [3] */
    lpmDBId = 3;

    indexesRange.firstIndex = maxTcamIndex / 2;
    indexesRange.lastIndex  = maxTcamIndex - 1;

    retVal = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);

    if((retVal != GT_OK)&&(retVal!=GT_ALREADY_EXIST))
        return retVal;

    /* Create LpmDB [4] */
    lpmDBId = 4;

    indexesRange.firstIndex = 101;
    indexesRange.lastIndex  = 199;

    retVal = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);

    if((retVal != GT_OK)&&(retVal!=GT_ALREADY_EXIST))
        return retVal;

    /* Create LpmDB [5] */
    lpmDBId = 5;

    indexesRange.firstIndex = 1001;
    indexesRange.lastIndex  = 1020;

    retVal = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);

    retVal = GT_ALREADY_EXIST == retVal ? GT_OK : retVal;

    return retVal;
}

/*******************************************************************************
* prvUtfCreateMcLpmDB
*
* DESCRIPTION:
*       This routine create LpmDB for testing multicast
*
* INPUTS:
*
* OUTPUTS:
*
*       GT_OK              - Operation OK
*       GT_BAD_PARAM       - Internal error
*       GT_BAD_PTR         - Null pointer
*       GT_NOT_IMPLEMENTED - multicast policy based routing not implemented for
*                            this dev family
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfCreateMcLpmDB(GT_U8 dev)
{
    GT_STATUS                                    retVal          = GT_OK;
    GT_U32                                       lpmDBId         = 0;
    CPSS_DXCH_IP_TCAM_SHADOW_TYPE_ENT            shadowType      = CPSS_DXCH_IP_TCAM_CHEETAH_SHADOW_E;
    CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack   = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC indexesRange;
    GT_BOOL                                      partitionEnable = GT_FALSE;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC tcamLpmManagerCapcityCfg;


    /* set shadow type according to device family */
    switch (PRV_CPSS_PP_MAC(dev)->devFamily)
    {
        case CPSS_PP_FAMILY_CHEETAH_E:
            shadowType = CPSS_DXCH_IP_TCAM_CHEETAH_SHADOW_E;
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
        case CPSS_PP_FAMILY_DXCH_LION_E:
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            shadowType = CPSS_DXCH_IP_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E;
            break;
        default:
            return GT_NOT_IMPLEMENTED;
    }

    protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
    partitionEnable = GT_TRUE;

    tcamLpmManagerCapcityCfg.numOfIpv4Prefixes         = 10;
    tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 5;
    tcamLpmManagerCapcityCfg.numOfIpv6Prefixes         = 10;

    /* Create LpmDB */
    lpmDBId = 2;

    indexesRange.firstIndex = 200;
    indexesRange.lastIndex  = 1000;

    retVal = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack, &indexesRange,
                                   partitionEnable, &tcamLpmManagerCapcityCfg,NULL);

    retVal = GT_ALREADY_EXIST == retVal ? GT_OK : retVal;

    return retVal;
}

/*******************************************************************************
* prvUtfCreateLpmDBAndVirtualRouterAdd
*
* DESCRIPTION:
*       This routine creates Virtual Routers based on prvUtfCreateDefaultLpmDB
*       configuration of lpmDBId.
*
* INPUTS:
*
*       dev - device ID
*
* OUTPUTS:
*
*       GT_OK           - Operation OK
*       GT_BAD_PARAM    - Internal error
*       GT_BAD_PTR      - Null pointer
*
* COMMENTS:
*        Cheetah2 doesn't support multiply virtual routers
*        (can be only one vrId in one lpmDBId)
*
*******************************************************************************/
static GT_STATUS prvUtfCreateLpmDBAndVirtualRouterAdd
(
    IN GT_U8 dev
)
{
    GT_STATUS                              retVal        = GT_OK;
    GT_U32                                 lpmDBId       = 0;
    GT_U32                                 vrId          = 0;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC         vrConfigInfo;

    cpssOsBzero((GT_VOID*) &vrConfigInfo, sizeof(CPSS_DXCH_IP_LPM_VR_CONFIG_STC));


    /* restore configuration after other UTs */
    /* remove changed virtual routers first */
    cpssDxChIpLpmVirtualRouterDel(0, 0);
    cpssDxChIpLpmVirtualRouterDel(1, 0);

    /* Create LpmDB */
    /* prepare device iterator */
    retVal = prvUtfNextNotApplicableDeviceReset(&dev, UTF_NONE_FAMILY_E);
    if (GT_OK != retVal)
    {
        return retVal;
    }

    /* Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        retVal = prvUtfCreateDefaultLpmDB(dev);
        if (GT_OK != retVal)
        {
            return retVal;
        }

        /* Adding Virtual Router for device */
        if ((CPSS_PP_FAMILY_CHEETAH_E == PRV_CPSS_PP_MAC(dev)->devFamily) || prvUtfIsPbrModeUsed())
        {
            vrConfigInfo.supportIpv4Mc = GT_FALSE;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;
            vrConfigInfo.supportIpv4Uc = GT_TRUE;
            vrConfigInfo.supportIpv6Uc = GT_TRUE;
            /* Fill defUcNextHopInfoPtr structure */
            prvUtfSetDefaultActionEntry(&vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction);
            prvUtfSetDefaultActionEntry(&vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction);

            /* add virtual router for 0 db*/
            lpmDBId = 0;
            retVal = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            if(retVal != GT_OK)
            {
                if(retVal==GT_ALREADY_EXIST)
                    continue;
                else
                    return retVal;
            }
            /* add virtual router for 1 db*/
            lpmDBId = 1;
            retVal = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            if(retVal != GT_OK)
            {
                if(retVal==GT_ALREADY_EXIST)
                    continue;
                else
                    return retVal;
            }
        }
        else
        {
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            vrConfigInfo.supportIpv6Mc = GT_TRUE;
            vrConfigInfo.supportIpv4Uc = GT_TRUE;
            vrConfigInfo.supportIpv6Uc = GT_TRUE;
            prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry);
            prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry);
            prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv4McRouteLttEntry);
            prvUtfSetDefaultIpLttEntry(&vrConfigInfo.defIpv6McRouteLttEntry);

            /* add virtual router 0 for 0 db*/
            lpmDBId = 0;
            vrId    = 0;
            retVal = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            if((retVal != GT_OK)&&(retVal!=GT_ALREADY_EXIST))
                return retVal;

            /* add virtual router 1 for 0 db (support only UC)*/
            lpmDBId = 3;
            vrId    = 1;
            vrConfigInfo.supportIpv4Mc = GT_FALSE;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;

            retVal = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            if((retVal != GT_OK)&&(retVal!=GT_ALREADY_EXIST))
                return retVal;

            /* add virtual router 0 for 1 db*/
            lpmDBId = 1;
            vrId    = 0;
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            vrConfigInfo.supportIpv6Mc = GT_TRUE;

            retVal = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            if((retVal != GT_OK)&&(retVal!=GT_ALREADY_EXIST))
                return retVal;

            /* add virtual router 1 for 1 db (support only MC)*/
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            vrConfigInfo.supportIpv6Mc = GT_TRUE;
            vrConfigInfo.supportIpv4Uc = GT_FALSE;
            vrConfigInfo.supportIpv6Uc = GT_FALSE;

            lpmDBId = 4;
            vrId    = 1;
            retVal = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
            if((retVal != GT_OK)&&(retVal!=GT_ALREADY_EXIST))
                return retVal;
        }
    }

    retVal = GT_ALREADY_EXIST == retVal ? GT_OK : retVal;

    return retVal;
}

/*******************************************************************************
* prvUtfGetDevShadow
*
* DESCRIPTION:
*       This routine getting dev shadow.
*
* INPUTS:
*
* OUTPUTS:
*
*       GT_OK           - Operation OK
*       GT_BAD_PARAM    - Internal error
*       GT_BAD_PTR      - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfGetDevShadow(GT_U8 dev,CPSS_DXCH_IP_TCAM_SHADOW_TYPE_ENT *shadowType)
{
    /* set shadow type according to device family */
    switch (PRV_CPSS_PP_MAC(dev)->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            *shadowType = CPSS_DXCH_IP_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E;
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
            *shadowType = prvUtfIsPbrModeUsed() ? CPSS_DXCH_IP_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E
                                                : CPSS_DXCH_IP_TCAM_XCAT_SHADOW_E;

            break;
        case CPSS_PP_FAMILY_DXCH_LION_E:
            *shadowType = CPSS_DXCH_IP_TCAM_XCAT_SHADOW_E;
            break;
        case CPSS_PP_FAMILY_CHEETAH3_E:
            *shadowType = CPSS_DXCH_IP_TCAM_CHEETAH3_SHADOW_E;
            break;
        case CPSS_PP_FAMILY_CHEETAH2_E:
            *shadowType = CPSS_DXCH_IP_TCAM_CHEETAH2_SHADOW_E;
            break;
        case CPSS_PP_FAMILY_CHEETAH_E:
            *shadowType = CPSS_DXCH_IP_TCAM_CHEETAH_SHADOW_E;
            break;
        default:
            return GT_BAD_PARAM;
    }
    return GT_OK;
}
