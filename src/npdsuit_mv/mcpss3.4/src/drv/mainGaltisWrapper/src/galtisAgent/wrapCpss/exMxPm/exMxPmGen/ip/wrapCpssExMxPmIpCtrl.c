/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmIpCtrl.c
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
/* get private types */
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpCtrl.h>

/*******************************************************************************
* cpssExMxPmIpRouterEngineEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Globally enables/disables Router Engine.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       enableRouting     - If GT_TRUE  then Router Engine is enabled
*                           If GT_FALSE then Router Engine is disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_BAD_PARAM      - on illegal parameter
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpRouterEngineEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enableRouting;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enableRouting = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpRouterEngineEnableSet(devNum, enableRouting);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpRouterEngineEnableGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Returns whether Router Engine is enabled/disabled.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*
* OUTPUTS:
*       enabledRoutingPtr - If GT_TRUE  then Router Engine is enabled
*                           If GT_FALSE then Router Engine is disabled
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_BAD_PARAM      - on illegal parameter
*       GT_BAD_PTR        - on NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpRouterEngineEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enabledRouting;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmIpRouterEngineEnableGet(devNum, &enabledRouting);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enabledRouting);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpSpecialRouterTriggerEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       This function enables/disables the special router trigger for packets
*       with bridge command other then FORWARD or MIRROR.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       routerTriggerType - special router trigger type
*       enable            - enable / disable router trigger
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_FAIL           - on error
*       GT_BAD_PARAM      - Illegal parameter in function called
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpSpecialRouterTriggerEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_TYPE_ENT routerTriggerType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    routerTriggerType = (CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_TYPE_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmIpSpecialRouterTriggerEnableSet(devNum, routerTriggerType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpSpecialRouterTriggerEnableGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       This function retrieves state of the special router trigger for packets
*       with bridge command other then FORWARD or MIRROR.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       routerTriggerType - special router trigger type
*       enablePtr         - points to enable / disable router trigger
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_FAIL           - on error
*       GT_BAD_PARAM      - Illegal parameter in function called
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpSpecialRouterTriggerEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_TYPE_ENT routerTriggerType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    routerTriggerType = (CPSS_EXMXPM_IP_SPECIAL_ROUTER_TRIGGER_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpSpecialRouterTriggerEnableGet(devNum, routerTriggerType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpExceptionCmdSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       This routine sets the command for a specific exception.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       exceptionType     - Exception type
*       protocolStack     - IP protocol to set for. Valid values:
*                               CPSS_IP_PROTOCOL_IPV4_E
*                               CPSS_IP_PROTOCOL_IPV6_E
*       packetType        - Packet type. Valid values:
*                               CPSS_IP_UNICAST_E
*                               CPSS_IP_MULTICAST_E
*       exceptionCmd      - Exception command
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_BAD_PARAM      - on illegal parameter
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       For explanation on specific exception and valid commands refer to
*       CPSS_EXMXPM_IP_EXCEPTION_TYPE_ENT.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpExceptionCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_IP_EXCEPTION_TYPE_ENT exceptionType;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    CPSS_IP_UNICAST_MULTICAST_ENT packetType;
    CPSS_PACKET_CMD_ENT exceptionCmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_EXMXPM_IP_EXCEPTION_TYPE_ENT)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    packetType = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[3];
    exceptionCmd = (CPSS_PACKET_CMD_ENT)inArgs[4];

     /* call cpss api function */
    result = cpssExMxPmIpExceptionCmdSet(devNum, exceptionType, protocolStack, packetType, exceptionCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpExceptionCmdGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       This routine gets the command for a specific exception.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       exceptionType     - Exception type
*       protocolStack     - IP protocol to set for
*       packetType        - Packet type. Valid values:
*                               CPSS_IP_UNICAST_E
*                               CPSS_IP_MULTICAST_E
*
* OUTPUTS:
*       exceptionCmdPtr   - points to exception command
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_BAD_PARAM      - on illegal parameter
*       GT_BAD_PTR        - on NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       For explanation on specific exception and valid commands refer to
*       CPSS_EXMXPM_IP_EXCEPTION_TYPE_ENT.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpExceptionCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_IP_EXCEPTION_TYPE_ENT exceptionType;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    CPSS_IP_UNICAST_MULTICAST_ENT packetType;
    CPSS_EXMXPM_IP_EXCEPTION_COMMAND_ENT ipExceptionCmd;
    CPSS_PACKET_CMD_ENT exceptionCmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_EXMXPM_IP_EXCEPTION_TYPE_ENT)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    packetType = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmIpExceptionCmdGet(devNum, exceptionType, protocolStack, packetType, &ipExceptionCmd);

    switch (ipExceptionCmd)
    {
    case CPSS_EXMXPM_IP_EXCEPTION_COMMAND_ROUTE_E:
        exceptionCmd = CPSS_PACKET_CMD_ROUTE_E;
        break;
    case CPSS_EXMXPM_IP_EXCEPTION_COMMAND_ROUTE_AND_MIRROR_E:
        exceptionCmd = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
        break;
    case CPSS_EXMXPM_IP_EXCEPTION_COMMAND_BRIDGE_AND_MIRROR_E:
        exceptionCmd = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
        break;
    case CPSS_EXMXPM_IP_EXCEPTION_COMMAND_BRIDGE_E:
        exceptionCmd = CPSS_PACKET_CMD_BRIDGE_E;
        break;
    case CPSS_EXMXPM_IP_EXCEPTION_COMMAND_ROUTE_ALL_TRAP_DF_E:
    case CPSS_EXMXPM_IP_EXCEPTION_COMMAND_DONT_ROUTE_DF_E:
        exceptionCmd = CPSS_PACKET_CMD_NONE_E;
        break;
    default:
        exceptionCmd = ipExceptionCmd;
        break;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", exceptionCmd);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpRouteEntryAgeRefreshEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Enables/disables aging/refreshing for route entry <Age/Refresh>.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum       - device number
*       enable       - If GT_TRUE  : Aging/Refreshing is enabled.
*                    - If GT_FALSE : Aging/Refreshing is disabled.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpRouteEntryAgeRefreshEnableSet
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
    result = cpssExMxPmIpRouteEntryAgeRefreshEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpRouteEntryAgeRefreshEnableGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Gets enabled/disabled status of aging/refreshing for route
*       entry <Age/Refresh>.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to) Age/refresh enable value
*                      If GT_TRUE  : Aging/Refreshing is enabled.
*                      If GT_FALSE : Aging/Refreshing is disabled.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpRouteEntryAgeRefreshEnableGet
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
    result = cpssExMxPmIpRouteEntryAgeRefreshEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpEcmpUcRpfCheckEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Globally enables/disables ECMP/QoS unicast RPF check.
*
* APPLICABLE DEVICES: All EXMXPM devices.
*
* INPUTS:
*       devNum    - the device number
*       enable    - enable / disable ECMP/Qos unicast RPF check
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpEcmpUcRpfCheckEnableSet
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

    devNum  = (GT_U8)inArgs[0];
    enable  = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpEcmpUcRpfCheckEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpEcmpUcRpfCheckEnableGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Gets globally enabled/disabled ECMP/QoS unicast RPF check state.
*
* APPLICABLE DEVICES: All EXMXPM devices.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       enablePtr - (points to) enable / disable ECMP/Qos unicast RPF check
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpEcmpUcRpfCheckEnableGet
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
    result = cpssExMxPmIpEcmpUcRpfCheckEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableSet
*
* DESCRIPTION:
*       Function Relevant mode: High/Low Level API modes
*       Enable fetching only the first part of the Next Hop Entry for SIP lookup.
*       When this mode is enabled only three interfaces are checked, which
*       improves bandwidth utilization.
*
* APPLICABLE DEVICES: All EXMXPM devices.
*
* INPUTS:
*       devNum    - the device number
*       enable    - GT_FALSE: Get both parts of the SIP entry.
*                   GT_TRUE: Get only the first part of the SIP entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableSet
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

    devNum  = (GT_U8)inArgs[0];
    enable  = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet
*
* DESCRIPTION:
*       Function Relevant mode: High/Low Level API modes
*       Get Fetching mode of the Next Hop Entry for SIP lookup.
*       When this mode is enabled only three interfaces are checked, which
*       improves bandwidth utilization.
*
* APPLICABLE DEVICES: All EXMXPM devices.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       enablePtr - (points to)
*                   GT_FALSE: Get both parts of the SIP entry.
*                   GT_TRUE: Get only the first part of the SIP entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet
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
    result = cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPmIpMtuProfileSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Sets the global MTU profile limit values.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       mtuProfileIndex   - the mtu profile index (0..15)
*       mtu               - the maximum transmission unit
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - one of the parameters with bad value
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpMtuProfileSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mtuProfileIndex;
    GT_U32 mtu;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mtuProfileIndex = (GT_U32)inArgs[1];
    mtu = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmIpMtuProfileSet(devNum, mtuProfileIndex, mtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpMtuProfileGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Retrieves the global MTU profile limit values.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       mtuProfileIndex   - the mtu profile index (0..15)
*
* OUTPUTS:
*       mtuPtr            - points to the maximum transmission unit
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - one of the parameters with bad value
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpMtuProfileGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mtuProfileIndex;
    GT_U32 mtu;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mtuProfileIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpMtuProfileGet(devNum, mtuProfileIndex, &mtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mtu);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpv6AddrPrefixScopeSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       The Ipv6 address prefix scope table maps ipv6 address (up to 16 bits
*       prefix) to scope level.
*       This functions configures an entry in the prefix look up table.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       prefixScopeIndex  - index of the new prefix scope entry (0..3)
*       prefixPtr         - points to IPv6 address prefix
*       prefixLen         - length of the prefix (0..15)
*       addressScope      - type of the address scope spanned by the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE   - on parameter out of range
*       GT_BAD_PARAM      - one of the parameters with bad value
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpv6AddrPrefixScopeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 prefixScopeIndex;
    GT_IPV6ADDR prefix;
    GT_U32 prefixLen;
    CPSS_IPV6_PREFIX_SCOPE_ENT addressScope;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    prefixScopeIndex = (GT_U32)inFields[0];
    galtisIpv6Addr(&prefix, (GT_U8*)inFields[1]);
    prefixLen = inFields[2];
    addressScope = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[3];

    /* call cpss api function */
    result = cpssExMxPmIpv6AddrPrefixScopeSet(devNum, prefixScopeIndex, &prefix, prefixLen, addressScope);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpv6AddrPrefixScopeGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       The Ipv6 address prefix scope table maps ipv6 address (up to 16 bits
*       prefix) to scope level.
*       This functions retrieves configuration of an entry in the prefix look
*       up table.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       prefixScopeIndex  - index of the new prefix scope entry (0..3)
*
* OUTPUTS:
*       prefixPtr         - points to IPv6 address prefix
*       prefixLenPtr      - points to length of the prefix (0..15)
*       addressScopePtr   - points to type of the address scope spanned by the prefix
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - one of the parameters with bad value
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static  GT_U32 index;

static CMD_STATUS ipv6AddrPrefixScopeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_IPV6ADDR prefix;
    GT_U32 prefixLen;
    CPSS_IPV6_PREFIX_SCOPE_ENT addressScope;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmIpv6AddrPrefixScopeGet(devNum, index, &prefix, &prefixLen, &addressScope);


     if(result != CMD_OK)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = index;
    inFields[2] = prefixLen;
    inFields[3] = addressScope;

    /* pack output arguments to galtis string */
    fieldOutput("%d%16B%d%d", inFields[0], prefix.arIP,
                                inFields[2],  inFields[3]);

    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpv6AddrPrefixScopeGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CMD_STATUS res;

    index = 0;
    res = ipv6AddrPrefixScopeGet(inArgs,inFields,numFields,outArgs);
    if (res == CMD_OK)
        index++;
    return res;
}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpv6AddrPrefixScopeGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CMD_STATUS res;

    if(index > 3)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    res = ipv6AddrPrefixScopeGet(inArgs,inFields,numFields,outArgs);
    if (res == CMD_OK)
        index++;
    return res;
}
/*******************************************************************************
* cpssExMxPmIpv6UcScopeCommandSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Sets the ipv6 Unicast scope commands.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       addressScopeSrc   - IPv6 address scope of source address
*       addressScopeDest  - IPv6 address scope of destination address
*       isBorderCrossed   - GT_TRUE if source site ID is diffrent to destination
*                           site ID
*       scopeCommand      - action to be done on a packet that match the above
*                           scope configuration. Valid commands:
*                             - CPSS_PACKET_CMD_ROUTE_E,
*                             - CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
*                             - CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                             - CPSS_PACKET_CMD_DROP_SOFT_E,
*                             - CPSS_PACKET_CMD_DROP_HARD_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - one of the parameters with bad value
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpv6UcScopeCommandSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_IPV6_PREFIX_SCOPE_ENT addressScopeSrc;
    CPSS_IPV6_PREFIX_SCOPE_ENT addressScopeDest;
    GT_BOOL isBorderCrossed;
    CPSS_PACKET_CMD_ENT scopeCommand;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    addressScopeSrc = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[1];
    addressScopeDest = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[2];
    isBorderCrossed = (GT_BOOL)inArgs[3];
    scopeCommand = (CPSS_PACKET_CMD_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmIpv6UcScopeCommandSet(devNum, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpv6UcScopeCommandGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Sets the ipv6 Unicast scope commands.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       addressScopeSrc   - IPv6 address scope of source address
*       addressScopeDest  - IPv6 address scope of destination address
*       isBorderCrossed   - GT_TRUE if source site ID is diffrent to destination
*                           site ID
*       scopeCommandPtr   - points to action to be done on a packet that match
*                           the above scope configuration. Valid commands:
*                             - CPSS_PACKET_CMD_ROUTE_E,
*                             - CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
*                             - CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                             - CPSS_PACKET_CMD_DROP_SOFT_E,
*                             - CPSS_PACKET_CMD_DROP_HARD_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - one of the parameters with bad value
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpv6UcScopeCommandGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_IPV6_PREFIX_SCOPE_ENT addressScopeSrc;
    CPSS_IPV6_PREFIX_SCOPE_ENT addressScopeDest;
    GT_BOOL isBorderCrossed;
    CPSS_PACKET_CMD_ENT scopeCommand;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    addressScopeSrc = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[1];
    addressScopeDest = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[2];
    isBorderCrossed = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmIpv6UcScopeCommandGet(devNum, addressScopeSrc, addressScopeDest, isBorderCrossed, &scopeCommand);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", scopeCommand);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpQosModeSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       This function configures the QoS Routing Mode.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       mode              - QoS Mode (Bitwise or Modulus)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - one of the parameters with bad value
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpQosModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_QOS_ROUTING_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_QOS_ROUTING_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpQosModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpQosModeGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       This function retrieves the QoS Routing Mode.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       modePtr           - points to QoS Mode (Bitwise or Modulus)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - one of the parameters with bad value
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpQosModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_QOS_ROUTING_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmIpQosModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpRouterMacSaBaseSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Sets 40 MSBs of Router MAC SA Base address on specified device.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       dev           - device number
*       macPtr        - (pointer to)The system Mac address to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - one of the parameters with bad value
*       GT_FAIL       - on error
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpRouterMacSaBaseSet
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
    galtisMacAddr(&mac,(GT_U8*)inArgs[1]);

    /* call cpss api function */
    result = cpssExMxPmIpRouterMacSaBaseSet(devNum, &mac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpRouterMacSaBaseGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Get 40 MSBs of Router MAC SA Base address on specified device.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       dev           - device number
*
* OUTPUTS:
*       macPtr        - (pointer to)The system Mac address to set.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - one of the parameters with bad value
*       GT_FAIL       - on error
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpRouterMacSaBaseGet
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
    result = cpssExMxPmIpRouterMacSaBaseGet(devNum, &mac);

    /* pack output arguments to galtis string */


    galtisOutput(outArgs, result, "%6b",mac.arEther);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpRouterMacSaLsbModeSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Sets the mode in which the device sets the packet's MAC SA least
*       significant bytes.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       saLsbMode         - The MAC SA least-significant bit mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_SUPPORTED  - if the feature does not supported by specified device
*       GT_BAD_PARAM      - one of the parameters with bad value
*       GT_HW_ERROR       - on hardware error
*       GT_FAIL           - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpRouterMacSaLsbModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_U8 devNum;
    CPSS_MAC_SA_LSB_MODE_ENT saLsbMode;
    GT_U8 portNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    saLsbMode = (CPSS_MAC_SA_LSB_MODE_ENT)inArgs[1];

    /* call cpss api function */
    for (portNum = 0 ; portNum < PRV_CPSS_PP_MAC(devNum)->numOfVirtPorts ; portNum++)
    {
        result = cpssExMxPmIpPortRouterMacSaLsbModeSet(devNum, portNum, saLsbMode);
        if (result != GT_OK)
            break;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpRouterMacSaLsbModeGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Gets the mode in which the device sets the packet's MAC SA least
*       significant bytes.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       saLsbModePtr    - (pointer to) The MAC SA least-significant bit mode
*
* RETURNS:
*      GT_OK            - on success
*      GT_NOT_SUPPORTED - if the feature does not supported by specified device
*      GT_BAD_PTR       - one of the parameters is NULL pointer
*      GT_BAD_PARAM     - one of the parameters with bad value
*      GT_HW_ERROR      - on hardware error
*      GT_FAIL          - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpRouterMacSaLsbModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_MAC_SA_LSB_MODE_ENT saLsbMode;
    GT_U8 portNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    /* use port 0 as representative of all ports */
    portNum = 0;
    result = cpssExMxPmIpPortRouterMacSaLsbModeGet(devNum, portNum, &saLsbMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", saLsbMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpRouterPortVlanMacSaLsbSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Sets the 8 LSB Router MAC SA for this VLAN / EGGRESS PORT.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*     devNum              - device number.
*     vlanPortId          - VLAN Id or Eggress Port Id
*     saMac               - The 8 bits SA mac value to be written to the SA bits
*                           of routed packet if SA alteration mode is configured
*                           to take LSB according to VLAN id / Eggress Port Id.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK               - on success
*     GT_NOT_SUPPORTED    - if the feature does not supported by specified device
*     GT_BAD_PARAM        - one of the parameters with bad value
*     GT_HW_ERROR         - on hardware error
*     GT_FAIL             - on error.
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpRouterPortVlanMacSaLsbSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16 vlanPortId;
    GT_U32 saMac;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vlanPortId = (GT_U16)inArgs[1];
    saMac = (GT_U32)inArgs[2];

    /* call cpss api function */
    if (vlanPortId < 4096)
    {
        result = cpssExMxPmIpRouterVlanMacSaLsbSet(devNum, vlanPortId, saMac);
    }
    else
    {
        result = cpssExMxPmIpRouterPortMacSaLsbSet(devNum, (GT_U8)(vlanPortId-4096), saMac);
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpRouterPortVlanMacSaLsbGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Gets the 8 LSB Router MAC SA for this VLAN / EGGRESS PORT.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*     devNum            - device number.
*     vlanPortId        - VLAN Id or Eggress Port Id
*
* OUTPUTS:
*     saMacPtr          - (pointer to) The 8 bits SA mac value written to the SA
*                         bits of routed packet if SA alteration mode is configured
*                         to take LSB according to VLAN id / Eggress Port Id.
*
* RETURNS:
*     GT_OK             - on success
*     GT_NOT_SUPPORTED  - if the feature does not supported by specified device
*     GT_BAD_PTR        - one of the parameters is NULL pointer
*     GT_BAD_PARAM      - one of the parameters with bad value
*     GT_HW_ERROR       - on hardware error
*     GT_FAIL           - on error.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpRouterPortVlanMacSaLsbGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16 vlanPortId;
    GT_U32 saMac;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vlanPortId = (GT_U16)inArgs[1];

    /* call cpss api function */
    if (vlanPortId < 4096)
    {
        result = cpssExMxPmIpRouterVlanMacSaLsbGet(devNum, vlanPortId, &saMac);
    }
    else
    {
        result = cpssExMxPmIpRouterPortMacSaLsbGet(devNum, (GT_U8)(vlanPortId-4096), &saMac);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", saMac);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Configures uRPF check/ICMP Redirect Interface Mode per ingress port.
*       Determines how to check/whether to mirror the incoming packet.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number to set
*       protocolStack   - types of IP stack used.
*                         Valid values:
*                               CPSS_IP_PROTOCOL_IPV4_E
*                               CPSS_IP_PROTOCOL_IPV6_E
*       mode            - mode on which to base the check
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - if the feature does not supported by specified device
*       GT_BAD_PARAM     - one of the parameters with bad value
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpIcmpRedirectIfModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8    portNum;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    mode = (CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_MODE_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeSet(devNum, portNum, protocolStack, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Gets configuration of uRPF check/ICMP Redirect Interface Mode per ingress
*       port. Determines how to check/whether to mirror the incoming packet.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       protocolStack   - types of IP stack used.
*                         Valid values:
*                               CPSS_IP_PROTOCOL_IPV4_E
*                               CPSS_IP_PROTOCOL_IPV6_E
*
* OUTPUTS:
*       modePTR         - (pointer to) mode value on which the check is based
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - if the feature does not supported by specified device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_BAD_PARAM     - one of the parameters with bad value
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpIcmpRedirectIfModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8     portNum;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    CPSS_EXMXPM_RPF_ICMP_REDIRECT_IF_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmIpPortUcRpfIcmpRedirectIfModeGet(devNum, portNum, protocolStack, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpDataErrorHandlingModeSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Sets the mode of handling IP router data errors (ECC for internal
*       memory or parity error for external memory) when accessing LPM table
*       memory or the Route entry table memory.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       mode              - data error handling mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - one of the parameters with bad value
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDataErrorHandlingModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /**********************************************************/
    /* this API was removed as this configuration is internal */
    /**********************************************************/

    GT_STATUS result;
    GT_U8 devNum;
/*    CPSS_EXMXPM_DATA_ERROR_HANDLING_MODE_ENT mode;*/


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
/*    mode = (CPSS_EXMXPM_DATA_ERROR_HANDLING_MODE_ENT)inArgs[1];*/

    /* call cpss api function */
    result = GT_NOT_SUPPORTED; /*cpssExMxPmIpDataErrorHandlingModeSet(devNum, mode);*/

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpDataErrorHandlingModeGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Gets the mode of handling IP router data errors (ECC for internal
*       memory or parity error for external memory) when accessing LPM table
*       memory or the Route entry table memory.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       modePtr           - points to data error handling mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - one of the parameters with bad value
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDataErrorHandlingModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /**********************************************************/
    /* this API was removed as this configuration is internal */
    /**********************************************************/

    GT_STATUS result;
    GT_U8 devNum;
/*    CPSS_EXMXPM_DATA_ERROR_HANDLING_MODE_ENT mode;*/


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = GT_NOT_SUPPORTED; /*cpssExMxPmIpDataErrorHandlingModeGet(devNum, &mode);*/

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpArpBcCommandSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Sets the command for the Router engine handling of ARP Broadcast
*       packets.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       arpBcCmd      - The ARP broadcast command. Possible Commands:
*                           CPSS_PACKET_CMD_NONE_E
*                           CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                           CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL       - on error
*       GT_BAD_PARAM  - on illegal parameter
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpArpBcCommandSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PACKET_CMD_ENT arpBcCmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    arpBcCmd = (CPSS_PACKET_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpArpBcCommandSet(devNum, arpBcCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpArpBcCommandGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Gets the command of the Router engine for handling the ARP Broadcast
*       packets.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       arpBcCmdPtr   - point to the ARP broadcast command. Possible Commands:
*                           CPSS_PACKET_CMD_NONE_E
*                           CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                           CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*                           CPSS_PACKET_CMD_INVALID_E
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL       - on error
*       GT_BAD_PARAM  - on illegal parameter
*       GT_BAD_PTR    - on NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpArpBcCommandGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PACKET_CMD_ENT arpBcCmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmIpArpBcCommandGet(devNum, &arpBcCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", arpBcCmd);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmMemoryOffsetSet
*
* DESCRIPTION:
*       Function Relevant mode : Low Level API modes
*       The LPM table may reside in internal memory or external memory.
*       For each lookup type, there is a configurable LPM base address.
*       This function sets the LPM base address.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       lpmType           - the LPM type; refer to CPSS_EXMXPM_IP_LPM_TYPE_ENT
*                           for valid values
*       lpmOffset         - the LPM base offset
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - one of the parameters with bad value
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmMemoryOffsetSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_IP_LPM_TYPE_ENT lpmType;
    GT_U32 lpmOffset;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    lpmType = (CPSS_EXMXPM_IP_LPM_TYPE_ENT)inArgs[1];
    lpmOffset = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmIpLpmMemoryOffsetSet(devNum, lpmType, lpmOffset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmMemoryOffsetGet
*
* DESCRIPTION:
*       Function Relevant mode : Low Level API modes
*       The LPM table may reside in internal memory or external memory.
*       For each lookup type, there is a configurable LPM base address.
*       This function gets the LPM base address.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       lpmType           - the LPM type; refer to CPSS_EXMXPM_IP_LPM_TYPE_ENT
*                           for valid values
*       lpmOffsetPtr      - points to the LPM base offset
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - one of the parameters with bad value
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmMemoryOffsetGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_IP_LPM_TYPE_ENT lpmType;
    GT_U32 lpmOffset;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    lpmType = (CPSS_EXMXPM_IP_LPM_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpLpmMemoryOffsetGet(devNum, lpmType, &lpmOffset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", lpmOffset);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpv6McScopeCommandSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Sets the ipv6 Multicast scope commands.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       addressScopeSrc   - IPv6 address scope of source address
*       addressScopeDest  - IPv6 address scope of destination address
*       isBorderCrossed   - GT_TRUE if source site ID is diffrent to destination
*                           site ID
*       scopeCommand      - action to be done on a packet that match the above
*                           scope configuration. Valid commands:
*                             - CPSS_PACKET_CMD_ROUTE_E,
*                             - CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
*                             - CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                             - CPSS_PACKET_CMD_DROP_SOFT_E,
*                             - CPSS_PACKET_CMD_DROP_HARD_E,
*                             - CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,
*                             - CPSS_PACKET_CMD_BRIDGE_E
*       ditSelector       - type of dit selector for this scope
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpv6McScopeCommandSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_IPV6_PREFIX_SCOPE_ENT          addressScopeSrc;
    CPSS_IPV6_PREFIX_SCOPE_ENT          addressScopeDest;
    GT_BOOL                             isBorderCrossed;
    CPSS_PACKET_CMD_ENT                 scopeCommand;
    CPSS_EXMXPM_IP_DIT_POINTER_TYPE_ENT ditSelector;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    addressScopeSrc = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[1];
    addressScopeDest = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[2];
    isBorderCrossed = (GT_BOOL)inArgs[3];
    scopeCommand = (CPSS_PACKET_CMD_ENT)inArgs[4];
    ditSelector = (CPSS_EXMXPM_IP_DIT_POINTER_TYPE_ENT)inArgs[5];


    /* call cpss api function */
    result = cpssExMxPmIpv6McScopeCommandSet(devNum, addressScopeSrc, addressScopeDest, isBorderCrossed, scopeCommand, ditSelector);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpv6McScopeCommandGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Gets the ipv6 Multicast scope commands.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       addressScopeSrc   - IPv6 address scope of source address
*       addressScopeDest  - IPv6 address scope of destination address
*       isBorderCrossed   - GT_TRUE if source site ID is diffrent to destination
*                           site ID
*
* OUTPUTS:
*       scopeCommandPtr   - points to action to be done on a packet that match
*                           the above scope configuration. Valid commands:
*                             - CPSS_PACKET_CMD_ROUTE_E
*                             - CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E
*                             - CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                             - CPSS_PACKET_CMD_DROP_SOFT_E
*                             - CPSS_PACKET_CMD_DROP_HARD_E
*                             - CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,
*                             - CPSS_PACKET_CMD_BRIDGE_E
*       ditSelectorPtr    - points to type of dit selector for this scope
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpv6McScopeCommandGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_IPV6_PREFIX_SCOPE_ENT          addressScopeSrc;
    CPSS_IPV6_PREFIX_SCOPE_ENT          addressScopeDest;
    GT_BOOL                             isBorderCrossed;
    CPSS_PACKET_CMD_ENT                 scopeCommand;
    CPSS_EXMXPM_IP_DIT_POINTER_TYPE_ENT ditSelector;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    addressScopeSrc = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[1];
    addressScopeDest = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[2];
    isBorderCrossed = (GT_BOOL)inArgs[3];


    /* call cpss api function */
    result = cpssExMxPmIpv6McScopeCommandGet(devNum,
                                           addressScopeSrc,
                                           addressScopeDest,
                                           isBorderCrossed,
                                           &scopeCommand,
                                           &ditSelector);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", scopeCommand, ditSelector);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpv6SourcePortSiteIdSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       The Site ID reflects whether the packetes source or destination is
*       associated with a network Internal or External to the administrative site.
*       This function configures the source site id of a port.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number
*       ipv6SourceSiteId  - type of the ipv6 source id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpv6SourcePortSiteIdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    GT_U8               portNum;
    CPSS_IP_SITE_ID_ENT ipv6SourceSiteId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ipv6SourceSiteId = (CPSS_IP_SITE_ID_ENT)inArgs[2];


    /* call cpss api function */
    result = cpssExMxPmIpv6SourcePortSiteIdSet(devNum, portNum, ipv6SourceSiteId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpv6SourcePortSiteIdGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       The Site ID reflects whether the packetes source or destination is
*       associated with a network Internal or External to the administrative site.
*       This function gets the source site id of a port.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*
* OUTPUTS:
*       ipv6SourceSiteIdPtr - (pointer to) type of the ipv6 source id
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpv6SourcePortSiteIdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    GT_U8               portNum;
    CPSS_IP_SITE_ID_ENT ipv6SourceSiteId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];


    /* call cpss api function */
    result = cpssExMxPmIpv6SourcePortSiteIdGet(devNum, portNum, &ipv6SourceSiteId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ipv6SourceSiteId);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpv6McPclIdSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Sets PCL ID for IPv6 MC rules
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       pclId         - PCL ID for IPv6 MC rules (0..1023)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpv6McPclIdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    pclId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    pclId = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpv6McPclIdSet(devNum, pclId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpv6McPclIdGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Gets PCL ID for IPv6 MC rules
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       pclIdPtr      - (pointer to) PCL ID for IPv6 MC rules (0..1023)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpv6McPclIdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    pclId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmIpv6McPclIdGet(devNum, &pclId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", pclId);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpv6McRuleValidStatusSet
*
* DESCRIPTION:
*       Function Relevant mode : Low Level API mode
*       Sets the valid status of IPv6 MC rule.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       ruleIndex     - index of the rule in the TCAM.
*                       IPv6 MC Rules and Policy rules use the same TCAM.
*       isValid       - rule validity state
*                       GT_TRUE:  entry is valid and will be used in TCAM lookup
*                       GT_FALSE: entry is not valid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpv6McRuleValidStatusSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    ruleIndex;
    GT_BOOL   isValid;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inArgs[1];
    isValid = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmIpv6McRuleValidStatusSet(devNum, ruleIndex, isValid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpv6McRuleValidStatusGet
*
* DESCRIPTION:
*       Function Relevant mode : Low Level API mode
*       Gets the valid status of IPv6 MC rule.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       ruleIndex     - index of the rule in the TCAM.
*                       IPv6 MC Rules and Policy rules use the same TCAM.
*
* OUTPUTS:
*       isValidPtr    - (pointer to) rule validity state
*                       GT_TRUE:  entry is valid and will be used in TCAM lookup
*                       GT_FALSE: entry is not valid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpv6McRuleValidStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    ruleIndex;
    GT_BOOL   isValid;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpv6McRuleValidStatusGet(devNum, ruleIndex, &isValid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isValid);

    return CMD_OK;
}



/**** database initialization **************************************/




static CMD_COMMAND dbCommands[] =
{

        {"cpssExMxPmIpRouterEngineEnableSet",
         &wrCpssExMxPmIpRouterEngineEnableSet,
         2, 0},
        {"cpssExMxPmIpRouterEngineEnableGet",
         &wrCpssExMxPmIpRouterEngineEnableGet,
         1, 0},
        {"cpssExMxPmIpSpecialRouterTriggerEnableSet",
         &wrCpssExMxPmIpSpecialRouterTriggerEnableSet,
         3, 0},
        {"cpssExMxPmIpSpecialRouterTriggerEnableGet",
         &wrCpssExMxPmIpSpecialRouterTriggerEnableGet,
         2, 0},
        {"cpssExMxPmIpExceptionCmdSet",
         &wrCpssExMxPmIpExceptionCmdSet,
         5, 0},
        {"cpssExMxPmIpExceptionCmdSet_1",
         &wrCpssExMxPmIpExceptionCmdSet,
         5, 0},
        {"cpssExMxPmIpExceptionCmdGet",
         &wrCpssExMxPmIpExceptionCmdGet,
         4, 0},
        {"cpssExMxPmIpRouteEntryAgeRefreshEnableSet",
         &wrCpssExMxPmIpRouteEntryAgeRefreshEnableSet,
         2, 0},
        {"cpssExMxPmIpRouteEntryAgeRefreshEnableGet",
         &wrCpssExMxPmIpRouteEntryAgeRefreshEnableGet,
         1, 0},
        {"cpssExMxPmIpEcmpUcRpfCheckEnableSet",
         &wrCpssExMxPmIpEcmpUcRpfCheckEnableSet,
         2, 0},
        {"cpssExMxPmIpEcmpUcRpfCheckEnableGet",
         &wrCpssExMxPmIpEcmpUcRpfCheckEnableGet,
         1, 0},
       {"cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableSet",
         &wrCpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableSet,
         2, 0},
        {"cpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet",
         &wrCpssExMxPmIpEcmpUcRpfFetchReducedSIPEntryEnableGet,
         1, 0},
        {"cpssExMxPmIpMtuProfileSet",
         &wrCpssExMxPmIpMtuProfileSet,
         3, 0},
        {"cpssExMxPmIpMtuProfileGet",
         &wrCpssExMxPmIpMtuProfileGet,
         2, 0},
        {"cpssExMxPmIpv6AddrPrefixScopeSet",
         &wrCpssExMxPmIpv6AddrPrefixScopeSet,
         1, 4},
        {"cpssExMxPmIpv6AddrPrefixScopeGetFirst",
         &wrCpssExMxPmIpv6AddrPrefixScopeGetFirst,
         1, 0},
        {"cpssExMxPmIpv6AddrPrefixScopeGetNext",
         &wrCpssExMxPmIpv6AddrPrefixScopeGetNext,
         1, 0},
        {"cpssExMxPmIpv6UcScopeCommandSet",
         &wrCpssExMxPmIpv6UcScopeCommandSet,
         5, 0},
        {"cpssExMxPmIpv6UcScopeCommandGet",
         &wrCpssExMxPmIpv6UcScopeCommandGet,
         4, 0},
        {"cpssExMxPmIpQosModeSet",
         &wrCpssExMxPmIpQosModeSet,
         2, 0},
        {"cpssExMxPmIpQosModeGet",
         &wrCpssExMxPmIpQosModeGet,
         1, 0},
        {"cpssExMxPmIpRouterMacSaBaseSet",
         &wrCpssExMxPmIpRouterMacSaBaseSet,
         2, 0},
        {"cpssExMxPmIpRouterMacSaBaseGet",
         &wrCpssExMxPmIpRouterMacSaBaseGet,
         1, 0},
        {"cpssExMxPmIpRouterMacSaLsbModeSet",
         &wrCpssExMxPmIpRouterMacSaLsbModeSet,
         2, 0},
        {"cpssExMxPmIpRouterMacSaLsbModeGet",
         &wrCpssExMxPmIpRouterMacSaLsbModeGet,
         1, 0},
        {"cpssExMxPmIpRouterPortVlanMacSaLsbSet",
         &wrCpssExMxPmIpRouterPortVlanMacSaLsbSet,
         3, 0},
        {"cpssExMxPmIpRouterPortVlanMacSaLsbGet",
         &wrCpssExMxPmIpRouterPortVlanMacSaLsbGet,
         2, 0},
        {"cpssExMxPmIpIcmpRedirectIfModeSet",
         &wrCpssExMxPmIpIcmpRedirectIfModeSet,
         4, 0},
        {"cpssExMxPmIpIcmpRedirectIfModeGet",
         &wrCpssExMxPmIpIcmpRedirectIfModeGet,
         3, 0},
        {"cpssExMxPmIpDataErrorHandlingModeSet",
         &wrCpssExMxPmIpDataErrorHandlingModeSet,
         2, 0},
        {"cpssExMxPmIpDataErrorHandlingModeGet",
         &wrCpssExMxPmIpDataErrorHandlingModeGet,
         1, 0},
        {"cpssExMxPmIpArpBcCommandSet",
         &wrCpssExMxPmIpArpBcCommandSet,
         2, 0},
        {"cpssExMxPmIpArpBcCommandGet",
         &wrCpssExMxPmIpArpBcCommandGet,
         1, 0},
        {"cpssExMxPmIpLpmMemoryOffsetSet",
         &wrCpssExMxPmIpLpmMemoryOffsetSet,
         3, 0},
        {"cpssExMxPmIpLpmMemoryOffsetGet",
         &wrCpssExMxPmIpLpmMemoryOffsetGet,
         2, 0},
        {"cpssExMxPmIpv6McScopeCommandSet",
         &wrCpssExMxPmIpv6McScopeCommandSet,
         6, 0},
        {"cpssExMxPmIpv6McScopeCommandGet",
         &wrCpssExMxPmIpv6McScopeCommandGet,
         4, 0},
        {"cpssExMxPmIpv6SourcePortSiteIdSet",
         &wrCpssExMxPmIpv6SourcePortSiteIdSet,
         3, 0},
        {"cpssExMxPmIpv6SourcePortSiteIdGet",
         &wrCpssExMxPmIpv6SourcePortSiteIdGet,
         2, 0},
        {"cpssExMxPmIpv6McPclIdSet",
         &wrCpssExMxPmIpv6McPclIdSet,
         2, 0},
        {"cpssExMxPmIpv6McPclIdGet",
         &wrCpssExMxPmIpv6McPclIdGet,
         1, 0},
        {"cpssExMxPmIpv6McRuleValidStatusSet",
         &wrCpssExMxPmIpv6McRuleValidStatusSet,
         3, 0},
        {"cpssExMxPmIpv6McRuleValidStatusGet",
         &wrCpssExMxPmIpv6McRuleValidStatusGet,
         2, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmIpCtrl
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
GT_STATUS cmdLibInitCpssExMxPmIpCtrl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

