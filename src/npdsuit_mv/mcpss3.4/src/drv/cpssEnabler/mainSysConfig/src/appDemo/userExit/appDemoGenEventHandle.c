/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* appDemoGenEventHandle.c
*
* DESCRIPTION:
*       this library contains the implementation of the event handling functions
*       for the Gen device  , and the redirection to the GalTis Agent for
*       extra actions .
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/utils/appDemoFdbUpdateLock.h>
#include <cpss/generic/networkIf/cpssGenNetIfMii.h>

#if defined CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfMii.h>
#endif

#if defined EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdb.h>
#endif

/****************************************************************************
* Debug                                                                     *
****************************************************************************/

#define BUFF_LEN    5
#define AU_SIZE      10


#ifdef IMPL_GALTIS
extern GT_STATUS cmdGenRxPktReceive
(
    IN GT_U8      devNum,
    IN GT_U8      queueIdx,
    IN GT_U32     numOfBuff,
    IN GT_U8     *packetBuffs[],
    IN GT_U32     buffLen[],
    IN void      *rxParamsPtr
);
#endif /* IMPL_GALTIS */

#if (defined CHX_FAMILY)
    #define DXCH_CODE
#endif /* (defined CHX_FAMILY) */

#if (defined EXMXPM_FAMILY)
    #define PM_CODE
#endif /* (defined EXMXPM_FAMILY) */

#if (defined EX_FAMILY)
    #define EX_CODE
#endif /* (defined EX_FAMILY) */



/* array of CB functions to be called when AUQ message arrive */
static APP_DEMO_BRG_FDB_AUQ_FUNC    auqCbFuncArr[CPSS_FU_E + 1] = {NULL};

/* trace info for AUQ messages */
/* trace flag */
static GT_U32   traceAuq = 0;
/* trace indications */
static char* traceAuqArr[CPSS_FU_E + 1] = {
    "+",/*CPSS_NA_E*/
    "qa",/*CPSS_QA_E*//* should not be */
    "qr",/*CPSS_QR_E*/
    "-",/*CPSS_AA_E*/
    "t",/*CPSS_TA_E*/
    "sa",/*CPSS_SA_E*//* should not be */
    "qi",/*CPSS_QI_E*//* should not be */
    "f"/*CPSS_FU_E*/
};


#if defined CHX_FAMILY
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>

static  CPSS_DXCH_NET_RX_PARAMS_STC     chRxParams;
/*******************************************************************************
* appDemoGenNetRxPktHandle
*
* DESCRIPTION:
*       application routine to receive frames . -- Gen function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*     The user is responsible to free the gtBuf structures and rx data buffers.
*     The lport that needs to be transferred to the function netFreeRxGtbuf()
*     is the intLport and not srcLport.
*
*
*******************************************************************************/
static GT_STATUS appDemoNetRxPktHandleChPCI
(
    IN    GT_U8      devNum,
    IN    GT_U8      queueIdx,
    INOUT GT_U32    *numOfBuffPtr,
    OUT   GT_U8     *packetBuffs[],
    OUT   GT_U32     buffLen[],
    OUT   void     **rxParams
)
{
    GT_STATUS                       rc;


    /* get the packet from the device */
    rc = cpssDxChNetIfSdmaRxPacketGet(devNum, queueIdx, numOfBuffPtr,
                        packetBuffs, buffLen, &chRxParams);

    if (rc != GT_OK)  /* GT_NO_MORE is good and not an error !!! */
    {
        /* no need to free buffers because not got any */
        return rc;
    }
    *rxParams = &chRxParams;


    /* call GalTis Agent for counting and capturing */
#ifdef IMPL_GALTIS
    /* ignore the return code from this function because we want to keep on
       getting RX buffers , regardless to the CMD manager */
    (void) cmdGenRxPktReceive(devNum, queueIdx, *numOfBuffPtr,
                              packetBuffs, buffLen, *rxParams);
#endif /* IMPL_GALTIS */


    /* now you need to free the buffers */
    rc = cpssDxChNetIfRxBufFree(devNum, queueIdx, packetBuffs, *numOfBuffPtr);

    return rc;
}
#endif

#if defined EXMXPM_FAMILY
#include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>

static  CPSS_EXMXPM_NET_RX_PARAMS_STC     pmRxParams;

/*******************************************************************************
* appDemoNetRxPktHandlePmPCI
*
* DESCRIPTION:
*       application routine to receive frames . -- Gen function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*     The user is responsible to free the gtBuf structures and rx data buffers.
*     The lport that needs to be transferred to the function netFreeRxGtbuf()
*     is the intLport and not srcLport.
*
*
*******************************************************************************/
static GT_STATUS appDemoNetRxPktHandlePmPCI
(
    IN    GT_U8      devNum,
    IN    GT_U8      queueIdx,
    INOUT GT_U32    *numOfBuffPtr,
    OUT   GT_U8     *packetBuffs[],
    OUT   GT_U32     buffLen[],
    OUT   void     **rxParams
)
{
    GT_STATUS                       rc;

    /* get the packet from the device */
    rc = cpssExMxPmNetIfSdmaRxPacketGet(devNum, queueIdx, numOfBuffPtr,
                        packetBuffs, buffLen, &pmRxParams);

    if (rc != GT_OK)  /* GT_NO_MORE is good and not an error !!! */
    {
        /* no need to free buffers because not got any */
        return rc;
    }
    *rxParams = &pmRxParams;


    /* call GalTis Agent for counting and capturing */
#ifdef IMPL_GALTIS
    /* ignore the return code from this function because we want to keep on
       getting RX buffers , regardless to the CMD manager */
    (void) cmdGenRxPktReceive(devNum,queueIdx, *numOfBuffPtr,
                              packetBuffs, buffLen, *rxParams);
#endif /* IMPL_GALTIS */


    /* now you need to free the buffers */
    rc = cpssExMxPmNetIfSdmaRxBufFree(devNum, queueIdx, packetBuffs, *numOfBuffPtr);

    return rc;
}
#endif

#if defined EX_CODE
#include <cpss/exMx/exMxGen/networkIf/cpssExMxNetIf.h>

static CPSS_EXMX_NET_RX_PARAMS_STC     exRxParams;


/*******************************************************************************
* appDemoNetRxPktHandleExMxPCI
*
* DESCRIPTION:
*       application routine to receive frames . -- Gen function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*     The user is responsible to free the gtBuf structures and rx data buffers.
*     The lport that needs to be transferred to the function netFreeRxGtbuf()
*     is the intLport and not srcLport.
*
*
*******************************************************************************/
GT_STATUS appDemoNetRxPktHandleExPCI
(
    IN    GT_U8      devNum,
    IN    GT_U8      queueIdx,
    INOUT GT_U32    *numOfBuffPtr,
    OUT   GT_U8     *packetBuffs[],
    OUT   GT_U32     buffLen[],
    OUT   void     **rxParams
)
{
    GT_STATUS                       rc;

    /* get the packet from the device */
    rc = cpssExMxNetIfRxPacketGet(devNum, queueIdx, numOfBuffPtr,
                        packetBuffs, buffLen, &exRxParams);

    if (rc != GT_OK)  /* GT_NO_MORE is good and not an error !!! */
    {
        /* no need to free buffers because not got any */
        return rc;
    }
    *rxParams = &exRxParams;


    /* call GalTis Agent for counting and capturing */
#ifdef IMPL_GALTIS
    /* ignore the return code from this function because we want to keep on
       getting RX buffers , regardless to the CMD manager */
    (void) cmdGenRxPktReceive(devNum,queueIdx, *numOfBuffPtr,
                              packetBuffs, buffLen, *rxParams);
#endif /* IMPL_GALTIS */


    /* now you need to free the buffers */
    rc = cpssExMxNetIfRxBufFree(devNum, queueIdx, packetBuffs, *numOfBuffPtr);

    return rc;
}
#endif


/*******************************************************************************
* appDemoGenNetRxPktHandle
*
* DESCRIPTION:
*       application routine to receive frames . -- Gen function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*     The user is responsible to free the gtBuf structures and rx data buffers.
*     The lport that needs to be transferred to the function netFreeRxGtbuf()
*     is the intLport and not srcLport.
*
*
*******************************************************************************/
GT_STATUS appDemoGenNetRxPktHandle
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx
)
{
    GT_STATUS                           rc = GT_OK;
    GT_U32                              numOfBuff = BUFF_LEN;
    GT_U8*                              packetBuffs[BUFF_LEN];
    GT_U32                              buffLenArr[BUFF_LEN];
    void                               *rxParams;

    /* get the packet from the device */
    if(appDemoPpConfigList[devNum].cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
    {
        if(CPSS_IS_EXMX_FAMILY_MAC(appDemoPpConfigList[devNum].devFamily))
        {
#if defined EX_CODE
            rc = appDemoNetRxPktHandleExPCI(devNum, queueIdx, &numOfBuff,
                                packetBuffs, buffLenArr, &rxParams);
#endif /*EX_CODE*/
        }
        else if (CPSS_IS_DXCH_FAMILY_MAC(appDemoPpConfigList[devNum].devFamily))
        {
#if defined DXCH_CODE
            rc = appDemoNetRxPktHandleChPCI(devNum, queueIdx, &numOfBuff,
                                            packetBuffs, buffLenArr, &rxParams);
#endif /*DXCH_CODE*/
        }
        else if (CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[devNum].devFamily))
        {
#if defined PM_CODE
            rc = appDemoNetRxPktHandlePmPCI(devNum, queueIdx, &numOfBuff,
                                            packetBuffs, buffLenArr, &rxParams);
#endif /*PM_CODE*/
        }
        else
        {
            numOfBuff = 1;
            packetBuffs[0]=NULL;
            buffLenArr[0]=1;
            rxParams = NULL;
        }

        return rc;
    }
    else if(appDemoPpConfigList[devNum].cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_MII_E)
    {
#if defined DXCH_CODE
        rc = cpssDxChNetIfMiiRxPacketGet(devNum,queueIdx,&numOfBuff,packetBuffs,buffLenArr,&chRxParams);
        if (rc != GT_OK)  /* GT_NO_MORE is good and not an error !!! */
        {
            /* no need to free buffers because not got any */
            return rc;
        }
        rxParams = (void*)(&chRxParams);

    /* call GalTis Agent for counting and capturing */
#ifdef IMPL_GALTIS
        /* ignore the return code from this function because we want to keep on
           getting RX buffers , regardless to the CMD manager */
        (void) cmdGenRxPktReceive(devNum,queueIdx,numOfBuff,
                                  packetBuffs,buffLenArr,rxParams);
#endif /* IMPL_GALTIS */

        rc = cpssDxChNetIfMiiRxBufFree(devNum,queueIdx,packetBuffs,numOfBuff);
#else /* if not dx */
        return GT_NOT_IMPLEMENTED;
#endif /*DXCH_CODE*/
    }
    else
    {
        rc = GT_NOT_IMPLEMENTED;
    }

    return rc;
}




/*******************************************************************************/
/*******************************************************************************/
/********************************* AU MESSAGES HANDLE **************************/
/*******************************************************************************/
/*******************************************************************************/


#if defined PM_CODE
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdb.h>
#endif

#if defined DXCH_CODE
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#endif

typedef GT_STATUS fdbAuMsgBlockGetFunc(IN    GT_U8 devNum,
                                       INOUT GT_U32                      *numOfAuPtr,
                                       OUT   CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagesPtr);

typedef GT_STATUS fdbEntrySetFunc(IN GT_U8                   devNum,
                                  IN GT_U32                  messagePortGroupId,
                                  IN CPSS_MAC_ENTRY_EXT_STC *fdbEntryPtr);

typedef GT_STATUS fdbEntryDeleteFunc(IN GT_U8                        devNum,
                                     IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr);

typedef GT_STATUS fdbFromCpuAuMsgStatusGetFunc(IN  GT_U8    devNum,
                                               OUT GT_BOOL  *completedPtr,
                                               OUT GT_BOOL  *succeededPtr);

static fdbAuMsgBlockGetFunc          *cpssEnFdbBrgFdbAuMsgBlockGet = NULL;
static fdbEntrySetFunc               *cpssEnFdbBrgFdbEntrySetFunc;
static fdbEntryDeleteFunc            *cpssEnFdbBrgFdbEntryDeleteFunc;
static fdbFromCpuAuMsgStatusGetFunc  *cpssEnFdbFromCpuAuMsgStatusGetFunc;

#if defined PM_CODE

extern void         wrapCpssExMxPmBrgAuqMutexLock(void);
extern void         wrapCpssExMxPmBrgAuqMutexUnLock(void);

typedef enum
{
    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_NA_AA_FROM_CPU_E,
    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_QUERY_FROM_CPU_E,
    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_TRIGGERED_ACTION_E
}WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_ENT;

extern GT_STATUS wrapCpssExMxPmBrgFdbResolveDeviceDeadLock
(
    IN  GT_U8                                     devNum,
    IN  WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_ENT  action,
    OUT GT_BOOL                                   *repeatActionPtr
);

/*******************************************************************************
* prvFdbEntryConvertToExMxPmFormat
*
* DESCRIPTION:
*       The function converts FDB entry from general format to EXMXPM format.
*
*  APPLICABLE DEVICES:  ALL EXMXPM Devices
*
* INPUTS:
*       macEntryPtr - pointer to FDB entry with general format
*
* OUTPUTS:
*       fdbEntryPtr  - pointer to FDB entry with EXMXPM format
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_VOID prvFdbEntryConvertToExMxPmFormat
(
    IN  CPSS_MAC_ENTRY_EXT_STC      *macEntryPtr,
    OUT CPSS_EXMXPM_FDB_ENTRY_STC   *fdbEntryPtr
)
{

    switch(macEntryPtr->key.entryType)
    {
       case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
           fdbEntryPtr->key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E;
           break;
       case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
           fdbEntryPtr->key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E;
           break;
       case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
           fdbEntryPtr->key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E;
           break;
       default:
           break;
    }

    if((macEntryPtr->key.entryType == CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E) ||
       (macEntryPtr->key.entryType == CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E))
    {
        cpssOsMemCpy(fdbEntryPtr->key.key.ipMcast.sip,
                     macEntryPtr->key.key.ipMcast.sip, 4);
        cpssOsMemCpy(fdbEntryPtr->key.key.ipMcast.dip,
                     macEntryPtr->key.key.ipMcast.dip, 4);
        fdbEntryPtr->key.key.ipMcast.fId = macEntryPtr->key.key.ipMcast.vlanId;
    }
    else
    {
        cpssOsMemCpy(fdbEntryPtr->key.key.macFid.macAddr.arEther,
                     macEntryPtr->key.key.macVlan.macAddr.arEther, 6);

        fdbEntryPtr->key.key.macFid.fId = macEntryPtr->key.key.macVlan.vlanId;
    }

    osMemCpy(&(fdbEntryPtr->dstOutlif.interfaceInfo),
            &(macEntryPtr->dstInterface),
            sizeof(macEntryPtr->dstInterface));

    fdbEntryPtr->isStatic = macEntryPtr->isStatic;
    fdbEntryPtr->dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;

    switch(macEntryPtr->daCommand)
    {
       case CPSS_MAC_TABLE_FRWRD_E:
           fdbEntryPtr->daCommand = CPSS_PACKET_CMD_FORWARD_E;
           break;
       case CPSS_MAC_TABLE_DROP_E:
           fdbEntryPtr->daCommand = CPSS_PACKET_CMD_DROP_HARD_E;
           break;
       case CPSS_MAC_TABLE_INTERV_E:
       case CPSS_MAC_TABLE_CNTL_E:
           fdbEntryPtr->daCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
           break;
       case CPSS_MAC_TABLE_MIRROR_TO_CPU_E:
           fdbEntryPtr->daCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
           break;
       case CPSS_MAC_TABLE_SOFT_DROP_E:
           fdbEntryPtr->daCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
           break;
       default:
           break;
    }

    switch(macEntryPtr->saCommand)
    {
       case CPSS_MAC_TABLE_FRWRD_E:
           fdbEntryPtr->saCommand = CPSS_PACKET_CMD_FORWARD_E;
           break;
       case CPSS_MAC_TABLE_DROP_E:
           fdbEntryPtr->saCommand = CPSS_PACKET_CMD_DROP_HARD_E;
           break;
       case CPSS_MAC_TABLE_INTERV_E:
       case CPSS_MAC_TABLE_CNTL_E:
           fdbEntryPtr->saCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
           break;
       case CPSS_MAC_TABLE_MIRROR_TO_CPU_E:
           fdbEntryPtr->saCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
           break;
       case CPSS_MAC_TABLE_SOFT_DROP_E:
           fdbEntryPtr->saCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
           break;
       default:
           break;
    }

    fdbEntryPtr->daRoute = macEntryPtr->daRoute;
    fdbEntryPtr->mirrorToRxAnalyzerPortEn = macEntryPtr->mirrorToRxAnalyzerPortEn;
    fdbEntryPtr->sourceId = macEntryPtr->sourceID;
    fdbEntryPtr->userDefined = macEntryPtr->userDefined;
    fdbEntryPtr->daQosIndex = macEntryPtr->daQosIndex;
    fdbEntryPtr->saQosIndex = macEntryPtr->saQosIndex;
    fdbEntryPtr->daSecurityLevel = macEntryPtr->daSecurityLevel;
    fdbEntryPtr->saSecurityLevel = macEntryPtr->saSecurityLevel;
    fdbEntryPtr->appSpecificCpuCode = macEntryPtr->appSpecificCpuCode;

    /* The correct support of new field in generic structure is: */
    /* fdbEntryPtr->spUnknown = macEntryPtr->spUnknown;          */
    /* But the code kept backward compatible.                    */
    fdbEntryPtr->spUnknown = GT_FALSE;

}

/*******************************************************************************
* prvExMxPmConvertToAuGenFormat
*
* DESCRIPTION:
*       The function converts EXMXPM AU message to general AU message format.
*
*  APPLICABLE DEVICES:  ALL EXMXPM Devices
*
* INPUTS:
*       auMessagesPtr    - pointer to AU message in EXMXPM format
*
* OUTPUTS:
*       auMessagesGenPtr - pointer to AU message in general format
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_VOID prvExMxPmConvertToAuGenFormat
(
    IN  CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC  *auMessagesPtr,
    OUT CPSS_MAC_UPDATE_MSG_EXT_STC  *auMessagesGenPtr
)
{


    auMessagesGenPtr->updType         = auMessagesPtr->updType;
    auMessagesGenPtr->entryWasFound   = auMessagesPtr->entryWasFound;
    auMessagesGenPtr->macEntryIndex   = auMessagesPtr->fdbEntryIndex;

    switch(auMessagesPtr->fdbEntry.key.entryType)
    {
       case CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E:
           auMessagesGenPtr->macEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
           break;
       case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E:
           auMessagesGenPtr->macEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
           break;
       case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E:
           auMessagesGenPtr->macEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;
           break;
       default:
           break;
    }

    if((auMessagesPtr->fdbEntry.key.entryType ==
        CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E) ||
       (auMessagesPtr->fdbEntry.key.entryType ==
         CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E))
    {
        cpssOsMemCpy(auMessagesGenPtr->macEntry.key.key.ipMcast.sip,
                     auMessagesPtr->fdbEntry.key.key.ipMcast.sip, 4);
        cpssOsMemCpy(auMessagesGenPtr->macEntry.key.key.ipMcast.dip,
                     auMessagesPtr->fdbEntry.key.key.ipMcast.dip, 4);
        auMessagesGenPtr->macEntry.key.key.ipMcast.vlanId =
             (GT_U16)auMessagesPtr->fdbEntry.key.key.ipMcast.fId;
    }
    else
    {
        cpssOsMemCpy(auMessagesGenPtr->macEntry.key.key.macVlan.macAddr.arEther,
                     auMessagesPtr->fdbEntry.key.key.macFid.macAddr.arEther, 6);

        auMessagesGenPtr->macEntry.key.key.macVlan.vlanId =
            (GT_U16)auMessagesPtr->fdbEntry.key.key.macFid.fId;
    }



    osMemCpy(&(auMessagesGenPtr->macEntry.dstInterface),
             &(auMessagesPtr->fdbEntry.dstOutlif.interfaceInfo),
            sizeof(auMessagesPtr->fdbEntry.dstOutlif.interfaceInfo));



    auMessagesGenPtr->macEntry.isStatic = auMessagesPtr->fdbEntry.isStatic;

    switch(auMessagesPtr->fdbEntry.daCommand)
    {
       case CPSS_PACKET_CMD_FORWARD_E:
           auMessagesGenPtr->macEntry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
           break;
       case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
           auMessagesGenPtr->macEntry.daCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
           break;
       case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
           auMessagesGenPtr->macEntry.daCommand = CPSS_MAC_TABLE_CNTL_E;
           break;
       case CPSS_PACKET_CMD_DROP_HARD_E:
           auMessagesGenPtr->macEntry.daCommand = CPSS_MAC_TABLE_DROP_E;
           break;
       case CPSS_PACKET_CMD_DROP_SOFT_E:
           auMessagesGenPtr->macEntry.daCommand = CPSS_MAC_TABLE_SOFT_DROP_E;
           break;
       default:
           break;
    }


    switch(auMessagesPtr->fdbEntry.saCommand)
    {
       case CPSS_PACKET_CMD_FORWARD_E:
           auMessagesGenPtr->macEntry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
           break;
       case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
           auMessagesGenPtr->macEntry.saCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
           break;
       case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
           auMessagesGenPtr->macEntry.saCommand = CPSS_MAC_TABLE_CNTL_E;
           break;
       case CPSS_PACKET_CMD_DROP_HARD_E:
           auMessagesGenPtr->macEntry.saCommand = CPSS_MAC_TABLE_DROP_E;
           break;
       case CPSS_PACKET_CMD_DROP_SOFT_E:
           auMessagesGenPtr->macEntry.saCommand = CPSS_MAC_TABLE_SOFT_DROP_E;
           break;
       default:
           break;
    }


    auMessagesGenPtr->macEntry.daRoute = auMessagesPtr->fdbEntry.daRoute;
    auMessagesGenPtr->macEntry.mirrorToRxAnalyzerPortEn =
        auMessagesPtr->fdbEntry.mirrorToRxAnalyzerPortEn;
    auMessagesGenPtr->macEntry.sourceID = auMessagesPtr->fdbEntry.sourceId;
    auMessagesGenPtr->macEntry.userDefined = auMessagesPtr->fdbEntry.userDefined;
    auMessagesGenPtr->macEntry.daQosIndex = auMessagesPtr->fdbEntry.daQosIndex;
    auMessagesGenPtr->macEntry.saQosIndex = auMessagesPtr->fdbEntry.saQosIndex;
    auMessagesGenPtr->macEntry.daSecurityLevel =
        auMessagesPtr->fdbEntry.daSecurityLevel;
    auMessagesGenPtr->macEntry.saSecurityLevel =
        auMessagesPtr->fdbEntry.saSecurityLevel;
    auMessagesGenPtr->macEntry.appSpecificCpuCode =
        auMessagesPtr->fdbEntry.appSpecificCpuCode;

    auMessagesGenPtr->skip = auMessagesPtr->skip;
    auMessagesGenPtr->aging = auMessagesPtr->aging;
    auMessagesGenPtr->associatedDevNum = auMessagesPtr->associatedDevNum;
    auMessagesGenPtr->entryOffset = auMessagesPtr->entryOffset;


}

/*******************************************************************************
* prvExMxPmBrgFdbAuMsgBlockGet
*
* DESCRIPTION:
*       The function returns a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller.
*       The PP may place AU messages in common FDB Upload (FU) messages
*       queue or in separate queue only for AU messages. The function
*       cpssExMxPmHwPpPhase2Init configures queue for AU messages by the
*       fuqUseSeparate parameter. If common FU queue is used then function
*       returns all AU messages in the queue including FU ones.
*       If separate AU queue is used then function returns only AU messages.
*
*  APPLICABLE DEVICES:  ALL EXMXPM Devices
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE   - the action succeeded and there are no more waiting
*                      AU messages
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvExMxPmBrgFdbAuMsgBlockGet
(
    IN      GT_U8                                    devNum,
    INOUT   GT_U32                                   *numOfAuPtr,
    OUT     CPSS_MAC_UPDATE_MSG_EXT_STC              *auMessagesPtr
)
{
    GT_U32 ii;
    CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC  auPmMessagesArr[AU_SIZE];
    GT_STATUS rc;

    wrapCpssExMxPmBrgAuqMutexLock();
    rc = cpssExMxPmBrgFdbAuMsgBlockGet(devNum, numOfAuPtr, auPmMessagesArr);
    wrapCpssExMxPmBrgAuqMutexUnLock();
    if ((rc != GT_OK) && (rc != GT_NO_MORE))
    {
        return rc;
    }

    for (ii = 0; ii < *numOfAuPtr; ii++)
    {
       prvExMxPmConvertToAuGenFormat(&auPmMessagesArr[ii], &auMessagesPtr[ii]);

    }

    return rc;

}

/*******************************************************************************
* prvExMxPmBrgFdbEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware FDB table through
*       Address Update message.(AU message to the PP is non direct access to FDB
*       table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices
*
* INPUTS:
*       devNum          - device number
*       fdbEntryPtr     - pointer to FDB table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvExMxPmBrgFdbEntrySet
(
    IN GT_U8                            devNum,
    IN GT_U32                           messagePortGroupId,
    IN CPSS_MAC_ENTRY_EXT_STC           *fdbEntryPtr
)
{
    GT_STATUS       rc;
    CPSS_EXMXPM_FDB_ENTRY_STC   pmFdbEntry;
    GT_BOOL repeatAction;
    GT_U32  numRetries=10;

    messagePortGroupId = messagePortGroupId;/* ignored parameter */

    prvFdbEntryConvertToExMxPmFormat(fdbEntryPtr, &pmFdbEntry);

    wrapCpssExMxPmBrgAuqMutexLock();
    do{
        rc = cpssExMxPmBrgFdbEntrySet(devNum, &pmFdbEntry);
        wrapCpssExMxPmBrgFdbResolveDeviceDeadLock(devNum,
                    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_NA_AA_FROM_CPU_E,
                    &repeatAction);
        numRetries --;
    }while(repeatAction == GT_TRUE && numRetries);
    wrapCpssExMxPmBrgAuqMutexUnLock();

    return rc;
}


/*******************************************************************************
* prvExMxPmBrgFdbMacEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware FDB table through Address Update
*       message.(AU message to the PP is non direct access to FDB table).
*       The function use New Address message (NA) format with skip bit set to 1.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       fdbEntryKeyPtr  - pointer to key parameters of the FDB entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or vlanId
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssExMxPmBrgFdbEntrySet,
*       cpssExMxPmBrgFdbQaSend and cpssExMxPmBrgFdbEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssExMxPmBrgFdbFromCpuAuMsgStatusGet.
*
*******************************************************************************/
static GT_STATUS prvExMxPmBrgFdbMacEntryDelete
(
    IN GT_U8                            devNum,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC       *fdbEntryKeyPtr
)
{
    GT_STATUS       rc;
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC   pumaFdbEntryKey;
    GT_BOOL repeatAction;
    GT_U32  numRetries=10;

    pumaFdbEntryKey.entryType = fdbEntryKeyPtr->entryType;
    switch(fdbEntryKeyPtr->entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
            pumaFdbEntryKey.key.macFid.macAddr = fdbEntryKeyPtr->key.macVlan.macAddr;
            pumaFdbEntryKey.key.macFid.fId = fdbEntryKeyPtr->key.macVlan.vlanId;
            break;
        default:
            osMemCpy(fdbEntryKeyPtr->key.ipMcast.sip,pumaFdbEntryKey.key.ipMcast.sip,4);
            osMemCpy(fdbEntryKeyPtr->key.ipMcast.dip,pumaFdbEntryKey.key.ipMcast.dip,4);
            pumaFdbEntryKey.key.ipMcast.fId = fdbEntryKeyPtr->key.ipMcast.vlanId;
            break;
    }

    wrapCpssExMxPmBrgAuqMutexLock();
    do{
        rc = cpssExMxPmBrgFdbEntryDelete(devNum, &pumaFdbEntryKey);
        wrapCpssExMxPmBrgFdbResolveDeviceDeadLock(devNum,
                    WRAP_CPSS_EXMXPM_BRG_FDB_ACTION_TYPE_NA_AA_FROM_CPU_E,
                    &repeatAction);
        numRetries --;
    }while(repeatAction == GT_TRUE && numRetries);
    wrapCpssExMxPmBrgAuqMutexUnLock();

    return rc;
}

/*******************************************************************************
* prvExMxPmBrgFdbFromCpuAuMsgStatusGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*       The function checks the status of last AU message process:
*       completion and success.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       completedPtr - pointer to AU message processing completion.
*                      GT_TRUE - AU message was processed by PP.
*                      GT_FALSE - AU message processing is not completed yet by PP.
*       succeededPtr - pointer to a success of AU operation.
*                      GT_TRUE  - the AU action succeeded.
*                      GT_FALSE - the AU action has failed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or msgType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvExMxPmBrgFdbFromCpuAuMsgStatusGet
(
    IN  GT_U8                   devNum,
    OUT GT_BOOL                 *completedPtr,
    OUT GT_BOOL                 *succeededPtr
)
{
    return cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(devNum, CPSS_NA_E,
                                                 completedPtr, succeededPtr);

}

#endif

#if defined DXCH_CODE
GT_STATUS prvDxChBrgFdbFromCpuAuMsgStatusGet
(
    IN GT_U8    devNum,
    IN GT_BOOL  usePortGroupsBmp,
    IN GT_U32   currPortGroupsBmp
)
{
    GT_STATUS   st;
    GT_BOOL completed;
    GT_BOOL succeeded;
    GT_PORT_GROUPS_BMP completedBmp;
    GT_PORT_GROUPS_BMP succeededBmp;
    GT_PORT_GROUPS_BMP completedBmpSummary;
    GT_PORT_GROUPS_BMP succeededBmpSummary;

    if(usePortGroupsBmp == GT_FALSE)
    {
        completed = GT_FALSE;
        succeeded = GT_FALSE;

        do{
            #ifdef ASIC_SIMULATION
            osTimerWkAfter(1);
            #endif
            st = cpssDxChBrgFdbFromCpuAuMsgStatusGet(devNum, &completed, &succeeded);
            if(st != GT_OK)
            {
                goto exit_cleanly_lbl;
            }
        } while (completed == GT_FALSE);

        if(succeeded == GT_FALSE)
        {
            /* operation finished but not succeeded */
            st = GT_FAIL;
            goto exit_cleanly_lbl;
        }
    }
    else
    {
        completedBmpSummary = 0;
        succeededBmpSummary = 0;
        do{
            completedBmp = 0;
            succeededBmp = 0;
            #ifdef ASIC_SIMULATION
            osTimerWkAfter(1);
            #endif
            st = cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(devNum, currPortGroupsBmp, &completedBmp, &succeededBmp);
            if(st != GT_OK)
            {
                goto exit_cleanly_lbl;
            }

            completedBmpSummary |= completedBmp;
            succeededBmpSummary |= succeededBmp;
        }while (
            (completedBmpSummary & PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp) !=
            (currPortGroupsBmp & PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp));

        if(completedBmpSummary != succeededBmpSummary)
        {
            /* operation finished but not succeeded on all port groups */
            st = GT_FAIL;
            goto exit_cleanly_lbl;
        }
    }

exit_cleanly_lbl:

    return st;
}

/* 'mac set' entry for multi port groups device */
GT_STATUS prvDxChBrgFdbPortGroupMacEntrySet
(
    IN GT_U8                        devNum,
    IN GT_U32                       messagePortGroupId,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    GT_STATUS   rc;
    GT_PORT_GROUPS_BMP  deletedPortGroupsBmp;  /* bitmap of Port Groups - to delete the entry from */
    GT_PORT_GROUPS_BMP  portGroupsBmp;  /* bitmap of Port Groups */

    if(appDemoPpConfigList[devNum].numOfPortGroups == 0)
    {
        rc = cpssDxChBrgFdbMacEntrySet(devNum,macEntryPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc =  prvDxChBrgFdbFromCpuAuMsgStatusGet(devNum,GT_FALSE,0);
        return rc;
    }

    if(appDemoPpConfigList[devNum].fdbMode ==
        APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E)
    {
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }
    else
    {
        /* do NOT care if entry is :
            1. port or trunk
            2. or relate to local/remote device
           because this is ' controlled learn' that actually emulates 'auto learn'.
           so when message come from one of the port groups we add the entry
           to all port groups that associated with this src port group (unifiedFdbPortGroupsBmp)
        */
        if(messagePortGroupId < appDemoPpConfigList[devNum].numOfPortGroups)
        {
            portGroupsBmp =
                appDemoPpConfigList[devNum].portGroupsInfo[messagePortGroupId].unifiedFdbPortGroupsBmp;
        }
        else
        {
            /* error -- should not happen */
            portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }
    }

    rc =  cpssDxChBrgFdbPortGroupMacEntrySet(devNum,portGroupsBmp,macEntryPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(portGroupsBmp == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        rc = prvDxChBrgFdbFromCpuAuMsgStatusGet(devNum,GT_FALSE,0);
        return rc;
    }

    /* remove from the active port groups the port groups that we added the entry
       to them */
    deletedPortGroupsBmp =
        PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp & (~portGroupsBmp);

    /* we need to delete the mac entry from other port groups that it may be in
       (station movement) */
    rc = cpssDxChBrgFdbPortGroupMacEntryDelete(devNum,deletedPortGroupsBmp,&macEntryPtr->key);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* wait for finish on deleted port groups , but not require success ,
       because mac not necessarily exists there */
    rc =  prvDxChBrgFdbFromCpuAuMsgStatusGet(devNum,GT_TRUE,deletedPortGroupsBmp);
    if(rc != GT_OK)
    {
        /* this is not error !!!! */
        rc = GT_OK;
    }

    /* wait for finish and success on added port groups to see that that added
       entry succeeded */
    rc = prvDxChBrgFdbFromCpuAuMsgStatusGet(devNum,GT_TRUE,portGroupsBmp);

    return rc;
}

GT_STATUS prvDxChBrgFdbMacEntryDelete
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    GT_STATUS   rc;

    rc = cpssDxChBrgFdbMacEntryDelete(devNum,macEntryKeyPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc =  prvDxChBrgFdbFromCpuAuMsgStatusGet(devNum,GT_FALSE,0);

    if(rc != GT_OK)
    {
        if(appDemoPpConfigList[devNum].numOfPortGroups == 0 ||
           (appDemoPpConfigList[devNum].fdbMode ==
            APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E))
        {
            return rc;
        }

        /* wait for finish on deleted port groups , but not require success ,
           because mac not necessarily exists there */

        /* this is not error !!!! */
        rc = GT_OK;
    }

    return rc;
}

#endif /*DXCH_CODE*/

/*******************************************************************************
* cpssEnChDxAuMsgHandle
*
* DESCRIPTION:
*       This routine gets and handles the ChDx Address Update messages.
*
* INPUTS:
*       devNum      - the device number.
*       evExtData   - Unified event additional information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssEnGenAuMsgHandle
(
    IN GT_U8                devNum,
    GT_U32                  evExtData
)
{

    GT_STATUS                   rc = GT_OK;
    GT_U32                      numOfAu;
    GT_U32                      auIndex;
    GT_BOOL                     completed;
    GT_BOOL                     succeeded;
    CPSS_MAC_UPDATE_MSG_EXT_STC bufferArr[AU_SIZE];
    CPSS_MAC_ENTRY_EXT_STC      cpssMacEntry;
    GT_U8                       dev;
    APP_DEMO_BRG_FDB_AUQ_FUNC   cbFuncPtr;/*(pointer) to call back function*/
    GT_U32                      error = 0;/* number of errors */

    evExtData = evExtData;

    if(IS_API_CH_DEV_MAC(devNum))/* is device support dxch API ? */
    {
#if defined DXCH_CODE
        cpssEnFdbBrgFdbAuMsgBlockGet       = cpssDxChBrgFdbAuMsgBlockGet;
        cpssEnFdbBrgFdbEntrySetFunc        = prvDxChBrgFdbPortGroupMacEntrySet;
        cpssEnFdbBrgFdbEntryDeleteFunc     = prvDxChBrgFdbMacEntryDelete;
        cpssEnFdbFromCpuAuMsgStatusGetFunc = NULL;/*the set and delete functions will handle it*/
#endif /*DXCH_CODE*/
    }

    if(IS_API_EXMXPM_DEV_MAC(devNum))/* is device support EXMXPM API ? */
    {
#if defined PM_CODE
        cpssEnFdbBrgFdbAuMsgBlockGet       = prvExMxPmBrgFdbAuMsgBlockGet;
        cpssEnFdbBrgFdbEntrySetFunc        = prvExMxPmBrgFdbEntrySet;
        cpssEnFdbBrgFdbEntryDeleteFunc     = prvExMxPmBrgFdbMacEntryDelete;
        cpssEnFdbFromCpuAuMsgStatusGetFunc = prvExMxPmBrgFdbFromCpuAuMsgStatusGet;
#endif
    }

    if(NULL == cpssEnFdbBrgFdbAuMsgBlockGet)
    {
        /* the AU messages processing is not implemented yet for the device */
        return GT_OK;
    }

    while(rc != GT_NO_MORE)
    {
        /*************************************/
        /* get the AU message buffer from HW */
        /*************************************/
        numOfAu = AU_SIZE;

        APPDEMO_FDB_UPDATE_LOCK();
        rc = cpssEnFdbBrgFdbAuMsgBlockGet(devNum, &numOfAu, bufferArr);
        APPDEMO_FDB_UPDATE_UNLOCK();

        if((rc != GT_NO_MORE) && (rc != GT_OK))
        {

            return rc;
        }

        /*********************************/
        /* handle all of the AU messages */
        /*********************************/

        for (auIndex = 0; auIndex < numOfAu; auIndex++)
        {
            switch (bufferArr[auIndex].updType)
            {
                case CPSS_AA_E:
                    if(appDemoSysConfig.supportAaMessage == GT_FALSE)
                    {
                        break;
                    }
                    /* fall through ... */
                case CPSS_NA_E:
                {
                    osMemCpy(&cpssMacEntry, &bufferArr[auIndex].macEntry,
                                 sizeof(CPSS_MAC_ENTRY_EXT_STC));
                    cpssMacEntry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
                    cpssMacEntry.age = GT_TRUE;

                    for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount); dev++)
                    {
						/* Do not delele the FDB of other asic on lion-0, for L3 FDB error. zhangdi@autelan.com 2012-04-17 */
						if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
						{
    						if(dev != devNum)
    						{
    							continue;
    						}
						}
						
                        if(appDemoPpConfigList[dev].valid == GT_FALSE)
                        {
                            continue;
                        }

                        APPDEMO_FDB_UPDATE_LOCK();
                        if(bufferArr[auIndex].updType == CPSS_NA_E)
                        {
                            /* write data to cpss */
                            rc = cpssEnFdbBrgFdbEntrySetFunc(dev, bufferArr[auIndex].portGroupId,&cpssMacEntry);
                        }
                        else /*AA*/
                        {
                            /* delete data from cpss */
                            rc = cpssEnFdbBrgFdbEntryDeleteFunc(dev, &cpssMacEntry.key);
                        }

                        if (rc != GT_OK)
                        {
                            /* go to next message to handle */
                            error++;
                            APPDEMO_FDB_UPDATE_UNLOCK();
                            continue;
                        }

                        if(cpssEnFdbFromCpuAuMsgStatusGetFunc)
                        {
                       completed = GT_FALSE;
                       succeeded = GT_FALSE;/*fix warning: local variable 'succeeded' may be used without having been initialized*/
                       /* wait for the PP to process the action */
                       while(completed == GT_FALSE)
                       {
                            /* simulation task needs CPU time in order
                               to execute the process */
                            #ifdef ASIC_SIMULATION
                            osTimerWkAfter(1);
                            #endif

                            /* Check that the AU message processing has completed */
                            rc = cpssEnFdbFromCpuAuMsgStatusGetFunc(dev,
                                                                    &completed,
                                                                    &succeeded);
                            if (rc != GT_OK)
                            {
                                    /* go to next message to handle */
                                    error++;
                                    succeeded = GT_FALSE;
                                    break;
                                }
                           }
                        }
                        else
                        {
                            /* was checked in other place */
                            succeeded = GT_TRUE;
                        }

                        APPDEMO_FDB_UPDATE_UNLOCK();

                        if (succeeded == GT_FALSE)
                        {
                            /* the action competed but without success !? */
                            /* go to next message to handle */
                            error++;
                            continue;
                        }
                    }

                    break;

                }
                case CPSS_QA_E:
                case CPSS_QR_E:
                case CPSS_SA_E:
                case CPSS_QI_E:
                case CPSS_FU_E:
                case CPSS_TA_E:
                    break;
                default:
                    /* the action competed but without success !? */
                    /* go to next message to handle */
                    error++;
                    continue;
            }

            if(traceAuq)
            {
                osPrintf("%s",traceAuqArr[bufferArr[auIndex].updType]);
            }

            /* check if we have callback function for this type of message */
            cbFuncPtr = auqCbFuncArr[bufferArr[auIndex].updType];
            if(cbFuncPtr)
            {
                /* call the callback */
                cbFuncPtr(devNum,&bufferArr[auIndex]);
            }

        }
    }

    if(error)
    {
                            return GT_FAIL;
                        }

    return rc;
}

/*******************************************************************************
* appDemoBrgFdbAuqCbRegister
*
* DESCRIPTION:
*       register a CB (callback) function for a specific AUQ message type.
*       the registered function (cbFuncPtr) will be called for every AUQ message
*       that match the messageType.
*
* INPUTS:
*       messageType - AUQ message type
*       cbFuncPtr   - (pointer to) the CB function
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS   appDemoBrgFdbAuqCbRegister
(
    IN CPSS_UPD_MSG_TYPE_ENT       messageType,
    IN APP_DEMO_BRG_FDB_AUQ_FUNC   cbFuncPtr
)
{
    switch(messageType)
    {
        case CPSS_NA_E:
        case CPSS_QA_E:
        case CPSS_QR_E:
        case CPSS_AA_E:
        case CPSS_TA_E:
        case CPSS_SA_E:
        case CPSS_QI_E:
        case CPSS_FU_E:
                    break;
                default:
                    return GT_BAD_PARAM;
            }

    auqCbFuncArr[messageType] = cbFuncPtr;
    return GT_OK;
}

/*******************************************************************************
* appDemoTraceAuqFlagSet
*
* DESCRIPTION:
*       function to allow set the flag of : traceAuq
*
* INPUTS:
*       enable - enable/disable the printings of indication of AUQ/FUQ messages:
*           "+",    CPSS_NA_E
*           "qa",   CPSS_QA_E should not be
*           "qr",   CPSS_QR_E
*           "-",    CPSS_AA_E
*           "t",    CPSS_TA_E
*           "sa",   CPSS_SA_E should not be
*           "qi",   CPSS_QI_E should not be
*           "f"     CPSS_FU_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoTraceAuqFlagSet
(
    IN GT_U32   enable
)
{
    traceAuq = enable;
    return GT_OK;
}

