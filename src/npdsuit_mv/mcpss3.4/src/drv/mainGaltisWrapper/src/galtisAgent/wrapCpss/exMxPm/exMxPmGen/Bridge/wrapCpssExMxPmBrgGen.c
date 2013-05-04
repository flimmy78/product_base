/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmBrgGen.c
*
* DESCRIPTION:
*       Wrapper functions for cpssExMxPmBrgGen cpss.exMxPm functions
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


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgGen.h>
 

/*******************************************************************************
* cpssExMxPmBrgGenDropInvalidSaEnableSet    [DB+]
*
* DESCRIPTION:
*       Sets an ability (Enable/Disable) to drop Ethernet packets
*       with Multicast MAC SA (MACSA[40] = 1).
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - Boolean value:
*                         GT_TRUE  - packets with the Mc MAC SA are dropped.
*                                    These packets are assigned with a soft 
*                                    drop or a hard drop Command, according to
*                                    the Drop Mode of packets with invalid SA.
*                         GT_FALSE - packets with the Mc MAC SA aren't dropped.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenDropInvalidSaEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgGenDropInvalidSaEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenDropInvalidSaEnableGet    [DB+]
*
* DESCRIPTION:
*       Gets Ethernet packets with Multicast MAC SA (MACSA[40] = 1) 
*       dropping ability status(Enable/Disable).  
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer to the Boolean value:
*                         GT_TRUE  - packets with the Mc MAC SA are dropped.
*                                    These packets are assigned with a soft 
*                                    drop or a hard drop Command, according to
*                                    the Drop Mode of packets with invalid SA.
*                         GT_FALSE - packets with the Mc MAC SA aren't dropped.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenDropInvalidSaEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgGenDropInvalidSaEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenPortUcLocalSwitchingEnableSet [DB+]
*
* DESCRIPTION:
*       Sets local switching back ability (Enable/Disable) through  
*       the ingress interface for known Unicast packets.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       port            - port number.
*       enable          - Boolean value:
*                         GT_TRUE  - Unicast packets whose bridging decision 
*                                    is to be forwarded back to its Ingress 
*                                    port or trunk may be forwarded back to 
*                                    their source.
*                         GT_FALSE - Unicast packets whose bridging decision 
*                                    is to be forwarded back to its Ingress 
*                                    port or trunk are assigned with a soft 
*                                    drop command.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*
* COMMENTS:
*       Routed packets are not subject to local switching configuration
*       constraints.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenPortUcLocalSwitchingEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    port;
    GT_BOOL  enable;


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
    result = cpssExMxPmBrgGenPortUcLocalSwitchingEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet [DB+]
*
* DESCRIPTION:
*       Gets local switching back through the ingress interface    
*       for known Unicast packets ability status(Enable/Disable).
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       port            - port number.
*
* OUTPUTS:
*       enablePtr       - pointer to the Boolean value:
*                         GT_TRUE  - Unicast packets whose bridging decision 
*                                    is to be forwarded back to its Ingress 
*                                    port or trunk may be forwarded back to 
*                                    their source.
*                         GT_FALSE - Unicast packets whose bridging decision 
*                                    is to be forwarded back to its Ingress 
*                                    port or trunk are assigned with a soft 
*                                    drop command.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       Routed packets are not subject to local switching configuration
*       constraints.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenPortUcLocalSwitchingEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    port;
    GT_BOOL  enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet(devNum, port, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet [DB+]
*
* DESCRIPTION:
*       Enable/Disable sending Multicast packets back to its source 
*       port on the local device.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       port            - physical port number.
*       enable          - Boolean value:
*                         GT_TRUE  - Multicast packets may be sent back to   
*                                    their source port on the local device.
*                         GT_FALSE - Multicast packets are not sent back to  
*                                    their source port on the local device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*
* COMMENTS:
*       The Routed packets may be sent to their original source 
*       regardless of this setting.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenPortMcLocalSwitchingEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    port;
    GT_BOOL  enable;


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
    result = cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet [DB+]
*
* DESCRIPTION:       
*       Get the current status of Multicast packets local switching.
*
* INPUTS:
*       devNum          - physical device number.
*       port            - physical port number.
*
* OUTPUTS:
*       enablePtr       - pointer to the Boolean value:
*                         GT_TRUE  - Multicast packets may be sent back to   
*                                    their source port on the local device.
*                         GT_FALSE - Multicast packets are not sent back to  
*                                    their source port on the local device.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       Routed packets may be sent to their original source 
*       regardless of this setting.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenPortMcLocalSwitchingEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    port;
    GT_BOOL  enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet(devNum, port, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenDropIpMcEnableSet [DB+]
*
* DESCRIPTION:
*       Sets an ability(Enable/Disable) to drop IPv4/6 Mc packets.
*       If it is enabled, the Drop Command assigned to all    
*       Ethernet packets with MAC Multicast DA (MACDA[40] = 1) in the   
*       IPv4 Multicast IANA range (01-00-5E-00-00-00 to 01-00-5E-7F-FF-FF)   
*       or in IPv6 Multicast IANA range (33-33-xx-xx-xx). 
*       Drop type can be configured to hard or soft. 
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - Boolean value:
*                         GT_TRUE  - MC packets with a IP Mc MAC DA are dropped  
*                                    and are assigned with a hard or soft drop  
*                                    command according to the IP Mc Drop Mode.
*                         GT_FALSE - MC packets with a IP Mc MAC DA are not 
*                                    dropped.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenDropIpMcEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgGenDropIpMcEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenDropIpMcEnableGet [DB+]
*
* DESCRIPTION:
*       Gets IPv4/6 Mc packets dropping ability status(Enable/Disable).
*       If it is enabled, the Drop Command assigned to all    
*       Ethernet packets with MAC Multicast DA (MACDA[40] = 1) in the   
*       IPv4 Multicast IANA range (01-00-5E-00-00-00 to 01-00-5E-7F-FF-FF)   
*       or in IPv6 Multicast IANA range (33-33-xx-xx-xx). 
*       Drop type can be configured to hard or soft. 
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer to the Boolean value:
*                         GT_TRUE  - MC packets with a IP Mc MAC DA are dropped  
*                                    and are assigned with a hard or soft drop  
*                                    command according to the IP Mc Drop Mode.
*                         GT_FALSE - MC packets with a IP Mc MAC DA are not 
*                                    dropped.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenDropIpMcEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgGenDropIpMcEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenDropIpMcDropModeSet   [DB+]
*
* DESCRIPTION:
*       Sets the dropping mode of IPv4/6 Mc packets 
*       (when the global IPv4/6 Mc dropping is enabled). 
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       dropMode        - drop mode (soft or hard) for IPv4/6 Mc packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or dropMode.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenDropIpMcDropModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    CPSS_DROP_MODE_TYPE_ENT     dropMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dropMode = (CPSS_DROP_MODE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgGenDropIpMcDropModeSet(devNum, dropMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenDropIpMcDropModeGet   [DB+]
*
* DESCRIPTION:
*       Gets the dropping mode of IPv4/6 Mc packets 
*       (when the global IPv4/6 Mc dropping is enabled). 
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       dropModePtr     - pointer to the drop mode (soft or hard) value 
*                         for IPv4/6 Mc packets.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenDropIpMcDropModeGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    CPSS_DROP_MODE_TYPE_ENT     dropModePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgGenDropIpMcDropModeGet(devNum, &dropModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dropModePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenDropNonIpMcEnableSet  [DB+]
*
* DESCRIPTION:
*       Sets an ability(Enable/Disable) to drop non-IPv4/6 Mc packets.
*       If it is enabled, the Drop Command assigned to all Ethernet   
*       packets with MAC Multicast DA (MACDA[40] = 1), that are NOT in
*       the IPv4 Multicast IANA range (01-00-5E-00-00-00 to 01-00-5E-7F-FF-FF)   
*       or in IPv6 Multicast IANA range (33-33-xx-xx-xx). 
*       This does not include ARP Broadcast or Broadcast packets.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - Boolean value:
*                         GT_TRUE  - MC packets with a non-IP Mc MAC DA are   
*                                    dropped and are assigned with a hard or   
*                                    soft drop command according to the  
*                                    IP Mc Not-In-IANA Range Drop Mode.
*                         GT_FALSE - MC packets with a non-IP Mc MAC DA are not 
*                                    dropped.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenDropNonIpMcEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgGenDropNonIpMcEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPmBrgGenDropNonIpMcEnableGet  [DB+]
*
* DESCRIPTION:
*       Gets non-IPv4/6 Mc packets dropping ability status(Enable/Disable).
*       If it is enabled, the Drop Command assigned to all Ethernet   
*       packets with MAC Multicast DA (MACDA[40] = 1), that are NOT in
*       the IPv4 Multicast IANA range (01-00-5E-00-00-00 to 01-00-5E-7F-FF-FF)   
*       or in IPv6 Multicast IANA range (33-33-xx-xx-xx). 
*       This does not include ARP Broadcast or Broadcast packets.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer to the Boolean value:
*                         GT_TRUE  - MC packets with a non-IP Mc MAC DA are   
*                                    dropped and are assigned with a hard or   
*                                    soft drop command according to the  
*                                    IP Mc Not-In-IANA Range Drop Mode.
*                         GT_FALSE - MC packets with a non-IP Mc MAC DA are not 
*                                    dropped.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenDropNonIpMcEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgGenDropNonIpMcEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenDropNonIpMcDropModeSet    [DB+]
*
* DESCRIPTION:
*       Sets the dropping mode of non-IPv4/6 Mc packets 
*       (when the global non-IPv4/6 Mc dropping is enabled). 
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       dropMode        - drop mode (soft or hard) for non-IPv4/6 Mc packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or dropMode.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenDropNonIpMcDropModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                    devNum;
    CPSS_DROP_MODE_TYPE_ENT  dropMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dropMode = (CPSS_DROP_MODE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgGenDropNonIpMcDropModeSet(devNum, dropMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPmBrgGenDropNonIpMcDropModeGet    [DB+]
*
* DESCRIPTION:
*       Gets the dropping mode of non-IPv4/6 Mc packets 
*       (when the global non-IPv4/6 Mc dropping is enabled). 
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       dropModePtr     - pointer to the drop mode (soft or hard) value 
*                         for non-IPv4/6 Mc packets.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenDropNonIpMcDropModeGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                    devNum;
    CPSS_DROP_MODE_TYPE_ENT  dropMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgGenDropNonIpMcDropModeGet(devNum, &dropMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dropMode);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet   [DB+]
* Galtis str: cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFltrEnSet 
*
* DESCRIPTION:
*       Enables/Disables excluding of unregistered IPv4/v6 Link Local Multicast 
*       packets from the per-InLIF Unregistered IPv4/v6 Multicast command.
*       There is a global configuration option whether to apply this filter to 
*       unregistered IPv4/v6 link-local Multicast with destination IP address 
*       in the range of 224.0.0.X for IPv4 or FF02::/120 for IPv6. This is 
*       useful for IGMP snooping applications, which typically drop 
*       unregistered IPv4/v6 Multicast packets except for link-local Multicast.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       protocolStack   - IP stack version (IPv4, IPv6 or both).
*       enable          - Boolean value:
*                         GT_TRUE -  Unregistered Link-Local Ipv4/6 Mc packet  
*                                    are considered as registered (not subject  
*       `                            to Unregistered filters).
*                         GT_FALSE - Unregistered Link-Local Ipv4/6 Mc packet  
*                                    are considered as unregistered.   
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or protocolStack.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    CPSS_IP_PROTOCOL_STACK_ENT  protocolStack;
    GT_BOOL                     enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet(devNum, protocolStack, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet   [DB+]
* Galtis str: cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFltrEnGet 
*
* DESCRIPTION:
*       Gets current state (Enable/Disable) of excluding of unregistered 
*       IPv4/v6 Link Local Multicast packets from the per-InLIF Unregistered
*       IPv4/v6 Multicast command.
*       There is a global configuration option whether to apply this filter to 
*       unregistered IPv4/v6 link-local Multicast with destination IP address 
*       in the range of 224.0.0.X for IPv4 or FF02::/120 for IPv6. This is 
*       useful for IGMP snooping applications, which typically drop 
*       unregistered IPv4/v6 Multicast packets except for link-local Multicast.

*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       protocolStack   - IP stack version (IPv4, IPv6 or both).
*
* OUTPUTS:
*       enablePtr       - pointer to the Boolean value:
*                         GT_TRUE -  Unregistered Link-Local Ipv4/6 Mc packet  
*                                    are considered as registered (not subject  
*       `                            to Unregistered filters).
*                         GT_FALSE - Unregistered Link-Local Ipv4/6 Mc packet  
*                                    are considered as unregistered.   
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or when it is using for dual stack 
*                         (protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E).
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       In case of dual stack current API should be applied for 
*       Unregistered IPv4 and Unregistered IPv6 LL MC packets sepparatly.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    CPSS_IP_PROTOCOL_STACK_ENT  protocolStack;
    GT_BOOL                     enablePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet(devNum,
                                                  protocolStack, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/* cpssExMxPmBrgGenRateLimitGlobalCfg   Table   [DB+] */

/*******************************************************************************
* cpssExMxPmBrgGenRateLimitGlobalCfgSet      [table 1st func added]
*   Table: cpssExMxPmBrgGenRateLimitGlobalCfg
*
* DESCRIPTION:
*       Configures global ingress rate limit parameters: drop mode (soft/hard), 
*       rate limiting mode (bytes/packets counting) and four configurable 
*       global time window periods are assigned to ports as a function of 
*       their operating speed: 10 Gbps, 1 Gbps, 100 Mbps, and 10 Mbps.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       brgRateLimitPtr - pointer to the global rate limit parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on HW error.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_BAD_PTR          - on NULL pointer.
*       GT_OUT_OF_RANGE     - on wrong time windows size values.
*       GT_NOT_INITIALIZED  - on not initialized sw core clock value.
*
* COMMENTS:
*   Supported windows for ExMxPm devices:
*       1000 Mbps: range - 0-16128   uSec granularity - 256 uSec
*       100  Mbps: range - 0-130816  uSec granularity - 256 uSec
*       10   Mbps: range - 0-1048320 uSec granularity - 256 uSec
*       10   Gbps: range - 0-104832  uSec granularity - 25.6 uSec 
*
*       Time ranges and granularity specified for clock 200 Mhrz for
*       they changed linearly with the clock value.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenRateLimitGlobalCfgSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               dev;
    CPSS_EXMXPM_BRG_GEN_RATE_LIMIT_STC  brgRateLimitPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    brgRateLimitPtr.rateLimitModeL1 = (CPSS_EXMXPM_BRG_GEN_L1_RATE_LIMIT_ENT)inFields[0];
    brgRateLimitPtr.dropMode = (CPSS_DROP_MODE_TYPE_ENT)inFields[1];
    brgRateLimitPtr.rMode = (CPSS_RATE_LIMIT_MODE_ENT)inFields[2];
    brgRateLimitPtr.win10Mbps = (GT_U32)inFields[3];
    brgRateLimitPtr.win100Mbps = (GT_U32)inFields[4];
    brgRateLimitPtr.win1000Mbps = (GT_U32)inFields[5];
    brgRateLimitPtr.win10Gbps = (GT_U32)inFields[6];

    /* call cpss api function */
    result = cpssExMxPmBrgGenRateLimitGlobalCfgSet(dev, &brgRateLimitPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenRateLimitGlobalCfgGet
*
* DESCRIPTION:
*       Gets global ingress rate limit parameters: drop mode (soft/hard), 
*       rate limiting mode (bytes/packets counting) and four configurable 
*       global time window periods are assigned to ports as a function of 
*       their operating speed: 10 Gbps, 1 Gbps, 100 Mbps, and 10 Mbps.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       brgRateLimitPtr - pointer to the global rate limit parameters.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*   Supported windows for ExMxPm devices:
*       1000 Mbps: range - 0-16128   uSec granularity - 256 uSec
*       100  Mbps: range - 0-130816  uSec granularity - 256 uSec
*       10   Mbps: range - 0-1048320 uSec granularity - 256 uSec
*       10   Gbps: range - 0-104832  uSec granularity - 25.6 uSec 
*
*       Time ranges and granularity specified for clock 200 Mhrz for
*       they changed linearly with the clock value.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenRateLimitGlobalCfgGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               dev;
    CPSS_EXMXPM_BRG_GEN_RATE_LIMIT_STC  brgRateLimitPtr;
    static GT_U32                       isNext = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(isNext == 1)
    {
        isNext = 0;
        /* there is no GetNext for wrapper */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    isNext = 1;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgGenRateLimitGlobalCfgGet(dev, &brgRateLimitPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = brgRateLimitPtr.rateLimitModeL1;
    inFields[1] = brgRateLimitPtr.dropMode;
    inFields[2] = brgRateLimitPtr.rMode;
    inFields[3] = brgRateLimitPtr.win10Mbps;
    inFields[4] = brgRateLimitPtr.win100Mbps;
    inFields[5] = brgRateLimitPtr.win1000Mbps;
    inFields[6] = brgRateLimitPtr.win10Gbps;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                     inFields[3], inFields[4], inFields[5], inFields[6]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenRateLimitGlobalDropCntrSet    [DB+]    GT_U64 - ?
*
* DESCRIPTION:
*       Sets global rate-limit drop counter (40bits counter). 
*       This counter counts all packets dropped due to rate limit
*       on ports which have the rate-limit counting enabled.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       dropCntrPtr      - pointer to the counter value.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenRateLimitGlobalDropCntrSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U64   dropCntrPtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dropCntrPtr.l[0] = (GT_32)inArgs[1];
    dropCntrPtr.l[1] = (GT_32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmBrgGenRateLimitGlobalDropCntrSet(devNum, &dropCntrPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenRateLimitGlobalDropCntrGet    [DB+]    GT_U64 - ?
*
* DESCRIPTION:
*       Gets global rate-limit drop counter (40bits counter). 
*       This counter counts all packets dropped due to rate limit
*       on ports which have the rate-limit counting enabled.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       dropCntrPtr      - pointer to the counter value.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenRateLimitGlobalDropCntrGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U64   dropCntrPtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgGenRateLimitGlobalDropCntrGet(devNum, &dropCntrPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", dropCntrPtr.l[0], dropCntrPtr.l[1]);
    return CMD_OK;
}

/* cpssExMxPmBrgGenPortRateLimit    Table   [DB+]    */

/*******************************************************************************
* cpssExMxPmBrgGenPortRateLimitSet
*
* DESCRIPTION:
*       Configures port ingress rate limit parameters.
*       Each port maintains rate limits for unknown Unicast packets, known
*       Unicast packets, Multicast packets and Broadcast packets, TCP SYN 
*       packets, single configurable limit threshold value and a single 
*       internal counter.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       port            - port number.
*       portCfgPtr      - pointer to the port rate limit configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_OUT_OF_RANGE - on out of range rate limit value.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenPortRateLimitSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                result;

    GT_U8                                    dev;
    GT_U8                                    port;
    CPSS_EXMXPM_BRG_GEN_PORT_RATE_LIMIT_STC  portCfgPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    portCfgPtr.enableBc = (GT_BOOL)inFields[0];
    portCfgPtr.enableMc = (GT_BOOL)inFields[1];
    portCfgPtr.enableUcUnk = (GT_BOOL)inFields[2];
    portCfgPtr.enableUcKnown = (GT_BOOL)inFields[3];
    portCfgPtr.enableTcpSyn = (GT_BOOL)inFields[4];
    portCfgPtr.rateLimit = (GT_U32)inFields[5];
    /* initialize with constant */
    portCfgPtr.enableMcUnreg = GT_FALSE;


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPmBrgGenPortRateLimitSet(dev, port, &portCfgPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenPortRateLimitGet
*
* DESCRIPTION:
*       Gets port ingress rate limit parameters.
*       Each port maintains rate limits for unknown Unicast packets, known
*       Unicast packets, Multicast packets and Broadcast packets, TCP SYN 
*       packets, single configurable limit threshold value and a single 
*       internal counter.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       port            - port number.
*
* OUTPUTS:
*       portCfgPtr      - pointer to the port rate limit configuration.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenPortRateLimitGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       dev;
    GT_U8                                       port;
    CPSS_EXMXPM_BRG_GEN_PORT_RATE_LIMIT_STC     portGfgPtr;
    static GT_U32                               isNext = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(isNext == 1)
    {
        isNext = 0;
        /* there is no GetNext for wrapper */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    isNext = 1;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPmBrgGenPortRateLimitGet(dev, port, &portGfgPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = portGfgPtr.enableBc;
    inFields[1] = portGfgPtr.enableMc;
    inFields[2] = portGfgPtr.enableUcUnk;
    inFields[3] = portGfgPtr.enableUcKnown;
    inFields[4] = portGfgPtr.enableTcpSyn;
    inFields[5] = portGfgPtr.rateLimit;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1],
                   inFields[2], inFields[3], inFields[4], inFields[5]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenPortRateLimitDropCntrEnableSet    [DB+]
*
* DESCRIPTION:
*       Sets an ability (Enable/Disable) to count the packets (or bytes) 
*       received on this port, that were dropped due to ingress rate limiting.
*       The bytes or packets, dropped due to ingress rate limiting are counted 
*       by global counter which counts dropped packets from all port that have 
*       this setting enabled.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       port            - port number.
*       enable          - Boolean value:
*                         GT_TRUE  - rate limit drop counting enable on port.
*                         GT_FALSE - rate limit drop counting disable on port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenPortRateLimitDropCntrEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    port;
    GT_BOOL  enable;


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
    result = cpssExMxPmBrgGenPortRateLimitDropCntrEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet    [DB+]
*
* DESCRIPTION:
*       Gets an ability status (Enable/Disable) to count the packets (or bytes) 
*       received on this port, that were dropped due to ingress rate limiting.
*       The bytes or packets, dropped due to ingress rate limiting are counted 
*       by global counter which counts dropped packets from all port that have 
*       this setting enabled.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       port            - port number.
*
* OUTPUTS:
*       enablePtr       - pointer to the Boolean value:
*                         GT_TRUE  - rate limit drop counting enable on port.
*                         GT_FALSE - rate limit drop counting disable on port.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenPortRateLimitDropCntrEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    port;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet(devNum, port, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableSet    [DB+]
*
* DESCRIPTION:
*       Sets FDB DA lookup bypass ability (Enable/Disable) for IP packets are
*       sent to the Router MAC address (packet is IP and the InLIF is enabled   
*       for routing) when MAC DA does results in a MAC-to-ME match.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enable          - Boolean value:
*                         GT_TRUE  - Enable DA Lookup for MAC-to-ME
*                                    (FDB DA lookup isn't bypassed).
*                         GT_FALSE - DA lookup is not performed for packets
*                                    whose MAC DA matches the MAC-to-ME
*                                    configuration (FDB DA lookup is bypassed).
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*
* COMMENTS:
*       When the FDB resides in external memory, bypassing the 
*       DA lookup saves bandwidth on the external interface.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenDaLookupForMacToMePacketsEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet    [DB+]
*
* DESCRIPTION:
*       Gets FDB DA lookup bypass ability (Enable/Disable) for IP packets are
*       sent to the Router MAC address (packet is IP and the InLIF is enabled   
*       for routing) when MAC DA does results in a MAC-to-ME match.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer to the Boolean value:
*                         GT_TRUE  - Enable DA Lookup for MAC-to-ME
*                                    (FDB DA lookup isn't bypassed).
*                         GT_FALSE - DA lookup is not performed for packets
*                                    whose MAC DA matches the MAC-to-ME
*                                    configuration (FDB DA lookup is bypassed).
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       When the FDB resides in external memory, bypassing the 
*       DA lookup saves bandwidth on the external interface.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgGenBpduTrapEnableSet
*
* DESCRIPTION:
*     Sets an ability(Enable/Disable) to trap BPDU Multicast MAC 
*     address 01-80-C2-00-00-00.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum - physical device number.
*       enable - GT_TRUE - trap packets to CPU with BPDU Multicast MAC address
*                01-80-C2-00-00-00 on a ingress port whose span state
*                is not disabled.
*                GT_FALSE - packets with BPDU Multicast MAC address
*                01-80-C2-00-00-00 are not affected by ingress port's span state
*                and forwarded as standard Multicast frames
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenBpduTrapEnableSet
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
    result = cpssExMxPmBrgGenBpduTrapEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgGenBpduTrapEnableGet
*
* DESCRIPTION:
*     Gets an ability(Enable/Disable) to trap BPDU Multicast MAC 
*     address 01-80-C2-00-00-00.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr - pointer to the Boolean value:
*                GT_TRUE - trap packets to CPU with BPDU Multicast MAC address
*                01-80-C2-00-00-00 on a ingress port whose span state
*                is not disabled.
*                GT_FALSE - packets with BPDU Multicast MAC address
*                01-80-C2-00-00-00 are not affected by ingress port's span state
*                and forwarded as standard Multicast frames
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgGenBpduTrapEnableGet
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
    result = cpssExMxPmBrgGenBpduTrapEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxPmBrgGenDropInvalidSaEnableSet",
        &wrCpssExMxPmBrgGenDropInvalidSaEnableSet,
        2, 0},

    {"cpssExMxPmBrgGenDropInvalidSaEnableGet",
        &wrCpssExMxPmBrgGenDropInvalidSaEnableGet,
        1, 0},

    {"cpssExMxPmBrgGenPortUcLocalSwitchingEnableSet",
        &wrCpssExMxPmBrgGenPortUcLocalSwitchingEnableSet,
        3, 0},

    {"cpssExMxPmBrgGenPortUcLocalSwitchingEnableGet",
        &wrCpssExMxPmBrgGenPortUcLocalSwitchingEnableGet,
        2, 0},

    {"cpssExMxPmBrgGenPortMcLocalSwitchingEnableSet",
        &wrCpssExMxPmBrgGenPortMcLocalSwitchingEnableSet,
        3, 0},

    {"cpssExMxPmBrgGenPortMcLocalSwitchingEnableGet",
        &wrCpssExMxPmBrgGenPortMcLocalSwitchingEnableGet,
        2, 0},

    {"cpssExMxPmBrgGenDropIpMcEnableSet",
        &wrCpssExMxPmBrgGenDropIpMcEnableSet,
        2, 0},

    {"cpssExMxPmBrgGenDropIpMcEnableGet",
        &wrCpssExMxPmBrgGenDropIpMcEnableGet,
        1, 0},

    {"cpssExMxPmBrgGenDropIpMcDropModeSet",
        &wrCpssExMxPmBrgGenDropIpMcDropModeSet,
        2, 0},

    {"cpssExMxPmBrgGenDropIpMcDropModeGet",
        &wrCpssExMxPmBrgGenDropIpMcDropModeGet,
        1, 0},

    {"cpssExMxPmBrgGenDropNonIpMcEnableSet",
        &wrCpssExMxPmBrgGenDropNonIpMcEnableSet,
        2, 0},

    {"cpssExMxPmBrgGenDropNonIpMcEnableGet",
        &wrCpssExMxPmBrgGenDropNonIpMcEnableGet,
        1, 0},

    {"cpssExMxPmBrgGenDropNonIpMcDropModeSet",
        &wrCpssExMxPmBrgGenDropNonIpMcDropModeSet,
        2, 0},

    {"cpssExMxPmBrgGenDropNonIpMcDropModeGet",
        &wrCpssExMxPmBrgGenDropNonIpMcDropModeGet,
        1, 0},

    {"cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFltrEnSet",
        &wrCpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableSet,
        3, 0},

    {"cpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFltrEnGet",
        &wrCpssExMxPmBrgGenExcludeLLMcFromUnregIpMcFilterEnableGet,
        2, 0},

    /* ---- test:  cpssExMxPmBrgGenRateLimitGlobalCfgSet */
    /* table: cpssExMxPmBrgGenRateLimitGlobalCfg         */
    
    {"cpssExMxPmBrgGenRateLimitGlobalCfgSet",
        &wrCpssExMxPmBrgGenRateLimitGlobalCfgSet,
        1, 7},

    {"cpssExMxPmBrgGenRateLimitGlobalCfgGetFirst",
        &wrCpssExMxPmBrgGenRateLimitGlobalCfgGet,
        1, 0},

    {"cpssExMxPmBrgGenRateLimitGlobalCfgGetNext",
        &wrCpssExMxPmBrgGenRateLimitGlobalCfgGet,
        1, 0},
        
    /* ---- test:  cpssExMxPmBrgGenRateLimitGlobalCfgSet */

    {"cpssExMxPmBrgGenRateLimitGlobalDropCntrSet",
        &wrCpssExMxPmBrgGenRateLimitGlobalDropCntrSet,
        3, 0},

    {"cpssExMxPmBrgGenRateLimitGlobalDropCntrGet",
        &wrCpssExMxPmBrgGenRateLimitGlobalDropCntrGet,
        1, 0},

    /* ---- test:  cpssExMxPmBrgGenPortRateLimitSet */

    {"cpssExMxPmBrgGenPortRateLimitSet",
        &wrCpssExMxPmBrgGenPortRateLimitSet,
        2, 6},

    {"cpssExMxPmBrgGenPortRateLimitGetFirst",
        &wrCpssExMxPmBrgGenPortRateLimitGet,
        2, 0},

    {"cpssExMxPmBrgGenPortRateLimitGetNext",
        &wrCpssExMxPmBrgGenPortRateLimitGet,
        2, 0},
    
    /* ---- test:  cpssExMxPmBrgGenPortRateLimitSet */

    {"cpssExMxPmBrgGenPortRateLimitDropCntrEnableSet",
        &wrCpssExMxPmBrgGenPortRateLimitDropCntrEnableSet,
        3, 0},

    {"cpssExMxPmBrgGenPortRateLimitDropCntrEnableGet",
        &wrCpssExMxPmBrgGenPortRateLimitDropCntrEnableGet,
        2, 0},

    {"cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableSet",
        &wrCpssExMxPmBrgGenDaLookupForMacToMePacketsEnableSet,
        2, 0},

    {"cpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet",
        &wrCpssExMxPmBrgGenDaLookupForMacToMePacketsEnableGet,
        1, 0},

    {"cpssExMxPmBrgGenBpduTrapEnableSet",
         &wrCpssExMxPmBrgGenBpduTrapEnableSet,
         2, 0},

    {"cpssExMxPmBrgGenBpduTrapEnableGet",
         &wrCpssExMxPmBrgGenBpduTrapEnableGet,
         1, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmBrgGen
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
GT_STATUS cmdLibInitCpssExMxPmBrgGen
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


