/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmBrgNestVlan.c
*
* DESCRIPTION:
*       Wrapper functions for cpssExMxPmBrgNestVlan cpss.exMxPm functions
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
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgNestVlan.h>


/*******************************************************************************
* cpssExMxPmBrgNestVlanAccessPortEnableSet  [DB]
*
* DESCRIPTION:
*       Configure given port as nested VLAN access port.
*       The VID of all the packets received on a nested VLAN access port is
*       discarded and they are assigned with the Port VID.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum  - device number.
*       portNum - port number, or CPU port
*       enable  -  GT_TRUE  - port is nested VLAN Access Port.
*                  GT_FALSE - port is Core or Customer port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or portNum.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgNestVlanAccessPortEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               portNum;
    GT_BOOL             enable;

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
    result = cpssExMxPmBrgNestVlanAccessPortEnableSet(devNum, portNum,
                                                       enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgNestVlanAccessPortEnableGet  [DB]
*
* DESCRIPTION:
*       Gets the configuration of the given port (Nested VLAN access port
*       or Core/Customer port).
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number, or CPU port
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                       GT_TRUE  - port is nested VLAN Access Port.
*                       GT_FALSE - port is Core or Customer port.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or portNum values.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgNestVlanAccessPortEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               portNum;
    GT_BOOL             enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgNestVlanAccessPortEnableGet(devNum, portNum,
                                                       &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet    [DB]
*
* DESCRIPTION:
*       Sets selected ingress port VLAN EtherType.
*       This ingVlanSel value indicates which VLAN register is used for VLAN
*       EtherType identification and VLAN range filtering.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - port number, or CPU port
*      ingressVlanSel  - selected port VLAN EtherType.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or portNum or ingressVlanSel values.
*
* COMMENTS:
*       For most applications Ingress & Egress Ethertypes should be the same.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               portNum;
    CPSS_ETHER_MODE_ENT  ingressVlanSel;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ingressVlanSel = (CPSS_ETHER_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet(devNum, portNum,
                                                             ingressVlanSel);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet    [DB]
*
* DESCRIPTION:
*       Gets selected ingress port VLAN EtherType
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum              - device number.
*       portNum             - port number, or CPU port
*
* OUTPUTS:
*       ingressVlanSel   - (pointer to) selected port VLAN EtherType.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or portNum values.
*
* COMMENTS:
*       For most applications Ingress & Egress Ethertypes should be the same.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               portNum;
    CPSS_ETHER_MODE_ENT  ingressVlanSel;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet(devNum, portNum,
                                                                 &ingressVlanSel);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ingressVlanSel);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet [DB]
*
* DESCRIPTION:
*       Sets selected VLAN EtherType for tagged packets transmitted
*       via this egress port.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - port number, or CPU port
*       egressVlanSel   - selected port VLAN EtherType.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or portNum or ingressVlanSel values.
*
* COMMENTS:
*       For most applications Ingress & Egress Ethertypes should be the same.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               portNum;
    CPSS_ETHER_MODE_ENT  egressVlanSel;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    egressVlanSel = (CPSS_ETHER_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet(devNum, portNum,
                                                                egressVlanSel);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet   [DB]
*
* DESCRIPTION:
*       Gets selected egress port VLAN EtherType.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum              - device number.
*       portNum             - port number, or CPU port
*
* OUTPUTS:
*       egressVlanSelPtr   - (pointer to) selected port VLAN EtherType.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or portNum values.
*
* COMMENTS:
*       For most applications Ingress & Egress Ethertypes should be the same.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               portNum;
    CPSS_ETHER_MODE_ENT  egressVlanSelPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet(devNum, portNum,
                                                            &egressVlanSelPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", egressVlanSelPtr);
    return CMD_OK;
}




/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

    {"cpssExMxPmBrgNestVlanAccessPortEnableSet",
        &wrCpssExMxPmBrgNestVlanAccessPortEnableSet,
        3, 0},

    {"cpssExMxPmBrgNestVlanAccessPortEnableGet",
        &wrCpssExMxPmBrgNestVlanAccessPortEnableGet,
        2, 0},

    {"cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet",
        &wrCpssExMxPmBrgNestVlanPortIngressEtherTypeSelectSet,
        3, 0},

    {"cpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet",
        &wrCpssExMxPmBrgNestVlanPortIngressEtherTypeSelectGet,
        2, 0},

    {"cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet",
        &wrCpssExMxPmBrgNestVlanPortEgressEtherTypeSelectSet,
        3, 0},

    {"cpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet",
        &wrCpssExMxPmBrgNestVlanPortEgressEtherTypeSelectGet,
        2, 0},

    /* CMD_END */

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmBrgNestVlan
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
GT_STATUS cmdLibInitCpssExMxPmBrgNestVlan
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


