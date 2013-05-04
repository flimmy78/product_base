/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* wrapDiagCpssDxCh.c
*
* DESCRIPTION:
*       Wrapper functions for Diag cpss.dxCh functions
*
* DEPENDENCIES:
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


/* Feature specific includes */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiag.h>



/* support for multi port groups */

/*******************************************************************************
* diagMultiPortGroupsBmpGet
*
* DESCRIPTION:
*       Get the portGroupsBmp for multi port groups device.
*       when 'enabled' --> wrappers will use the APIs
*       with portGroupsBmp parameter
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - (pointer to)enable / disable the use of APIs with portGroupsBmp parameter.
*       portGroupsBmpPtr - (pointer to)port groups bmp , relevant only when enable = GT_TRUE
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*******************************************************************************/
static void diagMultiPortGroupsBmpGet
(
    IN   GT_U8               devNum,
    OUT  GT_BOOL             *enablePtr,
    OUT  GT_PORT_GROUPS_BMP  *portGroupsBmpPtr
)
{
    /* default */
    *enablePtr  = GT_FALSE;
    *portGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if (0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return;
    }

    utilMultiPortGroupsBmpGet(devNum, enablePtr, portGroupsBmpPtr);
}

/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChDiagMemWrite
(
    IN GT_U8                          devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         offset,
    IN GT_U32                         data
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    diagMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChDiagMemWrite(devNum, memType, offset, data);
    }
    else
    {
        return cpssDxChDiagPortGroupMemWrite(devNum, pgBmp, memType, offset,
                                             data);
    }
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChDiagMemRead
(
    IN  GT_U8                          devNum,
    IN  CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN  GT_U32                         offset,
    OUT GT_U32                         *dataPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    diagMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChDiagMemRead(devNum, memType, offset, dataPtr);
    }
    else
    {
        return cpssDxChDiagPortGroupMemRead(devNum, pgBmp, memType, offset,
                                            dataPtr);
    }
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChDiagRegsNumGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    *regsNumPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    diagMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChDiagRegsNumGet(devNum, regsNumPtr);
    }
    else
    {
        return cpssDxChDiagPortGroupRegsNumGet(devNum, pgBmp, regsNumPtr);
    }
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChDiagRegsDump
(
    IN    GT_U8     devNum,
    INOUT GT_U32    *regsNumPtr,
    IN    GT_U32    offset,
    OUT   GT_U32    *regAddrPtr,
    OUT   GT_U32    *regDataPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    diagMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChDiagRegsDump(devNum, regsNumPtr, offset, regAddrPtr,
                                    regDataPtr);
    }
    else
    {
        return cpssDxChDiagPortGroupRegsDump(devNum, pgBmp, regsNumPtr,
                                             offset, regAddrPtr, regDataPtr);
    }
}

/*******************************************************************************
* cpssDxChDiagMemTest
*
* DESCRIPTION:
*       Performs memory test on a specified memory location and size for a
*       specified memory type.
*
* APPLICABLE DEVICES:  All DX CH devices
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
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong memory type
*       GT_FAIL      - on error
*
* COMMENTS:
*       The test is done by writing and reading a test pattern.
*       The function may be called after Phase 2 initialization.
*       The test is destructive and leaves the memory corrupted.
*       Supported memories:
*       - Buffer DRAM
*       - MAC table memory
*       - VLAN table memory
*       For buffer DRAM:
*          startOffset must be aligned to 64 Bytes and size must be in 64 bytes
*          resolution.
*       For MAC table:
*          startOffset must be aligned to 16 Bytes and size must be in 16 bytes
*          resolution.
*       For VLAN table:
*          DX CH devices: startOffset must be aligned to 12 Bytes and size must
*                         be in 12 bytes resolution.
*          DX CH2 devices: startOffset must be aligned to 16 Bytes and size must
*                          be in 16 bytes resolution.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagMemTest

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    CPSS_DIAG_PP_MEM_TYPE_ENT      memType;
    GT_U32                         startOffset;
    GT_U32                         size;
    CPSS_DIAG_TEST_PROFILE_ENT     profile;
    GT_BOOL                        testStatusPtr;
    GT_U32                         addrPtr;
    GT_U32                         readValPtr;
    GT_U32                         writeValPtr;

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
    result = cpssDxChDiagMemTest(devNum, memType, startOffset, size, profile,
                         &testStatusPtr, &addrPtr, &readValPtr, &writeValPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", testStatusPtr, addrPtr,
                                             readValPtr, writeValPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagAllMemTest
*
* DESCRIPTION:
*       Performs memory test for all the internal and external memories.
*       Tested memories:
*       - Buffer DRAM
*       - MAC table memory
*       - VLAN table memory
*
* APPLICABLE DEVICES:  All DX CH devices
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
*       The test is done by invoking cpssDxChDiagMemTest in loop for all the
*       memory types and for AA-55, random and incremental patterns.
*       The function may be called after Phase 2 initialization.
*       The test is destructive and leaves the memory corrupted.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagAllMemTest

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    GT_BOOL                        testStatusPtr;
    GT_U32                         addrPtr;
    GT_U32                         readValPtr;
    GT_U32                         writeValPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* to avoid warning */
    testStatusPtr = GT_FALSE;

    /* call cpss api function */
    result = cpssDxChDiagAllMemTest(devNum, &testStatusPtr, &addrPtr,
                                           &readValPtr, &writeValPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", testStatusPtr, addrPtr,
                                             readValPtr, writeValPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagMemWrite
*
* DESCRIPTION:
*       performs a single 32 bit data write to one of the PP memory spaces.
*
* APPLICABLE DEVICES:  All DX CH devices
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
static CMD_STATUS wrCpssDxChDiagMemWrite

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    CPSS_DIAG_PP_MEM_TYPE_ENT      memType;
    GT_U32                         offset;
    GT_U32                         data;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DIAG_PP_MEM_TYPE_ENT)inArgs[1];
    offset = (GT_U32)inArgs[2];
    data = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = pg_wrap_cpssDxChDiagMemWrite(devNum, memType, offset, data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagMemRead
*
* DESCRIPTION:
*       performs a single 32 bit data read from one of the PP memory spaces.
*
* APPLICABLE DEVICES:  All DX CH devices
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
static CMD_STATUS wrCpssDxChDiagMemRead

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    CPSS_DIAG_PP_MEM_TYPE_ENT      memType;
    GT_U32                         offset;
    GT_U32                         dataPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DIAG_PP_MEM_TYPE_ENT)inArgs[1];
    offset = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChDiagMemRead(devNum, memType, offset, &dataPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dataPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagRegWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write to one of the PP PCI configuration or
*       internal registers.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       IfChannel  - interface channel (PCI/SMI/TWSI)
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
static CMD_STATUS wrCpssDxChDiagRegWrite

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U32                             baseAddr;
    CPSS_PP_INTERFACE_CHANNEL_ENT      ifChannel;
    CPSS_DIAG_PP_REG_TYPE_ENT          regType;
    GT_U32                             offset;
    GT_U32                             data;
    GT_BOOL                            doByteSwap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    baseAddr = (GT_U32)inArgs[0];
    ifChannel = (CPSS_PP_INTERFACE_CHANNEL_ENT)inArgs[1];
    regType = (CPSS_DIAG_PP_REG_TYPE_ENT)inArgs[2];
    offset = (GT_U32)inArgs[3];
    data = (GT_U32)inArgs[4];
    doByteSwap = (GT_BOOL)inArgs[5];

    /* call cpss api function */
    result = cpssDxChDiagRegWrite(baseAddr, ifChannel, regType,
                                      offset, data, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagRegRead
*
* DESCRIPTION:
*       Performs single 32 bit data read from one of the PP PCI configuration or
*       internal registers.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       IfChannel  - interface channel (PCI/SMI/TWSI)
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
static CMD_STATUS wrCpssDxChDiagRegRead

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U32                             baseAddr;
    CPSS_PP_INTERFACE_CHANNEL_ENT      ifChannel;
    CPSS_DIAG_PP_REG_TYPE_ENT          regType;
    GT_U32                             offset;
    GT_U32                             dataPtr;
    GT_BOOL                            doByteSwap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    baseAddr = (GT_U32)inArgs[0];
    ifChannel = (CPSS_PP_INTERFACE_CHANNEL_ENT)inArgs[1];
    regType = (CPSS_DIAG_PP_REG_TYPE_ENT)inArgs[2];
    offset = (GT_U32)inArgs[3];
    doByteSwap = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssDxChDiagRegRead(baseAddr, ifChannel, regType,
                                      offset, &dataPtr, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dataPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagPhyRegWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write to one of the PHY registers.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       baseAddr     - The base address to access the device
*       IfChannel    - interface channel (PCI/SMI/TWSI)
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
static CMD_STATUS wrCpssDxChDiagPhyRegWrite

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U32                             baseAddr;
    CPSS_PP_INTERFACE_CHANNEL_ENT      ifChannel;
    GT_U32                             smiRegOffset;
    GT_U32                             phyAddr;
    GT_U32                             offset;
    GT_U32                             data;
    GT_BOOL                            doByteSwap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    baseAddr = (GT_U32)inArgs[0];
    ifChannel = (CPSS_PP_INTERFACE_CHANNEL_ENT)inArgs[1];
    smiRegOffset = (GT_U32)inArgs[2];
    phyAddr = (GT_U32)inArgs[3];
    offset = (GT_U32)inArgs[4];
    data = (GT_U32)inArgs[5];
    doByteSwap = (GT_BOOL)inArgs[6];

    /* call cpss api function */
    result = cpssDxChDiagPhyRegWrite(baseAddr, ifChannel, smiRegOffset,
                                     phyAddr, offset, data, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagPhyRegRead
*
* DESCRIPTION:
*       Performs single 32 bit data read from one of the PHY registers.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       baseAddr     - The base address to access the device
*       IfChannel    - interface channel (PCI/SMI/TWSI)
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
static CMD_STATUS wrCpssDxChDiagPhyRegRead

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U32                             baseAddr;
    CPSS_PP_INTERFACE_CHANNEL_ENT      ifChannel;
    GT_U32                             smiRegOffset;
    GT_U32                             phyAddr;
    GT_U32                             offset;
    GT_U32                             dataPtr;
    GT_BOOL                            doByteSwap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    baseAddr = (GT_U32)inArgs[0];
    ifChannel = (CPSS_PP_INTERFACE_CHANNEL_ENT)inArgs[1];
    smiRegOffset = (GT_U32)inArgs[2];
    phyAddr = (GT_U32)inArgs[3];
    offset = (GT_U32)inArgs[4];
    doByteSwap = (GT_BOOL)inArgs[5];

    /* call cpss api function */
    result = cpssDxChDiagPhyRegRead(baseAddr, ifChannel, smiRegOffset,
                                phyAddr, offset, &dataPtr, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dataPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagRegsNumGet
*
* DESCRIPTION:
*       Gets the number of registers for the PP.
*       Used to allocate memory for cpssDxChDiagRegsDump.
*
* APPLICABLE DEVICES:  All DX CH devices
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
static CMD_STATUS wrCpssDxChDiagRegsNumGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U32                             regsNumPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChDiagRegsNumGet(devNum, &regsNumPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", regsNumPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagRegsDump
*
* DESCRIPTION:
*       Dumps the device register addresses and values.
*
* APPLICABLE DEVICES:  All DX CH devices
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
*       To dump all the registers the user may call cpssDxChDiagRegsNumGet in
*       order to get the number of registers of the PP.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagRegsDump

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      offset;
    GT_U32      regsNum;
    GT_U32      *regAddrPtr;
    GT_U32      *regDataPtr;
    GT_U32      tmpRegsNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    regsNum = tmpRegsNum= (GT_U32)inArgs[1];
    offset = (GT_U32)inArgs[2];

    regAddrPtr = cmdOsMalloc(regsNum * sizeof(GT_U32));
    if (regAddrPtr == NULL)
    {
        return CMD_AGENT_ERROR;
    }

    regDataPtr = cmdOsMalloc(regsNum * sizeof(GT_U32));
    if (regDataPtr == NULL)
    {
        cmdOsFree(regAddrPtr);
        return CMD_AGENT_ERROR;
    }

    /* call cpss api function */
    result = pg_wrap_cpssDxChDiagRegsDump(devNum, &tmpRegsNum, offset,
                                          regAddrPtr, regDataPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", regsNum, regAddrPtr[(regsNum - 1)],
                 regDataPtr[(regsNum - 1)]);

    cmdOsFree(regAddrPtr);
    cmdOsFree(regDataPtr);
    return CMD_OK;
}


GT_STATUS wrCpssDxChDiagRegsPrint(int devNum)
{
    GT_U32 *regAddrPtr, *regDataPtr; /* pointers to arrays of reg addresses and data*/
    GT_U32 regNum, i; /* number of regs */
    GT_STATUS       result;

    /* call cpss api function */
    result = cpssDxChDiagRegsNumGet((GT_U8)devNum, &regNum);
    if(result != GT_OK)
    {
        return (int)result;
    }
    cmdOsPrintf("registers number %d\n\n", regNum);
    regAddrPtr = cmdOsMalloc(regNum * sizeof(GT_U32));
    regDataPtr = cmdOsMalloc(regNum * sizeof(GT_U32));


    result = cpssDxChDiagRegsDump((GT_U8)devNum, &regNum, 0, regAddrPtr, regDataPtr);


    if(result != GT_OK)
    {
        return result;
    }

    cmdOsPrintf("registers number %d\n\n", regNum);
    for(i = 0; i < regNum; i++)
    {
        cmdOsPrintf("register addr 0x%08X value 0x%08X\n", regAddrPtr[i], regDataPtr[i]);
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChDiagRegTest
*
* DESCRIPTION:
*       Tests the device read/write ability of a specific register.
*
* APPLICABLE DEVICES:  All DX CH devices
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
static CMD_STATUS wrCpssDxChDiagRegTest

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U32                             regAddr;
    GT_U32                             regMask;
    CPSS_DIAG_TEST_PROFILE_ENT         profile;
    GT_BOOL                            testStatusPtr;
    GT_U32                             readValPtr;
    GT_U32                             writeValPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    regAddr = (GT_U32)inArgs[1];
    regMask = (GT_U32)inArgs[2];
    profile = (CPSS_DIAG_TEST_PROFILE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagRegTest(devNum, regAddr, regMask, profile,
                         &testStatusPtr, &readValPtr, &writeValPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", testStatusPtr, readValPtr,
                                                          writeValPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagAllRegTest
*
* DESCRIPTION:
*       Tests the device read/write ability of all the registers.
*
* APPLICABLE DEVICES:  All DX CH devices
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
*       The test is done by invoking cpssDxChDiagRegTest in loop for all the
*       diagnostics registers and for all the patterns.
*       After each register is tested, the original value prior to the test is
*       restored.
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagAllRegTest

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_BOOL                            testStatusPtr;
    GT_U32                             badRegPtr;
    GT_U32                             readValPtr;
    GT_U32                             writeValPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChDiagAllRegTest(devNum, &testStatusPtr, &badRegPtr,
                                             &readValPtr, &writeValPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", testStatusPtr, badRegPtr,
                                               readValPtr, writeValPtr);
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChDiagPrbsPortTransmitModeSet
*
* DESCRIPTION:
*       Set transmit mode for specified port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*       mode     - transmit mode
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       CPU port doesn't support the transmit mode.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortTransmitModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 laneNum;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    laneNum = (GT_U32)inArgs[2];
    mode = (CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagPrbsPortTransmitModeSet(devNum, portNum, laneNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortTransmitModeGet
*
* DESCRIPTION:
*       Get transmit mode for specified port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       modePtr  - transmit mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       CPU port doesn't support the transmit mode.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortTransmitModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 laneNum;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    laneNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagPrbsPortTransmitModeGet(devNum, portNum, laneNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortGenerateEnableSet
*
* DESCRIPTION:
*       Enable/Disable PRBS (Pseudo Random Bit Generator) pattern generation
*       per Port and per lane.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*       enable   - GT_TRUE - PRBS pattern generation is enabled
*                  GT_FALSE - PRBS pattern generation is disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       When operating, enable the PRBS checker before the generator.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortGenerateEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 laneNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    laneNum = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagPrbsPortGenerateEnableSet(devNum, portNum, laneNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortGenerateEnableGet
*
* DESCRIPTION:
*       Get the status of PRBS (Pseudo Random Bit Generator) pattern generation
*       per port and per lane.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - PRBS pattern generation is enabled
*                     GT_FALSE - PRBS pattern generation is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortGenerateEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 laneNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    laneNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagPrbsPortGenerateEnableGet(devNum, portNum, laneNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable PRBS (Pseudo Random Bit Generator) checker per port and
*       per lane.
*       When the checker is enabled, it seeks to lock onto the incoming bit
*       stream, and once this is achieved the PRBS checker starts counting the
*       number of bit errors. Tne number of errors can be retrieved by
*       cpssDxChDiagPrbsGigPortStatusGet API.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*       enable   - GT_TRUE - PRBS checker is enabled
*                  GT_FALSE - PRBS checker is disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       When operating, enable the PRBS checker before the generator.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortCheckEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 laneNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    laneNum = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagPrbsPortCheckEnableSet(devNum, portNum, laneNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortCheckEnableGet
*
* DESCRIPTION:
*       Get the status (enabled or disabled) of (Pseudo Random Bit Generator)
*       checker per port and per lane.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - PRBS checker is enabled
*                     GT_FALSE - PRBS checker is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortCheckEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 laneNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    laneNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagPrbsPortCheckEnableGet(devNum, portNum, laneNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortCheckReadyGet
*
* DESCRIPTION:
*       Get the PRBS checker ready status.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       isReadyPtr  - GT_TRUE - PRBS checker is ready.
*                     PRBS checker has completed the initialization phase.
*                     GT_FALSE - PRBS checker is not ready.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the Tri-speed ports.
*       The Check ready status indicates that the PRBS checker has completed
*       the initialization phase. The PRBS generator at the transmit side may
*       be enabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortCheckReadyGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL isReady;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);


    /* call cpss api function */
    result = cpssDxChDiagPrbsPortCheckReadyGet(devNum, portNum, &isReady);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isReady);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortStatusGet
*
* DESCRIPTION:
*       Get PRBS (Pseudo Random Bit Generator) Error Counter and Checker Locked
*       status per port and per lane.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       checkerLockedPtr        - GT_TRUE - checker is locked on the sequence
*                                           stream.
*                                 GT_FALSE - checker isn't locked on the sequence
*                                           stream.
*       errorCntrPtr            - PRBS Error counter, this counter represents
*                                 the number of bit mismatches detected since
*                                 the PRBS checker of the port has locked.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PRBS Error counter is cleared on read.
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 laneNum;
    GT_BOOL checkerLocked;
    GT_U32 errorCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    laneNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagPrbsPortStatusGet(devNum, portNum, laneNum, &checkerLocked, &errorCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", checkerLocked, errorCntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsCyclicDataSet
*
* DESCRIPTION:
*       Set cylic data for transmition. See cpssDxChDiagPrbsPortTransmitModeSet.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number
*       laneNum         - lane number
*       cyclicDataArr   - cyclic data array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the XAUI/HGS ports.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsCyclicDataSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 laneNumber;
    GT_U32 cyclicDataArr[4];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    laneNumber = (GT_U32)inArgs[2];
    cyclicDataArr[0]=(GT_U32)inArgs[3];
    cyclicDataArr[1]=(GT_U32)inArgs[4];
    cyclicDataArr[2]=(GT_U32)inArgs[5];
    cyclicDataArr[3]=(GT_U32)inArgs[6];

    /* call cpss api function */
    result = cpssDxChDiagPrbsCyclicDataSet(devNum, portNum, laneNumber, cyclicDataArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsCyclicDataGet
*
* DESCRIPTION:
*       Get cylic data for transmition. See cpssDxChDiagPrbsPortTransmitModeSet.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number
*       laneNum         - lane number
*
* OUTPUTS:
*       cyclicDataArr   - cyclic data array
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the XAUI/HGS ports.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsCyclicDataGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 laneNumber;
    GT_U32 cyclicDataArr[4];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    laneNumber = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagPrbsCyclicDataGet(devNum, portNum, laneNumber, cyclicDataArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d",cyclicDataArr[0],cyclicDataArr[1],cyclicDataArr[2],
                                             cyclicDataArr[3]);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChDiagPrbsSerdesTestEnableSet
*
* DESCRIPTION:
*       Enable/Disable SERDES PRBS (Pseudo Random Bit Generator) test mode.
*
* APPLICABLE DEVICES: DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*       enable    - GT_TRUE - test enabled
*                   GT_FALSE - test disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Transmit mode should be set before enabling test mode.
*       See test cpssDxChDiagPrbsSerdesTransmitModeSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsSerdesTestEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 laneNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    laneNum = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagPrbsSerdesTestEnableSet(devNum, portNum, laneNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsSerdesTestEnableGet
*
* DESCRIPTION:
*       Get the status of PRBS (Pseudo Random Bit Generator) test mode.
*
* APPLICABLE DEVICES: DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*
* OUTPUTS:
*       serdesPtr   - GT_TRUE - test enabled
*                     GT_FALSE - test disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsSerdesTestEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 laneNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    laneNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagPrbsSerdesTestEnableGet(devNum, portNum, laneNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsSerdesTransmitModeSet
*
* DESCRIPTION:
*       Set transmit mode for SERDES PRBS on specified port/lane.
*
* APPLICABLE DEVICES: DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*       mode      - transmit mode
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*                                  or unsupported transmit mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Only CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E supported.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsSerdesTransmitModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 laneNum;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    laneNum = (GT_U32)inArgs[2];
    mode = (CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagPrbsSerdesTransmitModeSet(devNum, portNum, laneNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsSerdesTransmitModeGet
*
* DESCRIPTION:
*       Get transmit mode for SERDES PRBS on specified port/lane.
*
* APPLICABLE DEVICES: DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
* OUTPUTS:
*       modePtr   - transmit mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on unkonown transmit mode
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsSerdesTransmitModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 laneNum;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    laneNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagPrbsSerdesTransmitModeGet(devNum, portNum, laneNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsSerdesStatusGet
*
* DESCRIPTION:
*       Get SERDES PRBS (Pseudo Random Bit Generator) pattern detector state,
*       error counter and pattern counter.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*       serdesNum - SERDES number
*
* OUTPUTS:
*       lockedPtr      - (pointer to) Pattern detector state.
*                         GT_TRUE - Pattern detector had locked onto the pattern.
*                         GT_FALSE - Pattern detector is not locked onto
*                                   the pattern.

*       errorCntrPtr   - (pointer to) PRBS Error counter. This counter represents
*                                 the number of bit mismatches detected since
*                                 the PRBS checker of the port has locked.
*       patternCntrPtr - (pointer to) Pattern counter. Number of 40-bit patterns
*                                 received since acquiring pattern lock.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsSerdesStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 laneNum;
    GT_BOOL locked;
    GT_U32 errorCntr;
    GT_U64 patternCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    laneNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagPrbsSerdesStatusGet(devNum, portNum, laneNum, &locked, &errorCntr, &patternCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    galtisOutput(outArgs, result, "%d%d%d%d", locked, errorCntr,patternCntr.l[0],patternCntr.l[1]);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagMemoryBistBlockStatusGet
*
* DESCRIPTION:
*       Gets redundancy block status.
*
* APPLICABLE DEVICES: DxCh3 only
*
* INPUTS:
*       devNum        - device number
*       memBistType   - type of memory
*       blockIndex    - block index
*                       CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E:
*                         valid blockIndex: 0 to 13.
*                       CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E:
*                         valid blockIndex: 0 to 19.
*
* OUTPUTS:
*       blockFixedPtr   - (pointer to) block status
*                                 GT_TRUE - row in the block was fixed
*                                 GT_FALSE - row in the block wasn't fixed
*       replacedIndexPtr - (pointer to) replaced row index.
*                                Only valid if  *blockFixedPtr is GT_TRUE.
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*          Each redundancy block contains 256 rows. Block 0: rows 0 to 255,
*          block 1: raws 256 to 511 ...
*          One raw replacement is possible though the full redundancy block
*          of 256 rows.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagMemoryBistBlockStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DIAG_PP_MEM_BIST_TYPE_ENT memBistType;
    GT_U32 blockIndex;
    GT_BOOL blockFixed;
    GT_U32 replacedIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memBistType = (CPSS_DIAG_PP_MEM_BIST_TYPE_ENT)inArgs[1];
    blockIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagMemoryBistBlockStatusGet(devNum, memBistType,
                                blockIndex, &blockFixed, &replacedIndex);

    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", blockFixed, replacedIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagMemoryBistsRun
*
* DESCRIPTION:
*       Runs BIST (Built-in self-test) on specified memory.
*
* APPLICABLE DEVICES: DxCh3 only
*
* INPUTS:
*       devNum        - device number
*       memBistType   - type of memory
*       timeOut       - maximal time in milisecond to wait for BIST finish.
*       clearMemoryAfterTest - GT_TRUE  - Clear memory after test.
*                              GT_FALSE - Don't clear memory after test.
* OUTPUTS:
*      None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT    - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_UNFIXABLE_BIST_ERROR  - on unfixable problem in the TCAM
*
* COMMENTS:
*       1. This function should not be called under traffic.
*       2. Bist destroys the content of the memory. If clearMemoryAfterTest set
*          function will clear the content of the memory after the test.
*          Application responsible to restore the content after the function
*          completion.
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagMemoryBistsRun
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DIAG_PP_MEM_BIST_TYPE_ENT memBistType;
    GT_U32 timeOut;
    GT_BOOL clearMemoryAfterTest;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memBistType = (CPSS_DIAG_PP_MEM_BIST_TYPE_ENT)inArgs[1];
    timeOut = (GT_U32)inArgs[2];
    clearMemoryAfterTest = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagMemoryBistsRun(devNum, memBistType,
                                        timeOut, clearMemoryAfterTest);

    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChDiagDeviceTemperatureGet
*
* DESCRIPTION:
*       Gets the PP temperature.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum          - device number
* OUTPUTS:
*       temperaturePtr  - (pointer to) temperature in Celsius degrees
*                         (can be negative)
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDeviceTemperatureGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_32     temperature;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    temperature = 0;

    /* call cpss api function */
    result = cpssDxChDiagDeviceTemperatureGet(
        devNum, &temperature);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", temperature);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChDiagMemTest",
        &wrCpssDxChDiagMemTest,
        5, 0},

    {"cpssDxChDiagAllMemTest",
        &wrCpssDxChDiagAllMemTest,
        1, 0},

    {"cpssDxChDiagMemWrite",
        &wrCpssDxChDiagMemWrite,
        4, 0},

    {"cpssDxChDiagMemRead",
        &wrCpssDxChDiagMemRead,
        3, 0},

    {"cpssDxChDiagRegWrite",
        &wrCpssDxChDiagRegWrite,
        6, 0},

    {"cpssDxChDiagRegRead",
        &wrCpssDxChDiagRegRead,
        5, 0},

    {"cpssDxChDiagPhyRegWrite",
        &wrCpssDxChDiagPhyRegWrite,
        7, 0},

    {"cpssDxChDiagPhyRegRead",
        &wrCpssDxChDiagPhyRegRead,
        6, 0},

    {"cpssDxChDiagRegsNumGet",
        &wrCpssDxChDiagRegsNumGet,
        1, 0},

    {"cpssDxChDiagRegsDump",
        &wrCpssDxChDiagRegsDump,
        3, 0},

    {"cpssDxChDiagRegTest",
        &wrCpssDxChDiagRegTest,
        4, 0},

    {"cpssDxChDiagAllRegTest",
        &wrCpssDxChDiagAllRegTest,
        1, 0},
    {"cpssDxChDiagPrbsPortTransmitModeSet",
        &wrCpssDxChDiagPrbsPortTransmitModeSet,
        4, 0},
    {"cpssDxChDiagPrbsPortTransmitModeGet",
        &wrCpssDxChDiagPrbsPortTransmitModeGet,
        3, 0},
    {"cpssDxChDiagPrbsPortGenerateEnableSet",
        &wrCpssDxChDiagPrbsPortGenerateEnableSet,
        4, 0},
    {"cpssDxChDiagPrbsPortGenerateEnableGet",
        &wrCpssDxChDiagPrbsPortGenerateEnableGet,
        3, 0},
    {"cpssDxChDiagPrbsPortCheckEnableSet",
        &wrCpssDxChDiagPrbsPortCheckEnableSet,
        4, 0},
    {"cpssDxChDiagPrbsPortCheckEnableGet",
        &wrCpssDxChDiagPrbsPortCheckEnableGet,
        3, 0},
    {"cpssDxChDiagPrbsPortCheckReadyGet",
        &wrCpssDxChDiagPrbsPortCheckReadyGet,
        2, 0},
    {"cpssDxChDiagPrbsPortStatusGet",
        &wrCpssDxChDiagPrbsPortStatusGet,
        3, 0},
    {"cpssDxChDiagPrbsCyclicDataSet",
        &wrCpssDxChDiagPrbsCyclicDataSet,
        7, 0},
    {"cpssDxChDiagPrbsCyclicDataGet",
        &wrCpssDxChDiagPrbsCyclicDataGet,
        3, 0},
    {"cpssDxChDiagPrbsSerdesTestEnableSet",
        &wrCpssDxChDiagPrbsSerdesTestEnableSet,
        4, 0},
    {"cpssDxChDiagPrbsSerdesTestEnableGet",
        &wrCpssDxChDiagPrbsSerdesTestEnableGet,
        3, 0},
    {"cpssDxChDiagPrbsSerdesTransmitModeSet",
        &wrCpssDxChDiagPrbsSerdesTransmitModeSet,
        4, 0},
    {"cpssDxChDiagPrbsSerdesTransmitModeGet",
        &wrCpssDxChDiagPrbsSerdesTransmitModeGet,
        3, 0},
    {"cpssDxChDiagPrbsSerdesStatusGet",
        &wrCpssDxChDiagPrbsSerdesStatusGet,
        3, 0},
    {"cpssDxChDiagMemoryBistBlockStatusGet",
        &wrCpssDxChDiagMemoryBistBlockStatusGet,
        3, 0},
    {"cpssDxChDiagMemoryBistsRun",
        &wrCpssDxChDiagMemoryBistsRun,
        4, 0},
    {"cpssDxChDiagDeviceTemperatureGet",
        &wrCpssDxChDiagDeviceTemperatureGet,
        1, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChDiag
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
GT_STATUS cmdLibInitCpssDxChDiag
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

