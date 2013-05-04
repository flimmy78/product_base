/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvUtfHelpers.c
*
* DESCRIPTION:
*       API for logger, time management, memory allocation.
*       It is used only in the utfMain.c
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <gtOs/gtOsStr.h>
#include <ctype.h>
#include <cpssCommon/cpssPresteraDefs.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>

/* Control the printing */
static UTF_LOG_OUTPUT_ENT logOutputPrint = UTF_LOG_OUTPUT_SERIAL_ALL_E;

static FILE* fp = NULL;
static FILE* fResultFilePtr = NULL;

/* flag to indicate if logger file is opened */
static GT_BOOL prvUtfIsFileOpened = GT_FALSE;

#ifndef ASIC_SIMULATION
#define cpss_fprintf(_log)
#else
#define cpss_fprintf(_log)  fprintf _log
#endif
/*******************************************************************************
* prvUtfLogOpen
*
* DESCRIPTION:
*       Opens logger with defined output interface.
*
* INPUTS:
*       fileName  -   log output file name
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           -   logger has been successfully opened.
*       GT_BAD_PARAM    -   Invalid output interface id was passed.
*       GT_FAIL         -   General failure error. Should never happen.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvUtfLogOpen
(
    IN const GT_CHAR* fileName
)
{
    if (NULL == fileName)
    {
        return GT_BAD_PTR;
    }

    if (NULL != fp)
    {
        return GT_ALREADY_EXIST;
    }

    fp = fopen(fileName, "wt");
    if (NULL == fp)
    {
        return GT_BAD_PTR;
    }

    /* set flag */
    prvUtfIsFileOpened = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* prvUtfLogReadParam
*
* DESCRIPTION:
*       Read parameters from file to array.
*
* INPUTS:
*       fileName        - log output file name to check
*       numberParams    - number of parameters to read
*
* OUTPUTS:
*       arrayOfParamPtr - pointer to array of params from result file
*
* RETURNS:
*       GT_OK           - logger has been successfully opened.
*       GT_BAD_PARAM    - Invalid output interface id was passed.
*       GT_BAD_SIZE     - Invalid input file size.
*       GT_NO_RESOURCE  - Memory error.
*       GT_FAIL         - General failure error. Should never happen.
*
* COMMENTS: this functions used by  utfLogResultRun
*
*******************************************************************************/
GT_STATUS prvUtfLogReadParam
(
    IN  const GT_CHAR         *fileName,
    IN  GT_8                  numberParams,
    OUT UTF_RESULT_PARAM_STC *arrayOfParamPtr
)
{
    GT_U32      lSize;
    GT_U8*      buffer;
    GT_8        valueAnsi[UTF_MAX_VALUE_LEN_CNS];
    GT_U32      result;
    GT_U8       count;
    GT_U8       paramCharCounter;
    GT_U32      bufferCounter;
    GT_BOOL     setItValue;

    CPSS_NULL_PTR_CHECK_MAC(fileName);
    CPSS_NULL_PTR_CHECK_MAC(arrayOfParamPtr);

    if (NULL != fResultFilePtr)
    {
        return GT_ALREADY_EXIST;
    }
    fResultFilePtr = fopen(fileName, "rt");

    if (NULL == fResultFilePtr)
    {
        return GT_FAIL;
    }
    else
    {
        /* obtain file size:*/
        fseek (fResultFilePtr, 0, SEEK_END);
        lSize = ftell (fResultFilePtr);
        rewind (fResultFilePtr);

        if (lSize > UTF_MAX_LOG_SIZE_CNS)
        {
            return GT_BAD_SIZE;
        }

        /* allocate memory to contain the whole file: */
        buffer = (GT_VOID*) cpssOsMalloc(lSize);
        if (NULL == buffer)
        {
            return GT_NO_RESOURCE;
        }

        /* copy the file into the buffer: */
        result = fread(buffer, 1, lSize, fResultFilePtr);

        /*  result value depend on type of text files (\r\n or \n), for windows it will be less then lSize*/
        /*  if (result != lSize) {fputs ("Reading error",stderr); exit (3);} */

        /* the whole file is now loaded in the memory buffer. */

        bufferCounter = 0;

        cpssOsBzero((GT_VOID*) arrayOfParamPtr, sizeof(*arrayOfParamPtr) * numberParams);

        for(count = 0; count < numberParams; count++)
        {
            setItValue = 0;
            paramCharCounter = 0;
            cpssOsBzero((GT_VOID*) valueAnsi, sizeof(valueAnsi));

            while (*(buffer + bufferCounter) != '\n' && bufferCounter < result)
            {
               if (*(buffer + bufferCounter) == '\t')
               {
                    setItValue = 1;
                    paramCharCounter = 0;
                    bufferCounter++;
               }
               if (setItValue == 0 && paramCharCounter < UTF_MAX_PARAM_NAME_LEN_CNS)
               {
                    arrayOfParamPtr[count].paramName[paramCharCounter] = *(buffer + bufferCounter);
                    paramCharCounter++;
               }
               if (setItValue == 1 && paramCharCounter < UTF_MAX_VALUE_LEN_CNS && isdigit(*(buffer + bufferCounter)))
               {
                    valueAnsi[paramCharCounter] = *(buffer + bufferCounter);
                    paramCharCounter++;
               }
               bufferCounter++;
            }
            if (*(buffer + bufferCounter) == '\n')
            {
                bufferCounter++;
                arrayOfParamPtr[count].paramValue = osStrTo32((GT_CHAR*)valueAnsi);
            }
        }
        cpssOsFree(buffer);
        fclose(fResultFilePtr);
        fResultFilePtr = NULL;
    }
    return GT_OK;
}

/*******************************************************************************
* prvUtfLogMessage
*
* DESCRIPTION:
*       This routine writes message to the output interface.
*       This function handles all issues regarding the logging to specific
*       interfaces.
*
* INPUTS:
*       formatStringPtr -   (pointer to) format string.
*       argvPtr         -   (pointer to) vector of output string arguments.
*       argc            -   number of arguments in output string.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       -   logging has been done successfully.
*       GT_FAIL     -   General failure error. Should never happen.
*
* COMMENTS:
*       It uses OS abstraction layer to output.
*******************************************************************************/
GT_VOID prvUtfLogMessage
(
    IN const GT_CHAR    *formatStringPtr,
    IN const GT_VOID    *argvPtr[],
    IN GT_U32           argc
)
{
    if ((logOutputPrint == UTF_LOG_OUTPUT_SERIAL_FINAL_E) ||
       ((logOutputPrint == UTF_LOG_OUTPUT_FILE_E) && (!prvUtfIsFileOpened)))
    {
        return;
    }

    switch (argc)
    {
        case 0:
            if (logOutputPrint == UTF_LOG_OUTPUT_FILE_E)
            {
                cpss_fprintf((fp, formatStringPtr));
            }
            cpssOsPrintf(formatStringPtr);
            break;
        case 1:
            if (logOutputPrint == UTF_LOG_OUTPUT_FILE_E)
            {
                cpss_fprintf((fp, formatStringPtr, argvPtr[0]));
            }
            cpssOsPrintf(formatStringPtr, argvPtr[0]);
            break;
        case 2:
            if (logOutputPrint == UTF_LOG_OUTPUT_FILE_E)
            {
                cpss_fprintf((fp, formatStringPtr, argvPtr[0], argvPtr[1]));
            }
            cpssOsPrintf(formatStringPtr, argvPtr[0], argvPtr[1]);
            break;
        case 3:
            if (logOutputPrint == UTF_LOG_OUTPUT_FILE_E)
            {
                cpss_fprintf((fp, formatStringPtr, argvPtr[0], argvPtr[1], argvPtr[2]));
            }
            cpssOsPrintf(formatStringPtr, argvPtr[0], argvPtr[1], argvPtr[2]);
            break;
        case 4:
            if (logOutputPrint == UTF_LOG_OUTPUT_FILE_E)
            {
                cpss_fprintf((fp, formatStringPtr, argvPtr[0], argvPtr[1], argvPtr[2], argvPtr[3]));
            }
            cpssOsPrintf(formatStringPtr, argvPtr[0], argvPtr[1], argvPtr[2], argvPtr[3]);
            break;
        case 5:
            if (logOutputPrint == UTF_LOG_OUTPUT_FILE_E)
            {
                cpss_fprintf((fp, formatStringPtr, argvPtr[0], argvPtr[1], argvPtr[2], argvPtr[3], argvPtr[4]));
            }
            cpssOsPrintf(formatStringPtr, argvPtr[0], argvPtr[1], argvPtr[2], argvPtr[3], argvPtr[4]);
            break;
        case 6:
            if (logOutputPrint == UTF_LOG_OUTPUT_FILE_E)
            {
                cpss_fprintf((fp, formatStringPtr, argvPtr[0], argvPtr[1], argvPtr[2], argvPtr[3], argvPtr[4], argvPtr[5]));
            }
            cpssOsPrintf(formatStringPtr, argvPtr[0], argvPtr[1], argvPtr[2], argvPtr[3], argvPtr[4], argvPtr[5]);
            break;
        case 7:
            if (logOutputPrint == UTF_LOG_OUTPUT_FILE_E)
            {
                cpss_fprintf((fp, formatStringPtr, argvPtr[0], argvPtr[1], argvPtr[2], argvPtr[3], argvPtr[4], argvPtr[5], argvPtr[6]));
            }
            cpssOsPrintf(formatStringPtr, argvPtr[0], argvPtr[1], argvPtr[2], argvPtr[3], argvPtr[4], argvPtr[5], argvPtr[6]);
            break;
        default:
            if (logOutputPrint == UTF_LOG_OUTPUT_FILE_E)
            {
                cpss_fprintf((fp, "[UTF]: prvUtfLogMessage: too many arguments [%d]\n", (int)argc));
            }
            cpssOsPrintf("[UTF]: prvUtfLogMessage: too many arguments [%d]\n", argc);
            break;
    }
}

/*******************************************************************************
* prvUtfLogClose
*
* DESCRIPTION:
*       This routine closes logger.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
*       GT_OK           -   logger has been successfully closed.
*       GT_FAIL         -   General failure error. Should never happen.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfLogClose
(
    GT_VOID
)
{
    if (NULL == fp)
    {
        return GT_BAD_PTR;
    }

    fclose(fp);
    fp = NULL;

    /* set flag */
    prvUtfIsFileOpened = GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* prvUtfLogPrintEnable
*
* DESCRIPTION:
*       This routine Enables\Disables printing.
*
* INPUTS:
*       printEnable   - If GT_TRUE print is enabled else disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       the previous settings
*
* COMMENTS:
*
*******************************************************************************/
UTF_LOG_OUTPUT_ENT prvUtfLogPrintEnable
(
    UTF_LOG_OUTPUT_ENT utfLogOutputSelectMode
)
{
    UTF_LOG_OUTPUT_ENT previous = logOutputPrint;

    logOutputPrint = utfLogOutputSelectMode;

    return previous;
}


