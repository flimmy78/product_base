/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmPortBufMg.c
*
* DESCRIPTION:
*       Wrapper functions for
*           cpss/exMxPm/exMxPmGen/Port/cpssExMxPmPortBufMg.h API's
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
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortBufMg.h>

/*******************************************************************************
* cpssExMxPmPortFlowControlXonLimitSet
*
* DESCRIPTION:
*       Sets the Global Xon limit value. Enables global control over the
*       number of buffers allocated for all ports by Global Xon limit setting.
*       When the total number of buffers allocated reaches this threshold
*       (xonLimit), all ports that support 802.3x Flow Control send an Xon
*       frame.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum     -  device number.
*       xonLimit   - Xon limit. Range:(0 - 0x7ff)
*                     Resolution of this field is two buffers in Ingress BM
*                     and four in Egress BM.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM     - on invalid input paramteres value.
*       GT_HW_ERROR     - on hardware error.
*       GT_FAIL      - on error
*
* COMMENTS:
*       1. The Global Xon limit must be not be greater than Global Xoff Limit.
*       2. To disable Global Xon, the 2*xonLimit should be set greater-or-equal
*          of 8*<Max Buffer Limit> (maximal number of buffers allocated for
*          all ports).
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlXonLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 xonLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    xonLimit = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPortFlowControlXonLimitSet(devNum, xonLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlXonLimitGet
*
* DESCRIPTION:
*       Gets the Global Xon limit value.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       xonLimitPtr     - pointer to the Xon limit value (resolution of this
*                         value is two buffers of 256 Byte each).
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on invalid input paramteres value.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error.
*       GT_FAIL         - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlXonLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 xonLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPortFlowControlXonLimitGet(devNum, &xonLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", xonLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlXoffLimitSet
*
* DESCRIPTION:
*       Sets the Global Xoff limit value. Enables global control over the
*       number of buffers allocated for all ports by Global Xoff limit setting.
*       When the total number of buffers allocated reaches this threshold
*       (xoffLimit), all ports that support 802.3x Flow Control send a pause
*       frame.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum      - device number.
*       xoffLimit   - Xoff limit. Range:(0 - 0x7ff)
*                     Resolution of this field is two buffers in Ingress BM
*                     and four in Egress BM.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on invalid input paramteres value
*       GT_HW_ERROR     - on hardware error.
*       GT_FAIL         - on error
*
* COMMENTS:
*       To disable Global Xoff, the 2*xoffLimit should be set greater-or-equal
*       of 8*<Max Buffer Limit> (maximal number of buffers allocated for
*          all ports).
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlXoffLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 xoffLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    xoffLimit = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPortFlowControlXoffLimitSet(devNum, xoffLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlXoffLimitGet
*
* DESCRIPTION:
*       Gets the Global Xoff limit value.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       xonLimitPtr     - pointer to the Xoff limit value (resolution of this
*                         value is two buffers of 256 Byte each).
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - on invalid input paramteres value
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error.
*       GT_FAIL       - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlXoffLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 xoffLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPortFlowControlXoffLimitGet(devNum, &xoffLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", xoffLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlXonLimitPortSet
*
* DESCRIPTION:
*       Sets X-ON port limit. When Flow Control is enabled, X-ON packet sent
*       when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*       xonLimit   - X-ON limit.Range:(0 - 0x7ff)
*                    Resolution of this field is two buffers in Ingress BM
*                    and four in Egress BM.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlXonLimitPortSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet;
    GT_U32 xonLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_RX_FC_PROFILE_SET_ENT)inArgs[1];
    xonLimit = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPortFlowControlXonLimitPortSet(devNum, profileSet, xonLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlXonLimitPortGet
*
* DESCRIPTION:
*       Gets the X-ON port limit. When Flow Control is enabled, X-ON packet
*       sent when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*       profileSet - the Profile Set in which the Flow Control Parameters of
*                    are associated.
*
* OUTPUTS:
*       xonLimitPtr  - (pointer to) the X-ON limit value
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlXonLimitPortGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet;
    GT_U32 xonLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_RX_FC_PROFILE_SET_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPortFlowControlXonLimitPortGet(devNum, profileSet, &xonLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", xonLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlXoffLimitPortSet
*
* DESCRIPTION:
*       Sets X-OFF port limit. When Flow Control is enabled, X-OFF packet sent
*       when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*       xoffLimit  - X-OFF limit.Range:(0 - 0x7ff)
*                     Resolution of this field is two buffers in Ingress BM
*                     and four in Egress BM.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlXoffLimitPortSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet;
    GT_U32 xoffLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_RX_FC_PROFILE_SET_ENT)inArgs[1];
    xoffLimit = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPortFlowControlXoffLimitPortSet(devNum, profileSet, xoffLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlXoffLimitPortGet
*
* DESCRIPTION:
*       Gets the X-OFF port limit. When Flow Control is enabled, X-OFF packet
*       sent when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*
* OUTPUTS:
*       xoffLimitPtr - (pointer to) the X-OFF limit value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlXoffLimitPortGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet;
    GT_U32 xoffLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_RX_FC_PROFILE_SET_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPortFlowControlXoffLimitPortGet(devNum, profileSet, &xoffLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", xoffLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlProfileSet
*
* DESCRIPTION:
*       Bind a port to a flow control profile.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlProfileSet
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
    CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    profileSet = (CPSS_PORT_RX_FC_PROFILE_SET_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortFlowControlProfileSet(devNum, portNum, profileSet);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortRxFlowControlProfileGet
*
* DESCRIPTION:
*       Get the port's flow control profile.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number
*
* OUTPUTS:
*       profileSetPtr - (pointer to) the Profile Set in which the Flow Control
*                       Parameters are associated.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortRxFlowControlProfileGet
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
    CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortRxFlowControlProfileGet(devNum, portNum, &profileSet);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileSet);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortRxBufLimitSet
*
* DESCRIPTION:
*       Sets receive buffer limit threshold for the specified port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*       rxBufLimit - Maximal number of buffers that may be allocated for ports
*                    associated with this Ports Buffers Limit Profile.
*                    Range:(0-0x3ff)
*                    Resolution of this field is four buffers in Ingress BM
*                    and eight buffers in Egress BM.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortRxBufLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet;
    GT_U32 rxBufLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_RX_FC_PROFILE_SET_ENT)inArgs[1];
    rxBufLimit = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPortRxBufLimitSet(devNum, profileSet, rxBufLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortRxBufLimitGet
*
* DESCRIPTION:
*       Gets the receive buffer limit threshold for the specified port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*
* OUTPUTS:
*       rxBufLimitPtr - (pointer to) the rx buffer limit value:
*                       Maximal number of buffers that may be allocated for
*                       ports associated with this Ports Buffers Limit Profile.
*                       Resolution of this field is two buffers in Ingress BM
*                       and eight buffers in Egress BM.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortRxBufLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet;
    GT_U32 rxBufLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_RX_FC_PROFILE_SET_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPortRxBufLimitGet(devNum, profileSet, &rxBufLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", rxBufLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortFlowControlHolSysModeSet
*
* DESCRIPTION:
*       Set Flow Control or HOL system mode on the specified device.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum       - device number
*       modeFcHol - CPSS_EXMXPM_PORT_FC_E  : set Flow Control mode
*                   CPSS_EXMXPM_PORT_HOL_E : set HOL system mode
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_NOT_SUPPORTED - on not supported HOF FC mode
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       To configure Flow Control or HOL system mode this API makes following
*       actions:
*         - enable/disable Tail Drop for all Unicast packets
*         - enable / disable Tail Drop according to the number of buffers in
*           the queues.
*         - set buffer's mode to symmetric FC or tail drop.
*       For all these actions there are separate APIs as listed below :
*          cpssExMxPmPortTxTailDropUcEnableSet,
*          cpssExMxPmPortTxBufferTailDropEnableSet.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortFlowControlHolSysModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_PORT_HOL_FC_ENT modeFcHol;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    modeFcHol = (CPSS_EXMXPM_PORT_HOL_FC_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPortFlowControlHolSysModeSet(devNum, modeFcHol);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmPortFlowControlXonLimitSet",
         &wrCpssExMxPmPortFlowControlXonLimitSet,
         2, 0},
        {"cpssExMxPmPortFlowControlXonLimitGet",
         &wrCpssExMxPmPortFlowControlXonLimitGet,
         1, 0},
        {"cpssExMxPmPortFlowControlXoffLimitSet",
         &wrCpssExMxPmPortFlowControlXoffLimitSet,
         2, 0},
        {"cpssExMxPmPortFlowControlXoffLimitGet",
         &wrCpssExMxPmPortFlowControlXoffLimitGet,
         1, 0},
        {"cpssExMxPmPortFlowControlXonLimitPortSet",
         &wrCpssExMxPmPortFlowControlXonLimitPortSet,
         3, 0},
        {"cpssExMxPmPortFlowControlXonLimitPortGet",
         &wrCpssExMxPmPortFlowControlXonLimitPortGet,
         2, 0},
        {"cpssExMxPmPortFlowControlXoffLimitPortSet",
         &wrCpssExMxPmPortFlowControlXoffLimitPortSet,
         3, 0},
        {"cpssExMxPmPortFlowControlXoffLimitPortGet",
         &wrCpssExMxPmPortFlowControlXoffLimitPortGet,
         2, 0},
        {"cpssExMxPmPortFlowControlProfileSet",
         &wrCpssExMxPmPortFlowControlProfileSet,
         3, 0},
        {"cpssExMxPmPortRxFlowControlProfileGet",
         &wrCpssExMxPmPortRxFlowControlProfileGet,
         2, 0},
        {"cpssExMxPmPortRxBufLimitSet",
         &wrCpssExMxPmPortRxBufLimitSet,
         3, 0},
        {"cpssExMxPmPortRxBufLimitGet",
         &wrCpssExMxPmPortRxBufLimitGet,
         2, 0},
        {"cpssExMxPmPortFlowControlHolSysModeSet",
         &wrCpssExMxPmPortFlowControlHolSysModeSet,
         2, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmPortBufMg
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
GT_STATUS cmdLibInitCpssExMxPmPortBufMg
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

