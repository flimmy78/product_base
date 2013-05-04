/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmNstBrg.c
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
#include <cpss/exMxPm/exMxPmGen/nst/cpssExMxPmNstBrg.h>

/*******************************************************************************
* cpssExMxPmNstBrgAccessMatrixCmdSet
*
* DESCRIPTION:
*     Set bridge access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP.
*     The device supports up to 16  SA and up to 16 DA levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*      command        - command assigned to a packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNstBrgAccessMatrixCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 saAccessLevel;
    GT_U32 daAccessLevel;
    CPSS_PACKET_CMD_ENT command;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    saAccessLevel = (GT_U32)inArgs[1];
    daAccessLevel = (GT_U32)inArgs[2];
    command = (CPSS_PACKET_CMD_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmNstBrgAccessMatrixCmdSet(devNum, saAccessLevel, daAccessLevel, command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstBrgAccessMatrixCmdGet
*
* DESCRIPTION:
*     Get bridge access matrix entry.
*     The packet is assigned two access levels based on its MAC SA/SIP and MAC
*     DA/DIP.
*     The device supports up to 16  SA and up to 16 DA levels.
*     The Access Matrix Configuration tables controls which access level
*     pairs can communicate with each other. Based on the access level pair,
*     the packet is assigned a command of: FORWARD, HARD DROP, and SOFT DROP.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*      devNum         - physical device number
*      saAccessLevel  - Source address access level
*      daAccessLevel  - Destination address access level
*
* OUTPUTS:
*      commandPtr     - command assigned to a packet
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, saAccessLevel, daAccessLevel.
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on NULL pointer parameter value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNstBrgAccessMatrixCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 saAccessLevel;
    GT_U32 daAccessLevel;
    CPSS_PACKET_CMD_ENT command;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    saAccessLevel = (GT_U32)inArgs[1];
    daAccessLevel = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmNstBrgAccessMatrixCmdGet(devNum, saAccessLevel, daAccessLevel, &command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", command);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstBrgPortAccessMatrixEnableSet
*
* DESCRIPTION:
*     Set enable/disable MAC DA/SA bridge access matrix commands.
*     see the cpssExMxPmNstBrgAccessMatrixCmdSet description
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*      devNum         - physical device number
*      port           - port number
*      enable         - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, port
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNstBrgPortAccessMatrixEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmNstBrgPortAccessMatrixEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstBrgPortAccessMatrixEnableGet
*
* DESCRIPTION:
*     Get enable/disable status MAC DA/SA bridge access matrix commands.
*     see the cpssExMxPmNstBrgAccessMatrixCmdSet description
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*      devNum         - physical device number
*      port           - port number
*
* OUTPUTS:
*      enablePtr      - GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, port
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on NULL pointer parameter value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNstBrgPortAccessMatrixEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmNstBrgPortAccessMatrixEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstBrgDefaultAccessLevelsSet
*
* DESCRIPTION:
*     Set default access levels for  Bridge Access Matrix Configuration table
*     that controls which access level pairs can communicate with each other.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*      devNum         - physical device number
*      paramType    -   type of bridge access matrix global parameter to
*                       configure default access level .
*      accessLevel  -  default security access level for parameter
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum,paramType,accessLevel
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNstBrgDefaultAccessLevelsSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_NST_AM_PARAM_ENT paramType;
    GT_U32 accessLevel;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    paramType = (CPSS_NST_AM_PARAM_ENT)inArgs[1];
    accessLevel = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmNstBrgDefaultAccessLevelsSet(devNum, paramType, accessLevel);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstBrgDefaultAccessLevelsGet
*
* DESCRIPTION:
*     Get default access levels for  Bridge Access Matrix Configuration table
*     that controls which access level pairs can communicate with each other.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*      devNum         - physical device number
*      paramType      - type of bridge access matrix global parameter to
*                        configure default access level .
* OUTPUTS:
*      accessLevelPtr - default security access level for parameter
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum,paramType
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on NULL pointer parameter value
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNstBrgDefaultAccessLevelsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_NST_AM_PARAM_ENT paramType;
    GT_U32 accessLevel;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    paramType = (CPSS_NST_AM_PARAM_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmNstBrgDefaultAccessLevelsGet(devNum, paramType, &accessLevel);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", accessLevel);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstBrgProtSanityCheckSet
*
* DESCRIPTION:
*      Set packet sanity checks.
*      Sanity Check engine identifies "suspicious" packets and
*      provides an option for assigning them a Hard Drop packet command.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*      devNum      - physical device number
*      checkType - the type of sanity check
*      enable    - GT_TRUE  enable packet's sanity check. Packet that not passed
*                  check will be dropped and treated as security breach event.
*                - GT_FALSE disable packet's sanity check.
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum,checkType
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNstBrgProtSanityCheckSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_NST_CHECK_ENT checkType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    checkType = (CPSS_NST_CHECK_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmNstBrgProtSanityCheckSet(devNum, checkType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstBrgProtSanityCheckGet
*
* DESCRIPTION:
*      Get packet sanity checks.
*      Sanity Check engine identifies "suspicious" packets and
*      provides an option for assigning them a Hard Drop packet command.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*      devNum      - physical device number
*      checkType - the type of sanity check
* OUTPUTS:
*      enablePtr - GT_TRUE  enable packet's sanity check. Packet that not passed
*                  check will be dropped and treated as security breach event.
*                - GT_FALSE disable packet's sanity check.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum,checkType
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on NULL pointer parameter value
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNstBrgProtSanityCheckGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_NST_CHECK_ENT checkType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    checkType = (CPSS_NST_CHECK_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmNstBrgProtSanityCheckGet(devNum, checkType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmNstBrgAccessMatrixCmdSet",
         &wrCpssExMxPmNstBrgAccessMatrixCmdSet,
         4, 0},
        {"cpssExMxPmNstBrgAccessMatrixCmdGet",
         &wrCpssExMxPmNstBrgAccessMatrixCmdGet,
         3, 0},
        {"cpssExMxPmNstBrgPortAccessMatrixEnableSet",
         &wrCpssExMxPmNstBrgPortAccessMatrixEnableSet,
         3, 0},
        {"cpssExMxPmNstBrgPortAccessMatrixEnableGet",
         &wrCpssExMxPmNstBrgPortAccessMatrixEnableGet,
         2, 0},
        {"cpssExMxPmNstBrgDefaultAccessLevelsSet",
         &wrCpssExMxPmNstBrgDefaultAccessLevelsSet,
         3, 0},
        {"cpssExMxPmNstBrgDefaultAccessLevelsGet",
         &wrCpssExMxPmNstBrgDefaultAccessLevelsGet,
         2, 0},
        {"cpssExMxPmNstBrgProtSanityCheckSet",
         &wrCpssExMxPmNstBrgProtSanityCheckSet,
         3, 0},
        {"cpssExMxPmNstBrgProtSanityCheckGet",
         &wrCpssExMxPmNstBrgProtSanityCheckGet,
         2, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmNstBrg
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
GT_STATUS cmdLibInitCpssExMxPmNstBrg
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

