/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpTcamMgm.c
*
* DESCRIPTION:
*       This file includes Rules alloction manegment for TCAM IP routing
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChIpTcamMgm.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChIpPatTrie.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpmTypes.h>

#define DEBUG_IP_MGM_TRIE

#undef DEBUG_IP_MGM_TRIE

#ifdef DEBUG_IP_MGM_TRIE
extern int osPrintf(char*, ...);
#define DBG_PRINT(x) osPrintf x
#else
#define DBG_PRINT(x)
#endif

/*******************************************************************************
* cpssDxChIpTcamMgmGetRuleIdx
*
* DESCRIPTION:
*       returns the rule index in the rules array
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       pIpTcamPoolPtr   - the pool pointer
*       allocNodePtr     - the rule to get the idx for.
*
* OUTPUTS:
*       ruleIdxPtr       - the rule idx.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamMgmGetRuleIdx
(
    IN  PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC     *pIpTcamPoolPtr,
    IN  PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC    *allocNodePtr,
    OUT GT_32                                  *ruleIdxPtr
)
{

    GT_32 id;
    if (allocNodePtr >= pIpTcamPoolPtr->coreIpTcamRulesAllocArray)
    {
        id = ((GT_U32)(allocNodePtr) -
              (GT_U32)(pIpTcamPoolPtr->coreIpTcamRulesAllocArray)) /
              sizeof(PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC);
        if ((GT_U32)id < pIpTcamPoolPtr->coreIpNumOfIpTcamRules)
        {
            *ruleIdxPtr = (GT_32)id;
            DBG_PRINT(("Found rule = %d at index = %d\n", allocNodePtr->offset, *ruleIdxPtr));
            return GT_OK;
        }
    }
    return GT_OUT_OF_RANGE;
}

/*******************************************************************************
* cpssDxChIpTcamMgmGetRuleByRuleIdx
*
* DESCRIPTION:
*       returns the rule node according to the internal rule index
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       pIpTcamPoolPtr   - the pool pointer
*       ruleIdx          - the rule idx.
*
*
* OUTPUTS:
*       ruleNodePtrPtr      - the rule node of this idx.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamMgmGetRuleByRuleIdx
(
    IN  PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC     *pIpTcamPoolPtr,
    IN  GT_32                                  ruleIdx,
    OUT PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC    **ruleNodePtrPtr
)
{
    if (pIpTcamPoolPtr == NULL)
    {
        return GT_FAIL;
    }

    /* check boundries */
    if ((ruleIdx < 0) ||
        ((GT_U32)ruleIdx >= pIpTcamPoolPtr->coreIpNumOfIpTcamRules))
    {
        return GT_OUT_OF_RANGE;
    }

    /* get the rule */
    *ruleNodePtrPtr = &pIpTcamPoolPtr->coreIpTcamRulesAllocArray[ruleIdx];
    DBG_PRINT(("Found index = %d -> rule = %d\n", ruleIdx, (*ruleNodePtrPtr)->offset));
    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpTcamMgmCreate
*
* DESCRIPTION:
*       Creates a new IP TCAM rules pool manager
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       numOfIpTcamRules            - the number of ip Tcam rules.
*       segmentsArrayPtr            - the array of index segments that
*                                     represents the physical indexes of the
*                                     rules in this pool.
*       numOfSegments               - the number of segments
*       redirectSegmentsArrayPtr    - If != NULL, redirect info for policy-based
*                                     routing. Must be the same size as segmentsArray.
*       numOfRedirectSegments       - the number of redirect segments.
*       interConnectedPoolPtrPtr    - another interconnected pool pointer that
*                                     shared the rules with this pool in some
*                                     way, the way to convert the indexes is
*                                     using the interConnectedPoolIndexConFun
*                                     if = NULL then there is no inter connected
*                                     pool.
*       interConnectedPoolIndexConFun - the inter connected pool index convert
*                                       function.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       the pool pointer if succeeded,
*       NULL otherwise.
*
* COMMENTS:
*       numOfIpTcamRules != 0!
*
*******************************************************************************/
PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC* cpssDxChIpTcamMgmCreate
(
    IN GT_U32                                               numOfIpTcamRules,
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC_IDX_SEG_STC       *segmentsArrayPtr,
    IN GT_U32                                               numOfSegments,
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC_IDX_SEG_STC       *redirectSegmentsArrayPtr,
    IN GT_U32                                               numOfRedirectSegments,
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC                   **interConnectedPoolPtrPtr,
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_INDEX_CONVERT_FUN     interConnectedPoolIndexConFun
)
{
    GT_32                                           i;
    GT_U32                                          currIdx,segIdx;
    PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC              *pPool;
    PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC_IDX_SEG_STC  *currSeg;
    PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC_IDX_SEG_STC  *currRedirectSeg;
    GT_U32                                          currRedirectIdx = 0,redirectSegIdx;
    if (numOfIpTcamRules == 0)
    {
        return NULL;
    }

    pPool = cpssOsMalloc(sizeof(PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC));
    if (pPool == NULL)
    {
        return NULL;
    }
    pPool->coreIpTcamRulesAllocArray = cpssOsMalloc(sizeof(PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC)*
                                        numOfIpTcamRules);
    pPool->coreIpNumOfIpTcamRules = numOfIpTcamRules;
    pPool->coreIpTcamBitMapSize = ((numOfIpTcamRules-1) / 32)+1;
    pPool->coreIpTcamRulesAllocBitMap = cpssOsMalloc(sizeof(GT_U32)*(pPool->coreIpTcamBitMapSize +1));

    cpssOsMemSet(pPool->coreIpTcamRulesAllocBitMap,0,sizeof(GT_U32)*(pPool->coreIpTcamBitMapSize));

    /* set as allocated all the non valid ones in the last GT_U32 of the bitmap*/
    for (i = 31; i >= (GT_32)(numOfIpTcamRules%32); i--)
    {
       pPool->coreIpTcamRulesAllocBitMap[numOfIpTcamRules/32] |= 1<<i;
    }

    /* now set the indexes according the given segments */
    currSeg         = segmentsArrayPtr;
    currRedirectSeg = redirectSegmentsArrayPtr;
    currIdx         = currSeg->startIndex;
    segIdx          = 0;
    redirectSegIdx  = 0;
    if (currRedirectSeg != NULL)
    {
        currRedirectIdx = currRedirectSeg->startIndex;
    }

    /* find first non-empty segment */
    while (currSeg->numOfIndexes == 0)
    {
        currSeg++;
        currIdx = currSeg->startIndex;
        segIdx++;
    }
    /* find first non-empty redirect segment */
    if (currRedirectSeg != NULL)
    {
        while (currRedirectSeg->numOfIndexes == 0)
        {
            redirectSegIdx++;
            currRedirectSeg++;
            currRedirectIdx = currRedirectSeg->startIndex;
        }
    }

    DBG_PRINT(("Creating pool: numOFrules = %d\n",numOfIpTcamRules));
    DBG_PRINT(("Creating pool: segment [start = %d end = %d]\n",
               currSeg->startIndex, currSeg->endIndex));
    if (currRedirectSeg != NULL)
    {
        DBG_PRINT(("Using redirect pool: segment [start = %d end = %d]\n",
                   currRedirectSeg->startIndex, currRedirectSeg->endIndex));
    }
    else
    {
        DBG_PRINT(("No redirect pool for this segment\n"));
    }

    for (i = 0; i < (GT_32)(numOfIpTcamRules) ; i++)
    {
        /* save index in array */
        pPool->coreIpTcamRulesAllocArray[i].offset          = currIdx;
        pPool->coreIpTcamRulesAllocArray[i].valid           = GT_FALSE;
        pPool->coreIpTcamRulesAllocArray[i].trieNode        = NULL;
        pPool->coreIpTcamRulesAllocArray[i].redirectOffset  = currIdx;
        if (currRedirectSeg != NULL)
        {
            pPool->coreIpTcamRulesAllocArray[i].redirectOffset = currRedirectIdx;
        }

        currIdx++;
        if (currRedirectSeg != NULL)
        {
            currRedirectIdx++;
        }

        if (currIdx > currSeg->endIndex)
        {
            /* find next non-empty segment */
            do {
                segIdx++;
                /* check if we got segments left */
                if ((segIdx >= numOfSegments) && (i < (GT_32)(numOfIpTcamRules - 1)))
                {
                    return (NULL);
                }
                currSeg++;
                currIdx = currSeg->startIndex;

            } while ((currSeg->numOfIndexes == 0) && (segIdx < numOfSegments));

            if ((segIdx < numOfSegments) && (currSeg->numOfIndexes > 0))
            {
                DBG_PRINT(("Creating pool: segment [start = %d end = %d]\n",
                           currSeg->startIndex, currSeg->endIndex));
                if (currRedirectSeg == NULL)
                {
                    DBG_PRINT(("No redirect pool for this segment\n"));
                }
            }
        }

        if ((currRedirectSeg != NULL) && (currRedirectIdx > currRedirectSeg->endIndex))
        {
            /* find next non-empty redirect segment */
            do {
                redirectSegIdx++;
                /* check if we got redirect segments left */
                if ((redirectSegIdx >= numOfRedirectSegments) && (i < (GT_32)(numOfIpTcamRules - 1)))
                {
                    return (NULL);
                }
                currRedirectSeg++;
                currRedirectIdx = currRedirectSeg->startIndex;

            } while ((currRedirectSeg->numOfIndexes == 0) && (redirectSegIdx < numOfRedirectSegments));

            if ((redirectSegIdx < numOfRedirectSegments) && (currRedirectSeg->numOfIndexes > 0))
            {

                DBG_PRINT(("Using redirect pool: segment [start = %d end = %d]\n",
                            currRedirectSeg->startIndex, currRedirectSeg->endIndex));
            }
        }
    }

    pPool->coreIpTcamNumOfAlloc = 0;

    pPool->interConnectedPoolIndexConFun = interConnectedPoolIndexConFun;
    pPool->interConnectedPoolPtr = interConnectedPoolPtrPtr;

    return pPool;
}

/*******************************************************************************
* cpssDxChIpTcamMgmDestroy
*
* DESCRIPTION:
*       Destroys a IP Tcam rules pool manager
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       pIpTcamPoolPtr   - the pool pointer
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if susccessed
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamMgmDestroy
(
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC     *pIpTcamPoolPtr
)
{
    if (pIpTcamPoolPtr == NULL)
    {
        return GT_FAIL;
    }

    cpssOsFree(pIpTcamPoolPtr->coreIpTcamRulesAllocArray);
    pIpTcamPoolPtr->coreIpTcamRulesAllocArray = NULL;

    cpssOsFree(pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap);
    pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap = NULL;


    pIpTcamPoolPtr->coreIpTcamBitMapSize = 0;
    pIpTcamPoolPtr->coreIpNumOfIpTcamRules = 0;
    pIpTcamPoolPtr->coreIpTcamNumOfAlloc = 0;

    cpssOsFree(pIpTcamPoolPtr);

    return GT_OK;
}

/*******************************************************************************
* gtCoreIpWrapAllocRule
*
* DESCRIPTION:
*       Wraps a alloction procedure and returns the allocated rule
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       pIpTcamPoolPtr  - the pool pointer
*       allocWordPtr    - the allocated from bitmap word.
*       allocBit        - the bit in the bitmap word which incateds the allocation
*                       index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       the allocated node pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC* gtCoreIpWrapAllocRule
(
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC   *pIpTcamPoolPtr,
    IN GT_U32                               *allocWordPtr,
    IN GT_U32                               allocBit
)
{
    GT_U32 offset;

    offset = allocWordPtr - pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap;
    offset *= 32;

    offset += allocBit;

    /* set the bit in the bit vector */
    *allocWordPtr |=  1 << allocBit;

    pIpTcamPoolPtr->coreIpTcamRulesAllocArray[offset].valid = GT_TRUE;

    pIpTcamPoolPtr->coreIpTcamNumOfAlloc++;

    return &(pIpTcamPoolPtr->coreIpTcamRulesAllocArray[offset]);


}

/*******************************************************************************
* ipTcamMgmAllocMid
*
* DESCRIPTION:
*       allocates a rule of the pool by near middle index allocation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       pIpTcamPoolPtr  - the pool pointer
*       lowerBoundPtr   - the lower bound rule to allocate between.
*                        if = NULL the use the lowest index.
*       upperBoundPtr     - the upper bound rule to allocate between.
*                        if = NULL the use the max index.
*
* OUTPUTS:
*       allocNodePtrPtr - the allocated rule node.
*
* RETURNS:
*       GT_OK if allocation succedded.
*       GT_OUT_OF_PP_MEM - if there is no space left between the upper and lower
*                          bounds.
*       GT_OUT_OF_RANGE - if the indexes were out of range of the pool.
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ipTcamMgmAllocMid
(
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC     *pIpTcamPoolPtr,
    IN PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC    *lowerBoundPtr,
    IN PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC    *upperBoundPtr,
    OUT PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC   **allocNodePtrPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 upperWordMask,lowerWordMask;
    GT_32 i,realLowerBound,step,top,realUpperBound;
    GT_U32 *lowerWord, *upperWord,*startUpperWord,*startLowerWord,*chosenWord;

    GT_U32 conStartUpperWord,conStartLowerWord,conChosenWord;

    if (pIpTcamPoolPtr == NULL)
    {
        return GT_FAIL;
    }

    /* first convert to real indexes*/
    if (upperBoundPtr == NULL)
        realUpperBound = pIpTcamPoolPtr->coreIpNumOfIpTcamRules;
    else
    {
        rc = cpssDxChIpTcamMgmGetRuleIdx(pIpTcamPoolPtr,upperBoundPtr,&realUpperBound);
        if(rc != GT_OK)
            return rc;
    }

    if (lowerBoundPtr == NULL)
        realLowerBound = -1;
    else
    {
        rc = cpssDxChIpTcamMgmGetRuleIdx(pIpTcamPoolPtr,lowerBoundPtr,&realLowerBound);
        if(rc != GT_OK)
            return rc;
    }

    /* first check for validty */
    if (realLowerBound >= (GT_32)realUpperBound)
    {
        return GT_OUT_OF_PP_MEM;
    }
    if (realLowerBound >= (GT_32)pIpTcamPoolPtr->coreIpNumOfIpTcamRules)
    {
        return GT_OUT_OF_RANGE;
    }
    if (realLowerBound == (GT_32)(pIpTcamPoolPtr->coreIpNumOfIpTcamRules -1))
    {
        return GT_OUT_OF_PP_MEM;
    }

    /* create the masks for the upper and lower words, to restrict for the bounds
       only */
    upperWordMask = 0xFFFFFFFF << (realUpperBound % 32);
    lowerWordMask = 0xFFFFFFFF >> (32 - ((realLowerBound+1) % 32));

    lowerWord = &(pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap[realLowerBound /32]);
    upperWord = &(pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap[realUpperBound /32]);
    startUpperWord = startLowerWord = lowerWord + ((upperWord - lowerWord) /2);

    /* convert the words using the mask if neccessary */
    conStartUpperWord = *startUpperWord;
    conStartUpperWord |= (startUpperWord == upperWord)? upperWordMask:0;
    conStartUpperWord |= ((startUpperWord == lowerWord)&& (realLowerBound != -1))?
        lowerWordMask:0;


    conStartLowerWord = *startLowerWord;
    conStartLowerWord |= (startLowerWord == upperWord)? upperWordMask :0;
    conStartLowerWord |= ((startLowerWord == lowerWord)&& (realLowerBound != -1))?
        lowerWordMask :0;

    /* start the search in the middle and move both ways */
    while ((conStartUpperWord == 0xFFFFFFFF) &&
           (conStartLowerWord == 0xFFFFFFFF)
           && ((startLowerWord != lowerWord) ||
               (startUpperWord != upperWord)))
    {

        /* if it's possible to move down , move */
        if (startLowerWord != lowerWord)
            startLowerWord--;

        /* convert the words using the mask if neccessary */
        conStartLowerWord = *startLowerWord;
        conStartLowerWord |= (startLowerWord == upperWord)? upperWordMask :0;
        conStartLowerWord |= ((startLowerWord == lowerWord)&& (realLowerBound != -1))?
                             lowerWordMask :0;

        /* if it's possible to move up , move */
        if (startUpperWord != upperWord)
            startUpperWord++;

        /* convert the words using the mask if neccessary */
        conStartUpperWord = *startUpperWord;
        conStartUpperWord |= (startUpperWord == upperWord)? upperWordMask:0;
        conStartUpperWord |= ((startUpperWord == lowerWord)&& (realLowerBound != -1))?
                             lowerWordMask:0;


    }

    /* if we found a lower word with a place, start with it */
    if (conStartLowerWord != 0xFFFFFFFF)
    {
        /* set up the choosing order - top buttom*/
        step = -1;
        i = 31;
        top = -1;
        /* set up the chosen word */
        chosenWord = startLowerWord;
        conChosenWord = conStartLowerWord;
    }
    else
    {
        /* set up the choosing order - buttom up*/
        step = 1;
        i = 0;
        top = 32;
        /* set up the chosen word */
        chosenWord = startUpperWord;
        conChosenWord = conStartUpperWord;
    }

    /* now decide on the bit/ empty place */
    if (conChosenWord != 0xFFFFFFFF)
    {
        while (i != top)
        {

            if ((((conChosenWord) >> i) & 0x1) == 0)
            {
                /* found a place, wrap it up and send it home */
                *allocNodePtrPtr = gtCoreIpWrapAllocRule(pIpTcamPoolPtr,chosenWord,i);
                return GT_OK;
            }
            i = i+step;

        }
    }

    return GT_OUT_OF_PP_MEM;

}

/*******************************************************************************
* ipTcamMgmAllocMax
*
* DESCRIPTION:
*       allocates a rule of the pool by maximum index allocation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       pIpTcamPoolPtr   - the pool pointer
*       lowerBoundPtr    - the lower bound rule to allocate between.
*                        if = NULL the use the lowest index.
*       upperBoundPtr    - the upper bound rule to allocate between.
*                        if = NULL the use the max index.
*
* OUTPUTS:
*       allocNodePtrPtr - the allocated rule node.
*
* RETURNS:
*       GT_OK if allocation succedded.
*       GT_OUT_OF_PP_MEM - if there is no space left between the upper and lower
*                          bounds.
*       GT_OUT_OF_RANGE - if the indexes were out of range of the pool.
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ipTcamMgmAllocMax
(
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC     *pIpTcamPoolPtr,
    IN PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC    *lowerBoundPtr,
    IN PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC    *upperBoundPtr,
    OUT PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC   **allocNodePtrPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 upperWordMask,lowerWordMask;
    GT_32 realLowerBound,realUpperBound;
    GT_32 i;
    GT_U32 *lowerWord, *upperWord;

    GT_U32 conUpperWord;

    if (pIpTcamPoolPtr == NULL)
    {
        return GT_FAIL;
    }

    /* first convert to real indexes*/
    if (upperBoundPtr == NULL)
        realUpperBound = pIpTcamPoolPtr->coreIpNumOfIpTcamRules;
    else
    {
        rc = cpssDxChIpTcamMgmGetRuleIdx(pIpTcamPoolPtr,upperBoundPtr,&realUpperBound);
        if(rc != GT_OK)
            return rc;
    }

    if (lowerBoundPtr == NULL)
        realLowerBound = -1;
    else
    {
        rc = cpssDxChIpTcamMgmGetRuleIdx(pIpTcamPoolPtr,lowerBoundPtr,&realLowerBound);
        if(rc != GT_OK)
            return rc;
    }

    /* first check for validty */
    if (realLowerBound >= (GT_32)realUpperBound)
    {
        return GT_OUT_OF_PP_MEM;
    }
    if (realLowerBound >= (GT_32)pIpTcamPoolPtr->coreIpNumOfIpTcamRules)
    {
        return GT_OUT_OF_RANGE;
    }
    if (realLowerBound == (GT_32)(pIpTcamPoolPtr->coreIpNumOfIpTcamRules -1))
    {
        return GT_OUT_OF_PP_MEM;
    }

    /* create the masks for the upper and lower words, to restrict for the bounds
       only */
    upperWordMask = 0xFFFFFFFF << (realUpperBound % 32);
    lowerWordMask = 0xFFFFFFFF >> (32 - ((realLowerBound+1) % 32));

    lowerWord = &(pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap[realLowerBound /32]);
    upperWord = &(pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap[realUpperBound /32]);

    /* convert the words using the mask if neccessary */
    conUpperWord = *upperWord | upperWordMask;
    conUpperWord |= ((lowerWord == upperWord) && (realLowerBound != -1))?
        lowerWordMask:0;

    /* start looking for the top and down */
    while ((conUpperWord == 0xFFFFFFFF) && (upperWord != lowerWord))
    {
        if (upperWord != lowerWord)
        {
            upperWord--;
        }

        /* convert the words using the mask if neccessary */
        conUpperWord = *upperWord;
        conUpperWord |= ((lowerWord == upperWord) && (realLowerBound != -1))?
                        lowerWordMask :0;
    }


    /* check if we have an empty place */
    if (conUpperWord != 0xFFFFFFFF)
    {
        for (i = 31; i >= 0; i--)
        {
            if ((((conUpperWord) >> i) & 0x1) == 0)
            {
                /* found a place, wrap it up and send it home */
                *allocNodePtrPtr = gtCoreIpWrapAllocRule(pIpTcamPoolPtr,upperWord,i);
                return GT_OK;
            }

        }
    }

    return GT_OUT_OF_PP_MEM;
}

/*******************************************************************************
* ipTcamMgmAllocMin
*
* DESCRIPTION:
*       allocates a rule of the pool by minimum index allocation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       pIpTcamPoolPtr   - the pool pointer
*       lowerBoundPtr    - the lower bound rule to allocate between.
*                        if = NULL the use the lowest index.
*       upperBoundPtr     - the upper bound rule to allocate between.
*                        if = NULL the use the max index.
*
* OUTPUTS:
*       allocNodePtrPtr - the allocated rule node.
*
* RETURNS:
*       GT_OK if allocation succedded.
*       GT_OUT_OF_PP_MEM - if there is no space left between the upper and lower
*                          bounds.
*       GT_OUT_OF_RANGE - if the indexes were out of range of the pool.
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ipTcamMgmAllocMin
(
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC     *pIpTcamPoolPtr,
    IN PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC    *lowerBoundPtr,
    IN PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC    *upperBoundPtr,
    OUT PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC   **allocNodePtrPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 upperWordMask,lowerWordMask;
    GT_32 i,realLowerBound,realUpperBound;
    GT_U32 *lowerWord, *upperWord;

    GT_U32 conLowerWord;

    if (pIpTcamPoolPtr == NULL)
    {
        return GT_FAIL;
    }

    /* first convert to real indexes*/
    if (upperBoundPtr == NULL)
        realUpperBound = pIpTcamPoolPtr->coreIpNumOfIpTcamRules;
    else
    {
        rc = cpssDxChIpTcamMgmGetRuleIdx(pIpTcamPoolPtr,upperBoundPtr,&realUpperBound);
        if(rc != GT_OK)
            return rc;
    }

    if (lowerBoundPtr == NULL)
        realLowerBound = -1;
    else
    {
        rc = cpssDxChIpTcamMgmGetRuleIdx(pIpTcamPoolPtr,lowerBoundPtr,&realLowerBound);
        if(rc != GT_OK)
            return rc;
    }

    /* first check for validty */
    if (realLowerBound >= (GT_32)realUpperBound)
    {
        return GT_OUT_OF_PP_MEM;
    }
    if (realLowerBound >= (GT_32)pIpTcamPoolPtr->coreIpNumOfIpTcamRules)
    {
        return GT_OUT_OF_RANGE;
    }
    if (realLowerBound == (GT_32)(pIpTcamPoolPtr->coreIpNumOfIpTcamRules -1))
    {
        return GT_OUT_OF_PP_MEM;
    }

    /* create the masks for the upper and lower words, to restrict for the bounds
       only */
    upperWordMask = 0xFFFFFFFF << (realUpperBound % 32);
    lowerWordMask = (realLowerBound != -1)?
        0xFFFFFFFF >> (32 - ((realLowerBound+1) % 32)) :0;

    lowerWord = &(pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap[realLowerBound /32]);
    upperWord = &(pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap[realUpperBound /32]);

    /* convert the words using the mask if neccessary */
    conLowerWord = *lowerWord | lowerWordMask;
    conLowerWord |= (lowerWord == upperWord) ?
        upperWordMask:0;

    /* start looking for the top and down */
    while ((conLowerWord == 0xFFFFFFFF) && (upperWord != lowerWord))
    {
        if (upperWord != lowerWord)
        {
            lowerWord++;
        }

        /* convert the words using the mask if neccessary */
        conLowerWord = *lowerWord ;
        conLowerWord |= (lowerWord == upperWord) ? upperWordMask :0;
    }

    /* check if we have an empty place */
    if (conLowerWord != 0xFFFFFFFF)
    {
        for (i = 0; i < 32; i++)
        {
            if ((((conLowerWord) >> i) & 0x1) == 0)
            {
                /* found a place, wrap it up and send it home */
                *allocNodePtrPtr = gtCoreIpWrapAllocRule(pIpTcamPoolPtr,lowerWord,i);
                return GT_OK;
            }

        }
    }

    return GT_OUT_OF_PP_MEM;
}

/*******************************************************************************
* cpssDxChIpTcamMgmAlloc
*
* DESCRIPTION:
*       allocates a rule of the pool by the method given
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       pIpTcamPoolPtr   - the pool pointer
*       lowerBoundPtr    - the lower bound rule to allocate between.
*                        if = NULL the use the lowest index.
*       upperBoundPtr     - the upper bound rule to allocate between.
*                        if = NULL the use the max index.
*       allocMethod      - the allocation method in which to allocate
*
*
* OUTPUTS:
*       allocNodePtrPtr - the allocated rule node.
*
* RETURNS:
*       GT_OK if allocation succedded.
*       GT_OUT_OF_PP_MEM - if there is no space left between the upper and lower
*                          bounds.
*       GT_OUT_OF_RANGE - if the indexes were out of range of the pool.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamMgmAlloc
(
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC         *pIpTcamPoolPtr,
    IN PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC        *lowerBoundPtr,
    IN PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC        *upperBoundPtr,
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_ALLOC_METHOD_ENT allocMethod,
    OUT PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC       **allocNodePtrPtr
)
{
    GT_STATUS ret;

    DBG_PRINT(("Allocating in [start = %d end = %d] ",
               (lowerBoundPtr == NULL)?-1:lowerBoundPtr->offset,
               (upperBoundPtr == NULL)?-1:upperBoundPtr->offset));

    if (pIpTcamPoolPtr == NULL)
    {
        return GT_FAIL;
    }

    switch (allocMethod)
    {
        case PRV_CPSS_DXCH_IP_TCAM_MGM_ALLOC_MIN_E:
            DBG_PRINT((" according to MIN method\n"));
            ret = ipTcamMgmAllocMin(pIpTcamPoolPtr,lowerBoundPtr,upperBoundPtr,allocNodePtrPtr);
            break;
        case PRV_CPSS_DXCH_IP_TCAM_MGM_ALLOC_MID_E:
            DBG_PRINT((" according to MID method\n"));
            ret = ipTcamMgmAllocMid(pIpTcamPoolPtr,lowerBoundPtr,upperBoundPtr,allocNodePtrPtr);
            break;
        case PRV_CPSS_DXCH_IP_TCAM_MGM_ALLOC_MAX_E:
            DBG_PRINT((" according to MAX method\n"));
            ret = ipTcamMgmAllocMax(pIpTcamPoolPtr,lowerBoundPtr,upperBoundPtr,allocNodePtrPtr);
            break;
        default:
            ret = GT_FAIL;
    }
    return ret;
}

/*******************************************************************************
* cpssDxChIpTcamMgmAllocByGivenRule
*
* DESCRIPTION:
*       allocates a given rule.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       pIpTcamPoolPtr   - the pool pointer
*       allocRulePtr     - the rule to allocate .
*
* OUTPUTS:
*       allocNodePtrPtr  - the allocated rule node.
*
* RETURNS:
*       GT_OK if allocation succedded.
*       GT_ALREADY_EXIST         - if the rule is already allocated.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamMgmAllocByGivenRule
(
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC     *pIpTcamPoolPtr,
    IN PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC    *allocRulePtr,
    OUT PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC   **allocNodePtrPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 vecByte,vecBit;
    GT_32 index;

    rc = cpssDxChIpTcamMgmGetRuleIdx(pIpTcamPoolPtr,allocRulePtr,&index);
    if(rc != GT_OK)
        return rc;

    vecByte = (GT_U32)(index /32);
    vecBit = (GT_U32)(index % 32);

    /* first check if it's really allocated */
    if (((pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap[vecByte] >> vecBit) & 0x1) == 1)
    {
        return GT_ALREADY_EXIST;
    }

    *allocNodePtrPtr = gtCoreIpWrapAllocRule(pIpTcamPoolPtr,
                                       &(pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap[vecByte]),
                                       index%32);
    return GT_OK;

}

/*******************************************************************************
* cpssDxChIpTcamMgmFree
*
* DESCRIPTION:
*       frees a rule.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       pIpTcamPoolPtr   - the pool pointer
*       allocNodePtr     - the rule to free.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if allocation succedded.
*       GT_NOT_FOUND             - if the rule wasn't found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamMgmFree
(
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC     *pIpTcamPoolPtr,
    IN PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC    *allocNodePtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 vecByte,vecBit;
    GT_32 ruleIdx;

    DBG_PRINT(("Freeing rule = %d\n",allocNodePtr->offset));

    rc = cpssDxChIpTcamMgmGetRuleIdx(pIpTcamPoolPtr,allocNodePtr,&ruleIdx);
    if(rc != GT_OK)
        return rc;

    vecByte = (GT_U32)(ruleIdx / 32);
    vecBit = (GT_U32)(ruleIdx % 32);

    /* first check if it's really allocated */
    if (((pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap[vecByte] >> vecBit) & 0x1) == 0)
    {
        return GT_NOT_FOUND;
    }

    pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap[vecByte] &= ~( 1 << vecBit);

    allocNodePtr->trieNode = NULL;
    allocNodePtr->valid = GT_FALSE;

    pIpTcamPoolPtr->coreIpTcamNumOfAlloc--;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpTcamMgmSetRealIdxAsAlloc
*
* DESCRIPTION:
*       sets a real hw index as allocated.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       pIpTcamPoolPtr   - the pool pointer
*       hwIndex          - the hwRule index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       NULL - if the rule is already allocated or it's not in the pool
*       or the rule pointer, if we managed to allocate it.
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC *cpssDxChIpTcamMgmSetRealIdxAsAlloc
(
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC     *pIpTcamPoolPtr,
    IN GT_U32                                 hwIndex
)
{
    GT_U32 vecByte,vecBit;
    GT_32 ruleIdx;
    GT_STATUS   rc;

    PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC *allocNode;
    allocNode = &pIpTcamPoolPtr->coreIpTcamRulesAllocArray[0];

    /* search for the rule */
    while ((allocNode->offset < hwIndex) &&
           ((GT_U32)allocNode < (GT_U32)&pIpTcamPoolPtr->coreIpTcamRulesAllocArray[pIpTcamPoolPtr->coreIpTcamNumOfAlloc]))
    {
        allocNode++;
    }

    if (allocNode->offset != hwIndex)
    {
        return NULL;
    }

    DBG_PRINT(("Virtual allocation of rule index = %d\n",hwIndex));

    rc = cpssDxChIpTcamMgmGetRuleIdx(pIpTcamPoolPtr,allocNode,&ruleIdx);
    if (rc != GT_OK)
        return NULL;

    vecByte = (GT_U32)(ruleIdx / 32);
    vecBit = (GT_U32)(ruleIdx % 32);

    /* first check if it's free */
    if (((pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap[vecByte] >> vecBit) & 0x1) == 1)
    {
        return NULL;
    }

    /* set it as allocated */
    pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap[vecByte] |= ( 1 << vecBit);

    allocNode->trieNode = NULL;
    allocNode->valid = GT_TRUE;

    pIpTcamPoolPtr->coreIpTcamNumOfAlloc++;

    return allocNode;
}


/*******************************************************************************
* cpssDxChIpTcamMgmCheckAvailbleMem
*
* DESCRIPTION:
*       Checks weather this poll has the availble memory
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       pIpTcamPoolPtr   - the pool pointer
*       numOfRulesNeeded - number of tcam rules needed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if there is room
*       GT_OUT_OF_PP_MEM otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       numOfIpTcamRules != 0!
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamMgmCheckAvailbleMem
(
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC   *pIpTcamPoolPtr,
    IN GT_U32                               numOfRulesNeeded
)
{
    if (pIpTcamPoolPtr == NULL)
    {
        return GT_FAIL;
    }

    DBG_PRINT(("Available mem. : [Total = %d] [Allocated = %d] [Needed = %d] \n",
               pIpTcamPoolPtr->coreIpNumOfIpTcamRules,
               pIpTcamPoolPtr->coreIpTcamNumOfAlloc,
               numOfRulesNeeded));

    if ((pIpTcamPoolPtr->coreIpTcamNumOfAlloc + numOfRulesNeeded) <= pIpTcamPoolPtr->coreIpNumOfIpTcamRules)
    {
        return GT_OK;
    }

    return GT_OUT_OF_PP_MEM;
}

/*******************************************************************************
* cpssDxChIpTcamSetRuleData
*
* DESCRIPTION:
*       set a rule data by rule index, only for valid rules!
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       pIpTcamPoolPtr  - the pool pointer
*       ruleIdx         - the rule index (pce num)
*       ruleData        - the data to set for that rule
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK if allocation succedded.
*       GT_ERROR                 - for trying to set an invalid rule.
*       GT_OUT_OF_RANGE          - if the index is out of range of the pool.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamSetRuleData
(
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC        *pIpTcamPoolPtr,
    IN GT_U32                                    ruleIdx,
    IN GT_PTR                                    ruleData
)
{

    GT_U32 vecByte,vecBit;

    vecByte = ruleIdx /32;
    vecBit = ruleIdx % 32;

    if (pIpTcamPoolPtr == NULL)
    {
        return GT_FAIL;
    }

    /* first check if it's really allocated */
    if (((pIpTcamPoolPtr->coreIpTcamRulesAllocBitMap[vecByte] >> vecBit) & 0x1) == 0)
    {
        return (GT_STATUS)GT_ERROR;
    }

    if (pIpTcamPoolPtr->coreIpTcamRulesAllocArray[ruleIdx].trieNode->pData == NULL)
        return GT_FAIL;

    pIpTcamPoolPtr->coreIpTcamRulesAllocArray[ruleIdx].trieNode->pData = ruleData;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpTcamCompRulesPos
*
* DESCRIPTION:
*       compares two rules in terms of position
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       allocNode1Ptr   - the first rule to compare.
*       allocNode2Ptr   - the secone rule to compare.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       comapre result GT_EQUAL , GT_GREATER , or GT_SMALLER.
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_COMP_RES cpssDxChIpTcamCompRulesPos
(
    IN PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC    *allocNode1Ptr,
    IN PRV_CPSS_DXCH_IP_TCAM_RULE_NODE_STC    *allocNode2Ptr
)
{
    /* Check the null cases     */
    if ((allocNode1Ptr == NULL) && (allocNode2Ptr == NULL))
        return GT_EQUAL;
    if (allocNode1Ptr == NULL)
        return GT_SMALLER;
    if (allocNode2Ptr == NULL)
        return GT_GREATER;

    if (allocNode1Ptr->offset == allocNode2Ptr->offset)
    {
        return GT_EQUAL;
    }
    else if (allocNode1Ptr->offset > allocNode2Ptr->offset)
    {
        return GT_GREATER;
    }

    return GT_SMALLER;
}

/*******************************************************************************
* cpssDxChIpTcamMgmGetMemPoolPtr
*
* DESCRIPTION:
*       Converts VR id to Tcam manager pool pointer according to shadow type.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vrId            - index of VR.
*       protocolStack   - stack protocol (IPv4/6).
*       shadowType      - LPM DB shadow type - determines work configuration
*                         (Ch+/Ch2/Ch2 multiple Vrs)
*       vrEntryPtr      - pointer to vr management information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Pointer to tcam memory pool.
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DXCH_IP_TCAM_MGM_POOL_STC* cpssDxChIpTcamMgmGetMemPoolPtr
(
    IN GT_U32                                       vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack,
    IN CPSS_DXCH_IP_TCAM_SHADOW_TYPE_ENT            shadowType,
    IN PRV_CPSS_DXCH_IP_TCAM_MGM_MULTIPLE_POOLS_STC vrEntryPool
)
{
    GT_U8 index;

    CPSS_COVERITY_NON_ISSUE_BOOKMARK
    /* coverity[array_null] */
    if (vrEntryPool.tcamMgm == NULL)
    {
        return NULL;
    }

    switch (shadowType)
    {
    case CPSS_DXCH_IP_TCAM_CHEETAH_SHADOW_E:
    case CPSS_DXCH_IP_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
        index = IP_TCAM_MGM_POOL_PCL_TCAM_INDEX_CNS;
        break;
    case CPSS_DXCH_IP_TCAM_CHEETAH2_SHADOW_E:
        index = (GT_U8)((vrId == 0) ?
                IP_TCAM_MGM_POOL_ROUTER_TCAM_INDEX_CNS : IP_TCAM_MGM_POOL_PCL_TCAM_INDEX_CNS);
        break;
    case CPSS_DXCH_IP_TCAM_CHEETAH3_SHADOW_E:
    case CPSS_DXCH_IP_TCAM_XCAT_SHADOW_E:
        index = IP_TCAM_MGM_POOL_ROUTER_TCAM_INDEX_CNS;
        break;
    default:
        return NULL;
    }

    return vrEntryPool.tcamMgm[index][protocolStack];
}
