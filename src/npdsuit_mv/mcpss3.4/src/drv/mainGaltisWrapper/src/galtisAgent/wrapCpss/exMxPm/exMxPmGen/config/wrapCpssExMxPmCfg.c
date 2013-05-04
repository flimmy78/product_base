/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmCfg.c
*
* DESCRIPTION:
*       Wrapper functions for cpssExMxPmCfg cpss.exMxPm functions
*
*       DEPENDENCIES:
*       None.
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
#include <cpss/exMxPm/exMxPmGen/config/cpssExMxPmCfg.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmHwTables.h>


/*******************************************************************************
* cpssExMxPmCfgPpLogicalInit
*
* DESCRIPTION:
*       packet processor initialization configuration function.
*       This function performs PP RAMs divisions to memory pools.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum - the device number
*       ppConfigPtr - (pointer to)includes PP specific logical initialization
*                     params.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - on wrong devNum
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory
*       GT_OUT_OF_PP_MEM  - the table sizes requested by application exceeds the
*                           PP RAM size
*       GT_BAD_PTR - on NULL pointer
*       GT_FAIL - on error
*
* COMMENTS:
*
*       1.  This function should perform Next PP initializations:
*               -   Ram structs init, and Ram size check.
*               -   Init the AU and FU (address update messages , FDB upload
*                   messages) mechanism according to the data.
*
*       Execution flow of function:
*
*
*             +-----------------+
*             | Init the   RAMs |   This initialization includes all structs
*             | conf. structs   |   fields but the base address field.
*             |                 |   The Rams are : all internal and external SRAMs.
*             +-----------------+
*                     |
*   (Ram is big       |                 +-------------------------+
*    enough to hold   +--(No)-----------| return GT_OUT_OF_PP_MEM |
*    all tables?)     |                 +-------------------------+
*                     |
*                   (Yes)
*                     |
*                     V
*             +-------------------+
*             | Set the Rams base |
*             | addr. according   |
*             | to the location   |
*             | fields.           |
*             +-------------------+
*                     |
*                     |
*                     V
*             +-----------------------+
*             | Init the internal CPSS|
*             | meta-data on RAM info |
*             | struct, according     |
*             | to the info in RAM    |
*             | conf. struct.         |
*             +-----------------------+
*                    |
*                    |
*                    V
*             +------------------------------+
*             | Init the AU and FU mechanism |
*             | according to the data        |
*             +------------------------------+
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmCfgPpLogicalInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum=0;
    CPSS_EXMXPM_PP_CONFIG_INIT_STC ppConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    /*TODO: add code here*/

    /* call cpss api function */
    result = cpssExMxPmCfgPpLogicalInit(devNum, &ppConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_NOT_IMPLEMENTED, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmCfgDevRemove
*
* DESCRIPTION:
*       Remove the device from the CPSS.
*       This function releases the DB of CPSS that is associated with the PP ,
*       and will "know" that the device is not longer valid.
*       This function will not do any HW actions, and therefore can be called
*       before or after the HW was actually removed.
*
*       After this function is called the devNum is no longer valid for any API
*       until this devNum is used in the initialization sequence for a "new" PP.
*
*       NOTE: 1. The application may want to disable the Traffic to the CPU , and
*             messages , prior to this operation (if the device still exists).
*             2. The application may want to a reset the HW PP , and there for need
*             to call the "hw reset" function (before calling this function)
*             and after calling this function to do the initialization
*             sequence all over again for the device.
*
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum - device number to remove.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmCfgDevRemove
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmCfgDevRemove(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmCfgDevEnableSet
*
* DESCRIPTION:
*       This routine enable/disable the device to/from processing any packets
*       While disabled no traffic is received/transmitted from/to networks ports
*       or uplink port
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum - device number to set.
*       enable - GT_TRUE device enable, GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmCfgDevEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmCfgDevEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmCfgDevEnableGet
*
* DESCRIPTION:
*       This routine get the state of PP , the device enabled/disabled to/from
*       processing any packets.
*       While disabled no traffic is received/transmitted from/to networks ports
*       or uplink port
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum - device number to set.
*
* OUTPUTS:
*       enablePtr - (pointer to) GT_TRUE device enable, GT_FALSE disable.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmCfgDevEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmCfgDevEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmCfgTableNumEntriesGet
*
*
* DESCRIPTION:
*       the function return the number of entries of each individual table in
*       the HW
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - physical device number
*       table        - type of the specific table
*
* OUTPUTS:
*       numEntriesPtr - (pointer to) number of entries
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmCfgTableNumEntriesGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_CFG_TABLES_ENT tableType;
    GT_U32 numEntries;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tableType = (CPSS_EXMXPM_CFG_TABLES_ENT)inArgs[1];
    /* call cpss api function */
    result = cpssExMxPmCfgTableNumEntriesGet(devNum, tableType, &numEntries);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", numEntries);

    return CMD_OK;
}

/*******************************************************************************
* prvCpssExMxPmTableEccEmulationAutoCorrectionTest_debug1
*
* DESCRIPTION:
*       Debug function :
*       Function read table entry from the HW , modify single/double bit  ,
*       and force the HW not to re-calculate the ECCs.
*       then the function read the entry and try to fix a single error or fail
*       on double errors.
*
* INPUTS:
*       devNum          - the device number
*       tableType       - the table in the CSU to get pointer to it's info
*       patternentryPtr - (pointer to) the pattern of data to write to the CSU
*       numErrors       - number of errors to generate (1 or 2)
*
* OUTPUTS:
*       entryWasFixedPtr - (pointer to) indication that the CPSS fixed single
*                          ECC error in the line (and wrote the fixed entry to the HW)
*                          GT_TRUE - cpss fixed the entry due to single error
*                          GT_FALSE - no single error found , so not fixed
*
* RETURNS:
*       GT_OK - entry is ok or was fixed
*       GT_BAD_PARAM - bad device or bad table type or entry index out of range
*       GT_BAD_PTR - on NULL pointer
*       GT_HW_ERROR - on HW error (read/write)
*       GT_NOT_SUPPORTED - the table not bound to controller ,
*                          or the CSU not set to ECC mode(see comment 2)
*       GT_UNFIXABLE_ECC_ERROR   - the CPSS detected ECC error that can't
*                                  be fixed when read from the memory that
*                                  protected by ECC generated.
*                                  if entry can't be fixed due to 2 data errors
*                                  NOTE: relevant only to memory controller that
*                                    work with ECC protection , and the CPSS need
*                                    to emulate ECC protection.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrPrvCpssExMxPmTableEccEmulationAutoCorrectionTest_debug1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32  ii;
    GT_U8 devNum;
    CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLES_ENT   tableType;
    GT_U32   patternEntry[8];
    GT_BOOL  entryWasFixed;
    GT_U32   numErrors;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tableType = (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLES_ENT)inArgs[1];
    numErrors = inArgs[2];

    for(ii = 0 ; ii < 8 ; ii++)
    {
        patternEntry[ii] = inArgs[2+ii];
    }

    /* call cpss api function */
    result = prvCpssExMxPmTableEccEmulationAutoCorrectionTest_debug1(devNum, tableType, patternEntry,&entryWasFixed,numErrors);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", entryWasFixed);

    return CMD_OK;
}

/*******************************************************************************
* prvCpssExMxPmTableEccEmulationTrustEccByCpuSet_debug
*
* DESCRIPTION:
*       debug function
*       Sets mgDataIntegrityGenDis bit in memory controller.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices
*
* INPUTS:
*       devNum  - device number
*       csu1OrCsu0     - CSU-1 or CSU-0
*                        GT_TRUE  - CSU-1
*                        GT_FALSE - CSU-0
*       trustEccByCpu  - GT_TRUE  - trust ECC by CPU
*                        GT_FALSE - don't trust ECC by CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS  wrPrvCpssExMxPmTableEccEmulationTrustEccByCpuSet_debug
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    IN GT_U8            devNum;
    IN GT_BOOL          csu1OrCsu0;
    IN GT_BOOL          trustEccByCpu;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    csu1OrCsu0 = (GT_BOOL)inArgs[1];
    trustEccByCpu = (GT_BOOL)inArgs[2];
    /* call cpss api function */
    result = prvCpssExMxPmTableEccEmulationTrustEccByCpuSet_debug(devNum, csu1OrCsu0, trustEccByCpu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmCfgPpLogicalInit",
         &wrCpssExMxPmCfgPpLogicalInit,
         0, 0},/* wrapper not implemented , since called during cpssInitSystem ... */
        {"cpssExMxPmCfgDevRemove",
         &wrCpssExMxPmCfgDevRemove,
         1, 0},
        {"cpssExMxPmCfgDevEnableSet",
         &wrCpssExMxPmCfgDevEnableSet,
         2, 0},
        {"cpssExMxPmCfgDevEnableGet",
         &wrCpssExMxPmCfgDevEnableGet,
         1, 0},

        {"cpssExMxPmCfgTableNumEntriesGet",
          &wrCpssExMxPmCfgTableNumEntriesGet,
          2, 0},

        {"prvCpssExMxPmTableEccEmulationAutoCorrectionTest_debug1",
          &wrPrvCpssExMxPmTableEccEmulationAutoCorrectionTest_debug1,
          11, 0},

        {"prvCpssExMxPmTableEccEmulationTrustEccByCpuSet_debug",
          &wrPrvCpssExMxPmTableEccEmulationTrustEccByCpuSet_debug,
          3, 0},


};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmCfg
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
GT_STATUS cmdLibInitCpssExMxPmCfg
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

