/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmPolicer.c
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
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
#include <cpss/exMxPm/exMxPmGen/policer/cpssExMxPmPolicer.h>

/* Set metering entry index in bitmap */
#define CPSS_EXMXPM_METERING_ENTRY_BITMAP_SET(index) \
    setEntryBitmap[((index) / 32)] |= 1 << ((index) % 32)

/* Get metering entry index from bitmap */
#define CPSS_EXMXPM_METERING_ENTRY_BITMAP_GET(index) \
    ((setEntryBitmap[((index) / 32)] >> ((index) % 32)) & 0x1)

/* Traffic flow direction  */
CPSS_DIRECTION_ENT cpssExMxPmPolicerDirection = CPSS_DIRECTION_INGRESS_E;

/*******************************************************************************
* cpssExMxPmPolicerInit
*
* DESCRIPTION:
*       Init Policer facility on specified PP device.
*       The Policer Init includes copy PP Policer Global parameters
*       to the Policer DB (enable metering, counting mode etc.).
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum  - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PARAM        - on wrong devNum.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerMeteringEnableSet
*
* DESCRIPTION:
*       Globally enables or disables metering on device.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum  - physical device number.
*       enable  - Enable/disable metering:
*                 GT_TRUE  - metering is enabled on the device.
*                 GT_FALSE - metering is disabled on the device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*        If metering is enabled, it can be triggered either from the
*        Policy engine or per port.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerMeteringEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerMeteringEnableSet(devNum,
                                                cpssExMxPmPolicerDirection,
                                                enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerMeteringEnableGet
*
* DESCRIPTION:
*       Gets device global metering status (Enable/Disable).
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - Pointer to Enable/Disable metering:
*                         GT_TRUE  - metering is enabled on the device.
*                         GT_FALSE - metering is disabled on the device.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*        If metering is enabled, it can be triggered either from the
*        Policy engine or per port.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerMeteringEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerMeteringEnableGet(devNum,
                                                cpssExMxPmPolicerDirection,
                                                &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmPolicerCountingConfigSet
*
* DESCRIPTION:
*       Gets the counting configuration: counting mode and number of desired
*       Policer Counting Entries.
*       The counting may be disabled or work in one of the following modes:
*       - Billing - Policer performs billing counting when triggered from PCL
*                   or Metering Entry.
*       - Policy  - Policer performs packet counting when triggered from PCL.
*       - Vlan    - Policer performs packet counting acording to packet's VID
*                   for each packet. This mode triggered by the Packet Command.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum                  - physical device number.
*       mode                    - Policer counting mode.
*       numOfCountingEntries    - number of desired Policer Counting Entries.
*                                 One Policer Counting Entry comprize:
*                                  - 1 Billing counters
*                                  - 4 VLAN counters
*                                  - 6 Policy counters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, mode or numOfCountingEntries.
*       GT_HW_ERROR     - on Hardware error.
*       GT_OUT_OF_RANGE - on number of Policer Counting Entries that is out
*                         of range of [1-256].
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerCountingConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT mode;
    GT_U32 numOfCountingEntries;
    GT_U32 numOfMeteringEntries;
    CPSS_EXMXPM_POLICER_CONFIG_STC  policerConf;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT)inArgs[1];
    numOfCountingEntries = (GT_U32)inArgs[2];

    if(mode == CPSS_EXMXPM_POLICER_COUNTING_VLAN_E)
    {
        policerConf.sharedCounters = GT_FALSE;
        /* don't share the counters between ingress and egress because counters
           will  give 'Unexpected results'
           (egress will override ingress see bugs: 106252 , 106307) */
        if(numOfCountingEntries > _1K)
        {
            /* for VLAN counting not need more then _1K entries to cover the 4K
               vlans */
            numOfCountingEntries = _1K;
        }

        /* update the number of meters */
        numOfMeteringEntries = _8K - 2 * numOfCountingEntries;
    }
    else
    {
        numOfMeteringEntries = _8K - numOfCountingEntries;

        policerConf.sharedCounters = GT_TRUE;
    }


    policerConf.egrCountingEntries =
    policerConf.ingrCountingEntries = numOfCountingEntries;
    policerConf.egrCountingMode =
    policerConf.ingrCountingMode = mode;
    policerConf.ingrMeterEntries = numOfMeteringEntries;
    policerConf.sharedMeters = GT_TRUE;



    /* call cpss api function */
    result = cpssExMxPmPolicerConfigSet(devNum, &policerConf);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmPolicerCountingConfigGet
*
* DESCRIPTION:
*       Gets the counting configuration: counting mode and possible number of
*       Policer Counting Entries.
*       The counting may be disabled or work in one of the following modes:
*       - Billing - Policer performs billing counting when triggered from PCL
*                   or Metering Entry.
*       - Policy  - Policer performs packet counting when triggered from PCL.
*       - Vlan    - Policer performs packet counting acording to packet's VID
*                   for each packet. This mode triggered by the Packet Command.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       modePtr                     - Pointer to the Policer counting mode.
*       numOfCountingEntriesPtr     - Pointer to the possible number of
*                                     Policer Counting Entries.
*                                     One Policer Counting Entry comprize:
*                                     - 1 Billing counters
*                                     - 4 VLAN counters
*                                     - 6 Policy counters
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerCountingConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_CONFIG_STC   policerConf;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerConfigGet(devNum, &policerConf);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",
                 policerConf.ingrCountingMode,
                 policerConf.ingrCountingEntries);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerPacketSizeModeSet
*
* DESCRIPTION:
*       Sets metered Packet Size Mode that metering and billing is done
*       according to.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum             - physical device number.
*       packetSize         - Type of packet size:
*                               - L3 datagram size only (this does not include
*                                 the L2 header size and packets CRC).
*                               - L2 packet length including.
*                               - L1 packet length including
*                                 (preamble + IFG + FCS).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PARAM        - on on wrong devNum or packetSize.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerPacketSizeModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_POLICER_PACKET_SIZE_MODE_ENT packetSize;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    packetSize = (CPSS_POLICER_PACKET_SIZE_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerPacketSizeModeSet(devNum,
                                                cpssExMxPmPolicerDirection,
                                                packetSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerPacketSizeModeGet
*
* DESCRIPTION:
*       Gets metered Packet Size Mode that metering and billing is done
*       according to.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       packetSizePtr   - Pointer to the Type of packet size:
*                           - L3 datagram size only (this does not include
*                             the L2 header size and packets CRC).
*                           - L2 packet length including.
*                           - L1 packet length including (preamble + IFG + FCS).
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerPacketSizeModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_POLICER_PACKET_SIZE_MODE_ENT packetSize;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerPacketSizeModeGet(devNum,
                                                cpssExMxPmPolicerDirection,
                                                &packetSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", packetSize);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerVlanCountingModeSet
*
* DESCRIPTION:
*       Sets the Policer VLAN counters mode as byte or packet based.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       mode            - VLAN counters mode: Byte or Packet based.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or mode.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerVlanCountingModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_VLAN_COUNT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_POLICER_VLAN_COUNT_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerVlanCountingModeSet(devNum,
                                                  cpssExMxPmPolicerDirection,
                                                  mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerVlanCountingModeGet
*
* DESCRIPTION:
*       Gets the Policer VLAN counters mode as byte or packet based.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       modePtr         - Pointer to the VLAN counters
*                         mode: Byte or Packet based.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerVlanCountingModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_VLAN_COUNT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerVlanCountingModeGet(devNum,
                                                  cpssExMxPmPolicerDirection,
                                                  &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet
*
* DESCRIPTION:
*       Enables or disables VLAN counting triggering according to the
*       specified packet command.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       cmdTrigger      - The packet command trigger.
*                         The following packet command are supported:
*                           Forward (CPSS_PACKET_CMD_FORWARD_E),
*                           Hard Drop (CPSS_PACKET_CMD_DROP_HARD_E),
*                           Soft Drop (CPSS_PACKET_CMD_DROP_SOFT_E),
*                           Trap (CPSS_PACKET_CMD_TRAP_TO_CPU_E),
*                           Mirror to CPU (CPSS_PACKET_CMD_MIRROR_TO_CPU_E).
*       enable          - Enable/Disable VLAN Counting according to the packet
*                         command trigger:
*                         GT_TRUE  - Enable VLAN counting on packet with
*                                    packet command trigger.
*                         GT_FALSE - Disable VLAN counting on packet with
*                                    packet command trigger.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or cmndTrigger.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       All packet commands are enabled in the HW by default.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerVlanCountingPacketCmdTriggerSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PACKET_CMD_ENT cmdTrigger;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cmdTrigger = (CPSS_PACKET_CMD_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet(devNum,
                                                              cpssExMxPmPolicerDirection,
                                                              cmdTrigger, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet
*
* DESCRIPTION:
*       Gets VLAN counting triggering status (Enable/Disable) according to the
*       specified packet command.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       cmdTrigger      - The packet command trigger.
*                         The following packetcommand are supported:
*                           Forward (CPSS_PACKET_CMD_FORWARD_E),
*                           Hard Drop (CPSS_PACKET_CMD_DROP_HARD_E),
*                           Soft Drop (CPSS_PACKET_CMD_DROP_SOFT_E),
*                           Trap (CPSS_PACKET_CMD_TRAP_TO_CPU_E),
*                           Mirror to CPU (CPSS_PACKET_CMD_MIRROR_TO_CPU_E).
*
* OUTPUTS:
*       enablePtr       - Pointer to Enable/Disable VLAN Counting according
*                         to the packet command trigger:
*                         GT_TRUE  - Enable VLAN counting on packet with
*                                    triggered packet command.
*                         GT_FALSE - Disable VLAN counting on packet with
*                                    triggered packet command.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or cmndTrigger.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       All packet commands are enabled in the HW by default.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerVlanCountingPacketCmdTriggerGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PACKET_CMD_ENT cmdTrigger;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cmdTrigger = (CPSS_PACKET_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet(devNum,
                                                              cpssExMxPmPolicerDirection,
                                                              cmdTrigger, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerDropTypeSet
*
* DESCRIPTION:
*       Sets the Policer out-of-profile (Red packets) drop command type.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       dropType        - Policer Drop Red Type: Soft or Hard.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or dropType.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerDropTypeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DROP_MODE_TYPE_ENT dropType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    dropType = (CPSS_DROP_MODE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerDropTypeSet(devNum,
                                          cpssExMxPmPolicerDirection,
                                          dropType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerDropTypeGet
*
* DESCRIPTION:
*       Gets the Policer out-of-profile (Red packets) drop command type.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       dropTypePtr     - Pointer to the Policer Drop Red Type: Soft or Hard.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerDropTypeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DROP_MODE_TYPE_ENT dropType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerDropTypeGet(devNum,
                                          cpssExMxPmPolicerDirection,
                                          &dropType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dropType);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerCountingColorModeSet
*
* DESCRIPTION:
*       Sets the Policer color counting mode.
*       The Color Counting can be done according to the packet's outgoing
*       Drop Precedence or packet's Conformance Level.
*       This affects both the Billing and Management(statistical) counters.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       mode            - Color counting mode: Drop Precedence or
*                         Conformance Level.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or mode.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerCountingColorModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_COLOR_COUNT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_POLICER_COLOR_COUNT_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerCountingColorModeSet(devNum,
                                                   cpssExMxPmPolicerDirection,
                                                   mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerCountingColorModeGet
*
* DESCRIPTION:
*       Gets the Policer color counting mode.
*       The Color Counting can be done according to the packet's outgoing
*       Drop Precedence or packet's Conformance Level.
*       This affects both the Billing and Management(statistical) counters.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       modePtr         - Pointer to the color counting mode: Drop Precedence or
*                         Conformance Level.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerCountingColorModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_COLOR_COUNT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerCountingColorModeGet(devNum,
                                                   cpssExMxPmPolicerDirection,
                                                   &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerL2RemarkModelSet
*
* DESCRIPTION:
*       Sets the L2 packets remarking model of the PP.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       model           - L2 packets remarking model. It defines QoS parameter,
*                         which used as index in the Qos table:
*                         User Priority or Traffic Class.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or model.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerL2RemarkModelSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_L2_REMARK_MODEL_ENT model;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    model = (CPSS_EXMXPM_POLICER_L2_REMARK_MODEL_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerL2RemarkModelSet(devNum,
                                               cpssExMxPmPolicerDirection,
                                               model);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerL2RemarkModelGet
*
* DESCRIPTION:
*       Gets the L2 packets remarking model of the PP.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       modelPtr        - L2 packets remarking model. It defines QoS parameter,
*                         which used as index in the Qos table:
*                         User Priority or Traffic Class.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerL2RemarkModelGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_L2_REMARK_MODEL_ENT model;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerL2RemarkModelGet(devNum,
                                               cpssExMxPmPolicerDirection,
                                               &model);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", model);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerManagementCntrsResolutionSet
*
* DESCRIPTION:
*       Sets the Management counters resolution either 1 Byte or 16 Bytes.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrSet         - Management Counters Set [0:2].
*       cntrResolution  - Management Counters resolution: 1 or 16 Bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum,Management Counters Set or cntrResolution.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerManagementCntrsResolutionSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT cntrSet;
    CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_ENT cntrResolution;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cntrSet = (CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT)inArgs[1];
    cntrResolution = (CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPolicerManagementCntrsResolutionSet(devNum,
                                                           cpssExMxPmPolicerDirection,
                                                           cntrSet, cntrResolution);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerManagementCntrsResolutionGet
*
* DESCRIPTION:
*       Gets the Management counters resolution (either 1 Byte or 16 Bytes).
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum              - physical device number.
*       cntrSet             - Management Counters Set [0:2].
*
* OUTPUTS:
*       cntrResolutionPtr   - Pointer to the Management Counters
*                             resolution: 1 or 16 Bytes.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or Management Counters Set
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerManagementCntrsResolutionGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT cntrSet;
    CPSS_EXMXPM_POLICER_MNG_CNTR_RESOLUTION_ENT cntrResolution;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cntrSet = (CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerManagementCntrsResolutionGet(devNum,
                                                           cpssExMxPmPolicerDirection,
                                                           cntrSet, &cntrResolution);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntrResolution);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerPacketSizeModeForTunnelTermSet
*
* DESCRIPTION:
*      Sets the policing counting mode for tunnel terminated packets
*      The Policy engine provides the following modes to define packet size:
*       - Regular packet metering. The packet size is defined by
*         cpssExMxPmPolicerPacketSizeModeSet.
*       - Passenger packet metering.
*         TT metering and billing is done according to L3 datagram size only.
*         For IP-Over-x packets, the counting includes the passenger packet's
*         BC, excluding the tunnel header and the packets CRC
*         For, Ethernet-Over-x packets, the counting includes the passenger
*         packet's BC, excluding the tunnel header and if packet includes Two
*         CRC patterns (one for the inner packet and one for the outer packets)
*         also excluding the outer CRC
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum              - physical device number.
*       ttPacketSizeMode    - Tunnel Termination Packet Size Mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PARAM    - on wrong devNum or ttPacketSizeMode.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerPacketSizeModeForTunnelTermSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_MODE_ENT ttPacketSizeMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ttPacketSizeMode = (CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerPacketSizeModeForTunnelTermSet(devNum,
                                                             cpssExMxPmPolicerDirection,
                                                             ttPacketSizeMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerPacketSizeModeForTunnelTermGet
*
* DESCRIPTION:
*      Gets the policing counting mode for tunnel terminated packets
*      The Policy engine provides the following modes to define packet size:
*       - Regular packet metering. The packet size is defined by
*         cpssExMxPmPolicerPacketSizeModeSet.
*       - Passenger packet metering.
*         TT metering and billing is done according to L3 datagram size only.
*         For IP-Over-x packets, the counting includes the passenger packet's
*         BC, excluding the tunnel header and the packets CRC
*         For, Ethernet-Over-x packets, the counting includes the passenger
*         packet's BC, excluding the tunnel header and if packet includes Two
*         CRC patterns (one for the inner packet and one for the outer packets)
*         also excluding the outer CRC
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum              - physical device number.
*
* OUTPUTS:
*       ttPacketSizeModePtr - Pointer to the Tunnel Termination
*                             Packet Size Mode.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerPacketSizeModeForTunnelTermGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_TT_PACKET_SIZE_MODE_ENT ttPacketSizeMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerPacketSizeModeForTunnelTermGet(devNum,
                                                             cpssExMxPmPolicerDirection,
                                                             &ttPacketSizeMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ttPacketSizeMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerPortMeteringEnableSet
*
* DESCRIPTION:
*       Enables or disables Metering triggering on specified port.
*       If specified port is CPU, then triggered a metering for
*       packets arriving from the CPU.
*       When feature is enabled the meter entry index is a port number.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number (CPU is included).
*       enable          - Enable/Disable Metering triggering on specified port:
*                         GT_TRUE  - Metering is triggered on specified port.
*                         GT_FALSE - Metering isn't triggered on specified port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or portNum.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerPortMeteringEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL enable;


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
    result = cpssExMxPmPolicerPortMeteringEnableSet(devNum,
                                                    cpssExMxPmPolicerDirection,
                                                    portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerPortMeteringEnableGet
*
* DESCRIPTION:
*       Gets Metering port status (Enable/Disable).
*       If specified port is CPU, then triggered a metering for
*       packets arriving from the CPU.
*       When feature is enabled the meter entry index is a port number.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number (CPU is included).
*
* OUTPUTS:
*       enablePtr       - Enable/Disable Metering triggering on specified port:
*                         GT_TRUE  - Metering is triggered on specified port.
*                         GT_FALSE - Metering isn't triggered on specified port.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or portNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerPortMeteringEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPolicerPortMeteringEnableGet(devNum,
                                                    cpssExMxPmPolicerDirection,
                                                    portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerMruSet
*
* DESCRIPTION:
*       Sets the Policer Maximum Receive Unit size.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       mruSize         - Policer MRU value.
*                         This value is used in the metering algorithm.
*                         When the number of bytes in the bucket is lower than
*                         this value a packet is marked as non conforming.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or mruSize.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerMruSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mruSize;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mruSize = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerMruSet(devNum, cpssExMxPmPolicerDirection, mruSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerMruGet
*
* DESCRIPTION:
*       Gets the Policer Maximum Receive Unit size.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       mruSizePtr      - Pointer to the Policer MRU value.
*                         This value is used in the metering algorithm.
*                         When the number of bytes in the bucket is lower than
*                         this value a packet is marked as non conforming.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerMruGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mruSize;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerMruGet(devNum, cpssExMxPmPolicerDirection, &mruSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mruSize);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerErrorGet
*
* DESCRIPTION:
*       Gets address and type of Policer Entry that had an error.
*       This information available if error is happened and not read till now.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum              - physical device number.
*
* OUTPUTS:
*       entryTypePtr    - Pointer to the Type of Entry (Metering or Billing)
*                         that had an error.
*       entryAddrPtr    - Pointer to the Policer Entry that had an error.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_EMPTY            - on missing error information.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_HW_ERROR         - on Hardware error.
*       GT_BAD_PTR          - on NULL pointer.
*
* COMMENTS:
*       Policer Error Register updating generates an interrupt.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerErrorGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_ENTRY_TYPE_ENT entryType;
    GT_U32 entryAddr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerErrorGet(devNum, cpssExMxPmPolicerDirection,
                                       &entryType, &entryAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", entryType, entryAddr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerErrorCounterGet
*
* DESCRIPTION:
*       Gets the value of the Policer Error Counter.
*       The Error Counter is a free-running non-sticky 8-bit read-only counter.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       cntrValuePtr    - Pointer to the Policer Error counter value.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_HW_ERROR         - on Hardware error.
*       GT_BAD_PTR          - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerErrorCounterGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 cntrValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerErrorCounterGet(devNum,
                                              cpssExMxPmPolicerDirection,
                                              &cntrValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntrValue);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerManagementCountersSet
*
* DESCRIPTION:
*       Sets the value of specified Management Counters.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       mngCntrSet      - Management Counters Set[0:2].
*       mngCntrType     - Management Counters Type (GREEN, YELLOW, RED, DROP).
*       mngCntrPtr      - Pointer to the Management Counters Entry must be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or mngCntrType or Management Counters Set number.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerManagementCountersSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT mngCntrSet;
    CPSS_EXMXPM_POLICER_MNG_CNTR_TYPE_ENT mngCntrType;
    CPSS_EXMXPM_POLICER_MNG_CNTR_ENTRY_STC mngCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mngCntrSet = (CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT)inArgs[1];
    mngCntrType = (CPSS_EXMXPM_POLICER_MNG_CNTR_TYPE_ENT)inArgs[2];

    /* inArgs[3] - 32 LS bits of counters, inArgs[4] - 10 MS bits of counters */
    galtisU64(&(mngCntr.duMngCntr), inArgs[3], inArgs[4]);

    mngCntr.packetMngCntr = inArgs[5];

    /* call cpss api function */
    result = cpssExMxPmPolicerManagementCountersSet(devNum,
                                                    cpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerManagementCountersGet
*
* DESCRIPTION:
*       Gets the value of specified Management Counters.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       mngCntrSet      - Management Counters Set[0:2].
*       mngCntrType     - Management Counters Type (GREEN, YELLOW, RED, DROP).
*
* OUTPUTS:
*       mngCntrPtr      - Pointer to the requested Management Counters Entry.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or mngCntrType or Management Counters Set number.
*       GT_HW_ERROR     - on Hardware error.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*
*********************************************************************************/

static CMD_STATUS wrCpssExMxPmPolicerManagementCountersGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT mngCntrSet;
    CPSS_EXMXPM_POLICER_MNG_CNTR_TYPE_ENT mngCntrType;
    CPSS_EXMXPM_POLICER_MNG_CNTR_ENTRY_STC mngCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mngCntrSet = (CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT)inArgs[1];
    mngCntrType = (CPSS_EXMXPM_POLICER_MNG_CNTR_TYPE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPolicerManagementCountersGet(devNum,
                                                    cpssExMxPmPolicerDirection,
                                                    mngCntrSet, mngCntrType, &mngCntr);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d %d",
                 mngCntr.duMngCntr.l[0], mngCntr.packetMngCntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerMeteringEntrySet
*
* DESCRIPTION:
*      Sets Metering Policer Entry configuration.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - policer entry index.
*       entryPtr        - pointer to the metering policer entry going to be set.
*
* OUTPUTS:
*       tbParamsPtr     - pointer to actual policer token bucket parameters.
*                         The token bucket parameters are returned as output
*                         values. This is due to the hardware rate resolution,
*                         the exact rate or burst size requested may not be
*                         honored. The returned value gives the user the
*                         actual parameters configured in the hardware.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - wrong devNum, entryIndex or entry parameter.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_TIMEOUT      - on time out.
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_U32 meteringEntryIndex;
static GT_U32 setEntryBitmap[256];

static CMD_STATUS wrCpssExMxPmPolicerMeteringEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 entryIndex;
    CPSS_EXMXPM_POLICER_METERING_ENTRY_STC entry;
    CPSS_EXMXPM_POLICER_METER_TB_PARAMS_UNT tbParams;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    entryIndex                          = (GT_U32)inFields[0];
    entry.countingEntryIndex            = (GT_U32)inFields[1];
    entry.meterColorMode    = (CPSS_POLICER_COLOR_MODE_ENT)inFields[2];
    entry.remarkMode        = (CPSS_EXMXPM_POLICER_REMARK_MODE_ENT)inFields[3];
    entry.mngCounterSet     = (CPSS_EXMXPM_POLICER_MNG_CNTR_SET_ENT)inFields[4];
    entry.meterMode         = (CPSS_EXMXPM_POLICER_METER_MODE_ENT)inFields[5];
    entry.modifyQosParams.modifyTc      = (GT_BOOL)inFields[10];
    entry.modifyQosParams.modifyUp      = (GT_BOOL)inFields[11];
    entry.modifyQosParams.modifyDp      = (GT_BOOL)inFields[12];
    entry.modifyQosParams.modifyDscp    = (GT_BOOL)inFields[13];
    entry.modifyQosParams.modifyExp     = (GT_BOOL)inFields[14];
    entry.dropRed                       = (GT_BOOL)inFields[15];

    switch(inFields[5])
    {
    case CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E:
        entry.tokenBucketsParams.srTcmParams.cir = (GT_U32)inFields[6];
        entry.tokenBucketsParams.srTcmParams.cbs = (GT_U32)inFields[7];
        entry.tokenBucketsParams.srTcmParams.ebs = (GT_U32)inFields[8];



        /* call cpss api function */
        result = cpssExMxPmPolicerMeteringEntrySet(devNum,
                                                   cpssExMxPmPolicerDirection,
                                                   entryIndex, &entry,
                                                   &tbParams);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%d%d%d",
                     tbParams.srTcmParams.cir,
                     tbParams.srTcmParams.cbs,
                     tbParams.srTcmParams.ebs, 0);
        break;

    case CPSS_EXMXPM_POLICER_METER_MODE_TR_TCM_E:
        entry.tokenBucketsParams.trTcmParams.cir = (GT_U32)inFields[6];
        entry.tokenBucketsParams.trTcmParams.cbs = (GT_U32)inFields[7];
        entry.tokenBucketsParams.trTcmParams.pir = (GT_U32)inFields[8];
        entry.tokenBucketsParams.trTcmParams.pbs = (GT_U32)inFields[9];

        /* call cpss api function */
        result = cpssExMxPmPolicerMeteringEntrySet(devNum,
                                                   cpssExMxPmPolicerDirection,
                                                   entryIndex, &entry,
                                                   &tbParams);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%d%d%d",
                     tbParams.trTcmParams.cir,
                     tbParams.trTcmParams.cbs,
                     tbParams.trTcmParams.pir,
                     tbParams.trTcmParams.pbs);
        break;

    default:
        break;
    }

    CPSS_EXMXPM_METERING_ENTRY_BITMAP_SET(entryIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerMeteringEntryGet
*
* DESCRIPTION:
*      Gets Metering Policer Entry configuration.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - policer entry index.
*
* OUTPUTS:
*       entryPtr        - pointer to the requested metering policer entry.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - wrong devNum, entryIndex.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS meteringEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_METERING_ENTRY_STC entry;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (meteringEntryIndex >= PRV_CPSS_EXMXPM_POLICER_MAX_INDEX_CNS)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    while((CPSS_EXMXPM_METERING_ENTRY_BITMAP_GET(meteringEntryIndex) == 0) &&
           meteringEntryIndex < PRV_CPSS_EXMXPM_POLICER_MAX_INDEX_CNS)
    {
        meteringEntryIndex++;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerMeteringEntryGet(devNum, cpssExMxPmPolicerDirection,
                                               meteringEntryIndex, &entry);
    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = meteringEntryIndex;
    inFields[1] = entry.countingEntryIndex;
    inFields[2] = entry.meterColorMode;
    inFields[3] = entry.remarkMode;
    inFields[4] = entry.mngCounterSet;
    inFields[5] = entry.meterMode;

    switch(entry.meterMode)
    {
        case CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E:
            inFields[6] = entry.tokenBucketsParams.srTcmParams.cir;
            inFields[7] = entry.tokenBucketsParams.srTcmParams.cbs;
            inFields[8] = entry.tokenBucketsParams.srTcmParams.ebs;
            inFields[9] = 0;
        break;

        case CPSS_EXMXPM_POLICER_METER_MODE_TR_TCM_E:
            inFields[6] = entry.tokenBucketsParams.trTcmParams.cir;
            inFields[7] = entry.tokenBucketsParams.trTcmParams.cbs;
            inFields[8] = entry.tokenBucketsParams.trTcmParams.pir;
            inFields[9] = entry.tokenBucketsParams.trTcmParams.pbs;
        break;

    default:
        break;
    }

    inFields[10] = entry.modifyQosParams.modifyTc;
    inFields[11] = entry.modifyQosParams.modifyUp;
    inFields[12] = entry.modifyQosParams.modifyDp;
    inFields[13] = entry.modifyQosParams.modifyDscp;
    inFields[14] = entry.modifyQosParams.modifyExp;
    inFields[15] = entry.dropRed;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],
                inFields[9],  inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14],
                inFields[15]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/***********************************************************/

static CMD_STATUS wrCpssExMxPmPolicerMeteringEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    meteringEntryIndex = 0;

    return meteringEntryGet(inArgs, inFields, numFields, outArgs);
}

/***********************************************************/

static CMD_STATUS wrCpssExMxPmPolicerMeteringEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    meteringEntryIndex++;

    return meteringEntryGet(inArgs, inFields, numFields, outArgs);
}

/*******************************************************************************
* cpssExMxPmPolicerEntryMeterParamsCalculate
*
* DESCRIPTION:
*      Calculate Token Bucket parameters in the Application format without
*      HW update.
*      The token bucket parameters are returned as output values. This is
*      due to the hardware rate resolution, the exact rate or burst size
*      requested may not be honored. The returned value gives the user the
*      actual parameters configured in the hardware.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number
*       tbInParamsPtr   - pointer to Token bucket input parameters.
*       meterMode       - Meter mode (SrTCM or TrTCM).
*
* OUTPUTS:
*       tbOutParamsPtr  - pointer to Token bucket output paramters.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerEntryMeterParamsCalculate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_METER_MODE_ENT meterMode;
    CPSS_EXMXPM_POLICER_METER_TB_PARAMS_UNT tbInParams;
    CPSS_EXMXPM_POLICER_METER_TB_PARAMS_UNT tbOutParams;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    meterMode = (CPSS_EXMXPM_POLICER_METER_MODE_ENT)inArgs[1];

    switch(inArgs[1])
    {
    case CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E:
        tbInParams.srTcmParams.cir = (GT_U32)inArgs[2];
        tbInParams.srTcmParams.cbs = (GT_U32)inArgs[3];
        tbInParams.srTcmParams.ebs = (GT_U32)inArgs[4];
        break;

    case CPSS_EXMXPM_POLICER_METER_MODE_TR_TCM_E:
        tbInParams.trTcmParams.cir = (GT_U32)inArgs[2];
        tbInParams.trTcmParams.cbs = (GT_U32)inArgs[3];
        tbInParams.trTcmParams.pbs = (GT_U32)inArgs[4];
        tbInParams.trTcmParams.pir = (GT_U32)inArgs[5];
        break;

    default:
        break;
    }

    /* call cpss api function */
    result = cpssExMxPmPolicerEntryMeterParamsCalculate(devNum,
                                                        meterMode,
                                                        &tbInParams, &tbOutParams);

        /* pack output arguments to galtis string */
    switch(inArgs[1])
    {
    case CPSS_EXMXPM_POLICER_METER_MODE_SR_TCM_E:
        galtisOutput(outArgs, result, "%d%d%d%s",
                     tbOutParams.srTcmParams.cir,
                     tbOutParams.srTcmParams.cbs,
                     tbOutParams.srTcmParams.ebs, "NO_SUCH");
        return CMD_OK;

    case CPSS_EXMXPM_POLICER_METER_MODE_TR_TCM_E:
        galtisOutput(outArgs, result, "%d%d%d%d",
                     tbOutParams.trTcmParams.cir,
                     tbOutParams.trTcmParams.cbs,
                     tbOutParams.trTcmParams.pbs,
                     tbOutParams.trTcmParams.pir);
        return CMD_OK;

    default:
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
}

/*******************************************************************************
* cpssExMxPmPolicerBillingEntrySet
*
* DESCRIPTION:
*      Set Policer Billing Counters.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index set by Policy Action Entry and belongs to the
*                         range from 0 up to maximal number of Policer Billing
*                         counters.
*       billingCntrPtr  - Billing Counters Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on disabled Billing Counting.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or entryIndex.
*
* COMMENTS:
*       The billing counters are free-running no-sticky counters.
*       Each entry counts green, yellow and red data units.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerBillingEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 entryIndex;
    CPSS_EXMXPM_POLICER_BILLING_ENTRY_STC billingCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    entryIndex = (GT_U32)inArgs[1];

    billingCntr.cntrMode = (CPSS_EXMXPM_POLICER_BILLING_CNTR_MODE_ENT)inArgs[2];
    galtisU64(&(billingCntr.greenCntr), inArgs[3], inArgs[4]);
    galtisU64(&(billingCntr.yellowCntr), inArgs[5], inArgs[6]);
    galtisU64(&(billingCntr.redCntr), inArgs[7], inArgs[8]);

    /* call cpss api function */
    result = cpssExMxPmPolicerBillingEntrySet(devNum, cpssExMxPmPolicerDirection,
                                              entryIndex, &billingCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/************* Table: cpssExMxPmPolicerBillingEntry ***************************/
static  GT_U32 minBillingEntryIndex = 0;
static  GT_U32 maxBillingEntryIndex = PRV_CPSS_EXMXPM_POLICER_MAX_INDEX_CNS;
static  GT_U32 billingEntryIndex = 0;

/*******************************************************************************
* cpssExMxPmPolicerBillingEntryGet
*
* DESCRIPTION:
*      Gets Policer Billing Counters.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index set by Policy Action Entry and belongs to the
*                         range from 0 up to maximal number of Policer Billing
*                         counters.
*       reset           - reset flag.
*                         GT_TRUE  - read-and-reset atomic operation is
*                                    performed.
*                         GT_FALSE - entry doesn't reset its value. Only
*                                    get counters values operation is perform.
*
* OUTPUTS:
*       billingCntrPtr  - Pointer to the Billing Counters Entry.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on disabled Billing Counting.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or entryIndex.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_TIMEOUT      - on time out.
*
* COMMENTS:
*       The billing counters are free-running no-sticky counters.
*       Each entry counts green, yellow and red data units.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerBillingEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 entryIndex;
    GT_BOOL reset;
    CPSS_EXMXPM_POLICER_BILLING_ENTRY_STC billingCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryIndex = (GT_U32)inArgs[1];
    reset = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPolicerBillingEntryGet(devNum, cpssExMxPmPolicerDirection,
                                              entryIndex, reset, &billingCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d",
                 billingCntr.cntrMode,
                 billingCntr.greenCntr.l[0],
                 billingCntr.greenCntr.l[1],
                 billingCntr.yellowCntr.l[0],
                 billingCntr.yellowCntr.l[1],
                 billingCntr.redCntr.l[0],
                 billingCntr.redCntr.l[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerVlanCntrSet
*
* DESCRIPTION:
*      Sets Policer VLAN Counters located in the Policer Counting Entry upon
*      enabled VLAN counting mode.
*      When working in VLAN Counting Mode each counter counts the number of
*      packets or bytes according to the VLAN counters mode set
*      by cpssExMxPmPolicerVlanCountingModeSet(). Index of counter is VLAN ID.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*       vid             - VLAN ID. Bits [1:0] of VID are used to select the
*                         relevant 32 bit counter inside the Counting Entry.
*       cntrValue       - Packet/Byte counter indexed by the packet's VID.
*                         For counter reset this parameter should be nullified.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on disabled VLAN Counting.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or vid (VID range is limited by max
*                         number of Policer Counting Entries).
*
* COMMENTS:
*       The VLAN counters are free-running no-sticky counters.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerVlanCntrSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16 vid;
    GT_U32 cntrValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vid = (GT_U16)inArgs[1];
    cntrValue = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPolicerVlanCntrSet(devNum, cpssExMxPmPolicerDirection,
                                          vid, cntrValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerVlanCntrGet
*
* DESCRIPTION:
*      Gets Policer VLAN Counters located in the Policer Counting Entry upon
*      enabled VLAN counting mode.
*      When working in VLAN Counting Mode each counter counts the number of
*      packets or bytes according to the VLAN counters mode set
*      by cpssExMxPmPolicerVlanCountingModeSet(). Index of counter is VLAN ID.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*       vid             - VLAN ID. Bits [1:0] of VID are used to select the
*                         relevant 32 bit counter inside the Counting Entry.
*
* OUTPUTS:
*       cntrValuePtr    - Pointer to the packet/Byte counter indexed by
*                         the packet's VID.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on disabled VLAN Counting.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or vid (VID range is limited by max
*                         number of Policer Counting Entries).
*       GT_BAD_PTR      - on NULL pointer.
*       GT_TIMEOUT      - on time out.
*
* COMMENTS:
*       The VLAN counters are free-running no-sticky counters.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerVlanCntrGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16 vid;
    GT_U32 cntrValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vid = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerVlanCntrGet(devNum, cpssExMxPmPolicerDirection,
                                          vid, &cntrValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntrValue);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerPolicyCntrSet
*
* DESCRIPTION:
*      Sets Policer Policy Counters located in the Policer Counting Entry upon
*      enabled Policy Counting mode.
*      When working in Policy Counting Mode each counter counts the number of
*      packets matching on a Policy rules.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*       index           - index set by Policy Action Entry and belongs to the
*                         range from 0 up to maximal number of Policer Policy
*                         counters.
*       cntrValue       - packets counter.
*                         For counter reset this parameter should be nullified.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on disabled Policy Counting.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or index (index range is limited by
*                         max number of Policer Policy counters).
*
* COMMENTS:
*       The Policy counters are free-running no-sticky counters.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerPolicyCntrSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    GT_U32 cntrValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    cntrValue = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPolicerPolicyCntrSet(devNum, cpssExMxPmPolicerDirection,
                                            index, cntrValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerPolicyCntrGet
*
* DESCRIPTION:
*      Gets Policer Policy Counters located in the Policer Counting Entry upon
*      enabled Policy Counting mode.
*      When working in Policy Counting Mode each counter counts the number of
*      packets matching on a a Policy rules.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*       index           - index set by Policy Action Entry and belongs to the
*                         range from 0 up to maximal number of Policer Policy
*                         counters.
*
* OUTPUTS:
*       cntrValuePtr    - Pointer to the packet counter.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on disabled Policy Counting.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or index.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_TIMEOUT      - on time out.
*
* COMMENTS:
*       The Policy counters are free-running no-sticky counters.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerPolicyCntrGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    GT_U32 cntrValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerPolicyCntrGet(devNum, cpssExMxPmPolicerDirection,
                                            index, &cntrValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntrValue);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerQosRemarkEntrySet
*
* DESCRIPTION:
*       Sets Policer Re-Marking table Entry.
*       Re-marking is the process of assigning new QoS parameters to the
*       packet, at the end of the metering process, based on the
*       Conformance Level the packet is assigned by metering process.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum              - device number.
*       remarkParamType     - QoS parameter on which remarking is based: DSCP,
*                             EXP,TC or UP.
*       remarkParamValue    - QoS parameter value.
*       confLevel           - Conformance Level: Green, Yellow, Red.
*       qosParamPtr         - pointer to the Re-Marking Entry going to be set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum, remarkParamType or confLevel.
*                             or remarkParamValue out of range:
*                              -------------------
*                             |QoS param | Range  |
*                             |----------|--------|
*                             |  DSCP    | [0:63] |
*                             |  TC      | [0:7]  |
*                             |  UP      | [0:7]  |
*                             |  EXP     | [0:7]  |
*                              -------------------
*       GT_BAD_PTR          - on NULL pointer.
*       GT_OUT_OF_RANGE     - on QoS parameter out of range
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerQosRemarkEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_QOS_PARAM_ENT remarkParamType;
    GT_U32 remarkParamValue;
    CPSS_DP_LEVEL_ENT confLevel;
    CPSS_EXMXPM_QOS_PARAM_STC qosParam;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    remarkParamType = (CPSS_EXMXPM_QOS_PARAM_ENT)inArgs[1];
    remarkParamValue = inArgs[2];
    confLevel = (CPSS_DP_LEVEL_ENT)inArgs[3];

    qosParam.tc = inFields[0];
    qosParam.dp = (CPSS_DP_LEVEL_ENT)inFields[1];
    qosParam.up = inFields[2];
    qosParam.dscp = inFields[3];
    qosParam.exp = inFields[4];

    /* call cpss api function */
    result = cpssExMxPmPolicerQosRemarkEntrySet(devNum,
                                                cpssExMxPmPolicerDirection,
                                                0, remarkParamType,
                                                remarkParamValue, confLevel, &qosParam);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerQosRemarkEntryGet
*
* DESCRIPTION:
*       Gets Policer Re-Marking table Entry.
*       Re-marking is the process of assigning new QoS parameters to the
*       packet, at the end of the metering process, based on the
*       Conformance Level the packet is assigned by metering process.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum              - device number.
*       remarkParamType     - QoS parameter on which remarking is based: DSCP,
*                             EXP,TC or UP.
*       remarkParamValue    - QoS parameter value.
*       confLevel           - Conformance Level: Green, Yellow, Red.
*
* OUTPUTS:
*       qosParamPtr         - pointer to the requested Re-Marking Entry.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum, remarkParamType or confLevel.
*                             or remarkParamValue out of range:
*                              -------------------
*                             |QoS param | Range  |
*                             |----------|--------|
*                             |  DSCP    | [0:63] |
*                             |  TC      | [0:7]  |
*                             |  UP      | [0:7]  |
*                             |  EXP     | [0:7]  |
*                              -------------------
*       GT_BAD_PTR          - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerQosRemarkEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_QOS_PARAM_ENT remarkParamType;
    GT_U32 remarkParamValue;
    CPSS_DP_LEVEL_ENT confLevel;
    CPSS_EXMXPM_QOS_PARAM_STC qosParam;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    remarkParamType = (CPSS_EXMXPM_QOS_PARAM_ENT)inArgs[1];
    remarkParamValue = inArgs[2];
    confLevel = (CPSS_DP_LEVEL_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmPolicerQosRemarkEntryGet(devNum,
                                                cpssExMxPmPolicerDirection,
                                                0, remarkParamType,
                                                remarkParamValue, confLevel, &qosParam);

    inFields[0] = qosParam.tc;
    inFields[1] = qosParam.dp;
    inFields[2] = qosParam.up;
    inFields[3] = qosParam.dscp;
    inFields[4] = qosParam.exp;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1],
                              inFields[2], inFields[3],
                              inFields[4]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerMeteringEntryRefresh
*
* DESCRIPTION:
*       Refresh the Policer Metering Entry.
*       Meter Bucket's state parameters refreshing are needed in order to
*       prevent mis-behavior due to wrap around of timers.
*
*       The wrap around problem can occur when there are long periods of
*       'silence' on the flow and the bucket's state parameters do not get
*       refreshed (meter is in the idle state). This causes a problem once the
*       flow traffic is re-started since the timers might have already wrapped
*       around which can cause a situation in which the bucket's state is
*       mis-interpreted and the incoming packet is marked as non-conforming
*       even though the bucket was actually supposed to be full.
*       To prevent this from happening the CPU needs to trigger a meter
*       refresh transaction at least once every 10 minutes per meter.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index of Policer Metering Entry
*                         going to be refreshed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or entryIndex.
*       GT_TIMEOUT      - on time out.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerMeteringEntryRefresh
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 entryIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    entryIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerMeteringEntryRefresh(devNum,
                                                   cpssExMxPmPolicerDirection,
                                                   entryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerMeteringAutoRefreshIntervalSet
*
* DESCRIPTION:
*       Sets the time interval between two refresh access to metering table.
*       The Auto Refresh Scan mechanism is enabled by the
*       cpssExMxPmPolicerMeteringAutoRefreshScanEnableSet().
*       The range of Auto Refresh Scan is configurable by the
*       cpssExMxPmPolicerMeteringAutoRefreshRangeSet().
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*       interval        - time interval between refresh of two entries
*                         in micro seconds. Upon 270 Mhz core clock value
*                         the range [0..15907286 microS].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_OUT_OF_RANGE             - on out of range of interval value.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerMeteringAutoRefreshIntervalSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 interval;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    interval = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerMeteringAutoRefreshIntervalSet(devNum,
                                                             cpssExMxPmPolicerDirection,
                                                             interval);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerMeteringAutoRefreshIntervalGet
*
* DESCRIPTION:
*       Gets the time interval between two refresh access to metering table.
*       The Auto Refresh Scan mechanism is enabled by the
*       cpssExMxPmPolicerMeteringAutoRefreshScanEnableSet().
*       The range of Auto Refresh Scan is configurable by the
*       cpssExMxPmPolicerMeteringAutoRefreshRangeSet().
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       intervalPtr     - pointer to time interval between refresh of two entries
*                         in micro seconds. Upon 270 Mhz core clock value
*                         the range [0..15907286 microS].
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_OUT_OF_RANGE             - on out of range of interval value.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerMeteringAutoRefreshIntervalGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 interval;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerMeteringAutoRefreshIntervalGet(devNum,
                                                             cpssExMxPmPolicerDirection,
                                                             &interval);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", interval);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerMeteringAutoRefreshRangeSet
*
*
* DESCRIPTION:
*       Sets Metering Refresh Scan address range (Start and Stop addresses).
*       The Auto Refresh Scan mechanism is enabled by the
*       cpssExMxPmPolicerMeteringAutoRefreshScanEnableSet().
*       The Auto Refresh Scan Rate can be controlled by setting time interval
*       between two refresh access to metering table. The time interval is
*       configured by the cpssExMxPmPolicerMeteringAutoRefreshIntervalSet().
*
* APPLICABLE DEVICES: ExMxPm and above.
*
* INPUTS:
*       devNum              - physical device number.
*       startEntryIndex     - beginning of the address range to be scanned
*                             by the Auto Refresh Scan mechanism (metering table indexes[0..8K]).
*       stopEntryIndex      - end of the address range to be scanned by the
*                             Auto Refresh Scan mechanism (metering table indexes[0..8K]).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum, or illegal values of
*                                     startEntryIndex and stopEntryIndex.
*       GT_OUT_OF_RANGE             - on out of range of startEntryIndex
*                                     or stopEntryIndex.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerMeteringAutoRefreshRangeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 startEntryIndex;
    GT_U32 stopEntryIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    startEntryIndex = (GT_U32)inArgs[1];
    stopEntryIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPolicerMeteringAutoRefreshRangeSet(devNum,
                                                          cpssExMxPmPolicerDirection,
                                                          startEntryIndex,
                                                          stopEntryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;

}


/*******************************************************************************
* cpssExMxPmPolicerMeteringAutoRefreshRangeGet
*
* DESCRIPTION:
*       Gets Metering Refresh Scan address range (Start and Stop addresses).
*       The Auto Refresh Scan mechanism is enabled by the
*       cpssExMxPmPolicerMeteringAutoRefreshScanEnableSet().
*       The Auto Refresh Scan Rate can be controlled by setting time interval
*       between two refresh access to metering table. The time interval is
*       configured by the cpssExMxPmPolicerMeteringAutoRefreshIntervalSet().
*
* APPLICABLE DEVICES: ExMxPm and above.
*
* INPUTS:
*       devNum              - physical device number.
*
* OUTPUTS:
*       startEntryIndexPtr  - pointer to the beginning address of Refresh
*                             Scan address range (metering table indexes[0..8K]).
*       stopEntryIndexPtr   - pointer to the end address of Refresh Scan
*                             address range (metering table indexes[0..8K]).
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointers.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerMeteringAutoRefreshRangeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 startEntryIndex;
    GT_U32 stopEntryIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerMeteringAutoRefreshRangeGet(devNum,
                                                          cpssExMxPmPolicerDirection,
                                                          &startEntryIndex,
                                                          &stopEntryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", startEntryIndex, stopEntryIndex);

    return CMD_OK;

}

/*******************************************************************************
* cpssExMxPmPolicerMeteringAutoRefreshScanEnableSet
*
* DESCRIPTION:
*       Enables or disables the metering Auto Refresh Scan mechanism.
*
*       If no traffic passes through a meter (long periods of "silence") the
*       meter state variables do not get refreshed. This can cause a problem of
*       state misinterpreted once the flow traffic is restarted, because the
*       timers can have already wrapped around.
*       To avoid this problem the software must initiate a refresh transaction
*       on the meter every ten minutes.
*
*       This CPU intensive metering refresh maintanance can be offloaded by
*       Auto Refresh Scan mechanism.
*       The range of Auto Refresh Scan is configurable by the
*       cpssExMxPmPolicerMeteringAutoRefreshRangeSet().
*       The Auto Refresh Scan Rate can be controlled by setting time interval
*       between two refresh access to metering table. The time interval is
*       configured by the cpssExMxPmPolicerMeteringAutoRefreshIntervalSet().
*
* APPLICABLE DEVICES: ExMxPm and above.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - Enable/disable Auto Refresh Scan mechanism:
*                         GT_TRUE  - enable Auto Refresh scan mechanism on
*                                    the device.
*                         GT_FALSE - disable Auto Refresh scan mechanism on
*                                    the device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The CPU is still capable to perform any Refresh Transactions during
*       the Auto Refresh Scan operation.
*       Once the CPU triggers Refresh transaction, the policer accesses the
*       metering entry and performs metering refresh with the packet's Byte
*       Count set to zero.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerMeteringAutoRefreshScanEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerMeteringAutoRefreshScanEnableSet(devNum,
                                                               cpssExMxPmPolicerDirection,
                                                               enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;

}

/*******************************************************************************
* cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet
*
* DESCRIPTION:
*       Gets metering Auto Refresh Scan mechanism status (Enabled/Disabled).
*
*       If no traffic passes through a meter (long periods of "silence") the
*       meter state variables do not get refreshed. This can cause a problem of
*       state misinterpreted once the flow traffic is restarted, because the
*       timers can have already wrapped around.
*       To avoid this problem the software must initiate a refresh transaction
*       on the meter every ten minutes.
*
*       This CPU intensive metering refresh maintanance can be offloaded by
*       Auto Refresh Scan mechanism.
*       The range of Auto Refresh Scan is configurable by the
*       cpssExMxPmPolicerMeteringAutoRefreshRangeSet().
*       The Auto Refresh Scan Rate can be controlled by setting time interval
*       between two refresh access to metering table. The time interval is
*       configured by the cpssExMxPmPolicerMeteringAutoRefreshIntervalSet().
*
* APPLICABLE DEVICES: ExMxPm and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer to Auto Refresh Scan mechanism:
*                         GT_TRUE  - enable Auto Refresh scan mechanism on
*                                    the device.
*                         GT_FALSE - disable Auto Refresh scan mechanism on
*                                    the device.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       The CPU is still capable to perform any Refresh Transactions during
*       the Auto Refresh Scan operation.
*       Once the CPU triggers Refresh transaction, the policer accesses the
*       metering entry and performs metering refresh with the packet's Byte
*       Count set to zero.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerMeteringAutoRefreshScanEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet(devNum,
                                                               cpssExMxPmPolicerDirection,
                                                               &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerMeterResolutionSet
*
* DESCRIPTION:
*       Sets metering algorithm resolution: packets per
*       second or bytes per second.
*
* APPLICABLE DEVICES: ExMxPm and above.
*
* INPUTS:
*       devNum          - physical device number.
*       resolution      - packet/Byte based Meter resolution.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum or resolution.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS  wrCpssExMxPmPolicerMeterResolutionSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_METER_RESOLUTION_ENT  resolution;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    resolution = (CPSS_EXMXPM_POLICER_METER_RESOLUTION_ENT)inArgs[1];

    result = cpssExMxPmPolicerMeterResolutionSet(devNum,
                                                 cpssExMxPmPolicerDirection,
                                                 resolution);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerMeterResolutionGet
*
* DESCRIPTION:
*       Gets metering algorithm resolution: packets per
*       second or bytes per second.
*
* APPLICABLE DEVICES: ExMxPm and above.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       resolutionPtr   - pointer to the Meter resolution: packet or Byte based.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_HW_ERROR                 - on Hardware error.
*       GT_BAD_PARAM                - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE    - on devNum of not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerMeterResolutionGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_METER_RESOLUTION_ENT   resolution;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerMeterResolutionGet(devNum,
                                                 cpssExMxPmPolicerDirection,
                                                 &resolution);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", resolution);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerPortQosRemarkProfileSet
*
* DESCRIPTION:
*       The function sets QoS remap profile per port.
*       There are four remap profiles. The profile determines the QoS remapping
*       tables sets to use.
*
* APPLICABLE DEVICES: ExMxPm and above.
*
* INPUTS:
*       devNum              - device number.
*       port                - port number.
*       remarkProfileTableIndex
*                           - remark profile table index (range 0-3)
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK               - success.
*       GT_HW_ERROR         - hardware error.
*       GT_BAD_PARAM        - wrong input parameters.
*       GT_OUT_OF_RANGE             - on out of range of remarkProfileTableIndex
*       GT_NOT_APPLICABLE_DEVICE    - not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerPortQosRemarkProfileSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32  port;
    GT_U32  remarkProfileTableIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U32)inArgs[1];
    remarkProfileTableIndex = (GT_U32)inArgs[2];

    result = cpssExMxPmPolicerPortQosRemarkProfileSet(devNum,
                                                      cpssExMxPmPolicerDirection,
                                                      port,
                                                      remarkProfileTableIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerPortQosRemarkProfileGet
*
* DESCRIPTION:
*       The function gets QoS remap profile per port.
*       There are four remap profiles. The profile determines the QoS remapping
*       tables sets to use.
*
* APPLICABLE DEVICES: ExMxPm and above.
*
* INPUTS:
*       devNum              - device number.
*       port                - port number
*
* OUTPUTS:
*       remarkProfileTableIndexPtr
*                           - pointer remark profile table index (range 0-3)
*
* RETURNS:
*       GT_OK               - success.
*       GT_HW_ERROR         - hardware error.
*       GT_BAD_PARAM        - wrong input parameters.
*       GT_BAD_PTR          - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerPortQosRemarkProfileGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32  port;
    GT_U32  remarkProfileTableIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerPortQosRemarkProfileGet(devNum,
                                                      cpssExMxPmPolicerDirection,
                                                      port,
                                                      &remarkProfileTableIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", remarkProfileTableIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerQosUpdateEnableSet
*
* DESCRIPTION:
*       The function enables or disables QoS remarking of conforming packets.
*
* APPLICABLE DEVICES: ALL ExMxPm and above.
*
* INPUTS:
*       devNum  - physical device number.
*       enable  - Enable/disable Qos update for conforming packets:
*                 GT_TRUE  - Qos update is enabled on the device.
*                 GT_FALSE - Qos update is disabled on the device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerQosUpdateEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    result = cpssExMxPmPolicerQosUpdateEnableSet(devNum,
                                                 cpssExMxPmPolicerDirection,
                                                 enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerQosUpdateEnableGet
*
* DESCRIPTION:
*       Gets Qos profile update for conforming packets (enable/disable).
*
* APPLICABLE DEVICES: ALL ExMxPm and above.
*
* INPUTS:
*       devNum  - physical device number.
*
* OUTPUTS:
*       enablePtr - pointer to Qos update state for conforming packets:
*                 GT_TRUE  - Qos update is enabled on the device.
*                 GT_FALSE - Qos update is disabled on the device.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointers.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerQosUpdateEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPolicerQosUpdateEnableGet(devNum,
                                                 cpssExMxPmPolicerDirection,
                                                 &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerCountingWriteBackCacheFlush
*
* DESCRIPTION:
*       Flush internal Write Back Cache (WBC) of counting entries.
*       The Policer implements internal Write Back Cache for frequent and rapid
*       update of counting entries.
*       Since the WBC holds the policer entries' data, the CPU must clear its
*       content when performing direct write access of the counting entries by
*       one of following functions:
*        - cpssExMxPmPolicerBillingEntrySet
*        - cpssExMxPmPolicerVlanCntrSet
*        - cpssExMxPmPolicerPolicyCntrSet
*       The policer counting should be disabled before flush of WBC if direct
*       write access is performed under traffic. And policer counting should be
*       enabled again after finish of write access.
*       The cpssExMxPmPolicerCountingModeSet may be used to disable/enable the
*       policer counting.
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
* INPUTS:
*       devNum                  - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_HW_ERROR              - on Hardware error.
*       GT_TIMEOUT               - on time out.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerCountingWriteBackCacheFlush
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    result = cpssExMxPmPolicerCountingWriteBackCacheFlush (devNum,
                                                           cpssExMxPmPolicerDirection);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* wrCpssExMxPmPolicerCountingModeSet
*
* DESCRIPTION:
*       Gets the counting configuration: counting mode.
*       The counting may be disabled or work in one of the following modes:
*       - Billing - Policer performs billing counting when triggered from PCL
*                   or Metering Entry.
*       - Policy  - Policer performs packet counting when triggered from PCL.
*       - Vlan    - Policer performs packet counting acording to packet's VID
*                   for each packet. This mode triggered by the Packet Command.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum                  - physical device number.
*       mode                    - Policer counting mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, mode or numOfCountingEntries.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerCountingModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPolicerCountingModeSet(devNum,
                                              cpssExMxPmPolicerDirection,
                                              mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmPolicerGetNext
*
* DESCRIPTION:
*       Default getNext Function to stop loop.

*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or entryIndex.
*       GT_TIMEOUT      - on time out.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d", -1);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerBillingEntryGet
*
* DESCRIPTION:
*      Gets Policer Billing Counters by range.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*       entryIndex      - index set by Policy Action Entry and belongs to the
*                         range from 0 up to maximal number of Policer Billing
*                         counters.
*       reset           - reset flag.
*                         GT_TRUE  - read-and-reset atomic operation is
*                                    performed.
*                         GT_FALSE - entry doesn't reset its value. Only
*                                    get counters values operation is perform.
* OUTPUTS:
*       billingCntrPtr  - Pointer to the Billing Counters Entry.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on disabled Billing Counting.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or entryIndex.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_TIMEOUT      - on time out.
*
* COMMENTS:
*       The billing counters are free-running no-sticky counters.
*       Each entry counts green, yellow and red data units.
*
*******************************************************************************/
static CMD_STATUS privateCpssExMxPmPolicerBillingEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL reset;
    CPSS_EXMXPM_POLICER_BILLING_ENTRY_STC billingCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (billingEntryIndex > maxBillingEntryIndex ||
        billingEntryIndex >= PRV_CPSS_EXMXPM_POLICER_MAX_INDEX_CNS)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    reset = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPolicerBillingEntryGet(devNum, cpssExMxPmPolicerDirection,
                                              billingEntryIndex, reset,
                                              &billingCntr);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = billingCntr.greenCntr.l[0];
    inFields[1] = billingCntr.greenCntr.l[1];
    inFields[2] = billingCntr.yellowCntr.l[0];
    inFields[3] = billingCntr.yellowCntr.l[1];
    inFields[4] = billingCntr.redCntr.l[0];
    inFields[5] = billingCntr.redCntr.l[1];
    inFields[6] = billingCntr.cntrMode;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],
                inFields[6]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*********************************************************************************/
static CMD_STATUS wrCpssExMxPmPolicerBillingEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    billingEntryIndex = (GT_U32)inArgs[1];
    minBillingEntryIndex = maxBillingEntryIndex = billingEntryIndex;

    return privateCpssExMxPmPolicerBillingEntryGet(inArgs, inFields,
                                                   numFields, outArgs);
}

/***********************************************************/
static CMD_STATUS wrCpssExMxPmPolicerBillingEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    billingEntryIndex++;

    return privateCpssExMxPmPolicerBillingEntryGet(inArgs, inFields,
                                                   numFields, outArgs);
}

/*******************************************************************************
* cpssExMxPmPolicerConfigSet
*
* DESCRIPTION:
*       Sets the metering/counting configuration: counting mode, number of desired
*       Policer Counting Entries and Metering Entries.
*       The counting may be disabled or work in one of the following modes:
*       - Billing - Policer performs billing counting when triggered from PCL
*                   or Metering Entry.
*       - Policy  - Policer performs packet counting when triggered from PCL.
*       - Vlan    - Policer performs packet counting acording to packet's VID
*                   for each packet. This mode triggered by the Packet Command.
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
* INPUTS:
*       devNum                  - physical device number.
*       policerConfPtr          - pointer to policer configuration structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                   - on success.
*       GT_BAD_PARAM            - on wrong devNum, mode or numOfCountingEntries.
*       GT_HW_ERROR             - on Hardware error.
*       GT_OUT_OF_RANGE         - on number of Policer Counting Entries that is out
*                                 of range of [1-8K].
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The fuction divides the policer memory (policing table) up to 4 parts.
*       Indexes described here below:
*       Egress Meters:   (0) to (number of Egress Meters - 1)
*       Ingress Meters:  (number of Egress Meters) to
*                         (number of Egress and Ingress Meters - 1)
*       Egress Counters: (number of Egress and Ingress Meters) to
*                         (number of Egress and Ingress Meters + number of Egress Counters - 1)
*       Ingress Counters: (number of Egress and Ingress Meters + number of Egress Counters) to
*                         (end of policing table)
*
*       In case Counters/Meters sharing is enabled, both Ingress and Egress
*       Counters/Meters use the same place respectively.
*
*******************************************************************************/
/*GT_STATUS cpssExMxPmPolicerConfigSet
(
    IN GT_U8                                devNum,
    IN CPSS_EXMXPM_POLICER_CONFIG_STC       *policerConfPtr
)*/
static CMD_STATUS wrCpssExMxPmPolicerConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;
    GT_U8                                devNum;
    CPSS_EXMXPM_POLICER_CONFIG_STC       policerConf;
    GT_U32      ii;/*index in inArgs[] */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    ii = 0;

/*0*/ devNum                          = (GT_U8)inArgs[ii++];
/*1*/ policerConf.sharedCounters      = (GT_BOOL)inArgs[ii++];
/*2*/ policerConf.ingrCountingMode    = (CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT)inArgs[ii++];
/*3*/ policerConf.ingrCountingEntries = inArgs[ii++];
/*4*/ policerConf.egrCountingMode     = (CPSS_EXMXPM_POLICER_COUNTING_MODE_ENT)inArgs[ii++];
/*5*/ policerConf.egrCountingEntries  = inArgs[ii++];
/*6*/ policerConf.sharedMeters        = (GT_BOOL)inArgs[ii++];
/*7*/ policerConf.ingrMeterEntries    = inArgs[ii++];
/*8*/ policerConf.useInternalBufMemForIngressMeters = (GT_BOOL)inArgs[ii++];

    /* call cpss api function */
    rc = cpssExMxPmPolicerConfigSet(devNum, &policerConf);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPolicerConfigGet
*
* DESCRIPTION:
*       Gets the metering/counting configuration: counting mode, number of desired
*       Policer Counting Entries and Metering Entries.
*       The counting may be disabled or work in one of the following modes:
*       - Billing - Policer performs billing counting when triggered from PCL
*                   or Metering Entry.
*       - Policy  - Policer performs packet counting when triggered from PCL.
*       - Vlan    - Policer performs packet counting acording to packet's VID
*                   for each packet. This mode triggered by the Packet Command.
*
* APPLICABLE DEVICES: All ExMxPx Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       policerConfPtr  - pointer to policer configuration structure
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters.
*       GT_HW_ERROR              - on Hardware error.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Based on the policing table division (described in function
*       "cpssExMxPmPolicerConfigSet" comments) Ingress and Egress,
*       Counters and Meters values are derived.
*
*******************************************************************************/
/*GT_STATUS cpssExMxPmPolicerConfigGet
(
    IN  GT_U8                              devNum,
    OUT CPSS_EXMXPM_POLICER_CONFIG_STC   * policerConfPtr
)*/
static CMD_STATUS wrCpssExMxPmPolicerConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8 devNum;
    CPSS_EXMXPM_POLICER_CONFIG_STC   policerConf;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cmdOsMemSet(&policerConf, 0, sizeof(policerConf));

    /* call cpss api function */
    rc = cpssExMxPmPolicerConfigGet(devNum, &policerConf);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d%d%d%d%d%d%d"
        ,policerConf.sharedCounters
        ,policerConf.ingrCountingMode
        ,policerConf.ingrCountingEntries
        ,policerConf.egrCountingMode
        ,policerConf.egrCountingEntries
        ,policerConf.sharedMeters
        ,policerConf.ingrMeterEntries
        ,policerConf.useInternalBufMemForIngressMeters
        );


    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmPolicerDirectionSet
*
* DESCRIPTION:
*       Set ingress/egress direction for policer API's.

*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       IN CPSS_DIRECTION_ENT direction
*               - CPSS_DIRECTION_INGRESS_E  = 0
*               - CPSS_DIRECTION_EGRESS_E   = 1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong direction
*
* COMMENTS:
*       Used to set global variable cpssExMxPmPolicerDirection.
*
*******************************************************************************/
GT_STATUS cpssExMxPmPolicerDirectionSet
(
    IN CPSS_DIRECTION_ENT direction
)
{
    switch(direction)
    {
        case CPSS_DIRECTION_INGRESS_E:
        case CPSS_DIRECTION_EGRESS_E:
            cpssExMxPmPolicerDirection = direction;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmPolicerInit",
         &wrCpssExMxPmPolicerInit,
         1, 0},
        {"cpssExMxPmPolicerMeteringEnableSet",
         &wrCpssExMxPmPolicerMeteringEnableSet,
         2, 0},
        {"cpssExMxPmPolicerMeteringEnableGet",
         &wrCpssExMxPmPolicerMeteringEnableGet,
         1, 0},
        {"cpssExMxPmPolicerCountingConfigSet",
         &wrCpssExMxPmPolicerCountingConfigSet,
         3, 0},
        {"cpssExMxPmPolicerCountingConfigGet",
         &wrCpssExMxPmPolicerCountingConfigGet,
         1, 0},
        {"cpssExMxPmPolicerPacketSizeModeSet",
         &wrCpssExMxPmPolicerPacketSizeModeSet,
         2, 0},
        {"cpssExMxPmPolicerPacketSizeModeGet",
         &wrCpssExMxPmPolicerPacketSizeModeGet,
         1, 0},
        {"cpssExMxPmPolicerVlanCountingModeSet",
         &wrCpssExMxPmPolicerVlanCountingModeSet,
         2, 0},
        {"cpssExMxPmPolicerVlanCountingModeGet",
         &wrCpssExMxPmPolicerVlanCountingModeGet,
         1, 0},
        {"cpssExMxPmPolicerVlanCountingPacketCmdTriggerSet",
         &wrCpssExMxPmPolicerVlanCountingPacketCmdTriggerSet,
         3, 0},
        {"cpssExMxPmPolicerVlanCountingPacketCmdTriggerGet",
         &wrCpssExMxPmPolicerVlanCountingPacketCmdTriggerGet,
         2, 0},
        {"cpssExMxPmPolicerDropTypeSet",
         &wrCpssExMxPmPolicerDropTypeSet,
         2, 0},
        {"cpssExMxPmPolicerDropTypeGet",
         &wrCpssExMxPmPolicerDropTypeGet,
         1, 0},
        {"cpssExMxPmPolicerCountingColorModeSet",
         &wrCpssExMxPmPolicerCountingColorModeSet,
         2, 0},
        {"cpssExMxPmPolicerCountingColorModeGet",
         &wrCpssExMxPmPolicerCountingColorModeGet,
         1, 0},
        {"cpssExMxPmPolicerL2RemarkModelSet",
         &wrCpssExMxPmPolicerL2RemarkModelSet,
         2, 0},
        {"cpssExMxPmPolicerL2RemarkModelGet",
         &wrCpssExMxPmPolicerL2RemarkModelGet,
         1, 0},
        {"cpssExMxPmPolicerManagementCntrsResolutionSet",
         &wrCpssExMxPmPolicerManagementCntrsResolutionSet,
         3, 0},
        {"cpssExMxPmPolicerManagementCntrsResolutionGet",
         &wrCpssExMxPmPolicerManagementCntrsResolutionGet,
         2, 0},
        {"cpssExMxPmPolicerPacketSizeModeForTunnelTermSet",
         &wrCpssExMxPmPolicerPacketSizeModeForTunnelTermSet,
         2, 0},
        {"cpssExMxPmPolicerPacketSizeModeForTunnelTermGet",
         &wrCpssExMxPmPolicerPacketSizeModeForTunnelTermGet,
         1, 0},
        {"cpssExMxPmPolicerPortMeteringEnableSet",
         &wrCpssExMxPmPolicerPortMeteringEnableSet,
         3, 0},
        {"cpssExMxPmPolicerPortMeteringEnableGet",
         &wrCpssExMxPmPolicerPortMeteringEnableGet,
         2, 0},
        {"cpssExMxPmPolicerMruSet",
         &wrCpssExMxPmPolicerMruSet,
         2, 0},
        {"cpssExMxPmPolicerMruGet",
         &wrCpssExMxPmPolicerMruGet,
         1, 0},
        {"cpssExMxPmPolicerErrorGet",
         &wrCpssExMxPmPolicerErrorGet,
         1, 0},
        {"cpssExMxPmPolicerErrorCounterGet",
         &wrCpssExMxPmPolicerErrorCounterGet,
         1, 0},
        {"cpssExMxPmPolicerManagementCountersSet",
         &wrCpssExMxPmPolicerManagementCountersSet,
         6, 0},
        {"cpssExMxPmPolicerManagementCountersGet",
         &wrCpssExMxPmPolicerManagementCountersGet,
         3, 0},
        {"cpssExMxPmPolicerManagementCountersSet_1",
         &wrCpssExMxPmPolicerManagementCountersSet,
         6, 0},
        {"cpssExMxPmPolicerMeteringEntrySet",
         &wrCpssExMxPmPolicerMeteringEntrySet,
         1, 17},
        {"cpssExMxPmPolicerMeteringEntryGetFirst",
         &wrCpssExMxPmPolicerMeteringEntryGetFirst,
         1, 0},
        {"cpssExMxPmPolicerMeteringEntryGetNext",
         &wrCpssExMxPmPolicerMeteringEntryGetNext,
         1, 0},
        {"cpssExMxPmPolicerMeteringEntry_1Set",
         &wrCpssExMxPmPolicerMeteringEntrySet,
         1, 17},
        {"cpssExMxPmPolicerMeteringEntry_1GetFirst",
         &wrCpssExMxPmPolicerMeteringEntryGetFirst,
         1, 0},
        {"cpssExMxPmPolicerMeteringEntry_1GetNext",
         &wrCpssExMxPmPolicerMeteringEntryGetNext,
         1, 0},

        {"cpssExMxPmPolicerEntryMeterParamsCalculate",
         &wrCpssExMxPmPolicerEntryMeterParamsCalculate,
         2, 6},
        {"cpssExMxPmPolicerBillingEntrySet",
         &wrCpssExMxPmPolicerBillingEntrySet,
         9, 0},
        {"cpssExMxPmPolicerBillingEntryGet",
         &wrCpssExMxPmPolicerBillingEntryGet,
         3, 0},
        {"cpssExMxPmPolicerBillingGetFirst",
         &wrCpssExMxPmPolicerBillingEntryGetFirst,
         3, 0},
        {"cpssExMxPmPolicerBillingGetNext",
         &wrCpssExMxPmPolicerBillingEntryGetNext,
         3, 0},
        {"cpssExMxPmPolicerVlanCntrSet",
         &wrCpssExMxPmPolicerVlanCntrSet,
         3, 0},
        {"cpssExMxPmPolicerVlanCntrGet",
         &wrCpssExMxPmPolicerVlanCntrGet,
         2, 0},
        {"cpssExMxPmPolicerPolicyCntrSet",
         &wrCpssExMxPmPolicerPolicyCntrSet,
         3, 0},
        {"cpssExMxPmPolicerPolicyCntrGet",
         &wrCpssExMxPmPolicerPolicyCntrGet,
         2, 0},
        {"cpssExMxPmPolicerQosRemarkEntrySet",
         &wrCpssExMxPmPolicerQosRemarkEntrySet,
         4, 9},
        {"cpssExMxPmPolicerQosRemarkEntryGetFirst",
         &wrCpssExMxPmPolicerQosRemarkEntryGet,
         4, 0},
        {"cpssExMxPmPolicerQosRemarkEntryGetNext",
         &wrCpssExMxPmPolicerGetNext,
         4, 0},
        {"cpssExMxPmPolicerMeteringEntryRefresh",
         &wrCpssExMxPmPolicerMeteringEntryRefresh,
         2, 0},
        {"cpssExMxPmPolicerMeteringAutoRefreshIntervalSet",
         &wrCpssExMxPmPolicerMeteringAutoRefreshIntervalSet,
         2, 0},
        {"cpssExMxPmPolicerMeteringAutoRefreshIntervalGet",
         &wrCpssExMxPmPolicerMeteringAutoRefreshIntervalGet,
         1, 0},
        {"cpssExMxPmPolicerMeteringAutoRefreshRangeSet",
         &wrCpssExMxPmPolicerMeteringAutoRefreshRangeSet,
         3, 0},
        {"cpssExMxPmPolicerMeteringAutoRefreshRangeGet",
         &wrCpssExMxPmPolicerMeteringAutoRefreshRangeGet,
         1, 0},
        {"cpssExMxPmPolicerMeteringAutoRefreshScanEnableSet",
         &wrCpssExMxPmPolicerMeteringAutoRefreshScanEnableSet,
         2, 0},
        {"cpssExMxPmPolicerMeteringAutoRefreshScanEnableGet",
         &wrCpssExMxPmPolicerMeteringAutoRefreshScanEnableGet,
         1, 0},
        {"cpssExMxPmPolicerMeterResolutionSet",
         &wrCpssExMxPmPolicerMeterResolutionSet,
         2, 0},
        {"cpssExMxPmPolicerMeterResolutionGet",
         &wrCpssExMxPmPolicerMeterResolutionGet,
         1, 0},
        {"cpssExMxPmPolicerPortQosRemarkProfileSet",
         &wrCpssExMxPmPolicerPortQosRemarkProfileSet,
         3, 0},
        {"cpssExMxPmPolicerPortQosRemarkProfileGet",
         &wrCpssExMxPmPolicerPortQosRemarkProfileGet,
         2, 0},
        {"cpssExMxPmPolicerQosUpdateEnableSet",
         &wrCpssExMxPmPolicerQosUpdateEnableSet,
         2, 0},
        {"cpssExMxPmPolicerQosUpdateEnableGet",
         &wrCpssExMxPmPolicerQosUpdateEnableGet,
         1, 0},
        {"cpssExMxPmPolicerCountingWriteBackCacheFlush",
         &wrCpssExMxPmPolicerCountingWriteBackCacheFlush,
         1, 0},
        {"cpssExMxPmPolicerCountingModeSet",
         &wrCpssExMxPmPolicerCountingModeSet,
         2, 0},
        {"cpssExMxPmPolicerConfigSet",
         &wrCpssExMxPmPolicerConfigSet,
         9, 0},
        {"cpssExMxPmPolicerConfigGet",
         &wrCpssExMxPmPolicerConfigGet,
         1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmPolicer
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
GT_STATUS cmdLibInitCpssExMxPmPolicer
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

