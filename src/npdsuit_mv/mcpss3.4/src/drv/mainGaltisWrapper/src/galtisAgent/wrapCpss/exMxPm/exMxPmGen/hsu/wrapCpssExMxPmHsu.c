/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmHsu.c
*
* DESCRIPTION:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/exMxPm/exMxPmGen/hsu/cpssExMxPmHsu.h>
#include <cpss/exMxPm/exMxPmGen/hsu/private/prvCpssExMxPmHsu.h>

static GT_U8  *hsuBlockMemPtr;
#ifdef ASIC_SIMULATION
static GT_U8  *origHsuBlockMemPtr[CPSS_EXMXPM_HSU_DATA_TYPE_ALL_E +1] = {NULL};
#endif
#ifndef ASIC_SIMULATION
static GT_BOOL hsuMemMapping = GT_FALSE;
#endif
/*******************************************************************************
* cpssExMxPmHsuBlockSizeGet
*
* DESCRIPTION:
*       This function gets the memory size needed to export the required HSU
*       data block.
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
* INPUTS:
*       dataType               - hsu data type.
*
* OUTPUTS:
*       sizePtr               - memory size needed to export required hsu data
*                               block. (calculated in bytes)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmHsuBlockSizeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_EXMXPM_HSU_DATA_TYPE_ENT dataType;
    GT_U32 hsuDataSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dataType = (CPSS_EXMXPM_HSU_DATA_TYPE_ENT)inArgs[0];
    /* call cpss api function */
    result = cpssExMxPmHsuBlockSizeGet(dataType, &hsuDataSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", hsuDataSize);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmHsuExport
*
* DESCRIPTION:
*       This function exports required HSU data block to survived restart
*       memory area supplied by application.
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
* INPUTS:
*       dataType               - hsu data type.
*       iteratorPtr            - pointer to iterator, to start - set iterator to 0.
*       hsuBlockMemSizePtr     - pointer to hsu block data size supposed to be exported
*                                in current iteration.The minimal value is 1k bytes.
*       hsuBlockMemPtr         - pointer to HSU survived restart memory area
*
* OUTPUTS:
*       iteratorPtr            - the iterator - points to the point from which
*                                process will be continued in future iteration.
*       hsuBlockMemSizePtr     - pointer to hsu block data size exported in current iteration.
*       exportCompletePtr      - GT_TRUE -  HSU data export is completed.
*                                GT_FALSE - HSU data export is not completed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong hsuBlockMemSize, dataType.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
CMD_STATUS wrCpssExMxPmHsuExport
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_EXMXPM_HSU_DATA_TYPE_ENT dataType;
    GT_U32 hsuDataSize;
    GT_U32 iterationSize;
    GT_U32 origIterationSize;
    GT_U32 tempIterSize;
    GT_BOOL exportComplete;
    GT_U32 iter;
    GT_U32 iterationNumber;
    GT_U32 i;
    GT_U32 remainedSize = 0;
    GT_U32      startSec  = 0;
    GT_U32      startNsec = 0;
    GT_U32      endSec  = 0;
    GT_U32      endNsec = 0;

#ifndef ASIC_SIMULATION
    GT_U32 hsuPtr;
#endif
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dataType = (CPSS_EXMXPM_HSU_DATA_TYPE_ENT)inArgs[0];
    iter = (GT_U32)inArgs[1];
    iterationNumber = (GT_U32)inArgs[2];
    result =  cpssExMxPmHsuBlockSizeGet(dataType, &hsuDataSize);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
#ifdef ASIC_SIMULATION
    hsuBlockMemPtr = cmdOsMalloc(hsuDataSize*sizeof(GT_U8));
    cmdOsMemSet(hsuBlockMemPtr, 0, hsuDataSize*sizeof(GT_U8));
    origHsuBlockMemPtr[dataType] = hsuBlockMemPtr;
#else
    if (hsuMemMapping == GT_FALSE)
    {
        result = cpssExtDrvHsuMemBaseAddrGet(&hsuPtr);
        hsuBlockMemPtr = (GT_U8*)hsuPtr;
        if (result != GT_OK)
        {
            return result;
        }
        hsuMemMapping = GT_TRUE;
    }
#endif
    origIterationSize = hsuDataSize/iterationNumber;
    remainedSize = hsuDataSize%iterationNumber;
    iterationSize  = origIterationSize;
    tempIterSize   = origIterationSize;
    /* call cpss api function */
    result = cpssOsTimeRT(&startSec, &startNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    for( i = 0; i < iterationNumber; i++)
    {
        if (i == iterationNumber - 1)
        {
            /* last iteration size */
            iterationSize += remainedSize;
            tempIterSize = iterationSize;
        }
        result = cpssExMxPmHsuExport(dataType,
                                     &iter,
                                     &iterationSize,
                                     hsuBlockMemPtr,
                                     &exportComplete);
        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "");
            return CMD_OK;
        }
        if (i != iterationNumber -1)
        {
            hsuBlockMemPtr = (GT_U8*)(hsuBlockMemPtr + (tempIterSize - iterationSize));
            tempIterSize = origIterationSize + iterationSize;
            iterationSize = tempIterSize;
        }
        else
        {
            /* last iteration is done. Promote hsuBlockMemPtr for case of another
               export call */
            hsuBlockMemPtr = (GT_U8*)(hsuBlockMemPtr + tempIterSize);
        }
    }
    result = cpssOsTimeRT(&endSec, &endNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    if (endNsec < startNsec)
    {
        endNsec += 1000000000;
        endSec--;
    }

    cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
    cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",iter,iterationSize,exportComplete);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmHsuImport
*
* DESCRIPTION:
*       This function imports required HSU data block from survived restart
*       memory area supplied by application.
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
* INPUTS:
*       dataType               - hsu data type.
*       iteratorPtr            - pointer to iterator, to start - set iterator to 0.
*       hsuBlockMemSizePtr     - pointer to hsu block data size supposed to be imported
*                                in current iteration.The minimal value is 1k bytes.
*       hsuBlockMemPtr         - pointer to HSU survived restart memory area
*
* OUTPUTS:
*       iteratorPtr            - the iterator - points to the point from which
*                                process will be continued in future iteration.
*       hsuBlockMemSizePtr     - pointer to hsu block data size imported in current iteration.
*       importCompletePtr      - GT_TRUE -  HSU data import is completed.
*                                GT_FALSE - HSU data import is not completed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
 CMD_STATUS wrCpssExMxPmHsuImport
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    CPSS_EXMXPM_HSU_DATA_TYPE_ENT dataType;
    GT_U32 hsuDataSize;
    GT_BOOL importComplete;
    GT_U32 iter;
    GT_U32 iterationNumber;
    GT_U32 i;
    GT_U32 iterationSize;
    GT_U32 origIterationSize;
    GT_U32 tempIterSize;
    GT_U32 remainedSize = 0;
    GT_U32      startSec  = 0;
    GT_U32      startNsec = 0;
    GT_U32      endSec  = 0;
    GT_U32      endNsec = 0;

#ifndef ASIC_SIMULATION
    GT_U32 hsuPtr;
#endif
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dataType = (CPSS_EXMXPM_HSU_DATA_TYPE_ENT)inArgs[0];
    hsuDataSize = (GT_U32)inArgs[2];
    iter = (GT_U32)inArgs[1];
    iterationNumber = (GT_U32)inArgs[3];

#ifndef ASIC_SIMULATION
    if (hsuMemMapping == GT_FALSE)
    {
        result = cpssExtDrvHsuMemBaseAddrGet(&hsuPtr);
        hsuBlockMemPtr = (GT_U8*)hsuPtr;
        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "");
            return CMD_OK;
        }
        hsuMemMapping = GT_TRUE;
    }
#else
    hsuBlockMemPtr = origHsuBlockMemPtr[dataType];
#endif

    origIterationSize = hsuDataSize/iterationNumber;
    remainedSize = hsuDataSize%iterationNumber;
    iterationSize  = origIterationSize;
    tempIterSize   = origIterationSize;
    /* call cpss api function */
    result = cpssOsTimeRT(&startSec, &startNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    for( i = 0; i < iterationNumber; i++)
    {
        if (i == iterationNumber - 1)
        {
            /* last iteration size */
            iterationSize += remainedSize;
            tempIterSize = iterationSize;
        }
        result = cpssExMxPmHsuImport(dataType,
                                     &iter,
                                     &iterationSize,
                                     hsuBlockMemPtr,
                                     &importComplete);
        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "");
            return CMD_OK;
        }
        if (i != iterationNumber-1)
        {
            hsuBlockMemPtr = (GT_U8*)(hsuBlockMemPtr + (tempIterSize - iterationSize));
            tempIterSize = origIterationSize + iterationSize;
            iterationSize = tempIterSize;
        }
        else
        {
            /* last iteration is done. Promote hsuBlockMemPtr for case of another
               import call */
            hsuBlockMemPtr = (GT_U8*)(hsuBlockMemPtr + tempIterSize );
        }
    }

    /* allocated space need to be freed after Importing the data */
#ifdef ASIC_SIMULATION
    cmdOsFree(origHsuBlockMemPtr[dataType]);
#endif
    result = cpssOsTimeRT(&endSec, &endNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    if (endNsec < startNsec)
    {
        endNsec += 1000000000;
        endSec--;
    }

    cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
    cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",iter,iterationSize,importComplete);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmHsuOldImagePreUpdatePreparation
*
* DESCRIPTION:
*       This function perform following actions:
*       1. Disable interrupts
*       2. export of all hsu datatypes
*       3. warm restart
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
*******************************************************************************/
CMD_STATUS wrCpssExMxPmHsuOldImagePreUpdatePreparation
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_BOOL isMultipleIteration;
    GT_U32 origIterationSize;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    isMultipleIteration = (GT_BOOL)inArgs[0];
    origIterationSize = (GT_U32)inArgs[1];

    result =  cmdAppExMxPmHsuOldImagePreUpdatePreparation(isMultipleIteration,
                                                          origIterationSize);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmHsuNewImageUpdateAndSynch
*
* DESCRIPTION:
*       This function perform following actions:
*       1. import of all hsu datatypes
*       2. setting HSU state in "HSU is complete"
*       3. enable interrupts
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
*******************************************************************************/
CMD_STATUS wrCpssExMxPmHsuNewImageUpdateAndSynch
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_BOOL isMultipleIteration;
    GT_U32 origIterationSize;
    GT_U32 hsuDataSize;
    GT_U32 intKey = 0;       /* The interrupt key        */
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    isMultipleIteration = (GT_BOOL)inArgs[0];
    origIterationSize = (GT_U32)inArgs[1];
    hsuDataSize = (GT_U32)inArgs[2];
    intKey = (GT_U32)inArgs[3];

    result = cmdAppExMxPmHsuNewImageUpdateAndSynch(isMultipleIteration,
                                                   origIterationSize,
                                                   hsuDataSize);
     /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPmHsuExport
*
* DESCRIPTION:
*       This function exports required HSU data block to survived restart
*       memory area supplied by application.
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
* INPUTS:
*       dataType               - hsu data type.
*       iteratorPtr            - pointer to iterator, to start - set iterator to 0.
*       hsuBlockMemSizePtr     - pointer to hsu block data size supposed to be exported
*                                in current iteration.The minimal value is 1k bytes.
*       hsuBlockMemPtr         - pointer to HSU survived restart memory area
*
* OUTPUTS:
*       iteratorPtr            - the iterator - points to the point from which
*                                process will be continued in future iteration.
*       hsuBlockMemSizePtr     - pointer to hsu block data size exported in current iteration.
*       exportCompletePtr      - GT_TRUE -  HSU data export is completed.
*                                GT_FALSE - HSU data export is not completed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong hsuBlockMemSize, dataType.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
CMD_STATUS wrCpssExMxPmHsuExport_1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_EXMXPM_HSU_DATA_TYPE_ENT dataType;
    GT_U32                        iter;
    GT_U32                        hsuBlockMemSize;
    GT_U8                         *hsuBlockPtr;
    GT_BOOL                       exportComplete;
    GT_U32      startSec  = 0;
    GT_U32      startNsec = 0;
    GT_U32      endSec  = 0;
    GT_U32      endNsec = 0;

#ifdef ASIC_SIMULATION
    return GT_NOT_SUPPORTED;
#endif
#ifndef ASIC_SIMULATION
    GT_U32 hsuPtr;
#endif
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dataType = (CPSS_EXMXPM_HSU_DATA_TYPE_ENT)inArgs[0];
    iter     = (GT_U32)inArgs[1];
    hsuBlockMemSize  = (GT_U32)inArgs[2];
    hsuBlockPtr = (GT_U8 *)(GT_U32)inArgs[3];
    #ifndef ASIC_SIMULATION
    if (hsuMemMapping == GT_FALSE)
    {
        result = cpssExtDrvHsuMemBaseAddrGet(&hsuPtr);
        hsuBlockMemPtr = (GT_U8*)hsuPtr;
        if (result != GT_OK)
        {
            return result;
        }
        hsuMemMapping = GT_TRUE;
    }
    #endif
    result = cpssOsTimeRT(&startSec, &startNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    result = cpssExMxPmHsuExport(dataType,
                                 &iter,
                                 &hsuBlockMemSize,
                                 hsuBlockPtr,
                                 &exportComplete);
    result = cpssOsTimeRT(&endSec, &endNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    if (endNsec < startNsec)
    {
        endNsec += 1000000000;
        endSec--;
    }

    cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
    cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",iter,hsuBlockMemSize,exportComplete);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmHsuImport
*
* DESCRIPTION:
*       This function imports required HSU data block from survived restart
*       memory area supplied by application.
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
* INPUTS:
*       dataType               - hsu data type.
*       iteratorPtr            - pointer to iterator, to start - set iterator to 0.
*       hsuBlockMemSizePtr     - pointer to hsu block data size supposed to be imported
*                                in current iteration.The minimal value is 1k bytes.
*       hsuBlockMemPtr         - pointer to HSU survived restart memory area
*
* OUTPUTS:
*       iteratorPtr            - the iterator - points to the point from which
*                                process will be continued in future iteration.
*       hsuBlockMemSizePtr     - pointer to hsu block data size imported in current iteration.
*       importCompletePtr      - GT_TRUE -  HSU data import is completed.
*                                GT_FALSE - HSU data import is not completed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
 CMD_STATUS wrCpssExMxPmHsuImport_1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    CPSS_EXMXPM_HSU_DATA_TYPE_ENT dataType;
    GT_U32                        iter;
    GT_U32                        hsuBlockMemSize;
    GT_U8                         *hsuBlockPtr;
    GT_BOOL                       importComplete;
    GT_U32      startSec  = 0;
    GT_U32      startNsec = 0;
    GT_U32      endSec  = 0;
    GT_U32      endNsec = 0;


#ifdef ASIC_SIMULATION
    return GT_NOT_SUPPORTED;
#endif

#ifndef ASIC_SIMULATION
    GT_U32 hsuPtr;
#endif
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dataType = (CPSS_EXMXPM_HSU_DATA_TYPE_ENT)inArgs[0];
    iter            = (GT_U32)inArgs[1];
    hsuBlockMemSize = (GT_U32)inArgs[2];
    hsuBlockPtr = (GT_U8 *)(GT_U32)inArgs[3];
#ifndef ASIC_SIMULATION
    if (hsuMemMapping == GT_FALSE)
    {
        result = cpssExtDrvHsuMemBaseAddrGet(&hsuPtr);
        hsuBlockMemPtr = (GT_U8*)hsuPtr;
        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "");
            return CMD_OK;
        }
        hsuMemMapping = GT_TRUE;
    }
#endif
    result = cpssOsTimeRT(&startSec, &startNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    result = cpssExMxPmHsuImport(dataType,
                                 &iter,
                                 &hsuBlockMemSize,
                                 hsuBlockPtr,
                                 &importComplete);
    result = cpssOsTimeRT(&endSec, &endNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    if (endNsec < startNsec)
    {
        endNsec += 1000000000;
        endSec--;
    }

    cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
    cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",iter,hsuBlockMemSize,importComplete);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmHsuBlockSizeGet",
         &wrCpssExMxPmHsuBlockSizeGet,
         1, 0},
        {"cpssExMxPmHsuExport",
         &wrCpssExMxPmHsuExport,
         3, 0},
        {"cpssExMxPmHsuImport",
         &wrCpssExMxPmHsuImport,
         4, 0},
        {"cpssExMxPmHsuOldImagePreUpdatePreparation",
         &wrCpssExMxPmHsuOldImagePreUpdatePreparation,
         2, 0},
        {"cpssExMxPmHsuNewImageUpdateAndSynch",
         &wrCpssExMxPmHsuNewImageUpdateAndSynch,
         4, 0},
        {"cpssExMxPmHsuBaseExport",
         &wrCpssExMxPmHsuExport_1,
         4, 0},
        {"cpssExMxPmHsuBaseImport",
         &wrCpssExMxPmHsuImport_1,
         4, 0},


};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmHsu
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitCpssExMxPmHsu
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

