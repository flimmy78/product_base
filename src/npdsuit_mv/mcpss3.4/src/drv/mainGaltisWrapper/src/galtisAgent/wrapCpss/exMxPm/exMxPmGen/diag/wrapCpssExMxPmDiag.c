/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmDiag.c
*
* DESCRIPTION:
*       Wrapper functions for cpssExMxPmDiag cpss.exMxPm functions
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
#include <cpss/generic/diag/cpssDiag.h>
#include <cpss/exMxPm/exMxPmGen/diag/cpssExMxPmDiag.h>

/* CPSS OS services */
#include <cpss/extServices/private/prvCpssBindFunc.h>


/*******************************************************************************
* cpssExMxPmDiagMemTest
*
* DESCRIPTION:
*       Performs memory test on a specified memory location and size for a
*       specified memory type.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - The device number to test
*       memType     - The packet processor memory type to verify.
*       startOffset - The offset address to start the test from. The offset is
*                     from start of tested memory defined by memType.
*       size        - The memory size in byte to test (start from offset).
*       profile     - The test profile.
*
* OUTPUTS:
*       testStatusPtr  - GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr        - Address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr     - Contains the value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr    - Contains the value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong memory type
*       GT_FAIL      - on error
*
* COMMENTS:
*       The resolution of memory startOffset and size is in bytes.
*       The test is done by writing and reading a test pattern.
*       The function may be called after cpssExMxPmHwPpStartInit.
*       The test is destructive and leaves the memory corrupted.
*
*       Supported memories: All external and internal memories
*       The size of the memory should be according to entry size and not
*       allignment. Consider the memory as a sequential one.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagMemTest

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_EXMXPM_DIAG_PP_MEM_TYPE_ENT    memType;
    GT_U32                              startOffset;
    GT_U32                              size;
    CPSS_DIAG_TEST_PROFILE_ENT          profile;
    GT_BOOL                             testStatus;
    GT_U32                              addr;
    GT_U32                              readVal;
    GT_U32                              writeVal;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_EXMXPM_DIAG_PP_MEM_TYPE_ENT)(inArgs[1]);
    startOffset = inArgs[2];
    size = inArgs[3];
    profile = (CPSS_DIAG_TEST_PROFILE_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmDiagMemTest(devNum,
                                   memType,
                                   startOffset,
                                   size,
                                   profile,
                                   &testStatus,
                                   &addr,
                                   &readVal,
                                   &writeVal);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d",
                 testStatus, addr, readVal, writeVal);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagAllMemTest
*
* DESCRIPTION:
*       Performs memory test for all the internal and external memories.
*       Tested memories: All internal and external memories
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - The device number to test
*
* OUTPUTS:
*       testStatusPtr  - GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr        - Address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr     - Contains the value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr    - Contains the value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The test is done by invoking cpssExMxPmDiagMemTest in loop for all the
*       memory types and for AA-55, random and incremental patterns.
*       The function may be called after cpssExMxPmHwPpStartInit.
*       The test is destructive and leaves the memory corrupted.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagAllMemTest

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    GT_BOOL                             testStatus;
    GT_U32                              addr;
    GT_U32                              readVal;
    GT_U32                              writeVal;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    result = cpssExMxPmDiagAllMemTest(devNum, &testStatus, &addr,
                                      &readVal,&writeVal);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", testStatus, addr,
                 readVal, writeVal);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagMemWrite
*
* DESCRIPTION:
*       performs a single 32 bit data write to one of the PP memory spaces.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum  - The device number
*       memType - The packet processor memory type
*       offset  - The offset address to write to
*       data    - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       offset must be aligned to 4 Bytes.
*       The function may be called after cpssExMxPmHwPpStartInit.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagMemWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_DIAG_PP_MEM_TYPE_ENT memType;
    GT_U32 offset;
    GT_U32 data;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_EXMXPM_DIAG_PP_MEM_TYPE_ENT)inArgs[1];
    offset = (GT_U32)inArgs[2];
    data = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmDiagMemWrite(devNum, memType, offset, data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagMemRead
*
* DESCRIPTION:
*       performs a single 32 bit data read from one of the PP memory spaces.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum  - The device number
*       memType - The packet processor memory type
*       offset  - The offset address to read from
*
* OUTPUTS:
*       dataPtr - read data
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       offset must be aligned to 4 Bytes.
*       The function may be called after cpssExMxPmHwPpStartInit.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagMemRead
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_DIAG_PP_MEM_TYPE_ENT memType;
    GT_U32 offset;
    GT_U32 data;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_EXMXPM_DIAG_PP_MEM_TYPE_ENT)inArgs[1];
    offset = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmDiagMemRead(devNum, memType, offset, &data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", data);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagRegWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write to one of the PP PEX or
*       internal registers
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       regType    - The register type
*       offset     - the register offset
*       data       - data to write
*       doByteSwap - GT_TRUE:  byte swap will be done on the written data
*                    GT_FALSE: byte swap will not be done on the written data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagRegWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 baseAddr;
    CPSS_DIAG_PP_REG_TYPE_ENT regType;
    GT_U32 offset;
    GT_U32 data;
    GT_BOOL doByteSwap;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    baseAddr = (GT_U32)inArgs[0];
    regType = (CPSS_DIAG_PP_REG_TYPE_ENT)inArgs[1];
    offset = (GT_U32)inArgs[2];
    data = (GT_U32)inArgs[3];
    doByteSwap = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmDiagRegWrite(baseAddr, regType, offset, data, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagRegRead
*
* DESCRIPTION:
*       Performs single 32 bit data read from one of the PP PEX or
*       internal registers.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       regType    - The register type
*       offset     - the register offset
*       doByteSwap - GT_TRUE:  byte swap will be done on the read data
*                    GT_FALSE: byte swap will not be done on the read data
*
* OUTPUTS:
*       dataPtr    - read data
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagRegRead
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 baseAddr;
    CPSS_DIAG_PP_REG_TYPE_ENT regType;
    GT_U32 offset;
    GT_U32 data;
    GT_BOOL doByteSwap;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    baseAddr = (GT_U32)inArgs[0];
    regType = (CPSS_DIAG_PP_REG_TYPE_ENT)inArgs[1];
    offset = (GT_U32)inArgs[2];
    doByteSwap = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmDiagRegRead(baseAddr, regType, offset, &data, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", data);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagPhyRegWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write to one of the PHY registers.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       baseAddr     - The base address to access internal register by PEX
*       smiRegOffset - The SMI register offset
*       phyAddr      - phy address to access
*       offset       - PHY register offset
*       data         - data to write
*       doByteSwap   - GT_TRUE:  byte swap will be done on the written data
*                      GT_FALSE: byte swap will not be done on the written data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagPhyRegWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 baseAddr;
    GT_U32 smiRegOffset;
    GT_U32 phyAddr;
    GT_U32 offset;
    GT_U32 data;
    GT_BOOL doByteSwap;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    baseAddr = (GT_U32)inArgs[0];
    smiRegOffset = (GT_U32)inArgs[1];
    phyAddr = (GT_U32)inArgs[2];
    offset = (GT_U32)inArgs[3];
    data = (GT_U32)inArgs[4];
    doByteSwap = (GT_BOOL)inArgs[5];

    /* call cpss api function */
    result = cpssExMxPmDiagPhyRegWrite(baseAddr, smiRegOffset, phyAddr, offset, data, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagPhyRegRead
*
* DESCRIPTION:
*       Performs single 32 bit data read from one of the PHY registers.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       baseAddr     - The base address to access internal register by PEX
*       smiRegOffset - The SMI register offset
*       phyAddr      - phy address to access
*       offset       - PHY register offset
*       doByteSwap   - GT_TRUE:  byte swap will be done on the read data
*                      GT_FALSE: byte swap will not be done on the read data
*
* OUTPUTS:
*       dataPtr    - read data
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagPhyRegRead
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 baseAddr;
    GT_U32 smiRegOffset;
    GT_U32 phyAddr;
    GT_U32 offset;
    GT_U32 data;
    GT_BOOL doByteSwap;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    baseAddr = (GT_U32)inArgs[0];
    smiRegOffset = (GT_U32)inArgs[1];
    phyAddr = (GT_U32)inArgs[2];
    offset = (GT_U32)inArgs[3];
    doByteSwap = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmDiagPhyRegRead(baseAddr, smiRegOffset, phyAddr, offset, &data, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", data);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagRegsNumGet
*
* DESCRIPTION:
*       Gets the number of registers for the PP.
*       Used to allocate memory for cpssExMxPmDiagRegsDump.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum        - The device number
*
* OUTPUTS:
*       regsNumPtr    - number of registers
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagRegsNumGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 regsNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmDiagRegsNumGet(devNum, CPSS_EXMXPM_DIAG_ALL_UNIT_TYPE_E, &regsNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", regsNum);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagRegsDump
*
* DESCRIPTION:
*       Dumps the device register addresses and values.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum        - The device number
*       regsNumPtr    - Number of registers to dump. This number must not be
*                       bigger than the number of registers that can be dumped
*                       (starting at offset).
*       offset        - The first register address to dump
*
* OUTPUTS:
*       regsNumPtr    - Number of registers that were dumped
*       regAddrPtr    - The addresses of the dumped registers
*       regDataPtr    - The data in the dumped registers
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called after cpssExMxPmHwPpStartInit.
*       To dump all the registers the user may call cpssExMxPmDiagRegsNumGet in
*       order to get the number of registers of the PP.
*
*******************************************************************************/

 static GT_U32 *regAddrPtr = NULL;
 static GT_U32 *regDataPtr = NULL;
 static GT_U32 index;

static CMD_STATUS wrCpssExMxPmDiagRegsDumpGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    GT_U32              regsNum;
    GT_U32              offset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    regsNum = inArgs[1];
    offset = inArgs[2];

    regAddrPtr = (GT_U32*)cmdOsMalloc(regsNum * (sizeof(GT_U32)));
    regDataPtr = (GT_U32*)cmdOsMalloc(regsNum * (sizeof(GT_U32)));
    index = 0;

    if(regAddrPtr == NULL || regDataPtr == NULL)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "%d", -1);
        return CMD_AGENT_ERROR;
    }

    result = cpssExMxPmDiagRegsDump(devNum,
                                    &regsNum,
                                    offset,
                                    CPSS_EXMXPM_DIAG_ALL_UNIT_TYPE_E,
                                    regAddrPtr,
                                    regDataPtr);

    inFields[0] = regAddrPtr[index];
    inFields[1] = regDataPtr[index];
    inFields[2] = regsNum;

    index++;

    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagRegsDumpGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32              regsNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    regsNum = inArgs[1];

    if (index == regsNum)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = regAddrPtr[index];
    inFields[1] = regDataPtr[index];


    index++;

    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagRegsNumGet
*
* DESCRIPTION:
*       Gets the number of registers for the PP.
*       Used to allocate memory for cpssExMxPmDiagRegsDump.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum        - The device number
*
* OUTPUTS:
*       regsNumPtr    - number of registers
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagUnitRegsNumGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 regsNum;
    CPSS_EXMXPM_DIAG_UNIT_TYPE_ENT unitId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    unitId = (CPSS_EXMXPM_DIAG_UNIT_TYPE_ENT)inArgs[1];


    /* call cpss api function */
    result = cpssExMxPmDiagRegsNumGet(devNum, unitId, &regsNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", regsNum);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagRegsDump
*
* DESCRIPTION:
*       Dumps the device register addresses and values.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum        - The device number
*       regsNumPtr    - Number of registers to dump. This number must not be
*                       bigger than the number of registers that can be dumped
*                       (starting at offset).
*       offset        - The first register address to dump
*
* OUTPUTS:
*       regsNumPtr    - Number of registers that were dumped
*       regAddrPtr    - The addresses of the dumped registers
*       regDataPtr    - The data in the dumped registers
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called after cpssExMxPmHwPpStartInit.
*       To dump all the registers the user may call cpssExMxPmDiagRegsNumGet in
*       order to get the number of registers of the PP.
*
*******************************************************************************/

static GT_U32 *regUnitAddrPtr = NULL;
static GT_U32 *regUnitDataPtr = NULL;
static GT_U32 indexUnit;

static CMD_STATUS wrCpssExMxPmDiagUnitRegsDumpGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    GT_U32              regsNum;
    GT_U32              offset;
    CPSS_EXMXPM_DIAG_UNIT_TYPE_ENT unitId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    regsNum = inArgs[1];
    offset = inArgs[2];
    unitId = (CPSS_EXMXPM_DIAG_UNIT_TYPE_ENT)inArgs[3];

    regUnitAddrPtr = (GT_U32*)cmdOsMalloc(regsNum * (sizeof(GT_U32)));
    regUnitDataPtr = (GT_U32*)cmdOsMalloc(regsNum * (sizeof(GT_U32)));
    indexUnit = 0;

    if(regUnitAddrPtr == NULL || regUnitDataPtr == NULL)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "%d", -1);
        return CMD_AGENT_ERROR;
    }

    result = cpssExMxPmDiagRegsDump(devNum,
                                    &regsNum,
                                    offset,
                                    unitId,
                                    regUnitAddrPtr,
                                    regUnitDataPtr);

    inFields[0] = regUnitAddrPtr[indexUnit];
    inFields[1] = regUnitDataPtr[indexUnit];
    inFields[2] = regsNum;

    indexUnit++;

    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagUnitRegsDumpGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32              regsNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    regsNum = inArgs[1];

    if (indexUnit == regsNum)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = regUnitAddrPtr[indexUnit];
    inFields[1] = regUnitDataPtr[indexUnit];


    indexUnit++;

    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/* Dump all registers in given unit */
/*************************************************************************/
GT_STATUS cpssExMxPmDiagRegsDumpUnit
(
    GT_32 inDevNum,
    CPSS_EXMXPM_DIAG_UNIT_TYPE_ENT unitId
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    GT_U32              regsNum, i;
    GT_U32              offset = 0;
    GT_U32              *regAddrPtr = NULL;
    GT_U32              *regDataPtr = NULL;

    /* map input arguments to locals */
    devNum = (GT_U8)inDevNum;

    result = cpssExMxPmDiagRegsNumGet(devNum, unitId, &regsNum);

    cpssOsPrintf("Reg_Num = %d\n", regsNum);

    if(result != GT_OK)
    {
        return result;
    }

    regAddrPtr = (GT_U32*)cpssOsMalloc(regsNum * (sizeof(GT_U32)));
    regDataPtr = (GT_U32*)cpssOsMalloc(regsNum * (sizeof(GT_U32)));

    if(regAddrPtr == NULL || regDataPtr == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    result = cpssExMxPmDiagRegsDump(devNum,
                                    &regsNum,
                                    offset,
                                    unitId,
                                    regAddrPtr,
                                    regDataPtr);

    if(result != GT_OK)
    {
        return result;
    }

    for(i = 0 ; i < regsNum; i++)
    {
       if((i % 10) == 0)
       {
           /* sleep to give UART time */
           cpssOsTimerWkAfter(1);
       }

       cpssOsPrintf("0x%08X  0x%08X\n", regAddrPtr[i], regDataPtr[i]);
    }

    return GT_OK;
}

/* dump all regs procedure */
/*******************************************************************************/
int cpssExMxPmRegsDump
(
    GT_32 inDevNum
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    GT_U32              regsNum, ii;
    GT_U32              offset = 0;
    GT_U32              *regAddrPtr = NULL;
    GT_U32              *regDataPtr = NULL;

    /* map input arguments to locals */
    devNum = (GT_U8)inDevNum;

    result = cpssExMxPmDiagRegsNumGet(devNum, CPSS_EXMXPM_DIAG_ALL_UNIT_TYPE_E, &regsNum);

    if(result != GT_OK)
    {
        return result;
    }

    regAddrPtr = (GT_U32*)cmdOsMalloc(regsNum * (sizeof(GT_U32)));
    regDataPtr = (GT_U32*)cmdOsMalloc(regsNum * (sizeof(GT_U32)));

    if(regAddrPtr == NULL || regDataPtr == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    result = cpssExMxPmDiagRegsDump(devNum,
                                    &regsNum,
                                    offset,
                                    CPSS_EXMXPM_DIAG_ALL_UNIT_TYPE_E,
                                    regAddrPtr,
                                    regDataPtr);

    if(result != GT_OK)
    {
        return result;
    }

    cmdOsPrintf("Dump registers for device %d\n", devNum);

    for(ii = 0 ; ii < regsNum; ii++)
    {
       if((ii%10) == 0)
       {
           /* sleep to give UART time */
           cmdOsTimerWkAfter(1);
       }

       cmdOsPrintf("0x%08X  0x%08X\n", regAddrPtr[ii], regDataPtr[ii]);
    }
    return GT_OK;
}


/*******************************************************************************
* cpssExMxPmDiagRegTest
*
* DESCRIPTION:
*       Tests the device read/write ability of a specific register.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum        - The device number
*       regAddr       - Register to test
*       regMask       - Register mask. The test verifies only the non-masked
*                       bits.
*       profile       - The test profile
*
* OUTPUTS:
*       testStatusPtr - GT_TRUE if the test succeeded or GT_FALSE for failure
*       readValPtr    - The value read from the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       writeValPtr   - The value written to the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*       After the register is tested, the original value prior to the test is
*       restored.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagRegTest
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 regAddr;
    GT_U32 regMask;
    CPSS_DIAG_TEST_PROFILE_ENT profile;
    GT_BOOL testStatus;
    GT_U32 readVal;
    GT_U32 writeVal;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    regAddr = (GT_U32)inArgs[1];
    regMask = (GT_U32)inArgs[2];
    profile = (CPSS_DIAG_TEST_PROFILE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmDiagRegTest(devNum, regAddr, regMask, profile, &testStatus, &readVal, &writeVal);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", testStatus, readVal, writeVal);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagAllRegTest
*
* DESCRIPTION:
*       Tests the device read/write ability of all the registers.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum        - The device number
*
* OUTPUTS:
*       testStatusPtr - GT_TRUE if the test succeeded or GT_FALSE for failure
*       badRegPtr     - Address of the register which caused the failure if
*                       testStatusPtr is GT_FALSE. Irrelevant if testStatusPtr
*                       is GT_TRUE.
*       readValPtr    - The value read from the register which caused the
*                       failure if testStatusPtr is GT_FALSE. Irrelevant if
*                       testStatusPtr is GT_TRUE
*       writeValPtr   - The value written to the register which caused the
*                       failure if testStatusPtr is GT_FALSE. Irrelevant if
*                       testStatusPtr is GT_TRUE
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The test is done by invoking cpssExMxPmDiagRegTest in loop for all the
*       diagnostics registers and for all the patterns.
*       After each register is tested, the original value prior to the test is
*       restored.
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagAllRegTest
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL testStatus;
    GT_U32 badReg;
    GT_U32 readVal;
    GT_U32 writeVal;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmDiagAllRegTest(devNum, &testStatus, &badReg, &readVal, &writeVal);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", testStatus, badReg, readVal, writeVal);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagPrbsPortTransmitModeSet
*
* DESCRIPTION:
*       Set transmit mode for specified port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       portType - port type
*       portNum  - Tri-Speed, XAUI or Fabric Hyper G. Link physical port number
*       laneNum  - lane number, relevant only for XAUI and HyperG.Link ports
*       mode     - transmit mode
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device, mode, laneNum, portType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port doesn't support the transmit mode.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagPrbsPortTransmitModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    CPSS_EXMXPM_DIAG_PORT_TYPE_ENT     portType;
    GT_U8                              portNum;
    GT_U32                             laneNum;
    CPSS_EXMXPM_DIAG_TRANSMIT_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portType = (CPSS_EXMXPM_DIAG_PORT_TYPE_ENT)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    laneNum = inArgs[3];
    mode = (CPSS_EXMXPM_DIAG_TRANSMIT_MODE_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmDiagPrbsPortTransmitModeSet(devNum, portType, portNum, laneNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagPrbsPortTransmitModeGet
*
* DESCRIPTION:
*       Get transmit mode for specified port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       portType - port type
*       portNum  - Tri-Speed, XAUI or Fabric Hyper G. Link physical port number
*       laneNum  - lane number, relevant only for XAUI and HyperG.Link ports
*
* OUTPUTS:
*       modePtr  - (pointer to) transmit mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device, laneNum, portType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port doesn't support the transmit mode.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagPrbsPortTransmitModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    CPSS_EXMXPM_DIAG_PORT_TYPE_ENT     portType;
    GT_U8                              portNum;
    GT_U32                             laneNum;
    CPSS_EXMXPM_DIAG_TRANSMIT_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portType = (CPSS_EXMXPM_DIAG_PORT_TYPE_ENT)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    laneNum = inArgs[3];
    
    /* call cpss api function */
    result = cpssExMxPmDiagPrbsPortTransmitModeGet(devNum, portType, portNum, laneNum, &mode);
    
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagPrbsPortGenerateEnableSet
*
* DESCRIPTION:
*       Enable/Disable PRBS (Pseudo Random Bit Generator) pattern generation 
*       per Lane.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       portType - port type
*       portNum  - Tri-Speed, XAUI or Fabric Hyper G. Link physical port number
*       laneNum  - lane number, relevant only for XAUI and HyperG.Link ports
*       enable   - GT_TRUE - PRBS pattern generation is enabled
*                  GT_FALSE - PRBS pattern generation is disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device, laneNum, portType
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagPrbsPortGenerateEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_EXMXPM_DIAG_PORT_TYPE_ENT  portType;
    GT_U8                           portNum;
    GT_U32                          laneNum;
    GT_BOOL                         enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portType = (CPSS_EXMXPM_DIAG_PORT_TYPE_ENT)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    laneNum = inArgs[3];
    enable = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmDiagPrbsPortGenerateEnableSet(devNum, portType, portNum, laneNum, enable);
    
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagPrbsGenerateEnableGet
*
* DESCRIPTION:
*       Get the status of PRBS (Pseudo Random Bit Generator) pattern generation 
*       per Lane.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       portType - port type
*       portNum  - Tri-Speed, XAUI or Fabric Hyper G. Link physical port number
*       laneNum  - lane number, relevant only for XAUI and HyperG.Link ports
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - PRBS pattern generation is enabled
*                     GT_FALSE - PRBS pattern generation is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device, laneNum, portType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagPrbsPortGenerateEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_EXMXPM_DIAG_PORT_TYPE_ENT  portType;
    GT_U8                           portNum;
    GT_U32                          laneNum;
    GT_BOOL                         enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portType = (CPSS_EXMXPM_DIAG_PORT_TYPE_ENT)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    laneNum = inArgs[3];
    
    /* call cpss api function */
    result = cpssExMxPmDiagPrbsPortGenerateEnableGet(devNum, portType, portNum, laneNum, &enable);
    
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagPrbsPortCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable PRBS (Pseudo Random Bit Generator) checker per port and
*       per lane.
*       When the checker is enabled, it seeks to lock onto the incoming bit
*       stream, and once this is achieved the PRBS checker starts counting the
*       number of bit errors. Tne number of errors can be retrieved by 
*       cpssExMxPmDiagPrbsErrCountGet API.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       portType - port type
*       portNum  - Tri-Speed, XAUI or Fabric Hyper G. Link physical port number
*       laneNum  - lane number, relevant only for XAUI and HyperG.Link ports
*       enable   - GT_TRUE - PRBS checker is enabled
*                  GT_FALSE - PRBS checker is disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device, laneNum, portType
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagPrbsPortCheckEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    IN   GT_U8                          devNum;
    IN   CPSS_EXMXPM_DIAG_PORT_TYPE_ENT portType;
    IN   GT_U8                          portNum;
    IN   GT_U32                         laneNum;
    IN   GT_BOOL                        enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portType = (CPSS_EXMXPM_DIAG_PORT_TYPE_ENT)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    laneNum = inArgs[3];
    enable = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmDiagPrbsPortCheckEnableSet(devNum, portType, portNum, laneNum, enable);
    
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagPrbsPortCheckEnableGet
*
* DESCRIPTION:
*       Get the status (enabled or disabled) of PRBS (Pseudo Random Bit Generator)
*       checker per port and per lane.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       portType - port type
*       portNum  - Tri-Speed, XAUI or Fabric Hyper G. Link physical port number
*       laneNum  - lane number, relevant only for XAUI and HyperG.Link ports
*
* OUTPUTS:
*       enablePtr   - (pointer to) PRBS checker state.
*                     GT_TRUE - PRBS checker is enabled
*                     GT_FALSE - PRBS checker is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device, laneNum, portType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagPrbsPortCheckEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_EXMXPM_DIAG_PORT_TYPE_ENT  portType;
    GT_U8                           portNum;
    GT_U32                          laneNum;
    GT_BOOL                         enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portType = (CPSS_EXMXPM_DIAG_PORT_TYPE_ENT)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    laneNum = inArgs[3];
    
    /* call cpss api function */
    result = cpssExMxPmDiagPrbsPortCheckEnableGet(devNum, portType, portNum, laneNum, &enable);
    
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagPrbsPortCheckReadyGet
*
* DESCRIPTION:
*       Get the PRBS checker ready status.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       isReadyPtr  - (pointer to) PRBS checker state.
*                     GT_TRUE - PRBS checker is ready.
*                     PRBS checker has completed the initialization phase.
*                     GT_FALSE - PRBS checker is not ready.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the Tri-speed ports.
*       The Check ready status indicates that the PRBS checker has completed
*       the initialization phase. The PRBS generator at the transmit side may
*       be enabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagPrbsPortCheckReadyGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U8       portNum;
    GT_BOOL     isReady;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    
    /* call cpss api function */
    result = cpssExMxPmDiagPrbsPortCheckReadyGet(devNum, portNum, &isReady);
    
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isReady);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagPrbsPortStatusGet
*
* DESCRIPTION:
*       Get PRBS (Pseudo Random Bit Generator) Error Counter and Checker Locked
*       status per port and per lane.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - device number
*       portType    - port type
*       portNum     - Tri-Speed, XAUI or Fabric Hyper G. Link physical port number
*       laneNum     - lane number, relevant only for XAUI and HyperG.Link ports
*
* OUTPUTS:
*       checkerLockedPtr        - (pointer to) checker locked state.
*                                 GT_TRUE - checker is locked on the sequence stream.
*                                 GT_FALSE - checker isn't locked on the sequence
*                                           stream.
*       errorCntrPtr            - (pointer to) PRBS Error counter. This counter represents
*                                 the number of bit mismatches detected since
*                                 the PRBS checker of the port has locked.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device, laneNum, portType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PRBS Error counter is cleared on read.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagPrbsPortStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    CPSS_EXMXPM_DIAG_PORT_TYPE_ENT     portType;
    GT_U8       portNum;
    GT_U32      laneNum;
    GT_BOOL     checkerLocked;
    GT_U32      errorCntr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portType = (CPSS_EXMXPM_DIAG_PORT_TYPE_ENT)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    laneNum = inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmDiagPrbsPortStatusGet(devNum, portType, portNum, laneNum, &checkerLocked, &errorCntr);
    
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", checkerLocked, errorCntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagPrbsCyclicDataSet
*
* DESCRIPTION:
*       Set cylic data for transmition. See cpssExMxPmDiagPrbsPortTransmitModeSet.
*
* APPLICABLE DEVICES:  ExMxPm and above
*
* INPUTS:
*       devNum          - device number
*       portType        - port type
*       portNum         - physical port number
*       laneNum         - lane number, relevant only for XAUI and HyperG.Link ports
*       cyclicDataArr[4] - cyclic data array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device, laneNum, portType
*       GT_NOT_SUPPORTED         - request is not supported for this port type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the XAUI/HyperG Link ports.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagPrbsCyclicDataSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    CPSS_EXMXPM_DIAG_PORT_TYPE_ENT     portType;
    GT_U8           portNum;
    GT_U32          laneNum;
    GT_U32          cyclicDataArr[4];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portType = (CPSS_EXMXPM_DIAG_PORT_TYPE_ENT)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    laneNum = inArgs[3];
    cyclicDataArr[0] = (GT_U32)inArgs[4];
    cyclicDataArr[1] = (GT_U32)inArgs[5];
    cyclicDataArr[2] = (GT_U32)inArgs[6];
    cyclicDataArr[3] = (GT_U32)inArgs[7];

    /* call cpss api function */
    result = cpssExMxPmDiagPrbsCyclicDataSet(devNum, portType, portNum, laneNum, cyclicDataArr );
    
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDiagPrbsCyclicDataGet
*
* DESCRIPTION:
*       Get cylic data for transmition. See cpssExMxPmDiagPrbsPortTransmitModeSet.
*
* APPLICABLE DEVICES:  ExMxPm and above
*
* INPUTS:
*       devNum          - device number
*       portType        - port type
*       portNum         - physical port number
*       laneNum         - lane number, relevant only for XAUI and HyperG.Link ports
*
* OUTPUTS:
*       cyclicDataArr[4]  - cyclic data array
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device, laneNum, portType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this port type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the XAUI/HyperG Link ports.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDiagPrbsCyclicDataGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    CPSS_EXMXPM_DIAG_PORT_TYPE_ENT     portType;
    GT_U8           portNum;
    GT_U32          laneNum;
    GT_U32          cyclicDataArr[4];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portType = (CPSS_EXMXPM_DIAG_PORT_TYPE_ENT)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    laneNum = inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmDiagPrbsCyclicDataGet(devNum, portType, portNum, laneNum, cyclicDataArr );
    
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", cyclicDataArr[0], cyclicDataArr[1], cyclicDataArr[2], cyclicDataArr[3]);

    return CMD_OK;    
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmDiagMemTest",
         &wrCpssExMxPmDiagMemTest,
         5, 0},
        {"cpssExMxPmDiagAllMemTest",
         &wrCpssExMxPmDiagAllMemTest,
         1, 0},
        {"cpssExMxPmDiagMemWrite",
         &wrCpssExMxPmDiagMemWrite,
         4, 0},
        {"cpssExMxPmDiagMemRead",
         &wrCpssExMxPmDiagMemRead,
         3, 0},
        {"cpssExMxPmDiagRegWrite",
         &wrCpssExMxPmDiagRegWrite,
         5, 0},
        {"cpssExMxPmDiagRegRead",
         &wrCpssExMxPmDiagRegRead,
         4, 0},
        {"cpssExMxPmDiagPhyRegWrite",
         &wrCpssExMxPmDiagPhyRegWrite,
         6, 0},
        {"cpssExMxPmDiagPhyRegRead",
         &wrCpssExMxPmDiagPhyRegRead,
         5, 0},
        {"cpssExMxPmDiagRegsNumGet",
         &wrCpssExMxPmDiagRegsNumGet,
         1, 0},
        {"cpssExMxPmDiagUnitRegsNumGet",
         &wrCpssExMxPmDiagUnitRegsNumGet,
         2, 0},
        {"cpssExMxPmDiagRegsDumpGetFirst",
         &wrCpssExMxPmDiagRegsDumpGetFirst,
         3, 0},
        {"cpssExMxPmDiagRegsDumpGetNext",
         &wrCpssExMxPmDiagRegsDumpGetNext,
         3, 0},
        {"cpssExMxPmDiagUnitRegsDumpGetFirst",
         &wrCpssExMxPmDiagUnitRegsDumpGetFirst,
         4, 0},
        {"cpssExMxPmDiagUnitRegsDumpGetNext",
         &wrCpssExMxPmDiagUnitRegsDumpGetNext,
         4, 0},
        {"cpssExMxPmDiagRegTest",
         &wrCpssExMxPmDiagRegTest,
         4, 0},
        {"cpssExMxPmDiagAllRegTest",
         &wrCpssExMxPmDiagAllRegTest,
         1, 0},
        {"cpssExMxPmDiagPrbsPortTransmitModeSet",
         &wrCpssExMxPmDiagPrbsPortTransmitModeSet,
         5, 0},
        {"cpssExMxPmDiagPrbsPortTransmitModeGet",
         &wrCpssExMxPmDiagPrbsPortTransmitModeGet,
         4, 0},
        {"cpssExMxPmDiagPrbsPortGenerateEnableSet",
         &wrCpssExMxPmDiagPrbsPortGenerateEnableSet,
         5, 0},
        {"cpssExMxPmDiagPrbsPortGenerateEnableGet",
         &wrCpssExMxPmDiagPrbsPortGenerateEnableGet,
         4, 0},
        {"cpssExMxPmDiagPrbsPortCheckEnableSet",
         &wrCpssExMxPmDiagPrbsPortCheckEnableSet,
         5, 0},
        {"cpssExMxPmDiagPrbsPortCheckEnableGet",
         &wrCpssExMxPmDiagPrbsPortCheckEnableGet,
         4, 0},
        {"cpssExMxPmDiagPrbsPortCheckReadyGet",
         &wrCpssExMxPmDiagPrbsPortCheckReadyGet,
         2, 0},
        {"cpssExMxPmDiagPrbsPortStatusGet",
         &wrCpssExMxPmDiagPrbsPortStatusGet,
         4, 0},
        {"cpssExMxPmDiagPrbsCyclicDataSet",
         &wrCpssExMxPmDiagPrbsCyclicDataSet,
         8, 0},
        {"cpssExMxPmDiagPrbsCyclicDataGet",
         &wrCpssExMxPmDiagPrbsCyclicDataGet,
         4, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmDiag
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
GT_STATUS cmdLibInitCpssExMxPmDiag
(
GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


