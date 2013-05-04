/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmNstPort.c
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
#include <cpss/exMxPm/exMxPmGen/nst/cpssExMxPmNstPort.h>

/*******************************************************************************
* cpssExMxPmNstPortIngressFrwFilterEnableSet
*
* DESCRIPTION:
*     Set port ingress forwarding filter.
*     For a given ingress port Enable/Disable traffic if it is destined to:
*     CPU, ingress analyzer, network.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - physical device number
*       port        - port number
*       filterType  - ingress filter type
*       enable      - GT_TRUE - enable ingress forwarding restriction according
*                     to filterType, filtered packets will be dropped.
*                   - GT_FALSE - disable ingress forwarding restriction
*                     according to filterType
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, port, filterType
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNstPortIngressFrwFilterEnableSet
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
    CPSS_NST_INGRESS_FRW_FILTER_ENT filterType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    filterType = (CPSS_NST_INGRESS_FRW_FILTER_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmNstPortIngressFrwFilterEnableSet(devNum, port, filterType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstPortIngressFrwFilterEnableGet
*
* DESCRIPTION:
*     Get port ingress forwarding filter.
*     For a given ingress port Enable/Disable traffic if it is destined to:
*     CPU, ingress analyzer, network.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - physical device number
*       port        - port number
*       filterType  - ingress filter type
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable ingress forwarding restriction according
*                     to filterType, filtered packets will be dropped.
*                   - GT_FALSE - disable ingress forwarding restriction
*                     according to filterType
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, port, filterType
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on NULL pointer parameter value
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNstPortIngressFrwFilterEnableGet
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
    CPSS_NST_INGRESS_FRW_FILTER_ENT filterType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    filterType = (CPSS_NST_INGRESS_FRW_FILTER_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmNstPortIngressFrwFilterEnableGet(devNum, port, filterType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstBrgIngressFrwFilterDropCntrGet
*
* DESCRIPTION:
*     Reads the global ingress forwarding restriction drop packet counter.
*     This counter counts the number of packets dropped due to Ingress forward
*     restrictions.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - physical device number
*
* OUTPUTS:
*      ingressCntrPtr - the counter value
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on NULL pointer parameter value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNstBrgIngressFrwFilterDropCntrGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ingressCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmNstBrgIngressFrwFilterDropCntrGet(devNum, &ingressCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ingressCntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstPortEgressFrwFilterEnableSet
*
* DESCRIPTION:
*     Set port egress forwarding filter.
*     For a given egress port Enable/Disable traffic if the packet was:
*     sent from CPU with FROM_CPU DSA tag, bridged or policy switched,
*     routed or policy routed.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - physical device number
*       port        - port number (or CPSS_CPU_PORT_NUM_CNS - CPU port)
*       filterType  - egress filter type
*       enable      - GT_TRUE - enable egress forwarding restriction according
*                     to filterType, filtered packets will be dropped.
*                   - GT_FALSE - disable egress forwarding restriction
*                     according to filterType.
* OUTPUTS:
*      None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, port, filterType
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNstPortEgressFrwFilterEnableSet
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
    CPSS_NST_EGRESS_FRW_FILTER_ENT filterType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    filterType = (CPSS_NST_EGRESS_FRW_FILTER_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmNstPortEgressFrwFilterEnableSet(devNum, port, filterType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstPortEgressFrwFilterEnableGet
*
* DESCRIPTION:
*     Get port egress forwarding filter.
*     For a given egress port Enable/Disable traffic if the packet was:
*     sent from CPU with FROM_CPU DSA tag, bridged or policy switched,
*     routed or policy routed.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - physical device number
*       port        - port number (or CPSS_CPU_PORT_NUM_CNS - CPU port)
*       filterType  - egress filter type
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable egress forwarding restriction according
*                     to filterType, filtered packets will be dropped.
*                   - GT_FALSE - disable egress forwarding restriction
*                     according to filterType.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong devNum, port, filterType
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on NULL pointer parameter value
*
* COMMENTS:
*      None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNstPortEgressFrwFilterEnableGet
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
    CPSS_NST_EGRESS_FRW_FILTER_ENT filterType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    filterType = (CPSS_NST_EGRESS_FRW_FILTER_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmNstPortEgressFrwFilterEnableGet(devNum, port, filterType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmNstPortIngressFrwFilterEnableSet",
         &wrCpssExMxPmNstPortIngressFrwFilterEnableSet,
         4, 0},
        {"cpssExMxPmNstPortIngressFrwFilterEnableGet",
         &wrCpssExMxPmNstPortIngressFrwFilterEnableGet,
         3, 0},
        {"cpssExMxPmNstBrgIngressFrwFilterDropCntrGet",
         &wrCpssExMxPmNstBrgIngressFrwFilterDropCntrGet,
         1, 0},
        {"cpssExMxPmNstPortEgressFrwFilterEnableSet",
         &wrCpssExMxPmNstPortEgressFrwFilterEnableSet,
         4, 0},
        {"cpssExMxPmNstPortEgressFrwFilterEnableGet",
         &wrCpssExMxPmNstPortEgressFrwFilterEnableGet,
         3, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmNstPort
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
GT_STATUS cmdLibInitCpssExMxPmNstPort
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

