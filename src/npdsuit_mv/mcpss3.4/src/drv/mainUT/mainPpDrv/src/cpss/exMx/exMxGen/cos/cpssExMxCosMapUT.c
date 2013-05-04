/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxCosMapUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxCosMap.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/exMx/exMxGen/cos/cpssExMxCosMap.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum                     */
#define COS_MAP_INVALID_ENUM_CNS             0x5AAAAAA5
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosByDscpDpSet
(
    IN  GT_U8                       dev,
    IN  GT_U8                       dscp,
    IN  CPSS_DP_LEVEL_ENT           dpLevel,
    IN  CPSS_COS_MAP_STC            *cosEntryPtr,
    IN  CPSS_COS_TABLE_TYPE_ENT     tableType
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosByDscpDpSet)
{
/*
ITERATE_DEVICES
1.1. Call function with dscp [CPSS_DSCP_RANGE_CNS - 1], 
    dpLevel [CPSS_DP_GREEN], non-NULL cosEntryPtr 
    (with correct values of fields), tableType 
    [CPSS_COS_TABLE_MARKING_E]. Expected: GT_OK.
1.2. Call function cpssExMxCosByDscpDpGet with 
    dscp [CPSS_DSCP_RANGE_CNS - 1], dpLevel [CPSS_DP_GREEN],
    non-NULL cosEntryPtr, tableType [CPSS_COS_TABLE_MARKING_E].
    Expected: GT_OK and cosEntry equal to the one just written.
1.3. Call function with dscp [CPSS_DSCP_RANGE_CNS - 1], 
    dpLevel [CPSS_DP_RED_E], non-NULL cosEntryPtr 
    (with correct values of fields), tableType 
    [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK.
1.4. Call function cpssExMxCosByDscpDpGet with 
    dscp [CPSS_DSCP_RANGE_CNS - 1], dpLevel [CPSS_DP_RED_E], 
    non-NULL cosEntryPtr (with correct values of fields), 
    tableType [CPSS_COS_TABLE_REMARKING_E]. 
    Expected: GT_OK and cosEntry equal to one the just written.
1.5. Call function with out of range dscp [CPSS_DSCP_RANGE_CNS]. 
    Expected: NON GT_OK.
1.6. Call function with out of range dpLevel [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
1.7. Call function with cosEntryPtr [NULL]. 
    Expected: GT_BAD_PTR.
1.8. Call function with out of range tableType [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                       dev;
    GT_U8                       dscp;
    CPSS_DP_LEVEL_ENT           dpLevel;
    CPSS_COS_MAP_STC            cosEntry;
    CPSS_COS_TABLE_TYPE_ENT     tableType;

    CPSS_COS_MAP_STC            retCosEntry;
    GT_BOOL                     failureWas;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with dscp [CPSS_DSCP_RANGE_CNS - 1],  */
        /* dpLevel [CPSS_DP_GREEN_E], non-NULL cosEntryPtr          */
        /* (with correct values of fields), tableType               */
        /* [CPSS_COS_TABLE_MARKING_E]. Expected: GT_OK.             */
        dscp        = CPSS_DSCP_RANGE_CNS - 1;
        dpLevel     = CPSS_DP_GREEN_E;
        tableType   = CPSS_COS_TABLE_MARKING_E;

        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));

        cosEntry.dscp = dscp;
        cosEntry.cosParam.dropPrecedence = dpLevel;

        st = cpssExMxCosByDscpDpSet(dev, dscp, dpLevel, &cosEntry, tableType);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, <cosEntryPtr>, %d",
                                     dev, dscp, dpLevel, tableType);

        /* restore cosEntry in case function has changed it */
        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));

        cosEntry.dscp = dscp;
        cosEntry.cosParam.dropPrecedence = dpLevel;

        /* 1.2. Call function cpssExMxCosByDscpDpGet with               */
        /* dscp [CPSS_DSCP_RANGE_CNS - 1], dpLevel [CPSS_DP_GREEN_E],   */
        /* non-NULL cosEntryPtr, tableType [CPSS_COS_TABLE_MARKING_E].  */
        /* Expected: GT_OK and cosEntry equal to the one just written.  */

        st = cpssExMxCosByDscpDpGet(dev, dscp, dpLevel, tableType, &retCosEntry);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxCosByDscpDpGet: %d, %d, %d, %d",
                                     dev, dscp, dpLevel, tableType);
        if (GT_OK == st)
        {
            /*failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&cosEntry,
                                            (const GT_VOID*)&retCosEntry, sizeof(cosEntry))) ? GT_FALSE : GT_TRUE;*/
            failureWas = GT_FALSE;

            if((retCosEntry.cosParam.dropPrecedence != cosEntry.cosParam.dropPrecedence) ||
               (retCosEntry.cosParam.trafficClass   != cosEntry.cosParam.trafficClass)   ||
               (retCosEntry.cosParam.userPriority   != cosEntry.cosParam.userPriority))
                failureWas = GT_TRUE;

            if((retCosEntry.dscp     != cosEntry.dscp)     ||
               (retCosEntry.expField != cosEntry.expField) ||
               (retCosEntry.userDef  != cosEntry.userDef))
                failureWas = GT_TRUE;

            UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                         "1 get another CoS mapping entry than has been written [dev = %d, dscp = %d, dpLevel = %d, tableType = %d]",
                                         dev, dscp, dpLevel, tableType);
        }

        /* 1.3. Call function with dscp [CPSS_DSCP_RANGE_CNS - 1],  */
        /* dpLevel [CPSS_DP_RED_E], non-NULL cosEntryPtr            */
        /* (with correct values of fields), tableType               */
        /* [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK.           */
        if(dpLevel == CPSS_DP_GREEN_E &&
           PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_TIGER_E)
        {
            dpLevel = CPSS_DP_RED_E;
            tableType = CPSS_COS_TABLE_REMARKING_E;
    
            /* dscp == CPSS_DSCP_RANGE_CNS - 1                      */
            /* cosEntry.dscp == dscp                                */  
            cosEntry.cosParam.dropPrecedence = dpLevel;
    
            st = cpssExMxCosByDscpDpSet(dev, dscp, dpLevel, &cosEntry, tableType);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, <cosEntryPtr>, %d",
                                         dev, dscp, dpLevel, tableType);
    
            /* restore cosEntry in case function has changed it */
            cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
    
            cosEntry.dscp = dscp;
            cosEntry.cosParam.dropPrecedence = dpLevel;
    
            /* 1.4. Call function cpssExMxCosByDscpDpGet with               */
            /* dscp [CPSS_DSCP_RANGE_CNS - 1], dpLevel [CPSS_DP_RED_E],     */    
            /* non-NULL cosEntryPtr (with correct values of fields),        */
            /* tableType [CPSS_COS_TABLE_REMARKING_E].                      */
            /* Expected: GT_OK and cosEntry equal to one the just written.  */
    
            st = cpssExMxCosByDscpDpGet(dev, dscp, dpLevel, tableType, &retCosEntry);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxCosByDscpDpGet: %d, %d, %d, %d",
                                         dev, dscp, dpLevel, tableType);
            if (GT_OK == st)
            {
                /*failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&cosEntry,
                                                (const GT_VOID*)&retCosEntry, sizeof(cosEntry))) ? GT_FALSE : GT_TRUE;*/
                failureWas = GT_FALSE;

                if((retCosEntry.cosParam.dropPrecedence != cosEntry.cosParam.dropPrecedence) ||
                   (retCosEntry.cosParam.trafficClass   != cosEntry.cosParam.trafficClass)   ||
                   (retCosEntry.cosParam.userPriority   != cosEntry.cosParam.userPriority))
                    failureWas = GT_TRUE;

                if((retCosEntry.dscp     != cosEntry.dscp)     ||
                   (retCosEntry.expField != cosEntry.expField) ||
                   (retCosEntry.userDef  != cosEntry.userDef))
                    failureWas = GT_TRUE;

                UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                             "2 get another CoS mapping entry than has been written [dev = %d, dscp = %d, dpLevel = %d, tableType = %d]",
                                             dev, dscp, dpLevel, tableType);
            }
        }

        /* 1.5. Call function with out of range dscp [CPSS_DSCP_RANGE_CNS]. */
        /* Expected: NON GT_OK.                                             */

        /* dpLevel == CPSS_DP_GREEN_E                   */
        /* tableType == CPSS_COS_TABLE_REMARKING_E      */
        /* cosEntry.dscp == CPSS_DSCP_RANGE_CNS - 1     */  
        /* cosEntry.cosParam.dropPrecedence == dpLevel  */
        dscp = CPSS_DSCP_RANGE_CNS;
        dpLevel = CPSS_DP_GREEN_E;

        st = cpssExMxCosByDscpDpSet(dev, dscp, dpLevel, &cosEntry, tableType);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, <cosEntryPtr>, %d",
                                         dev, dscp, dpLevel, tableType);

        /* restore cosEntry in case function has changed it and dscp*/
        dscp = 0;

        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
        cosEntry.cosParam.dropPrecedence = dpLevel;

        /* 1.6. Call function with out of range dpLevel [0x5AAAAAA5].   */
        /* Expected: GT_BAD_PARAM.                                      */

        dpLevel = COS_MAP_INVALID_ENUM_CNS;
        /* dscp == 0                                         */
        /* tableType == CPSS_COS_TABLE_REMARKING_E           */
        /* cosEntry.dscp == dscp                             */  
        /* cosEntry.cosParam.dropPrecedence == CPSS_DP_RED_E */

        st = cpssExMxCosByDscpDpSet(dev, dscp, dpLevel, &cosEntry, tableType);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, %d, <cosEntryPtr>, %d",
                                     dev, dscp, dpLevel, tableType);

        /* restore cosEntry in case function has changed it and dpLevel*/
        dpLevel = CPSS_DP_RED_E;

        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
        cosEntry.cosParam.dropPrecedence = dpLevel;

        /* 1.7. Call function with cosEntryPtr [NULL].  */
        /* Expected: GT_BAD_PTR.                        */

        st = cpssExMxCosByDscpDpSet(dev, dscp, dpLevel, NULL, tableType);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL, %d",
                                     dev, dscp, dpLevel, tableType);

        /* 1.8. Call function with out of range tableType [0x5AAAAAA5]. */
        /* Expected: GT_BAD_PARAM.                                      */

        tableType = COS_MAP_INVALID_ENUM_CNS;
        /* dscp == CPSS_DSCP_RANGE_CNS -1                    */
        /* dpLevel == CPSS_DP_RED_E                          */
        /* cosEntry.dscp == dscp                             */  
        /* cosEntry.cosParam.dropPrecedence == dpLevel       */

        st = cpssExMxCosByDscpDpSet(dev, dscp, dpLevel, &cosEntry, tableType);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, %d, <cosEntryPtr>, %d",
                                     dev, dscp, dpLevel, tableType);
    }

    dscp        = 0;
    dpLevel     = CPSS_DP_GREEN_E;
    tableType   = CPSS_COS_TABLE_MARKING_E;

    cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
    cosEntry.cosParam.dropPrecedence = dpLevel;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosByDscpDpSet(dev, dscp, dpLevel, &cosEntry, tableType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);

        /* restore cosEntry in case function has changed it */
        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
        cosEntry.cosParam.dropPrecedence = dpLevel;
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosByDscpDpSet(dev, dscp, dpLevel, &cosEntry, tableType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosByDscpDpGet
(
    IN  GT_U8                       dev,
    IN  GT_U8                       dscp,
    IN  CPSS_DP_LEVEL_ENT           dpLevel,
    IN  CPSS_COS_TABLE_TYPE_ENT     tableType,
    OUT CPSS_COS_MAP_STC            *cosEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosByDscpDpGet)
{
/*
ITERATE_DEVICES
1.1. Call function with dscp [CPSS_DSCP_RANGE_CNS - 1], 
    dpLevel [CPSS_DP_GREEN], non-NULL cosEntryPtr, 
    tableType [CPSS_COS_TABLE_MARKING_E]. Expected: GT_OK.
1.2. Call function with dscp [CPSS_DSCP_RANGE_CNS - 1], 
    dpLevel [CPSS_DP_RED_E], non-NULL cosEntryPtr, 
    tableType [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK.
1.3. Call function with out of range dscp [CPSS_DSCP_RANGE_CNS]. 
    Expected: NON GT_OK.
1.4. Call function with out of range dpLevel [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
1.5. Call function with cosEntryPtr [NULL]. 
    Expected: GT_BAD_PTR.
1.6. Call function with out of range tableType [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                       dev;
    GT_U8                       dscp;
    CPSS_DP_LEVEL_ENT           dpLevel;
    CPSS_COS_TABLE_TYPE_ENT     tableType;
    CPSS_COS_MAP_STC            cosEntry;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with dscp [CPSS_DSCP_RANGE_CNS - 1],  */
        /* dpLevel [CPSS_DP_GREEN], non-NULL cosEntryPtr,           */
        /*tableType [CPSS_COS_TABLE_MARKING_E]. Expected: GT_OK.    */
        dscp        = CPSS_DSCP_RANGE_CNS - 1;
        dpLevel     = CPSS_DP_GREEN_E;
        tableType   = CPSS_COS_TABLE_MARKING_E;

        st = cpssExMxCosByDscpDpGet(dev, dscp, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, dscp, dpLevel, tableType);

        /* 1.2. Call function with dscp [CPSS_DSCP_RANGE_CNS - 1],  */
        /* dpLevel [CPSS_DP_RED_E], non-NULL cosEntryPtr,           */
        /* tableType [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK. */
        dpLevel = CPSS_DP_RED_E;
        tableType = CPSS_COS_TABLE_REMARKING_E;
        /* dscp == CPSS_DSCP_RANGE_CNS - 1 */

        st = cpssExMxCosByDscpDpGet(dev, dscp, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, dscp, dpLevel, tableType);

        /* 1.3. Call function with out of range dscp [CPSS_DSCP_RANGE_CNS]. */
        /* Expected: NON GT_OK.                                             */
        dscp = CPSS_DSCP_RANGE_CNS;
        /* dpLevel == CPSS_DP_RED_E                 */
        /* tableType == CPSS_COS_TABLE_REMARKING_E  */

        st = cpssExMxCosByDscpDpGet(dev, dscp, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, dscp, dpLevel, tableType);

        /* restore dscp*/
        dscp = 0;

        /* 1.6. Call function with out of range dpLevel [0x5AAAAAA5].   */
        /* Expected: GT_BAD_PARAM.                                      */
        dpLevel = COS_MAP_INVALID_ENUM_CNS;
        /* dscp == 0                                */
        /* tableType == CPSS_COS_TABLE_REMARKING_E  */

        st = cpssExMxCosByDscpDpGet(dev, dscp, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, dscp, dpLevel, tableType);

        /* restore dpLevel*/
        dpLevel = CPSS_DP_RED_E;

        /* 1.7. Call function with cosEntryPtr [NULL].  */
        /* Expected: GT_BAD_PTR.                        */
        st = cpssExMxCosByDscpDpGet(dev, dscp, dpLevel, tableType, NULL);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, %d, NULL",
                                     dev, dscp, dpLevel, tableType);

        /* 1.8. Call function with out of range tableType [0x5AAAAAA5]. */
        /* Expected: GT_BAD_PARAM.                                      */
        tableType = COS_MAP_INVALID_ENUM_CNS;
        /* dscp == 0                        */
        /* dpLevel == CPSS_DP_RED_E         */

        st = cpssExMxCosByDscpDpGet(dev, dscp, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, dscp, dpLevel, tableType);
    }

    dscp        = 0;
    dpLevel     = CPSS_DP_GREEN_E;
    tableType   = CPSS_COS_TABLE_MARKING_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosByDscpDpGet(dev, dscp, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosByDscpDpGet(dev, dscp, dpLevel, tableType, &cosEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosByExpDpSet
(
    IN  GT_U8                       dev,
    IN  GT_U8                       exp,
    IN  CPSS_DP_LEVEL_ENT           dpLevel,
    IN  CPSS_COS_MAP_STC            *cosEntryPtr,
    IN  CPSS_COS_TABLE_TYPE_ENT     tableType
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosByExpDpSet)
{
/*
ITERATE_DEVICES
1.1. Call function with exp [CPSS_EXP_RANGE_CNS - 1], 
    dpLevel [CPSS_DP_GREEN], non-NULL cosEntryPtr 
    (with correct values of fields), tableType 
    [CPSS_COS_TABLE_MARKING_E]. Expected: GT_OK.
1.2. Call function cpssExMxCosByExpDpGet with 
    exp [CPSS_EXP_RANGE_CNS - 1], dpLevel [CPSS_DP_GREEN],
    non-NULL cosEntryPtr, tableType [CPSS_COS_TABLE_MARKING_E].
    Expected: GT_OK and cosEntry equal to the one just written.
1.3. Call function with exp [CPSS_EXP_RANGE_CNS - 1], 
    dpLevel [CPSS_DP_RED_E], non-NULL cosEntryPtr 
    (with correct values of fields), tableType 
    [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK.
1.4. Call function cpssExMxCosByExpDpGet with 
    exp [CPSS_EXP_RANGE_CNS - 1], dpLevel [CPSS_DP_RED_E], 
    non-NULL cosEntryPtr (with correct values of fields), 
    tableType [CPSS_COS_TABLE_REMARKING_E]. 
    Expected: GT_OK and cosEntry equal to one the just written.
1.5. Call function with out of range exp [CPSS_EXP_RANGE_CNS]. 
    Expected: NON GT_OK.
1.6. Call function with out of range dpLevel [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
1.7. Call function with cosEntryPtr [NULL]. 
    Expected: GT_BAD_PTR.
1.8. Call function with out of range tableType [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                       dev;
    GT_U8                       exp;
    CPSS_DP_LEVEL_ENT           dpLevel;
    CPSS_COS_MAP_STC            cosEntry;
    CPSS_COS_TABLE_TYPE_ENT     tableType;

    CPSS_COS_MAP_STC            retCosEntry;
    GT_BOOL                     failureWas;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with exp [CPSS_EXP_RANGE_CNS - 1],  */
        /* dpLevel [CPSS_DP_GREEN_E], non-NULL cosEntryPtr          */
        /* (with correct values of fields), tableType               */
        /* [CPSS_COS_TABLE_MARKING_E]. Expected: GT_OK.             */
        exp         = CPSS_EXP_RANGE_CNS - 1;
        dpLevel     = CPSS_DP_GREEN_E;
        tableType   = CPSS_COS_TABLE_MARKING_E;

        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));

        cosEntry.expField = exp;
        cosEntry.cosParam.dropPrecedence = dpLevel;

        st = cpssExMxCosByExpDpSet(dev, exp, dpLevel, &cosEntry, tableType);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, <cosEntryPtr>, %d",
                                     dev, exp, dpLevel, tableType);

        /* restore cosEntry in case function has changed it */
        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));

        cosEntry.expField = exp;
        cosEntry.cosParam.dropPrecedence = dpLevel;

        /* 1.2. Call function cpssExMxCosByExpDpGet with                */
        /* exp [CPSS_EXP_RANGE_CNS - 1], dpLevel [CPSS_DP_GREEN_E],     */
        /* non-NULL cosEntryPtr, tableType [CPSS_COS_TABLE_MARKING_E].  */
        /* Expected: GT_OK and cosEntry equal to the one just written.  */

        st = cpssExMxCosByExpDpGet(dev, exp, dpLevel, tableType, &retCosEntry);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxCosByExpDpGet: %d, %d, %d, %d",
                                     dev, exp, dpLevel, tableType);
        if (GT_OK == st)
        {
            /*failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&cosEntry,
                                            (const GT_VOID*)&retCosEntry, sizeof(cosEntry))) ? GT_FALSE : GT_TRUE;*/
            failureWas = GT_FALSE;

            if((retCosEntry.cosParam.dropPrecedence != cosEntry.cosParam.dropPrecedence) ||
               (retCosEntry.cosParam.trafficClass   != cosEntry.cosParam.trafficClass)   ||
               (retCosEntry.cosParam.userPriority   != cosEntry.cosParam.userPriority))
                failureWas = GT_TRUE;

            if((retCosEntry.dscp     != cosEntry.dscp)     ||
               (retCosEntry.expField != cosEntry.expField) ||
               (retCosEntry.userDef  != cosEntry.userDef))
                failureWas = GT_TRUE;

            UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                         "3 get another CoS mapping entry than has been written [dev = %d, exp = %d, dpLevel = %d, tableType = %d]",
                                         dev, exp, dpLevel, tableType);
        }

        /* 1.3. Call function with exp [CPSS_EXP_RANGE_CNS - 1],    */
        /* dpLevel [CPSS_DP_RED_E], non-NULL cosEntryPtr            */
        /* (with correct values of fields), tableType               */
        /* [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK.           */
        dpLevel = CPSS_DP_RED_E;
        tableType = CPSS_COS_TABLE_REMARKING_E;
        /* exp == CPSS_EXP_RANGE_CNS - 1                      */
        /* cosEntry.expField == exp                           */  
        cosEntry.cosParam.dropPrecedence = dpLevel;

        st = cpssExMxCosByExpDpSet(dev, exp, dpLevel, &cosEntry, tableType);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, <cosEntryPtr>, %d",
                                     dev, exp, dpLevel, tableType);

        /* restore cosEntry in case function has changed it */
        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
        cosEntry.expField = exp;
        cosEntry.cosParam.dropPrecedence = dpLevel;

        /* 1.4. Call function cpssExMxCosByExpDpGet with                */
        /* exp [CPSS_EXP_RANGE_CNS - 1], dpLevel [CPSS_DP_RED_E],       */    
        /* non-NULL cosEntryPtr (with correct values of fields),        */
        /* tableType [CPSS_COS_TABLE_REMARKING_E].                      */
        /* Expected: GT_OK and cosEntry equal to one the just written.  */

        st = cpssExMxCosByExpDpGet(dev, exp, dpLevel, tableType, &retCosEntry);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxCosByExpDpGet: %d, %d, %d, %d",
                                     dev, exp, dpLevel, tableType);
        if (GT_OK == st)
        {
            /*failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&cosEntry,
                                            (const GT_VOID*)&retCosEntry, sizeof(cosEntry))) ? GT_FALSE : GT_TRUE;*/
            failureWas = GT_FALSE;

            if((retCosEntry.cosParam.dropPrecedence != cosEntry.cosParam.dropPrecedence) ||
               (retCosEntry.cosParam.trafficClass   != cosEntry.cosParam.trafficClass)   ||
               (retCosEntry.cosParam.userPriority   != cosEntry.cosParam.userPriority))
                failureWas = GT_TRUE;

            if((retCosEntry.dscp     != cosEntry.dscp)     ||
               (retCosEntry.expField != cosEntry.expField) ||
               (retCosEntry.userDef  != cosEntry.userDef))
                failureWas = GT_TRUE;

            UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                         "4 get another CoS mapping entry than has been written [dev = %d, exp = %d, dpLevel = %d, tableType = %d]",
                                         dev, exp, dpLevel, tableType);
        }

        /* 1.5. Call function with out of range exp [CPSS_EXP_RANGE_CNS]. */
        /* Expected: NON GT_OK.                                             */

        exp = CPSS_EXP_RANGE_CNS;
        /* dpLevel == CPSS_DP_RED_E                     */
        /* tableType == CPSS_COS_TABLE_REMARKING_E      */
        /* cosEntry.expField == CPSS_EXP_RANGE_CNS - 1  */  
        /* cosEntry.cosParam.dropPrecedence == dpLevel  */

        st = cpssExMxCosByExpDpSet(dev, exp, dpLevel, &cosEntry, tableType);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, <cosEntryPtr>, %d",
                                         dev, exp, dpLevel, tableType);

        /* restore cosEntry in case function has changed it and exp*/
        exp = 0;

        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
        cosEntry.cosParam.dropPrecedence = dpLevel;

        /* 1.6. Call function with out of range dpLevel [0x5AAAAAA5].   */
        /* Expected: GT_BAD_PARAM.                                      */

        dpLevel = COS_MAP_INVALID_ENUM_CNS;
        /* exp == 0                                          */
        /* tableType == CPSS_COS_TABLE_REMARKING_E           */
        /* cosEntry.expField == exp                          */  
        /* cosEntry.cosParam.dropPrecedence == CPSS_DP_RED_E */

        st = cpssExMxCosByExpDpSet(dev, exp, dpLevel, &cosEntry, tableType);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, %d, <cosEntryPtr>, %d",
                                     dev, exp, dpLevel, tableType);

        /* restore cosEntry in case function has changed it and dpLevel*/
        dpLevel = CPSS_DP_RED_E;

        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
        cosEntry.cosParam.dropPrecedence = dpLevel;

        /* 1.7. Call function with cosEntryPtr [NULL].  */
        /* Expected: GT_BAD_PTR.                        */

        st = cpssExMxCosByExpDpSet(dev, exp, dpLevel, NULL, tableType);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL, %d",
                                     dev, exp, dpLevel, tableType);

        /* 1.8. Call function with out of range tableType [0x5AAAAAA5]. */
        /* Expected: GT_BAD_PARAM.                                      */

        tableType = COS_MAP_INVALID_ENUM_CNS;
        /* exp == 0                                          */
        /* dpLevel == CPSS_DP_RED_E                          */
        /* cosEntry.expField == exp                          */  
        /* cosEntry.cosParam.dropPrecedence == dpLevel       */

        st = cpssExMxCosByExpDpSet(dev, exp, dpLevel, &cosEntry, tableType);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, %d, <cosEntryPtr>, %d",
                                     dev, exp, dpLevel, tableType);
    }

    exp         = 0;
    dpLevel     = CPSS_DP_GREEN_E;
    tableType   = CPSS_COS_TABLE_MARKING_E;

    cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
    cosEntry.cosParam.dropPrecedence = dpLevel;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosByExpDpSet(dev, exp, dpLevel, &cosEntry, tableType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);

        /* restore cosEntry in case function has changed it */
        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
        cosEntry.cosParam.dropPrecedence = dpLevel;
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosByExpDpSet(dev, exp, dpLevel, &cosEntry, tableType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosByExpDpGet
(
    IN  GT_U8                   dev,
    IN  GT_U8                   expField,
    IN  CPSS_DP_LEVEL_ENT       dpLevel,
    IN  CPSS_COS_TABLE_TYPE_ENT tableType,
    OUT CPSS_COS_MAP_STC        *cosEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosByExpDpGet)
{
/*
ITERATE_DEVICES
1.1. Call function with expField [CPSS_EXP_RANGE_CNS - 1], 
    dpLevel [CPSS_DP_GREEN], non-NULL cosEntryPtr, 
    tableType [CPSS_COS_TABLE_MARKING_E]. Expected: GT_OK.
1.2. Call function with expField [CPSS_EXP_RANGE_CNS - 1], 
    dpLevel [CPSS_DP_RED_E], non-NULL cosEntryPtr, 
    tableType [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK.
1.3. Call function with out of range expField [CPSS_EXP_RANGE_CNS]. 
    Expected: NON GT_OK.
1.4. Call function with out of range dpLevel [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
1.5. Call function with cosEntryPtr [NULL]. 
    Expected: GT_BAD_PTR.
1.6. Call function with out of range tableType [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                       dev;
    GT_U8                       expField;
    CPSS_DP_LEVEL_ENT           dpLevel;
    CPSS_COS_TABLE_TYPE_ENT     tableType;
    CPSS_COS_MAP_STC            cosEntry;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with expField [CPSS_EXP_RANGE_CNS - 1],  */
        /* dpLevel [CPSS_DP_GREEN], non-NULL cosEntryPtr,           */
        /*tableType [CPSS_COS_TABLE_MARKING_E]. Expected: GT_OK.    */
        expField    = CPSS_EXP_RANGE_CNS - 1;
        dpLevel     = CPSS_DP_GREEN_E;
        tableType   = CPSS_COS_TABLE_MARKING_E;

        st = cpssExMxCosByExpDpGet(dev, expField, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, expField, dpLevel, tableType);

        /* 1.2. Call function with expField [CPSS_EXP_RANGE_CNS - 1],  */
        /* dpLevel [CPSS_DP_RED_E], non-NULL cosEntryPtr,           */
        /* tableType [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK. */
        dpLevel = CPSS_DP_RED_E;
        tableType = CPSS_COS_TABLE_REMARKING_E;
        /* expField == CPSS_EXP_RANGE_CNS - 1 */

        st = cpssExMxCosByExpDpGet(dev, expField, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, expField, dpLevel, tableType);

        /* 1.3. Call function with out of range expField [CPSS_EXP_RANGE_CNS]. */
        /* Expected: NON GT_OK.                                             */
        expField = CPSS_EXP_RANGE_CNS;
        /* dpLevel == CPSS_DP_RED_E                 */
        /* tableType == CPSS_COS_TABLE_REMARKING_E  */

        st = cpssExMxCosByExpDpGet(dev, expField, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, expField, dpLevel, tableType);

        /* restore expField*/
        expField = 0;

        /* 1.6. Call function with out of range dpLevel [0x5AAAAAA5].   */
        /* Expected: GT_BAD_PARAM.                                      */
        dpLevel = COS_MAP_INVALID_ENUM_CNS;
        /* expField == 0                            */
        /* tableType == CPSS_COS_TABLE_REMARKING_E  */

        st = cpssExMxCosByExpDpGet(dev, expField, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, expField, dpLevel, tableType);

        /* restore dpLevel*/
        dpLevel = CPSS_DP_RED_E;

        /* 1.7. Call function with cosEntryPtr [NULL].  */
        /* Expected: GT_BAD_PTR.                        */
        st = cpssExMxCosByExpDpGet(dev, expField, dpLevel, tableType, NULL);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, %d, NULL",
                                     dev, expField, dpLevel, tableType);

        /* 1.8. Call function with out of range tableType [0x5AAAAAA5]. */
        /* Expected: GT_BAD_PARAM.                                      */
        tableType = COS_MAP_INVALID_ENUM_CNS;
        /* expField == 0                    */
        /* dpLevel == CPSS_DP_RED_E         */

        st = cpssExMxCosByExpDpGet(dev, expField, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, expField, dpLevel, tableType);
    }

    expField    = 0;
    dpLevel     = CPSS_DP_GREEN_E;
    tableType   = CPSS_COS_TABLE_MARKING_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosByExpDpGet(dev, expField, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosByExpDpGet(dev, expField, dpLevel, tableType, &cosEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosByTcDpSet
(
    IN  GT_U8                       dev,
    IN  GT_U8                       trfClass,
    IN  CPSS_DP_LEVEL_ENT           dpLevel,
    IN  CPSS_COS_TABLE_TYPE_ENT     tableType,
    IN  CPSS_COS_MAP_STC            *cosEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosByTcDpSet)
{
/*
ITERATE_DEVICES
1.1. Call function with trfClass [CPSS_TC_RANGE_CNS - 1], 
    dpLevel [CPSS_DP_GREEN_E], non-NULL cosEntryPtr 
    (with correct values of fields), tableType 
    [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK.
1.2. Call function cpssExMxCosByTcDpGet with 
    trfClass [CPSS_TC_RANGE_CNS - 1], dpLevel [CPSS_DP_GREEN],
    non-NULL cosEntryPtr, tableType 
    [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK and cosEntry 
    equal to the one just written.
1.3. Call function with trfClass [CPSS_TC_RANGE_CNS - 1], 
    dpLevel [CPSS_DP_RED_E], non-NULL cosEntryPtr 
    (with correct values of fields), tableType 
    [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK.
1.4. Call function cpssExMxCosByTcDpGet with 
    trfClass [CPSS_TC_RANGE_CNS - 1], dpLevel [CPSS_DP_RED_E], 
    non-NULL cosEntryPtr, tableType [CPSS_COS_TABLE_REMARKING_E]. 
    Expected: GT_OK and cosEntry equal to the one just written.
1.5. Call function with trfClass [0], dpLevel [CPSS_DP_GREEN_E],
    non-NULL cosEntryPtr (with correct values of fields), 
    tableType [CPSS_COS_TABLE_MARKING_E]. Expected: GT_OK for devices 
    from Twist-C family and NON GT_OK for other devices.
1.6. Call function cpssExMxCosByTcDpGet with trfClass [CPSS_TC_RANGE_CNS - 1],
    dpLevel [CPSS_DP_GREEN_E], non-NULL cosEntryPtr, tableType [CPSS_COS_TABLE_MARKING_E]. 
    Expected: GT_OK and cosEntry equal to the one just written for devices 
    from Twist-C family and NON GT_OK for other devices.
1.7. Call function with out of range trfClass [CPSS_TC_RANGE_CNS]. 
    Expected: GT_BAD_PARAM.
1.8. Call function with out of range dpLevel [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
1.9. Call function with cosEntryPtr [NULL]. 
    Expected: GT_BAD_PTR.
1.10. Call function with out of range tableType [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                       dev;
    GT_U8                       trfClass;
    CPSS_DP_LEVEL_ENT           dpLevel;
    CPSS_COS_TABLE_TYPE_ENT     tableType;
    CPSS_COS_MAP_STC            cosEntry;

    CPSS_COS_MAP_STC            retCosEntry;
    GT_BOOL                     failureWas;
    CPSS_PP_FAMILY_TYPE_ENT     devFamily;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet");

        /* 1.1. Call function with trfClass [CPSS_TC_RANGE_CNS - 1],    */
        /* dpLevel [CPSS_DP_GREEN_E], non-NULL cosEntryPtr              */
        /* (with correct values of fields), tableType                   */
        /* [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK.               */
        trfClass    = CPSS_TC_RANGE_CNS - 1;
        dpLevel     = CPSS_DP_GREEN_E;
        tableType   = CPSS_COS_TABLE_REMARKING_E;

        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));

        cosEntry.cosParam.trafficClass = trfClass;
        cosEntry.cosParam.dropPrecedence = dpLevel;

        st = cpssExMxCosByTcDpSet(dev, trfClass, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trfClass, dpLevel, tableType);

        /* restore cosEntry in case function has changed it */
        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));

        cosEntry.cosParam.trafficClass = trfClass;
        cosEntry.cosParam.dropPrecedence = dpLevel;

        /* 1.2. Call function cpssExMxCosByTcDpGet with                 */
        /* trfClass [CPSS_TC_RANGE_CNS - 1], dpLevel [CPSS_DP_GREEN],   */
        /* non-NULL cosEntryPtr, tableType                              */
        /* [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK and cosEntry   */
        /* equal to the one just written.                               */

        st = cpssExMxCosByTcDpGet(dev, trfClass, dpLevel, tableType, &retCosEntry);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxCosByTcDpGet: %d, %d, %d, %d",
                                     dev, trfClass, dpLevel, tableType);
        if (GT_OK == st)
        {
            /*failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&cosEntry,
                                            (const GT_VOID*)&retCosEntry, sizeof(cosEntry))) ? GT_FALSE : GT_TRUE;*/
            failureWas = GT_FALSE;

            if((retCosEntry.cosParam.dropPrecedence != cosEntry.cosParam.dropPrecedence) ||
               (retCosEntry.cosParam.trafficClass   != cosEntry.cosParam.trafficClass)   ||
               (retCosEntry.cosParam.userPriority   != cosEntry.cosParam.userPriority))
                failureWas = GT_TRUE;

            if((retCosEntry.dscp     != cosEntry.dscp)     ||
               (retCosEntry.expField != cosEntry.expField) ||
               (retCosEntry.userDef  != cosEntry.userDef))
                failureWas = GT_TRUE;

            UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                         "5 get another CoS mapping entry than has been written [dev = %d, trfClass = %d, dpLevel = %d, tableType = %d]",
                                         dev, trfClass, dpLevel, tableType);
        }

        /* 1.3. Call function with trfClass [CPSS_TC_RANGE_CNS - 1],    */
        /* dpLevel [CPSS_DP_RED_E], non-NULL cosEntryPtr                */
        /* (with correct values of fields), tableType                   */
        /* [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK.               */
        dpLevel = CPSS_DP_RED_E;
        /* tableType == CPSS_COS_TABLE_REMARKING_E     */
        /* trfClass == CPSS_TC_RANGE_CNS - 1           */
        /* cosEntry.cosParam.trafficClass == trfClass  */  
        cosEntry.cosParam.dropPrecedence = dpLevel;

        st = cpssExMxCosByTcDpSet(dev, trfClass, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trfClass, dpLevel, tableType);

        /* restore cosEntry in case function has changed it */
        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));

        cosEntry.cosParam.trafficClass = trfClass;
        cosEntry.cosParam.dropPrecedence = dpLevel;

        /* 1.4. Call function cpssExMxCosByTcDpGet with                 */
        /* trfClass [CPSS_TC_RANGE_CNS - 1], dpLevel [CPSS_DP_RED_E],   */
        /* non-NULL cosEntryPtr, tableType [CPSS_COS_TABLE_REMARKING_E].*/ 
        /* Expected: GT_OK and cosEntry equal to the one just written.  */

        st = cpssExMxCosByTcDpGet(dev, trfClass, dpLevel, tableType, &retCosEntry);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxCosByTcDpGet: %d, %d, %d, %d",
                                     dev, trfClass, dpLevel, tableType);
        if (GT_OK == st)
        {
            /*failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&cosEntry,
                                            (const GT_VOID*)&retCosEntry, sizeof(cosEntry))) ? GT_FALSE : GT_TRUE;*/

            failureWas = GT_FALSE;

            if((retCosEntry.cosParam.dropPrecedence != cosEntry.cosParam.dropPrecedence) ||
               (retCosEntry.cosParam.trafficClass   != cosEntry.cosParam.trafficClass)   ||
               (retCosEntry.cosParam.userPriority   != cosEntry.cosParam.userPriority))
                failureWas = GT_TRUE;

            if((retCosEntry.dscp     != cosEntry.dscp)     ||
               (retCosEntry.expField != cosEntry.expField) ||
               (retCosEntry.userDef  != cosEntry.userDef))
                failureWas = GT_TRUE;

            UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                         "6 get another CoS mapping entry than has been written [dev = %d, trfClass = %d, dpLevel = %d, tableType = %d]",
                                         dev, trfClass, dpLevel, tableType);
        }

        /* 1.5. Call function with trfClass [0], dpLevel [CPSS_DP_GREEN_E],  */
        /* non-NULL cosEntryPtr (with correct values of fields),             */
        /* tableType [CPSS_COS_TABLE_MARKING_E]. Expected: GT_OK for devices */
        /* from Twist-C family and NON GT_OK for other devices.              */
        trfClass    = 0;
        dpLevel     = CPSS_DP_GREEN_E;
        tableType   = CPSS_COS_TABLE_MARKING_E;

        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
        cosEntry.cosParam.dropPrecedence = dpLevel;

        st = cpssExMxCosByTcDpSet(dev, trfClass, dpLevel, tableType, &cosEntry);
        if (CPSS_PP_FAMILY_TWISTC_E == devFamily)
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "for Twist-C device: %d, %d, %d, %d",
                                         dev, trfClass, dpLevel, tableType);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "for non Twist-C device: %d, %d, %d, %d",
                                             dev, trfClass, dpLevel, tableType);
        }

        /* restore cosEntry in case function has changed it */
        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
        cosEntry.cosParam.dropPrecedence = dpLevel;

        /* 1.6. Call function cpssExMxCosByTcDpGet with trfClass            */
        /* [0], dpLevel [CPSS_DP_GREEN_E],                                  */
        /* non-NULL cosEntryPtr, tableType [CPSS_COS_TABLE_MARKING_E].      */
        /* Expected: GT_OK and cosEntry equal to the one just written       */
        /* for devices from Twist-C family and NON GT_OK for other devices. */

        st = cpssExMxCosByTcDpGet(dev, trfClass, dpLevel, tableType, &retCosEntry);
        if (CPSS_PP_FAMILY_TWISTC_E == devFamily)
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxCosByTcDpGet: for Twist-C device: %d, %d, %d, %d",
                                         dev, trfClass, dpLevel, tableType);
            if (GT_OK == st)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&cosEntry,
                                                (const GT_VOID*)&retCosEntry, sizeof(cosEntry))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                             "for Twist-C device: get another CoS mapping entry than has been written [dev = %d, trfClass = %d, dpLevel = %d, tableType = %d]",
                                             dev, trfClass, dpLevel, tableType);
            }
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxCosByTcDpGet: for non Twist-C device: %d, %d, %d, %d",
                                             dev, trfClass, dpLevel, tableType);
        }

        tableType = CPSS_COS_TABLE_REMARKING_E;

        /* 1.7. Call function with out of range trfClass [CPSS_TC_RANGE_CNS]. */
        /* Expected: GT_BAD_PARAM.                                            */
        trfClass = CPSS_TC_RANGE_CNS;
        /* dpLevel == CPSS_DP_GREEN_E                   */
        /* cosEntry.cosParam.trafficClass == 0          */  
        /* cosEntry.cosParam.dropPrecedence == dpLevel  */

        st = cpssExMxCosByTcDpSet(dev, trfClass, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, trfClass, dpLevel, tableType);

        /* restore cosEntry in case function has changed it and trfClass*/
        trfClass = 0;

        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
        cosEntry.cosParam.dropPrecedence = dpLevel;

        /* 1.8. Call function with out of range dpLevel [0x5AAAAAA5].   */
        /* Expected: GT_BAD_PARAM.                                      */

        dpLevel = COS_MAP_INVALID_ENUM_CNS;
        /* trfClass == 0                                        */
        /* tableType == CPSS_COS_TABLE_REMARKING_E              */
        /* cosEntry.cosParam.trafficClass == trfClass           */  
        /* cosEntry.cosParam.dropPrecedence == CPSS_DP_GREEN_E  */

        st = cpssExMxCosByTcDpSet(dev, trfClass, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, trfClass, dpLevel, tableType);

        /* restore cosEntry in case function has changed it and dpLevel*/
        dpLevel = CPSS_DP_RED_E;

        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
        cosEntry.cosParam.dropPrecedence = dpLevel;

        /* 1.9. Call function with cosEntryPtr [NULL].  */
        /* Expected: GT_BAD_PTR.                        */

        st = cpssExMxCosByTcDpSet(dev, trfClass, dpLevel, tableType, NULL);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, %d, NULL",
                                     dev, trfClass, dpLevel, tableType);

        /* 1.10. Call function with out of range tableType [0x5AAAAAA5]. */
        /* Expected: GT_BAD_PARAM.                                      */
        tableType = COS_MAP_INVALID_ENUM_CNS;
        /* trfClass == 0                                     */
        /* dpLevel == CPSS_DP_RED_E                          */
        /* cosEntry.cosParam.trafficClass == trfClass        */  
        /* cosEntry.cosParam.dropPrecedence == dpLevel       */

        st = cpssExMxCosByTcDpSet(dev, trfClass, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, trfClass, dpLevel, tableType);
    }

    trfClass    = 0;
    dpLevel     = CPSS_DP_GREEN_E;
    tableType   = CPSS_COS_TABLE_REMARKING_E;

    cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
    cosEntry.cosParam.dropPrecedence = dpLevel;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosByTcDpSet(dev, trfClass, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);

        /* restore cosEntry in case function has changed it*/
        cpssOsBzero((GT_VOID*)&cosEntry, sizeof(cosEntry));
        cosEntry.cosParam.dropPrecedence = dpLevel;
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosByTcDpSet(dev, trfClass, dpLevel, tableType, &cosEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxCosByTcDpGet
(
    IN  GT_U8                       dev,
    IN  GT_U8                       trfClass,
    IN  CPSS_DP_LEVEL_ENT           dpLevel,
    IN  CPSS_COS_TABLE_TYPE_ENT     tableType,
    OUT CPSS_COS_MAP_STC            *cosEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxCosByTcDpGet)
{
/*
ITERATE_DEVICES
1.1. Call function with trfClass [CPSS_TC_RANGE_CNS - 1], 
    dpLevel [CPSS_DP_GREEN_E], non-NULL cosEntryPtr, 
    tableType [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK.
1.2. Call function with trfClass [CPSS_TC_RANGE_CNS - 1], 
    dpLevel [CPSS_DP_RED_E], non-NULL cosEntryPtr, 
    tableType [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK.
1.3. Call function with trfClass [0], dpLevel [CPSS_DP_GREEN_E], 
    non-NULL cosEntryPtr, tableType [CPSS_COS_TABLE_MARKING_E]. 
    Expected: GT_OK for devices from Twist-C family 
    and NON GT_OK for other devices.
1.4. Call function with out of range trfClass [CPSS_TC_RANGE_CNS]. 
    Expected: GT_BAD_PARAM.
1.5. Call function with out of range dpLevel [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
1.6. Call function with cosEntryPtr [NULL]. 
    Expected: GT_BAD_PTR.
1.7. Call function with out of range tableType [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                       dev;
    GT_U8                       trfClass;
    CPSS_DP_LEVEL_ENT           dpLevel;
    CPSS_COS_TABLE_TYPE_ENT     tableType;
    CPSS_COS_MAP_STC            cosEntry;

    CPSS_PP_FAMILY_TYPE_ENT     devFamily;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet");

        /* 1.1. Call function with trfClass [CPSS_TC_RANGE_CNS - 1],    */
        /* dpLevel [CPSS_DP_GREEN_E], non-NULL cosEntryPtr,             */
        /* tableType [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK.     */
        trfClass    = CPSS_TC_RANGE_CNS - 1;
        dpLevel     = CPSS_DP_GREEN_E;
        tableType   = CPSS_COS_TABLE_REMARKING_E;

        st = cpssExMxCosByTcDpGet(dev, trfClass, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trfClass, dpLevel, tableType);

        /* 1.2. Call function with trfClass [CPSS_TC_RANGE_CNS - 1],    */
        /* dpLevel [CPSS_DP_RED_E], non-NULL cosEntryPtr,               */
        /* tableType [CPSS_COS_TABLE_REMARKING_E]. Expected: GT_OK.     */
        dpLevel = CPSS_DP_RED_E;
        /* tableType == CPSS_COS_TABLE_REMARKING_E     */
        /* trfClass == CPSS_TC_RANGE_CNS - 1           */

        st = cpssExMxCosByTcDpGet(dev, trfClass, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trfClass, dpLevel, tableType);

        /* 1.3. Call function with trfClass [0], dpLevel [CPSS_DP_GREEN_E], */
        /* non-NULL cosEntryPtr , tableType [CPSS_COS_TABLE_MARKING_E].     */
        /* Expected: GT_OK for devices from Twist-C family                  */
        /* and NON GT_OK for other devices.                                 */
        trfClass    = 0;
        dpLevel     = CPSS_DP_GREEN_E;
        tableType   = CPSS_COS_TABLE_MARKING_E;

        st = cpssExMxCosByTcDpGet(dev, trfClass, dpLevel, tableType, &cosEntry);

        if (CPSS_PP_FAMILY_TWISTC_E == devFamily)
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "for Twist-C device: %d, %d, %d, %d",
                                         dev, trfClass, dpLevel, tableType);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "for non Twist-C device: %d, %d, %d, %d",
                                             dev, trfClass, dpLevel, tableType);
        }

        tableType = CPSS_COS_TABLE_REMARKING_E;

        /* 1.7. Call function with out of range trfClass [CPSS_TC_RANGE_CNS]. */
        /* Expected: GT_BAD_PARAM.                                            */
        trfClass = CPSS_TC_RANGE_CNS;
        /* dpLevel == CPSS_DP_GREEN_E                   */
        /* tableType == CPSS_COS_TABLE_REMARKING_E      */

        st = cpssExMxCosByTcDpGet(dev, trfClass, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, trfClass, dpLevel, tableType);

        /* restore trfClass*/
        trfClass = 0;

        /* 1.8. Call function with out of range dpLevel [0x5AAAAAA5].   */
        /* Expected: GT_BAD_PARAM.                                      */

        dpLevel = COS_MAP_INVALID_ENUM_CNS;
        /* trfClass == 0                                        */
        /* tableType == CPSS_COS_TABLE_REMARKING_E              */

        st = cpssExMxCosByTcDpGet(dev, trfClass, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, trfClass, dpLevel, tableType);

        /* restore dpLevel*/
        dpLevel = CPSS_DP_RED_E;

        /* 1.9. Call function with cosEntryPtr [NULL].  */
        /* Expected: GT_BAD_PTR.                        */

        st = cpssExMxCosByTcDpGet(dev, trfClass, dpLevel, tableType, NULL);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, %d, NULL",
                                     dev, trfClass, dpLevel, tableType);

        /* 1.10. Call function with out of range tableType [0x5AAAAAA5]. */
        /* Expected: GT_BAD_PARAM.                                      */
        tableType = COS_MAP_INVALID_ENUM_CNS;
        /* trfClass == 0                                     */
        /* dpLevel == CPSS_DP_RED_E                          */
        /* tableType == CPSS_COS_TABLE_REMARKING_E           */

        st = cpssExMxCosByTcDpGet(dev, trfClass, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, trfClass, dpLevel, tableType);
    }

    trfClass    = 0;
    dpLevel     = CPSS_DP_GREEN_E;
    tableType   = CPSS_COS_TABLE_REMARKING_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxCosByTcDpGet(dev, trfClass, dpLevel, tableType, &cosEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxCosByTcDpGet(dev, trfClass, dpLevel, tableType, &cosEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cppExMxCosMap suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxCosMap)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosByDscpDpSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosByDscpDpGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosByExpDpSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosByExpDpGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosByTcDpSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxCosByTcDpGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxCosMap)
