/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmSct.c
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
#include <cpss/exMxPm/exMxPmGen/sct/cpssExMxPmSct.h>
#include <cpss/exMxPm/exMxPmGen/inlif/cpssExMxPmInlif.h>

extern int osPrintf(char*, ...);

/* max num of inlif ID range 0..65535 */
#define MAX_NUM_INLIF_ID_CNS                               65535

/*******************************************************************************
* wrCpssExMxPm2InlifIndexToType
*
* DESCRIPTION:
*       Convert old inlif index to inlif type
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       inlifIndexOld    - Inlif Index (0..65535)
*
* OUTPUTS:
*       inlifTypePtr     - pointer to inlif type port/vlan/external
*       inlifIndexNewPtr - pointer to new inlif index
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong input parameters
*
* COMMENTS:
*       conversion from old inLif setting to new inLif setting:
*
*       OLD SETTING:
*       ============
*       internal inLif table
*       --------------------
*       if inLif index (4096..4159) - inLif ID is per port
*       access the inLif table with index: 4K + port num as index
*
*       if inLif index (0..4095) - inLif ID is per vlan
*       access the inLif table with VLAN ID as index
*
*       external inLif table
*       --------------------
*       if inLif index (4160..65535) - inLif ID is per vlan (TTI)
*       access the inLif table with index: inLif ID - 4K + 64
*
*       NEW SETTING:
*       ============
*       port - access the internal inLif table with index: (0..63)
*
*       vlan - access the internal inLif table with index: (0..4095)
*
*       external - access the external inLif table with index: (4096..65535)
*******************************************************************************/
/*puma 2*/
static CMD_STATUS wrCpssExMxPm2InlifIndexToType
(
    IN  GT_U32                       inlifIndexOld,
    OUT CPSS_EXMXPM_INLIF_TYPE_ENT  *inlifTypePtr,
    OUT GT_U32                      *inlifIndexNewPtr
)
{
    if (inlifIndexOld <= 4095)
    {
        *inlifIndexNewPtr = inlifIndexOld;
        *inlifTypePtr = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
    }
    else if (inlifIndexOld >= 4096 && inlifIndexOld <= 4159)
    {
        *inlifIndexNewPtr = inlifIndexOld - 4096;
        *inlifTypePtr = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    }
    else if (inlifIndexOld >= 4160 && inlifIndexOld <= 65535)
    {
        *inlifIndexNewPtr = inlifIndexOld;
        *inlifTypePtr = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
    }
    else
        return GT_BAD_PARAM;


    return GT_OK;
}

/*******************************************************************************
* cpssExMxPmSctPortIeeeReservedMcTrapEnableSet
*
* DESCRIPTION:
*       Enables trapping or mirroring to CPU Multicast packets per port with
*       MAC_DA in the IEEE reserved Multicast range.

*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum    - device number
*       portnum   - port number
*       enable    - GT_FALSE -
*                     Trapping or mirroring to CPU of packet with
*                     MAC_DA in the IEEE reserved Multicast range, at the port
*                     is disabled.
*                 - GT_TRUE -
*                     Trapping or mirroring to CPU of packet, with
*                     MAC_DA in the IEEE reserved Multicast range, at the port
*                     is enabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctPortIeeeReservedMcTrapEnableSet
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
    result = cpssExMxPmSctPortIeeeReservedMcTrapEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctPortIeeeReservedMcTrapEnableGet
*
* DESCRIPTION:
*       Get status of trapping or mirroring to CPU Multicast packets per port
*       with MAC_DA in the IEEE reserved Multicast range.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*
*
* OUTPUTS:
*       enablePtr - GT_FALSE -
*                     Trapping or mirroring to CPU of packet with
*                     MAC_DA in the IEEE reserved Multicast range, at the port
*                     is disabled.
*                   GT_TRUE -
*                     Trapping or mirroring to CPU of packet, with
*                     MAC_DA in the IEEE reserved Multicast range, at the port
*                     is enabled
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctPortIeeeReservedMcTrapEnableGet
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
    result = cpssExMxPmSctPortIeeeReservedMcTrapEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctPortIeeeReservedMcProfileSet
*
* DESCRIPTION:
*       This function binds the port to one of the IEEE reserved multicast
*       profiles. There are 2 profiles to allow for diffrerent configuration
*       on customer ports (which need to trap customer BPDU packets)
*       and provider network ports (which need to trap provider BPDU
*       packets and forward customer BPDU packets based on different MAC
*       addresses).
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       profileIndex - index of profile. Can be either 0 or 1.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctPortIeeeReservedMcProfileSet
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
    GT_U32 profileIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    profileIndex = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmSctPortIeeeReservedMcProfileSet(devNum, portNum, profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctPortIeeeReservedMcProfileGet
*
* DESCRIPTION:
*       This function gets the IEEE reserved multicast profile binded to the
*       port. There are 2 profiles to allow for diffrerent configuration
*       on customer ports (which need to trap customer BPDU packets)
*       and provider network ports (which need to trap provider BPDU
*       packets and forward customer BPDU packets based on different MAC
*       addresses).
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*
* OUTPUTS:
*       profileIndexPtr - pointer to profile index. The *profileIndexPtr
*                         can be either 0 or 1.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctPortIeeeReservedMcProfileGet
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
    GT_U32 profileIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmSctPortIeeeReservedMcProfileGet(devNum, portNum, &profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIeeeReservedMcProtocolCmdSet
*
* DESCRIPTION:
*       This function set the command for specific IEEE reserved multicast
*       protocol at specific profile. Each profile can be configured with 16
*       standard protocols and 16 GARP protocols.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum          - device number
*       profileIndex - index of profile. Can be either 0 or 1.
*       protocolType - This param can be :
*                      1. CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E -
*                         standart protocols, that has MAC range
*                         01-80-C2-00-00-00 to 01-80-C2-00-00-0F.
*                      2. CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E -
*                         GARP protocols, that has a MAC range of:
*                         01-80-C2-00-00-20 to 01-80-C2-00-00-2F.
*
*       protocol     -  specific protocol expressed by last 4 bits of
*                       01-80-C2-00-00-0x or 01-80-C2-00-00-2x
*       cmd          -  specify command to process packet with a MAC destination
*                       Multicast address. In our case the cmd could take the
*                       the following relevant values:
*                       CPSS_PACKET_CMD_FORWARD_E,
*                       CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                       CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                       CPSS_PACKET_CMD_DROP_SOFT_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIeeeReservedMcProtocolCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 profileIndex;
    CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT protocolType;
    GT_U32 protocol;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    profileIndex = (GT_U32)inArgs[1];
    protocolType = (CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT)inArgs[2];
    protocol = (GT_U32)inArgs[3];
    cmd = (CPSS_PACKET_CMD_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmSctIeeeReservedMcProtocolCmdSet(devNum, profileIndex, protocolType, protocol, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIeeeReservedMcProtocolCmdGet
*
* DESCRIPTION:
*       This function get the command for specific IEEE reserved multicast
*       protocol at specific profile. Each profile can be configured with 16
*       standard protocols and 16 GARP protocols.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum          - device number
*       profileIndex - index of profile. Can be either 0 or 1.
*       protocolType - This param could be :
*                      1. CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E -
*                         standart protocols, that has MAC range
*                         01-80-C2-00-00-00 to 01-80-C2-00-00-0F.
*
*                      2. CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E -
*                         GARP protocols, that has a MAC range of:
*                         01-80-C2-00-00-20 to 01-80-C2-00-00-2F.
*
*       protocol     - specific protocol expressed by last 4 bits of
*                      01-80-C2-00-00-0x or 01-80-C2-00-00-2x.
*
*
* OUTPUTS:
*       cmdPtr       - pointer to command that was set to process packet
*                      with a MAC  destination Multicast address. The relevant
*                      values of *cmdPtr as follows:
*                      CPSS_PACKET_CMD_FORWARD_E,
*                      CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                      CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                      CPSS_PACKET_CMD_DROP_SOFT_E
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIeeeReservedMcProtocolCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 profileIndex;
    CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT protocolType;
    GT_U32 protocol;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    profileIndex = (GT_U32)inArgs[1];
    protocolType = (CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT)inArgs[2];
    protocol = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmSctIeeeReservedMcProtocolCmdGet(devNum, profileIndex, protocolType, protocol, &cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cmd);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet
*
* DESCRIPTION:
*       This function set the CPU code for specific IEEE reserved multicast
*       protocol at specific profile.
*       Each profile can be configured with 16 standard protocols
*       and 16 GARP protocols.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum          - device number
*       profileIndex - index of profile. Can be either 0 or 1.
*       protocolType - This param could be :
*                      1. CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E -
*                         standart protocols, that has MAC range
*                         01-80-C2-00-00-00 to 01-80-C2-00-00-0F.
*                         (note that the specific protocol expressed
*                          by  last 4 bits are set by the next parameter)
*                      2. CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E -
*                         GARP protocols, that has a MAC range of:
*                         01-80-C2-00-00-20 to 01-80-C2-00-00-2F.
*                         (note that the specific protocol expressed
*                          by  last 4 bits are set by the next parameter)
*       protocol     -  specific protocol expressed by last 4 bits of
*                       01-80-C2-00-00-0x or 01-80-C2-00-00-2x.
*       cpuCode      -  CPU code for packets trapped or mirrored to CPU.
*                       Can be either:
*                       CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E
*                       CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E
*                       CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E
*                       CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 profileIndex;
    CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT protocolType;
    GT_U32 protocol;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    profileIndex = (GT_U32)inArgs[1];
    protocolType = (CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT)inArgs[2];
    protocol = (GT_U32)inArgs[3];
    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet(devNum, profileIndex, protocolType, protocol, cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet
*
* DESCRIPTION:
*       This function get the CPU code for specific IEEE reserved multicast
*       protocol at specific profile.
*       Each profile can be configured with 16 standard protocols
*       and 16 GARP protocols.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum          - device number
*       profileIndex - index of profile. Can be either 0 or 1.
*       protocolType - This param could be :
*                      1. CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_STD_E -
*                         standart protocols, that has MAC range
*                         01-80-C2-00-00-00 to 01-80-C2-00-00-0F.
*                         (note that the specific protocol expressed
*                          by  last 4 bits are set by the next parameter)
*                      2. CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_GARP_E -
*                         GARP protocols, that has a MAC range of:
*                         01-80-C2-00-00-20 to 01-80-C2-00-00-2F.
*                         (note that the specific protocol expressed
*                          by  last 4 bits are set by the next parameter)
*       protocol     -  specific protocol expressed by last 4 bits of
*                       01-80-C2-00-00-0x or 01-80-C2-00-00-2x.
*
*
* OUTPUTS:
*       cpuCodePtr   -  pointer to cpuCode that was set for packets trapped
*                       or mirrored to CPU.
*                       *cpuCodePtr can be either:
*                       CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E
*                       CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E
*                       CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E
*                       CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 profileIndex;
    CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT protocolType;
    GT_U32 protocol;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    profileIndex = (GT_U32)inArgs[1];
    protocolType = (CPSS_EXMXPM_SCT_IEEE_RSV_MC_PROTOCOL_ENT)inArgs[2];
    protocol = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet(devNum, profileIndex, protocolType, protocol, &cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cpuCode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIeeeReservedMcTrappedLearnEnableSet
*
* DESCRIPTION:
*       Sets Bridge engine source address learning ability (Enable/Disable)
*       of IEEE reserved Multicast packets that are trapped to the CPU.
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       enable          - Boolean value:
*                         GT_TRUE  - Packets that are assigned a command of
*                                    Trap by the IEEE Reserved MC mechanism,
*                                    perform MAC SA learning.
*                         GT_FALSE - Packets that are assigned a command of
*                                    Trap by the IEEE Reserved MC mechanism,
*                                    do not perform MAC SA learning.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIeeeReservedMcTrappedLearnEnableSet
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
    result = cpssExMxPmSctIeeeReservedMcTrappedLearnEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet
*
* DESCRIPTION:
*       Gets Bridge engine source address learning ability (Enable/Disable)
*       of IEEE reserved Multicast packets that are trapped to the CPU.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       enablePtr       - pointer to the Boolean value:
*                         GT_TRUE  - Packets that are assigned a command of
*                                    Trap by the IEEE Reserved MC mechanism,
*                                    perform MAC SA learning.
*                         GT_FALSE - Packets that are assigned a command of
*                                    Trap by the IEEE Reserved MC mechanism,
*                                    do not perform MAC SA learning.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet
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
    result = cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);



    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCiscoL2McCmdSet
*
* DESCRIPTION:
*       Set the Global command of the Cisco Proprietary Layer 2 Control
*       Multicasts, with MAC DA = 0x01-00-0C-xx-xx-xx for specified device.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*       cmd     - supported commands:
*                   CPSS_PACKET_CMD_FORWARD_E ,
*                   CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                   CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       To enable this feature set the Inlif's relevant
*       contol traffic state (enabled/disabled).
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCiscoL2McCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cmd = (CPSS_PACKET_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctCiscoL2McCmdSet(devNum, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCiscoL2McCmdGet
*
* DESCRIPTION:
*       Get the Global command of the Cisco Proprietary Layer 2 Control
*       Multicasts.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum - device number
*
*
* OUTPUTS:
*       cmdPtr   - pointer to cmd that was set for packets
*                  with MAC DA = 0x01-00-0C-xx-xx-xx running
*                  Cisco Proprietary Layer 2 Control Multicasts.
*                  The *cmdPtr should be either
*                       CPSS_PACKET_CMD_FORWARD_E ,
*                       CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                       CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCiscoL2McCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmSctCiscoL2McCmdGet(devNum, &cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cmd);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIgmpSnoopModeSet
*
* DESCRIPTION:
*       Set global trap/mirror mode for IGMP snoop on specified device
*       per inlif.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum    -     device number
*       mode   - CPSS_IGMP_ALL_TRAP_MODE_E [mode 0] : Trap mode: All IGMP
*                  packets are trapped to the CPU, regardless of their type.
*                CPSS_IGMP_SNOOP_TRAP_MODE_E [mode 1] : IGMP Snoop mode:
*                  Query messages are mirrored to the CPU. Non-Query messages
*                   are Trapped to the CPU.
*                CPSS_IGMP_ROUTER_MIRROR_MODE_E [mode 2] : IGMP Router mode:
*                   All IGMP packets are mirrored to the CPU, regardless of
*                   their type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on invalid input paramteres value
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error
*
* COMMENTS:
*       To enable this feature set the Inlif's relevant
*       contol traffic state (enabled/disabled).
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIgmpSnoopModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_IGMP_SNOOP_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_IGMP_SNOOP_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctIgmpSnoopModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIgmpSnoopModeGet
*
* DESCRIPTION:
*       Get global trap/mirror mode for IGMP snoop on specified device.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum       - device number
*
*
* OUTPUTS:
*       modePtr   - pointer to  IgmpSnoop mode. *modePtr shoud be either:
*                CPSS_IGMP_ALL_TRAP_MODE_E [mode 0] : All IGMP packets are
*                   trapped to the CPU, regardless of their type.
*                CPSS_IGMP_SNOOP_TRAP_MODE_E [mode 1] :IGMP Snoop mode: Query
*                    messages are mirrored to the CPU. Non-Query messages are
*                    Trapped to the CPU.
*                CPSS_IGMP_ROUTER_MIRROR_MODE_E [mode 2] :IGMP Router mode: All
*                    IGMP packets are mirrored to the CPU, regardless of their
*                    type.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on invalid input paramteres value
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIgmpSnoopModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_IGMP_SNOOP_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_IGMP_SNOOP_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctIgmpSnoopModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIcmpV6MsgTypeSet
*
* DESCRIPTION:
*       Set trap/mirror/forward command for specified ICMP V6 message type.
*
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum     - device number
*       index   - index of the message type entry(up to eight IPv6 ICMP types)
*                 values 0..7
*       msgType - ICMP V6  message type 0..255
*       cmd     - supported commands:
*                   CPSS_PACKET_CMD_FORWARD_E ,
*                   CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                   CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       To enable this feature set the Inlif's relevant
*       control traffic state (enabled/disabled).
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIcmpV6MsgTypeSet
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
    GT_U32 msgType;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    msgType = (GT_U32)inArgs[2];
    cmd = (CPSS_PACKET_CMD_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmSctIcmpV6MsgTypeSet(devNum, index, msgType, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIcmpV6MsgTypeGet
*
* DESCRIPTION:
*       Get trap/mirror/forward command for specified ICMP V6 message type.
*
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum     - device number
*       index   - index of the message type entry(up to eight IPv6 ICMP types)
*                 values 0..7
*
* OUTPUTS:
*       msgTypePtr - ICMP V6 message type pointer
*       cmdPtr     - ICMP V6 command pointer (0..255)
*           *cmdPtr can be either:
*                  CPSS_PACKET_CMD_FORWARD_E ,
*                  CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                  CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIcmpV6MsgTypeGet
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
    GT_U32 msgType;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctIcmpV6MsgTypeGet(devNum, index, &msgType, &cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", msgType, cmd);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctUdpBcDestPortCfgSet
*
* DESCRIPTION:
*       Configure UDP Broadcast Destination Port configuration table entry.
*       It's possible to configure up to 12 UDP destination ports with their
*       Trap or Mirror to CPU command and CPU code.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum         - device number
*       entryIndex  - index in the global UDP Broadcast Port Table, allows to
*                     application to configure up to 12 UDP ports (0..11)
*       udpPortNum  - UDP destination port number 0-65335
*       cpuCode     - CPU code for packets trapped or mirrored by the feature.
*                     Acceptable CPU codes are:
*                     CPSS_NET_UDP_BC_MIRROR_TRAP0_E,
*                     CPSS_NET_UDP_BC_MIRROR_TRAP1_E,
*                     CPSS_NET_UDP_BC_MIRROR_TRAP2_E,
*                     CPSS_NET_UDP_BC_MIRROR_TRAP3_E
*       cmd         - command for UDP Broadcast packets
*                     either CPSS_DXCH_BRG_GEN_CNTRL_TRAP_E
*                     or CPSS_DXCH_BRG_GEN_CNTRL_MIRROR_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PARAM        - on invalid input paramteres value
*       GT_HW_ERROR         - failed to write to hw.
*       GT_FAIL             - on error
*
* COMMENTS:
*       To enable this feature set the Inlif's relevant
*       contol traffic state (enabled/disabled).
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctUdpBcDestPortCfgSet
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
    GT_U32 udpPortNum;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    entryIndex = (GT_U32)inArgs[1];
    udpPortNum = (GT_U32)inArgs[2];
    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[3];
    cmd = (CPSS_PACKET_CMD_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmSctUdpBcDestPortCfgSet(devNum, entryIndex, udpPortNum, cpuCode, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctUdpBcDestPortCfgGet
*
* DESCRIPTION:
*       Get UDP Broadcast Destination Port configuration.
*       It's possible to configure up to 12 UDP destination ports with their
*       Trap or Mirror to CPU command and CPU code.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum             - device number
*       entryIndex      - index in the global UDP Broadcast Port Table, allows
*                         to application to configure up to 12 UDP ports (0..11)
*
*
* OUTPUTS:
*       udpPortNumPtr   - UDP destination port number pointer (0-65335)
*       cpuCodePtr      - CPU code pointer for packets trapped or mirrored by
*                         the feature.  Acceptable CPU codes are:
*                                       CPSS_NET_UDP_BC_MIRROR_TRAP0_E,
*                                       CPSS_NET_UDP_BC_MIRROR_TRAP1_E,
*                                       CPSS_NET_UDP_BC_MIRROR_TRAP2_E,
*                                       CPSS_NET_UDP_BC_MIRROR_TRAP3_E
*       cmdPtr          - command pointer for UDP Broadcast packets.
*                           It could be either
*                               CPSS_DXCH_BRG_GEN_CNTRL_TRAP_E
*                               CPSS_DXCH_BRG_GEN_CNTRL_MIRROR_E
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PARAM        - oon invalid input paramteres value
*       GT_BAD_PTR          - one of the parameters is NULL pointer
*       GT_HW_ERROR         - failed to write to hw.
*       GT_FAIL             - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctUdpBcDestPortCfgGet
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
    GT_U32 udpPortNum;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    entryIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctUdpBcDestPortCfgGet(devNum, entryIndex, &udpPortNum, &cpuCode, &cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", udpPortNum, cpuCode, cmd);

    return CMD_OK;
}

/*******************************************************************************
*   cpssExMxPmSctUdpBcDestPortCfgInvalidate
*
* DESCRIPTION:
*       Invalidate UDP Broadcast Destination Port configuration table's entry.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
*
* INPUTS:
*       devNum        - device number
*       entryIndex - index in the global UDP Broadcast Port Table, allows to
*                    application to configure up to 12 UDP ports
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PARAM        - on invalid input paramteres value
*       GT_NOT_SUPPORTED    - request is not supported for this device type.
*       GT_HW_ERROR         - failed to write to hw.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctUdpBcDestPortCfgInvalidate
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
    result = cpssExMxPmSctUdpBcDestPortCfgInvalidate(devNum, entryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
*   cpssExMxPmSctArpBcastCmdSet
*
* DESCRIPTION:
*       Configures the ARP BC command to trap/mirror/forward to CPU
*       all ARP Broadcast packets.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*       cmd     - supported commands:
*                   CPSS_PACKET_CMD_FORWARD_E ,
*                   CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                   CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       To enable this feature set the Inlif's relevant
*       contol traffic state (enabled/disabled).
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctArpBcastCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cmd = (CPSS_PACKET_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctArpBcastCmdSet(devNum, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
*   cpssExMxPmSctArpBcastCmdGet
*
* DESCRIPTION:
*       Get the ARP BC command that was set to trap/mirror/forward to CPU
*       all ARP Broadcast packets .
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*
*
*
* OUTPUTS:
*       cmdPtr - ARP Bcast command pointer. Acceptable *cmdPtr values are:
*                 CPSS_PACKET_CMD_FORWARD_E ,
                  CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                 CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       To enable this feature set the Inlif's relevant
*       contol traffic state (enabled/disabled).
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctArpBcastCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmSctArpBcastCmdGet(devNum, &cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cmd);

    return CMD_OK;
}

/*******************************************************************************
*   cpssExMxPmSctArpToMeCmdSet
*
* DESCRIPTION:
*       Configures the ARP-To-Me  command to trap/mirror/forward to CPU
*       only ARP request packets which are requesting a MAC address of the
*       current system.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*       cmd     - supported commands:
*                  CPSS_PACKET_CMD_FORWARD_E ,
*                  CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                  CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       To enable this feature set the Inlif's relevant
*       contol traffic state (enabled/disabled).
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctArpToMeCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cmd = (CPSS_PACKET_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctArpToMeCmdSet(devNum, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
*   cpssExMxPmSctArpToMeCmdGet
*
* DESCRIPTION:
*       Get the ARP-To-Me  command was set to trap/mirror/forward to CPU
*       only ARP request packets which are requesting a MAC address of the
*       current system.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*
*
*
* OUTPUTS:
*       cmdPtr - ARP Bcast command pointer. Acceptable *cmdPtr values are:
*                 CPSS_PACKET_CMD_FORWARD_E ,
*                 CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                 CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctArpToMeCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmSctArpToMeCmdGet(devNum, &cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cmd);

    return CMD_OK;
}

/*******************************************************************************
*   wrCpssExMxPmSctArpBcMirrorEgressEnableSet
*
* DESCRIPTION:
*       Enable ARP BC Mirror to CPU
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*       enable     - GT_TRUE - Arp BC packets will be mirrored to CPU
*                    GT_FALSE - Arp BC packets will be not mirrored to CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctArpBcMirrorEgressEnableSet
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
    osPrintf("\nCall cpss API");
    result = cpssExMxPmSctArpBcMirrorEgressEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
*   wrCpssExMxPmSctArpBcMirrorEgressEnableGet
*
* DESCRIPTION:
*       Get the ARP BC mirror to CPU state (Enable/Disable)
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE - Arp BC packets will be mirrored to CPU
*                   GT_FALSE - Arp BC packets will be not mirrored to CPU
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctArpBcMirrorEgressEnableGet
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
    result = cpssExMxPmSctArpBcMirrorEgressEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
*   cpssExMxPmSctIpV6SolicitedNodeMcastPrefixSet
*
* DESCRIPTION:
*       Set IPv6 Solicited-Node Multicast Address Prefix Configuration and Mask.
*       IPv6 Neighbor Solicitation messages may be trapped or mirrored to the
*       CPU, if the packets Inlif has this setting enabled, DIP matches the
*       configured prefix and according to the setting of IPv6 Neighbor
*       Solicited Node Command.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum         - device number
*       addressPtr  - (pointer to)IP address
*       maskPtr     - (pointer to) mask of the address
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on invalid input paramteres value
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error.
*
* COMMENTS:
*       To enable this feature set the Inlif's relevant
*       contol traffic state (enabled/disabled).
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIpV6SolicitedNodeMcPrefixSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_IPV6ADDR address;
    GT_IPV6ADDR mask;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum= (GT_U8)inArgs[0];
    galtisIpv6Addr(&address, (GT_U8*)inFields[0]);
    galtisIpv6Addr(&mask, (GT_U8*)inFields[1]);

    /* call cpss api function */
    result = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixSet(devNum, &address, &mask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
*   cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet
*
* DESCRIPTION:
*       Get IPv6 Solicited-Node Multicast Address Prefix Configuration and Mask.
*       IPv6 Neighbor Solicitation messages may be trapped or mirrored to the
*       CPU, if the packets Inlif has this setting enabled, DIP matches the
*       configured prefix and according to the setting of IPv6 Neighbor
*       Solicited Node Command.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*
*
* OUTPUTS:
*       addressPtr - (pointer to)IP address
*       maskPtr    - (pointer to) mask of the address
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on invalid input paramteres value
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error.
*
* COMMENTS:
*      Wrapper function to  get first element
*
*******************************************************************************/

static CMD_STATUS wrCpssExMxPmSctIpV6SolicitedNodeMcPrefixGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_IPV6ADDR address;
    GT_IPV6ADDR mask;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;



    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];



    /* call cpss api function */
    result = cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet(devNum, &address, &mask);


    if(result !=GT_OK)
 {
     galtisOutput(outArgs, result, "");
     return CMD_OK;
 }





   /* pack and output table fields */
 fieldOutput("%16B%16B", address.arIP, mask.arIP);

 /* pack output arguments to galtis string */
 galtisOutput(outArgs, result, "%f");
 return CMD_OK;
}
/*******************************************************************************
*   cpssExMxPmSctIpV6SolicitedNodeMcastPrefixGet
*
* DESCRIPTION:
*       Get IPv6 Solicited-Node Multicast Address Prefix Configuration and Mask.
*       IPv6 Neighbor Solicitation messages may be trapped or mirrored to the
*       CPU, if the packets Inlif has this setting enabled, DIP matches the
*       configured prefix and according to the setting of IPv6 Neighbor
*       Solicited Node Command.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*
*
* OUTPUTS:
*       addressPtr - (pointer to)IP address
*       maskPtr    - (pointer to) mask of the address
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on invalid input paramteres value
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error.
*
* COMMENTS:
*      Wrapper function to  get next element(empty because there is no next element)
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIpV6SolicitedNodeMcPrefixGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{


    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}
/*******************************************************************************
*   cpssExMxPmSctIpV6SolicitedCmdSet
*
* DESCRIPTION:
*       Set  command for IPv6 Neighbor Solicitation Protocol.
*       When enabled relevant Inlif, IPv6 Neighbor Solicitation messages may be
*       trapped or mirrored to the CPU, if the packets DIP matches the
*       configured prefix and according to the setting of IPv6 Neighbor
*       Solicited Node Command.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*       cmd     - supported commands:
*                 CPSS_PACKET_CMD_FORWARD_E ,
*                 CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                 CPSS_PACKET_CMD_TRAP_TO_CPU_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on invalid input paramteres value
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error.
*
* COMMENTS:
*       To enable this feature set the Inlif's relevant
*       contol traffic state (enabled/disabled).
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIpV6SolicitedCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cmd = (CPSS_PACKET_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctIpV6SolicitedCmdSet(devNum, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
*   cpssExMxPmSctIpV6SolicitedCmdGet
*
* DESCRIPTION:
*       Get command for IPv6 Neighbor Solicitation Protocol.
*       When enabled relevant Inlif, IPv6 Neighbor Solicitation messages may be
*       trapped or mirrored to the CPU, if the packets DIP matches the
*       configured prefix and according to the setting of IPv6 Neighbor
*       Solicited Node Command.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum     - device number
*
*
* OUTPUTS:
*       cmdPtr  - command pointer. The correct *cmdPtr values are:
*                                   CPSS_PACKET_CMD_FORWARD_E ,
*                                   CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                                   CPSS_PACKET_CMD_TRAP_TO_CPU_E .
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on invalid input paramteres value
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error.
*       GT_FAIL         - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIpV6SolicitedCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmSctIpV6SolicitedCmdGet(devNum, &cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cmd);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIpLinkLocalMirrorToCpuEnableSet
*
* DESCRIPTION:
*       Global Enable/disable mirroring to CPU for IPv4/6 Link Local Control
*       protocols.
*       In IPv4, the link-local IANA Multicast range is 224.0.0.0/24.
*       In IPv6, the link-local IANA Multicast range is FF02::/16.
*
* INPUTS:
*        devNum            - device number
*        protocolStack  - IPv4 or IPv6 protocol version
*        enable         - enable/disable mirroring to CPU
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       To enable this feature set the Inlif's relevant
*       contol traffic state (enabled/disabled).
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIpLinkLocalMirrorToCpuEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    GT_BOOL enable;
    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntry;
    GT_U32 index;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    GT_U32 inlifIndexNew;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    if((protocolStack != CPSS_IP_PROTOCOL_IPV4_E) &&
       (protocolStack != CPSS_IP_PROTOCOL_IPV6_E))
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    for(index = 0; index < MAX_NUM_INLIF_ID_CNS; index++)
    {
        wrCpssExMxPm2InlifIndexToType(index,&inlifType,&inlifIndexNew);

        result = cpssExMxPmInlifEntryGet(devNum, inlifType, inlifIndexNew, &inlifEntry);

        if(result != GT_OK)
        {
            galtisOutput(outArgs, result, "");
            return CMD_OK;
        }

        if(protocolStack == CPSS_IP_PROTOCOL_IPV4_E)
        {
            inlifEntry.ipv4LinkLocalMcCommandEnable = enable;
        }
        else
        {
            inlifEntry.ipv6LinkLocalMcCommandEnable = enable;
        }

        result = cpssExMxPmInlifEntrySet(devNum, inlifType, inlifIndexNew, &inlifEntry);

        if(result != GT_OK)
        {
            galtisOutput(outArgs, result, "");
            return CMD_OK;
        }

    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIpLinkLocalMirrorToCpuEnableGet
*
* DESCRIPTION:
*       Get the current state (Enable/disable) of  mirroring to CPU
*       IPv4/6 link-local Control Protocols.
*
*       In IPv4, the link-local IANA Multicast range is 224.0.0.0/24.
*       In IPv6, the link-local IANA Multicast range is FF02::/16.
*
*
* INPUTS:
*        devNum            - device number
*        protocolStack  - IPv4 or IPv6 protocol version
*
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* OUTPUTS:
*       enablePtr -   enable/disable pointer. It could be as follows:
*                     - GT_FALSE -
*                        disable mirroring to CPU for IP Link Local Control
*                        protocols
*                     - GT_TRUE -
*                        enable mirroring to CPU for IP Link Local Control
*                        protocols
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIpLinkLocalMirrorToCpuEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    GT_BOOL enable;
    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntry;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];

    result = cpssExMxPmInlifEntryGet(devNum, CPSS_EXMXPM_INLIF_TYPE_PORT_E, 0, &inlifEntry);

    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", GT_FALSE);
        return CMD_OK;
    }

    switch(protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV4_E:
            enable = inlifEntry.ipv4LinkLocalMcCommandEnable;
            break;
        case CPSS_IP_PROTOCOL_IPV6_E:
            enable = inlifEntry.ipv6LinkLocalMcCommandEnable;
            break;
        default:
            galtisOutput(outArgs, result, "%d", GT_FALSE);
            return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableSet
*
* DESCRIPTION:
*       Enable/disable mirroring to CPU for specific IPv4/6 Link Local Control
*       protocol.
*       IPv4 Multicast addresses in the range 224.0.0.x and IPv6 Multicast
*       addresses in the range FF:02::/16 are reserved by IANA
*       for link-local control protocols. if inlif link-local multicat
*       mirroring mechanism is enabled and there is any of the IPv4/6 Multicast
*       packets with a DIP in this range it will be mirrored to the CPU.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum           - device number
*       protocolStack - IPv4 or IPv6 protocol version
*       protocol      - Specifies the LSB of IP Link Local multicast protocol
*                       (range: 0-255)
*       enable        - enable/disable mirroring to CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on invalid input paramteres value
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error
*
* COMMENTS:
*       To enable this feature set the Inlif's relevant
*       contol traffic state (enabled/disabled).
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    GT_U32 protocol;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    protocol = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableSet(devNum, protocolStack, protocol, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet
*
* DESCRIPTION:
*       Get Enable/disable state of mirroring to CPU for specific IP Link Local
*       Control protocol.
*       IPv4 Multicast addresses in the range 224.0.0.x and IPv6 Multicast
*       addresses in the range FF:02::/16 are reserved by IANA
*       for link-local control protocols.
*
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum           - device number
*       protocolStack - IPv4 or IPv6 protocol version
*       protocol      - Specifies the LSB of IP Link Local multicast protocol
*                       (range: 0-255)
*
* OUTPUTS:
*       enablePtr     enable/disable pointer. It could be as follows:
*                     - GT_FALSE -
*                           disable mirroring to CPU for specific IP Link Local
*                           Control protocol
*                     - GT_TRUE -
*                           enable mirroring to CPU for IP specific Link Local
*                           Control protocol
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on invalid input paramteres value
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    GT_U32 protocol;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    protocol = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet(devNum, protocolStack, protocol, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet
*
* DESCRIPTION:
*       Configures the specific CPU code to be used for the IPv4/6 Link Local
*       control packets mirrored to the CPU.
*       Each "protocol" may be assgined with one of 4 possible CPU codes.
*
*
* INPUTS:
*       devNum            - device number
*       protocolStack  - IPv4 or IPv6 protocol version
*       protocol       - Specifies the LSB of IP Link Local multicast protocol
*                       (range: 0-255)
*       cpuCode        - CPU code for IPv4/6 Link Local control packets
*                         mirrored to the CPU .
*                         Can be either:
*                               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E
*                               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E
*                               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E
*                               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on invalid input paramteres value
*       GT_HW_ERROR   - on hardware error
*       GT_FAIL       - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIpLinkLocalProtocolCpuCodeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    GT_U32 protocol;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    protocol = (GT_U32)inArgs[2];
    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet(devNum, protocolStack, protocol, cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet
*
* DESCRIPTION:
*       Get the specific CPU code being used for the IPv4/6 Link Local
*       control packets mirrored to the CPU.
*       Each "protocol" may be assgined with one of 4 possible CPU codes
*
*
* INPUTS:
*       devNum            - device number
*       protocolStack  - IPv4 or IPv6 protocol version
*       protocol       - Specifies the LSB of IP Link Local multicast protocol
*                       (range: 0-255)
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* OUTPUTS:
*       cpuCodePtr     - CPU code pointer.
*       *cpuCodePtr correct values are:
*                   CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E
*                   CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E
*                   CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E
*                   CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctIpLinkLocalProtocolCpuCodeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    GT_U32 protocol;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    protocol = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet(devNum, protocolStack, protocol, &cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cpuCode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableSet
*
* DESCRIPTION:
*       Enable/Disable Application Specific CPU Code for TCP SYN packets
*       forwarded to the CPU (TCP_SYN_TO_CPU).
*
* INPUTS:
*       devNum            - device number
*       enable         - enable/disable application specific CPU Code for TCP
*                         SYN packets.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*       This feature is enabled for packets if one of conditions are true:
*       1. MAC DA is found in the FDB and Application Specific CPU Code field
*          of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY).
*       2. Packet passed Routing and Application Specific CPU Code field is
*          True in the Route entry (Next Hop).
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctAppSpecificCpuCodeTcpSynEnableSet
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
    result = cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet
*
* DESCRIPTION:
*       Get current state of Application Specific CPU Code for TCP SYN
*       packets forwarded to the CPU (TCP_SYN_TO_CPU).
*
*
* INPUTS:
*       devNum            - device number
*
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* OUTPUTS:
*       enablePtr       - enable/disable pointer. It could be as follows:
*                         - GT_FALSE -
*                           Disable Application Specific CPU Code for TCP SYN
*                           packets forwarded to the CPU.
*                         - GT_TRUE -
*                           Enable Application Specific CPU Code for TCP SYN
*                           packets forwarded to the CPU.
*
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet
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
    result = cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet
*
* DESCRIPTION:
*       Set range to TCP/UPD Destination Port Range CPU Code Table with specific
*       CPU Code. There are 16 ranges may be defined.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (range 0..15)
*       maxDstPort - the maximum destination port in this range.
*       minDstPort - the minimum destination port in this range
*       packetType - packet type:(Unicast/Multicast/BOTH_Unicast_Multicast)
*                                CPSS_NET_TCP_UDP_PACKET_UC_E,
*                                CPSS_NET_TCP_UDP_PACKET_MC_E,
*                                CPSS_NET_TCP_UDP_PACKET_BOTH_UC_MC_E
*       protocol   - protocol type: (UDP/TCP/BOTH_UDP_TCP)
*                                   CPSS_NET_PROT_UDP_E,
*                                   CPSS_NET_PROT_TCP_E,
*                                   CPSS_NET_PROT_BOTH_UDP_TCP_E
*       cpuCode    - CPU Code Index for this TCP/UDP range
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       This feature enabled for packets if one of conditions are true:
*        1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*        2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rangeIndex;
    GT_U32 minDstPort;
    GT_U32 maxDstPort;
    CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetType;
    CPSS_NET_PROT_ENT protocol;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    rangeIndex = (GT_U32)inArgs[1];
    minDstPort = (GT_U32)inArgs[2];
    maxDstPort = (GT_U32)inArgs[3];
    packetType = (CPSS_NET_TCP_UDP_PACKET_TYPE_ENT)inArgs[4];
    protocol = (CPSS_NET_PROT_ENT)inArgs[5];
    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[6];

    /* call cpss api function */
    result = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet(devNum, rangeIndex, minDstPort, maxDstPort, packetType, protocol, cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet
*
* DESCRIPTION:
*       Get range for TCP/UPD Destination Port Range CPU Code Table with
*       specific CPU Code. There are 16 ranges may be defined.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (range 0..15)
*
* OUTPUTS:
*       validPtr  - (pointer to) is the entry valid
*                   GT_FALSE - range is't valid
*                   GT_TRUE  - range is valid
*       maxDstPortPtr - (pointer to)the maximum destination port in this range.
*       minDstPortPtr - (pointer to)the minimum destination port in this range
*       packetTypePtr - (pointer to)packet type.
*                        *packetTypePtr can be either:
*                                CPSS_NET_TCP_UDP_PACKET_UC_E,
*                                CPSS_NET_TCP_UDP_PACKET_MC_E,
*                                CPSS_NET_TCP_UDP_PACKET_BOTH_UC_MC_E
*       protocolPtr   - (pointer to)protocol type.
*                       *protocolPtr can be either:
*                                CPSS_NET_PROT_UDP_E,
*                                CPSS_NET_PROT_TCP_E,
*                                CPSS_NET_PROT_BOTH_UDP_TCP_E
*
*       cpuCodePtr - (pointer to)User defined CPU code for this range
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rangeIndex;
    GT_BOOL valid;
    GT_U32 minDstPort;
    GT_U32 maxDstPort;
    CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetType;
    CPSS_NET_PROT_ENT protocol;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    rangeIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet(devNum, rangeIndex, &valid, &minDstPort, &maxDstPort, &packetType, &protocol, &cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d", valid, minDstPort, maxDstPort, packetType, protocol, cpuCode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate
*
* DESCRIPTION:
*        Invalidate specific TCP/UDP destination ports range CPU code entry
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (range 0..15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rangeIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    rangeIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(devNum, rangeIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet
*
* DESCRIPTION:
*       Set user-defined CPU codes for specific ICMP type table entry.
*       12 types may be defined.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum        - Device number.
*       entryIndex    - Index of the entry (range 0..11)
*       icmpType      - ICMP Type (range 0..255)
*       packetType    - ICMP packet type:(Unicast/Multicast/
*                                        BOTH_Unicast_Multicast):
*                                CPSS_NET_ICMP_PACKET_UC_E
*                                CPSS_NET_ICMP_PACKET_MC_E
*                                CPSS_NET_ICMP_PACKET_BOTH_UC_MC_E
*       protocolStack - IPv4/IPv6/DUAL stack:
*                       CPSS_IP_PROTOCOL_IPV4_E
*                       CPSS_IP_PROTOCOL_IPV6_E
*                       CPSS_IP_PROTOCOL_IPV4V6_E
*
*       cpuCode       - User defined CPU code for this ICMP type
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       This feature enabled for packets if one of conditions are true:
*        1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*        2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet
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
    GT_U32 icmpType;
    CPSS_EXMXPM_SCT_ICMP_PACKET_TYPE_ENT packetType;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    entryIndex = (GT_U32)inArgs[1];
    icmpType = (GT_U32)inArgs[2];
    packetType = (CPSS_EXMXPM_SCT_ICMP_PACKET_TYPE_ENT)inArgs[3];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[4];
    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[5];

    /* call cpss api function */
    result = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet(devNum, entryIndex, icmpType, packetType, protocolStack, cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet
*
* DESCRIPTION:
*       Get user-defined CPU codes for specific ICMP type table entry.
*       12 types may be defined.
*
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum        - Device number.
*       entryIndex    - Index of the entry (range 0..11)
*
* OUTPUTS:
*       validPtr        - (pointer to) is the entry valid
*                           GT_FALSE - entry is not valid
*                           GT_TRUE  - entry is valid
*       icmpTypePtr     - (pointer to) ICMP Type
*       packetTypePtr   - (pointer to) ICMP packet type.
*                             The  possible correct value of *packetTypePtr:
*                                CPSS_EXMXPM_SCT_ICMP_PACKET_UC_E
*                                CPSS_EXMXPM_SCT_ICMP_PACKET_MC_E
*                                CPSS_EXMXPM_SCT_ICMP_PACKET_BOTH_UC_MC_E
*       protocolStackPtr- (pointer to) protocol stack.
*                             The  possible correct value of *protocolStackPtr:
*                                CPSS_IP_PROTOCOL_IPV4_E
*                                CPSS_IP_PROTOCOL_IPV6_E
*                                CPSS_IP_PROTOCOL_IPV4V6_E
*       cpuCodePtr      - (pointer to) User defined CPU code for this ICMP type
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - oon invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet
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
    GT_BOOL valid;
    GT_U32 icmpType;
    CPSS_EXMXPM_SCT_ICMP_PACKET_TYPE_ENT packetType;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    entryIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet(devNum, entryIndex, &valid, &icmpType, &packetType, &protocolStack, &cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d", valid, icmpType, packetType, protocolStack, cpuCode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate
*
* DESCRIPTION:
*       Invalidate specific ICMP type table entry with user defined CPU code.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum     - Device number.
*       entryIndex - Index of the entry (range 0..11)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate
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
    result = cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate(devNum, entryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet
*
* DESCRIPTION:
*       Set IP Protocol in the IP Protocol CPU Code Table  with specific
*       CPU Code. There are 8 IP Protocols may be defined.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum    - Device number.
*       index     - the index of the IP Protocol (range 0..7)
*       protocol  - IP protocol (range 0..255)
*       cpuCode   - User defined CPU code for this protocol
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       The feature enabled for packets if one of conditions are true:
*       1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*       2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctAppSpecificCpuCodeIpProtocolSet
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
    GT_U32 protocol;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    protocol = (GT_U32)inArgs[2];
    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet(devNum, index, protocol, cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet
*
* DESCRIPTION:
*       Get IP Protocol with specific CPU Code from IP Protocol CPU Code Table
*       There are 8 IP Protocols may be defined.
*
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum      - Device number.
*       index       - the index of the IP Protocol (range 0..7)
*
* OUTPUTS:
*       validPtr    - (pointer to) is the entry valid
*                       GT_FALSE - entry is not valid
*                       GT_TRUE  - entry is valid
*       protocolPtr - (pointer to) IP protocol (range 0..255)
*       cpuCodePtr  - (pointer to) User defined CPU code for this IP protocol
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctAppSpecificCpuCodeIpProtocolGet
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
    GT_BOOL valid;
    GT_U32 protocol;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet(devNum, index, &valid, &protocol, &cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", valid, protocol, cpuCode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate
*
* DESCRIPTION:
*        Invalidate entry in the IP Protocol CPU Code Table with user defined
*        CPU code.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum     - Device number.
*       entryIndex - Index of the entry (range 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate
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
    result = cpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate(devNum, entryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/****************Table cpssExMxPmSctCpuCode***************/
static GT_U32 gCPUcodeIndex;

/*******************************************************************************
* cpssExMxPmSctCpuCodeTableSet
*
* DESCRIPTION:
*       Set the "CPU code table" entry, for a specific CPU code.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code (key parameter to access the "CPU code table").
*                 use CPSS_NET_ALL_CPU_OPCODES_E as a wildcard to be used when
*                 need to set all the table entries with the same info.
*       entryInfoPtr - (pointer to) The entry information
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeTableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;
    CPSS_EXMXPM_SCT_CPU_CODE_TABLE_ENTRY_STC entryInfo;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inFields[0];
    entryInfo.tc =  (GT_U8)inFields[1];
    entryInfo.dp =  (CPSS_DP_LEVEL_ENT)inFields[2];
    entryInfo.truncate =  (GT_BOOL)inFields[3];
    entryInfo.cpuCodeRateLimiterIndex =  (GT_U32)inFields[4];
    entryInfo.cpuCodeStatRateLimitIndex =  (GT_U32)inFields[5];
    entryInfo.TrgCpuDestIndex=(GT_U32)inFields[6];
    entryInfo.DropCounterEnable=(GT_BOOL)inFields[7];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeTableSet(devNum, cpuCode, &entryInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* dsaToCpuCode
*
* DESCRIPTION:
*       Convert the DSA CPU code to the CPU code.
*
* INPUTS:
*       cpuCodeIndex - DSA CPU code index
*
* OUTPUTS:
*       cpuCodePtr - CPU code.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - None DSA code for this CPU code.
*
* COMMENTS:
*       Not all cpu codes translation implemented
*
*******************************************************************************/

static GT_STATUS dsaToCpuCode
(
    IN  GT_U32    cpuCodeIndex,
    OUT CPSS_NET_RX_CPU_CODE_ENT    *cpuCodePtr
)
{
    if ((cpuCodeIndex >= 192) && (cpuCodeIndex <= 255))
    {
        *cpuCodePtr = 500 + (cpuCodeIndex - 192);
        return GT_OK;
    }

    switch (cpuCodeIndex)
    {
        case 1:
            *cpuCodePtr = CPSS_NET_CONTROL_E;
            break;
        case 2:
            *cpuCodePtr = CPSS_NET_CONTROL_BPDU_E;
            break;
        case 3:
            *cpuCodePtr = CPSS_NET_CONTROL_SRC_DST_MAC_TRAP_E;
            break;

            /* code used as default for PCL TRAP */
        case 4:
            *cpuCodePtr = CPSS_NET_CLASS_KEY_TRAP_E;
            break;

        case 5:
            *cpuCodePtr = CPSS_NET_INTERVENTION_ARP_E;
            break;

        case 6:
            *cpuCodePtr = CPSS_NET_INTERVENTION_IGMP_E;
            break;

        case 7:
            *cpuCodePtr = CPSS_NET_INTERVENTION_SA_DA_E;
            break;

        case 8:
            *cpuCodePtr = CPSS_NET_INTERVENTION_PORT_LOCK_E;
            break;

        case 9:
            *cpuCodePtr = CPSS_NET_CLASS_KEY_MIRROR_E;
            break;

        case 11:
        case 25:
            *cpuCodePtr = CPSS_NET_UNKNOWN_UC_E;
            break;

        case 10:
            *cpuCodePtr = CPSS_NET_LOCK_PORT_MIRROR_E;
            break;

        case 12:
            *cpuCodePtr = CPSS_NET_UNREG_MC_E;
            break;

        case 0:
            *cpuCodePtr = CPSS_NET_ETH_BRIDGED_LLT_E;
            break;

        case 32:
            *cpuCodePtr = CPSS_NET_UDP_BC_MIRROR_TRAP0_E;
            break;

        case 33:
            *cpuCodePtr = CPSS_NET_UDP_BC_MIRROR_TRAP1_E;
            break;

        case 34:
            *cpuCodePtr = CPSS_NET_UDP_BC_MIRROR_TRAP2_E;
            break;

        case 35:
            *cpuCodePtr = CPSS_NET_UDP_BC_MIRROR_TRAP3_E;
            break;

        case 13:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E;
            break;
        case 26:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;
            break;
        case 27:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E;
            break;
        case 28:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E;
            break;
        case 14:
            *cpuCodePtr = CPSS_NET_IPV6_ICMP_PACKET_E;
            break;
        case 16 :
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
            break;
        case 29 :
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E;
            break;
        case 30 :
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E;
            break;
        case 31 :
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E;
            break;
        case 17 :
            *cpuCodePtr = CPSS_NET_MIRROR_IPV4_RIPV1_TO_CPU_E;
            break;
        case 18:
            *cpuCodePtr = CPSS_NET_IPV6_NEIGHBOR_SOLICITATION_E;
            break;
        case 19:
            *cpuCodePtr = CPSS_NET_IPV4_BROADCAST_PACKET_E;
            break;
        case 20:
            *cpuCodePtr = CPSS_NET_NON_IPV4_BROADCAST_PACKET_E;
            break;
        case 21:
            *cpuCodePtr = CPSS_NET_CISCO_MULTICAST_MAC_RANGE_E;
            break;
        case 22:
            *cpuCodePtr = CPSS_NET_UNREGISTERED_MULTICAST_E;
            break;
        case 23:
            *cpuCodePtr = CPSS_NET_IPV4_UNREGISTERED_MULTICAST_E;
            break;
        case 24:
            *cpuCodePtr = CPSS_NET_IPV6_UNREGISTERED_MULTICAST_E;
            break;
        case 64:
            *cpuCodePtr = CPSS_NET_ROUTED_PACKET_FORWARD_E;
            break;
        case 65:
            *cpuCodePtr = CPSS_NET_BRIDGED_PACKET_FORWARD_E;
            break;
        case 66:
            *cpuCodePtr = CPSS_NET_INGRESS_MIRRORED_TO_ANLYZER_E;
            break;
        case 67 :
            *cpuCodePtr = CPSS_NET_EGRESS_MIRRORED_TO_ANLYZER_E;
            break;
        case 68:
            *cpuCodePtr = CPSS_NET_MAIL_FROM_NEIGHBOR_CPU_E;
            break;
        case 69:
            *cpuCodePtr = CPSS_NET_CPU_TO_CPU_E;
            break;
        case 70:
            *cpuCodePtr = CPSS_NET_EGRESS_SAMPLED_E;
            break;
        case 71:
            *cpuCodePtr = CPSS_NET_INGRESS_SAMPLED_E;
            break;
        case 74:
            *cpuCodePtr = CPSS_NET_INVALID_PCL_KEY_TRAP_E;
            break;
        case 133:
            *cpuCodePtr = CPSS_NET_IP_ZERO_TTL_TRAP_E;
            break;
        case 134:
            *cpuCodePtr = CPSS_NET_IP_MTU_EXCEED_E;
            break;
        case 135:
            *cpuCodePtr = CPSS_NET_IPV6_TTL_TRAP_E;
            break;
        case 136:
            *cpuCodePtr = CPSS_NET_ILLEGAL_DIP_E;
            break;
        case 137:
            *cpuCodePtr = CPSS_NET_IP_HDR_ERROR_E;
            break;
        case 138:
            *cpuCodePtr = CPSS_NET_IP_DIP_DA_MISMATCH_E;
            break;
        case 139:
            *cpuCodePtr = CPSS_NET_IPV6_BAD_HEADER_E;
            break;
        case 140:
            *cpuCodePtr = CPSS_NET_IP_UC_SIP_SA_MISMATCH_E;
            break;
        case 141:
            *cpuCodePtr = CPSS_NET_OPTIONS_IN_IP_HDR_E;
            break;
        case 142:
            *cpuCodePtr = CPSS_NET_IPV6_EXT_HEADER_E;
            break;
        case 143:
            *cpuCodePtr = CPSS_NET_IPV6_HOP_BY_HOP_E;
            break;
        case 159:
            *cpuCodePtr = CPSS_NET_IPV6_SCOP_FAIL_E;
            break;
        case 160:
            *cpuCodePtr = CPSS_NET_ROUTE_ENTRY_TRAP_E;
            break;
        case 161:
            *cpuCodePtr = CPSS_NET_IPV4_UC_ROUTE1_TRAP_E;
            break;
        case 162:
            *cpuCodePtr = CPSS_NET_IPV4_UC_ROUTE2_TRAP_E;
            break;
        case 163:
            *cpuCodePtr = CPSS_NET_IPV4_UC_ROUTE3_TRAP_E;
            break;
        case 164:
            *cpuCodePtr = CPSS_NET_IPV4_MC_ROUTE0_TRAP_E;
            break;
        case 165:
            *cpuCodePtr = CPSS_NET_IPV4_MC_ROUTE1_TRAP_E;
            break;
        case 166:
            *cpuCodePtr = CPSS_NET_IPV4_MC_ROUTE2_TRAP_E;
            break;
        case 167:
            *cpuCodePtr = CPSS_NET_IPV4_MC_ROUTE3_TRAP_E;
            break;
        case 168:
            *cpuCodePtr = CPSS_NET_IPV6_ROUTE_TRAP_E;
            break;
        case 169:
            *cpuCodePtr = CPSS_NET_IPV6_UC_ROUTE1_TRAP_E;
            break;
        case 170:
            *cpuCodePtr = CPSS_NET_IPV6_UC_ROUTE2_TRAP_E;
            break;
        case 171:
            *cpuCodePtr = CPSS_NET_IPV6_UC_ROUTE3_TRAP_E;
            break;
        case 172:
            *cpuCodePtr = CPSS_NET_IPV6_MC_ROUTE0_TRAP_E;
            break;
        case 173:
            *cpuCodePtr = CPSS_NET_IPV6_MC_ROUTE1_TRAP_E;
            break;
        case 174:
            *cpuCodePtr = CPSS_NET_IPV6_MC_ROUTE2_TRAP_E;
            break;
        case 175:
            *cpuCodePtr = CPSS_NET_IPV6_MC_ROUTE3_TRAP_E;
            break;
        case 176:
            *cpuCodePtr = CPSS_NET_IP_UC_RPF_FAIL_E;
            break;
        case 177:
            *cpuCodePtr = CPSS_NET_RPF_CHECK_FAILED_E;
            break;
        case 178:
            *cpuCodePtr = CPSS_NET_MLL_RPF_TRAP_E;
            break;
        case 179:
            *cpuCodePtr = CPSS_NET_ARP_BC_TO_ME_E;
            break;
        case 180:
            *cpuCodePtr = CPSS_NET_MIRROR_IPV4_UC_ICMP_REDIRECT_E;
            break;
        case 181:
            *cpuCodePtr = CPSS_NET_MIRROR_IPV6_UC_ICMP_REDIRECT_E;
            break;
        case 191:
            *cpuCodePtr = CPSS_NET_PACKET_TO_VIRTUAL_ROUTER_PORT_E;
            break;
        case 36:
            *cpuCodePtr = CPSS_NET_SEC_AUTO_LEARN_UNK_SRC_TRAP_E;
            break;
        case 75:
            *cpuCodePtr = CPSS_NET_IPV4_TT_HEADER_ERROR_E;
            break;
        case 76:
            *cpuCodePtr = CPSS_NET_IPV4_TT_OPTION_FRAG_ERROR_E;
            break;
        case 77:
            *cpuCodePtr = CPSS_NET_IPV4_TT_UNSUP_GRE_ERROR_E;
            break;
        case 188:
            *cpuCodePtr = CPSS_NET_ARP_REPLY_TO_ME_E;
            break;
        case 189:
            *cpuCodePtr = CPSS_NET_CPU_TO_ALL_CPUS_E;
            break;
        case 190:
            *cpuCodePtr = CPSS_NET_TCP_SYN_TO_CPU_E;
            break;

        case 37:
            *cpuCodePtr = CPSS_NET_MIRROR_INLIF_E;
            break;
        case 46:
            *cpuCodePtr = CPSS_NET_MC_BRIDGED_PACKET_FORWARD_E;
            break;
        case 47:
            *cpuCodePtr = CPSS_NET_MC_ROUTED_PACKET_FORWARD_E;
            break;
        case 48:
            *cpuCodePtr = CPSS_NET_L2VPN_PACKET_FORWARD_E;
            break;
        case 49:
            *cpuCodePtr = CPSS_NET_MPLS_PACKET_FORWARD_E;
            break;
        case 50:
            *cpuCodePtr = CPSS_NET_UN_REGISTERD_MC_E;
            break;
        case 51:
            *cpuCodePtr = CPSS_NET_UN_KNOWN_UC_E;
            break;
        case 52:
            *cpuCodePtr = CPSS_NET_ARP_BC_EGRESS_MIRROR_E;
            break;
        case 53:
            *cpuCodePtr = CPSS_NET_VPLS_UNREGISTERED_MC_EGRESS_FILTER_E;
            break;
        case 54:
            *cpuCodePtr = CPSS_NET_VPLS_UNKWONW_UC_EGRESS_FILTER_E;
            break;
        case 55:
            *cpuCodePtr = CPSS_NET_VPLS_BC_EGRESS_FILTER_E;
            break;
        case 56:
            *cpuCodePtr = CPSS_NET_MC_CPU_TO_CPU_E;
            break;

        case 72:
            *cpuCodePtr = CPSS_NET_PCL_MIRRORED_TO_ANALYZER_E;
            break;
        case 78:
            *cpuCodePtr = CPSS_NET_VPLS_UNTAGGED_MRU_FILTER_E;
            break;
        case 79:
            *cpuCodePtr = CPSS_NET_TT_MPLS_HEADER_CHECK_E;
            break;
        case 80:
            *cpuCodePtr = CPSS_NET_TT_MPLS_TTL_EXCEED_E;
            break;
        case 81:
            *cpuCodePtr = CPSS_NET_TTI_MIRROR_E;
            break;
        case 82:
            *cpuCodePtr = CPSS_NET_MPLS_SRC_FILTERING_E;
            break;

        case 83:
            *cpuCodePtr =CPSS_NET_OAM_PDU_TRAP_E;
            break;
        case 84:
            *cpuCodePtr = CPSS_NET_IPCL_MIRROR_E;
            break;
        case 110:
            *cpuCodePtr = CPSS_NET_MPLS_MTU_EXCEED_E;
            break;
        case 111:
            *cpuCodePtr = CPSS_NET_MPLS_ZERO_TTL_TRAP_E;
            break;
        case 112:
            *cpuCodePtr = CPSS_NET_MPLS_TTL1_EXCEEDED_E;
            break;
        case 113:
            *cpuCodePtr = CPSS_NET_NHLFE_ENTRY_TRAP_E;
            break;
        case 114:
            *cpuCodePtr = CPSS_NET_MPLS_ROUTE_ENTRY_1_E;
            break;
        case 115:
            *cpuCodePtr = CPSS_NET_MPLS_ROUTE_ENTRY_2_E;
            break;
        case 116:
            *cpuCodePtr = CPSS_NET_MPLS_ROUTE_ENTRY_3_E;
            break;
        case 117:
            *cpuCodePtr = CPSS_NET_INVALID_MPLS_IF_E;
            break;

        case 144:
            *cpuCodePtr = CPSS_NET_IPV4_TTL1_EXCEEDED_E;
            break;
        case 145:
            *cpuCodePtr = CPSS_NET_IPV6_HOPLIMIT1_EXCEED_E;
            break;

        default :
            /* not supported ?? */
            *cpuCodePtr = CPSS_NET_FIRST_UNKNOWN_HW_CPU_CODE_E + cpuCodeIndex;
            break;
    }


    return GT_OK;

}
/*******************************************************************************
* cpssExMxPmSctCpuCodeTableGet
*
* DESCRIPTION:
*       Get the "CPU code table" entry, for a specific CPU code.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code (key parameter to access the "CPU code table").
*                 use CPSS_NET_ALL_CPU_OPCODES_E as a wildcard to be used when
*                 need to set all the table entries with the same info.
*
*
* OUTPUTS:
*       entryInfoPtr - (pointer to) The entry information
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*        Wrapper for getting first element
*
*******************************************************************************/

static CMD_STATUS wrCpssExMxPmSctCpuCodeTableGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;
    CPSS_EXMXPM_SCT_CPU_CODE_TABLE_ENTRY_STC entryInfo;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

   /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gCPUcodeIndex = 0;

    /*use func to go over all  CPU codes only. DSA index = 0-255*/
    result = dsaToCpuCode(gCPUcodeIndex, &cpuCode);

    if(result !=GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeTableGet(devNum, cpuCode, &entryInfo);
    if((result != GT_OK) && (result != GT_NOT_SUPPORTED))
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    if(result != GT_NOT_SUPPORTED)
    {
        inFields[0] = cpuCode;
        inFields[1] = entryInfo.tc;
        inFields[2] = entryInfo.dp;
        inFields[3] = entryInfo.truncate;
        inFields[4] = entryInfo.cpuCodeRateLimiterIndex;
        inFields[5] = entryInfo.cpuCodeStatRateLimitIndex;
        inFields[6] = entryInfo.TrgCpuDestIndex;
        inFields[7] = entryInfo.DropCounterEnable;
    }
    else
    {
        inFields[0] = 0xFFFF;
        inFields[1] = 0;
        inFields[2] = 0;
        inFields[3] = 0;
        inFields[4] = 0;
        inFields[5] = 0;
        inFields[6] = 0;
        inFields[7] = 0;
        result = GT_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                               inFields[3], inFields[4], inFields[5],
                               inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCpuCodeTableGet
*
* DESCRIPTION:
*       Get the "CPU code table" entry, for a specific CPU code.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code (key parameter to access the "CPU code table").
*                 use CPSS_NET_ALL_CPU_OPCODES_E as a wildcard to be used when
*                 need to set all the table entries with the same info.
*
*
* OUTPUTS:
*       entryInfoPtr - (pointer to) The entry information
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*        Wrapper for getting next element
*
*******************************************************************************/

static CMD_STATUS wrCpssExMxPmSctCpuCodeTableGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;
    CPSS_EXMXPM_SCT_CPU_CODE_TABLE_ENTRY_STC entryInfo;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

   /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gCPUcodeIndex++;

    if(gCPUcodeIndex > 255)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /*use prv func to go over all  CPU codes only. DSA index = 0-255*/
    result = dsaToCpuCode(gCPUcodeIndex, &cpuCode);

    if(result !=GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeTableGet(devNum, cpuCode, &entryInfo);
    if((result != GT_OK) && (result != GT_NOT_SUPPORTED))
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    if(result != GT_NOT_SUPPORTED)
    {
        inFields[0] = cpuCode;
        inFields[1] = entryInfo.tc;
        inFields[2] = entryInfo.dp;
        inFields[3] = entryInfo.truncate;
        inFields[4] = entryInfo.cpuCodeRateLimiterIndex;
        inFields[5] = entryInfo.cpuCodeStatRateLimitIndex;
        inFields[6] = entryInfo.TrgCpuDestIndex;
        inFields[7] = entryInfo.DropCounterEnable;
    }
    else
    {
        inFields[0] = 0xFFFF;
        inFields[1] = 0;
        inFields[2] = 0;
        inFields[3] = 0;
        inFields[4] = 0;
        inFields[5] = 0;
        inFields[6] = 0;
        inFields[7] = 0;
        result = GT_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                               inFields[3], inFields[4], inFields[5],
                               inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet
*
* DESCRIPTION:
*       Set the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum            - Device number.
*       windowResolution - The TO CPU window size resolution -- this field is
*                          the number of NanoSeconds.Range is 0x0-0x7FF
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          microseconds, steps of 160 nanoseconds)
*                          So this function will round the value to the nearest
*                          PP's option.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_OUT_OF_RANGE  - on windowResolution too large(depends on system
*                           clock)
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 windowResolution;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    windowResolution = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet(devNum, windowResolution);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet
*
* DESCRIPTION:
*       Get the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum             - Device number.
*
*
* OUTPUTS:
*       windowResolutionPtr -  (point to) - The TO CPU window size resolution.
*                               This field is the number of NanoSeconds.
*                               Range is 0x0-0x7FF
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          microseconds, steps of 160 nanoseconds)
*                          So this function will round the value to the nearest
*                          PP's option.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_OUT_OF_RANGE  - on windowResolution too large(depend on system clock)
*       GT_FAIL          - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 windowResolution;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet(devNum, &windowResolution);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", windowResolution);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCpuCodeRateLimiterTableSet
*
* DESCRIPTION:
*       Configure rate limiter window size and packets limit forwarded to CPU
*       per rate limiter index.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum            - Device number.
*       rateLimiterIndex  -  CPU Code Rate Limiter Index
*                           range 1..255.
*       windowSize        - window size for this Rate Limiter in steps of
*                           Rate Limiter Window Resolution set by
*                       cpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet(...)
*                           range 0..0xFFF (12 bits)
*       pktLimit          - number of packets allowed to be sent to CPU
*                           during within the configured windowSize
*                           range 0..0xFFFFF (20 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_OUT_OF_RANGE  - on pktLimit >= 0x100000 or windowSize >= 0x1000
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       The defined CPU Code Table Entry points to this table on specific
*       rate limiter. (by field CPUCodeRateLimiterIndex)
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeRateLimiterTableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rateLimiterIndex;
    GT_U32 windowSize;
    GT_U32 pktLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    rateLimiterIndex = (GT_U32)inArgs[1];
    windowSize = (GT_U32)inArgs[2];
    pktLimit = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeRateLimiterTableSet(devNum, rateLimiterIndex, windowSize, pktLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCpuCodeRateLimiterTableGet
*
* DESCRIPTION:
*       Get the configurated rate limiter window size and packets limit
*       forwarded to CPU per rate limiter index.
*
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum           - Device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           range 1..255.
*
* OUTPUTS:
*       windowSizePtr    - (pointer to) window size for this Rate Limiter in
*                           steps of Rate Limiter Window Resolution set by
*                       cpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet(...)
*                           range 0..0xFFF (12 bits)
*       pktLimitPtr      - (pointer to) number of packets allowed to be sent to
*                           CPU during within the configured windowSize
*                           range 0..0xFFFFF (20 bits)
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_FAIL          - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeRateLimiterTableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rateLimiterIndex;
    GT_U32 windowSize;
    GT_U32 pktLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    rateLimiterIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeRateLimiterTableGet(devNum, rateLimiterIndex, &windowSize, &pktLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", windowSize, pktLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCpuCodeRateLimiterPacketCntrGet
*
* DESCRIPTION:
*       Gets the number of packets transferred to the CPU during the current
*       window size. This is per rate limiter value. The value is 20 bits and
*       its automatically cleared when when windows ends.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum           - Device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           range 1..255.
*
* OUTPUTS:
*       packetsCntrPtr    - (pointer to) Number of packets transffered to the
*                           CPU during current window time.
*                           range 0..0xFFFFF (20 bits)
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_FAIL          - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeRateLimiterPacketCntrGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rateLimiterIndex;
    GT_U32 packetsCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    rateLimiterIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeRateLimiterPacketCntrGet(devNum, rateLimiterIndex, &packetsCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", packetsCntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCpuCodeRateLimiterDropCntrGet
*
* DESCRIPTION:
*       Get the global CPU code rate limiter drop counter value.
*       This counter counts the number of packets transffered to the
*       CPU and dropped due to any of the CPU code limiter.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum           - Device number.
*
* OUTPUTS:
*       valuePtr         - (pointer to) CPU code limiter drop counter value
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_FAIL          - on error
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeRateLimiterDropCntrGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 value;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeRateLimiterDropCntrGet(devNum, &value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", value);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCpuCodeStatisticalRateLimitsTableSet
*
* DESCRIPTION:
*       Set the "Statistical Rate Limits Table" entry.
*
*          This mechanism is based on a hardware-based 32-bit pseudo-random
*           generator (PRNG). The PRNG generates evenly-distributed 32-bit
*           numbers ranging from 1 through 0xFFFFFFFF-1
*           Each defined CPU Code Table Entry may point to one of the entries
*           set here. (by field CpuCodeStatRateLimitIndex)
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum  - device number
*       index - index into the Statistical Rate Limits Table (range 0..31)
*       statisticalRateLimit - The statistical rate limit compared to the
*                              32-bit pseudo-random generator (PRNG).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*           For all packets to be forwarded to the CPU, set this field to
*           0xFFFFFFFF.
*           For all packets to be dropped, set this field to 0x0.
************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeStatisticalRateLimitsTableSet
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
    GT_U32 statisticalRateLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    statisticalRateLimit = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableSet(devNum, index, statisticalRateLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet
*
* DESCRIPTION:
*       Get the "Statistical Rate Limits Table" entry.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum  - device number
*       index   - index into the Statistical Rate Limits Table (range 0..31)
*
*
* OUTPUTS:
*       statisticalRateLimitPtr - (point to) statistical rate limit compared
*                                 to the 32-bit pseudo-random generator (PRNG).
*       NOTE:
*           For all packets to be forwarded to the CPU, set this field to
*           0xFFFFFFFF.
*           For all packets to be dropped, set this field to 0x0.
*
*           This mechanism is based on a hardware-based 32-bit pseudo-random
*           generator (PRNG). The PRNG generates evenly-distributed 32-bit
*           numbers ranging from 1 through 0xFFFFFFFF-1
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet
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
    GT_U32 statisticalRateLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet(devNum, index, &statisticalRateLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", statisticalRateLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctPortDuplicateToCpuEnableSet
*
* DESCRIPTION:
*       Packets received from the port with CPU as the
*       duplication target, will be duplicated to the CPU or just
*       forwarded to their original target. It is applicable per ingress port.
*
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  -
*               GT_FALSE = Packets received from the port that are duplicated
*                          and their duplication target is the CPU, (mirror, STC
*                          and ingress mirroring to analyzer port when the
*                          analyzer port is the CPU) are not duplicated and are
*                          only forwarded to their original target.
*
*               GT_TRUE = Packets received from the port that are duplicated and
*                         their duplication target is the CPU, (mirror, STC and
*                         ingress mirroring to analyzer port when the analyzer
*                         port is the CPU) are duplicated and are only forwarded
*                         to their original target and to the CPU.
*
* OUTPUTS:
*       None;
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
*
* COMMENTS:
*       NOTE : port may be "CPU port"
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctPortDuplicateToCpuEnableSet
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
    result = cpssExMxPmSctPortDuplicateToCpuEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctPortDuplicateToCpuEnableGet
*
* DESCRIPTION:
*       Get per ingress port state of Duplication of Packets to CPU :
*       if Packets received from the port with CPU as the
*       duplication target, will be duplicated to the CPU or just
*       forwarded to their original target.
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
*
* OUTPUTS:
*       enablePtr  - pointer to DuplicateToCpu state. *enablePtr could be :
 *              GT_FALSE = Packets received from the port that are duplicated
*                          and their duplication target is the CPU, (mirror, STC
*                          and ingress mirroring to analyzer port when the
*                          analyzer port is the CPU) are not duplicated and are
*                          only forwarded to their original target.
*
*               GT_TRUE = Packets received from the port that are duplicated and
*                         their duplication target is the CPU, (mirror, STC and
*                         ingress mirroring to analyzer port when the analyzer
*                         port is the CPU) are duplicated and are only forwarded
*                         to their original target and to the CPU.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
*
* COMMENTS:
*       port may be "CPU port"
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctPortDuplicateToCpuEnableGet
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
    result = cpssExMxPmSctPortDuplicateToCpuEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctDuplicateEnableSet
*
* DESCRIPTION:
*       Enable descriptor duplication (mirror, STC and ingress mirroring
*       to analyzer port when the analyzer port is the CPU), Or Disable any kind
*       of duplication.
*
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum - device number
*       enable -
*               GT_FALSE = Disable any kind of duplication
*
*               GT_TRUE = enable descriptor duplication (mirror, STC and ingress
*                         mirroring to analyzer port when the analyzer port is
*                         the CPU).
*
* OUTPUTS:
*       None;
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise.
*
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctDuplicateEnableSet
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
    result = cpssExMxPmSctDuplicateEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctDuplicateEnableGet
*
* DESCRIPTION:
*       Get descriptor duplication enable/disable status.
*
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum - device number
*
*
* OUTPUTS:
*       enablePtr - pointer to descriptor duplication status. It can be :
*               GT_FALSE = Disable any kind of duplication
*
*               GT_TRUE = enable descriptor duplication (mirror, STC and ingress
*                         mirroring to analyzer port when the analyzer port is
*                         the CPU)
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise.
*
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctDuplicateEnableGet
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
    result = cpssExMxPmSctDuplicateEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCpuCodeTrgDevPortTableSet
*
* DESCRIPTION:
*       Set the "CPU target device/port" table entry. These entries are
*       pointed from the CPU code table
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum      - device number
*       index       - Index of entry in table [0-31]
*       trgDevice   - Target device. Range is 0...127
*       trgPort     - Target port. Range is 0...63
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       None
************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeTrgDevPortTableSet
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
    GT_U8 trgDevice;
    GT_U8 trgPort;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    trgDevice = (GT_U8)inArgs[2];
    trgPort = (GT_U8)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeTrgDevPortTableSet(devNum, index, trgDevice, trgPort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCpuCodeTrgDevPortTableGet
*
* DESCRIPTION:
*       Get the "CPU target device/port" table entry. These entries are
*       pointed from the CPU code table
*
* APPLICABLE DEVICES: EXMXPM devices
*
* INPUTS:
*       devNum      - device number
*       index       - Index of entry in table [0-31]
*
* OUTPUTS:
*       trgDevicePtr   - pointer to Target device
*       trgPortPtr     - pointer to Target port
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*       None
************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeTrgDevPortTableGet
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
    GT_U8 trgDevice;
    GT_U8 trgPort;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeTrgDevPortTableGet(devNum, index, &trgDevice, &trgPort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", trgDevice, trgPort);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet
*
* DESCRIPTION:
*       Configure rate limiter window countdown per rate limiter index.
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*       devNum            - Device number.
*       rateLimiterIndex  - Rate Limiter Entry Index range 1..255,
*                           which may be pointed by multiple CPU code table entries.
*       windowCountdown   - window countdown for this Rate Limiter Index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_OUT_OF_RANGE  - on windowCountdown >= 0x1000
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Setting initial window time will affect the window countdown counter
*       only when it reaches zero.
*       Setting window countdown directly will affect immediately.
*       When it will reach zero, it will regularly be loaded from
*       the initial window time field.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rateLimiterIndex;
    GT_U32 windowCountdown;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    rateLimiterIndex = (GT_U32)inArgs[1];
    windowCountdown  = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet(devNum,
                                                               rateLimiterIndex,
                                                               windowCountdown);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet
*
* DESCRIPTION:
*       Get the configurated rate limiter window countdown per rate limiter index.
*
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*       devNum            - Device number.
*       rateLimiterIndex  - Rate Limiter Entry Index range 1..255,
*                           which may be pointed by multiple CPU code table entries.
*
* OUTPUTS:
*       windowCountdownPtr  - (pointer to) window countdown for this Rate Limiter Index.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_FAIL          - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rateLimiterIndex;
    GT_U32 windowCountdown;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    rateLimiterIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet(devNum,
                                                               rateLimiterIndex,
                                                               &windowCountdown);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", windowCountdown);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet
*
* DESCRIPTION:
*       Sets an additional number of packets beyond the <Packet Count Threshold>
*       that are received during the window period before the interrupt for this
*        rate limiter is raised.

* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum            - Device number.
*       rateLimiterIndex  - Rate Limiter Entry Index range 1..255,
*                           which may be pointed by multiple CPU code table entries.
*       pktThreshold      - The value to be loaded to <Packet Countdown> the first
*                           time it reaches zero in the window period. An interrupt
*                           is asserted only when the <PacketCountdown> reaches zero again.
*                           This allows the Interrupt to be deferred for this number
*                           of packets after the <Packet Count Threshold> has been exceeded.
*                           range 1..0xFFFFF (20 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad devNum or rateLimiterIndex > 255 or rateLimiterIndex == 0
*       GT_OUT_OF_RANGE          - on pktThreshold > 0xFFFFF or pktThreshold == 0
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rateLimiterIndex;
    GT_U32 pktThreshold;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    rateLimiterIndex = (GT_U32)inArgs[1];
    pktThreshold  = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet(devNum,
                                                                             rateLimiterIndex,
                                                                             pktThreshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet
*
* DESCRIPTION:
*       Gets an additional number of packets beyond the <Packet Count Threshold>
*       that are received during the window period before the interrupt for this
*        rate limiter is raised.

* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum            - Device number.
*       rateLimiterIndex  - Rate Limiter Entry Index range 1..255,
*                           which may be pointed by multiple CPU code table entries.
*
* OUTPUTS:
*       pktThresholdPtr   - Pointer to the value to be loaded to <Packet Countdown>
*                           the first time it reaches zero in the window period. An interrupt
*                           is asserted only when the <PacketCountdown> reaches zero again.
*                           This allows the Interrupt to be deferred for this number
*                           of packets after the <Packet Count Threshold> has been exceeded.
*                           range 1..0xFFFFF (20 bits)
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad devNum or rateLimiterIndex > 255 or rateLimiterIndex == 0
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rateLimiterIndex;
    GT_U32 pktThreshold;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    rateLimiterIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet(devNum,
                                                                             rateLimiterIndex,
                                                                             &pktThreshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", pktThreshold);

    return CMD_OK;
}

/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmSctPortIeeeReservedMcTrapEnableSet",
         &wrCpssExMxPmSctPortIeeeReservedMcTrapEnableSet,
         3, 0},
        {"cpssExMxPmSctPortIeeeReservedMcTrapEnableGet",
         &wrCpssExMxPmSctPortIeeeReservedMcTrapEnableGet,
         2, 0},
        {"cpssExMxPmSctPortIeeeReservedMcProfileSet",
         &wrCpssExMxPmSctPortIeeeReservedMcProfileSet,
         3, 0},
        {"cpssExMxPmSctPortIeeeReservedMcProfileGet",
         &wrCpssExMxPmSctPortIeeeReservedMcProfileGet,
         2, 0},
        {"cpssExMxPmSctIeeeReservedMcProtocolCmdSet",
         &wrCpssExMxPmSctIeeeReservedMcProtocolCmdSet,
         5, 0},
        {"cpssExMxPmSctIeeeReservedMcProtocolCmdGet",
         &wrCpssExMxPmSctIeeeReservedMcProtocolCmdGet,
         4, 0},
        {"cpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet",
         &wrCpssExMxPmSctIeeeReservedMcProtocolCpuCodeSet,
         5, 0},
        {"cpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet",
         &wrCpssExMxPmSctIeeeReservedMcProtocolCpuCodeGet,
         4, 0},
        {"cpssExMxPmSctIeeeReservedMcTrappedLearnEnableSet",
         &wrCpssExMxPmSctIeeeReservedMcTrappedLearnEnableSet,
         2, 0},
        {"cpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet",
         &wrCpssExMxPmSctIeeeReservedMcTrappedLearnEnableGet,
         1, 0},
        {"cpssExMxPmSctCiscoL2McCmdSet",
         &wrCpssExMxPmSctCiscoL2McCmdSet,
         2, 0},
        {"cpssExMxPmSctCiscoL2McCmdGet",
         &wrCpssExMxPmSctCiscoL2McCmdGet,
         1, 0},
        {"cpssExMxPmSctIgmpSnoopModeSet",
         &wrCpssExMxPmSctIgmpSnoopModeSet,
         2, 0},
        {"cpssExMxPmSctIgmpSnoopModeGet",
         &wrCpssExMxPmSctIgmpSnoopModeGet,
         2, 0},
        {"cpssExMxPmSctIcmpV6MsgTypeSet",
         &wrCpssExMxPmSctIcmpV6MsgTypeSet,
         4, 0},
        {"cpssExMxPmSctIcmpV6MsgTypeGet",
         &wrCpssExMxPmSctIcmpV6MsgTypeGet,
         2, 0},
        {"cpssExMxPmSctUdpBcDestPortCfgSet",
         &wrCpssExMxPmSctUdpBcDestPortCfgSet,
         5, 0},
        {"cpssExMxPmSctUdpBcDestPortCfgGet",
         &wrCpssExMxPmSctUdpBcDestPortCfgGet,
         2, 0},
        {"cpssExMxPmSctUdpBcDestPortCfgInvalidate",
         &wrCpssExMxPmSctUdpBcDestPortCfgInvalidate,
         2, 0},
        {"cpssExMxPmSctArpBcastCmdSet",
         &wrCpssExMxPmSctArpBcastCmdSet,
         2, 0},
        {"cpssExMxPmSctArpBcastCmdGet",
         &wrCpssExMxPmSctArpBcastCmdGet,
         1, 0},
        {"cpssExMxPmSctArpToMeCmdSet",
         &wrCpssExMxPmSctArpToMeCmdSet,
         2, 0},
        {"cpssExMxPmSctArpToMeCmdGet",
         &wrCpssExMxPmSctArpToMeCmdGet,
         1, 0},
        {"cpssExMxPmSctArpBcMirrorEgressEnableSet",
         &wrCpssExMxPmSctArpBcMirrorEgressEnableSet,
         2, 0},
        {"cpssExMxPmSctArpBcMirrorEgressEnableGet",
         &wrCpssExMxPmSctArpBcMirrorEgressEnableGet,
         1, 0},
        {"cpssExMxPmSctIpV6SolicitedNodeMcPrefixSet",
         &wrCpssExMxPmSctIpV6SolicitedNodeMcPrefixSet,
         1, 2},
        {"cpssExMxPmSctIpV6SolicitedNodeMcPrefixGetFirst",
         &wrCpssExMxPmSctIpV6SolicitedNodeMcPrefixGetFirst,
         1, 0},
        {"cpssExMxPmSctIpV6SolicitedNodeMcPrefixGetNext",
         &wrCpssExMxPmSctIpV6SolicitedNodeMcPrefixGetNext,
         1, 0},
        {"cpssExMxPmSctIpV6SolicitedCmdSet",
         &wrCpssExMxPmSctIpV6SolicitedCmdSet,
         2, 0},
        {"cpssExMxPmSctIpV6SolicitedCmdGet",
         &wrCpssExMxPmSctIpV6SolicitedCmdGet,
         1, 0},
        {"cpssExMxPmSctIpLinkLocalMirrorToCpuEnableSet",
         &wrCpssExMxPmSctIpLinkLocalMirrorToCpuEnableSet,
         3, 0},
        {"cpssExMxPmSctIpLinkLocalMirrorToCpuEnableGet",
         &wrCpssExMxPmSctIpLinkLocalMirrorToCpuEnableGet,
         2, 0},
        {"cpssExMxPmSctIpLinkLocalProtoMirrorToCpuEnableSet",
         &wrCpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableSet,
         4, 0},
        {"cpssExMxPmSctIpLinkLocalProtoMirrorToCpuEnableGet",
         &wrCpssExMxPmSctIpLinkLocalProtocolMirrorToCpuEnableGet,
         3, 0},
        {"cpssExMxPmSctIpLinkLocalProtocolCpuCodeSet",
         &wrCpssExMxPmSctIpLinkLocalProtocolCpuCodeSet,
         4, 0},
        {"cpssExMxPmSctIpLinkLocalProtocolCpuCodeGet",
         &wrCpssExMxPmSctIpLinkLocalProtocolCpuCodeGet,
         3, 0},
        {"cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableSet",
         &wrCpssExMxPmSctAppSpecificCpuCodeTcpSynEnableSet,
         2, 0},
        {"cpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet",
         &wrCpssExMxPmSctAppSpecificCpuCodeTcpSynEnableGet,
         1, 0},
        {"cpssExMxPmSctAppSpecCpuCodeTcpUdpDestPortRangeSet",
         &wrCpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeSet,
         7, 0},
        {"cpssExMxPmSctAppSpecCpuCodeTcpUdpDestPortRangeGet",
         &wrCpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeGet,
         2, 0},
        {"cpssExMxPmSctAppSpecCpuCodeTcpUdpDestPortRangeInv",
         &wrCpssExMxPmSctAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate,
         2, 0},
        {"cpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet",
         &wrCpssExMxPmSctAppSpecificCpuCodeIcmpTypeSet,
         6, 0},
        {"cpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet",
         &wrCpssExMxPmSctAppSpecificCpuCodeIcmpTypeGet,
         2, 0},
        {"cpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate",
         &wrCpssExMxPmSctAppSpecificCpuCodeIcmpTypeInvalidate,
         2, 0},
        {"cpssExMxPmSctAppSpecificCpuCodeIpProtocolSet",
         &wrCpssExMxPmSctAppSpecificCpuCodeIpProtocolSet,
         4, 0},
        {"cpssExMxPmSctAppSpecificCpuCodeIpProtocolGet",
         &wrCpssExMxPmSctAppSpecificCpuCodeIpProtocolGet,
         2, 0},
        {"cpssExMxPmSctAppSpecCpuCodeIpProtoInvld",
         &wrCpssExMxPmSctAppSpecificCpuCodeIpProtocolInvalidate,
         2, 0},

        {"cpssExMxPmSctCpuCodeTableSet",
         &wrCpssExMxPmSctCpuCodeTableSet,
         1, 8},
        {"cpssExMxPmSctCpuCodeTableGetFirst",
         &wrCpssExMxPmSctCpuCodeTableGetFirst,
         1, 0},
         {"cpssExMxPmSctCpuCodeTableGetNext",
         &wrCpssExMxPmSctCpuCodeTableGetNext,
         1, 0},

        {"cpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet",
         &wrCpssExMxPmSctCpuCodeRateLimiterWindowResolutionSet,
         2, 0},
        {"cpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet",
         &wrCpssExMxPmSctCpuCodeRateLimiterWindowResolutionGet,
         1, 0},
        {"cpssExMxPmSctCpuCodeRateLimiterTableSet",
         &wrCpssExMxPmSctCpuCodeRateLimiterTableSet,
         4, 0},
        {"cpssExMxPmSctCpuCodeRateLimiterTableGet",
         &wrCpssExMxPmSctCpuCodeRateLimiterTableGet,
         2, 0},
        {"cpssExMxPmSctCpuCodeRateLimiterPacketCntrGet",
         &wrCpssExMxPmSctCpuCodeRateLimiterPacketCntrGet,
         2, 0},
        {"cpssExMxPmSctCpuCodeRateLimiterDropCntrGet",
         &wrCpssExMxPmSctCpuCodeRateLimiterDropCntrGet,
         1, 0},
        {"cpssExMxPmSctCpuCodeStatisticalRateLimitsTableSet",
         &wrCpssExMxPmSctCpuCodeStatisticalRateLimitsTableSet,
         3, 0},
        {"cpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet",
         &wrCpssExMxPmSctCpuCodeStatisticalRateLimitsTableGet,
         2, 0},
        {"cpssExMxPmSctPortDuplicateToCpuEnableSet",
         &wrCpssExMxPmSctPortDuplicateToCpuEnableSet,
         3, 0},
        {"cpssExMxPmSctPortDuplicateToCpuEnableGet",
         &wrCpssExMxPmSctPortDuplicateToCpuEnableGet,
         2, 0},
        {"cpssExMxPmSctDuplicateEnableSet",
         &wrCpssExMxPmSctDuplicateEnableSet,
         2, 0},
        {"cpssExMxPmSctDuplicateEnableGet",
         &wrCpssExMxPmSctDuplicateEnableGet,
         1, 0},
        {"cpssExMxPmSctCpuCodeTrgDevPortTableSet",
         &wrCpssExMxPmSctCpuCodeTrgDevPortTableSet,
         4, 0},
        {"cpssExMxPmSctCpuCodeTrgDevPortTableGet",
         &wrCpssExMxPmSctCpuCodeTrgDevPortTableGet,
         2, 0},
        {"cpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet",
         &wrCpssExMxPmSctCpuCodeRateLimiterWindowCountdownSet,
         3, 0},
        {"cpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet",
         &wrCpssExMxPmSctCpuCodeRateLimiterWindowCountdownGet,
         1, 0},
        {"cpssExMxPmSctCpuCodeRateLimiterPktIntCntThrholdSet",
         &wrCpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdSet,
         3, 0},
        {"cpssExMxPmSctCpuCodeRateLimiterPktIntCntThrholdGet",
         &wrCpssExMxPmSctCpuCodeRateLimiterPacketInterruptCountThresholdGet,
         1, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmSct
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
GT_STATUS cmdLibInitCpssExMxPmSct
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

