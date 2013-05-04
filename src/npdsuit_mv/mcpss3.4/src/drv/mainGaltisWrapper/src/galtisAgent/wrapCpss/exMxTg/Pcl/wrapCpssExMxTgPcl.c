/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapTgBridgeCpss.c
*
* DESCRIPTION:
*       Wrapper functions for TgBridge cpss functions.
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
#include <cpss/exMx/exMxTg/pcl/cpssExMxTgPcl.h>
#include <cpss/exMx/exMxGen/policy/cpssExMxPolicy.h>
#include <cpss/exMx/exMxTg/ip/cpssExMxTgIp.h>

/*******************************************************************************
* cpssExMxTgPclProfileSizeSet
*
* DESCRIPTION:
*   The function sets the size of profile
*
*
* INPUTS:
*       dev             - device number
*       profileSize     - profile size. 8, 4 or 1 template per profile
*
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgPclProfileSizeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                             dev;
    CPSS_EXMX_TG_PCL_PROFILE_SIZE_ENT profileSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    profileSize = (CPSS_EXMX_TG_PCL_PROFILE_SIZE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgPclProfileSizeSet(dev, profileSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgPclLookupKeySizeSelectModeSet
*
* DESCRIPTION:
*   The function configures selection of Policy key size.
*   It possible to force Policy to produce standard Policy keys
*   regardless the IN-LIF settings per packet type. Or use InLif setting
*   to define key size.
*
*
* INPUTS:
*       dev           - device number
*       pktType       - packet type
*       mode          - key selection mode
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgPclLookupKeySizeSelectModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                dev;
    CPSS_EXMX_PCL_PKT_TYPE_ENT           pktType;
    CPSS_EXMX_TG_PCL_KEY_SIZE_SELECT_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    pktType = (CPSS_EXMX_PCL_PKT_TYPE_ENT)inArgs[1];
    mode = (CPSS_EXMX_TG_PCL_KEY_SIZE_SELECT_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxTgPclLookupKeySizeSelectModeSet(dev, pktType, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgPclL3OffsetsModeSet
*
* DESCRIPTION:
*       Set L3 offset calculation mode for user defined bytes.
*
* INPUTS:
*       dev         - device Number
*       mode        - L3 offset calculation mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgPclL3OffsetsModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                dev;
    CPSS_EXMX_TG_PCL_L3_OFFSETS_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mode = (CPSS_EXMX_TG_PCL_L3_OFFSETS_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgPclL3OffsetsModeSet(dev, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgPclRedirectMtuCfgSet
*
* DESCRIPTION:
*       Set behavior of packet, redirected by Policy to outlif
*       with size exceeded MTU.
*
* INPUTS:
*       dev       - device Number
*       exceedCmd - command for packet with size > mtu
*       mtu       - maximal packet size in bytes
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgPclRedirectMtuCfgSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                dev;
    CPSS_EXMX_POLICY_MTU_EXCEED_CMD_ENT  exceedCmd;
    GT_U32                               mtu;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    exceedCmd = (CPSS_EXMX_POLICY_MTU_EXCEED_CMD_ENT)inArgs[1];
    mtu = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPolicyRedirectMtuSet(dev, mtu);
    if (result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    result = cpssExMxPolicyRedirectMtuExceedCmdSet(dev, exceedCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgPclIpv6KeyDataSelectSet
*
* DESCRIPTION:
*       Set format of the extender key of IPV6 packets .
*
* INPUTS:
*       dev         - device Number
*       keyDataType - key data selection type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgPclIpv6KeyDataSelectSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                           dev;
    CPSS_EXMX_TG_PCL_IPV6_KEY_DATA_SELECT_TYPE_ENT  keyDataType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    keyDataType = (CPSS_EXMX_TG_PCL_IPV6_KEY_DATA_SELECT_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgPclIpv6KeyDataSelectSet(dev, keyDataType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgPclDefaultLookupActionIndexSet
*
* DESCRIPTION:
*   The the index for action that will be selected for TCAM lookup
*   that not matched any rule, if another lookup matched some rule.
*
*
* INPUTS:
*       dev             - device number
*       actionIndex     - number of Action in action table
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgPclDefaultLookupActionIndexSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U32  actionIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    actionIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgPclDefaultLookupActionIndexSet(dev, actionIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

    {"cpssExMxTgPclProfileSizeSet",
        &wrCpssExMxTgPclProfileSizeSet,
        2, 0},

    {"cpssExMxTgPclLookupKeySizeSelectModeSet",
        &wrCpssExMxTgPclLookupKeySizeSelectModeSet,
        3, 0},

    {"cpssExMxTgPclL3OffsetsModeSet",
        &wrCpssExMxTgPclL3OffsetsModeSet,
        2, 0},

    {"cpssExMxTgPclRedirectMtuCfgSet",
        &wrCpssExMxTgPclRedirectMtuCfgSet,
        3, 0},

    {"cpssExMxTgPclIpv6KeyDataSelectSet",
        &wrCpssExMxTgPclIpv6KeyDataSelectSet,
        2, 0},

    {"cpssExMxTgPclDefaultLookupActionIndexSet",
        &wrCpssExMxTgPclDefaultLookupActionIndexSet,
        2, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxTgPcl
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
GT_STATUS cmdLibInitCpssExMxTgPcl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


