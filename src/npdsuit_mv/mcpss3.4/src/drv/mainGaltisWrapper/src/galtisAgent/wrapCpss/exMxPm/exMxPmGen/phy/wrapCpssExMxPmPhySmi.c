/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmPhySmi.c
*
* DESCRIPTION:
*       TODO: Add proper description of this file here
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
#include <cpss/exMxPm/exMxPmGen/phy/cpssExMxPmPhySmi.h>

/*******************************************************************************
* cpssExMxPmPhyPortAddrSet
*
* DESCRIPTION:
*       Configure the port's default phy address, this function should be
*       used to change the default port's phy address.
*
* APPLICABLE DEVICES:  All ExMxPm Devices
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number.
*       phyAddr     - The new phy address, (value 0...31).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE  - phyAddr bigger then 31
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       This function should be called after PpHwPhase1Init(), and before
*       PortSmiInit()
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPhyPortAddrSet
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
    GT_U8 phyAddr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    phyAddr = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPhyPortAddrSet(devNum, portNum, phyAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPhyPortSmiInit
*
* DESCRIPTION:
*       Initialiaze the SMI control register port, Check all GE ports and
*       activate the errata initialization fix.
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong devNum.
*       GT_FAIL      - on error Errata fix.
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPhyPortSmiInit
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
    result = cpssExMxPmPhyPortSmiInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPhyPortSmiRegisterWrite
*
* DESCRIPTION:
*       Write value to specified SMI Register on a specified port on
*       specified device.
*
* APPLICABLE DEVICES:  All ExMxPm Devices
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number.
*       phyReg      - The new phy address, (value 0...31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong devNum or portNum.
*       GT_NOT_SUPPORTED - for XG ports
*       GT_OUT_OF_RANGE  - phyAddr bigger then 31
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPhyPortSmiRegisterWrite
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
    GT_U8 phyReg;
    GT_U16 data;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    phyReg = (GT_U8)inArgs[2];
    data = (GT_U16)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPhyPortSmiRegisterWrite(devNum, portNum, phyReg, data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPhyPortSmiRegisterRead
*
* DESCRIPTION:
*       Read specified SMI Register on a specified port on specified device.
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       phyReg    - SMI register (value 0..31)
*
* OUTPUTS:
*       dataPtr   - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE  - phyAddr bigger then 31
*       GT_NOT_SUPPORTED - for XG ports
*       GT_BAD_PTR       - for NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPhyPortSmiRegisterRead
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
    GT_U8 phyReg;
    GT_U16 data;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    phyReg = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPhyPortSmiRegisterRead(devNum, portNum, phyReg, &data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", data);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPhyPort10GSmiRegisterWrite
*
* DESCRIPTION:
*       Write value to a specified 10G SMI Register and PHY device of
*       specified port on specified device.
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum  - device number
*       portNum - Physical port number - relevant for internal PHY.
*       phyId   - Unique ID of the PHY device on the XSMI bus (value of 0..31)- relevant for external PHY.
*       useExternalPhy - boolean variable, defines if to use external/Internal 10G PHY
*       phyReg  - 10G SMI register, the register of PHY to read from
*       phyDev  - the PHY device to read from (value of 0..31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE  - phyDev & phyAddr bigger then 31
*       GT_NOT_SUPPORTED - for non XG ports
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       If useExternalPhy flag is GT_FALSE, the phyId is being ignored
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPhyPort10GSmiRegisterWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 phyId;
    GT_BOOL useExternalPhy;
    GT_U16 phyReg;
    GT_U8 phyDev;
    GT_U16 data;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    phyId = (GT_U8)inArgs[2];
    useExternalPhy = (GT_BOOL)inArgs[3];
    phyReg = (GT_U16)inArgs[4];
    phyDev = (GT_U8)inArgs[5];
    data = (GT_U16)inArgs[6];

    if (useExternalPhy == GT_FALSE)
    {
        result = GT_NOT_SUPPORTED;
    }
    else
    {
        /* call cpss api function */
        result = cpssExMxPmPhyPort10GSmiRegisterWrite(devNum, phyId, phyReg, phyDev, data);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPhyPort10GSmiRegisterRead
*
* DESCRIPTION:
*       Read specified 10G SMI Register and PHY device of specified port
*       on specified device.
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum  - device number
*       portNum - Physical port number - relevant for internal PHY.
*       phyId   - Unique ID of the PHY device on the XSMI bus (value of 0..31)- relevant for external PHY.
*       useExternalPhy - boolean variable, defines if to use external/Internal 10G PHY
*       phyReg  - 10G SMI register, the register of PHY to read from
*       phyDev  - the PHY device to read from (value of 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE  - phyDev & phyAddr bigger then 31
*       GT_NOT_SUPPORTED - for non XG ports
*       GT_BAD_PTR       - for NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       If useExternalPhy flag is GT_FALSE, the phyId ignored
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPhyPort10GSmiRegisterRead
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 phyId;
    GT_BOOL useExternalPhy;
    GT_U16 phyReg;
    GT_U8 phyDev;
    GT_U16 data = 0;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    phyId = (GT_U8)inArgs[2];
    useExternalPhy = (GT_BOOL)inArgs[3];
    phyReg = (GT_U16)inArgs[4];
    phyDev = (GT_U8)inArgs[5];

    if (useExternalPhy == GT_FALSE)
    {
        result = GT_NOT_SUPPORTED;
    }
    else
    {
        /* call cpss api function */
        result = cpssExMxPmPhyPort10GSmiRegisterRead(devNum, phyId, phyReg, phyDev, &data);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", data);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPhyAutonegSmiEnableSet
*
* DESCRIPTION:
*       Enable/disable the auto negotiation process, between the PP and PHY.
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number.
*       enabled     - GT_TRUE  = Auto-Negotiation process is performed.
*                     GT_FALSE = Auto-Negotiation process is not performe.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong devNum or portNum.
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       Although the device ignores the information read from the PHY
*       registers, it keeps polling those registers.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPhyAutonegSmiEnableSet
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPhyAutonegSmiEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPhyAutonegSmiEnableGet
*
* DESCRIPTION:
*       Get the auto negotiation status between the PP and PHY.
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number.
*
* OUTPUTS:
*       enabledPtr   - (Pointer to) the auto negotiation process state between
*                      PP and Phy:
*                     GT_TRUE  = Auto-Negotiation process is performed.
*                     GT_FALSE = Auto-Negotiation process is not performe.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong devNum or portNum.
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       Although the device ignores the information read from the PHY
*       registers, it keeps polling those registers.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPhyAutonegSmiEnableGet
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
    GT_BOOL enabled;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPhyAutonegSmiEnableGet(devNum, portNum, &enabled);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enabled);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPhySmiAutoMediaSelectSet
*
* DESCRIPTION:
*       Set a bit per port bitmap indicating whether this port is
*       connected to a dual-media PHY, such as Marvell 88E1112, that is able
*       to perform Auto-media select.
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum    - device number.
*       portNum   - physical port number.
*       enable    - GT_TRUE = Port is connected to dual-media PHY
*                   GT_FALSE = Port is not connected to dual-media PHY
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE - phyAddr bigger then 31
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPhySmiAutoMediaSelectSet
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPhySmiAutoMediaSelectSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPhySmiAutoMediaSelectGet
*
* DESCRIPTION:
*       Get a bit per port bitmap indicating whether this port is
*       connected to a dual-media PHY, such as Marvell 88E1112, that is able
*       to perform Auto-media select.
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum          - device number.
*       portNum         - physical port number.
*
* OUTPUTS:
*       enablePtr - (Pointer to) port state.
*                   GT_TRUE  = Port is connected to dual-media PHY
*                   GT_FALSE = Port is not connected to dual-media PHY
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong devNum or portNum.
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR       - for NULL pointer
*       GT_FAIL          - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPhySmiAutoMediaSelectGet
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPhySmiAutoMediaSelectGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmPhyPortAddrSet",
         &wrCpssExMxPmPhyPortAddrSet,
         3, 0},
        {"cpssExMxPmPhyPortSmiInit",
         &wrCpssExMxPmPhyPortSmiInit,
         1, 0},
        {"cpssExMxPmPhyPortSmiRegisterWrite",
         &wrCpssExMxPmPhyPortSmiRegisterWrite,
         4, 0},
        {"cpssExMxPmPhyPortSmiRegisterRead",
         &wrCpssExMxPmPhyPortSmiRegisterRead,
         3, 0},
        {"cpssExMxPmPhyPort10GSmiRegisterWrite",
         &wrCpssExMxPmPhyPort10GSmiRegisterWrite,
         7, 0},
        {"cpssExMxPmPhyPort10GSmiRegisterRead",
         &wrCpssExMxPmPhyPort10GSmiRegisterRead,
         6, 0},
        {"cpssExMxPmPhyAutonegSmiEnableSet",
         &wrCpssExMxPmPhyAutonegSmiEnableSet,
         3, 0},
        {"cpssExMxPmPhyAutonegSmiEnableGet",
         &wrCpssExMxPmPhyAutonegSmiEnableGet,
         2, 0},
        {"cpssExMxPmPhySmiAutoMediaSelectSet",
         &wrCpssExMxPmPhySmiAutoMediaSelectSet,
         3, 0},
        {"cpssExMxPmPhySmiAutoMediaSelectGet",
         &wrCpssExMxPmPhySmiAutoMediaSelectGet,
         3, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmPhySmi
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
GT_STATUS cmdLibInitCpssExMxPmPhySmi
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

