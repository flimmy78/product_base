/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxTgTunnelEntry.c
*
* DESCRIPTION:
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
#include <cpss/exMx/exMxTg/tunnel/cpssExMxTgTunnelEntry.h>


/* Tunnel entry Table*/
static GT_BOOL *tunnelEntryValidArr = NULL;
static GT_U32 tunnelEntryValidArrSize = 0;

static GT_U16 tunnelEntryIndex;


/*******************************************************************************
* cpssExMxTgTunnelEntrySet
*
* DESCRIPTION:
*    Sets tunnel  entry. The entry contains the information
*    required to constract the IPV4 tunneling header.
*
* APPLICABLE DEVICES:
*       EX1x6
*
* INPUTS:
*       devNum                  - device number
*       tunnelEntryIndex        - tunnel  entry index. Its range is defined
*                                 under cpss initialization.
*       tunnelEntryPtr          - points to tunnel entry configuration
*
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_BAD_PTR     - on NULL pointer
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       The 98X116/126/136 devices support IPV4 as the tunneling protocol.
*       The following tunnels could be configured:
*       IPV6-over-IPV4 tunneling support - IPv6-over-IPv4 and IPv6-over-GRE/IPv4
*       tunneling. Entry to such tunnels is controled by IPV6 routing engine.
*
*       IPV6 unicast packet entry to tunnel is triggered by IPV6 router engine
*       according to the <OutLIF> bit field in the IPv4/6 Unicast Next Hop info.
*       IPv6 Multicast packet entry to a tunnel is triggered by the router egress
*       according to the <OutLIF> bit field in the IPv4/6 Multicast Linked list
*       entry.
*
*       IPV4-over-IPV4 tunneling support - IPv4-over-IPv4 and IPv4-over-GRE/IPv4
*       tunneling. Entry to such tunnels is controled by IPV4 routing engine.
*
*       IPV4 unicast packet entry to tunnel is triggered by IPV4 router engine
*       according to the <OutLIF> bit field in the IPv4/6 Unicast Next Hop info.
*       IPv4 Multicast packet entry to a tunnel is triggered by the router egress
*       according to the <OutLIF> bit field in the IPv4/6 Multicast Linked list
*       entry.
*
*       The tunnelEntryIndex should be set in Tunnel Output Logical
*       interface as tunnelPointer to this Tunnel Table Entry.
*       (look cpssExMxIpMcLinkListWrite and cpssExMxIpUcRouteEntriesWrite)
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMX_TG_TUNNEL_TABLE_ENTRY_STC tunnelEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /*Manage a deterministic Handles array*/

    cmdOsMemSet(&tunnelEntry, 0, sizeof(CPSS_EXMX_TG_TUNNEL_TABLE_ENTRY_STC));

    if(tunnelEntryValidArr == NULL)
    {
        /* retrieve max num of tunnel entries to allocate tunnelEntryValidArr*/
        tunnelEntryValidArrSize = PRV_CPSS_EXMX_DEV_MODULE_CFG_MAC(devNum)->tunnelCfg.tunnelTblSize/16;

        /* allocate memory for tunnel entries handles */
        tunnelEntryValidArr = cmdOsMalloc(tunnelEntryValidArrSize * sizeof(GT_BOOL));
        cmdOsMemSet(tunnelEntryValidArr, GT_FALSE, tunnelEntryValidArrSize * sizeof(GT_BOOL));
    }

    /* map input arguments to locals */
    tunnelEntryIndex = (GT_U16)inFields[0];
    tunnelEntry.copyDscpAndUp = (CPSS_EXMXTG_TUNNEL_ENTRY_QOS_MARK_MODE_ENT)inFields[1];

    if(inFields[1] == CPSS_EXMXTG_TUNNEL_ENTRY_QOS_MARK_FROM_ENTRY_E)
    {
        tunnelEntry.dscp = inFields[2];
        tunnelEntry.up = inFields[3];
    }

    tunnelEntry.dfFlag = (GT_BOOL)inFields[4];
    tunnelEntry.ipv4TunnelType = (CPSS_EXMXTG_TUNNEL_ENTRY_IPV4_TUNNEL_TYPE_ENT)inFields[5];
    tunnelEntry.ttl = inFields[6];
    tunnelEntry.arpIndex = inFields[7];
    tunnelEntry.isTagged = (GT_BOOL)inFields[8];
    tunnelEntry.vid = (GT_U16)inFields[9];
    tunnelEntry.autoTunnel = (GT_BOOL)inFields[10];
    tunnelEntry.autoTunnelOffset = inFields[11];
    galtisIpAddr(&tunnelEntry.destIp,(GT_U8*)inFields[12]);
    galtisIpAddr(&tunnelEntry.srcIp,(GT_U8*)inFields[13]);

    /* call cpss api function */
    result = cpssExMxTgTunnelEntrySet(devNum, tunnelEntryIndex, &tunnelEntry);

    if (result == GT_OK)
    {
        tunnelEntryValidArr[tunnelEntryIndex] = GT_TRUE;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgTunnelEntryGet
*
* DESCRIPTION:
*    Get tunnel entry. The entry contains the information
*    required to constract the IPV4 tunneling header.
*
* APPLICABLE DEVICES:
*       EX1x6
*
* INPUTS:
*       devNum                  - device number
*       tunnelEntryIndex        - tunnel  entry index. Its range is defined
*                                 under cpss initialization.
*
* OUTPUTS:
*       tunnelEntryPtr          - points to tunnel entry configuration
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_BAD_PTR     - on NULL pointer
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       The 98X116/126/136 devices support IPV4 as the tunneling protocol.
*       The following tunnels could be configured:
*       IPV6-over-IPV4 tunneling support - IPv6-over-IPv4 and IPv6-over-GRE/IPv4
*       tunneling. Entry to such tunnels is controled by IPV6 routing engine.
*
*       IPV6 unicast packet entry to tunnel is triggered by IPV6 router engine
*       according to the OutLIF type  field in the IPv4/6 Unicast Next Hop info.
*       IPv6 Multicast packet entry to a tunnel is triggered by the router egress
*       according to the OutLIF type field in the IPv4/6 Multicast Linked list
*       entry.
*
*       IPV4-over-IPV4 tunneling support - IPv4-over-IPv4 and IPv4-over-GRE/IPv4
*       tunneling. Entry to such tunnels is controled by IPV4 routing engine.
*
*       IPV4 unicast packet entry to tunnel is triggered by IPV4 router engine
*       according to the OutLIF type field in the IPv4/6 Unicast Next Hop info.
*       IPv4 Multicast packet entry to a tunnel is triggered by the router egress
*       according to the OutLIF type field in the IPv4/6 Multicast Linked list
*       entry.
*
*       The tunnelEntryIndex should be set in Tunnel Output Logical
*       interface as tunnelPointer to this Tunnel Table Entry.
*       (look cpssExMxIpMcLinkListWrite and cpssExMxIpUcRouteEntriesWrite)
*******************************************************************************/


/*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMX_TG_TUNNEL_TABLE_ENTRY_STC tunnelEntry;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* make sure teValidArr was allocated */
    if ((tunnelEntryValidArr == NULL))
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* find index of first tunnel entry that was previously set */
    for(; tunnelEntryIndex < tunnelEntryValidArrSize; tunnelEntryIndex++)
    {
        if (tunnelEntryValidArr[tunnelEntryIndex] == GT_TRUE)
            break;
    }

    /* check if we reached end of array */
    if (tunnelEntryIndex == tunnelEntryValidArrSize)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxTgTunnelEntryGet(devNum, tunnelEntryIndex, &tunnelEntry);

    inFields[0] = tunnelEntryIndex;
    inFields[1] = tunnelEntry.copyDscpAndUp;

    if (tunnelEntry.copyDscpAndUp == CPSS_EXMXTG_TUNNEL_ENTRY_QOS_MARK_FROM_TUNNELED_IP_PACKET_E)
    {
        inFields[2] = tunnelEntry.dscp;
        inFields[3] = tunnelEntry.up;
    }

    inFields[4] = tunnelEntry.dfFlag;
    inFields[5] = tunnelEntry.ipv4TunnelType;
    inFields[6] = tunnelEntry.ttl;
    inFields[7] = tunnelEntry.arpIndex;
    inFields[8] = tunnelEntry.isTagged;
    inFields[9] = tunnelEntry.vid;
    inFields[10] = tunnelEntry.autoTunnel;
    inFields[11] = tunnelEntry.autoTunnelOffset;

    tunnelEntryIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%4b%4b",
                inFields[0],  inFields[1], inFields[2],  inFields[3],
                inFields[4],  inFields[5], inFields[6],  inFields[7],
                inFields[8],  inFields[9], inFields[10],  inFields[11],
                tunnelEntry.destIp.arIP, tunnelEntry.srcIp.arIP);

     /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    tunnelEntryIndex=0;

    return wrCpssExMxTgTunnelEntryGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssExMxTgTunnelEntryGet(inArgs,inFields,numFields,outArgs);
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxTgTunnelEntrySet",
         &wrCpssExMxTgTunnelEntrySet,
         1, 14},
        {"cpssExMxTgTunnelEntryGetFirst",
         &wrCpssExMxTgTunnelEntryGetFirst,
         1, 0},
        {"cpssExMxTgTunnelEntryGetNext",
         &wrCpssExMxTgTunnelEntryGetNext,
         1, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxTgTunnelEntry
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
GT_STATUS cmdLibInitCpssExMxTgTunnelEntry
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

