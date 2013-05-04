/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssGenHsu.c
*
* DESCRIPTION:
*       Generic HSU ApIs
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/generic/interrupts/private/prvCpssGenIntDefs.h>
#include <cpss/generic/hsu/cpssGenHsu.h>
#include <cpss/generic/hsu/private/prvCpssGenHsu.h>

/* generic stages address array */
GT_U8 *stageAddressArray[PRV_CPSS_GEN_HSU_LAST_STAGE_E]=
{
   (GT_U8*)(&sysGenGlobalInfo),
   (GT_U8*)(&drvGlobalInfo)
};

/* generic stages size array */
GT_U32 stageSizeArray[PRV_CPSS_GEN_HSU_LAST_STAGE_E]=
{
    sizeof(PRV_CPSS_GEN_GLOBAL_DATA_STC),
    sizeof(PRV_CPSS_DRV_GLOBAL_DATA_STC)
};

/* generic stages array */
PRV_CPSS_GEN_HSU_DATA_STAGE_ENT stageArray[PRV_CPSS_GEN_HSU_LAST_STAGE_E]=
{
    PRV_CPSS_GEN_HSU_CPSS_STAGE_E,
    PRV_CPSS_GEN_HSU_DRV_STAGE_E
};



/*******************************************************************************
* prvCpssGenGlobalDataSizeGet
*
* DESCRIPTION:
*       This function calculate  size of generic CPSS and driver data
*
* APPLICABLE DEVICES: All  Devices.
*
* INPUTS:
*       None.
* OUTPUTS:
*       sizePtr                  -  size calculated in bytes
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssGenGlobalDataSizeGet
(
    OUT   GT_U32    *sizePtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(sizePtr);
    *sizePtr = sizeof(PRV_CPSS_GEN_GLOBAL_DATA_STC) + sizeof(PRV_CPSS_DRV_GLOBAL_DATA_STC);
    return GT_OK;
}
/*******************************************************************************
* prvCpssGenHsuExportImport
*
* DESCRIPTION:
*       This function exports/imports Generic HSU data to/from survived restart
*       memory area.
*
* APPLICABLE DEVICES: All  Devices.
*
* INPUTS:
*       actionType             - PRV_CPSS_HSU_EXPORT_E - export action
*                                PRV_CPSS_HSU_IMPORT_E - import action
*       currentIterPtr         - points to the current iteration.
*       hsuBlockMemSizePtr     - pointer hsu block data size supposed to be exported
*                                in current iteration.
*       hsuBlockMemPtr         - pointer to HSU survived restart memory area
*
* OUTPUTS:
*       currentIterPtr         - points to the current iteration
*       hsuBlockMemSizePtr     - pointer to hsu block data size exported in current iteration.
*       accumSizePtr           - points to accumulated size
*       actiontCompletePtr     - GT_TRUE - export/import action is complete
*                                GT_FALSE - otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong hsuBlockMemSize, wrong iterator.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssGenHsuExportImport
(
    IN     PRV_CPSS_HSU_ACTION_TYPE_ENT            actionType,
    INOUT  PRV_CPSS_HSU_GEN_ITERATOR_STC           *currentIterPtr,
    INOUT  GT_U32                                  *hsuBlockMemSizePtr,
    IN     GT_U8                                   **hsuBlockMemPtrPtr,
    OUT    GT_U32                                  *accumSizePtr,
    OUT    GT_BOOL                                 *actiontCompletePtr
)
{
    GT_STATUS rc;
    PRV_CPSS_GEN_HSU_DATA_STAGE_ENT currentStage;
    GT_BOOL enoughMemoryForStage = GT_FALSE;

    for( currentStage = PRV_CPSS_GEN_HSU_CPSS_STAGE_E;
         currentStage < PRV_CPSS_GEN_HSU_LAST_STAGE_E;
         currentStage++)
    {
        if (currentIterPtr->currStage == stageArray[currentStage])
        {
            if (currentIterPtr->currStageMemPtr == NULL)
            {
                currentIterPtr->currStageMemPtr = stageAddressArray[currentStage];
                currentIterPtr->currentStageRemainedSize = stageSizeArray[currentStage];
            }
            if (*hsuBlockMemSizePtr >= currentIterPtr->currentStageRemainedSize )
            {
                enoughMemoryForStage = GT_TRUE;
            }

            rc = prvCpssGenHsuExportImportDataHandling(actionType,
                                                       currentIterPtr,
                                                       hsuBlockMemSizePtr,
                                                       hsuBlockMemPtrPtr,
                                                       accumSizePtr);
            if (rc != GT_OK)
            {
                return rc;
            }

            if (enoughMemoryForStage == GT_FALSE)
            {
                return rc;
            }
        }
    }
    *actiontCompletePtr = GT_TRUE;

    return  GT_OK;
}


/*******************************************************************************
* prvCpssGenHsuExportImportDataHandling
*
* DESCRIPTION:
*       This function handle import/export generic data
*
* APPLICABLE DEVICES: All  Devices.
*
* INPUTS:
*       actionType             - PRV_CPSS_HSU_EXPORT_E - export action
*                                PRV_CPSS_HSU_IMPORT_E - import action
*       currentIterPtr         - points to the current iteration.
*       hsuBlockMemSizePtr     - pointer hsu block data size supposed to be exported
*                                in current iteration.
*       hsuBlockMemPtrPtr      - pointer to HSU survived restart memory area
*
* OUTPUTS:
*       currentIterPtr         - points to the current iteration
*       hsuBlockMemSizePtr     - pointer to hsu block data size exported in current iteration.
*       hsuBlockMemPtrPtr      - pointer to HSU survived restart memory area
*       accumSizePtr           - points to accumulated size
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong hsuBlockMemSize, wrong iterator.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssGenHsuExportImportDataHandling
(
    IN     PRV_CPSS_HSU_ACTION_TYPE_ENT            actionType,
    INOUT  PRV_CPSS_HSU_GEN_ITERATOR_STC           *currentIterPtr,
    INOUT  GT_U32                                  *hsuBlockMemSizePtr,
    INOUT  GT_U8                                   **hsuBlockMemPtrPtr,
    OUT    GT_U32                                  *accumSizePtr
)
{
    GT_PTR dst;
    GT_PTR src;
    if (actionType == PRV_CPSS_HSU_EXPORT_E)
    {
        dst = *hsuBlockMemPtrPtr;
        src = currentIterPtr->currStageMemPtr;
    }
    else
    {
        dst = currentIterPtr->currStageMemPtr;
        src = *hsuBlockMemPtrPtr;
    }
    if (*hsuBlockMemSizePtr >= currentIterPtr->currentStageRemainedSize )
    {
        cpssOsMemCpy(dst,src,currentIterPtr->currentStageRemainedSize);
        *hsuBlockMemSizePtr = *hsuBlockMemSizePtr - currentIterPtr->currentStageRemainedSize;
        currentIterPtr->currStageMemPtr = NULL;
        currentIterPtr->currStage++;
        *accumSizePtr += currentIterPtr->currentStageRemainedSize;
        *hsuBlockMemPtrPtr += currentIterPtr->currentStageRemainedSize;
        currentIterPtr->currentStageRemainedSize = 0;
    }
    else
    {
        cpssOsMemCpy(dst, src,*hsuBlockMemSizePtr);
        currentIterPtr->currStageMemPtr += *hsuBlockMemSizePtr;
        currentIterPtr->currentStageRemainedSize = currentIterPtr->currentStageRemainedSize - *hsuBlockMemSizePtr;
        *hsuBlockMemPtrPtr += *hsuBlockMemSizePtr;
        *accumSizePtr += *hsuBlockMemSizePtr;
        *hsuBlockMemSizePtr = 0;
    }
    return GT_OK;
}


/*******************************************************************************
* cpssHsuStateSet
*
* DESCRIPTION:
*       This function inform cpss about HSU state
*
*  APPLICABLE DEVICES: ALL
*
* INPUTS:
*       hsuInfoPtr               - pointer to hsu information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                  - on success.
*       GT_BAD_PTR             - on NULL pointer
*       GT_BAD_PARAM           - on bad hsuState
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssHsuStateSet
(
    IN CPSS_HSU_INFO_STC    *hsuInfoPtr
)
{

    CPSS_NULL_PTR_CHECK_MAC(hsuInfoPtr);
    switch(hsuInfoPtr->hsuState)
    {
    case CPSS_HSU_STATE_IN_PROGRESS_E:
    case CPSS_HSU_STATE_COMPLETED_E:
        break;
    default:
        return GT_BAD_PARAM;
    }
    hsuInfo.hsuState = hsuInfoPtr->hsuState;
    return GT_OK;
}


/*******************************************************************************
* cpssHsuEventHandleUpdate
*
* DESCRIPTION:
*       This function replace old event  handle created after regular init
*       to new event handle that application got during HSU.
*
*  APPLICABLE DEVICES: ALL
*
* INPUTS:
*       oldEvReqHndl               - old event  handle created after regular init.
*       newEvReqHndl               - new event handle  created during HSU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                  - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssHsuEventHandleUpdate
(
    IN     GT_U32 oldEvReqHndl,
    IN    GT_U32  newEvReqHndl
)
{
    GT_U8 devNum;
    GT_U32 txQueueNumber,descNum;
    PRV_CPSS_TX_DESC_LIST_STC *txDescList = NULL;
    PRV_CPSS_SW_TX_DESC_STC   *firstSwTxDesc = NULL;

    for (devNum = 0; devNum < PRV_CPSS_MAX_PP_DEVICES_CNS; devNum++)
    {
        if (prvCpssPpConfig[devNum] == NULL)
        {
            continue;
        }
        for(txQueueNumber = 0; txQueueNumber < NUM_OF_TX_QUEUES; txQueueNumber++)
        {
            txDescList  = &PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueueNumber];
            firstSwTxDesc = txDescList->swTxDescBlock;
            for (descNum = 0; descNum < txDescList->maxDescNum; descNum++)
            {
                if (firstSwTxDesc[descNum].evReqHndl == oldEvReqHndl)
                {
                    firstSwTxDesc[descNum].evReqHndl = newEvReqHndl;
                }
            }
        }
    }
    return GT_OK;
}


