/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapBridgeCpss.c
*
* DESCRIPTION:
*       Wrapper functions for Bridge cpss functions
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
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgMc.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgVlan.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgNestVlan.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgPrvEdgeVlan.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgGen.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgFdb.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgFdbHash.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgEgrFlt.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgStp.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgCount.h>
#include <cpss/exMx/exMxGen/cos/cpssExMxL2Cos.h>

/* semaphore for access to mac table action registers */
GT_SEM  macActionSemId;

/* lock the mac action -- return on fail */
#define LOCK_MAC_ACTION_MAC \
    if (cmdOsMutexLock(macActionSemId) != GT_OK)\
        return GT_FAIL

/* unlock the mac action */
#define UNLOCK_MAC_ACTION_MAC \
    cmdOsMutexUnlock(macActionSemId)

/* semaphore for access to mac table action registers */
extern GT_MUTEX  macActionSemId ;

/* API of CPSS that need to be protected by the:
     LOCK_MAC_ACTION_MAC , UNLOCK_MAC_ACTION_MAC

cpssExMxBrgFdbAgingTimeoutSet
cpssExMxBrgFdbMacTriggerModeSet
cpssExMxBrgFdbMacTriggerToggle
cpssExMxBrgFdbStaticTransEnable
cpssExMxBrgFdbStaticDelEnable
cpssExMxBrgFdbAgeWithoutRemovalEnable
cpssExMxBrgFdbFlushPortEmulate
cpssExMxBrgFdbFlush
cpssExMxBrgFdbTransplant
*/


/*******************************************************************************
* wrcpssExMxBrgMcUplinkFilterSet
*
* DESCRIPTION:
*       Sets CPU Multicast Uplink Filter. A Multicast packet coming from the
*       uplink to the CPU will be dropped when this bit is set.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       status  - GT_TRUE for Drop Multicast packet from uplink to CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong dev
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgMcUplinkFilterSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_BOOL  status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    status = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgMcUplinkFilterSet(dev, status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgPortEgrFltUnkEnable
*
* DESCRIPTION:
*       If enabled, bridged unknown unicast packets are filtered on the
*       specified egress logical port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       value  - GT_TRUE/GT_FALSE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or portNum
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgPortEgrFltUnkEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_U8    port;
    GT_BOOL  value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    value = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxBrgPortEgrFltUnkEnable(dev, port, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgUnkUcEgressFilterCmdSet
*
* DESCRIPTION:
*       Sets Egress Filtering command for forwarding an unknown unicast packet
*       on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       egrCmd  - egress filtering command
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong egress command or wrong dev
*
*
* COMMENTS:
*       This filter valid only for packet that was not dropped due to Ingress
*       filtering or trapped to CPU due to any reason
*
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgUnkUcEgressFilterCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8              dev;
    CPSS_EXMX_EGR_FILTER_CMD_ENT  egrCmd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    egrCmd = (CPSS_EXMX_EGR_FILTER_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgUnkUcEgressFilterCmdSet(dev, egrCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgUnregMcEgressFilterCmdSet
*
* DESCRIPTION:
*       Sets Egress Filtering command for forwarding an unregistered multicast
*       and broadcast packet on specified device
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       egrCmd  - egress filtering command
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong egress command or wrong dev
*
* COMMENTS:
*       This filter valid only for packet that was not dropped due to Ingress
*       filtering or trapped to CPU due to any reason
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgUnregMcEgressFilterCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8              dev;
    CPSS_EXMX_EGR_FILTER_CMD_ENT  egrCmd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    egrCmd = (CPSS_EXMX_EGR_FILTER_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgUnregMcEgressFilterCmdSet(dev, egrCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgCpuBcFilteringEnable
*
* DESCRIPTION:
*       Enable/disable CPU Broadcast Filtering, excluding ARP broadcasts on
*       specified device.
*
*       When enabled, broadcast packets are not forwarded to CPU
*       (even if it is a member of VLAN group.)  ARP broadcasts
*       however are not filtered by this mechanism.
*
*       When disabled, broadcast packets are forwarded according to its
*       VLAN group membership.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       en      - GT_TRUE for enable or GT_FLASE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong dev
*
* COMMENTS:
*         This mechanism is useful when the CPU is a member of VLANs, but
*         does not want to receive any MAC broadcasts EXCEPT for ARP.
*
*         Unicast/multicast MAC packets are unaffected by this filter,
*         only MAC broadcasts are affected.
*
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgCpuBcFilteringEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_BOOL  en;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    en = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgCpuBcFilteringEnable(dev, en);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgVlanEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/disable VLAN Egress Filtering on bridged known unicast packets
*       on specified device.
*       When enabled, VLAN egress filtering verifies that the egress port is a
*                     member of the packet's VLAN classification
*
*       When disabled, the VLAN egress filtering check is disabled.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev - physical device number
*       en  - GT_TRUE for enable or GT_FLASE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong dev
*
* COMMENTS:
*       VLAN egress filtering is required by 802.1Q, but if desired, this
*       mechanism can be disabled, thus allowing "leaky VLANs".
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanEgressFilteringEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_BOOL  en;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    en = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgVlanEgressFilteringEnable(dev, en);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgEgressFilteringForL3Enable
*
* DESCRIPTION:
*       Enable/disable Bridge Spanning Tree and and Egress Filtering on packets
*       that were treated by upper layer engines (above bridging) on specified
*       device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev - physical device number
*       en  - GT_TRUE for enable or GT_FLASE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong dev
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgEgressFilteringForL3Enable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_BOOL  en;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    en = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgEgressFilteringForL3Enable(dev, en);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*************************************/


/*******************************************************************************
* wrcpssExMxBrgFdbAgingTimeoutSet
*
* DESCRIPTION:
*       Sets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       timeout - aging time in seconds.  (10..630)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_OUT_OF_RANGE - timeout out of range (10..630)
*
* COMMENTS:
*       The value ranges from 10 seconds (1d) to 630 seconds (63d) in steps of
*       10 seconds.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbAgingTimeoutSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_U32  timeout;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    timeout = (GT_U32)inArgs[1];

    /* call cpss api function */
    LOCK_MAC_ACTION_MAC;
    result = cpssExMxBrgFdbAgingTimeoutSet(dev, timeout);
    UNLOCK_MAC_ACTION_MAC;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgFdbMacAddrEntrySet
*
* DESCRIPTION:
*       Creates the new entry in Hardware MAC address table through Address
*       Update register.(AU message to the PP - non direct access)
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev         - physical device number
*       entryPtr    - (pointer to) mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum,saCommand,daCommand
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*
* COMMENTS:
*
*******************************************************************************/
#if 0
static CMD_STATUS wrcpssExMxBrgFdbMacAddrEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS result;

    GT_ETHERADDR lMacAddr;


    GT_U8                dev;
    CPSS_MAC_ENTRY_STC  entryPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    galtisMacAddr(&lMacAddr, (GT_U8*)inFields[0]);


    cmdOsMemSet(&entryPtr, 0, sizeof(entryPtr));

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    cmdOsMemCpy(&entryPtr.macAddr, &lMacAddr, sizeof(lMacAddr));
    entryPtr.dstInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[1];
    entryPtr.dstInterface.devPort.devNum = (GT_U8)inFields[2];
    entryPtr.dstInterface.devPort.portNum = (GT_U8)inFields[3];
    CONVERT_DEV_PORT_DATA_MAC(entryPtr.dstInterface.devPort.devNum,
                         entryPtr.dstInterface.devPort.portNum);

    entryPtr.dstInterface.trunkId = (GT_U16)inFields[4];
    entryPtr.dstInterface.vidx = (GT_U16)inFields[5];
    entryPtr.isStatic = (GT_BOOL)inFields[6];
    entryPtr.vlanId = (GT_U16)inFields[7];
    entryPtr.srcTc = (GT_U8)inFields[8];
    entryPtr.dstTc = (GT_U8)inFields[9];
    entryPtr.daCommand = (CPSS_MAC_TABLE_CMD_ENT)inFields[10];
    entryPtr.saCommand = (CPSS_MAC_TABLE_CMD_ENT)inFields[11];
    entryPtr.saClass = (GT_BOOL)inFields[12];
    entryPtr.daClass = (GT_BOOL)inFields[13];
    entryPtr.saCib = (GT_BOOL)inFields[14];
    entryPtr.daCib = (GT_BOOL)inFields[15];
    entryPtr.daRoute = (GT_BOOL)inFields[16];
    entryPtr.cosIpv4En = (GT_BOOL)inFields[17];
    entryPtr.mirrorToRxAnalyzerPortEn = (GT_BOOL)inFields[18];

    /* call tapi api function */
    result = cpssExMxBrgFdbMacAddrEntrySet(dev, &entryPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgFdbMacAddrEntryDel
*
* DESCRIPTION:
*       Deletes an old entry in Hardware MAC address table through Address
*       Update register.(AU message to the PP - non direct access)
*
* INPUTS:
*       dev   - physical device number
*       entryPtr - (pointer to)mac table entry
*
* APPLICABLE DEVICES:  All ExMx devices
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or vlanId
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/

static CMD_STATUS wrcpssExMxBrgFdbMacAddrEntryDel
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    CPSS_MAC_ENTRY_STC  entryPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxBrgFdbMacAddrEntryDel(dev, &entryPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
#endif
/*******************************************************************************
* wrcpssExMxBrgFdbMacTriggerModeSet
*
* DESCRIPTION:
*       Sets Mac address table Triggered\Automatic action mode.
*       Action is done Automatically or Action is done via a trigger from CPU.
*       All actions will be taken on entries belonging to a certain VLAN
*       or a subset of VLANs.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev  - physical device number
*       mode - action mode.
*       vlanId  - Vlan Id
*       vlanMask - vlan mask filter
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM - wrong devNum or vlanId or vlanMask or mode
*
* COMMENTS:
*       Use mask 0xFFF and VLAN ID to set mode on a single VLAN
*       Use mask 0 and VLAN 0 to set mode for all VLANs
*       For 98Mx620 , 98Mx6x5/8 The setting of aging mode is system-wide.
*       For these devices vlanId and vlanMask must be set to 0, attempt to
*       set parameters other than 0 will result in GT_NOT_SUPPORTED return code
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbMacTriggerModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                dev;
    CPSS_MAC_ACTION_MODE_ENT   mode;
    GT_U16               vlanId;
    GT_U16               vlanMask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mode = (CPSS_MAC_ACTION_MODE_ENT)inArgs[1];
    vlanId = (GT_U16)inArgs[2];
    vlanMask = (GT_U16)inArgs[3];

    /* call cpss api function */
    LOCK_MAC_ACTION_MAC;
    result = cpssExMxBrgFdbMacTriggerModeSet(dev, mode, vlanId, vlanMask);
    UNLOCK_MAC_ACTION_MAC;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgFdbMacTriggerToggle
*
* DESCRIPTION:
*       Toggle Agging Trigger and cause to device scans its MAC address table.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbMacTriggerToggle
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                dev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    LOCK_MAC_ACTION_MAC;
    result = cpssExMxBrgFdbMacTriggerToggle(dev);
    UNLOCK_MAC_ACTION_MAC;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgFdbStaticTransEnable
*
* DESCRIPTION:
*       This routine determines whether the transplanting operate on static
*       entries.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev - physical device number
*       enable  - GT_TRUE transplanting is enabled on static entries,
*             GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbStaticTransEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    LOCK_MAC_ACTION_MAC;
    result = cpssExMxBrgFdbStaticTransEnable(dev, enable);
    UNLOCK_MAC_ACTION_MAC;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgFdbStaticDelEnable
*
* DESCRIPTION:
*       This routine determines whether flush delete operates on static entries.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev - physical device number
*       enable  - GT_TRUE delete is enabled on static entries, GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbStaticDelEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    LOCK_MAC_ACTION_MAC;
    result = cpssExMxBrgFdbStaticDelEnable(dev, enable);
    UNLOCK_MAC_ACTION_MAC;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgFdbAgeWithoutRemovalEnable
*
* DESCRIPTION:
*       This routine determines whether the Aging performs the actual entry
*       removal from the Mac table or not.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev - physical device number
*       enable  - GT_TRUE age without removal, GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbAgeWithoutRemovalEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    LOCK_MAC_ACTION_MAC;
    result = cpssExMxBrgFdbAgeWithoutRemovalEnable(dev, enable);
    UNLOCK_MAC_ACTION_MAC;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgFdbHashOptionsSet
*
* DESCRIPTION:
*       The function sets FDB Hash parameters, the MAC/VID Masks and
*       Left Cyclic Shifts are used upon MAC Table Lookup.
*
*       To ensure MAC table coherency in the system, all devices must have
*       the same values of MAC/VID Lookup Masks and Left Cyclic Shifts.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum              - PP's device number.
*       hashOptParamsPtr    - pointer to the FDB Hash Options Structure
*                             includes the MAC/VID Lookup Masks and
*                             Left Cyclic Shifts values.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PTR          - on NULL pointer.
*       GT_BAD_PARAM        - on wrong parameters.
*       GT_NOT_INITIALIZED  - if the driver was not initialized.
*
* COMMENTS:
*       It is essential to use by this function upon empty MAC Table,
*       otherwise the alteration of FDB Hash Options might cause of
*       unmatching of existing MAC Table entries.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxBrgFdbHashOptionsSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    CPSS_FDB_HASH_OPTION_STC  hashOptParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    galtisMacAddr(&(hashOptParams.macLookupMask), (GT_U8*)inArgs[1]);
    hashOptParams.vidLookupMask = (GT_U16)inArgs[2];
    hashOptParams.macShiftLeft = (GT_U32)inArgs[3];
    hashOptParams.vidShiftLeft = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssExMxBrgFdbHashOptionsSet(dev, &hashOptParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgFdbHashOptionsGet
*
* DESCRIPTION:
*       The function gets FDB Hash parameters, the MAC/VID Masks and
*       Left Cyclic Shifts are used upon MAC Table Lookup.
*
*       To ensure MAC table coherency in the system, all devices must have
*       the same values of MAC/VID Lookup Masks and Left Cyclic Shifts.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum              - PP's device number.
*
* OUTPUTS:
*       hashOptParamsPtr    - pointer to the FDB Hash Options Structure
*                             includes the MAC/VID Lookup Masks and
*                             Left Cyclic Shifts values.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PTR          - on NULL pointer.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_NOT_INITIALIZED  - if the driver was not initialized.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxBrgFdbHashOptionsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    CPSS_FDB_HASH_OPTION_STC  hashOptParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxBrgFdbHashOptionsGet(dev, &hashOptParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%6b%d%d%d",
                 hashOptParams.macLookupMask.arEther,
                 hashOptParams.vidLookupMask,
                 hashOptParams.macShiftLeft,
                 hashOptParams.vidShiftLeft);

    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgFdbMacVlanLookupModeSet
*
* DESCRIPTION:
*       Sets the VLAN Lookup mode.
*       CPSS_SVL_E - Only the MAC is used for MAC table lookup and learning
*       CPSS_IVL_E - The Vid + MAC are used for MAC table lookup and learning
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev  - physical device number
*       mode - lookup mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or mode
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbMacVlanLookupModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    CPSS_MAC_VL_ENT      mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mode = (CPSS_MAC_VL_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgFdbMacVlanLookupModeSet(dev, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgFdbMacAddrLookupModeSet
*
* DESCRIPTION:
*       Sets the algorithm that optimizes the MAC Address lookup depending on
*       the network configuration.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev      - physical device number
*       addrMode - one from address lookup mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or mode
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbMacAddrLookupModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    CPSS_ADDR_LOOKUP_MODE_ENT   addrMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    addrMode = (CPSS_ADDR_LOOKUP_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgFdbMacAddrLookupModeSet(dev, addrMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgFdbPortLearnSet
*
* DESCRIPTION:
*       Enable/disable learning of new source MAC addresses for packets received
*       on specified port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       status - GT_TRUE for enable  or GT_FALSE otherwise
*       cmd     - how to forward packets with unknown/changed SA,
*                 if status is GT_FALSE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or port or cmd
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbPortLearnSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8            dev;
    GT_U8            port;
    GT_BOOL          status;
    CPSS_PORT_LOCK_CMD_ENT cmd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    status = (GT_BOOL)inArgs[2];
    cmd = (CPSS_PORT_LOCK_CMD_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxBrgFdbPortLearnSet(dev, port, status, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgFdbPortLearnGet
*
* DESCRIPTION:
*       Get state of new source MAC addresses learning on packets received
*       on specified port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       statusPtr - (pointer to) GT_TRUE for enable  or GT_FALSE otherwise
*       cmdPtr    - (pointer to) how to forward packets with unknown/changed SA,
*                   when (*statusPtr) is GT_FALSE
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum or port
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbPortLearnGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL statusPtr;
    CPSS_PORT_LOCK_CMD_ENT cmdPt;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxBrgFdbPortLearnGet(dev, port, &statusPtr, &cmdPt);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", statusPtr, cmdPt);
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgFdbMacFilterPortEnable
*
* DESCRIPTION:
*       Enable/disable MAC range filter facility on specified port.
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       status - GT_TRUE for enable  or GT_FALSE otherwise
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or port
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbMacFilterPortEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U8   port;
    GT_BOOL status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    status = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxBrgFdbMacFilterPortEnable(dev, port, status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgFdbMacFilterEntrySet
*
* DESCRIPTION:
*       Sets MAC range filter and associates control actions with this range in
*       specific index.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - physical device number
*       entryInd  - entry index in MAC address range table
*       macAddrPtr - (pointer to) mac address
*       macMaskPtr - (pointer to) 48 bit mask
*       daCommand - action taken when a packet's DA matches this range
*       saCommand - action taken when a packet's SA matches this range
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or daCommand or saCommand
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbMacFilterEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                dev;
    GT_32                entryInd;
    GT_ETHERADDR         macAddrPtr;
    GT_ETHERADDR         macMaskPtr;
    CPSS_MAC_FLT_CMD_ENT daCommand;
    CPSS_MAC_FLT_CMD_ENT saCommand;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    entryInd = (GT_32)inArgs[1];
    galtisMacAddr(&macAddrPtr, (GT_U8*)inFields[0]);
    galtisMacAddr(&macMaskPtr, (GT_U8*)inFields[1]);
    daCommand = (CPSS_MAC_FLT_CMD_ENT)inFields[2];
    saCommand = (CPSS_MAC_FLT_CMD_ENT)inFields[3];


    /* call cpss api function */
    result = cpssExMxBrgFdbMacFilterEntrySet(dev, entryInd, &macAddrPtr, &macMaskPtr, daCommand, saCommand);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgFdbMacFilterEntryGet
*
* DESCRIPTION:
*       Gets MAC range filter and associates control actions with this range in
*       specific index.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - physical device number
*       entryInd  - entry index in MAC address range table
*
* OUTPUTS:
*       macAddrPtr - (pointer to) mac address
*       macMaskPtr - (pointer to) 48 bit mask
*       daCommandPtr - (pointer to)action taken when a packet's DA matches this
*                      range
*       saCommandPtr - (pointer to)action taken when a packet's SA matches this
*                      range
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*
*******************************************************************************/
#if 0
static CMD_STATUS wrcpssExMxBrgFdbMacFilterEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                dev;
    GT_32                entryInd;
    GT_ETHERADDR         macAddrPtr;
    GT_ETHERADDR         macMaskPtr;
    CPSS_MAC_FLT_CMD_ENT daCommand;
    CPSS_MAC_FLT_CMD_ENT saCommand;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    entryInd = (GT_32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgFdbMacFilterEntryGet(dev, entryInd, &macAddrPtr, &macMaskPtr, &daCommand, &saCommand);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", macAddrPtr, macMaskPtr, daCommand, saCommand);
    return CMD_OK;
}
#endif
/*******************************************************************************
* cpssExMxBrgFdbNaToCpuEnable
*
* DESCRIPTION:
*       Enable/disable forwarding a new mac address message to CPU.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       status  - If GT_TRUE, forward NA message to CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbNaToCpuEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_BOOL status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    status = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgFdbNaToCpuEnable(dev, status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgFdbMacUpd2CpuEnable
*
* DESCRIPTION:
*       Enable/disable forwarding following  mac address message to CPU:
*       AA, TA, SA, QA.
*
* APPLICABLE DEVICES:  All ExMx devices
*                      Mx device - Disable/Enable SA other
*                      Ex device - Disable/Enable TA and AA , QA
*
* INPUTS:
*       dev     - physical device number
*       status  - If GT_TRUE, forward update message to CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbMacUpd2CpuEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_BOOL status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    status = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgFdbMacUpd2CpuEnable(dev, status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgFdbAuToCpuSet
*
* DESCRIPTION:
*       Enable/Disable the PP from sending to the CPU an address
*       update messages on one unit device.
*
*       The following commands available:
*        1. Disable/Enable both TA and AA messages.
*        2. Disable/Enable SA sent due to locked port or VLAN learning disabled.
*        3. Disable/Enable SA sent due to other reason.
*        4. Disable/Enable QA messages.
*        5. Disable/Enable all of the above.
*
* APPLICABLE DEVICES:  All ExMx devices
*                      Mx device - Disable/Enable SA ,SA other, TA and AA , QA
*                      Ex device - Disable/Enable TA and AA , QA
*
* INPUTS:
*       dev         - physical device number
*       auMsgType   - type of message to enable / disable.
*       enable      - enable or disable the message
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or auMsgType
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbAuToCpuSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                dev;
    CPSS_MAC_AU2CPU_TYPE_ENT   auMsgType;
    GT_BOOL              enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    auMsgType = (CPSS_MAC_AU2CPU_TYPE_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxBrgFdbAuToCpuSet(dev, auMsgType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgFdbAuToCpuGet
*
* DESCRIPTION:
*       Get the PP state for sending to the CPU an address
*       update messages on one unit device.
*
*       One of the following commands available:
*        1. get both TA and AA messages.
*        2. get SA sent due to locked port or VLAN learning disabled.
*        3. get SA sent due to other reason.
*        4. get QA messages.
*
* APPLICABLE DEVICES:  All ExMx devices
*                      Mx device - get SA ,SA other, TA and AA , QA
*                      Ex device - get TA and AA , QA
*
* INPUTS:
*       dev         - physical device number
*       auMsgType   - type of message to get it's state.
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - the messages to CPU enabled
*                                  GT_FALSE - the messages to CPU disabled
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or auMsgType
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbAuToCpuGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       dev;
    CPSS_MAC_AU2CPU_TYPE_ENT    auMsgType;
    GT_BOOL                     enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    auMsgType = (CPSS_MAC_AU2CPU_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgFdbAuToCpuGet(dev, auMsgType, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgFdbFlushPortEmulate
*
* DESCRIPTION:
*       Deletes all addresses learned on this {port,portDev} for specific VLAN
*       or group VLANs from MAC Address Table.
*
*       the function is not supported by those PP in HW , as single operation ,
*       there for it should be emulated by several HW operations.
*
*       This function waits for HW operation to end.
*       to restore the original register value
*
*
*       Important: the function emulate "flush port" by using next technique:
*              1. transplant the mac entries of the specified port to non-exists
*               device number
*               - this operation will/wont send TA (transplant) messages to the
*                 CPU ,according to the pre-defined PP about AA/TA messages to
*                 CPU
*              2. perform trigger aging that will cause the PP to delete the
*                 mac entries associated with the "non-exists" device.
*
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev      - physical device number - to transplant flush entries in it
*       portDev - physical device number of the flushed port
*       port    - physical port number
*       vlanId  - Vlan Id
*       vlanMask - vlan mask filter
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_NO_SUCH      - the emulation could not find non-exists device number
*                          needed for the transplant action that is the first
*                          step of the emulation
*       GT_OUT_OF_RANGE - out of range parameters portDev,port
*       GT_BAD_PARAM - wrong devNum/vlanId/vlanMsask
*
* COMMENTS:
*       This function will delete all entries with specified port and:
*       Address Entry<Vid> * <vlanMask> = <vlanId>.
*       To delete all entries for this port, set vid & vlanMask to ZERO.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbFlushPortEmulate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_U8    portDev;
    GT_U8    port;
    GT_U16   vlanId;
    GT_U16   vlanMask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portDev = (GT_U8)inArgs[1];
    port = (GT_U8)inArgs[2];
    vlanId = (GT_U16)inArgs[3];
    vlanMask = (GT_U16)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    LOCK_MAC_ACTION_MAC;
    result = cpssExMxBrgFdbFlushPortEmulate(dev, portDev, port, vlanId, vlanMask);
    UNLOCK_MAC_ACTION_MAC;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgFdbFlush
*
* DESCRIPTION:
*       Deletes all addresses for specific VLAN or group VLANs from MAC Address
*       Table on specified device.
*
*       If Aged Address (AA) update messages to the CPU is enabled,
*       an AA update message is sent to the CPU for each deleted entry.
*
*       If AA messages are sent to the CPU and the CPU Address Update Queue
*       (AUQ) fills up, the delete pass pauses at the entry that generated the
*       address update and it proceeds only when the AUQ has room for new AA
*       entries.
*       This ensures that the CPU is always synchronized with the FDB.
*
*       This function waits for HW operation to end.
*       to restore the original register value
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev      - physical device number
*       vlanId   - Vlan Id
*       vlanMask - vlan mask filter
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM - wrong devNum/vlanId/vlanMsask
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbFlush
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_U16   vlanId;
    GT_U16   vlanMask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    vlanMask = (GT_U16)inArgs[2];

    /* call cpss api function */
    LOCK_MAC_ACTION_MAC;
    result = cpssExMxBrgFdbFlush(dev, vlanId, vlanMask);
    UNLOCK_MAC_ACTION_MAC;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgFdbTransplant
*
* DESCRIPTION:
*       Transplants entries belonging to a certain VLAN or subset of VLANS from
*       specific existing {old port,old dev} to another {new port,new dev}
*
*       FDB address transplanting is intended to efficiently handle the
*       relocation of a set of Unicast MAC Address table entries from one
*       device/port to another.
*       This feature is very useful for implementing IEEE 802.1w Rapid
*       Reconfiguration.
*
*       For example, when an active link fails and the previously blocked
*       redundant link becomes active, all the addresses learned on the failed
*       link can quickly be relearned to reside on the activated redundant link.
*
*       If Transplant Address (TA) update messages to the CPU is enabled
*       (via <ForAA_TAUpdMsg2CPU>), a TA update message is sent to the CPU for
*       each transplanted entry.
*       If TA messages are sent to the CPU and the CPU Address Update Queue
*       (AUQ) fills, the transplant pass pauses at the entry that generated the
*       address update, and it proceeds only when the AUQ has room for new AA
*       entries. This ensures that the CPU is always synchronized with the FDB.
*
*       Function waits for HW operation to finish
*       to restore the original register value
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev      - physical device number - to transplant entries in it
*       devNew   - new physical device number
*       nPort    - new physical port number
*       devOld   - old physical device number
*       oPort    - old physical port number
*       vlanId   - Vlan Id
*       vlanMask - vlan mask filter
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong dev/vlanId/vlanMask
*       GT_OUT_OF_RANGE - nPort/oPort/devNew/devOld out of range
*
* COMMENTS:
*       This function will transplant all entries with:
*       Address Entry<Vid> * <vlanMask> = <vlanId>.
*
*
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbTransplant
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_U8    devNew;
    GT_U8    nPort;
    GT_U8    devOld;
    GT_U8    oPort;
    GT_U16   vlanId;
    GT_U16   vlanMask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    devNew = (GT_U8)inArgs[1];
    nPort = (GT_U8)inArgs[2];
    devOld = (GT_U8)inArgs[3];
    oPort = (GT_U8)inArgs[4];
    vlanId = (GT_U16)inArgs[5];
    vlanMask = (GT_U16)inArgs[6];

    /* call cpss api function */
    LOCK_MAC_ACTION_MAC;
    result = cpssExMxBrgFdbTransplant(dev, devNew, nPort, devOld, oPort, vlanId, vlanMask);
    UNLOCK_MAC_ACTION_MAC;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgFdbMacAddrHwEntryGet
*
* DESCRIPTION:
*       The function return the entire entry format for a specific key{mac,vid}
*
*       The function Send Query Address (QA) message to the hardware MAC address
*       table and waits for answer.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - physical device number
*       entryPtr  - (pointer to) key parameters --> mac,vid
*           entryPtr->macEntry.macAddr   - mac address
*           entryPtr->macEntry.vid       - vlan id
*
* OUTPUTS:
*       entryPtr  - (pointer to) AU message format
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_NOT_FOUND - the entry was not found
*       GT_BAD_PARAM - wrong devNum/vlan
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
#if 0
static CMD_STATUS wrcpssExMxBrgFdbMacAddrHwEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                     dev;
    CPSS_MAC_UPDATE_MSG_STC   entryPtr;

    GT_ETHERADDR lMacAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[1];

    galtisMacAddr(&lMacAddr, (GT_U8*)inFields[2]);
    cmdOsMemCpy(&(entryPtr.macEntry.macAddr), &lMacAddr, sizeof(lMacAddr));
    entryPtr.macEntry.vlanId = (GT_U16)inFields[9];

    /* call cpss api function */
    result = cpssExMxBrgFdbMacAddrHwEntryGet(dev, &entryPtr);

    if (result != GT_OK)
    {
        if (result == GT_NO_SUCH)
            galtisOutput(outArgs, GT_OK, "%d", -1);
        else
            galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = entryPtr.updType;
    inFields[1] = entryPtr.entryWasFound;
    inFields[2] = entryPtr.macEntryIndex;
    inFields[3] = (GT_U32)(entryPtr.macEntry.macAddr.arEther);
    inFields[4] = entryPtr.macEntry.dstInterface.type;
    CONVERT_BACK_DEV_PORT_DATA_MAC(entryPtr.dstInterface.devPort.devNum,
                         entryPtr.dstInterface.devPort.portNum);
    inFields[5] = entryPtr.macEntry.dstInterface.devPort.devNum;
    inFields[6] = entryPtr.macEntry.dstInterface.devPort.portNum;

    inFields[7] = entryPtr.macEntry.dstInterface.trunkId;
    inFields[8] = entryPtr.macEntry.dstInterface.vidx;
    inFields[9] = entryPtr.macEntry.isStatic;
    inFields[10] = entryPtr.macEntry.vlanId;
    inFields[11] = entryPtr.macEntry.srcTc;
    inFields[12] = entryPtr.macEntry.dstTc;
    inFields[13] = entryPtr.macEntry.daCommand;
    inFields[14] = entryPtr.macEntry.saCommand;
    inFields[15] = entryPtr.macEntry.saClass;
    inFields[16] = entryPtr.macEntry.daClass;
    inFields[17] = entryPtr.macEntry.saCib;
    inFields[18] = entryPtr.macEntry.daCib;
    inFields[19] = entryPtr.macEntry.daRoute;
    inFields[20] = entryPtr.macEntry.cosIpv4En;
    inFields[21] = entryPtr.macEntry.mirrorToRxAnalyzerPortEn;
    inFields[22] = 0;
    inFields[23] = 0;

    /* pack and output table fields */
    fieldOutput("%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],
                inFields[1], inFields[2],inFields[3], inFields[4],
                inFields[5], inFields[6], inFields[7], inFields[8],
                inFields[9], inFields[10], inFields[11], inFields[12],
                inFields[13], inFields[14], inFields[15], inFields[16],
                inFields[17], inFields[18], inFields[19], inFields[20],
                inFields[21], inFields[22], inFields[23]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgFdbMacAddrHwEntryIndexGet
*
* DESCRIPTION:
*       The function return the index in HW of a specific key{mac,vid}
*
*       The function Send Query Index (QI) message to the hardware MAC address
*       table and waits for answer.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - physical device number
*       vid       - vlan id
*       macAddrPtr- (pointer to)mac address
*
* OUTPUTS:
*       indexPtr   - (pointer to)mac address entry index.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_NOT_FOUND - the entry was not found
*       GT_BAD_PARAM - wrong devNum/vlan
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbMacAddrHwEntryIndexGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               dev;
    GT_U16              vid;
    GT_ETHERADDR        macAddrPtr;
    GT_U32              indexPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    vid = (GT_U16)inArgs[1];
    galtisMacAddr(&macAddrPtr, (GT_U8*)inArgs[2]);

    /* call cpss api function */
    result = cpssExMxBrgFdbMacAddrHwEntryIndexGet(dev, vid, &macAddrPtr, &indexPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", indexPtr);
    return CMD_OK;
}
#endif

/*******************************************************************************
* wrcpssExMxBrgFdbMaxLookupLenSet
*
* DESCRIPTION:
*       Set the the number of entries to be looked up in the MAC table lookup
*       (the hash chain length)
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev         - physical device number
*       lookupLen   - The maximal length of MAC table lookup, this must be an
*                     even value, in range. (2,4,6..16)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success,
*       GT_OUT_OF_RANGE - lookupLen is too large lookupLen > 16 or lookupLen <2
*       GT_FAIL         - otherwise.
*       GT_BAD_PARAM - wrong devNum or lookupLen is odd
*
* COMMENTS:
*
*       The Maximal Hash chain length in HW calculated as follows:
*       Maximal Hash chain length = (regVal + 1) * 2
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbMaxLookupLenSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_U8    lookupLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    lookupLen = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgFdbMaxLookupLenSet(dev, lookupLen);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxBrgFdbMacAddrEntryWrite/Set
*
* DESCRIPTION:
*       Write the new entry in Hardware MAC address table in specified offset.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev         - physical device number
*       index       - hw mac entry index
*       skip        - entry skip control
*       entryPtr    - (pointer to)mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum,saCommand,daCommand
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*
* COMMENTS:
*       caller may need to disable the Layer 2 Ingress processor in the PP
*       before this action (and re-enable after the invalidation)
*       use function cpssExMxBrgFdbL2IngrProcEnable
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbMacAddrEntreySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS result;
    GT_BOOL dowrite;

    GT_U8         dev;
    GT_U32        index = 0;
    GT_BOOL       skip = GT_FALSE;
    CPSS_MAC_ENTRY_STC  entryPtr;

    GT_ETHERADDR lMacAddr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&entryPtr, 0, sizeof(entryPtr));

    /* map input arguments to locals */
    dowrite = (GT_BOOL)inArgs[0]; /* switch if call write or call set*/
    dev = (GT_U8)inArgs[1];

    if(dowrite)
    {
        index = (GT_U32)inFields[0];
        skip = (GT_BOOL)inFields[1];
    }

    galtisMacAddr(&lMacAddr, (GT_U8*)inFields[2]);
    cmdOsMemCpy(&(entryPtr.macAddr), &lMacAddr, sizeof(lMacAddr));
    entryPtr.dstInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[3];
    entryPtr.dstInterface.devPort.devNum = (GT_U8)inFields[4];
    entryPtr.dstInterface.devPort.portNum = (GT_U8)inFields[5];
    CONVERT_DEV_PORT_DATA_MAC(entryPtr.dstInterface.devPort.devNum,
                         entryPtr.dstInterface.devPort.portNum);

    entryPtr.dstInterface.trunkId = (GT_U16)inFields[6];
    entryPtr.dstInterface.vidx = (GT_U16)inFields[7];
    entryPtr.isStatic = (GT_BOOL)inFields[8];
    entryPtr.vlanId = (GT_U16)inFields[9];
    entryPtr.srcTc = (GT_U8)inFields[10];
    entryPtr.dstTc = (GT_U8)inFields[11];
    entryPtr.daCommand = (CPSS_MAC_TABLE_CMD_ENT)inFields[12];
    entryPtr.saCommand = (CPSS_MAC_TABLE_CMD_ENT)inFields[13];
    entryPtr.saClass = (GT_BOOL)inFields[14];
    entryPtr.daClass = (GT_BOOL)inFields[15];
    entryPtr.saCib = (GT_BOOL)inFields[16];
    entryPtr.daCib = (GT_BOOL)inFields[17];
    entryPtr.daRoute = (GT_BOOL)inFields[18];
    entryPtr.cosIpv4En = (GT_BOOL)inFields[19];
    entryPtr.mirrorToRxAnalyzerPortEn = (GT_BOOL)inFields[20];

    /* call tapi api function */
    if(dowrite)
    {
        result = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entryPtr);
    }
    else
    {
        result = cpssExMxBrgFdbMacAddrEntrySet(dev, &entryPtr);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgFdbMacAddrEntryInvalidateOrDelete
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified offset.
*       the invalidation done by resetting to first word of the entry
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev         - physical device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*       caller may need to disable the Layer 2 Ingress processor in the PP
*       before this action (and re-enable after the invalidation)
*       use function cpssExMxBrgFdbL2IngrProcEnable
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbMacAddrEntryInvalidateOrDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_U32   index;
    CPSS_MAC_ENTRY_STC  entryPtr;
    GT_ETHERADDR lMacAddr;

    GT_BOOL DoInvalidate;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    DoInvalidate = (GT_BOOL)inArgs[0];

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[1];

    if(DoInvalidate)
    {
        index = (GT_U32)inFields[0];

        result = cpssExMxBrgFdbMacAddrEntryInvalidate(dev, index);
    }
    else
    {
        cmdOsMemSet(&entryPtr, 0, sizeof(entryPtr));
        galtisMacAddr(&lMacAddr, (GT_U8*)inFields[2]);
        cmdOsMemCpy(&(entryPtr.macAddr), &lMacAddr, sizeof(lMacAddr));
        entryPtr.vlanId = (GT_U16)inFields[9];

        result = cpssExMxBrgFdbMacAddrEntryDel(dev, &entryPtr);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgFdbL2IngrProcEnable
*
* DESCRIPTION:
*       enable/Disable the the Layer 2 Ingress processor in the PP
*       Mx devices - need to disable L2I when writing directly to FDB RAM
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev         - physical device number
*       enable      - hw mac entry index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbL2IngrProcEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_U32   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    enable = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgFdbL2IngrProcEnable(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgFdbPortAuToCpuSet
*
* DESCRIPTION:
*       Enable/Disable the logical port from sending to the CPU an address
*       update messages of the specified type.
*
*       The following address update messages on the logical port can
*       controlled:
*       1. Disable/Enable both TA and AA messages.
*       2. Disable/Enable SA sent due to locked port or VLAN learning disabled.
*       3. Disable/Enable SA sent due to other reasons.
*       4. Disable/Enable all the above.
*
* APPLICABLE DEVICES:  98Ex1x6 , 98Ex1x5 devices
*
* INPUTS:
*       dev             - device number
*       portNum         - port number
*       auMsgType       - address update message type to enable or disable
*       enable          - GT_TRUE enable sending the message to the CPU
*                         GT_FALSE for disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success,
*       GT_FAIL             - on error.
*       GT_NOT_SUPPORTED    - one of the devices does not support this feature.
*       GT_BAD_PARAM - wrong devNum/portNum/auMsgType
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbPortAuToCpuSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                dev;
    GT_U8                portNum;
    CPSS_MAC_AU2CPU_TYPE_ENT   auMsgType;
    GT_BOOL              enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    auMsgType = (CPSS_MAC_AU2CPU_TYPE_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssExMxBrgFdbPortAuToCpuSet(dev, portNum, auMsgType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgFdbDeviceTableSet
*
* DESCRIPTION:
*       This function sets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*       So for proper work of PP the application must set the relevant bits of
*       all devices in the system prior to inserting FDB entries associated with
*       them
*
* INPUTS:
*       devNum      - device number
*       devTable    - (Array of) bmp of devices to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxBrgFdbDeviceTableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum, i;
    GT_U32   devTable[CPSS_EXMX_FDB_DEV_TABLE_SIZE_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    for(i = 0;i < CPSS_EXMX_FDB_DEV_TABLE_SIZE_CNS;i++)
    {
        devTable[i] = (GT_U32)inFields[i];
    }

    /* call cpss api function */
    result = cpssExMxBrgFdbDeviceTableSet(devNum, devTable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxBrgFdbMacActionsEnableGet
*
* DESCRIPTION:
*       get whether the FDB Table actions enabled/disabled.
*
*       This relate to the HW's Aging daemon , that can perform on of the next
*       at a time:
*       1) automatic FDB aging
*       2) triggered FDB aging
*       3) delete FDB entries (FLUSH)
*       4) transplant FDB entries
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr - (pointer to)enable value.
*                   GT_TRUE - the actions are enabled
*                   GT_FALSE - the actions are disabled
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxBrgFdbMacActionsEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxBrgFdbMacActionsEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxBrgFdbMacActionsEnableSet
*
* DESCRIPTION:
*       enabled/disabled the FDB Table actions.
*
*       This relate to the HW's Aging daemon , that can perform on of the next
*       at a time:
*       1) automatic FDB aging
*       2) triggered FDB aging
*       3) delete FDB entries (FLUSH)
*       4) transplant FDB entries
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum      - physical device number
*       enable      - enable value.
*                     GT_TRUE - the actions are enabled
*                     GT_FALSE - the actions are disabled
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxBrgFdbMacActionsEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgFdbMacActionsEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxBrgFdbMacAddrRawEntryRead
*
* DESCRIPTION:
*       Read the "raw" FDB entry from Hardware MAC address table in specified
*       offset.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum      - physical device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       entryArray  - (pointer to)"raw" mac table entry
*                     the array is allocated by the caller
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*       Caller need to disable for all ExMx devices exclude 98EX1x6 , the
*       Layer 2 Ingress processor in the PP before this action (and re-enable
*       after the writing ended) use function cpssExMxBrgFdbL2IngrProcEnable.
*       This is needed in-order to achieve mutual exclusion between the PP and
*       the CPU , when read/write the FDB table by direct RAM operation.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxBrgFdbMacAddrRawEntryRead

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum, i;
    GT_U32   index;
    GT_U32   entryArray[CPSS_EXMX_FDB_RAW_ENTRY_SIZE_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgFdbMacAddrRawEntryRead(devNum, index, entryArray);

    for(i = 0;i < CPSS_EXMX_FDB_RAW_ENTRY_SIZE_CNS;i++)
    {
        inFields[i] = entryArray[i];
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgFdbMacAddrRawEntryWrite
*
* DESCRIPTION:
*       Write the "raw" FDB entry to Hardware MAC address table in specified
*       offset.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum      - physical device number
*       index       - hw mac entry index
*       entryArray  - (pointer to)"raw" mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*       Caller need to disable for all ExMx devices exclude 98EX1x6 , the
*       Layer 2 Ingress processor in the PP before this action (and re-enable
*       after the writing ended) use function cpssExMxBrgFdbL2IngrProcEnable.
*       This is needed in-order to achieve mutual exclusion between the PP and
*       the CPU , when read/write the FDB table by direct RAM operation.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxBrgFdbMacAddrRawEntryWrite

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum, i;
    GT_U32   index;
    GT_U32   entryArray[CPSS_EXMX_FDB_RAW_ENTRY_SIZE_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    for(i = 0;i < CPSS_EXMX_FDB_RAW_ENTRY_SIZE_CNS;i++)
    {
        entryArray[i] = (GT_U32)inFields[i];
    }

    /* call cpss api function */
    result = cpssExMxBrgFdbMacAddrRawEntryWrite(devNum, index, entryArray);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* Function: cpssExMxBrgFdbTrunkAgingModeSet
*
* DESCRIPTION:
*       Sets bridge FDB Aging Mode for trunk entries.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum  - the device number.
*       trunkAgingMode - FDB aging mode for trunk entries.
*            CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E:
*                 In Controlled Aging mode, the device ages Trunk entries
*                 whether it is the owner or not. When a Trunk entry is aged,
*                 it is not deleted from the MAC table, but an Aged
*                 Address (AA) message is sent to the CPU. This allows
*                 the CPU to track whether the MAC has been aged on all
*                 devices that have port members for the Trunk. Only then
*                 the CPU explicitly deletes the Trunk entry on all
*                 devices.
*            CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E:
*                 This is Automatic Trunk Entry Aging mode, which treats
*                 Trunk entries in the MAC table just like non-Trunk entries.
*                 That is, aging is performed only on entries owned by the
*                 device. The advantage of this mode is that it does not
*                 require any CPU intervention. This mode has the
*                 disadvantage of possibly aging a Trunk entry on all devices
*                 when there still may be active traffic for the MAC on a
*                 different device, where packets were sent over another
*                 link of the Trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or aging mode
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxBrgFdbTrunkAgingModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                        devNum;
    CPSS_FDB_AGE_TRUNK_MODE_ENT  trunkAgingMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkAgingMode = (CPSS_FDB_AGE_TRUNK_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgFdbTrunkAgingModeSet(devNum, trunkAgingMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgFdbAuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of AU messages , the max number of
*       elements defined by the caller
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*
* RETURNS:
*       GT_OK       - on success
*       GT_NO_MORE  - the action succeeded and there are no more waiting
*                     AU messages
*
*       GT_FAIL     - on failure
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbAuMsgBlockGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_U32  numOfAuPtr;
    CPSS_EXMX_ADDR_UP_MSG_STC auMessagesPtr;
    GT_U8                          dummyPort = 0;/* dummy port num for conversion */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    numOfAuPtr = 1;
    cmdOsMemSet((void*)&auMessagesPtr, 0, sizeof(auMessagesPtr));

    /* call cpss api function */
    result = cpssExMxBrgFdbAuMsgBlockGet(devNum, &numOfAuPtr, &auMessagesPtr);


    if (result != GT_OK)
    {
        if (result == GT_NO_SUCH)
            galtisOutput(outArgs, GT_OK, "%d", -1);
        else
            galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] =  auMessagesPtr.messageType;
    inFields[1] =  auMessagesPtr.messageData.macUpdate.aging;
    CONVERT_BACK_DEV_PORT_DATA_MAC(auMessagesPtr.messageData.macUpdate.associatedDevNum,
                             dummyPort);
    inFields[2] =  auMessagesPtr.messageData.macUpdate.associatedDevNum;
    inFields[3] =  auMessagesPtr.messageData.macUpdate.entryWasFound;
    inFields[4] =  auMessagesPtr.messageData.macUpdate.macEntry.cosIpv4En;
    inFields[5] =  auMessagesPtr.messageData.macUpdate.macEntry.daCib;
    inFields[6] =  auMessagesPtr.messageData.macUpdate.macEntry.daClass;
    inFields[7] =  auMessagesPtr.messageData.macUpdate.macEntry.daCommand;
    inFields[8] =  auMessagesPtr.messageData.macUpdate.macEntry.daRoute;
    CONVERT_BACK_DEV_PORT_DATA_MAC(auMessagesPtr.messageData.macUpdate.macEntry.dstInterface.devPort.devNum,
                         auMessagesPtr.messageData.macUpdate.macEntry.dstInterface.devPort.portNum);
    inFields[9] =  auMessagesPtr.messageData.macUpdate.macEntry.dstInterface.devPort.devNum;
    inFields[10] =  auMessagesPtr.messageData.macUpdate.macEntry.dstInterface.devPort.portNum;

    inFields[11] =  auMessagesPtr.messageData.macUpdate.macEntry.dstInterface.trunkId;
    inFields[12] =  auMessagesPtr.messageData.macUpdate.macEntry.dstInterface.type;
    inFields[13] =  auMessagesPtr.messageData.macUpdate.macEntry.dstInterface.vidx;
    inFields[14] =  auMessagesPtr.messageData.macUpdate.macEntry.dstTc;
    inFields[15] =  auMessagesPtr.messageData.macUpdate.macEntry.isStatic;
    inFields[16] =  auMessagesPtr.messageData.macUpdate.macEntry.mirrorToRxAnalyzerPortEn;
    inFields[17] =  auMessagesPtr.messageData.macUpdate.macEntry.saCib;
    inFields[18] =  auMessagesPtr.messageData.macUpdate.macEntry.saClass;
    inFields[19] =  auMessagesPtr.messageData.macUpdate.macEntry.saCommand;
    inFields[20] =  0;
    inFields[21] =  auMessagesPtr.messageData.macUpdate.macEntry.srcTc;
    inFields[22] =  0;
    inFields[23] =  auMessagesPtr.messageData.macUpdate.macEntry.vlanId;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],
                inFields[1], inFields[2], inFields[3],
                auMessagesPtr.messageData.macUpdate.macEntry.macAddr.arEther, inFields[4],
                inFields[5], inFields[6], inFields[7], inFields[8],
                inFields[9], inFields[10], inFields[11], inFields[12],
                inFields[13], inFields[14], inFields[15], inFields[16],
                inFields[17], inFields[18], inFields[19], inFields[20],
                inFields[21], inFields[22], inFields[23]);

    galtisOutput(outArgs, result, "%f");


    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgFdbInit
*
* DESCRIPTION:
*       initialize the FDB lib for the device
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev         - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgFdbInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxBrgFdbInit(dev);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgStpInit
*
* DESCRIPTION:
*       Initialize STP library.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgStpInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxBrgStpInit(dev);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxBrgStpModeSet
*
* DESCRIPTION:
*       Sets STP mode: SST (SINGLE STP) or MST (MULTIPLE STP).
*       In SST mode, each local port maintains its Spanning Tree port state.
*       In MST mode, the Spanning Tree state is maintained per-port per-VLAN,
*       and is stored in the VLAN Table entry.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       mode    - STP mode
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       Write to the PerVLANSpanEn field in the L2 ingress control register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgStpModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                    dev;
    CPSS_EXMX_STP_MODE_ENT   mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mode = (CPSS_EXMX_STP_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgStpModeSet(dev, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxBrgStpPortStateSet
*
* DESCRIPTION:
*       Sets STP state for specified port, in case per-VLAN SPT is disabled.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev      - hpysical device number
*       port     - physical port number
*       state    - STP port state.
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
*       Write to the STPortState field in the Bridge Port control Registers.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgStpPortStateSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                    dev;
    GT_U8                    port;
    CPSS_STP_STATE_ENT       state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    state = (CPSS_STP_STATE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxBrgStpPortStateSet(dev, port, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxBrgStpStateSet
*
* DESCRIPTION:
*       Sets STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - device number
*       port   - port number
*       stpId  - STP Group Id
*       state  - STP port state.
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
*       Should be used only for MST mode.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgStpStateSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                    dev;
    GT_U8                    port;
    GT_U16                   stpId;
    CPSS_STP_STATE_ENT       state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    stpId = (GT_U16)inArgs[2];
    state = (CPSS_STP_STATE_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxBrgStpStateSet(dev, port, stpId, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxBrgStpEntryGet
*
* DESCRIPTION:
*       Gets the STP entry from HW.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev             - PP number
*       stpId           - multicast group index
*
* OUTPUTS:
*       stpEntryPtr     - the stp entry
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*
* COMMENTS:
*
*******************************************************************************/
#if 0
static CMD_STATUS wrcpssExMxBrgStpEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U16  stpId;
    GT_U32  stpEntryPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    stpId = (GT_U16)inArgs[1];

    cmdOsMemSet((void*)&stpEntryPtr, 0, sizeof(stpEntryPtr));

    /* call cpss api function */
    result = cpssExMxBrgStpEntryGet(dev, stpId, &stpEntryPtr);

    stpEntryPtr.

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", stpEntryPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxBrgStpEntryWrite
*
* DESCRIPTION:
*       Writes the STP entry to HW.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev             - PP number
*       stpId           - multicast group index
*       stpEntryPtr     - the stp entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgStpEntryWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U16  stpId;
    GT_U32  stpEntryPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    stpId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgStpEntryWrite(dev, stpId, &stpEntryPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
#endif
/*******************************************************************************
* wrcpssExMxBrgVlanInit
*
* DESCRIPTION:
*       Initialize VLAN_PP_DATA structure fields for specific device.
*
* INPUTS:
*       devNum          - PP number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on system init error.
*       GT_OUT_OF_CPU_MEM - on malloc failed
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxBrgVlanInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgVlanMaxVidSet
*
* DESCRIPTION:
*       Sets the maximal Vid for an incoming tagged packet.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum - physical device number
*       maxVid - Vlan Id.
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
*       This function sets the vid range for etherType 0. If nested vlan is
*       used, the vid range should be configured also for etherType 1 and
*       in this case user should use the function coreVlanSetEtherType.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanMaxVidSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   maxVid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    maxVid = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgVlanMaxVidSet(devNum, maxVid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*cpssExMxBrgVlan Table*/
static GT_U16   vidIter;

/*******************************************************************************
* wrcpssExMxBrgVlanEntryWrite
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW .
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum          - PP number
*       vid             - VLAN Id
*       portsMembers    - bmp of ports members in vlan
*       portsTagging    - bmp of ports tagged in the vlan
*       vlanInfoPtr     - VLAN specific information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanEntryWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vid;
    CPSS_PORTS_BMP_STC portsMembers;
    CPSS_PORTS_BMP_STC portsTagging;
    CPSS_VLAN_INFO_STC vlanInfo;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vid = (GT_U16)inFields[0];
    portsMembers.ports[0] = (GT_U32)inFields[1];
    portsMembers.ports[1] = (GT_U32)inFields[2];
    portsTagging.ports[0] = (GT_U32)inFields[3];
    portsTagging.ports[1] = (GT_U32)inFields[4];
    vlanInfo.hasLocals = (GT_BOOL)inFields[5];
    vlanInfo.hasUplinks = (GT_BOOL)inFields[6];
    vlanInfo.isCpuMember = (GT_BOOL)inFields[7];
    vlanInfo.learnEnable = (GT_BOOL)inFields[8];
    vlanInfo.unregMcFilterCmd = (GT_U32)inFields[9];
    vlanInfo.perVlanSaMacEnabled = (GT_BOOL)inFields[10];
    vlanInfo.saMacSuffix = (GT_U8)inFields[11];
    vlanInfo.stpId = (GT_U32)inFields[12];

    /* call cpss api function */
    result = cpssExMxBrgVlanEntryWrite(devNum, vid, &portsMembers,
                                    &portsTagging, &vlanInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgVlanEntryRead
*
* DESCRIPTION:
*       Read vlan entry.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum          - PP number
*       vid             - VLAN Id
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) VLAN entry status
*                           GT_TRUE = Vlan is valid
*                           GT_FALSE = Vlan is not Valid
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8              devNum;
    CPSS_PORTS_BMP_STC portsMembers;
    CPSS_PORTS_BMP_STC portsTagging;
    CPSS_VLAN_INFO_STC vlanInfo;
    GT_BOOL            isValid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vidIter = 1;

    /* call cpss api function */
    result = cpssExMxBrgVlanEntryRead(devNum,vidIter,&portsMembers,
                                      &portsTagging,&vlanInfo,&isValid);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(isValid)
    {
        inFields[0] = vidIter;
        inFields[1] = portsMembers.ports[0];
        inFields[2] = portsMembers.ports[1];
        inFields[3] = portsTagging.ports[0];
        inFields[4] = portsTagging.ports[1];
        inFields[5] = vlanInfo.hasLocals;
        inFields[6] = vlanInfo.hasUplinks;
        inFields[7] = vlanInfo.isCpuMember;
        inFields[8] = vlanInfo.learnEnable;
        inFields[9] = vlanInfo.unregMcFilterCmd;
        inFields[10] = vlanInfo.perVlanSaMacEnabled;
        inFields[11] = vlanInfo.saMacSuffix;
        inFields[12] = vlanInfo.stpId;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1],
                    inFields[2], inFields[3], inFields[4], inFields[5],
                    inFields[6], inFields[7], inFields[8], inFields[9],
                    inFields[10], inFields[11], inFields[12]);

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
       galtisOutput(outArgs, result, "");
    }
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxBrgVlanEntryRead
*
* DESCRIPTION:
*       Read vlan entry.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum          - PP number
*       vid             - VLAN Id
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) VLAN entry status
*                           GT_TRUE = Vlan is valid
*                           GT_FALSE = Vlan is not Valid
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8              devNum;
    CPSS_PORTS_BMP_STC portsMembers;
    CPSS_PORTS_BMP_STC portsTagging;
    CPSS_VLAN_INFO_STC vlanInfo;
    GT_BOOL            isValid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vidIter++;

    if(vidIter >= 4096)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxBrgVlanEntryRead(devNum,vidIter,&portsMembers,
                                      &portsTagging,&vlanInfo,&isValid);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(isValid)
    {
        inFields[0] = vidIter;
        inFields[1] = portsMembers.ports[0];
        inFields[2] = portsMembers.ports[1];
        inFields[3] = portsTagging.ports[0];
        inFields[4] = portsTagging.ports[1];
        inFields[5] = vlanInfo.hasLocals;
        inFields[6] = vlanInfo.hasUplinks;
        inFields[7] = vlanInfo.isCpuMember;
        inFields[8] = vlanInfo.learnEnable;
        inFields[9] = vlanInfo.unregMcFilterCmd;
        inFields[10] = vlanInfo.perVlanSaMacEnabled;
        inFields[11] = vlanInfo.saMacSuffix;
        inFields[12] = vlanInfo.stpId;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1],
                    inFields[2], inFields[3], inFields[4], inFields[5],
                    inFields[6], inFields[7], inFields[8], inFields[9],
                    inFields[10], inFields[11], inFields[12]);

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
       galtisOutput(outArgs, result, "");
    }
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgVlanMemberAdd
*
* DESCRIPTION:
*       Add new port member to vlan entry. This function can be called only for
*       add port belongs to device that already member of the vlan.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum          - PP number
*       vid             - VLAN Id
*       portNum         - physical port number
*       isTagged        - GT_TRUE, to set the port as tagged member,
*                         GT_FALSE, to set the port as untagged
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       In case that added port belongs to device that is new device for vlan
*       other function is used.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanMemberAdd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vid;
    GT_U8   portNum;
    GT_BOOL   isTagged;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vid = (GT_U16)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    isTagged = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxBrgVlanMemberAdd(devNum, vid, portNum, isTagged);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanPortDelete
*
* DESCRIPTION:
*       Delete port member from vlan entry.
*       If the deleted port is a last port from specified device belongs to the
*       vlan, devices bitmap in PP adapters will be updated.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum      - PP number
*       vid         - VLAN
*       port        - port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanPortDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vid;
    GT_U8   port;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vid = (GT_U16)inArgs[1];
    port = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxBrgVlanPortDelete(devNum, vid, port);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgVlanHasUplinksSet
*
* DESCRIPTION:
*       Set the device bit specifiwing if the device has uplinks.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum  - PP number
*       vid     - vlan number
*       hasUplinks - GT_FALSE:This VLAN has no port members residing
*                             on a remote device. bit value=1
*                    GT_TRUE: This VLAN has port members residing on a
*                             local device. bit value=0
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanHasUplinksSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vid;
    GT_BOOL   hasUplinks;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vid = (GT_U16)inArgs[1];
    hasUplinks = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxBrgVlanHasUplinksSet(devNum, vid, hasUplinks);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanHasLocalsSet
*
* DESCRIPTION:
*       Set the device bit specifiwing if the device has uplinks.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum     - PP number
*       vid        - vlan number
*       hasLocals  - GT_FALSE:This VLAN does not have any local member ports
*                             on this device. bit value=1
*                    GT_TRUE: This VLAN has one or more local port members
*                             on this device. bit value=0
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanHasLocalsSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vid;
    GT_BOOL   hasLocals;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vid = (GT_U16)inArgs[1];
    hasLocals = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxBrgVlanHasLocalsSet(devNum, vid, hasLocals);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgVlanPortPvidGet
*
* DESCRIPTION:
*       Read default port VLAN Id from Port Vid Reg.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum   - PP number
*       port     - port number
*
* OUTPUTS:
*       vid   - default VLAN ID.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanPortPvidGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8   port;
    GT_U16   vid;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxBrgVlanPortPvidGet(devNum, port, &vid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vid);
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanPortPvidSet
*
* DESCRIPTION:
*       Set default port VLAN Id to Port Vid Reg.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum   - PP number
*       port     - port number
*       vid      - VLAN Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanPortPvidSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8   port;
    GT_U16   vid;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    vid = (GT_U16)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxBrgVlanPortPvidSet(devNum, port, vid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanPortTcGet
*
* DESCRIPTION:
*       Read default traffic class from Port Control Reg.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum   - PP number
*       port     - port number
*
* OUTPUTS:
*       vid   - default VLAN ID.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanPortTcGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8   port;
    GT_U8   trafClass;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxCosPortDefaultTcGet(devNum, port, &trafClass);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", trafClass);
    return CMD_OK;
}



/*******************************************************************************
* wrcpssExMxBrgVlanPortIngFltEnable
*
* DESCRIPTION:
*       Enable/disable PortIngressFiltering for specific port
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum   - PP number
*       port     - port number
*       value    - GT_TRUE, enable secure VLAN;
*                  GT_FALSE, disable ingress filtering
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       Write secure VLAN bit into Port L2i Control Reg.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanPortIngFltEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8   port;
    GT_BOOL   value;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    value = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxBrgVlanPortIngFltEnable(devNum, port, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanIpClassEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4 Subnet based VLAN classification for specific
*       port and device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum   - PP number
*       port     - port number
*       value    - GT_TRUE for enable, otherwise GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanIpClassEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8   port;
    GT_BOOL   value;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    value = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxBrgVlanIpClassEnable(devNum, port, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgVlanSubnetClassEntrySet
*
* DESCRIPTION:
*       Add / Delete Ip address from the subnet classification table.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum      - PP number
*       entryNum    - entry number
*       sip         - source Ip address
*       ipPrefix    - number of consecutive '1' in the Subnet mask.
*       addDel      - GT_TRUE to add this entry, GT_FALSE to delete it.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanSubnetClassEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8   entryNum;
    GT_IPADDR   sip;
    GT_U8   ipPrefix;
    GT_BOOL   addDel;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryNum = (GT_U8)inArgs[1];
    galtisIpAddr(&sip, (GT_U8*)inArgs[2]);
    ipPrefix = (GT_U8)inArgs[3];
    addDel = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssExMxBrgVlanSubnetClassEntrySet(devNum, entryNum, sip, ipPrefix, addDel);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanSubnetClassVidSet
*
* DESCRIPTION:
*       Write default vlan ID of IPv4 Subnet based Vlan classification entry
*       for specific device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum    - PP number
*       entryNum  - entry number
*       vlanId    - vlan Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanSubnetClassVidSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8   entryNum;
    GT_U16   vlanId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryNum = (GT_U8)inArgs[1];
    vlanId = (GT_U16)inArgs[2];

    /* call cpss api function */
    result = cpssExMxBrgVlanSubnetClassVidSet(devNum, entryNum, vlanId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanSubnetClassTcSet
*
* DESCRIPTION:
*       Write traffic class of IPv4 Subnet based Vlan classification entry
*       for specific device.
*       Set specific IPv4 Subnet based classification entry to be valid.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum    - PP number
*       entryNum  - entry number
*       trafClass - traffic class.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanSubnetClassTcSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8   entryNum;
    GT_U8   trafClass;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryNum = (GT_U8)inArgs[1];
    trafClass = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxBrgVlanSubnetClassTcSet(devNum, entryNum, trafClass);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgVlanSubnetClassEntryEnable
*
* DESCRIPTION:
*       Enable IPv4 Subnet based classification specific entry
*       for specific device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum    - PP number
*       entryNum  - entry number
*       value     - GT_TRUE, enable entry;
*                   GT_FALSE, disable entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanSubnetClassEntryEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8   entryNum;
    GT_BOOL   value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryNum = (GT_U8)inArgs[1];
    value = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxBrgVlanSubnetClassEntryEnable(devNum, entryNum, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanProtoClassEnable
*
* DESCRIPTION:
*       Enable/Disable Protocol based classification for specific port and
*       device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum   - PP number
*       port     - port number
*       value    - GT_TRUE for enable, otherwise GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanProtoClassEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8   port;
    GT_BOOL   value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    value = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxBrgVlanProtoClassEnable(devNum, port, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgVlanProtoClassGet
*
* DESCRIPTION:
*       Read etherType and encapsulation of Protocol based classification
*       for specific device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum    - PP number
*       entryNum  - entry number
*       etherType - Ether Type or DSAP/SSAP
*       encList   - encapsulation.
*       portConf  - configuratin of logical port to be bind
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanProtoClassGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_32   entryNum;
    GT_U16   etherType;
    CPSS_PROT_CLASS_ENCAP_STC   encList;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryNum = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgVlanProtoClassGet(devNum, entryNum, &etherType, &encList);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", etherType, encList.ethV2,
                                    encList.nonLlcSnap, encList.llcSnap);
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanProtoClassSet
*
* DESCRIPTION:
*       Write etherType and encapsulation of Protocol based classification
*       for specific device. Bind the specified port and it associated
*       VID/Traffic class to the protocol entry.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum    - PP number
*       entryNum  - entry number
*       etherType - Ether Type or DSAP/SSAP
*       encList   - encapsulation.
*       portConf  - configuratin of logical port to be bind
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanProtoClassSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_32   entryNum;
    GT_U16   etherType;
    CPSS_PROT_CLASS_ENCAP_STC   encList;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryNum = (GT_U8)inArgs[1];
    etherType = (GT_U16)inArgs[2];
    encList.ethV2 = (GT_BOOL)inArgs[3];
    encList.nonLlcSnap = (GT_BOOL)inArgs[4];
    encList.llcSnap = (GT_BOOL)inArgs[5];

     /* call cpss api function */
    result = cpssExMxBrgVlanProtoClassSet(devNum, entryNum, etherType, &encList);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxBrgVlanVidProtoSet
*
* DESCRIPTION:
*       Write Vlan Id of Protocol based classification
*       entry for specific device and port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum              - PP number
*       port                - port number
*       entryNum            - entry number
*       vlanId              - vlan Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanVidProtoSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    port;
    GT_32   entryNum;
    GT_U16   vlanId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    entryNum = (GT_32)inArgs[2];
    vlanId = (GT_U16)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxBrgVlanVidProtoSet(devNum, port, entryNum, vlanId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgVlanTcProtoSet
*
* DESCRIPTION:
*       Write traffic class of Protocol based classification
*       entry for specific device and port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum              - PP number
*       port                - port number
*       entryNum            - entry number
*       trafClass           - traffic class.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanTcProtoSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    port;
    GT_32   entryNum;
    GT_U8   trafClass;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    entryNum = (GT_32)inArgs[2];
    trafClass = (GT_U8)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxBrgVlanTcProtoSet(devNum, port, entryNum, trafClass);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanLearningStateSet
*
* DESCRIPTION:
*       Sets state of VLAN based learning to specified VLAN on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum    - physical device number
*       vlanId    - vlan Id for MST or ZERO for SST
*       status    - GT_TRUE for enable  or GT_FALSE otherwise.
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
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanLearningStateSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16    vid;
    GT_BOOL   status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vid = (GT_U16)inArgs[1];
    status = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxBrgVlanLearningStateSet(devNum, vid, status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgVlanMcLikeEntryWrite
*
* DESCRIPTION:
*       Builds and writes MC group entry in format of vlan entry to HW table
*       in VLAN Table memory space.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       vid             - VLAN Id
*       devNum          - PP number
*       isCpuMember     - if GT_TRUE, CPU is a member of this multicast group
*       portBitmapPtr   - bitmap of ports on specified device that
*                         belong to mc group
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
#if 0
static CMD_STATUS wrcpssExMxBrgVlanMcLikeEntryWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16    vid;
    GT_BOOL   isCpuMember;
    CPSS_PORTS_BMP_STC   portBitmapPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vid = (GT_U16)inArgs[1];
    isCpuMember = (GT_BOOL)inArgs[2];
    portBitmapPtr.ports[2] = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxBrgVlanMcLikeEntryWrite(devNum, vid, isCpuMember, &portBitmapPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
#endif

/*******************************************************************************
* wrcpssExMxBrgVlanIngressFilteringStatusGet
*
* DESCRIPTION:
*       Get Vid and source port number that caused VLAN Ingress
*       filtering of the packet.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum - physical device numbers
*
* OUTPUTS:
*       causeVid  - Vid that caused VLAN Ingress filtering of the packet.
*       causePort - physical port number that caused VLAN Ingress filtering
*                   of the packet.
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
static CMD_STATUS wrcpssExMxBrgVlanIngressFilteringStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16    causeVid;
    GT_U8   causePort;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxBrgVlanIngressFilteringStatusGet(devNum, &causeVid, &causePort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", causeVid, causePort);
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanIsDevMember
*
* DESCRIPTION:
*       Checks if specified device has members belong to specified vlan
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum  - physical device number
*       vidx    - multicast group
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE   - if port is member of vlan
*       GT_FALSE  - if port does not member of vlan
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanIsDevMember
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BOOL result;

    GT_U8    devNum;
    GT_U16    vlanId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgVlanIsDevMember(devNum, vlanId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgVlan2StpIdBind
*
* DESCRIPTION:
*       Bind VLAN to STP Id.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum  - device Id
*       vlanId  - vlan Id
*       stpId   - STP Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_HW_ERROR         - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlan2StpIdBind
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16    vlanId;
    GT_U16    stpId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    stpId = (GT_U16)inArgs[2];

    /* call cpss api function */
    result = cpssExMxBrgVlan2StpIdBind(devNum, vlanId, stpId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanStpIdGet
*
* DESCRIPTION:
*       Read STP Id that bind to specified VLAN in Multiple STP mode.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       vlanId  - vlan Id
*
* OUTPUTS:
*       stpId   - STP Id
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_HW_ERROR         - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanStpIdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16    vlanId;
    GT_U16    stpId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgVlanStpIdGet(devNum, vlanId, &stpId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", stpId);
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgVlanEtherTypeSet
*
* DESCRIPTION:
*       Sets vlan Ether Type and vlan range in appropriate register.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum        - physical device number
*       etherTypeMode - mode of tagged vlan etherType. May be either
*                       CPSS_VLAN_ETHERTYPE0_E or CPSS_VLAN_ETHERTYPE1_E.
*                       See comment.
*       ethertype     - the EtherType to be recognized tagged packets. See
*                       comment.
*       vidRange      - the maximum vlan range to accept for the specified
*                       etheType.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM- on illegal vidRange value
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       If the vid of the incoming packet is greater or equal (>=) than the vlan
*       range value, the packet is filtered.
*
*       Configuration of vid and range for CPSS_VLAN_ETHERTYPE1_E should be done
*       only when nested vlan is used. User can alter the etherType field of
*       the encapsulating vlan by setting a number other then 0x8100 in the
*       parameter <ethertype>.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanEtherTypeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_ETHER_MODE_ENT etherTypeMode;
    GT_U16              etherType;
    GT_U16              vidRange;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    etherTypeMode = (CPSS_ETHER_MODE_ENT)inArgs[1];
    etherType = (GT_U16)inArgs[2];
    vidRange = (GT_U16)inArgs[3];

    /* call cpss api function */
    result = cpssExMxBrgVlanEtherTypeSet(devNum, etherTypeMode, etherType, vidRange);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgVlanPortTagModeSet
*
* DESCRIPTION:
*       Set the tagging mode for packets transmitted via port for
*       specific vlan
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum    - pp Device number
*       vlanId    - VLAN id
*       portNum - port number
*       tagged - if GT_TRUE, packets are transmitted as tagged
*                if GT_FALSE, packets are transmitted as untagged on the port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success,
*       GT_FAIL          - otherwise.
*       GT_BAD_PARAM     - on bad parameters,
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanPortTagModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  devNum;
    GT_U16 vlanId;
    GT_U8 portNum;
    GT_BOOL tagged;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    tagged = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxBrgVlanPortTagModeSet(devNum, vlanId, portNum, tagged);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanTableInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN Table entries by writing 0 to the
*       first word.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum  - physical device number.
*       vid     - VLAN id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       !!!!!!!! Assumed that Vlan and mcmac tables are continuous !!!!!!!
*       !!!!!!!! first Vlan table then from its end starts Mcmac table !!!!!!!
*******************************************************************************/
#if 0
static CMD_STATUS wrcpssExMxBrgVlanTableInvalidate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxBrgVlanTableInvalidate(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
#endif

/*******************************************************************************
* wrcpssExMxBrgVlanEntryInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN entry by writing 0 to the
*       first word.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum     - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanEntryInvalidate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  devNum;
    GT_U16   vid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vid = (GT_U16)inFields[0];

    /* call cpss api function */
    result = cpssExMxBrgVlanEntryInvalidate(devNum, vid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanMacPerVlanSet
*
* DESCRIPTION:
*       This function
*       This function should set bits [10:5] Vlan_Mac_Sa of the VLAN table
*       Entry and should mark the Vlan_Mac_Sa_En [11] in the same entry.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*     vid             - VLAN Id
*     saMac           - The 6 bit SA mac value to be written to the SA bits in
*                       the VLAN entry if saMacEn = GT_TRUE
*     saMacEn         - Enable/Disable use of SA mac for this VLAN entry.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_SUPPORTED - if the feature does not supported by specified device
*     GT_FAIL          - on error.
*     GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*     In case that added port belongs to device that is new device for vlan
*     other function is used.
*     Support Twist devices.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanMacPerVlanSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  devNum;
    GT_U16   vlanId;
    GT_U8   saMac;
    GT_BOOL   saMacEn;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    saMac = (GT_U8)inArgs[2];
    saMacEn = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxBrgVlanMacPerVlanSet(devNum, vlanId, saMac, saMacEn);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrcpssExMxBrgVlanUnregMcFilterSet
*
* DESCRIPTION:
*       Sets Filtering command for forwarding an unknown multicast/broadcast
*       packets for the specified VLAN.
*
* APPLICABLE DEVICES:  All ExMx devices, except SALSA
*
* INPUTS:
        devNum           - device number
*       vlanId           - the VLAN-ID to which the filter command is to be
*                          applied
*       unregMcFilterCmd - the unregistered filter command
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       This filter applies to bridge unregistered multicast/broadcast packets
*       that are not otherwise dropped or trapped to the CPU by other
*       ingress pipline mechanism.
*
*       Write to the VUnregistered MCCommand field in the VLAN Table Entry.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanUnregMcFilterSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                 devNum;
    GT_U16                                vlanId;
    CPSS_UNREG_MC_EGR_FILTER_CMD_ENT unregMcFilterCmd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    unregMcFilterCmd = (CPSS_UNREG_MC_EGR_FILTER_CMD_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxBrgVlanUnregMcFilterSet(devNum, vlanId, unregMcFilterCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrcpssExMxBrgVlanUnregMcEnable
*
* DESCRIPTION:
*       Enables/Disables unregistered multicast/broadcast filtering on a
*       per-VLAN basis.
*
* APPLICABLE DEVICES:  All ExMx devices, except SALSA
*
* INPUTS:
*       devNum                      - device number
*       unregMcFilterPerVlanEnable  -
*           GT_TRUE = Unregistered Multicast command is per VLAN.
*           GT_FALSE = Unregistered Multicast command is taken from the
*               <Unreg MulticastCmd> field in the Layer 2 Ingress Control Register
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       Write to the VlanUnregMcCmdEn field in the Egress Configuration Register 1.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgVlanUnregMcEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     devNum;
    GT_BOOL   unregMcFilterPerVlanEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    unregMcFilterPerVlanEnable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgVlanUnregMcEnable(devNum, unregMcFilterPerVlanEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxBrgMcInit
*
* DESCRIPTION:
*       Initialize MC_PP_DATA structure fields for specific device.
*
* INPUTS:
*       devNum          - PP number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on system init error.
*       GT_OUT_OF_CPU_MEM - on malloc failed
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgMcInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxBrgMcInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgMcEntryWrite
*
* DESCRIPTION:
*       Builds and writes Multicast Group entry to HW.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum          - PP number
*       vidx            - multicast group index
*       isCpuMember     - if GT_TRUE, CPU is a member of this multicast group
*       portBitmapPtr   - bitmap of ports on specified device that
*                         belong to mc group
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - if vidx is larger than the allowed value
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       This function always reads 4 bytes (at least), modifies the appropriate
*       bits and writes back.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgMcEntryWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                devNum;
    GT_U16               vidx;
    GT_BOOL              isCpuMember;
    CPSS_PORTS_BMP_STC   portBitmapPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&portBitmapPtr, 0, sizeof(portBitmapPtr));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vidx = (GT_U16)inFields[0];
    isCpuMember = (GT_BOOL)inFields[1];
    portBitmapPtr.ports[0] = (GT_U32)inFields[2];
    portBitmapPtr.ports[1] = (GT_U32)inFields[3];


    /* call cpss api function */
    result = cpssExMxBrgMcEntryWrite(devNum, vidx, isCpuMember, &portBitmapPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgMcMemberAdd
*
* DESCRIPTION:
*       Add new port member to mc group entry.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum          - PP number
*       vidx            - multicast group index
*       portNum         - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       For add CPU to Multicast group members list portNum should be
*       CPU_PORT_NUM.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgMcMemberAdd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vidx;
    GT_U8    portNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vidx = (GT_U16)inArgs[1];
    portNum = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxBrgMcMemberAdd(devNum, vidx, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgMcMemberDelete
*
* DESCRIPTION:
*       Delete port member from Mc group entry.
*       If the deleted port is a last port from specified device belongs to the
*       Mc group, devices bitmap in PP adapters will be updated.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum      - PP number
*       vidx         - multicast group index
*       portNum     - physical port number
*       devBitmap   - bitmap of all devices belong to vlan
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       For delete CPU from Multicast group members list portNum should be
*       CPU_PORT_NUM.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgMcMemberDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vidx;
    GT_U8    portNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vidx = (GT_U16)inArgs[1];
    portNum = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxBrgMcMemberDelete(devNum, vidx, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxBrgMcGroupDelete
*
* DESCRIPTION:
*       Delete a specified Multicast group from system.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum  - device number
*       vidx     - multicast group index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgMcGroupDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vidx;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vidx = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgMcGroupDelete(devNum, vidx);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgMcEntryGet
*
* DESCRIPTION:
*       Gets the Multicast entry from HW.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum          - PP number
*       vidx            - multicast group index
*
* OUTPUTS:
*       entryPtr        - the multicast entry
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*
* COMMENTS:
*
*******************************************************************************/
#if 0
static CMD_STATUS wrcpssExMxBrgMcEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    static GT_U16   vidx = 0;
    GT_U32   entry[2];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxBrgMcEntryGet(devNum, vidx, entry);


    if (result != GT_OK)
    {
        if (result == GT_NO_SUCH)
            galtisOutput(outArgs, GT_OK, "%d", -1);
        else
            galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", vidx, inFields[1], entry[0] , entry[1]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*===============================================================================*/

static CMD_STATUS wrcpssExMxBrgMcEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    static GT_U16   vidx;
    GT_U32   entry[2];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    vidx++;

    /* call cpss api function */
    result = cpssExMxBrgMcEntryGet(devNum, vidx, entry);


    if (result != GT_OK)
    {
        if (result == GT_NO_SUCH)
            galtisOutput(outArgs, GT_OK, "%d", -1);
        else
            galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", vidx, inFields[1], entry[0] , entry[1]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
#endif

/*===============================================================================*/
/*===============================================================================*/


/*******************************************************************************
* cpssExMxBrgNestVlanEnable
*
* DESCRIPTION:
*       Enable/disable the nested Vlan mode on specidied device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       enable - GT_TRUE for enable nested vlan mode,
*                GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       Write to the EnNestedVLAN field in the L2 ingress control register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgNestVlanEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgNestVlanEnable(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgNestVlanPortStateSet
*
* DESCRIPTION:
*       Sets the nested vlan port state.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       state  - the port state. (customer or core)
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
*       Write to the VLANSel and CustomerPort fields
*       in the Bridge Port <n> Control register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgNestVlanPortStateSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                               dev;
    GT_U8                               port;
    CPSS_EXMX_NESTED_PORT_STATE_ENT     state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    state = (CPSS_EXMX_NESTED_PORT_STATE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxBrgNestVlanPortStateSet(dev, port, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*===============================================================================*/
/*===============================================================================*/


/*******************************************************************************
* cpssExMxBrgPrvEdgeVlanEnable
*
* DESCRIPTION:
*       This function enables/disables the Private Edge VLAN on
*       specified device
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE to enable the feature,
*                GT_FALSE to disable the feature
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_NOT_SUPPORTED - request is not support on current device
*
* COMMENTS:
*       Write to the EnPrivateEdgeVLAN field in the L2 ingress control register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgPrvEdgeVlanEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                               devNum;
    GT_BOOL                               enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgPrvEdgeVlanEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgPrvEdgeVlanPortEnable
*
* DESCRIPTION:
*       Enable/Disable a specified port to operate in Private Edge VLAN mode.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       enable    - GT_TRUE for enabled, GT_FALSE for disabled
*       srcPort   - physical port number to set state.
*       srcDev    - physiacl device number to set state.
*       dstPort   - the destination "uplink" physical port to which all traffic
*                   received on srcPort&srcDev is forwarded.  This parameter
*                   is ignored if disabling the mode.
*       dstDev    - the destination "uplink" physical device to which all
*                   traffic received on srcPort&srcDev is forwarded.  This
*                   parameter is ignored if disabling the mode.
*       dstTrunk  - the destination is a trunk member
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_NOT_SUPPORTED - request is not support on devices
*
* COMMENTS:
*       Write to the Port <n> PVLAN Register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgPrvEdgeVlanPortEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;


    GT_U8      devNum;
    GT_U8      portNum;
    GT_BOOL    enable;
    GT_U8      dstPort;
    GT_U8      dstDev;
    GT_BOOL    dstTrunk;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    enable = (GT_BOOL)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    devNum = (GT_U8)inArgs[2];
    dstPort = (GT_U8)inArgs[3];
    dstDev = (GT_U8)inArgs[4];
    dstTrunk = (GT_BOOL)inArgs[5];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxBrgPrvEdgeVlanPortEnable(devNum, portNum, enable,
                                                dstPort, dstDev, dstTrunk);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*===============================================================================*/
/*===============================================================================*/


/*******************************************************************************
* cpssExMxBrgGenBrdPortLimitSet
*
* DESCRIPTION:
*       Enable limiting the rate of broadcasts received on a port on
*       a specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       rMode  - rate calculation mode (packet or byte based)
*       rValue - permitted broadcast rate value (in packets or bytes)
*                This counts in quantities of 64K.
*       window - time window for calculating the rate (microseconds)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_OUT_OF_RANGE  - if rateValue or  window is out of range
*
* COMMENTS:
*       Write Bridge Port <n> Control Register and Port Broadcast Rate
*       Limit Registers.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgGenBrdPortLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       dev;
    GT_U8                       port;
    CPSS_RATE_LIMIT_MODE_ENT    rMode;
    GT_U32                      rateValue;
    GT_U32                      window;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    rMode = (CPSS_RATE_LIMIT_MODE_ENT)inArgs[2];
    rateValue = (GT_U32)inArgs[3];
    window = (GT_U32)inArgs[4];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxBrgGenBrdPortLimitSet(dev, port, rMode, rateValue, window);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgGenBcLimitDisable
*
* DESCRIPTION:
*       Disable limiting the rate of broadcasts received on a port of
*       specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
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
*       Write to the EnBCRL field in the Bridge Port <n> Control Register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgGenBcLimitDisable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                    dev;
    GT_U8                    port;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxBrgGenBcLimitDisable(dev, port);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgGenIgmpSnoopEnable
*
* DESCRIPTION:
*       Enable/disable trapping to CPU IGMP packets.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       status - GT_TRUE for enable and GT_FALSE otherwise
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
*       Write to the TrapIgmpEn field in the Bridge Port <n> Control register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgGenIgmpSnoopEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                    dev;
    GT_U8                    port;
    GT_BOOL                  status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    status = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxBrgGenIgmpSnoopEnable(dev, port, status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgGenDropIpMcEnable
*
* DESCRIPTION:
*       Discard all non-Control-classified Ethernet packets
*       with a MAC Multicast DA corresponding to the IP Multicast range,
*       i.e. the MAC range 01-00-5e-00-00-00 to 01-00-5e-7f-ff-ff.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       enable - GT_TRUE, enable filtering mode
*                GT_FALSE, disable filtering mode
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
*       Write to the DropIpMcEn field in the L2 ingress control register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgGenDropIpMcEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                    dev;
    GT_BOOL                  state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    state = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgGenDropIpMcEnable(dev, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgGenDropNonIpMcEnable
*
* DESCRIPTION:
*       Discard all non-Control-classified Ethernet
*       packets with a MAC Multicast DA (but not the Broadcast MAC address)
*       not corresponding to the IP Multicast range
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       enable - GT_TRUE, enable filtering mode
*                GT_FALSE, disable filtering mode
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
*       Write to the DropNonIpMcEn field in the L2 ingress control register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgGenDropNonIpMcEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                    dev;
    GT_BOOL                  state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    state = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgGenDropNonIpMcEnable(dev, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgGenArpIgmpTrapTcSet
*
* DESCRIPTION:
*       Sets the traffic class assigned to the ARP\IGMP packet forwarded to CPU.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - physical device number
*       trClass   - traffic class
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
*       Write to the TrapArpIGMPPrio field in the L2 ingress control register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgGenArpIgmpTrapTcSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                  dev;
    GT_U8                  trClass;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    trClass = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgGenArpIgmpTrapTcSet(dev, trClass);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgGenControlPktTcSet
*
* DESCRIPTION:
*       Sets the traffic class assigned to the control packets forwarded to CPU.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - physical device number
*       trClass   - traffic class
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
*       Write to the ControlPrio field in the L2 ingress control register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgGenControlPktTcSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                  dev;
    GT_U8                  trClass;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    trClass = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgGenControlPktTcSet(dev, trClass);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgGenMcRateLimitEnable
*
* DESCRIPTION:
*       Enable/Disable the Rate Limiting mechanism to count
*       MAC multicasts packets as well as MAC Broadcasts on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - physical device number
*       enable    - GT_TRUE, the Rate Limiting mechanism counts
*                   MAC multicasts packets as well as Broadcasts
*                   GT_FALSE, the Rate Limiting mechanism counts
*                   only MAC Broadcasts packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       Write to the EnMcRL field in the L2 ingress control register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgGenMcRateLimitEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                    dev;
    GT_BOOL                  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgGenMcRateLimitEnable(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgGenUnkRateLimitPpEnable
*
* DESCRIPTION:
*       Enable limiting the rate of unknown packets received from all local
*       ports of specified device. Unknown packets which exceed the
*       configured limit are discarded.
*
* APPLICABLE DEVICES:  All ExMx devices, although the Twist C family
*                      does not support byte based limiting.
* INPUTS:
*       dev    - physical device number
*       rMode  - rate calculation mode (packet or byte based)
*       rValue - permitted broadcast rate value (in packets or bytes)
*                This counts in quantities of 64K.
*       window - time window for calculating the rate (microseconds)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on bad parameters.
*
* COMMENTS:
*       Write to the UnknownRLTimeWindow field in the Unknown RL1 register.
*       Write to the enable, mode and limit fields in the Unknown RL0 register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgGenUnkRateLimitPpEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       dev;
    CPSS_RATE_LIMIT_MODE_ENT    rMode;
    GT_U32                      rValue;
    GT_U32                      window;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    rMode = (CPSS_RATE_LIMIT_MODE_ENT)inArgs[1];
    rValue = (GT_U32)inArgs[2];
    window = (GT_U32)inArgs[3];


    /* call cpss api function */
    result = cpssExMxBrgGenUnkRateLimitPpEnable(dev, rMode, rValue, window);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgGenUnkRateLimitPpDisable
*
* DESCRIPTION:
*       Disable limiting the rate of unknown packets received on
*       specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
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
*       Write to the UnknowRLEn field in the Unknown RL0 register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgGenUnkRateLimitPpDisable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                    dev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxBrgGenUnkRateLimitPpDisable(dev);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgGenBcRateLimitForArpBc
*
* DESCRIPTION:
*       Enable/Disable including ARP broadcast packets in the Broadcast Rate
*       Limiting feature.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       enable - GT_TRUE, do not distinguish ARP broadcast from other broadcasts
*       disable- GT_FALSE, do not Broadcast Rate Limit ARP broadcast packets
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_NO_RESOURCE - Failed to allocate t2c structure.
*
* COMMENTS:
*       Write to the DoNotFilterArp field in the Unknown RL0 register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgGenBcRateLimitForArpBc
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];


    /* call cpss api function */
    result = cpssExMxBrgGenBcRateLimitForArpBc(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgGenLocalSwitchingEnable
*
* DESCRIPTION:
*       Enable/disable local switching back through the ingress interface
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       localSwitchEn - GT_TRUE, enable local switching back through the ingress
*                         interface. GT_FALSE disable.
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
*       Write to the LocalEn field in the Port Control <n> register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgGenLocalSwitchingEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U8     port;
    GT_BOOL   localSwitchEn;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    localSwitchEn = (GT_BOOL)inArgs[2];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxBrgGenLocalSwitchingEnable(dev, port, localSwitchEn);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgGenArpTrapEnable
*
* DESCRIPTION:
*       Enable/disable trapping to CPU ARP packets.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - port number
*       status - GT_TRUE for enable and GT_FALSE otherwise
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
*       Write to the TrapARPEn field in the Port Control <n> register.
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgGenArpTrapEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U8     port;
    GT_BOOL   status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    status = (GT_BOOL)inArgs[2];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxBrgGenArpTrapEnable(dev, port, status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/**********************25.12.05***************************************/


/*******************************************************************************
* cpssExMxBrgCntMacDaSaSet
*
* DESCRIPTION:
*       Sets a specific MAC DA and SA to be monitored by host and matrix
*       group counters on specified device.
*
* APPLICABLE DEVICES: ALL ExMx Devices
*
* INPUTS:
*       devNum      - physical device number
*       saAddrPtr   - source MAC address
*       daAddrPtr   - destination MAC address
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hw error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgCntMacDaSaSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8           devNum;
    GT_ETHERADDR    saAddrPtr;
    GT_ETHERADDR    daAddrPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    galtisMacAddr(&saAddrPtr, (GT_U8*)inArgs[1]);
    galtisMacAddr(&daAddrPtr, (GT_U8*)inArgs[2]);

    /* call cpss api function */
    result = cpssExMxBrgCntMacDaSaSet(devNum, &saAddrPtr, &daAddrPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgCntHostGroupCntrsGet
*
* DESCRIPTION:
*       Gets host group counters from specified device.
*
* APPLICABLE DEVICES: ALL ExMx Devices
*
* INPUTS:
*       devNum     - physical device number
*
* OUTPUTS:
*       hostGroupPtr - structure with current counters value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hw error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgCntHostGroupCntrsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                              devNum;
    CPSS_EXMX_BRIDGE_HOST_CNTR_STC     hostGroupPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxBrgCntHostGroupCntrsGet(devNum, &hostGroupPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d", hostGroupPtr.gtHostInPkts,
                hostGroupPtr.gtHostInOctets, hostGroupPtr.gtHostOutPkts,
                hostGroupPtr.gtHostOutOctets, hostGroupPtr. gtHostOutBroadcastPkts,
                hostGroupPtr.gtHostOutMulticastPkts);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgCntMatrixGroupCntrsGet
*
* DESCRIPTION:
*       Gets matrix group counters from specified device.
*
* APPLICABLE DEVICES: ALL ExMx Devices
*
* INPUTS:
*       devNum     - physical device number
*
* OUTPUTS:
*       matrixGroupPtr - structure with current counters value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hw error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgCntMatrixGroupCntrsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                              devNum;
    CPSS_EXMX_BRIDGE_MATRIX_CNTR_STC   matrixGroupPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxBrgCntMatrixGroupCntrsGet(devNum, &matrixGroupPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", matrixGroupPtr.gtMatrixSaDaPkts,
                matrixGroupPtr.gtMatrixSaDaOctets);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgCntBridgeIngressCntrModeSet
*
* DESCRIPTION:
*       Reserves a set of ingress counters that work in specified bridge ingress
*       counters mode.
*
* APPLICABLE DEVICES: ALL ExMx Devices
*
* INPUTS:
*       devNum    - physical device number
*       cntrSetId - counter set ID
*       setMode   - counter mode
*       port      - source logical port number monitored by set
*       vlan      - VLAN Id for mode 2 and mode 3, or ZERO.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hw error
*       GT_BAD_PARAM    - wrong devNum
*       GT_OUT_OF_RANGE - counter set number is out of range (0-3)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgCntBridgeIngressCntrModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                   devNum;
    CPSS_EXMX_BRIDGE_CNTR_SET_ID_ENT        cntrSetId;
    CPSS_BRIDGE_INGR_CNTR_MODES_ENT     setMode;
    GT_U8                                   port;
    GT_U16                                  vlan;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntrSetId = (CPSS_EXMX_BRIDGE_CNTR_SET_ID_ENT)inArgs[1];
    setMode = (CPSS_BRIDGE_INGR_CNTR_MODES_ENT)inArgs[2];
    port = (GT_U8)inArgs[3];
    vlan = (GT_U16)inArgs[4];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxBrgCntBridgeIngressCntrModeSet(devNum, cntrSetId, setMode, port, vlan);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgCntBridgeIngressCntrsGet
*
* DESCRIPTION:
*       Gets a bridge ingress counters from specified counter set.
*
* APPLICABLE DEVICES: ALL ExMx Devices
*
* INPUTS:
*       devNum    - physical device number
*       cntrSetId - counter set number
*
* OUTPUTS:
*       ingrCntrPtr - structure of bridge ingress counters current values.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hw error
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - counter set number is out of range (0-3)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgCntBridgeIngressCntrsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                               devNum;
    CPSS_EXMX_BRIDGE_CNTR_SET_ID_ENT    cntrSetId;
    CPSS_BRIDGE_INGRESS_CNTR_STC        ingrCntrPt;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntrSetId = (CPSS_EXMX_BRIDGE_CNTR_SET_ID_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxBrgCntBridgeIngressCntrsGet(devNum, cntrSetId, &ingrCntrPt);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", ingrCntrPt.gtBrgInFrames,
                ingrCntrPt.gtBrgVlanIngFilterDisc, ingrCntrPt.gtBrgSecFilterDisc,
                ingrCntrPt.gtBrgLocalPropDisc);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxBrgCntLearnedEntryDiscGet
*
* DESCRIPTION:
*       Gets the total number of source addresses the were not learned due to
*       bridge internal congestion.
*
* APPLICABLE DEVICES: ALL ExMx Devices
*
* INPUTS:
*       devNum       - physical device number
*
* OUTPUTS:
*       valPtr - The value of the counter.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hw error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxBrgCntLearnedEntryDiscGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_U32      valPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxBrgCntLearnedEntryDiscGet(devNum, &valPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", valPtr);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxBrgMacHashCalc
*
* DESCRIPTION:
*       This function calculates the hash index for the mac address table.
*       for specific mac address and VLAN id.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum      - device number
*       addr        - the mac address.
*       vid         - the VLAN id.
*
* OUTPUTS:
*       hash - the hash index.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxBrgMacHashCalc

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8         devNum;
    GT_ETHERADDR  addr;
    GT_U16        vid;
    GT_U32        hash;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    galtisMacAddr(&addr, (GT_U8*)inArgs[1]);
    vid = (GT_U16)inArgs[2];

    /* call cpss api function */
    result = cpssExMxBrgMacHashCalc(devNum, &addr, vid, &hash);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", hash);
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxBrgMcUplinkFilterSet",
        &wrcpssExMxBrgMcUplinkFilterSet,
        2, 0},

    {"cpssExMxBrgPortEgrFltUnkEnable",
        &wrcpssExMxBrgPortEgrFltUnkEnable,
        3, 0},

     {"cpssExMxBrgUnkUcEgressFilterCmdSet",
        &wrcpssExMxBrgUnkUcEgressFilterCmdSet,
        2, 0},

    {"cpssExMxBrgUnregMcEgressFilterCmdSet",
        &wrcpssExMxBrgUnregMcEgressFilterCmdSet,
        2, 0},

    {"cpssExMxBrgCpuBcFilteringEnable",
        &wrcpssExMxBrgCpuBcFilteringEnable,
        2, 0},

    {"cpssExMxBrgVlanEgressFilteringEnable",
        &wrcpssExMxBrgVlanEgressFilteringEnable,
        2, 0},

    {"cpssExMxBrgEgressFilteringForL3Enable",
        &wrcpssExMxBrgEgressFilteringForL3Enable,
        2, 0},

    {"cpssExMxBrgFdbAgingTimeoutSet",
        &wrcpssExMxBrgFdbAgingTimeoutSet,
        2, 0},

    {"cpssExMxBrgFdbMacAddrSet",
        &wrcpssExMxBrgFdbMacAddrEntreySet,
        2, 22},
/*
    {"cpssExMxBrgFdbMacAddrEntryDel",
        &wrcpssExMxBrgFdbMacAddrEntryDel,
        1, 0},
  */
    {"cpssExMxBrgFdbMacTriggerModeSet",
        &wrcpssExMxBrgFdbMacTriggerModeSet,
        4, 0},

    {"cpssExMxBrgFdbMacTriggerToggle",
        &wrcpssExMxBrgFdbMacTriggerToggle,
        1, 0},

    {"cpssExMxBrgFdbStaticTransEnable",
        &wrcpssExMxBrgFdbStaticTransEnable,
        2, 0},

    {"cpssExMxBrgFdbStaticDelEnable",
        &wrcpssExMxBrgFdbStaticDelEnable,
        2, 0},

    {"cpssExMxBrgFdbAgeWithoutRemovalEnable",
        &wrcpssExMxBrgFdbAgeWithoutRemovalEnable,
        2, 0},

    {"cpssExMxBrgFdbHashOptionsSet",
        &wrCpssExMxBrgFdbHashOptionsSet,
        5, 0},

    {"cpssExMxBrgFdbHashOptionsGet",
        &wrCpssExMxBrgFdbHashOptionsGet,
        1, 0},

    {"cpssExMxBrgFdbMacVlanLookupModeSet",
        &wrcpssExMxBrgFdbMacVlanLookupModeSet,
        2, 0},

    {"cpssExMxBrgFdbMacAddrLookupModeSet",
        &wrcpssExMxBrgFdbMacAddrLookupModeSet,
        2, 0},

    {"cpssExMxBrgFdbPortLearnSet",
        &wrcpssExMxBrgFdbPortLearnSet,
        4, 0},

    {"cpssExMxBrgFdbPortLearnGet",
        &wrcpssExMxBrgFdbPortLearnGet,
        2, 0},

    {"cpssExMxBrgFdbMacFilterPortEnable",
        &wrcpssExMxBrgFdbMacFilterPortEnable,
        3, 0},

    {"cpssExMxBrgFdbMacFilterEntrySet",
        &wrcpssExMxBrgFdbMacFilterEntrySet,
        2, 4},
 /*
    {"cpssExMxBrgFdbMacFilterEntryGet",
        &wrcpssExMxBrgFdbMacFilterEntryGet,
        2, 0},
 */
    {"cpssExMxBrgFdbNaToCpuEnable",
        &wrcpssExMxBrgFdbNaToCpuEnable,
        2, 0},

    {"cpssExMxBrgFdbMacUpd2CpuEnable",
        &wrcpssExMxBrgFdbMacUpd2CpuEnable,
        2, 0},

    {"cpssExMxBrgFdbAuToCpuSet",
        &wrcpssExMxBrgFdbAuToCpuSet,
        3, 0},

    {"cpssExMxBrgFdbAuToCpuGet",
        &wrcpssExMxBrgFdbAuToCpuGet,
        2, 0},

    {"cpssExMxBrgFdbFlushPortEmulate",
        &wrcpssExMxBrgFdbFlushPortEmulate,
        5, 0},

    {"cpssExMxBrgFdbFlush",
        &wrcpssExMxBrgFdbFlush,
        3, 0},

    {"cpssExMxBrgFdbTransplant",
        &wrcpssExMxBrgFdbTransplant,
        7, 0},
#if 0
    {"cpssExMxBrgFdbMacAddrHwEntryGet",
        &wrcpssExMxBrgFdbMacAddrHwEntryGet,    --> 1. name changed to cpssExMxBrgFdbQaSend
                                                   2. function only send query !!
                                                      and not return any info back !!!
        1, 24},
    {"cpssExMxBrgFdbMacAddrHwEntryIndexGet",
        &wrcpssExMxBrgFdbMacAddrHwEntryIndexGet, --> name changed to cpssExMxBrgFdbQiSend
                                                   2. function only send query !!
                                                      and not return any info back !!!
        3, 0},
#endif
    {"cpssExMxBrgFdbMaxLookupLenSet",
        &wrcpssExMxBrgFdbMaxLookupLenSet,
        2, 0},

   /* {"cpssExMxBrgFdbMacAddrEntryWrite",
        &wrcpssExMxBrgFdbMacAddrEntryWrite,
        3, 21},
     */
    {"cpssExMxBrgFdbMacAddrDelete",
        &wrcpssExMxBrgFdbMacAddrEntryInvalidateOrDelete,
        2, 3},

    {"cpssExMxBrgFdbL2IngrProcEnable",
        &wrcpssExMxBrgFdbL2IngrProcEnable,
        2, 0},

    {"cpssExMxBrgFdbPortAuToCpuSet",
        &wrcpssExMxBrgFdbPortAuToCpuSet,
        4, 0},

    {"cpssExMxBrgFdbDeviceSet",
       &wrCpssExMxBrgFdbDeviceTableSet,
       1, CPSS_EXMX_FDB_DEV_TABLE_SIZE_CNS},

    {"cpssExMxBrgFdbMacActionsEnableGet",
       &wrCpssExMxBrgFdbMacActionsEnableGet,
       1, 0},

    {"cpssExMxBrgFdbMacActionsEnableSet",
       &wrCpssExMxBrgFdbMacActionsEnableSet,
       2, 0},

    {"cpssExMxBrgFdbMacAddrRawEntryGetFirst",
       &wrCpssExMxBrgFdbMacAddrRawEntryRead,
       2, 0},

    {"cpssExMxBrgFdbMacAddrRawEntrySet",
       &wrCpssExMxBrgFdbMacAddrRawEntryWrite,
       2, CPSS_EXMX_FDB_RAW_ENTRY_SIZE_CNS},

    {"cpssExMxBrgFdbTrunkAgingModeSet",
       &wrCpssExMxBrgFdbTrunkAgingModeSet,
       2, 0},

    {"cpssExMxBrgFdbAuMsgBlockGetFirst",
        &wrcpssExMxBrgFdbAuMsgBlockGet,
        1, 0},


    {"cpssExMxBrgFdbAuMsgBlockGetNext",
        &wrcpssExMxBrgFdbAuMsgBlockGet,
        1, 0},

    {"cpssExMxBrgFdbInit",
        &wrcpssExMxBrgFdbInit,
        1, 0},

    {"cpssExMxBrgStpInit",
        &wrcpssExMxBrgStpInit,
        1,0},

    {"cpssExMxBrgStpModeSet",
        &wrcpssExMxBrgStpModeSet,
        2,0},

    {"cpssExMxBrgStpPortStateSet",
        &wrcpssExMxBrgStpPortStateSet,
        3,0},

    {"cpssExMxBrgStpStateSet",
        &wrcpssExMxBrgStpStateSet,
        4,0},
  /*
    {"cpssExMxBrgStpEntryGet",
        &wrcpssExMxBrgStpEntryGet,
        3,0},

    {"cpssExMxBrgStpEntryWrite",
        &wrcpssExMxBrgStpEntryWrite,
        3,0},
  */
    {"cpssExMxBrgVlanInit",
        &wrcpssExMxBrgVlanInit,
        1, 0},

    {"cpssExMxBrgVlanMaxVidSet",
        &wrcpssExMxBrgVlanMaxVidSet,
        2, 0},

    {"cpssExMxBrgVlanSet",
        &wrcpssExMxBrgVlanEntryWrite,
        1, 13},

    {"cpssExMxBrgVlanGetFirst",
        &wrcpssExMxBrgVlanEntryGetFirst,
        1, 0},

    {"cpssExMxBrgVlanGetNext",
        &wrcpssExMxBrgVlanEntryGetNext,
        1, 0},

    {"cpssExMxBrgVlanMemberAdd",
        &wrcpssExMxBrgVlanMemberAdd,
        4, 0},

    {"cpssExMxBrgVlanPortDelete",
        &wrcpssExMxBrgVlanPortDelete,
        3, 0},

    {"cpssExMxBrgVlanHasUplinksSet",
        &wrcpssExMxBrgVlanHasUplinksSet,
        3, 0},

    {"cpssExMxBrgVlanHasLocalsSet",
        &wrcpssExMxBrgVlanHasLocalsSet,
        3, 0},

    {"cpssExMxBrgVlanPortPvidGet",
        &wrcpssExMxBrgVlanPortPvidGet,
        2, 0},

    {"cpssExMxBrgVlanPortPvidSet",
        &wrcpssExMxBrgVlanPortPvidSet,
        3, 0},

    {"cpssExMxBrgVlanPortTcGet",
        &wrcpssExMxBrgVlanPortTcGet,
        2, 0},

    {"cpssExMxBrgVlanPortIngFltEnable",
        &wrcpssExMxBrgVlanPortIngFltEnable,
        3, 0},

    {"cpssExMxBrgVlanPortIngFltEnable",
        &wrcpssExMxBrgVlanPortIngFltEnable,
        3, 0},

    {"cpssExMxBrgVlanIpClassEnable",
        &wrcpssExMxBrgVlanIpClassEnable,
        3, 0},

    {"cpssExMxBrgVlanSubnetClassEntrySet",
        &wrcpssExMxBrgVlanSubnetClassEntrySet,
        5, 0},

    {"cpssExMxBrgVlanSubnetClassVidSet",
        &wrcpssExMxBrgVlanSubnetClassVidSet,
        3, 0},

    {"cpssExMxBrgVlanSubnetClassTcSet",
        &wrcpssExMxBrgVlanSubnetClassTcSet,
        3, 0},

    {"cpssExMxBrgVlanSubnetClassEntryEnable",
        &wrcpssExMxBrgVlanSubnetClassEntryEnable,
        3, 0},

    {"cpssExMxBrgVlanProtoClassEnable",
        &wrcpssExMxBrgVlanProtoClassEnable,
        3, 0},

    {"cpssExMxBrgVlanProtoClassGet",
        &wrcpssExMxBrgVlanProtoClassGet,
        2, 0},

    {"cpssExMxBrgVlanProtoClassSet",
        &wrcpssExMxBrgVlanProtoClassSet,
        6, 0},

    {"cpssExMxBrgVlanVidProtoSet",
        &wrcpssExMxBrgVlanVidProtoSet,
        4, 0},

    {"cpssExMxBrgVlanTcProtoSet",
        &wrcpssExMxBrgVlanTcProtoSet,
        4, 0},

    {"cpssExMxBrgVlanLearningStateSet",
        &wrcpssExMxBrgVlanLearningStateSet,
       3, 0},
 /*
    {"cpssExMxBrgVlanMcLikeEntryWrite",
        &wrcpssExMxBrgVlanMcLikeEntryWrite,
        4, 0},
 */
    {"cpssExMxBrgVlanIngressFilteringStatusGet",
        &wrcpssExMxBrgVlanIngressFilteringStatusGet,
        1, 0},

    {"cpssExMxBrgVlanIsDevMember",
        &wrcpssExMxBrgVlanIsDevMember,
        2, 0},

    {"cpssExMxBrgVlan2StpIdBind",
        &wrcpssExMxBrgVlan2StpIdBind,
        3, 0},

    {"cpssExMxBrgVlanStpIdGet",
        &wrcpssExMxBrgVlanStpIdGet,
        2, 0},

    {"cpssExMxBrgVlanEtherTypeSet",
        &wrcpssExMxBrgVlanEtherTypeSet,
        4, 0},

    {"cpssExMxBrgVlanPortTagModeSet",
        &wrcpssExMxBrgVlanPortTagModeSet,
        4, 0},
/*
    {"cpssExMxBrgVlanTableInvalidate",
        &wrcpssExMxBrgVlanTableInvalidate,
        1, 0},
*/
    {"cpssExMxBrgVlanDelete",
        &wrcpssExMxBrgVlanEntryInvalidate,
        1, 1},

    {"cpssExMxBrgVlanEntryInvalidate",
        &wrcpssExMxBrgVlanEntryInvalidate,
        2, 0},

     {"cpssExMxBrgVlanMacPerVlanSet",
        &wrcpssExMxBrgVlanMacPerVlanSet,
        4, 0},

     {"cpssExMxBrgVlanUnregMcFilterSet",
        &wrcpssExMxBrgVlanUnregMcFilterSet,
        3, 0},

     {"cpssExMxBrgVlanUnregMcEnable",
        &wrcpssExMxBrgVlanUnregMcEnable,
        2, 0},

     {"cpssExMxBrgMcInit",
        &wrcpssExMxBrgMcInit,
        1, 0},

     {"cpssExMxBrgMcSet",
        &wrcpssExMxBrgMcEntryWrite,
        1, 4},

     {"cpssExMxBrgMcMemberAdd",
        &wrcpssExMxBrgMcMemberAdd,
        3, 0},

     {"cpssExMxBrgMcMemberDelete",
        &wrcpssExMxBrgMcMemberDelete,
        3, 0},

     {"cpssExMxBrgMcGroupDelete",
        &wrcpssExMxBrgMcGroupDelete,
        2, 0},
 /*
     {"cpssExMxBrgMcGetFirst",
        &wrcpssExMxBrgMcEntryGetFirst,
        1, 0},

     {"cpssExMxBrgMcGetNext",
        &wrcpssExMxBrgMcEntryGetNext,
        1, 0},
 */
     {"cpssExMxBrgNestVlanEnable",
        &wrcpssExMxBrgNestVlanEnable,
        2, 0},

     {"cpssExMxBrgNestVlanPortStateSet",
        &wrcpssExMxBrgNestVlanPortStateSet,
        3, 0},

     {"cpssExMxBrgPrvEdgeVlanEnable",
        &wrcpssExMxBrgPrvEdgeVlanEnable,
        2, 0},

     {"cpssExMxBrgPrvEdgeVlanPortEnable",
        &wrcpssExMxBrgPrvEdgeVlanPortEnable,
        6, 0},

     {"cpssExMxBrgGenBrdPortLimitSet",
        &wrcpssExMxBrgGenBrdPortLimitSet,
        5, 0},

     {"cpssExMxBrgGenBcLimitDisable",
        &wrcpssExMxBrgGenBcLimitDisable,
        2, 0},

     {"cpssExMxBrgGenIgmpSnoopEnable",
        &wrcpssExMxBrgGenIgmpSnoopEnable,
        3, 0},

     {"cpssExMxBrgGenDropIpMcEnable",
        &wrcpssExMxBrgGenDropIpMcEnable,
        2, 0},

     {"cpssExMxBrgGenDropNonIpMcEnable",
        &wrcpssExMxBrgGenDropNonIpMcEnable,
        2, 0},

     {"cpssExMxBrgGenArpIgmpTrapTcSet",
        &wrcpssExMxBrgGenArpIgmpTrapTcSet,
        2, 0},

     {"cpssExMxBrgGenControlPktTcSet",
        &wrcpssExMxBrgGenControlPktTcSet,
        2, 0},

     {"cpssExMxBrgGenMcRateLimitEnable",
        &wrcpssExMxBrgGenMcRateLimitEnable,
        2, 0},

     {"cpssExMxBrgGenUnkRateLimitPpEnable",
        &wrcpssExMxBrgGenUnkRateLimitPpEnable,
        4, 0},

     {"cpssExMxBrgGenUnkRateLimitPpDisable",
        &wrcpssExMxBrgGenUnkRateLimitPpDisable,
        1, 0},

     {"cpssExMxBrgGenBcRateLimitForArpBc",
        &wrcpssExMxBrgGenBcRateLimitForArpBc,
        2, 0},

     {"cpssExMxBrgGenLocalSwitchingEnable",
        &wrcpssExMxBrgGenLocalSwitchingEnable,
        3, 0},

     {"cpssExMxBrgGenArpTrapEnable",
        &wrcpssExMxBrgGenArpTrapEnable,
        3, 0},

     {"cpssExMxBrgCntMacDaSaSet",
        &wrcpssExMxBrgCntMacDaSaSet,
        3, 0},

     {"cpssExMxBrgCntHostGroupCntrsGet",
        &wrcpssExMxBrgCntHostGroupCntrsGet,
        1, 0},

     {"cpssExMxBrgCntMatrixGroupCntrsGet",
        &wrcpssExMxBrgCntMatrixGroupCntrsGet,
        1, 0},

     {"cpssExMxBrgCntBridgeIngressCntrModeSet",
        &wrcpssExMxBrgCntBridgeIngressCntrModeSet,
        5, 0},

     {"cpssExMxBrgCntBridgeIngressCntrsGet",
        &wrcpssExMxBrgCntBridgeIngressCntrsGet,
        2, 0},

     {"cpssExMxBrgCntLearnedEntryDiscGet",
        &wrcpssExMxBrgCntLearnedEntryDiscGet,
        1, 0},

     {"cpssExMxBrgMacHashCalc",
        &wrCpssExMxBrgMacHashCalc,
        3, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxBridge
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
GT_STATUS cmdLibInitCpssExMxBridge
(
    GT_VOID
)
{
        GT_STATUS rc;

        /* create semaphore for Mac Actions */
    rc = cmdOsMutexCreate("macActionSem", &macActionSemId);
    if (rc != GT_OK)
    {
        return rc;
    }

    return cmdInitLibrary(dbCommands, numCommands);
}




