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
#include <cpss/exMx/exMxTg/bridge/cpssExMxTgBrgGen.h>


/*******************************************************************************
* cpssExMxTgBrgGenEgressArpBcMirrorEnable
*
* DESCRIPTION:
*       Enable/disable brodcast ARP packets mirroring to CPU port
*
* APPLICABLE DEVICES:  Only TIGER devices
*
* INPUTS:
*       dev    - physical device number
*       enable - GT_TRUE - enable the mirroing , GT_FALSE - disable the mirroring
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxTgBrgGenEgressArpBcMirrorEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgBrgGenEgressArpBcMirrorEnable(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxTgBrgGenIfIgmpSnoopModeSet
*
* DESCRIPTION:
*       Sets IGMP snooping lif global command mode.
*
* APPLICABLE DEVICES:  Only TIGER devices
*
* INPUTS:
*       dev  - device number
*       mode - IGMP snooping command mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_NOT_SUPPORTED   - command mode isn't supported
*
* COMMENTS:
*       Write to the IGMP Command field in the InLIF Control Register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxTgBrgGenIfIgmpSnoopModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       dev;
    CPSS_IGMP_SNOOP_MODE_ENT    mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mode = (CPSS_IGMP_SNOOP_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgBrgGenIfIgmpSnoopModeSet(dev, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxTgBrgGenIcmpv6MsgTypeSet
*
* DESCRIPTION:
*     Set trap/mirror/forward mode for specified ICMP message type
*
*
* APPLICABLE DEVICES:  Only TIGER devices
*
* INPUTS:
*        dev       - device number
*        msgType   - ICMP message type
*        packetCmd - trapping/mirroring to CPU mode
*        index     - the index of the message type
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       Write the message type and command to the ICMPv6 Management Registers.
*       in case of forwarding value 0 should be written to the type and command.
*******************************************************************************/
static CMD_STATUS wrcpssExMxTgBrgGenIcmpv6MsgTypeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                   dev;
    GT_U8                                   msgType;
    CPSS_EXMX_BRIDGE_CNTRL_PACKET_MODE_ENT  packetCmd;
    GT_U32                                  index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    msgType = (GT_U8)inArgs[1];
    packetCmd = (CPSS_EXMX_BRIDGE_CNTRL_PACKET_MODE_ENT)inArgs[2];
    index = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxTgBrgGenIcmpv6MsgTypeSet(dev, msgType, packetCmd, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxTgBrgGenEgressArpBcMirrorEnable",
        &wrcpssExMxTgBrgGenEgressArpBcMirrorEnable,
        2, 0},

    {"cpssExMxTgBrgGenIfIgmpSnoopModeSet",
        &wrcpssExMxTgBrgGenIfIgmpSnoopModeSet,
        2, 0},

     {"cpssExMxTgBrgGenIcmpv6MsgTypeSet",
        &wrcpssExMxTgBrgGenIcmpv6MsgTypeSet,
       4, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))


/*******************************************************************************
* cmdLibInitCpssExMxTgBridge
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
GT_STATUS cmdLibInitCpssExMxTgBridge
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}



