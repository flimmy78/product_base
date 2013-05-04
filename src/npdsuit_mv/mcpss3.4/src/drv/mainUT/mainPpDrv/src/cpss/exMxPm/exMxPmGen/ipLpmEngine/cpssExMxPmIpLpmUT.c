/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmIpLpmUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmIpLpm, that provides
*       the CPSS EXMXPM LPM Engine support.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/ipLpmEngine/cpssExMxPmIpLpm.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>
#include <utf/private/prvUtfExtras.h>

#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsStr.h>
#include <gtOs/gtOsRand.h>
#include <gtOs/gtOsTimer.h>

/* defines */

/* Default lpmDbId */
#define IP_LPM_DEFAULT_DB_ID_CNS            1

/* Default vrId */
#define IP_LPM_DEFAULT_VR_ID_CNS            0

/* Invalid enum */
#define IP_LPM_INVALID_ENUM_CNS             0x5AAAAAA5

/* Invalid lpmDBId */
#define IP_LPM_INVALID_DB_ID_CNS            55555

/* Invalid vrId */
#define IP_LPM_INVALID_VR_ID_CNS            77777

/* Max number of lpmDbId */
#define IP_LPM_DB_ID_MAX_CNS                32

/* Max number of Vitrual Routers in LpmDb */
#define IP_LPM_VIRTUAL_ROUTERS_MAX_CNS      32

/* Number of iteration for memory tests */
#define IP_LPM_ITER_COUNT                   1

/* Global variable for creating LpmDB */
static CPSS_EXMXPM_IP_MEM_CFG_STC  prvUtfMemCfgArray;

/* Max PrefixLen for IPV6 (bits) */
#define IP_LPM_IPV6_INVALID_PREFIX      129

/* Max param's in result param array */
#define IP_LPM_MAX_ARRAY_PARAMS_CNS      9

/* Default percent value for counters in result param array */
#define IP_LPM_PERCENT_COUNT_CNS        15

/* Default percent value for timers in result param array */
#define IP_LPM_PERCENT_TIME_CNS          7

/* BGP percent value for timers in result param array, it's
 * bigger because used prefix generator*/
#define IP_LPM_PERCENT_TIME_BGP_CNS     35

/* Bulk array size for add bulk prefixes. It is works about 50 seconds for 2000
 * arrays on linux simulation. Please, be patient.*/
#define IP_LPM_BAR_CNS                  2000

/* Max IP address octet (1 byte) */
#define IP_LPM_IP_ADDR_MAX              0xFF

/*Max range for IPV4 distributed prefixes (222 - 5) */
#define IP_LPM_IPV4_MAX_RANGE                  217

/*Max uniq values for prefix generate algoritm */
#define IP_LPM_MAX_UNIQ_VALUES                  42

/* Internal functions forward declaration */

/* Creates two LpmDBs. */
static GT_STATUS prvUtfCreateDefaultLpmDB();

/* Delete LpmDB. */
static GT_STATUS prvUtfDeleteDefaultLpmDB();

/* Creates LpmDB and Virtual Routers. */
static GT_STATUS prvUtfCreateLpmDBAndVirtualRouterAdd();

/* Delete LpmDB and Virtual Routers. */
static GT_STATUS prvUtfDeleteLpmDBAndVirtualRouter();

/* Generates IPV4 prefixes */
static GT_STATUS prvUtfGeneratePrefix
(
     OUT GT_U8 *firstBytePtr,
     OUT GT_U8 *secondBytePtr,
     OUT GT_U8 *thirdBytePtr,
     OUT GT_U8 *prefixLenPtr
);

/* Init prefix  generator */
static GT_VOID prvUtfInitGenerate();

/* Count values for prefix generator*/
static GT_U8   count0 = 0;
static GT_U8   count1 = IP_LPM_IP_ADDR_MAX;
static GT_U8   count2 = IP_LPM_IP_ADDR_MAX;

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmDbCreate
(
    IN  GT_U32                              lpmDbId,
    IN  CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC    *lpmDbConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmDbCreate)
{
/*
    1.1. Call with lpmDbId [1 / 2]
                   and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                    sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                    CPSS_SRAM_SIZE_512KB_E],
                                                    numOfSramsSizes [3 / 5]},
                                       numOfMemCfg [1],
                                       protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                       ipv6MemShare [50 / 0],
                                       numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpLpmDbConfigGet with the same lpmDbId and non-NULL lpmDbConfigPtr.
    Expected: GT_OK and the same lpmDbConfigPtr.
    1.3. Call with lpmDbId [0] (already created)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with lpmDbId [3],
                   lpmDbConfigPtr->memCfgArray->routingSramCfgType [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with lpmDbId [4],
                   lpmDbConfigPtr->memCfgArray->sramsSizeArray [0] [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with lpmDbId [5],
                   lpmDbConfigPtr->memCfgArray->routingSramCfgType [0x5AAAAAA5],
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with lpmDbId [6],
                   lpmDbConfigPtr->protocolStack [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with lpmDbId [7],
                   lpmDbConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.9. Call cpssExMxPmIpLpmDbDelete with lpmDbId [1 / 2] to delete created DB.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC    lpmDbConfig;
    CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC    lpmDbConfigGet;
    CPSS_EXMXPM_IP_MEM_CFG_STC          memCfgArray;
    CPSS_EXMXPM_IP_MEM_CFG_STC          memCfgArrayGet[10];

    cpssOsBzero((GT_VOID*) &lpmDbConfig, sizeof(lpmDbConfig));
    cpssOsBzero((GT_VOID*) &lpmDbConfigGet, sizeof(lpmDbConfigGet));
    cpssOsBzero((GT_VOID*) &memCfgArray, sizeof(memCfgArray));
    cpssOsBzero((GT_VOID*) &memCfgArrayGet, sizeof(memCfgArrayGet));

    /*
        1.1. Call with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
        Expected: GT_OK.
    */

    /* Call with lpmDbId [1] */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    memCfgArray.routingSramCfgType = CPSS_EXMXPM_LPM_ALL_EXTERNAL_E;
    memCfgArray.sramsSizeArray[0]  = CPSS_SRAM_SIZE_2MB_E;
    memCfgArray.sramsSizeArray[1]  = CPSS_SRAM_SIZE_2MB_E;
    memCfgArray.sramsSizeArray[2]  = CPSS_SRAM_SIZE_2MB_E;

    memCfgArray.numOfSramsSizes    = 3;

    lpmDbConfig.memCfgArray = &memCfgArray;

    lpmDbConfig.numOfMemCfg         = 1;
    lpmDbConfig.protocolStack       = CPSS_IP_PROTOCOL_IPV4V6_E;
    lpmDbConfig.ipv6MemShare        = 50;
    lpmDbConfig.numOfVirtualRouters = 1;

    st = cpssExMxPmIpLpmDbCreate(lpmDbId, &lpmDbConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    /*
        1.2. Call cpssExMxPmIpLpmDbConfigGet with the same lpmDbId and non-NULL lpmDbConfigPtr.
        Expected: GT_OK and the same lpmDbConfigPtr.
    */
    cpssOsBzero((GT_VOID*) &lpmDbConfigGet, sizeof(lpmDbConfigGet));
    cpssOsBzero((GT_VOID*) &memCfgArrayGet, sizeof(memCfgArrayGet));
    lpmDbConfigGet.memCfgArray = memCfgArrayGet;
    lpmDbConfigGet.numOfMemCfg = 10; /* with reserve */

    st = cpssExMxPmIpLpmDbConfigGet(lpmDbId, &lpmDbConfigGet);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmDbConfigGet: %d", lpmDbId);

    /* Verifying values - memCfgArray */
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->routingSramCfgType,
                                 lpmDbConfigGet.memCfgArray->routingSramCfgType,
               "get another lpmDbConfigPtr->memCfgArray->routingSramCfgType than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->sramsSizeArray[0],
                                 lpmDbConfigGet.memCfgArray->sramsSizeArray[0],
               "get another lpmDbConfigPtr->memCfgArray->sramsSizeArray[0] than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->sramsSizeArray[1],
                                 lpmDbConfigGet.memCfgArray->sramsSizeArray[1],
               "get another lpmDbConfigPtr->memCfgArray->sramsSizeArray[1] than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->sramsSizeArray[2],
                                 lpmDbConfigGet.memCfgArray->sramsSizeArray[2],
               "get another lpmDbConfigPtr->memCfgArray->sramsSizeArray[2] than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->numOfSramsSizes,
                                 lpmDbConfigGet.memCfgArray->numOfSramsSizes,
               "get another lpmDbConfigPtr->memCfgArray->numOfSramsSizes than was set: %d", lpmDbId);
    /* Verifying values - lpmDbConfig */
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.numOfMemCfg, lpmDbConfigGet.numOfMemCfg,
               "get another lpmDbConfigPtr->numOfMemCfg than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.protocolStack, lpmDbConfigGet.protocolStack,
               "get another lpmDbConfigPtr->protocolStack than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.ipv6MemShare, lpmDbConfigGet.ipv6MemShare,
               "get another lpmDbConfigPtr->ipv6MemShare than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.numOfVirtualRouters, lpmDbConfigGet.numOfVirtualRouters,
               "get another lpmDbConfigPtr->numOfVirtualRouters than was set: %d", lpmDbId);

    /* Call with lpmDbId [IP_LPM_DEFAULT_DB_ID_CNS+1] */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS+1;

    memCfgArray.routingSramCfgType = CPSS_EXMXPM_LPM_ALL_EXTERNAL_E;
    memCfgArray.sramsSizeArray[0]  = CPSS_SRAM_SIZE_512KB_E;
    memCfgArray.sramsSizeArray[1]  = CPSS_SRAM_SIZE_512KB_E;
    memCfgArray.sramsSizeArray[2]  = CPSS_SRAM_SIZE_512KB_E;
    memCfgArray.sramsSizeArray[3]  = CPSS_SRAM_SIZE_512KB_E;
    memCfgArray.sramsSizeArray[4]  = CPSS_SRAM_SIZE_512KB_E;

    memCfgArray.numOfSramsSizes    = 5;

    lpmDbConfig.memCfgArray = &memCfgArray;

    lpmDbConfig.numOfMemCfg         = 1;
    lpmDbConfig.protocolStack       = CPSS_IP_PROTOCOL_IPV4V6_E;
    lpmDbConfig.ipv6MemShare        = 0;
    lpmDbConfig.numOfVirtualRouters = 32;

    st = cpssExMxPmIpLpmDbCreate(lpmDbId, &lpmDbConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    /*
        1.2. Call cpssExMxPmIpLpmDbConfigGet with the same lpmDbId and non-NULL lpmDbConfigPtr.
        Expected: GT_OK and the same lpmDbConfigPtr.
    */
    cpssOsBzero((GT_VOID*) &lpmDbConfigGet, sizeof(lpmDbConfigGet));
    cpssOsBzero((GT_VOID*) &memCfgArrayGet, sizeof(memCfgArrayGet));
    lpmDbConfigGet.memCfgArray = memCfgArrayGet;
    lpmDbConfigGet.numOfMemCfg = 10; /* with reserve */

    st = cpssExMxPmIpLpmDbConfigGet(lpmDbId, &lpmDbConfigGet);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmDbConfigGet: %d", lpmDbId);

    /* Verifying values - memCfgArray */
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->routingSramCfgType,
                                 lpmDbConfigGet.memCfgArray->routingSramCfgType,
               "get another lpmDbConfigPtr->memCfgArray->routingSramCfgType than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->sramsSizeArray[0],
                                 lpmDbConfigGet.memCfgArray->sramsSizeArray[0],
               "get another lpmDbConfigPtr->memCfgArray->sramsSizeArray[0] than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->sramsSizeArray[1],
                                 lpmDbConfigGet.memCfgArray->sramsSizeArray[1],
               "get another lpmDbConfigPtr->memCfgArray->sramsSizeArray[1] than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->sramsSizeArray[2],
                                 lpmDbConfigGet.memCfgArray->sramsSizeArray[2],
               "get another lpmDbConfigPtr->memCfgArray->sramsSizeArray[2] than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->sramsSizeArray[3],
                                 lpmDbConfigGet.memCfgArray->sramsSizeArray[3],
               "get another lpmDbConfigPtr->memCfgArray->sramsSizeArray[3] than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->sramsSizeArray[4],
                                 lpmDbConfigGet.memCfgArray->sramsSizeArray[4],
               "get another lpmDbConfigPtr->memCfgArray->sramsSizeArray[4] than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->numOfSramsSizes,
                                 lpmDbConfigGet.memCfgArray->numOfSramsSizes,
               "get another lpmDbConfigPtr->memCfgArray->numOfSramsSizes than was set: %d", lpmDbId);
    /* Verifying values - lpmDbConfig */
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.numOfMemCfg, lpmDbConfigGet.numOfMemCfg,
               "get another lpmDbConfigPtr->numOfMemCfg than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.protocolStack, lpmDbConfigGet.protocolStack,
               "get another lpmDbConfigPtr->protocolStack than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.ipv6MemShare, lpmDbConfigGet.ipv6MemShare,
               "get another lpmDbConfigPtr->ipv6MemShare than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.numOfVirtualRouters, lpmDbConfigGet.numOfVirtualRouters,
               "get another lpmDbConfigPtr->numOfVirtualRouters than was set: %d", lpmDbId);

    /*
        1.3. Call with lpmDbId [0] (already created)
                       and other parameters from 1.1.
        Expected: NOT GT_OK.
    */
    lpmDbId = 0;

    st = cpssExMxPmIpLpmDbCreate(lpmDbId, &lpmDbConfig);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    /*
        1.4. Call with lpmDbId [3],
                       lpmDbConfigPtr->memCfgArray->routingSramCfgType [0x5AAAAAA5]
                       and other parameters from 1.1.
        Expected: GT_BAD_PARAM.
    */
    lpmDbId = 3;

    lpmDbConfig.memCfgArray[0].routingSramCfgType = IP_LPM_INVALID_ENUM_CNS;

    st = cpssExMxPmIpLpmDbCreate(lpmDbId, &lpmDbConfig);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, lpmDbConfigPtr->memCfgArray->routingSramCfgType = %d",
                                 lpmDbId, lpmDbConfig.memCfgArray[0].routingSramCfgType);

    lpmDbConfig.memCfgArray[0].routingSramCfgType = CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E;

    /*
        1.5. Call with lpmDbId [4],
                       lpmDbConfigPtr->memCfgArray->sramsSizeArray [0] [0x5AAAAAA5]
                       and other parameters from 1.1.
        Expected: GT_BAD_PARAM.
    */
    lpmDbId = 4;

    lpmDbConfig.memCfgArray[0].sramsSizeArray[0] = IP_LPM_INVALID_ENUM_CNS;

    st = cpssExMxPmIpLpmDbCreate(lpmDbId, &lpmDbConfig);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, lpmDbConfigPtr->memCfgArray->sramsSizeArray = %d",
                                 lpmDbId, lpmDbConfig.memCfgArray[0].sramsSizeArray[0]);

    lpmDbConfig.memCfgArray[0].sramsSizeArray[0] = CPSS_SRAM_SIZE_256KB_E;

    /*
        1.6. Call with lpmDbId [5],
                       lpmDbConfigPtr->memCfgArray->routingSramCfgType [0x5AAAAAA5],
                       and other parameters from 1.1.
        Expected: GT_BAD_PARAM.
    */
    lpmDbId = 5;

    lpmDbConfig.memCfgArray[0].routingSramCfgType = IP_LPM_INVALID_ENUM_CNS;

    st = cpssExMxPmIpLpmDbCreate(lpmDbId, &lpmDbConfig);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->memCfgArray->routingSramCfgType = %d",
                                 lpmDbId, lpmDbConfig.memCfgArray[0].routingSramCfgType);

    lpmDbConfig.memCfgArray[0].routingSramCfgType = CPSS_EXMXPM_LPM_ALL_EXTERNAL_E;

    /*
        1.7. Call with lpmDbId [6],
                       lpmDbConfigPtr->protocolStack [0x5AAAAAA5]
                       and other parameters from 1.1.
        Expected: GT_BAD_PARAM.
    */
    lpmDbId = 6;

    lpmDbConfig.protocolStack = IP_LPM_INVALID_ENUM_CNS;

    st = cpssExMxPmIpLpmDbCreate(lpmDbId, &lpmDbConfig);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, lpmDbConfigPtr->protocolStack = %d",
                                 lpmDbId, lpmDbConfig.protocolStack);

    lpmDbConfig.protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

    /*
        1.8. Call with lpmDbId [7],
                       lpmDbConfigPtr [NULL]
                       and other parameters from 1.1.
        Expected: GT_BAD_PTR.
    */
    lpmDbId = 7;

    st = cpssExMxPmIpLpmDbCreate(lpmDbId, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lpmDbConfigPtr = NULL", lpmDbId);

    /*
        1.9. Call cpssExMxPmIpLpmDbDelete with lpmDbId [1 / 2] to delete created DB.
        Expected: GT_OK.
    */

    /* Call with lpmDbId [1] */
    lpmDbId = 1;

    st = cpssExMxPmIpLpmDbDelete(lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmDbDelete: %d", lpmDbId);

    /* Call with lpmDbId [2] */
    lpmDbId = 2;

    st = cpssExMxPmIpLpmDbDelete(lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmDbDelete: %d", lpmDbId);
}

/*******************************************************************************
* IpLpmDB create, iterate delete, check memory leakages
1. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_before_DB.
2. Call cpssExMxPmIpLpmDbCreate to create IpLpmDB, fill all table with lpmDbId [1 .. max].
3. Call cpssExMxPmIpLpmDbConfigGet to check all IpLpmDB with lpmDbId [1 .. max].
4. Call cpssExMxPmIpLpmDbDelete to delete all routers for every IpLpmDB with lpmDbId [1 .. max].
5. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_after_DB.
Expected: (heap_bytes_allocated_after_DB - heap_bytes_allocated_before_DB) = 0
*******************************************************************************/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmDbMemoryCheck)
{
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      iTemp   = 0;

    GT_U32      heap_bytes_allocated_before_DB = 0;
    GT_U32      heap_bytes_allocated_after_DB = 0;

    CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC    lpmDbConfig;
    CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC    lpmDbConfigGet;
    CPSS_EXMXPM_IP_MEM_CFG_STC          memCfgArray;
    CPSS_EXMXPM_IP_MEM_CFG_STC          memCfgArrayGet;


    cpssOsBzero((GT_VOID*) &lpmDbConfig, sizeof(lpmDbConfig));
    cpssOsBzero((GT_VOID*) &lpmDbConfigGet, sizeof(lpmDbConfigGet));
    cpssOsBzero((GT_VOID*) &memCfgArray, sizeof(memCfgArray));
    cpssOsBzero((GT_VOID*) &memCfgArrayGet, sizeof(memCfgArrayGet));
    /*
        1. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_before_DB.
    */
    heap_bytes_allocated_before_DB = osMemGetHeapBytesAllocated();

    /*
        2. Call cpssExMxPmIpLpmDbCreate to create IpLpmDB, fill all table
                       with lpmDbId [1 .. max].
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_32KB_E],
                                                        numOfSramsSizes [1]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4_E],
                                           ipv6MemShare [50],
                                           numOfVirtualRouters [IP_LPM_VIRTUAL_ROUTERS_MAX_CNS]}.
        Expected: GT_OK.
    */
    memCfgArray.routingSramCfgType = CPSS_EXMXPM_LPM_ALL_EXTERNAL_E;
    memCfgArray.sramsSizeArray[0]  = CPSS_SRAM_SIZE_512KB_E;
    memCfgArray.sramsSizeArray[1]  = CPSS_SRAM_SIZE_512KB_E;
    memCfgArray.sramsSizeArray[2]  = CPSS_SRAM_SIZE_512KB_E;

    memCfgArray.numOfSramsSizes    = 3;

    lpmDbConfig.memCfgArray = &memCfgArray;

    lpmDbConfig.numOfMemCfg         = 1;
    lpmDbConfig.protocolStack       = CPSS_IP_PROTOCOL_IPV4V6_E;
    lpmDbConfig.ipv6MemShare        = 50;
    lpmDbConfig.numOfVirtualRouters = IP_LPM_VIRTUAL_ROUTERS_MAX_CNS;

    for(iTemp=1; iTemp<IP_LPM_DB_ID_MAX_CNS; ++iTemp)
    {
        lpmDbId = iTemp;

        /* make every entry unique */
        lpmDbConfig.numOfVirtualRouters = iTemp;

        st = cpssExMxPmIpLpmDbCreate(lpmDbId, &lpmDbConfig);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmDbCreate: %d", lpmDbId);
    }

    /*
        3. Call cpssExMxPmIpLpmDbConfigGet to check all IpLpmDB with lpmDbId [1 .. max].
        Expected: GT_OK and the same lpmDbConfigPtr as was set.
    */
    for(iTemp=1; iTemp<IP_LPM_DB_ID_MAX_CNS; ++iTemp)
    {
        lpmDbId = iTemp;

        /* restore unique entry before compare */
        lpmDbConfig.numOfVirtualRouters = iTemp;

        cpssOsBzero((GT_VOID*) &lpmDbConfigGet, sizeof(lpmDbConfigGet));
        lpmDbConfigGet.memCfgArray = &memCfgArrayGet;
        lpmDbConfigGet.numOfMemCfg = 1;

        st = cpssExMxPmIpLpmDbConfigGet(lpmDbId, &lpmDbConfigGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmDbConfigGet: %d", lpmDbId);

        if(st == GT_OK)
        {
            /* Verifying values - memCfgArray */
            UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->routingSramCfgType,
                                         lpmDbConfigGet.memCfgArray->routingSramCfgType,
                       "get another lpmDbConfigPtr->memCfgArray->routingSramCfgType than was set: %d", lpmDbId);
            UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->sramsSizeArray[0],
                                         lpmDbConfigGet.memCfgArray->sramsSizeArray[0],
                       "get another lpmDbConfigPtr->memCfgArray->sramsSizeArray[0] than was set: %d", lpmDbId);
            UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->sramsSizeArray[1],
                                         lpmDbConfigGet.memCfgArray->sramsSizeArray[1],
                       "get another lpmDbConfigPtr->memCfgArray->sramsSizeArray[1] than was set: %d", lpmDbId);
            UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->sramsSizeArray[2],
                                         lpmDbConfigGet.memCfgArray->sramsSizeArray[2],
                       "get another lpmDbConfigPtr->memCfgArray->sramsSizeArray[2] than was set: %d", lpmDbId);
            UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.memCfgArray->numOfSramsSizes,
                                         lpmDbConfigGet.memCfgArray->numOfSramsSizes,
                       "get another lpmDbConfigPtr->memCfgArray->numOfSramsSizes than was set: %d", lpmDbId);
            /* Verifying values - lpmDbConfig */
            UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.numOfMemCfg, lpmDbConfigGet.numOfMemCfg,
                       "get another lpmDbConfigPtr->numOfMemCfg than was set: %d", lpmDbId);
            UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.protocolStack, lpmDbConfigGet.protocolStack,
                       "get another lpmDbConfigPtr->protocolStack than was set: %d", lpmDbId);
            UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.ipv6MemShare, lpmDbConfigGet.ipv6MemShare,
                       "get another lpmDbConfigPtr->ipv6MemShare than was set: %d", lpmDbId);
            UTF_VERIFY_EQUAL1_STRING_MAC(lpmDbConfig.numOfVirtualRouters, lpmDbConfigGet.numOfVirtualRouters,
                       "get another lpmDbConfigPtr->numOfVirtualRouters than was set: %d", lpmDbId);
        }
    }

    /*
        4. Call cpssExMxPmIpLpmDbDelete to delete all routers for every IpLpmDB with lpmDbId [1 .. max].
        Expected: GT_OK.
    */
    for(iTemp=1; iTemp<IP_LPM_DB_ID_MAX_CNS; ++iTemp)
    {
        lpmDbId = iTemp;

        st = cpssExMxPmIpLpmDbDelete(lpmDbId);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmDbDelete: %d", lpmDbId);
    }

    /*
        5. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_after_DB.
        Expected: (heap_bytes_allocated_after_DB - heap_bytes_allocated_before_DB) = 0
    */
    heap_bytes_allocated_after_DB = osMemGetHeapBytesAllocated();
    UTF_VERIFY_EQUAL0_STRING_MAC(heap_bytes_allocated_before_DB, heap_bytes_allocated_after_DB,
                                 " size of heap changed (heap_bytes_allocated)");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmDbDelete
(
    IN  GT_U32      lpmDbId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmDbDelete)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.
    1.2. Call with lpmDbId [1 / 2].
    Expected: GT_OK.
    1.3. Call with lpmDbId [1] (already deleted).
    Expected: NOT GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                    CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [0 / 0]} to create DB.
        Expected: GT_OK.
    */
    st = prvUtfCreateDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

    /*
        1.2. Call with lpmDbId [1 / 2].
        Expected: GT_OK.
    */

    /* Call with lpmDbId [1] */
    lpmDbId = 1;

    st = cpssExMxPmIpLpmDbDelete(lpmDbId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    /* Call with lpmDbId [2] */
    lpmDbId = 2;

    st = cpssExMxPmIpLpmDbDelete(lpmDbId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    /*
        1.3. Call with lpmDbId [1] (already deleted).
        Expected: NOT GT_OK.
    */
    lpmDbId = 0;

    st = cpssExMxPmIpLpmDbDelete(lpmDbId);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmDbConfigGet
(
    IN  GT_U32                              lpmDbId,
    OUT CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC    *lpmDbConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmDbConfigGet)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.
    1.2. Call with lpmDbId [1]
                   and non-NULL lpmDbConfigPtr.
    Expected: GT_OK.
    1.3. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.2.
    Expected: NOT GT_OK.
    1.4. Call with lpmDbId [1]
                   and lpmDbConfigPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC    lpmDbConfig;
    CPSS_EXMXPM_IP_MEM_CFG_STC          memCfgArray;

    cpssOsBzero((GT_VOID*) &lpmDbConfig, sizeof(lpmDbConfig));
    cpssOsBzero((GT_VOID*) &memCfgArray, sizeof(memCfgArray));

    lpmDbConfig.memCfgArray = &memCfgArray;
    lpmDbConfig.numOfMemCfg = 1;

    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E],
                                                                                numOfSramsSizes [1]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50],
                                                                   numOfVirtualRouters [1]} to create DB.
        Expected: GT_OK.
    */
    st = prvUtfCreateDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

    /*
        1.2. Call with lpmDbId [1]
                       and non-NULL lpmDbConfigPtr.
        Expected: GT_OK.
    */
    lpmDbId = 1;

    st = cpssExMxPmIpLpmDbConfigGet(lpmDbId, &lpmDbConfig);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    /*
        1.3. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.2.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmDbConfigGet(lpmDbId, &lpmDbConfig);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    /*
        1.4. Call with lpmDbId [1]
                       and lpmDbConfigPtr [NULL].
        Expected: GT_BAD_PTR.
    */
    lpmDbId = 1;

    st = cpssExMxPmIpLpmDbConfigGet(lpmDbId, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lpmDbConfigPtr = NULL", lpmDbId);

    /* Delete defauld LpmDB*/
    st = prvUtfDeleteDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteDefaultLpmDB");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmDbDevListAdd
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      numOfDevs,
    IN  GT_U8       devListArray[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmDbDevListAdd)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1]
                                           and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E],
                                                                            sramsSizeArray [CPSS_SRAM_SIZE_32KB_E],
                                                                            numOfSramsSizes [1]},
                                                               numOfMemCfg [1],
                                                               protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                                               ipv6MemShare [50],
                                                               numOfVirtualRouters [0]} to create DB.
    Expected: GT_OK.
    1.2. Call with lpmDbId [1],
                   numOfDevs [numOfDevs]
                   and devListArray [dev1, dev2 … devN].
    Expected: GT_OK.
    1.3. Call cpssExMxPmIpLpmDbDevListGet with the same numOfDevsPtr
                                               and non-NULL devListArray.
    Expected: GT_OK and the same numOfDevs and devListArray.
    1.4. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.2.
    Expected: NOT GT_OK.
    1.5. Call with out of range numOfDevs [0]
                   and other parameters from 1.2.
    Expected: NOT GT_OK.
    1.6. Call with out of range devListArray[0] [PRV_CPSS_MAX_PP_DEVICES_CNS]
                   and other parameters from 1.2.
    Expected: NOT GT_OK.
    1.7. Call with devListArray [NULL]
                   and other parameters from 1.2.
    Expected: GT_BAD_PTR.
    1.8. Call cpssExMxPmIpLpmDbDevListRemove with lpmDbId [1],
                                                  numOfDevs [numOfDevs]
                                                  and devListArray [dev1, dev2 … devN] to remove added diveces.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U8       dev;

    GT_U32      numOfDevs    = 0;
    GT_U8       devListArray[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U32      numOfDevsGet = 0;
    GT_U32      numOfDevsTmp = 0;
    GT_U8       devListArrayGet[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_BOOL     isEqual      = GT_FALSE;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E],
                                                                                numOfSramsSizes [1]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50],
                                                                   numOfVirtualRouters [0]} to create DB.
        Expected: GT_OK.
    */
    st = prvUtfCreateDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

    /*
        1.2. Call with lpmDbId [1],
                       numOfDevs [numOfDevs]
                       and devListArray [dev1, dev2 … devN].
        Expected: GT_OK.
    */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        devListArray[numOfDevs++] = dev;
    }

    st = cpssExMxPmIpLpmDbDevListAdd(lpmDbId, numOfDevs, devListArray);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, numOfDevs);

    /*
        1.3. Call cpssExMxPmIpLpmDbDevListGet with the same numOfDevsPtr
                                                   and non-NULL devListArray.
        Expected: GT_OK and the same numOfDevs and devListArray.
    */
    numOfDevsGet = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpLpmDbDevListGet(lpmDbId, &numOfDevsGet, devListArrayGet);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmDbDevListGet: %d, %d", lpmDbId, numOfDevsGet);

    /* Verifying values */
    UTF_VERIFY_EQUAL1_STRING_MAC(numOfDevs, numOfDevsGet,
               "get another numOfDevs than was set: %d", lpmDbId);

    isEqual = (0 == cpssOsMemCmp((GT_VOID*) devListArray,
                                 (GT_VOID*) devListArrayGet,
                                 sizeof(devListArray[0] * numOfDevs))) ? GT_TRUE : GT_FALSE;
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
               "get another devListArray[] than was set: %d", lpmDbId);

    /*
        1.4. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.2.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmDbDevListAdd(lpmDbId, numOfDevs, devListArray);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.5. Call with out of range numOfDevs [0]
                       and other parameters from 1.2.
        Expected: NOT GT_OK.
    */
    numOfDevsTmp = 0;

    st = cpssExMxPmIpLpmDbDevListAdd(lpmDbId, numOfDevsTmp, devListArray);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, numOfDevsTmp);

    /*
        1.6. Call with out of range devListArray [0] [PRV_CPSS_MAX_PP_DEVICES_CNS]
                       and other parameters from 1.2.
        Expected: NOT GT_OK.
    */
    numOfDevsTmp    = 1;
    devListArray[0] = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpLpmDbDevListAdd(lpmDbId, numOfDevsTmp, devListArray);
    UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, devListArray [0] = %d", lpmDbId, devListArray[0]);

    devListArray[0] = 0;

    /*
        1.7. Call with devListArray [NULL]
                       and other parameters from 1.2.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmDbDevListAdd(lpmDbId, numOfDevs, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, devListArray = NULL", lpmDbId);

    /*
        1.8. Call cpssExMxPmIpLpmDbDevListRemove with lpmDbId [1],
                                                      numOfDevs [numOfDevs]
                                                      and devListArray [dev1, dev2 … devN] to remove added diveces.
        Expected: GT_OK.
    */
    lpmDbId = 1;

    st = cpssExMxPmIpLpmDbDevListRemove(lpmDbId, numOfDevs, devListArray);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmDbDevListRemove: %d, %d", lpmDbId, numOfDevs);

    /* Delete defauld LpmDB*/
    st = prvUtfDeleteDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteDefaultLpmDB");
}

/*******************************************************************************
* DevLists add, check, delete, check memory leakages
1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
Expected: GT_OK.
2. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_before_DEV.
3. Call cpssExMxPmIpLpmDbDevListAdd to create dev list for every IpLpmDB with lpmDbId [1 .. 2].
Expected: GT_OK.
4. Call cpssExMxPmIpLpmDbDevListGet to check dev list for every IpLpmDB with lpmDbId [1 .. 2].
Expected: GT_OK.
5. Call cpssExMxPmIpLpmDbDevListRemove to remove dev list for every IpLpmDB with lpmDbId [1 .. 2].
Expected: GT_OK.
6. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_after_DEV.
Expected: (heap_bytes_allocated_after_DEV-heap_bytes_allocated_before_DEV) = 0
7. Delete defauld LpmDB.
Expected: GT_OK.
*******************************************************************************/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmDevListMemoryCheck)
{
    GT_STATUS   st        = GT_OK;
    GT_BOOL     isEqual   = GT_FALSE;
    GT_U8       dev       = 0;
    GT_U8       ii        = 0;

    GT_U32      lpmDbId = 0;

    GT_U8       devListArray[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U8       devListArrayGet[PRV_CPSS_MAX_PP_DEVICES_CNS];

    GT_U32      numOfDevs = 0;
    GT_U32      numOfDevsGet = 0;

    GT_U32 heap_bytes_allocated_before_DEV = 0;
    GT_U32 heap_bytes_allocated_after_DEV = 0;

    cpssOsBzero((GT_VOID*) &devListArray, sizeof(devListArray));
    cpssOsBzero((GT_VOID*) &devListArrayGet, sizeof(devListArrayGet));

    /*
        1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
        Expected: GT_OK.
    */
    st = prvUtfCreateDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

    /*
        2. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_before_DEV.
    */
    heap_bytes_allocated_before_DEV = osMemGetHeapBytesAllocated();

    for(ii=0; ii<IP_LPM_ITER_COUNT; ++ii)
    {
        /*
            3. Call cpssExMxPmIpLpmDbDevListAdd to create dev list for every IpLpmDB
                      with lpmDbId [1 .. 2].
                           numOfDevs [numOfDevs]
                           and devListArray [dev1, dev2 … devN].
            Expected: GT_OK.
        */
        /* prepare device iterator */
        st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* Go over all active devices. */
        while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
        {
            devListArray[numOfDevs++] = dev;
        }

        lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

        st = cpssExMxPmIpLpmDbDevListAdd(lpmDbId, numOfDevs, devListArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmDbDevListAdd: %d, %d", lpmDbId, numOfDevs);

        lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS + 1;

        st = cpssExMxPmIpLpmDbDevListAdd(lpmDbId, numOfDevs, devListArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmDbDevListAdd: %d, %d", lpmDbId, numOfDevs);

        /*
            4. Call cpssExMxPmIpLpmDbDevListGet to check dev list for every IpLpmDB
                                        with lpmDbId [1 .. 2],
                                             the same numOfDevsPtr,
                                         and non-NULL devListArray.
            Expected: GT_OK and the same numOfDevs and devListArray.
        */
        /* iterate with lpmDbId [IP_LPM_DEFAULT_DB_ID_CNS] */
        lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
        numOfDevsGet = PRV_CPSS_MAX_PP_DEVICES_CNS;
        cpssOsBzero((GT_VOID*) &devListArrayGet, sizeof(devListArrayGet));

        st = cpssExMxPmIpLpmDbDevListGet(lpmDbId, &numOfDevsGet, devListArrayGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmDbDevListGet: %d, %d", lpmDbId, numOfDevsGet);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(numOfDevs, numOfDevsGet, "get another numOfDevs than was set: %d", lpmDbId);
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) devListArray, (GT_VOID*) devListArrayGet,
                                     sizeof(devListArray[0] * numOfDevs))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another devListArray[] than was set: %d", lpmDbId);

        /* iterate with lpmDbId [IP_LPM_DEFAULT_DB_ID_CNS + 1] */
        lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS + 1;
        numOfDevsGet = PRV_CPSS_MAX_PP_DEVICES_CNS;
        cpssOsBzero((GT_VOID*) &devListArrayGet, sizeof(devListArrayGet));

        st = cpssExMxPmIpLpmDbDevListGet(lpmDbId, &numOfDevsGet, devListArrayGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmDbDevListGet: %d, %d", lpmDbId, numOfDevsGet);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(numOfDevs, numOfDevsGet, "get another numOfDevs than was set: %d", lpmDbId);
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) devListArray, (GT_VOID*) devListArrayGet,
                                     sizeof(devListArray[0] * numOfDevs))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "get another devListArray[] than was set: %d", lpmDbId);

        /*
            5. Call cpssExMxPmIpLpmDbDevListRemove to remove dev list for every IpLpmDB
                                            with lpmDbId [1 .. 2].
                                                 numOfDevs [numOfDevs],
                                             and devListArray [dev1, dev2 … devN] to remove added diveces.
            Expected: GT_OK.
        */
        /* iterate with lpmDbId [IP_LPM_DEFAULT_DB_ID_CNS] */
        lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

        st = cpssExMxPmIpLpmDbDevListRemove(lpmDbId, numOfDevs, devListArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmDbDevListRemove: %d, %d", lpmDbId, numOfDevs);

        /* iterate with lpmDbId [IP_LPM_DEFAULT_DB_ID_CNS + 1] */
        lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS + 1;

        st = cpssExMxPmIpLpmDbDevListRemove(lpmDbId, numOfDevs, devListArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmDbDevListRemove: %d, %d", lpmDbId, numOfDevs);
    }/* for */

    /*
        6. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_after_DB.
        Expected: (heap_bytes_allocated_after_DEV - heap_bytes_allocated_before_DEV) = 0
    */
    heap_bytes_allocated_after_DEV = osMemGetHeapBytesAllocated();
    UTF_VERIFY_EQUAL0_STRING_MAC(heap_bytes_allocated_before_DEV, heap_bytes_allocated_after_DEV,
                                 " size of heap changed (heap_bytes_allocated)");

    /*
        7. Delete defauld LpmDB.
        Expected: GT_OK.
    */
    st = prvUtfDeleteDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteDefaultLpmDB");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmDbDevListRemove
(
    IN  GT_U32  lpmDbId,
    IN  GT_U32  numOfDevs,
    IN  GT_U8   devListArray[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmDbDevListRemove)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1]
                                           and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E],
                                                                            sramsSizeArray [CPSS_SRAM_SIZE_32KB_E],
                                                                            numOfSramsSizes [1]},
                                                               numOfMemCfg [1],
                                                               protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                                               ipv6MemShare [50],
                                                               numOfVirtualRouters [0]} to create DB.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpLpmDbDevListAdd with lpmDbId [1],
                                               numOfDevs [numOfDevs]
                                               and devListArray [dev1, dev2 … devN] to add device list.
    Expected: GT_OK.
    1.3. Call with lpmDbId [1],
                   numOfDevs [numOfDevs]
                   and devListArray [dev1, dev2 … devN].
    Expected: GT_OK.
    1.4. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.5. Call with lpmDbId [1],
                   out of range numOfDevs [0]
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.6. Call with lpmDbId [1],
                   and out of range devListArray [PRV_CPSS_MAX_PP_DEVICES_CNS]
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.7. Call with lpmDbId [1],
                   devListArray [NULL]
                   and other parameters from 1.3.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U8       dev;

    GT_U32      numOfDevs    = 0;
    GT_U8       devListArray[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U32      numOfDevsTmp = 0;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E],
                                                                                numOfSramsSizes [1]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50],
                                                                   numOfVirtualRouters [0]} to create DB.
        Expected: GT_OK.
    */
    st = prvUtfCreateDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

    /*
        1.2. Call cpssExMxPmIpLpmDbDevListAdd with lpmDbId [1],
                                                   numOfDevs [numOfDevs]
                                                   and devListArray [dev1, dev2 … devN] to add device list.
        Expected: GT_OK.
    */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        devListArray[numOfDevs++] = dev;
    }

    st = cpssExMxPmIpLpmDbDevListAdd(lpmDbId, numOfDevs, devListArray);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmDbDevListAdd: %d, %d", lpmDbId, numOfDevs);

    /*
        1.3. Call with lpmDbId [1],
                       numOfDevs [numOfDevs]
                       and devListArray [dev1, dev2 … devN].
        Expected: GT_OK.
    */
    lpmDbId = 1;

    st = cpssExMxPmIpLpmDbDevListRemove(lpmDbId, numOfDevs, devListArray);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, numOfDevs);

    /*
        1.4. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.2.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmDbDevListRemove(lpmDbId, numOfDevs, devListArray);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.5. Call with out of range numOfDevs [0]
                       and other parameters from 1.2.
        Expected: NOT GT_OK.
    */
    numOfDevsTmp = 0;

    st = cpssExMxPmIpLpmDbDevListRemove(lpmDbId, numOfDevsTmp, devListArray);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, numOfDevsTmp);

    /*
        1.6. Call with out of range devListArray [0] [PRV_CPSS_MAX_PP_DEVICES_CNS]
                       and other parameters from 1.2.
        Expected: NOT GT_OK.
    */
    numOfDevsTmp    = 1;
    devListArray[0] = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpLpmDbDevListRemove(lpmDbId, numOfDevsTmp, devListArray);
    UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, devListArray [0] = %d", lpmDbId, devListArray[0]);

    devListArray[0] = 0;

    /*
        1.7. Call with devListArray [NULL]
                       and other parameters from 1.2.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmDbDevListRemove(lpmDbId, numOfDevs, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, devListArray = NULL", lpmDbId);

    /* Delete defauld LpmDB*/
    st = prvUtfDeleteDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteDefaultLpmDB");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmDbDevListGet
(
    IN    GT_U32   lpmDbId,
    INOUT GT_U32   *numOfDevsPtr,
    OUT   GT_U8    devListArray[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmDbDevListGet)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1]
                                           and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E],
                                                                            sramsSizeArray [CPSS_SRAM_SIZE_32KB_E],
                                                                            numOfSramsSizes [1]},
                                                               numOfMemCfg [1],
                                                               protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                                               ipv6MemShare [50],
                                                               numOfVirtualRouters [0]} to create DB.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpLpmDbDevListAdd with lpmDbId [1],
                                               numOfDevs [numOfDevs]
                                               and devListArray [dev1, dev2 … devN] to add device list.
    Expected: GT_OK.
    1.3. Call with lpmDbId [1],
                   numOfDevsPtr [numOfDevs]
                   and non-NULL devListArray.
    Expected: GT_OK.
    1.4. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.5. Call with numOfDevsPtr [NULL]
                   and other parameters from 1.3.
    Expected: GT_BAD_PTR.
    1.6. Call with devListArray [NULL]
                   and other parameters from 1.3.
    Expected: GT_BAD_PTR.
    1.7. Call cpssExMxPmIpLpmDbDevListRemove with lpmDbId [1],
                                                  numOfDevs [numOfDevs]
                                                  and devListArray [dev1, dev2 … devN] to remove added diveces.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U8       dev;

    GT_U32      numOfDevs    = 0;
    GT_U8       devListArray[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U32      numOfDevsTmp = 0;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E],
                                                                                numOfSramsSizes [1]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50],
                                                                   numOfVirtualRouters [0]} to create DB.
        Expected: GT_OK.
    */
    st = prvUtfCreateDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

    /*
        1.2. Call cpssExMxPmIpLpmDbDevListAdd with lpmDbId [1],
                                                   numOfDevs [numOfDevs]
                                                   and devListArray [dev1, dev2 … devN] to add device list.
        Expected: GT_OK.
    */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        devListArray[numOfDevs++] = dev;
    }

    st = cpssExMxPmIpLpmDbDevListAdd(lpmDbId, numOfDevs, devListArray);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmDbDevListAdd: %d, %d", lpmDbId, numOfDevs);

    /*
        1.3. Call with lpmDbId [1],
                       numOfDevsPtr [numOfDevs]
                       and non-NULL devListArray.
        Expected: GT_OK.
    */
    lpmDbId      = 1;
    numOfDevsTmp = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpLpmDbDevListGet(lpmDbId, &numOfDevsTmp, devListArray);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, numOfDevsTmp);

    /*
        1.4. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.2.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmDbDevListGet(lpmDbId, &numOfDevsTmp, devListArray);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.5. Call with numOfDevsPtr [NULL]
                       and other parameters from 1.3.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmDbDevListGet(lpmDbId, NULL, devListArray);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfDevsPtr = NULL", lpmDbId);

    /*
        1.6. Call with devListArray [NULL]
                       and other parameters from 1.2.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmDbDevListGet(lpmDbId, &numOfDevsTmp, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, devListArray = NULL", lpmDbId);

    /*
        1.7. Call cpssExMxPmIpLpmDbDevListRemove with lpmDbId [1],
                                                      numOfDevs [numOfDevs]
                                                      and devListArray [dev1, dev2 … devN] to remove added diveces.
        Expected: GT_OK.
    */
    lpmDbId = 1;

    st = cpssExMxPmIpLpmDbDevListRemove(lpmDbId, numOfDevs, devListArray);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmDbDevListRemove: %d, %d", lpmDbId, numOfDevs);

    /* Delete defauld LpmDB*/
    st = prvUtfDeleteDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteDefaultLpmDB");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmVirtualRouterAdd
(
    IN  GT_U32                              lpmDbId,
    IN  GT_U32                              vrId,
    IN  CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC    *vrConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmVirtualRouterAdd)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.
    1.2. Call with lpmDbId [1 / 2],
                   vrId [0 / 0]
                   and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportMcIpv4 [GT_TRUE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportUcIpv6 [GT_TRUE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}}.
                                    supportMcIpv6 [GT_TRUE],
                                    defaultMcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                    defaultMcIpv6RuleIndex [0 / 100],
                                    defaultMcIpv6PclId [0 / 100]}.
    Expected: GT_OK.
    1.3. Call with lpmDbId [1],
                   vrId [1]
                   and vrConfigPtr {supportUcIpv4 [GT_FALSE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0],
                                                             blockSize [0],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E]},
                                    supportMcIpv4 [GT_FALSE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0],
                                                             blockSize [0],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E]},
                                    supportUcIpv6 [GT_FALSE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0],
                                                             blockSize [0],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E]}}.
                                    supportMcIpv6 [GT_FALSE],
                                    defaultMcIpv6RouteEntry {routeEntryBaseMemAddr [0],
                                                             blockSize [0],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E]}
                                    defaultMcIpv6RuleIndex [0 / 100],
                                    defaultMcIpv6PclId [0 / 100]}.
    Expected: GT_OK.
    1.4. Call cpssExMxPmIpLpmVirtualRouterConfigGet with the same lpmDbId,
                                                         vrId and non-NULL vrConfigPtr.
    Expected: GT_OK and the same vrConfigPtr.
    1.5. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.6. Call with lpmDbId [1],
                   vrId[0] (already exist)
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.7. Call with lpmDbId [2],
                   vrId[2], vrConfigPtr->defaultUcIpv4RouteEntry.blockSize [0] (for regular should be 1),
                   vrConfigPtr->defaultUcIpv4RouteEntry.routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]
                   and other parameters from 1.2.
    Expected: NOT GT_OK.
    1.8. Call with lpmDbId [2],
                   vrId[2],
                   out of range vrConfigPtr->defaultUcIpv4RouteEntry.routeEntryMethod [0x5AAAAAA5]
                   and other parameters from 1.2.
    Expected: GT_BAD_PARAM.
    1.9. Call with lpmDbId [2],
                   vrId[2],
                   out of range vrConfigPtr->defaultMcIpv4RouteEntry.routeEntryMethod [0x5AAAAAA5]
                   and other parameters from 1.2.
    Expected: GT_BAD_PARAM.
    1.10. Call with lpmDbId [2],
                    vrId[2],
                    out of range vrConfigPtr->efaultUcIpv6RouteEntry.routeEntryMethod [0x5AAAAAA5]
                    and other parameters from 1.2.
    Expected: GT_BAD_PARAM.
    1.11. Call with lpmDbId [2],
                    vrId[2]
                    and vrConfigPtr [NULL].
    Expected: GT_BAD_PTR.
    1.12. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId [1 / 2]
                                                       and vrId [0 / 1] to delete virtual routers.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC    vrConfig;
    CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC    vrConfigGet;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                    CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.
    */
    st = prvUtfCreateDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

    /*
        1.2. Call with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}}.
                                        supportMcIpv6 [GT_TRUE],
                                        defaultMcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                        defaultMcIpv6RuleIndex [2047],
                                        defaultMcIpv6PclId [0 / 100]}.
        Expected: GT_OK.
    */

    /* Call with lpmDbId [1] */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    vrConfig.supportUcIpv4 = GT_TRUE;
    vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr = 0;
    vrConfig.defaultUcIpv4RouteEntry.blockSize = 1;

    vrConfig.supportMcIpv4 = GT_TRUE;
    vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr = 1;
    vrConfig.defaultMcIpv4RouteEntry.blockSize = 1;

    vrConfig.supportUcIpv6 = GT_TRUE;
    vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr = 2;
    vrConfig.defaultUcIpv6RouteEntry.blockSize = 1;

    vrConfig.supportMcIpv6 = GT_TRUE;
    vrConfig.defaultMcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultMcIpv6RouteEntry.routeEntryBaseMemAddr = 3;
    vrConfig.defaultMcIpv6RouteEntry.blockSize = 1;

    vrConfig.defaultMcIpv6RuleIndex = 2047;
    vrConfig.defaultMcIpv6PclId = 0;

    st = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &vrConfig);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*
        1.4. Call cpssExMxPmIpLpmVirtualRouterConfigGet with the same lpmDbId,
                                                             vrId and non-NULL vrConfigPtr.
        Expected: GT_OK and the same vrConfigPtr.
    */
    st = cpssExMxPmIpLpmVirtualRouterConfigGet(lpmDbId, vrId, &vrConfigGet);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmVirtualRouterConfigGet: %d, %d", lpmDbId, vrId);

    /* Verifying values */
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.supportUcIpv4, vrConfigGet.supportUcIpv4,
               "get another vrConfigPtr->supportUcIpv4 than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr,
                                 vrConfigGet.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr,
               "get another vrConfigPtr->defaultUcIpv4RouteEntry.routeEntryBaseMemAddr than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultUcIpv4RouteEntry.blockSize,
                                 vrConfigGet.defaultUcIpv4RouteEntry.blockSize,
               "get another vrConfigPtr->defaultUcIpv4RouteEntry.blockSize than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod,
                                 vrConfigGet.defaultUcIpv4RouteEntry.routeEntryMethod,
               "get another vrConfigPtr->defaultUcIpv4RouteEntry.routeEntryMethod than was set: %d", lpmDbId);

    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.supportMcIpv4, vrConfigGet.supportMcIpv4,
               "get another vrConfigPtr->supportMcIpv4 than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr,
                                 vrConfigGet.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr,
               "get another vrConfigPtr->defaultMcIpv4RouteEntry.routeEntryBaseMemAddr than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultMcIpv4RouteEntry.blockSize,
                                 vrConfigGet.defaultMcIpv4RouteEntry.blockSize,
               "get another vrConfigPtr->defaultMcIpv4RouteEntry.blockSize than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod,
                                 vrConfigGet.defaultMcIpv4RouteEntry.routeEntryMethod,
               "get another vrConfigPtr->defaultMcIpv4RouteEntry.routeEntryMethod than was set: %d", lpmDbId);

    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.supportUcIpv6, vrConfigGet.supportUcIpv6,
               "get another vrConfigPtr->supportUcIpv6 than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr,
                                 vrConfigGet.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr,
               "get another vrConfigPtr->defaultUcIpv6RouteEntry.routeEntryBaseMemAddr than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultUcIpv6RouteEntry.blockSize,
                                 vrConfigGet.defaultUcIpv6RouteEntry.blockSize,
               "get another vrConfigPtr->defaultUcIpv6RouteEntry.blockSize than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod,
                                 vrConfigGet.defaultUcIpv6RouteEntry.routeEntryMethod,
               "get another vrConfigPtr->defaultUcIpv6RouteEntry.routeEntryMethod than was set: %d", lpmDbId);

    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.supportMcIpv6, vrConfigGet.supportMcIpv6,
               "get another vrConfigPtr->supportMcIpv6 than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultMcIpv6RouteEntry.routeEntryBaseMemAddr,
                                 vrConfigGet.defaultMcIpv6RouteEntry.routeEntryBaseMemAddr,
               "get another vrConfigPtr->defaultMcIpv6RouteEntry.routeEntryBaseMemAddr than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultMcIpv6RouteEntry.blockSize,
                                 vrConfigGet.defaultMcIpv6RouteEntry.blockSize,
               "get another vrConfigPtr->defaultMcIpv6RouteEntry.blockSize than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultMcIpv6RouteEntry.routeEntryMethod,
                                 vrConfigGet.defaultMcIpv6RouteEntry.routeEntryMethod,
               "get another vrConfigPtr->defaultMcIpv6RouteEntry.routeEntryMethod than was set: %d", lpmDbId);

    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultMcIpv6RuleIndex, vrConfigGet.defaultMcIpv6RuleIndex,
                                 "get another vrConfig.defaultMcIpv6RuleIndex than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultMcIpv6PclId, vrConfigGet.defaultMcIpv6PclId,
                                 "get another vrConfig.defaultMcIpv6PclId than was set: %d", lpmDbId);

    /* Call with lpmDbId [2] */
    lpmDbId = 2;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    vrConfig.supportUcIpv4 = GT_TRUE;
    vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    vrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr = 0;
    vrConfig.defaultUcIpv4RouteEntry.blockSize = 1;

    vrConfig.supportMcIpv4 = GT_TRUE;
    vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    vrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr = 1;
    vrConfig.defaultMcIpv4RouteEntry.blockSize = 1;

    vrConfig.supportUcIpv6 = GT_FALSE;
    vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    vrConfig.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr = 2;
    vrConfig.defaultUcIpv6RouteEntry.blockSize = 1;

    vrConfig.supportMcIpv6 = GT_FALSE;
    vrConfig.defaultMcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    vrConfig.defaultMcIpv6RouteEntry.routeEntryBaseMemAddr = 3;
    vrConfig.defaultMcIpv6RouteEntry.blockSize = 1;

    vrConfig.defaultMcIpv6RuleIndex = 2047;
    vrConfig.defaultMcIpv6PclId = 0;

    st = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &vrConfig);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*
        1.4. Call cpssExMxPmIpLpmVirtualRouterConfigGet with the same lpmDbId,
                                                             vrId and non-NULL vrConfigPtr.
        Expected: GT_OK and the same vrConfigPtr.
    */
    st = cpssExMxPmIpLpmVirtualRouterConfigGet(lpmDbId, vrId, &vrConfigGet);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmVirtualRouterConfigGet: %d, %d", lpmDbId, vrId);

    /* Verifying values */
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.supportUcIpv4, vrConfigGet.supportUcIpv4,
               "get another vrConfigPtr->supportUcIpv4 than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr,
                                 vrConfigGet.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr,
               "get another vrConfigPtr->defaultUcIpv4RouteEntry.routeEntryBaseMemAddr than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultUcIpv4RouteEntry.blockSize,
                                 vrConfigGet.defaultUcIpv4RouteEntry.blockSize,
               "get another vrConfigPtr->defaultUcIpv4RouteEntry.blockSize than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod,
                                 vrConfigGet.defaultUcIpv4RouteEntry.routeEntryMethod,
               "get another vrConfigPtr->defaultUcIpv4RouteEntry.routeEntryMethod than was set: %d", lpmDbId);

    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.supportMcIpv4, vrConfigGet.supportMcIpv4,
               "get another vrConfigPtr->supportMcIpv4 than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr,
                                 vrConfigGet.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr,
               "get another vrConfigPtr->defaultMcIpv4RouteEntry.routeEntryBaseMemAddr than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultMcIpv4RouteEntry.blockSize,
                                 vrConfigGet.defaultMcIpv4RouteEntry.blockSize,
               "get another vrConfigPtr->defaultMcIpv4RouteEntry.blockSize than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod,
                                 vrConfigGet.defaultMcIpv4RouteEntry.routeEntryMethod,
               "get another vrConfigPtr->defaultMcIpv4RouteEntry.routeEntryMethod than was set: %d", lpmDbId);

    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.supportUcIpv6, vrConfigGet.supportUcIpv6,
               "get another vrConfigPtr->supportUcIpv6 than was set: %d", lpmDbId);

    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.supportMcIpv6, vrConfigGet.supportMcIpv6,
               "get another vrConfigPtr->supportMcIpv6 than was set: %d", lpmDbId);

    /*
        1.3. Call with lpmDbId [2],
                       vrId [1]
                       and vrConfigPtr {supportUcIpv4 [GT_FALSE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0],
                                                                 blockSize [0],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E]},
                                        supportMcIpv4 [GT_FALSE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0],
                                                                 blockSize [0],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E]},
                                        supportUcIpv6 [GT_FALSE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0],
                                                                 blockSize [0],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E]}}.
                                        supportMcIpv6 [GT_FALSE],
                                        defaultMcIpv6RouteEntry {routeEntryBaseMemAddr [0],
                                                                 blockSize [0],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E]}}.
                                        defaultMcIpv6RuleIndex [0 / 100],
                                        defaultMcIpv6PclId [0 / 100]}.
        Expected: GT_OK.
    */
    vrId = 1;

    vrConfig.supportUcIpv4 = GT_TRUE;
    vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr = 0;
    vrConfig.defaultUcIpv4RouteEntry.blockSize = 3;

    vrConfig.supportMcIpv4 = GT_TRUE;
    vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr = 1;
    vrConfig.defaultMcIpv4RouteEntry.blockSize = 3;

    vrConfig.supportUcIpv6 = GT_FALSE;
    vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr = 2;
    vrConfig.defaultUcIpv6RouteEntry.blockSize = 3;

    vrConfig.supportMcIpv6 = GT_FALSE;
    vrConfig.defaultMcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultMcIpv6RouteEntry.routeEntryBaseMemAddr = 3;
    vrConfig.defaultMcIpv6RouteEntry.blockSize = 3;

    vrConfig.defaultMcIpv6RuleIndex = 2047;
    vrConfig.defaultMcIpv6PclId = 0;

    st = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &vrConfig);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*
        1.4. Call cpssExMxPmIpLpmVirtualRouterConfigGet with the same lpmDbId,
                                                             vrId and non-NULL vrConfigPtr.
        Expected: GT_OK and the same vrConfigPtr.
    */
    st = cpssExMxPmIpLpmVirtualRouterConfigGet(lpmDbId, vrId, &vrConfigGet);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmVirtualRouterConfigGet: %d, %d", lpmDbId, vrId);

    /* Verifying values */
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.supportUcIpv4, vrConfigGet.supportUcIpv4,
               "get another vrConfigPtr->supportUcIpv4 than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr,
                                 vrConfigGet.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr,
               "get another vrConfigPtr->defaultUcIpv4RouteEntry.routeEntryBaseMemAddr than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultUcIpv4RouteEntry.blockSize,
                                 vrConfigGet.defaultUcIpv4RouteEntry.blockSize,
               "get another vrConfigPtr->defaultUcIpv4RouteEntry.blockSize than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod,
                                 vrConfigGet.defaultUcIpv4RouteEntry.routeEntryMethod,
               "get another vrConfigPtr->defaultUcIpv4RouteEntry.routeEntryMethod than was set: %d", lpmDbId);

    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.supportMcIpv4, vrConfigGet.supportMcIpv4,
               "get another vrConfigPtr->supportMcIpv4 than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr,
                                 vrConfigGet.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr,
               "get another vrConfigPtr->defaultMcIpv4RouteEntry.routeEntryBaseMemAddr than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultMcIpv4RouteEntry.blockSize,
                                 vrConfigGet.defaultMcIpv4RouteEntry.blockSize,
               "get another vrConfigPtr->defaultMcIpv4RouteEntry.blockSize than was set: %d", lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod,
                                 vrConfigGet.defaultMcIpv4RouteEntry.routeEntryMethod,
               "get another vrConfigPtr->defaultMcIpv4RouteEntry.routeEntryMethod than was set: %d", lpmDbId);

    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.supportUcIpv6, vrConfigGet.supportUcIpv6,
               "get another vrConfigPtr->supportUcIpv6 than was set: %d", lpmDbId);

    UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig.supportMcIpv6, vrConfigGet.supportMcIpv6,
               "get another vrConfigPtr->supportMcIpv6 than was set: %d", lpmDbId);

    /*
        1.5. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.3.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &vrConfig);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.6. Call with lpmDbId [1],
                       vrId[0] (already exist)
                       and other parameters from 1.3.
        Expected: NOT GT_OK.
    */
    vrId = 0;

    st = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &vrConfig);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*
        1.7. Call with lpmDbId [2],
                       vrId[2], vrConfigPtr->defaultUcIpv4RouteEntry.blockSize [0] (for regular should be 1),
                       vrConfigPtr->defaultUcIpv4RouteEntry.routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]
                       and other parameters from 1.2.
        Expected: NOT GT_OK.
    */
    lpmDbId = 2;
    vrId    = 2;

    vrConfig.defaultUcIpv4RouteEntry.blockSize        = 0;
    vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    st = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &vrConfig);
    UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
               "%d, ->defaultUcIpv4RouteEntry.blockSize = %d, defaultUcIpv4RouteEntry.routeEntryMethod = %d",
               lpmDbId, vrConfig.defaultUcIpv4RouteEntry.blockSize, vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod);

    /*
        1.8. Call with lpmDbId [2],
                       vrId[2],
                       out of range vrConfigPtr->defaultUcIpv4RouteEntry.routeEntryMethod [0x5AAAAAA5]
                       and other parameters from 1.2.
        Expected: GT_BAD_PARAM.
    */
    vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod = IP_LPM_INVALID_ENUM_CNS;

    st = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &vrConfig);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->defaultUcIpv4RouteEntry.routeEntryMethod = %d",
                                 lpmDbId, vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod);

    vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;

    /*
        1.9. Call with lpmDbId [2],
                       vrId[2],
                       out of range vrConfigPtr->defaultMcIpv4RouteEntry.routeEntryMethod [0x5AAAAAA5]
                       and other parameters from 1.2.
        Expected: GT_BAD_PARAM.
    */
    vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod = IP_LPM_INVALID_ENUM_CNS;

    st = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &vrConfig);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->defaultMcIpv4RouteEntry.routeEntryMethod = %d",
                                 lpmDbId, vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod);

    vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;

    /*
        1.10. Call with lpmDbId [2],
                        vrId[2],
                        out of range vrConfigPtr->efaultUcIpv6RouteEntry.routeEntryMethod [0x5AAAAAA5]
                        and other parameters from 1.2.
        Expected: GT_BAD_PARAM.
    */
    vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod = IP_LPM_INVALID_ENUM_CNS;

    st = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &vrConfig);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->defaultUcIpv6RouteEntry.routeEntryMethod = %d",
                                 lpmDbId, vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod);

    vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;

    /*
        1.11. Call with lpmDbId [2],
                        vrId[2]
                        and vrConfigPtr [NULL].
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vrConfigPtr = NULL", lpmDbId);

    /*
        1.12. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId [1 / 2]
                                                           and vrId [0 / 1] to delete virtual routers.
        Expected: GT_OK.
    */

    /* Call with lpmDbId [1] and vrId [0] */
    lpmDbId = 1;
    vrId    = 0;

    st = cpssExMxPmIpLpmVirtualRouterDelete(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,"cpssExMxPmIpLpmVirtualRouterDelete: %d, %d", lpmDbId, vrId);

    /* Call with lpmDbId [2] and vrId [0] */
    lpmDbId = 2;

    st = cpssExMxPmIpLpmVirtualRouterDelete(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,"cpssExMxPmIpLpmVirtualRouterDelete: %d, %d", lpmDbId, vrId);

    /* Call with lpmDbId [2] and vrId [1] */
    vrId = 1;

    st = cpssExMxPmIpLpmVirtualRouterDelete(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,"cpssExMxPmIpLpmVirtualRouterDelete: %d, %d", lpmDbId, vrId);

    /* Delete defauld LpmDB*/
    st = prvUtfDeleteDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteDefaultLpmDB");
}

/*******************************************************************************
* Router add, check, iterate, delete, check memory leakages
1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
Expected: GT_OK.
2. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_before_Router.
3. Call cpssExMxPmIpLpmVirtualRouterAdd to add router for every IpLpmDB with lpmDbId [1 .. 2] and vrId [0 .. 2].
Expected: GT_OK.
4. Call cpssExMxPmIpLpmVirtualRouterConfigGet to check router for every IpLpmDB with lpmDbId [1 .. 2] and vrId [0 .. 2].
Expected: GT_OK.
5. Call cpssExMxPmIpLpmVirtualRouterDelete to delete all routers for every IpLpmDB with lpmDbId [1 .. 2] and vrId [0 .. 2].
Expected: GT_OK.
6. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_after_Router.
Expected: (heap_bytes_allocated_after_Router-heap_bytes_allocated_before_Router) = 0
7. Delete defauld LpmDB [1 / 2]
Expected: GT_OK.
*******************************************************************************/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmVirtualRouterMemoryCheck)
{
    GT_STATUS   st        = GT_OK;
    GT_U32      iTemp     = 0;
    GT_U32      jTemp     = 0;
    GT_U32      ii        = 0;

    GT_U32      lpmDbId = 0;
    GT_U32      vrId    = 0;

    GT_U32      heap_bytes_allocated_before_Router = 0;
    GT_U32      heap_bytes_allocated_after_Router  = 0;

    CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC    vrConfig[3];
    CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC    vrConfigGet;

    cpssOsBzero((GT_VOID*) &vrConfig, sizeof(vrConfig));
    cpssOsBzero((GT_VOID*) &vrConfigGet, sizeof(vrConfigGet));

    /*
        1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
        Expected: GT_OK.
    */
    st = prvUtfCreateDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB");

    /*
        2. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_before_Router.
    */
    heap_bytes_allocated_before_Router = osMemGetHeapBytesAllocated();

    for(ii=0; ii<IP_LPM_ITER_COUNT; ++ii)
    {
        /*
            3. Call cpssExMxPmIpLpmVirtualRouterAdd to add router for every IpLpmDB
                      with lpmDbId [1 .. 2].
                           vrId [0 .. 2]
            Expected: GT_OK.
        */
        for(iTemp=IP_LPM_DEFAULT_DB_ID_CNS; iTemp<3; ++iTemp)
        {
            lpmDbId = iTemp;
            vrId    = 0;

            vrConfig[0].supportUcIpv4 = GT_TRUE;
            vrConfig[0].defaultUcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
            vrConfig[0].defaultUcIpv4RouteEntry.routeEntryBaseMemAddr = 0;
            vrConfig[0].defaultUcIpv4RouteEntry.blockSize = 1;

            vrConfig[0].supportMcIpv4 = GT_TRUE;
            vrConfig[0].defaultMcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
            vrConfig[0].defaultMcIpv4RouteEntry.routeEntryBaseMemAddr = 1;
            vrConfig[0].defaultMcIpv4RouteEntry.blockSize = 1;

            vrConfig[0].supportUcIpv6 = GT_TRUE;
            vrConfig[0].defaultUcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
            vrConfig[0].defaultUcIpv6RouteEntry.routeEntryBaseMemAddr = 2;
            vrConfig[0].defaultUcIpv6RouteEntry.blockSize = 1;

            vrConfig[0].supportMcIpv6 = GT_TRUE;
            vrConfig[0].defaultMcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
            vrConfig[0].defaultMcIpv6RouteEntry.routeEntryBaseMemAddr = 3;
            vrConfig[0].defaultMcIpv6RouteEntry.blockSize = 1;

            vrConfig[0].defaultMcIpv6RuleIndex = 2047;
            vrConfig[0].defaultMcIpv6PclId = 0;

            st = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &vrConfig[0]);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmVirtualRouterAdd: %d, %d", lpmDbId, vrId);

            vrId    = 1;

            vrConfig[1].supportUcIpv4 = GT_TRUE;
            vrConfig[1].defaultUcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
            vrConfig[1].defaultUcIpv4RouteEntry.routeEntryBaseMemAddr = 4;
            vrConfig[1].defaultUcIpv4RouteEntry.blockSize = 1;

            vrConfig[1].supportMcIpv4 = GT_TRUE;
            vrConfig[1].defaultMcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
            vrConfig[1].defaultMcIpv4RouteEntry.routeEntryBaseMemAddr = 5;
            vrConfig[1].defaultMcIpv4RouteEntry.blockSize = 1;

            vrConfig[1].supportUcIpv6 = GT_FALSE;
            vrConfig[1].defaultUcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
            vrConfig[1].defaultUcIpv6RouteEntry.routeEntryBaseMemAddr = 6;
            vrConfig[1].defaultUcIpv6RouteEntry.blockSize = 1;

            vrConfig[1].supportMcIpv6 = GT_FALSE;
            vrConfig[1].defaultMcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
            vrConfig[1].defaultMcIpv6RouteEntry.routeEntryBaseMemAddr = 7;
            vrConfig[1].defaultMcIpv6RouteEntry.blockSize = 1;

            vrConfig[1].defaultMcIpv6RuleIndex = 0;
            vrConfig[1].defaultMcIpv6PclId = 0;

            st = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &vrConfig[1]);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmVirtualRouterAdd: %d, %d", lpmDbId, vrId);

            vrId    = 2;

            vrConfig[2].supportUcIpv4 = GT_TRUE;
            vrConfig[2].defaultUcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
            vrConfig[2].defaultUcIpv4RouteEntry.routeEntryBaseMemAddr = 8;
            vrConfig[2].defaultUcIpv4RouteEntry.blockSize = 1;

            vrConfig[2].supportMcIpv4 = GT_TRUE;
            vrConfig[2].defaultMcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
            vrConfig[2].defaultMcIpv4RouteEntry.routeEntryBaseMemAddr = 9;
            vrConfig[2].defaultMcIpv4RouteEntry.blockSize = 1;

            vrConfig[2].supportUcIpv6 = GT_FALSE;
            vrConfig[2].defaultUcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
            vrConfig[2].defaultUcIpv6RouteEntry.routeEntryBaseMemAddr = 10;
            vrConfig[2].defaultUcIpv6RouteEntry.blockSize = 1;

            vrConfig[2].supportMcIpv6 = GT_FALSE;
            vrConfig[2].defaultMcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
            vrConfig[2].defaultMcIpv6RouteEntry.routeEntryBaseMemAddr = 11;
            vrConfig[2].defaultMcIpv6RouteEntry.blockSize = 1;

            vrConfig[2].defaultMcIpv6RuleIndex = 2047;
            vrConfig[2].defaultMcIpv6PclId = 0;

            st = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &vrConfig[2]);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmVirtualRouterAdd: %d, %d", lpmDbId, vrId);
        }

        /*
            4. Call cpssExMxPmIpLpmVirtualRouterConfigGet to check router for every IpLpmDB
                                                          with lpmDbId [1 .. 2].
                                                               vrId [0 .. 2]
                                                           and non-NULL vrConfigPtr.
            Expected: GT_OK and the same vrConfigPtr as was set.
        */
        for(iTemp=IP_LPM_DEFAULT_DB_ID_CNS; iTemp<3; ++iTemp)
            for(jTemp=0; jTemp<3; ++jTemp)
            {
                lpmDbId = iTemp;
                vrId    = jTemp;

                cpssOsBzero((GT_VOID*) &vrConfigGet, sizeof(vrConfigGet));

                st = cpssExMxPmIpLpmVirtualRouterConfigGet(lpmDbId, vrId, &vrConfigGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmVirtualRouterConfigGet: %d, %d", lpmDbId, vrId);

                if(st == GT_OK)
                {
                    /* Verifying values */
                    if(vrConfig[jTemp].supportUcIpv4)
                    {
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].supportUcIpv4, vrConfigGet.supportUcIpv4,
                                   "get another vrConfigPtr->supportUcIpv4 than was set: %d", lpmDbId);
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].defaultUcIpv4RouteEntry.routeEntryBaseMemAddr,
                                                     vrConfigGet.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr,
                                   "get another vrConfigPtr->defaultUcIpv4RouteEntry.routeEntryBaseMemAddr than was set: %d", lpmDbId);
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].defaultUcIpv4RouteEntry.blockSize,
                                                     vrConfigGet.defaultUcIpv4RouteEntry.blockSize,
                                   "get another vrConfigPtr->defaultUcIpv4RouteEntry.blockSize than was set: %d", lpmDbId);
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].defaultUcIpv4RouteEntry.routeEntryMethod,
                                                     vrConfigGet.defaultUcIpv4RouteEntry.routeEntryMethod,
                                   "get another vrConfigPtr->defaultUcIpv4RouteEntry.routeEntryMethod than was set: %d", lpmDbId);
                    }/* Uc IpV4 */

                    if(vrConfig[jTemp].supportMcIpv4)
                    {
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].supportMcIpv4, vrConfigGet.supportMcIpv4,
                                   "get another vrConfigPtr->supportMcIpv4 than was set: %d", lpmDbId);
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].defaultMcIpv4RouteEntry.routeEntryBaseMemAddr,
                                                     vrConfigGet.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr,
                                   "get another vrConfigPtr->defaultMcIpv4RouteEntry.routeEntryBaseMemAddr than was set: %d", lpmDbId);
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].defaultMcIpv4RouteEntry.blockSize,
                                                     vrConfigGet.defaultMcIpv4RouteEntry.blockSize,
                                   "get another vrConfigPtr->defaultMcIpv4RouteEntry.blockSize than was set: %d", lpmDbId);
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].defaultMcIpv4RouteEntry.routeEntryMethod,
                                                     vrConfigGet.defaultMcIpv4RouteEntry.routeEntryMethod,
                                   "get another vrConfigPtr->defaultMcIpv4RouteEntry.routeEntryMethod than was set: %d", lpmDbId);
                    }/* Mc IpV4 */

                    if(vrConfig[jTemp].supportUcIpv6)
                    {
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].supportUcIpv6, vrConfigGet.supportUcIpv6,
                                   "get another vrConfigPtr->supportUcIpv6 than was set: %d", lpmDbId);
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].defaultUcIpv6RouteEntry.routeEntryBaseMemAddr,
                                                     vrConfigGet.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr,
                                   "get another vrConfigPtr->defaultUcIpv6RouteEntry.routeEntryBaseMemAddr than was set: %d", lpmDbId);
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].defaultUcIpv6RouteEntry.blockSize,
                                                     vrConfigGet.defaultUcIpv6RouteEntry.blockSize,
                                   "get another vrConfigPtr->defaultUcIpv6RouteEntry.blockSize than was set: %d", lpmDbId);
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].defaultUcIpv6RouteEntry.routeEntryMethod,
                                                     vrConfigGet.defaultUcIpv6RouteEntry.routeEntryMethod,
                                   "get another vrConfigPtr->defaultUcIpv6RouteEntry.routeEntryMethod than was set: %d", lpmDbId);
                    }/* Uc IpV6 */

                    if(vrConfig[jTemp].supportMcIpv6)
                    {
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].supportMcIpv6, vrConfigGet.supportMcIpv6,
                                   "get another vrConfigPtr->supportMcIpv6 than was set: %d", lpmDbId);
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].defaultMcIpv6RouteEntry.routeEntryBaseMemAddr,
                                                     vrConfigGet.defaultMcIpv6RouteEntry.routeEntryBaseMemAddr,
                                   "get another vrConfigPtr->defaultMcIpv6RouteEntry.routeEntryBaseMemAddr than was set: %d", lpmDbId);
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].defaultMcIpv6RouteEntry.blockSize,
                                                     vrConfigGet.defaultMcIpv6RouteEntry.blockSize,
                                   "get another vrConfigPtr->defaultMcIpv6RouteEntry.blockSize than was set: %d", lpmDbId);
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].defaultMcIpv6RouteEntry.routeEntryMethod,
                                                     vrConfigGet.defaultMcIpv6RouteEntry.routeEntryMethod,
                                   "get another vrConfigPtr->defaultMcIpv6RouteEntry.routeEntryMethod than was set: %d", lpmDbId);

                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].defaultMcIpv6RuleIndex, vrConfigGet.defaultMcIpv6RuleIndex,
                                                     "get another vrConfig.defaultMcIpv6RuleIndex than was set: %d", lpmDbId);
                        UTF_VERIFY_EQUAL1_STRING_MAC(vrConfig[jTemp].defaultMcIpv6PclId, vrConfigGet.defaultMcIpv6PclId,
                                                     "get another vrConfig.defaultMcIpv6PclId than was set: %d", lpmDbId);
                    }/* Mc IpV6 */
                }/* st == GT_OK */
            }/* for */

        /*
            5. Call cpssExMxPmIpLpmVirtualRouterDelete to delete all routers for every IpLpmDB
                                                          with lpmDbId [1 .. 2].
                                                               vrId [0 .. 2]
            Expected: GT_OK.
        */
        for(iTemp=IP_LPM_DEFAULT_DB_ID_CNS; iTemp<3; ++iTemp)
            for(jTemp=0; jTemp<3; ++jTemp)
            {
                lpmDbId = iTemp;
                vrId    = jTemp;

                st = cpssExMxPmIpLpmVirtualRouterDelete(lpmDbId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmVirtualRouterDelete: %d, %d", lpmDbId, vrId);
            }

    }/* for */

    /*
        6. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_after_Router.
        Expected: (heap_bytes_allocated_after_Router - heap_bytes_allocated_before_Router) = 0
    */
    heap_bytes_allocated_after_Router = osMemGetHeapBytesAllocated();
    UTF_VERIFY_EQUAL0_STRING_MAC(heap_bytes_allocated_before_Router, heap_bytes_allocated_after_Router,
                                 "size of heap changed (heap_bytes_allocated)");
    /*
        7. Delete defauld LpmDB [1 / 2]
        Expected: GT_OK.
    */
    st = prvUtfDeleteDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteDefaultLpmDB");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmVirtualRouterDelete
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmVirtualRouterDelete)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                   vrId [0 / 0]
                   and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportMcIpv4 [GT_TRUE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportUcIpv6 [GT_TRUE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                    defaultMcIpv6RuleIndex [0 / 100],
                                    defaultMcIpv6PclId [0 / 100]}.
    Expected: GT_OK.
    1.3. Call with lpmDbId [1]
                   and vrId [0].
    Expected: GT_OK.
    1.4. Call with lpmDbId [1]
                   and vrId [0] (already deleted).
    Expected: NOT GT_OK.
    1.5. Call with lpmDbId [100] (not exist)
                   and vrId [0].
    Expected: NOT GT_OK.
    1.6. Call with lpmDbId [2]
                   and vrId [100] (not exist).
    Expected: NOT GT_OK.
    1.7. Call with lpmDbId [2]
                   and vrId [0].
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                    CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.

        1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                        defaultMcIpv6RuleIndex [0 / 100],
                                        defaultMcIpv6PclId [0 / 100]}.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        1.3. Call with lpmDbId [1]
                       and vrId [0].
        Expected: GT_OK.
    */
    lpmDbId = 1;
    vrId    = 0;

    st = cpssExMxPmIpLpmVirtualRouterDelete(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*
        1.4. Call with lpmDbId [1]
                       and vrId [0] (already deleted).
        Expected: NOT GT_OK.
    */
    st = cpssExMxPmIpLpmVirtualRouterDelete(lpmDbId, vrId);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*
        1.5. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.3.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmVirtualRouterDelete(lpmDbId, vrId);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.6. Call with lpmDbId [2]
                       and vrId [100] (not exist).
        Expected: NOT GT_OK.
    */
    lpmDbId = 2;
    vrId    = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmVirtualRouterDelete(lpmDbId, vrId);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    vrId = 0;

    /*
        1.7. Call with lpmDbId [2]
                       and vrId [0].
        Expected: GT_OK.
    */
    st = cpssExMxPmIpLpmVirtualRouterDelete(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /* delete all VR added to the default LPM DB before delete it */
    st = cpssExMxPmIpLpmVirtualRouterDelete(IP_LPM_DEFAULT_DB_ID_CNS, 1);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /* Delete defauld LpmDB*/
    st = prvUtfDeleteDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteDefaultLpmDB");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmVirtualRouterConfigGet
(
    IN  GT_U32                              lpmDbId,
    IN  GT_U32                              vrId,
    OUT CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC    *vrConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmVirtualRouterConfigGet)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                   vrId [0 / 0]
                   and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportMcIpv4 [GT_TRUE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportUcIpv6 [GT_TRUE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                    defaultMcIpv6RuleIndex [0 / 100],
                                    defaultMcIpv6PclId [0 / 100]}.
    Expected: GT_OK.
    1.3. Call with lpmDbId [1],
                   vrId [0]
                   and non-NULL vrConfigPtr.
    Expected: GT_OK.
    1.4. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.5. Call with vrId [100] (not exist)
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.6. Call with vrConfigPtr [NULL]
                   and other parameters from 1.3.
    Expected: GT_BAD_PTR.
    1.7. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId [1]
                                                      and vrId [0] to delete virtual routers.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC    vrConfig;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                    CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.

        1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                        defaultMcIpv6RuleIndex [0 / 100],
                                        defaultMcIpv6PclId [0 / 100]}.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        1.3. Call with lpmDbId [1],
                       vrId [0]
                       and non-NULL vrConfigPtr.
        Expected: GT_OK.
    */
    lpmDbId = 1;
    vrId    = 0;

    st = cpssExMxPmIpLpmVirtualRouterConfigGet(lpmDbId, vrId, &vrConfig);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*
        1.4. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.3.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmVirtualRouterConfigGet(lpmDbId, vrId, &vrConfig);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.5. Call with vrId [100] (not exist)
                       and other parameters from 1.3.
        Expected: NOT GT_OK.
    */
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmVirtualRouterConfigGet(lpmDbId, vrId, &vrConfig);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    vrId = 0;

    /*
        1.6. Call with vrConfigPtr [NULL]
                       and other parameters from 1.3.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmVirtualRouterConfigGet(lpmDbId, vrId, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vrConfigPtr = NULL", lpmDbId);

    /*
        1.7. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId [1]
                                                          and vrId [0] to delete virtual routers.
        Expected: GT_OK.
    */
    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv4UcPrefixAdd
(
    IN  GT_U32                                  lpmDbId,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    IN  GT_U32                                  prefixLen,
    IN  CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  *nextHopPointer,
    IN  GT_BOOL                                 override
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4UcPrefixAdd)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.

    Expected: GT_OK.

    1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                   vrId [0 / 0]
                   and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportMcIpv4 [GT_TRUE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportUcIpv6 [GT_TRUE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                    defaultMcIpv6RuleIndex [0 / 100],
                                    defaultMcIpv6PclId [0 / 100]}.

    Expected: GT_OK.

    1.3. Call with lpmDbId [1],
                   vrId [0],
                   ipAddr.arIP [10.16.0.0 /10.17.0.1 - 254],
                   prefixLen [8 / 32],
                   nextHopPointer {routeEntryBaseMemAddr [0],
                                   blockSize [1],
                                   routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                   and override [GT_FALSE / GT_TRUE].
    Iterate with 254 IP Addreses for 10.17.0.1.
    Expected: GT_OK.

    1.4. Call cpssExMxPmIpLpmIpv4UcPrefixGet with the same lpmDbId,
                                                  vrId,
                                                  ipAddr,
                                                  prefixLen
                                                  and non-NULL nextHopPointerPtr.
    Iterate with 254 IP Addreses for 10.17.0.1.
    Expected: GT_OK and the same nextHopPointer.

    1.5. Call with lpmDbId [1],
                   vrId [0],
                   ipAddr.arIP [10.16.0.0],
                   prefixLen [8],
                   nextHopPointer {routeEntryBaseMemAddr [0],
                                   blockSize [1],
                                   routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                   and override [GT_FALSE].
    Expected: GT_ALREADY_EXIST.

    1.6. Call with lpmDbId [2],
                   vrId [0],
                   ipAddr.arIP [192.168.1.0],
                   prefixLen [24],
                   nextHopPointer {routeEntryBaseMemAddr [0],
                                   blockSize [1],
                                   routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                   and override [GT_TRUE].
    Expected: GT_NOT_FOUND.


    1.7. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.3.
    Expected: NOT GT_OK.

    1.8. Call with lpmDbId [2],
                   vrId [100] (not exist)
                   and other parameters from 1.3.
    Expected: NOT GT_OK.

    1.9. Call with lpmDbId [2],
                   vrId [0],
                   out of range nextHopPointer->routeEntryMethod[0x5AAAAAA5]
                   and other parameters from 1.3.
    Expected: NOT GT_OK.

    1.10. Call with lpmDbId [2],
                   vrId [0],
                   nextHopPointer [NULL]
                   and other parameters from 1.3.
    Expected: GT_BAD_PTR.

    1.11. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush with lpmDbId [1],
                                                      vrId [0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPADDR                               ipAddr;
    GT_U32                                  prefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointerGet;
    GT_BOOL                                 override  = GT_FALSE;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                    CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.

        1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                        defaultMcIpv6RuleIndex [0 / 100],
                                        defaultMcIpv6PclId [0 / 100]}.
        Expected: GT_OK.
    */
        st = prvUtfCreateLpmDBAndVirtualRouterAdd();
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        1.3. Call with lpmDbId [1],
                       vrId [0],
                       ipAddr.arIP [10.16.0.0],
                       prefixLen [8],
                       nextHopPointer {routeEntryBaseMemAddr [0],
                                       blockSize [1],
                                       routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                       and override [GT_FALSE / GT_TRUE].
        Expected: GT_OK.
    */

        /* Call with override [GT_FALSE] */
        lpmDbId = 1;
        vrId    = 0;

        ipAddr.arIP[0] = 10;
        ipAddr.arIP[1] = 16;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 0;

        prefixLen = 8;
        override  = GT_FALSE;

        nextHopPointer.routeEntryBaseMemAddr = 0;
        nextHopPointer.blockSize             = 1;
        nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

                st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d, override = %d",
                                             lpmDbId, vrId, prefixLen, override);
    /*

        1.4. Call cpssExMxPmIpLpmIpv4UcPrefixGet with the same lpmDbId,
                                                      vrId,
                                                      ipAddr,
                                                      prefixLen
                                                      and non-NULL nextHopPointerPtr.
        Expected: GT_OK and the same nextHopPointer.
    */
                st = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointerGet);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                           "cpssExMxPmIpLpmIpv4UcPrefixGet: %d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryBaseMemAddr, nextHopPointerGet.routeEntryBaseMemAddr,
                           "get another nextHopPointerPtr->routeEntryBaseMemAddr than was set: %d, %d", lpmDbId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.blockSize, nextHopPointerGet.blockSize,
                           "get another nextHopPointerPtr->blockSize than was set: %d, %d", lpmDbId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryMethod, nextHopPointerGet.routeEntryMethod,
                           "get another nextHopPointerPtr->routeEntryMethod than was set: %d, %d", lpmDbId, vrId);

                /* Call with override [GT_TRUE] */
                override = GT_TRUE;

                st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d, override = %d",
                                             lpmDbId, vrId, prefixLen, override);

    /*
        1.3. Call with lpmDbId [1],
                       vrId [0],
                       ipAddr.arIP [10.17.0.1 - 254],
                       prefixLen [32],
                       nextHopPointer {routeEntryBaseMemAddr [0],
                                       blockSize [1],
                                       routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                       and override [GT_FALSE / GT_TRUE].
        Iterate with 254 IP Addreses.
        Expected: GT_OK.
    */

    /* Call with override [GT_FALSE] */
    ipAddr.arIP[1] = 17;
    prefixLen = 32;

    /*     Iterate with 254 IP Addreses 10.17.0.1 - 10.17.0.254 */

        for (ipAddr.arIP[3] = 1; ipAddr.arIP[3] < 255; ipAddr.arIP[3]++)
            {
                st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d, override = %d",
                                             lpmDbId, vrId, prefixLen, override);

                /*
                    1.4. Call cpssExMxPmIpLpmIpv4UcPrefixGet with the same lpmDbId,
                                                                  vrId,
                                                                  ipAddr,
                                                                  prefixLen
                                                                  and non-NULL nextHopPointerPtr.
                    Iterate with 254 IP Addreses.
                    Expected: GT_OK and the same nextHopPointer.
                */

                st = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointerGet);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                           "cpssExMxPmIpLpmIpv4UcPrefixGet: %d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryBaseMemAddr, nextHopPointerGet.routeEntryBaseMemAddr,
                           "get another nextHopPointerPtr->routeEntryBaseMemAddr than was set: %d, %d", lpmDbId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.blockSize, nextHopPointerGet.blockSize,
                           "get another nextHopPointerPtr->blockSize than was set: %d, %d", lpmDbId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryMethod, nextHopPointerGet.routeEntryMethod,
                           "get another nextHopPointerPtr->routeEntryMethod than was set: %d, %d", lpmDbId, vrId);

                /* Call with override [GT_TRUE] */
                override = GT_TRUE;

                st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d, override = %d",
                                             lpmDbId, vrId, prefixLen, override);
             }
    /*
        1.5. Call with lpmDbId [1],
                       vrId [0],
                       ipAddr.arIP [10.16.0.0],
                       prefixLen [8],
                       nextHopPointer {routeEntryBaseMemAddr [0],
                                       blockSize [1],
                                       routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                       and override [GT_FALSE].
        Expected: GT_ALREADY_EXIST.
    */
    /* Call with override [GT_FALSE] */
    lpmDbId = 1;
    vrId    = 0;

    ipAddr.arIP[0] = 10;
    ipAddr.arIP[1] = 16;
    ipAddr.arIP[2] = 0;
    ipAddr.arIP[3] = 0;

    prefixLen = 8;
    override  = GT_FALSE;

    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

            st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_ALREADY_EXIST, st, "%d, %d, prefixLen = %d, override = %d",
                                         lpmDbId, vrId, prefixLen, override);

    /*
        1.6. Call with lpmDbId [1],
                       vrId [0],
                       ipAddr.arIP [192.168.1.0],
                       prefixLen [24],
                       nextHopPointer {routeEntryBaseMemAddr [0],
                                       blockSize [1],
                                       routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                       and override [GT_TRUE].
        Expected: GT_NOT_FOUND.
    */
    lpmDbId = 2;
    vrId    = 0;

    ipAddr.arIP[0] = 192;
    ipAddr.arIP[1] = 168;
    ipAddr.arIP[2] = 1;
    ipAddr.arIP[3] = 0;

    prefixLen = 24;
    override  = GT_TRUE;

    nextHopPointer.routeEntryBaseMemAddr = 0xFF;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

            st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_NOT_FOUND, st, "%d, %d, prefixLen = %d, override = %d",
                                         lpmDbId, vrId, prefixLen, override);

    /*
        1.7. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.3.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 0;

    /*
        1.8. Call with lpmDbId [2],
                       vrId [100] (not exist)
                       and other parameters from 1.3.
        Expected: NOT GT_OK.
    */
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    vrId = 0;

    /*
        1.9. Call with lpmDbId [2],
                       vrId [0],
                       out of range nextHopPointer->routeEntryMethod[0x5AAAAAA5]
                       and other parameters from 1.3.
        Expected: NOT GT_OK.
    */
    nextHopPointer.routeEntryMethod = IP_LPM_INVALID_ENUM_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, nextHopPointer->routeEntryMethod = %d",
                                     lpmDbId, vrId, nextHopPointer.routeEntryMethod);

    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    /*
        1.10. Call with lpmDbId [2],
                       vrId [0],
                       nextHopPointer [NULL]
                       and other parameters from 1.3.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, NULL, override);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nextHopPointer = NULL", lpmDbId, vrId);

    /*
        1.11. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush with lpmDbId [1],
                                                          vrId [0] to invalidate changes.
        Expected: GT_OK.
    */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d", lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}


/*******************************************************************************
* Prefix IpV4 add, check, delete, check memory leakages
1. Create defauld LpmDB and Virtual Routers.
Expected: GT_OK.
2. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_before_IPV4_Prefix.
3. Call cpssExMxPmIpLpmIpv4UcPrefixAdd to fill table of prefixes
Expected: GT_OK.
4. Call cpssExMxPmIpLpmIpv4UcPrefixGet to check prefixes
Expected: GT_OK.
5. Call cpssExMxPmIpLpmIpv4UcPrefixDelete to delete all prefixes
Expected: GT_OK.
6. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_IPV4_Prefix.
Expected: (heap_bytes_allocated_after_IPV4_Prefix - heap_bytes_allocated_before_IPV4_Prefix) = 0
7. Delete defauld LpmDB and Virtual Routers
Expected: GT_OK.
*******************************************************************************/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4UcPrefixMemoryCheck)
{
    GT_STATUS   st        = GT_OK;

    GT_U32      lpmDbId = 0;
    GT_U32      vrId    = 0;
    GT_U32      ii      = 0;

    GT_U32      heap_bytes_allocated_before_IPV4_Prefix = 0;
    GT_U32      heap_bytes_allocated_after_IPV4_Prefix = 0;

    GT_BOOL     override  = GT_FALSE;
    GT_U32      prefixLen = 0;

    GT_IPADDR ipAddr;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointerGet;

    cpssOsBzero((GT_VOID*) &ipAddr, sizeof(ipAddr));
    cpssOsBzero((GT_VOID*) &nextHopPointer, sizeof(nextHopPointer));
    cpssOsBzero((GT_VOID*) &nextHopPointerGet, sizeof(nextHopPointerGet));

    /*
        1. Create defauld LpmDB and Virtual Routers.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        2. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_before_IPV4_Prefix.
    */
    heap_bytes_allocated_before_IPV4_Prefix = osMemGetHeapBytesAllocated();

    for(ii=0; ii<IP_LPM_ITER_COUNT; ++ii)
    {
        /*
            3. Call cpssExMxPmIpLpmIpv4UcPrefixAdd to fill table of prefixes
                      with lpmDbId [1 .. 2].
                           vrId [0 .. 2]
                           ipAddr.arIP [10.17.0.1],
                           prefixLen [32],
                           nextHopPointer {routeEntryBaseMemAddr [0],
                                           blockSize [1],
                                           routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                           and override [GT_FALSE].
            Expected: GT_OK.
        */
        ipAddr.arIP[0] = 10;
        ipAddr.arIP[1] = 17;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 1;

        prefixLen = 32;
        override  = GT_FALSE;

        nextHopPointer.routeEntryBaseMemAddr = 0;
        nextHopPointer.blockSize             = 1;
        nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

        /* iterate with lpmDbId [1], vrId[0] */
        lpmDbId = 1;
        vrId    = 0;

        /* make every entry unique */
        ipAddr.arIP[3] = 1;

        nextHopPointer.routeEntryBaseMemAddr = 1;
        nextHopPointer.blockSize             = 1;

        st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixAdd: %d, %d, prefixLen = %d, override = %d",
                                     lpmDbId, vrId, prefixLen, override);

        /* iterate with lpmDbId [1], vrId[1] */
        lpmDbId = 1;
        vrId    = 1;

        /* make every entry unique */
        ipAddr.arIP[3] = 2;

        nextHopPointer.routeEntryBaseMemAddr = 2;
        nextHopPointer.blockSize             = 2;

        st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixAdd: %d, %d, prefixLen = %d, override = %d",
                                     lpmDbId, vrId, prefixLen, override);

        /* iterate with lpmDbId [2], vrId[0] */
        lpmDbId = 2;
        vrId    = 0;

        /* make every entry unique */
        ipAddr.arIP[3] = 3;

        nextHopPointer.routeEntryBaseMemAddr = 3;
        nextHopPointer.blockSize             = 3;

        st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixAdd: %d, %d, prefixLen = %d, override = %d",
                                     lpmDbId, vrId, prefixLen, override);

        /*
            4. Call cpssExMxPmIpLpmIpv4UcPrefixGet to check prefixes
                                                  with lpmDbId [1 .. 2].
                                                       vrId [0 .. 2]
                                                       ipAddr, prefixLen
                                                   and non-NULL nextHopPointerPtr.
            Expected: GT_OK and the same nextHopPointer as was set.
        */
        /* iterate with lpmDbId [1], vrId[0] */
        lpmDbId = 1;
        vrId    = 0;

        /* restore unique entry before compare */
        ipAddr.arIP[3] = 1;

        nextHopPointer.routeEntryBaseMemAddr = 1;
        nextHopPointer.blockSize             = 1;

        st = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointerGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixGet: %d, %d, prefixLen = %d",
                                     lpmDbId, vrId, prefixLen);

        if(st == GT_OK)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryBaseMemAddr, nextHopPointerGet.routeEntryBaseMemAddr,
                       "get another nextHopPointerPtr->routeEntryBaseMemAddr than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.blockSize, nextHopPointerGet.blockSize,
                       "get another nextHopPointerPtr->blockSize than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryMethod, nextHopPointerGet.routeEntryMethod,
                       "get another nextHopPointerPtr->routeEntryMethod than was set: %d, %d", lpmDbId, vrId);
        }

        /* iterate with lpmDbId [1], vrId[1] */
        lpmDbId = 1;
        vrId    = 1;

        /* restore unique entry before compare */
        ipAddr.arIP[3] = 2;

        nextHopPointer.routeEntryBaseMemAddr = 2;
        nextHopPointer.blockSize             = 2;

        st = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointerGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixGet: %d, %d, prefixLen = %d",
                                     lpmDbId, vrId, prefixLen);

        if(st == GT_OK)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryBaseMemAddr, nextHopPointerGet.routeEntryBaseMemAddr,
                       "get another nextHopPointerPtr->routeEntryBaseMemAddr than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.blockSize, nextHopPointerGet.blockSize,
                       "get another nextHopPointerPtr->blockSize than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryMethod, nextHopPointerGet.routeEntryMethod,
                       "get another nextHopPointerPtr->routeEntryMethod than was set: %d, %d", lpmDbId, vrId);
        }

        /* iterate with lpmDbId [2], vrId[0] */
        lpmDbId = 2;
        vrId    = 0;

        /* restore unique entry before compare */
        ipAddr.arIP[3] = 3;

        nextHopPointer.routeEntryBaseMemAddr = 3;
        nextHopPointer.blockSize             = 3;

        st = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointerGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixGet: %d, %d, prefixLen = %d",
                                     lpmDbId, vrId, prefixLen);

        if(st == GT_OK)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryBaseMemAddr, nextHopPointerGet.routeEntryBaseMemAddr,
                       "get another nextHopPointerPtr->routeEntryBaseMemAddr than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.blockSize, nextHopPointerGet.blockSize,
                       "get another nextHopPointerPtr->blockSize than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryMethod, nextHopPointerGet.routeEntryMethod,
                       "get another nextHopPointerPtr->routeEntryMethod than was set: %d, %d", lpmDbId, vrId);
        }

        /*
            5. Call cpssExMxPmIpLpmIpv4UcPrefixDelete to delete all prefixes
                                                  with lpmDbId [1 .. 2].
                                                       vrId [0 .. 2]
                                                       ipAddr, prefixLen.
            Expected: GT_OK.
        */
        /* iterate with lpmDbId [1], vrId[0] */
        lpmDbId = 1;
        vrId    = 0;

        ipAddr.arIP[3] = 1;

        st = cpssExMxPmIpLpmIpv4UcPrefixDelete(lpmDbId, vrId, ipAddr, prefixLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,"cpssExMxPmIpLpmIpv4UcPrefixDelete: %d, %d, prefixLen = %d",
                                     lpmDbId, vrId, prefixLen);

        /* iterate with lpmDbId [1], vrId[1] */
        lpmDbId = 1;
        vrId    = 1;

        ipAddr.arIP[3] = 2;

        st = cpssExMxPmIpLpmIpv4UcPrefixDelete(lpmDbId, vrId, ipAddr, prefixLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,"cpssExMxPmIpLpmIpv4UcPrefixDelete: %d, %d, prefixLen = %d",
                                     lpmDbId, vrId, prefixLen);

        /* iterate with lpmDbId [2], vrId[0] */
        lpmDbId = 2;
        vrId    = 0;

        ipAddr.arIP[3] = 3;

        st = cpssExMxPmIpLpmIpv4UcPrefixDelete(lpmDbId, vrId, ipAddr, prefixLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,"cpssExMxPmIpLpmIpv4UcPrefixDelete: %d, %d, prefixLen = %d",
                                     lpmDbId, vrId, prefixLen);
    }/* for */

    /*
        6. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_after_Router.
        Expected: (heap_bytes_allocated_after_IPV4_Prefix - heap_bytes_allocated_before_IPV4_Prefix) = 0
    */
    heap_bytes_allocated_after_IPV4_Prefix = osMemGetHeapBytesAllocated();
    UTF_VERIFY_EQUAL0_STRING_MAC(heap_bytes_allocated_before_IPV4_Prefix, heap_bytes_allocated_after_IPV4_Prefix,
                                 "size of heap changed (heap_bytes_allocated)");

    /*
        7. Delete defauld LpmDB and Virtual Routers.
        Expected: GT_OK.
    */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv4UcPrefixBulkAdd
(
    IN  GT_U32                                      lpmDbId,
    IN  GT_U32                                      ipv4PrefixArrayLen,
    IN  CPSS_EXMXPM_IP_LPM_IPV4_UC_ADD_PREFIX_STC   *ipv4PrefixArrayPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4UcPrefixBulkAdd)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                   vrId [0 / 0]
                   and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportMcIpv4 [GT_TRUE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportUcIpv6 [GT_TRUE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                    defaultMcIpv6RuleIndex [0 / 100],
                                    defaultMcIpv6PclId [0 / 100]}.
    Expected: GT_OK.
    1.3. Call with lpmDbId [1],
                   ipv4PrefixArrayLen [2],
                   ipv4PrefixArrayPtr[0, 1] {vrId [0 / 1],
                                             ipAddr[10.17.0.1 / 10.17.0.2],
                                             prefixLen [32 / 32],
                                             nextHopPointer {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_COS_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                             override [GT_FALSE],
                                             returnStatus [GT_OK] }.
    Expected: GT_OK.
    1.4. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.5. Call with ipv4PrefixArrayPtr[0]->vrId [100] (not exist)
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.6. Call with out of range ipv4PrefixArrayPtr[0]->nextHopPointer->routeEntryMethod[0x5AAAAAA5]
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.7. Call with ipv4PrefixArrayPtr [NULL]
                   and other parameters from 1.3.
    Expected: GT_BAD_PTR.
    1.8. Call cpssExMxPmIpLpmIpv4UcPrefixBulkDelete with the same lpmDbId,
                                                         ipv4PrefixArrayLen
                                                         and ipv4PrefixArrayPtr to delete added prefixes.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_U32                                      ipv4PrefixArrayLen = 0;
    CPSS_EXMXPM_IP_LPM_IPV4_UC_ADD_PREFIX_STC   ipv4PrefixArray[2];


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                    CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.

        1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                        defaultMcIpv6RuleIndex [0 / 100],
                                        defaultMcIpv6PclId [0 / 100]}.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        1.3. Call with lpmDbId [1],
                       ipv4PrefixArrayLen [2],
                       ipv4PrefixArrayPtr[0, 1] {vrId [0 / 0],
                                                 ipAddr[10.17.0.1 / 10.17.0.2],
                                                 prefixLen [32 / 32],
                                                 nextHopPointer {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_COS_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                                 override [GT_FALSE],
                                                 returnStatus [GT_OK] }.
        Expected: GT_OK.
    */
    lpmDbId            = 1;
    ipv4PrefixArrayLen = 2;

    ipv4PrefixArray[0].vrId = 0;

    ipv4PrefixArray[0].ipAddr.arIP[0] = 10;
    ipv4PrefixArray[0].ipAddr.arIP[1] = 17;
    ipv4PrefixArray[0].ipAddr.arIP[2] = 0;
    ipv4PrefixArray[0].ipAddr.arIP[3] = 1;

    ipv4PrefixArray[0].prefixLen = 32;

    ipv4PrefixArray[0].nextHopPointer.routeEntryBaseMemAddr = 0;
    ipv4PrefixArray[0].nextHopPointer.blockSize             = 1;
    ipv4PrefixArray[0].nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    ipv4PrefixArray[0].override     = GT_FALSE;
    ipv4PrefixArray[0].returnStatus = GT_OK;

    ipv4PrefixArray[1].vrId = 1;

    ipv4PrefixArray[1].ipAddr.arIP[0] = 10;
    ipv4PrefixArray[1].ipAddr.arIP[1] = 17;
    ipv4PrefixArray[1].ipAddr.arIP[2] = 0;
    ipv4PrefixArray[1].ipAddr.arIP[3] = 2;

    ipv4PrefixArray[1].prefixLen = 32;

    ipv4PrefixArray[1].nextHopPointer.routeEntryBaseMemAddr = 1;
    ipv4PrefixArray[1].nextHopPointer.blockSize             = 1;
    ipv4PrefixArray[1].nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    ipv4PrefixArray[1].override     = GT_FALSE;
    ipv4PrefixArray[1].returnStatus = GT_OK;

    st = cpssExMxPmIpLpmIpv4UcPrefixBulkAdd(lpmDbId, ipv4PrefixArrayLen, ipv4PrefixArray);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, ipv4PrefixArrayLen);

    /*
        1.4. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.3.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixBulkAdd(lpmDbId, ipv4PrefixArrayLen, ipv4PrefixArray);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.5. Call with ipv4PrefixArrayPtr[0]->vrId [100] (not exist)
                       and other parameters from 1.3.
        Expected: NOT GT_OK.
    */
    ipv4PrefixArray[0].vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixBulkAdd(lpmDbId, ipv4PrefixArrayLen, ipv4PrefixArray);
    UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipv4PrefixArrayPtr[0]->vrId = %d",
                                     lpmDbId, ipv4PrefixArray[0].vrId);

    ipv4PrefixArray[0].vrId = IP_LPM_INVALID_VR_ID_CNS;

    /*
        1.6. Call with out of range ipv4PrefixArrayPtr[0]->nextHopPointer->routeEntryMethod[0x5AAAAAA5]
                       and other parameters from 1.3.
        Expected: NOT GT_OK.
    */
    ipv4PrefixArray[0].nextHopPointer.routeEntryMethod = IP_LPM_INVALID_ENUM_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixBulkAdd(lpmDbId, ipv4PrefixArrayLen, ipv4PrefixArray);
    UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipv4PrefixArrayPtr[0]->nextHopPointer.routeEntryMethod = %d",
                                     lpmDbId, ipv4PrefixArray[0].nextHopPointer.routeEntryMethod);

    ipv4PrefixArray[0].nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    /*
        1.7. Call with ipv4PrefixArrayPtr [NULL]
                       and other parameters from 1.3.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv4UcPrefixBulkAdd(lpmDbId, ipv4PrefixArrayLen, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipv4PrefixArrayPtr  = NULL", lpmDbId);


    /* Delete Prefixes */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d", lpmDbId, vrId);

    vrId = IP_LPM_DEFAULT_VR_ID_CNS+1;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d", lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv4UcPrefixDelete
(
    IN  GT_U32              lpmDbId,
    IN  GT_U32              vrId,
    IN  GT_IPADDR           ipAddr,
    IN  GT_U32              prefixLen
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4UcPrefixDelete)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.

    1.2. Call with cpssExMxPmIpLpmIpv4UcPrefixAdd lpmDbId [1],
                   vrId [0],
                   ipAddr.arIP [10.17.0.1],
                   prefixLen [32],
                   nextHopPointer {routeEntryBaseMemAddr [0],
                                   blockSize [1],
                                   routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                   and override [GT_FALSE].
    Expected: NOT GT_OK (VR not created yet).

    1.3. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                   vrId [0 / 0]
                   and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportMcIpv4 [GT_TRUE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportUcIpv6 [GT_TRUE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                    defaultMcIpv6RuleIndex [0 / 100],
                                    defaultMcIpv6PclId [0 / 100]}.
    Expected: GT_OK.
    1.4. Call with cpssExMxPmIpLpmIpv4UcPrefixAdd lpmDbId [1],
                   vrId [0],
                   ipAddr.arIP [10.17.0.1],
                   prefixLen [32],
                   nextHopPointer {routeEntryBaseMemAddr [0],
                                   blockSize [1],
                                   routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                   and override [GT_FALSE].
    Expected: GT_OK.
    1.5. Call with lpmDbId [1],
                   vrId [0],
                   ipAddr.arIP [10.17.0.1]
                   and prefixLen [32].
    Expected: GT_OK.
    1.6. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.7. Call with lpmDbId [1],
                   vrId [100] (not exist)
                   and other parameters from 1.5.
    Expected: NOT GT_OK.
    1.8. Call with lpmDbId [1],
               vrId [0],
               ipAddr.arIP [192.168.0.1]
               and prefixLen [32].
    Expected: NOT GT_OK.

*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPADDR                               ipAddr;
    GT_U32                                  prefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                                 override  = GT_FALSE;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                    CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.
    */
    /*
        1.2. Call with lpmDbId [1],
                       vrId [0],
                       ipAddr.arIP [10.17.0.1]
                       and prefixLen [32].
        Expected: NOT GT_OK (VR not created yet).
    */

        lpmDbId = 1;
        vrId    = 0;

        ipAddr.arIP[0] = 10;
        ipAddr.arIP[1] = 17;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 1;

        prefixLen = 32;
        st = cpssExMxPmIpLpmIpv4UcPrefixDelete(lpmDbId, vrId, ipAddr, prefixLen);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    /*
        1.3. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                        defaultMcIpv6RuleIndex [0 / 100],
                                        defaultMcIpv6PclId [0 / 100]}.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        1.4. Call with cpssExMxPmIpLpmIpv4UcPrefixAdd lpmDbId [1],
                       vrId [0],
                       ipAddr.arIP [10.17.0.1],
                       prefixLen [32],
                       nextHopPointer {routeEntryBaseMemAddr [0],
                                       blockSize [1],
                                       routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                       and override [GT_FALSE].
        Expected: GT_OK.
    */
    lpmDbId = 1;
    vrId    = 0;

    ipAddr.arIP[0] = 10;
    ipAddr.arIP[1] = 17;
    ipAddr.arIP[2] = 0;
    ipAddr.arIP[3] = 1;

    prefixLen = 32;
    override  = GT_FALSE;

    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4UcPrefixAdd: %d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    /*
        1.5. Call with lpmDbId [1],
                       vrId [0],
                       ipAddr.arIP [10.17.0.1]
                       and prefixLen [32].
        Expected: GT_OK.
    */
    st = cpssExMxPmIpLpmIpv4UcPrefixDelete(lpmDbId, vrId, ipAddr, prefixLen);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,"%d, %d, prefixLen = %d",
                                 lpmDbId, vrId, prefixLen);

    /*
        1.6. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.4.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixDelete(lpmDbId, vrId, ipAddr, prefixLen);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.7. Call with lpmDbId [1], vrId [100] (not exist)
                       and other parameters from 1.4.
        Expected: NOT GT_OK.
    */

    vrId = 100;

    st = cpssExMxPmIpLpmIpv4UcPrefixDelete(lpmDbId, vrId, ipAddr, prefixLen);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    vrId = 0;

    /*
        1.8. Call with lpmDbId [1],
                       vrId [0],
                       ipAddr.arIP [192.168.0.1]
                       and prefixLen [32].
        Expected: NOT GT_OK.
    */
        ipAddr.arIP[0] = 192;
        ipAddr.arIP[1] = 168;
        ipAddr.arIP[2] = 0;
        ipAddr.arIP[3] = 1;

        prefixLen = 32;
        override  = GT_FALSE;

        nextHopPointer.routeEntryBaseMemAddr = 0;
        nextHopPointer.blockSize             = 1;
        nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    st = cpssExMxPmIpLpmIpv4UcPrefixDelete(lpmDbId, vrId, ipAddr, prefixLen);
    UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,"%d, %d, prefixLen = %d",
                                 lpmDbId, vrId, prefixLen);


    /* Delete Prefixes */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d", lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv4UcPrefixBulkDelete
(
    IN  GT_U32                                          lpmDbId,
    IN  GT_U32                                          ipv4PrefixArrayLen,
    IN  CPSS_EXMXPM_IP_LPM_IPV4_UC_DELETE_PREFIX_STCT   *ipv4PrefixArrayPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4UcPrefixBulkDelete)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                   vrId [0 / 0]
                   and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportMcIpv4 [GT_TRUE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportUcIpv6 [GT_TRUE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                    defaultMcIpv6RuleIndex [0 / 100],
                                    defaultMcIpv6PclId [0 / 100]}.
    Expected: GT_OK.
    1.3. Call with cpssExMxPmIpLpmIpv4UcPrefixBulkAdd lpmDbId [1],
                   ipv4PrefixArrayLen [2],
                   ipv4PrefixArrayPtr[0, 1] {vrId [0 / 1],
                                             ipAddr[10.17.0.1 / 10.17.0.2],
                                             prefixLen [32 / 32],
                                             nextHopPointer {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_COS_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                             override [GT_FALSE],
                                             returnStatus [GT_OK] }.
    Expected: GT_OK.
    1.4. Call with lpmDbId [1],
                   ipv4PrefixArrayLen [2],
                   ipv4PrefixArrayPtr[0, 1] {vrId [0 / 1],
                                             ipAddr[10.17.0.1 / 10.17.0.2],
                                             prefixLen [32 / 32],
                                             nextHopPointer {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_COS_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                             override [GT_FALSE],
                                             returnStatus [GT_OK] }.
    Expected: GT_OK.
    1.5. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.6. Call with ipv4PrefixArrayPtr[0]->vrId [100] (not exist)
                   and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.7. Call with ipv4PrefixArrayPtr [NULL]
                   and other parameters from 1.4.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    GT_U32                                          ipv4PrefixArrayLen = 0;
    CPSS_EXMXPM_IP_LPM_IPV4_UC_ADD_PREFIX_STC       ipv4PrefixArray   [2];
    CPSS_EXMXPM_IP_LPM_IPV4_UC_DELETE_PREFIX_STCT   ipv4PrefixDelArray[2];


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                    CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.

        1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                        defaultMcIpv6RuleIndex [0 / 100],
                                        defaultMcIpv6PclId [0 / 100]}.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        1.3. Call cpssExMxPmIpLpmIpv4UcPrefixBulkAdd with lpmDbId [1],
                       ipv4PrefixArrayLen [2],
                       ipv4PrefixArrayPtr[0, 1] {vrId [0 / 1],
                                                 ipAddr[10.17.0.1 / 10.17.0.2],
                                                 prefixLen [32 / 32],
                                                 nextHopPointer {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_COS_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                                 override [GT_FALSE],
                                                 returnStatus [GT_OK] }.
        Expected: GT_OK.
    */
    lpmDbId            = 1;
    ipv4PrefixArrayLen = 2;

    ipv4PrefixArray[0].vrId = 0;

    ipv4PrefixArray[0].ipAddr.arIP[0] = 10;
    ipv4PrefixArray[0].ipAddr.arIP[1] = 17;
    ipv4PrefixArray[0].ipAddr.arIP[2] = 0;
    ipv4PrefixArray[0].ipAddr.arIP[3] = 1;

    ipv4PrefixArray[0].prefixLen = 32;

    ipv4PrefixArray[0].nextHopPointer.routeEntryBaseMemAddr = 0;
    ipv4PrefixArray[0].nextHopPointer.blockSize             = 1;
    ipv4PrefixArray[0].nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_COS_ROUTE_ENTRY_GROUP_E;

    ipv4PrefixArray[0].override     = GT_FALSE;
    ipv4PrefixArray[0].returnStatus = GT_OK;

    ipv4PrefixArray[1].vrId = 1;

    ipv4PrefixArray[1].ipAddr.arIP[0] = 10;
    ipv4PrefixArray[1].ipAddr.arIP[1] = 17;
    ipv4PrefixArray[1].ipAddr.arIP[2] = 0;
    ipv4PrefixArray[1].ipAddr.arIP[3] = 2;

    ipv4PrefixArray[1].prefixLen = 32;

    ipv4PrefixArray[1].nextHopPointer.routeEntryBaseMemAddr = 0xFF;
    ipv4PrefixArray[1].nextHopPointer.blockSize             = 1;
    ipv4PrefixArray[1].nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    ipv4PrefixArray[1].override     = GT_FALSE;
    ipv4PrefixArray[1].returnStatus = GT_OK;

    st = cpssExMxPmIpLpmIpv4UcPrefixBulkAdd(lpmDbId, ipv4PrefixArrayLen, ipv4PrefixArray);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4UcPrefixBulkAdd: %d, %d", lpmDbId, ipv4PrefixArrayLen);

    /*
        1.4. Call with lpmDbId [1],
                       ipv4PrefixArrayLen [2],
                       ipv4PrefixArrayPtr[0, 1] {vrId [0 / 1],
                                                 ipAddr[10.17.0.1 / 10.17.0.2],
                                                 prefixLen [32 / 32],
                                                 nextHopPointer {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_COS_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                                 override [GT_FALSE],
                                                 returnStatus [GT_OK] }.
        Expected: GT_OK.
    */
    ipv4PrefixArrayLen = 2;

    ipv4PrefixDelArray[0].vrId = 0;

    ipv4PrefixDelArray[0].ipAddr.arIP[0] = 10;
    ipv4PrefixDelArray[0].ipAddr.arIP[1] = 17;
    ipv4PrefixDelArray[0].ipAddr.arIP[2] = 0;
    ipv4PrefixDelArray[0].ipAddr.arIP[3] = 1;

    ipv4PrefixDelArray[0].prefixLen    = 32;
    ipv4PrefixDelArray[0].returnStatus = GT_OK;

    ipv4PrefixDelArray[1].vrId = 1;

    ipv4PrefixDelArray[1].ipAddr.arIP[0] = 10;
    ipv4PrefixDelArray[1].ipAddr.arIP[1] = 17;
    ipv4PrefixDelArray[1].ipAddr.arIP[2] = 0;
    ipv4PrefixDelArray[1].ipAddr.arIP[3] = 2;

    ipv4PrefixDelArray[1].prefixLen    = 32;
    ipv4PrefixDelArray[1].returnStatus = GT_OK;

    st = cpssExMxPmIpLpmIpv4UcPrefixBulkDelete(lpmDbId, ipv4PrefixArrayLen, ipv4PrefixDelArray);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, ipv4PrefixArrayLen);

    /*
        1.5. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.4.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixBulkDelete(lpmDbId, ipv4PrefixArrayLen, ipv4PrefixDelArray);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.6. Call with ipv4PrefixArrayPtr[0]->vrId [100] (not exist)
                       and other parameters from 1.4.
        Expected: NOT GT_OK.
    */
    ipv4PrefixDelArray[0].vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixBulkDelete(lpmDbId, ipv4PrefixArrayLen, ipv4PrefixDelArray);
    UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipv4PrefixArrayPtr[0]->vrId = %d",
                                     lpmDbId, ipv4PrefixArray[0].vrId);

    ipv4PrefixDelArray[0].vrId = IP_LPM_INVALID_VR_ID_CNS;

    /*
        1.7. Call with ipv4PrefixArrayPtr [NULL]
                       and other parameters from 1.4.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv4UcPrefixBulkDelete(lpmDbId, ipv4PrefixArrayLen, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipv4PrefixArrayPtr  = NULL", lpmDbId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv4UcPrefixesFlush
(
    IN  GT_U32  lpmDbId,
    IN  GT_U32  vrId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4UcPrefixesFlush)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                   vrId [0 / 0]
                   and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportMcIpv4 [GT_TRUE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportUcIpv6 [GT_TRUE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                    defaultMcIpv6RuleIndex [0 / 100],
                                    defaultMcIpv6PclId [0 / 100]}.
    Expected: GT_OK.
    1.3. Call with lpmDbId [1]
                   and vrId [0]
    Expected: GT_OK.
    1.4. Call with lpmDbId [100] (not exist)
                   and vrId [0].
    Expected: NOT GT_OK.
    1.5. Call with lpmDbId [1]
                   and vrId [100].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                    CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.

        1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                        defaultMcIpv6RuleIndex [0 / 100],
                                        defaultMcIpv6PclId [0 / 100]}.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        1.3. Call with lpmDbId [1]
                       and vrId [0]
        Expected: GT_OK.
    */
    lpmDbId = 1;
    vrId    = 0;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);


    /*
        1.4. Call with lpmDbId [100] (not exist)
                       and vrId [0].
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.5. Call with lpmDbId [1]
                       and vrId [100].
        Expected: NOT GT_OK.
    */
    /* Delete Prefixes */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d", lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv4UcPrefixGet
(
    IN  GT_U32                                  lpmDbId,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    IN  GT_U32                                  prefix,
    OUT CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  *nextHopPointerPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4UcPrefixGet)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                   vrId [0 / 0]
                   and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportMcIpv4 [GT_TRUE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportUcIpv6 [GT_TRUE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                    defaultMcIpv6RuleIndex [0 / 100],
                                    defaultMcIpv6PclId [0 / 100]}.
    Expected: GT_OK.
    1.3. Call with cpssExMxPmIpLpmIpv4UcPrefixAdd lpmDbId [1],
                   vrId [0],
                   ipAddr.arIP [10.17.0.1],
                   prefixLen [32],
                   nextHopPointer {routeEntryBaseMemAddr [0],
                                   blockSize [1],
                                   routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                   and override [GT_FALSE].
    Expected: GT_OK.
    1.4. Call with lpmDbId [1],
                   vrId [0],
                   ipAddr [10.17.0.1],
                   prefixLen [32]
                   and non-NULL nextHopPointerPtr.
    Expected: GT_OK.
    1.5. Call with lpmDbId [1],
                   vrId [0],
                   ipAddr [192.168.0.1],
                   prefixLen [32]
                   and non-NULL nextHopPointerPtr.
    Expected: GT_NOT_FOUND.
    1.6. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.7. Call with lpmDbId [1],
                   vrId [100] (not exist)
                   and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.8. Call with lpmDbId [1],
                   vrId [0],
                   nextHopPointer [NULL]
                   and other parameters from 1.3.
    Expected: GT_BAD_PTR.
    1.9. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush with lpmDbId [1],
                                                      vrId [0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPADDR                               ipAddr;
    GT_U32                                  prefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                                 override  = GT_FALSE;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                    CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.

        1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                        defaultMcIpv6RuleIndex [0 / 100],
                                        defaultMcIpv6PclId [0 / 100]}.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        1.3. Call with cpssExMxPmIpLpmIpv4UcPrefixAdd lpmDbId [1],
                       vrId [0],
                       ipAddr.arIP [10.17.0.1],
                       prefixLen [32],
                       nextHopPointer {routeEntryBaseMemAddr [0],
                                       blockSize [1],
                                       routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                       and override [GT_FALSE].
        Expected: GT_OK.
    */
    lpmDbId = 1;
    vrId    = 0;

    ipAddr.arIP[0] = 10;
    ipAddr.arIP[1] = 17;
    ipAddr.arIP[2] = 0;
    ipAddr.arIP[3] = 1;

    prefixLen = 32;
    override  = GT_FALSE;

    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4UcPrefixAdd: %d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    /*
        1.4. Call with lpmDbId [1],
                       vrId [0],
                       ipAddr [10.17.0.1],
                       prefixLen [32]
                       and non-NULL nextHopPointerPtr.
        Expected: GT_OK.
    */
    st = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,"%d, %d, prefixLen = %d",
                                 lpmDbId, vrId, prefixLen);
    /*
        1.5. Call with lpmDbId [1],
                       vrId [0],
                       ipAddr [192.168.0.1],
                       prefixLen [32]
                       and non-NULL nextHopPointerPtr.
        Expected: GT_NOT_FOUND.
    */

    ipAddr.arIP[0] = 192;
    ipAddr.arIP[1] = 168;
    ipAddr.arIP[2] = 0;
    ipAddr.arIP[3] = 1;

    st = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_NOT_FOUND, st,"%d, %d, prefixLen = %d",
                                 lpmDbId, vrId, prefixLen);

    /*
        1.6. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.4.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;
    ipAddr.arIP[0] = 10;
    ipAddr.arIP[1] = 17;
    ipAddr.arIP[2] = 0;
    ipAddr.arIP[3] = 1;

    st = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.7. Call with lpmDbId [1],
                       vrId [100] (not exist)
                       and other parameters from 1.4.
        Expected: NOT GT_OK.
    */
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    vrId = 0;

    /*
        1.8. Call with lpmDbId [1],
                       vrId [0],
                       nextHopPointer [NULL]
                       and other parameters from 1.3.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, prefixLen, NULL);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nextHopPointer = NULL", lpmDbId, vrId);

    /*
        1.9. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush with lpmDbId [1],
                                                          vrId [0] to invalidate changes.
        Expected: GT_OK.
    */
    /* Delete Prefixes */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d", lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv4UcPrefixGetNext
(
    IN    GT_U32                                    lpmDbId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPADDR                                 *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC    *nextHopPointerPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4UcPrefixGetNext)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                   vrId [0 / 0]
                   and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportMcIpv4 [GT_TRUE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportUcIpv6 [GT_TRUE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                    defaultMcIpv6RuleIndex [0 / 100],
                                    defaultMcIpv6PclId [0 / 100]}.
    Expected: GT_OK.
    1.3. Call cpssExMxPmIpLpmIpv4UcPrefixAdd with lpmDbId [1],
                                                  vrId [0],
                                                  ipAddr.arIP [10.17.0.1 / 10.17.0.2],
                                                  prefixLen [32 / 32],
                                                  nextHopPointer {routeEntryBaseMemAddr [0 / 0xFF],
                                                                  blockSize [0 / 1],
                                                                  routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                    CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                                  and override [GT_FALSE].
    Expected: GT_OK.
    1.4. Call lpmDbId [1],
              vrId[0],
              ipAddrPtr->arIP [10.17.0.1],
              prefixLenPtr[32]
              and non-NULL nextHopPointerPtr.
    Expected: GT_OK.
    1.4.1. Call lpmDbId [1],
              vrId[0],
              ipAddrPtr->arIP [10.17.0.2],
              prefixLenPtr[32]
              and non-NULL nextHopPointerPtr.
    Expected: GT_NOT_FOUND.
    1.5. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.6. Call with vrId [100] (not exist)
                   and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.7. Call with prefixLenPtr [NULL]
                   and other parameters from 1.4.
    Expected: GT_BAD_PTR.
    1.8. Call with ipAddrPtr [NULL]
                   and other parameters from 1.4.
    Expected: GT_BAD_PTR.
    1.9. Call with nextHopPointerPtr [NULL]
                   and other parameters from 1.4.
    Expected: GT_BAD_PTR.
    1.10. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush with lpmDbId [1],
                                                      vrId [0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPADDR                               ipAddr;
    GT_U32                                  prefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                                 override  = GT_FALSE;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                    CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.

        1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                        defaultMcIpv6RuleIndex [0 / 100],
                                        defaultMcIpv6PclId [0 / 100]}.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        1.3. Call cpssExMxPmIpLpmIpv4UcPrefixAdd with lpmDbId [1],
                                                      vrId [0],
                                                      ipAddr.arIP [10.17.0.1 / 10.17.0.2],
                                                      prefixLen [32 / 32],
                                                      nextHopPointer {routeEntryBaseMemAddr [0 / 0xFF],
                                                                      blockSize [0 / 1],
                                                                      routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                        CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                                      and override [GT_FALSE].
        Expected: GT_OK.
    */
    lpmDbId = 1;
    vrId    = 0;

    /* Call with ipAddr.arIP [10.17.0.1] */
    ipAddr.arIP[0] = 10;
    ipAddr.arIP[1] = 17;
    ipAddr.arIP[2] = 0;
    ipAddr.arIP[3] = 1;

    prefixLen = 32;
    override  = GT_FALSE;

    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;

    st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4UcPrefixAdd: %d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    /* Call with ipAddr.arIP [10.17.0.2] */
    ipAddr.arIP[0] = 10;
    ipAddr.arIP[1] = 17;
    ipAddr.arIP[2] = 0;
    ipAddr.arIP[3] = 2;

    prefixLen = 32;
    override  = GT_FALSE;

    nextHopPointer.routeEntryBaseMemAddr = 1;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4UcPrefixAdd: %d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    /*
        1.4. Call lpmDbId [1],
                  vrId[0],
                  ipAddrPtr->arIP [10.17.0.1],
                  prefixLenPtr[32]
                  and non-NULL nextHopPointerPtr.
        Expected: GT_OK.
    */
    ipAddr.arIP[0] = 10;
    ipAddr.arIP[1] = 17;
    ipAddr.arIP[2] = 0;
    ipAddr.arIP[3] = 1;

    prefixLen = 32;

    st = cpssExMxPmIpLpmIpv4UcPrefixGetNext(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,"%d, %d, prefixLen = %d",
                                 lpmDbId, vrId, prefixLen);
    /*
        1.4.1. Call lpmDbId [1],
                  vrId[0],
                  ipAddrPtr->arIP [10.17.0.2],
                  prefixLenPtr[32]
                  and non-NULL nextHopPointerPtr.
        Expected: GT_NOT_FOUND.
    */
    ipAddr.arIP[0] = 10;
    ipAddr.arIP[1] = 17;
    ipAddr.arIP[2] = 0;
    ipAddr.arIP[3] = 2;

    prefixLen = 32;

    st = cpssExMxPmIpLpmIpv4UcPrefixGetNext(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_NOT_FOUND, st,"%d, %d, prefixLen = %d",
                                 lpmDbId, vrId, prefixLen);

    /*
        1.5. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.4.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixGetNext(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.6. Call with lpmDbId [1],
                       vrId [100] (not exist)
                       and other parameters from 1.4.
        Expected: NOT GT_OK.
    */
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixGetNext(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    vrId = 0;

    /*
        1.7. Call with prefixLenPtr [NULL]
                       and other parameters from 1.4.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv4UcPrefixGetNext(lpmDbId, vrId, &ipAddr, NULL, &nextHopPointer);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, prefixLenPtr = NULL", lpmDbId, vrId);

    /*
        1.8. Call with ipAddrPtr [NULL]
                       and other parameters from 1.4.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv4UcPrefixGetNext(lpmDbId, vrId, NULL, &prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, ipAddrPtr = NULL", lpmDbId, vrId);

    /*
        1.9. Call with nextHopPointerPtr [NULL]
                       and other parameters from 1.4.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv4UcPrefixGetNext(lpmDbId, vrId, &ipAddr, &prefixLen, NULL);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nextHopPointerPtr = NULL", lpmDbId, vrId);

    /*
        1.9. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush with lpmDbId [1],
                                                          vrId [0] to invalidate changes.
        Expected: GT_OK.
    */
    /* Delete Prefixes */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d", lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv4UcLpmGet
(
    IN  GT_U32                                  lpmDbId,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    OUT GT_U32                                  *prefixLenPtr,
    OUT CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  *nextHopPointerPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4UcLpmGet)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                   vrId [0 / 0]
                   and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportMcIpv4 [GT_TRUE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportUcIpv6 [GT_TRUE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                    defaultMcIpv6RuleIndex [0 / 100],
                                    defaultMcIpv6PclId [0 / 100]}.
    Expected: GT_OK.
    1.3. Call cpssExMxPmIpLpmIpv4UcPrefixAdd with lpmDbId [1],
                                                  vrId [0],
                                                  ipAddr.arIP [10.17.0.1 / 10.17.0.2],
                                                  prefixLen [32 / 32],
                                                  nextHopPointer {routeEntryBaseMemAddr [0 / 0xFF],
                                                                  blockSize [0 / 1],
                                                                  routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                    CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                                  and override [GT_FALSE].
    Expected: GT_OK.
    1.4. Call with lpmDbId [1],
                   vrId[0],
                   ipAddrPtr->arIP [10.17.0.1 / 10.17.0.2],
                   non-NULL prefixLenPtr
                   and non-NULL nextHopPointerPtr.
    Expected: GT_OK.
    1.4.1. Call with lpmDbId [2],
                   vrId[0],
                   ipAddrPtr->arIP [10.17.0.3],
                   non-NULL prefixLenPtr
                   and non-NULL nextHopPointerPtr.
    Expected: GT_NOT_FOUND.
    1.5. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.6. Call with vrId [100] (not exist)
                   and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.7. Call with prefixLenPtr [NULL]
                   and other parameters from 1.4.
    Expected: GT_BAD_PTR.
    1.8. Call with nextHopPointerPtr [NULL]
                   and other parameters from 1.4.
    Expected: GT_BAD_PTR.
    1.9. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush with lpmDbId [1],
                                                      vrId [0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPADDR                               ipAddr;
    GT_U32                                  prefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                                 override  = GT_FALSE;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                    CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.

        1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                        defaultMcIpv6RuleIndex [0 / 100],
                                        defaultMcIpv6PclId [0 / 100]}.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        1.3. Call cpssExMxPmIpLpmIpv4UcPrefixAdd with lpmDbId [1],
                                                      vrId [0],
                                                      ipAddr.arIP [10.17.0.1 / 10.17.0.2],
                                                      prefixLen [32 / 32],
                                                      nextHopPointer {routeEntryBaseMemAddr [0 / 0xFF],
                                                                      blockSize [0 / 1],
                                                                      routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                        CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                                      and override [GT_FALSE].
        Expected: GT_OK.
    */
    lpmDbId = 1;
    vrId    = 0;

    /* Call with ipAddr.arIP [10.17.0.1] */
    ipAddr.arIP[0] = 10;
    ipAddr.arIP[1] = 17;
    ipAddr.arIP[2] = 0;
    ipAddr.arIP[3] = 1;

    prefixLen = 32;
    override  = GT_FALSE;

    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;

    st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4UcPrefixAdd: %d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    /* Call with ipAddr.arIP [10.17.0.2] */
    ipAddr.arIP[0] = 10;
    ipAddr.arIP[1] = 17;
    ipAddr.arIP[2] = 0;
    ipAddr.arIP[3] = 2;

    nextHopPointer.routeEntryBaseMemAddr = 0xFF;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4UcPrefixAdd: %d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    /*
        1.4. Call with lpmDbId [1],
                       vrId[0],
                       ipAddrPtr->arIP [10.17.0.1 / 10.17.0.2],
                       non-NULL prefixLenPtr
                       and non-NULL nextHopPointerPtr.
        Expected: GT_OK.
    */

    /* Call with ipAddrPtr->arIP [10.17.0.1] */
    ipAddr.arIP[0] = 10;
    ipAddr.arIP[1] = 17;
    ipAddr.arIP[2] = 0;
    ipAddr.arIP[3] = 1;

    st = cpssExMxPmIpLpmIpv4UcLpmGet(lpmDbId, vrId, ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,"%d, %d", lpmDbId, vrId);

    /* Call with ipAddrPtr->arIP [10.17.0.2] */
    ipAddr.arIP[0] = 10;
    ipAddr.arIP[1] = 17;
    ipAddr.arIP[2] = 0;
    ipAddr.arIP[3] = 2;

    st = cpssExMxPmIpLpmIpv4UcLpmGet(lpmDbId, vrId, ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,"%d, %d", lpmDbId, vrId);
    /*
        1.4.1. Call with lpmDbId [1],
                       vrId[0],
                       ipAddrPtr->arIP [10.17.0.3],
                       non-NULL prefixLenPtr
                       and non-NULL nextHopPointerPtr.
        Expected: GT_NOT_FOUND.
    */
    lpmDbId = 2;
    vrId    = 0;
    ipAddr.arIP[0] = 10;
    ipAddr.arIP[1] = 17;
    ipAddr.arIP[2] = 0;
    ipAddr.arIP[3] = 3;

    st = cpssExMxPmIpLpmIpv4UcLpmGet(lpmDbId, vrId, ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_NOT_FOUND, st,"%d, %d", lpmDbId, vrId);


    /*
        1.5. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.4.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcLpmGet(lpmDbId, vrId, ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.6. Call with vrId [100] (not exist)
                       and other parameters from 1.4.
        Expected: NOT GT_OK.
    */
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcLpmGet(lpmDbId, vrId, ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    vrId = 0;

    /*
        1.7. Call with prefixLenPtr [NULL]
                       and other parameters from 1.4.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv4UcLpmGet(lpmDbId, vrId, ipAddr, NULL, &nextHopPointer);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, prefixLenPtr = NULL", lpmDbId, vrId);

    /*
        1.8. Call with nextHopPointerPtr [NULL]
                       and other parameters from 1.4.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv4UcLpmGet(lpmDbId, vrId, ipAddr, &prefixLen, NULL);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nextHopPointerPtr = NULL", lpmDbId, vrId);

    /*
        1.9. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush with lpmDbId [1],
                                                          vrId [0] to invalidate changes.
        Expected: GT_OK.
    */
    /* Delete Prefixes */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d", lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv4McEntryAdd
(
    IN  GT_U32                                  lpmDbId,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipGroup,
    IN  GT_IPADDR                               ipSrc,
    IN  GT_U32                                  ipSrcPrefixLen,
    IN  CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  *mcRoutePointerPtr,
    IN  GT_BOOL                                 override
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4McEntryAdd)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                   vrId [0 / 0]
                   and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportMcIpv4 [GT_TRUE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportUcIpv6 [GT_TRUE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                    defaultMcIpv6RuleIndex [0 / 100],
                                    defaultMcIpv6PclId [0 / 100]}.
    Expected: GT_OK.
    1.3. Call with ipGroup (224.1.1.1)
                   ipSrc.arIP [10.17.0.1 / 10.17.0.2],
                   ipSrcPrefixLen [32],
                   mcRoutePointerPtr {routeEntryBaseMemAddr [0 / 0xFF],
                                      blockSize [0 / 1],
                                      routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                        CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                   override [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.4. Call cpssExMxPmIpLpmIpv4McEntryGet with the same lpmDbId,
                                                 vrId,
                                                 ipGroup,
                                                 ipSrc,
                                                 ipSrcPrefixLen
                                                 and non-NULL mcRoutePointerPtr.
    Expected: GT_OK and the same mcRoutePointerPtr.
    1.5. Call with ipGroup (224.1.1.1),
                   ipSrc.arIP [10.17.0.3],
                   ipSrcPrefixLen [32],
                   mcRoutePointerPtr {routeEntryBaseMemAddr [0xFF],
                                      blockSize [1],
                                      routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E],                                                            ]},
                   override [GT_TRUE].
    Expected: GT_NOT_FOUND.
    1.6. Call with ipGroup (224.1.1.1),
                   ipSrc.arIP [10.17.0.2], (already exists)
                   ipSrcPrefixLen [32],
                   mcRoutePointerPtr {routeEntryBaseMemAddr [0],
                                      blockSize [1],
                                      routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                   override [GT_TRUE].
    Expected: GT_ALREADY_EXIST.
    1.7. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.8. Call with vrId [100] (not exist)
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.9. Call with out of range mcRoutePointerPtr->routeEntryMethod [0x5AAAAAA5]
                   and other parameters from 1.3.
    Expected: GT_BAD_PARAM.
    1.10. Call with mcRoutePointerPtr->blockSize [0],
                    mcRoutePointerPtr->routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E] (for regular should be 1)
                    and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.11. Call with mcRoutePointerPtr [NULL]
                    and other parameters from 1.3.
    Expected: GT_BAD_PTR.
    1.12. Call cpssExMxPmIpLpmIpv4McEntriesFlush with lpmDbId [1],
                                                     vrId [0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPADDR                               ipGroup;
    GT_IPADDR                               ipSrc;
    GT_U32                                  ipSrcPrefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  mcRoutePointer;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  mcRoutePointerGet;
    GT_BOOL                                 override       = GT_FALSE;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                    CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.

        1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                                        defaultMcIpv6RuleIndex [0 / 100],
                                        defaultMcIpv6PclId [0 / 100]}.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        1.3. Call with ipGroup (0),
                       ipSrc.arIP [10.17.0.1 / 10.17.0.2],
                       ipSrcPrefixLen [32],
                       mcRoutePointerPtr {routeEntryBaseMemAddr [0 / 0xFF],
                                          blockSize [0 / 1],
                                          routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                            CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                       override [GT_FALSE / GT_TRUE].
        Expected: GT_OK.
    */

    /* Call with ipSrc.arIP [10.17.0.1] */
    ipGroup.arIP[0] = 224;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 10;
    ipSrc.arIP[1] = 17;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 1;

    ipSrcPrefixLen = 32;

    mcRoutePointer.routeEntryBaseMemAddr = 0;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;

    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,"%d, %d, ipSrcPrefixLen = %d, override = %d",
                                 lpmDbId, vrId, ipSrcPrefixLen, override);

    /*
        1.4. Call cpssExMxPmIpLpmIpv4McEntryGet with the same lpmDbId,
                                                     vrId,
                                                     ipGroup,
                                                     ipSrc,
                                                     ipSrcPrefixLen
                                                     and non-NULL mcRoutePointerPtr.
        Expected: GT_OK and the same mcRoutePointerPtr.
    */
    st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointerGet);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4McEntryGet: %d, %d, ipSrcPrefixLen = %d", lpmDbId, vrId, ipSrcPrefixLen);

    /* Verifying values */
    UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.routeEntryBaseMemAddr, mcRoutePointerGet.routeEntryBaseMemAddr,
               "get another mcRoutePointerPtr->routeEntryBaseMemAddr than was set: %d, %d", lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.blockSize, mcRoutePointerGet.blockSize,
               "get another mcRoutePointerPtr->blockSize than was set: %d, %d", lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.routeEntryMethod, mcRoutePointerGet.routeEntryMethod,
               "get another mcRoutePointerPtr->routeEntryMethod than was set: %d, %d", lpmDbId, vrId);

    /* Call with ipSrc.arIP [10.17.0.1] */
    ipGroup.arIP[0] = 224;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 10;
    ipSrc.arIP[1] = 17;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 2;

    ipSrcPrefixLen = 32;

    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,"%d, %d, ipSrcPrefixLen = %d, override = %d",
                                 lpmDbId, vrId, ipSrcPrefixLen, override);

    /*
        1.4. Call cpssExMxPmIpLpmIpv4McEntryGet with the same lpmDbId,
                                                     vrId,
                                                     ipGroup,
                                                     ipSrc,
                                                     ipSrcPrefixLen
                                                     and non-NULL mcRoutePointerPtr.
        Expected: GT_OK and the same mcRoutePointerPtr.
    */
    st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointerGet);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4McEntryGet: %d, %d, ipSrcPrefixLen = %d", lpmDbId, vrId, ipSrcPrefixLen);

    /* Verifying values */
    UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.routeEntryBaseMemAddr, mcRoutePointerGet.routeEntryBaseMemAddr,
               "get another mcRoutePointerPtr->routeEntryBaseMemAddr than was set: %d, %d", lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.blockSize, mcRoutePointerGet.blockSize,
               "get another mcRoutePointerPtr->blockSize than was set: %d, %d", lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.routeEntryMethod, mcRoutePointerGet.routeEntryMethod,
               "get another mcRoutePointerPtr->routeEntryMethod than was set: %d, %d", lpmDbId, vrId);



    /*
        1.5. Call with ipGroup (224.1.1.1),
                       ipSrc.arIP [10.17.0.3],
                       ipSrcPrefixLen [32],
                       mcRoutePointerPtr {routeEntryBaseMemAddr [0xFF],
                                          blockSize [1],
                                          routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E],                                                            ]},
                       override [GT_TRUE].
        Expected: GT_NOT_FOUND.
    */

    /* Call with ipSrc.arIP [10.17.0.3] */
    ipGroup.arIP[0] = 224;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 10;
    ipSrc.arIP[1] = 17;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 3;

    ipSrcPrefixLen = 32;

    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;

    override = GT_TRUE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_NOT_FOUND, st,"%d, %d, ipSrcPrefixLen = %d, override = %d",
                                 lpmDbId, vrId, ipSrcPrefixLen, override);

    /*
        1.6. Call with ipGroup (224.1.1.1),
                       ipSrc.arIP [10.17.0.2], (already exists)
                       ipSrcPrefixLen [32],
                       mcRoutePointerPtr {routeEntryBaseMemAddr [0],
                                          blockSize [1],
                                          routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E],
                       override [GT_TRUE].
        Expected: GT_ALREADY_EXIST.
    */

    /* Call with override = GT_FALSE */
    ipGroup.arIP[0] = 224;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 10;
    ipSrc.arIP[1] = 17;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 2;

    ipSrcPrefixLen = 32;

    mcRoutePointer.routeEntryBaseMemAddr = 0;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;

    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_ALREADY_EXIST, st,"%d, %d, ipSrcPrefixLen = %d, override = %d",
                                 lpmDbId, vrId, ipSrcPrefixLen, override);


    /*
        1.7. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.3.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.8. Call with vrId [100] (not exist)
                       and other parameters from 1.3.
        Expected: NOT GT_OK.
    */
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    vrId = 0;

    /*
        1.9. Call with out of range mcRoutePointerPtr->routeEntryMethod [0x5AAAAAA5]
                       and other parameters from 1.3.
        Expected: GT_BAD_PARAM.
    */
    mcRoutePointer.routeEntryMethod = IP_LPM_INVALID_ENUM_CNS;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, mcRoutePointerPtr->routeEntryMethod = %d",
                                 lpmDbId, vrId, mcRoutePointer.routeEntryMethod);

    mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    /*
        1.10. Call with mcRoutePointerPtr->blockSize [0],
                        mcRoutePointerPtr->routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E] (for regular should be 1)
                        and other parameters from 1.3.
        Expected: NOT GT_OK.
    */
    mcRoutePointer.blockSize = 0;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, ->blockSize = %d. ->routeEntryMethod = %d",
                                     lpmDbId, vrId, mcRoutePointer.blockSize, mcRoutePointer.routeEntryMethod);

    mcRoutePointer.blockSize = 1;

    /*
        1.11. Call with mcRoutePointerPtr [NULL]
                        and other parameters from 1.3.
        Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, NULL, override);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, mcRoutePointerPtr = NULL", lpmDbId, vrId);

    /*
        1.12. Call cpssExMxPmIpLpmIpv4McEntriesFlush with lpmDbId [1],
                                                         vrId [0] to invalidate changes.
        Expected: GT_OK.
    */
    /* Delete Prefixes */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntriesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4McEntriesFlush: %d, %d", lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*******************************************************************************
* Entry IpV4 add, check,  delete, check memory leakages
1. Create defauld LpmDB and Virtual Routers.
Expected: GT_OK.
2. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_before_IPV4_Entry.
3. Call cpssExMxPmIpLpmIpv4McEntryAdd to fill table of entries
Expected: GT_OK.
4. Call cpssExMxPmIpLpmIpv4McEntryGet to check entries
Expected: GT_OK.
5. Call cpssExMxPmIpLpmIpv4McEntryDelete to delete all entries
Expected: GT_OK.
6. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_IPV4_Entry.
Expected: (heap_bytes_allocated_after_IPV4_Entry-heap_bytes_allocated_before_IPV4_Entry) = 0
7. Delete defauld LpmDB and Virtual Routers
Expected: GT_OK.
*******************************************************************************/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4McEntryMemoryCheck)
{
    GT_STATUS   st        = GT_OK;

    GT_U32      lpmDbId = 0;
    GT_U32      vrId    = 0;
    GT_U32      ii      = 0;

    GT_U32      heap_bytes_allocated_before_IPV4_Entry = 0;
    GT_U32      heap_bytes_allocated_after_IPV4_Entry = 0;

    GT_U32      ipSrcPrefixLen = 0;
    GT_BOOL     override       = GT_FALSE;

    GT_IPADDR   ipGroup;
    GT_IPADDR   ipSrc;

    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  mcRoutePointer;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  mcRoutePointerGet;


    cpssOsBzero((GT_VOID*) &ipGroup, sizeof(ipGroup));
    cpssOsBzero((GT_VOID*) &ipSrc, sizeof(ipSrc));
    cpssOsBzero((GT_VOID*) &mcRoutePointer, sizeof(mcRoutePointer));
    cpssOsBzero((GT_VOID*) &mcRoutePointerGet, sizeof(mcRoutePointerGet));

    /*
        1. Create defauld LpmDB and Virtual Routers.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        2. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_before_IPV4_Entry.
    */
    heap_bytes_allocated_before_IPV4_Entry = osMemGetHeapBytesAllocated();

    for(ii=0; ii<IP_LPM_ITER_COUNT; ++ii)
    {
        /*
            3. Call cpssExMxPmIpLpmIpv4McEntryAdd to fill table of entries
                              with lpmDbId [1 .. max].
                                   vrId [0 .. max]
                                   ipGroup (0),
                                   ipSrc.arIP [10.17.0.1 / 10.17.0.2],
                                   ipSrcPrefixLen [32],
                                   mcRoutePointerPtr {routeEntryBaseMemAddr [0 / 0xFF],
                                                      blockSize [0 / 1],
                                                      routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                        CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                   override [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
        */
        ipGroup.u32Ip = 0;

        ipSrc.arIP[0] = 10;
        ipSrc.arIP[1] = 17;
        ipSrc.arIP[2] = 0;
        ipSrc.arIP[3] = 1;

        ipSrcPrefixLen = 32;

        mcRoutePointer.routeEntryBaseMemAddr = 0;
        mcRoutePointer.blockSize             = 1;
        mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;

        override = GT_FALSE;

        /* iterate with lpmDbId [1], vrId[0] */
        lpmDbId = 1;
        vrId    = 0;

        ipSrc.arIP[3] = 1;

        st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,"cpssExMxPmIpLpmIpv4McEntryAdd: %d, %d, ipSrcPrefixLen = %d, override = %d",
                                     lpmDbId, vrId, ipSrcPrefixLen, override);

        /* iterate with lpmDbId [1], vrId[1] */
        lpmDbId = 1;
        vrId    = 1;

        ipSrc.arIP[3] = 2;

        st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,"cpssExMxPmIpLpmIpv4McEntryAdd: %d, %d, ipSrcPrefixLen = %d, override = %d",
                                     lpmDbId, vrId, ipSrcPrefixLen, override);

        /* iterate with lpmDbId [2], vrId[0] */
        lpmDbId = 2;
        vrId    = 0;

        ipSrc.arIP[3] = 3;

        st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,"cpssExMxPmIpLpmIpv4McEntryAdd: %d, %d, ipSrcPrefixLen = %d, override = %d",
                                     lpmDbId, vrId, ipSrcPrefixLen, override);

        /*
            4. Call cpssExMxPmIpLpmIpv4McEntryGet to check entries
                                                  with lpmDbId [1 .. max].
                                                       vrId [0 .. max]
                                                       ipGroup [0],
                                                       ipSrc.arIP [10.17.0.1 / 10.17.0.2],
                                                       ipSrcPrefixLen [32],
                                                       and non-NULL mcRoutePointerPtr.
            Expected: GT_OK and the same mcRoutePointer as was set.
        */
        /* iterate with lpmDbId [1], vrId[0] */
        lpmDbId = 1;
        vrId    = 0;

        ipSrc.arIP[3] = 1;

        st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointerGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4McEntryGet: %d, %d, ipSrcPrefixLen = %d", lpmDbId, vrId, ipSrcPrefixLen);

        if(st == GT_OK)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.routeEntryBaseMemAddr, mcRoutePointerGet.routeEntryBaseMemAddr,
                       "get another mcRoutePointerPtr->routeEntryBaseMemAddr than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.blockSize, mcRoutePointerGet.blockSize,
                       "get another mcRoutePointerPtr->blockSize than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.routeEntryMethod, mcRoutePointerGet.routeEntryMethod,
                       "get another mcRoutePointerPtr->routeEntryMethod than was set: %d, %d", lpmDbId, vrId);
        }

        /* iterate with lpmDbId [1], vrId[1] */
        lpmDbId = 1;
        vrId    = 1;

        ipSrc.arIP[3] = 2;

        st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointerGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4McEntryGet: %d, %d, ipSrcPrefixLen = %d", lpmDbId, vrId, ipSrcPrefixLen);

        if(st == GT_OK)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.routeEntryBaseMemAddr, mcRoutePointerGet.routeEntryBaseMemAddr,
                       "get another mcRoutePointerPtr->routeEntryBaseMemAddr than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.blockSize, mcRoutePointerGet.blockSize,
                       "get another mcRoutePointerPtr->blockSize than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.routeEntryMethod, mcRoutePointerGet.routeEntryMethod,
                       "get another mcRoutePointerPtr->routeEntryMethod than was set: %d, %d", lpmDbId, vrId);
        }

        /* iterate with lpmDbId [2], vrId[0] */
        lpmDbId = 2;
        vrId    = 0;

        ipSrc.arIP[3] = 3;

        st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointerGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4McEntryGet: %d, %d, ipSrcPrefixLen = %d", lpmDbId, vrId, ipSrcPrefixLen);

        if(st == GT_OK)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.routeEntryBaseMemAddr, mcRoutePointerGet.routeEntryBaseMemAddr,
                       "get another mcRoutePointerPtr->routeEntryBaseMemAddr than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.blockSize, mcRoutePointerGet.blockSize,
                       "get another mcRoutePointerPtr->blockSize than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.routeEntryMethod, mcRoutePointerGet.routeEntryMethod,
                       "get another mcRoutePointerPtr->routeEntryMethod than was set: %d, %d", lpmDbId, vrId);
        }


        /*
            5. Call cpssExMxPmIpLpmIpv4McEntryDelete to delete all entries.
            Expected: GT_OK.
        */
        /* iterate with lpmDbId [1], vrId[0] */
        lpmDbId = 1;
        vrId    = 0;

        ipSrc.arIP[3] = 1;

        st = cpssExMxPmIpLpmIpv4McEntryDelete(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4McEntryDelete: %d, %d, ipSrcPrefixLen = %d",
                                     lpmDbId, vrId, ipSrcPrefixLen);

        /* iterate with lpmDbId [1], vrId[1] */
        lpmDbId = 1;
        vrId    = 1;

        ipSrc.arIP[3] = 2;

        st = cpssExMxPmIpLpmIpv4McEntryDelete(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4McEntryDelete: %d, %d, ipSrcPrefixLen = %d",
                                     lpmDbId, vrId, ipSrcPrefixLen);

        /* iterate with lpmDbId [2], vrId[0] */
        lpmDbId = 2;
        vrId    = 0;

        ipSrc.arIP[3] = 3;

        st = cpssExMxPmIpLpmIpv4McEntryDelete(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4McEntryDelete: %d, %d, ipSrcPrefixLen = %d",
                                     lpmDbId, vrId, ipSrcPrefixLen);
    }/* for */

    /*
        6. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_after_Router.
        Expected: (heap_bytes_allocated_after_IPV4_Entry - heap_bytes_allocated_before_IPV4_Entry) = 0
    */
    heap_bytes_allocated_after_IPV4_Entry = osMemGetHeapBytesAllocated();
    UTF_VERIFY_EQUAL0_STRING_MAC(heap_bytes_allocated_before_IPV4_Entry, heap_bytes_allocated_after_IPV4_Entry,
                                 "size of heap changed (heap_bytes_allocated)");

    /*
        7. Delete defauld LpmDB and Virtual Routers.
        Expected: GT_OK.
    */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv4McEntryDelete
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPADDR   ipGroup,
    IN  GT_IPADDR   ipSrc,
    IN  GT_U32      ipSrcPrefixLen
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4McEntryDelete)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                   vrId [0 / 0]
                   and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportMcIpv4 [GT_TRUE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportUcIpv6 [GT_TRUE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}}.
    Expected: GT_OK.
    1.3. Call cpssExMxPmIpLpmIpv4McEntryAdd with ipGroup.arIP [224.1.1.1 / 225.1.1.1],
                                                 ipSrc.arIP [10.17.0.1 /192.168.0.1 / 192.170.0.0],
                                                 ipSrcPrefixLen [32 / 24 / 8],
                                                 mcRoutePointerPtr {routeEntryBaseMemAddr [0 / 0xFF],
                                                                    blockSize [1],
                                                                    routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                                 override [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.4. Call with with ipGroup.arIP [224.1.1.1 / 225.1.1.1],
                                                 ipSrc.arIP [10.17.0.1 /192.168.0.1 / 192.170.0.0],
                                                 ipSrcPrefixLen [32 / 24 / 8].
    Expected: GT_OK.
    1.4.1. Call with ipGroup.arIP [224.1.1.1],
                   ipSrc.arIP [10.17.0.1], (not exists)
                   ipSrcPrefixLen [32].
    Expected: GT_NOT_FOUND.
    1.5. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.6. Call with vrId [100] (not exist)
                   and other parameters from 1.4.
    Expected: NOT GT_OK.
    1.7. Call cpssExMxPmIpLpmIpv4McEntriesFlush with lpmDbId [1],
                                                     vrId [0] to invalidate changes.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPADDR                               ipGroup;
    GT_IPADDR                               ipSrc;
    GT_U32                                  ipSrcPrefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  mcRoutePointer;
    GT_BOOL                                 override       = GT_FALSE;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                    CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.

        1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}}.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        1.3. Call cpssExMxPmIpLpmIpv4McEntryAdd with ipGroup.arIP [224.1.1.1 / 225.1.1.1],
                                                     ipSrc.arIP [10.17.0.1 /192.168.0.1 / 192.170.0.0],
                                                     ipSrcPrefixLen [32 / 24 / 8],
                                                     mcRoutePointerPtr {routeEntryBaseMemAddr [0 / 0xFF],
                                                                        blockSize [1],
                                                                        routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                                     override [GT_FALSE / GT_TRUE].
        Expected: GT_OK.
    */
    /*Call with ipGroup.arIP [224.1.1.1], ipSrc.arIP [10.17.0.1], ipSrcPrefixLen [32]. */

    ipGroup.arIP[0] = 224;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 10;
    ipSrc.arIP[1] = 17;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 1;

    ipSrcPrefixLen = 32;

    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4McEntryAdd: %d, %d, ipSrcPrefixLen = %d", lpmDbId, vrId, ipSrcPrefixLen);


    /*Call with ipGroup.arIP [224.1.1.1], ipSrc.arIP [192.168.0.1], ipSrcPrefixLen [24]. */

    ipSrc.arIP[0] = 192;
    ipSrc.arIP[1] = 168;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 1;

    ipSrcPrefixLen = 24;

    mcRoutePointer.routeEntryBaseMemAddr = 0;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4McEntryAdd: %d, %d, ipSrcPrefixLen = %d", lpmDbId, vrId, ipSrcPrefixLen);


    /*Call with ipGroup.arIP [225.1.1.1], ipSrc.arIP [192.170.0.0], ipSrcPrefixLen [8]. */

    ipGroup.arIP[0] = 225;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;


    ipSrc.arIP[0] = 192;
    ipSrc.arIP[1] = 170;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 0;

    ipSrcPrefixLen = 8;

    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4McEntryAdd: %d, %d, ipSrcPrefixLen = %d", lpmDbId, vrId, ipSrcPrefixLen);

    /*
        1.4. Call with ipGroup.arIP [224.1.1.1 / 225.1.1.1],
                                                     ipSrc.arIP [10.17.0.1 /192.168.0.1 / 192.170.0.0],
                                                     ipSrcPrefixLen [32 / 24 / 8].
        Expected: GT_OK.
    */

    /*Call with ipGroup.arIP [224.1.1.1], ipSrc.arIP [10.17.0.1], ipSrcPrefixLen [32]. */

    ipGroup.arIP[0] = 224;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 10;
    ipSrc.arIP[1] = 17;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 1;

    ipSrcPrefixLen = 32;

    st = cpssExMxPmIpLpmIpv4McEntryDelete(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, ipSrcPrefixLen = %d",
                                 lpmDbId, vrId, ipSrcPrefixLen);

    /*Call with ipGroup.arIP [224.1.1.1], ipSrc.arIP [192.168.0.1], ipSrcPrefixLen [24]. */

    ipSrc.arIP[0] = 192;
    ipSrc.arIP[1] = 168;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 1;

    ipSrcPrefixLen = 24;

    st = cpssExMxPmIpLpmIpv4McEntryDelete(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, ipSrcPrefixLen = %d",
                                 lpmDbId, vrId, ipSrcPrefixLen);


    /*Call with ipGroup.arIP [225.1.1.1], ipSrc.arIP [192.170.0.0], ipSrcPrefixLen [8]. */

    ipGroup.arIP[0] = 225;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;


    ipSrc.arIP[0] = 192;
    ipSrc.arIP[1] = 170;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 0;

    ipSrcPrefixLen = 8;

    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryDelete(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, ipSrcPrefixLen = %d",
                                 lpmDbId, vrId, ipSrcPrefixLen);


    /*
        1.4.1. Call with ipGroup.arIP [224.1.1.1],
                       ipSrc.arIP [10.17.0.1], (not exists already)
                       ipSrcPrefixLen [32].
        Expected: GT_NOT_FOUND.
    */
    ipGroup.arIP[0] = 224;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 10;
    ipSrc.arIP[1] = 17;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 1;

    ipSrcPrefixLen = 32;

    st = cpssExMxPmIpLpmIpv4McEntryDelete(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_NOT_FOUND, st, "%d, %d, ipSrcPrefixLen = %d",
                                 lpmDbId, vrId, ipSrcPrefixLen);


    /*
        1.5. Call with lpmDbId [100] (not exist)
                       and other parameters from 1.4.
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntryDelete(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.6. Call with vrId [100] (not exist)
                       and other parameters from 1.4.
        Expected: NOT GT_OK.
    */
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntryDelete(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    vrId = 0;

    /*
        1.7. Call cpssExMxPmIpLpmIpv4McEntriesFlush with lpmDbId [1],
                                                         vrId [0] to invalidate changes.
        Expected: GT_OK.
    */
    /* Delete Entry */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntriesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4McEntriesFlush: %d, %d", lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv4McEntriesFlush
(
    IN  GT_U32  lpmDbId,
    IN  GT_U32  vrId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4McEntriesFlush)
{
/*
    1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                       and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                        sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                        CPSS_SRAM_SIZE_512KB_E],
                                                        numOfSramsSizes [3 / 5]},
                                           numOfMemCfg [1],
                                           protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                           ipv6MemShare [50 / 0],
                                           numOfVirtualRouters [1 / 32]}.
    Expected: GT_OK.

    1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                   vrId [0 / 0]
                   and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                    defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportMcIpv4 [GT_TRUE],
                                    defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                    supportUcIpv6 [GT_TRUE],
                                    defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                             blockSize [0 / 1],
                                                             routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                               CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}}.
    Expected: GT_OK.

    1.3. Call cpssExMxPmIpLpmIpv4McEntryAdd with ipGroup.arIP [224.1.1.1 / 225.1.1.1],
                                                 ipSrc.arIP [10.17.0.1 /192.168.0.1 / 192.170.0.0],
                                                 ipSrcPrefixLen [32 / 24 / 8],
                                                 mcRoutePointerPtr {routeEntryBaseMemAddr [0 / 0xFF],
                                                                    blockSize [1],
                                                                    routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                                 override [GT_FALSE / GT_TRUE].
    Expected: GT_OK.

    1.4. Call  cpssExMxPmIpLpmIpv4McEntryGet with lpmDbId [1], vrId [0], ipGroup [225.1.1.1],
                ipSrc[192.170.0.0], ipSrcPrefixLen [8], non-NULL mcRoutePointerPtr.
    Expected: GT_OK.
    1.5. Call with lpmDbId [1]
                   and vrId [0].
    Expected: GT_OK.
    1.6. Call  cpssExMxPmIpLpmIpv4McEntryGet with lpmDbId [1], vrId [0], ipGroup [225.1.1.1],
                ipSrc[192.170.0.0], ipSrcPrefixLen [8], non-NULL mcRoutePointerPtr.
    Expected: GT_NOT_FOUND. (already flushed)
    1.7. Call with lpmDbId [100] (not exist)
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.8. Call with vrId [100] (not exist)
                   and other parameters from 1.3.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPADDR                               ipGroup;
    GT_IPADDR                               ipSrc;
    GT_U32                                  ipSrcPrefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  mcRoutePointer;
    GT_BOOL                                 override       = GT_FALSE;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                   CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.

        1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}}.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");


    /*
        1.3. Call cpssExMxPmIpLpmIpv4McEntryAdd with ipGroup.arIP [224.1.1.1 / 225.1.1.1],
                                                     ipSrc.arIP [10.17.0.1 /192.168.0.1 / 192.170.0.0],
                                                     ipSrcPrefixLen [32 / 24 / 8],
                                                     mcRoutePointerPtr {routeEntryBaseMemAddr [0 / 0xFF],
                                                                        blockSize [1],
                                                                        routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                                     override [GT_FALSE / GT_TRUE].
        Expected: GT_OK.
    */
    /*Call with ipGroup.arIP [224.1.1.1], ipSrc.arIP [10.17.0.1], ipSrcPrefixLen [32]. */

    ipGroup.arIP[0] = 224;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 10;
    ipSrc.arIP[1] = 17;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 1;

    ipSrcPrefixLen = 32;

    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4McEntryAdd: %d, %d, ipSrcPrefixLen = %d", lpmDbId, vrId, ipSrcPrefixLen);


    /*Call with ipGroup.arIP [224.1.1.1], ipSrc.arIP [192.168.0.1], ipSrcPrefixLen [24]. */

    ipSrc.arIP[0] = 192;
    ipSrc.arIP[1] = 168;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 1;

    ipSrcPrefixLen = 24;

    mcRoutePointer.routeEntryBaseMemAddr = 0;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4McEntryAdd: %d, %d, ipSrcPrefixLen = %d", lpmDbId, vrId, ipSrcPrefixLen);


    /*Call with ipGroup.arIP [225.1.1.1], ipSrc.arIP [192.170.0.0], ipSrcPrefixLen [8]. */

    ipGroup.arIP[0] = 225;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;


    ipSrc.arIP[0] = 192;
    ipSrc.arIP[1] = 170;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 0;

    ipSrcPrefixLen = 8;

    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4McEntryAdd: %d, %d, ipSrcPrefixLen = %d", lpmDbId, vrId, ipSrcPrefixLen);


    /*
    1.4. Call  cpssExMxPmIpLpmIpv4McEntryGet with lpmDbId [1], vrId [0], ipGroup [225.1.1.1],
                ipSrc[10.10.0.0], ipSrcPrefixLen [16], non-NULL mcRoutePointerPtr.
    Expected: GT_OK.
    */

    ipGroup.arIP[0] = 225;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 192;
    ipSrc.arIP[1] = 170;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 0;

    ipSrcPrefixLen = 8;

    st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*
        1.5. Call with lpmDbId [1]
                       and vrId [0]
        Expected: GT_OK.
    */
    lpmDbId = 1;
    vrId    = 0;

    st = cpssExMxPmIpLpmIpv4McEntriesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*
    1.6. Call  cpssExMxPmIpLpmIpv4McEntryGet with lpmDbId [1], vrId [0], ipGroup [225.1.1.1],
                ipSrc[192.170.0.0], ipSrcPrefixLen [8], non-NULL mcRoutePointerPtr.
    Expected: GT_NOT_FOUND.
    */

    ipGroup.arIP[0] = 225;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 192;
    ipSrc.arIP[1] = 170;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 0;

    ipSrcPrefixLen = 8;

    st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_FOUND, st, lpmDbId, vrId);

    /*
        1.7. Call with lpmDbId [100] (not exist)
                       and vrId [0].
        Expected: NOT GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntriesFlush(lpmDbId, vrId);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);

    lpmDbId = 1;

    /*
        1.8. Call with lpmDbId [1]
                       and vrId [100].
        Expected: NOT GT_OK.
    */
    /* Delete Entry */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntriesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4McEntriesFlush: %d, %d", lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv4McEntryGet
(
    IN  GT_U32                                  lpmDbId,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipGroup,
    IN  GT_IPADDR                               ipSrc,
    IN  GT_U32                                  ipSrcPrefixLen,
    OUT CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  *mcRoutePointerPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4McEntryGet)
{
/*
(no test pattern, no device iteration)
1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                   and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                    sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                    CPSS_SRAM_SIZE_512KB_E],
                                                    numOfSramsSizes [3 / 5]},
                                       numOfMemCfg [1],
                                       protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                       ipv6MemShare [50 / 0],
                                       numOfVirtualRouters [1 / 32]}.
Expected: GT_OK.

1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
               vrId [0 / 0]
               and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                         blockSize [0 / 1],
                                                         routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                           CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                supportMcIpv4 [GT_TRUE],
                                defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                         blockSize [0 / 1],
                                                         routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                           CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                supportUcIpv6 [GT_TRUE],
                                defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                         blockSize [0 / 1],
                                                         routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                           CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}}.
Expected: GT_OK.

1.3. Call cpssExMxPmIpLpmIpv4McEntryAdd with lpmDbId [1], vrId [0], ipGroup.arIP [224.1.1.1 / 225.1.1.1],
                                             ipSrc.arIP [10.17.0.1 /192.168.0.1 / 192.170.0.0],
                                             ipSrcPrefixLen [32 / 24 / 8],
                                             mcRoutePointerPtr {routeEntryBaseMemAddr [0 / 0xFF],
                                                                blockSize [1],
                                                                routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                             override [GT_FALSE / GT_TRUE].
Expected: GT_OK.

1.4. Call  cpssExMxPmIpLpmIpv4McEntryGet with lpmDbId [1], vrId [0], ipGroup.arIP [224.1.1.1 / 225.1.1.1],
                                              ipSrc.arIP [10.17.0.1 /192.168.0.1 / 192.170.0.0],
                                              ipSrcPrefixLen [32 / 24 / 8] and non-NULL mcRoutePointerPtr.
Expected: GT_OK.

1.5. Call cpssExMxPmIpLpmIpv4McEntriesFlush with lpmDbId [1] and vrId [0].
Expected: GT_OK.

1.6. Call  cpssExMxPmIpLpmIpv4McEntryGet with lpmDbId [1], vrId [0], ipGroup.arIP [224.1.1.1 / 225.1.1.1],
                                             ipSrc.arIP [10.17.0.1 /192.168.0.1 / 192.170.0.0],
                                             ipSrcPrefixLen [32 / 24 / 8] non-NULL mcRoutePointerPtr.
Expected: GT_NOT_FOUND. (already flushed)

1.7. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent),
other params same as in 4.
Expected: non GT_OK.

1.8. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent),
other params same as in 4.
Expected: non GT_OK.

--- incorrect params
1.9. Call function with mcRoutePointerPtr [NULL], other params same as in 4.
Expected: GT_BAD_PTR.

--- delete VR/DB
1.10. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0].
Expected: GT_OK.

1.11. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1].
Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPADDR                               ipGroup;
    GT_IPADDR                               ipSrc;
    GT_U32                                  ipSrcPrefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  mcRoutePointer;
    GT_BOOL                                 override       = GT_FALSE;


    /*
        1.1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                   CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.

        1.2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}}.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");


    /*
        1.3. Call cpssExMxPmIpLpmIpv4McEntryAdd with ipGroup.arIP [224.1.1.1 / 225.1.1.1],
                                                     ipSrc.arIP [10.17.0.1 /192.168.0.1 / 192.170.0.0],
                                                     ipSrcPrefixLen [32 / 24 / 8],
                                                     mcRoutePointerPtr {routeEntryBaseMemAddr [0 / 0xFF],
                                                                        blockSize [1],
                                                                        routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                                     override [GT_FALSE / GT_TRUE].
        Expected: GT_OK.
    */
    /*Call with ipGroup.arIP [224.1.1.1], ipSrc.arIP [10.17.0.1], ipSrcPrefixLen [32]. */

    ipGroup.arIP[0] = 224;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 10;
    ipSrc.arIP[1] = 17;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 1;

    ipSrcPrefixLen = 32;

    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4McEntryAdd: %d, %d, ipSrcPrefixLen = %d", lpmDbId, vrId, ipSrcPrefixLen);


    /*Call with ipGroup.arIP [224.1.1.1], ipSrc.arIP [192.168.0.1], ipSrcPrefixLen [24]. */

    ipSrc.arIP[0] = 192;
    ipSrc.arIP[1] = 168;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 1;

    ipSrcPrefixLen = 24;

    mcRoutePointer.routeEntryBaseMemAddr = 0;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4McEntryAdd: %d, %d, ipSrcPrefixLen = %d", lpmDbId, vrId, ipSrcPrefixLen);


    /*Call with ipGroup.arIP [225.1.1.1], ipSrc.arIP [192.170.0.0], ipSrcPrefixLen [8]. */

    ipGroup.arIP[0] = 225;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 192;
    ipSrc.arIP[1] = 170;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 0;

    ipSrcPrefixLen = 8;

    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
               "cpssExMxPmIpLpmIpv4McEntryAdd: %d, %d, ipSrcPrefixLen = %d", lpmDbId, vrId, ipSrcPrefixLen);

    /*
    1.4. Call  cpssExMxPmIpLpmIpv4McEntryGet with lpmDbId [1], vrId [0], ipGroup.arIP [224.1.1.1 / 225.1.1.1],
                                                  ipSrc.arIP [10.17.0.1 /192.168.0.1 / 192.170.0.0],
                                                  ipSrcPrefixLen [32 / 24 / 8] and non-NULL mcRoutePointerPtr.
    Expected: GT_OK.
    */

     /*Call with ipGroup.arIP [224.1.1.1], ipSrc.arIP [10.17.0.1], ipSrcPrefixLen [32]. */

    ipGroup.arIP[0] = 224;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 10;
    ipSrc.arIP[1] = 17;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 1;

    ipSrcPrefixLen = 32;

    st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);


    /*Call with ipGroup.arIP [224.1.1.1], ipSrc.arIP [192.168.0.1], ipSrcPrefixLen [24]. */

    ipSrc.arIP[0] = 192;
    ipSrc.arIP[1] = 168;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 1;

    ipSrcPrefixLen = 24;

    st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);


    /*Call with ipGroup.arIP [225.1.1.1], ipSrc.arIP [192.170.0.0], ipSrcPrefixLen [8]. */

    ipGroup.arIP[0] = 225;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 192;
    ipSrc.arIP[1] = 170;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 0;

    ipSrcPrefixLen = 8;

    st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*
        1.5. Call cpssExMxPmIpLpmIpv4McEntriesFlush  with lpmDbId [1]
                       and vrId [0]
        Expected: GT_OK.
    */
    lpmDbId = 1;
    vrId    = 0;

    st = cpssExMxPmIpLpmIpv4McEntriesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*
    1.6. Call  cpssExMxPmIpLpmIpv4McEntryGet with lpmDbId [1], vrId [0], ipGroup [225.1.1.1],
                ipSrc[192.170.0.0], ipSrcPrefixLen [8], non-NULL mcRoutePointerPtr.
    Expected: GT_NOT_FOUND. (already flushed)
    */

    ipGroup.arIP[0] = 225;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 192;
    ipSrc.arIP[1] = 170;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 0;

    ipSrcPrefixLen = 8;

    st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_FOUND, st, lpmDbId, vrId);

    /*--- invalid ID's:*/
    /*
    1.7. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent),
    other params same as in 1.4.
    Expected: non GT_OK.
    */
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "lpmDbId = %d", lpmDbId);

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /*
    1.8. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent),
    other params same as in 1.4.
    Expected: non GT_OK.
    */

    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "vrId = %d", vrId);

    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /*--- incorrect params*/
    /*
    1.9. Call function with mcRoutePointerPtr [NULL], other params same as in 1.4.
    Expected: GT_BAD_PTR.
    */

    st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mcRoutePointerPtr = NULL", lpmDbId);

    /* Delete Entry */

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntriesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4McEntriesFlush: %d, %d", lpmDbId, vrId);

    /* Delete default virtual router and Lpm Db */

    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv4McEntryGetNext
(
    IN    GT_U32                                    lpmDbId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPADDR                                 *ipGroupPtr,
    INOUT GT_IPADDR                                 *ipSrcPtr,
    INOUT GT_U32                                    *ipSrcPrefixLenPtr,
    OUT   CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC    *mcRoutePointerPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4McEntryGetNext)
{
/*
(no test pattern, no device iteration)
1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                   and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_EXTERNAL_E],
                                                    sramsSizeArray [CPSS_SRAM_SIZE_2MB_E /
                                                                    CPSS_SRAM_SIZE_512KB_E],
                                                    numOfSramsSizes [3 / 5]},
                                       numOfMemCfg [1],
                                       protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                       ipv6MemShare [50 / 0],
                                       numOfVirtualRouters [1 / 32]}.
Expected: GT_OK.

2. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
               vrId [0 / 0]
               and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                         blockSize [0 / 1],
                                                         routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                           CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                supportMcIpv4 [GT_TRUE],
                                defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                         blockSize [0 / 1],
                                                         routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                           CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                supportUcIpv6 [GT_TRUE],
                                defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                         blockSize [0 / 1],
                                                         routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                           CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}}.
Expected: GT_OK.

3. Call cpssExMxPmIpLpmIpv4McEntryAdd with lpmDbId [1], vrId [0], ipGroup.arIP [224.1.1.1],
                                             ipSrc.arIP [10.17.0.1 / 10.17.0.2],
                                             ipSrcPrefixLen [16],
                                             mcRoutePointerPtr {routeEntryBaseMemAddr [0 / 0xFF],
                                                                blockSize [1],
                                                                routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                             override [GT_FALSE].
Expected: GT_OK.

4. Call  cpssExMxPmIpLpmIpv4McEntryGetNext with lpmDbId [1], vrId [0], ipGroup.arIP [0.0.0.0],
                                              ipSrc.arIP [0.0.0.0], (twice)
                                              ipSrcPrefixLen [16] and non-NULL mcRoutePointerPtr.
Expected: GT_OK.

4.1. Call  cpssExMxPmIpLpmIpv4McEntryGetNext with lpmDbId [1], vrId [0], ipGroup.arIP [0.0.0.0],
                                              ipSrc.arIP [0.0.0.0], (third prefix not exists)
                                              ipSrcPrefixLen [16] and non-NULL mcRoutePointerPtr.
Expected: GT_NOT_FOUND.
--- invalid ID's:
5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 4.
Expected: non GT_OK.
6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 4.
Expected: non GT_OK.
--- incorrect params
7. Call function with ipGroupPtr [NULL], other params same as in 4.
Expected: GT_BAD_PTR.
8. Call function with ipSrcPtr [NULL], other params same as in 4.
Expected: GT_BAD_PTR.
9. Call function with ipSrcPrefixLenPtr [NULL], other params same as in 4.
Expected: GT_BAD_PTR.
10. Call function with mcRoutePointerPtr [NULL], other params same as in 4.
Expected: GT_BAD_PTR.
--- delete VR/DB
11. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0].
Expected: GT_OK.
12. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1].
Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U32                  lpmDbId;
    GT_U32                  vrId;
    GT_IPADDR               ipGroup;
    GT_IPADDR               ipSrc;
    GT_U32                  ipSrcPrefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  mcRoutePointer;
    GT_BOOL                 override;

    /* set default DB id and VR id*/
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /*
        1. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1 / 2]
                                               and lpmDbConfigPtr {memCfgArray {routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E /
                                                                                                   CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E CPSS_EXMXPM_LPM_SRAM_NOT_EXISTS_E],
                                                                                sramsSizeArray [CPSS_SRAM_SIZE_32KB_E /
                                                                                                CPSS_SRAM_SIZE_256KB_E],
                                                                                numOfSramsSizes [1 / 0]},
                                                                   numOfMemCfg [1],
                                                                   protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                                                  CPSS_IP_PROTOCOL_IPV4V6_E],
                                                                   ipv6MemShare [50 / 0],
                                                                   numOfVirtualRouters [1 / 2]} to create DB.
        Expected: GT_OK.

        1. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId [1 / 2],
                       vrId [0 / 0]
                       and vrConfigPtr {supportUcIpv4 [GT_TRUE],
                                        defaultUcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportMcIpv4 [GT_TRUE],
                                        defaultMcIpv4RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                        supportUcIpv6 [GT_TRUE],
                                        defaultUcIpv6RouteEntry {routeEntryBaseMemAddr [0 / 0xFF],
                                                                 blockSize [0 / 1],
                                                                 routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
                                                                                   CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}}.
        Expected: GT_OK.
    */


    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");

    /*
    3. Call cpssExMxPmIpLpmIpv4McEntryAdd with lpmDbId [1], vrId [0], ipGroup.arIP [224.1.1.1],
                                                 ipSrc.arIP [10.17.0.1 / 10.17.0.2],
                                                 ipSrcPrefixLen [16],
                                                 mcRoutePointerPtr {routeEntryBaseMemAddr [0 / 0xFF],
                                                                    blockSize [1],
                                                                    routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                                 override [GT_FALSE].
    Expected: GT_OK.
    */
    /*Call with ipGroup.arIP [224.1.1.1], ipSrc.arIP [10.17.0.1], ipSrcPrefixLen [16]. */

    ipGroup.arIP[0] = 224;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 10;
    ipSrc.arIP[1] = 10;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 1;

    ipSrcPrefixLen = 16;
    mcRoutePointer.routeEntryBaseMemAddr = 0;
    mcRoutePointer.blockSize = 1;
    mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4McEntryAdd: %d, %d, prefixLen = %d",
                                  lpmDbId, vrId, ipSrcPrefixLen);

    /*Call with ipGroup.arIP [224.1.1.1], ipSrc.arIP [10.17.0.2], ipSrcPrefixLen [16]. */

    ipGroup.arIP[0] = 224;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0] = 10;
    ipSrc.arIP[1] = 11;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 2;

    ipSrcPrefixLen = 16;
    mcRoutePointer.routeEntryBaseMemAddr = 0;
    mcRoutePointer.blockSize = 1;
    mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4McEntryAdd: %d, %d, prefixLen = %d",
                                  lpmDbId, vrId, ipSrcPrefixLen);

    /*--- correct call:*/

    /*
    4. Call  cpssExMxPmIpLpmIpv4McEntryGetNext with lpmDbId [1], vrId [0], ipGroup.arIP [0.0.0.0],
                                                  ipSrc.arIP [0.0.0.0], (twice)
                                                  ipSrcPrefixLen [16] and non-NULL mcRoutePointerPtr.
    Expected: GT_OK.
    */
    /*Call first. */

    ipGroup.arIP[0] = 0;
    ipGroup.arIP[1] = 0;
    ipGroup.arIP[2] = 0;
    ipGroup.arIP[3] = 0;

    ipSrc.arIP[0] = 0;
    ipSrc.arIP[1] = 0;
    ipSrc.arIP[2] = 0;
    ipSrc.arIP[3] = 0;

    ipSrcPrefixLen = 16;

    st = cpssExMxPmIpLpmIpv4McEntryGetNext(lpmDbId, vrId, &ipGroup, &ipSrc, &ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*Call second. */

    st = cpssExMxPmIpLpmIpv4McEntryGetNext(lpmDbId, vrId, &ipGroup, &ipSrc, &ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*
    4.1. Call  cpssExMxPmIpLpmIpv4McEntryGetNext with lpmDbId [1], vrId [0], ipGroup.arIP [0.0.0.0],
                                                  ipSrc.arIP [0.0.0.0], (third prefix not exists)
                                                  ipSrcPrefixLen [16] and non-NULL mcRoutePointerPtr.
    Expected: GT_NOT_FOUND.
    */

    st = cpssExMxPmIpLpmIpv4McEntryGetNext(lpmDbId, vrId, &ipGroup, &ipSrc, &ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_FOUND, st, lpmDbId, vrId);


    /*--- invalid ID's:*/

    /*5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent),
    other params same as in 4. Expected: non GT_OK.*/

    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntryGetNext(lpmDbId, vrId, &ipGroup, &ipSrc, &ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "lpmDbId = %d", lpmDbId);

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /*6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent),
    other params same as in 4. Expected: non GT_OK.*/

    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntryGetNext(lpmDbId, vrId, &ipGroup, &ipSrc, &ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "vrId = %d", vrId);

    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /*--- incorrect params*/

    /*7. Call function with ipGroupPtr [NULL], other params same as in 4.
    Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv4McEntryGetNext(lpmDbId, vrId, NULL, &ipSrc, &ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipGroupPtr = NULL", lpmDbId);

    /*8. Call function with ipSrcPtr [NULL], other params same as in 4.
    Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv4McEntryGetNext(lpmDbId, vrId, &ipGroup, NULL, &ipSrcPrefixLen, &mcRoutePointer);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipSrcPtr = NULL", lpmDbId);

    /*9. Call function with ipSrcPrefixLenPtr [NULL], other params same as
    in 4. Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv4McEntryGetNext(lpmDbId, vrId, &ipGroup, &ipSrc, NULL, &mcRoutePointer);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipSrcPrefixLenPtr = NULL", lpmDbId);

    /*10. Call function with mcRoutePointerPtr [NULL], other params same as
    in 4. Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv4McEntryGetNext(lpmDbId, vrId, &ipGroup, &ipSrc, &ipSrcPrefixLen, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mcRoutePointerPtr = NULL", lpmDbId);

    /* Delete Entry */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntriesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4McEntriesFlush: %d, %d", lpmDbId, vrId);

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv6UcPrefixAdd
(
    IN  GT_U32                                  lpmDbId,
    IN  GT_U32                                  vrId,
    IN  GT_IPV6ADDR                             *ipAddrPtr,
    IN  GT_U32                                  prefixLen,
    IN  CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  *nextHopPointer,
    IN  GT_BOOL                                 override
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6UcPrefixAdd)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Create virtual router. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId[1], vrId [0], vrConfigPtr { supportUcIpv4 [GT_TRUE], defaultUcIpv4RouteEntry { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]} ; supportMcIpv4 [GT_TRUE], defaultMcIpv4RouteEntry { same as defaultUcIpv4RouteEntry } ; supportUcIpv6 [GT_TRUE], defaultUcIpv6RouteEntry { same as defaultUcIpv4RouteEntry } }. Expected: GT_OK.
--- correct calls:
3. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: ], prefixLen [32], nextHopPointer { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},  override [GT_FALSE]. Expected: GT_OK.
4. Call with all params same as in 3. (prefix already exist). Expected: GT_ALREADY_EXIST.
5. Call with override [GT_TRUE], nextHopPointer { blockSize [2 / 5 / 1], routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E  / CPSS_EXMXPM_IP_COS_ROUTE_ENTRY_GROUP_E / CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E] }, other params same as in 3. Expected: GT_OK.
6. Call with override [GT_TRUE], ipAddrPtr [::all arIP = 30:: ], other params same as in 3. (override non-existent). Expected: non GT_OK.
7. Change default prefix. Call with prefixLen [0], override [GT_TRUE / GT_FALSE], other params same as in 3. Expected: GT_OK
--- invalid ID's:
8. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
9. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK.
--- incorrect params
10. Call with invalid enum nextHopPointer.routeEntryMethod [0x5AAAAA5], override [GT_TRUE], other params same as in 5. Expected: GT_BAD_PARAM.
11. Call function with ipAddrPtr [NULL], other params same as in 5. Expected: GT_BAD_PTR.
12. Call function with nextHopPointerPtr [NULL], other params same as in 5. Expected: GT_BAD_PTR.
--- delete VR/DB
13. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0]. Expected: GT_OK.
14. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U32                  lpmDbId;
    GT_U32                  vrId;
    GT_IPV6ADDR             ipAddr;
    GT_U32                  prefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                 override;

    /* set default DB id and VR id*/
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");

    /*--- correct calls:*/

    /*3. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: ],
    prefixLen [32], nextHopPointer { routeEntryBaseMemAddr [0], blockSize
    [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}, override
    [GT_FALSE]. Expected: GT_OK. */

    cpssOsMemSet((GT_VOID*) &(ipAddr), 20, sizeof(ipAddr));
    prefixLen = 32;
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize = 1;
    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d, override = %d",
                                  lpmDbId, vrId, prefixLen, override);

    /*4. Call with all params same as in 3. (prefix already exist).
    Expected: GT_ALREADY_EXIST.*/

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_ALREADY_EXIST, st, "%d, %d, prefixLen = %d, override = %d",
                                  lpmDbId, vrId, prefixLen, override);

    /*5. Call with override [GT_TRUE], nextHopPointer { blockSize [2 / 5 /
    1], routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E /
    CPSS_EXMXPM_IP_COS_ROUTE_ENTRY_GROUP_E /
    CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E] }, other params same as in 3.
    Expected: GT_OK.*/

    override = GT_TRUE;
    nextHopPointer.blockSize = 2;
    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d, override = %d",
                                  lpmDbId, vrId, prefixLen, override);

    nextHopPointer.blockSize = 5;
    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_COS_ROUTE_ENTRY_GROUP_E;

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d, override = %d",
                                  lpmDbId, vrId, prefixLen, override);

    nextHopPointer.blockSize = 1;
    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d, override = %d",
                                  lpmDbId, vrId, prefixLen, override);


    /*6. Call with override [GT_TRUE], ipAddrPtr [::all arIP = 30:: ], other
    params same as in 3. (override non-existent). Expected: non GT_OK.*/

    cpssOsMemSet((GT_VOID*) &(ipAddr), 30, sizeof(ipAddr));

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d, override = %d",
                                  lpmDbId, vrId, prefixLen, override);

    /*7. Change default prefix. Call with prefixLen [0], override [GT_TRUE /
    GT_FALSE], other params same as in 3. Expected: GT_OK*/

    cpssOsMemSet((GT_VOID*) &(ipAddr), 20, sizeof(ipAddr));
    prefixLen = 0;
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize = 1;
    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    override = GT_TRUE;

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d, override = %d",
                                  lpmDbId, vrId, prefixLen, override);

    prefixLen = 32;

    /*--- invalid ID's:*/

    /*8. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent),
    other params same as in 3. Expected: non GT_OK.*/

    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "lpmDbId = %d", lpmDbId);

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /*9. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent),
    other params same as in 3. Expected: non GT_OK.*/

    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "vrId = %d", vrId);

    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /*--- incorrect params*/

    /*10. Call with invalid enum nextHopPointer.routeEntryMethod
    [0x5AAAAA5], override [GT_TRUE], other params same as in 5. Expected:
    GT_BAD_PARAM.*/

    override = GT_TRUE;
    nextHopPointer.routeEntryMethod = IP_LPM_INVALID_ENUM_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nextHopPointer.routeEntryMethod = %d",
                                  lpmDbId, nextHopPointer.routeEntryMethod);

    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    /*11. Call function with ipAddrPtr [NULL], other params same as in 5.
    Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, NULL, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipAddrPtr = NULL", lpmDbId);

    /*12. Call function with nextHopPointerPtr [NULL], other params same as
    in 5. Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, NULL, override);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, nextHopPointerPtr = NULL", lpmDbId);

    /* Delete Entry */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}

/*******************************************************************************
* Prefix IPV6 add, check,  delete, check memory leakages
1. Create defauld LpmDB and Virtual Routers.
Expected: GT_OK.
2. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_before_IPV6_Prefix.
3. Call cpssExMxPmIpLpmIPV6UcPrefixAdd to fill table of prefixes
Expected: GT_OK.
4. Call cpssExMxPmIpLpmIPV6UcPrefixGet to check prefixes
Expected: GT_OK.
5. Call cpssExMxPmIpLpmIPV6UcPrefixDelete to delete all prefixes
Expected: GT_OK.
6. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_IPV6_Prefix.
Expected: (heap_bytes_allocated_after_IPV6_Prefix-heap_bytes_allocated_before_IPV6_Prefix) = 0
7. Delete defauld LpmDB and Virtual Routers
Expected: GT_OK.
*******************************************************************************/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6UcPrefixMemoryCheck)
{
    GT_STATUS   st        = GT_OK;

    GT_U32      lpmDbId = 0;
    GT_U32      vrId    = 0;
    GT_U32      ii      = 0;

    GT_U32      heap_bytes_allocated_before_IPV6_Prefix = 0;
    GT_U32      heap_bytes_allocated_after_IPV6_Prefix = 0;

    GT_U32      prefixLen = 0;
    GT_BOOL     override = GT_FALSE;

    GT_IPV6ADDR ipAddr;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointerGet;


    cpssOsBzero((GT_VOID*) &ipAddr, sizeof(ipAddr));
    cpssOsBzero((GT_VOID*) &nextHopPointer, sizeof(nextHopPointer));
    cpssOsBzero((GT_VOID*) &nextHopPointerGet, sizeof(nextHopPointerGet));

    /*
        1. Create defauld LpmDB and Virtual Routers.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        2. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_before_IPV6_Prefix.
    */
    heap_bytes_allocated_before_IPV6_Prefix = osMemGetHeapBytesAllocated();

    for(ii=0; ii<IP_LPM_ITER_COUNT; ++ii)
    {
        /*
            3. Call cpssExMxPmIpLpmIPV6UcPrefixAdd to fill table of prefixes
                              with lpmDbId [1 .. max].
                                   vrId [0 .. max]
                                   ipAddrPtr [::all arIP = 20:: ],
                                   prefixLen [32],
                                   nextHopPointer { routeEntryBaseMemAddr [0],
                                                    blockSize [1],
                                                    routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                   override [GT_FALSE].
            Expected: GT_OK.
        */
        cpssOsMemSet((GT_VOID*) &(ipAddr), 20, sizeof(ipAddr));
        prefixLen = 32;
        nextHopPointer.routeEntryBaseMemAddr = 0;
        nextHopPointer.blockSize = 1;
        nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
        override = GT_FALSE;

        /* iterate with lpmDbId [1], vrId[0] */
        lpmDbId = 1;
        vrId    = 0;

        cpssOsMemSet((GT_VOID*) &(ipAddr), 0x11, sizeof(ipAddr));

        st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixAdd: %d, %d, prefixLen = %d, override = %d",
                                     lpmDbId, vrId, prefixLen, override);

        /* iterate with lpmDbId [1], vrId[1] -> Not support IPV6! */
        lpmDbId = 1;
        vrId    = 1;

        cpssOsMemSet((GT_VOID*) &(ipAddr), 0x22, sizeof(ipAddr));

        st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixAdd: %d, %d, prefixLen = %d, override = %d",
                                     lpmDbId, vrId, prefixLen, override);

        /* iterate with lpmDbId [2], vrId[0] */
        lpmDbId = 2;
        vrId    = 0;

        cpssOsMemSet((GT_VOID*) &(ipAddr), 0x33, sizeof(ipAddr));

        st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixAdd: %d, %d, prefixLen = %d, override = %d",
                                     lpmDbId, vrId, prefixLen, override);

        /*
            4. Call cpssExMxPmIpLpmIPV6UcPrefixGet to check prefixes
                                                  with lpmDbId [1 .. max].
                                                       vrId [0 .. max]
                                                       ipAddrPtr [::all arIP = 20:: ],
                                                       prefixLen [32],
                                                       and not NULL nextHopPointerPtr
            Expected: GT_OK and the same nextHopPointer as was set.
        */
        /* iterate with lpmDbId [1], vrId[0] */
        lpmDbId = 1;
        vrId    = 0;

        cpssOsMemSet((GT_VOID*) &(ipAddr), 0x11, sizeof(ipAddr));
        cpssOsBzero((GT_VOID*) &nextHopPointerGet, sizeof(nextHopPointerGet));

        st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointerGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixGet:%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

        if(st == GT_OK)
        {
            /* verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryBaseMemAddr, nextHopPointerGet.routeEntryBaseMemAddr,
                       "get another nextHopPointerPtr->routeEntryBaseMemAddr than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.blockSize, nextHopPointerGet.blockSize,
                       "get another nextHopPointerPtr->blockSize than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryMethod, nextHopPointerGet.routeEntryMethod,
                       "get another nextHopPointerPtr->routeEntryMethod than was set: %d, %d", lpmDbId, vrId);
        }

        /* iterate with lpmDbId [1], vrId[1] -> Not support IPV6! */
        lpmDbId = 1;
        vrId    = 1;

        cpssOsMemSet((GT_VOID*) &(ipAddr), 0x22, sizeof(ipAddr));
        cpssOsBzero((GT_VOID*) &nextHopPointerGet, sizeof(nextHopPointerGet));

        st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointerGet);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixGet:%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

        if(st == GT_OK)
        {
            /* verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryBaseMemAddr, nextHopPointerGet.routeEntryBaseMemAddr,
                       "get another nextHopPointerPtr->routeEntryBaseMemAddr than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.blockSize, nextHopPointerGet.blockSize,
                       "get another nextHopPointerPtr->blockSize than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryMethod, nextHopPointerGet.routeEntryMethod,
                       "get another nextHopPointerPtr->routeEntryMethod than was set: %d, %d", lpmDbId, vrId);
        }

        /* iterate with lpmDbId [2], vrId[0] */
        lpmDbId = 2;
        vrId    = 0;

        cpssOsMemSet((GT_VOID*) &(ipAddr), 0x33, sizeof(ipAddr));
        cpssOsBzero((GT_VOID*) &nextHopPointerGet, sizeof(nextHopPointerGet));

        st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointerGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixGet:%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

        if(st == GT_OK)
        {
            /* verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryBaseMemAddr, nextHopPointerGet.routeEntryBaseMemAddr,
                       "get another nextHopPointerPtr->routeEntryBaseMemAddr than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.blockSize, nextHopPointerGet.blockSize,
                       "get another nextHopPointerPtr->blockSize than was set: %d, %d", lpmDbId, vrId);
            UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryMethod, nextHopPointerGet.routeEntryMethod,
                       "get another nextHopPointerPtr->routeEntryMethod than was set: %d, %d", lpmDbId, vrId);
        }

        /*
            5. Call cpssExMxPmIpLpmIPV6UcPrefixDelete to delete all prefixes
            Expected: GT_OK.
        */
        /* iterate with lpmDbId [1], vrId[0] */
        lpmDbId = 1;
        vrId    = 0;

        cpssOsMemSet((GT_VOID*) &(ipAddr), 0x11, sizeof(ipAddr));

        st = cpssExMxPmIpLpmIpv6UcPrefixDelete(lpmDbId, vrId, &ipAddr, prefixLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixDelete: %d, %d, prefixLen = %d",
                                     lpmDbId, vrId, prefixLen);

        /* iterate with lpmDbId [1], vrId[1] -> Not support IPV6! */
        /*lpmDbId = 1;
        vrId    = 1;

        cpssOsMemSet((GT_VOID*) &(ipAddr), 0x22, sizeof(ipAddr));

        st = cpssExMxPmIpLpmIpv6UcPrefixDelete(lpmDbId, vrId, &ipAddr, prefixLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixDelete: %d, %d, prefixLen = %d",
                                     lpmDbId, vrId, prefixLen); */

        /* iterate with lpmDbId [2], vrId[0] */
        lpmDbId = 2;
        vrId    = 0;

        cpssOsMemSet((GT_VOID*) &(ipAddr), 0x33, sizeof(ipAddr));

        st = cpssExMxPmIpLpmIpv6UcPrefixDelete(lpmDbId, vrId, &ipAddr, prefixLen);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixDelete: %d, %d, prefixLen = %d",
                                     lpmDbId, vrId, prefixLen);
    }/* for */

    /*
        6. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_after_Router.
        Expected: (heap_bytes_allocated_after_IPV6_Prefix - heap_bytes_allocated_before_IPV6_Prefix) = 0
    */
    heap_bytes_allocated_after_IPV6_Prefix = osMemGetHeapBytesAllocated();
    UTF_VERIFY_EQUAL0_STRING_MAC(heap_bytes_allocated_before_IPV6_Prefix, heap_bytes_allocated_after_IPV6_Prefix,
                                 "size of heap changed (heap_bytes_allocated)");

    /*
        7. Delete defauld LpmDB and Virtual Routers.
        Expected: GT_OK.
    */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv6UcPrefixBulkAdd
(
    IN  GT_U32                                      lpmDbId,
    IN  GT_U32                                      ipv6PrefixArrayLen,
    IN  CPSS_EXMXPM_IP_LPM_IPV6_UC_ADD_PREFIX_STC   *ipv6PrefixArrayPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6UcPrefixBulkAdd)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Create virtual router. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId[1], vrId [0], vrConfigPtr { supportUcIpv4 [GT_TRUE], defaultUcIpv4RouteEntry { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]} ; supportMcIpv4 [GT_TRUE], defaultMcIpv4RouteEntry { same as defaultUcIpv4RouteEntry } ; supportUcIpv6 [GT_TRUE], defaultUcIpv6RouteEntry { same as defaultUcIpv4RouteEntry } }. Expected: GT_OK.
--- correct calls:
3. Call with lpmDbId [1], ipv6PrefixArrayLen [1], ipv6PrefixArrayPtr[0]  { vrId [0], ipAddr [::all arIP = 20:: ], prefixLen [32], nextHopPointer { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},  override [GT_FALSE] }. Expected: GT_OK and ipv6PrefixArrayPtr[0].returnStatus GT_OK.
4. Call with all params same as in 3. (prefix already exist). Expected: non GT_OK and ipv6PrefixArrayPtr[0].returnStatus non GT_OK.
--- invalid ID's:
5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
6. Call with ipv6PrefixArrayPtr[0].vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK and ipv6PrefixArrayPtr[0].returnStatus - non GT_OK.
--- incorrect params
7. Call function with ipv6PrefixArrayPtr [NULL], other params same as in 3. Expected: GT_BAD_PTR.
--- delete VR/DB
8. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0]. Expected: GT_OK.
9. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U32                  lpmDbId;
    GT_U32                  vrId;
    GT_U32                  ipv6PrefixArrayLen;
    CPSS_EXMXPM_IP_LPM_IPV6_UC_ADD_PREFIX_STC  ipv6PrefixArray[1];
    GT_IPV6ADDR             ipAddr2;    /* {{20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20}}; */

    cpssOsMemSet((GT_VOID*) &(ipAddr2), 20, sizeof(ipAddr2));

    /* set default DB id and VR id*/
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");

    /*--- correct calls:*/

    /*3. Call with lpmDbId [1], ipv6PrefixArrayLen [1],
    ipv6PrefixArrayPtr[0] { vrId [0], ipAddr [::all arIP = 20:: ], prefixLen
    [32], nextHopPointer { routeEntryBaseMemAddr [0], blockSize [1],
    routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}, override
    [GT_FALSE] }. Expected: GT_OK and ipv6PrefixArrayPtr[0].returnStatus
    GT_OK.*/

    ipv6PrefixArrayLen = 1;

    ipv6PrefixArray[0].vrId = IP_LPM_DEFAULT_VR_ID_CNS;
    ipv6PrefixArray[0].ipAddr = ipAddr2;
    ipv6PrefixArray[0].prefixLen = 32;
    ipv6PrefixArray[0].nextHopPointer.routeEntryBaseMemAddr = 0;
    ipv6PrefixArray[0].nextHopPointer.blockSize = 1;
    ipv6PrefixArray[0].nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    ipv6PrefixArray[0].override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv6UcPrefixBulkAdd(lpmDbId, ipv6PrefixArrayLen, ipv6PrefixArray);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipv6PrefixArrayLen = %d",
                                  lpmDbId, ipv6PrefixArrayLen);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, ipv6PrefixArray[0].returnStatus,
                      "ipv6PrefixArray[0].returnStatus: %d (GT_OK expected)", ipv6PrefixArray[0].returnStatus);

    /*4. Call with all params same as in 3. (prefix already exist).
    Expected: non GT_OK and ipv6PrefixArrayPtr[0].returnStatus non GT_OK.*/

    st = cpssExMxPmIpLpmIpv6UcPrefixBulkAdd(lpmDbId, ipv6PrefixArrayLen, ipv6PrefixArray);
    UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipv6PrefixArrayLen = %d",
                                     lpmDbId, ipv6PrefixArrayLen);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, ipv6PrefixArray[0].returnStatus,
                      "ipv6PrefixArray[0].returnStatus: %d (non GT_OK expected)", ipv6PrefixArray[0].returnStatus);

    /*--- invalid ID's:*/

    /* to be this param correct for next calls */
    ipv6PrefixArray[0].override = GT_TRUE;

    /*5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent),
    other params same as in 3. Expected: non GT_OK.*/

    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixBulkAdd(lpmDbId, ipv6PrefixArrayLen, ipv6PrefixArray);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "lpmDbId = %d", lpmDbId);

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /*6. Call with ipv6PrefixArrayPtr[0].vrId [IP_LPM_INVALID_VR_ID_CNS =
    6666] (non-existent), other params same as in 3. Expected: non GT_OK and
    ipv6PrefixArrayPtr[0].returnStatus - non GT_OK.*/

    ipv6PrefixArray[0].vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixBulkAdd(lpmDbId, ipv6PrefixArrayLen, ipv6PrefixArray);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "ipv6PrefixArrayPtr[0].vrId = %d", ipv6PrefixArray[0].vrId);

    ipv6PrefixArray[0].vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /*--- incorrect params*/

    /*7. Call function with ipv6PrefixArrayPtr [NULL], other params same as
    in 3. Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6UcPrefixBulkAdd(lpmDbId, ipv6PrefixArrayLen, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipv6PrefixArrayPtr = NULL", lpmDbId);

    /* Delete Prefix */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv6UcPrefixDelete
(
    IN  GT_U32              lpmDbId,
    IN  GT_U32              vrId,
    IN  GT_IPV6ADDR         *ipAddrPtr,
    IN  GT_U32              prefixLen
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6UcPrefixDelete)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Create virtual router. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId[1], vrId [0], vrConfigPtr { supportUcIpv4 [GT_TRUE], defaultUcIpv4RouteEntry { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]} ; supportMcIpv4 [GT_TRUE], defaultMcIpv4RouteEntry { same as defaultUcIpv4RouteEntry } ; supportUcIpv6 [GT_TRUE], defaultUcIpv6RouteEntry { same as defaultUcIpv4RouteEntry } }. Expected: GT_OK.
3. Add prefix for testing. Call cpssExMxPmIpLpmIpv6UcPrefixAdd with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: ], prefixLen [32], nextHopPointer { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},  override [GT_FALSE]. Expected: GT_OK.
--- correct call:
4. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: ], prefixLen [32]. Expected: GT_OK.
5. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: / [::all arIP = 30::], prefixLen [32] (prefix already deleted / non-existent prefix). Expected: non GT_OK.
--- invalid ID's:
6. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 4. Expected: non GT_OK.
7. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 4. Expected: non GT_OK.
--- incorrect params
8. Try to delete default prefix (incorrect). Call with prefixLen [0], other params same as in 4. Expected: non GT_OK.
9. Call function with ipAddrPtr [NULL], other params same as in 4. Expected: GT_BAD_PTR.
--- delete VR/DB
10. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0]. Expected: GT_OK.
11. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U32                  lpmDbId;
    GT_U32                  vrId;
    GT_U32                  prefixLen;

    GT_IPV6ADDR             ipAddr2; /* {{20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20}};*/
    GT_IPV6ADDR             ipAddr3; /* {{30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30}};*/
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                 override;

    cpssOsMemSet((GT_VOID*) &(ipAddr2), 20, sizeof(ipAddr2));
    cpssOsMemSet((GT_VOID*) &(ipAddr3), 30, sizeof(ipAddr3));

    /* set default DB id and VR id*/
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");

    /* 3. Add prefix for testing. Call cpssExMxPmIpLpmIpv6UcPrefixAdd
          with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: ],
          prefixLen [32], nextHopPointer { routeEntryBaseMemAddr [0],
          blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
          override [GT_FALSE]. Expected: GT_OK. */

    prefixLen = 32;
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize = 1;
    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr2, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixAdd: %d, %d, prefixLen = %d",
                                  lpmDbId, vrId, prefixLen);


    /*--- correct call:*/

    /*4. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: ],
    prefixLen [32]. Expected: GT_OK. */

    st = cpssExMxPmIpLpmIpv6UcPrefixDelete(lpmDbId, vrId, &ipAddr2, prefixLen);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    /*5. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: /
    [::all arIP = 30::], prefixLen [32] (prefix already deleted /
    non-existent prefix). Expected: non GT_OK. */

    st = cpssExMxPmIpLpmIpv6UcPrefixDelete(lpmDbId, vrId, &ipAddr2, prefixLen);
    UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    st = cpssExMxPmIpLpmIpv6UcPrefixDelete(lpmDbId, vrId, &ipAddr3, prefixLen);
    UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    /*--- invalid ID's:*/

    /*6. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent),
    other params same as in 4. Expected: non GT_OK.*/

    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixDelete(lpmDbId, vrId, &ipAddr2, prefixLen);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "lpmDbId = %d", lpmDbId);

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /*7. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent),
    other params same as in 4. Expected: non GT_OK.*/

    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixDelete(lpmDbId, vrId, &ipAddr2, prefixLen);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "vrId = %d", vrId);

    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /*--- incorrect params*/

    /*8. Try to delete default prefix (incorrect). Call with prefixLen [0],
    other params same as in 4. Expected: non GT_OK.*/

    prefixLen = 0;

    st = cpssExMxPmIpLpmIpv6UcPrefixDelete(lpmDbId, vrId, &ipAddr2, prefixLen);
    UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    prefixLen = 32;

    /*9. Call function with ipAddrPtr [NULL], other params same as in 4.
    Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6UcPrefixDelete(lpmDbId, vrId, NULL, prefixLen);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipAddrPtr = NULL", lpmDbId);

    /* Delete Prefix */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv6UcPrefixBulkDelete
(
    IN  GT_U32                                          lpmDbId,
    IN  GT_U32                                          ipv6PrefixArrayLen,
    IN  CPSS_EXMXPM_IP_LPM_IPV6_UC_DELETE_PREFIX_STC    *ipv6PrefixArrayPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6UcPrefixBulkDelete)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Create virtual router. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId[1], vrId [0], vrConfigPtr { supportUcIpv4 [GT_TRUE], defaultUcIpv4RouteEntry { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]} ; supportMcIpv4 [GT_TRUE], defaultMcIpv4RouteEntry { same as defaultUcIpv4RouteEntry } ; supportUcIpv6 [GT_TRUE], defaultUcIpv6RouteEntry { same as defaultUcIpv4RouteEntry } }. Expected: GT_OK.
3. Add prefix for testing. Call cpssExMxPmIpLpmIpv6UcPrefixAdd with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: ], prefixLen [32], nextHopPointer { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},  override [GT_FALSE]. Expected: GT_OK.
--- correct call:
4. Call with lpmDbId [1], ipv6PrefixArrayLen [1], ipv6PrefixArrayPtr { vrId [0], ipAddr [::all arIP = 20:: ], prefixLen [32]} . Expected: GT_OK and ipv6PrefixArrayPtr[0].returnStatus - GT_OK.
5. Call with lpmDbId [1], ipv6PrefixArrayLen [1], ipv6PrefixArrayPtr { vrId [0], ipAddrPtr [::all arIP = 30::], prefixLen [32] } (prefix already deleted / non-existent prefix). Expected: non GT_OK and ipv6PrefixArrayPtr[0].returnStatus - non GT_OK.
--- invalid ID's:
6. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 4. Expected: non GT_OK.
7. Call with ipv6PrefixArrayLen [1], ipv6PrefixArrayPtr { vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] } (non-existent), other params same as in 4. Expected: non GT_OK and ipv6PrefixArrayPtr[0].returnStatus - non GT_OK.
--- incorrect params
8. Call function with ipv6PrefixArrayPtr [NULL], other params same as in 4. Expected: GT_BAD_PTR.
--- delete VR/DB
9. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0]. Expected: GT_OK.
10. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U32                  lpmDbId;
    GT_U32                  ipv6PrefixArrayLen;
    CPSS_EXMXPM_IP_LPM_IPV6_UC_DELETE_PREFIX_STC  ipv6PrefixArray[1];

    GT_U32                  vrId;
    GT_IPV6ADDR             ipAddr2; /* {{20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20}};*/
    GT_IPV6ADDR             ipAddr3; /* {{30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30}};*/
    GT_U32                  prefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                 override;

    cpssOsMemSet((GT_VOID*) &(ipAddr2), 20, sizeof(ipAddr2));
    cpssOsMemSet((GT_VOID*) &(ipAddr3), 30, sizeof(ipAddr3));

    /* set default DB id and VR id*/
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");

    /* 3. Add prefix for testing. Call cpssExMxPmIpLpmIpv6UcPrefixAdd
          with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: ],
          prefixLen [32], nextHopPointer { routeEntryBaseMemAddr [0],
          blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
          override [GT_FALSE]. Expected: GT_OK. */

    prefixLen = 32;
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize = 1;
    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr2, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixAdd: %d, %d, prefixLen = %d",
                                  lpmDbId, vrId, prefixLen);



    /* 4. Call with lpmDbId [1], ipv6PrefixArrayLen [1], ipv6PrefixArrayPtr
     { vrId [0], ipAddr [::all arIP = 20:: ], prefixLen [32]} . Expected:
        GT_OK and ipv6PrefixArrayPtr[0].returnStatus - GT_OK.   */

    ipv6PrefixArrayLen = 1;
    ipv6PrefixArray[0].vrId = IP_LPM_DEFAULT_VR_ID_CNS;
    ipv6PrefixArray[0].ipAddr = ipAddr2;
    ipv6PrefixArray[0].prefixLen = 32;

    st = cpssExMxPmIpLpmIpv6UcPrefixBulkDelete(lpmDbId, ipv6PrefixArrayLen, ipv6PrefixArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, ipv6PrefixArray[0].returnStatus,
                      "ipv6PrefixArray[0].returnStatus: %d (GT_OK expected)", ipv6PrefixArray[0].returnStatus);

    /* 5. Call with lpmDbId [1], ipv6PrefixArrayLen [1], ipv6PrefixArrayPtr
      { vrId [0], ipAddrPtr [::all arIP = 30::], prefixLen [32] }
      (prefix already deleted / non-existent prefix). Expected: non GT_OK
       and ipv6PrefixArrayPtr[0].returnStatus - non GT_OK.  */

    ipv6PrefixArray[0].ipAddr = ipAddr3;

    st = cpssExMxPmIpLpmIpv6UcPrefixBulkDelete(lpmDbId, ipv6PrefixArrayLen, ipv6PrefixArray);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDbId);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, ipv6PrefixArray[0].returnStatus,
                      "ipv6PrefixArray[0].returnStatus: %d (non GT_OK expected)", ipv6PrefixArray[0].returnStatus);

    ipv6PrefixArray[0].ipAddr = ipAddr2;

    /*--- invalid ID's:*/

    /*6. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent),
    other params same as in 4. Expected: non GT_OK.*/

    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixBulkDelete(lpmDbId, ipv6PrefixArrayLen, ipv6PrefixArray);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "lpmDbId = %d", lpmDbId);

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /*7. Call with ipv6PrefixArrayLen [1], ipv6PrefixArrayPtr { vrId
    [IP_LPM_INVALID_VR_ID_CNS = 6666] } (non-existent), other params same as
    in 4. Expected: non GT_OK and ipv6PrefixArrayPtr[0].returnStatus - non
    GT_OK.*/


    ipv6PrefixArray[0].vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixBulkDelete(lpmDbId, ipv6PrefixArrayLen, ipv6PrefixArray);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "ipv6PrefixArray[0].vrId = %d", ipv6PrefixArray[0].vrId);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, ipv6PrefixArray[0].returnStatus,
                      "ipv6PrefixArray[0].returnStatus: %d (non GT_OK expected)", ipv6PrefixArray[0].returnStatus);

    ipv6PrefixArray[0].vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /*--- incorrect params*/

    /*8. Call function with ipv6PrefixArrayPtr [NULL], other params same as
    in 4. Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6UcPrefixBulkDelete(lpmDbId, ipv6PrefixArrayLen, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipv6PrefixArrayPtr = NULL", lpmDbId);

    /* Delete Prefix */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv6UcPrefixesFlush
(
    IN  GT_U32  lpmDbId,
    IN  GT_U32  vrId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6UcPrefixesFlush)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Create virtual router. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId[1], vrId [0], vrConfigPtr { supportUcIpv4 [GT_TRUE], defaultUcIpv4RouteEntry { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]} ; supportMcIpv4 [GT_TRUE], defaultMcIpv4RouteEntry { same as defaultUcIpv4RouteEntry } ; supportUcIpv6 [GT_TRUE], defaultUcIpv6RouteEntry { same as defaultUcIpv4RouteEntry } }. Expected: GT_OK.
--- correct call:
3. Call with lpmDbId [1], vrId [0]. Expected: GT_OK.
--- invalid ID's:
4. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
5. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK.
--- delete VR/DB
6. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0]. Expected: GT_OK.
7. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U32                  lpmDbId;
    GT_U32                  vrId;
    GT_IPV6ADDR             ipAddr; /* {{10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}};*/
    GT_U32                  prefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                 override;
    cpssOsMemSet((GT_VOID*) &(ipAddr), 10, sizeof(ipAddr));

    /* set default DB id and VR id*/
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");

    /* 3. Add prefix for testing. Call cpssExMxPmIpLpmIpv6UcPrefixAdd
          with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ],
          prefixLen [32], nextHopPointer { routeEntryBaseMemAddr [0],
          blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
          override [GT_FALSE]. Expected: GT_OK. */

    prefixLen = 32;
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize = 1;
    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixAdd: %d, %d, prefixLen = %d",
                                  lpmDbId, vrId, prefixLen);

    /*4. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ],
     prefixLen [32], non-NULL nextHopPointerPtr. Expected: GT_OK. */

    prefixLen = 32;

    st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);


    /* 3. Call with lpmDbId [1], vrId [0]. Expected: GT_OK. */

    st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);


    /*4. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ],
     prefixLen [32], non-NULL nextHopPointerPtr. Expected: GT_OK. */

    st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_NOT_FOUND, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    /*--- invalid ID's:*/

    /*4. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent),
    other params same as in 3. Expected: non GT_OK.*/

    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "lpmDbId = %d", lpmDbId);

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /*5. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent),
    other params same as in 3. Expected: non GT_OK.*/

    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "vrId = %d", vrId);

    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv6UcPrefixGet
(
    IN  GT_U32                                  lpmDbId,
    IN  GT_U32                                  vrId,
    IN  GT_IPV6ADDR                             *ipAddrPtr,
    IN  GT_U32                                  prefixLen,
    OUT CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  *nextHopPointerPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6UcPrefixGet)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Create virtual router. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId[1], vrId [0], vrConfigPtr { supportUcIpv4 [GT_TRUE], defaultUcIpv4RouteEntry { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]} ; supportMcIpv4 [GT_TRUE], defaultMcIpv4RouteEntry { same as defaultUcIpv4RouteEntry } ; supportUcIpv6 [GT_TRUE], defaultUcIpv6RouteEntry { same as defaultUcIpv4RouteEntry } }. Expected: GT_OK.
3. Add prefix for testing. Call cpssExMxPmIpLpmIpv6UcPrefixAdd with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ], prefixLen [32], nextHopPointer { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},  override [GT_FALSE]. Expected: GT_OK.
--- correct call:
4. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ], prefixLen [32], non-NULL nextHopPointerPtr. Expected: GT_OK.
--- invalid ID's:
5. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ], prefixLen [16 / 0 / 128 / 256] (invalid), non-NULL nextHopPointerPtr. Expected: NOT GT_OK.
5.1. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: ] (non-existent), prefixLen [32], non-NULL nextHopPointerPtr. Expected: GT_NOT_FOUND.
6. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 4. Expected: non GT_OK.
7. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 4. Expected: non GT_OK.
--- incorrect params
8. Call function with ipAddrPtr [NULL], other params same as in 4. Expected: GT_BAD_PTR.
9. Call function with nextHopPointerPtr [NULL], other params same as in 4. Expected: GT_BAD_PTR.
--- delete VR/DB
10. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0]. Expected: GT_OK.
11. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U32                  lpmDbId;
    GT_U32                  vrId;
    GT_IPV6ADDR             ipAddr; /* {{10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}};*/
    GT_U32                  prefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                 override;

    cpssOsMemSet((GT_VOID*) &(ipAddr), 10, sizeof(ipAddr));

    /* set default DB id and VR id*/
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");

    /* 3. Add prefix for testing. Call cpssExMxPmIpLpmIpv6UcPrefixAdd
          with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ],
          prefixLen [32], nextHopPointer { routeEntryBaseMemAddr [0],
          blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
          override [GT_FALSE]. Expected: GT_OK. */

    prefixLen = 32;
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize = 1;
    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixAdd: %d, %d, prefixLen = %d",
                                  lpmDbId, vrId, prefixLen);


    /*4. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ],
     prefixLen [32], non-NULL nextHopPointerPtr. Expected: GT_OK. */

    prefixLen = 32;

    st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    /*5. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ],
     prefixLen [16 / 0 / 128 / 256] (invalid number), non-NULL nextHopPointerPtr. Expected: NOT GT_OK. */

    prefixLen = 16;

    st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    prefixLen = 0;

    /* prefixLen = 0 means default prefix; this prefix is added when virtual router is added */
    st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    prefixLen = 128;

    st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    prefixLen = 256;

    st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    /*5.1. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: ] (non-existent),
     prefixLen [32], non-NULL nextHopPointerPtr. Expected: GT_NOT_FOUND. */

    prefixLen = 32;
    cpssOsMemSet((GT_VOID*) &(ipAddr), 20, sizeof(ipAddr));

    st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_NOT_FOUND, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    cpssOsMemSet((GT_VOID*) &(ipAddr), 10, sizeof(ipAddr));

    /*--- invalid ID's:*/

    /*6. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent),
    other params same as in 4. Expected: non GT_OK.*/

    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "lpmDbId = %d", lpmDbId);

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /*7. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent),
    other params same as in 4. Expected: non GT_OK.*/

    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "vrId = %d", vrId);

    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /*--- incorrect params*/

    /*8. Call function with ipAddrPtr [NULL], other params same as in 4.
    Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, NULL, prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipAddrPtr = NULL", lpmDbId);

    /*9. Call function with nextHopPointerPtr [NULL], other params same as
    in 4. Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, nextHopPointerPtr = NULL", lpmDbId);

    /* Delete Prefix */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv6UcPrefixGetNext
(
    IN    GT_U32                                    lpmDbId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPV6ADDR                               *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC    *nextHopPointerPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6UcPrefixGetNext)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Create virtual router. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId[1], vrId [0], vrConfigPtr { supportUcIpv4 [GT_TRUE], defaultUcIpv4RouteEntry { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]} ; supportMcIpv4 [GT_TRUE], defaultMcIpv4RouteEntry { same as defaultUcIpv4RouteEntry } ; supportUcIpv6 [GT_TRUE], defaultUcIpv6RouteEntry { same as defaultUcIpv4RouteEntry } }. Expected: GT_OK.
3. Add prefix for testing. Call cpssExMxPmIpLpmIpv6UcPrefixAdd with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ], prefixLen [32], nextHopPointer { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},  override [GT_FALSE]. Expected: GT_OK.
--- correct call:
4. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ], non-NULL prefixLenPtr [32 / 0 / 256] and nextHopPointerPtr. Expected: GT_OK.
--- invalid ID's:
5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 4. Expected: non GT_OK.
6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 4. Expected: non GT_OK.
--- incorrect params
7. Call function with ipAddrPtr [NULL], other params same as in 4. Expected: GT_BAD_PTR.
8. Call function with prefixLenPtr [NULL], other params same as in 4. Expected: GT_BAD_PTR.
9. Call function with nextHopPointerPtr [NULL], other params same as in 4. Expected: GT_BAD_PTR.
--- delete VR/DB
10. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0]. Expected: GT_OK.
11. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U32                  lpmDbId;
    GT_U32                  vrId;
    GT_IPV6ADDR             ipAddr; /* {{10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}};*/
    GT_U32                  prefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                 override;

    cpssOsMemSet((GT_VOID*) &(ipAddr), 0x10, sizeof(ipAddr));

    /* set default DB id and VR id*/
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");

    /* 3. Add prefix for testing. Call cpssExMxPmIpLpmIpv6UcPrefixAdd
       with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ],
       prefixLen [32], nextHopPointer { routeEntryBaseMemAddr [0],
       blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
       override [GT_FALSE]. Expected: GT_OK. */

    prefixLen = 32;
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize = 1;
    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    override = GT_FALSE;
    cpssOsMemSet((GT_VOID*) &(ipAddr), 0x10, sizeof(ipAddr));

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixAdd: %d, %d, prefixLen = %d",
                                  lpmDbId, vrId, prefixLen);

    prefixLen = 32;
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize = 1;
    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    override = GT_FALSE;
    cpssOsMemSet((GT_VOID*) &(ipAddr), 0x20, sizeof(ipAddr));

    st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6UcPrefixAdd: %d, %d, prefixLen = %d",
                                  lpmDbId, vrId, prefixLen);

    /* --- correct call: */
    prefixLen = 32;
    cpssOsMemSet((GT_VOID*) &(ipAddr), 0x10, sizeof(ipAddr));

    st = cpssExMxPmIpLpmIpv6UcPrefixGetNext(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    /* 4. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ],
       non-NULL prefixLenPtr [0 / 256] (invalid) and nextHopPointerPtr. Expected: NOT GT_OK. */

    prefixLen = 0;

    /* prefixLen = 0 means default prefix; this prefix is added when virtual router is added */
    st = cpssExMxPmIpLpmIpv6UcPrefixGetNext(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    prefixLen = 256;

    st = cpssExMxPmIpLpmIpv6UcPrefixGetNext(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, prefixLen = %d", lpmDbId, vrId, prefixLen);

    prefixLen = 32;

    /*--- invalid ID's:*/

    /*5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent),
    other params same as in 4. Expected: non GT_OK.*/

    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;
    prefixLen = 32;

    st = cpssExMxPmIpLpmIpv6UcPrefixGetNext(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "lpmDbId = %d", lpmDbId);

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /*6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent),
    other params same as in 4. Expected: non GT_OK.*/

    vrId = IP_LPM_INVALID_VR_ID_CNS;
    prefixLen = 32;

    st = cpssExMxPmIpLpmIpv6UcPrefixGetNext(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "vrId = %d", vrId);

    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /*--- incorrect params*/

    /*7. Call function with ipAddrPtr [NULL], other params same as in 4.
    Expected: GT_BAD_PTR.*/

    prefixLen = 32;

    st = cpssExMxPmIpLpmIpv6UcPrefixGetNext(lpmDbId, vrId, NULL, &prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipAddrPtr = NULL", lpmDbId);

    /*8. Call function with prefixLenPtr [NULL], other params same as in 4.
    Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6UcPrefixGetNext(lpmDbId, vrId, &ipAddr, NULL, &nextHopPointer);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, prefixLenPtr = NULL", lpmDbId);

    /*9. Call function with nextHopPointerPtr [NULL], other params same as
    in 4. Expected: GT_BAD_PTR.*/

    prefixLen = 32;

    st = cpssExMxPmIpLpmIpv6UcPrefixGetNext(lpmDbId, vrId, &ipAddr, &prefixLen, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, nextHopPointerPtr = NULL", lpmDbId);

    /* Delete Prefix */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv6UcLpmGet
(
    IN  GT_U32                                  lpmDbId,
    IN  GT_U32                                  vrId,
    IN  GT_IPV6ADDR                             *ipAddrPtr,
    OUT GT_U32                                  *prefixLenPtr,
    OUT CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  *nextHopPointerPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6UcLpmGet)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Create virtual router. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId[1], vrId [0], vrConfigPtr { supportUcIpv4 [GT_TRUE], defaultUcIpv4RouteEntry { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]} ; supportMcIpv4 [GT_TRUE], defaultMcIpv4RouteEntry { same as defaultUcIpv4RouteEntry } ; supportUcIpv6 [GT_TRUE], defaultUcIpv6RouteEntry { same as defaultUcIpv4RouteEntry } }. Expected: GT_OK.
--- correct call:
3. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ], non-NULL prefixLenPtr and nextHopPointerPtr. Expected: GT_OK.
3.1. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: ] (non-existent), non-NULL prefixLenPtr and nextHopPointerPtr. Expected: GT_NOT_FOUND.
--- invalid ID's:
4. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
5. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK.
--- incorrect params
6. Call function with ipAddrPtr [NULL], other params same as in 3. Expected: GT_BAD_PTR.
7. Call function with prefixLenPtr [NULL], other params same as in 3. Expected: GT_BAD_PTR.
8. Call function with nextHopPointerPtr [NULL], other params same as in 3. Expected: GT_BAD_PTR.
--- delete VR/DB
9. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0]. Expected: GT_OK.
10. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U32                  lpmDbId;
    GT_U32                  vrId;
    GT_IPV6ADDR             ipAddr; /* {{10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}};*/
    GT_U32                  prefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;

    cpssOsMemSet((GT_VOID*) &(ipAddr), 10, sizeof(ipAddr));

    /* set default DB id and VR id*/
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");

    /*3. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 10:: ],
    non-NULL prefixLenPtr and nextHopPointerPtr. Expected: GT_OK. */

    st = cpssExMxPmIpLpmIpv6UcLpmGet(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*3.1. Call with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: ] (non-existent),
    non-NULL prefixLenPtr and nextHopPointerPtr. Expected: GT_NOT_FOUND. */

    cpssOsMemSet((GT_VOID*) &(ipAddr), 20, sizeof(ipAddr));

    st = cpssExMxPmIpLpmIpv6UcLpmGet(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_FOUND, st, lpmDbId, vrId);

    cpssOsMemSet((GT_VOID*) &(ipAddr), 10, sizeof(ipAddr));

    /*--- invalid ID's:*/

    /*4. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent),
    other params same as in 3. Expected: non GT_OK.*/

    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcLpmGet(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "lpmDbId = %d", lpmDbId);

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /*5. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent),
    other params same as in 3. Expected: non GT_OK.*/

    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcLpmGet(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "vrId = %d", vrId);

    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    /*6. Call function with ipAddrPtr [NULL], other params same as in 3.
    Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6UcLpmGet(lpmDbId, vrId, NULL, &prefixLen, &nextHopPointer);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipAddrPtr = NULL", lpmDbId);

    /*7. Call function with prefixLenPtr [NULL], other params same as in 3.
    Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6UcLpmGet(lpmDbId, vrId, &ipAddr, NULL, &nextHopPointer);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, prefixLenPtr = NULL", lpmDbId);

    /*8. Call function with nextHopPointerPtr [NULL], other params same as
    in 3. Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6UcLpmGet(lpmDbId, vrId, &ipAddr, &prefixLen, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, nextHopPointerPtr = NULL", lpmDbId);

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv6McEntryAdd
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPV6ADDR                              ipGroup,
    IN GT_U32                                   ipGroupRuleIndex,
    IN GT_IPV6ADDR                              ipSrc,
    IN GT_U32                                   ipSrcPrefixLen,
    IN CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC   *mcRoutePointerPtr,
    IN GT_BOOL                                  override
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6McEntryAdd)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Create virtual router. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId[1], vrId [0], vrConfigPtr { supportUcIpv4 [GT_TRUE], defaultUcIpv4RouteEntry { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]} ; supportMcIpv4 [GT_TRUE], defaultMcIpv4RouteEntry { same as defaultUcIpv4RouteEntry } ; supportUcIpv6 [GT_TRUE], defaultUcIpv6RouteEntry { same as defaultUcIpv4RouteEntry } }. Expected: GT_OK.
--- correct call:
3. Call with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]},
             ipGroupRuleIndex[0],
             ipSrc{arIP [0xAA, ..., 0xAA]},
             ipSrcPrefixLen[32],
             mcRoutePointerPtr {routeEntryBaseMemAddr [0xFF],
                                blockSize [0],
                                routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
             override[GT_FALSE].
Ecpected: GT_OK.
4. Call cpssExMxPmIpLpmIpv6McEntryGet with not NULL pointers and other params from 3.
Expected: GT_OK and the same params as was set.
4.1. Call with override [GT_TRUE] and ipSrc{arIP [0xBB, ..., 0xBB]} (non-existent) and other params from 3.
Expected: GT_NOT_FOUND.
4.2. Call with override [GT_FALSE] and ipSrc{arIP [0xAA, ..., 0xAA]} and other params from 3.
Expected: GT_ALREADY_EXIST.

--- incorrect call:
5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK.
9. Call with out of range mcRoutePointerPtr->routeEntryMethod [0x5AAAAAA5] and other parameters from 3. Expected: GT_BAD_PARAM.
10. Call with mcRoutePointerPtr->blockSize [0], mcRoutePointerPtr->routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E] (for regular should be 1)
       and other parameters from 3.Expected: NOT GT_OK.
11. Call function with mcRoutePointerPtr [NULL], other params same as in 3. Expected: GT_BAD_PTR.
--- delete VR/DB
12. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0]. Expected: GT_OK.
13. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS                                st = GT_OK;
    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPV6ADDR                              ipGroup;
    GT_U32                                   ipGroupRuleIndex;
    GT_U32                                   ipGroupRuleIndexGet;
    GT_IPV6ADDR                              ipSrc;
    GT_U32                                   ipSrcPrefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC   mcRoutePointer;
    GT_BOOL                                  override;

    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC   mcRoutePointerGet;
    GT_BOOL                                  isEqual = GT_FALSE;


    cpssOsBzero((GT_VOID*) &ipSrc, sizeof(ipSrc));
    cpssOsBzero((GT_VOID*) &ipGroup, sizeof(ipGroup));
    cpssOsBzero((GT_VOID*) &mcRoutePointer, sizeof(mcRoutePointer));
    cpssOsBzero((GT_VOID*) &mcRoutePointerGet, sizeof(mcRoutePointerGet));

    /* set default DB id and VR id*/
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");
    /*
        3. Call with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]},
                     ipGroupRuleIndex[0],
                     ipSrc{arIP [0xAA, ..., 0xAA]},
                     ipSrcPrefixLen[32],
                     mcRoutePointerPtr {routeEntryBaseMemAddr [0xFF],
                                        blockSize [0],
                                        routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                     override[GT_FALSE].
        Ecpected: GT_OK.
    */

    lpmDBId = 1;
    vrId    = 0;
    cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 10, sizeof(ipGroup.arIP));
    ipGroup.arIP[0] = 0xFF;  /* IPv6 MC group are of the form FF::/8 */
    ipGroupRuleIndex = 0;
    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));
    ipSrcPrefixLen = 32;
    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize = 1;
    mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

    /*
        4. Call cpssExMxPmIpLpmIpv6McEntryGet with not NULL pointers and other params from 3.
        Expected: GT_OK and the same params as was set.
    */
    st = cpssExMxPmIpLpmIpv6McEntryGet(lpmDBId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &ipGroupRuleIndexGet, &mcRoutePointerGet);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6McEntryGet: %d, %d" ,lpmDBId, vrId);

    UTF_VERIFY_EQUAL2_STRING_MAC(ipGroupRuleIndex, ipGroupRuleIndexGet,
                                 "got another ipGroupRuleIndex than was set: %d, %d", lpmDBId, vrId);

    isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mcRoutePointer, (GT_VOID*) &mcRoutePointerGet, sizeof(mcRoutePointer) )) ? GT_TRUE : GT_FALSE;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "got another mcRoutePointer than was set: %d, %d", lpmDBId, vrId);

    /*
        4.1. Call with override [GT_TRUE] and ipSrc{arIP [0xBB, ..., 0xBB]} (non-existent) and other params from 3.
        Expected: GT_NOT_FOUND.
    */

    override = GT_TRUE;
    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xBB, sizeof(ipSrc.arIP));

    st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_FOUND, st, lpmDBId, vrId);

    /*
        4.2. Call with override [GT_FALSE] and ipSrc{arIP [0xAA, ..., 0xAA]} and other params from 3.
        Expected: GT_ALREADY_EXIST.
    */

    override = GT_FALSE;
    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));

    st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_ALREADY_EXIST, st, lpmDBId, vrId);


    /*
        5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
    */
    lpmDBId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

    lpmDBId = 1;

    /*
        6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK.
    */
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "vrId = %d", vrId);

    vrId = 0;

    /*
        9. Call with out of range mcRoutePointerPtr->routeEntryMethod [0x5AAAAAA5] and other parameters from 3. Expected: GT_BAD_PARAM.
    */
    mcRoutePointer.routeEntryMethod = IP_LPM_INVALID_ENUM_CNS;

    st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PARAM, st, "mcRoutePointerPtr->routeEntryMethod = %d", mcRoutePointer.routeEntryMethod);

    mcRoutePointer.routeEntryMethod = CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E;

    /*
        10. Call with mcRoutePointerPtr->blockSize [0], mcRoutePointerPtr->routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E] (for regular should be 1)
               and other parameters from 3.Expected: NOT GT_OK.
    */
    mcRoutePointer.blockSize        = 0;
    mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "mcRoutePointerPtr->blockSize = %d, mcRoutePointerPtr->routeEntryMethod = %d",
                                     mcRoutePointer.blockSize, mcRoutePointer.routeEntryMethod);

    mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;

    /*
        11. Call function with mcRoutePointerPtr [NULL], other params same as in 3. Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, NULL, override);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_BAD_PTR, st, "mcRoutePointerPtr = NULL");

    /* Delete Entry */
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv6McEntryGet
(
    IN  GT_U32                                  lpmDbId,
    IN  GT_U32                                  vrId,
    IN  GT_IPV6ADDR                             ipGroup,
    IN  GT_IPV6ADDR                             ipSrc,
    IN  GT_U32                                  ipSrcPrefixLen,
    OUT GT_U32                                  *ipGroupRuleIndexPtr,
    OUT CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  *mcRoutePointerPtr
)

*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6McEntryGet)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Create virtual router. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId[1], vrId [0], vrConfigPtr { supportUcIpv4 [GT_TRUE], defaultUcIpv4RouteEntry { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]} ; supportMcIpv4 [GT_TRUE], defaultMcIpv4RouteEntry { same as defaultUcIpv4RouteEntry } ; supportUcIpv6 [GT_TRUE], defaultUcIpv6RouteEntry { same as defaultUcIpv4RouteEntry } }. Expected: GT_OK.
--- correct call:
3. Call cpssExMxPmIpLpmIpv6McEntryGet with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]},
             ipGroupRuleIndex[0],
             ipSrc{arIP [0xAA, ..., 0xAA]},
             ipSrcPrefixLen[32],
             mcRoutePointerPtr {routeEntryBaseMemAddr [0xFF],
                                blockSize [0],
                                routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
             override[GT_FALSE].
Ecpected: GT_OK.
4. Call cpssExMxPmIpLpmIpv6McEntryGet with not NULL pointers and other params from 3.
Expected: GT_OK and the same params as was set.
4.1. Call cpssExMxPmIpLpmIpv6McEntryGet with not NULL pointers and other params from 3.
Expected: GT_NOT_FOUND and the same params as was set.
--- incorrect call:
5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK.
7. Call function with mcRoutePointerPtr [NULL], other params same as in 3. Expected: GT_BAD_PTR.
8. Call function with ipGroupRuleIndexPtr [NULL], other params same as in 3. Expected: GT_BAD_PTR.
--- delete VR/DB
9. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0]. Expected: GT_OK.
10. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS                                st = GT_OK;
    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPV6ADDR                              ipGroup;
    GT_IPV6ADDR                              ipSrc;
    GT_U32                                   ipGroupRuleIndex;
    GT_U32                                   ipGroupRuleIndexGet;
    GT_U32                                   ipSrcPrefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC   mcRoutePointer;
    GT_BOOL                                  override;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC   mcRoutePointerGet;
    GT_BOOL                                  isEqual = GT_FALSE;


    cpssOsBzero((GT_VOID*) &ipSrc, sizeof(ipSrc));
    cpssOsBzero((GT_VOID*) &ipGroup, sizeof(ipGroup));
    cpssOsBzero((GT_VOID*) &mcRoutePointer, sizeof(mcRoutePointer));
    cpssOsBzero((GT_VOID*) &mcRoutePointerGet, sizeof(mcRoutePointerGet));

    /* set default DB id and VR id*/
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");
    /*
        3. Call with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]},
                     ipGroupRuleIndex[0],
                     ipSrc{arIP [0xAA, ..., 0xAA]},
                     ipSrcPrefixLen[32],
                     override[GT_FALSE].
        Ecpected: GT_OK.
    */

    lpmDBId = 1;
    vrId    = 0;
    cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 10, sizeof(ipGroup.arIP));
    ipGroup.arIP[0] = 0xFF;  /* IPv6 MC group are of the form FF::/8 */
    ipGroupRuleIndex = 0;
    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));
    ipSrcPrefixLen = 32;
    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize = 1;
    mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

    /*
        4. Call cpssExMxPmIpLpmIpv6McEntryGet with not NULL pointers and other params from 3.
        Expected: GT_OK and the same params as was set.
    */
    st = cpssExMxPmIpLpmIpv6McEntryGet(lpmDBId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &ipGroupRuleIndexGet, &mcRoutePointerGet);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6McEntryGet: %d, %d" ,lpmDBId, vrId);


    isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mcRoutePointer, (GT_VOID*) &mcRoutePointerGet, sizeof(mcRoutePointer) )) ? GT_TRUE : GT_FALSE;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "got another mcRoutePointer than was set: %d, %d", lpmDBId, vrId);

    /*
        4.1. Call cpssExMxPmIpLpmIpv6McEntryGet with not NULL pointers and other params from 3.
        Expected: GT_NOT_FOUND and the same params as was set.
    */
    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xBB, sizeof(ipSrc.arIP));

    st = cpssExMxPmIpLpmIpv6McEntryGet(lpmDBId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &ipGroupRuleIndexGet, &mcRoutePointerGet);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_NOT_FOUND, st, "cpssExMxPmIpLpmIpv6McEntryGet: %d, %d" ,lpmDBId, vrId);

    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));

    /*
        5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
    */
    lpmDBId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntryGet(lpmDBId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &ipGroupRuleIndexGet, &mcRoutePointerGet);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

    lpmDBId = 1;

    /*
        6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK.
    */
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntryGet(lpmDBId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &ipGroupRuleIndexGet, &mcRoutePointerGet);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "vrId = %d", vrId);

    vrId = 0;

    /*7. Call function with mcRoutePointerPtr [NULL], other params same as
    in 4. Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6McEntryGet(lpmDBId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &ipGroupRuleIndexGet, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mcRoutePointerPtr = NULL", lpmDBId);

    /*8. Call function with ipGroupRuleIndexPtr [NULL], other params same as
    in 4. Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6McEntryGet(lpmDBId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, NULL, &mcRoutePointerGet);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipGroupRuleIndexPtr = NULL", lpmDBId);

    /*9. Delete Entry */
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

    /*10. Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv6McEntrySearch
(
    IN    GT_U32                                 lpmDBId,
    IN    GT_U32                                 vrId,
    IN    GT_IPV6ADDR                            ipGroup,
    OUT   GT_U32                                 *ipGroupRuleIndexPtr,
    IN    GT_IPV6ADDR                            ipSrc,
    IN    GT_U32                                 ipSrcPrefixLen,
    OUT   CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC *mcRoutePointerPtr
)

*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6McEntrySearch)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Create virtual router. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId[1], vrId [0], vrConfigPtr { supportUcIpv4 [GT_TRUE], defaultUcIpv4RouteEntry { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]} ; supportMcIpv4 [GT_TRUE], defaultMcIpv4RouteEntry { same as defaultUcIpv4RouteEntry } ; supportUcIpv6 [GT_TRUE], defaultUcIpv6RouteEntry { same as defaultUcIpv4RouteEntry } }. Expected: GT_OK.
--- correct call:
3. Call cpssExMxPmIpLpmIpv6McEntryAdd with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]},
             ipGroupRuleIndex[0],
             ipSrc{arIP [0xAA, ..., 0xAA], [0xAB, ..., 0xAB], [0xAC, ..., 0xAC]},
                                     (add 3 entry),
             ipSrcPrefixLen[32],
             mcRoutePointerPtr {routeEntryBaseMemAddr [0xFF],
                                blockSize [0],
                                routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
             override[GT_FALSE].
Ecpected: GT_OK.
4. Call cpssExMxPmIpLpmIpv6McEntrySearch with not NULL pointers and other params from 3.
Expected: GT_OK and the same params as was set.
--- incorrect call:
5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK.
7. Call function with ipGroup.arIP[0] not 0xFF, other params same as in 4. Expected: GT_BAD_PARAM.
8. Call function with ipSrc [NULL], other params same as in 4.  Expected: GT_NOT_FOUND.
9. Call function with ipSrcPrefixLen [IP_LPM_IPV6_INVALID_PREFIX], other params same as  in 4. Expected: NOT GT_OK.
10. Call function with mcRoutePointerPtr [NULL], other params same as in 3. Expected: GT_BAD_PTR.
11. Call function with ipGroupRuleIndexPtr [NULL], other params same as in 3. Expected: GT_BAD_PTR.
--- delete VR/DB
12. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0]. Expected: GT_OK.
13. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS                                st = GT_OK;
    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPV6ADDR                              ipGroup;
    GT_IPV6ADDR                              ipSrc;
    GT_U32                                   ipGroupRuleIndex;
    GT_U32                                   ipGroupRuleIndexGet;
    GT_U32                                   ipSrcPrefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC   mcRoutePointer;
    GT_BOOL                                  override;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC   mcRoutePointerGet;
    GT_BOOL                                  isEqual = GT_FALSE;

    /*IPv6 Address for loop*/
    GT_U8                                    ipv6Addr_current;


    cpssOsBzero((GT_VOID*) &ipSrc, sizeof(ipSrc));
    cpssOsBzero((GT_VOID*) &ipGroup, sizeof(ipGroup));
    cpssOsBzero((GT_VOID*) &mcRoutePointer, sizeof(mcRoutePointer));
    cpssOsBzero((GT_VOID*) &mcRoutePointerGet, sizeof(mcRoutePointerGet));

    /* set default DB id and VR id*/
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");
    /*
        3. Call cpssExMxPmIpLpmIpv6McEntryAdd with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]},
                     ipGroupRuleIndex[0],
                     ipSrc{arIP [0xAA, ..., 0xAA], [0xAB, ..., 0xAB], [0xAC, ..., 0xAC]},
                     (add 3 entry)
                     ipSrcPrefixLen[32],
                     override[GT_FALSE].
        Ecpected: GT_OK.
    */

    lpmDBId = 1;
    vrId    = 0;
    cpssOsMemSet ((GT_VOID*) &(ipGroup.arIP), 10, sizeof(ipGroup.arIP));
    ipGroup.arIP[0] = 0xFF;  /* IPv6 MC group are of the form FF::/8 */
    ipGroupRuleIndex = 0;
    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));
    ipSrcPrefixLen = 32;
    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize = 1;
    mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    override = GT_FALSE;

    ipv6Addr_current = 0xAA;

    for (ipv6Addr_current = 0xAA; ipv6Addr_current < 0xAD; ipv6Addr_current++)
        {
            cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), ipv6Addr_current, sizeof(ipSrc.arIP));
            st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAC, sizeof(ipSrc.arIP));

    /*
        4. Call cpssExMxPmIpLpmIpv6McEntrySearch with not NULL pointers and other params from 3.
        Expected: GT_OK and the same params as was set.
    */
    st = cpssExMxPmIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, &ipGroupRuleIndexGet, ipSrc, ipSrcPrefixLen,  &mcRoutePointerGet);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6McEntrySearch: %d, %d" ,lpmDBId, vrId);

    isEqual = (0 == cpssOsMemCmp((GT_VOID*) &ipGroupRuleIndex, (GT_VOID*) &ipGroupRuleIndexGet, sizeof(ipGroupRuleIndex) )) ? GT_TRUE : GT_FALSE;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "got another ipGroupRuleIndex than was set: %d, %d", lpmDBId, vrId);

    isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mcRoutePointer, (GT_VOID*) &mcRoutePointerGet, sizeof(mcRoutePointer) )) ? GT_TRUE : GT_FALSE;
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "got another mcRoutePointer than was set: %d, %d", lpmDBId, vrId);

    /*
        5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 55555 (non-existent), other params same as in 3. Expected: non GT_OK.
    */
    lpmDBId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, &ipGroupRuleIndexGet, ipSrc, ipSrcPrefixLen,  &mcRoutePointerGet);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

    lpmDBId = 1;

    /*
        6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 77777 (non-existent), other params same as in 3. Expected: non GT_OK.
    */
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, &ipGroupRuleIndexGet, ipSrc, ipSrcPrefixLen,  &mcRoutePointerGet);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "vrId = %d", vrId);

    vrId = 0;

    /*--- incorrect params*/

    /*7. Call function with ipGroup.arIP[0] not 0xFF, other params same as in 4.
    Expected: GT_BAD_PARAM.*/

        ipGroup.arIP[0] = 0x0;

    st = cpssExMxPmIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, &ipGroupRuleIndexGet, ipSrc, ipSrcPrefixLen,  &mcRoutePointerGet);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PARAM, st, "ipGroup.arIP[0] = %d", ipGroup.arIP[0]);

        ipGroup.arIP[0] = 0xFF;  /* IPv6 MC group are of the form FF::/8 */

    /*8. Call function with ipSrc [NULL], other params same as in 4.
    Expected: GT_NOT_FOUND.*/

    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0x0, sizeof(ipSrc.arIP));

    st = cpssExMxPmIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, &ipGroupRuleIndexGet, ipSrc, ipSrcPrefixLen,  &mcRoutePointerGet);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_FOUND, st, "ipSrc.arIP[0] = %d", ipGroup.arIP[0]);

    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));

    /*9. Call function with ipSrcPrefixLen [IP_LPM_IPV6_INVALID_PREFIX], other params same as in 4.
    Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, &ipGroupRuleIndexGet, ipSrc, IP_LPM_IPV6_INVALID_PREFIX,  &mcRoutePointerGet);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_FOUND, st, "ipSrcPrefixLen = %d", ipSrcPrefixLen);

    /*10. Call function with mcRoutePointerPtr [NULL], other params same as in 4.
    Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, &ipGroupRuleIndexGet, ipSrc, ipSrcPrefixLen,  NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mcRoutePointerPtr = NULL", lpmDBId);

    /*11. Call function with ipGroupRuleIndexPtr [NULL], other params same as  in 4.
    Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup, NULL, ipSrc, ipSrcPrefixLen, &mcRoutePointerGet);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipGroupRuleIndexPtr = NULL", lpmDBId);

    /*12. Delete Entry */
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

    /*13. Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}


/*******************************************************************************
* Entry IPV6 add, check,  delete
1. Create defauld LpmDB and Virtual Routers.
Expected: GT_OK.
2. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_before_IPV6_Entry.
3. Call cpssExMxPmIpLpmIpv6McEntryAdd to fill table of entries
Expected: GT_OK.
4. Call cpssExMxPmIpLpmIpv6McEntryGet to check entries
Expected: GT_OK.
5. Call cpssExMxPmIpLpmIpv6McEntryDel to delete all entries
Expected: GT_OK.
6. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_IPV6_Entry.
Expected: (heap_bytes_allocated_after_IPV6_Entry-heap_bytes_allocated_before_IPV6_Entry) = 0
7. Delete defauld LpmDB and Virtual Routers
Expected: GT_OK.
*******************************************************************************/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6McEntryMemoryCheck)
{
    GT_STATUS   st        = GT_OK;

    GT_U32      lpmDbId = 0;
    GT_U32      vrId    = 0;
    GT_U32      ii      = 0;

    GT_U32      heap_bytes_allocated_before_IPV6_Entry = 0;
    GT_U32      heap_bytes_allocated_after_IPV6_Entry = 0;

    GT_U32      ipGroupRuleIndex = 0;
    GT_U32      ipGroupRuleIndexGet = 0;
    GT_U32      ipSrcPrefixLen = 0;
    GT_BOOL     override = GT_FALSE;
    GT_BOOL     isEqual = GT_FALSE;

    GT_IPV6ADDR ipGroup;
    GT_IPV6ADDR ipSrc;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC   mcRoutePointer;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC   mcRoutePointerGet;


    cpssOsBzero((GT_VOID*) &ipSrc, sizeof(ipSrc));
    cpssOsBzero((GT_VOID*) &ipGroup, sizeof(ipGroup));
    cpssOsBzero((GT_VOID*) &mcRoutePointer, sizeof(mcRoutePointer));
    cpssOsBzero((GT_VOID*) &mcRoutePointerGet, sizeof(mcRoutePointerGet));

    /*
        1. Create defauld LpmDB and Virtual Routers.
        Expected: GT_OK.
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        2. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_before_IPV6_Entry.
    */
    heap_bytes_allocated_before_IPV6_Entry = osMemGetHeapBytesAllocated();

    for(ii=0; ii<IP_LPM_ITER_COUNT; ++ii)
    {
        /*
            3. Call cpssExMxPmIpLpmIpv6McEntryAdd to fill table of entries
                                                    with lpmDbId [1 .. max].
                                                         vrId [0 .. max]
                                                         ipGroup{arIP [0x10, ..., 0x10]},
                                                         ipGroupRuleIndex[0],
                                                         ipSrc{arIP [0xAA, ..., 0xAA]},
                                                         ipSrcPrefixLen[32],
                                                         mcRoutePointerPtr {routeEntryBaseMemAddr [0xFF],
                                                                            blockSize [0],
                                                                            routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                                                         override[GT_FALSE].
            Ecpected: GT_OK.
        */
        /* iterate with lpmDbId [1], vrId[0] */
        lpmDbId = 1;
        vrId    = 0;

        /* make every entry unique */
        cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 10, sizeof(ipGroup.arIP));
        ipGroupRuleIndex = 0;
        cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));
        ipSrcPrefixLen = 32;
        mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
        mcRoutePointer.blockSize = 1;
        mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
        override = GT_FALSE;

        st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDbId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6McEntryAdd: %d, %d",  lpmDbId, vrId);

        /* iterate with lpmDbId [1], vrId[1] -> not support IpV6 */
        lpmDbId = 1;
        vrId    = 1;

        /* make every entry unique */
        cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 0x11, sizeof(ipGroup.arIP));
        ipGroupRuleIndex = 0;
        cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0x22, sizeof(ipSrc.arIP));
        ipSrcPrefixLen = 32;
        mcRoutePointer.routeEntryBaseMemAddr = 1;
        mcRoutePointer.blockSize = 1;
        mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
        override = GT_FALSE;

        st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDbId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6McEntryAdd: %d, %d",  lpmDbId, vrId);

        /* iterate with lpmDbId [2], vrId[0] */
        lpmDbId = 2;
        vrId    = 0;

        /* make every entry unique */
        cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 0x33, sizeof(ipGroup.arIP));
        ipGroupRuleIndex = 0;
        cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0x44, sizeof(ipSrc.arIP));
        ipSrcPrefixLen = 32;
        mcRoutePointer.routeEntryBaseMemAddr = 2;
        mcRoutePointer.blockSize = 2;
        mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
        override = GT_FALSE;

        st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDbId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6McEntryAdd: %d, %d",  lpmDbId, vrId);

        /*
            4. Call cpssExMxPmIpLpmIpv6McEntryGet to check entries with not NULL pointers
            Expected: GT_OK and the same params as was set.
        */
        /* iterate with lpmDbId [1], vrId[0] */
        lpmDbId = 1;
        vrId    = 0;

        /* restore unique entry before compare */
        cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 10, sizeof(ipGroup.arIP));
        ipGroupRuleIndex = 0;
        cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));
        ipSrcPrefixLen = 32;
        mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
        mcRoutePointer.blockSize = 1;
        mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;

        st = cpssExMxPmIpLpmIpv6McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &ipGroupRuleIndexGet, &mcRoutePointerGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6McEntryGet: %d, %d" ,lpmDbId, vrId);

        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(ipGroupRuleIndex, ipGroupRuleIndexGet,
                                         "got another ipGroupRuleIndex than was set: %d, %d", lpmDbId, vrId);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mcRoutePointer, (GT_VOID*) &mcRoutePointerGet,
                                         sizeof(mcRoutePointer) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "got another mcRoutePointer than was set: %d, %d", lpmDbId, vrId);
        }

        /* iterate with lpmDbId [1], vrId[1] -> not support IpV6 */
        lpmDbId = 1;
        vrId    = 1;

        /* restore unique entry before compare */
        cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 0x11, sizeof(ipGroup.arIP));
        ipGroupRuleIndex = 0;
        cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0x22, sizeof(ipSrc.arIP));
        ipSrcPrefixLen = 32;
        mcRoutePointer.routeEntryBaseMemAddr = 1;
        mcRoutePointer.blockSize = 1;
        mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

        st = cpssExMxPmIpLpmIpv6McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &ipGroupRuleIndexGet, &mcRoutePointerGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6McEntryGet: %d, %d" ,lpmDbId, vrId);

        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(ipGroupRuleIndex, ipGroupRuleIndexGet,
                                         "got another ipGroupRuleIndex than was set: %d, %d", lpmDbId, vrId);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mcRoutePointer, (GT_VOID*) &mcRoutePointerGet,
                                         sizeof(mcRoutePointer) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "got another mcRoutePointer than was set: %d, %d", lpmDbId, vrId);
        }

        /* iterate with lpmDbId [2], vrId[0] */
        lpmDbId = 2;
        vrId    = 0;

        /* restore unique entry before compare */
        cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 0x33, sizeof(ipGroup.arIP));
        ipGroupRuleIndex = 0;
        cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0x44, sizeof(ipSrc.arIP));
        ipSrcPrefixLen = 32;
        mcRoutePointer.routeEntryBaseMemAddr = 2;
        mcRoutePointer.blockSize = 2;
        mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

        st = cpssExMxPmIpLpmIpv6McEntryGet(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &ipGroupRuleIndexGet, &mcRoutePointerGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6McEntryGet: %d, %d" ,lpmDbId, vrId);

        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(ipGroupRuleIndex, ipGroupRuleIndexGet,
                                         "got another ipGroupRuleIndex than was set: %d, %d", lpmDbId, vrId);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mcRoutePointer, (GT_VOID*) &mcRoutePointerGet,
                                         sizeof(mcRoutePointer) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "got another mcRoutePointer than was set: %d, %d", lpmDbId, vrId);
        }

        /*
            5. Call cpssExMxPmIpLpmIpv6McEntryDel to delete all entries
        */
        /* iterate with lpmDbId [1], vrId[0] */
        lpmDbId = 1;
        vrId    = 0;

        /* restore unique entry before delete */
        cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 10, sizeof(ipGroup.arIP));
        cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));
        ipSrcPrefixLen = 32;

        st = cpssExMxPmIpLpmIpv6McEntryDel(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6McEntryDel: %d %d", lpmDbId, vrId);

        /* iterate with lpmDbId [1], vrId[1] -> not support IpV6 */
        /*lpmDbId = 1;
        vrId    = 1;


        cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 0x11, sizeof(ipGroup.arIP));
        cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0x22, sizeof(ipSrc.arIP));
        ipSrcPrefixLen = 32;

        st = cpssExMxPmIpLpmIpv6McEntryDel(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6McEntryDel: %d %d", lpmDbId, vrId); */

        /* iterate with lpmDbId [1], vrId[0] */
        lpmDbId = 2;
        vrId    = 0;

        /* restore unique entry before delete */
        cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 0x33, sizeof(ipGroup.arIP));
        cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0x44, sizeof(ipSrc.arIP));
        ipSrcPrefixLen = 32;

        st = cpssExMxPmIpLpmIpv6McEntryDel(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6McEntryDel: %d %d", lpmDbId, vrId);

    } /* for */

    /*
        6. Call osMemGetHeapBytesAllocated to store heap size in heap_bytes_allocated_after_Router.
        Expected: (heap_bytes_allocated_after_IPV6_Entry - heap_bytes_allocated_before_IPV6_Entry) = 0
    */
    heap_bytes_allocated_after_IPV6_Entry = osMemGetHeapBytesAllocated();
    UTF_VERIFY_EQUAL0_STRING_MAC(heap_bytes_allocated_before_IPV6_Entry, heap_bytes_allocated_after_IPV6_Entry,
                                 "size of heap changed (heap_bytes_allocated)");

    /*
        7. Delete defauld LpmDB and Virtual Routers.
        Expected: GT_OK.
    */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv6McEntryDel
(
    IN  GT_U32      lpmDbId,
    IN  GT_U32      vrId,
    IN  GT_IPV6ADDR ipGroup,
    IN  GT_IPV6ADDR ipSrc,
    IN  GT_U32      ipSrcPrefixLen
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6McEntryDel)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Create virtual router. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId[1], vrId [0], vrConfigPtr { supportUcIpv4 [GT_TRUE], defaultUcIpv4RouteEntry { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]} ; supportMcIpv4 [GT_TRUE], defaultMcIpv4RouteEntry { same as defaultUcIpv4RouteEntry } ; supportUcIpv6 [GT_TRUE], defaultUcIpv6RouteEntry { same as defaultUcIpv4RouteEntry } }. Expected: GT_OK.
3. Create Entry. Call cpssExMxPmIpLpmIpv6McEntryAdd with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]},
             ipGroupRuleIndex[0],
             ipSrc{arIP [0xAA, ..., 0xAA]},
             ipSrcPrefixLen[32],
             mcRoutePointerPtr {routeEntryBaseMemAddr [0xFF],
                                blockSize [0],
                                routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
             override[GT_FALSE].
Ecpected: GT_OK.
--- correct call:
4. Call with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]}, ipSrc{arIP [0xAA, ..., 0xAA]}, ipSrcPrefixLen[32]. Expected: GT_OK.
4.1. Call with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]}, ipSrc{arIP [0xAA, ..., 0xAA]}, ipSrcPrefixLen[32].Expected: GT_NOT_FOUND. (already deleted)
--- incorrect call:
5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK.
--- delete VR/DB
7. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0]. Expected: GT_OK.
8. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS                                st = GT_OK;
    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPV6ADDR                              ipGroup;
    GT_U32                                   ipGroupRuleIndex;
    GT_IPV6ADDR                              ipSrc;
    GT_U32                                   ipSrcPrefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC   mcRoutePointer;
    GT_BOOL                                  override;


    cpssOsBzero((GT_VOID*) &mcRoutePointer, sizeof(mcRoutePointer));

    /* set default DB id and VR id*/
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");
    /*
        3. Call cpssExMxPmIpLpmIpv6McEntryAdd with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]},
                     ipGroupRuleIndex[0],
                     ipSrc{arIP [0xAA, ..., 0xAA]},
                     ipSrcPrefixLen[32],
                     mcRoutePointerPtr {routeEntryBaseMemAddr [0xFF],
                                        blockSize [0],
                                        routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                     override[GT_FALSE].
        Ecpected: GT_OK.
    */

    lpmDBId = 1;
    vrId    = 0;
    cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 10, sizeof(ipGroup.arIP));
    ipGroup.arIP[0] = 0xFF;  /* IPv6 MC group are of the form FF::/8 */
    ipGroupRuleIndex = 0;
    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));
    ipSrcPrefixLen = 32;
    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize = 1;
    mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

    /*
        4. Call with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]}, ipSrc{arIP [0xAA, ..., 0xAA]}, ipSrcPrefixLen[32].
        Expected: GT_OK.
    */
    lpmDBId = 1;
    vrId    = 0;
    cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 10, sizeof(ipGroup.arIP));
    ipGroup.arIP[0] = 0xFF;  /* IPv6 MC group are of the form FF::/8 */
    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));
    ipSrcPrefixLen = 32;

    st = cpssExMxPmIpLpmIpv6McEntryDel(lpmDBId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

    /*
        4.1. Call with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]}, ipSrc{arIP [0xAA, ..., 0xAA]}, ipSrcPrefixLen[32].
        Expected: GT_NOT_FOUND. (already deleted)
    */
    st = cpssExMxPmIpLpmIpv6McEntryDel(lpmDBId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_FOUND, st, lpmDBId, vrId);

    /*
        5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
    */
    lpmDBId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntryDel(lpmDBId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

    lpmDBId = 1;

    /*
        6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK.
    */
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntryDel(lpmDBId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "vrId = %d", vrId);

    vrId = 0;

    /* Delete Entry */
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv6McEntriesFlush
(
    IN  GT_U32  lpmDbId,
    IN  GT_U32  vrId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6McEntriesFlush)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Create virtual router. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId[1], vrId [0], vrConfigPtr { supportUcIpv4 [GT_TRUE], defaultUcIpv4RouteEntry { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]} ; supportMcIpv4 [GT_TRUE], defaultMcIpv4RouteEntry { same as defaultUcIpv4RouteEntry } ; supportUcIpv6 [GT_TRUE], defaultUcIpv6RouteEntry { same as defaultUcIpv4RouteEntry } }. Expected: GT_OK.
3. Create Entry. Call cpssExMxPmIpLpmIpv6McEntryAdd with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]},
3.1. Call cpssExMxPmIpLpmIpv6McEntryGet with not NULL pointers and other params from 3. Expected: GT_OK.
--- correct call:
4. Call with lpmDbId [1], vrId [0]. Expected: GT_OK.
4.1. Call cpssExMxPmIpLpmIpv6McEntryGet with not NULL pointers and other params from 3. Expected: GT_NOT_FOUND. (already flushed)
--- incorrect call:
5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK.
--- delete VR/DB
7. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0]. Expected: GT_OK.
8. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS                                st = GT_OK;
    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPV6ADDR                              ipGroup;
    GT_U32                                   ipGroupRuleIndex;
    GT_IPV6ADDR                              ipSrc;
    GT_U32                                   ipSrcPrefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC   mcRoutePointer;
    GT_BOOL                                  override;
    GT_U8                                    ipv6Addr_current;


    cpssOsBzero((GT_VOID*) &mcRoutePointer, sizeof(mcRoutePointer));

    /* set default DB id and VR id*/
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");
    /*
        3. Call cpssExMxPmIpLpmIpv6McEntryAdd with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]},
                     ipGroupRuleIndex[0],
                     ipSrc{arIP [0xAA, ..., 0xAA], [0xAB, ..., 0xAB], [0xAC, ..., 0xAC]},
                                             (add 3 entry),
                     ipSrcPrefixLen[32],
                     mcRoutePointerPtr {routeEntryBaseMemAddr [0xFF],
                                        blockSize [0],
                                        routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                     override[GT_FALSE].
        Ecpected: GT_OK.
    */
    lpmDBId = 1;
    vrId    = 0;
    cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 10, sizeof(ipGroup.arIP));
    ipGroup.arIP[0] = 0xFF;  /* IPv6 MC group are of the form FF::/8 */
    ipGroupRuleIndex = 0;
    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));
    ipSrcPrefixLen = 32;
    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize = 1;
    mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    override = GT_FALSE;

    for (ipv6Addr_current = 0xAA; ipv6Addr_current < 0xAD; ipv6Addr_current++)
        {
            cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), ipv6Addr_current, sizeof(ipSrc.arIP));
            st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);
        }

    /*
        3.1. Call cpssExMxPmIpLpmIpv6McEntryGet with not NULL pointers and other params from 3.
        Expected: GT_OK.
    */
    for (ipv6Addr_current = 0xAA; ipv6Addr_current < 0xAD; ipv6Addr_current++)
        {
            cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), ipv6Addr_current, sizeof(ipSrc.arIP));
            st = cpssExMxPmIpLpmIpv6McEntryGet(lpmDBId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &ipGroupRuleIndex, &mcRoutePointer);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv6McEntryGet: %d, %d" ,lpmDBId, vrId);
        }

    /*
        4. Call with lpmDbId [1], vrId [0]. Expected: GT_OK.
    */
    lpmDBId = 1;
    vrId    = 0;

    st = cpssExMxPmIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

    /*
        4.1. Call cpssExMxPmIpLpmIpv6McEntryGet with not NULL pointers and other params from 3.
        Expected: GT_NOT_FOUND. (already flushed)
    */

    for (ipv6Addr_current = 0xAA; ipv6Addr_current < 0xAD; ipv6Addr_current++)
        {
            cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), ipv6Addr_current, sizeof(ipSrc.arIP));
            st = cpssExMxPmIpLpmIpv6McEntryGet(lpmDBId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &ipGroupRuleIndex, &mcRoutePointer);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_NOT_FOUND, st, "cpssExMxPmIpLpmIpv6McEntryGet: %d, %d" ,lpmDBId, vrId);
        }

    /*
        5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
    */
    lpmDBId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

    lpmDBId = 1;

    /*
        6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK.
    */
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "vrId = %d", vrId);

    vrId = 0;

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmIpv6McEntryGetNext
(
    IN    GT_U32                                    lpmDbId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPV6ADDR                               *ipGroupPtr,
    INOUT GT_IPV6ADDR                               *ipSrcPtr,
    INOUT GT_U32                                    *ipSrcPrefixLenPtr,
    OUT   GT_U32                                    *ipGroupRuleIndexPtr,
    OUT   CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC    *mcRoutePointerPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6McEntryGetNext)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Create virtual router. Call cpssExMxPmIpLpmVirtualRouterAdd with lpmDbId[1], vrId [0], vrConfigPtr { supportUcIpv4 [GT_TRUE], defaultUcIpv4RouteEntry { routeEntryBaseMemAddr [0], blockSize [1], routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]} ; supportMcIpv4 [GT_TRUE], defaultMcIpv4RouteEntry { same as defaultUcIpv4RouteEntry } ; supportUcIpv6 [GT_TRUE], defaultUcIpv6RouteEntry { same as defaultUcIpv4RouteEntry } }. Expected: GT_OK.
3. Create Entry. Call cpssExMxPmIpLpmIpv6McEntryAdd with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]},
--- correct call:
4. Call with not NULL pointers and other params from 3. Expected: GT_OK and the same params as was set.
4.1. Call with not NULL pointers and other params from 3 (aready getted). Expected: GT_NOT_FOUND.
--- incorrect call:
5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK.
7. Call with ipGroupPtr[NULL], other params same as in 3. Expected: GT_BAD_PTR.
8. Call with ipSrcPtr[NULL], other params same as in 3. Expected: GT_BAD_PTR.
9. Call with ipSrcPrefixLenPtr[NULL], other params same as in 3. Expected: GT_BAD_PTR.
11. Call with ipGroupRuleIndexPtr[NULL], other params same as in 3. Expected: GT_BAD_PTR.
13. Call with mcRoutePointerPtr[NULL], other params same as in 3. Expected: GT_BAD_PTR.
--- delete VR/DB
14. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete with lpmDbId[1], vrId [0]. Expected: GT_OK.
15. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS                                st = GT_OK;
    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPV6ADDR                              ipGroup;
    GT_U32                                   ipGroupRuleIndex;
    GT_IPV6ADDR                              ipSrc;
    GT_U32                                   ipSrcPrefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC   mcRoutePointer;
    GT_BOOL                                  override;

    GT_U32                                  ipGroupRuleIndexGet;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  mcRoutePointerGet;

    /* prevent compiler warnings */
    mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    mcRoutePointerGet.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    cpssOsBzero((GT_VOID*) &mcRoutePointer, sizeof(mcRoutePointer));
    cpssOsBzero((GT_VOID*) &mcRoutePointerGet, sizeof(mcRoutePointerGet));

    /* set default DB id and VR id*/
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");
    /*
        3. Call cpssExMxPmIpLpmIpv6McEntryAdd with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]},
                     ipGroupRuleIndex[0],
                     ipSrc{arIP [0xAA, ..., 0xAA]},
                     ipSrcPrefixLen[32],
                     mcRoutePointerPtr {routeEntryBaseMemAddr [0xFF],
                                        blockSize [0],
                                        routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
                     override[GT_FALSE].
        Ecpected: GT_OK.
    */
    lpmDBId = 1;
    vrId    = 0;
    cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 10, sizeof(ipGroup.arIP));
    ipGroup.arIP[0] = 0xFF;  /* IPv6 MC group are of the form FF::/8 */
    ipGroupRuleIndex = 0;
    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));
    ipSrcPrefixLen = 32;
    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize = 1;
    mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    override = GT_FALSE;

    st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);


    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAB, sizeof(ipSrc.arIP));

    st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);


    cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 20, sizeof(ipGroup.arIP));
    ipGroup.arIP[0] = 0xFF;  /* IPv6 MC group are of the form FF::/8 */
    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xBB, sizeof(ipSrc.arIP));

    st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

    /*
        4. Call with not NULL pointers and other params from 3.
        Expected: GT_OK and the same params as was set.
    */
    cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 10, sizeof(ipGroup.arIP));
    ipGroup.arIP[0] = 0xFF;  /* IPv6 MC group are of the form FF::/8 */
    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));

    /* Get the first entry for group 10*/

    st = cpssExMxPmIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipSrc, &ipSrcPrefixLen,
                                           &ipGroupRuleIndexGet, &mcRoutePointerGet);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

    /* Get the second entry for group 10*/

    st = cpssExMxPmIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipSrc, &ipSrcPrefixLen,
                                           &ipGroupRuleIndexGet, &mcRoutePointerGet);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

    /*
        4.1. Call with not NULL pointers and other params from 3. (aready getted)
        Expected: GT_NOT_FOUND.
    */

    st = cpssExMxPmIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipSrc, &ipSrcPrefixLen,
                                           &ipGroupRuleIndexGet, &mcRoutePointerGet);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_FOUND, st, lpmDBId, vrId);

    /*
        5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 3. Expected: non GT_OK.
    */
    lpmDBId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipSrc, &ipSrcPrefixLen,
                                           &ipGroupRuleIndexGet, &mcRoutePointerGet);
    UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, lpmDBId);

    lpmDBId = 1;

    /*
        6. Call with vrId [IP_LPM_INVALID_VR_ID_CNS = 6666] (non-existent), other params same as in 3. Expected: non GT_OK.
    */
    vrId = IP_LPM_INVALID_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipSrc, &ipSrcPrefixLen,
                                           &ipGroupRuleIndexGet, &mcRoutePointerGet);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "vrId = %d", vrId);

    vrId = 0;

    /*
        7. Call with ipGroupPtr[NULL], other params same as in 3. Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv6McEntryGetNext(lpmDBId, vrId, NULL, &ipSrc, &ipSrcPrefixLen,
                                           &ipGroupRuleIndexGet, &mcRoutePointerGet);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_BAD_PTR, st, "ipGroupPtr = %d");

    /*
        8. Call with ipSrcPtr[NULL], other params same as in 3. Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, NULL, &ipSrcPrefixLen,
                                           &ipGroupRuleIndexGet, &mcRoutePointerGet);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_BAD_PTR, st, "ipSrcPtr = %d");

    /*
        9. Call with ipSrcPrefixLenPtr[NULL], other params same as in 3. Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipSrc, NULL,
                                           &ipGroupRuleIndexGet, &mcRoutePointerGet);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_BAD_PTR, st, "ipSrcPrefixLenPtr = %d");

    /*
        11. Call with ipGroupRuleIndexPtr[NULL], other params same as in 3. Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipSrc, &ipSrcPrefixLen,
                                           NULL, &mcRoutePointerGet);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_BAD_PTR, st, "ipGroupRuleIndexPtr = NULL");

    /*
        13. Call with mcRoutePointerPtr[NULL], other params same as in 3. Expected: GT_BAD_PTR.
    */
    st = cpssExMxPmIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &ipGroup, &ipSrc, &ipSrcPrefixLen,
                                           &ipGroupRuleIndexGet, NULL);
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_BAD_PTR, st, "mcRoutePointerPtr = NULL");

    /* Delete Entry */
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmDbMemSizeGet
(
    IN    GT_U32    lpmDbId,
    IN    GT_U32    numOfEntriesInStep,
    INOUT GT_U32    *lpmDbSizePtr,
    INOUT GT_U32    *iterPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmDbMemSizeGet)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Call with lpmDbId [1], numOfEntriesInStep [1], non-NULL lpmDbSizePtr with initial value [0], non-NULL iterPtr with initial value [0]. Expected: GT_OK.
3. Continue calling with same pointers lpmDbSizePtr, iterPtr (without changing value), (other params same as in 2) until it return  iterPtr = 0. Expected: GT_OK for each call and iterPtr = 0 returned in less than 50 iterations.
4. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 2. Expected: non GT_OK.
5. Call with numOfEntriesInStep[0], other parameters same as in 2. Expected: GT_OK.
6. Call function with lpmDbSizePtr [NULL], other params same as in 2. Expected: GT_BAD_PTR.
7. Call function with iterPtr [NULL], other params same as in 2. Expected: GT_BAD_PTR.
8. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U32                  lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32                  numOfEntriesInStep;
    GT_U32                  lpmDbSize;
    GT_U32                  iter;
    GT_U32                  counter;

    /* 1. Create default Lpm Db */
    st = prvUtfCreateDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB()");

    /* 2. Call with lpmDbId [1], numOfEntriesInStep [1], non-NULL lpmDbSizePtr
       with initial value [0], non-NULL iterPtr with initial value [0].
       Expected: GT_OK. */

    /*3. Continue calling with same pointers lpmDbSizePtr, iterPtr (without
    changing value), (other params same as in 2) until it return iterPtr =
    0. Expected: GT_OK for each call and iterPtr = 0 returned in less than
    50 iterations.*/

    numOfEntriesInStep = 1;
    lpmDbSize = 0;
    iter = 0;

    counter = 0;
    do
    {
        st = cpssExMxPmIpLpmDbMemSizeGet(lpmDbId, numOfEntriesInStep, &lpmDbSize, &iter);
        counter++;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "lpmDbId = %d, iteration: %d", lpmDbId, counter);

    } while ((GT_OK == st) && (counter < 50) && (0 != iter));
    UTF_VERIFY_EQUAL1_STRING_MAC(0, iter,
        "Iterating cpssExMxPmIpLpmDbMemSizeGet is not finished correctly. Iterations: %d", counter);

    /*4. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent),
    other params same as in 2. Expected: non GT_OK.*/

    lpmDbSize = 0;
    iter = 0;
    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;

    st = cpssExMxPmIpLpmDbMemSizeGet(lpmDbId, numOfEntriesInStep, &lpmDbSize, &iter);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "lpmDbId = %d", lpmDbId);

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /*5. Call with numOfEntriesInStep[0], other parameters same as in 2.
    Expected: GT_OK.*/

    lpmDbSize = 0;
    iter = 0;
    numOfEntriesInStep = 0;

    st = cpssExMxPmIpLpmDbMemSizeGet(lpmDbId, numOfEntriesInStep, &lpmDbSize, &iter);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "lpmDbId = %d, numOfEntriesInStep = %d", lpmDbId, numOfEntriesInStep);

    numOfEntriesInStep = 1;

    /*6. Call function with lpmDbSizePtr [NULL], other params same as in 2.
    Expected: GT_BAD_PTR.*/

    iter = 0;

    st = cpssExMxPmIpLpmDbMemSizeGet(lpmDbId, numOfEntriesInStep, NULL, &iter);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "lpmDbId = %d, lpmDbSizePtr = NULL", lpmDbId);

    /*7. Call function with iterPtr [NULL], other params same as in 2.
    Expected: GT_BAD_PTR.*/

    lpmDbSize = 0;

    st = cpssExMxPmIpLpmDbMemSizeGet(lpmDbId, numOfEntriesInStep, &lpmDbSize, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "lpmDbId = %d, iterPtr = NULL", lpmDbId);

    /* Delete default Lpm Db */

    st = prvUtfDeleteDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteDefaultLpmDB()");
}



/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmDbExport
(
    IN    GT_U32    lpmDbId,
    IN    GT_U32    numOfEntriesInStep,
    OUT   GT_U32    *lpmDbPtr,
    INOUT GT_U32    *iterPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmDbExport)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Call with lpmDbId [1], non-null lpmDbPtr [0] (4 bytes of memory allocated), numOfEntriesInStep[1], iterPtr[0]. Expected: GT_OK.
3. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent), other params same as in 2. Expected: non GT_OK.
4. Call with numOfEntriesInStep[0], other parameters same as in 2. Expected: GT_OK.
5. Call with lpmDbPtr [NULL] and other parameters same as in 2. Expected: GT_BAD_PTR.
6. Call with iterPtr [NULL] and other parameters same as in 2. Expected: GT_BAD_PTR.
7. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U32                  lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32                  numOfEntriesInStep;
    GT_U32                  *lpmDbPtr;
    GT_U32                  iter;

    /* 1. Create default Lpm Db */
    st = prvUtfCreateDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB()");

    /* 2. Call with lpmDbId [1], non-null lpmDbPtr [0] (4 bytes of memory allocated),
       numOfEntriesInStep[1], iterPtr[0]. Expected: GT_OK.  */

    lpmDbPtr = (GT_U32*)cpssOsMalloc(1000);
    UTF_VERIFY_NOT_EQUAL0_STRING_MAC((GT_U32)NULL, (GT_U32)lpmDbPtr, "cpssOsMalloc: Memory allocation error.");

    numOfEntriesInStep = 1;
    iter = 0;

    st = cpssExMxPmIpLpmDbExport(lpmDbId, numOfEntriesInStep, lpmDbPtr, &iter);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, numOfEntriesInStep);

    /*3. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555] (non-existent),
    other params same as in 2. Expected: non GT_OK.*/

    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;
    iter = 0;

    st = cpssExMxPmIpLpmDbExport(lpmDbId, numOfEntriesInStep, lpmDbPtr, &iter);
    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, numOfEntriesInStep);

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /*4. Call with numOfEntriesInStep[0], other parameters same as in 2.
    Expected: GT_OK.*/

    numOfEntriesInStep = 0;
    iter = 0;

    st = cpssExMxPmIpLpmDbExport(lpmDbId, numOfEntriesInStep, lpmDbPtr, &iter);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, numOfEntriesInStep);

    numOfEntriesInStep = 1;

    /*5. Call with lpmDbPtr [NULL] and other parameters same as in 2.
    Expected: GT_BAD_PTR.*/

    iter = 0;
    st = cpssExMxPmIpLpmDbExport(lpmDbId, numOfEntriesInStep, NULL, &iter);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lpmDbPtr = NULL", lpmDbId);

    /*6. Call with iterPtr [NULL] and other parameters same as in 2.
    Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmDbExport(lpmDbId, numOfEntriesInStep, lpmDbPtr, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, iterPtr = NULL", lpmDbId);

    cpssOsFree(lpmDbPtr);

    /* Delete default Lpm Db */
    st = prvUtfDeleteDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteDefaultLpmDB()");
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpLpmDbImport
(
    IN    GT_U32    lpmDbId,
    IN    GT_U32    *lpmDbPtr,
    IN    GT_U32    numOfEntriesInStep,
    INOUT GT_U32    *iterPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmDbImport)
{
/*
(no test pattern, no device iteration)
1. Create Lpm Db. Call cpssExMxPmIpLpmDbCreate with lpmDbId [1], lpmDbConfigPtr { memCfgArray(0) { routingSramCfgType [CPSS_EXMXPM_LPM_ALL_INTERNAL_E], sramsSizeArray [CPSS_SRAM_SIZE_NOT_USED_E], numOfSramsSizes [1] }, numOfMemCfg [1], protocolStack [CPSS_IP_PROTOCOL_IPV4_E], ipv6MemShare [0], numOfVirtualRouters [1] }. Expected: GT_OK.
2. Calculate size of Lpm Db, created in 1.
    2.1. First call cpssExMxPmIpLpmDbMemSizeGet with lpmDbId [1], numOfEntriesInStep [1], non-NULL lpmDbSizePtr with initial value [0], non-NULL iterPtr with initial value [0]. Expected: GT_OK.
    2.2. Continue calling cpssExMxPmIpLpmDbMemSizeGet with same pointers lpmDbSizePtr, iterPtr (without changing value), (other params same as in 2.1) until it return  iterPtr = 0. Expected: GT_OK for each call and iterPtr = 0 returned in less than 50 iterations.
3. Allocate memory to lpmDbPtr, size calculated in 2.
4. Call with lpmDbId [1], non-null lpmDbPtr [memory allocated in 3], numOfEntriesInStep[1], iterPtr[0].
Expected: GT_OK.
5. Call with lpmDbId [PRV_NON_EXISTENT_DB_ID_CNS = 5555] (non-existent), other params same as in 4. Expected: non GT_OK.
6. Call with numOfEntriesInStep[0], other parameters same as in 4. Expected: GT_OK.
7. Call with lpmDbPtr [NULL] and other parameters same as in 4. Expected: GT_BAD_PTR.
8. Call with iterPtr [NULL] and other parameters same as in 4. Expected: GT_BAD_PTR.
9. Delete Lpm Db. Call cpssExMxPmIpLpmDbDelete with lpmDbId[1]. Expected: GT_OK.
10. Free memory allocated in step 3 (lpmDbPtr).
*/
    GT_STATUS               st = GT_OK;
    GT_U32                  lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32                  *lpmDbPtr;
    GT_U32                  numOfEntriesInStep;
    GT_U32                  iter;

    GT_U32                  lpmDbSize;
    GT_U32                  counter;


    /* 1. Create default Lpm Db */
    st = prvUtfCreateDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateDefaultLpmDB()");

    /*2. Calculate size of Lpm Db, created in 1. */

    /*2.1. First call cpssExMxPmIpLpmDbMemSizeGet with lpmDbId [1],
    numOfEntriesInStep [1], non-NULL lpmDbSizePtr with initial value [0],
    non-NULL iterPtr with initial value [0]. Expected: GT_OK.*/

    /*2.2. Continue calling cpssExMxPmIpLpmDbMemSizeGet with same pointers
    lpmDbSizePtr, iterPtr (without changing value), (other params same as in
    2.1) until it return *iterPtr = 0. Expected: GT_OK for each call and
    iterPtr = 0 returned in less than 50 iterations. */

    numOfEntriesInStep = 1;
    lpmDbSize = 0;
    iter = 0;

    counter = 0;
    do
    {
        st = cpssExMxPmIpLpmDbMemSizeGet(lpmDbId, numOfEntriesInStep, &lpmDbSize, &iter);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmDbMemSizeGet: %d", lpmDbId);
        counter++;

    } while ((GT_OK == st) && (counter < 50) && (0 != iter));
    UTF_VERIFY_EQUAL1_STRING_MAC(0, iter,
        "Iterating cpssExMxPmIpLpmDbMemSizeGet is not finished correctly. Iterations: %d", counter);

    if ((GT_OK != st) || (0 != iter))
    {
        return;
    }

    /*3. Allocate memory to lpmDbPtr, size calculated in 2.*/

    lpmDbPtr = (GT_U32*)cpssOsMalloc(lpmDbSize);
    UTF_VERIFY_NOT_EQUAL0_STRING_MAC((GT_U32)NULL, (GT_U32)lpmDbPtr, "cpssOsMalloc: Memory allocation error.");

    /*4. Call with lpmDbId [1], non-null lpmDbPtr [memory allocated in 3],
    numOfEntriesInStep[1], iterPtr[0]. Expected: GT_OK.*/

    numOfEntriesInStep = 1;
    iter = 0;

    st = cpssExMxPmIpLpmDbImport(lpmDbId, lpmDbPtr, numOfEntriesInStep, &iter);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "lpmDbId = %d, numOfEntriesInStep = %d", lpmDbId, numOfEntriesInStep);

    /*5. Call with lpmDbId [IP_LPM_INVALID_DB_ID_CNS = 5555]
    (non-existent), other params same as in 4. Expected: non GT_OK.*/

    lpmDbId = IP_LPM_INVALID_DB_ID_CNS;
    iter = 0;

    st = cpssExMxPmIpLpmDbImport(lpmDbId, lpmDbPtr, numOfEntriesInStep, &iter);
    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "lpmDbId = %d", lpmDbId);

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /*6. Call with numOfEntriesInStep[0], other parameters same as in 4.
    Expected: GT_OK.*/

    numOfEntriesInStep = 0;
    iter = 0;

    st = cpssExMxPmIpLpmDbImport(lpmDbId, lpmDbPtr, numOfEntriesInStep, &iter);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "lpmDbId = %d, numOfEntriesInStep = %d", lpmDbId, numOfEntriesInStep);

    /*7. Call with lpmDbPtr [NULL] and other parameters same as in 4.
    Expected: GT_BAD_PTR.*/

    numOfEntriesInStep = 1;
    iter = 0;

    st = cpssExMxPmIpLpmDbImport(lpmDbId, NULL, numOfEntriesInStep, &iter);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "lpmDbId = %d, lpmDbPtr = NULL", lpmDbId);

    /*8. Call with iterPtr [NULL] and other parameters same as in 4.
    Expected: GT_BAD_PTR.*/

    st = cpssExMxPmIpLpmDbImport(lpmDbId, lpmDbPtr, numOfEntriesInStep, NULL);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "lpmDbId = %d, iterPtr = NULL", lpmDbId);

    /* 9. Delete default Lpm Db */
    st = prvUtfDeleteDefaultLpmDB();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteDefaultLpmDB()");

    /* 10. Free memory allocated in step 3 (lpmDbPtr). */
    if (lpmDbPtr != NULL)
    {
        /* if memory wasn't allocated */
        cpssOsFree(lpmDbPtr);
        lpmDbPtr = (GT_U32*)NULL;
    }

}



/*----------------------------------------------------------------------------*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4UcPrefixAddMax)
{
/*
    1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).

    2. Call cpssExMxPmIpLpmIpv4UcPrefixAdd with lpmDbId [1], vrId [0],
                   ipAddr.arIP [10. 0-254 . 0 - 254 . 1 - 254],  prefixLen [32],
                   nextHopPointer {routeEntryBaseMemAddr [0], blockSize [1],
                                   routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                   and override [GT_FALSE].
    Iterate with all IP. Maximum number of prefixes that can be added (with GT_OK)
    stored in the variable prefixCounterAdd. Iterate stopped when function return not GT_OK.
    Expected: GT_OK.

    3. Call cpssExMxPmIpLpmIpv4UcPrefixGet with the same parameters.

    Iterate with all IP. Maximum number of prefixes that can be getted (with GT_OK)
    stored in the variable prefixCounterGet. Iterate stopped when function return not GT_OK..
    prefixCounterGet must equal prefixCounterAdd.
    Expected: GT_OK, the same nextHopPointer and the same prefixCounter value.

    4. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush with lpmDbId [1], vrId [0] to invalidate changes.
    Expected: GT_OK.

    5. Analyse parameters with framework functions. Compare with old parameters value
    if they exists. Check parameters delta range and print result.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPADDR                               ipAddr;
    GT_U32                                  prefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointerGet;
    GT_BOOL                                 override  = GT_FALSE;

    /* max prefixes that can be added  with GT_OK*/
    GT_U32                  prefixCounterAdd = 0;
    /* max prefixes that can be getted with GT_OK and the same nextHopPointer */
    GT_U32                  prefixCounterGet = 0;

    GT_U32                  timeToAdd    = 0;
    GT_U32                  timeToGet    = 0;
    GT_U8                   paramCounter = 0;
    GT_U8                   paramNum     = 0;
    GT_BOOL                 paramDeltaCompareArr[IP_LPM_MAX_ARRAY_PARAMS_CNS];
    UTF_RESULT_PARAM_STC    utfResultParamArray[IP_LPM_MAX_ARRAY_PARAMS_CNS] =
                            {{"Added prefixes ", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Getted prefixes", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Time to add    ", 0, IP_LPM_PERCENT_TIME_CNS },
                             {"Time to get    ", 0, IP_LPM_PERCENT_TIME_CNS }};
    /*
        1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        2. Call cpssExMxPmIpLpmIpv4UcPrefixAdd with lpmDbId [1], vrId [0],
                       ipAddr.arIP [10. 0-254 . 0 - 254 . 1 - 254],  prefixLen [32],
                       nextHopPointer {routeEntryBaseMemAddr [0], blockSize [1],
                                       routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}
                       and override [GT_FALSE].
        Iterate with all IP. Maximum number of prefixes that can be added (with GT_OK)
        stored in the variable prefixCounterAdd. Iterate stopped when function return not GT_OK.
        Expected: GT_OK.
    */
    lpmDbId   = 1;
    vrId      = 0;
    prefixLen = 32;
    override  = GT_FALSE;
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    ipAddr.arIP[0] = 10; /*Unicast*/
    st = GT_OK;

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();

    /*     Iterate with all IP        */
    for (ipAddr.arIP[1] = 0; ipAddr.arIP[1] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[1]++)
        for (ipAddr.arIP[2] = 0; ipAddr.arIP[2] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[2]++)
            for (ipAddr.arIP[3] = 1; ipAddr.arIP[3] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[3]++)
            {
                st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen,
                                                    &nextHopPointer, override);
                prefixCounterAdd++;
            }

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToAdd);
    prefixCounterAdd--;

    /*
        3. Call cpssExMxPmIpLpmIpv4UcPrefixGet with the same parameters.

        Iterate with all IP. Maximum number of prefixes that can be getted (with GT_OK)
        stored in the variable prefixCounterGet. Iterate stopped when function return not GT_OK..
        prefixCounterGet must equal prefixCounterAdd.

        Expected: GT_OK, the same nextHopPointer and the same prefixCounter value.
    */
    lpmDbId        = 1;
    vrId           = 0;
    prefixLen      = 32;
    ipAddr.arIP[0] = 10;
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    st = GT_OK;

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();

    for (ipAddr.arIP[1] = 0; ipAddr.arIP[1] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[1]++)
        for (ipAddr.arIP[2] = 0; ipAddr.arIP[2] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[2]++)
            for (ipAddr.arIP[3] = 1; ipAddr.arIP[3] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[3]++)
            {
                st = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, prefixLen,
                                                    &nextHopPointerGet);
                if (GT_OK == st)
                {
                    /* Verifying values */
                    UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryBaseMemAddr,
                                                 nextHopPointerGet.routeEntryBaseMemAddr,
                        "get another nextHopPointerPtr->routeEntryBaseMemAddr than was set: %d, %d",
                                                 lpmDbId, vrId);
                    UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.blockSize,
                                                 nextHopPointerGet.blockSize,
                        "get another nextHopPointerPtr->blockSize than was set: %d, %d",
                                                 lpmDbId, vrId);
                    UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryMethod,
                                                 nextHopPointerGet.routeEntryMethod,
                        "get another nextHopPointerPtr->routeEntryMethod than was set: %d, %d",
                                                 lpmDbId, vrId);
                    prefixCounterGet++;
                }
                else if (prefixCounterGet >= prefixCounterAdd)
                {
                    break;
                }
            }

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToGet);

    /* prefixCounterGet must equal prefixCounterAdd */

    UTF_VERIFY_EQUAL2_STRING_MAC(prefixCounterAdd, prefixCounterGet,
        "get another prefixCounterGet than was set: prefixCounterAdd = %d, prefixCounterGet = %d",
                                 prefixCounterAdd, prefixCounterGet);
    /*
        4. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush to invalidate changes.
        Expected: GT_OK.
    */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d",
                                 lpmDbId, vrId);

    /* Delete default LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");

    /*
        5. Analyse parameters with framework functions. Compare with old parameters value
        if they exists. Check parameters delta range and print result.
        Expected: GT_OK.
    */
    /* Fill an array of values*/
    utfResultParamArray[paramNum++].paramValue = prefixCounterAdd;
    utfResultParamArray[paramNum++].paramValue = prefixCounterGet;
    utfResultParamArray[paramNum++].paramValue = timeToAdd;
    utfResultParamArray[paramNum++].paramValue = timeToGet;

    cpssOsBzero((GT_VOID*) paramDeltaCompareArr, sizeof(paramDeltaCompareArr));

    st = utfLogResultRun(utfResultParamArray, paramNum, paramDeltaCompareArr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "utfLogResultRun fail with error %d", st);

    for(paramCounter = 0; paramCounter < paramNum; paramCounter++)
    {
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, paramDeltaCompareArr[paramCounter],
                                     "Delta fail on param \t\"%s\"\n",
                                     utfResultParamArray[paramCounter].paramName);
    }
}


/*----------------------------------------------------------------------------*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4UcPrefixBulkAddMax)
{
/*
    1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).

    2. Call cpssExMxPmIpLpmIpv4UcPrefixBulkAdd. This step works about 40 seconds for 2000
       arrays. Please, be patient.
       Iterate with arrays. Maximum number of arrays that can be added (with GT_OK)
       stored in the variable counter.
    Expected: GT_OK.

    3. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush to invalidate changes.
    Expected: GT_OK.

    4. Analyse parameters with framework functions. Compare with old parameters value
    if they exists. Check parameters delta range and print result.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    CPSS_EXMXPM_IP_LPM_IPV4_UC_ADD_PREFIX_STC   ipv4PrefixArray[IP_LPM_BAR_CNS];
    GT_U32                  counter      = 0;
    GT_U8                   counter1     = 0;
    GT_U8                   counter2     = 0;
    GT_U8                   counter3     = 0;
    GT_U32                  timeToAdd    = 0;
    GT_U8                   paramCounter = 0;
    GT_U8                   paramNum     = 0;
    GT_BOOL                 paramDeltaCompareArr[IP_LPM_MAX_ARRAY_PARAMS_CNS];
    UTF_RESULT_PARAM_STC    utfResultParamArray[IP_LPM_MAX_ARRAY_PARAMS_CNS] =
                            {{"Added arrays", 0, IP_LPM_PERCENT_COUNT_CNS },
                             {"Time to add ", 0, IP_LPM_PERCENT_TIME_CNS  }};
    /*
        1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        2. Call cpssExMxPmIpLpmIpv4UcPrefixBulkAdd
             Iterate with arrays. Maximum number of arrays that can be added (with GT_OK)
             stored in the variable counter.
        Expected: GT_OK.
    */
    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();

    /* Fill array of prefixes */
    for(counter = 0; counter < IP_LPM_BAR_CNS; counter++)
    {
        ipv4PrefixArray[counter].vrId           = 0;
        ipv4PrefixArray[counter].ipAddr.arIP[0] = 10;

        if(counter2 > IP_LPM_IP_ADDR_MAX - 1)
        {
            counter1++;
            counter2 = 0;
        }
        if(counter3 > IP_LPM_IP_ADDR_MAX - 1)
        {
            counter2++;
            counter3 = 1;
        }
        else
        {
            counter3++;
        }

        ipv4PrefixArray[counter].ipAddr.arIP[1] = counter1;
        ipv4PrefixArray[counter].ipAddr.arIP[2] = counter2;
        ipv4PrefixArray[counter].ipAddr.arIP[3] = counter3;
        ipv4PrefixArray[counter].prefixLen      = 32;
        ipv4PrefixArray[counter].nextHopPointer.routeEntryBaseMemAddr = 0;
        ipv4PrefixArray[counter].nextHopPointer.blockSize             = 1;
        ipv4PrefixArray[counter].nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
        ipv4PrefixArray[counter].override       = GT_FALSE;
        ipv4PrefixArray[counter].returnStatus   = GT_OK;

        st = cpssExMxPmIpLpmIpv4UcPrefixBulkAdd(lpmDbId, counter, ipv4PrefixArray);
        if(st != GT_OK)
        {
            continue;
        }

        st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d",
                                     lpmDbId, vrId);
        if(st != GT_OK)
        {
            break;
        }
    }

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToAdd);

    /*
        3. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush to invalidate changes.
        Expected: GT_OK.
    */

    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d",
                                 lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");

    /*
        4. Analyse parameters with framework functions. Compare with old parameters value
           if they exists. Check parameters delta range and print result.
        Expected: GT_OK.
    */
    /* Fill an array of values*/
    utfResultParamArray[paramNum++].paramValue = counter;
    utfResultParamArray[paramNum++].paramValue = timeToAdd;

    cpssOsBzero((GT_VOID*) paramDeltaCompareArr, sizeof(paramDeltaCompareArr));

    st = utfLogResultRun(utfResultParamArray, IP_LPM_MAX_ARRAY_PARAMS_CNS, paramDeltaCompareArr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "utfLogResultRun fail with error %d", st);

    for(paramCounter = 0; paramCounter < paramNum; paramCounter++)
    {
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, paramDeltaCompareArr[paramCounter],
                                     "Delta fail on param \t\"%s\"\n",
                                     utfResultParamArray[paramCounter].paramName);
    }
}


/*--------------------------------------------------------------------------------------------*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4McEntryAddMax)
{
/*
    1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).

    2. Call cpssExMxPmIpLpmIpv4McEntryAdd with ipGroup (224.1.1.1)
                   ipSrc.arIP [10.17.0.1], ipSrcPrefixLen [32],
                   mcRoutePointerPtr {routeEntryBaseMemAddr [0],  blockSize [0],
                                      routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E]},
                   override [GT_FALSE].
    Iterate with as in cpssExMxPmIpLpmIpv4UcPrefixAddMax.
    Expected: GT_OK.

    3. Call cpssExMxPmIpLpmIpv4McEntryGet with the same parameters.
    Iterate with as in cpssExMxPmIpLpmIpv4UcPrefixAddMax.
    Expected: GT_OK and the same mcRoutePointerPtr. entryCounterGet must equal entryCounterAdd.

    4. Call cpssExMxPmIpLpmIpv4McEntriesFlush to invalidate changes.
    Expected: GT_OK.

    5. Analyse parameters with framework functions. Compare with old parameters value
    if they exists. Check parameters delta range and print result.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPADDR                               ipGroup;
    GT_IPADDR                               ipSrc;
    GT_U32                                  ipSrcPrefixLen  = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  mcRoutePointer;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  mcRoutePointerGet;
    GT_BOOL                                 override        = GT_FALSE;
    GT_U32                                  entryCounterAdd = 0;
    GT_U32                                  entryCounterGet = 0;

    GT_U32                  timeToAdd    = 0;
    GT_U32                  timeToGet    = 0;
    GT_U8                   paramCounter = 0;
    GT_U8                   paramNum     = 0;
    GT_BOOL                 paramDeltaCompareArr[IP_LPM_MAX_ARRAY_PARAMS_CNS];
    UTF_RESULT_PARAM_STC    utfResultParamArray[IP_LPM_MAX_ARRAY_PARAMS_CNS] =
                            {{"Added prefixes ", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Getted prefixes", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Time to add    ", 0, IP_LPM_PERCENT_TIME_CNS },
                             {"Time to get    ", 0, IP_LPM_PERCENT_TIME_CNS}};

    /*
        1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        2. Call cpssExMxPmIpLpmIpv4McEntryAdd with ipGroup (224.1.1.1)
                       ipSrc.arIP [10.17.0.1], ipSrcPrefixLen [32],
                       mcRoutePointerPtr {routeEntryBaseMemAddr [0],  blockSize [0],
                                          routeEntryMethod [CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E]},
                       override [GT_FALSE].
        Iterate with as in cpssExMxPmIpLpmIpv4UcPrefixAddMax.
        Expected: GT_OK.
    */
    ipGroup.arIP[0] = 224;
    ipGroup.arIP[1] = 1;
    ipGroup.arIP[2] = 1;
    ipGroup.arIP[3] = 1;

    ipSrc.arIP[0]   = 10;
    ipSrcPrefixLen  = 32;

    mcRoutePointer.routeEntryBaseMemAddr = 0;
    mcRoutePointer.blockSize             = 1;
    mcRoutePointer.routeEntryMethod      = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();

    for (ipSrc.arIP[1] = 0; ipSrc.arIP[1] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipSrc.arIP[1]++)
        for (ipSrc.arIP[2] = 0; ipSrc.arIP[2] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipSrc.arIP[2]++)
            for (ipSrc.arIP[3] = 1; ipSrc.arIP[3] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipSrc.arIP[3]++)
            {
                st = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc,
                                                   ipSrcPrefixLen, &mcRoutePointer, override);
                entryCounterAdd++;
            }

    entryCounterAdd--;

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToAdd);

    /*
        3. Call cpssExMxPmIpLpmIpv4McEntryGet with the same parameters.
        Iterate with as in cpssExMxPmIpLpmIpv4UcPrefixAddMax.
        Expected: GT_OK and the same mcRoutePointerPtr. entryCounterGet must equal entryCounterAdd.
    */

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    st = GT_OK;
    utfStartTimer();

    for (ipSrc.arIP[1] = 0; ipSrc.arIP[1] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipSrc.arIP[1]++)
        for (ipSrc.arIP[2] = 0; ipSrc.arIP[2] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipSrc.arIP[2]++)
            for (ipSrc.arIP[3] = 1; ipSrc.arIP[3] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipSrc.arIP[3]++)
            {
                st = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipGroup, ipSrc,
                                                   ipSrcPrefixLen, &mcRoutePointerGet);
                if (GT_OK == st)
                {
                    /* Verifying values */
                    UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.routeEntryBaseMemAddr,
                                                 mcRoutePointerGet.routeEntryBaseMemAddr,
                        "get another mcRoutePointerPtr->routeEntryBaseMemAddr than was set: %d, %d",
                                                 lpmDbId, vrId);
                    UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.blockSize,
                                                 mcRoutePointerGet.blockSize,
                        "get another mcRoutePointerPtr->blockSize than was set: %d, %d",
                                                 lpmDbId, vrId);
                    UTF_VERIFY_EQUAL2_STRING_MAC(mcRoutePointer.routeEntryMethod,
                                                 mcRoutePointerGet.routeEntryMethod,
                        "get another mcRoutePointerPtr->routeEntryMethod than was set: %d, %d",
                                                 lpmDbId, vrId);
                    entryCounterGet++;
                }
                else if (entryCounterGet >= entryCounterAdd)
                {
                    break;
                }
            }

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToGet);

    /* entryCounterGet must equal entryCounterAdd */

    UTF_VERIFY_EQUAL2_STRING_MAC(entryCounterAdd, entryCounterGet,
       "get another entryCounterGet than was set: entryCounterAdd = %d, entryCounterGet = %d",
                                 entryCounterAdd, entryCounterGet);

    /*
        4. Call cpssExMxPmIpLpmIpv4McEntriesFlush to invalidate changes.
        Expected: GT_OK.
    */
    /* Delete Prefixes */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4McEntriesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4McEntriesFlush: %d, %d",
                                 lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");

    /*
        5. Analyse parameters with framework functions. Compare with old parameters value
        if they exists. Check parameters delta range and print result.
        Expected: GT_OK.
    */
    /* Fill an array of values*/
    utfResultParamArray[paramNum++].paramValue = entryCounterAdd;
    utfResultParamArray[paramNum++].paramValue = entryCounterGet;
    utfResultParamArray[paramNum++].paramValue = timeToAdd;
    utfResultParamArray[paramNum++].paramValue = timeToGet;

    cpssOsBzero((GT_VOID*) paramDeltaCompareArr, sizeof(paramDeltaCompareArr));

    st = utfLogResultRun(utfResultParamArray, IP_LPM_MAX_ARRAY_PARAMS_CNS, paramDeltaCompareArr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "utfLogResultRun fail with error %d", st);

    for(paramCounter = 0; paramCounter < paramNum; paramCounter++)
    {
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, paramDeltaCompareArr[paramCounter],
                                     "Delta fail on param \t\"%s\"\n",
                                     utfResultParamArray[paramCounter].paramName);
    }
}


/*--------------------------------------------------------------------------------------------*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6UcPrefixBulkAddMax)
{
/*
    1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).

    2. Call cpssExMxPmIpLpmIpv6UcPrefixBulkAdd with lpmDbId [1], ipv6PrefixArrayLen [1],
         ipv6PrefixArrayPtr[0], vrId [0], ipAddr [::all arIP = 20:: ], prefixLen [32],
         nextHopPointer { routeEntryBaseMemAddr [0], blockSize [1],
         routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
         override [GT_FALSE] }.
    Expected: GT_OK and ipv6PrefixArrayPtr[0].returnStatus GT_OK.

    3. Invalidate changes.
    Expected: GT_OK.

    4. Analyse parameters with framework functions. Compare with old parameters value
       if they exists. Check parameters delta range and print result.
    Expected: GT_OK.
*/
    GT_STATUS               st      = GT_OK;
    GT_U32                  lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32                  vrId    = IP_LPM_DEFAULT_VR_ID_CNS;
    CPSS_EXMXPM_IP_LPM_IPV6_UC_ADD_PREFIX_STC  ipv6PrefixArray[IP_LPM_BAR_CNS];
    GT_IPV6ADDR             ipAddr;
    GT_U32                  prefixCounterAdd = 0;
    GT_U8                   counter13        = 0;
    GT_U8                   counter14        = 0;
    GT_U8                   counter15        = 1;

    /* max array's that can be added  with GT_OK*/
    GT_U32                  timeToAdd        = 0;
    GT_U8                   paramCounter     = 0;
    GT_U8                   paramNum         = 0;
    GT_BOOL                 paramDeltaCompareArr[IP_LPM_MAX_ARRAY_PARAMS_CNS];
    UTF_RESULT_PARAM_STC    utfResultParamArray[IP_LPM_MAX_ARRAY_PARAMS_CNS] =
                            {{"Added arrays", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Time to add ", 0, IP_LPM_PERCENT_TIME_CNS}};

    cpssOsMemSet((GT_VOID*) &(ipAddr), 20, sizeof(ipAddr));

    /* 1. Create default Lpm Db and virtual router */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");

    /*
    2. Call cpssExMxPmIpLpmIpv6UcPrefixBulkAdd with lpmDbId [1], ipv6PrefixArrayLen [1],
         ipv6PrefixArrayPtr[0], vrId [0], ipAddr [::all arIP = 20:: ], prefixLen [32],
         nextHopPointer { routeEntryBaseMemAddr [0], blockSize [1],
         routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},
         override [GT_FALSE] }.
    Expected: GT_OK and ipv6PrefixArrayPtr[0].returnStatus GT_OK.
    */

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();

    /*Fill array prefixes*/
    for(prefixCounterAdd = 0; prefixCounterAdd < IP_LPM_BAR_CNS; prefixCounterAdd++)
    {
        if(counter14 > IP_LPM_IP_ADDR_MAX - 1)
        {
            counter13++;
            counter14 = 0;
        }
        if(counter15 > IP_LPM_IP_ADDR_MAX - 1)
        {
            counter14++;
            counter15 = 1;
        }
        else
        {
            counter15++;
        }

        ipAddr.arIP[13] = counter13;
        ipAddr.arIP[14] = counter14;
        ipAddr.arIP[15] = counter15;
        ipv6PrefixArray[prefixCounterAdd].vrId = IP_LPM_DEFAULT_VR_ID_CNS;
        ipv6PrefixArray[prefixCounterAdd].ipAddr = ipAddr;
        ipv6PrefixArray[prefixCounterAdd].prefixLen = 128;
        ipv6PrefixArray[prefixCounterAdd].nextHopPointer.routeEntryBaseMemAddr = 0;
        ipv6PrefixArray[prefixCounterAdd].nextHopPointer.blockSize = 1;
        ipv6PrefixArray[prefixCounterAdd].nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
        ipv6PrefixArray[prefixCounterAdd].override = GT_FALSE;

        st = cpssExMxPmIpLpmIpv6UcPrefixBulkAdd(lpmDbId, prefixCounterAdd, ipv6PrefixArray);
        if(st != GT_OK)
        {
            continue;
        }

        st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);
        if(st != GT_OK)
        {
            break;
        }
    }

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToAdd);

    /*
        3. Invalidate changes.
        Expected: GT_OK.
    */

    st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");

    /*
        4. Analyse parameters with framework functions. Compare with old parameters value
           if they exists. Check parameters delta range and print result.
        Expected: GT_OK.
    */
    /* Fill an array of values*/
    utfResultParamArray[paramNum++].paramValue = prefixCounterAdd;
    utfResultParamArray[paramNum++].paramValue = timeToAdd;

    cpssOsBzero((GT_VOID*) paramDeltaCompareArr, sizeof(paramDeltaCompareArr));

    st = utfLogResultRun(utfResultParamArray, IP_LPM_MAX_ARRAY_PARAMS_CNS, paramDeltaCompareArr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "utfLogResultRun fail with error %d", st);

    for(paramCounter = 0; paramCounter < paramNum; paramCounter++)
    {
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, paramDeltaCompareArr[paramCounter],
                                     "Delta fail on param \t\"%s\"\n",
                                     utfResultParamArray[paramCounter].paramName);
    }
}


/*----------------------------------------------------------------------------*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6McEntryAddMax)
{
/*
    1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).

    2. Call cpssExMxPmIpLpmIpv6McEntryAdd with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]},
                 ipGroupRuleIndex[0], ipSrc{arIP [0xAA, ..., 0xAA]}, ipSrcPrefixLen[32],
                 mcRoutePointerPtr {routeEntryBaseMemAddr [0xFF],   blockSize [0],
                 routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}, override[GT_FALSE].
    Expected: GT_OK.

    3. Call cpssExMxPmIpLpmIpv6McEntryGet with not NULL pointers and other params from 2.
    Expected: GT_OK and the same counter.

    4. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete to invalidate changes.
    Expected: GT_OK.

    5. Analyse parameters with framework functions. Compare with old parameters value
    if they exists. Check parameters delta range and print result.
    Expected: GT_OK.

*/
    GT_STATUS                                st      = GT_OK;
    GT_U32                                   lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32                                   vrId    = IP_LPM_DEFAULT_VR_ID_CNS;
    GT_IPV6ADDR                              ipGroup;
    GT_U32                                   ipGroupRuleIndex;
    GT_U32                                   ipGroupRuleIndexGet;
    GT_IPV6ADDR                              ipSrc;
    GT_U32                                   ipSrcPrefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC   mcRoutePointer;
    GT_BOOL                                  override = GT_FALSE;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC   mcRoutePointerGet;

    GT_U32                  entryCounterAdd = 0;
    GT_U32                  entryCounterGet = 0;
    GT_U32                  timeToAdd       = 0;
    GT_U32                  timeToGet       = 0;
    GT_U8                   paramCounter    = 0;
    GT_U8                   paramNum        = 0;
    GT_BOOL                 paramDeltaCompareArr[IP_LPM_MAX_ARRAY_PARAMS_CNS];
    UTF_RESULT_PARAM_STC    utfResultParamArray[IP_LPM_MAX_ARRAY_PARAMS_CNS] =
                            {{"Added prefixes",  0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Getted prefixes", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Time to add",     0, IP_LPM_PERCENT_TIME_CNS },
                             {"Time to get",     0, IP_LPM_PERCENT_TIME_CNS }};

    cpssOsBzero((GT_VOID*) &ipSrc, sizeof(ipSrc));
    cpssOsBzero((GT_VOID*) &ipGroup, sizeof(ipGroup));
    cpssOsBzero((GT_VOID*) &mcRoutePointer, sizeof(mcRoutePointer));
    cpssOsBzero((GT_VOID*) &mcRoutePointerGet, sizeof(mcRoutePointerGet));

    /*
        1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");

    /*
        2. Call cpssExMxPmIpLpmIpv6McEntryAdd with lpmDbId [1], vrId [0], ipGroup{arIP [0x10, ..., 0x10]},
                     ipGroupRuleIndex[0], ipSrc{arIP [0xAA, ..., 0xAA]}, ipSrcPrefixLen[32],
                     mcRoutePointerPtr {routeEntryBaseMemAddr [0xFF],   blockSize [0],
                     routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]}, override[GT_FALSE].
        Expected: GT_OK.
    */

    cpssOsMemSet((GT_VOID*) &(ipGroup.arIP), 10, sizeof(ipGroup.arIP));
    ipGroup.arIP[0]  = 0xFF;  /* IPv6 MC group are of the form FF::/8 */
    ipGroupRuleIndex = 0;
    cpssOsMemSet((GT_VOID*) &(ipSrc.arIP), 0xAA, sizeof(ipSrc.arIP));
    ipSrcPrefixLen   = 128;
    mcRoutePointer.routeEntryBaseMemAddr = 0xFF;
    mcRoutePointer.blockSize = 1;
    mcRoutePointer.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    st = GT_OK;

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();

    for (ipSrc.arIP[1] = 0; ipSrc.arIP[1] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipSrc.arIP[1]++)
        for (ipSrc.arIP[2] = 0; ipSrc.arIP[2] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipSrc.arIP[2]++)
            for (ipSrc.arIP[3] = 1; ipSrc.arIP[3] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipSrc.arIP[3]++)
            {
                st = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupRuleIndex,
                                                   ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);
                entryCounterAdd++;
            }
    entryCounterAdd--;

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToAdd);

    /*
        3. Call cpssExMxPmIpLpmIpv6McEntryGet with not NULL pointers and other params from 2.
        Expected: GT_OK and the same counter.
    */

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    st = GT_OK;

    utfStartTimer();

    for (ipSrc.arIP[1] = 0; ipSrc.arIP[1] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipSrc.arIP[1]++)
        for (ipSrc.arIP[2] = 0; ipSrc.arIP[2] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipSrc.arIP[2]++)
            for (ipSrc.arIP[3] = 1; ipSrc.arIP[3] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipSrc.arIP[3]++)
            {
                st = cpssExMxPmIpLpmIpv6McEntryGet(lpmDBId, vrId, ipGroup, ipSrc, ipSrcPrefixLen,
                                                   &ipGroupRuleIndexGet, &mcRoutePointerGet);
                if(st == GT_OK)
                {
                    entryCounterGet++;
                }
                else if (entryCounterGet >= entryCounterAdd)
                {
                    break;
                }
            }

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToGet);

    /* entryCounterGet must equal entryCounterAdd */

    UTF_VERIFY_EQUAL2_STRING_MAC(entryCounterAdd, entryCounterGet,
        "get another entryCounterGet than was set: entryCounterAdd = %d, entryCounterGet = %d",
                                 entryCounterAdd, entryCounterGet);

    /*
        4. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete to invalidate changes.
        Expected: GT_OK.
    */
    /* Delete Entry */
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDBId, vrId);

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");

    /*
        5. Analyse parameters with framework functions. Compare with old parameters value
        if they exists. Check parameters delta range and print result.
        Expected: GT_OK.
    */
    /* Fill an array of values*/
    utfResultParamArray[paramNum++].paramValue = entryCounterAdd;
    utfResultParamArray[paramNum++].paramValue = entryCounterGet;
    utfResultParamArray[paramNum++].paramValue = timeToAdd;
    utfResultParamArray[paramNum++].paramValue = timeToGet;

    cpssOsBzero((GT_VOID*) paramDeltaCompareArr, sizeof(paramDeltaCompareArr));

    st = utfLogResultRun(utfResultParamArray, IP_LPM_MAX_ARRAY_PARAMS_CNS, paramDeltaCompareArr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "utfLogResultRun fail with error %d", st);

    for(paramCounter = 0; paramCounter < paramNum; paramCounter++)
    {
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, paramDeltaCompareArr[paramCounter],
                                     "Delta fail on param \t\"%s\"\n",
                                     utfResultParamArray[paramCounter].paramName);
    }
}

/*----------------------------------------------------------------------------*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6UcPrefixAddMax)
{
/*
    1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).

    2. Call cpssExMxPmIpLpmIpv6UcPrefixAdd with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: ],
    prefixLen [32], nextHopPointer { routeEntryBaseMemAddr [0], blockSize [1],
    routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},  override [GT_FALSE].
    Expected: GT_OK.

    3. Call cpssExMxPmIpLpmIpv6UcPrefixGet with with the same parameters as added.
    Expected: GT_OK and the same prefixCounter.

    4. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete to invalidate changes.
    Expected: GT_OK.

    5. Analyse parameters with framework functions. Compare with old parameters value
    if they exists. Check parameters delta range and print result.
    Expected: GT_OK.
*/
    GT_STATUS               st       = GT_OK;
    GT_U32                  lpmDbId  = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32                  vrId     = IP_LPM_DEFAULT_VR_ID_CNS;
    GT_IPV6ADDR             ipAddr;
    GT_U32                  prefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                 override = GT_FALSE;

    GT_U32                  prefixCounterAdd = 0;
    GT_U32                  prefixCounterGet = 0;
    GT_U32                  timeToAdd        = 0;
    GT_U32                  timeToGet        = 0;
    GT_U8                   paramCounter     = 0;
    GT_U8                   paramNum         = 0;
    GT_BOOL                 paramDeltaCompareArr[IP_LPM_MAX_ARRAY_PARAMS_CNS];
    UTF_RESULT_PARAM_STC    utfResultParamArray[IP_LPM_MAX_ARRAY_PARAMS_CNS] =
                            {{"Added prefixes ", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Getted prefixes", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Time to add    ", 0, IP_LPM_PERCENT_TIME_CNS },
                             {"Time to get    ", 0, IP_LPM_PERCENT_TIME_CNS }};

    /*
        1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");

    /*
        2. Call cpssExMxPmIpLpmIpv6UcPrefixAdd with lpmDbId [1], vrId [0], ipAddrPtr [::all arIP = 20:: ],
        prefixLen [32], nextHopPointer { routeEntryBaseMemAddr [0], blockSize [1],
        routeEntryMethod [CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E]},  override [GT_FALSE].
        Expected: GT_OK.
    */

    cpssOsMemSet((GT_VOID*) &(ipAddr), 20, sizeof(ipAddr));
    prefixLen = 128;
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize = 1;
    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();

    for (ipAddr.arIP[13] = 0; ipAddr.arIP[13] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[13]++)
        for (ipAddr.arIP[14] = 0; ipAddr.arIP[14] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[14]++)
            for (ipAddr.arIP[15] = 1; ipAddr.arIP[15] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[15]++)
            {
                st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen,
                                                    &nextHopPointer, override);
                prefixCounterAdd++;
            }
    prefixCounterAdd--;

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToAdd);

    /*
        3. Call cpssExMxPmIpLpmIpv6UcPrefixGet with with the same parameters as added.
        Expected: GT_OK and the same prefixCounter.
    */
    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    st = GT_OK;

    utfStartTimer();

    for (ipAddr.arIP[13] = 0; ipAddr.arIP[13] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[13]++)
        for (ipAddr.arIP[14] = 0; ipAddr.arIP[14] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[14]++)
            for (ipAddr.arIP[15] = 1; ipAddr.arIP[15] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[15]++)
            {
                st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen,
                                                    &nextHopPointer);
                if(st == GT_OK)
                {
                    prefixCounterGet++;
                }
                else if(prefixCounterGet >= prefixCounterAdd)
                {
                    break;
                }
            }

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToGet);

    /* prefixCounterGet must equal prefixCounterAdd */

    UTF_VERIFY_EQUAL2_STRING_MAC(prefixCounterAdd, prefixCounterGet,
        "get another prefixCounterGet than was set: prefixCounterAdd = %d, prefixCounterGet = %d",
                                 prefixCounterAdd, prefixCounterGet);

    /*
        4. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete to invalidate changes.
        Expected: GT_OK.
    */

    /* Delete Entry */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");

    /*
        5. Analyse parameters with framework functions. Compare with old parameters value
        if they exists. Check parameters delta range and print result.
        Expected: GT_OK.
    */
    /* Fill an array of values*/
    utfResultParamArray[paramNum++].paramValue = prefixCounterAdd;
    utfResultParamArray[paramNum++].paramValue = prefixCounterGet;
    utfResultParamArray[paramNum++].paramValue = timeToAdd;
    utfResultParamArray[paramNum++].paramValue = timeToGet;

    cpssOsBzero((GT_VOID*) paramDeltaCompareArr, sizeof(paramDeltaCompareArr));

    st = utfLogResultRun(utfResultParamArray, IP_LPM_MAX_ARRAY_PARAMS_CNS, paramDeltaCompareArr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "utfLogResultRun fail with error %d", st);

    for(paramCounter = 0; paramCounter < paramNum; paramCounter++)
    {
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, paramDeltaCompareArr[paramCounter],
                                     "Delta fail on param \t\"%s\"\n",
                                     utfResultParamArray[paramCounter].paramName);
    }
}

/*----------------------------------------------------------------------------*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4UcPrefixAddDistributed)
{
/*
    1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).

    2. Call cpssExMxPmIpLpmIpv4UcPrefixAdd, generate random prefixes.
    Expected: GT_OK.

    3. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete to invalidate changes.
    Expected: GT_OK.

    4. Analyse parameters with framework functions. Compare with old parameters value
    if they exists. Check parameters delta range and print result.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPADDR                               ipAddr;
    GT_U8                                   prefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointerGet;
    GT_BOOL                                 override  = GT_FALSE;

    /* max prefixes that can be added  with GT_OK*/
    GT_U32                  prefixCounterAdd = 0;
    /* max prefixes that can be getted with GT_OK and the same nextHopPointer */
    GT_U32                  prefixCounterGet = 0;

    GT_U32                  timeToAdd    = 0;
    GT_U8                   paramCounter = 0;
    GT_U8                   paramNum     = 0;
    GT_BOOL                 paramDeltaCompareArr[IP_LPM_MAX_ARRAY_PARAMS_CNS];
    UTF_RESULT_PARAM_STC    utfResultParamArray[IP_LPM_MAX_ARRAY_PARAMS_CNS] =
                            {{"Added prefixes ", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Getted prefixes", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Time to add    ", 0, IP_LPM_PERCENT_TIME_BGP_CNS}};
    /*
        1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        2. Call cpssExMxPmIpLpmIpv4UcPrefixAdd.
        Expected: GT_OK.
    */
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    /*Fourth byte can be just zero because we not used mask more than 24 */
    ipAddr.arIP[3] = 0;

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();

    /* Init prefix generator */
    prvUtfInitGenerate();

    while (st == GT_OK)
    {
        /* generate prefix */
        st = prvUtfGeneratePrefix(&ipAddr.arIP[0], &ipAddr.arIP[1], &ipAddr.arIP[2],
                                      &prefixLen);

        /* add the current prefix */
        st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr,
                                            prefixLen, &nextHopPointer, override);
        /* printout creates huge log file and therefor was removed
        PRV_UTF_LOG5_MAC("Last prefix %d.%d.%d.%d, prefixLen = %d ", ipAddr.arIP[0],
                         ipAddr.arIP[1], ipAddr.arIP[2], ipAddr.arIP[3], prefixLen);
        PRV_UTF_LOG1_MAC("st = %d\n", st);
        */

        if (st == GT_OK)
        {
            prefixCounterAdd++;

            /* Call cpssExMxPmIpLpmIpv4UcPrefixGet with the same parameters.  */

            st = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointerGet);

            if (GT_OK == st)
            {
                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryBaseMemAddr,
                                             nextHopPointerGet.routeEntryBaseMemAddr,
                     "get another nextHopPointerPtr->routeEntryBaseMemAddr than was set: %d, %d",
                                             lpmDbId, vrId);
                UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.blockSize,
                                             nextHopPointerGet.blockSize,
                     "get another nextHopPointerPtr->blockSize than was set: %d, %d",
                                             lpmDbId, vrId);
                UTF_VERIFY_EQUAL3_STRING_MAC(nextHopPointer.routeEntryMethod,
                                             nextHopPointerGet.routeEntryMethod,
                     "get another nextHopPointerPtr->routeEntryMethod than was set: %d, %d, prefixCounterGet = %d",
                                             lpmDbId, vrId, prefixCounterGet);
                prefixCounterGet++;
            }
        }
    }

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToAdd);

    /* prefixCounterGet must equal prefixCounterAdd */

    UTF_VERIFY_EQUAL2_STRING_MAC(prefixCounterAdd, prefixCounterGet,
        "get another prefixCounterGet than was set: prefixCounterAdd = %d, prefixCounterGet = %d",
                                 prefixCounterAdd, prefixCounterGet);
    /*
        3. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush to invalidate changes.
        Expected: GT_OK.
    */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d",
                                 lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");

    /*
        4. Analyse parameters with framework functions. Compare with old parameters value
        if they exists. Check parameters delta range and print result.
        Expected: GT_OK.
    */
    utfResultParamArray[paramNum++].paramValue = prefixCounterAdd;
    utfResultParamArray[paramNum++].paramValue = prefixCounterGet;
    utfResultParamArray[paramNum++].paramValue = timeToAdd;

    cpssOsBzero((GT_VOID*) paramDeltaCompareArr, sizeof(paramDeltaCompareArr));

    st = utfLogResultRun(utfResultParamArray, IP_LPM_MAX_ARRAY_PARAMS_CNS, paramDeltaCompareArr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "utfLogResultRun fail with error %d", st);

    for(paramCounter = 0; paramCounter < paramNum; paramCounter++)
    {
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, paramDeltaCompareArr[paramCounter],
                                     "Delta fail on param \t\"%s\"\n",
                                     utfResultParamArray[paramCounter].paramName);
    }
}

/*---------------------------------------------------------------------------------------------*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4UcPrefixAddDistributedAndExactTime)
{
/*
    1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).

    2. Call cpssExMxPmIpLpmIpv4UcPrefixAdd and generate prefixes.
    Expected: GT_OK.

    3. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush  to invalidate changes.
    Expected: GT_OK.

    4. Call cpssExMxPmIpLpmIpv4UcPrefixAdd to add exact prefixes.
    Expected: GT_OK.

    5. Delete virtual router. Call cpssExMxPmIpLpmVirtualRouterDelete to invalidate changes.
    Expected: GT_OK.

    6. Analyse parameters with framework functions. Compare with old parameters value
       if they exists. Check parameters delta range and print result.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPADDR                               ipAddr;
    GT_U8                                   prefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                                 override  = GT_FALSE;

    /* max prefixes that can be added  with GT_OK*/
    GT_U32                  prefixCounterAdd1 = 0;
    GT_U32                  prefixCounterAdd2 = 0;
    GT_U32                  timeToAdd1        = 0;
    GT_U32                  timeToAdd2        = 0;

    GT_U8                   paramCounter = 0;
    GT_U8                   paramNum     = 0;
    GT_BOOL                 paramDeltaCompareArr[IP_LPM_MAX_ARRAY_PARAMS_CNS];
    UTF_RESULT_PARAM_STC    utfResultParamArray[IP_LPM_MAX_ARRAY_PARAMS_CNS] =
                            {{"Added prefixes (BGP)  ", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Added prefixes (exact)", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Time to add (BGP)     ", 0, IP_LPM_PERCENT_TIME_BGP_CNS },
                             {"Time to add (exact)   ", 0, IP_LPM_PERCENT_TIME_CNS }};

    /*
        1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        2. Call cpssExMxPmIpLpmIpv4UcPrefixAdd read params from external array.
        Expected: GT_OK.
    */
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    /*Init arIP with zeroes */
    ipAddr.arIP[0] = 0;
    ipAddr.arIP[2] = 0;
    /*Fourth byte can be just zero because we not used mask more than 24 */
    ipAddr.arIP[3] = 0;

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();

    /* Init prefix generator */
    prvUtfInitGenerate();

    while (st == GT_OK)
    {
        /* generate prefix */
        st = prvUtfGeneratePrefix(&ipAddr.arIP[0], &ipAddr.arIP[1], &ipAddr.arIP[2],
                                      &prefixLen);

        /* add the current prefix */
        st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr,
                                            prefixLen, &nextHopPointer, override);

        if (st != GT_OK)
        {
            /*
            PRV_UTF_LOG5_MAC("\nLast prefix %d.%d.%d.%d, prefixLen = %d\n\n", ipAddr.arIP[0],
                             ipAddr.arIP[1], ipAddr.arIP[2], ipAddr.arIP[3], prefixLen);
            PRV_UTF_LOG1_MAC("st = %d\n", st);
            */
            break;
        }
        else
        {
            prefixCounterAdd1++;
        }
    }
    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToAdd1);
    /*
        3. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush  to invalidate changes.
        Expected: GT_OK.
    */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;
    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d",
                                 lpmDbId, vrId);

    /*
        4. Call cpssExMxPmIpLpmIpv4UcPrefixAdd to add exact prefixes.
        Expected: GT_OK.
    */
    lpmDbId   = 1;
    vrId      = 0;
    prefixLen = 32;
    override  = GT_FALSE;
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    ipAddr.arIP[0] = 10;
    st = GT_OK;

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();

    /*     Iterate with all IP        */
    for (ipAddr.arIP[1] = 0; ipAddr.arIP[1] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[1]++)
        for (ipAddr.arIP[2] = 0; ipAddr.arIP[2] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[2]++)
            for (ipAddr.arIP[3] = 1; ipAddr.arIP[3] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[3]++)
            {
                st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen,
                                                    &nextHopPointer, override);
                if(st != GT_OK || prefixCounterAdd2 > prefixCounterAdd1)
                {
                    break;
                }
                prefixCounterAdd2++;
            }
    prefixCounterAdd2--;

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToAdd2);

    UTF_VERIFY_EQUAL0_STRING_MAC(prefixCounterAdd1, prefixCounterAdd2,
                                 "Prefix counters are different \n");
    /*
        5. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush to invalidate changes.
        Expected: GT_OK.
    */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d",
                                 lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");

    /*
        6. Analyse parameters with framework functions. Compare with old parameters value
           if they exists. Check parameters delta range and print result.
        Expected: GT_OK.
    */
    utfResultParamArray[paramNum++].paramValue = prefixCounterAdd1;
    utfResultParamArray[paramNum++].paramValue = prefixCounterAdd2;
    utfResultParamArray[paramNum++].paramValue = timeToAdd1;
    utfResultParamArray[paramNum++].paramValue = timeToAdd2;

    cpssOsBzero((GT_VOID*) paramDeltaCompareArr, sizeof(paramDeltaCompareArr));

    st = utfLogResultRun(utfResultParamArray, IP_LPM_MAX_ARRAY_PARAMS_CNS, paramDeltaCompareArr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "utfLogResultRun fail with error %d", st);

    for(paramCounter = 0; paramCounter < paramNum; paramCounter++)
    {
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, paramDeltaCompareArr[paramCounter],
                                     "Delta fail on param \t\"%s\"\n",
                                     utfResultParamArray[paramCounter].paramName);
    }
}

/*---------------------------------------------------------------------------------------------*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6UcPrefixAddDistributed)
{
/*
    1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).

    2. Call cpssExMxPmIpLpmIpv4UcPrefixAdd, generate IPV6 BGP prefixes.
    Expected: GT_OK.

    3. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush to invalidate changes.
    Expected: GT_OK.

    4. Analyse parameters with framework functions. Compare with old parameters value
    if they exists. Check parameters delta range and print result.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPV6ADDR                             ipAddr;
    GT_U8                                   prefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                                 override  = GT_FALSE;

    /* max prefixes that can be added  with GT_OK*/
    GT_U32                  prefixCounterAdd = 0;
    /* max prefixes that can be getted with GT_OK and the same nextHopPointer */
    GT_U32                  prefixCounterGet = 0;

    GT_U32                  timeToAdd    = 0;
    GT_U8                   paramCounter = 0;
    GT_U8                   paramNum     = 0;
    GT_BOOL                 paramDeltaCompareArr[IP_LPM_MAX_ARRAY_PARAMS_CNS];
    UTF_RESULT_PARAM_STC    utfResultParamArray[IP_LPM_MAX_ARRAY_PARAMS_CNS] =
                            {{"Added prefixes ", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Getted prefixes", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Time to add    ", 0, IP_LPM_PERCENT_TIME_BGP_CNS }};
    /*
        1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        2. Call cpssExMxPmIpLpmIpv6UcPrefixAdd with generating prefixes.
        Expected: GT_OK.
    */
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    cpssOsMemSet((GT_VOID*) &(ipAddr), 20, sizeof(ipAddr));

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();

    /* Init prefix generator */
    prvUtfInitGenerate();

    while (st == GT_OK)
    {
        /* generate prefix */
        st = prvUtfGeneratePrefix(&ipAddr.arIP[0], &ipAddr.arIP[1], &ipAddr.arIP[2],
                                      &prefixLen);

        /* add the current prefix */
        st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen,
                                            &nextHopPointer, override);
        if (st == GT_OK)
        {
            prefixCounterAdd++;

            lpmDbId        = 1;
            vrId           = 0;
            nextHopPointer.routeEntryBaseMemAddr = 0;
            nextHopPointer.blockSize             = 1;
            nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

            /* get the current prefix */
            st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer);
            if (st == GT_OK)
            {
                prefixCounterGet++;
            }
            else
            {
                st = GT_OK;
            }
        }
    }

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToAdd);

    /* prefixCounterGet must equal prefixCounterAdd */
    UTF_VERIFY_EQUAL2_STRING_MAC(prefixCounterAdd, prefixCounterGet,
        "get another prefixCounterGet than was set: prefixCounterAdd = %d, prefixCounterGet = %d",
                                 prefixCounterAdd, prefixCounterGet);
    /*
        3. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush to invalidate changes.
        Expected: GT_OK.
    */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d",
                                 lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");

    /*
        4. Analyse parameters with framework functions. Compare with old parameters value
        if they exists. Check parameters delta range and print result.
        Expected: GT_OK.
    */
    utfResultParamArray[paramNum++].paramValue = prefixCounterAdd;
    utfResultParamArray[paramNum++].paramValue = prefixCounterGet;
    utfResultParamArray[paramNum++].paramValue = timeToAdd;

    cpssOsBzero((GT_VOID*) paramDeltaCompareArr, sizeof(paramDeltaCompareArr));

    st = utfLogResultRun(utfResultParamArray, IP_LPM_MAX_ARRAY_PARAMS_CNS, paramDeltaCompareArr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "utfLogResultRun fail with error %d", st);

    for(paramCounter = 0; paramCounter < paramNum; paramCounter++)
    {
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, paramDeltaCompareArr[paramCounter],
                                     "Delta fail on param \t\"%s\"\n",
                                     utfResultParamArray[paramCounter].paramName);
    }
}

/*--------------------------------------------------------------------------------------------*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv4UcPrefixAddDeleteAdd)
{
/*
    1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).

    2. Call cpssExMxPmIpLpmIpv4UcPrefixAdd.
    Expected: GT_OK.

    3. Call cpssExMxPmIpLpmIpv4UcPrefixDelete with the same params.
    Iterate with all IP. Maximum number of prefixes that can be getted (with GT_OK)
    stored in the variable prefixCounterGet.
    Expected: GT_OK, and the same prefixCounterDeleted.

    4. Reply step 2.
    Expected: GT_OK and the same prefixCounterAdd.

    5. Call cpssExMxPmIpLpmIpv4UcPrefixGet with the same params.

    6. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush to invalidate changes.
    Expected: GT_OK.

    7. Analyse parameters with framework functions. Compare with old parameters value
       if they exists. Check parameters delta range and print result.
    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPADDR                               ipAddr;
    GT_U32                                  prefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointerGet;
    GT_BOOL                                 override  = GT_FALSE;

    /* max prefixes that can be added  with GT_OK*/
    GT_U32                  prefixCounterAdd1   = 0;
    GT_U32                  prefixCounterAdd2   = 0;
    /* max prefixes that can be deleted with GT_OK*/
    GT_U32                  prefixCounterDelete = 0;
    /* max prefixes that can be getted with GT_OK and the same nextHopPointer */
    GT_U32                  prefixCounterGet    = 0;

    GT_U32                  timeToAdd    = 0;
    GT_U32                  timeToGet    = 0;
    GT_U8                   paramCounter = 0;
    GT_U8                   paramNum     = 0;
    GT_BOOL                 paramDeltaCompareArr[IP_LPM_MAX_ARRAY_PARAMS_CNS];
    UTF_RESULT_PARAM_STC    utfResultParamArray[IP_LPM_MAX_ARRAY_PARAMS_CNS] =
                            {{"Added prefixes1 ", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Deleted prefixes", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Added prefixes2 ", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Getted prefixes ", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Time to add     ", 0, IP_LPM_PERCENT_TIME_CNS },
                             {"Time to get     ", 0, IP_LPM_PERCENT_TIME_CNS }};

    /*
        1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        2. Call cpssExMxPmIpLpmIpv4UcPrefixAdd.
        Expected: GT_OK.
    */
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    ipAddr.arIP[0] = 10;
    prefixLen = 32;

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();

    /*     Iterate with all IP        */
    for (ipAddr.arIP[1] = 0; ipAddr.arIP[1] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[1]++)
        for (ipAddr.arIP[2] = 0; ipAddr.arIP[2] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[2]++)
            for (ipAddr.arIP[3] = 1; ipAddr.arIP[3] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[3]++)
            {
                st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen,
                                                    &nextHopPointer, override);
                prefixCounterAdd1++;
            }

    prefixCounterAdd1--;

    /*
        3. Call cpssExMxPmIpLpmIpv4UcPrefixDelete with the same params.
        Expected: GT_OK, and the same prefixCounterDeleted.
    */
    /*     Iterate with all IP        */
    st = GT_OK;

    for (ipAddr.arIP[1] = 0; ipAddr.arIP[1] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[1]++)
        for (ipAddr.arIP[2] = 0; ipAddr.arIP[2] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[2]++)
            for (ipAddr.arIP[3] = 1; ipAddr.arIP[3] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[3]++)
            {
                st = cpssExMxPmIpLpmIpv4UcPrefixDelete(lpmDbId, vrId, ipAddr, prefixLen);
                prefixCounterDelete++;
            }

    prefixCounterDelete--;

    /*
    4. Reply step 2.
    Expected: GT_OK and the same prefixCounterAdd.
    */
    /*     Iterate with all IP        */
    st = GT_OK;

    for (ipAddr.arIP[1] = 0; ipAddr.arIP[1] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[1]++)
        for (ipAddr.arIP[2] = 0; ipAddr.arIP[2] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[2]++)
            for (ipAddr.arIP[3] = 1; ipAddr.arIP[3] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[3]++)
            {
                st = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen,
                                                    &nextHopPointer, override);
                prefixCounterAdd2++;
            }

    prefixCounterAdd2--;

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToAdd);

    /*
        5. Call cpssExMxPmIpLpmIpv4UcPrefixGet with the same params.
        Expected: GT_OK
    */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;
    prefixLen = 32;
    ipAddr.arIP[0] = 10;

    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    prefixCounterGet  = 0;
    st = GT_OK;

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();

    for (ipAddr.arIP[1] = 0; ipAddr.arIP[1] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[1]++)
        for (ipAddr.arIP[2] = 0; ipAddr.arIP[2] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[2]++)
            for (ipAddr.arIP[3] = 1; ipAddr.arIP[3] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[3]++)
            {
                st = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, prefixLen,
                                                    &nextHopPointerGet);
                if (GT_OK == st)
                {
                    /* Verifying values */
                    UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryBaseMemAddr,
                                                 nextHopPointerGet.routeEntryBaseMemAddr,
                        "get another nextHopPointerPtr->routeEntryBaseMemAddr than was set: %d, %d",
                                                 lpmDbId, vrId);
                    UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.blockSize,
                                                 nextHopPointerGet.blockSize,
                        "get another nextHopPointerPtr->blockSize than was set: %d, %d",
                                                 lpmDbId, vrId);
                    UTF_VERIFY_EQUAL2_STRING_MAC(nextHopPointer.routeEntryMethod,
                                                 nextHopPointerGet.routeEntryMethod,
                        "get another nextHopPointerPtr->routeEntryMethod than was set: %d, %d",
                                                 lpmDbId, vrId);
                    prefixCounterGet++;
                }
                else if (prefixCounterGet >= prefixCounterAdd2)
                {
                    break;
                }
            }

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToGet);

    /*  prefixCounterAdd1, prefixCounterAdd2, prefixCounterDelete and prefixCounterGet
        must equal prefixCounterAdd */

    UTF_VERIFY_EQUAL2_STRING_MAC(prefixCounterAdd1, prefixCounterAdd2,
        "get another prefixCounterAdd2 than prefixCounterAdd1: prefixCounterAdd1 = %d, prefixCounterAdd2 = %d",
                                 prefixCounterAdd1, prefixCounterAdd2);

    UTF_VERIFY_EQUAL2_STRING_MAC(prefixCounterAdd1, prefixCounterDelete,
        "get another prefixCounterDelete than prefixCounterAdd1: prefixCounterDelete = %d, prefixCounterAdd1 = %d",
                                 prefixCounterAdd1, prefixCounterAdd2);

    UTF_VERIFY_EQUAL2_STRING_MAC(prefixCounterAdd2, prefixCounterGet,
        "get another prefixCounterGet than was set: prefixCounterAdd = %d, prefixCounterGet = %d",
                                 prefixCounterAdd2, prefixCounterGet);

    /*
        6. Call cpssExMxPmIpLpmIpv4UcPrefixesFlush to invalidate changes.
        Expected: GT_OK.
    */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d",
                                 lpmDbId, vrId);

    /* Delete defauld LpmDB and Virtual Routers */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter");

    /*
        7. Analyse parameters with framework functions. Compare with old parameters value
           if they exists. Check parameters delta range and print result.
        Expected: GT_OK.
    */
    utfResultParamArray[paramNum++].paramValue = prefixCounterAdd1;
    utfResultParamArray[paramNum++].paramValue = prefixCounterDelete;
    utfResultParamArray[paramNum++].paramValue = prefixCounterAdd2;
    utfResultParamArray[paramNum++].paramValue = prefixCounterGet;
    utfResultParamArray[paramNum++].paramValue = timeToAdd;
    utfResultParamArray[paramNum++].paramValue = timeToGet;

    cpssOsBzero((GT_VOID*) paramDeltaCompareArr, sizeof(paramDeltaCompareArr));

    st = utfLogResultRun(utfResultParamArray, IP_LPM_MAX_ARRAY_PARAMS_CNS, paramDeltaCompareArr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "utfLogResultRun fail with error %d", st);

    for(paramCounter = 0; paramCounter < paramNum; paramCounter++)
    {
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, paramDeltaCompareArr[paramCounter],
                                     "Delta fail on param \t\"%s\"\n",
                                     utfResultParamArray[paramCounter].paramName);
    }
}

/*---------------------------------------------------------------------------------------------*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6UcPrefixAddDeleteAdd)
{
/*
    1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).

    2. Call cpssExMxPmIpLpmIpv6UcPrefixAdd.
    Expected: GT_OK.

    3. Call cpssExMxPmIpLpmIpv6UcPrefixDelete with the same params.
    Iterate with all IP. Maximum number of prefixes that can be getted (with GT_OK)
    stored in the variable prefixCounterGet.
    Expected: GT_OK, and the same prefixCounterDeleted.

    4. Reply step 2.
    Expected: GT_OK and the same prefixCounterAdd.

    5. Call cpssExMxPmIpLpmIpv6UcPrefixGet with the same params.
    Expected: GT_OK.

    6. Call cpssExMxPmIpLpmIpv6UcPrefixesFlush to invalidate changes.
    Expected: GT_OK.

    7. Analyse parameters with framework functions. Compare with old parameters value
       if they exists. Check parameters delta range and print result.
    Expected: GT_OK.

*/
    GT_STATUS               st      = GT_OK;
    GT_U32                  lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32                  vrId    = IP_LPM_DEFAULT_VR_ID_CNS;
    GT_IPV6ADDR             ipAddr;
    GT_U32                  prefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                 override;

    /* max prefixes that can be added  with GT_OK*/
    GT_U32                  prefixCounterAdd1   = 0;
    GT_U32                  prefixCounterAdd2   = 0;

    /* max prefixes that can be deleted with GT_OK*/
    GT_U32                  prefixCounterDelete = 0;
    GT_U32                  prefixCounterGet    = 0;
    GT_U32                  timeToAdd           = 0;
    GT_U32                  timeToGet           = 0;
    GT_U8                   paramCounter        = 0;
    GT_U8                   paramNum            = 0;
    GT_BOOL                 paramDeltaCompareArr[IP_LPM_MAX_ARRAY_PARAMS_CNS];
    UTF_RESULT_PARAM_STC    utfResultParamArray[IP_LPM_MAX_ARRAY_PARAMS_CNS] =
                            {{"Added prefixes1 ", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Deleted prefixes", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Added prefixes2 ", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Getted prefixes ", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Time to add     ", 0, IP_LPM_PERCENT_TIME_CNS },
                             {"Time to get     ", 0, IP_LPM_PERCENT_TIME_CNS }};

    /*
        1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd()");

    /*
        2. Call cpssExMxPmIpLpmIpv6UcPrefixAdd.
        Expected: GT_OK.
    */

    cpssOsMemSet((GT_VOID*) &(ipAddr), 20, sizeof(ipAddr));
    prefixLen = 128;
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize = 1;
    nextHopPointer.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    override = GT_FALSE;

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();

    for (ipAddr.arIP[13] = 0; ipAddr.arIP[13] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[13]++)
        for (ipAddr.arIP[14] = 0; ipAddr.arIP[14] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[14]++)
            for (ipAddr.arIP[15] = 1; ipAddr.arIP[15] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[15]++)
            {
                st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen,
                                                    &nextHopPointer, override);
                prefixCounterAdd1++;
            }

    prefixCounterAdd1--;

    /*
        3. Call cpssExMxPmIpLpmIpv6UcPrefixDelete with the same params.
        Iterate with all IP. Maximum number of prefixes that can be getted (with GT_OK)
        stored in the variable prefixCounterGet.
        Expected: GT_OK, and the same prefixCounterDeleted.
    */
    st = GT_OK;

    for (ipAddr.arIP[13] = 0; ipAddr.arIP[13] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[13]++)
        for (ipAddr.arIP[14] = 0; ipAddr.arIP[14] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[14]++)
            for (ipAddr.arIP[15] = 1; ipAddr.arIP[15] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[15]++)
            {
                st = cpssExMxPmIpLpmIpv6UcPrefixDelete(lpmDbId, vrId, &ipAddr, prefixLen);
                prefixCounterDelete++;
            }

    prefixCounterDelete--;

    /*
        4. Reply step 2.
        Expected: GT_OK and the same prefixCounterAdd.
    */
    st = GT_OK;

    for (ipAddr.arIP[13] = 0; ipAddr.arIP[13] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[13]++)
        for (ipAddr.arIP[14] = 0; ipAddr.arIP[14] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[14]++)
            for (ipAddr.arIP[15] = 1; ipAddr.arIP[15] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[15]++)
            {
                st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen,
                                                    &nextHopPointer, override);
                prefixCounterAdd2++;
            }
    prefixCounterAdd2--;

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToAdd);

    /*
        5. Call cpssExMxPmIpLpmIpv6UcPrefixGet with the same params.
        Expected: GT_OK.
    */
    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    st = GT_OK;

    utfStartTimer();

    for (ipAddr.arIP[13] = 0; ipAddr.arIP[13] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[13]++)
        for (ipAddr.arIP[14] = 0; ipAddr.arIP[14] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[14]++)
            for (ipAddr.arIP[15] = 1; ipAddr.arIP[15] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[15]++)
            {
                st = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipAddr,
                                                    prefixLen, &nextHopPointer);
                if(st == GT_OK)
                {
                    prefixCounterGet++;
                }
                else if (prefixCounterGet >= prefixCounterAdd2)
                {
                    break;
                }
            }

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToGet);

    /*  prefixCounterAdd1, prefixCounterAdd2, prefixCounterDelete and prefixCounterGet
        must equal prefixCounterAdd */

    UTF_VERIFY_EQUAL2_STRING_MAC(prefixCounterAdd1, prefixCounterAdd2,
        "get another prefixCounterAdd2 than prefixCounterAdd1: prefixCounterAdd1 = %d, prefixCounterAdd2 = %d",
                                 prefixCounterAdd1, prefixCounterAdd2);

    UTF_VERIFY_EQUAL2_STRING_MAC(prefixCounterAdd1, prefixCounterDelete,
        "get another prefixCounterDelete than prefixCounterAdd1: prefixCounterDelete = %d, prefixCounterAdd1 = %d",
                                 prefixCounterAdd1, prefixCounterAdd2);

    UTF_VERIFY_EQUAL2_STRING_MAC(prefixCounterAdd2, prefixCounterGet,
        "get another prefixCounterGet than was set: prefixCounterAdd = %d, prefixCounterGet = %d",
                                 prefixCounterAdd2, prefixCounterGet);

    /*
        6. Call cpssExMxPmIpLpmIpv6UcPrefixesFlush to invalidate changes.
        Expected: GT_OK.
    */
    /* Delete Entry */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /* Delete default virtual router and Lpm Db */
    st = prvUtfDeleteLpmDBAndVirtualRouter();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeleteLpmDBAndVirtualRouter()");

    /*
        7. Analyse parameters with framework functions. Compare with old parameters value
           if they exists. Check parameters delta range and print result.

        Expected: GT_OK.
    */
    utfResultParamArray[paramNum++].paramValue = prefixCounterAdd1;
    utfResultParamArray[paramNum++].paramValue = prefixCounterDelete;
    utfResultParamArray[paramNum++].paramValue = prefixCounterAdd2;
    utfResultParamArray[paramNum++].paramValue = prefixCounterGet;
    utfResultParamArray[paramNum++].paramValue = timeToAdd;
    utfResultParamArray[paramNum++].paramValue = timeToGet;

    cpssOsBzero((GT_VOID*) paramDeltaCompareArr, sizeof(paramDeltaCompareArr));

    st = utfLogResultRun(utfResultParamArray, IP_LPM_MAX_ARRAY_PARAMS_CNS, paramDeltaCompareArr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "utfLogResultRun fail with error %d", st);

    for(paramCounter = 0; paramCounter < paramNum; paramCounter++)
    {
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, paramDeltaCompareArr[paramCounter],
                                     "Delta fail on param \t\"%s\"\n",
                                     utfResultParamArray[paramCounter].paramName);
    }
}

/*--------------------------------------------------------------------------------------------*/
UTF_TEST_CASE_MAC(cpssExMxPmIpLpmIpv6UcPrefixAddDistributedAndExactTime)
{
/*
    1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).

    2. Call cpssExMxPmIpLpmIpv6UcPrefixAdd, generate prefixes.
    Expected: GT_OK.

    3. Call cpssExMxPmIpLpmIpv6UcPrefixesFlush  to invalidate changes.
    Expected: GT_OK.

    4. Call cpssExMxPmIpLpmIpv6UcPrefixAdd to add exact prefixes.
    Expected: GT_OK.

    5. Invalidate changes.
    Expected: GT_OK.

    6. Analyse parameters with framework functions. Compare with old parameters value
       if they exists. Check parameters delta range and print result.

    Expected: GT_OK.
*/
    GT_STATUS   st      = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    GT_IPV6ADDR                             ipAddr;
    GT_U8                                   prefixLen = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC  nextHopPointer;
    GT_BOOL                                 override  = GT_FALSE;

    /* max prefixes that can be added  with GT_OK*/
    GT_U32                  prefixCounterAdd1 = 0;
    GT_U32                  prefixCounterAdd2 = 0;
    GT_U32                  timeToAdd1   = 0;
    GT_U32                  timeToAdd2   = 0;

    GT_U8                   paramCounter = 0;
    GT_U8                   paramNum     = 0;
    GT_BOOL                 paramDeltaCompareArr[IP_LPM_MAX_ARRAY_PARAMS_CNS];
    UTF_RESULT_PARAM_STC    utfResultParamArray[IP_LPM_MAX_ARRAY_PARAMS_CNS] =
                            {{"Added prefixes (BGP)  ", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Added prefixes (exact)", 0, IP_LPM_PERCENT_COUNT_CNS},
                             {"Time to add (BGP)     ", 0, IP_LPM_PERCENT_TIME_BGP_CNS },
                             {"Time to add (exact)   ", 0, IP_LPM_PERCENT_TIME_CNS }};

    /*
        1. Create default LPM configuration (see cpssExMxPmIpLpmIpv4UcPrefixAdd).
    */
    st = prvUtfCreateLpmDBAndVirtualRouterAdd();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfCreateLpmDBAndVirtualRouterAdd");

    /*
        2. Call cpssExMxPmIpLpmIpv6UcPrefixAdd read params from external array.
        Expected: GT_OK.
    */
    nextHopPointer.routeEntryBaseMemAddr = 0;
    nextHopPointer.blockSize             = 1;
    nextHopPointer.routeEntryMethod      = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;

    cpssOsMemSet((GT_VOID*) &(ipAddr), 20, sizeof(ipAddr));

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    utfStartTimer();


    /* Init prefix generator */
    prvUtfInitGenerate();

    while (st == GT_OK)
    {
        /* generate prefix */
        st = prvUtfGeneratePrefix(&ipAddr.arIP[0], &ipAddr.arIP[1], &ipAddr.arIP[2],
                                      &prefixLen);

        /* add the current prefix */
        st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen,
                                            &nextHopPointer, override);
        if (st == GT_OK)
        {
            prefixCounterAdd1++;
        }
    }

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToAdd1);

    /*
        3. Call cpssExMxPmIpLpmIpv6UcPrefixesFlush  to invalidate changes.
        Expected: GT_OK.
    */
    st = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, lpmDbId, vrId);

    /*
        4. Call cpssExMxPmIpLpmIpv6UcPrefixAdd to add exact prefixes.
        Expected: GT_OK.
    */

    cpssOsMemSet((GT_VOID*) &(ipAddr), 20, sizeof(ipAddr));
    prefixLen = 128;

    /* Starting UTF timer to measure time operation. All code between start and
     * stop timer functions will be measured.*/
    st = GT_OK;

    utfStartTimer();

    for (ipAddr.arIP[13] = 0; ipAddr.arIP[13] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[13]++)
        for (ipAddr.arIP[14] = 0; ipAddr.arIP[14] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[14]++)
            for (ipAddr.arIP[15] = 1; ipAddr.arIP[15] < IP_LPM_IP_ADDR_MAX && st == GT_OK; ipAddr.arIP[15]++)
            {
                st = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen,
                                                    &nextHopPointer, override);
                if(st != GT_OK || prefixCounterAdd2 > prefixCounterAdd1)
                {
                    break;
                }
                prefixCounterAdd2++;
            }

    prefixCounterAdd2--;

    /*Stop UTF timer. Time between start and stop saved to time param.*/
    utfStopTimer(&timeToAdd2);

    /*
        5. Call cpssExMxPmIpLpmVirtualRouterDelete to invalidate changes.
        Expected: GT_OK.
    */
    lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    st = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpLpmIpv4UcPrefixesFlush: %d, %d",
                                 lpmDbId, vrId);

    /*
        6. Analyse parameters with framework functions. Compare with old parameters value
        if they exists. Check parameters delta range and print result.
        Expected: GT_OK.
    */
    utfResultParamArray[paramNum++].paramValue = prefixCounterAdd1;
    utfResultParamArray[paramNum++].paramValue = prefixCounterAdd2;
    utfResultParamArray[paramNum++].paramValue = timeToAdd1;
    utfResultParamArray[paramNum++].paramValue = timeToAdd2;

    cpssOsBzero((GT_VOID*) paramDeltaCompareArr, sizeof(paramDeltaCompareArr));

    st = utfLogResultRun(utfResultParamArray, IP_LPM_MAX_ARRAY_PARAMS_CNS, paramDeltaCompareArr);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "utfLogResultRun fail with error %d", st);

    for(paramCounter = 0; paramCounter < paramNum; paramCounter++)
    {
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, paramDeltaCompareArr[paramCounter],
                                     "Delta fail on param \t\"%s\"\n",
                                     utfResultParamArray[paramCounter].paramName);
    }
}

/*******************************************************************************
* prvUtfCreateDefaultLpmDB
*
* DESCRIPTION:
*       This routine creates two LpmDBs for Puma device.
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
static GT_STATUS prvUtfCreateDefaultLpmDB()
{
    GT_STATUS   retVal  = GT_OK;
    GT_U32      lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;

    CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC    lpmDbConfig;


    /* Create first LpmDB */
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;

    prvUtfMemCfgArray.routingSramCfgType = CPSS_EXMXPM_LPM_ALL_EXTERNAL_E;
    prvUtfMemCfgArray.sramsSizeArray[0]  = CPSS_SRAM_SIZE_1MB_E;
    prvUtfMemCfgArray.sramsSizeArray[1]  = CPSS_SRAM_SIZE_2MB_E;
    prvUtfMemCfgArray.sramsSizeArray[2]  = CPSS_SRAM_SIZE_2MB_E;

    prvUtfMemCfgArray.numOfSramsSizes    = 3;

    lpmDbConfig.memCfgArray = &prvUtfMemCfgArray;

    lpmDbConfig.numOfMemCfg         = 1;
    lpmDbConfig.protocolStack       = CPSS_IP_PROTOCOL_IPV4V6_E;
    lpmDbConfig.ipv6MemShare        = 50;
    lpmDbConfig.numOfVirtualRouters = 5;

    retVal = cpssExMxPmIpLpmDbCreate(lpmDBId, &lpmDbConfig);

    if ((GT_OK != retVal) && (GT_ALREADY_EXIST != retVal))
    {
        return retVal;
    }

    /* Create second LpmDB */
    prvUtfMemCfgArray.routingSramCfgType = CPSS_EXMXPM_LPM_ALL_EXTERNAL_E;
    prvUtfMemCfgArray.sramsSizeArray[0]  = CPSS_SRAM_SIZE_512KB_E;
    prvUtfMemCfgArray.sramsSizeArray[1]  = CPSS_SRAM_SIZE_512KB_E;
    prvUtfMemCfgArray.sramsSizeArray[2]  = CPSS_SRAM_SIZE_512KB_E;
    prvUtfMemCfgArray.sramsSizeArray[1]  = CPSS_SRAM_SIZE_512KB_E;
    prvUtfMemCfgArray.sramsSizeArray[2]  = CPSS_SRAM_SIZE_512KB_E;

    prvUtfMemCfgArray.numOfSramsSizes    = 5;

    lpmDbConfig.memCfgArray = &prvUtfMemCfgArray;

    lpmDbConfig.numOfMemCfg         = 1;
    lpmDbConfig.protocolStack       = CPSS_IP_PROTOCOL_IPV4V6_E;
    lpmDbConfig.ipv6MemShare        = 0;
    lpmDbConfig.numOfVirtualRouters = 32;

    retVal = cpssExMxPmIpLpmDbCreate(++lpmDBId, &lpmDbConfig);

    if (GT_ALREADY_EXIST == retVal)
    {
        retVal = GT_OK;
    }

    return retVal;
}

/*******************************************************************************
* prvUtfDeleteDefaultLpmDB
*
* DESCRIPTION:
*       This routine delete default LpmDB for Puma device. (lpmDbId = 0)
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
static GT_STATUS prvUtfDeleteDefaultLpmDB()
{
    GT_STATUS   retVal  = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;

    /* Delete LpmDB with lpmDbId = 1 */
    retVal = cpssExMxPmIpLpmDbDelete(lpmDbId);
    if ((GT_OK != retVal) && (GT_NOT_FOUND != retVal))
    {
        return retVal;
    }

    /* Delete LpmDB with lpmDbId = 2 */
    retVal = cpssExMxPmIpLpmDbDelete(++lpmDbId);
    if (GT_NOT_FOUND == retVal)
    {
        retVal = GT_OK;
    }

    return retVal;
}

/*******************************************************************************
* prvUtfCreateLpmDBAndVirtualRouterAdd
*
* DESCRIPTION:
*       This routine creates LpmDB and Virtual Router for Puma device.
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
static GT_STATUS prvUtfCreateLpmDBAndVirtualRouterAdd()
{
    GT_STATUS   retVal  = GT_OK;
    GT_U32      lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC    vrConfig;


    /* Create LpmDB */
    retVal = prvUtfCreateDefaultLpmDB();
    if (GT_OK != retVal)
    {
        return retVal;
    }

    /* Adding Virtual Router 1, 0 */
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    vrConfig.supportUcIpv4 = GT_TRUE;
    vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr = 0;
    vrConfig.defaultUcIpv4RouteEntry.blockSize = 1;

    vrConfig.supportMcIpv4 = GT_TRUE;
    vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr = 1;
    vrConfig.defaultMcIpv4RouteEntry.blockSize = 1;

    vrConfig.supportUcIpv6 = GT_TRUE;
    vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr = 2;
    vrConfig.defaultUcIpv6RouteEntry.blockSize = 1;

    vrConfig.supportMcIpv6 = GT_TRUE;
    vrConfig.defaultMcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultMcIpv6RouteEntry.routeEntryBaseMemAddr = 3;
    vrConfig.defaultMcIpv6RouteEntry.blockSize = 1;

    vrConfig.defaultMcIpv6RuleIndex = 2047;
    vrConfig.defaultMcIpv6PclId = 0;

    retVal = cpssExMxPmIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfig);

    if ((GT_OK != retVal) && (GT_ALREADY_EXIST != retVal))
    {
        return retVal;
    }

    /* Adding Virtual Router 1, 1 */
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS + 1;

    vrConfig.supportUcIpv4 = GT_TRUE;
    vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr = 4;
    vrConfig.defaultUcIpv4RouteEntry.blockSize = 1;

    vrConfig.supportMcIpv4 = GT_TRUE;
    vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr = 5;
    vrConfig.defaultMcIpv4RouteEntry.blockSize = 1;

    vrConfig.supportUcIpv6 = GT_FALSE;
    vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr = 2;
    vrConfig.defaultUcIpv6RouteEntry.blockSize = 1;

    vrConfig.supportMcIpv6 = GT_FALSE;
    vrConfig.defaultMcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    vrConfig.defaultMcIpv6RouteEntry.routeEntryBaseMemAddr = 3;
    vrConfig.defaultMcIpv6RouteEntry.blockSize = 1;

    vrConfig.defaultMcIpv6RuleIndex = 0;
    vrConfig.defaultMcIpv6PclId = 0;

    retVal = cpssExMxPmIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfig);

    if ((GT_OK != retVal) && (GT_ALREADY_EXIST != retVal))
    {
        return retVal;
    }

    /* Adding Virtual Router 2, 0 */
    lpmDBId = IP_LPM_DEFAULT_DB_ID_CNS + 1;
    vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    vrConfig.supportUcIpv4 = GT_TRUE;
    vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    vrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr = 0;
    vrConfig.defaultUcIpv4RouteEntry.blockSize = 1;

    vrConfig.supportMcIpv4 = GT_TRUE;
    vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    vrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr = 1;
    vrConfig.defaultMcIpv4RouteEntry.blockSize = 1;

    vrConfig.supportUcIpv6 = GT_TRUE;
    vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    vrConfig.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr = 2;
    vrConfig.defaultUcIpv6RouteEntry.blockSize = 1;

    vrConfig.supportMcIpv6 = GT_TRUE;
    vrConfig.defaultMcIpv6RouteEntry.routeEntryMethod = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
    vrConfig.defaultMcIpv6RouteEntry.routeEntryBaseMemAddr = 3;
    vrConfig.defaultMcIpv6RouteEntry.blockSize = 1;

    vrConfig.defaultMcIpv6RuleIndex = 2047;
    vrConfig.defaultMcIpv6PclId = 0;

    retVal = cpssExMxPmIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfig);

    if (GT_ALREADY_EXIST == retVal)
    {
        retVal = GT_OK;
    }

    return retVal;
}

/*******************************************************************************
* prvUtfDeleteLpmDBAndVirtualRouter
*
* DESCRIPTION:
*       This routine delete default LpmDB and Virtual Router for Puma device.
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
static GT_STATUS prvUtfDeleteLpmDBAndVirtualRouter()
{
    GT_STATUS   retVal  = GT_OK;
    GT_U32      lpmDbId = IP_LPM_DEFAULT_DB_ID_CNS;
    GT_U32      vrId    = IP_LPM_DEFAULT_VR_ID_CNS;

    /* Delete Virtual Router with vrId = 0 and lpmDbId = 1 */
    retVal = cpssExMxPmIpLpmVirtualRouterDelete(lpmDbId, vrId);
    if ((GT_OK != retVal) && (GT_NOT_FOUND != retVal))
    {
        return retVal;
    }

    /* Delete Virtual Router with vrId = 1 and lpmDbId = 1 */
    lpmDbId = 1;
    vrId    = 1;

    retVal = cpssExMxPmIpLpmVirtualRouterDelete(lpmDbId, vrId);
    if ((GT_OK != retVal) && (GT_NOT_FOUND != retVal))
    {
        return retVal;
    }

    /* Delete LpmDB with lpmDbId = 1*/
    retVal = cpssExMxPmIpLpmDbDelete(lpmDbId);
    if ((GT_OK != retVal) && (GT_NOT_FOUND != retVal))
    {
        return retVal;
    }

    /* Delete Virtual Router with vrId = 0 and lpmDbId = 2*/
    lpmDbId = 2;
    vrId    = 0;

    retVal = cpssExMxPmIpLpmVirtualRouterDelete(++lpmDbId, vrId);
    if ((GT_OK != retVal) && (GT_NOT_FOUND != retVal))
    {
        return retVal;
    }

    /* Delete LpmDB with lpmDbId = 2 */
    retVal = cpssExMxPmIpLpmDbDelete(lpmDbId);
    if (GT_NOT_FOUND == retVal)
    {
        retVal = GT_OK;
    }

    return retVal;
}

/*******************************************************************************
* prvUtfGeneratePrefix
*
* DESCRIPTION:
*       This routine generate prefix for Puma device.
*
* INPUTS:
*
* OUTPUTS:
*
*       GT_U8    *firstBytePtr    - pointer to first byte
*       GT_U8    *secondBytePtr   - pointer to second byte
*       GT_U8    *thirdBytePtr    - pointer to third byte
*       GT_U8    *prefixLenPtr    - pounter to prefix length
*
* RETURNS:
*
*       GT_OK           - Operation OK
*       GT_BAD_PTR      - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfGeneratePrefix
(
     OUT   GT_U8 *firstBytePtr,
     OUT   GT_U8 *secondBytePtr,
     OUT   GT_U8 *thirdBytePtr,
     OUT   GT_U8 *prefixLenPtr
)
{
    GT_U32      temp    = 0;
    GT_U8       temp1   = 0;

    CPSS_NULL_PTR_CHECK_MAC(firstBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(secondBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(thirdBytePtr);
    CPSS_NULL_PTR_CHECK_MAC(prefixLenPtr);

    /* Iterate IP prefixes from 1.0.0.0 */
    if (count1 < IP_LPM_IP_ADDR_MAX - IP_LPM_MAX_UNIQ_VALUES + 1 &&
        count2 < IP_LPM_IP_ADDR_MAX - IP_LPM_MAX_UNIQ_VALUES + 1)
    {
        count0++;
        count1 = count2 = IP_LPM_IP_ADDR_MAX;
    }

    if(count0 % 2)
    {
        *firstBytePtr = IP_LPM_IPV4_MAX_RANGE / 2 + count0;
    }
    else
    {
        *firstBytePtr = IP_LPM_IPV4_MAX_RANGE / 2 - count0;
    }
    /* Skip non-routable networks */

    /* Private address space (10.0.0.0/8, 172.16.0.0/12 and 192.168.0.0/16) and other
     * seemingly legitimate, but non-routable (via available BGP tables) addresses
     * account for a noticeable fraction of observed IPs. A second set of private
     * networks is the link-local address range codified in RFC 3330 and RFC 3927.
     * The intention behind these RFCs is to provide an IP address
     * (and by implication, network connectivity) without a DHCP server
     * being available and without having to configure a network address manually.
     * The network 169.254/16 has been reserved for this purpose. Within this address
     * range, the networks 169.254.0.0/24 and 169.254.255.255/24 have been
     * set aside for future use.
     * */

    if(*firstBytePtr == 10  || *firstBytePtr == 127 ||
       *firstBytePtr == 169 || *firstBytePtr == 172 ||
       *firstBytePtr == 192 )
    {
        (*firstBytePtr)++;
    }

    /* Iterate second byte of prefix */
    if(count2 < IP_LPM_IP_ADDR_MAX - IP_LPM_MAX_UNIQ_VALUES)
    {
        count1--;
        count2 = IP_LPM_IP_ADDR_MAX;
    }

    if(count1 % 2)
    {
        *secondBytePtr = (count1 << (count1 % 8)) + 1;
    }
    else
    {
        *secondBytePtr = (count1 >> (count1 % 8));
    }
    *secondBytePtr += (count1 % 7);


    /* Iterate third byte*/
    if(count2 % 2)
    {
        *thirdBytePtr = (count2 << (count2 % 8)) + 1;
    }
    else
    {
        *thirdBytePtr = (count2 >> (count2 % 8));
    }
    *thirdBytePtr += (count2 % 7);

    count2--;

    /* Calculate prefix lenght, check last setted bit at the right side */
    temp = (((*firstBytePtr << 8) | *secondBytePtr) << 8) | *thirdBytePtr;

    for(temp1 = 24; temp1 > 0; temp1--)
    {
        if(temp & 01)
        {
            break;
        }
        temp >>= 1;
    }

    *prefixLenPtr = temp1;

    return GT_OK;
}

/*******************************************************************************
* prvUtfInitGenerate
*
* DESCRIPTION:
*       This routine initialize prefix generator.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK              - when initialization complete
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_VOID prvUtfInitGenerate()
{
    count0 = 0;
    count1 = IP_LPM_IP_ADDR_MAX;
    count2 = IP_LPM_IP_ADDR_MAX;
}

/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxPmIpLpm suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmIpLpm)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmDbCreate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmDbMemoryCheck)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmDbDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmDbConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmDbDevListAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmDevListMemoryCheck)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmDbDevListRemove)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmDbDevListGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmVirtualRouterAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmVirtualRouterMemoryCheck)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmVirtualRouterDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmVirtualRouterConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4UcPrefixAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4UcPrefixMemoryCheck)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4UcPrefixBulkAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4UcPrefixDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4UcPrefixBulkDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4UcPrefixesFlush)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4UcPrefixGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4UcPrefixGetNext)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4UcLpmGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4McEntryAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4McEntryMemoryCheck)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4McEntryDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4McEntriesFlush)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4McEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4McEntryGetNext)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6UcPrefixAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6UcPrefixMemoryCheck)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6UcPrefixBulkAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6UcPrefixDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6UcPrefixBulkDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6UcPrefixesFlush)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6UcPrefixGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6UcPrefixGetNext)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6UcLpmGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6McEntryAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6McEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6McEntryMemoryCheck)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6McEntryDel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6McEntriesFlush)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6McEntryGetNext)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6McEntrySearch)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmDbMemSizeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmDbExport)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmDbImport)
    /*Other LPM tests*/
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4UcPrefixAddMax)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4UcPrefixBulkAddMax)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4McEntryAddMax)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6UcPrefixBulkAddMax)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6McEntryAddMax)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6UcPrefixAddMax)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4UcPrefixAddDistributed)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4UcPrefixAddDistributedAndExactTime)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6UcPrefixAddDistributed)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv4UcPrefixAddDeleteAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6UcPrefixAddDeleteAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpLpmIpv6UcPrefixAddDistributedAndExactTime)

UTF_SUIT_END_TESTS_MAC(cpssExMxPmIpLpm)


