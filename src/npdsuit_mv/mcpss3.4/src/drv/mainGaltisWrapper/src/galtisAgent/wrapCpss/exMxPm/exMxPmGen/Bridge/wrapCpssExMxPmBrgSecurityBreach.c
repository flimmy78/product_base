/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmBrgSecurityBreach.c
*
* DESCRIPTION:
*       Wrapper functions for cpssExMxPmBrgSecurityBreach cpss.exMxPm functions
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
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgSecurityBreach.h>


/*******************************************************************************
* cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableSet   [DB]    [>50]
*
* DESCRIPTION:
*       Enables\Disables Security Breach for Moved Static addresses.
*       When the FDB entry <Static> bit is set to 1, it is not subject to Aging.
*       In addition, there may be cases in which the Port#/Trunk# in this entry
*       doesn't match the Port#/Trunk# from which this packet was received.
*       In this case the moved static address could (or couldn't) to be
*       considered as Security Breach at the specified port.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - device number.
*       port            - port number.
*       enable          - Boolean value:
*                           GT_TRUE  - moved static address is considered as
*                                      Security Breach at the specified port
*                                      and are dropped according to the
*                                      Moved Static Drop Mode (hard or soft).
*                           GT_FALSE - moved static address isn't considered
*                                      as Security Breach at the specified port
*                                      and are forwarded regularly.
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
static CMD_STATUS wrCpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_BOOL             enable;

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
    result = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableSet(devNum,
                                                                 port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet   [DB]    >50
*
* DESCRIPTION:
*       Gets the current working mode(Enables\Disables) of Security Breach
*       for Moved Static addresses.
*       When the FDB entry <Static> bit is set to 1, it is not subject to Aging.
*       In addition, there may be cases in which the Port#/Trunk# in this entry
*       doesn't match the Port#/Trunk# from which this packet was received.
*       In this case the moved static address could (or couldn't) to be
*       considered as Security Breach at the specified port.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - device number.
*       port            - port number.
*
* OUTPUTS:
*       enablePtr       - pointer to the Boolean value:
*                           GT_TRUE  - moved static address is considered as
*                                      Security Breach at the specified port
*                                      and are dropped according to the
*                                      Moved Static Drop Mode (hard or soft).
*                           GT_FALSE - moved static address isn't considered
*                                      as Security Breach at the specified port
*                                      and are forwarded regularly.
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
static CMD_STATUS wrCpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_BOOL             enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet(devNum,
                                                            port, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSecurityBreachEventDropModeSet   [DB]
*
* DESCRIPTION:
*       Sets the Drop Mode for given Security Breach event.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - device number.
*       eventType       - Security Breach event type.
*       dropMode        - soft or hard drop mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum, eventType or dropMode.
*
* COMMENTS:
*       The acceptable events are:
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSecurityBreachEventDropModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_BRG_SECUR_BREACH_EVENTS_ENT  eventType;
    CPSS_DROP_MODE_TYPE_ENT  dropMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    eventType = (CPSS_BRG_SECUR_BREACH_EVENTS_ENT)inArgs[1];
    dropMode = (CPSS_DROP_MODE_TYPE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmBrgSecurityBreachEventDropModeSet(devNum, eventType,
                                                          dropMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSecurityBreachEventDropModeGet   [DB]
*
* DESCRIPTION:
*      Gets Drop Mode for given Security Breach event.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - device number.
*       eventType       - security breach event type.
*
* OUTPUTS:
*       dropModePtr     - pointer to the drop mode: soft or hard.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or eventType.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       The acceptable events are:
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_MAC_SA_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_PORT_NOT_IN_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_VLAN_RANGE_DROP_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_INVALID_VLAN_E,
*           CPSS_BRG_SECUR_BREACH_EVENTS_MOVED_STATIC_E.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSecurityBreachEventDropModeGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_BRG_SECUR_BREACH_EVENTS_ENT  eventType;
    CPSS_DROP_MODE_TYPE_ENT  dropModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    eventType = (CPSS_BRG_SECUR_BREACH_EVENTS_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgSecurityBreachEventDropModeGet(devNum, eventType,
                                                          &dropModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dropModePtr);
    return CMD_OK;
}


/* cpssExMxPmBrgSecurityBreachMsg   table   [DB]   (same as DxCh) */


/*******************************************************************************
* cpssExMxPmBrgSecurityBreachMsgGet
*
* DESCRIPTION:
*       Reads the Security Breach message update.
*       When Security Breach event occurs, a message
*       is locked until CPU reads it.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       msgPtr          - pointer to the security breach message.
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
static CMD_STATUS wrCpssExMxPmBrgSecurityBreachMsgGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           devNum;
    CPSS_BRG_SECUR_BREACH_MSG_STC   sbMsgPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgSecurityBreachMsgGet(devNum, &sbMsgPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[1] = sbMsgPtr.vlan;
    inFields[2] = sbMsgPtr.port;
    inFields[3] = sbMsgPtr.code;


    /* pack and output table fields */
    fieldOutput("%6b%d%d%d", sbMsgPtr.macSa.arEther, 
                inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSecurityBreachMsgEndGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSecurityBreachGlobalDropCntrSet  [DB]
*
* DESCRIPTION:
*       Sets value of Security Breach Global Drop counter.
*       It counts all dropped packets due to security breach event.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - device number.
*       counterValue    - Security Breach Global Drop counter value.
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
static CMD_STATUS wrCpssExMxPmBrgSecurityBreachGlobalDropCntrSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              counterValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    counterValue = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgSecurityBreachGlobalDropCntrSet(devNum,
                                                          counterValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSecurityBreachGlobalDropCntrGet  [DB]
*
* DESCRIPTION:
*       Gets value of Security Breach Global Drop counter.
*       It counts all dropped packets due to security breach event.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       counterValuePtr - pointer to the Security Breach Global Drop counter
*                         value.
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
static CMD_STATUS wrCpssExMxPmBrgSecurityBreachGlobalDropCntrGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              counterValuePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgSecurityBreachGlobalDropCntrGet(devNum,
                                                          &counterValuePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", counterValuePtr);
    return CMD_OK;
}


/* cpssExMxPmBrgSecBrchPortVlanDropCntrMode  table  [DB]  [was >40 chars!] */

/* same as cpssDxChBrgSecurBreachPortVlanDropCntrMode  */
static   CPSS_EXMXPM_BRG_SECUR_BREACH_DROP_COUNT_CFG_STC        cntrCfgPtr;


/*******************************************************************************
* cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet
*
* DESCRIPTION:
*      Sets Port/Fid Security Breach Drop Counter to count Security Breach 
*      Dropped packets based on their ingress port or their assigned FID.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - device number.
*       cntrCfgPtr      - pointer to the Security Breach Port/Fid counter
*                         mode and it's configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or counter configuration parameters.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];


    cntrCfgPtr.dropCntMode = 
                         (CPSS_EXMXPM_BRG_SECUR_BREACH_DROP_COUNT_MODE_ENT)inFields[0];
    cntrCfgPtr.port = (GT_U8)inFields[1];
    cntrCfgPtr.fId = (GT_U16)inFields[2];

    /* call cpss api function */
    result = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet(devNum,
                                                                 &cntrCfgPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet
*
* DESCRIPTION:
*       Gets mode and it's configuration parameters for  
*       Security Breach Port/Fid Drop counter.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       cntrCfgPtr      - pointer to the Security Breach Port/Fid counter
*                         mode and it's configuration parameters.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGet(devNum, &cntrCfgPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = cntrCfgPtr.dropCntMode;
    inFields[1] = cntrCfgPtr.port;
    inFields[2] = cntrCfgPtr.fId;


    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSecurityBreachPortVlanDropCntrModeEndGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgSecurityBreachPortVlanCntrSet    [DB]
*
* DESCRIPTION:
*       Sets value of Security Breach Port/Vlan Drop counter.
*       It counts all packets received according to Port/Vlan Security Breach
*       Drop count mode configuration and dropped due to any security breach
*       event.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - device number.
*       counterValue    - Security Breach Port/Vlan Drop counter value.
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
static CMD_STATUS wrCpssExMxPmBrgSecurityBreachPortVlanCntrSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              counterValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    counterValue = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgSecurityBreachPortVlanCntrSet(devNum, counterValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSecurityBreachPortVlanCntrGet    [DB]
*
* DESCRIPTION:
*       Gets value of Security Breach Port/Vlan Drop counter.
*       It counts all packets received according to Port/Vlan Security Breach
*       Drop count mode configuration and dropped due to any security breach
*       event.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       counterValuePtr - pointer to the Security Breach Port/Vlan Drop counter
*                         value.
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
static CMD_STATUS wrCpssExMxPmBrgSecurityBreachPortVlanCntrGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              counterValuePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgSecurityBreachPortVlanCntrGet(devNum,
                                                         &counterValuePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", counterValuePtr);
    return CMD_OK;
}



/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

    /* ---  >50 chars */
    
    {"cpssExMxPmBrgSecurBreachPortMovedStaticAddrEnblSet",
        &wrCpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableSet,
        3, 0},

    {"cpssExMxPmBrgSecurBreachPortMovedStaticAddrEnblGet",
        &wrCpssExMxPmBrgSecurityBreachPortMovedStaticAddrEnableGet,
        2, 0},

    /* --- end >50 chars */
    
    {"cpssExMxPmBrgSecurityBreachEventDropModeSet",
        &wrCpssExMxPmBrgSecurityBreachEventDropModeSet,
        3, 0},

    {"cpssExMxPmBrgSecurityBreachEventDropModeGet",
        &wrCpssExMxPmBrgSecurityBreachEventDropModeGet,
        2, 0},

    /* --- cpssExMxPmBrgSecurityBreachMsg  table */
    
    {"cpssExMxPmBrgSecurityBreachMsgGetFirst",
        &wrCpssExMxPmBrgSecurityBreachMsgGet,
        1, 0},

    {"cpssExMxPmBrgSecurityBreachMsgGetNext",
        &wrCpssExMxPmBrgSecurityBreachMsgEndGet,
        1, 0},

    /* --- cpssExMxPmBrgSecurityBreachMsg  table */
    
    {"cpssExMxPmBrgSecurityBreachGlobalDropCntrSet",
        &wrCpssExMxPmBrgSecurityBreachGlobalDropCntrSet,
        2, 0},

    {"cpssExMxPmBrgSecurityBreachGlobalDropCntrGet",
        &wrCpssExMxPmBrgSecurityBreachGlobalDropCntrGet,
        1, 0},

    /* --- cpssExMxPmBrgSecBrchPortVlanDropCntrMode  table (was >40 chars !) */

    {"cpssExMxPmBrgSecBrchPortVlanDropCntrModeSet",
        &wrCpssExMxPmBrgSecurityBreachPortVlanDropCntrModeSet,
        1, 3},

    {"cpssExMxPmBrgSecBrchPortVlanDropCntrModeGetFirst",
        &wrCpssExMxPmBrgSecurityBreachPortVlanDropCntrModeGetFirst,
        1, 0},

    {"cpssExMxPmBrgSecBrchPortVlanDropCntrModeGetNext",
        &wrCpssExMxPmBrgSecurityBreachPortVlanDropCntrModeEndGet,
        1, 0},

    /* --- cpssExMxPmBrgSecBrchPortVlanDropCntrMode  table */
    
    {"cpssExMxPmBrgSecurityBreachPortVlanCntrSet",
        &wrCpssExMxPmBrgSecurityBreachPortVlanCntrSet,
        2, 0},

    {"cpssExMxPmBrgSecurityBreachPortVlanCntrGet",
        &wrCpssExMxPmBrgSecurityBreachPortVlanCntrGet,
        1, 0},

    /* CMD_END */

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmBrgSecurityBreach
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
GT_STATUS cmdLibInitCpssExMxPmBrgSecurityBreach
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


