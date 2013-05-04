/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpLpmHsu.c
*
* DESCRIPTION:
*       the CPSS DXCH LPM Hitless software upgrade for Hierarchy manager .
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
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
#include <cpss/dxCh/dxChxGen/resourceManager/cpssDxChTcamManager.h>

/*******************************************************************************
 *  local vars                                                                 *
 ******************************************************************************/
extern GT_VOID *prvCpssDxChIplpmDbSL;

#define PRV_CPSS_IP_LPM_HSU_TCAM_MANAGER_ENTRY_SIZE_CNS _1K

/*******************************************************************************
* ipLpmDbGetL3TcamManager
*
* DESCRIPTION:
*       Retrieves the TCAM manager (in case of internal TCAM manager mode)
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       ipTcamShadowPtr         - (pointer to)the shadow to retrieve from iter
*       iterPtr                 - (pointer to)the current iter
*       numOfEntriesLeftPtr     - (pointer to)the num of entries left to insert
*
* OUTPUTS:
*       tableSizePtr        - (pointer to)the table size calculated (in bytes)
*       tablePtrPtr         - (pointer to)the table size info block
*       iterPtr             - (pointer to)the current iter
*       numOfEntriesLeftPtr - (pointer to)the num of entries left to insert
*
* RETURNS:
*       GT_OK           - on success
*
* COMMENTS:
*       For TCAM manager, the size of each entry is PRV_CPSS_IP_LPM_HSU_TCAM_MANAGER_ENTRY_SIZE_CNS
*       of the TCAM manager HSU data.
*
*******************************************************************************/
static GT_STATUS ipLpmDbGetL3TcamManager
(
    IN     PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *ipTcamShadowPtr,
    OUT    GT_U32                           *tableSizePtr,
    OUT    GT_VOID                          **tablePtrPtr,
    INOUT  GT_U32                           *iterPtr,
    INOUT  GT_U32                           *numOfEntriesLeftPtr
)
{
    GT_STATUS   retVal = GT_OK;
    GT_U32      numOfEntries = 0;
    GT_U32      hsuSize = 0;
    GT_U32      entrySize;
    GT_BOOL     exportCompleted = GT_FALSE;

    /* get the TCAM manager HSU size */
    retVal = cpssDxChTcamManagerHsuSizeGet(ipTcamShadowPtr->tcamManagerHandlerPtr,&hsuSize);
    if (retVal != GT_OK)
        return retVal;

    while ((numOfEntries < *numOfEntriesLeftPtr) && (exportCompleted == GT_FALSE))
    {
        entrySize = PRV_CPSS_IP_LPM_HSU_TCAM_MANAGER_ENTRY_SIZE_CNS;

        if (tableSizePtr != NULL)
        {
            if ((*tableSizePtr + entrySize) > hsuSize)
            {
                *tableSizePtr = hsuSize;
                exportCompleted = GT_TRUE;
                *iterPtr = 0;
            }
            else
            {
                *tableSizePtr += entrySize;
                *iterPtr += entrySize;
            }
        }

        if (*tablePtrPtr != NULL)
        {
            retVal = cpssDxChTcamManagerHsuExport(ipTcamShadowPtr->tcamManagerHandlerPtr,
                                                  iterPtr,
                                                  &entrySize,
                                                  *tablePtrPtr,
                                                  &exportCompleted);
            if (retVal != GT_OK)
                return retVal;

            /* advance the pointer */
            (*((GT_U8**)tablePtrPtr)) += (PRV_CPSS_IP_LPM_HSU_TCAM_MANAGER_ENTRY_SIZE_CNS - entrySize);
        }

        numOfEntries++;
    }

    /* if stage was completed, zero the iterator */
    if (exportCompleted == GT_TRUE)
        *iterPtr = 0;

    /* update number of entries left */
    *numOfEntriesLeftPtr -= numOfEntries;

    return retVal;
}

/*******************************************************************************
* ipLpmDbGetL3Vr
*
* DESCRIPTION:
*       Retrives the valid virtual routers
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr         - (pointer to)the shadow to retrive from iter
*       iterPtr                 - (pointer to)the current iter
*       numOfEntriesLeftPtr     - (pointer to)the num of entries
*                                  left to insert
*
* OUTPUTS:
*       tableSizePtr        - (pointer to)the table size calculated (in bytes)
*       tablePtrPtr         - (pointer to)the table size info block
*       iterPtr             - (pointer to)the current iter
*       numOfEntriesLeftPtr - (pointer to)the num of entries
*                             left to insert
*
* RETURNS:
*       GT_OK           - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbGetL3Vr
(
    IN     PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC   *ipTcamShadowPtr,
    OUT    GT_U32                             *tableSizePtr,
    OUT    GT_VOID                            **tablePtrPtr,
    INOUT  GT_U32                             *iterPtr,
    INOUT  GT_U32                             *numOfEntriesLeftPtr
)
{
    GT_STATUS                           retVal = GT_OK;
    GT_U32                              numOfEntries = 0;
    PRV_CPSS_DXCH_VR_TCAM_TBL_BLOCK_STC *vrEntryPtr = NULL;
    PRV_CPSS_DXCH_IP_VR_INFO_STC        *vrEntry;

    if (*iterPtr == 0)
    {
        /* this is the first call , so initialize the stage iterator */
        *iterPtr = (GT_U32)cpssOsMalloc(sizeof (GT_U32));
        if (*iterPtr == 0)
            return GT_OUT_OF_CPU_MEM;

        *((GT_U32*)*iterPtr) = 0;
    }

    /* set the pointer */
    vrEntry = (PRV_CPSS_DXCH_IP_VR_INFO_STC*)(*tablePtrPtr);

    while ((numOfEntries < *numOfEntriesLeftPtr) &&
           ((vrEntryPtr = prvCpssSlGetNext(ipTcamShadowPtr->vrSl,(GT_U32*)*iterPtr)) != NULL))
    {
        if (tableSizePtr != NULL)
        {
            *tableSizePtr += sizeof(PRV_CPSS_DXCH_IP_VR_INFO_STC);
        }
        if (*tablePtrPtr != NULL)
        {
             /* advance the pointer in case we are not in the first iteration */
             if(numOfEntries!=0)
                 vrEntry++;

             vrEntry->vrId = vrEntryPtr->vrId;
             vrEntry->isLast = GT_FALSE;

             retVal = cpssDxChIpTcamGetVirtualRouter(vrEntryPtr->vrId,ipTcamShadowPtr,
                                                     &vrEntry->supportUc[CPSS_IP_PROTOCOL_IPV4_E],
                                                     &vrEntry->defUcRouteEntrey[CPSS_IP_PROTOCOL_IPV4_E],
                                                     &vrEntry->supportUc[CPSS_IP_PROTOCOL_IPV6_E],
                                                     &vrEntry->defUcRouteEntrey[CPSS_IP_PROTOCOL_IPV6_E],
                                                     vrEntry->defUcRouteEntreyRuleIdx,
                                                     &vrEntry->supportMc[CPSS_IP_PROTOCOL_IPV4_E],
                                                     &vrEntry->defMcRouteEntrey[CPSS_IP_PROTOCOL_IPV4_E],
                                                     &vrEntry->supportMc[CPSS_IP_PROTOCOL_IPV6_E],
                                                     &vrEntry->defMcRouteEntrey[CPSS_IP_PROTOCOL_IPV6_E],
                                                     vrEntry->defMcRouteEntreyRuleIdx);
             if (retVal != GT_OK)
                 return retVal;

             /* get VR protocol stack */
             if ((vrEntry->supportUc[0] == GT_TRUE) || (vrEntry->supportMc[0] == GT_TRUE))
             {
                 if ((vrEntry->supportUc[1] == GT_TRUE) || (vrEntry->supportMc[1] == GT_TRUE))
                     vrEntry->protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
                 else
                     vrEntry->protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
             }
             else
                 vrEntry->protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        }

        numOfEntries++;
    }

    /* check if we finished iterating and verify there is place for dummy entry
       to indicate the end of VR stage */
    if ((vrEntryPtr == NULL) && (numOfEntries < *numOfEntriesLeftPtr))
    {
        /* add extra dummy entry to indicate end of VR stage */
        if (tableSizePtr != NULL)
        {
            *tableSizePtr += sizeof(PRV_CPSS_DXCH_IP_VR_INFO_STC);
        }
        if (*tablePtrPtr != NULL)
        {
            vrEntry++;
            vrEntry->isLast = GT_TRUE;
        }
        numOfEntries++;
        /* free & zero the iterator */
        cpssOsFree((GT_PTR)*iterPtr);
        *iterPtr = 0;

        retVal = GT_OK;
    }

    if (*tablePtrPtr != NULL)
    {
        /* update the pointer to after last exported entry */
        *tablePtrPtr = (GT_VOID *)(++vrEntry);
    }

    /* update number of entries left */
    *numOfEntriesLeftPtr -= numOfEntries;

    return retVal;
}


/*******************************************************************************
* ipLpmDbGetL3UcPrefix
*
* DESCRIPTION:
*       Retrives a specifc device's core ip uc prefix memory Size needed and info
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr     - (pointer to)the shadow to retrive from
*       vrId                - the virtual router id
*       protocolStack       - type of ip protocol stack to work on.
*       iterPtr             - (pointer to) the current iter
*       numOfEntriesLeftPtr - (pointer to) the num of entries left to insert
*
* OUTPUTS:
*       tableSizePtr        - (pointer to) the table size calculated (in bytes)
*       tablePtrPtr         - (pointer to) the table size info block
*       iterPtr             - (pointer to) the current iter
*       numOfEntriesLeftPtr - (pointer to) the num of entries left to insert
*
* RETURNS:
*       GT_OK           - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbGetL3UcPrefix
(
    IN     PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *ipTcamShadowPtr,
    IN     GT_U32                           vrId,
    IN     CPSS_IP_PROTOCOL_STACK_ENT       protocolStack,
    OUT    GT_U32                           *tableSizePtr,
    OUT    GT_VOID                          **tablePtrPtr,
    INOUT  GT_U32                           *iterPtr,
    INOUT  GT_U32                           *numOfEntriesLeftPtr
)
{
    GT_STATUS                               retVal = GT_OK;
    GT_U8                                   ipAddr[16];
    GT_U32                                  ipPrefix;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  currNextHopInfo;
    GT_U32                                  numOfEntries = 0;
    GT_U32                                  internalRuleIdx;

    PRV_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC *ipv6Prefix;
    PRV_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC *ipv4Prefix;

    if (*iterPtr == 0)
    {
        /* this is the first call, so initialize the stage iterator */
        *iterPtr = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) ?
            (GT_U32)cpssOsMalloc(sizeof (PRV_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC)):
            (GT_U32)cpssOsMalloc(sizeof (PRV_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC));
        ipPrefix = 0;
    }
    else
    {
        /* get the current */
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            ipv4Prefix = (PRV_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC*)(*iterPtr);
            cpssOsMemCpy(ipAddr,ipv4Prefix->ipAddr.arIP,4*sizeof(GT_U8));
            ipPrefix = ipv4Prefix->prefixLen;
        }
        else
        {
            ipv6Prefix = (PRV_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC*)(*iterPtr);
            cpssOsMemCpy(ipAddr,ipv6Prefix->ipAddr.arIP,16*sizeof(GT_U8));
            ipPrefix = ipv6Prefix->prefixLen ;
        }
    }

    /* set the pointer */
    ipv4Prefix = (PRV_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC*)(*tablePtrPtr);
    ipv6Prefix = (PRV_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC*)(*tablePtrPtr);

    retVal = cpssDxChIpTcamGetIpUcEntry(vrId,ipAddr,&ipPrefix,protocolStack,
                                        &currNextHopInfo,&internalRuleIdx,
                                        ipTcamShadowPtr);

    while (retVal == GT_OK)
    {
        if (tableSizePtr != NULL)
        {
            *tableSizePtr += (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
                sizeof(PRV_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC):
                sizeof(PRV_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC);
        }
        if (*tablePtrPtr != NULL)
        {
            /* copy the prefix info, start with ip address*/
            if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
            {
                cpssOsMemCpy(ipv4Prefix->ipAddr.arIP,ipAddr,4*sizeof(GT_U8));
                cpssOsMemCpy(&ipv4Prefix->nextHopInfo,&currNextHopInfo,
                             sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
                ipv4Prefix->prefixLen = ipPrefix;
                ipv4Prefix->vrId = vrId;
                ipv4Prefix->ruleIdx = internalRuleIdx;
                ipv4Prefix->last = GT_FALSE;

                /* advance the pointer */
                ipv4Prefix = ipv4Prefix + 1;

            }
            else
            {
                cpssOsMemCpy(ipv6Prefix->ipAddr.arIP,ipAddr,16*sizeof(GT_U8));
                cpssOsMemCpy(&ipv6Prefix->nextHopInfo,&currNextHopInfo,
                             sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
                ipv6Prefix->prefixLen = ipPrefix;
                ipv6Prefix->vrId = vrId;
                ipv6Prefix->ruleIdx = internalRuleIdx;
                ipv6Prefix->last = GT_FALSE;

                /* advance the pointer */
                ipv6Prefix = ipv6Prefix + 1;
            }
        }

        numOfEntries++;

        if (numOfEntries >= *numOfEntriesLeftPtr)
            /* we run out of entries , we'll continue later */
            break;

        retVal = cpssDxChIpTcamGetIpUcEntry(vrId,ipAddr,&ipPrefix,protocolStack,
                                            &currNextHopInfo,&internalRuleIdx,
                                            ipTcamShadowPtr);
    }

    /* check if we finished iterating and verify there is place for dummy entry
       to indicate the end of section */
    if ((retVal != GT_OK) && (numOfEntries < *numOfEntriesLeftPtr))
    {
        if (tableSizePtr != NULL)
        {
            /* make space for that the last dummy entry */
            *tableSizePtr += (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
                sizeof(PRV_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC):
                sizeof(PRV_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC);

        }
        if (*tablePtrPtr != NULL)
        {
            if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
            {
                ipv4Prefix->last = GT_TRUE;
                /* advance the pointer */
                *tablePtrPtr = (GT_VOID *)(ipv4Prefix + 1);
            }
            else
            {
                ipv6Prefix->last = GT_TRUE;
                /* advance the pointer */
                *tablePtrPtr = (GT_VOID *)(ipv6Prefix + 1);
            }
        }
        numOfEntries++;
        /* free & zero the iterator */
        cpssOsFree((GT_PTR)*iterPtr);
        *iterPtr = 0;

        retVal = GT_OK;
    }
    else
    {
        /* we didn't finish , record the currnet prefix. */
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            PRV_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC *tmpIpv4Prefix =
                (PRV_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC*)*iterPtr;
            cpssOsMemCpy(tmpIpv4Prefix->ipAddr.arIP,ipAddr,4*sizeof(GT_U8));
            tmpIpv4Prefix->prefixLen = ipPrefix;

            /* record the ptr */
            *tablePtrPtr = (GT_VOID *)ipv4Prefix;
        }
        else
        {
            PRV_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC *tmpIpv6Prefix =
                (PRV_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC*)*iterPtr;
            cpssOsMemCpy(tmpIpv6Prefix->ipAddr.arIP,ipAddr,16*sizeof(GT_U8));
            tmpIpv6Prefix->prefixLen = ipPrefix;

            /* record the ptr */
            *tablePtrPtr = (GT_VOID *)ipv6Prefix ;
        }
    }

    /* update number of entries left */
    *numOfEntriesLeftPtr -= numOfEntries;

    return retVal;
}


/*******************************************************************************
* ipLpmDbGetL3McRoutes
*
* DESCRIPTION:
*       Retrives a specifc device's core ip mc routes memory Size needed and info
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr     - (pointer to) the shadow to retrive from
*       vrId                - the virtual router id
*       protocolStack       - type of ip protocol stack to work on.
*       iterPtr             - (pointer to) the current iter
*       numOfEntriesLeftPtr - (pointer to) the num of entries left to insert
*
*
* OUTPUTS:
*       tableSizePtr        - (pointer to) the table size calculated (in bytes)
*       tablePtrPtr         - (pointer to) the table size info block
*       iterPtr             - (pointer to) the current iter
*       numOfEntriesLeftPtr - (pointer to) the num of entries left to insert
*
* RETURNS:
*       GT_OK               - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbGetL3McRoutes
(
    IN     PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC    *ipTcamShadowPtr,
    IN     GT_U32                              vrId,
    IN     CPSS_IP_PROTOCOL_STACK_ENT          protocolStack,
    OUT    GT_U32                              *tableSizePtr,
    OUT    GT_VOID                             **tablePtrPtr,
    INOUT  GT_U32                              *iterPtr,
    INOUT  GT_U32                              *numOfEntriesLeftPtr
)
{
    GT_STATUS                   retVal = GT_OK;
    GT_U8                       groupIp[16],srcAddr[16];
    GT_U32                      groupPrefix,srcPrefix;
    CPSS_DXCH_IP_LTT_ENTRY_STC  mcLttEntry;
    GT_U32                      numOfEntries = 0;
    GT_U32                      internalRuleIdx[2];

    PRV_CPSS_DXCH_IP_LPM_IPV6_MC_PREFIX_STC *ipv6McRoute;
    PRV_CPSS_DXCH_IP_LPM_IPV4_MC_PREFIX_STC *ipv4McRoute;

    if (*iterPtr == 0)
    {
        /* this is the first call , so initialize stage iterator;
           set the initial group to the default MC group */
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            *iterPtr = (GT_U32)cpssOsMalloc(sizeof(PRV_CPSS_DXCH_IP_LPM_IPV4_MC_PREFIX_STC));
            groupIp[0] = 224;
            groupPrefix = 4;

        }
        else
        {
            *iterPtr = (GT_U32)cpssOsMalloc(sizeof(PRV_CPSS_DXCH_IP_LPM_IPV6_MC_PREFIX_STC));
            groupIp[0] = 0xFF;
            groupPrefix = 8;
        }
    }
    else
    {
        /* get the current */
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            ipv4McRoute = (PRV_CPSS_DXCH_IP_LPM_IPV4_MC_PREFIX_STC*)(*iterPtr);
            cpssOsMemCpy(groupIp,ipv4McRoute->mcGroup.arIP,4*sizeof(GT_U8));
            cpssOsMemCpy(srcAddr,ipv4McRoute->ipSrc.arIP,4*sizeof(GT_U8));
            groupPrefix = ipv4McRoute->grpPrefixLen;
            srcPrefix = ipv4McRoute->srcPrefixLen;
        }
        else
        {
            ipv6McRoute = (PRV_CPSS_DXCH_IP_LPM_IPV6_MC_PREFIX_STC*)(*iterPtr);
            cpssOsMemCpy(groupIp,ipv6McRoute->mcGroup.arIP,16*sizeof(GT_U8));
            cpssOsMemCpy(srcAddr,ipv6McRoute->ipSrc.arIP,16*sizeof(GT_U8));
            groupPrefix = ipv6McRoute->grpPrefixLen;
            srcPrefix = ipv6McRoute->srcPrefixLen;
        }
    }

    /* set the ptrs */
    ipv4McRoute = (PRV_CPSS_DXCH_IP_LPM_IPV4_MC_PREFIX_STC*)*tablePtrPtr;
    ipv6McRoute = (PRV_CPSS_DXCH_IP_LPM_IPV6_MC_PREFIX_STC*)*tablePtrPtr;

    retVal = cpssDxChIpTcamGetMcEntry(vrId,groupIp,&groupPrefix,srcAddr,
                                      &srcPrefix,&mcLttEntry,internalRuleIdx,
                                      protocolStack,ipTcamShadowPtr);

    while (retVal == GT_OK)
    {
        if (tableSizePtr != NULL)
        {
            *tableSizePtr  +=
                (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
                sizeof(PRV_CPSS_DXCH_IP_LPM_IPV4_MC_PREFIX_STC):
                sizeof(PRV_CPSS_DXCH_IP_LPM_IPV6_MC_PREFIX_STC);
        }
        if (*tablePtrPtr != NULL)
        {
            if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
            {
                cpssOsMemCpy(ipv4McRoute->mcGroup.arIP,groupIp,4*sizeof(GT_U8));
                ipv4McRoute->grpPrefixLen = groupPrefix;
                cpssOsMemCpy(ipv4McRoute->ipSrc.arIP,srcAddr,4*sizeof(GT_U8));
                ipv4McRoute->srcPrefixLen = srcPrefix;
                ipv4McRoute->vrId = vrId;
                cpssOsMemCpy(&ipv4McRoute->mcLttEntry,&mcLttEntry,
                             sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
                ipv4McRoute->ruleIdx[0] = internalRuleIdx[0];
                ipv4McRoute->ruleIdx[1] = internalRuleIdx[1];
                ipv4McRoute->last = GT_FALSE;

                ipv4McRoute++;
            }
            else
            {
                cpssOsMemCpy(ipv6McRoute->mcGroup.arIP,groupIp,16*sizeof(GT_U8));
                ipv6McRoute->grpPrefixLen = groupPrefix;
                cpssOsMemCpy(ipv6McRoute->ipSrc.arIP,srcAddr,16*sizeof(GT_U8));
                ipv6McRoute->srcPrefixLen = srcPrefix;
                ipv6McRoute->vrId = vrId;
                cpssOsMemCpy(&ipv6McRoute->mcLttEntry,&mcLttEntry,
                             sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
                ipv6McRoute->ruleIdx[0] = internalRuleIdx[0];
                ipv6McRoute->ruleIdx[1] = internalRuleIdx[1];
                ipv6McRoute->last = GT_FALSE;

                ipv6McRoute++;
            }
        }

        numOfEntries++;

        if (numOfEntries >= *numOfEntriesLeftPtr)
            /* we run out of entries, we'll continue later */
            break;

        retVal = cpssDxChIpTcamGetMcEntry(vrId,groupIp,&groupPrefix,srcAddr,
                                          &srcPrefix,&mcLttEntry,internalRuleIdx,
                                          protocolStack,ipTcamShadowPtr);
    }

    /* check if we finished iterating and verify there is place for dummy entry
       to indicate the end of section */
    if ((retVal != GT_OK) && (numOfEntries < *numOfEntriesLeftPtr))
    {

        if (tableSizePtr != NULL)
        {
            /* make space for that the last dummy entry */
            *tableSizePtr  += (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
                sizeof(PRV_CPSS_DXCH_IP_LPM_IPV4_MC_PREFIX_STC):
                sizeof(PRV_CPSS_DXCH_IP_LPM_IPV6_MC_PREFIX_STC);
        }


        if (*tablePtrPtr != NULL)
        {
            /* indicate last for mc */
            if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
            {
                ipv4McRoute->last = GT_TRUE;
                *tablePtrPtr = (GT_VOID*)(ipv4McRoute + 1);
            }
            else
            {
                ipv6McRoute->last = GT_TRUE;
                *tablePtrPtr = (GT_VOID*)(ipv6McRoute + 1);
            }
        }
        numOfEntries++;
        /* free & zero the iterator */
        cpssOsFree((GT_PTR)*iterPtr);
        *iterPtr = 0;

        retVal = GT_OK;
    }
    else
    {
        /* we didn't finish , record the currnet prefix. */
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            PRV_CPSS_DXCH_IP_LPM_IPV4_MC_PREFIX_STC* tmpIpv4McRoute =
                (PRV_CPSS_DXCH_IP_LPM_IPV4_MC_PREFIX_STC*)*iterPtr;
            cpssOsMemCpy(tmpIpv4McRoute->mcGroup.arIP,groupIp,4*sizeof(GT_U8));
            tmpIpv4McRoute->grpPrefixLen = groupPrefix;
            cpssOsMemCpy(tmpIpv4McRoute->ipSrc.arIP,srcAddr,4*sizeof(GT_U8));
            tmpIpv4McRoute->srcPrefixLen = srcPrefix;

            /* record the ptr */
            *tablePtrPtr = (GT_VOID *)ipv4McRoute;
        }
        else
        {
            PRV_CPSS_DXCH_IP_LPM_IPV6_MC_PREFIX_STC* tmpIpv6McRoute =
                (PRV_CPSS_DXCH_IP_LPM_IPV6_MC_PREFIX_STC*)*iterPtr;
            cpssOsMemCpy(tmpIpv6McRoute->mcGroup.arIP,groupIp,16*sizeof(GT_U8));
            tmpIpv6McRoute->grpPrefixLen = groupPrefix;
            cpssOsMemCpy(tmpIpv6McRoute->ipSrc.arIP,srcAddr,16*sizeof(GT_U8));
            tmpIpv6McRoute->srcPrefixLen = srcPrefix;

            /* record the ptr */
            *tablePtrPtr = (GT_VOID *)ipv6McRoute;
        }
    }

    /* update number of entries left */
    *numOfEntriesLeftPtr -= numOfEntries;

    return (retVal);
}

/*******************************************************************************
* cpssDxChIpTcamLpmDbGetL3
*
* DESCRIPTION:
*       Retrieves a specific shadow's ip Table memory Size needed and info
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
*       tableSizePtr  - (pointer to) the table size calculated (in bytes)
*       tablePtr      - (pointer to) the table size info block
*       iterPtr       - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case the LPM DB uses TCAM manager and creates the TCAM manager
*       internally, then the LPM DB is also responsible for exporting and
*       importing the TCAM manager data. The TCAM manager must be imported
*       before VR, UC and MC entries.
*       If the LPM uses TCAM manager (internally or externally) then the LPM DB
*       must update the TCAM manager client callback functions.
*       Data is arranged in entries of different types in the following order:
*        - TCAM manager entries, if needed. Each TCAM manager entry is 1k
*          of the TCAM manager HSU data (last entry is up to 1k).
*        - VR entry per virtual router in the LPM DB. If the LPM DB contains
*          no VR, then this section is empty.
*        - Dummy VR entry. Note that the dummy VR entry will always exists,
*          even if there are no VR in the LPM DB
*        - For each VR in the LPM DB have the following sections. The order of
*          the VR is according to the VR ID. In case there are no VR, this
*          section will be empty.
*            - UC entry for each IPv4 UC entry other than the default.
*            - Dummy UC entry.
*            - MC entry for each IPv4 MC entry other than the default.
*            - Dummy MC entry
*            - UC entry for each IPv6 UC entry other than the default.
*            - Dummy UC entry.
*            - MC entry for each IPv6 MC entry other than the default.
*            - Dummy MC entry.
*          Note that if the LPM DB doesn't support a protocol stack then the
*          sections related to this protocol stack will be empty (will not include
*          the dummy entries as well).
*          If a VR doesn't support one of the prefix types then the section for
*          this prefix type will be empty but will include dummy.
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamLpmDbGetL3
(
    IN     GT_U32                       lpmDBId,
    OUT    GT_U32                       *tableSizePtr,
    OUT    GT_VOID                      *tablePtr,
    IN     GT_U32                       numOfEntriesInStep,
    INOUT  GT_U32                       *iterPtr
)
{
    GT_STATUS                           retVal = GT_OK;
    PRV_DXCH_IP_HSU_ITERATOR_STC        *tmpIterPtr;
    GT_U32                              currNumOfEntries;

    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC    *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_VR_TCAM_TBL_BLOCK_STC *vrEntryPtr;

    CPSS_NULL_PTR_CHECK_MAC(iterPtr);

    if (numOfEntriesInStep == 0)
        return GT_BAD_PARAM;

    /* get the LPM DB */
    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
        return GT_NOT_FOUND;

    tmpIterPtr = (PRV_DXCH_IP_HSU_ITERATOR_STC*)*iterPtr;

    if (tmpIterPtr == NULL)
    {
        /* we need to allocate the iterator */
        tmpIterPtr = (PRV_DXCH_IP_HSU_ITERATOR_STC*)cpssOsMalloc(sizeof(PRV_DXCH_IP_HSU_ITERATOR_STC));
        if (tmpIterPtr == NULL)
            return GT_OUT_OF_CPU_MEM;

        /* reset it */
        cpssOsMemSet(tmpIterPtr,0,sizeof(PRV_DXCH_IP_HSU_ITERATOR_STC));

        /* set the starting stage; if the LPM DB uses TCAM manager internally
           then first stage is TCAM manager, otherwise the first stage is VR */
        if (lpmDbPtr->isTcamManagerInternal == GT_TRUE)
            tmpIterPtr->currStage = PRV_DXCH_IP_HSU_TCAM_MANAGER_STAGE_E;
        else
            tmpIterPtr->currStage = PRV_DXCH_IP_HSU_VR_STAGE_E;

        /* set the memory position */
        tmpIterPtr->currMemPtr = tablePtr;

        /* set the first vr */
        vrEntryPtr = prvCpssSlGetNext(lpmDbPtr->vrSl,&tmpIterPtr->currVrId);

        *iterPtr = (GT_U32)tmpIterPtr;

        /* init the table size */
        if (tableSizePtr != NULL)
            *tableSizePtr = 0;
    }
    else
    {
        /* retrieve the vr */
        vrEntryPtr = prvCpssSlGet(lpmDbPtr->vrSl,tmpIterPtr->currVrId);
    }

    currNumOfEntries = numOfEntriesInStep;

    if (tmpIterPtr->currStage == PRV_DXCH_IP_HSU_TCAM_MANAGER_STAGE_E)
    {
        retVal = ipLpmDbGetL3TcamManager(lpmDbPtr,tableSizePtr,
                                         &tmpIterPtr->currMemPtr,
                                         &tmpIterPtr->currIter,
                                         &currNumOfEntries);
        if (tmpIterPtr->currIter == 0)
        {
            /* set the next stage */
            tmpIterPtr->currStage = PRV_DXCH_IP_HSU_VR_STAGE_E;
        }

        /* if there was a problem or if the number of entries in this iteration ended, return */
        if ((retVal != GT_OK) || (currNumOfEntries == 0))
        {
            return (retVal);
        }
    }

    if (tmpIterPtr->currStage == PRV_DXCH_IP_HSU_VR_STAGE_E)
    {
        retVal = ipLpmDbGetL3Vr(lpmDbPtr,tableSizePtr,
                                &tmpIterPtr->currMemPtr,
                                &tmpIterPtr->currIter,
                                &currNumOfEntries);
        /* check if we finished with the phase */
        if (tmpIterPtr->currIter == 0)
        {
            /* set the next stage */
            tmpIterPtr->currStage = PRV_DXCH_IP_HSU_UC_PREFIX_STAGE_E;
        }

        if ((retVal != GT_OK) || (currNumOfEntries == 0))
        {
            return (retVal);
        }
    }

    while ((tmpIterPtr->currProtocolStack < 2) && (vrEntryPtr != NULL))
    {
        if (lpmDbPtr->isIpVerIsInitialized[tmpIterPtr->currProtocolStack] == GT_FALSE)
        {
            tmpIterPtr->currProtocolStack++;
            continue;
        }

        while (vrEntryPtr != NULL)
        {
            if (tmpIterPtr->currStage == PRV_DXCH_IP_HSU_UC_PREFIX_STAGE_E)
            {
                retVal = ipLpmDbGetL3UcPrefix(lpmDbPtr,
                                              vrEntryPtr->vrId,
                                              tmpIterPtr->currProtocolStack,
                                              tableSizePtr,
                                              &tmpIterPtr->currMemPtr,
                                              &tmpIterPtr->currIter,
                                              &currNumOfEntries);

                /* check if we finished with the phase */
                if (tmpIterPtr->currIter == 0)
                {
                    /* set the next stage */
                    tmpIterPtr->currStage = PRV_DXCH_IP_HSU_MC_PREFIX_STAGE_E;
                }

                if ((retVal != GT_OK) || (currNumOfEntries == 0))
                {
                    return(retVal);
                }
            }

            if (tmpIterPtr->currStage == PRV_DXCH_IP_HSU_MC_PREFIX_STAGE_E)
            {
                retVal = ipLpmDbGetL3McRoutes(lpmDbPtr,
                                              vrEntryPtr->vrId,
                                              tmpIterPtr->currProtocolStack,
                                              tableSizePtr,
                                              &tmpIterPtr->currMemPtr,
                                              &tmpIterPtr->currIter,
                                              &currNumOfEntries);

                /* check if we finished with the phase */
                if (tmpIterPtr->currIter == 0)
                {
                    /* set the next stage */
                    tmpIterPtr->currStage = PRV_DXCH_IP_HSU_UC_PREFIX_STAGE_E;

                    /* this is the end for this vrid, advance to next vrid */
                     vrEntryPtr = prvCpssSlGetNext(lpmDbPtr->vrSl,&tmpIterPtr->currVrId);
                     /* if it was last vrid, go to next protocol stack and first vrid */
                     if (vrEntryPtr == NULL)
                     {
                         tmpIterPtr->currProtocolStack++;
                         tmpIterPtr->currVrId = 0;
                         vrEntryPtr = prvCpssSlGetNext(lpmDbPtr->vrSl,&tmpIterPtr->currVrId);
                         if ((retVal != GT_OK) || (currNumOfEntries == 0))
                             return (retVal);
                         /* break from the VR loop to the outer protocol stack loop */
                         break;
                     }
                }

                if ((retVal != GT_OK) || (currNumOfEntries == 0))
                {
                    return(retVal);
                }
            }
        }
    }

    /* we reached the end free the iterator */
    cpssOsFree(tmpIterPtr);
    *iterPtr = 0;

    return (GT_OK);
}

/*******************************************************************************
* ipLpmDbSetL3TcamManager
*
* DESCRIPTION:
*       Sets the TCAM manager (in case of internal TCAM manager mode)
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       ipTcamShadowPtr       - (pointer to)the shadow to retrieve from iter
*       tablePtrPtr           - (pointer to)the table size info block
*       iterPtr               - (pointer to)the current iter
*       numOfEntriesLeftPtr   - (pointer to)the num of entries left to insert
*
* OUTPUTS:
*       iterPtr               - (pointer to)the current iter
*       numOfEntriesLeftPtr   - (pointer to)the num of entries left to insert
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       For TCAM manager, the size of each entry is PRV_CPSS_IP_LPM_HSU_TCAM_MANAGER_ENTRY_SIZE_CNS
*       of the TCAM manager HSU data.
*
*******************************************************************************/
static GT_STATUS ipLpmDbSetL3TcamManager
(
    IN     PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *ipTcamShadowPtr,
    IN     GT_VOID                          **tablePtrPtr,
    INOUT  GT_U32                           *iterPtr,
    INOUT  GT_U32                           *numOfEntriesLeftPtr
)
{
    GT_STATUS   retVal = GT_OK;
    GT_U32      numOfEntries = 0;
    GT_U32      entrySize;
    GT_BOOL     importCompleted = GT_FALSE;

    while ((numOfEntries < *numOfEntriesLeftPtr) && (importCompleted == GT_FALSE))
    {
        entrySize = PRV_CPSS_IP_LPM_HSU_TCAM_MANAGER_ENTRY_SIZE_CNS;

        retVal = cpssDxChTcamManagerHsuImport(ipTcamShadowPtr->tcamManagerHandlerPtr,
                                              iterPtr,
                                              &entrySize,
                                              *tablePtrPtr,
                                              &importCompleted);
        if (retVal != GT_OK)
            return retVal;

        /* advance the pointer */
        (*((GT_U8**)tablePtrPtr)) += (PRV_CPSS_IP_LPM_HSU_TCAM_MANAGER_ENTRY_SIZE_CNS - entrySize);

        numOfEntries++;
    }

    /* if stage was completed, zero the iterator */
    if (importCompleted == GT_TRUE)
    {
        *iterPtr = 0;
    }

    /* update number of entries left */
    *numOfEntriesLeftPtr -= numOfEntries;

    return retVal;
}

/*******************************************************************************
* ipLpmDbSetL3Vr
*
* DESCRIPTION:
*       sets the valid virtual routers
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr     - (pointer to) the shadow to retrive from
*       tablePtrPtr         - (pointer to) the table size info block
*       iterPtr             - (pointer to) the current iter
*       numOfEntriesLeftPtr - (pointer to) the num of entries left to insert
*
* OUTPUTS:
*       iterPtr             - (pointer to) the current iter
*       numOfEntriesLeftPtr - (pointer to)the num of entries left to insert
*
* RETURNS:
*       GT_OK           - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbSetL3Vr
(
    IN     PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC *ipTcamShadowPtr,
    IN     GT_VOID                          **tablePtrPtr,
    INOUT  GT_U32                           *iterPtr,
    INOUT  GT_U32                           *numOfEntriesLeftPtr
)
{
    GT_STATUS                       retVal = GT_OK;
    GT_U32                          numOfEntries = 0;
    PRV_CPSS_DXCH_IP_VR_INFO_STC    *vrEntry;

    /* set the pointer */
    vrEntry = (PRV_CPSS_DXCH_IP_VR_INFO_STC*)(*tablePtrPtr);

    do
    {
        /* advance the pointer in case we are not in the first iteration */
        if (numOfEntries != 0)
            vrEntry++;

        if (vrEntry->isLast == GT_TRUE)
        {
            numOfEntries++;
            break;
        }

        retVal = cpssDxChIpTcamAddVirtualRouter(vrEntry->vrId,
                                                (vrEntry->supportUc[0] == GT_TRUE) ? &vrEntry->defUcRouteEntrey[0] : NULL,
                                                (vrEntry->supportUc[1] == GT_TRUE) ? &vrEntry->defUcRouteEntrey[1] : NULL,
                                                vrEntry->defUcRouteEntreyRuleIdx,
                                                (vrEntry->supportMc[0] == GT_TRUE) ? &vrEntry->defMcRouteEntrey[0] : NULL,
                                                (vrEntry->supportMc[1] == GT_TRUE) ? &vrEntry->defMcRouteEntrey[1] : NULL,
                                                vrEntry->defMcRouteEntreyRuleIdx,
                                                vrEntry->protocolStack,
                                                ipTcamShadowPtr);
        if (retVal != GT_OK)
            return retVal;

        numOfEntries++;

    } while (numOfEntries < *numOfEntriesLeftPtr);

    /* check if we finished iterating */
    if (vrEntry->isLast == GT_TRUE)
    {
        *iterPtr = 0;

        /* set the ptr */
        *tablePtrPtr = (GT_VOID *)(vrEntry + 1);

        retVal = GT_OK;
    }
    else
    {
        /* we didn't finish record the ptr */
        *tablePtrPtr = (GT_VOID *)vrEntry;
    }

    /* update number of entries left */
    *numOfEntriesLeftPtr -= numOfEntries;

    return retVal;
}



/*******************************************************************************
* ipLpmDbSetL3UcPrefix
*
* DESCRIPTION:
*       Set the data needed for core uc prefix IP shadow reconstruction used
*       after HSU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr     - (pointer to)the shadow to set in
*       protocolStack       - type of ip protocol stack to work on.
*       tablePtrPtr         - (pointer to)the table size info block
*       iterPtr             - (pointer to)the current iter
*       numOfEntriesLeftPtr - (pointer to)the num of entries left to insert
*
* OUTPUTS:
*       iterPtr             - (pointer to)the current iter
*       numOfEntriesLeftPtr - (pointer to)the num of entries left to insert
*
* RETURNS:
*       GT_OK           - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbSetL3UcPrefix
(
    IN     PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC   *ipTcamShadowPtr,
    IN     CPSS_IP_PROTOCOL_STACK_ENT         protocolStack,
    IN     GT_VOID                            **tablePtrPtr,
    INOUT  GT_U32                             *iterPtr,
    INOUT  GT_U32                             *numOfEntriesLeftPtr
)
{
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr;
    GT_STATUS                               retVal = GT_OK;
    GT_U8                                   *ipAddr;
    GT_U32                                  ipPrefix;
    GT_U32                                  ruleIdx;
    GT_U32                                  vrId;
    GT_U32                                  numOfEntries = 0;
    GT_BOOL                                 isLast;

    PRV_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC *ipv6Prefix;
    PRV_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC *ipv4Prefix;

    /* set the pointer */
    ipv4Prefix = (PRV_CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC*)(*tablePtrPtr);
    ipv6Prefix = (PRV_CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC*)(*tablePtrPtr);

    /* start with uc prefixes */
    isLast = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
             (ipv4Prefix->last):(ipv6Prefix->last);

    while ((!isLast) && (numOfEntries < *numOfEntriesLeftPtr))
    {
        /* copy the prefix info, start with ip address */
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            ipAddr = ipv4Prefix->ipAddr.arIP;
            ipPrefix = ipv4Prefix->prefixLen;
            nextHopInfoPtr = &ipv4Prefix->nextHopInfo;
            vrId = ipv4Prefix->vrId;
            ruleIdx = ipv4Prefix->ruleIdx;

            /* advance the pointer */
            ipv4Prefix = ipv4Prefix + 1;

            isLast = ipv4Prefix->last;
        }
        else
        {
            ipAddr = ipv6Prefix->ipAddr.arIP;
            ipPrefix = (GT_U8)ipv6Prefix->prefixLen;
            nextHopInfoPtr = &ipv6Prefix->nextHopInfo;
            vrId = ipv6Prefix->vrId;
            ruleIdx = ipv6Prefix->ruleIdx;

            /* advance the pointer */
            ipv6Prefix = ipv6Prefix + 1;

            isLast = ipv6Prefix->last;
        }

        retVal = cpssDxChIpTcamAddIpUcEntry(vrId,ipAddr,ipPrefix,nextHopInfoPtr,
                                            &ruleIdx,GT_FALSE,GT_FALSE,
                                            GT_FALSE /* defragEnable */,
                                            protocolStack,ipTcamShadowPtr);
        if (retVal != GT_OK)
        {
            return retVal;
        }

        numOfEntries++;
    }

    /* check if we finished */
    if ((isLast) && (numOfEntries < *numOfEntriesLeftPtr))
    {
        /* advance the pointer */
        *tablePtrPtr = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
            (GT_VOID *)(ipv4Prefix + 1) : (GT_VOID *)(ipv6Prefix + 1);
        numOfEntries++;

        /* just indicate we finished using a fake iterator */
        *iterPtr = 0;
    }
    else
    {
        /* just indicate we didn't finish using a fake iterator */
        *iterPtr = 0xff;

        /* and record the pointer */
        *tablePtrPtr = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
            (GT_VOID *)ipv4Prefix : (GT_VOID *)ipv6Prefix;
    }

    /* update number of entries left */
    *numOfEntriesLeftPtr -= numOfEntries;

    return (retVal);
}

/*******************************************************************************
* ipLpmDbSetL3McRoutes
*
* DESCRIPTION:
*       Set the data needed for core IP shadow reconstruction used after HSU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ipTcamShadowPtr     - (pointer to)the shadow to set in
*       protocolStack       - type of ip protocol stack to work on.
*       tablePtrPtr         - (pointer to)the table size info block
*       iterPtr             - (pointer to)the current iter
*       numOfEntriesLeftPtr - (pointer to)the num of entries left to insert
*
* OUTPUTS:
*       tablePtrPtr         - (pointer to)where we stopped in the block
*       iterPtr             - (pointer to)the current iter
*       numOfEntriesLeftPtr - (pointer to)the num of entries left to insert
*
* RETURNS:
*       GT_OK           - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS ipLpmDbSetL3McRoutes
(
    IN     PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC   *ipTcamShadowPtr,
    IN     CPSS_IP_PROTOCOL_STACK_ENT         protocolStack,
    IN     GT_VOID                            **tablePtrPtr,
    INOUT  GT_U32                             *iterPtr,
    INOUT  GT_U32                             *numOfEntriesLeftPtr
)
{
    GT_STATUS                   retVal = GT_OK;
    GT_U8                       *groupIp = NULL,*srcAddr = NULL;
    GT_U32                      groupPrefix,srcPrefix;
    GT_U32                      ruleIdxArr[2];
    GT_U32                      vrId;
    CPSS_DXCH_IP_LTT_ENTRY_STC  *mcLttEntryPtr;
    GT_U32                      numOfEntries = 0;
    GT_BOOL                     isLast;

    PRV_CPSS_DXCH_IP_LPM_IPV6_MC_PREFIX_STC *ipv6McRoute;
    PRV_CPSS_DXCH_IP_LPM_IPV4_MC_PREFIX_STC *ipv4McRoute;

    ipv4McRoute = (PRV_CPSS_DXCH_IP_LPM_IPV4_MC_PREFIX_STC*)*tablePtrPtr;
    ipv6McRoute = (PRV_CPSS_DXCH_IP_LPM_IPV6_MC_PREFIX_STC*)*tablePtrPtr;

    isLast = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
             ipv4McRoute->last:ipv6McRoute->last;

    while ((isLast == GT_FALSE) && (numOfEntries < *numOfEntriesLeftPtr))
    {
        if (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            groupIp = ipv4McRoute->mcGroup.arIP;
            groupPrefix = ipv4McRoute->grpPrefixLen;

            srcAddr = ipv4McRoute->ipSrc.arIP;
            srcPrefix = ipv4McRoute->srcPrefixLen;

            vrId = ipv4McRoute->vrId;
            mcLttEntryPtr = &ipv4McRoute->mcLttEntry;

            ruleIdxArr[0] = ipv4McRoute->ruleIdx[0];
            ruleIdxArr[1] = ipv4McRoute->ruleIdx[1];

            ipv4McRoute++;

            isLast = ipv4McRoute->last;
        }
        else
        {
            groupIp = ipv6McRoute->mcGroup.arIP;
            groupPrefix = ipv6McRoute->grpPrefixLen;

            srcAddr = ipv6McRoute->ipSrc.arIP;
            srcPrefix = ipv6McRoute->srcPrefixLen;

            vrId = ipv6McRoute->vrId;
            mcLttEntryPtr = &ipv6McRoute->mcLttEntry;
            ruleIdxArr[0] = ipv6McRoute->ruleIdx[0];
            ruleIdxArr[1] = ipv6McRoute->ruleIdx[1];

            ipv6McRoute++;

            isLast = ipv6McRoute->last;
        }

        retVal = cpssDxChIpTcamAddIpMcEntry(vrId,groupIp,groupPrefix,srcAddr,srcPrefix,
                                            mcLttEntryPtr,ruleIdxArr,GT_FALSE,GT_FALSE /* defragEnable */,
                                            protocolStack,ipTcamShadowPtr);
        if (retVal != GT_OK)
        {
            return retVal;
        }

        numOfEntries++;
    }

    /* check if we finished */
    if ((isLast == GT_TRUE) && (numOfEntries < *numOfEntriesLeftPtr))
    {
        /* indicate we finished using a fake iterator */
        *iterPtr = 0;

        /* advance the pointer */
        *tablePtrPtr = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
            (GT_VOID *)(ipv4McRoute + 1) : (GT_VOID *)(ipv6McRoute + 1);
        numOfEntries++;
    }
    else
    {
        /* indicate we didn't finish using a fake iterator */
        *iterPtr = 0xff;

        /* and record the pointer */
        *tablePtrPtr = (protocolStack == CPSS_IP_PROTOCOL_IPV4_E)?
            (GT_VOID *)ipv4McRoute : (GT_VOID *)ipv6McRoute;
    }

    /* update number of entries left */
    *numOfEntriesLeftPtr -= numOfEntries;

    return (retVal);
}


/*******************************************************************************
* cpssDxChIpTcamLpmDbSetL3
*
* DESCRIPTION:
*       Set the data needed for core IP shadow reconstruction used after HSU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       tablePtr            - (pointer to)the table size info block
*       numOfEntriesInStep  - how many entries in each iteration step (value > 0)
*       iterPtr             - the iterator, to start - set to 0.
*
* OUTPUTS:
*       iterPtr - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       Refer to cpssDxChIpTcamLpmDbGetL3.
*
*******************************************************************************/
GT_STATUS cpssDxChIpTcamLpmDbSetL3
(
    IN     GT_U32                       lpmDBId,
    IN     GT_VOID                      *tablePtr,
    IN     GT_U32                       numOfEntriesInStep,
    INOUT  GT_U32                       *iterPtr
)
{

    GT_STATUS                               retVal = GT_OK;
    PRV_DXCH_IP_HSU_ITERATOR_STC            *tmpIterPtr;
    GT_U32                                  currNumOfEntries;
    CPSS_DXCH_TCAM_MANAGER_CLIENT_FUNC_STC  clientFunc;

    PRV_CPSS_DXCH_IP_TCAM_SHADOW_STC        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_VR_TCAM_TBL_BLOCK_STC     *vrEntryPtr = NULL;

    CPSS_NULL_PTR_CHECK_MAC(tablePtr);
    CPSS_NULL_PTR_CHECK_MAC(iterPtr);

    if (numOfEntriesInStep == 0)
        return GT_BAD_PARAM;

    /* get the LPM DB */
    tmpLpmDb.lpmDBId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
        return GT_NOT_FOUND;

    tmpIterPtr = (PRV_DXCH_IP_HSU_ITERATOR_STC*)*iterPtr;

    if (tmpIterPtr == NULL)
    {
        /* we need to allocate the iterator */
        tmpIterPtr = (PRV_DXCH_IP_HSU_ITERATOR_STC*)cpssOsMalloc(sizeof(PRV_DXCH_IP_HSU_ITERATOR_STC));
        if (tmpIterPtr == NULL)
            return (GT_OUT_OF_CPU_MEM);

        /* reset it */
        cpssOsMemSet(tmpIterPtr,0,sizeof(PRV_DXCH_IP_HSU_ITERATOR_STC));

        /* set the starting stage; if the LPM DB uses TCAM manager then first
           stage is TCAM manager, otherwise the first stage if VR; note that
           if the LPM DB uses the TCAM manager externally, the LPM needs to update
           its client functions so TCAM manager stage is still needed */
        if (lpmDbPtr->tcamManagementMode == PRV_CPSS_DXCH_IP_TCAM_MANAGEMENT_TCAM_MANAGER_MODE_E)
            tmpIterPtr->currStage = PRV_DXCH_IP_HSU_TCAM_MANAGER_STAGE_E;
        else
            tmpIterPtr->currStage = PRV_DXCH_IP_HSU_VR_STAGE_E;

        /* set the memory position */
        tmpIterPtr->currMemPtr = tablePtr;

        *iterPtr = (GT_U32)tmpIterPtr;
    }
    else
    {
        /* retrieve the VR if in UC or MC stages */
        if ((tmpIterPtr->currStage == PRV_DXCH_IP_HSU_UC_PREFIX_STAGE_E) ||
            (tmpIterPtr->currStage == PRV_DXCH_IP_HSU_MC_PREFIX_STAGE_E))
        {
            vrEntryPtr = prvCpssSlGetNext(lpmDbPtr->vrSl,&tmpIterPtr->currVrId);
        }
    }

    currNumOfEntries = numOfEntriesInStep;

    if (tmpIterPtr->currStage == PRV_DXCH_IP_HSU_TCAM_MANAGER_STAGE_E)
    {
        /* the LPM DB need to import the TCAM manager data only if the LPM
           uses the TCAM manager internally */
        if (lpmDbPtr->isTcamManagerInternal == GT_TRUE)
        {
            retVal = ipLpmDbSetL3TcamManager(lpmDbPtr,
                                             &tmpIterPtr->currMemPtr,
                                             &tmpIterPtr->currIter,
                                             &currNumOfEntries);
        }
        if (tmpIterPtr->currIter == 0)
        {
            /* set the next stage */
            tmpIterPtr->currStage = PRV_DXCH_IP_HSU_VR_STAGE_E;
            /* regardless if the LPM DB uses the TCAM manager internally or
               externally, the LPM needs to update its client callback functions */
            clientFunc.moveToAnywhereFuncPtr = prvCpssDxChMoveToAnywhere;
            clientFunc.moveToLocationFuncPtr = prvCpssDxChMoveToLocation;
            retVal = cpssDxChTcamManagerClientFuncUpdate(lpmDbPtr->tcamManagerHandlerPtr,
                                                         lpmDbPtr->clientId,
                                                         &clientFunc);
            if (retVal != GT_OK)
                return retVal;
        }

        /* if there was a problem or if the number of entries in this iteration ended return */
        if ((retVal != GT_OK) || (currNumOfEntries == 0))
        {
            return(retVal);
        }
    }

    if (tmpIterPtr->currStage == PRV_DXCH_IP_HSU_VR_STAGE_E)
    {
        retVal = ipLpmDbSetL3Vr(lpmDbPtr,
                                &tmpIterPtr->currMemPtr,
                                &tmpIterPtr->currIter,
                                &currNumOfEntries);

        /* check if we finished with the phase */
        if (tmpIterPtr->currIter == 0)
        {
            /* set the next stage */
            tmpIterPtr->currStage = PRV_DXCH_IP_HSU_UC_PREFIX_STAGE_E;

            /* retrieve first VR */
            tmpIterPtr->currVrId = 0;
            vrEntryPtr = prvCpssSlGetNext(lpmDbPtr->vrSl,&tmpIterPtr->currVrId);
        }

        if ((retVal != GT_OK) || (currNumOfEntries == 0))
        {
            return retVal;
        }
    }

    while ((tmpIterPtr->currProtocolStack < 2) && (vrEntryPtr != NULL))
    {
        if (lpmDbPtr->isIpVerIsInitialized[tmpIterPtr->currProtocolStack] == GT_FALSE)
        {
            tmpIterPtr->currProtocolStack++;
            continue;
        }

        while (vrEntryPtr != NULL)
        {
            if (tmpIterPtr->currStage == PRV_DXCH_IP_HSU_UC_PREFIX_STAGE_E)
            {
                retVal = ipLpmDbSetL3UcPrefix(lpmDbPtr,
                                              tmpIterPtr->currProtocolStack,
                                              &tmpIterPtr->currMemPtr,
                                              &tmpIterPtr->currIter,
                                              &currNumOfEntries);

                /* check if we finished with the phase */
                if (tmpIterPtr->currIter == 0)
                {
                    /* set the next stage */
                    tmpIterPtr->currStage = PRV_DXCH_IP_HSU_MC_PREFIX_STAGE_E;
                }

                if ((retVal != GT_OK) || (currNumOfEntries == 0))
                {
                    return retVal;
                }
            }

            if (tmpIterPtr->currStage == PRV_DXCH_IP_HSU_MC_PREFIX_STAGE_E)
            {
                retVal = ipLpmDbSetL3McRoutes(lpmDbPtr,
                                              tmpIterPtr->currProtocolStack,
                                              &tmpIterPtr->currMemPtr,
                                              &tmpIterPtr->currIter,
                                              &currNumOfEntries);
                /* check if we finished with the phase */
                if (tmpIterPtr->currIter == 0)
                {
                    /* set the next stage */
                    tmpIterPtr->currStage = PRV_DXCH_IP_HSU_UC_PREFIX_STAGE_E;
                    /* this is the end for this vrid, advance to next vrid */
                     vrEntryPtr = prvCpssSlGetNext(lpmDbPtr->vrSl,&tmpIterPtr->currVrId);
                    /* if it was last vrid, go to next protocol stack and first vrid */
                    if (vrEntryPtr == NULL)
                    {
                        tmpIterPtr->currProtocolStack++;
                        tmpIterPtr->currVrId = 0;
                        vrEntryPtr = prvCpssSlGetNext(lpmDbPtr->vrSl,&tmpIterPtr->currVrId);
                        if ((retVal != GT_OK) || (currNumOfEntries == 0))
                            return (retVal);
                        /* break from the VR loop to the outer protocol stack loop */
                        break;
                    }
                }

                if ((retVal != GT_OK) || (currNumOfEntries == 0))
                {
                    return retVal;
                }
            }
        }
    }

    /* we reached the end free the iterator */
    cpssOsFree(tmpIterPtr);
    *iterPtr = 0;

    return (GT_OK);
}

