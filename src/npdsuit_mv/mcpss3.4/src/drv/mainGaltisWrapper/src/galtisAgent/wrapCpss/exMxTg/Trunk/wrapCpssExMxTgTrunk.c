
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
#include <cpss/exMx/exMxTg/trunk/cpssExMxTgTrunk.h>
#include <cpss/exMx/exMxGen/trunk/cpssExMxTrunk.h>



/*******************************************************************************
* cpssExMxTgTrunkHashIpv6ModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the hash generation function for Ipv6 packets.
*
* APPLICABLE DEVICES:   Only Ex1x6 devices
*
* INPUTS:
*       devNum     - the device number.
*       hashMode   - the Ipv6 hash mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or hash mode
*
* COMMENTS:
*       none
*******************************************************************************/
CMD_STATUS wrCpssExMxTgTrunkHashIpv6ModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                               devNum;
    CPSS_EXMX_TRUNK_IPV6_HASH_MODE_ENT  hashMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    hashMode = (CPSS_EXMX_TRUNK_IPV6_HASH_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgTrunkHashIpv6ModeSet(devNum, hashMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* Function: cpssExMxTgTrunkHashMaskParametersSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       The function set masks for the parameters used in the hash calculation
*       (in HW) of trunk related tables
*
* APPLICABLE DEVICES:   Only Ex1x6 devices
*
* INPUTS:
*       devNum         - the device number.
*       trunkMaskInfoPtr - (Pointer to) the info about all the trunk's mask
*                          hash fields
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*       Write values for masks to HW :
*       SA,DA - 7 lsb
*       IPv4-SA , IPv4-DA - 7 lsb in bytes[1,3]
*       IPv6-SA , IPv6-DA - 7 lsb in bytes[1,3]  in LSW (least significant word)
*       IPv6-flow lable -- 7 lsb
*       TCP source port, destination port - 7lsb in each byte (2 bytes)
*       UDP source port, destination port - 7lsb in each byte (2 bytes)
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgTrunkHashMaskParametersSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                     devNum;
    CPSS_TRUNK_EXMX_MASK_HASH_PARAM_INFO_STC  trunkMaskInfoPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    galtisMacAddr(&trunkMaskInfoPtr.saMacMask, (GT_U8*)inFields[0]);
    galtisMacAddr(&trunkMaskInfoPtr.daMacMask, (GT_U8*)inFields[1]);
    galtisIpAddr(&trunkMaskInfoPtr.daIpv4Mask, (GT_U8*)inFields[2]);
    galtisIpAddr(&trunkMaskInfoPtr.saIpv4Mask, (GT_U8*)inFields[3]);
    galtisIpv6Addr(&trunkMaskInfoPtr.daIpv6Mask, (GT_U8*)inFields[4]);
    galtisIpv6Addr(&trunkMaskInfoPtr.saIpv6Mask, (GT_U8*)inFields[5]);
    trunkMaskInfoPtr.flowLableIpv6Mask = (GT_U32)inFields[6];
    trunkMaskInfoPtr.sPortTcpMask = (GT_U16)inFields[7];
    trunkMaskInfoPtr.dPortTcpMask = (GT_U16)inFields[8];
    trunkMaskInfoPtr.sPortUdpMask = (GT_U16)inFields[9];
    trunkMaskInfoPtr.dPortUdpMask = (GT_U16)inFields[10];

    /* call cpss api function */
    result = cpssExMxTgTrunkHashMaskParametersSet(devNum, &trunkMaskInfoPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}





/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxTgTrunkHashIpv6ModeSet",
        &wrCpssExMxTgTrunkHashIpv6ModeSet,
        2, 0},

    {"cpssExMxTgTrunkHashMaskParametersSet",
        &wrCpssExMxTgTrunkHashMaskParametersSet,
        1, 11}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxTgTrunk
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
GT_STATUS cmdLibInitCpssExMxTgTrunk
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


