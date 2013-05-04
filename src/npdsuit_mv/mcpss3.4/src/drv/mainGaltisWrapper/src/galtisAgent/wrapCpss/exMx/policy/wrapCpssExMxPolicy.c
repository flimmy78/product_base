
/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPolicy.c
*
* DESCRIPTION:
*       Wrapper functions for Policy cpss functions.
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
#include <cpss/exMx/exMxGen/policy/cpssExMxPolicy.h>

/*******************************************************************************
* cpssExMxPolicyLookupModeSet
*
* DESCRIPTION:
*      Set ingress Policy lookup mode.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - packet processor number.
*       mode  - defines the classification mechanisms utilized by the
*               Ingress Policy engine.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number, on wrong policy lookup mode.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicyLookupModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                           result;

    GT_U8                               devNum;
	CPSS_EXMX_POLICY_LOOKUP_MODE_ENT    mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
	mode = (CPSS_EXMX_POLICY_LOOKUP_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicyLookupModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicyCosParamModifyEnable
*
* DESCRIPTION:
*       Enable/Disable changing the CoS parameter according to CoS tables
*       in the marking and re-marking stages.
*
*       APPLICABLE DEVICES: All ExMx devices exclude 98EX1x6.
*
* INPUTS:
*       dev      - packet processor number.
*       cosParam - Class-of-Service parameter, which would be enabled/ disabled
*                  for modification.
*
*       enable   - GT_TRUE  - Modify the CoS Parameter.
*                  GT_FALSE -  Do not modify the CoS Parameter.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device number, on wrong cosParam.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicyCosParamModifyEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                           result;

    GT_U8                               devNum;
	CPSS_COS_PARAM_ENT                  cosParam;
	GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
	cosParam = (CPSS_COS_PARAM_ENT)inArgs[1];
	enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPolicyCosParamModifyEnable(devNum, cosParam, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicyRstFinModeSet
*
* DESCRIPTION:
*      Set mode for handling of TCP packets that have RST or FIN flag bits set.
*
*       APPLICABLE DEVICES: All ExMx devices.
*
* INPUTS:
*       dev    - packet processor number.
*       mode   - mode for handling TCP packets that have RST/FIN flag bits set.
*                NOTE: CPSS_EXMX_POLICY_RSTFIN_MODE_PASS_TO_EPI_E not supported
*                      in 98MX6x5/8, 98EX1x6 .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device number, on wrong mode.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicyRstFinModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                           result;

    GT_U8                               devNum;
	CPSS_EXMX_POLICY_RST_FIN_MODE_ENT   mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
	mode = (CPSS_EXMX_POLICY_RST_FIN_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicyRstFinModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
*  cpssExMxPolicyRedirectMtuExceedCmdSet
*
* DESCRIPTION:
*       Set MTU Exceed Command.
*       Packets that are redirected by the policy engine to an OutLIF and exceed
*       the MTU value are subject to the <MTU Exceed Command>.
*
*       APPLICABLE DEVICES: 98EX1x6.
*
* INPUTS:
*       dev         - packet processor number.
*       exceedCmd   - command for packet with size > mtu
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number, on wrong exceedCmd.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicyRedirectMtuExceedCmdSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                               result;

    GT_U8                                   devNum;
	CPSS_EXMX_POLICY_MTU_EXCEED_CMD_ENT     exceedCmd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
	exceedCmd = (CPSS_EXMX_POLICY_MTU_EXCEED_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicyRedirectMtuExceedCmdSet(devNum, exceedCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
*  cpssExMxPolicyRedirectMtuSet
*
* DESCRIPTION:
*       Set the maximum transfer unit (MTU) for packets that are redirected
*       by the Policy Engine to an OutLIF (i.e. Flow based switching).
*
*       APPLICABLE DEVICES: All ExMx devices.
*
* INPUTS:
*       dev         - packet processor number.
*       mtu         - the maximum transfer unit in bytes.(0..65535)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number, on wrong mtu.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicyRedirectMtuSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                               result;

    GT_U8                                   devNum;
	GT_U32                                  mtu;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
	mtu = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicyRedirectMtuSet(devNum, mtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
*  cpssExMxPolicyTrapTcSet
*
* DESCRIPTION:
*       Set the traffic class assigned to packets trapped to the CPU by the
*       policy or the traffic-policing engines.
*
*       APPLICABLE DEVICES: All ExMx devices.
*
* INPUTS:
*       dev       - packet processor number.
*       trafClass - the trap packets traffic class.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number, on wrong traffic class.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicyTrapTcSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_STATUS                               result;

    GT_U8                                   devNum;
	GT_U8                                   trafClass;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
	trafClass = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicyTrapTcSet(devNum, trafClass);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxPolicyLookupModeSet",
        &wrCpssExMxPolicyLookupModeSet,
        2, 0},

    {"cpssExMxPolicyCosParamModifyEnable",
        &wrCpssExMxPolicyCosParamModifyEnable,
        3, 0},

    {"cpssExMxPolicyRstFinModeSet",
        &wrCpssExMxPolicyRstFinModeSet,
        2, 0},

    {"cpssExMxPolicyRedirectMtuExceedCmdSet",
        &wrCpssExMxPolicyRedirectMtuExceedCmdSet,
        2, 0},

    {"cpssExMxPolicyRedirectMtuSet",
        &wrCpssExMxPolicyRedirectMtuSet,
        2, 0},

    {"cpssExMxPolicyTrapTcSet",
        &wrCpssExMxPolicyTrapTcSet,
        2, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPolicy
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
GT_STATUS cmdLibInitCpssExMxPolicy
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}




