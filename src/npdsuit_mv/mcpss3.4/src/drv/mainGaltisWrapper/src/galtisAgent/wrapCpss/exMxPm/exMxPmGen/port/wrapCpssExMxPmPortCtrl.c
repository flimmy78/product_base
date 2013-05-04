/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmPortCtrl.c
*
* DESCRIPTION:
*       Wrapper functions for
*           cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortCtrl.h API's
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
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortCtrl.h>

/*******************************************************************************
* cpssExMxPmPortEnableSet
*
* DESCRIPTION:
*       Enable/disable MAC (Rx and Tx) per port.
.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number, CPU port number
*       enable   - GT_TRUE:  enable port,
*                  GT_FALSE: disable port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortEnableSet
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
    result = cpssExMxPmPortEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable port status
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number, CPU port number.
*
* OUTPUTS:
*       enablePtr - Pointer to the  enable/disable state of the port.
*                  GT_TRUE for enabled port, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortEnableGet
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
    result = cpssExMxPmPortEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortDuplexModeSet
*
* DESCRIPTION:
*       Set duplex mode for specified port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number or CPU port
*       dMode    - CPSS_PORT_HALF_DUPLEX_E,
*                  CPSS_PORT_FULL_DUPLEX_E.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or dMode
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported duplex mode
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortDuplexModeSet
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
    CPSS_PORT_DUPLEX_ENT dMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    dMode = (CPSS_PORT_DUPLEX_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortDuplexModeSet(devNum, portNum, dMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortDuplexModeGet
*
* DESCRIPTION:
*       Gets duplex mode for specified port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       dModePtr - pointer to duplex mode:
*                   - CPSS_PORT_HALF_DUPLEX_E,
*                   - CPSS_PORT_FULL_DUPLEX_E
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortDuplexModeGet
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
    CPSS_PORT_DUPLEX_ENT dMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortDuplexModeGet(devNum, portNum, &dMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified port .
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number or CPU port
*       speed    - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or port speed
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       GE ports supports 10/100/1000MB, XG support higher then 1GE speed
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortSpeedSet
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
    CPSS_PORT_SPEED_ENT speed;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    speed = (CPSS_PORT_SPEED_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortSpeedSet(devNum, portNum, speed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       GE ports supports 10/100/1000MB, XG support higher then 1GE speed.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortSpeedGet
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
    CPSS_PORT_SPEED_ENT speed;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortSpeedGet(devNum, portNum, &speed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", speed);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortDuplexAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for MAC duplex mode per port.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode can be
*       set manually.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       enable   - GT_TRUE to enable Auto-Negotiation for duplex mode,
*                - GT_FALSE to disable Auto-Negotiation for duplex mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported state
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortDuplexAutoNegEnableSet
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
    result = cpssExMxPmPortDuplexAutoNegEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortDuplexAutoNegEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of an Auto-Negotiation for MAC duplex mode
*       per port.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode can be
*       set manually.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*
*
* OUTPUTS:
*       enablePtr - pointer to enable/disable status of an Auto-Negotiation for
*                  MAC duplex mode:
*                  - GT_TRUE to enable Auto-Negotiation for duplex mode,
*                  - GT_FALSE to disable Auto-Negotiation for duplex mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortDuplexAutoNegEnableGet
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
    result = cpssExMxPmPortDuplexAutoNegEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for Flow Control per port
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum         - device number
*       portNum        - physical port number
*       enable         - Auto-Negotiation Flow Control state:
*                           GT_TRUE - enable
*                           GT_FALSE - disable
*       pauseAdvertise - Advertise symmetric flow control support in
*                        Auto-Negotiation:
*                           GT_TRUE -  enable.
*                           GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported state
*
* COMMENTS:
*       Flow Control Auto-Negotiation is applicable only for the Tri-speed ports
*       CPU port doesn't support the Flow Control Auto-Negotiation.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlAutoNegEnableSet
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
    GT_BOOL pauseAdvertise;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];
    pauseAdvertise = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortFlowControlAutoNegEnableSet(devNum, portNum, enable, pauseAdvertise);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlAutoNegEnableGet
*
* DESCRIPTION:
*       Get Auto-Negotiation enable/disable state for Flow Control per port
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum            - device number
*       portNum           - physical port number
*
* OUTPUTS:
*       enablePtr         - pointer to Auto-Negotiation Flow Control state:
*                           GT_TRUE - enable
*                           GT_FALSE - disable
*       pauseAdvertisePtr - pointer to Advertise symmetric flow control support
*                           state in Auto-Negotiation:
*                           GT_TRUE -  enable.
*                           GT_FALSE - disable
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       Flow Control Auto-Negotiation is applicable only for the Tri-speed ports
*       CPU port doesn't support the Flow Control Auto-Negotiation.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlAutoNegEnableGet
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
    GT_BOOL pauseAdvertise;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortFlowControlAutoNegEnableGet(devNum, portNum, &enable, &pauseAdvertise);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable, pauseAdvertise);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortSpeedAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation of interface speed per port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       enable    - GT_TRUE to enable Auto-Negotiation of interface speed
*                - GT_FALSE to desable Auto-Negotiation of interface speed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_SUPPORTED - on not supported state
*
* COMMENTS:
*       CPU port not supports the speed auto negotiation
*       It is applicable only for the Tri-speed ports
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortSpeedAutoNegEnableSet
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
    result = cpssExMxPmPortSpeedAutoNegEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortSpeedAutoNegEnableGet
*
* DESCRIPTION:
*       Get Auto-Negotiation of interface speed status per port
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*
* OUTPUTS:
*       enablePtr - pointer to Auto-Negotiation of interface speed status :
*                    - GT_TRUE to enable Auto-Negotiation of interface speed
*                    - GT_FALSE to desable Auto-Negotiation of interface speed.
*.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_SUPPORTED - on not supported state
*
* COMMENTS:
*       CPU port not supports the speed auto negotiation
*       It is applicable only for the Tri-speed ports
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortSpeedAutoNegEnableGet
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
    result = cpssExMxPmPortSpeedAutoNegEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlEnableSet
*
* DESCRIPTION:
*       Enable/disable receiving and transmission of 802.3x Flow Control frames
*       in full duplex per port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*       enable   - GT_TRUE to enable Flow Control
*                - GT_FALSE to disable Flow Control
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlEnableSet
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
    result = cpssExMxPmPortFlowControlEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlEnableGet
*
* DESCRIPTION:
*       Get status of 802.3x Flow Control on specific physical port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr - Pointer to the Flow Control of the port.
*                  GT_TRUE for enabled Flow Control, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlEnableGet
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
    result = cpssExMxPmPortFlowControlEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlXonPeriodicEnableSet
*
* DESCRIPTION:
*       To enable/disable periodic X-on 802.3x Flow Control packets transmission
*
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*       enable   - If GT_TRUE,  enable periodic X-on flow control transmit.
*                  If GT_FALSE, disable periodic X-on flow control transmit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       It is relevant when Flow Control frames transmission is enabled.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlXonPeriodicEnableSet
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
    result = cpssExMxPmPortFlowControlXonPeriodicEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlXonPeriodicEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of periodic X-on 802.3x Flow Control packets
*       transmission.
*
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr - pointer to periodic X-on status:
*                   If GT_TRUE, periodic X-on flow control transmit is enabled.
*                   If GT_FALSE,periodic X-on flow control transmit is disabled.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       It is relevant when Flow Control frames transmission is enabled.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlXonPeriodicEnableGet
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
    result = cpssExMxPmPortFlowControlXonPeriodicEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlXoffPeriodicEnableSet
*
* DESCRIPTION:
*       To enable/disable periodic X-off 802.3x Flow Control packets
*       transmission. It is applicable only for the HyperG.Stack ports.
*
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*       enable   - If GT_TRUE,  enable periodic X-off flow control transmit.
*                  If GT_FALSE, disable periodic X-off flow control transmit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported state
*
* COMMENTS:
*       It is relevant when Flow Control frames transmission is enabled.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlXoffPeriodicEnableSet
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
    result = cpssExMxPmPortFlowControlXoffPeriodicEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlXoffPeriodicEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of periodic X-off 802.3x Flow Control packets
*       transmission. It is applicable only for the HyperG.Stack ports.
*
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr - pointer to periodic X-on status:
*                   If GT_TRUE, periodic X-off flow control transmit is enabled
*                   If GT_FALSE,periodic X-off flow control transmit is disabled
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       It is relevant when Flow Control frames transmission is enabled.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlXoffPeriodicEnableGet
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
    result = cpssExMxPmPortFlowControlXoffPeriodicEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortBackPressureEnableSet
*
* DESCRIPTION:
*       Enable/disable of Back Pressure in half-duplex per port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*       enable   - GT_TRUE to enable Back Pressure
*                - GT_FALSE to disable Back Pressure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported feature for a port
*
* COMMENTS:
*       For XGMII (10 Gbps) ports feature is not supported.
*       For tri-speed ports relevant only when ports operates in 10/100Mb.
*       If the port is enabled then the function disables the port before the
*       operation and re-enables it at the end.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortBackPressureEnableSet
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
    result = cpssExMxPmPortBackPressureEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortBackPressureEnableGet
*
* DESCRIPTION:
*       Enable/disable of Back Pressure in half-duplex per port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*      enablePtr  - pointer to back pressure enable/disable state:
*                       - GT_TRUE to enable Back Pressure
*                       - GT_FALSE to disable Back Pressure.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported feature for a port
*
* COMMENTS:
*       For XGMII (10 Gbps) ports feature is not supported.
*       For tri-speed ports relevant only when ports operates in 10/100Mb.
*       If the port is enabled then the function disables the port before the
*       operation and re-enables it at the end.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortBackPressureEnableGet
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
    result = cpssExMxPmPortBackPressureEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortLinkStatusGet
*
* DESCRIPTION:
*       Gets Link Status per port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       isLinkUpPtr - pointer to link status:
*           GT_TRUE for link up, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortLinkStatusGet
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
    GT_BOOL isLinkUp;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortLinkStatusGet(devNum, portNum, &isLinkUp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isLinkUp);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortInterfaceModeGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       ifModePtr - pointer to interface mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortInterfaceModeGet
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
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortInterfaceModeGet(devNum, portNum, &ifMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ifMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortForceLinkUpSet
*
* DESCRIPTION:
*       Force Link Pass(Up) per port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number (or CPU port)
*       force     - GT_TRUE - force link pass,
*                 - GT_FALSE  - don't force link pass.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       When both <ForceLink Down> and <ForceLink Pass> are HIGH, the
*       link is forced UP. When both ForceLinkDown and ForceLinkPass
*       are LOW, the port link state is determined according to the line state
*       that is determined by Auto-Negotiation.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortForceLinkUpSet
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
    GT_BOOL force;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    force = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortForceLinkUpSet(devNum, portNum, force);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortForceLinkUpGet
*
* DESCRIPTION:
*       Get Force  Link UP state per port regardless of the status of the line.
*
*
* APPLICABLE DEVICES: EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*
*
* OUTPUTS:
*       forcePtr - pointer to force link up state:
*                  GT_TRUE  -  force link up,
*                  GT_FALSE -  don't force link up .
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*        When both <ForceLink Down> and <ForceLink Pass> are HIGH, the
*       link is forced UP. When both ForceLinkDown and ForceLinkPass
*       are LOW, the port link state is determined according to the line state
*       that is determined by Auto-Negotiation.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortForceLinkUpGet
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
    GT_BOOL force;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortForceLinkUpGet(devNum, portNum, &force);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", force);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortForceLinkDownSet
*
* DESCRIPTION:
*       Force/Unforce  Link per port to Link DOWN state,  regardless of the
*       status  of the line.
*
* APPLICABLE DEVICES: EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*       force    - GT_TRUE for force link down,
*                  GT_FALSE for don't force link down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       When both <ForceLink Down> and <ForceLink Pass> are HIGH, the
*       link is forced UP. When both ForceLinkDown and ForceLinkPass
*       are LOW, the port link state is determined according to the line state
*       that is determined by Auto-Negotiation.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortForceLinkDownSet
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
    GT_BOOL force;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    force = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortForceLinkDownSet(devNum, portNum, force);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortForceLinkDownGet
*
* DESCRIPTION:
*       Get Force Link DOWN state per port regardless of the status of the line.
*
*
* APPLICABLE DEVICES: EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*
*
* OUTPUTS:
*       forcePtr - pointer to force link down state:
*                  GT_TRUE  -  force link down,
*                  GT_FALSE -  don't force link down .
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       When both <ForceLink Down> and <ForceLink Pass> are HIGH, the
*       link is forced UP. When both ForceLinkDown and ForceLinkPass
*       are LOW, the port link state is determined according to the line state
*       that is determined by Auto-Negotiation.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortForceLinkDownGet
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
    GT_BOOL force;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortForceLinkDownGet(devNum, portNum, &force);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", force);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMruSet
*
* DESCRIPTION:
*       Sets the Maximal Receive Packet size per port
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number (or CPU port)
*       mruSize    - max receive packet size in bytes.
*                    value must be even.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or
*                          odd value of mruSize
*       GT_HW_ERROR      - on hardware error
*       GT_OUT_OF_RANGE  - mruSize > 10240
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMruSet
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
    GT_U32 mruSize;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    mruSize = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortMruSet(devNum, portNum, mruSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMruGet
*
* DESCRIPTION:
*       Get the Maximal Receive Packet size per port
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*       mruSizePtr  - pointer to Maximal Receive Packet size in bytes.
*                     Value must be even.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or
*                          odd value of mruSize
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_OUT_OF_RANGE  - mruSize > 10240
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMruGet
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
    GT_U32 mruSize;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortMruGet(devNum, portNum, &mruSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mruSize);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortCrcCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable CRC checking on received packets.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number (or CPU port)
*       enable     - If GT_TRUE, enable CRC checking
*                    If GT_FALSE, disable CRC checking
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortCrcCheckEnableSet
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
    result = cpssExMxPmPortCrcCheckEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortCrcCheckEnableGet
*
* DESCRIPTION:
*       Get CRC checking (Enable/Disable) state for received packets.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*        enablePtr - Pointer to the CRS checking state :
*                       GT_TRUE  - CRC checking is enable,
*                       GT_FALSE - CRC checking is disable.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortCrcCheckEnableGet
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
    result = cpssExMxPmPortCrcCheckEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxIpgModeSet
*
* DESCRIPTION:
*       Configure  XGMII Transmit Inter-Packet Gap mode on specified port on
*       specified device.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       ipgMode - CPSS_PORT_XGMII_LAN_E,
*                 CPSS_PORT_XGMII_WAN_E,
*                 CPSS_PORT_XGMII_FIXED_E
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxIpgModeSet
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
    CPSS_PORT_XGMII_MODE_ENT ipgMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ipgMode = (CPSS_PORT_XGMII_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxIpgModeSet(devNum, portNum, ipgMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxIpgModeGet
*
* DESCRIPTION:
*       Get XGMII Transmit Inter-Packet Gap mode on specified port on
*       specified device.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       ipgmodePtr - pointer to ipg mode:
*                       CPSS_PORT_XGMII_LAN_E,
*                       CPSS_PORT_XGMII_WAN_E,
*                       PSS_PORT_XGMII_FIXED_E
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxIpgModeGet
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
    CPSS_PORT_XGMII_MODE_ENT ipgmode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxIpgModeGet(devNum, portNum, &ipgmode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ipgmode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFixedIpgBaseSet
*
* DESCRIPTION:
*       Configure the IPG base for Fixed IPG mode.
*       Relevant when ipg mode is PSS_PORT_XGMII_MODE_FIXED_E.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum       - device number
*       portNum      - physical port number
*       ipgLength    - base IPG length. Can be 8 or 12
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM - on wrong port number or device or ipgbaseIndex
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFixedIpgBaseSet
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
    GT_U32 ipgLength;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ipgLength = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortFixedIpgBaseSet(devNum, portNum, ipgLength);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFixedIpgBaseGet
*
* DESCRIPTION:
*       Configure the IPG base for Fixed IPG mode.
*       Relevant when ipg mode is PSS_PORT_XGMII_MODE_FIXED_E.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number
*
* OUTPUTS:
*       ipgLengthPtr -  pointer to base IPG length. Can be 8 or 12.
*
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device or ipgbaseIndex
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFixedIpgBaseGet
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
    GT_U32 ipgLength;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortFixedIpgBaseGet(devNum, portNum, &ipgLength);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ipgLength);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortExtraIpgSet
*
* DESCRIPTION:
*       Sets the number of 32-bit words to add to the 12-byte IPG.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       number   - number of extra words (range: 0-3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM - on wrong port number or device or number
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       Extra Ipg field contains the number of 32-bit words to add to the
*       12-byte base IPG or 8-byte base IPG. Hence, 12+4*ExtraIPG is
*       the basis for the entire IPG calculation.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortExtraIpgSet
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
    GT_U32 number;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    number = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortExtraIpgSet(devNum, portNum, number);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortExtraIpgGet
*
* DESCRIPTION:
*       Gets the number of 32-bit words to add to the 12-byte IPG.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       numberPtr   - pointer to number of extra words (range: 0-3).
*
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       Extra Ipg field contains the number of 32-bit words to add to the
*       12-byte base IPG or 8-byte base IPG. Hence, 12+4*ExtraIPG is
*       the basis for the entire IPG calculation.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortExtraIpgGet
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
    GT_U32 number;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortExtraIpgGet(devNum, portNum, &number);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", number);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortLocalFaultGet
*
* DESCRIPTION:
*       Get bit that indicates if the XGMII Reconciliation Sublayer  has
*       detected local fault messages.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum         - device number
*       portNum        - physical port number
*
* OUTPUTS:
*       *isLocalFaultPtr  - pointer to Xgmii Local Fault status:
*                         - GT_TRUE, if bit set. The bit is set when
*                           XGMII MAC Reconciliation Sublayer (RS) has detected
*                           local fault messages or ForceLink Down is GT_TRUE.
*                         - GT_FALSE, if bit is not set. The bit is not set when
*                           XGMII MAC Reconciliation Sublayer (RS) has not
*                           detected local fault messages or ForceLink Pass is
*                           GT_TRUE.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       none
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortLocalFaultGet
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
    GT_BOOL isLocalFault;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortLocalFaultGet(devNum, portNum, &isLocalFault);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isLocalFault);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortRemoteFaultGet
*
* DESCRIPTION:
*       Gets bit that indicates if the XGMII Reconciliation Sublayer has
*       detected remote fault messages.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number
*
* OUTPUTS:
*       *isRemoteFaultPtr  - GT_TRUE, if bit set. This bit is set when
*                            XGMII MAC Reconciliation Sublayer (RS) has detected
*                            remote fault messages or ForceLink Down is GT_TRUE.
*                          - GT_FALSE, if bit is not set. This bit isn't set
*                            when XGMII MAC Reconciliation Sublayer (RS) has
*                            not detected local fault messages or ForceLink Pass
*                            is GT_TRUE.
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortRemoteFaultGet
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
    GT_BOOL isRemoteFault;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortRemoteFaultGet(devNum, portNum, &isRemoteFault);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isRemoteFault);

    return CMD_OK;
}

 /*******************************************************************************
* cpssExMxPmPortMacStatusGet
*
* DESCRIPTION:
*       Reads bits that indicate different problems on specified port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number
*
* OUTPUTS:
*       portMacStatusPtr  - info about port MAC status.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U8 devNum;
static GT_U8 portNum;

static GT_STATUS portMacStatusGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    CPSS_PORT_MAC_STATUS_STC        portMacStatus;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
         return GT_INIT_ERROR;

    /* call cpss api function */
    result = cpssExMxPmPortMacStatusGet(devNum, portNum, &portMacStatus);

    if(result == GT_BAD_PARAM)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return GT_BAD_PARAM;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return GT_OK;
    }

    inFields[0] = devNum;
    inFields[1] = portNum;
    inFields[2] =portMacStatus.isPortRxPause;
    inFields[3] =portMacStatus.isPortTxPause;
    inFields[4] =portMacStatus.isPortBackPres;
    inFields[5] =portMacStatus.isPortBufFull;
    inFields[6] =portMacStatus.isPortSyncFail;
    inFields[7] =portMacStatus.isPortHiErrorRate;
    inFields[8] =portMacStatus.isPortAnDone;
    inFields[9] =portMacStatus.isPortFatalError;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d",  inFields[0], inFields[1],
                      inFields[2], inFields[3], inFields[4],
                      inFields[5], inFields[6], inFields[7],
                      inFields[8], inFields[9]);

    galtisOutput(outArgs, result, "%f");

    return GT_OK;
}

/*******************************************************************************/
  static CMD_STATUS wrCpssExMxPmPortMacStatusGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;
    devNum = 0;
    portNum = 0;

    while(devNum < 128)
    {
        result = portMacStatusGet(inArgs,inFields,numFields,outArgs);
        if(result == GT_BAD_PARAM)
        {
            portNum++;
            if(portNum == 64)
            {
                portNum = 0;
                devNum++;
            }
        }
        else break;
    }
    if (devNum == 128)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    portNum++;
    return CMD_OK;
}

/*******************************************************************************/
  static CMD_STATUS wrCpssExMxPmPortMacStatusGetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result = GT_OK;

    while(devNum < 128)
    {
        result = portMacStatusGet(inArgs,inFields,numFields,outArgs);
        if(result == GT_BAD_PARAM)
        {
            portNum++;
            if(portNum == 64)
            {
                portNum = 0;
                devNum++;
            }
        }
        else break;
    }
    if (devNum == 128)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    portNum++;
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortInternalLoopbackEnableSet
*
* DESCRIPTION:
*       Set the internal Loopback state on the specified port on the specified
*       device.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       enable    - If GT_TRUE, enable loopback
*                   If GT_FALSE, disable loopback
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_NOT_SUPPORTED - The feature is not supported the port
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortInternalLoopbackEnableSet
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
    result = cpssExMxPmPortInternalLoopbackEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortInternalLoopbackEnableGet
*
* DESCRIPTION:
*       Get Internal Loopback state
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number
*
* OUTPUTS:
*       enablePtr  - Pointer to the Loopback state.
*                    If GT_TRUE, loopback is enabled
*                    If GT_FALSE, loopback is disabled
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*        None.
********************************************************************************/
static CMD_STATUS wrCpssExMxPmPortInternalLoopbackEnableGet
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
    result = cpssExMxPmPortInternalLoopbackEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortInbandAutoNegEnableSet
*
* DESCRIPTION:
*       Configure MAC Auto-Negotiation mode for a port.
*       The Tri-Speed port MAC may operate in one of the following two modes:
*       - SGMII Mode - In this mode, Auto-Negotiation may be performed
*         out-of-band via the device's Master SMI interface or in-band.
*         The function sets the mode of Auto-Negotiation to in-band or
*         out-of-band.
*       - 1000BASE-X mode - In this mode, the port operates at 1000 Mbps,
*         full-duplex only and supports in-band Auto-Negotiation for link and
*         for Flow Control.
*         The function set in-band Auto-Negotiation mode only.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number
*       enable  - GT_TRUE  - Auto-Negotiation works in in-band mode.
*                 GT_FALSE - Auto-Negotiation works in out-of-band via
*                            the device's Master SMI interface mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success
*       GT_BAD_PARAM       - on wrong port number or device
*       GT_HW_ERROR        - on hardware error
*       GT_NOT_SUPPORTED   - The feature is not supported.
*
* COMMENTS:
*       Not relevant for CPU port  and not supported for XG ports.
*       SGMII port In-band Auto-Negotiation is performed by the PCS layer to
*       establish link, speed, and duplex mode.
*       1000BASE-X port In-band Auto-Negotiation is performed by the PCS layer
*       to establish link and flow control support.
*       The change of the Auto-Negotiation causes temporary change of the link
*       to down and up for ports with link up.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortInbandAutoNegEnableSet
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
    result = cpssExMxPmPortInbandAutoNegEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortInbandAutoNegEnableGet
*
* DESCRIPTION:
*       Get MAC Auto-Negotiation mode for a port.
*       The Tri-Speed port MAC may operate in one of the following two modes:
*       - SGMII Mode - In this mode, Auto-Negotiation may be performed
*         out-of-band via the device's Master SMI interface or in-band.
*         The function sets the mode of Auto-Negotiation to in-band or
*         out-of-band.
*       - 1000BASE-X mode - In this mode, the port operates at 1000 Mbps,
*         full-duplex only and supports in-band Auto-Negotiation for link and
*         for Flow Control.
*         The function set in-band Auto-Negotiation mode only.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
* OUTPUTS:
*       enablePtr - pointer to Auto-Negotiation value :
*                   - GT_TRUE  - Auto-Negotiation works in in-band mode.
*                   - GT_FALSE - Auto-Negotiation works in out-of-band via
*                                the device's Master SMI interface mode.
*
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED   - The feature is not supported.
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       Not relevant for CPU port  and not supported for XG ports.
*       SGMII port In-band Auto-Negotiation is performed by the PCS layer to
*       establish link, speed, and duplex mode.
*       1000BASE-X port In-band Auto-Negotiation is performed by the PCS layer
*       to establish link and flow control support.
*       The change of the Auto-Negotiation causes temporary change of the link
*       to down and up for ports with link up.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortInbandAutoNegEnableGet
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
    result = cpssExMxPmPortInbandAutoNegEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortAttributesGet
*
* DESCRIPTION:
*       Gets port attributes per port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number
*
* OUTPUTS:
*       portAttributesPtr - Pointer to attributes values array.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong port number or device
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/

static GT_U8 devNum;
static GT_U8 portNum;

 static GT_STATUS portAttributesGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result1, result2, result3,
            result4, result5, result6, result7;

    CPSS_PORT_ATTRIBUTES_STC        portAttributSetArray;
    OUT CPSS_PORT_INTERFACE_MODE_ENT ifMode;
    GT_BOOL                         flowCtrlEnable;
    GT_BOOL                         speedAutoNegEnable;
    GT_BOOL                         duplexAutoNegEnable;
    GT_BOOL                         flowCtrlAutoNegEnable;
    GT_BOOL                         pauseAdvertise;
    GT_U32                          mruSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return GT_INIT_ERROR;


    /* call cpss api function */
    result1 = cpssExMxPmPortAttributesGet(devNum, portNum,
                                    &portAttributSetArray);
    result2 = cpssExMxPmPortInterfaceModeGet(devNum,portNum,&ifMode);
    result3 = cpssExMxPmPortFlowControlEnableGet(devNum,portNum,&flowCtrlEnable);
    result4 = cpssExMxPmPortSpeedAutoNegEnableGet(devNum,portNum,&speedAutoNegEnable);
    result5 = cpssExMxPmPortDuplexAutoNegEnableGet(devNum,portNum,&duplexAutoNegEnable);
    result6 = cpssExMxPmPortFlowControlAutoNegEnableGet(devNum,portNum,
                                                        &flowCtrlAutoNegEnable,
                                                        &pauseAdvertise);
    result7 = cpssExMxPmPortMruGet(devNum,portNum,&mruSize);

    if(result1 == GT_NOT_APPLICABLE_DEVICE)
    {
        galtisOutput(outArgs, result1, "%d", -1);
        return GT_BAD_PARAM;
    }

    if ((result1 != GT_OK)||(result2 != GT_OK)||(result3 != GT_OK)||
        (result4 != GT_OK)||(result5 != GT_OK)||(result6 != GT_OK)||
        (result7 != GT_OK))
    {
        galtisOutput(outArgs, result1, "%d", -1);
        return GT_OK;
    }

    inFields[0] = devNum;
    inFields[1] = portNum;
    inFields[2] = portAttributSetArray.portLinkUp;
    inFields[3] = portAttributSetArray.portSpeed;
    inFields[4] = portAttributSetArray.portDuplexity;
    inFields[5] = ifMode;
    inFields[6] = flowCtrlEnable;
    inFields[7] = speedAutoNegEnable;
    inFields[8] = duplexAutoNegEnable;
    inFields[9] = flowCtrlAutoNegEnable;
    inFields[10] = pauseAdvertise;
    inFields[11] = mruSize;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d",  inFields[0],  inFields[1],  inFields[2], inFields[3], inFields[4],
                               inFields[5],  inFields[6],  inFields[7], inFields[8], inFields[9],
                               inFields[10], inFields[11]);

    galtisOutput(outArgs, result1, "%f");

    return GT_OK;
}
/*******************************************************************************/
 static CMD_STATUS wrCpssExMxPmPortAttributesGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    devNum = 0;
    portNum = 0;

    while(devNum < 64)
    {
        result = portAttributesGet(inArgs,inFields,numFields,outArgs);
        if(result == GT_NOT_APPLICABLE_DEVICE)
        {
            if(portNum == 64)
            {
                portNum = 0;
                devNum++;
            }
            portNum++;
        }
        else break;
    }
    if (devNum == 64)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    portNum++;
    return CMD_OK;
}
/*******************************************************************************/
 static CMD_STATUS wrCpssExMxPmPortAttributesGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result = GT_OK;
    while(devNum < 64)
    {
        result = portAttributesGet(inArgs,inFields,numFields,outArgs);
        if(result == GT_NOT_APPLICABLE_DEVICE)
        {
            if(portNum == 64)
            {
                portNum = 0;
                devNum++;
            }
            portNum++;
        }
        else break;
    }
    if (devNum == 64)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    portNum++;
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortPreambleLengthSet
*
* DESCRIPTION:
*       Set the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       dev          - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction and "both directions"
*                      options (GE ports support only Tx direction)
*       length       - length of preamble in bytes
*                      support only values of 4,8
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortPreambleLengthSet
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
    CPSS_PORT_DIRECTION_ENT direction;
    GT_U32 length;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[2];
    length = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortPreambleLengthSet(devNum, portNum, direction, length);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortPreambleLengthGet
*
* DESCRIPTION:
*       Get the port with preamble length for Rx or Tx  directions.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       dev          - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx  directions
*
*
* OUTPUTS:
*       lengthPtr    - pointer to preamble length in bytes :
*                      supported length values are : 4,8.
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortPreambleLengthGet
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
    CPSS_PORT_DIRECTION_ENT direction;
    GT_U32 length;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortPreambleLengthGet(devNum, portNum, direction, &length);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", length);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMacSaBaseSet
*
* DESCRIPTION:
*       Sets the base part(40 upper bits) of all device's ports MAC addresses.
*       Port MAC addresses are used as the MAC SA for Flow Control Packets
*       transmitted by the device.In addition these addresses can be used as
*       MAC DA for Flow Control packets received by these ports.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       dev - device number
*       macPtr - (pointer to)The system Mac address to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL         - on error
*
* COMMENTS:
*       The upper 40 bits of the MAC Address are the same for all ports in the
*       device and the lower eight bits are unique per port.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMacSaBaseSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_ETHERADDR mac;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    galtisMacAddr(&mac, (GT_U8*)inArgs[1]);

    /* call cpss api function */
    result = cpssExMxPmPortMacSaBaseSet(devNum, &mac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMacSaBaseGet
*
* DESCRIPTION:
*       Gets the base part (40 upper bits) of all device's ports MAC addresses.
*       Port MAC addresses are used as the MAC SA for Flow Control Packets
*       transmitted by the device.In addition these addresses can be used as
*       MAC DA for Flow Control packets received by these ports.

*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       dev - device number
*
* OUTPUTS:
*       macPtr - (pointer to)The system Mac address.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL         - on error
*
* COMMENTS:
*       The upper 40 bits of the MAC Address are the same for all ports in the
*       device and the lower eight bits are unique per port.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMacSaBaseGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_ETHERADDR mac;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPortMacSaBaseGet(devNum, &mac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%6b", mac.arEther);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMacSaLsbSet
*
* DESCRIPTION:
*       Set the least significant byte of the MAC SA of specified port on
*       specified device.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number
*       macSaLsb    - The ls byte of the MAC SA
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       The upper 40 bits of the MAC Address are the same for all ports in the
*       device and the lower eight bits are unique per port. The upper 40 bits
*       are configured by cpssExMxPmPortMacSaBaseSet.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMacSaLsbSet
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
    GT_U8 macSaLsb;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    macSaLsb = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortMacSaLsbSet(devNum, portNum, macSaLsb);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMacSaLsbGet
*
* DESCRIPTION:
*       Get the least significant byte of the MAC SA of specified port on
*       specified device.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number
*
* OUTPUTS:
*       macSaLsbPtr - pointer to ls byte of the MAC SA.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       The upper 40 bits of the MAC Address are the same for all ports in the
*       device and the lower eight bits are unique per port.The upper 40 bits
*       are configured by cpssExMxPmPortMacSaBaseSet.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMacSaLsbGet
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
    GT_U8 macSaLsb;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortMacSaLsbGet(devNum, portNum, &macSaLsb);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", macSaLsb);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxPaddingEnableSet
*
* DESCRIPTION:
*       Enable/Disable padding of transmitted packets shorter than 64Byte.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       dev          - device number.
*       portNum      - physical port number (or CPU port)
*       enable       - GT_TRUE  - padding is enable.
*                    - GT_FALSE - padding is disable.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_FAIL      - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxPaddingEnableSet
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
    result = cpssExMxPmPortTxPaddingEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxPaddingEnableGet
*
* DESCRIPTION:
*       Get (enable/disable) padding status of transmitted packets shorter
*       than 64Byte.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       dev          - device number.
*       portNum      - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr    - pointer to padding status.
*                     - GT_TRUE  - padding is enable.
*                     - GT_FALSE - padding is disable.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxPaddingEnableGet
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
    result = cpssExMxPmPortTxPaddingEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortExcessiveCollisionDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Excessive Collision Packets Drop on specified port on
*       specified device.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number, CPU port number
*       enable   - GT_TRUE:  A collided packet will be retransmitted by the
*                            device until it is transmitted without any
*                            collisions, regardless of the number of collisions
*                            on the packet.
*                  GT_FALSE: If the number of collisions on the same packet is
*                            16, it is dropped.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_NOT_SUPPORTED - on not supported port type
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortExcessiveCollisionDropEnableSet
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
    result = cpssExMxPmPortExcessiveCollisionDropEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortExcessiveCollisionDropEnableGet
*
* DESCRIPTION:
*       Get enable/disable Excessive Collision Packets Drop status on specified
*       port on specified device.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       dev   - device number
*       port  - physical port number
*
* OUTPUTS:
*       enablePtr - Pointer to the Excessive Collision Packets Drop status:
*                  - GT_TRUE:  A collided packet will be retransmitted by the
*                              device until it is transmitted without any
*                              collisions, regardless of the number of
*                              collisions on the packet.
*                  - GT_FALSE: If the number of collisions on the same packet is
*                              16, it is dropped.
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED - on not supported port type
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortExcessiveCollisionDropEnableGet
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
    result = cpssExMxPmPortExcessiveCollisionDropEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmPortPriorityFlowControlEnableSet
*
* DESCRIPTION:
*       Enable/disable per priority flow control.
*       When enabled the traffic on a specific port can be stopped according
*       to priority.
*       When disabled all the traffic is stopped (regardless of priority).
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number, CPU port number
*       enable   - GT_TRUE:  enable per priority flow control for this port
*                  GT_FALSE: disable per priority flow control for this port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortPriorityFlowControlEnableSet
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
    result = cpssExMxPmPortPriorityFlowControlEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortPriorityFlowControlEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable state for per priority flow control on this port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number, CPU port number.
*
* OUTPUTS:
*       enablePtr - Pointer to the  enable/disable state of per priority
*                   flow control for this port.
*                  GT_TRUE - per priority flow control for this port is enabled.
*                  GT_FALSE - per priority flow control for this port is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortPriorityFlowControlEnableGet
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
    result = cpssExMxPmPortPriorityFlowControlEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmPortFlowControlPeriodicTypeSet
*
* DESCRIPTION:
*       Select the type of flow control packets to be sent.
*       When Per Priority FC is enabled, select which type of periodic FC
*       to send: per priority, or per port (802.3x)
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum           - device number
*       portNum          - physical port number, CPU port number
*       fcPeriodicType   - CPSS_EXMXPM_PORT_FC_802_3X_E -
*                               802.3x flow control packets are sent.
*                          CPSS_EXMXPM_PORT_FC_PER_PRIORITY_E -
*                               per priority flow control packets are sent.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlPeriodicTypeSet
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
    CPSS_EXMXPM_PORT_FC_TYPE_ENT fcPeriodicType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    fcPeriodicType = (CPSS_EXMXPM_PORT_FC_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortFlowControlPeriodicTypeSet(devNum, portNum, fcPeriodicType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlPeriodicTypeGet
*
* DESCRIPTION:
*       Get the type of flow control packets to be sent.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       fcPeriodicTypePtr - pointer to the type of flow control packets:
*                           CPSS_EXMXPM_PORT_FC_802_3X_E,
*                           CPSS_EXMXPM_PORT_FC_PER_PRIORITY_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlPeriodicTypeGet
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
    CPSS_EXMXPM_PORT_FC_TYPE_ENT fcPeriodicType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortFlowControlPeriodicTypeGet(devNum, portNum, &fcPeriodicType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", fcPeriodicType);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmPortForwardFlowControlEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of 802.3x flow control frames (instead of
*       termination in port). When active, Flow Control packets are forwarded
*       into the ingress pipe and can be processed by it.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number, CPU port number
*       enable   - GT_TRUE:  enable forwarding of 802.3x flow control frames
*                  GT_FALSE: disable forwarding of 802.3x flow control frames
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortForwardFlowControlEnableSet
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
    result = cpssExMxPmPortForwardFlowControlEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortForwardFlowControlEnableGet
*
* DESCRIPTION:
*       Get forwarding state of 802.3x flow control frames.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number, CPU port number.
*
* OUTPUTS:
*       enablePtr - Pointer to state of forwarding 802.3x flow control frames
*                  GT_TRUE:  forwarding of 802.3x flow control frames in enabled.
*                  GT_FALSE: forwarding of 802.3x flow control frames is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortForwardFlowControlEnableGet
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
    result = cpssExMxPmPortForwardFlowControlEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortSerdesLoopbackEnableSet
*
* DESCRIPTION:
*       Enable / Disable Loopback on the SERDES differential pairs.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       enable    - GT_TRUE  -  enable loopback.
*                   GT_FALSE -  disable loopback.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For XAUI ports the function set PCS to Reset state
*       during loopback configuration.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortSerdesLoopbackEnableSet
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
    result = cpssExMxPmPortSerdesLoopbackEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortSerdesLoopbackEnableGet
*
* DESCRIPTION:
*       Get Loopback state on the SERDES differential pairs.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number
*
* OUTPUTS:
*       enablePtr  - Pointer to the SERDES Loopback state.
*                    GT_TRUE  -  enable loopback.
*                    GT_FALSE -  disable loopback.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None.
********************************************************************************/
static CMD_STATUS wrCpssExMxPmPortSerdesLoopbackEnableGet
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
    result = cpssExMxPmPortSerdesLoopbackEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);



    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortXgPcsLanesSwapSet
*
* DESCRIPTION:
*       Set swapping configuration of XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number.
*       txSerdesLaneArr - array for binding port Tx PCS and SERDES lanes:
*                         array index is SERDES lane, array value is PCS lane.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - wrong SERDES lane
*       GT_BAD_VALUE             - multiple connection detected
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortXgPcsLanesSwapSet
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
    GT_U32 txSerdesLaneArr[CPSS_EXMXPM_PORT_XG_LANES_NUM_CNS];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    txSerdesLaneArr[0] = (GT_U32)inArgs[2];
    txSerdesLaneArr[1] = (GT_U32)inArgs[3];
    txSerdesLaneArr[2] = (GT_U32)inArgs[4];
    txSerdesLaneArr[3] = (GT_U32)inArgs[5];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortXgPcsLanesSwapSet(devNum, portNum, txSerdesLaneArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortXgPcsLanesSwapGet
*
* DESCRIPTION:
*       Get swapping configuration of XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number.
*
* OUTPUTS:
*       txSerdesLaneArr - array for binding port Tx PCS and SERDES lanes:
*                         array index is SERDES lane, array value is PCS lane.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - wrong SERDES lane
*       GT_BAD_VALUE             - multiple connection detected
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortXgPcsLanesSwapGet
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
    GT_U32 txSerdesLaneArr[CPSS_EXMXPM_PORT_XG_LANES_NUM_CNS];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortXgPcsLanesSwapGet(devNum, portNum, txSerdesLaneArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d",
                 txSerdesLaneArr[0],
                 txSerdesLaneArr[1],
                 txSerdesLaneArr[2],
                 txSerdesLaneArr[3]);



    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortInbandAutoNegRestartSet
*
* DESCRIPTION:
*       Restart In-Band Auto-Negotiation on port.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - The feature is not supported.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not relevant for CPU port  and not supported for XG ports.
*       SGMII port In-band Auto-Negotiation is performed by the PCS layer to
*       establish link, speed, and duplex mode.
*       1000BASE-X port In-band Auto-Negotiation is performed by the PCS layer
*       to establish link and flow control support.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortInbandAutoNegRestartSet
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


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortInbandAutoNegRestartSet(devNum, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortInbandAutoNegBypassEnableSet
*
* DESCRIPTION:
*       Enable/Disable Auto-Negotiation bypass.
*       If the link partner doesn't respond to Auto-Negotiation process,
*       the link is established by bypassing the Auto-Negotiation procedure.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number
*       enable   - GT_TRUE  - Auto-Negotiation is bypassed.
*                - GT_FALSE - Auto-Negotiation is not bypassed.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not relevant for CPU port  and not supported for XG ports.
*       Relevant when Inband Auto-Negotiation is enabled.
*       (See cpssExMxPmPortInbandAutoNegEnableSet.)
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortInbandAutoNegBypassEnableSet
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

    /* call cpss api function */
    result = cpssExMxPmPortInbandAutoNegBypassEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortInbandAutoNegBypassEnableGet
*
* DESCRIPTION:
*       Gets Auto-Negotiation bypass status.
*       If the link partner doesn't respond to Auto-Negotiation process,
*       the link is established by bypassing the Auto-Negotiation procedure.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number.
*
* OUTPUTS:
*       enablePtr - pointer to Auto-Negotiation bypass status.
*       enable   - GT_TRUE  - Auto-Negotiation is bypassed.
*                - GT_FALSE - Auto-Negotiation is not bypassed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not relevant for CPU port  and not supported for XG ports.
*       Relevant when Inband Auto-Negotiation is enabled.
*       (See cpssExMxPmPortInbandAutoNegEnableSet.)
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortInbandAutoNegBypassEnableGet
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

    /* call cpss api function */
    result = cpssExMxPmPortInbandAutoNegBypassEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);



    return CMD_OK;
}

/******************************************************************************
* cpssExMxPmPortSerdesPowerStatusSet
*
* DESCRIPTION:
*       Sets power state of SERDES port lanes according to port capabilities.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       dev       - physical device number
*       port      - physical port number
*       powerUp   - GT_TRUE  = power up, GT_FALSE = power down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK                    - on success.
*     GT_BAD_PARAM             - on bad parameters
*     GT_FAIL                  - on error
*     GT_NOT_SUPPORTED         - HW does not support the requested operation
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortSerdesPowerStatusSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 dev;
    GT_U8 port;
    GT_BOOL powerUp;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    powerUp = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmPortSerdesPowerStatusSet(dev, port, powerUp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}




/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
       {"cpssExMxPmPortEnableSet",
         &wrCpssExMxPmPortEnableSet,
         3, 0},
        {"cpssExMxPmPortEnableGet",
         &wrCpssExMxPmPortEnableGet,
         2, 0},
        {"cpssExMxPmPortDuplexModeSet",
         &wrCpssExMxPmPortDuplexModeSet,
         3, 0},
        {"cpssExMxPmPortDuplexModeGet",
         &wrCpssExMxPmPortDuplexModeGet,
         2, 0},
        {"cpssExMxPmPortSpeedSet",
         &wrCpssExMxPmPortSpeedSet,
         3, 0},
        {"cpssExMxPmPortSpeedGet",
         &wrCpssExMxPmPortSpeedGet,
         2, 0},
        {"cpssExMxPmPortDuplexAutoNegEnableSet",
         &wrCpssExMxPmPortDuplexAutoNegEnableSet,
         3, 0},
        {"cpssExMxPmPortDuplexAutoNegEnableGet",
         &wrCpssExMxPmPortDuplexAutoNegEnableGet,
         2, 0},
        {"cpssExMxPmPortFlowControlAutoNegEnableSet",
         &wrCpssExMxPmPortFlowControlAutoNegEnableSet,
         4, 0},
        {"cpssExMxPmPortFlowControlAutoNegEnableGet",
         &wrCpssExMxPmPortFlowControlAutoNegEnableGet,
         2, 0},
        {"cpssExMxPmPortSpeedAutoNegEnableSet",
         &wrCpssExMxPmPortSpeedAutoNegEnableSet,
         3, 0},
        {"cpssExMxPmPortSpeedAutoNegEnableGet",
         &wrCpssExMxPmPortSpeedAutoNegEnableGet,
         2, 0},
        {"cpssExMxPmPortFlowControlEnableSet",
         &wrCpssExMxPmPortFlowControlEnableSet,
         3, 0},
        {"cpssExMxPmPortFlowControlEnableGet",
         &wrCpssExMxPmPortFlowControlEnableGet,
         2, 0},
        {"cpssExMxPmPortFlowControlXonPeriodicEnableSet",
         &wrCpssExMxPmPortFlowControlXonPeriodicEnableSet,
         3, 0},
        {"cpssExMxPmPortFlowControlXonPeriodicEnableGet",
         &wrCpssExMxPmPortFlowControlXonPeriodicEnableGet,
         2, 0},
        {"cpssExMxPmPortFlowControlXoffPeriodicEnableSet",
         &wrCpssExMxPmPortFlowControlXoffPeriodicEnableSet,
         3, 0},
        {"cpssExMxPmPortFlowControlXoffPeriodicEnableGet",
         &wrCpssExMxPmPortFlowControlXoffPeriodicEnableGet,
         2, 0},
        {"cpssExMxPmPortBackPressureEnableSet",
         &wrCpssExMxPmPortBackPressureEnableSet,
         3, 0},
        {"cpssExMxPmPortBackPressureEnableGet",
         &wrCpssExMxPmPortBackPressureEnableGet,
         2, 0},
        {"cpssExMxPmPortLinkStatusGet",
         &wrCpssExMxPmPortLinkStatusGet,
         2, 0},
        {"cpssExMxPmPortInterfaceModeGet",
         &wrCpssExMxPmPortInterfaceModeGet,
         2, 0},
        {"cpssExMxPmPortForceLinkUpSet",
         &wrCpssExMxPmPortForceLinkUpSet,
         3, 0},
        {"cpssExMxPmPortForceLinkUpGet",
         &wrCpssExMxPmPortForceLinkUpGet,
         2, 0},
        {"cpssExMxPmPortForceLinkDownSet",
         &wrCpssExMxPmPortForceLinkDownSet,
         3, 0},
        {"cpssExMxPmPortForceLinkDownGet",
         &wrCpssExMxPmPortForceLinkDownGet,
         2, 0},
        {"cpssExMxPmPortMruSet",
         &wrCpssExMxPmPortMruSet,
         3, 0},
        {"cpssExMxPmPortMruGet",
         &wrCpssExMxPmPortMruGet,
         2, 0},
        {"cpssExMxPmPortCrcCheckEnableSet",
         &wrCpssExMxPmPortCrcCheckEnableSet,
         3, 0},
        {"cpssExMxPmPortCrcCheckEnableGet",
         &wrCpssExMxPmPortCrcCheckEnableGet,
         2, 0},
        {"cpssExMxPmPortTxIpgModeSet",
         &wrCpssExMxPmPortTxIpgModeSet,
         3, 0},
        {"cpssExMxPmPortTxIpgModeGet",
         &wrCpssExMxPmPortTxIpgModeGet,
         2, 0},
        {"cpssExMxPmPortFixedIpgBaseSet",
         &wrCpssExMxPmPortFixedIpgBaseSet,
         3, 0},
        {"cpssExMxPmPortFixedIpgBaseGet",
         &wrCpssExMxPmPortFixedIpgBaseGet,
         2, 0},
        {"cpssExMxPmPortExtraIpgSet",
         &wrCpssExMxPmPortExtraIpgSet,
         3, 0},
        {"cpssExMxPmPortExtraIpgGet",
         &wrCpssExMxPmPortExtraIpgGet,
         2, 0},
        {"cpssExMxPmPortLocalFaultGet",
         &wrCpssExMxPmPortLocalFaultGet,
         2, 0},
        {"cpssExMxPmPortRemoteFaultGet",
         &wrCpssExMxPmPortRemoteFaultGet,
         2, 0},
        {"cpssExMxPmPortMacStatusGetFirst",
         &wrCpssExMxPmPortMacStatusGetFirst,
         0, 0},
        {"cpssExMxPmPortMacStatusGetNext",
         &wrCpssExMxPmPortMacStatusGetNext,
         0, 0},
        {"cpssExMxPmPortInternalLoopbackEnableSet",
         &wrCpssExMxPmPortInternalLoopbackEnableSet,
         3, 0},
        {"cpssExMxPmPortInternalLoopbackEnableGet",
         &wrCpssExMxPmPortInternalLoopbackEnableGet,
         2, 0},
        {"cpssExMxPmPortInbandAutoNegEnableSet",
         &wrCpssExMxPmPortInbandAutoNegEnableSet,
         3, 0},
        {"cpssExMxPmPortInbandAutoNegEnableGet",
         &wrCpssExMxPmPortInbandAutoNegEnableGet,
         2, 0},
        {"cpssExMxPmPortAttributesGetFirst",
         &wrCpssExMxPmPortAttributesGetFirst,
         0, 0},
        {"cpssExMxPmPortAttributesGetNext",
         &wrCpssExMxPmPortAttributesGetNext,
         0, 0},
        {"cpssExMxPmPortPreambleLengthSet",
         &wrCpssExMxPmPortPreambleLengthSet,
         4, 0},
        {"cpssExMxPmPortPreambleLengthGet",
         &wrCpssExMxPmPortPreambleLengthGet,
         3, 0},
        {"cpssExMxPmPortMacSaBaseSet",
         &wrCpssExMxPmPortMacSaBaseSet,
         2, 0},
        {"cpssExMxPmPortMacSaBaseGet",
         &wrCpssExMxPmPortMacSaBaseGet,
         1, 0},
        {"cpssExMxPmPortMacSaLsbSet",
         &wrCpssExMxPmPortMacSaLsbSet,
         3, 0},
        {"cpssExMxPmPortMacSaLsbGet",
         &wrCpssExMxPmPortMacSaLsbGet,
         2, 0},
        {"cpssExMxPmPortTxPaddingEnableSet",
         &wrCpssExMxPmPortTxPaddingEnableSet,
         3, 0},
        {"cpssExMxPmPortTxPaddingEnableGet",
         &wrCpssExMxPmPortTxPaddingEnableGet,
         2, 0},
        {"cpssExMxPmPortExcessiveCollisionDropEnableSet",
         &wrCpssExMxPmPortExcessiveCollisionDropEnableSet,
         3, 0},
        {"cpssExMxPmPortExcessiveCollisionDropEnableGet",
         &wrCpssExMxPmPortExcessiveCollisionDropEnableGet,
         2, 0},

        {"cpssExMxPm2PortPriorityFlowControlEnableSet",
         &wrCpssExMxPmPortPriorityFlowControlEnableSet,
         3, 0},
        {"cpssExMxPm2PortPriorityFlowControlEnableGet",
         &wrCpssExMxPmPortPriorityFlowControlEnableGet,
         2, 0},

        {"cpssExMxPm2PortFlowControlPeriodicTypeSet",
         &wrCpssExMxPmPortFlowControlPeriodicTypeSet,
         3, 0},
        {"cpssExMxPm2PortFlowControlPeriodicTypeGet",
         &wrCpssExMxPmPortFlowControlPeriodicTypeGet,
         2, 0},

        {"cpssExMxPm2PortForwardFlowControlEnableSet",
         &wrCpssExMxPmPortForwardFlowControlEnableSet,
         3, 0},

        {"cpssExMxPm2PortForwardFlowControlEnableGet",
         &wrCpssExMxPmPortForwardFlowControlEnableGet,
         2, 0},
        {"cpssExMxPmPortSerdesLoopbackEnableSet",
         &wrCpssExMxPmPortSerdesLoopbackEnableSet,
         3, 0},
        {"cpssExMxPmPortSerdesLoopbackEnableGet",
         &wrCpssExMxPmPortSerdesLoopbackEnableGet,
         2, 0},
        {"cpssExMxPmPortXgPcsLanesSwapSet",
         &wrCpssExMxPmPortXgPcsLanesSwapSet,
         6, 0},
        {"cpssExMxPmPortXgPcsLanesSwapGet",
         &wrCpssExMxPmPortXgPcsLanesSwapGet,
         2, 0},
        {"cpssExMxPmPortInbandAutoNegRestartSet",
         &wrCpssExMxPmPortInbandAutoNegRestartSet,
         2, 0},
        {"cpssExMxPmPortInbandAutoNegBypassEnableSet",
         &wrCpssExMxPmPortInbandAutoNegBypassEnableSet,
         3, 0},
        {"cpssExMxPmPortInbandAutoNegBypassEnableGet",
         &wrCpssExMxPmPortInbandAutoNegBypassEnableGet,
         2, 0},
        {"cpssExMxPmPortSerdesPowerStatusSet",
         &wrCpssExMxPmPortSerdesPowerStatusSet,
         3, 0},



};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmPortCtrl
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
GT_STATUS cmdLibInitCpssExMxPmPortCtrl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

