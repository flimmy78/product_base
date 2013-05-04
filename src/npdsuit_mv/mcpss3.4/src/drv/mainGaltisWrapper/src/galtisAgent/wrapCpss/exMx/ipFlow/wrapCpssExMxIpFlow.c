/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxIpFlow.c
*
* DESCRIPTION:
*       Wrapper functions for Ip Flow cpss functions.
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
#include <cpss/exMx/exMxGen/ipFlow/cpssExMxIpFlow.h>





/*********** cpssExMxIpFlow Table *****************/

#define MAX_NUM_OF_IPFLOW_ENTRIES 24   /*8 TCP 8 UDP 8 IP*/
static GT_U32 gIpFlowEntryIndex;
/*******************************************************************************
* cpssExMxIpFlowEntrySet
*
* DESCRIPTION:
*    This function sets IP Flow Entry in Policy Action Table
*    for the TCP/UDP port / IP protocol Flow.
*    IP flow classification allows IPv4/IPv6 packets to be classified according
*    to one of the following Layer 3/4 header fields:
*    UDP port (source or destination): Packet is compared to a list of ports.
*    TCP port (source or destination): Packet is compared to a list of ports.
*    IP Header Protocol ID: Packet is compared to a list of protocols.
*    When a packet matches the defined classification criteria, the packet is
*    forwarded according to IP Flow Action.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev         - packet processor number.
*       entryType   - entry type in IP flow table.
*       entryIndex  - index in entryType entry  of IP flow table (0..7)
*       portProt    - port number (when UDP/TCP port is matched) /
*                     protocol number (when IP Protocol is matched)
*                     to configure in IP flow table.
*       actionPtr   - defines the action for this port / protocol.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device number, entryType, entryIndex.
*       GT_NOT_SUPPORTED - this request isn't supported.
*
* COMMENTS:
*       The EX and MX devices support up to 8 IP and 8 UDP and 8 TCP protocol
*       IP Flow Classifier entries.
*       Actions are used for 98MX6x8, 98MX6x5 devices:
*            Packet command, CoS Marking Command, DSCP, Mirror to CPU,
*            Policer, Traffic Class, User Priority, Drop Precedence.
*            Policer and CoS Marking Command Mark by entry are mutual exclusive.
*
*       Actions are used for 98EX1x5 devices:
*            Packet command, CoS Marking Command, DSCP, Mirror to CPU,
*            Policer, Traffic Class, User Priority, Drop Precedence,
*            Flow Redirection to Output Logical Interface or Route Entry Pointer,
*            Set LBI, LBI.
*            Policer and CoS Marking Command Mark by entry are mutual exclusive.
*
*       Actions are used for 98EX1x6 devices:
*            All
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpFlowEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                dev;
    CPSS_EXMX_IP_FLOW_ENTRY_TYPE_ENT     entryType;
    GT_U32                               entryIndex;
    GT_U16                               portProt;
    CPSS_EXMX_POLICY_ACTION_STC          action;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    entryIndex = (GT_U32)inFields[0];
    entryType = (CPSS_EXMX_IP_FLOW_ENTRY_TYPE_ENT)inFields[1];
    portProt = (GT_U16)inFields[2];
    action.pktCmd = (CPSS_EXMX_POLICY_ACTION_PKT_CMD_ENT)inFields[3];
    action.mirror.mirrorToCpu = (GT_BOOL)inFields[4];
    action.mirror.mirrorToRxAnalyzerPort = (GT_BOOL)inFields[5];
    action.qos.markCmd = (CPSS_EXMX_POLICY_ACTION_MARK_CMD_ENT)inFields[6];
    action.qos.markDscp = (GT_BOOL)inFields[7];
    action.qos.dscp = (GT_U8)inFields[8];
    action.qos.markTc = (GT_BOOL)inFields[9];
    action.qos.tc = (GT_U8)inFields[10];
    action.qos.markDp = (GT_BOOL)inFields[11];
    action.qos.dp = (GT_U8)inFields[12];
    action.qos.markUp = (GT_BOOL)inFields[13];
    action.qos.up = (GT_U8)inFields[14];
    action.policer.policerMode = (CPSS_EXMX_POLICY_ACTION_POLICER_MODE_ENT)inFields[15];
    action.policer.policerId = (GT_U32)inFields[16];
    action.fa.setLbi = (GT_BOOL)inFields[17];
    action.fa.lbi = (GT_U8)inFields[18];
    action.fa.flowId = (GT_U32)inFields[19];
    action.vlan.modifyVlan = (CPSS_EXMX_POLICY_ACTION_VLAN_CMD_ENT)inFields[20];
    action.vlan.nestedVlan = (GT_BOOL)inFields[21];
    action.vlan.vlanId = (GT_U16)inFields[22];

    action.redirect.redirectCmd = (CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_ENT)inFields[23];
    action.redirect.redirectVid = (GT_U16)inFields[24];

    if(action.redirect.redirectCmd == CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_OUT_IF_E)
    {
        action.redirect.redirectData.outInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[25];
        action.redirect.redirectData.outInterface.devPort.devNum = (GT_U8)inFields[26];
        action.redirect.redirectData.outInterface.devPort.portNum = (GT_U8)inFields[27];
        action.redirect.redirectData.outInterface.trunkId = (GT_TRUNK_ID)inFields[28];
        action.redirect.redirectData.outInterface.vidx = (GT_U16)inFields[29];
        action.redirect.redirectData.outInterface.vlanId = (GT_U16)inFields[30];
    }
    else if(action.redirect.redirectCmd == CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_ROUTER_E)
        action.redirect.redirectData.routeEntryPointer = (GT_U32)inFields[31];

    else if(action.redirect.redirectCmd == CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_MPLS_E)
        action.redirect.redirectData.mplsNhlfePointer = (GT_U32)inFields[32];

    /* call cpss api function */
    result = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpFlowEntryGet
*
* DESCRIPTION:
*    This function gets IP Flow Entry for the TCP/UDP port / IP protocol Flow.
*    IP flow classification allows IPv4/IPv6 packets to be classified according
*    to one of the following Layer 3/4 header fields:
*    UDP port (source or destination): Packet is compared to a list of ports.
*    TCP port (source or destination): Packet is compared to a list of ports.
*    IP Header Protocol ID: Packet is compared to a list of protocols.
*    When a packet matches the defined classification criteria, the packet is
*    forwarded according to IP Flow Action.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev         - packet processor number.
*       entryType   - entry type in IP flow table.
*       entryIndex  - index in entryType entry  of IP flow table (0..7)
*
* OUTPUTS:
*       portProtPtr - port number (when UDP/TCP port is matched) /
*                     protocol number (when IP Protocol is matched)
*                     that is configured in IP flow table.
*       actionPtr   - the action for this port / protocol
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device, entryIndex.
*       GT_NOT_SUPPORTED - this request isn't supported.
*
* COMMENTS:
*       The EX and MX devices support up to 8 IP and 8 UDP and 8 TCP protocol
*       IP Flow Classifier entries.
*       Actions are used for 98MX6x8, 98MX6x5 devices:
*            Packet command, CoS Marking Command, DSCP, Mirror to CPU,
*            Policer, Traffic Class, User Priority, Drop Precedence.
*            Policer and CoS Marking Command Mark by entry are mutual exclusive.
*
*       Actions are used for 98EX1x5 devices:
*            Packet command, CoS Marking Command, DSCP, Mirror to CPU,
*            Policer, Traffic Class, User Priority, Drop Precedence,
*            Flow Redirection to Output Logical Interface or Route Entry Pointer,
*            Set LBI, LBI.
*            Policer and CoS Marking Command Mark by entry are mutual exclusive.
*
*       Actions are used for 98EX1x6 devices:
*            All
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpFlowEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32    realIndex;

    GT_U8                               dev;
    CPSS_EXMX_IP_FLOW_ENTRY_TYPE_ENT    entryType;
    GT_U16                              portProt;
    CPSS_EXMX_POLICY_ACTION_STC         action;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    gIpFlowEntryIndex = 0;/*reset on first*/

    entryType = (CPSS_EXMX_IP_FLOW_ENTRY_TYPE_ENT)(gIpFlowEntryIndex / 8);

    realIndex =  gIpFlowEntryIndex % 8;


    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxIpFlowEntryGet(dev, entryType, realIndex,
                                     &portProt, &action);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = realIndex;
    inFields[1] = entryType;
    inFields[2] = portProt;
    inFields[3] = action.pktCmd;
    inFields[4] = action.mirror.mirrorToCpu;
    inFields[5] = action.mirror.mirrorToRxAnalyzerPort;
    inFields[6] = action.qos.markCmd;
    inFields[7] = action.qos.markDscp;
    inFields[8] = action.qos.dscp;
    inFields[9] = action.qos.markTc;
    inFields[10] = action.qos.tc;
    inFields[11] = action.qos.markDp;
    inFields[12] = action.qos.dp;
    inFields[13] = action.qos.markUp;
    inFields[14] = action.qos.up;
    inFields[15] = action.policer.policerMode;
    inFields[16] = action.policer.policerId;
    inFields[17] = action.fa.setLbi;
    inFields[18] = action.fa.lbi;
    inFields[19] = action.fa.flowId;
    inFields[20] = action.vlan.modifyVlan;
    inFields[21] = action.vlan.nestedVlan;
    inFields[22] = action.vlan.vlanId;
    inFields[23] = action.redirect.redirectCmd;
    inFields[24] = action.redirect.redirectVid;
    inFields[25] = action.redirect.redirectData.outInterface.type;
    inFields[26] = action.redirect.redirectData.outInterface.devPort.devNum;
    inFields[27] = action.redirect.redirectData.outInterface.devPort.portNum;
    inFields[28] = action.redirect.redirectData.outInterface.trunkId;
    inFields[29] = action.redirect.redirectData.outInterface.vidx;
    inFields[30] = action.redirect.redirectData.outInterface.vlanId;
    inFields[31] = action.redirect.redirectData.routeEntryPointer;
    inFields[32] = action.redirect.redirectData.mplsNhlfePointer;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29], inFields[30], inFields[31],
                inFields[32]);

    galtisOutput(outArgs, result, "%f");

    gIpFlowEntryIndex++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpMcRouteEntryRead
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       read an array of Mc route entries from the hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev                  - the device number
*       baseMemAddr          - the base memory address (offset). The
*                              entries will be read from this base on.
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       routeEntriesArrayPtr - the mc route entries array
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpFlowEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32    realIndex;

    GT_U8                               dev;
    CPSS_EXMX_IP_FLOW_ENTRY_TYPE_ENT    entryType;
    GT_U16                              portProt;
    CPSS_EXMX_POLICY_ACTION_STC         action;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    if(gIpFlowEntryIndex >= MAX_NUM_OF_IPFLOW_ENTRIES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    entryType = (CPSS_EXMX_IP_FLOW_ENTRY_TYPE_ENT)(gIpFlowEntryIndex / 8);

    realIndex =  gIpFlowEntryIndex % 8;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxIpFlowEntryGet(dev, entryType, realIndex,
                                     &portProt, &action);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = realIndex;
    inFields[1] = entryType;
    inFields[2] = portProt;
    inFields[3] = action.pktCmd;
    inFields[4] = action.mirror.mirrorToCpu;
    inFields[5] = action.mirror.mirrorToRxAnalyzerPort;
    inFields[6] = action.qos.markCmd;
    inFields[7] = action.qos.markDscp;
    inFields[8] = action.qos.dscp;
    inFields[9] = action.qos.markTc;
    inFields[10] = action.qos.tc;
    inFields[11] = action.qos.markDp;
    inFields[12] = action.qos.dp;
    inFields[13] = action.qos.markUp;
    inFields[14] = action.qos.up;
    inFields[15] = action.policer.policerMode;
    inFields[16] = action.policer.policerId;
    inFields[17] = action.fa.setLbi;
    inFields[18] = action.fa.lbi;
    inFields[19] = action.fa.flowId;
    inFields[20] = action.vlan.modifyVlan;
    inFields[21] = action.vlan.nestedVlan;
    inFields[22] = action.vlan.vlanId;
    inFields[23] = action.redirect.redirectCmd;
    inFields[24] = action.redirect.redirectVid;
    inFields[25] = action.redirect.redirectData.outInterface.type;
    inFields[26] = action.redirect.redirectData.outInterface.devPort.devNum;
    inFields[27] = action.redirect.redirectData.outInterface.devPort.portNum;
    inFields[28] = action.redirect.redirectData.outInterface.trunkId;
    inFields[29] = action.redirect.redirectData.outInterface.vidx;
    inFields[30] = action.redirect.redirectData.outInterface.vlanId;
    inFields[31] = action.redirect.redirectData.routeEntryPointer;
    inFields[32] = action.redirect.redirectData.mplsNhlfePointer;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29], inFields[30], inFields[31],
                inFields[32]);

    galtisOutput(outArgs, result, "%f");

    gIpFlowEntryIndex++;

    return CMD_OK;
}





/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxIpFlowSet",
        &wrCpssExMxIpFlowEntrySet,
        1, 33},

    {"cpssExMxIpFlowGetFirst",
        &wrCpssExMxIpFlowEntryGetFirst,
        1, 0},

    {"cpssExMxIpFlowGetNext",
        &wrCpssExMxIpFlowEntryGetNext,
        1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPcl
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
GT_STATUS cmdLibInitCpssExMxIpFlow
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


