/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmDit.c
*
* DESCRIPTION:
*       wrappers for cpssExMxPmDit.c
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
#include <cpss/exMxPm/exMxPmGen/dit/cpssExMxPmDit.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
/*******************************************************************************
* cpssExMxPmDitIpMcPairLastSet
*
* DESCRIPTION:
*       The Downstream Interface table IP Mc Dual entry is organized as a linked
*       list of pairs of DIT entries. Each DIT entry contains a <Last> bit
*       indicating whether this entry is the last entry in the list.
*       This function writes <isLast> field to first or second DIT entry in
*       a pair.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       ditPairEntryIndex - The DIT Pair entry index.
*       ditPairWriteForm  - The way to write the DIT pair, isLast field of
*                           first part or second part of DIT pair.
*       isLast            - Value to set to isLast field
*                             - 0: DIT is not last in the list
*                             - 1: DIT is last in the list
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitIpMcPairLastSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ditPairEntryIndex;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_LAST_FIELD_WRITE_MODE_ENT ditPairWriteForm;
    GT_U32 isLast;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ditPairEntryIndex = (GT_U32)inArgs[1];
    ditPairWriteForm = (CPSS_EXMXPM_DIT_IP_MC_PAIR_LAST_FIELD_WRITE_MODE_ENT)inArgs[2];
    isLast = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmDitIpMcPairLastSet(devNum, ditPairEntryIndex, ditPairWriteForm, isLast);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmDitIpMplsEntryLastSet
*
* DESCRIPTION:
*       The Downstream Interface table IP/Mpls entry is organized as a linked
*       list of DIT entries. Each DIT entry contains a <Last> bit indicating
*       whether this entry is the last entry in the list.
*       This function writes <isLast> field of DIT entry.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       ditEntryIndex - The DIT entry index.
*       ditIpMpls     - dit entry type selection (Ip/Mpls)
*       ditType       - dit type selection (unicast/multicast)
*       isLast        - Value to set to isLast field
*                         - 0: There are more entries in the list.
*                         - 1: DIT is last entry in the list.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitIpMplsEntryLastSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ditEntryIndex;
    CPSS_EXMXPM_DIT_IP_MPLS_SELECTION_ENT ditIpMpls;
    CPSS_UNICAST_MULTICAST_ENT ditType;
    GT_U32 isLast;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ditEntryIndex = (GT_U32)inArgs[1];
    ditIpMpls = (CPSS_EXMXPM_DIT_IP_MPLS_SELECTION_ENT)inArgs[2];
    ditType = (CPSS_UNICAST_MULTICAST_ENT)inArgs[3];
    isLast = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmDitIpMplsEntryLastSet(devNum, ditEntryIndex, ditIpMpls, ditType, isLast);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitTtlTableEntrySet
*
* DESCRIPTION:
*       The Downstream Interface table IP/Mpls entry's TTL index points to TTL
*       value table. These values used in MPLS label to be pushed from DIT entry.
*       This function configures single value this table.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       index         - The TTL entry index. Range 0-7.
*       value         - Value to set to TTL entry (0-255).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitTtlTableEntrySet
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
    GT_U32 value;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    value = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmDitTtlTableEntrySet(devNum, index, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitTtlTableEntryGet
*
* DESCRIPTION:
*       The Downstream Interface table IP/Mpls entry's TTL index points to TTL
*       value table. These values used in MPLS label to be pushed from DIT entry.
*       This function retreives single value from this table.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       index         - The TTL entry index. Range 0-7.
*
* OUTPUTS:
*       valuePtr      - (pointer to)ttl entry value (0-255).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitTtlTableEntryGet
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
    GT_U32 value;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmDitTtlTableEntryGet(devNum, index, &value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", value);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitVplsEntryLastSet
*
* DESCRIPTION:
*       The Downstream Interface table Vpls entry is organized as a linked
*       list of DIT entries. Each DIT entry contains a <Last> bit indicating
*       whether this entry is the last entry in the list.
*       This function writes <isLast> field of DIT entry.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       ditEntryIndex - The DIT entry index.
*       ditType       - dit packet type selection (unicast/multicast)
*       isLast        - Value to set to isLast field
*                         - 0: There are more entries in the list.
*                         - 1: DIT is last entry in the list.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitVplsEntryLastSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ditEntryIndex;
    CPSS_UNICAST_MULTICAST_ENT ditType;
    GT_U32 isLast;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ditEntryIndex = (GT_U32)inArgs[1];
    ditType = (CPSS_UNICAST_MULTICAST_ENT)inArgs[2];
    isLast = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmDitVplsEntryLastSet(devNum, ditEntryIndex, ditType, isLast);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmDitCounterGet
*
* DESCRIPTION:
*       DIT (downsteam interface table) counter counts the number of multicast
*       packets that triggered the Router engine and have a downstream interface
*       associated with this counter.
*       This function gets DIT counter value.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       counterIndex  - counter index. Range 0..1
*       type          - counter type selection (unicast/multicast)
*
* OUTPUTS:
*       valuePtr      - points to DIT counter value (range 32 bit)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on bad parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitCounterGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 counterIndex;
    CPSS_UNICAST_MULTICAST_ENT type;
    GT_U32 value;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    counterIndex = (GT_U32)inArgs[1];
    type = (CPSS_UNICAST_MULTICAST_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmDitCounterGet(devNum, counterIndex, type, &value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", value);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitBasedBridgeEnableSet
*
* DESCRIPTION:
*       The Bridge engine supports bridging of IPv4/6 Multicast packets based
*       on the packet (S, G) or (*, G).
*       This function enables/disables DIT based bridging.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       enable            - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitBasedBridgeEnableSet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmDitBasedBridgeEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitBasedBridgeEnableGet
*
* DESCRIPTION:
*       The Bridge engine supports bridging of IPv4/6 Multicast packets based
*       on the packet (S, G) or (*, G).
*       This function geta enable/disable state of DIT based bridging.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*
* OUTPUTS:
*       enablePtr         - points to enable/disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitBasedBridgeEnableGet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmDitBasedBridgeEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitMultiTargetMcQueuePriorityConfigSet
*
* DESCRIPTION:
*      Sets the priority assigned to each MC queue. This priority affects the
*      arbitration order between queues.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum             - the device number
*       queue              - the multi-target MC queue (0..3)
*       priority           - the priority to assign to the queue (0-3).
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       It is user's responsibility to assign unique priority for each queue.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitMultiTargetMcQueuePriorityConfigSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 queue;
    GT_U32 priority;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    queue = (GT_U32)inArgs[1];
    priority = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmDitMultiTargetMcQueuePriorityConfigSet(devNum, queue, priority);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet
*
* DESCRIPTION:
*      Sets the priority assigned to each MC queue. This priority affects the
*      arbitration order between queues.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum             - the device number
*       queue              - the multi-target MC queue (0..3)
*
*
* OUTPUTS:
*       priorityPtr       - (pointer to)the priority to assign to the queue (0-3).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       It is user's responsibility to assign unique priority for each queue.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitMultiTargetMcQueuePriorityConfigGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 queue;
    GT_U32 priority;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    queue = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet(devNum, queue, &priority);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", priority);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitMcTc2IpTvQueueConfigSet
*
* DESCRIPTION:
*      This function configures Mc Tc2 queue, designed to support IP TV applications.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum              - the device number
*       fullThreshold       - number of descriptors in queue before Reassembly
*       xoffThreshold       - maximal number of entries in the queue before
*                             IP Tv XON indication is asserted LOW.
*       xonThreshold        - minimal number of entries in the queue before
*                             IP Tv XON indication is asserted HIGH.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitMcTc2IpTvQueueConfigSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 fullThreshold;
    GT_U32 xoffThreshold;
    GT_U32 xonThreshold;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    fullThreshold = (GT_U32)inArgs[1];
    xoffThreshold = (GT_U32)inArgs[2];
    xonThreshold = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmDitMcTc2IpTvQueueConfigSet(devNum, fullThreshold, xoffThreshold, xonThreshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitMcTc2IpTvQueueConfigGet
*
* DESCRIPTION:
*      This function gets configuration of Mc Tc2 queue, designed to support IP
*      TV applications.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum              - the device number
*
* OUTPUTS:
*       fullThresholdPtr    - (pointer to)number of descriptors in queue before Reassembly
*       xoffThresholdPtr    - (pointer to)maximal number of entries in the queue
*                                         before IP Tv XON indication is asserted LOW.
*       xonThresholdPtr     - (pointer to)minimal number of entries in the queue
*                                         before IP Tv XON indication is asserted HIGH.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitMcTc2IpTvQueueConfigGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 fullThreshold;
    GT_U32 xoffThreshold;
    GT_U32 xonThreshold;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmDitMcTc2IpTvQueueConfigGet(devNum, &fullThreshold, &xoffThreshold, &xonThreshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", fullThreshold, xoffThreshold, xonThreshold);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitMcTc2IpTvQueueIpTvXonStatusGet
*
* DESCRIPTION:
*      Ip Tv Xon indicates queue fill status. When the number of entries in the
*      queue is more then configured in XOFF_THRESHOLD, then IP Tv XON indication
*      is asserted LOW (queue status is full). When the number of entries in the
*      queue is less or equal to XON_THRESHOLD, then IP Tv XON indication is
*      asserted HIGH (queue status is empty). For configuration of
*      XOFF_THRESHOLD/XON_THRESHOLD value refer to cpssExMxPmDitMcTc2IpTvQueueConfigSet.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum              - the device number
*
* OUTPUTS:
*       isQueueEmptyPtr     - (pointer to)Ip Tv Xon status
*                                         if GT_TRUE  - queue is empty
*                                         if GT_FALSE - queue is full
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitMcTc2IpTvQueueIpTvXonStatusGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL isQueueEmpty;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmDitMcTc2IpTvQueueIpTvXonStatusGet(devNum, &isQueueEmpty);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isQueueEmpty);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitMultiTargetTCQueueConfigSet
*
* DESCRIPTION:
*      Sets the multi-target TC queue assigned to multi-target Control,
*      analyzer and multicast RPF fail which command is assigned from DIT entry.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum             - the device number
*       analyzerMcQueue    - The multi-target TC queue for sniffed
*                            traffic. There are 4 possible queues (0..3)
*       mcRpfFailTcQueue   - the multi-target TC queue for rpf fail
*                            traffic. There are 4 possible queues (0..3)
*       controlMcTcQueue   - The multi-target TC queue for control
*                            traffic. There are 4 possible queues (0..3)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       A packet is considered as a multi-target control if it is a FROM_CPU DSA
*       Tagged with DSA<Use_Vidx> = 1 or a multi-target packet that is also to
*       be mirrored to the CPU.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitMultiTargetTCQueueConfigSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 analyzerMcQueue;
    GT_U32 mcRpfFailTcQueue;
    GT_U32 controlMcTcQueue;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    analyzerMcQueue = (GT_U32)inArgs[1];
    mcRpfFailTcQueue = (GT_U32)inArgs[2];
    controlMcTcQueue = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmDitMultiTargetTCQueueConfigSet(devNum, analyzerMcQueue, mcRpfFailTcQueue, controlMcTcQueue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitMultiTargetTCQueueConfigGet
*
* DESCRIPTION:
*      Gets the multi-target TC queue assigned to multi-target Control,
*      analyzer and multicast RPF fail which command is assigned from DIT entry.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum              - the device number
*
* OUTPUTS:
*       analyzerMcQueuePtr  - (pointer to)The multi-target TC queue for sniffed
*                             traffic. There are 4 possible queues (0..3)
*       mcRpfFailTcQueuePtr - (pointer to)The multi-target TC queue for rpf fail
*                             traffic. There are 4 possible queues (0..3)
*       controlMcTcQueuePtr - (pointer to)The multi-target TC queue for control
*                             traffic. There are 4 possible queues (0..3)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       A packet is considered as a multi-target control if it is a FROM_CPU DSA
*       Tagged with DSA<Use_Vidx> = 1 or a multi-target packet that is also to
*       be mirrored to the CPU.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitMultiTargetTCQueueConfigGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 analyzerMcQueue;
    GT_U32 mcRpfFailTcQueue;
    GT_U32 controlMcTcQueue;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmDitMultiTargetTCQueueConfigGet(devNum, &analyzerMcQueue, &mcRpfFailTcQueue, &controlMcTcQueue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", analyzerMcQueue, mcRpfFailTcQueue, controlMcTcQueue);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableSet
*
* DESCRIPTION:
*      Sets strict priority enable assigned to multi-target TC queues.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum         - the device number
*       tcQueue        - tc queue to configure. Values 0-3.
*       enable         - if GT_TRUE , Strict priority is enabled for this tc queue
*                        if GT_FALSE, Strict priority is disabled for this tc queue
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 tcQueue;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tcQueue = (GT_U32)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableSet(devNum, tcQueue, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet
*
* DESCRIPTION:
*      Gets strict priority enable assigned to multi-target TC queues.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum          - the device number
*       tcQueue         - tc queue to configure. Values 0-3.
*
*
* OUTPUTS:
*       enablePtr       - (pointer to)Strict priority assigned to this tc queue
*                         if GT_TRUE , Strict priority is enabled for this tc queue
*                         if GT_FALSE, Strict priority is disabled for this tc queue
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 tcQueue;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tcQueue = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet(devNum, tcQueue, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitMultiTargetTCQueueWeightSet
*
* DESCRIPTION:
*      Sets weight assigned to multi-target TC queues.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum       - the device number
*       tcQueue      - tc queue to configure. Values 0-3.
*       weight       - weight, in packets, of this tc queue. Values 0-255.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitMultiTargetTCQueueWeightSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 tcQueue;
    GT_U32 weight;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tcQueue = (GT_U32)inArgs[1];
    weight = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmDitMultiTargetTCQueueWeightSet(devNum, tcQueue, weight);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitMultiTargetTCQueueWeightGet
*
* DESCRIPTION:
*      Gets weight assigned to multi-target TC queues.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum       - the device number
*       tcQueue      - tc queue to configure. Values 0-3.
*
*
* OUTPUTS:
*       weightPtr    - (pointer to)weight, in packets, of this tc queue. Values 0-255.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitMultiTargetTCQueueWeightGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 tcQueue;
    GT_U32 weight;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tcQueue = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmDitMultiTargetTCQueueWeightGet(devNum, tcQueue, &weight);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", weight);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitRateShaperSet
*
* DESCRIPTION:
*      Sets the multi-target and access memory Rate shaper parameters.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - the device number
*       enable        - enable/disable the Multi-target Rate shaper
*                       if GT_TRUE  - enable Multi-target Rate shaper
*                       if GT_FALSE - disenable Multi-target Rate shaper
*       windowSize    - the shaper window size between two
*                       subsequent multi-target packets. Range 16 bits.
*       csuAccessRate - the shaper window size between two
*                       subsequent DIT (UC or MC) accesses to the CSU. Range 3 bits.
*       mcAccessRate  - both UC DIT and MC DIT can access the memory (internal
*                       or external). The shaper window size between two
*                       subsequent MC DIT. Range 7 bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitRateShaperSet
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
    GT_U32 windowSize;
    GT_U32 csuAccessRate;
    GT_U32 mcAccessRate;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];
    windowSize = (GT_U32)inArgs[2];
    csuAccessRate = (GT_U32)inArgs[3];
    mcAccessRate = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmDitRateShaperSet(devNum, enable, windowSize, csuAccessRate, mcAccessRate);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitRateShaperGet
*
* DESCRIPTION:
*      Gets the multi-target and access memory Rate shaper parameters.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum           -  the device number
*
* OUTPUTS:
*       enablePtr        -  (pointer to)enable/disable the Multi-target Rate shaper
*                           if GT_TRUE  - Multi-target Rate shaper enabled
*                           if GT_FALSE - Multi-target Rate shaper disabled
*       windowSizePtr    -  (pointer to)the shaper window size between two
*                           subsequent multi-target packets.
*                           Range 16 bits.
*       csuAccessRatePtr -  (pointer to)the shaper window size between two
*                           subsequent DIT (UC or MC) accesses to the CSU.
*                           Range 3 bits.
*       mcAccessRatePtr  -  (pointer to)the shaper window size between two
*                           subsequent MC DIT. Range 7 bits.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitRateShaperGet
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
    GT_U32 windowSize;
    GT_U32 csuAccessRate;
    GT_U32 mcAccessRate;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmDitRateShaperGet(devNum, &enable, &windowSize, &csuAccessRate, &mcAccessRate);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", enable, windowSize, csuAccessRate, mcAccessRate);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitMultiTargetUcSchedModeSet
*
* DESCRIPTION:
*      Sets the Multi target/unicast scheduler mode.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum                 - the device number
*       ucTableAccessStrictEnable - if GT_TRUE then enable strict priority to
*                                              UC DIT memory access over MC DIT
*       schedulerMtu           - The MTU used by the scheduler
*       ucStrictPriorityEnable - if GT_TRUE then Unicast uses Strict Priority and
*                                           Multi-target packets are scheduled
*                                           when Unicast packets are not pending.
*                                if GT_FALSE then both the Unicast and multi target
*                                            use SDWRR scheduling
*       ucWeight               - if SDWRR scheduler is used
*                                (ucStrictPriorityEnable == GT_FALSE) then this
*                                is the unicast weight in mtu units (0..255).
*       mcWeight               - if SDWRR scheduler is used
*                                (ucStrictPriorityEnable == GT_FALSE) then this
*                                is the multi target weight (0..255).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitMultiTargetUcSchedModeSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL ucTableAccessStrictEnable;
    CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_ENT schedulerMtu;
    GT_BOOL ucStrictPriorityEnable;
    GT_U32 ucWeight;
    GT_U32 mcWeight;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ucTableAccessStrictEnable = (GT_BOOL)inArgs[1];
    schedulerMtu = (CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_ENT)inArgs[2];
    ucStrictPriorityEnable = (GT_BOOL)inArgs[3];
    ucWeight = (GT_U32)inArgs[4];
    mcWeight = (GT_U32)inArgs[5];

    /* call cpss api function */
    result = cpssExMxPmDitMultiTargetUcSchedModeSet(devNum, ucTableAccessStrictEnable, schedulerMtu, ucStrictPriorityEnable, ucWeight, mcWeight);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitMultiTargetUcSchedModeGet
*
* DESCRIPTION:
*      Gets the Multi target/unicast scheduler mode.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum                  - the device number
*
* OUTPUTS:
*       ucTableAccessStrictEnablePtr
*                               - (pointer to)enable strict priority for
*                                 UC DIT memory access over MC DIT
*                                 if GT_TRUE - strict priority for UC DIT memory
*                                              access over MC DIT is enabled
*       schedulerMtuPtr         - (pointer to)the MTU used by the scheduler
*       ucStrictPriorityEnablePtr
*                               - (pointer to)enable strict priority for unicast
*                                 if GT_TRUE - unicast uses strict priority
*                                              and multi-target packets are
*                                              scheduled when unicast packets
*                                              are not pending.
*                                 if GT_FALSE - both the unicast and multi-target
*                                               use SDWRR scheduling
*       ucWeightPtr             - (pointer to)unicast weight in mtu units (0..255)
*                                 Relevant if SDWRR scheduler is used.
*       mcWeightPtr             - (pointer to)multi target weight (0..255).
*                                 Relevant if SDWRR scheduler is used.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitMultiTargetUcSchedModeGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL ucTableAccessStrictEnable;
    CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_ENT schedulerMtu;
    GT_BOOL ucStrictPriorityEnable;
    GT_U32 ucWeight;
    GT_U32 mcWeight;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmDitMultiTargetUcSchedModeGet(devNum, &ucTableAccessStrictEnable, &schedulerMtu, &ucStrictPriorityEnable, &ucWeight, &mcWeight);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d", ucTableAccessStrictEnable, schedulerMtu, ucStrictPriorityEnable, ucWeight, mcWeight);

    return CMD_OK;
}
#define SECOND_PAIR_IND 13
/*******************************************************************************
* cpssExMxPmDitIpMcPairWrite
*
* DESCRIPTION:
*       Writes a Downstream interface Table (DIT) Ip Mc pair entry to hw according
*       to the scheme :
*           1) First part only
*           2) Second part + next pointer only
*           3) Whole DIT pair.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       ditPairEntryIndex - The DIT Pair entry index.
*       ditPairWriteForm  - The way to write the DIT pair, first part only/
*                           second + next pointer only/ whole DIT pair.
*       ditPairEntryPtr   - The downstream interface pair entry
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This entry type is available only in internal memory while working with
*       IP MC Entry Dual Mode as defined in documentation.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitIpMcPairWrite
(
  IN  GT_32 inArgs[CMD_MAX_ARGS],
  IN  GT_32 inFields[CMD_MAX_FIELDS],
  IN  GT_32 numFields,
  OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ditPairEntryIndex;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_MODE_ENT ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_WHOLE_E;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_STC ditPairEntry;
    CPSS_INTERFACE_INFO_STC      * nextHopIfPtr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum =(GT_U8)inArgs[0];
    switch (inArgs[1])
    {
        case 2:
            ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E;
            break;
        case 3:
            ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_SECOND_DIT_NEXT_POINTER_ONLY_E;
            break;
        case 4:
            ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_WHOLE_E;
            break;
        default:
            galtisOutput(outArgs, GT_BAD_PARAM, "");
            return CMD_OK;
    }

    ditPairEntryIndex=(GT_U32)inFields[0];
    ditPairEntry.ditNextPointer = (GT_U32)inFields[1];

    /*firstDitNode*/
    ditPairEntry.firstDitNode.ditRpfFailCommand=(CPSS_PACKET_CMD_ENT)inFields[2];
    ditPairEntry.firstDitNode.isTunnelStart=(GT_BOOL)inFields[3];
    ditPairEntry.firstDitNode.nextHopTunnelPointer=(GT_U32)inFields[4];

    nextHopIfPtr=&ditPairEntry.firstDitNode.nextHopIf;

    nextHopIfPtr->type=(CPSS_INTERFACE_TYPE_ENT)inFields[5] ;

    switch (nextHopIfPtr->type)
    {
    case CPSS_INTERFACE_PORT_E:
        nextHopIfPtr->devPort.devNum = (GT_U8)inFields[6];
        nextHopIfPtr->devPort.portNum = (GT_U8)inFields[7];

        CONVERT_DEV_PORT_DATA_MAC(nextHopIfPtr->devPort.devNum ,nextHopIfPtr->devPort.portNum);

        break;

    case CPSS_INTERFACE_TRUNK_E:
        nextHopIfPtr->trunkId = (GT_TRUNK_ID)inFields[8];
        break;

    case CPSS_INTERFACE_VIDX_E:
        nextHopIfPtr->vidx = (GT_U16)inFields[9];
        break;

    default:
        break;
    }

    ditPairEntry.firstDitNode.nextHopVlanId= (GT_U16)inFields[10];
    ditPairEntry.firstDitNode.ttlHopLimitThreshold= (GT_U32)inFields[11];
    ditPairEntry.firstDitNode.excludeSourceVlanIf= (GT_BOOL)inFields[12];
    ditPairEntry.firstDitNode.isLast= (GT_BOOL)inFields[13];

    /*secondDitNode*/
    ditPairEntry.secondDitNode.ditRpfFailCommand=(CPSS_PACKET_CMD_ENT)inFields[SECOND_PAIR_IND+1];
    ditPairEntry.secondDitNode.isTunnelStart=(GT_BOOL)inFields[SECOND_PAIR_IND+2];
    ditPairEntry.secondDitNode.nextHopTunnelPointer=(GT_U32)inFields[SECOND_PAIR_IND+3];

    nextHopIfPtr=&ditPairEntry.secondDitNode.nextHopIf;

    nextHopIfPtr->type=(CPSS_INTERFACE_TYPE_ENT)inFields[SECOND_PAIR_IND+4] ;

    switch (nextHopIfPtr->type)
    {
    case CPSS_INTERFACE_PORT_E:
        nextHopIfPtr->devPort.devNum = (GT_U8)inFields[SECOND_PAIR_IND+5];
        nextHopIfPtr->devPort.portNum = (GT_U8)inFields[SECOND_PAIR_IND+6];

        CONVERT_DEV_PORT_DATA_MAC(nextHopIfPtr->devPort.devNum ,nextHopIfPtr->devPort.portNum);

        break;

    case CPSS_INTERFACE_TRUNK_E:
        nextHopIfPtr->trunkId = (GT_TRUNK_ID)inFields[SECOND_PAIR_IND+7];
        break;

    case CPSS_INTERFACE_VIDX_E:
        nextHopIfPtr->vidx = (GT_U16)inFields[SECOND_PAIR_IND+8];
        break;

    default:
        break;
    }

    ditPairEntry.secondDitNode.nextHopVlanId= (GT_U16)inFields[SECOND_PAIR_IND+9];
    ditPairEntry.secondDitNode.ttlHopLimitThreshold= (GT_U32)inFields[SECOND_PAIR_IND+10];
    ditPairEntry.secondDitNode.excludeSourceVlanIf= (GT_BOOL)inFields[SECOND_PAIR_IND+11];
    ditPairEntry.secondDitNode.isLast= (GT_BOOL)inFields[SECOND_PAIR_IND+12];

    /* call cpss api function */
    result = cpssExMxPmDitIpMcPairWrite(devNum, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
static GT_U32 minDitMcIndex;
static GT_U32 maxDitMcIndex;
static GT_U32 ditMcIndex;
/*******************************************************************************
* cpssExMxPmDitIpMcPairRead
*
* DESCRIPTION:
*       Reads a Downstream Interface Table Ip Mc pair entry from the hw.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       ditPairEntryIndex - The DIT Pair entry index.
*
* OUTPUTS:
*       ditPairEntryPtr   - The downstream interface pair entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This entry type is available only in internal memory while working with
*       IP MC Entry Dual Mode as defined in documentation.
*
*******************************************************************************/
static CMD_STATUS prvWrDitIpMcPairEntryGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_STC ditPairEntry;
    CPSS_INTERFACE_INFO_STC      * nextHopIfPtr;
    GT_U8 tempDevNum;
    GT_U8 tempPortNum;

    CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_MODE_ENT ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_WHOLE_E;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum =(GT_U8)inArgs[0];
    switch (inArgs[1])
    {
        case 2:
            ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E;
            break;
        case 3:
            ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_SECOND_DIT_NEXT_POINTER_ONLY_E;
            break;
        case 4:
            ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_WHOLE_E;
            break;
        default:
            galtisOutput(outArgs, GT_BAD_PARAM, "");
            return CMD_OK;
    }

    if (ditMcIndex>maxDitMcIndex)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

     /* call cpss api function */
    result = cpssExMxPmDitIpMcPairRead(devNum, ditMcIndex, ditPairWriteForm, &ditPairEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }


    cpssOsMemSet(inFields,0, CMD_MAX_FIELDS);

    inFields[0]=ditMcIndex;
    inFields[1] = ditPairEntry.ditNextPointer;

    /*firstDitNode*/
    inFields[2]=ditPairEntry.firstDitNode.ditRpfFailCommand;
    inFields[3]=ditPairEntry.firstDitNode.isTunnelStart;
    inFields[4]=ditPairEntry.firstDitNode.nextHopTunnelPointer;

    nextHopIfPtr=&ditPairEntry.firstDitNode.nextHopIf;

    inFields[5]=nextHopIfPtr->type;

    switch (nextHopIfPtr->type)
    {
    case CPSS_INTERFACE_PORT_E:
        tempDevNum=nextHopIfPtr->devPort.devNum;
        tempPortNum=nextHopIfPtr->devPort.portNum;

        CONVERT_BACK_DEV_PORT_DATA_MAC(tempDevNum,tempPortNum)

        inFields[6]=tempDevNum;
        inFields[7]=tempPortNum;
        break;

    case CPSS_INTERFACE_TRUNK_E:
        inFields[8]=nextHopIfPtr->trunkId;
        break;

    case CPSS_INTERFACE_VIDX_E:
        inFields[9]=nextHopIfPtr->vidx;
        break;



    default:
        break;
    }

    inFields[10]=ditPairEntry.firstDitNode.nextHopVlanId;
    inFields[11]=ditPairEntry.firstDitNode.ttlHopLimitThreshold;
    inFields[12]=ditPairEntry.firstDitNode.excludeSourceVlanIf;
    inFields[13]=ditPairEntry.firstDitNode.isLast ;

    /*secondDitNode*/
    inFields[SECOND_PAIR_IND+1]=ditPairEntry.secondDitNode.ditRpfFailCommand;
    inFields[SECOND_PAIR_IND+2]=ditPairEntry.secondDitNode.isTunnelStart;
    inFields[SECOND_PAIR_IND+3]=ditPairEntry.secondDitNode.nextHopTunnelPointer;

    nextHopIfPtr=&ditPairEntry.secondDitNode.nextHopIf;

    inFields[SECOND_PAIR_IND+4]=nextHopIfPtr->type;

    switch (nextHopIfPtr->type)
    {
    case CPSS_INTERFACE_PORT_E:

        tempDevNum=nextHopIfPtr->devPort.devNum;
        tempPortNum=nextHopIfPtr->devPort.portNum;

        CONVERT_BACK_DEV_PORT_DATA_MAC(tempDevNum,tempPortNum)

        inFields[SECOND_PAIR_IND+5]=tempDevNum;
        inFields[SECOND_PAIR_IND+6]=tempPortNum;

        break;

    case CPSS_INTERFACE_TRUNK_E:
        inFields[SECOND_PAIR_IND+7]=nextHopIfPtr->trunkId;
        break;

    case CPSS_INTERFACE_VIDX_E:
        inFields[SECOND_PAIR_IND+8]=nextHopIfPtr->vidx;
        break;



    default:
        break;
    }

    inFields[SECOND_PAIR_IND+9]=ditPairEntry.secondDitNode.nextHopVlanId;
    inFields[SECOND_PAIR_IND+10]=ditPairEntry.secondDitNode.ttlHopLimitThreshold;
    inFields[SECOND_PAIR_IND+11]=ditPairEntry.secondDitNode.excludeSourceVlanIf;
    inFields[SECOND_PAIR_IND+12]=ditPairEntry.secondDitNode.isLast ;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],  inFields[6],
                inFields[7],  inFields[8],  inFields[9],  inFields[10],
                inFields[11], inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17], inFields[18],
                inFields[19], inFields[20], inFields[21], inFields[22],
                inFields[23], inFields[24],inFields[25]);



    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;


}
/********************************************************************/

static CMD_STATUS wrDitIpMcPairEntryGetFirst
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

  /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    minDitMcIndex=(GT_U32)inArgs[2];
    maxDitMcIndex=(GT_U32)inArgs[3];
    ditMcIndex=minDitMcIndex;

    return prvWrDitIpMcPairEntryGet(inArgs,inFields,numFields,outArgs);
}
/********************************************************************/
static CMD_STATUS wrDitIpMcPairEntryGetNext
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    ditMcIndex++;

    return prvWrDitIpMcPairEntryGet(inArgs,inFields,numFields,outArgs);

}
/*******************************************************************************
* cpssExMxPmDitIpMplsEntryWrite
*
* DESCRIPTION:
*       Writes a Downstream interface Table (DIT) Ip/Mpls entry to hw.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number
*       ditEntryIndex   - The DIT Ip/Mpls entry index.
*       ditIpMpls       - dit entry type selection (Ip/Mpls)
*       ditType         - packet type selection (unicast/multicast)
*       ditEntryPtr     - The downstream interface entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitIpMplsEntryWrite
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    GT_U32                                  ditEntryIndex;
    CPSS_EXMXPM_DIT_IP_MPLS_SELECTION_ENT   ditIpMpls;
    CPSS_UNICAST_MULTICAST_ENT              ditType;
    CPSS_EXMXPM_DIT_IP_MPLS_STC             ditEntry;
    CPSS_INTERFACE_INFO_STC               * outLifPtr;

   /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum=(GT_U8)inArgs[0];
    ditIpMpls=(CPSS_EXMXPM_DIT_IP_MPLS_SELECTION_ENT)inArgs[1];
    ditType=(CPSS_UNICAST_MULTICAST_ENT)inArgs[2];

    ditEntryIndex=(GT_U32)inFields[0];

    ditEntry.ditRpfFailCommand=(CPSS_PACKET_CMD_ENT)inFields[1];

    outLifPtr=&ditEntry.outLif;

    outLifPtr->type=(CPSS_INTERFACE_TYPE_ENT)inFields[2] ;

    switch (outLifPtr->type)
    {
    case CPSS_INTERFACE_PORT_E:
        outLifPtr->devPort.devNum = (GT_U8)inFields[3];
        outLifPtr->devPort.portNum = (GT_U8)inFields[4];

        CONVERT_DEV_PORT_DATA_MAC(outLifPtr->devPort.devNum ,outLifPtr->devPort.portNum);

        break;

    case CPSS_INTERFACE_TRUNK_E:
        outLifPtr->trunkId = (GT_TRUNK_ID)inFields[5];
        break;

    case CPSS_INTERFACE_VIDX_E:
        outLifPtr->vidx = (GT_U16)inFields[6];
        break;

    default:
        break;
    }
    ditEntry.isTunnelStart=(GT_BOOL)inFields[7];
    ditEntry.tunnelStartPtr=(GT_U32)inFields[8];
    ditEntry.tunnelStartPassengerPacketType=(CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[9];
    ditEntry.arpPointer=(GT_U32)inFields[10];
    ditEntry.vlanId=(GT_U16)inFields[11];
    ditEntry.ttlHopLimitThreshold=(GT_U32)inFields[12];
    ditEntry.excludeSourceVlanIf=(GT_BOOL)inFields[13];
    ditEntry.mplsLabel=(GT_U32)inFields[14];
    ditEntry.exp=(GT_U32)inFields[15];
    ditEntry.qosExpCommand=(CPSS_EXMXPM_DIT_QOS_COMMAND_TYPE_ENT)inFields[16];
    ditEntry.mplsCommand=(GT_U32)inFields[17];
    ditEntry.ttlIndex=(GT_U32)inFields[18];
    ditEntry.isVidSet=(GT_BOOL)inFields[19];

    ditEntry.srcInterface.type=(CPSS_INTERFACE_TYPE_ENT)inFields[20];
    ditEntry.srcInterface.devPort.devNum=(GT_U8)inFields[21];
    ditEntry.srcInterface.devPort.portNum=(GT_U8)inFields[22];

    CONVERT_DEV_PORT_DATA_MAC(ditEntry.srcInterface.devPort.devNum,
                         ditEntry.srcInterface.devPort.portNum);

    ditEntry.srcInterface.trunkId=(GT_TRUNK_ID)inFields[23];
    ditEntry.srcInterface.vidx=(GT_U16)inFields[24];
    ditEntry.srcInterface.vlanId=(GT_U16)inFields[25];
    ditEntry.srcInterface.devNum=(GT_U8)inFields[26];
    ditEntry.srcInterface.fabricVidx=(GT_U16)inFields[27];


    ditEntry.srcInterfaceCompareMode=(CPSS_EXMXPM_DIT_SOURCE_INTERFACE_COMPARE_MODE_ENT)inFields[28];
    ditEntry.nextPointer=(GT_U32)inFields[29];
    ditEntry.isLast=(GT_BOOL)inFields[30];

    /* call cpss api function */
    result = cpssExMxPmDitIpMplsEntryWrite(devNum,ditEntryIndex,ditIpMpls,ditType,&ditEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

static GT_U32 minDitIpMplsIndex;
static GT_U32 maxDitIpMplsIndex;
static GT_U32 ditIpMplsIndex;
/*******************************************************************************
* cpssExMxPmDitIpMplsEntryRead
*
* DESCRIPTION:
*       Reads a Downstream interface Table (DIT) Ip/Mpls entry from the hw.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       ditEntryIndex - The DIT Ip/Mpls entry index.
*       ditIpMpls     - dit entry type selection (Ip/Mpls)
*       ditType       - packet type selection (unicast/multicast)
*
* OUTPUTS:
*       ditEntryPtr   - The downstream interface entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvWrCpssExMxPmDitIpMplsEntryRead
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_EXMXPM_DIT_IP_MPLS_SELECTION_ENT   ditIpMpls;
    CPSS_UNICAST_MULTICAST_ENT              ditType;
    CPSS_EXMXPM_DIT_IP_MPLS_STC             ditEntry;
    CPSS_INTERFACE_INFO_STC               * outLifPtr;
    GT_U8                                   tempDevNum;
    GT_U8                                   tempPortNum;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum =(GT_U8)inArgs[0];
    ditIpMpls=(CPSS_EXMXPM_DIT_IP_MPLS_SELECTION_ENT)inArgs[1];
    ditType=(CPSS_UNICAST_MULTICAST_ENT)inArgs[2];
    cpssOsMemSet(&ditEntry,0,sizeof(CPSS_EXMXPM_DIT_IP_MPLS_STC));
    if (ditIpMplsIndex>maxDitIpMplsIndex)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

     /* call cpss api function */
    result = cpssExMxPmDitIpMplsEntryRead(devNum, ditIpMplsIndex,ditIpMpls,ditType,&ditEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    cpssOsMemSet(inFields,0, CMD_MAX_FIELDS);

    inFields[0]=ditIpMplsIndex;
    inFields[1]=ditEntry.ditRpfFailCommand;

    outLifPtr=&ditEntry.outLif;

    inFields[2]=outLifPtr->type ;

    switch (outLifPtr->type)
    {
    case CPSS_INTERFACE_PORT_E:
        tempDevNum=outLifPtr->devPort.devNum;
        tempPortNum=outLifPtr->devPort.portNum;

        CONVERT_BACK_DEV_PORT_DATA_MAC(tempDevNum,tempPortNum)

        inFields[3]=tempDevNum;
        inFields[4]=tempPortNum;
      break;

    case CPSS_INTERFACE_TRUNK_E:
        inFields[5]=outLifPtr->trunkId;
        break;

    case CPSS_INTERFACE_VIDX_E:
        inFields[6]=outLifPtr->vidx;
        break;

    default:
        break;
    }
    inFields[7]=ditEntry.isTunnelStart;
    inFields[8]=ditEntry.tunnelStartPtr;
    inFields[9]=ditEntry.tunnelStartPassengerPacketType;
    inFields[10]=ditEntry.arpPointer;
    inFields[11]=ditEntry.vlanId;
    inFields[12]=ditEntry.ttlHopLimitThreshold;
    inFields[13]=ditEntry.excludeSourceVlanIf;
    inFields[14]=ditEntry.mplsLabel;
    inFields[15]=ditEntry.exp;
    inFields[16]=ditEntry.qosExpCommand;
    inFields[17]=ditEntry.mplsCommand;
    inFields[18]=ditEntry.ttlIndex;
    inFields[19]=ditEntry.isVidSet;
    inFields[20]=ditEntry.srcInterface.type;

    tempDevNum=ditEntry.srcInterface.devPort.devNum;
    tempPortNum=ditEntry.srcInterface.devPort.portNum;

    CONVERT_BACK_DEV_PORT_DATA_MAC(tempDevNum,tempPortNum)

    inFields[21]=tempDevNum;
    inFields[22]=tempPortNum;


    inFields[23]=ditEntry.srcInterface.trunkId;
    inFields[24]=ditEntry.srcInterface.vidx;
    inFields[25]=ditEntry.srcInterface.vlanId;
    inFields[26]=ditEntry.srcInterface.devNum;
    inFields[27]=ditEntry.srcInterface.fabricVidx;


    inFields[28]=ditEntry.srcInterfaceCompareMode;
    inFields[29]=ditEntry.nextPointer;
    inFields[30]=ditEntry.isLast;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],  inFields[6],
                inFields[7],  inFields[8],  inFields[9],  inFields[10],
                inFields[11], inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17], inFields[18],
                inFields[19], inFields[20], inFields[21], inFields[22],
                inFields[23], inFields[24],inFields[25], inFields[26],
                inFields[27], inFields[28],inFields[29], inFields[30]);



    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;


}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitIpMplsEntryReadFirst
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    minDitIpMplsIndex=(GT_U32)inArgs[3];
    maxDitIpMplsIndex=(GT_U32)inArgs[4];
    ditIpMplsIndex=minDitIpMplsIndex;

    return prvWrCpssExMxPmDitIpMplsEntryRead(inArgs,inFields,numFields,outArgs);
}
/********************************************************************/
static CMD_STATUS wrCpssExMxPmDitIpMplsEntryReadNext
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
   /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    ditIpMplsIndex++;

    return prvWrCpssExMxPmDitIpMplsEntryRead(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* cpssExMxPmDitVplsEntryWrite
*
* DESCRIPTION:
*       Writes a Downstream interface Table (DIT) Vpls entry to hw.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number
*       ditEntryIndex   - The DIT Vpls entry index.
*       ditType         - dit packet type selection (unicast/multicast)
*       ditEntryPtr     - The downstream interface entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitVplsEntryWrite
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                               result;
    GT_U8                                   devNum;
    GT_U32                                  ditEntryIndex;
    CPSS_UNICAST_MULTICAST_ENT              ditType;
    CPSS_EXMXPM_DIT_VPLS_STC                ditEntry;
    CPSS_INTERFACE_INFO_STC               * outLifPtr;

   /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum=(GT_U8)inArgs[0];
    ditType=(CPSS_UNICAST_MULTICAST_ENT)inArgs[1];

    ditEntryIndex=(GT_U32)inFields[0];

    cpssOsMemSet(&ditEntry,0, sizeof(CPSS_EXMXPM_DIT_VPLS_STC));

    outLifPtr=&ditEntry.outLif;

    outLifPtr->type=(CPSS_INTERFACE_TYPE_ENT)inFields[1] ;

    switch (outLifPtr->type)
    {
    case CPSS_INTERFACE_PORT_E:
        outLifPtr->devPort.devNum = (GT_U8)inFields[2];
        outLifPtr->devPort.portNum = (GT_U8)inFields[3];

        CONVERT_DEV_PORT_DATA_MAC(outLifPtr->devPort.devNum ,outLifPtr->devPort.portNum);

        break;

    case CPSS_INTERFACE_TRUNK_E:
        outLifPtr->trunkId = (GT_TRUNK_ID)inFields[4];
        break;

    case CPSS_INTERFACE_VIDX_E:
        outLifPtr->vidx = (GT_U16)inFields[5];
        break;

    default:
        break;
    }
    ditEntry.isTunnelStart=(GT_BOOL)inFields[6];
    ditEntry.tunnelStartPtr=(GT_U32)inFields[7];
    ditEntry.tunnelStartPassengerPacketType=(CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[8];
    ditEntry.arpPointer=(GT_U32)inFields[9];
    ditEntry.vlanId=(GT_U16)inFields[10];
    ditEntry.ttl=(GT_U32)inFields[11];
    ditEntry.vlanTagCommand=(CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_ENT)inFields[12];
    ditEntry.label=(GT_U32)inFields[13];
    ditEntry.exp=(GT_U32)inFields[14];
    ditEntry.qosExpCommand=(CPSS_EXMXPM_DIT_QOS_COMMAND_TYPE_ENT)inFields[15];
    ditEntry.up=(GT_U32)inFields[16];
    ditEntry.qosUpCommand=(CPSS_EXMXPM_DIT_QOS_COMMAND_TYPE_ENT)inFields[17];
    ditEntry.mplsCommand=(GT_U32)inFields[18];
    ditEntry.isProviderEdge=(GT_BOOL)inFields[19];
    ditEntry.vplsUnregisteredBcEnable=(GT_BOOL)inFields[20];
    ditEntry.vplsUnknownUcEnable=(GT_BOOL)inFields[21];
    ditEntry.vplsUnregisteredMcEnable=(GT_BOOL)inFields[22];
    ditEntry.pwId=(GT_U32)inFields[23];
    ditEntry.nextPointer=(GT_U32)inFields[24];
    ditEntry.isLast=(GT_BOOL)inFields[25];

    /* call cpss api function */
    result = cpssExMxPmDitVplsEntryWrite(devNum, ditEntryIndex, ditType, &ditEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
static GT_U32 minDitVplsIndex;
static GT_U32 maxDitVplsIndex;
static GT_U32 ditVplsIndex;
/*******************************************************************************
* cpssExMxPmDitVplsEntryRead
*
* DESCRIPTION:
*       Reads a Downstream interface Table (DIT) Vpls entry from the hw.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       ditEntryIndex - The DIT Vpls entry index.
*       ditType       - dit packet type selection (unicast/multicast)
*
* OUTPUTS:
*       ditEntryPtr   - The downstream interface entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS prvWrCpssExMxPmDitVplsEntryRead
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_UNICAST_MULTICAST_ENT              ditType;
    CPSS_EXMXPM_DIT_VPLS_STC                ditEntry;
    CPSS_INTERFACE_INFO_STC               * outLifPtr;
    GT_U8                                   tempDevNum;
    GT_U8                                   tempPortNum;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum =(GT_U8)inArgs[0];
    ditType=(CPSS_UNICAST_MULTICAST_ENT)inArgs[1];

    cpssOsMemSet(&ditEntry,0, sizeof(CPSS_EXMXPM_DIT_VPLS_STC));

    if (ditVplsIndex>maxDitVplsIndex)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

     /* call cpss api function */
    result = cpssExMxPmDitVplsEntryRead(devNum, ditVplsIndex,ditType,&ditEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    cpssOsMemSet(inFields,0, CMD_MAX_FIELDS);

    inFields[0]=ditVplsIndex;

    outLifPtr=&ditEntry.outLif;

    inFields[1]=outLifPtr->type ;

    switch (outLifPtr->type)
    {
    case CPSS_INTERFACE_PORT_E:
        inFields[2]=outLifPtr->devPort.devNum ;
        inFields[3]=outLifPtr->devPort.portNum;

        tempDevNum=outLifPtr->devPort.devNum;
        tempPortNum=outLifPtr->devPort.portNum;

        CONVERT_BACK_DEV_PORT_DATA_MAC(tempDevNum,tempPortNum)

        inFields[2]=tempDevNum;
        inFields[3]=tempPortNum;

        break;

    case CPSS_INTERFACE_TRUNK_E:
        inFields[4]=outLifPtr->trunkId;
        break;

    case CPSS_INTERFACE_VIDX_E:
        inFields[5]=outLifPtr->vidx;
        break;

    default:
        break;
    }
    inFields[6]=ditEntry.isTunnelStart;
    inFields[7]=ditEntry.tunnelStartPtr;
    inFields[8]=ditEntry.tunnelStartPassengerPacketType;
    inFields[9]=ditEntry.arpPointer;
    inFields[10]=ditEntry.vlanId;
    inFields[11]=ditEntry.ttl;
    inFields[12]=ditEntry.vlanTagCommand;
    inFields[13]=ditEntry.label;
    inFields[14]=ditEntry.exp;
    inFields[15]=ditEntry.qosExpCommand;
    inFields[16]=ditEntry.up;
    inFields[17]=ditEntry.qosUpCommand;
    inFields[18]=ditEntry.mplsCommand;
    inFields[19]=ditEntry.isProviderEdge;
    inFields[20]=ditEntry.vplsUnregisteredBcEnable;
    inFields[21]=ditEntry.vplsUnknownUcEnable;
    inFields[22]=ditEntry.vplsUnregisteredMcEnable;
    inFields[23]=ditEntry.pwId;
    inFields[24]=ditEntry.nextPointer;
    inFields[25]=ditEntry.isLast;



    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],  inFields[6],
                inFields[7],  inFields[8],  inFields[9],  inFields[10],
                inFields[11], inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17], inFields[18],
                inFields[19], inFields[20], inFields[21], inFields[22],
                inFields[23], inFields[24],inFields[25]);



    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;


}
/*************************************************************************************/
static CMD_STATUS wrCpssExMxPmDitVplsEntryReadFirst
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
   /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    minDitVplsIndex=(GT_U32)inArgs[2];
    maxDitVplsIndex=(GT_U32)inArgs[3];
    ditVplsIndex=minDitVplsIndex;

    return prvWrCpssExMxPmDitVplsEntryRead(inArgs,inFields,numFields,outArgs);
}
/*****************************************************************************/
static CMD_STATUS wrCpssExMxPmDitVplsEntryReadNext
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    ditVplsIndex++;

    return prvWrCpssExMxPmDitVplsEntryRead(inArgs,inFields,numFields,outArgs);

}
/*******************************************************************************
* cpssExMxPmDitVplsEntryWrite
*
* DESCRIPTION:
*       Writes a Downstream interface Table (DIT) Vpls entry to hw.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number
*       ditEntryIndex   - The DIT Vpls entry index.
*       ditType         - dit packet type selection (unicast/multicast)
*       ditEntryPtr     - The downstream interface entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitVplsEntryWrite_1
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                               result;
    GT_U8                                   devNum;
    GT_U32                                  ditEntryIndex;
    CPSS_UNICAST_MULTICAST_ENT              ditType;
    CPSS_EXMXPM_DIT_VPLS_STC                ditEntry;
    CPSS_INTERFACE_INFO_STC               * outLifPtr;

   /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum=(GT_U8)inArgs[0];
    ditType=(CPSS_UNICAST_MULTICAST_ENT)inArgs[1];

    ditEntryIndex=(GT_U32)inFields[0];



    outLifPtr=&ditEntry.outLif;

    outLifPtr->type=(CPSS_INTERFACE_TYPE_ENT)inFields[1] ;

    switch (outLifPtr->type)
    {
    case CPSS_INTERFACE_PORT_E:
        outLifPtr->devPort.devNum = (GT_U8)inFields[2];
        outLifPtr->devPort.portNum = (GT_U8)inFields[3];

        CONVERT_DEV_PORT_DATA_MAC(outLifPtr->devPort.devNum ,outLifPtr->devPort.portNum);

        break;

    case CPSS_INTERFACE_TRUNK_E:
        outLifPtr->trunkId = (GT_TRUNK_ID)inFields[4];
        break;

    case CPSS_INTERFACE_VIDX_E:
        outLifPtr->vidx = (GT_U16)inFields[5];
        break;

    default:
        break;
    }
    ditEntry.isTunnelStart=(GT_BOOL)inFields[6];
    ditEntry.tunnelStartPtr=(GT_U32)inFields[7];
    ditEntry.tunnelStartPassengerPacketType=(CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[8];
    ditEntry.arpPointer=(GT_U32)inFields[9];
    ditEntry.vlanId=(GT_U16)inFields[10];
    ditEntry.ttl=(GT_U32)inFields[11];
    ditEntry.vlanTagCommand=(CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_ENT)inFields[12];
    ditEntry.label=(GT_U32)inFields[13];
    ditEntry.exp=(GT_U32)inFields[14];
    ditEntry.qosExpCommand=(CPSS_EXMXPM_DIT_QOS_COMMAND_TYPE_ENT)inFields[15];
    ditEntry.up=(GT_U32)inFields[16];
    ditEntry.qosUpCommand=(CPSS_EXMXPM_DIT_QOS_COMMAND_TYPE_ENT)inFields[17];
    ditEntry.mplsCommand=(GT_U32)inFields[18];
    ditEntry.isProviderEdge=(GT_BOOL)inFields[19];
    ditEntry.vplsUnregisteredBcEnable=(GT_BOOL)inFields[20];
    ditEntry.vplsUnknownUcEnable=(GT_BOOL)inFields[21];
    ditEntry.vplsUnregisteredMcEnable=(GT_BOOL)inFields[22];
    ditEntry.pwId=(GT_U32)inFields[23];
    ditEntry.nextPointer=(GT_U32)inFields[24];
    ditEntry.isLast=(GT_BOOL)inFields[25];
    ditEntry.vllEgressFilteringEnable = (GT_BOOL)inFields[26];

    /* call cpss api function */
    result = cpssExMxPmDitVplsEntryWrite(devNum, ditEntryIndex, ditType, &ditEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
static GT_U32 minDitVplsIndex_1;
static GT_U32 maxDitVplsIndex_1;
static GT_U32 ditVplsIndex_1;
/*******************************************************************************
* cpssExMxPmDitVplsEntryRead
*
* DESCRIPTION:
*       Reads a Downstream interface Table (DIT) Vpls entry from the hw.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       ditEntryIndex - The DIT Vpls entry index.
*       ditType       - dit packet type selection (unicast/multicast)
*
* OUTPUTS:
*       ditEntryPtr   - The downstream interface entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS prvWrCpssExMxPmDitVplsEntryRead_1
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_UNICAST_MULTICAST_ENT              ditType;
    CPSS_EXMXPM_DIT_VPLS_STC                ditEntry;
    CPSS_INTERFACE_INFO_STC               * outLifPtr;
    GT_U8                                   tempDevNum;
    GT_U8                                   tempPortNum;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum =(GT_U8)inArgs[0];
    ditType=(CPSS_UNICAST_MULTICAST_ENT)inArgs[1];

    if (ditVplsIndex_1>maxDitVplsIndex_1)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

     /* call cpss api function */
    result = cpssExMxPmDitVplsEntryRead(devNum, ditVplsIndex_1,ditType,&ditEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    cpssOsMemSet(inFields,0, CMD_MAX_FIELDS);

    inFields[0]=ditVplsIndex_1;

    outLifPtr=&ditEntry.outLif;

    inFields[1]=outLifPtr->type ;

    switch (outLifPtr->type)
    {
    case CPSS_INTERFACE_PORT_E:
        inFields[2]=outLifPtr->devPort.devNum ;
        inFields[3]=outLifPtr->devPort.portNum;

        tempDevNum=outLifPtr->devPort.devNum;
        tempPortNum=outLifPtr->devPort.portNum;

        CONVERT_BACK_DEV_PORT_DATA_MAC(tempDevNum,tempPortNum)

        inFields[2]=tempDevNum;
        inFields[3]=tempPortNum;

        break;

    case CPSS_INTERFACE_TRUNK_E:
        inFields[4]=outLifPtr->trunkId;
        break;

    case CPSS_INTERFACE_VIDX_E:
        inFields[5]=outLifPtr->vidx;
        break;

    default:
        break;
    }
    inFields[6]=ditEntry.isTunnelStart;
    inFields[7]=ditEntry.tunnelStartPtr;
    inFields[8]=ditEntry.tunnelStartPassengerPacketType;
    inFields[9]=ditEntry.arpPointer;
    inFields[10]=ditEntry.vlanId;
    inFields[11]=ditEntry.ttl;
    inFields[12]=ditEntry.vlanTagCommand;
    inFields[13]=ditEntry.label;
    inFields[14]=ditEntry.exp;
    inFields[15]=ditEntry.qosExpCommand;
    inFields[16]=ditEntry.up;
    inFields[17]=ditEntry.qosUpCommand;
    inFields[18]=ditEntry.mplsCommand;
    inFields[19]=ditEntry.isProviderEdge;
    inFields[20]=ditEntry.vplsUnregisteredBcEnable;
    inFields[21]=ditEntry.vplsUnknownUcEnable;
    inFields[22]=ditEntry.vplsUnregisteredMcEnable;
    inFields[23]=ditEntry.pwId;
    inFields[24]=ditEntry.nextPointer;
    inFields[25]=ditEntry.isLast;
    inFields[26]=ditEntry.vllEgressFilteringEnable;



    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],  inFields[6],
                inFields[7],  inFields[8],  inFields[9],  inFields[10],
                inFields[11], inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17], inFields[18],
                inFields[19], inFields[20], inFields[21], inFields[22],
                inFields[23], inFields[24],inFields[25],inFields[26]);



    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;


}
/*************************************************************************************/
static CMD_STATUS wrCpssExMxPmDitVplsEntryReadFirst_1
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
   /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    minDitVplsIndex_1=(GT_U32)inArgs[2];
    maxDitVplsIndex_1=(GT_U32)inArgs[3];
    ditVplsIndex_1=minDitVplsIndex_1;

    return prvWrCpssExMxPmDitVplsEntryRead_1(inArgs,inFields,numFields,outArgs);
}
/*****************************************************************************/
static CMD_STATUS wrCpssExMxPmDitVplsEntryReadNext_1
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    ditVplsIndex_1++;

    return prvWrCpssExMxPmDitVplsEntryRead_1(inArgs,inFields,numFields,outArgs);

}

/*******************************************************************************
* cpssExMxPmDitVpwsTargetTaggedModeSet
*
* DESCRIPTION:
*       Controls the default target tagged mode for VPWS packets.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum                - device number
*       vpwsTargetTaggedMode  - VPWS target tagged mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitVpwsTargetTaggedModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_MODE_ENT vpwsTargetTaggedMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vpwsTargetTaggedMode = (CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmDitVpwsTargetTaggedModeSet(devNum, vpwsTargetTaggedMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitVpwsTargetTaggedModeGet
*
* DESCRIPTION:
*       Get the default target tagged mode for VPWS packets.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum                   - device number
*
* OUTPUTS:
*       vpwsTargetTaggedModePtr  - points to VPWS target tagged mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitVpwsTargetTaggedModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_MODE_ENT vpwsTargetTaggedMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmDitVpwsTargetTaggedModeGet(devNum, &vpwsTargetTaggedMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vpwsTargetTaggedMode);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmDitVpwsTargetTaggedControlSet
*
* DESCRIPTION:
*       Controls the tag state of the outgoing VPWS packets.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum                   - device number
*       vpwsTargetTaggedControl  - VPWS target tag state
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitVpwsTargetTaggedControlSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_ENT vpwsTargetTaggedControl;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vpwsTargetTaggedControl = (CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmDitVpwsTargetTaggedControlSet(devNum, vpwsTargetTaggedControl);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitVpwsTargetTaggedControlGet
*
* DESCRIPTION:
*       Get the tag state of the outgoing VPWS packets.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       vpwsTargetTaggedControlPtr  - points to VPWS target tag state
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_STATE             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitVpwsTargetTaggedControlGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_ENT vpwsTargetTaggedControl;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmDitVpwsTargetTaggedControlGet(devNum, &vpwsTargetTaggedControl);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vpwsTargetTaggedControl);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitVplsUnknownUnicastFloodEnableSet
*
* DESCRIPTION:
*       This function enables/disables the VPLS flooding using DIT entry.
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - Boolean value:
*                         GT_TRUE  - Unknown VPLS unicast packets are flooded to DIT using the
*                                   <Flood DIT Pointer> field in the TTI Action Entry.
*                         GT_FALSE - Unknown unicast packets are always flooded in the VLAN.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - if the driver was not initialized
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitVplsUnknownUnicastFloodEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmDitVplsUnknownUnicastFloodEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmDitVplsUnknownUnicastFloodEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of VPLS
*       flooding using DIT entry.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable VPLS flooding using DIT entry
*                       GT_TRUE: VPLS flooding using DIT entry enabled
*                       GT_FALSE: VPLS flooding using DIT entry disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - if the driver was not initialized
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmDitVplsUnknownUnicastFloodEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmDitVplsUnknownUnicastFloodEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{



    {"cpssExMxPm2DitIpMcPairLastSet",
        &wrCpssExMxPmDitIpMcPairLastSet,
        4, 0},

    {"cpssExMxPm2DitIpMplsEntryLastSet",
        &wrCpssExMxPmDitIpMplsEntryLastSet,
        5, 0},
    {"cpssExMxPm2DitTtlTableEntrySet",
        &wrCpssExMxPmDitTtlTableEntrySet,
        3, 0},
    {"cpssExMxPm2DitTtlTableEntryGet",
        &wrCpssExMxPmDitTtlTableEntryGet,
        2, 0},

    {"cpssExMxPm2DitVplsEntryLastSet",
        &wrCpssExMxPmDitVplsEntryLastSet,
        4, 0},
    {"cpssExMxPm2DitCounterGet",
        &wrCpssExMxPmDitCounterGet,
        3, 0},
    {"cpssExMxPm2DitBasedBridgeEnableSet",
        &wrCpssExMxPmDitBasedBridgeEnableSet,
        2, 0},
    {"cpssExMxPm2DitBasedBridgeEnableGet",
        &wrCpssExMxPmDitBasedBridgeEnableGet,
        1, 0},
    {"cpssExMxPm2DitMultiTargetMcQueuePriorityConfigSet",
        &wrCpssExMxPmDitMultiTargetMcQueuePriorityConfigSet,
        3, 0},
    {"cpssExMxPm2DitMultiTargetMcQueuePriorityConfigGet",
        &wrCpssExMxPmDitMultiTargetMcQueuePriorityConfigGet,
        2, 0},
    {"cpssExMxPm2DitMcTc2IpTvQueueConfigSet",
        &wrCpssExMxPmDitMcTc2IpTvQueueConfigSet,
        4, 0},
    {"cpssExMxPm2DitMcTc2IpTvQueueConfigGet",
        &wrCpssExMxPmDitMcTc2IpTvQueueConfigGet,
        1, 0},
    {"cpssExMxPm2DitMcTc2IpTvQueueIpTvXonStatusGet",
        &wrCpssExMxPmDitMcTc2IpTvQueueIpTvXonStatusGet,
        1, 0},
    {"cpssExMxPm2DitMultiTargetTCQueueConfigSet",
        &wrCpssExMxPmDitMultiTargetTCQueueConfigSet,
        4, 0},
    {"cpssExMxPm2DitMultiTargetTCQueueConfigGet",
        &wrCpssExMxPmDitMultiTargetTCQueueConfigGet,
        1, 0},
    {"cpssExMxPm2DitMultiTargetTCQueueStrictPriorityEnableSet",
        &wrCpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableSet,
        3, 0},
    {"cpssExMxPm2DitMultiTargetTCQueueStrictPriorityEnableGet",
        &wrCpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet,
        2, 0},
    {"cpssExMxPm2DitMultiTargetTCQueueWeightSet",
        &wrCpssExMxPmDitMultiTargetTCQueueWeightSet,
        3, 0},
    {"cpssExMxPm2DitMultiTargetTCQueueWeightGet",
        &wrCpssExMxPmDitMultiTargetTCQueueWeightGet,
        2, 0},
    {"cpssExMxPm2DitRateShaperSet",
        &wrCpssExMxPmDitRateShaperSet,
        5, 0},
    {"cpssExMxPm2DitRateShaperGet",
        &wrCpssExMxPmDitRateShaperGet,
        1, 0},
    {"cpssExMxPm2DitMultiTargetUcSchedModeSet",
        &wrCpssExMxPmDitMultiTargetUcSchedModeSet,
        6, 0},
    {"cpssExMxPm2DitMultiTargetUcSchedModeGet",
        &wrCpssExMxPmDitMultiTargetUcSchedModeGet,
        1, 0},


    {"cpssExMxPm2DitIpMcPairSet",
        &wrCpssExMxPmDitIpMcPairWrite,
        2, 25},
    {"cpssExMxPm2DitIpMcPairGetFirst",
        &wrDitIpMcPairEntryGetFirst,
        4, 0},
    {"cpssExMxPm2DitIpMcPairGetNext",
        &wrDitIpMcPairEntryGetNext,
        4, 0},

    {"cpssExMxPm2DitIpMplsEntrySet",
        &wrCpssExMxPmDitIpMplsEntryWrite,
        3, 31},
    {"cpssExMxPm2DitIpMplsEntryGetFirst",
        &wrCpssExMxPmDitIpMplsEntryReadFirst,
        5, 0},
    {"cpssExMxPm2DitIpMplsEntryGetNext",
        &wrCpssExMxPmDitIpMplsEntryReadNext,
        5, 0},

    {"cpssExMxPm2DitVplsEntrySet",
        &wrCpssExMxPmDitVplsEntryWrite,
        2, 26},
    {"cpssExMxPm2DitVplsEntryGetFirst",
        &wrCpssExMxPmDitVplsEntryReadFirst,
        4, 0},
    {"cpssExMxPm2DitVplsEntryGetNext",
        &wrCpssExMxPmDitVplsEntryReadNext,
        4, 0},
    {"cpssExMxPm2DitVplsEntry_1Set",
        &wrCpssExMxPmDitVplsEntryWrite_1,
        2, 27},
    {"cpssExMxPm2DitVplsEntry_1GetFirst",
        &wrCpssExMxPmDitVplsEntryReadFirst_1,
        4, 0},
    {"cpssExMxPm2DitVplsEntry_1GetNext",
        &wrCpssExMxPmDitVplsEntryReadNext_1,
        4, 0},
    {"cpssExMxPm2DitVplsUnknownUnicastFloodEnableSet",
        &wrCpssExMxPmDitVplsUnknownUnicastFloodEnableSet,
        2, 0},
    {"cpssExMxPm2DitVplsUnknownUnicastFloodEnableGet",
        &wrCpssExMxPmDitVplsUnknownUnicastFloodEnableGet,
        1, 0},

    {"cpssExMxPm2DitVpwsTargetTaggedModeSet",
        &wrCpssExMxPmDitVpwsTargetTaggedModeSet,
        2, 0},
    {"cpssExMxPm2DitVpwsTargetTaggedModeGet",
        &wrCpssExMxPmDitVpwsTargetTaggedModeGet,
        1, 0},
    {"cpssExMxPm2DitVpwsTargetTaggedControlSet",
        &wrCpssExMxPmDitVpwsTargetTaggedControlSet,
        2, 0},
    {"cpssExMxPm2DitVpwsTargetTaggedControlGet",
        &wrCpssExMxPmDitVpwsTargetTaggedControlGet,
        1, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmDit
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
GT_STATUS cmdLibInitCpssExMxPmDit
(
GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

