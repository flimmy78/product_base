/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmExternalMemory.c
*
* DESCRIPTION:
*       wrappers for cpssExMxPmExternalMemory.c
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
#include <cpss/exMxPm/exMxPmGen/extMemory/cpssExMxPmExternalMemory.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmHwTables.h>

/*************************************************************************
* cpssExMxPmExternalMemoryParityErrorCntrGet
*
* DESCRIPTION:
*   The function gets the parity error counter value of the selected
*   external memory controller.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       memType - the selected external memory controller
*
* OUTPUTS:
*       cntrPtr - (pointer to) the value of the parity error counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CSU and LMPs parity error counters are cleared by read.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalMemoryParityErrorCntrGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_ENT    memType;
    GT_U32                                  cntrData;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    memType=(CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmExternalMemoryParityErrorCntrGet(devNum, memType, &cntrData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntrData);

    return CMD_OK;
}

/*************************************************************************
* cpssExMxPmExternalMemoryCsuEccErrorCntrGet
*
* DESCRIPTION:
*   The function gets the ECC error counter value of the CSU.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       cntrPtr - (pointer to) the value of the ECC error counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CSU ECC error counter is cleared by read.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalMemoryCsuEccErrorCntrGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    GT_U32                                  cntrData;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmExternalMemoryCsuEccErrorCntrGet(devNum, &cntrData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntrData);

    return CMD_OK;
}

/*************************************************************************
* cpssExMxPmExternalMemoryLastErrorAddressGet
*
* DESCRIPTION:
*   The function gets the last erroneous sram address of the selected
*   external memory controller.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       memType - the selected external memory controller
*
* OUTPUTS:
*       addrPtr - (pointer to) the last erroneous SRAM line number
*                 (offset from external SRAM start).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CSU and LMPs last errorneous address registers are cleared by read.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalMemoryLastErrorAddressGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_ENT    memType;
    GT_U32                                  addrData;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    memType=(CPSS_EXMXPM_EXTERNAL_MEMORY_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmExternalMemoryLastErrorAddressGet(devNum, memType, &addrData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", addrData);

    return CMD_OK;
}

/*************************************************************************
* wrCpssExMxPmExternalMemoryCsuEccEmulationTableInfoGet
*
* DESCRIPTION:
*   The function gets the info about the table in the CSU when using ECC mode.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - the device number
*       tableType       - the table in the CSU to get pointer to it's info
*
*
* OUTPUTS:
*       baseAddr - base address of start of the table.
*       entrySize - number of bits the width of table entry
*       numberOfLinesInCsu - number of CSU lines that the single table entry occupies
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalMemoryCsuEccEmulationTableInfoGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                            devNum;
    CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLES_ENT   tableType;
    PRV_CPSS_EXMXPM_TABLE_INFO_STC  *tableInfoPtr;/*pointer to the table info*/

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    tableType = (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLES_ENT)inArgs[1];

    /* convert the table type to pointer to table info */
    result = prvCpssExMxPmTableEccEmulationAutoCorrectionCsuTableGet(devNum,tableType,&tableInfoPtr);
    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result,"");
        return CMD_OK;
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result , "%d%d%d",
                tableInfoPtr->baseAddr,
                tableInfoPtr->entrySize,
                (tableInfoPtr->entrySize / tableInfoPtr->memCtrlPtr->lineWidthInBits) + 1
                );

    return CMD_OK;
}

/*************************************************************************
* cpssExMxPmExternalMemoryCsuEccEmulationModeSet
*
* DESCRIPTION:
*       Set the ECC emulation modes , used for the CSU when working in ECC
*       protection in HW.
*
*       The function gives fully control on the ECC emulation support from the CPSS :
*       The API applicable to runtime and not only to initialization stage (for debugging).
*           1. Controls if the ECC emulation needed :
*               a. write only
*               b. read only
*               c. Read + write
*               d. not needed (none)
*           2. Controls the Lines type supported:
*               a. Multi-lines only
*               b. Single line + multi lines (all)
*
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       readWriteMode - read/write mode.
*           Controls if the ECC emulation needed :
*               a. write only
*               b. read only
*               c. Read + write
*               d. not needed (none)
*       linesMode - lines mode.
*           Controls the Lines type supported:
*               a. Multi-lines only
*               b. Single line + multi lines (all)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. part of the ECC emulation in the CPSS.
*
*       2. All the CSU ECC emulations relevant only when the 'phase 1' init
*       parameter indicates that the CSU  used with ECC.
*       Meaning : CPSS_EXMXPM_PP_PHASE1_INIT_INFO_STC::controlSramProtect == CPSS_EXMXPM_CSU_PROTECT_ECC_E
*
*******************************************************************************/
static CMD_STATUS  wrCpssExMxPmExternalMemoryCsuEccEmulationModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8            devNum;
    CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_ECC_EMULATION_MODE_ENT       readWriteMode;
    CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_ECC_EMULATION_LINES_MODE_ENT linesMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    readWriteMode = (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_ECC_EMULATION_MODE_ENT)inArgs[1];
    linesMode = (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_ECC_EMULATION_LINES_MODE_ENT)inArgs[2];

    result = cpssExMxPmExternalMemoryCsuEccEmulationModeSet(devNum,readWriteMode,linesMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*************************************************************************
* cpssExMxPmExternalMemoryCsuEccEmulationErrorCntrGet
*
* DESCRIPTION:
*       Function return the number of single and double ECC errors found
*       (and fixed) by the emulation of the ECC.
*
*       NOTEs:
*           1. Those counters not relate to the ECC counters of the HW.
*           2. The counters are 'Clear by Read' (the function set 0 to those
*              counters at the end of function).
*               a. Application must synchronize the 'read' tables with this API
*                  to avoid non protected 'Clear by Read' action by this function.
*           3. The 'Extended TTI action' entry although spread on 3 lines,
*              considered as 2 lines with ECC + single line with ECC.
*           4. for HW counter use cpssExMxPmExternalMemoryCsuEccErrorCntrGet(...)
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - the device number
*
* OUTPUT:
*       singleErrorFixedCounterPtr - (pointer to) the number of single errors found
*                   and fixed in table in CSU line.
*       doubleErrorUnfixableCounterPtr - (pointer to) the number of double errors
*           found (and unfixable) in table entries in CSU.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad device
*       GT_BAD_PTR - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. part of the ECC emulation in the CPSS.
*
*       2. All the CSU ECC emulations relevant only when the 'phase 1' init
*       parameter indicates that the CSU  used with ECC.
*       Meaning : CPSS_EXMXPM_PP_PHASE1_INIT_INFO_STC::controlSramProtect == CPSS_EXMXPM_CSU_PROTECT_ECC_E
*
*******************************************************************************/
static CMD_STATUS  wrCpssExMxPmExternalMemoryCsuEccEmulationErrorCntrGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8    devNum;
    GT_U32   singleErrorFixedCounter;
    GT_U32   doubleErrorUnfixableCounter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

    result = cpssExMxPmExternalMemoryCsuEccEmulationErrorCntrGet(devNum,&singleErrorFixedCounter,&doubleErrorUnfixableCounter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",
        singleErrorFixedCounter,doubleErrorUnfixableCounter);

    return CMD_OK;
}


/*************************************************************************
* cpssExMxPmExternalMemoryCsuEccEmulationLastErrorIndexGet
*
* DESCRIPTION:
*       Function return the table type and the index of the last ECC error
*       encountered.
*
*       NOTEs:
*           1. This info not relate to the ECC info of the HW.
*               see function cpssExMxPmExternalMemoryLastErrorAddressGet(...)
*           2. The info is 'Clear by Read' (the function set 'not valid' to this
*              info at the end of function).
*               a. Application must synchronize the 'read' tables with this API
*                  to avoid non protected 'Clear by Read' action by this function.
*           3. The info updated on every error (both 'fixed' and 'unfixable').
*               meaning that if more then single error happened before application
*               called this API , the data about the last error returned.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - the device number
*
* OUTPUT:
*       wasErrorPtr - (pointer to) was there an error since function last called?
*                GT_TRUE - has new error info
*                GT_FALSE - no new error occurred since last call to this function
*       tableTypePtr  - (pointer to)the table indication , for tables that uses the CSU .
*                relevant only when *wasErrorPtr == GT_TRUE
*       entryIndexPtr - (pointer to)index in the table .
*                relevant only when *wasErrorPtr == GT_TRUE
*                    NOTE : considered as 'Csu-line' for the 'Extended TTI action'
*                           and 'Standard TTI action' tables (csu line from start
*                           of the TTI actions table).
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad device
*       GT_BAD_PTR - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. part of the ECC emulation in the CPSS.
*
*       2. All the CSU ECC emulations relevant only when the 'phase 1' init
*       parameter indicates that the CSU  used with ECC.
*       Meaning : CPSS_EXMXPM_PP_PHASE1_INIT_INFO_STC::controlSramProtect == CPSS_EXMXPM_CSU_PROTECT_ECC_E
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalMemoryCsuEccEmulationLastErrorIndexGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;
    GT_U8           devNum;
    GT_BOOL         wasError;
    CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLES_ENT   tableType;
    GT_U32          entryIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

    result = cpssExMxPmExternalMemoryCsuEccEmulationLastErrorIndexGet(devNum,&wasError,&tableType,&entryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",
        wasError,tableType,entryIndex);

    return CMD_OK;
}

/* current index to use for CSU Ecc Emulation Auto Correction */
static GT_U32   csuEccEmulationAutoCorrectionCurrentIndex = 0;

/* last index to use for CSU Ecc Emulation Auto Correction */
static GT_U32   csuEccEmulationAutoCorrectionLastIndex = 0;

enum{
    FULL_TABLE,/* - loop on all the table , look for ECC errors*/
    RANGE_INDEXES /*- loop on entries from start index to (include) last index , (ignor indexes outside the table)*/
};


/*************************************************************************
* cpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionTableEntry
*
* DESCRIPTION:
*       Function reads the specified CSU table entry and checks for ECC errors:
*           1. no errors --> function returns GT_OK
*           2. single error --> function fix it , and rewrite it back to HW
*               a. returns GT_OK
*               b. returns indication that the entry was fixed (see parameter
*                  entryWasFixedPtr)
*           3. two errors --> function returns GT_UNFIXABLE_ECC_ERROR
*
*       NOTE:
*           1. The function will do ECC calculation and emulate ECC corrections,
*              regardless to the mode given in cpssExMxPmExternalMemoryCsuEccEmulationModeSet(...)
*           2. function not returns the table's entry content.
*           3. The application can implement 'low priority' task that will loop
*              over the entries of the relevant tables and will call this function
*              to read , check and fix errors.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - the device number
*       tableType  - the table indication , for tables that uses the CSU .
*       entryIndex - index in the table .
*                    NOTE : considered as 'Csu-line' for the 'Extended TTI action'
*                           and 'Standard TTI action' tables (csu line from start
*                           of the TTI actions table).
*
* OUTPUT:
*       entryWasFixedPtr - (pointer to) indication that the CPSS fixed single
*                          ECC error in the line (and wrote the fixed entry to the HW)
*                          GT_TRUE - cpss fixed the entry due to single error
*                          GT_FALSE - no single error found , so not fixed
*
*
* RETURNS:
*       GT_OK - entry is ok or was fixed
*       GT_BAD_PARAM - bad device or bad table type or entry index out of range
*       GT_BAD_PTR - on NULL pointer
*       GT_HW_ERROR - on HW error (read/write)
*       GT_NOT_SUPPORTED - the table not supported in ECC mode , should be used only for parity mode.
*                          or table (entry type) not supported for current configuration
*                          or the CSU not set to ECC mode(see comment 2)
*       GT_UNFIXABLE_ECC_ERROR - if entry can't be fixed due to 2 data errors
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*       1. part of the ECC emulation in the CPSS.
*
*       2. All the CSU ECC emulations relevant only when the 'phase 1' init
*       parameter indicates that the CSU  used with ECC.
*       Meaning : CPSS_EXMXPM_PP_PHASE1_INIT_INFO_STC::controlSramProtect == CPSS_EXMXPM_CSU_PROTECT_ECC_E
*
*******************************************************************************/
static CMD_STATUS  wrCpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionGetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      rc;
    IN GT_U8            devNum;
    IN CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLES_ENT   tableType;
    OUT GT_BOOL         entryWasFixed;

    devNum = (GT_U8)inArgs[0];
    tableType = (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLES_ENT)inArgs[1];

    do{
        rc = cpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionTableEntry(devNum,tableType,
            csuEccEmulationAutoCorrectionCurrentIndex,&entryWasFixed);

        csuEccEmulationAutoCorrectionCurrentIndex++;

        if(rc == GT_UNFIXABLE_ECC_ERROR)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_OK, "%d%d",csuEccEmulationAutoCorrectionCurrentIndex-1, 2 );/* 2 errors found */
            return CMD_OK;
        }
        else if (rc == GT_BAD_PARAM)
        {
            /* end of table */
            break;
        }
        else if (rc != GT_OK)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, rc, "%d%d",0,rc);
            return CMD_OK;
        }

        /* rc == GT_OK */
        if(entryWasFixed == GT_TRUE)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_OK, "%d%d",csuEccEmulationAutoCorrectionCurrentIndex-1, 1 );/* 1 error found */
            return CMD_OK;
        }

    }while(csuEccEmulationAutoCorrectionLastIndex >= csuEccEmulationAutoCorrectionCurrentIndex);

    /* end of table */

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d",(GT_U32)(-1));
    return CMD_OK;
}

static CMD_STATUS  wrCpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(inArgs[2] == FULL_TABLE)
    {
        csuEccEmulationAutoCorrectionCurrentIndex = 0;
        csuEccEmulationAutoCorrectionLastIndex = 0xffffffff;
    }
    else  /*RANGE_INDEXES*/
    {
        csuEccEmulationAutoCorrectionCurrentIndex = inArgs[3];
        csuEccEmulationAutoCorrectionLastIndex = inArgs[4];
    }

    return wrCpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionGetEntry(
        inArgs,inFields,numFields,outArgs);
}


static CMD_STATUS  wrCpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    return wrCpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionGetEntry(
        inArgs,inFields,numFields,outArgs);
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxPmExternalMemoryParityErrorCntrGet",
     &wrCpssExMxPmExternalMemoryParityErrorCntrGet,
     2, 0},

    {"cpssExMxPmExternalMemoryCsuEccErrorCntrGet",
     &wrCpssExMxPmExternalMemoryCsuEccErrorCntrGet,
     1, 0},

    {"cpssExMxPmExternalMemoryLastErrorAddressGet",
     &wrCpssExMxPmExternalMemoryLastErrorAddressGet,
     2, 0},

    {"cpssExMxPmExternalMemoryCsuEccEmulationModeSet",
     &wrCpssExMxPmExternalMemoryCsuEccEmulationModeSet,
     3, 0},

    {"wrCpssExMxPmExternalMemoryCsuEccEmulationTableInfoGet",
     &wrCpssExMxPmExternalMemoryCsuEccEmulationTableInfoGet,
     2, 0},

    {"cpssExMxPmExternalMemoryCsuEccEmulationErrorCntrGet",
     &wrCpssExMxPmExternalMemoryCsuEccEmulationErrorCntrGet,
     1, 0},

    {"cpssExMxPmExternalMemoryCsuEccEmulationLastErrorIndexGet",
     &wrCpssExMxPmExternalMemoryCsuEccEmulationLastErrorIndexGet,
     1, 0},

    {"cpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionGetFirst",
        &wrCpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionGetFirst,
        5, 0},

    {"cpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionGetNext",
        &wrCpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionGetNext,
        5, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmExtMemory
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
GT_STATUS cmdLibInitCpssExMxPmExternalMemory
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

