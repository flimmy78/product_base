/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
********************************************************************************
* appDemoExMxPmHsuUtils.c
*
* DESCRIPTION:
*       App demo ExMxPm HSU API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/exMxPm/exMxPmGen/hsu/cpssExMxPmHsu.h>
#include <cpss/exMxPm/exMxPmGen/hsu/private/prvCpssExMxPmHsu.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/utils/hsu/appDemoExMxPmHsuUtils.h>
#include <appDemo/userExit/userEventHandler.h>

static GT_U8  *hsuBlockMemPtr;

/*******************************************************************************
* appDemoExMxPmHsuOldImagePreUpdatePreparation
*
* DESCRIPTION:
*       This function perform following steps of HSU process:
*       1. Disable interrupts
*       2. export of all hsu datatypes
*       3. warm restart
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       isMultipleIteration - GT_TRUE - multiIteration HSU operation
*                             GT_FALSE - single iteration HSU operation
*       origIterationSize   - hsu iteration size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NO_RESOURCE           - no place to save the request
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmHsuOldImagePreUpdatePreparation
(
  IN GT_BOOL                               isMultipleIteration,
  IN GT_U32                                origIterationSize
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 i;
    GT_U32 iterationSize;
    GT_U32 hsuDataSize;
    GT_U32 tempIterSize;
    CPSS_EXMXPM_HSU_DATA_TYPE_ENT dataType = CPSS_EXMXPM_HSU_DATA_TYPE_ALL_E;
    GT_BOOL exportComplete;
    GT_U32 iterationNumber;
    GT_U32 remainedSize = 0;
#ifndef ASIC_SIMULATION
    GT_U32 hsuPtr;
#endif

    GT_U32 iter = 0;

    /* disable interrupts */
    rc = cpssPpInterruptsDisable(0);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* perform full export */

    /* get the size of all exported data */
    rc =  cpssExMxPmHsuBlockSizeGet(dataType, &hsuDataSize);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* check with maximum size 8M RAM (max HSU memory size in our system)*/
    if (hsuDataSize > _8M)
    {
        return GT_NO_RESOURCE;
    }
    if (isMultipleIteration == GT_FALSE)
    {
        iterationNumber = 1;
        iterationSize = hsuDataSize;
    }
    else
    {
        iterationNumber = hsuDataSize/origIterationSize;
        remainedSize = hsuDataSize%origIterationSize;
        iterationSize  = origIterationSize;
    }

#ifdef ASIC_SIMULATION
    hsuBlockMemPtr = osMalloc(hsuDataSize*sizeof(GT_U8));
    osMemSet(hsuBlockMemPtr, 0, hsuDataSize*sizeof(GT_U8));
#else
    rc = cpssExtDrvHsuMemBaseAddrGet(&hsuPtr);
    hsuBlockMemPtr = (GT_U8*)hsuPtr;
    if (rc != GT_OK)
    {
        return rc;
    }
#endif
    tempIterSize = origIterationSize;
    /* call cpss api function */
    for( i = 0; i < iterationNumber; i++)
    {
        if (i == iterationNumber -1)
        {
            iterationSize += remainedSize;
        }
        rc = cpssExMxPmHsuExport(dataType,
                                     &iter,
                                     &iterationSize,
                                     hsuBlockMemPtr,
                                     &exportComplete);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (i != iterationNumber -1 )
        {
            hsuBlockMemPtr = (GT_U8*)(hsuBlockMemPtr + (tempIterSize - iterationSize));
            tempIterSize = origIterationSize + iterationSize;
            iterationSize = tempIterSize;
        }
    }

    /* this delay currently solve timing problem. The root cause
       should be revealed */
    osTimerWkAfter(100);
#ifndef ASIC_SIMULATION
    cpssExtDrvHsuWarmRestart();
#endif
    return GT_OK;
}


/*******************************************************************************
* appDemoExMxPmHsuNewImageUpdateAndSynch
*
* DESCRIPTION:
*       This function perform following steps of HSU process:
*       1. import of all hsu datatypes
*       2. setting HSU state in "HSU is complete"
*       4. open pp interrupts
*       3. enable interrupts
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       isMultipleIteration - GT_TRUE - multiIteration HSU operation
*                             GT_FALSE - single iteration HSU operation
*       origIterationSize   - hsu iteration size
*       hsuDataSize         - hsu import data size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NO_RESOURCE           - no place to save the request
*
* COMMENTS:
*       When appDemoExMxPmHsuOldImagePreUpdatePreparation is finished and new image
*       have arisen after warm restart, user should perform
*       cpssHsuStateSet(CPSS_HSU_STATE_IN_PROGRESS_E), cpssInitSystem and only
*       after that call appDemoExMxPmHsuNewImageUpdateAndSynch.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmHsuNewImageUpdateAndSynch
(
  IN GT_BOOL                               isMultipleIteration,
  IN GT_U32                                origIterationSize,
  IN GT_U32                                hsuDataSize
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 i;
    GT_U32 iterationSize;
    GT_U32 tempIterSize;
    CPSS_EXMXPM_HSU_DATA_TYPE_ENT dataType;
    GT_BOOL importComplete;
    GT_U32 iterationNumber;
    GT_U32 remainedSize = 0;
    CPSS_HSU_INFO_STC    hsuInfo;
#ifndef ASIC_SIMULATION
    GT_U32 hsuPtr;
#endif
    GT_U32 iter = 0;

    hsuInfo.hsuState = CPSS_HSU_STATE_COMPLETED_E;
    dataType = CPSS_EXMXPM_HSU_DATA_TYPE_ALL_E;

    /* perform full import */

    if (isMultipleIteration == GT_FALSE)
    {
        iterationNumber = 1;
        iterationSize = hsuDataSize;
    }
    else
    {
        iterationNumber = hsuDataSize/origIterationSize;
        remainedSize = hsuDataSize%origIterationSize;
        iterationSize  = origIterationSize;
    }
#ifndef ASIC_SIMULATION
    rc = cpssExtDrvHsuMemBaseAddrGet(&hsuPtr);
    hsuBlockMemPtr = (GT_U8*)hsuPtr;
    if (rc != GT_OK)
    {
        return rc;
    }
#endif
    tempIterSize = origIterationSize;
    /* call cpss api function */
    for( i = 0; i < iterationNumber; i++)
    {
        if (i == iterationNumber -1)
        {
            iterationSize += remainedSize;
        }

        rc = cpssExMxPmHsuImport(dataType,
                                     &iter,
                                     &iterationSize,
                                     hsuBlockMemPtr,
                                     &importComplete);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (i != iterationNumber - 1)
        {
            hsuBlockMemPtr = (GT_U8*)(hsuBlockMemPtr + (tempIterSize - iterationSize));
            tempIterSize = origIterationSize + iterationSize;
            iterationSize = tempIterSize;
        }
    }
    /* set HSU state in "HSU_COMPLETE" */

    rc =  cpssHsuStateSet(&hsuInfo);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* spawn the user event handler processes and open interrupts */
    rc = appDemoEventRequestDrvnModeInit();
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}
