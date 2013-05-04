/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapI2cCpss.c
*
* DESCRIPTION:
*       Wrapper functions for I2c cpss functions.
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
#include <cpss/exMx/exMxGen/i2c/cpssExMxI2c.h>
/*#include <galtisAgent/wrapCpss/exMx/wrapI2cCpss.h> */

/*******************************************************************************
* cpssExMxI2cInit
*
* DESCRIPTION:
*               The function  resets the I2C machine , sets I2C's freuency
*               registers, and enables it, and set i2c slave register.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum      - device number to init i2c.
*
* OUTPUTS:
*           none.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*       GT_BAD_PARAM     - on wrong device number.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxI2cInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxI2cInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxI2cEnable
*
* DESCRIPTION:
*               The function enables/disables the I2c machine.
*
* INPUTS:
*       devNum      - device number to set the i2c machine.
*       en          - enable/disable parameter.
* OUTPUTS:
*           none.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*       GT_BAD_PARAM     - on wrong device number.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxI2cEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS result;

    GT_U8   devNum;
	GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
	enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxI2cEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*****************************************************************************
* cpssExMxI2cMasterRead
*
* DESCRIPTION:
*           Read bytes from an I2C device.
* INPUTS:
*       devNum      - device number for i2c master.
*       i2cAddress  - i2c 7-bit or 10-bit slave device address to access.
*                     if bit31 is set to 1 , the i2c address is 10 bit format
*       dataLen     - Number of bytes to read.
*
* OUTPUTS:
*       dataPtr     -   Data read .
*
* RETURNS:
*       GT_OK           - on success]
*       GT_TIMEOUT      - when I2c bus time out has expired
*       GT_NO_SUCH      - No i2c device with such address
*       GT_FAIL         - on other error.
*       GT_BAD_PARAM    - on wrong device number.
*
* COMMENTS:
*
******************************************************************************/
static CMD_STATUS wrcpssExMxI2cMasterRead
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS result;

    GT_U8  devNum;
	GT_U16 i2cAddress;
	GT_U8  dataLen;
	GT_U8  dataPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
	i2cAddress = (GT_U16)inArgs[1];
	dataLen = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxI2cMasterRead(devNum, i2cAddress, dataLen, &dataPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dataPtr);
    return CMD_OK;
}



/*****************************************************************************
* cpssExMxI2cMasterWrite
*
* DESCRIPTION:
*           Writes bytes to an I2C device
* INPUTS:
*       devNum          - device number for i2c read
*       i2cAddress      - i2c 7-bit or 10-bit slave device address to access.
*                         if bit31 is set to 1 , the i2c address is 10 bit format
*       length          - Number of bytes to read.
*       dataPtr         - pointer to data.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success]
*       GT_TIMEOUT      - when I2c bus time out has expired
*       GT_NO_SUCH      - No i2c device with such address
*       GT_FAIL         - on other error.
*       GT_BAD_PARAM    - on wrong device number.
*
* COMMENTS:
*
******************************************************************************/
static CMD_STATUS wrcpssExMxI2cMasterWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS result;

    GT_U8  devNum;
	GT_U16 i2cAddress;
	GT_U8  dataLen;
	GT_U8  dataPtr[4];
    GT_U32 dataWord;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
	i2cAddress = (GT_U16)inArgs[1];
	dataLen = (GT_U8)inArgs[2];
    dataWord = inArgs[3];

    dataPtr[0] = (GT_U8)(dataWord >> 24);
    dataPtr[1] = (GT_U8)(dataWord >> 16);
    dataPtr[2] = (GT_U8)(dataWord >> 8);
    dataPtr[3] = (GT_U8)(dataWord);

    /* call cpss api function */
    result = cpssExMxI2cMasterWrite(devNum, i2cAddress, dataLen, dataPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*****************************************************************************
* cpssExMxI2cEpromWordRead
*
* DESCRIPTION:
*           Read word from an EEPROM device.
* INPUTS:
*       devNum      - device number for master i2c read.
*       i2cAddress  - i2c 7-bit or 10-bit slave device address to access.
*                     if bit31 is set to 1 , the i2c address is 10 bit format
*       epromAddress - address in EEPROM memory.
*
*
* APPLICABLE DEVICES:  All ExMx devices
*
* OUTPUTS:
*       i2cData     -   Data (when  success)
*
* RETURNS:
*       GT_OK           - on success]
*       GT_TIMEOUT      - when I2c bus time out has expired
*       GT_NO_SUCH      - No i2c device with such address
*       GT_FAIL         - on other error.
*       GT_BAD_PARAM    - on wrong device number.
*
* COMMENTS:
*
******************************************************************************/
static CMD_STATUS wrcpssExMxI2cEpromWordRead
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS result;

    GT_U8  devNum;
	GT_U16 i2cAddress;
	GT_U16  epromAddress;
	GT_U32  i2cDataPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
	i2cAddress = (GT_U16)inArgs[1];
	epromAddress = (GT_U16)inArgs[2];

    /* call cpss api function */
    result = cpssExMxI2cEpromWordRead(devNum, i2cAddress, epromAddress, &i2cDataPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", i2cDataPtr);
    return CMD_OK;
}



/*****************************************************************************
* cpssExMxI2cEpromWordWrite
*
* DESCRIPTION:
*           Writes word to an EEPROM device
* INPUTS:
*       devNum          - device number for i2c read
*       i2cAddress      - i2c 7-bit or 10-bit slave device address to access.
*                         if bit31 is set to 1 , the i2c address is 10 bit format
*       epromAddress    - address in EEPROM memory.
*       i2cValue        - Data value.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success]
*       GT_TIMEOUT      - when I2c bus time out has expired
*       GT_NO_SUCH      - No i2c device with such address
*       GT_FAIL         - on other error.
*       GT_BAD_PARAM    - on wrong device number.
*
* COMMENTS:
*
******************************************************************************/
static CMD_STATUS wrcpssExMxI2cEpromWordWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS result;

    GT_U8  devNum;
	GT_U16 i2cAddress;
	GT_U16  epromAddress;
	GT_U32  i2cValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
	i2cAddress = (GT_U16)inArgs[1];
	epromAddress = (GT_U16)inArgs[2];
	i2cValue = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxI2cEpromWordWrite(devNum, i2cAddress, epromAddress, i2cValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxI2cInit",
        &wrcpssExMxI2cInit,
        1, 0},

    {"cpssExMxI2cEnable",
        &wrcpssExMxI2cEnable,
        2, 0},

     {"cpssExMxI2cMasterRead",
        &wrcpssExMxI2cMasterRead,
       3, 0},

    {"cpssExMxI2cMasterWrite",
        &wrcpssExMxI2cMasterWrite,
        4, 0},

    {"cpssExMxI2cEpromWordRead",
        &wrcpssExMxI2cEpromWordRead,
        3, 0},

    {"cpssExMxI2cEpromWordWrite",
        &wrcpssExMxI2cEpromWordWrite,
        4, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxI2c
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
GT_STATUS cmdLibInitCpssExMxI2c
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}




