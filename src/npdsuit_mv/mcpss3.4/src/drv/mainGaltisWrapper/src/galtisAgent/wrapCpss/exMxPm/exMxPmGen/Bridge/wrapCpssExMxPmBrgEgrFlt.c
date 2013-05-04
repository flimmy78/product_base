/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmBrgEgrFlt.c
*
* DESCRIPTION:
*       Wrapper functions for
*           cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgEgrFlt.h API's
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
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgEgrFlt.h>

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringPortUnkUnicastEnableSet  
*
* DESCRIPTION:
*      Enable/Disable egress Filtering for bridged Unknown Unicast packets  
*      on the specified egress port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       port     - CPU port, physical port number
*       enable   - GT_TRUE: Unknown Unicast packets are filtered and are 
*                           not forwarded to this port.
*                  GT_FALSE: Unknown Unicast packets are not filtered and may 
*                           be forwarded to this port. 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM        - wrong dev or port
*       GT_FAIL             - otherwise
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringPortUnkUnicastEnableSet
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
    result = cpssExMxPmBrgEgressFilteringPortUnkUnicastEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringPortUnkUnicastEnableGet
*
* DESCRIPTION:
*       Gets the current status of bridged Unknown Unicast packets filtering 
*       on the specified egress port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       port     - CPU port, physical port number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE: Unknown Unicast packets are filtered and are 
*                              not forwarded to this port.
*                     GT_FALSE: Unknown Unicast packets are not filtered and may 
*                              be forwarded to this port. 
*
* RETURNS:
*       GT_OK               - on success
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM        - wrong dev or port
*       GT_BAD_PTR          - one of the parameters is NULL pointer
*       GT_FAIL             - otherwise
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringPortUnkUnicastEnableGet
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
    result = cpssExMxPmBrgEgressFilteringPortUnkUnicastEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringPortUnregMcastEnableSet 
*
* DESCRIPTION:
*      Enable/Disable egress Filtering for bridged Unregistered Multicast packets
*      on the specified egress port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       port     - CPU port, physical port number
*       enable   - GT_TRUE: Unregistered Multicast packets are filtered and  
*                           are not forwarded to this port.
*                  GT_FALSE:Unregistered Multicast packets are not filtered  
*                           and may be forwarded to this port. 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong dev or port
*       GT_FAIL         - otherwise
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringPortUnregMcastEnableSet
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
    result = cpssExMxPmBrgEgressFilteringPortUnregMcastEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringPortUnregMcastEnableGet
*
* DESCRIPTION:
*       Gets the current status of bridged Unregistered Multicast packets  
*       filtering on the specified egress port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       port     - CPU port, physical port number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE: Unregistered Multicast packets are filtered and  
*                              are not forwarded to this port.
*                     GT_FALSE: Unregistered Multicast packets are not filtered  
*                              and may be forwarded to this port. 
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong dev or port
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringPortUnregMcastEnableGet
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
    result = cpssExMxPmBrgEgressFilteringPortUnregMcastEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringPortUnregBcEnableSet  
*
* DESCRIPTION:
*       Enable/Disable egress Filtering for Unregistered Broadcast packets
*       on the specified egress port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       port    - CPU port, physical port number
*       enable  - GT_TRUE  - Unregistered Broadcast packets are filtered and 
*                            are not forwarded to this port
*                 GT_FALSE - Unregistered Broadcast packets are not filtered  
*                            and may be forwarded to this port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM        - on wrong dev
*       GT_NOT_SUPPORTED    - This request is not supported.
*       GT_FAIL             - otherwise
*
* COMMENTS:
*       Unregistered Broadcast packets are:
*        - packets with destination MAC ff-ff-ff-ff-ff-ff
*        - destination MAC address lookup does not find a matching entry
*        - packets were not routed
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringPortUnregBcEnableSet
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
    result = cpssExMxPmBrgEgressFilteringPortUnregBcEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringPortUnregBcEnableGet
*
* DESCRIPTION:
*       Gets the current status of bridged unregistered broadcast packets 
*       filtering on the specified egress port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       port    - CPU port, physical port number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE  - Unregistered Broadcast packets are filtered and 
*                               are not forwarded to this port
*                    GT_FALSE - Unregistered Broadcast packets are not filtered  
*                               and may be forwarded to this port
*
* RETURNS:
*       GT_OK               - on success
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM        - on wrong dev
*       GT_NOT_SUPPORTED    - This request is not supported.
*       GT_BAD_PTR          - one of the parameters is NULL pointer
*       GT_FAIL             - otherwise
*
* COMMENTS:
*       Unregistered Broadcast packets are:
*        - packets with destination MAC ff-ff-ff-ff-ff-ff
*        - destination MAC address lookup not finds matching entry
*        - packets were not routed
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringPortUnregBcEnableGet
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
    result = cpssExMxPmBrgEgressFilteringPortUnregBcEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringCpuBcEnableSet 
*
* DESCRIPTION:
*      Enable/Disable egress Filtering for Registered Broadcast packets sent to the CPU.
*
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE: Registered Broadcast packets are filtered and 
*                           are not forwarded to the CPU
*                  GT_FLASE: Registered Broadcast packets are not filtered  
*                           and may be forwarded to the CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong dev
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None    
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringCpuBcEnableSet
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
    result = cpssExMxPmBrgEgressFilteringCpuBcEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringCpuBcEnableGet
*
* DESCRIPTION:
*       Gets the current state of egress Filtering for Registered Broadcast 
*       packets sent to the CPU.
*
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE: Registered Broadcast packets are filtered and 
*                           are not forwarded to the CPU
*                   GT_FLASE: Registered Broadcast packets are not filtered  
*                           and may be forwarded to the CPU
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong dev
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringCpuBcEnableGet
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
    result = cpssExMxPmBrgEgressFilteringCpuBcEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringVlanEnableSet
*
* DESCRIPTION:
*       Enable/Disable VLAN Egress Filtering on specified device for Bridged 
*       Known Unicast packets.
*       If enabled the VLAN egress filter verifies that the egress port is a 
*       member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE: VLAN egress filtering verifies that the egress
*                          port is a member of the packet's VLAN classification
*                 GT_FLASE: the VLAN egress filtering check is disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong dev
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       VLAN egress filtering is required by 802.1Q, but if required, this
*       mechanism can be disabled, which results in allowing "leaky VLANs".
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringVlanEnableSet
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
    result = cpssExMxPmBrgEgressFilteringVlanEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringVlanEnableGet
*
* DESCRIPTION:
*       Gets the current state of VLAN Egress Filtering on specified devices for  
*       Bridged Known Unicast packets.
*       If enabled, the VLAN egress filter verifies that the egress port is a 
*       member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE: VLAN egress filtering verifies that the egress
*                          port is a member of the packet's VLAN classification
*                   GT_FLASE: the VLAN egress filtering check is disabled.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong dev
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       VLAN egress filtering is required by 802.1Q, but if required, this
*       mechanism can be disabled, which results in allowing "leaky VLANs".
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringVlanEnableGet
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
    result = cpssExMxPmBrgEgressFilteringVlanEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringRoutedUnicastEnableSet
*
* DESCRIPTION:
*       Enable/Disable VLAN egress filtering on Routed Unicast packets.
*       If disabled, the destination port may or may not be a member of the VLAN.      
*       If enabled, the VLAN egress filter verifies that the egress port is a 
*       member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE: Egress filtering is enabled
*                  GT_FLASE: Egress filtering is disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong dev
*       GT_FAIL         - otherwise
*
* COMMENTS:   
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringRoutedUnicastEnableSet
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
    result = cpssExMxPmBrgEgressFilteringRoutedUnicastEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringRoutedUnicastEnableGet
*
* DESCRIPTION:
*       Gets the Enable/Disable VLAN egress filtering on Routed Unicast packets.       
*       If disabled, the destination port may or may not be a member of the VLAN.
*       If enabled, the VLAN egress filter verifies that the egress port is a 
*       member of the VID assigned to the packet.     
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE: Egress filtering is enabled
*                     GT_FLASE: Egress filtering is disabled
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong dev
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringRoutedUnicastEnableGet
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
    result = cpssExMxPmBrgEgressFilteringRoutedUnicastEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringRoutedSpanEnableSet
*
* DESCRIPTION:
*       Enable/Disable STP egress Filtering on Routed Packets.
*       If disabled the packet may be forwarded to its egress port, 
*       regardless of its Span State.
*
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE: Span state filtering is enabled
*                  GT_FLASE: Span state filtering is disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong dev
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringRoutedSpanEnableSet
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
    result = cpssExMxPmBrgEgressFilteringRoutedSpanEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringRoutedSpanEnableGet
*
* DESCRIPTION:
*       Gets the STP egress Filtering current state on Routed Packets.
*       If disabled, the packet may be forwarded to its egress port, 
*       regardless of its Span State.
*
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE: Span state filtering is enabled
*                     GT_FLASE: Span state filtering is disabled
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong dev
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringRoutedSpanEnableGet
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
    result = cpssExMxPmBrgEgressFilteringRoutedSpanEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringUnregMcastModeSet
*
* DESCRIPTION:
*      Set unregistered MC packets filtering mode.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       mode     - CPSS_EXMXPM_BRG_EGR_FLT_UNREG_MC_MODE_GLOBAL_E: Global configuration.
*                  CPSS_EXMXPM_BRG_EGR_FLT_UNREG_MC_MODE_VLAN_E: Per vlan configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev or mode
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringUnregMcastModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_BRG_EGR_FLT_UNREG_MC_MODE_ENT    mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode   = (CPSS_EXMXPM_BRG_EGR_FLT_UNREG_MC_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgEgressFilteringUnregMcastModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringUnregMcastModeGet
*
* DESCRIPTION:
*      Get unregistered MC packets filtering mode.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       modePtr - (pointer to)
*                 CPSS_EXMXPM_BRG_EGR_FLT_UNREG_MC_MODE_GLOBAL_E: Global configuration.
*                 CPSS_EXMXPM_BRG_EGR_FLT_UNREG_MC_MODE_VLAN_E: Per vlan configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringUnregMcastModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_BRG_EGR_FLT_UNREG_MC_MODE_ENT   mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgEgressFilteringUnregMcastModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringUnregMcastCmdSet
*
* DESCRIPTION:
*       Set the Egress command for unregistered multicast packets when global
*       configuration is used.       
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*       unregMcCmd - The command applied on unregistered multicast packets.
*
* OUTPUTS:
*       None;
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When per vlan configuration is used for filtering unregistered 
*       multicast packets, the appropriate per vlan command is set using 
*       "cpssExMxPmBrgVlanEgressUnregMcFilterCmdSet" API. 
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringUnregMcastCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_UNREG_MC_EGR_FILTER_CMD_ENT    unregMcCmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    unregMcCmd  = (CPSS_UNREG_MC_EGR_FILTER_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgEgressFilteringUnregMcastCmdSet(devNum, unregMcCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringUnregMcastCmdGet
*
* DESCRIPTION:
*       Get the Egress command for unregistered multicast packets when global
*       configuration is used.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       unregMcCmdPtr - (pointer to) the command applied on unregistered 
*                       multicast packets.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_STATE    - on wrong HW value
*       GT_BAD_PTR      - on NULL pointer
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When per vlan configuration is used for filtering unregistered multicast 
*       packets, the appropriate per vlan command is retreived using 
*       "cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet" API.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringUnregMcastCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_UNREG_MC_EGR_FILTER_CMD_ENT    unregMcCmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgEgressFilteringUnregMcastCmdGet(devNum, &unregMcCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", unregMcCmd);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringUnkUnicastCmdSet
*
* DESCRIPTION:
*       Set the Egress command for unknown unicast packets.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       unkUnicastCmd - The command applied on unknown unicast packets.
*
* OUTPUTS:
*       None;
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None. 
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringUnkUnicastCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_UNREG_MC_EGR_FILTER_CMD_ENT    unkUnicastCmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    unkUnicastCmd   = (CPSS_UNREG_MC_EGR_FILTER_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgEgressFilteringUnkUnicastCmdSet(devNum, unkUnicastCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgEgressFilteringUnkUnicastCmdGet
*
* DESCRIPTION:
*       Get the Egress command for unknown unicast packets.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                 - device number
*
* OUTPUTS:
*       unkUnicastCmdPtr - (pointer to) the command applied on unknown 
*                          unicast packets.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_STATE    - on wrong HW value
*       GT_BAD_PTR      - on NULL pointer
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgEgressFilteringUnkUnicastCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_UNREG_MC_EGR_FILTER_CMD_ENT    unkUnicastCmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgEgressFilteringUnkUnicastCmdGet(devNum, &unkUnicastCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", unkUnicastCmd);

    return CMD_OK;
}

/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmBrgEgressFilterinPortUnkUnicastEnableSet",
         &wrCpssExMxPmBrgEgressFilteringPortUnkUnicastEnableSet,
         3, 0},
        {"cpssExMxPmBrgEgressFilterinPortUnkUnicastEnableGet",
         &wrCpssExMxPmBrgEgressFilteringPortUnkUnicastEnableGet,
         2, 0},
        {"cpssExMxPmBrgEgressFilterinPortUnregMcastEnableSet",
         &wrCpssExMxPmBrgEgressFilteringPortUnregMcastEnableSet,
         3, 0},
        {"cpssExMxPmBrgEgressFilterinPortUnregMcastEnableGet",
         &wrCpssExMxPmBrgEgressFilteringPortUnregMcastEnableGet,
         2, 0},
        {"cpssExMxPmBrgEgressFilteringPortUnregBcEnableSet",
         &wrCpssExMxPmBrgEgressFilteringPortUnregBcEnableSet,
         3, 0},
        {"cpssExMxPmBrgEgressFilteringPortUnregBcEnableGet",
         &wrCpssExMxPmBrgEgressFilteringPortUnregBcEnableGet,
         2, 0},
        {"cpssExMxPmBrgEgressFilteringCpuBcEnableSet",
         &wrCpssExMxPmBrgEgressFilteringCpuBcEnableSet,
         2, 0},
        {"cpssExMxPmBrgEgressFilteringCpuBcEnableGet",
         &wrCpssExMxPmBrgEgressFilteringCpuBcEnableGet,
         1, 0},
        {"cpssExMxPmBrgEgressFilteringVlanEnableSet",
         &wrCpssExMxPmBrgEgressFilteringVlanEnableSet,
         2, 0},
        {"cpssExMxPmBrgEgressFilteringVlanEnableGet",
         &wrCpssExMxPmBrgEgressFilteringVlanEnableGet,
         1, 0},
        {"cpssExMxPmBrgEgressFilteringRoutedUnicastEnableSet",
         &wrCpssExMxPmBrgEgressFilteringRoutedUnicastEnableSet,
         2, 0},
        {"cpssExMxPmBrgEgressFilteringRoutedUnicastEnableGet",
         &wrCpssExMxPmBrgEgressFilteringRoutedUnicastEnableGet,
         1, 0},
        {"cpssExMxPmBrgEgressFilteringRoutedSpanEnableSet",
         &wrCpssExMxPmBrgEgressFilteringRoutedSpanEnableSet,
         2, 0},
        {"cpssExMxPmBrgEgressFilteringRoutedSpanEnableGet",
         &wrCpssExMxPmBrgEgressFilteringRoutedSpanEnableGet,
         1, 0},
        {"cpssExMxPmBrgEgressFilteringUnregMcastModeSet",
         &wrCpssExMxPmBrgEgressFilteringUnregMcastModeSet,
         2, 0},
        {"cpssExMxPmBrgEgressFilteringUnregMcastModeGet",
         &wrCpssExMxPmBrgEgressFilteringUnregMcastModeGet,
         1, 0},
        {"cpssExMxPmBrgEgressFilteringUnregMcastCmdSet",
         &wrCpssExMxPmBrgEgressFilteringUnregMcastCmdSet,
         2, 0},
        {"cpssExMxPmBrgEgressFilteringUnregMcastCmdGet",
         &wrCpssExMxPmBrgEgressFilteringUnregMcastCmdGet,
         1, 0},
        {"cpssExMxPmBrgEgressFilteringUnkUnicastCmdSet",
         &wrCpssExMxPmBrgEgressFilteringUnkUnicastCmdSet,
         2, 0},
        {"cpssExMxPmBrgEgressFilteringUnkUnicastCmdGet",
         &wrCpssExMxPmBrgEgressFilteringUnkUnicastCmdGet,
         1, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmBrgEgrFlt
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
GT_STATUS cmdLibInitCpssExMxPmBrgEgrFlt
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

