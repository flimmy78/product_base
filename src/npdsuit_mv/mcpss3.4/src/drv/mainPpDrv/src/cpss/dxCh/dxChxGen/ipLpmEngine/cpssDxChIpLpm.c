/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpLpm.c
*
* DESCRIPTION:
*       the CPSS DXCH LPM Hierarchy manager .
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/private/prvCpssSkipList.h>
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChIpTcamMgm.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChIpPatTrie.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpm.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpmUc.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpmMc.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/prvCpssDxChIpTcamEntriesHandler.h>
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManager.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/prvCpssDxChIpTcamDispatcher.h>


/*******************************************************************************
 *  extern vars                                                                 *
 ******************************************************************************/
/* Pointers to dummy trie nodes to support policy based routing MC defaults */
GT_U32                              pbrDefaultMcTrieNodePtrArraySize = 0;
PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC  **pbrDefaultMcTrieNodePtrArray = NULL;

/* number of rows in the TCAM */
extern GT_U32  tcamRows;

/*******************************************************************************
 *  local vars                                                                 *
 ******************************************************************************/
GT_VOID *prvCpssDxChIplpmDbSL;

/*******************************************************************************
* lpmDbComp
*
* DESCRIPTION:
*   lpm DB compare function
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       data1Ptr        - the first lpm DB to compare.
*       data2Ptr        - the second lpm DB to compare.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_COMP_RES - equal ,smaller or bigger.
*
* COMMENTS:
*       none.
*
*
*******************************************************************************/
static GT_COMP_RES lpmDbComp
(
    IN GT_VOID *data1Ptr,
    IN GT_VOID *data2Ptr
)
{
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDb1Ptr,*lpmDb2Ptr;

    lpmDb1Ptr = data1Ptr;
    lpmDb2Ptr = data2Ptr;

    /* Check the null cases     */
    if((lpmDb1Ptr == NULL) && (lpmDb2Ptr == NULL))
    {
        return GT_EQUAL;
    }
    if (lpmDb1Ptr == NULL)
    {
        return GT_SMALLER;
    }
    if (lpmDb2Ptr == NULL)
    {
        return GT_GREATER;
    }

    /* Both data's are not NULL */
    if (lpmDb1Ptr->lpmDBId != lpmDb2Ptr->lpmDBId)
    {
        if (lpmDb1Ptr->lpmDBId > lpmDb2Ptr->lpmDBId)
        {
            return GT_GREATER;
        }
        else
        {
            return GT_SMALLER;
        }
    }

    return GT_EQUAL;
}

/*******************************************************************************
* cpssDxChIpLpmDBCreate
*
* DESCRIPTION:
*   This function creates an LPM DB for a shared LPM managment.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId               - the LPM DB id.
*       shadowType            - the type of shadow to maintain (cheetah/cheetha2)
*       protocolStack         - the type of protocol stack this LPM DB support.
*       indexesRangePtr       - the range of TCAM indexes availble for this
*                               LPM DB (see explanation in
*                               CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC)
*       partitionEnable       - GT_TRUE:  partition the TCAM range according to the
*                                         capacityCfgPtr, any unused TCAM entries will
*                                         be allocated to IPv4 UC entries
*                               GT_FALSE: allocate TCAM entries on demand while
*                                         guarantee entries as specified in capacityCfgPtr
*       tcamLpmManagerCapcityCfgPtr - holds the capacity configuration required
*                                     from this TCAM LPM manager
*       tcamManagerHandlerPtr       - the TCAM manager handler
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_ALREADY_EXIST         - if the LPM DB id is already used.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_SUPPORTED         - request is not supported if partitioning
*                                  is disabled.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      1. if partitionEnable = GT_FALSE then the TCAM is one big shared resource
*         for all kinds of prefixes. In a shared resource situation a best effort
*         is applied to put the inserted prefixes in a way they wouldn't interfere
*         with other prefixes. But there could be a situation where due to
*         fragmentation a prefix couldn't be inserted, in this situation all
*         prefix insert function has a defragmationEnable to enable a performance
*         costing de-fragmentation process in order to fit the inserted prefix.
*      2. TCAM Manager - external TCAM Manager module. TCAM manager is intended to
*         manage tcams of XCAT and above devices otherwise legacy TCAM Entries Handler is
*         used. TCAM manager could be created explicitly before calling to
*         cpssDxChIpLpmDBCreate or (if tcamManagerHandlerPtr == NULL)implicitly
*         in cpssDxChIpLpmDBCreate (backward compatible mode).
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBCreate
(
    IN GT_U32                                       lpmDBId,
    IN CPSS_DXCH_IP_TCAM_SHADOW_TYPE_ENT            shadowType,
    IN CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    IN GT_BOOL                                      partitionEnable,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *tcamLpmManagerCapcityCfgPtr,
    IN  GT_VOID                                     *tcamManagerHandlerPtr
)
{
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    GT_U32 slIter;

    CPSS_NULL_PTR_CHECK_MAC(indexesRangePtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamLpmManagerCapcityCfgPtr);
    PRV_CPSS_DXCH_IP_CHECK_SHADOW_TYPE_MAC(shadowType);
    PRV_CPSS_DXCH_IP_CHECK_PROTOCOL_STACK_MAC(protocolStack);

    if (partitionEnable == GT_TRUE)
    {
        if ((tcamLpmManagerCapcityCfgPtr->numOfIpv6Prefixes == 0) &&
            ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E)))
                return GT_BAD_PARAM;

        if ((tcamLpmManagerCapcityCfgPtr->numOfIpv4McSourcePrefixes == 0) &&
            (tcamLpmManagerCapcityCfgPtr->numOfIpv4Prefixes == 0) &&
            ((protocolStack == CPSS_IP_PROTOCOL_IPV4_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E)))
                return GT_BAD_PARAM;
    }

    if (prvCpssDxChIplpmDbSL == NULL)
    {
        prvCpssDxChIplpmDbSL = prvCpssSlInit(lpmDbComp,PRV_CPSS_MAX_PP_DEVICES_CNS);
        if (prvCpssDxChIplpmDbSL == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }
    }

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr != NULL)
    {
        /* can't redefine an lpm DB */
        return GT_ALREADY_EXIST;
    }

    retVal = cpssDxChIpTcamInit(shadowType,indexesRangePtr,partitionEnable,
                                tcamLpmManagerCapcityCfgPtr,protocolStack,
                                tcamManagerHandlerPtr, &lpmDbPtr);

    if (retVal == GT_OK)
    {
        /* record the lpm db */
        lpmDbPtr->lpmDBId = lpmDBId;
        retVal = prvCpssSlAdd(prvCpssDxChIplpmDbSL,lpmDbPtr,&slIter);
    }

    return retVal;
}



/*******************************************************************************
* cpssDxChIpLpmDBCapacityUpdate
*
* DESCRIPTION:
*   This function updates the initial LPM DB allocation.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       lpmDBId                     - the LPM DB id.
*       indexesRangePtr             - the range of TCAM indexes availble for this
*                                     LPM DB (see explanation in
*                                     CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       tcamLpmManagerCapcityCfgPtr - the new capacity configuration. when
*                                     partitionEnable in cpssDxChIpLpmDBCreate
*                                     was set to GT_TRUE this means new prefixes
*                                     partition, when this was set to GT_FALSE
*                                     this means the new prefixes guaranteed
*                                     allocation.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success
*       GT_BAD_PARAM                - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*       GT_NOT_FOUND                - if the LPM DB id is not found
*       GT_NO_RESOURCE              - failed to allocate resources needed to the
*                                     new configuration
*
* COMMENTS:
*       This API is relevant only when using TCAM Manager. This API is used for
*       updating only the capacity configuration of the LPM. for updating the
*       lines reservation for the TCAM Manger use cpssDxChTcamManagerRangeUpdate.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBCapacityUpdate
(
    IN GT_U32                                       lpmDBId,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *tcamLpmManagerCapcityCfgPtr
)
{
    GT_STATUS                                           retVal = GT_OK;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC                    *lpmDbPtr,tmpLpmDb;
    CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT reservationType;
    GT_BOOL                                             partitionEnable;
    CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC             reservedEntriesArray[4];
    CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC             reservedEntriesArrayDummy[4];
    CPSS_DXCH_TCAM_MANAGER_RANGE_STC                    reservationRange;
    GT_U32                                              totalLines;
    GT_U32                                              totalSingleEntries;
    GT_U32                                              totalQuadEntries;
    GT_U32                                              sizeOfArray = 4;
    GT_U32                                              sizeOfDummyArray = 4;
    GT_U32                                              numberOfUnusedTcamEntries = 0;

    CPSS_NULL_PTR_CHECK_MAC(indexesRangePtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamLpmManagerCapcityCfgPtr);

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    if (lpmDbPtr->tcamManagementMode != PRV_CPSS_DXCH_IP_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)
    {
        /*this api is relevant only when working with tcam manager*/
        return GT_NOT_SUPPORTED;
    }

    /* get reservation type */
    retVal = cpssDxChTcamManagerEntriesReservationGet(lpmDbPtr->tcamManagerHandlerPtr,
                                                      lpmDbPtr->clientId,
                                                      &reservationType,
                                                      reservedEntriesArrayDummy,
                                                      &sizeOfDummyArray,
                                                      &reservationRange);

    if (retVal != GT_OK)
        return retVal;

    if (reservationType == CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E )
    {
        partitionEnable = GT_TRUE;
    }
    else
    {
        partitionEnable = GT_FALSE;
    }

    retVal = prvCpssDxChCheckCapacityForTcamShadow(lpmDbPtr->shadowType,
                                                   indexesRangePtr,
                                                   partitionEnable,
                                                   tcamLpmManagerCapcityCfgPtr,
                                                   &totalLines,
                                                   &totalSingleEntries,
                                                   &totalQuadEntries);

    if (retVal != GT_OK)
        return retVal;

    retVal = prvCpssDxChPrepareTcamReservation(lpmDbPtr->shadowType,
                                               partitionEnable,
                                               tcamLpmManagerCapcityCfgPtr,
                                               totalSingleEntries + totalQuadEntries * 4,
                                               totalLines,
                                               &reservationType,
                                               reservedEntriesArray,
                                               &sizeOfArray,
                                               &numberOfUnusedTcamEntries);

    if (retVal != GT_OK)
        return retVal;

    reservationRange.firstLine = indexesRangePtr->firstIndex;
    reservationRange.lastLine = indexesRangePtr->lastIndex;

    /* Reserve entries for a client in the TCAM manager*/
    retVal =  cpssDxChTcamManagerEntriesReservationSet(lpmDbPtr->tcamManagerHandlerPtr,
                                                       lpmDbPtr->clientId,
                                                       reservationType,
                                                       reservedEntriesArray,
                                                       sizeOfArray,
                                                       &reservationRange);
    if (retVal == GT_OK)
    {
        /* update TCAM shadow */
        lpmDbPtr->tcamIndexRange.firstIndex = indexesRangePtr->firstIndex;
        lpmDbPtr->tcamIndexRange.lastIndex = indexesRangePtr->lastIndex;
        lpmDbPtr->numOfIpMcSrcAddr =
            tcamLpmManagerCapcityCfgPtr->numOfIpv4McSourcePrefixes;
        lpmDbPtr->numOfIpPrefixes[CPSS_IP_PROTOCOL_IPV4_E] =
            tcamLpmManagerCapcityCfgPtr->numOfIpv4Prefixes;
        if ( (lpmDbPtr->isIpVerIsInitialized[CPSS_IP_PROTOCOL_IPV4_E] == GT_TRUE) &&
             (lpmDbPtr->tcamPartitionEnable == GT_TRUE) )
        {
            tcamLpmManagerCapcityCfgPtr->numOfIpv4Prefixes += numberOfUnusedTcamEntries;
        }
        lpmDbPtr->numOfIpPrefixes[CPSS_IP_PROTOCOL_IPV6_E] =
            tcamLpmManagerCapcityCfgPtr->numOfIpv6Prefixes;
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmDBCapacityGet
*
* DESCRIPTION:
*   This function gets the current LPM DB allocation.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       lpmDBId                     - the LPM DB id.
*
* OUTPUTS:
*       indexesRangePtr             - the range of TCAM indexes availble for this
*                                     LPM DB (see explanation in
*                                     CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       partitionEnablePtr          - GT_TRUE:  the TCAM is partitioned according
*                                     to the capacityCfgPtr, any unused TCAM entries
*                                     were allocated to IPv4 UC entries.
*                                     GT_FALSE: TCAM entries are allocated on demand
*                                     while entries are guaranteed as specified
*                                     in capacityCfgPtr.
*       tcamLpmManagerCapcityCfgPtr - the current capacity configuration. when
*                                     partitionEnable in cpssDxChIpLpmDBCreate
*                                     was set to GT_TRUE this means current
*                                     prefixes partition, when this was set to
*                                     GT_FALSE this means the current guaranteed
*                                     prefixes allocation.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NO_RESOURCE           - failed to allocate resources needed to the
*                                  new configuration.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBCapacityGet
(
    IN  GT_U32                                          lpmDBId,
    OUT GT_BOOL                                         *partitionEnablePtr,
    OUT CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC    *indexesRangePtr,
    OUT CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    *tcamLpmManagerCapcityCfgPtr
)
{
    GT_STATUS retVal = GT_OK;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    CPSS_DXCH_TCAM_MANAGER_ENTRIES_RESERVATION_TYPE_ENT reservationType;
    CPSS_DXCH_TCAM_MANAGER_ENTRY_AMOUNT_STC             reservedEntriesArray[2];
    CPSS_DXCH_TCAM_MANAGER_RANGE_STC                    reservationRange;
    GT_U32 sizeOfArray = 2;

    CPSS_NULL_PTR_CHECK_MAC(partitionEnablePtr);
    CPSS_NULL_PTR_CHECK_MAC(indexesRangePtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamLpmManagerCapcityCfgPtr);

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    if (lpmDbPtr->tcamManagementMode != PRV_CPSS_DXCH_IP_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)
    {
        /*this api is relevant only when working with tcam manager*/
        return GT_NOT_SUPPORTED;
    }

    retVal = cpssDxChTcamManagerEntriesReservationGet(lpmDbPtr->tcamManagerHandlerPtr,
                                                      lpmDbPtr->clientId,
                                                      &reservationType,
                                                      reservedEntriesArray,
                                                      &sizeOfArray,
                                                      &reservationRange);

    if (retVal != GT_OK)
        return retVal;

    if (reservationType == CPSS_DXCH_TCAM_MANAGER_STATIC_ENTRIES_RESERVATION_E)
    {
        *partitionEnablePtr = GT_TRUE;
    }
    else
    {
        *partitionEnablePtr = GT_FALSE;
    }

    indexesRangePtr->firstIndex = lpmDbPtr->tcamIndexRange.firstIndex;
    indexesRangePtr->lastIndex = lpmDbPtr->tcamIndexRange.lastIndex;

    tcamLpmManagerCapcityCfgPtr->numOfIpv4McSourcePrefixes = lpmDbPtr->numOfIpMcSrcAddr;
    tcamLpmManagerCapcityCfgPtr->numOfIpv4Prefixes = lpmDbPtr->numOfIpPrefixes[CPSS_IP_PROTOCOL_IPV4_E];
    tcamLpmManagerCapcityCfgPtr->numOfIpv6Prefixes = lpmDbPtr->numOfIpPrefixes[CPSS_IP_PROTOCOL_IPV6_E];

    return GT_OK;
}


/*******************************************************************************
* cpssDxChIpLpmDBCheetah2VrSupportCreate
*
* DESCRIPTION:
*   This function creates an LPM DB for a shared LPM managment in Cheetah2
*   devices with support for virtual router.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       lpmDBId          - the LPM DB id.
*       protocolStack    - the type of protocol stack this LPM DB support.
*       pclTcamCfgPtr    - the pcl tcam configuration
*       routerTcamCfgPtr - the router tcam configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_ALREADY_EXIST         - if the LPM DB id is already used.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - NULL pointer.
*
* COMMENTS:
*       Since LTT rules are shared between PCL and Router Tcam, several checks
*       are made to supplied parameters to make sure that all prefixes could be
*       inserted.
*
*       Note that LTT entries corresponding to routerTcamCfgPtr->tcamRange
*       are allocated to Lpm Manager.
*
*       Furthermore:
*
*       routerTcamCfgPtr->tcamRange.lastIndex -
*       routerTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
*       (number of Router Tcam lines).
*
*       pclTcamCfgPtr->tcamRange.lastIndex -
*       pclTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
*       (number of Pcl Tcam lines).
*
*       (routerTcamCfgPtr->tcamRange.lastIndex -
*        routerTcamCfgPtr->tcamRange.firstIndex + 1) * 5
*           must be larger or equal to
*       pclTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
*       pclTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes +
*       routerTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
*       routerTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes
*
*       Multicast is not supported.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBCheetah2VrSupportCreate
(
    IN GT_U32                                                   lpmDBId,
    IN CPSS_IP_PROTOCOL_STACK_ENT                               protocolStack,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *pclTcamCfgPtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *routerTcamCfgPtr
)
{
    GT_STATUS                           retVal = GT_OK;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC    *lpmDbPtr,tmpLpmDb;
    GT_U32                              slIter;

    CPSS_NULL_PTR_CHECK_MAC(pclTcamCfgPtr);
    CPSS_NULL_PTR_CHECK_MAC(routerTcamCfgPtr);
    PRV_CPSS_DXCH_IP_CHECK_PROTOCOL_STACK_MAC(protocolStack);

    if (prvCpssDxChIplpmDbSL == NULL)
    {
        prvCpssDxChIplpmDbSL = prvCpssSlInit(lpmDbComp,PRV_CPSS_MAX_PP_DEVICES_CNS);
        if (prvCpssDxChIplpmDbSL == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }
    }

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr != NULL)
    {
        /* can't redefine an lpm DB */
        return GT_ALREADY_EXIST;
    }

    /* ok, create a new one */
    lpmDbPtr = (PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC*)
                        cpssOsMalloc(sizeof(PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC));
    if (lpmDbPtr == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    retVal = cpssDxChIpTcamCheetah2VrSupportInit(protocolStack, pclTcamCfgPtr,
                                                 routerTcamCfgPtr, &lpmDbPtr);

    if (retVal == GT_OK)
    {
        /* record the lpm db */
        lpmDbPtr->lpmDBId = lpmDBId;
        retVal = prvCpssSlAdd(prvCpssDxChIplpmDbSL,lpmDbPtr,&slIter);
    }
    else
    {
        cpssOsFree(lpmDbPtr);
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmDBDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing LPM DB . this addition will
*   invoke a hot sync of the newly added devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devListArr     - the array of device ids to add to the LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if devListArr is NULL pointer.
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBDevListAdd
(
    IN GT_U32   lpmDBId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
)
{
    PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STC addedDevList;
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    if(numOfDevs)
    {
        CPSS_NULL_PTR_CHECK_MAC(devListArr);
    }


    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    addedDevList.ipShareDevNum = numOfDevs;
    addedDevList.ipShareDevs = devListArr;
    retVal = cpssDxChIpTcamAddDevToShadow(&addedDevList,lpmDbPtr);
    return retVal;
}


/*******************************************************************************
* cpssDxChIpLpmDBDevsListRemove
*
* DESCRIPTION:
*   This function removes devices from an existing LPM DB . this remove will
*   invoke a hot sync removal of the devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devListArr     - the array of device ids to remove from the
*                        LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBDevsListRemove
(
    IN GT_U32   lpmDBId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
)
{
    PRV_CPSS_DXCH_IP_SHADOW_DEVS_LIST_STC addedDevList;
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    if(numOfDevs > 0)
    CPSS_NULL_PTR_CHECK_MAC(devListArr);

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    addedDevList.ipShareDevNum = numOfDevs;
    addedDevList.ipShareDevs = devListArr;
    retVal = cpssDxChIpTcamRemoveDevsFromShadow(&addedDevList,lpmDbPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmTcamLinesReserve
*
* DESCRIPTION:
*   This function reserves lines in the router TCAM. The LPM DB will not use
*   those lines for prefixes. Those lines will be considered as allocated
*   entries.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat2; Lion.
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*       linesToReserveArray       - lines to be reserved
*       numberOfLinesToReserve    - number of lines in linesToReserveArray
*       allocateReserveLinesAs    - relevant only when LPM DB is configured to
*                                   use dynamic TCAM partitioning.
*                                   Indicates as what entry type to treat the
*                                   reserved lines.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - all lines to reserve must reside in the TCAM section allocated
*           to the LPM DB
*         - all lines to reserve must be unique
*       In case the LPM DB is configured to use dynamic partitioning those
*       additional conditions must be matched:
*         - the sum of the lines in allocateReserveLinesAs must be equal to
*           the number of lines to reserve
*         - the lines in allocateReserveLinesAs must comply with the protocol
*           stacks supported by the LPM DB
*         - the LPM DB must have enough space to hold the allocateReserveLinesAs
*           as allocated entires.
*       Note that the parameters in allocateReserveLinesAs are in lines. Meaning
*       that if (allocateReserveLinesAs.numOfIpv4Prefixes == 1) it means that
*       one whole TCAM line will be considered allocated as IPv4 prefixes. In
*       4 columns TCAM it means that 4 IPv4 prefixes will be "allocated" in
*       this line to reserve.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmTcamLinesReserve
(
    IN  GT_U32                                          lpmDBId,
    IN  GT_U32                                          linesToReserveArray[],
    IN  GT_U32                                          numberOfLinesToReserve,
    IN  CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    allocateReserveLinesAs
)
{
    GT_STATUS                                           retVal = GT_OK;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC                    *lpmDbPtr,tmpLpmDb;
    GT_U32                                              index;
    GT_U32                                              tcamColumns = 0;
    GT_U32                                              entriesUsed;
    GT_U32                                              i,j;
    PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType       = PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E;
    PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   forcedEntryType = PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E;
    PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC                 *ruleNodePtr = NULL;
    PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC                 *allocatedRuleNodePtr = NULL;

    CPSS_NULL_PTR_CHECK_MAC(linesToReserveArray);

    /* search for the LPM DB */
    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        return GT_NOT_FOUND;
    }
    if (lpmDbPtr->shadowType != CPSS_DXCH_IP_TCAM_CHEETAH3_SHADOW_E)
    {
        return GT_NOT_SUPPORTED;
    }

    /* the LPM DB must be empty when using this API (can't
       contain any prefixes or virtual routers) */
    if (prvCpssSlIsEmpty(lpmDbPtr->vrSl) != GT_TRUE)
    {
        return GT_BAD_STATE;
    }

    /*when using tcam manager lines can be exluded via static client*/
    if (lpmDbPtr->tcamManagementMode == PRV_CPSS_DXCH_IP_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)
        return GT_NOT_SUPPORTED;

    /* verify that all lines to reserve resides within the TCAM range allocated
       to the LPM DB and that all lines in the array are unique */
    for (i = 0 ; i < numberOfLinesToReserve ; i++)
    {
        if ((linesToReserveArray[i] < lpmDbPtr->tcamIndexRange.firstIndex) ||
            (linesToReserveArray[i] > lpmDbPtr->tcamIndexRange.lastIndex))
            return GT_BAD_PARAM;

        for (j = i + 1 ; j < numberOfLinesToReserve ; j++)
        {
            if (linesToReserveArray[j] == linesToReserveArray[i])
                return GT_BAD_PARAM;
        }
    }

    /* get number of columns in the TCAM according to shadow type */
    switch (lpmDbPtr->shadowType)
    {
    case CPSS_DXCH_IP_TCAM_CHEETAH3_SHADOW_E:
        tcamColumns = 4;
        break;
    default:
        /* no router TCAM is supported */
        return GT_BAD_PARAM;
    }

    /* parameter checks in case of dynamic partitioning only */
    if (lpmDbPtr->tcamEntriesHandler->operationMode == PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E)
    {
        /* verify number of lines in allocateReserveLinesAs match the number of
           lines to reserve */
        if ((allocateReserveLinesAs.numOfIpv4Prefixes +
             allocateReserveLinesAs.numOfIpv4McSourcePrefixes +
             allocateReserveLinesAs.numOfIpv6Prefixes) != numberOfLinesToReserve)
        {
            return GT_BAD_PARAM;
        }

        /* verify the allocateReserveLinesAs match LPM DB supported protocols */
        if ((lpmDbPtr->isIpVerIsInitialized[CPSS_IP_PROTOCOL_IPV4_E] == GT_FALSE) &&
            ((allocateReserveLinesAs.numOfIpv4Prefixes + allocateReserveLinesAs.numOfIpv4McSourcePrefixes) != 0))
            return GT_BAD_PARAM;
        if ((lpmDbPtr->isIpVerIsInitialized[CPSS_IP_PROTOCOL_IPV6_E] == GT_FALSE) &&
            (allocateReserveLinesAs.numOfIpv6Prefixes != 0))
            return GT_BAD_PARAM;

        /* verify that reserved lines can be marked as allocated according
           to allocateReserveLinesAs parameter */
        entriesUsed = 0;
        entriesUsed += ((allocateReserveLinesAs.numOfIpv4Prefixes * tcamColumns) > lpmDbPtr->tcamEntriesHandler->guaranteedIpv4) ?
            (allocateReserveLinesAs.numOfIpv4Prefixes * tcamColumns) : lpmDbPtr->tcamEntriesHandler->guaranteedIpv4;
        entriesUsed += ((allocateReserveLinesAs.numOfIpv4McSourcePrefixes * tcamColumns) > lpmDbPtr->tcamEntriesHandler->guaranteedIpv4McSrc) ?
            (allocateReserveLinesAs.numOfIpv4McSourcePrefixes * tcamColumns) : lpmDbPtr->tcamEntriesHandler->guaranteedIpv4McSrc;
        entriesUsed += (((allocateReserveLinesAs.numOfIpv6Prefixes > lpmDbPtr->tcamEntriesHandler->guaranteedIpv6) ?
            allocateReserveLinesAs.numOfIpv6Prefixes : lpmDbPtr->tcamEntriesHandler->guaranteedIpv6) * tcamColumns);

        if (entriesUsed > lpmDbPtr->tcamEntriesHandler->numOfTcamEntries)
            return GT_BAD_STATE;
    }

    /* Mark the reserved line as allocated in the TCAM entries handler */
    for (i = 0 ; i < numberOfLinesToReserve ; i++)
    {
        /* in case of dynamic partition, detemine how to allocated the reserved line */
        if (lpmDbPtr->tcamEntriesHandler->operationMode == PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E)
        {
            if (allocateReserveLinesAs.numOfIpv6Prefixes > 0)
            {
                forcedEntryType = PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E;
                allocateReserveLinesAs.numOfIpv6Prefixes--;
            }
            else if (allocateReserveLinesAs.numOfIpv4Prefixes > 0)
            {
                forcedEntryType = PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E;
                allocateReserveLinesAs.numOfIpv4Prefixes--;
            }
            else
            {
                forcedEntryType = PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E;
                allocateReserveLinesAs.numOfIpv4McSourcePrefixes--;
            }
        }

        /* get the entry type associated with the TCAM index */
        index = linesToReserveArray[i];
        /* in case of dynamic partition set entry type as input */
        if (lpmDbPtr->tcamEntriesHandler->operationMode == PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E)
            entryType = forcedEntryType;
        retVal = prvCpssDxChTcamEntriesHandlerGetRuleInfoByRealIdx(lpmDbPtr->tcamEntriesHandler,
                                                                   index,
                                                                   &entryType,
                                                                   &ruleNodePtr);
        if (retVal != GT_OK)
            return retVal;

        /* if entry type is IPv6 just allocate this line */
        if (entryType == PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E)
        {
            retVal = prvCpssDxChTcamEntriesHandlerAllocByGivenRule(lpmDbPtr->tcamEntriesHandler,
                                                                   entryType,
                                                                   ruleNodePtr,
                                                                   &allocatedRuleNodePtr);
            if (retVal != GT_OK)
                return retVal;
        }
        /* if entry type is not IPv6, go over each entry in the line
                   and check if IPv4 UC or IPv4 MC Src */
        else
        {
            for (j = 0 ; j < tcamColumns ; j++)
            {
                index = linesToReserveArray[i] + j * tcamRows;
                /* in case of dynamic partition set entry type as input */
                if (lpmDbPtr->tcamEntriesHandler->operationMode == PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E)
                    entryType = forcedEntryType;
                retVal = prvCpssDxChTcamEntriesHandlerGetRuleInfoByRealIdx(lpmDbPtr->tcamEntriesHandler,
                                                                           index,
                                                                           &entryType,
                                                                           &ruleNodePtr);
                if (retVal != GT_OK)
                    return retVal;

                retVal = prvCpssDxChTcamEntriesHandlerAllocByGivenRule(lpmDbPtr->tcamEntriesHandler,
                                                                       entryType,
                                                                       ruleNodePtr,
                                                                       &allocatedRuleNodePtr);
                if (retVal != GT_OK)
                    return retVal;
            }
        }
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
*
* DESCRIPTION:
*   The function uses available TCAM rules at the beginning of the TCAM range
*   allocated to the LPM DB and place there "do nothing" rules that will make
*   sure that Multicast traffic is matched by these rules and not by the
*   Unicast default rule.
*   When policy based routing Unicast traffic coexists with IP based Multicast
*   (S,G,V) bridging (used in IGMP and MLD protocols), there is a need to add
*   default Multicast rules in order to make sure that the Multicast traffic
*   will not be matched by the Unicast default rule.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*       pclIdArrayLen             - size of the PCL ID array (range 1..1048575)
*       pclIdArray                - array of PCL ID that may be in used by the LPM DB
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_BAD_PARAM              - if wrong value in any of the parameters
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_ALREADY_EXIST          - if default MC already set
*       GT_NOT_SUPPORTED          - if the LPM DB doesn't configured to
*                                   operate in policy based routing mode
*       GT_FULL                   - if TCAM is full
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - the LPM DB must be configured to use policy based routing
*       One default Multicast rule will be set for each protocol stack supported
*       by the LPM DB and for each PCL ID that may be used in the LPM DB (each
*       virtaul router uses unique PCL ID).
*       For each protocol stack, the Multicast default rules will use the
*       prefixes allocated for Unicast prefixes for this protocol. For example
*       if the application allocates 10 IPv4 Unicast prefixes, then after setting
*       one default Multicast prefix, only 9 IPv4 Unicast prefixes will be
*       available.
*       The default Multicast rules will capture all IPv4/IPv6 Multicast
*       traffic with the same assigned PCL ID. As a result all rules configured
*       to match IP Multicast traffic with same assigned PCL ID that reside
*       after the Multicast defaults will not be matched.
*       The default Multicast rules can not be deleted after set.
*       The default Multicast rules can be set no more than once.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
(
    IN  GT_U32  lpmDBId,
    IN  GT_U32  pclIdArrayLen,
    IN  GT_U32  pclIdArray[]
)
{
    GT_STATUS                                           rc = GT_OK;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC                    *lpmDbPtr,tmpLpmDb;
    CPSS_IP_PROTOCOL_STACK_ENT                          protocolStack;
    PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT   entryType;
    GT_PTR                                              tcamRulePtr = NULL;
    GT_U32                                              arrayIndex;
    GT_U32                                              i;
    GT_U32                                              prefixNumber = 1;
    CPSS_IP_UNICAST_MULTICAST_ENT                       ipMode = CPSS_IP_UNICAST_E;
    PRV_CPSS_DXCH_IP_LPM_MC_ENTRY_TYPE_ENT              mcEntryType = PRV_CPSS_DXCH_IP_LPM_MC_GROUP_E;

    if ((pclIdArrayLen == 0) || (pclIdArrayLen > 1048575))
        return GT_BAD_PARAM;

    /* search for the LPM DB */
    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        return GT_NOT_FOUND;
    }
    /* default MC can not be set more than once */
    if (lpmDbPtr->usePbrDefaultMc == GT_TRUE)
    {
        return GT_ALREADY_EXIST;
    }
    /* the LPM DB must be empty when using this API (can't
       contain any prefixes or virtual routers) */
    if (prvCpssSlIsEmpty(lpmDbPtr->vrSl) != GT_TRUE)
    {
        return GT_BAD_STATE;
    }
    /* the LPM DB must be configured to policy based routing mode */
    switch (lpmDbPtr->shadowType)
    {
    case CPSS_DXCH_IP_TCAM_CHEETAH_SHADOW_E:
    case CPSS_DXCH_IP_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
        break;
    default:
        return GT_NOT_SUPPORTED;
    }

    /* allocate memory for the default MC rule pointers: 2 rules per PCL ID
       that may be used be the LPM DB (one per protocol stack) */
    pbrDefaultMcTrieNodePtrArraySize = 2 * pclIdArrayLen;
    pbrDefaultMcTrieNodePtrArray = cpssOsMalloc(pbrDefaultMcTrieNodePtrArraySize *
                                                sizeof(PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC*));
    if (pbrDefaultMcTrieNodePtrArray == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }
    /* init all pointers to NULL */
    cpssOsMemSet(pbrDefaultMcTrieNodePtrArray,0,pbrDefaultMcTrieNodePtrArraySize *
                 sizeof(PRV_CPSS_DXCH_IP_PAT_TRIE_NODE_STC*));

    /* for each PCL ID add one MC default per protocol stack supported by the LPM DB */
    for (i = 0 ; i < pclIdArrayLen ; i++)
    {
        /* for each supported protocol stack, set rule for default MC */
        for (protocolStack = CPSS_IP_PROTOCOL_IPV4_E ; protocolStack <= CPSS_IP_PROTOCOL_IPV6_E ; protocolStack++)
        {
            arrayIndex = 2 * i + protocolStack;
            /* verify the LPM DB supports the protocol stack */
            if (lpmDbPtr->isIpVerIsInitialized[protocolStack] == GT_FALSE)
                continue;
            /* The new TCAM allocation is on the way -- check by means of prefix
               counters if it is possible to do */
            rc = prvCpssDxChIpTcamFreeSpaceCountersCheck(prefixNumber,ipMode,mcEntryType,
                                                         protocolStack,lpmDbPtr);
            if (rc != GT_OK)
            {
                cpssOsFree(pbrDefaultMcTrieNodePtrArray);
                return rc;
            }
            /* create dummy trie node for policy based routing MC defaults */
            rc = cpssDxChIpPatTrieInit(lpmDbPtr,&pbrDefaultMcTrieNodePtrArray[arrayIndex]);
            if (rc != GT_OK)
                return rc;

            /* to support pbr mc defaults in Tcam Manager Mode allocate memory for
               token info attached to dummy pbrDefaultMcTrieNode */
            PRV_CPSS_DXCH_ALLOCATE_TOKEN_NODE_MAC(lpmDbPtr->tcamManagementMode,
                                                  pbrDefaultMcTrieNodePtrArray[arrayIndex]);

            if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
                entryType = PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E;
            else /* protocolStack == CPSS_IP_PROTOCOL_IPV6_E */
                entryType = PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E;

            /* allocate minimal rule for the default MC from the UC pool */
            rc =  prvCpssDxChIpTcamDispatchAllocate(lpmDbPtr,
                                                    entryType,
                                                    NULL,
                                                    NULL,
                                                    PRV_CPSS_DXCH_IP_TCAM_MGM_ALLOC_MIN_E,
                                                    GT_FALSE,
                                                    pbrDefaultMcTrieNodePtrArray[arrayIndex],
                                                    &tcamRulePtr);
            if (rc != GT_OK)
                return rc;
            lpmDbPtr->allocatedIpPrefixCounters.numOfAllocatedIpPrefixes[protocolStack]++;
            /* update allocated rule rule */
            rc = prvCpssDxChIpTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_DISPATCHER_TCAM_ALLOCATED_RULE_UPDATE_E,
                                                           lpmDbPtr->tcamManagementMode,
                                                           NULL,
                                                           pbrDefaultMcTrieNodePtrArray[arrayIndex],
                                                           tcamRulePtr);
            if (rc != GT_OK)
                return rc;

            /* update TCAM rule data */
            rc = prvCpssDxChIpTcamDispatcherRuleInfoUpdate(PRV_CPSS_DXCH_DISPATCHER_TCAM_RULE_UPDATE_TRIE_E,
                                                           lpmDbPtr->tcamManagementMode,
                                                           pbrDefaultMcTrieNodePtrArray[arrayIndex],
                                                           pbrDefaultMcTrieNodePtrArray[arrayIndex],
                                                           NULL);
            if (rc != GT_OK)
                return rc;

            /* store the PCL ID in the trie node vrId field */
            pbrDefaultMcTrieNodePtrArray[arrayIndex]->vrId = pclIdArray[i];
        }
    }

    /* set the default MC rule in the PCL */
    rc = prvCpssDxChIpLpmPbrDefaultMcRuleSet(lpmDbPtr,
                                             lpmDbPtr->workDevListPtr);
    if (rc != GT_OK)
        return rc;

    /* mark that this LPM DB uses policy based routing default MC */
    lpmDbPtr->usePbrDefaultMc = GT_TRUE;

    return rc;
}

/*******************************************************************************
* cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet
*
* DESCRIPTION:
*   The function gets whether the LPM DB is configured to support default MC
*   rules in policy based routing mode and the rule indexes and PCL ID of those
*   default rules.
*   Refer to cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet for more details.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId                           - the LPM DB id.
*       defaultIpv4RuleIndexArrayLenPtr   - points to the allocated size of the array
*       defaultIpv6RuleIndexArrayLenPtr   - points to the allocated size of the array
*       pclIdArrayLenPtr                  - points to the allocated size of the array
*
* OUTPUTS:
*       defaultMcUsedPtr                  - whether default MC is used for this LPM DB
*       protocolStackPtr                  - protocol stack supported by this LPM DB
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv4RuleIndexArray         - rule indexes of the default IPv4 MC (in case
*                                           the LPM DB support IPv4)
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv4RuleIndexArrayLenPtr   - points to number of elements filled in the array
*       defaultIpv6RuleIndexArray         - rule indexes of the default IPv6 MC (in case
*                                           the LPM DB support IPv6)
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv6RuleIndexArrayLenPtr   - points to number of elements filled in the array
*       pclIdArray                        - array of PCL ID that may be in used by the LPM DB
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       pclIdArrayLenPtr                  - points to number of elements filled in the array
*
* RETURNS:
*       GT_OK                             - on success
*       GT_NOT_FOUND                      - if the LPM DB id is not found
*       GT_BAD_PTR                        - if one of the parameters is NULL pointer.
*       GT_FULL                           - if any of the arrays is not big enough
*       GT_FAIL                           - on failure
*       GT_NOT_APPLICABLE_DEVICE          - on not applicable device
*
* COMMENTS:
*       The rule indexes are according to explanations in cpssDxChPclRuleSet.
*
*******************************************************************************/
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
{
    GT_STATUS rc = GT_OK;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    GT_PTR tcamRulePtr = NULL;
    GT_U32 offset;
    GT_U32 i;
    GT_U32 ipv4ArrayCurrentIndex = 0;
    GT_U32 ipv6ArrayCurrentIndex = 0;
    GT_U32 pclIdArrayCurrentindex = 0;

    CPSS_NULL_PTR_CHECK_MAC(defaultMcUsedPtr);
    CPSS_NULL_PTR_CHECK_MAC(protocolStackPtr);
    CPSS_NULL_PTR_CHECK_MAC(defaultIpv4RuleIndexArray);
    CPSS_NULL_PTR_CHECK_MAC(defaultIpv4RuleIndexArrayLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(defaultIpv6RuleIndexArray);
    CPSS_NULL_PTR_CHECK_MAC(defaultIpv6RuleIndexArrayLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(pclIdArray);
    CPSS_NULL_PTR_CHECK_MAC(pclIdArrayLenPtr);

    /* search for the LPM DB */
    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        return GT_NOT_FOUND;
    }

    *defaultMcUsedPtr = lpmDbPtr->usePbrDefaultMc;

    if (*defaultMcUsedPtr == GT_TRUE)
    {
        /* determine the protocol stack the LPM DB supports */
        if (lpmDbPtr->isIpVerIsInitialized[CPSS_IP_PROTOCOL_IPV4_E] == GT_TRUE)
        {
            if (lpmDbPtr->isIpVerIsInitialized[CPSS_IP_PROTOCOL_IPV6_E] == GT_TRUE)
                *protocolStackPtr = CPSS_IP_PROTOCOL_IPV4V6_E;
            else
                *protocolStackPtr = CPSS_IP_PROTOCOL_IPV4_E;
        }
        else
        {
            if (lpmDbPtr->isIpVerIsInitialized[CPSS_IP_PROTOCOL_IPV6_E] == GT_TRUE)
                *protocolStackPtr = CPSS_IP_PROTOCOL_IPV6_E;
            else
                return GT_FAIL;     /* should never happen */
        }

        /* go over default MC rules allocated and fill output arrays */
        for (i = 0 ; i < pbrDefaultMcTrieNodePtrArraySize ; i++)
        {
            if (pbrDefaultMcTrieNodePtrArray[i] == NULL)
                continue;

            rc = prvCpssDxChIpTcamDispatcherRuleSet(pbrDefaultMcTrieNodePtrArray[i],
                                                    lpmDbPtr,&tcamRulePtr);
            if (rc != GT_OK)
                return rc;

            rc = prvCpssDxChGetIpTcamOffset(lpmDbPtr,tcamRulePtr,GT_TRUE,&offset);
            if (rc != GT_OK)
                return rc;

            /* even entries are used to IPv4 while odd one for IPv6 */
            if ((i % 2) == 0)
            {
                if ((ipv4ArrayCurrentIndex + 1) > *defaultIpv4RuleIndexArrayLenPtr)
                    return GT_FULL;
                defaultIpv4RuleIndexArray[ipv4ArrayCurrentIndex] = offset;
                ipv4ArrayCurrentIndex++;

                /* if IPv6 is not supported, update the PCL ID from the ipv4 entry */
                if (*protocolStackPtr != CPSS_IP_PROTOCOL_IPV6_E)
                {
                    if ((pclIdArrayCurrentindex + 1) > *pclIdArrayLenPtr)
                        return GT_FULL;
                    pclIdArray[pclIdArrayCurrentindex] = pbrDefaultMcTrieNodePtrArray[i]->vrId;
                    pclIdArrayCurrentindex++;
                }
            }
            else /* odd (ipv6) entry */
            {
                if (ipv6ArrayCurrentIndex + 1 > *defaultIpv6RuleIndexArrayLenPtr)
                    return GT_FULL;
                defaultIpv6RuleIndexArray[ipv6ArrayCurrentIndex] = offset;
                ipv6ArrayCurrentIndex++;

                /* if only IPv6 is supported, update the PCL ID from the ipv6 entry */
                if (*protocolStackPtr == CPSS_IP_PROTOCOL_IPV6_E)
                {
                    if ((pclIdArrayCurrentindex + 1) > *pclIdArrayLenPtr)
                        return GT_FULL;
                    pclIdArray[pclIdArrayCurrentindex] = pbrDefaultMcTrieNodePtrArray[i]->vrId;
                    pclIdArrayCurrentindex++;
                }
            }
        }
    }

    *defaultIpv4RuleIndexArrayLenPtr = ipv4ArrayCurrentIndex;
    *defaultIpv4RuleIndexArrayLenPtr = ipv6ArrayCurrentIndex;
    *pclIdArrayLenPtr                = pclIdArrayCurrentindex;

    return rc;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*   This function adds a virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.
*       vrConfigPtr          - Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PARAM             - if wrong value in any of the parameters
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       Refer to cpssDxChIpLpmVirtualRouterSharedAdd for limitation when shared
*       virtual router is used.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterAdd
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId,
    IN  CPSS_DXCH_IP_LPM_VR_CONFIG_STC        *vrConfigPtr
)
{
    GT_STATUS                               retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_VR_TCAM_TBL_BLOCK_STC     *sharedVrPtr,tmpVrEntry;
    CPSS_IP_PROTOCOL_STACK_ENT               protocolStack;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *defIpv4UcNextHopInfoPtr = NULL;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *defIpv6UcNextHopInfoPtr = NULL;
    CPSS_DXCH_IP_LTT_ENTRY_STC              *defIpv4McRouteLttEntryPtr = NULL;
    CPSS_DXCH_IP_LTT_ENTRY_STC              *defIpv6McRouteLttEntryPtr = NULL;

    CPSS_NULL_PTR_CHECK_MAC(vrConfigPtr);

    /* at least one of the ipv4/6 uc/mc features must be supported */
    if ((vrConfigPtr->supportIpv4Uc == GT_FALSE) &&
        (vrConfigPtr->supportIpv6Uc == GT_FALSE) &&
        (vrConfigPtr->supportIpv4Mc == GT_FALSE) &&
        (vrConfigPtr->supportIpv6Mc == GT_FALSE))
    {
        return GT_BAD_PARAM;
    }

    /* init pointers */
    if (vrConfigPtr->supportIpv4Uc == GT_TRUE)
    {
        defIpv4UcNextHopInfoPtr = &vrConfigPtr->defIpv4UcNextHopInfo;
    }
    if (vrConfigPtr->supportIpv6Uc == GT_TRUE)
    {
        defIpv6UcNextHopInfoPtr = &vrConfigPtr->defIpv6UcNextHopInfo;
    }
    if (vrConfigPtr->supportIpv4Mc == GT_TRUE)
    {
        defIpv4McRouteLttEntryPtr = &vrConfigPtr->defIpv4McRouteLttEntry;
    }
    if (vrConfigPtr->supportIpv6Mc == GT_TRUE)
    {
        defIpv6McRouteLttEntryPtr = &vrConfigPtr->defIpv6McRouteLttEntry;
    }

    /* determine the supported protocols */
    if ((vrConfigPtr->supportIpv6Uc == GT_TRUE) || (vrConfigPtr->supportIpv6Mc == GT_TRUE))
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
    }
    else
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    }

    /* if protocol stack was determined to be ipv6, we will check if it ipv4v6 */
    if ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) &&
        ((vrConfigPtr->supportIpv4Uc == GT_TRUE) || (vrConfigPtr->supportIpv4Mc == GT_TRUE)))
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
    }

    /* search for the LPM DB */
    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    /* policy based shadow types do not support MC */
    if ((lpmDbPtr->shadowType == CPSS_DXCH_IP_TCAM_CHEETAH_SHADOW_E) ||
        ((lpmDbPtr->shadowType == CPSS_DXCH_IP_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E)))
    {
        if ( (vrConfigPtr->supportIpv4Mc == GT_TRUE) || (vrConfigPtr->supportIpv6Mc == GT_TRUE) )
            return GT_NOT_SUPPORTED;
    }

    /* if shared virtual router exists, make sure this added virtual router
       does not share prefixes with the shared virtual router */
    if (lpmDbPtr->sharedVrExist == GT_TRUE)
    {
        /* retrieve the shared virtual router */
        tmpVrEntry.vrId = lpmDbPtr->sharedVrId;
        sharedVrPtr = prvCpssSlSearch(lpmDbPtr->vrSl,&tmpVrEntry);
        if (sharedVrPtr == NULL)
            return GT_FAIL; /* should never happen */

        /* check if UC entries exist in the shared virtual router */
        if ((vrConfigPtr->supportIpv4Uc == GT_TRUE) || (vrConfigPtr->supportIpv6Uc == GT_TRUE))
        {
            if ((protocolStack == CPSS_IP_PROTOCOL_IPV4_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
                if (sharedVrPtr->ipUcTrie[0] != NULL)
                    return GT_BAD_PARAM;
            if ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
                if (sharedVrPtr->ipUcTrie[1] != NULL)
                    return GT_BAD_PARAM;
        }

        /* check if MC entries exist in the shared virtual router */
        if ((vrConfigPtr->supportIpv4Mc == GT_TRUE) || (vrConfigPtr->supportIpv6Mc == GT_TRUE))
        {
            if ((protocolStack == CPSS_IP_PROTOCOL_IPV4_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
                if (sharedVrPtr->ipMcTrie[0] != NULL)
                    return GT_BAD_PARAM;
            if ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
                if (sharedVrPtr->ipMcTrie[1] != NULL)
                    return GT_BAD_PARAM;
        }
    }

    retVal = cpssDxChIpTcamAddVirtualRouter(vrId,
                                            defIpv4UcNextHopInfoPtr,
                                            defIpv6UcNextHopInfoPtr,
                                            NULL,
                                            defIpv4McRouteLttEntryPtr,
                                            defIpv6McRouteLttEntryPtr,
                                            NULL,
                                            protocolStack,
                                            lpmDbPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterSharedAdd
*
* DESCRIPTION:
*   This function adds a shared virtual router in system for specific LPM DB.
*   Prefixes that reside within shared virtual router will participate in the
*   lookups of all virtual routers.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The  virtual's router ID.
*       vrConfigPtr         - Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       1.  Only one shared virtual router can exists at any time within a
*           given LPM DB.
*       2.  Virtual router ID that is used for non-shared virtual router can't
*           be used for the shared virtual router and via versa.
*       3.  Shared prefixes can't overlap non-shared prefixes. If the shared
*           virtual router supports shared prefixes type then adding non-shared
*           virtual router that supports the same prefixes type will fail.
*           Also, if a non-shared virtual router that supports prefixes type
*           exists, then adding a shared virtual router that supports the same
*           prefixes type will fail.
*       4.  When the shared virtual router supports IPv4 UC prefixes, then the
*           the following will apply:
*             - The <match all> default TCAM entry will not be written to TCAM
*               (however TCAM entry will still be allocated to this entry)
*             - The following prefixes will be added to match all non-MC traffic:
*               0x00/1, 0x80/2, 0xC0/3, 0xF0/4
*           Same applies when the shared virtual router supports IPv6 UC
*           prefixes. The prefixes added to match all non-MC traffic are:
*           0x00/1, 0x80/2, 0xC0/3, 0xE0/4, 0xF0/5, 0xF8/6, 0xFC/7, 0xFE/8
*       5.  The application should not delete the non-MC prefixes (when added).
*       6.  Manipulation of the default UC will not be applied on the non-MC
*           prefixes. The application should manipulate those non-MC entries
*           directly when needed.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterSharedAdd
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId,
    IN  CPSS_DXCH_IP_LPM_VR_CONFIG_STC        *vrConfigPtr
)
{
    GT_STATUS                               retVal;
    GT_STATUS                               retVal2;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_VR_TCAM_TBL_BLOCK_STC     *vrEntryPtr;
    GT_U32                                  iter;
    GT_U32                                  internalVrId;
    GT_IPADDR                               nonMcIpv4Addr[4];
    GT_U32                                  nonMcIpv4PrefixLen[4];
    GT_IPV6ADDR                             nonMcIpv6Addr[8];
    GT_U32                                  nonMcIpv6PrefixLen[8];
    GT_U32                                  i;
    GT_BOOL                                 defragEnable; /* whether LPM DB supports dynamic partition */
    CPSS_IP_PROTOCOL_STACK_ENT               protocolStack;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *defIpv4UcNextHopInfoPtr = NULL;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *defIpv6UcNextHopInfoPtr = NULL;
    CPSS_DXCH_IP_LTT_ENTRY_STC              *defIpv4McRouteLttEntryPtr = NULL;
    CPSS_DXCH_IP_LTT_ENTRY_STC              *defIpv6McRouteLttEntryPtr = NULL;

    CPSS_NULL_PTR_CHECK_MAC(vrConfigPtr);

    /* at least one of the ipv4/6 uc/mc features must be supported */
    if ((vrConfigPtr->supportIpv4Uc == GT_FALSE) &&
        (vrConfigPtr->supportIpv6Uc == GT_FALSE) &&
        (vrConfigPtr->supportIpv4Mc == GT_FALSE) &&
        (vrConfigPtr->supportIpv6Mc == GT_FALSE))
    {
        return GT_BAD_PARAM;
    }

    /* init pointers */
    if (vrConfigPtr->supportIpv4Uc == GT_TRUE)
    {
        defIpv4UcNextHopInfoPtr = &vrConfigPtr->defIpv4UcNextHopInfo;
    }
    if (vrConfigPtr->supportIpv6Uc == GT_TRUE)
    {
        defIpv6UcNextHopInfoPtr = &vrConfigPtr->defIpv6UcNextHopInfo;
    }
    if (vrConfigPtr->supportIpv4Mc == GT_TRUE)
    {
        defIpv4McRouteLttEntryPtr = &vrConfigPtr->defIpv4McRouteLttEntry;
    }
    if (vrConfigPtr->supportIpv6Mc == GT_TRUE)
    {
        defIpv6McRouteLttEntryPtr = &vrConfigPtr->defIpv6McRouteLttEntry;
    }

    /* determine the supported protocols */
    if ((vrConfigPtr->supportIpv6Uc == GT_TRUE) || (vrConfigPtr->supportIpv6Mc == GT_TRUE))
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
    }
    else
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    }

    /* if protocol stack was determined to be ipv6, we will check if it ipv4v6 */
    if ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) &&
        ((vrConfigPtr->supportIpv4Uc == GT_TRUE) || (vrConfigPtr->supportIpv4Mc == GT_TRUE)))
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
    }

    /* Currently, shared MC prefixes are not supported */
    if ( (vrConfigPtr->supportIpv4Mc == GT_TRUE) || (vrConfigPtr->supportIpv6Mc == GT_TRUE) )
        return GT_NOT_SUPPORTED;

    /* search for the LPM DB */
    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    /* only one shared virtual router can exists within LPM DB */
    if (lpmDbPtr->sharedVrExist == GT_TRUE)
        return GT_ALREADY_EXIST;

    /* Only ch3 shadow type and above are supported. Policy based routing
       shadow types are not supported */
    if ((lpmDbPtr->shadowType != CPSS_DXCH_IP_TCAM_CHEETAH3_SHADOW_E) &&
        (lpmDbPtr->shadowType != CPSS_DXCH_IP_TCAM_XCAT_SHADOW_E))
    {
        return GT_NOT_SUPPORTED;
    }

    /* scan all existing non-shared virtual routers and verify that the shared
       virtual router will not share prefixes with them */
    iter = 0;
    while ((vrEntryPtr = prvCpssSlGetNext(lpmDbPtr->vrSl,&iter)) != NULL)
    {
        /* check if UC entries exist in non-shared virtual router */
        if ((vrConfigPtr->supportIpv4Uc == GT_TRUE) || (vrConfigPtr->supportIpv6Uc == GT_TRUE))
        {
            if ((protocolStack == CPSS_IP_PROTOCOL_IPV4_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
                if (vrEntryPtr->ipUcTrie[0] != NULL)
                    return GT_BAD_PARAM;
            if ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
                if (vrEntryPtr->ipUcTrie[1] != NULL)
                    return GT_BAD_PARAM;
        }

        /* check if MC entries exist in non-shared virtual router */
        if ( (vrConfigPtr->supportIpv4Mc == GT_TRUE) || (vrConfigPtr->supportIpv6Mc == GT_TRUE) )
        {
            if ((protocolStack == CPSS_IP_PROTOCOL_IPV4_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
                if (vrEntryPtr->ipMcTrie[0] != NULL)
                    return GT_BAD_PARAM;
            if ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
                if (vrEntryPtr->ipMcTrie[1] != NULL)
                    return GT_BAD_PARAM;
        }
    }

    /* internally, bit 15 in vrId indicates shared virtual router */
    internalVrId = vrId | BIT_15;

    /* add the virtual router */
    retVal = cpssDxChIpTcamAddVirtualRouter(internalVrId,
                                            defIpv4UcNextHopInfoPtr,
                                            defIpv6UcNextHopInfoPtr,
                                            NULL,
                                            defIpv4McRouteLttEntryPtr,
                                            defIpv6McRouteLttEntryPtr,
                                            NULL,
                                            protocolStack,lpmDbPtr);
    if (retVal != GT_OK)
        return retVal;

    /* the virtual router was added successfully */
    lpmDbPtr->sharedVrExist = GT_TRUE;
    lpmDbPtr->sharedVrId = vrId;

    /* For shared virtual router that supports shared IPv4 UC and/or IPv6 UC,
       the <match all> TCAM entry that is used as UC default is not written to
       the TCAM. Instead add prefixes that represent all non-MC prefixes */
    if ((vrConfigPtr->supportIpv4Uc == GT_TRUE) || (vrConfigPtr->supportIpv6Uc == GT_TRUE))
    {
        /* add non-MC IPv4 prefixes */
        if ((protocolStack == CPSS_IP_PROTOCOL_IPV4_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
        {
            /* IPv4 MC prefixes are 0xE0/4. Therefore non IPv4 MC prefixes
               are: 0x00/1, 0x80/2, 0xC0/3, 0xF0/4 */
            cpssOsMemSet(nonMcIpv4Addr,0,sizeof(nonMcIpv4Addr));
            nonMcIpv4Addr[0].arIP[0] = 0;   nonMcIpv4PrefixLen[0] = 1;
            nonMcIpv4Addr[1].arIP[0] = 128; nonMcIpv4PrefixLen[1] = 2;
            nonMcIpv4Addr[2].arIP[0] = 192; nonMcIpv4PrefixLen[2] = 3;
            nonMcIpv4Addr[3].arIP[0] = 240; nonMcIpv4PrefixLen[3] = 4;

            for (i = 0 ; i < 4 ; i++)
            {
                retVal = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId,vrId,
                                                      nonMcIpv4Addr[i],
                                                      nonMcIpv4PrefixLen[i],
                                                      defIpv4UcNextHopInfoPtr,
                                                      GT_FALSE);
                if (retVal != GT_OK)
                {
                    retVal2 = cpssDxChIpLpmVirtualRouterDel(lpmDBId,vrId);
                    return retVal;
                }
            }
        }

        /* add non-MC IPv6 prefixes */
        if ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
        {
            /* IPv6 MC prefixes are 0xFF/8. Therefore non IPv6 MC prefixes
               are: 0x00/1, 0x80/2, 0xC0/3, 0xE0/4, 0xF0/5, 0xF8/6, 0xFC/7, 0xFE/8 */
            cpssOsMemSet(nonMcIpv6Addr,0,sizeof(nonMcIpv6Addr));
            nonMcIpv6Addr[0].arIP[0] = 0x00;    nonMcIpv6PrefixLen[0] = 1;
            nonMcIpv6Addr[1].arIP[0] = 0x80;    nonMcIpv6PrefixLen[1] = 2;
            nonMcIpv6Addr[2].arIP[0] = 0xC0;    nonMcIpv6PrefixLen[2] = 3;
            nonMcIpv6Addr[3].arIP[0] = 0xE0;    nonMcIpv6PrefixLen[3] = 4;
            nonMcIpv6Addr[4].arIP[0] = 0xF0;    nonMcIpv6PrefixLen[4] = 5;
            nonMcIpv6Addr[5].arIP[0] = 0xF8;    nonMcIpv6PrefixLen[5] = 6;
            nonMcIpv6Addr[6].arIP[0] = 0xFC;    nonMcIpv6PrefixLen[6] = 7;
            nonMcIpv6Addr[7].arIP[0] = 0xFE;    nonMcIpv6PrefixLen[7] = 8;

            if (lpmDbPtr->tcamManagementMode == PRV_CPSS_DXCH_IP_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)
            {
                CPSS_TBD_BOOKMARK
                defragEnable = GT_FALSE;
            }
            else
            {
                if (lpmDbPtr->tcamEntriesHandler->operationMode == PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_PARTITION_MODE_E)
                    defragEnable = GT_FALSE;
                else if (lpmDbPtr->tcamEntriesHandler->operationMode == PRV_CPSS_DXCH_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E)
                    defragEnable = GT_TRUE;
                else
                    return GT_BAD_STATE;
            }

            for (i = 0 ; i < 8 ; i++)
            {
                retVal = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId,vrId,
                                                      nonMcIpv6Addr[i],
                                                      nonMcIpv6PrefixLen[i],
                                                      defIpv6UcNextHopInfoPtr,
                                                      GT_FALSE,defragEnable);
                if (retVal != GT_OK)
                {
                    retVal2 = cpssDxChIpLpmVirtualRouterDel(lpmDBId,vrId);
                    return retVal;
                }
            }
        }
    } /* end adding non-MC prefixes */

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterGet
*
* DESCRIPTION:
*   This function gets the virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.
*
* OUTPUTS:
*       vrConfigPtr    - Virtual router configuration
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterGet
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    OUT CPSS_DXCH_IP_LPM_VR_CONFIG_STC          *vrConfigPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    CPSS_NULL_PTR_CHECK_MAC(vrConfigPtr);

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamGetVirtualRouter(vrId,
                                            lpmDbPtr,
                                            &vrConfigPtr->supportIpv4Uc,
                                            &vrConfigPtr->defIpv4UcNextHopInfo,
                                            &vrConfigPtr->supportIpv6Uc,
                                            &vrConfigPtr->defIpv6UcNextHopInfo,
                                            NULL,
                                            &vrConfigPtr->supportIpv4Mc,
                                            &vrConfigPtr->defIpv4McRouteLttEntry,
                                            &vrConfigPtr->supportIpv6Mc,
                                            &vrConfigPtr->defIpv6McRouteLttEntry,
                                            NULL);
    return retVal;

}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterDel
*
* DESCRIPTION:
*   This function removes a virtual router in system for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterDel
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamDeleteVirtualRouter(vrId,lpmDbPtr);

    /* if shared virtual router was deleted, mark that in the LPM DB */
    if ((lpmDbPtr->sharedVrExist == GT_TRUE) && (vrId == lpmDbPtr->sharedVrId))
    {
        lpmDbPtr->sharedVrExist = GT_FALSE;
        lpmDbPtr->sharedVrId = 0;
    }

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv4 prefix in a Virtual Router for
*   the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info accosiated with this UC prefix.
*       override        - override an existing entry for this mask if it already
*                         exists, or don't override and return error.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR                - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAdd
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPADDR                                ipAddr,
    IN GT_U32                                   prefixLen,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *nextHopInfoPtr,
    IN GT_BOOL                                  override
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);

    if (prefixLen > 32)
    {
        return GT_OUT_OF_RANGE;
    }

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamAddIpUcEntry(vrId,ipAddr.arIP,prefixLen,
                                        nextHopInfoPtr,NULL,override,GT_FALSE,
                                        GT_FALSE /* defragEnable */,
                                        CPSS_IP_PROTOCOL_IPV4_E,lpmDbPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArrayPtr - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of prefixes' lengths is too big
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAddBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArrayPtr
)
{
    GT_STATUS retVal,retVal2;
    GT_U32 i;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    CPSS_NULL_PTR_CHECK_MAC(ipv4PrefixArrayPtr);

    retVal = GT_OK;
    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    for (i = 0; i < ipv4PrefixArrayLen; i++)
    {
        if (ipv4PrefixArrayPtr[i].prefixLen > 32)
        {
            return GT_OUT_OF_RANGE;
        }

        ipv4PrefixArrayPtr[i].returnStatus =
            cpssDxChIpTcamAddIpUcEntry(ipv4PrefixArrayPtr[i].vrId,
                                       ipv4PrefixArrayPtr[i].ipAddr.arIP,
                                       ipv4PrefixArrayPtr[i].prefixLen,
                                       &ipv4PrefixArrayPtr[i].nextHopInfo,
                                       NULL,ipv4PrefixArrayPtr[i].override,GT_TRUE,
                                       GT_FALSE /* defragEnable */,
                                       CPSS_IP_PROTOCOL_IPV4_E,lpmDbPtr);
        if (ipv4PrefixArrayPtr[i].returnStatus != GT_OK)
        {
            retVal = GT_FAIL;
        }
    }

    /* now update the hw */
    retVal2 = cpssDxChIpTcamUpdateHW(lpmDbPtr,
                                     PRV_DXCH_IP_UPDATE_UC_E,
                                     CPSS_IP_PROTOCOL_IPV4_E);
    if (retVal == GT_OK)
        retVal = retVal2;


    return retVal;
}


/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv4 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixDel
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               vrId,
    IN GT_IPADDR                            ipAddr,
    IN GT_U32                               prefixLen
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    if (prefixLen > 32)
    {
        return GT_OUT_OF_RANGE;
    }

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamDeleteIpUcEntry(vrId,ipAddr.arIP,prefixLen,
                                           CPSS_IP_PROTOCOL_IPV4_E,lpmDbPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixDelBulk
*
* DESCRIPTION:
*   Deletes an existing bulk of Ipv4 prefixes in a Virtual Router for the
*   specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArrayPtr - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of the prefixes' lengths is too big
*       GT_BAD_PTR                - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixDelBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArrayPtr
)
{
    GT_STATUS retVal = GT_OK;
    GT_U32 i;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    if(ipv4PrefixArrayLen)
    {
        CPSS_NULL_PTR_CHECK_MAC(ipv4PrefixArrayPtr);
    }


    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    for (i = 0; i < ipv4PrefixArrayLen; i++)
    {
        if (ipv4PrefixArrayPtr[i].prefixLen > 32)
        {
            return GT_OUT_OF_RANGE;
        }

        ipv4PrefixArrayPtr[i].returnStatus =
            cpssDxChIpTcamDeleteIpUcEntry(ipv4PrefixArrayPtr[i].vrId,
                                          ipv4PrefixArrayPtr[i].ipAddr.arIP,
                                          ipv4PrefixArrayPtr[i].prefixLen,
                                          CPSS_IP_PROTOCOL_IPV4_E,lpmDbPtr);
        if (ipv4PrefixArrayPtr[i].returnStatus != GT_OK)
            retVal = GT_FAIL;
    }

    return retVal;
}


/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv4 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamFlushIpUcEntries(vrId,CPSS_IP_PROTOCOL_IPV4_E,
                                            lpmDbPtr,GT_FALSE);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the
*   information associated with it.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr  - If found, this is the route entry info accosiated with
*                         this UC prefix.
*
* RETURNS:
*       GT_OK - if the required entry was found, or
*       GT_NOT_FOUND - if the given ip prefix was not found.
*
* COMMENTS:
*       use by npd - cpss1.3
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixSearch_tmp
(
    IN  unsigned int                                  lpmDBId,
    IN  unsigned int                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    IN  unsigned int                                  prefixLen,
    OUT unsigned int                                      *internalRuleIdx,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamSearchIpUcEntry(vrId,ipAddr.arIP,prefixLen,
                                           nextHopInfoPtr,internalRuleIdx,
                                           CPSS_IP_PROTOCOL_IPV4_E,lpmDbPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the
*   information associated with it.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr       - if found, this is the route entry info.
*                               accosiated with this UC prefix.
*       tcamRowIndexPtr      - if found, TCAM row index of this uc prefix.
*       tcamColumnIndexPtr   - if found, TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixSearch
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    IN  GT_U32                                  prefixLen,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    GT_U32                            *internalRuleIdxPtr = NULL;

    if (prefixLen > 32)
    {
        return GT_OUT_OF_RANGE;
    }

    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamColumnIndexPtr);

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamSearchIpUcEntry(vrId,ipAddr.arIP,prefixLen,
                                           nextHopInfoPtr,internalRuleIdxPtr,
                                           CPSS_IP_PROTOCOL_IPV4_E,lpmDbPtr);
    if(retVal != GT_OK)
        return retVal;

    retVal = cpssDxChIpTcamUcRuleOffsetGet(vrId,ipAddr.arIP,
                                           prefixLen,
                                           CPSS_IP_PROTOCOL_IPV4_E,
                                           lpmDbPtr,
                                           tcamRowIndexPtr,
                                           tcamColumnIndexPtr);
    return retVal;
}


/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one and prefix Tcam index; it used for iterating over the exisiting
*   prefixes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId      - The LPM DB id.
*       vrId         - The virtual router Id to get the entry from.
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr            - The ip address of the found entry.
*       prefixLenPtr         - The prefix length of the found entry.
*       nextHopInfoPtr       - If found, this is the route entry info accosiated
*                              with this UC prefix.
*       tcamRowIndexPtr      - TCAM row index of this uc prefix.
*       tcamColumnIndexPtr   - TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixGetNext
(
    IN    GT_U32                                    lpmDBId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPADDR                                 *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr,
    OUT GT_U32                                      *tcamRowIndexPtr,
    OUT GT_U32                                      *tcamColumnIndexPtr

)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    GT_U32                            *internalRuleIdxPtr = NULL;

    GT_U8  tempIpAddr[4];
    GT_U32 ipAddrNumOfOctets;
    GT_U32 ipAddrNumOfBitsToMask;

    CPSS_NULL_PTR_CHECK_MAC(ipAddrPtr);
    CPSS_NULL_PTR_CHECK_MAC(prefixLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamColumnIndexPtr);

    if (*prefixLenPtr > 32)
    {
        return GT_OUT_OF_RANGE;
    }

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamGetIpUcEntry(vrId,
                                        ipAddrPtr->arIP,
                                        prefixLenPtr,
                                        CPSS_IP_PROTOCOL_IPV4_E,
                                        nextHopInfoPtr,
                                        internalRuleIdxPtr,
                                        lpmDbPtr);
    if(retVal != GT_OK)
        return retVal;

    retVal = cpssDxChIpTcamUcRuleOffsetGet(vrId,
                                           ipAddrPtr->arIP,
                                           *prefixLenPtr,
                                           CPSS_IP_PROTOCOL_IPV4_E,
                                           lpmDbPtr,
                                           tcamRowIndexPtr,
                                           tcamColumnIndexPtr);
     if(retVal != GT_OK)
        return retVal;

    /* reset non relevant bit according to prefix length */

    cpssOsMemSet(tempIpAddr,0,sizeof(tempIpAddr));
    ipAddrNumOfOctets = *prefixLenPtr / 8;
    ipAddrNumOfBitsToMask = *prefixLenPtr % 8;

    if(ipAddrNumOfBitsToMask == 0)
    {
        cpssOsMemCpy(tempIpAddr,&ipAddrPtr->arIP,sizeof(GT_U8)*ipAddrNumOfOctets);
    }
    else
    {
        cpssOsMemCpy(tempIpAddr,&ipAddrPtr->arIP,sizeof(GT_U8)*(ipAddrNumOfOctets+1));
    }

    cpssOsMemCpy(&ipAddrPtr->arIP,tempIpAddr,sizeof(ipAddrPtr->arIP));

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryAdd
*
* DESCRIPTION:
*   Add IP multicast route for a particular/all source and a particular
*   group address. this is done for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual private network identifier.
*       ipGroup             - The IP multicast group address.
*       ipGroupPrefixLen    - The number of bits that are actual valid in,
*                             the ipGroup.
*       ipSrc               - the root address for source base multi tree
*                             protocol.
*       ipSrcPrefixLen      - The number of bits that are actual valid in,
*                             the ipSrc.
*       mcRouteLttEntryPtr  - the LTT entry pointing to the MC route entry
*                             associated with this MC route.
*       override            - whether to override an mc Route pointer for the
*                             given prefix
*       defragmentationEnable - whether to enable performance costing
*                             de-fragmentation process in the case that there
*                             is no place to insert the prefix. To point of the
*                             process is just to make space for this prefix.
*                             relevant only if the LPM DB was created with
*                             partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - if one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = 0.
*
*******************************************************************************/
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
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    CPSS_NULL_PTR_CHECK_MAC(mcRouteLttEntryPtr);

    if (ipGroupPrefixLen > 32)
    {
        return GT_OUT_OF_RANGE;
    }

    if (ipSrcPrefixLen > 32)
    {
        return GT_OUT_OF_RANGE;
    }

    /* IPv4 group prefix must be in the range 224/4 - 239/4
       therefore the first octate must start with 0xE               */
    if((ipGroupPrefixLen!=0) && (ipGroup.arIP[0] & 0xF0) != 0xE0)
        return GT_BAD_PARAM;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamAddIpMcEntry(vrId,ipGroup.arIP,ipGroupPrefixLen,
                                        ipSrc.arIP,ipSrcPrefixLen,
                                        mcRouteLttEntryPtr,NULL,override,
                                        defragmentationEnable,
                                        CPSS_IP_PROTOCOL_IPV4_E,lpmDbPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  Inorder to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntryDel
(
    IN GT_U32       lpmDBId,
    IN GT_U32       vrId,
    IN GT_IPADDR    ipGroup,
    IN GT_U32       ipGroupPrefixLen,
    IN GT_IPADDR    ipSrc,
    IN GT_U32       ipSrcPrefixLen
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    if (ipGroupPrefixLen > 32)
    {
        return GT_OUT_OF_RANGE;
    }

    if (ipSrcPrefixLen > 32)
    {
        return GT_OUT_OF_RANGE;
    }

    /* IPv4 group prefix must be in the range 224/4 - 239/4
       therefore the first octate must start with 0xE               */
    if((ipGroupPrefixLen!=0) && (ipGroup.arIP[0] & 0xF0) != 0xE0)
        return GT_BAD_PARAM;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamDeleteIpMcEntry(vrId,ipGroup.arIP,ipGroupPrefixLen,
                                           ipSrc.arIP,ipSrcPrefixLen,
                                           CPSS_IP_PROTOCOL_IPV4_E,lpmDbPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamFlushIpMcEntries(vrId,CPSS_IP_PROTOCOL_IPV4_E,
                                            lpmDbPtr,GT_FALSE);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next muticast (ipSrc,ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc,ipGroup TCAM indexes.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual router Id.
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - the ipGroup prefix Length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr                 - The next ip Group address.
*       ipGroupPrefixLenPtr        - the ipGroup prefix Length.
*       ipSrcPtr                   - The next ip Source address.
*       ipSrcPrefixLenPtr          - ipSrc prefix length.
*       mcRouteLttEntryPtr         - the LTT entry pointer pointing to the MC route.
*                                    entry associated with this MC route.
*       tcamGroupRowIndexPtr       - pointer to TCAM group row  index.
*       tcamGroupColumnIndexPtr    - pointer to TCAM group column  index.
*       tcamSrcRowIndexPtr         - pointer to TCAM source row  index.
*       tcamSrcColumnIndexPtr      - pointer to TCAM source column  index.
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1.  The value of ipGroupPtr must be a valid value, it
*           means that it exists in the IP-Mc Table, unless this is the first
*           call to this function, then it's value is 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntryGetNext
(
    IN    GT_U32                        lpmDBId,
    IN    GT_U32                        vrId,
    INOUT GT_IPADDR                     *ipGroupPtr,
    INOUT GT_U32                        *ipGroupPrefixLenPtr,
    INOUT GT_IPADDR                     *ipSrcPtr,
    INOUT GT_U32                        *ipSrcPrefixLenPtr,
    OUT   CPSS_DXCH_IP_LTT_ENTRY_STC    *mcRouteLttEntryPtr,
    OUT GT_U32                          *tcamGroupRowIndexPtr,
    OUT GT_U32                          *tcamGroupColumnIndexPtr,
    OUT GT_U32                          *tcamSrcRowIndexPtr,
    OUT GT_U32                          *tcamSrcColumnIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    GT_U8  tempIpAddr[4];
    GT_U32 ipAddrNumOfOctets;
    GT_U32 ipAddrNumOfBitsToMask;


    CPSS_NULL_PTR_CHECK_MAC(ipGroupPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipGroupPrefixLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipSrcPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipSrcPrefixLenPtr);

    CPSS_NULL_PTR_CHECK_MAC(mcRouteLttEntryPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamGroupRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamGroupColumnIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamSrcRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamSrcColumnIndexPtr);

    if (*ipGroupPrefixLenPtr > 32)
    {
        return GT_OUT_OF_RANGE;
    }

    if (*ipSrcPrefixLenPtr > 32)
    {
        return GT_OUT_OF_RANGE;
    }

    /* IPv4 group prefix must be in the range 224/4 - 239/4
       therefore the first octate must start with 0xE               */
    if((*ipGroupPrefixLenPtr!=0) && (ipGroupPtr->arIP[0] & 0xF0) != 0xE0)
        return GT_BAD_PARAM;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamGetMcEntry(vrId,ipGroupPtr->arIP,ipGroupPrefixLenPtr,
                                      ipSrcPtr->arIP,ipSrcPrefixLenPtr,
                                      mcRouteLttEntryPtr,NULL,
                                      CPSS_IP_PROTOCOL_IPV4_E,lpmDbPtr);

    if (retVal != GT_OK)
    {
        return retVal;
    }

    retVal = cpssDxChIpTcamMcRuleOffsetGet(vrId,
                                           ipGroupPtr->arIP,
                                           *ipGroupPrefixLenPtr,
                                           ipSrcPtr->arIP,
                                           *ipSrcPrefixLenPtr,
                                           CPSS_IP_PROTOCOL_IPV4_E,
                                           lpmDbPtr,
                                           tcamGroupRowIndexPtr,
                                           tcamGroupColumnIndexPtr,
                                           tcamSrcRowIndexPtr,
                                           tcamSrcColumnIndexPtr);
    if (retVal != GT_OK)
    {
        return retVal;
    }

    /* reset non relevant bit according to prefix length */

    /* IP Group */
    cpssOsMemSet(tempIpAddr,0,sizeof(tempIpAddr));
    ipAddrNumOfOctets = *ipGroupPrefixLenPtr / 8;
    ipAddrNumOfBitsToMask = *ipGroupPrefixLenPtr % 8;

    if(ipAddrNumOfBitsToMask == 0)
    {
        cpssOsMemCpy(tempIpAddr,&ipGroupPtr->arIP,sizeof(GT_U8)*ipAddrNumOfOctets);
    }
    else
    {
        cpssOsMemCpy(tempIpAddr,&ipGroupPtr->arIP,sizeof(GT_U8)*(ipAddrNumOfOctets+1));
    }

    cpssOsMemCpy(&ipGroupPtr->arIP,tempIpAddr,sizeof(ipGroupPtr->arIP));

    /* IP Source */
    cpssOsMemSet(tempIpAddr,0,sizeof(tempIpAddr));
    ipAddrNumOfOctets = *ipSrcPrefixLenPtr / 8;
    ipAddrNumOfBitsToMask = *ipSrcPrefixLenPtr % 8;

    if(ipAddrNumOfBitsToMask == 0)
    {
        cpssOsMemCpy(tempIpAddr,&ipSrcPtr->arIP,sizeof(GT_U8)*ipAddrNumOfOctets);
    }
    else
    {
        cpssOsMemCpy(tempIpAddr,&ipSrcPtr->arIP,sizeof(GT_U8)*(ipAddrNumOfOctets+1));
    }

    cpssOsMemCpy(&ipSrcPtr->arIP,tempIpAddr,sizeof(ipSrcPtr->arIP));

    return retVal;
}


/*******************************************************************************
* cpssDxChIpLpmIpv4McEntrySearch
*
* DESCRIPTION:
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc,ipGroup TCAM indexes
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId          - The LPM DB id.
*       vrId             - The virtual router Id.
*       ipGroup          - The ip Group address to get the next entry for.
*       ipGroupPrefixLen - The ip Group prefix len.
*       ipSrc            - The ip Source address to get the next entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr         - the LTT entry pointer pointing to the MC route
*                                    entry associated with this MC route.
*       tcamGroupRowIndexPtr       - pointer to TCAM group row  index.
*       tcamGroupColumnIndexPtr    - pointer to TCAM group column  index.
*       tcamSrcRowIndexPtr         - pointer to TCAM source row  index.
*       tcamSrcColumnIndexPtr      - pointer to TCAM source column  index.
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPADDR                   ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPADDR                   ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                      *tcamGroupRowIndexPtr,
    OUT GT_U32                      *tcamGroupColumnIndexPtr,
    OUT GT_U32                      *tcamSrcRowIndexPtr,
    OUT GT_U32                      *tcamSrcColumnIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    GT_U32                            *internalRuleIdxPtr = NULL;

    CPSS_NULL_PTR_CHECK_MAC(mcRouteLttEntryPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamGroupRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamGroupColumnIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamSrcRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamSrcColumnIndexPtr);

    if (ipGroupPrefixLen > 32)
    {
        return GT_OUT_OF_RANGE;
    }

    if (ipSrcPrefixLen > 32)
    {
        return GT_OUT_OF_RANGE;
    }

    /* IPv4 group prefix must be in the range 224/4 - 239/4
       therefore the first octate must start with 0xE               */
    if((ipGroupPrefixLen!=0) && (ipGroup.arIP[0] & 0xF0) != 0xE0)
        return GT_BAD_PARAM;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamSearchMcEntry(vrId,ipGroup.arIP,ipGroupPrefixLen,
                                         ipSrc.arIP,ipSrcPrefixLen,
                                         mcRouteLttEntryPtr,internalRuleIdxPtr,
                                         CPSS_IP_PROTOCOL_IPV4_E,lpmDbPtr);
    if (retVal != GT_OK)
    {
        return retVal;
    }

    retVal = cpssDxChIpTcamMcRuleOffsetGet(vrId,
                                           ipGroup.arIP,
                                           ipGroupPrefixLen,
                                           ipSrc.arIP,
                                           ipSrcPrefixLen,
                                           CPSS_IP_PROTOCOL_IPV4_E,
                                           lpmDbPtr,
                                           tcamGroupRowIndexPtr,
                                           tcamGroupColumnIndexPtr,
                                           tcamSrcRowIndexPtr,
                                           tcamSrcColumnIndexPtr);
    if (retVal != GT_OK)
    {
        return retVal;
    }

    return retVal;
}





/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv6 prefix in a Virtual Router
*   for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info accosiated with this UC prefix.
*       override        - override an existing entry for this mask if it already
*                         exists, or don't override and return error.
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If prefix length is too big.
*       GT_ERROR                 - If the vrId was not created yet.
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
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
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);

    if (prefixLen > 128)
    {
        return GT_OUT_OF_RANGE;
    }

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamAddIpUcEntry(vrId,ipAddr.arIP,prefixLen,
                                        nextHopInfoPtr,NULL,override,
                                        GT_FALSE,defragmentationEnable,
                                        CPSS_IP_PROTOCOL_IPV6_E,lpmDbPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArrayPtr - The UC prefix array.
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixAddBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC  *ipv6PrefixArrayPtr,
    IN GT_BOOL                              defragmentationEnable
)
{
    GT_STATUS retVal,retVal2;
    GT_U32 i;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    CPSS_NULL_PTR_CHECK_MAC(ipv6PrefixArrayPtr);

    retVal = GT_OK;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    for (i = 0; i < ipv6PrefixArrayLen; i++)
    {
        if (ipv6PrefixArrayPtr[i].prefixLen > 128)
        {
            return GT_OUT_OF_RANGE;
        }

        ipv6PrefixArrayPtr[i].returnStatus =
            cpssDxChIpTcamAddIpUcEntry(ipv6PrefixArrayPtr[i].vrId,
                                       ipv6PrefixArrayPtr[i].ipAddr.arIP,
                                       ipv6PrefixArrayPtr[i].prefixLen,
                                       &ipv6PrefixArrayPtr[i].nextHopInfo,
                                       NULL,ipv6PrefixArrayPtr[i].override,
                                       GT_TRUE,defragmentationEnable,
                                       CPSS_IP_PROTOCOL_IPV6_E,lpmDbPtr);
        if (ipv6PrefixArrayPtr[i].returnStatus != GT_OK)
        {
            retVal = GT_FAIL;
        }
    }

    /* now update the hw */
    retVal2 = cpssDxChIpTcamUpdateHW(lpmDbPtr,
                                     PRV_DXCH_IP_UPDATE_UC_E,
                                     CPSS_IP_PROTOCOL_IPV6_E);
    if (retVal == GT_OK)
        retVal = retVal2;


    return retVal;
}


/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv6 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH               - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixDel
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               vrId,
    IN GT_IPV6ADDR                          ipAddr,
    IN GT_U32                               prefixLen
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    if (prefixLen > 128)
    {
        return GT_OUT_OF_RANGE;
    }

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamDeleteIpUcEntry(vrId,ipAddr.arIP,prefixLen,
                                           CPSS_IP_PROTOCOL_IPV6_E,lpmDbPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixDelBulk
*
* DESCRIPTION:
*   Deletes an existing bulk of Ipv6 prefixes in a Virtual Router for the
*   specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArrayPtr - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big, or
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixDelBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC  *ipv6PrefixArrayPtr
)
{
    GT_STATUS retVal = GT_OK;
    GT_U32 i;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    if(ipv6PrefixArrayLen)
    {
        CPSS_NULL_PTR_CHECK_MAC(ipv6PrefixArrayPtr);
    }

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    for (i = 0; i < ipv6PrefixArrayLen; i++)
    {
        if (ipv6PrefixArrayPtr[i].prefixLen > 128)
        {
            return GT_OUT_OF_RANGE;
        }

        ipv6PrefixArrayPtr[i].returnStatus =
            cpssDxChIpTcamDeleteIpUcEntry(ipv6PrefixArrayPtr[i].vrId,
                                          ipv6PrefixArrayPtr[i].ipAddr.arIP,
                                          ipv6PrefixArrayPtr[i].prefixLen,
                                          CPSS_IP_PROTOCOL_IPV6_E,lpmDbPtr);
        if (ipv6PrefixArrayPtr[i].returnStatus != GT_OK)
            retVal = GT_FAIL;
    }

    return retVal;
}


/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv6 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamFlushIpUcEntries(vrId,CPSS_IP_PROTOCOL_IPV6_E,
                                            lpmDbPtr,GT_FALSE);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the next
*   hop pointer associated with it and TCAM prefix index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr  - If  found, the route entry info accosiated with
*                         this UC prefix.
*       tcamRowIndexPtr - if found, TCAM row index of this uc prefix.
*       tcamColumnIndexPtr - if found, TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixSearch
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPV6ADDR                             ipAddr,
    IN  GT_U32                                  prefixLen,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    GT_U32                           *internalRuleIdxPtr = NULL;
    GT_U32                           tcamRowIndex;
    GT_U32                           tcamColumnIndex;

    if (prefixLen > 128)
    {
        return GT_OUT_OF_RANGE;
    }
    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamColumnIndexPtr);

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }
    retVal = cpssDxChIpTcamSearchIpUcEntry(vrId,
                                           ipAddr.arIP,
                                           prefixLen,
                                           nextHopInfoPtr,
                                           internalRuleIdxPtr,
                                           CPSS_IP_PROTOCOL_IPV6_E,
                                           lpmDbPtr);
    if(retVal != GT_OK)
        return retVal;

    retVal = cpssDxChIpTcamUcRuleOffsetGet(vrId,
                                           ipAddr.arIP,
                                           prefixLen,
                                           CPSS_IP_PROTOCOL_IPV6_E,
                                           lpmDbPtr,
                                           &tcamRowIndex,
                                           &tcamColumnIndex);
    if(retVal != GT_OK)
        return retVal;

    *tcamRowIndexPtr = tcamRowIndex;
    *tcamColumnIndexPtr = tcamColumnIndex;

    return retVal;
}



/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one and prefix Tcam index; it used for iterating over the exisiting
*   prefixes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*       vrId          - The virtual router Id to get the entry from.
*       ipAddrPtr     - The ip address to start the search from.
*       prefixLenPtr  - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr       - The ip address of the found entry.
*       prefixLenPtr    - The prefix length of the found entry.
*       nextHopInfoPtr  - If found, the route entry info accosiated with
*                         this UC prefix.
*       tcamRowIndexPtr - If found, TCAM row index of this uc prefix.
*       tcamColumnIndexPtr - If found, TCAM column index of this uc prefix.
*
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixGetNext
(
    IN    GT_U32                                    lpmDBId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPV6ADDR                               *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr,
    OUT GT_U32                                      *tcamRowIndexPtr,
    OUT GT_U32                                      *tcamColumnIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    GT_U32                           *internalRuleIdxPtr = NULL;
    GT_U32                           tcamRowIndex;
    GT_U32                           tcamColumnIndex;

    GT_U8  tempIpAddr[16];
    GT_U32 ipAddrNumOfOctets;
    GT_U32 ipAddrNumOfBitsToMask;

    CPSS_NULL_PTR_CHECK_MAC(ipAddrPtr);
    CPSS_NULL_PTR_CHECK_MAC(prefixLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(nextHopInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamColumnIndexPtr);

    if (*prefixLenPtr > 128)
    {
        return GT_OUT_OF_RANGE;
    }

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamGetIpUcEntry(vrId,
                                        ipAddrPtr->arIP,
                                        prefixLenPtr,
                                        CPSS_IP_PROTOCOL_IPV6_E,
                                        nextHopInfoPtr,
                                        internalRuleIdxPtr,
                                        lpmDbPtr);
    if(retVal != GT_OK)
        return retVal;

    retVal = cpssDxChIpTcamUcRuleOffsetGet(vrId,
                                           ipAddrPtr->arIP,
                                           *prefixLenPtr,
                                           CPSS_IP_PROTOCOL_IPV6_E,
                                           lpmDbPtr,
                                           &tcamRowIndex,
                                           &tcamColumnIndex);
     if(retVal != GT_OK)
        return retVal;

     *tcamRowIndexPtr    = tcamRowIndex;
     *tcamColumnIndexPtr = tcamColumnIndex;


    /* reset non relevant bit according to prefix length */

    cpssOsMemSet(tempIpAddr,0,sizeof(tempIpAddr));
    ipAddrNumOfOctets = *prefixLenPtr / 8;
    ipAddrNumOfBitsToMask = *prefixLenPtr % 8;

    if(ipAddrNumOfBitsToMask == 0)
    {
        cpssOsMemCpy(tempIpAddr,&ipAddrPtr->arIP,sizeof(GT_U8)*ipAddrNumOfOctets);
    }
    else
    {
        cpssOsMemCpy(tempIpAddr,&ipAddrPtr->arIP,sizeof(GT_U8)*(ipAddrNumOfOctets+1));
    }

    cpssOsMemCpy(&ipAddrPtr->arIP,tempIpAddr,sizeof(ipAddrPtr->arIP));

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryAdd
*
* DESCRIPTION:
*   To add the multicast routing information for IP datagrams from a particular
*   source and addressed to a particular IP multicast group address for a
*   specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual private network identifier.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*       mcRouteLttEntryPtr - the LTT entry pointing to the MC route entry
*                            associated with this MC route.
*       override        - weather to override the mcRoutePointerPtr for the
*                         given prefix
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented .
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = ipGroupPrefixLen = 0.
*
*******************************************************************************/
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
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    CPSS_NULL_PTR_CHECK_MAC(mcRouteLttEntryPtr);

    if (ipGroupPrefixLen > 128)
    {
        return GT_OUT_OF_RANGE;
    }

    if (ipSrcPrefixLen > 128)
    {
        return GT_OUT_OF_RANGE;
    }

    /* IPv6 multicast group prefix address format ff00::/8 */
    if ((ipGroupPrefixLen!=0) && (ipGroup.arIP[0] != 0xFF))
        return GT_BAD_PARAM;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamAddIpMcEntry(vrId,ipGroup.arIP,ipGroupPrefixLen,
                                        ipSrc.arIP,ipSrcPrefixLen,
                                        mcRouteLttEntryPtr,NULL,override,
                                        defragmentationEnable,
                                        CPSS_IP_PROTOCOL_IPV6_E,lpmDbPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  Inorder to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntryDel
(
    IN GT_U32       lpmDBId,
    IN GT_U32       vrId,
    IN GT_IPV6ADDR  ipGroup,
    IN GT_U32       ipGroupPrefixLen,
    IN GT_IPV6ADDR  ipSrc,
    IN GT_U32       ipSrcPrefixLen
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    if (ipGroupPrefixLen > 128)
    {
        return GT_OUT_OF_RANGE;
    }

    if (ipSrcPrefixLen > 128)
    {
        return GT_OUT_OF_RANGE;
    }

    /* IPv6 multicast group prefix address format ff00::/8 */
    if ((ipGroupPrefixLen!=0) && (ipGroup.arIP[0] != 0xFF))
        return GT_BAD_PARAM;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamDeleteIpMcEntry(vrId,ipGroup.arIP,ipGroupPrefixLen,
                                           ipSrc.arIP,ipSrcPrefixLen,
                                           CPSS_IP_PROTOCOL_IPV6_E,lpmDbPtr);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamFlushIpMcEntries(vrId,CPSS_IP_PROTOCOL_IPV6_E,
                                            lpmDbPtr,GT_FALSE);
    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next muticast (ipSrc,ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc,ipGroup TCAM indexes.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual router Id.
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - ipGroup prefix length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr           - The next ip Group address.
*       ipGroupPrefixLenPtr  - ipGroup prefix length.
*       ipSrcPtr             - The next ip Source address.
*       ipSrcPrefixLenPtr    - ipSrc prefix length.
*       mcRouteLttEntryPtr   - the LTT entry pointer pointing to the MC route
*                              entry associated with this MC route.
*       tcamGroupRowIndexPtr - pointer to TCAM group row  index.
*       tcamSrcRowIndexPtr   - pointer to TCAM source row  index.
*
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1.  The values of (ipGroupPtr,ipGroupPrefixLenPtr) must be a valid
*           values, it means that they exist in the IP-Mc Table, unless this is
*           the first call to this function, then the value of (ipGroupPtr,
*           ipSrcPtr) is (0,0).
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntryGetNext
(
    IN    GT_U32                      lpmDBId,
    IN    GT_U32                      vrId,
    INOUT GT_IPV6ADDR                 *ipGroupPtr,
    INOUT GT_U32                      *ipGroupPrefixLenPtr,
    INOUT GT_IPV6ADDR                 *ipSrcPtr,
    INOUT GT_U32                      *ipSrcPrefixLenPtr,
    OUT   CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                        *tcamGroupRowIndexPtr,
    OUT GT_U32                        *tcamSrcRowIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    GT_U8  tempIpAddr[16];
    GT_U32 ipAddrNumOfOctets;
    GT_U32 ipAddrNumOfBitsToMask;
    GT_U32 tcamGroupColumnIndex,tcamSrcColumnIndex;

    CPSS_NULL_PTR_CHECK_MAC(ipGroupPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipGroupPrefixLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipSrcPtr);
    CPSS_NULL_PTR_CHECK_MAC(ipSrcPrefixLenPtr);
    CPSS_NULL_PTR_CHECK_MAC(mcRouteLttEntryPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamGroupRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamSrcRowIndexPtr);

    if (*ipGroupPrefixLenPtr > 128)
    {
        return GT_OUT_OF_RANGE;
    }

    if (*ipSrcPrefixLenPtr > 128)
    {
        return GT_OUT_OF_RANGE;
    }

    /* IPv6 multicast group prefix address format ff00::/8 */
    if ((*ipGroupPrefixLenPtr != 0) && (ipGroupPtr->arIP[0] != 0xFF))
        return GT_BAD_PARAM;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }
    retVal = cpssDxChIpTcamGetMcEntry(vrId,ipGroupPtr->arIP,ipGroupPrefixLenPtr,
                                      ipSrcPtr->arIP,ipSrcPrefixLenPtr,
                                      mcRouteLttEntryPtr,NULL,
                                      CPSS_IP_PROTOCOL_IPV6_E,lpmDbPtr);
    if (retVal != GT_OK)
    {
        return retVal;
    }
    retVal = cpssDxChIpTcamMcRuleOffsetGet(vrId,ipGroupPtr->arIP,*ipGroupPrefixLenPtr,
                                           ipSrcPtr->arIP,*ipSrcPrefixLenPtr,
                                           CPSS_IP_PROTOCOL_IPV6_E,lpmDbPtr,
                                           tcamGroupRowIndexPtr,
                                           &tcamGroupColumnIndex,
                                           tcamSrcRowIndexPtr,
                                           &tcamSrcColumnIndex);
    if (retVal != GT_OK)
    {
        return retVal;
    }

    /* reset non relevant bit according to prefix length */

    /* IP Group */
    cpssOsMemSet(tempIpAddr,0,sizeof(tempIpAddr));
    ipAddrNumOfOctets = *ipGroupPrefixLenPtr / 8;
    ipAddrNumOfBitsToMask = *ipGroupPrefixLenPtr % 8;

    if(ipAddrNumOfBitsToMask == 0)
    {
        cpssOsMemCpy(tempIpAddr,&ipGroupPtr->arIP,sizeof(GT_U8)*ipAddrNumOfOctets);
    }
    else
    {
        cpssOsMemCpy(tempIpAddr,&ipGroupPtr->arIP,sizeof(GT_U8)*(ipAddrNumOfOctets+1));
    }

    cpssOsMemCpy(&ipGroupPtr->arIP,tempIpAddr,sizeof(ipGroupPtr->arIP));

    /* IP Source */
    cpssOsMemSet(tempIpAddr,0,sizeof(tempIpAddr));
    ipAddrNumOfOctets = *ipSrcPrefixLenPtr / 8;
    ipAddrNumOfBitsToMask = *ipSrcPrefixLenPtr % 8;

    if(ipAddrNumOfBitsToMask == 0)
    {
        cpssOsMemCpy(tempIpAddr,&ipSrcPtr->arIP,sizeof(GT_U8)*ipAddrNumOfOctets);
    }
    else
    {
        cpssOsMemCpy(tempIpAddr,&ipSrcPtr->arIP,sizeof(GT_U8)*(ipAddrNumOfOctets+1));
    }

    cpssOsMemCpy(&ipSrcPtr->arIP,tempIpAddr,sizeof(ipSrcPtr->arIP));

    return retVal;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntrySearch
*
* DESCRIPTION:
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc,ipGroup TCAM indexes
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId           - The LPM DB id.
*       vrId              - The virtual router Id.
*       ipGroup           - The ip Group address to get the entry for.
*       ipGroupPrefixLen  - ipGroup prefix length.
*       ipSrc             - The ip Source address to get the entry for.
*       ipSrcPrefixLen    - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr   - the LTT entry pointer pointing to the MC route
*                              entry associated with this MC route.
*       tcamGroupRowIndexPtr - pointer to TCAM group row  index.
*       tcamSrcRowIndexPtr   - pointer to TCAM source row  index.
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPV6ADDR                 ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPV6ADDR                 ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                      *tcamGroupRowIndexPtr,
    OUT GT_U32                      *tcamSrcRowIndexPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *lpmDbPtr,tmpLpmDb;
    GT_U32                           *internalRuleIdxPtr = NULL;
    GT_U32                           tcamGroupColumnIndex;
    GT_U32                           tcamSrcColumnIndex;


    CPSS_NULL_PTR_CHECK_MAC(mcRouteLttEntryPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamGroupRowIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamSrcRowIndexPtr);

    if (ipGroupPrefixLen > 128)
    {
        return GT_OUT_OF_RANGE;
    }

    if (ipSrcPrefixLen > 128)
    {
        return GT_OUT_OF_RANGE;
    }

    /* IPv6 multicast group prefix address format ff00::/8 */
    if ((ipGroupPrefixLen!=0) && (ipGroup.arIP[0] != 0xFF))
        return GT_BAD_PARAM;

    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    retVal = cpssDxChIpTcamSearchMcEntry(vrId,ipGroup.arIP,ipGroupPrefixLen,
                                         ipSrc.arIP,ipSrcPrefixLen,
                                         mcRouteLttEntryPtr,internalRuleIdxPtr,
                                         CPSS_IP_PROTOCOL_IPV6_E,lpmDbPtr);
    if (retVal != GT_OK)
    {
        return retVal;
    }

    retVal = cpssDxChIpTcamMcRuleOffsetGet(vrId,
                                           ipGroup.arIP,
                                           ipGroupPrefixLen,
                                           ipSrc.arIP,
                                           ipSrcPrefixLen,
                                           CPSS_IP_PROTOCOL_IPV6_E,
                                           lpmDbPtr,
                                           tcamGroupRowIndexPtr,
                                           &tcamGroupColumnIndex,
                                           tcamSrcRowIndexPtr,
                                           &tcamSrcColumnIndex);
    return retVal;
}



/*******************************************************************************
* cpssDxChIpLpmDBMemSizeGet
*
* DESCRIPTION:
*   This function gets the memory size needed to export the Lpm DB,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       numOfEntriesInStep - how many entries in each iteration step (value > 0)
*       lpmDbSizePtr       - the table size calculated up to now (start with 0)
*       iterPtr            - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbSizePtr  - the table size calculated (in bytes)
*       iterPtr       - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK on success
*       GT_FAIL otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBMemSizeGet
(
    IN    GT_U32 lpmDBId,
    INOUT GT_U32 *lpmDbSizePtr,
    IN    GT_U32 numOfEntriesInStep,
    INOUT GT_U32 *iterPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(lpmDbSizePtr);
    CPSS_NULL_PTR_CHECK_MAC(iterPtr);

    return cpssDxChIpTcamLpmDbGetL3(lpmDBId,lpmDbSizePtr,NULL,
                                    numOfEntriesInStep,iterPtr);
}

/*******************************************************************************
* cpssDxChIpLpmDBExport
*
* DESCRIPTION:
*   This function exports the Lpm DB into the preallocated memory,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       numOfEntriesInStep - how many entries in each iteration step (value > 0)
*       iterPtr            - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbPtr  - the lpm DB info block allocated
*       iterPtr   - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK on success
*       GT_FAIL otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBExport
(
    IN    GT_U32 lpmDBId,
    OUT   GT_U32 *lpmDbPtr,
    IN    GT_U32 numOfEntriesInStep,
    INOUT GT_U32 *iterPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(lpmDbPtr);
    CPSS_NULL_PTR_CHECK_MAC(iterPtr);

    return cpssDxChIpTcamLpmDbGetL3(lpmDBId,NULL,lpmDbPtr,numOfEntriesInStep,
                                    iterPtr);
}

/*******************************************************************************
* cpssDxChIpLpmDBImport
*
* DESCRIPTION:
*   This function imports the Lpm DB recived and reconstruct it,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       numOfEntriesInStep - how many entries in each iteration step (value > 0)
*       lpmDbPtr           - the lpm DB info block
*       iterPtr            - the iterator, to start - set to 0.
*
* OUTPUTS:
*       iterPtr   - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK on success
*       GT_FAIL otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBImport
(
    IN    GT_U32 lpmDBId,
    IN    GT_U32 *lpmDbPtr,
    IN    GT_U32 numOfEntriesInStep,
    INOUT GT_U32 *iterPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(lpmDbPtr);
    CPSS_NULL_PTR_CHECK_MAC(iterPtr);

    return cpssDxChIpTcamLpmDbSetL3(lpmDBId,lpmDbPtr,numOfEntriesInStep,
                                    iterPtr);
}

/*******************************************************************************
* prvCpssPpConfigDevDbRelease
*
* DESCRIPTION:
*       private (internal) function to release the DB of LPM.
*       NOTE: function not 'free' the allocated memory. only detach from it ,
*             and restore DB to 'pre-init' state
*
*             The assumption is that the 'cpssOsMalloc' allocations will be not
*             valid any more by the application , so no need to 'free' each and
*             every allocation !
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChIpLpmDbRelease
(
    void
)
{
    /* we detach the base of the DB and application can call again the :
        create function (cpssDxChIpLpmDBCreate)
    */

    /* we not indend to 'clean the DB' and free all allocations !!! */
    prvCpssDxChIplpmDbSL = NULL;
    tcamRows = 0;

    return GT_OK;
}

