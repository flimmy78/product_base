/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmBrgPrvEdgeVlan.c
*
* DESCRIPTION:
*       Wrapper functions for
*           cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgPrvEdgeVlan.h API's
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
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgPrvEdgeVlan.h>

/*******************************************************************************
* cpssExMxPmBrgPrvEdgeVlanEnable
*
* DESCRIPTION:
*       Enable/disable the Private Edge VLAN on specified device.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE to enable PVE on device.
*                      GT_FALSE to disable PVE on device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on bad devNum
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgPrvEdgeVlanEnable
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
    result = cpssExMxPmBrgPrvEdgeVlanEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgPrvEdgeVlanEnableGet
*
* DESCRIPTION:
*       Get the Private Edge VLAN state on specified device.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to) the PVE state on device.
*                      GT_TRUE - PVE is enabled.
*                      GT_FALSE - PVE is disable.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on bad devNum
*       GT_BAD_PTR   - on NULL pointer parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgPrvEdgeVlanEnableGet
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
    result = cpssExMxPmBrgPrvEdgeVlanEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgPrvEdgeVlanPortConfigSet
*
* DESCRIPTION:
*       Enable/Disable and configure a specified port to operate in PVE mode.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       enable     - GT_TRUE for enabled, GT_FALSE for disabled.
*       pveIfPtr   - (pointer to) PVE Interface structure.
*                    Note this interface type applies only for:
*                    
*                    CPSS_INTERFACE_PORT_E:
*                    pveIfPtr->devPort.portNum - the destination PVE "uplink"  
*                    physical port to which all traffic received on given 
*                    {portNum, devNum} is forwarded. 
*                    pveIfPtr->devPort.devNum - the destination PVE "uplink"  
*                    physical device to which all traffic received on given 
*                    {portNum, devNum} is forwarded.
*                    
*                    CPSS_INTERFACE_TRUNK_E
*                    trunkId - the value of the destination trunkId.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on bad portNum ,devNum or interface info.
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       When mode is disabled parameter pveIfPtr is ignored.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgPrvEdgeVlanPortConfigSet
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
    CPSS_INTERFACE_INFO_STC pveIf;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    portNum=(GT_U8)inFields[0];
    enable=(GT_BOOL)inFields[1];
    pveIf.type=(CPSS_INTERFACE_TYPE_ENT)inFields[2];
    pveIf.devPort.devNum=(GT_U8)inFields[3];
    pveIf.devPort.portNum= (GT_U8)inFields[4];
    pveIf.trunkId=(GT_TRUNK_ID)inFields[5];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(devNum, portNum, enable, &pveIf);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

static GT_U8 portConfigIndex;
/*******************************************************************************
* cpssExMxPmBrgPrvEdgeVlanPortConfigGet
*
* DESCRIPTION:
*       Get a specified port PVE configuration.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE for enabled, GT_FALSE for disabled.
*       pveIfPtr   - (pointer to) PVE Interface structure.
*                    Note this interface type applies only for:
*                    
*                    CPSS_INTERFACE_PORT_E:
*                    pveIfPtr->devPort.portNum - the destination PVE "uplink"  
*                    physical port to which all traffic received on given 
*                    {portNum, devNum} is forwarded. 
*                    pveIfPtr->devPort.devNum - the destination PVE "uplink"  
*                    physical device to which all traffic received on given 
*                    {portNum, devNum} is forwarded.
*                    
*                    CPSS_INTERFACE_TRUNK_E
*                    trunkId - the value of the destination trunkId.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on bad portNum or devNum
*       GT_BAD_PTR    - on NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       When mode is disabled parameter pveIfPtr is ignored.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgPrvEdgeVlanPortConfigGetEntry
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
    CPSS_INTERFACE_INFO_STC pveIf;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
   devNum = (GT_U8)inArgs[0];
   portNum=portConfigIndex;
   
    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgPrvEdgeVlanPortConfigGet(devNum, portNum, &enable, &pveIf);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
   inFields[0]=portNum;
   inFields[1]=enable;
   inFields[2]=pveIf.type;
   inFields[3]=pveIf.devPort.devNum;
   inFields[4]=pveIf.devPort.portNum;
   inFields[5]=pveIf.trunkId;

    fieldOutput("%d%d%d%d%d%d",inFields[0],inFields[1],inFields[2],
                                  inFields[3], inFields[4],inFields[5]); 
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgPrvEdgeVlanPortConfigGet
*
* DESCRIPTION:
*       Get a specified port PVE configuration.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE for enabled, GT_FALSE for disabled.
*       pveIfPtr   - (pointer to) PVE Interface structure.
*                    Note this interface type applies only for:
*                    
*                    CPSS_INTERFACE_PORT_E:
*                    pveIfPtr->devPort.portNum - the destination PVE "uplink"  
*                    physical port to which all traffic received on given 
*                    {portNum, devNum} is forwarded. 
*                    pveIfPtr->devPort.devNum - the destination PVE "uplink"  
*                    physical device to which all traffic received on given 
*                    {portNum, devNum} is forwarded.
*                    
*                    CPSS_INTERFACE_TRUNK_E
*                    trunkId - the value of the destination trunkId.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on bad portNum or devNum
*       GT_BAD_PTR    - on NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       When mode is disabled parameter pveIfPtr is ignored.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgPrvEdgeVlanPortConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     portConfigIndex = 0; /*reset on first*/
     return wrCpssExMxPmBrgPrvEdgeVlanPortConfigGetEntry(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssExMxPmBrgPrvEdgeVlanPortConfigGet
*
* DESCRIPTION:
*       Get a specified port PVE configuration.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE for enabled, GT_FALSE for disabled.
*       pveIfPtr   - (pointer to) PVE Interface structure.
*                    Note this interface type applies only for:
*                    
*                    CPSS_INTERFACE_PORT_E:
*                    pveIfPtr->devPort.portNum - the destination PVE "uplink"  
*                    physical port to which all traffic received on given 
*                    {portNum, devNum} is forwarded. 
*                    pveIfPtr->devPort.devNum - the destination PVE "uplink"  
*                    physical device to which all traffic received on given 
*                    {portNum, devNum} is forwarded.
*                    
*                    CPSS_INTERFACE_TRUNK_E
*                    trunkId - the value of the destination trunkId.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on bad portNum or devNum
*       GT_BAD_PTR    - on NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       When mode is disabled parameter pveIfPtr is ignored.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgPrvEdgeVlanPortConfigGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    portConfigIndex++;/* go to next index */
    return wrCpssExMxPmBrgPrvEdgeVlanPortConfigGetEntry(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet
*
* DESCRIPTION:
*       Enable/Disable per port forwarding control traffic to PVE interface
*       ("PVE Uplink"), or to CPU.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_TRUE  - Control Traffic is forwarded to the PVE interface.   
*                 all packets are assigned FORWARD (no TRAP/MIRROR).
*                 bridge filters are still active, packets assigned HARD DROP 
*                 or SOFT DROP will not be forward to the PVE interface. 
*                 GT_FALSE - Control Traffic is not forwarded to the PVE interface
*                 and TRAP/MIRROR to CPU assigned packets are sent to CPU port.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong devNum, portNum
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       This parameter is ignored if PVE is disabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet
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
    result = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet
*
* DESCRIPTION:
*       Get the current state per Port of forwarding control packets to
*       PVE interface ("PVE Uplink").
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number.
*
* OUTPUTS:
*       enablePtr - GT_TRUE  - Control Traffic is forwarded to the PVE interface.   
*                 all packets are assigned FORWARD (no TRAP/MIRROR).
*                 bridge filters are still active, packets assigned HARD DROP 
*                 or SOFT DROP will not be forward to the PVE interface. 
*                 GT_FALSE - Control Traffic is not forwarded to the PVE interface
*                 and TRAP/MIRROR to CPU assigned packets are sent to CPU port.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong devNum, port
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       This parameter is ignored if PVE is disabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet
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
    result = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
   {"cpssExMxPmBrgPrvEdgeVlanEnable",
    &wrCpssExMxPmBrgPrvEdgeVlanEnable,
    2, 0},
   {"cpssExMxPmBrgPrvEdgeVlanEnableGet",
    &wrCpssExMxPmBrgPrvEdgeVlanEnableGet,
    1, 0},
   {"cpssExMxPmBrgPrvEdgeVlanPortConfigSet",
    &wrCpssExMxPmBrgPrvEdgeVlanPortConfigSet,
    1, 6},
   {"cpssExMxPmBrgPrvEdgeVlanPortConfigGetFirst",
    &wrCpssExMxPmBrgPrvEdgeVlanPortConfigGetFirst,
    1, 0},
   {"cpssExMxPmBrgPrvEdgeVlanPortConfigGetNext",
    &wrCpssExMxPmBrgPrvEdgeVlanPortConfigGetNext,
    1, 0},
   {"cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet",
    &wrCpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet,
    3, 0},
   {"cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet",
    &wrCpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet,
    2, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmBrgPrvEdgeVlan
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
GT_STATUS cmdLibInitCpssExMxPmBrgPrvEdgeVlan
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

