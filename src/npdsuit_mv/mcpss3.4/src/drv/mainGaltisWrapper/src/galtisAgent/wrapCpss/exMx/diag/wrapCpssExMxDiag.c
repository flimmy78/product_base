
/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxDiag.c
*
* DESCRIPTION:
*       Wrapper functions for diag cpss functions.
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
#include <cpss/exMx/exMxGen/diag/cpssExMxDiag.h>

/*******************************************************************************
* cpssExMxDiagMemTest
*
* DESCRIPTION:
*       Performs memory test on a specified memory location and size for a
*       specified memory type.
*
* APPLICABLE DEVICES:  All EX/MX devices
*
* INPUTS:
*       devNum      - The device number to test
*       memType     - The packet processor memory type to verify.
*       startOffset - The offset address to start the test from.
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
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The test is done by writing and reading a test pattern.
*       startOffset must be aligned to 4 Bytes and size must be in 4 bytes
*       resolution.
*       The function may be called after Phase 2 initialization.
*       The test is destructive and leaves the memory corrupted.
*       Supported memories:
*       - Buffer DRAM
*       - Flow DRAM (only where exists)
*       - Narrow SRAM
*       - Wide SRAM
*       - MAC table memory
*       - VLAN table memory
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxDiagMemTest

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                   result;

    GT_U8                       devNum;
	CPSS_DIAG_PP_MEM_TYPE_ENT   memType;
	GT_U32                      startOffset;
	GT_U32                      size;
	CPSS_DIAG_TEST_PROFILE_ENT  profile;
	GT_BOOL                     testStatusPtr;
	GT_U32                      addrPtr;
	GT_U32                      readValPtr;
	GT_U32                      writeValPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
	memType = (CPSS_DIAG_PP_MEM_TYPE_ENT)inArgs[1];
	startOffset = (GT_U32)inArgs[2];
	size = (GT_U32)inArgs[3];
	profile = (CPSS_DIAG_TEST_PROFILE_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssExMxDiagMemTest(devNum, memType, startOffset, size, profile, 
                         &testStatusPtr, &addrPtr, &readValPtr, &writeValPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", testStatusPtr, addrPtr, 
                                             readValPtr, writeValPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxDiagAllMemTest
*
* DESCRIPTION:
*       Performs memory test for all the internal and external memories.
*       Tested memories:
*       - Buffer DRAM
*       - Flow DRAM (only where exists)
*       - Narrow SRAM
*       - Wide SRAM
*       - MAC table memory
*       - VLAN table memory
*
* APPLICABLE DEVICES:  All EX/MX devices
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
*       The test is done by invoking cpssExMxDiagMemTest in loop for all the
*       memory types and for AA-55, random and incremental patterns.
*       The function may be called after Phase 2 initialization.
*       The test is destructive and leaves the memory corrupted.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxDiagAllMemTest

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                   result;

    GT_U8                       devNum;
	GT_BOOL                     testStatusPtr;
	GT_U32                      addrPtr;
	GT_U32                      readValPtr;
	GT_U32                      writeValPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxDiagAllMemTest(devNum, &testStatusPtr, &addrPtr, 
                                           &readValPtr, &writeValPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", testStatusPtr, addrPtr, 
                                             readValPtr, writeValPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxDiagMemWrite
*
* DESCRIPTION:
*       performs a single 32 bit data write to one of the PP memory spaces.
*
* APPLICABLE DEVICES:  All EX/MX devices
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
*       The function may be called after Phase 2 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxDiagMemWrite

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                   result;

    GT_U8                       devNum;
	CPSS_DIAG_PP_MEM_TYPE_ENT   memType;
	GT_U32                      offset;
	GT_U32                      data;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DIAG_PP_MEM_TYPE_ENT)inArgs[1];
    offset = (GT_U32)inArgs[2];
    data = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxDiagMemWrite(devNum, memType, offset, data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxDiagMemRead
*
* DESCRIPTION:
*       performs a single 32 bit data read from one of the PP memory spaces.
*
* APPLICABLE DEVICES:  All EX/MX devices
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
*       The function may be called after Phase 2 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxDiagMemRead

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                   result;

    GT_U8                       devNum;
	CPSS_DIAG_PP_MEM_TYPE_ENT   memType;
	GT_U32                      offset;
	GT_U32                      dataPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DIAG_PP_MEM_TYPE_ENT)inArgs[1];
    offset = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxDiagMemRead(devNum, memType, offset, &dataPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dataPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxDiagRegWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write to one of the PP PCI configuration or
*       internal registers.
*
* APPLICABLE DEVICES:  All EX/MX devices
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
static CMD_STATUS wrCpssExMxDiagRegWrite

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                   result;

    GT_U32                      baseAddr;
	CPSS_DIAG_PP_REG_TYPE_ENT   regType;
	GT_U32                      offset;
	GT_U32                      data;
	GT_BOOL                     doByteSwap;

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
    result = cpssExMxDiagRegWrite(baseAddr, regType, offset, data, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxDiagRegRead
*
* DESCRIPTION:
*       Performs single 32 bit data read from one of the PP PCI configuration or
*       internal registers.
*
* APPLICABLE DEVICES:  All EX/MX devices
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
static CMD_STATUS wrCpssExMxDiagRegRead

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                   result;

    GT_U32                      baseAddr;
	CPSS_DIAG_PP_REG_TYPE_ENT   regType;
	GT_U32                      offset;
	GT_U32                      dataPtr;
	GT_BOOL                     doByteSwap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    baseAddr = (GT_U32)inArgs[0];
    regType = (CPSS_DIAG_PP_REG_TYPE_ENT)inArgs[1];
    offset = (GT_U32)inArgs[2];
    doByteSwap = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxDiagRegRead(baseAddr, regType, offset, 
                                      &dataPtr, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dataPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxDiagPhyRegWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write to one of the PHY registers.
*
* APPLICABLE DEVICES:  All EX/MX devices
*
* INPUTS:
*       baseAddr     - The base address to access the device
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
static CMD_STATUS wrCpssExMxDiagPhyRegWrite

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                   result;

    GT_U32                      baseAddr;
	GT_U32                      smiRegOffset;
	GT_U32                      phyAddr;
	GT_U32                      offset;
	GT_U32                      data;
	GT_BOOL                     doByteSwap;

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
    result = cpssExMxDiagPhyRegWrite(baseAddr, smiRegOffset, phyAddr, 
                                            offset, data, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxDiagPhyRegRead
*
* DESCRIPTION:
*       Performs single 32 bit data read from one of the PHY registers.
*
* APPLICABLE DEVICES:  All EX/MX devices
*
* INPUTS:
*       baseAddr     - The base address to access the device
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
static CMD_STATUS wrCpssExMxDiagPhyRegRead

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                   result;

    GT_U32                      baseAddr;
	GT_U32                      smiRegOffset;
	GT_U32                      phyAddr;
	GT_U32                      offset;
	GT_U32                      dataPtr;
	GT_BOOL                     doByteSwap;

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
    result = cpssExMxDiagPhyRegRead(baseAddr, smiRegOffset, phyAddr, 
                                       offset, &dataPtr, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dataPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxDiagRegsNumGet
*
* DESCRIPTION:
*       Gets the number of registers for the PP.
*       Used to allocate memory for cpssExMxDiagRegsDump.
*
* APPLICABLE DEVICES:  All EX/MX devices
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
static CMD_STATUS wrCpssExMxDiagRegsNumGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                   result;

    GT_U8                       devNum;
	GT_U32                      regsNumPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxDiagRegsNumGet(devNum, &regsNumPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", regsNumPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxDiagRegsDump
*
* DESCRIPTION:
*       Dumps the device register addresses and values.
*
* APPLICABLE DEVICES:  All EX/MX devices
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
*       The function may be called after Phase 1 initialization.
*       To dump all the registers the user may call cpssExMxDiagRegsNumGet in
*       order to get the number of registers of the PP.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxDiagRegsDump

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                   result;

    GT_U8                       devNum;
	GT_U32                      regsNumPtr;
	GT_U32                      offset;
	GT_U32                      regAddrPtr;
	GT_U32                      regDataPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    regsNumPtr = (GT_U32)inArgs[1];
    offset = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxDiagRegsDump(devNum, &regsNumPtr, offset, 
                                     &regAddrPtr, &regDataPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", regsNumPtr, regAddrPtr, regDataPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxDiagRegTest
*
* DESCRIPTION:
*       Tests the device read/write ability of a specific register.
*
* APPLICABLE DEVICES:  All EX/MX devices
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
static CMD_STATUS wrCpssExMxDiagRegTest

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                   result;

    GT_U8                       devNum;
	GT_U32                      regAddr;
	GT_U32                      regMask;
	CPSS_DIAG_TEST_PROFILE_ENT  profile;
	GT_BOOL                     testStatusPtr;
	GT_U32                      readValPtr;
	GT_U32                      writeValPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    regAddr = (GT_U32)inArgs[1];
    regMask = (GT_U32)inArgs[2];
    profile = (CPSS_DIAG_TEST_PROFILE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssExMxDiagRegTest(devNum, regAddr, regMask, profile, 
                         &testStatusPtr, &readValPtr, &writeValPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", testStatusPtr, readValPtr, 
                                                          writeValPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxDiagAllRegTest
*
* DESCRIPTION:
*       Tests the device read/write ability of all the registers.
*
* APPLICABLE DEVICES:  All EX/MX devices
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
*       The test is done by invoking cpssExMxDiagRegTest in loop for all the
*       diagnostics registers and for all the patterns.
*       After each register is tested, the original value prior to the test is
*       restored.
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxDiagAllRegTest

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                   result;

    GT_U8                       devNum;
	GT_BOOL                     testStatusPtr;
	GT_U32                      badRegPtr;
	GT_U32                      readValPtr;
	GT_U32                      writeValPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxDiagAllRegTest(devNum, &testStatusPtr, &badRegPtr, 
                                             &readValPtr, &writeValPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", testStatusPtr, badRegPtr, 
                                               readValPtr, writeValPtr);
    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxDiagMemTest",
        &wrCpssExMxDiagMemTest,
        5, 0},

    {"cpssExMxDiagAllMemTest",
        &wrCpssExMxDiagAllMemTest,
        1, 0},

    {"cpssExMxDiagMemWrite",
        &wrCpssExMxDiagMemWrite,
        4, 0},

    {"cpssExMxDiagMemRead",
        &wrCpssExMxDiagMemRead,
        3, 0},

    {"cpssExMxDiagRegWrite",
        &wrCpssExMxDiagRegWrite,
        5, 0},

    {"cpssExMxDiagRegRead",
        &wrCpssExMxDiagRegRead,
        4, 0},

    {"cpssExMxDiagPhyRegWrite",
        &wrCpssExMxDiagPhyRegWrite,
        6, 0},

    {"cpssExMxDiagPhyRegRead",
        &wrCpssExMxDiagPhyRegRead,
        5, 0},

    {"cpssExMxDiagRegsNumGet",
        &wrCpssExMxDiagRegsNumGet,
        1, 0},

    {"cpssExMxDiagRegsDump",
        &wrCpssExMxDiagRegsDump,
        3, 0},

    {"cpssExMxDiagRegTest",
        &wrCpssExMxDiagRegTest,
        4, 0},

    {"cpssExMxDiagAllRegTest",
        &wrCpssExMxDiagAllRegTest,
        1, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxDiag
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
GT_STATUS cmdLibInitCpssExMxDiag
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}




