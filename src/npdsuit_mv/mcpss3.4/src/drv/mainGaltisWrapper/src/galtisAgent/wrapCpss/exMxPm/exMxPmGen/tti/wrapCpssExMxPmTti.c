/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmTti.c
*
* DESCRIPTION:
*       Wrappers for  cpssExMxPmTti.c
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
#include <cpss/generic/tunnel/cpssGenTunnelTypes.h>
#include <cpss/exMxPm/exMxPmGen/tti/cpssExMxPmTti.h>



/*******************************************************************************
* cpssExMxPmTtiLookupModeSet
*
* DESCRIPTION:
*       This function sets the lookup mode for all key types (lookup mode
*       defines if the lookup would take place over internal or external TCAM).
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       mode          - lookup mode; valid values:
*                           CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E
*                           CPSS_EXMXPM_TTI_LOOKUP_MODE_EXT_TCAM_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or mode
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiLookupModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_LOOKUP_MODE_ENT mode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_TTI_LOOKUP_MODE_ENT)inArgs[1];

    /* call cpss api function */
    /*result = cpssExMxPmTtiLookupModeSet(devNum, mode);*/
    /* will work only for port 0 and ipv4 */
    result = cpssExMxPmTtiPortLookupModeSet(devNum,0,CPSS_EXMXPM_TTI_KEY_IPV4_E,mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiLookupModeGet
*
* DESCRIPTION:
*       This function gets the lookup mode for all key types (lookup mode
*       defines if the lookup would take place over internal or external TCAM).
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       modePtr       - lookup mode; valid values:
*                           CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E
*                           CPSS_EXMXPM_TTI_LOOKUP_MODE_EXT_TCAM_E
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiLookupModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_LOOKUP_MODE_ENT mode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    /*result = cpssExMxPmTtiLookupModeGet(devNum, &mode);*/
    /* will work only for port 0 and ipv4 */
    result = cpssExMxPmTtiPortLookupModeGet(devNum,0,CPSS_EXMXPM_TTI_KEY_IPV4_E,&mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiLookupConfigSet
*
* DESCRIPTION:
*       This function sets the lookup configuration.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_ETH_E
*       configPtr     - points to lookup configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiLookupConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    CPSS_EXMXPM_TTI_LOOKUP_CONFIG_STC config;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    keyType=(CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[1];
    config.actionType=(CPSS_EXMXPM_TTI_ACTION_TYPE_ENT)inArgs[2];
    /*config.externalTcamProfile=(GT_U32)inArgs[3];*/
    config.pclId=(GT_U32)inArgs[4];
    /* call cpss api function */
    /*result = cpssExMxPmTtiLookupConfigSet(devNum, keyType, &config);*/
    /* will work only for port 0 */
    result = cpssExMxPmTtiPortLookupConfigSet(devNum,0,keyType,&config);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiLookupConfigGet
*
* DESCRIPTION:
*       This function gets the lookup configuration.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_ETH_E
*
* OUTPUTS:
*       configPtr     - points to lookup configuration
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or key type
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiLookupConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    CPSS_EXMXPM_TTI_LOOKUP_CONFIG_STC config;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    keyType=(CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    /*result = cpssExMxPmTtiLookupConfigGet(devNum, keyType, &config);*/
    /* will work only for port 0 */
    result = cpssExMxPmTtiPortLookupConfigGet(devNum,0,keyType,&config);

    /* pack output arguments to galtis string */
    /*galtisOutput(outArgs, result, "%d%d%d",config.actionType,config.externalTcamProfile,config.pclId);*/
    galtisOutput(outArgs, result, "%d%d",config.actionType,config.pclId);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_ETH_E
*       macMode       - MAC mode to use; valid values:
*                           CPSS_EXMXPM_TTI_MAC_MODE_DA_E
*                           CPSS_EXMXPM_TTI_MAC_MODE_SA_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiMacModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    CPSS_EXMXPM_TTI_MAC_MODE_ENT macMode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[1];
    macMode = (CPSS_EXMXPM_TTI_MAC_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmTtiMacModeSet(devNum, keyType, macMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_ETH_E
*
* OUTPUTS:
*       macModePtr    - MAC mode to use; valid values:
*                           CPSS_EXMXPM_TTI_MAC_MODE_DA_E
*                           CPSS_EXMXPM_TTI_MAC_MODE_SA_E
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or key type
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiMacModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    CPSS_EXMXPM_TTI_MAC_MODE_ENT macMode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTtiMacModeGet(devNum, keyType, &macMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", macMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiIpv4McEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for IPv4 multicast
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE: enable TTI lookup for IPv4 MC
*                       GT_FALSE: disable TTI lookup for IPv4 MC
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiIpv4McEnableSet
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
    result = cpssExMxPmTtiIpv4McEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiIpv4McEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of TTI lookup for
*       IPv4 multicast (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TTI lookup for IPv4 MC
*                       GT_TRUE: TTI lookup for IPv4 MC enabled
*                       GT_FALSE: TTI lookup for IPv4 MC disabled
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiIpv4McEnableGet
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
    result = cpssExMxPmTtiIpv4McEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiIpv4GreEthTypeSet
*
* DESCRIPTION:
*       This function sets the IPv4 GRE protocol Ethernet type. Two Ethernet
*       types are supported, used by TTI for Ethernet over GRE tunnels
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type; valid values:
*                           CPSS_TUNNEL_GRE_ETHER_TYPE_0_E
*                           CPSS_TUNNEL_GRE_ETHER_TYPE_1_E
*       ethType       - Ethernet type value (range 16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiIpv4GreEthTypeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_TUNNEL_GRE_ETHER_TYPE_ENT greTunnelType;
    GT_U32 ethType;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    greTunnelType = (CPSS_TUNNEL_GRE_ETHER_TYPE_ENT)inArgs[1];
    ethType = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmTtiIpv4GreEthTypeSet(devNum, greTunnelType, ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiIpv4GreEthTypeGet
*
* DESCRIPTION:
*       This function gets the IPv4 GRE protocol Ethernet type. Two Ethernet
*       types are supported, used by TTI for Ethernet over GRE tunnels
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type; valid values:
*                           CPSS_TUNNEL_GRE_ETHER_TYPE_0_E
*                           CPSS_TUNNEL_GRE_ETHER_TYPE_1_E
*
* OUTPUTS:
*       ethTypePtr    - points to Ethernet type value
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or GRE tunnel type
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiIpv4GreEthTypeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_TUNNEL_GRE_ETHER_TYPE_ENT greTunnelType;
    GT_U32 ethType;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    greTunnelType = (CPSS_TUNNEL_GRE_ETHER_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTtiIpv4GreEthTypeGet(devNum, greTunnelType, &ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ethType);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI MacToMe filter at the TTI
*       trigger phase (incoming packets would bypass the TTI lookup if their
*       DST MAC & VLAN doesn't fall in the MacToMe/VlanToMe range).
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*       enable        - GT_TRUE: enable MacToMe check
*                       GT_FALSE: disable MacToMe check
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or key type
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiMacToMeEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    /*result = cpssExMxPmTtiMacToMeEnableSet(devNum, keyType, enable);*/
    /* will work only on port 0 */
    result = cpssExMxPmTtiPortMacToMeEnableSet(devNum,0,keyType,enable);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of  the TTI
*       MacToMe filter.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*
* OUTPUTS:
*       enablePtr     - points to enable/disable MacToMe check
*                       GT_TRUE: MacToMe check is enabled
*                       GT_FALSE: MacToMe check is disabled
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or key type
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiMacToMeEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    /*result = cpssExMxPmTtiMacToMeEnableGet(devNum, keyType, &enable);*/
    /* will work only on port 0 */
    result = cpssExMxPmTtiPortMacToMeEnableGet(devNum,0,keyType,&enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiMacToMeSet
*
* DESCRIPTION:
*       This function sets the TTI MacToMe relevant Mac addresses & Vlans
*       (when this feature enabled packets not falling in the Mac to me,
*       Vlan to Me would not invoke the TTI lookup).
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       macVlanRangePtr   - points to Mac To Me, Vlan To Me ranges
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong device id or vlan range
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiMacToMeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#if 0
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_MAC_VLAN_RANGE_STC macVlanRange;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    galtisMacAddr(&macVlanRange.minMac,(GT_U8*)inArgs[1]);
    galtisMacAddr(&macVlanRange.maxMac,(GT_U8*)inArgs[2]);
    macVlanRange.minVlanId=(GT_U16)inArgs[3];
    macVlanRange.maxVlanId=(GT_U16)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmTtiMacToMeSet(devNum, &macVlanRange);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
#endif

    cmdOsPrintf("This command is OBSOLETTE\n");
    cmdOsPrintf("USE cpssExMxPm2TtiMacToMeSet\n");
    return CMD_SYNTAX_ERROR;
}

/*******************************************************************************
* cpssExMxPmTtiMacToMeGet
*
* DESCRIPTION:
*       This function gets the TTI MacToMe relevant Mac addresses & Vlans
*       (when this feature enabled packets not falling in the Mac to me,
*       Vlan to Me would not invoke the TTI lookup).
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*
* OUTPUTS:
*       macVlanRangePtr   - points to Mac To Me, Vlan To Me ranges
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong device id
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiMacToMeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#if 0
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_MAC_VLAN_RANGE_STC macVlanRange;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmTtiMacToMeGet(devNum, &macVlanRange);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%6b%6b%d%d",macVlanRange.minMac.arEther,macVlanRange.maxMac.arEther,
                 macVlanRange.minVlanId,macVlanRange.maxVlanId);
#endif
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiPortLookupEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for the specified key
*       type at the port.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_ETH_E
*       enable        - GT_TRUE: enable TTI lookup
*                       GT_FALSE: disable TTI lookup
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiPortLookupEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    keyType = (CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmTtiPortLookupEnableSet(devNum, port, keyType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiPortLookupEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of  the
*       TTI lookup for the specified key type.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_ETH_E
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TTI lookup
*                       GT_TRUE: TTI lookup is enabled
*                       GT_FALSE: TTI lookup is disabled
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id, port or key type
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiPortLookupEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    keyType = (CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmTtiPortLookupEnableGet(devNum, port, keyType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiPortIpv4OnlyTunneledEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only tunneled
*       packets received on port.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: enable IPv4 TTI lookup only for tunneled packets
*                       GT_FALSE: disable IPv4 TTI lookup only for tunneled packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiPortIpv4OnlyTunneledEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmTtiPortIpv4OnlyTunneledEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only tunneled packets received on port.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       tunneled packets
*                       GT_TRUE: IPv4 TTI lookup only for tunneled packets is enabled
*                       GT_FALSE: IPv4 TTI lookup only for tunneled packets is disabled
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or port
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiPortIpv4OnlyTunneledEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/* --- cpssExMxPmTtiRuleAction Table*/

/* table cpssExMxPmTtiRuleAction global variables */
#if 0
static struct
{
    CPSS_EXMXPM_TTI_ACTION_STC          actionEntry;
    CPSS_EXMXPM_TTI_EXTENDED_ACTION_STC extendedActionEntry;
    GT_BOOL                             isExtended;
    GT_U32                              ruleIndex;
    GT_U8                               devNum;
    GT_BOOL                             valid;
    GT_U32                              nextFree;
}actionTable[1024];
#endif

/*static GT_U32   firstFree = 0;*/
/*static GT_BOOL  firstRun = GT_TRUE;*/
#define INVALID_RULE_INDEX 0x7FFFFFF0
#if 0
/*******************************************************************************
* ttiActionGet (table command)
*
* DESCRIPTION:
*  The function searches for action table entry by rule index and device number
*  and returns the actionEntry.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       ruleIndex       - Index of the rule in the internal or external TCAM.
*
* OUTPUTS:
*       actionTableEntry     - contains actionEntry, isExtended flag, extendedActionEntry, next free index and valid flag.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
static void pmTtiActionGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  ruleIndex,
    OUT GT_U32                  *tableIndex
)
{

    GT_U32              index = 0;
    GT_BOOL             entryFound = GT_FALSE;

    while ((!entryFound) && index < 1024)
    {
        while (!actionTable[index].valid && index < 1024) index++;
        if ( (index < 1024) &&
             (actionTable[index].ruleIndex == ruleIndex) &&
             (actionTable[index].devNum == devNum))
            entryFound = GT_TRUE;
        else index++;
    }

    if (entryFound)
    {
        *tableIndex = index;
    }
    else
    {
        *tableIndex = INVALID_RULE_INDEX;
    }

}
#endif
/*******************************************************************************
* cpssExMxPmTtiActionClear  (table command)
*
* DESCRIPTION:
*  The function initializes the table entries and sets all entries invalid.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       ruleIndex       - Index of the rule in the internal or external TCAM.
*
* OUTPUTS:
*       actionTable      - contains actionEntry, next free index and valid flag.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*       The use of this function is for first initialization and for clearing all
*       antries in the table.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiActionClear
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#if 0
    GT_U32          index = 0;
    firstFree = 0;
    while (index < 1023)
    {
        actionTable[index].valid = GT_FALSE;
        actionTable[index].nextFree = index + 1;
        index++;
    }
    actionTable[index].nextFree = INVALID_RULE_INDEX;
    actionTable[index].valid = GT_FALSE;
#endif
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiRuleActionUpdate
*
* DESCRIPTION:
*       This function updates rule action.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       ruleIndex     - Index of the rule in the internal or external TCAM.
*       useExtendedAction - whether to use extended action parameter; note that
*                       extended action parameter should be used in case the
*                       lookup action type is set to extended mode
*       actionPtr     - points to the TTI rule action that applied on packet
*                       if packet's search key matched with masked pattern.
*       extendedActionPtr - points to the TTI rule extended action parameters
*                       that applied on packet if packet's search key matched
*                       with masked pattern; relevant only if lookup action type
*                       is extended action
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleActionUpdate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#if 0
    GT_STATUS                           result;
    GT_U8                               devNum;
    GT_U32                              ruleIndex;
    GT_U32                              index;
    CPSS_EXMXPM_TTI_ACTION_STC          *actionPtr;
    CPSS_EXMXPM_TTI_EXTENDED_ACTION_STC *extendedActionPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  =       (GT_U8)inFields[0];
    ruleIndex =     (GT_U32)inFields[1];
    if (ruleIndex > 1023)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Rule index has to be in range [0..1023].\n");

        return CMD_AGENT_ERROR;
    }
    if (firstRun)
    {
        wrCpssExMxPmTtiActionClear(inArgs,inFields,numFields,outArgs);
        firstRun = GT_FALSE;
    }

    pmTtiActionGet((GT_U8)inFields[0], inFields[1], &index);

    /* the rule wasn't previously in action table */
    if (index == INVALID_RULE_INDEX)
    {
        if(firstFree == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
            return CMD_AGENT_ERROR;
        }

        index = firstFree;
        firstFree = actionTable[firstFree].nextFree;
    }

    actionTable[index].valid = GT_TRUE;
    actionTable[index].ruleIndex = ruleIndex;
    actionTable[index].devNum = devNum;
    actionPtr = &(actionTable[index].actionEntry);
    extendedActionPtr = &(actionTable[index].extendedActionEntry);
    actionTable[index].isExtended = (GT_BOOL)inFields[2];

    actionPtr->command = (CPSS_PACKET_CMD_ENT)inFields[3];
    actionPtr->mirrorToIngressAnalyzerEnable = (GT_BOOL)inFields[4];
    actionPtr->tunnelTerminateEnable = (GT_BOOL)inFields[5];
    actionPtr->redirectCommand = (CPSS_EXMXPM_TTI_REDIRECT_COMMAND_ENT)inFields[6];
    actionPtr->userDefinedCpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inFields[7];

    actionPtr->outlif.targetType = (CPSS_EXMXPM_TTI_OUTLIF_TARGET_TYPE_ENT)inFields[8];
    actionPtr->outlif.devPort.devNum = (GT_U8)inFields[9];
    actionPtr->outlif.devPort.portNum = (GT_U8)inFields[10];
    actionPtr->outlif.trunkId = (GT_TRUNK_ID)inFields[11];
    actionPtr->outlif.vidx = (GT_U16)inFields[12];
    actionPtr->outlif.type = (CPSS_EXMXPM_TTI_OUTLIF_TYPE_ENT)inFields[13];
    actionPtr->outlif.mllPointer = inFields[14];
    actionPtr->outlif.tunnelParams.tunnelPointer = inFields[15];
    actionPtr->outlif.tunnelParams.nextLayerAboveTunnelType = (CPSS_EXMXPM_TTI_LAYER_ABOVE_TUNNEL_ENT)inFields[16];
    actionPtr->outlif.arpPointer = inFields[17];

    actionPtr->ipNextHopIndex = inFields[18];
    actionPtr->vpnId = inFields[19];
    actionPtr->inlifId = inFields[20];
    actionPtr->sourceIsPE = (GT_BOOL)inFields[21];
    actionPtr->untaggedMruIndex = inFields[22];
    actionPtr->enableSourceLocalFiltering = (GT_BOOL)inFields[23];
    actionPtr->enableInlifAutoLearn = (GT_BOOL)inFields[24];
    actionPtr->enableInlifForwardUnknownMacSa = (GT_BOOL)inFields[25];
    actionPtr->enableInlifNewAddrMsg = (GT_BOOL)inFields[26];
    actionPtr->enableInlifMovedStaticCheck = (GT_BOOL)inFields[27];
    actionPtr->vlanId = (GT_U16)inFields[28];
    actionPtr->vlanCmd = (CPSS_EXMXPM_TTI_VLAN_COMMAND_ENT)inFields[29];
    actionPtr->vlanPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[30];
    actionPtr->passengerPacketType = (CPSS_EXMXPM_TTI_PASSENGER_TYPE_ENT)inFields[31];
    actionPtr->bypassBridge = (GT_BOOL)inFields[32];
    actionPtr->flowId = inFields[33];
    actionPtr->nestedVlanEnable = (GT_BOOL)inFields[34];
    actionPtr->mplsCmd = (CPSS_EXMXPM_TTI_MPLS_COMMAND_ENT)inFields[35];
    actionPtr->exp = inFields[36];
    actionPtr->setExp = (GT_BOOL)inFields[37];
    actionPtr->enableDecTtl = (GT_BOOL)inFields[38];
    actionPtr->mplsLabel = inFields[39];
    actionPtr->ttl = inFields[40];
    actionPtr->copyTtlFromTunnelHeader = (GT_BOOL)inFields[41];
    actionPtr->matchCounterIndex = inFields[42];
    actionPtr->matchCounterEnable = (GT_BOOL)inFields[43];
    actionPtr->meterEnable = (GT_BOOL)inFields[44];
    actionPtr->policerIndex = inFields[45];

    if (actionTable[index].isExtended)
    {
        extendedActionPtr->keepPreviousQoS = (GT_BOOL)inFields[46];
        extendedActionPtr->trustUp = (GT_BOOL)inFields[47];
        extendedActionPtr->trustDscp = (GT_BOOL)inFields[48];
        extendedActionPtr->trustExp = (GT_BOOL)inFields[49];
        extendedActionPtr->enableUpToUpRemapping = (GT_BOOL)inFields[50];
        extendedActionPtr->enableDscpToDscpRemapping = (GT_BOOL)inFields[51];
        extendedActionPtr->enableCfiToDpMapping = (GT_BOOL)inFields[52];
        extendedActionPtr->up = inFields[53];
        extendedActionPtr->dscp = inFields[54];
        extendedActionPtr->exp = inFields[55];
        extendedActionPtr->tc = inFields[56];
        extendedActionPtr->dp = (CPSS_DP_LEVEL_ENT)inFields[57];
        extendedActionPtr->modifyDscp = (GT_BOOL)inFields[58];
        extendedActionPtr->modifyUp = (GT_BOOL)inFields[59];
        extendedActionPtr->modifyExp = (GT_BOOL)inFields[60];
        extendedActionPtr->qosPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[61];
    }

    /* call cpss api function */
    result = cpssExMxPmTtiRuleActionUpdate(devNum, ruleIndex, actionTable[index].isExtended, actionPtr, extendedActionPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
#endif
    return CMD_OK;
}

/*******************************************************************************/

/*static  GT_U32   index;*/

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleActionGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#if 0
    CPSS_EXMXPM_TTI_ACTION_STC          *actionPtr;
    CPSS_EXMXPM_TTI_EXTENDED_ACTION_STC *extendedActionPtr;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    while(index < 1024 && !actionTable[index].valid)
        index++;

    if (index == 1024)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    actionPtr = &(actionTable[index].actionEntry);
    extendedActionPtr = &(actionTable[index].extendedActionEntry);

    inFields[0] = actionTable[index].devNum;
    inFields[1] = actionTable[index].ruleIndex;
    inFields[2] = actionTable[index].isExtended;
    inFields[3] = actionPtr->command;
    inFields[4] = actionPtr->mirrorToIngressAnalyzerEnable;
    inFields[5] = actionPtr->tunnelTerminateEnable;
    inFields[6] = actionPtr->redirectCommand;
    inFields[7] = actionPtr->userDefinedCpuCode;
    inFields[8] = actionPtr->outlif.targetType;
    inFields[9] = actionPtr->outlif.devPort.devNum;
    inFields[10] = actionPtr->outlif.devPort.portNum;
    inFields[11] = actionPtr->outlif.trunkId;
    inFields[12] = actionPtr->outlif.vidx;
    inFields[13] = actionPtr->outlif.type;
    inFields[14] = actionPtr->outlif.mllPointer;
    inFields[15] = actionPtr->outlif.tunnelParams.tunnelPointer;
    inFields[16] = actionPtr->outlif.tunnelParams.nextLayerAboveTunnelType;
    inFields[17] = actionPtr->outlif.arpPointer;
    inFields[18] = actionPtr->ipNextHopIndex;
    inFields[19] = actionPtr->vpnId;
    inFields[20] = actionPtr->inlifId;
    inFields[21] = actionPtr->sourceIsPE;
    inFields[22] = actionPtr->untaggedMruIndex;
    inFields[23] = actionPtr->enableSourceLocalFiltering;
    inFields[24] = actionPtr->enableInlifAutoLearn;
    inFields[25] = actionPtr->enableInlifForwardUnknownMacSa;
    inFields[26] = actionPtr->enableInlifNewAddrMsg;
    inFields[27] = actionPtr->enableInlifMovedStaticCheck;
    inFields[28] = actionPtr->vlanId;
    inFields[29] = actionPtr->vlanCmd;
    inFields[30] = actionPtr->vlanPrecedence;
    inFields[31] = actionPtr->passengerPacketType;
    inFields[32] = actionPtr->bypassBridge;
    inFields[33] = actionPtr->flowId;
    inFields[34] = actionPtr->nestedVlanEnable;
    inFields[35] = actionPtr->mplsCmd;
    inFields[36] = actionPtr->exp;
    inFields[37] = actionPtr->setExp;
    inFields[38] = actionPtr->enableDecTtl;
    inFields[39] = actionPtr->mplsLabel;
    inFields[40] = actionPtr->ttl;
    inFields[41] = actionPtr->copyTtlFromTunnelHeader;
    inFields[42] = actionPtr->matchCounterIndex;
    inFields[43] = actionPtr->matchCounterEnable;
    inFields[44] = actionPtr->meterEnable;
    inFields[45] = actionPtr->policerIndex;

    if (actionTable[index].isExtended)
    {
        inFields[46] = extendedActionPtr->keepPreviousQoS;
        inFields[47] = extendedActionPtr->trustUp;
        inFields[48] = extendedActionPtr->trustDscp;
        inFields[49] = extendedActionPtr->trustExp;
        inFields[50] = extendedActionPtr->enableUpToUpRemapping;
        inFields[51] = extendedActionPtr->enableDscpToDscpRemapping;
        inFields[52] = extendedActionPtr->enableCfiToDpMapping;
        inFields[53] = extendedActionPtr->up;
        inFields[54] = extendedActionPtr->dscp;
        inFields[55] = extendedActionPtr->exp;
        inFields[56] = extendedActionPtr->tc;
        inFields[57] = extendedActionPtr->dp;
        inFields[58] = extendedActionPtr->modifyDscp;
        inFields[59] = extendedActionPtr->modifyUp;
        inFields[60] = extendedActionPtr->modifyExp;
        inFields[61] = extendedActionPtr->qosPrecedence;
    }

    index++;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],   inFields[2],   inFields[3],
                inFields[4],  inFields[5],   inFields[6],   inFields[7],
                inFields[8],  inFields[9],   inFields[10],  inFields[11],
                inFields[12], inFields[13],  inFields[14],  inFields[15],
                inFields[16], inFields[17],  inFields[18],  inFields[19],
                inFields[20], inFields[21],  inFields[22],  inFields[23],
                inFields[24], inFields[25],  inFields[26],  inFields[27],
                inFields[28], inFields[29],  inFields[30],  inFields[31],
                inFields[32], inFields[33],  inFields[34],  inFields[35],
                inFields[36], inFields[37],  inFields[38],  inFields[39],
                inFields[40], inFields[41],  inFields[42],  inFields[43],
                inFields[44], inFields[45],  inFields[46],  inFields[47],
                inFields[48], inFields[49],  inFields[50],  inFields[51],
                inFields[52], inFields[53],  inFields[54],  inFields[55],
                inFields[56], inFields[57],  inFields[58],  inFields[59],
                inFields[60], inFields[61]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
#endif
    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleActionGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#if 0
    index = 0;

    return wrCpssExMxPmTtiRuleActionGet(inArgs,inFields,numFields,outArgs);
#endif
    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleActionDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#if 0
    GT_U32                              ruleIndex;
    GT_U8                               devNum;
    GT_U32                              index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum =  (GT_U8)inFields[0];
    ruleIndex = (GT_U32)inFields[1];
    if (ruleIndex > 1023)
        return CMD_AGENT_ERROR;

    pmTtiActionGet(devNum, ruleIndex, &index);

    /* the rule is not found */
    if (index == INVALID_RULE_INDEX)
    {
        galtisOutput(outArgs, GT_NO_SUCH, "%d", -1);
        return CMD_AGENT_ERROR;
    }

    actionTable[index].valid = GT_FALSE;
    actionTable[index].nextFree = firstFree;
    firstFree = index;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
#endif
    return CMD_OK;
}


/*******************************************************************************/

/* --- cpssExMxPmTtiRule Table--- */

/* tables cpssExMxPmTtiRule global variables */

/*static    CPSS_EXMXPM_TTI_RULE_UNT      maskData;
static    CPSS_EXMXPM_TTI_RULE_UNT      patternData;
static    CPSS_EXMXPM_TTI_KEY_TYPE_ENT  ruleFormat;*/
static    GT_BOOL                       mask_set = GT_FALSE; /* is mask set*/
static    GT_BOOL                       pattern_set = GT_FALSE; /* is pattern set*/
/*static    GT_U32                        mask_ruleIndex = 0;
static    GT_U32                        pattern_ruleIndex = 0;*/

#if 0
/*******************************************************************************
* cpssExMxPmTtiRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_ETH_E
*       ruleIndex     - Index of the rule in the internal or external TCAM.
*       useExtendedAction - whether to use extended action parameter; note that
*                       extended action parameter should be used in case the
*                       lookup action type is set to extended mode
*       patternPtr    - points to the rule's pattern
*       maskPtr       - points to the rule's mask. The rule mask is "AND STYLED
*                       ONE". Mask bit's 0 means don't care bit (corresponding
*                       bit in the pattern is not used in the TCAM lookup).
*                       Mask bit's 1 means that corresponding bit in the pattern
*                       is using in the TCAM lookup.
*       actionPtr     - points to the TTI rule action that applied on packet
*                       if packet's search key matched with masked pattern.
*       extendedActionPtr - points to the TTI rule extended action parameters
*                       that applied on packet if packet's search key matched
*                       with masked pattern; relevant only if lookup action type
*                       is extended action
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRule_KEY_IPV4_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    ruleFormat = CPSS_EXMXPM_TTI_KEY_IPV4_E;

    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ipv4.pclId = (GT_U32)inFields[2];
        maskData.ipv4.srcIsTrunk = (GT_U32)inFields[3];
        maskData.ipv4.srcPortTrunk = (GT_U32)inFields[4];
        galtisMacAddr(&maskData.ipv4.mac, (GT_U8*)inFields[5]);
        maskData.ipv4.vid = (GT_U16)inFields[6];
        maskData.ipv4.isTagged = (GT_BOOL)inFields[7];
        maskData.ipv4.tunneltype = (CPSS_EXMXPM_TTI_TUNNEL_TYPE_ENT)inFields[8];
        galtisIpAddr(&maskData.ipv4.srcIp, (GT_U8*)inFields[9]);
        galtisIpAddr(&maskData.ipv4.destIp, (GT_U8*)inFields[10]);
        maskData.ipv4.dsaSrcPort = (GT_U8)inFields[11];
        maskData.ipv4.dsaSrcDevice = (GT_U8)inFields[12];

        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ipv4.pclId = (GT_U32)inFields[2];
        patternData.ipv4.srcIsTrunk = (GT_U32)inFields[3];
        patternData.ipv4.srcPortTrunk = (GT_U32)inFields[4];
        galtisMacAddr(&patternData.ipv4.mac, (GT_U8*)inFields[5]);
        patternData.ipv4.vid = (GT_U16)inFields[6];
        patternData.ipv4.isTagged = (GT_BOOL)inFields[7];
        patternData.ipv4.tunneltype = (CPSS_EXMXPM_TTI_TUNNEL_TYPE_ENT)inFields[8];
        galtisIpAddr(&patternData.ipv4.srcIp, (GT_U8*)inFields[9]);
        galtisIpAddr(&patternData.ipv4.destIp, (GT_U8*)inFields[10]);
        patternData.ipv4.dsaSrcPort = (GT_U8)inFields[11];
        patternData.ipv4.dsaSrcDevice = (GT_U8)inFields[12];

        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRule_KEY_MPLS_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    ruleFormat = CPSS_EXMXPM_TTI_KEY_IPV4_E;

    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.mpls.pclId = (GT_U32)inFields[2];
        maskData.mpls.srcIsTrunk = (GT_U32)inFields[3];
        maskData.mpls.srcPortTrunk = (GT_U32)inFields[4];
        galtisMacAddr(&maskData.mpls.mac, (GT_U8*)inFields[5]);
        maskData.mpls.vid = (GT_U16)inFields[6];
        maskData.mpls.isTagged = (GT_BOOL)inFields[7];
        maskData.mpls.label1 = (GT_U32)inFields[8];
        maskData.mpls.sBit1 = (GT_U32)inFields[9];
        maskData.mpls.exp1 = (GT_U32)inFields[10];
        maskData.mpls.label2 = (GT_U32)inFields[11];
        maskData.mpls.sBit2 = (GT_U32)inFields[12];
        maskData.mpls.exp2 = (GT_U32)inFields[13];
        maskData.mpls.dsaSrcPort = (GT_U8)inFields[14];
        maskData.mpls.dsaSrcDevice = (GT_U8)inFields[15];

        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.mpls.pclId = (GT_U32)inFields[2];
        patternData.mpls.srcIsTrunk = (GT_U32)inFields[3];
        patternData.mpls.srcPortTrunk = (GT_U32)inFields[4];
        galtisMacAddr(&patternData.mpls.mac, (GT_U8*)inFields[5]);
        patternData.mpls.vid = (GT_U16)inFields[6];
        patternData.mpls.isTagged = (GT_BOOL)inFields[7];
        patternData.mpls.label1 = (GT_U32)inFields[8];
        patternData.mpls.sBit1 = (GT_U32)inFields[9];
        patternData.mpls.exp1 = (GT_U32)inFields[10];
        patternData.mpls.label2 = (GT_U32)inFields[11];
        patternData.mpls.sBit2 = (GT_U32)inFields[12];
        patternData.mpls.exp2 = (GT_U32)inFields[13];
        patternData.mpls.dsaSrcPort = (GT_U8)inFields[14];
        patternData.mpls.dsaSrcDevice = (GT_U8)inFields[15];

        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRule_KEY_ETH_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    ruleFormat = CPSS_EXMXPM_TTI_KEY_IPV4_E;

    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.eth.pclId = (GT_U32)inFields[2];
        maskData.eth.srcIsTrunk = (GT_U32)inFields[3];
        maskData.eth.srcPortTrunk = (GT_U32)inFields[4];
        galtisMacAddr(&maskData.eth.mac, (GT_U8*)inFields[5]);
        maskData.eth.isTagged = (GT_BOOL)inFields[6];
        maskData.eth.vid0 = (GT_U16)inFields[7];
        maskData.eth.up0 = inFields[8];
        maskData.eth.cfi0 = inFields[9];
        maskData.eth.isVlan1Exists = (GT_BOOL)inFields[10];
        maskData.eth.vid1 = (GT_U16)inFields[11];
        maskData.eth.up1 = inFields[12];
        maskData.eth.cfi1 = inFields[13];
        maskData.eth.dsaSrcPort = (GT_U8)inFields[14];
        maskData.eth.dsaSrcDevice = (GT_U8)inFields[15];

        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.eth.pclId = (GT_U32)inFields[2];
        patternData.eth.srcIsTrunk = (GT_U32)inFields[3];
        patternData.eth.srcPortTrunk = (GT_U32)inFields[4];
        galtisMacAddr(&patternData.eth.mac, (GT_U8*)inFields[5]);
        patternData.eth.isTagged = (GT_BOOL)inFields[6];
        patternData.eth.vid0 = (GT_U16)inFields[7];
        patternData.eth.up0 = inFields[8];
        patternData.eth.cfi0 = inFields[9];
        patternData.eth.isVlan1Exists = (GT_BOOL)inFields[10];
        patternData.eth.vid1 = (GT_U16)inFields[11];
        patternData.eth.up1 = inFields[12];
        patternData.eth.cfi1 = inFields[13];
        patternData.eth.dsaSrcPort = (GT_U8)inFields[14];
        patternData.eth.dsaSrcDevice = (GT_U8)inFields[15];

        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}
#endif
/******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#if 0
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_EXMXPM_TTI_RULE_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_EXMXPM_TTI_RULE_UNT));

    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;

    switch (inArgs[1])
    {
        case CPSS_EXMXPM_TTI_KEY_IPV4_E:
            wrCpssExMxPmTtiRule_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_MPLS_E:
            wrCpssExMxPmTtiRule_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_ETH_E:
            wrCpssExMxPmTtiRule_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
            break;

        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
            return CMD_AGENT_ERROR;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
#endif
    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#if 0
    GT_STATUS result = GT_OK;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    GT_U32                              actionIndex;
    GT_BOOL                             useExtendedAction;
    CPSS_EXMXPM_TTI_ACTION_STC          *actionPtr;
    CPSS_EXMXPM_TTI_EXTENDED_ACTION_STC *extendedActionPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* Check Validity */
    if(inFields[1] > 1)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be MASK or PATTERN).\n");
        return CMD_AGENT_ERROR;
    }

    if(inFields[1] == 0 && mask_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if(inFields[1] == 1 && pattern_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    switch (inArgs[1])
    {
        case CPSS_EXMXPM_TTI_KEY_IPV4_E:
            wrCpssExMxPmTtiRule_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_MPLS_E:
            wrCpssExMxPmTtiRule_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_ETH_E:
            wrCpssExMxPmTtiRule_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
            break;

        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
            return CMD_AGENT_ERROR;
    }

    if(mask_set && pattern_set && (mask_ruleIndex != pattern_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }


    /* Get the action */
    pmTtiActionGet(devNum, inFields[0], &actionIndex);

    if(mask_set && pattern_set && (actionIndex != INVALID_RULE_INDEX))
    {
        actionPtr = &(actionTable[actionIndex].actionEntry);
        extendedActionPtr = &(actionTable[actionIndex].extendedActionEntry);
        useExtendedAction = actionTable[actionIndex].isExtended;
        keyType = (CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[1];

        if(useExtendedAction)
            /* call cpss api function */
            result = cpssExMxPmTtiRuleSet(devNum, keyType, actionTable[actionIndex].ruleIndex, useExtendedAction, &patternData, &maskData, actionPtr, extendedActionPtr);
        else
            /* call cpss api function */
            result = cpssExMxPmTtiRuleSet(devNum, keyType, actionTable[actionIndex].ruleIndex, useExtendedAction, &patternData, &maskData, actionPtr, NULL);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        cmdOsMemSet(&maskData , 0, sizeof(CPSS_EXMXPM_TTI_RULE_UNT));
        cmdOsMemSet(&patternData, 0, sizeof(CPSS_EXMXPM_TTI_RULE_UNT));
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
#endif
    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if(mask_set || pattern_set)
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_ETH_E
*       ruleIndex     - Index of the rule in the internal or external TCAM.
*       useExtendedAction - whether to use extended action parameter; note that
*                       extended action parameter should be used in case the
*                       lookup action type is set to extended mode
*
* OUTPUTS:
*       patternPtr    - points to the rule's pattern
*       maskPtr       - points to the rule's mask. The rule mask is "AND STYLED
*                       ONE". Mask bit's 0 means don't care bit (corresponding
*                       bit in the pattern is not used in the TCAM lookup).
*                       Mask bit's 1 means that corresponding bit in the pattern
*                       is using in the TCAM lookup.
*       actionPtr     - points to the TTI rule action that applied on packet
*                       if packet's search key matched with masked pattern.
*       extendedActionPtr - points to the TTI rule extended action parameters
*                       that applied on packet if packet's search key matched
*                       with masked pattern; relevant only if lookup action type
*                       is extended action
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/

static GT_U32 ruleIndex;
static GT_BOOL isMask;
#if 0
static CMD_STATUS wrCpssExMxPmTtiRule_KEY_IPV4_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    ruleFormat = CPSS_EXMXPM_TTI_KEY_IPV4_E;

    if(isMask) /* mask */
    {
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        inFields[2] = maskData.ipv4.pclId;
        inFields[3] = maskData.ipv4.srcIsTrunk;
        inFields[4] = maskData.ipv4.srcPortTrunk;
        inFields[6] = maskData.ipv4.vid;
        inFields[7] = maskData.ipv4.isTagged;
        inFields[8] = maskData.ipv4.tunneltype;
        inFields[11] = maskData.ipv4.dsaSrcPort;
        inFields[12] = maskData.ipv4.dsaSrcDevice;
        inFields[13] = GT_FALSE; /* fictial value. Used because of Galtis constraints */

        isMask = GT_FALSE;

        fieldOutput("%d%d%d%d%d%6b%d%d%d%4b%4b%d%d%d",
                    inFields[0], inFields[1],  inFields[2], inFields[3],
                    inFields[4], maskData.ipv4.mac.arEther, inFields[6],
                    inFields[7], inFields[8], maskData.ipv4.srcIp.arIP,
                    maskData.ipv4.destIp.arIP, inFields[11], inFields[12],
                    inFields[13]);
    }
    else /* pattern */
    {
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        inFields[2] = patternData.ipv4.pclId;
        inFields[3] = patternData.ipv4.srcIsTrunk;
        inFields[4] = patternData.ipv4.srcPortTrunk;
        inFields[6] = patternData.ipv4.vid;
        inFields[7] = patternData.ipv4.isTagged;
        inFields[8] = patternData.ipv4.tunneltype;
        inFields[11] = patternData.ipv4.dsaSrcPort;
        inFields[12] = patternData.ipv4.dsaSrcDevice;
        inFields[13] = GT_FALSE; /* fictial value. Used because of Galtis constraints */

        isMask = GT_TRUE;

        fieldOutput("%d%d%d%d%d%6b%d%d%d%4b%4b%d%d%d",
                    inFields[0], inFields[1],  inFields[2], inFields[3],
                    inFields[4], patternData.ipv4.mac.arEther, inFields[6],
                    inFields[7], inFields[8], patternData.ipv4.srcIp.arIP,
                    patternData.ipv4.destIp.arIP, inFields[11], inFields[12],
                    inFields[13]);
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRule_KEY_MPLS_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    ruleFormat = CPSS_EXMXPM_TTI_KEY_MPLS_E;

    if(isMask) /* mask */
    {
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        inFields[2] = maskData.mpls.pclId;
        inFields[3] = maskData.mpls.srcIsTrunk;
        inFields[4] = maskData.mpls.srcPortTrunk;
        inFields[6] = maskData.mpls.vid;
        inFields[7] = maskData.mpls.isTagged;
        inFields[8] = maskData.mpls.label1;
        inFields[9] = maskData.mpls.sBit1;
        inFields[10] = maskData.mpls.exp1;
        inFields[11] = maskData.mpls.label2;
        inFields[12] = maskData.mpls.sBit2;
        inFields[13] = maskData.mpls.exp2;
        inFields[14] = maskData.mpls.dsaSrcPort;
        inFields[15] = maskData.mpls.dsaSrcDevice;
        inFields[16] = GT_FALSE; /* fictial value. Used because of Galtis constraints */

        isMask = GT_FALSE;

        fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4], maskData.mpls.mac.arEther,  inFields[6],
                    inFields[7], inFields[8], inFields[9],  inFields[10],
                    inFields[11], inFields[12],inFields[13], inFields[14],
                    inFields[15], inFields[16]);
    }
    else /* pattern */
    {
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        inFields[2] = patternData.mpls.pclId;
        inFields[3] = patternData.mpls.srcIsTrunk;
        inFields[4] = patternData.mpls.srcPortTrunk;
        inFields[6] = patternData.mpls.vid;
        inFields[7] = patternData.mpls.isTagged;
        inFields[8] = patternData.mpls.label1;
        inFields[9] = patternData.mpls.sBit1;
        inFields[10] = patternData.mpls.exp1;
        inFields[11] = patternData.mpls.label2;
        inFields[12] = patternData.mpls.sBit2;
        inFields[13] = patternData.mpls.exp2;
        inFields[14] = patternData.mpls.dsaSrcPort;
        inFields[15] = patternData.mpls.dsaSrcDevice;
        inFields[16] = GT_FALSE; /* fictial value. Used because of Galtis constraints */

        isMask = GT_TRUE;

        fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4], patternData.mpls.mac.arEther,  inFields[6],
                    inFields[7], inFields[8], inFields[9],  inFields[10],
                    inFields[11], inFields[12],inFields[13], inFields[14],
                    inFields[15], inFields[16]);
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);

    return CMD_OK;

}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRule_KEY_ETH_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    ruleFormat = CPSS_EXMXPM_TTI_KEY_ETH_E;

    if(isMask) /* mask */
    {
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        inFields[2] = maskData.eth.pclId;
        inFields[3] = maskData.eth.srcIsTrunk;
        inFields[4] = maskData.eth.srcPortTrunk;
        inFields[6] = maskData.eth.isTagged;
        inFields[7] = maskData.eth.vid0;
        inFields[8] = maskData.eth.up0;
        inFields[9] = maskData.eth.cfi0;
        inFields[10] = maskData.eth.isVlan1Exists;
        inFields[11] = maskData.eth.vid1;
        inFields[12] = maskData.eth.up1;
        inFields[13] = maskData.eth.cfi1;
        inFields[14] = maskData.eth.dsaSrcPort;
        inFields[15] = maskData.eth.dsaSrcDevice;
        inFields[16] = GT_FALSE; /* fictial value. Used because of Galtis constraints */

        isMask = GT_FALSE;

        fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4], maskData.eth.mac.arEther,  inFields[6],
                    inFields[7], inFields[8], inFields[9],  inFields[10],
                    inFields[11],inFields[12],inFields[13], inFields[14],
                    inFields[15], inFields[16]);
    }
    else /* pattern */
    {
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        inFields[2] = patternData.eth.pclId;
        inFields[3] = patternData.eth.srcIsTrunk;
        inFields[4] = patternData.eth.srcPortTrunk;
        inFields[6] = patternData.eth.isTagged;
        inFields[7] = patternData.eth.vid0;
        inFields[8] = patternData.eth.up0;
        inFields[9] = patternData.eth.cfi0;
        inFields[10] = patternData.eth.isVlan1Exists;
        inFields[11] = patternData.eth.vid1;
        inFields[12] = patternData.eth.up1;
        inFields[13] = patternData.eth.cfi1;
        inFields[14] = patternData.eth.dsaSrcPort;
        inFields[15] = patternData.eth.dsaSrcDevice;
        inFields[16] = GT_FALSE; /* fictial value. Used because of Galtis constraints */

        isMask = GT_TRUE;

        fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4], patternData.eth.mac.arEther,  inFields[6],
                    inFields[7], inFields[8], inFields[9],  inFields[10],
                    inFields[11],inFields[12],inFields[13], inFields[14],
                    inFields[15], inFields[16]);
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);

    return CMD_OK;
}

#endif
/*******************************************************************************/

static CMD_STATUS wrCpssExMxPmTtiRuleGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#if 0
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT        keyType;
    GT_BOOL                             useExtendedAction;
    CPSS_EXMXPM_TTI_ACTION_STC          action;
    CPSS_EXMXPM_TTI_EXTENDED_ACTION_STC extendedAction;
    GT_U32                              actionIndex;

    /* if isMask == GT_TRUE it is mask's turn (the first one to get), so there need to refresh the data
        else the data is updated and it is pattern's turn (was updated earlier)*/
    if (isMask)
    {
        /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];
        keyType = inArgs[1];

        actionIndex = INVALID_RULE_INDEX;
        for(; actionIndex == INVALID_RULE_INDEX && ruleIndex < 1024; ruleIndex++)
            pmTtiActionGet(devNum, ruleIndex, &actionIndex);

        if (ruleIndex == 1024)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_OK, "%d", -1);

            return CMD_OK;
        }

        ruleIndex--;
        useExtendedAction = actionTable[actionIndex].isExtended;

        /* call cpss api function */
        result = cpssExMxPmTtiRuleGet(devNum, keyType, ruleIndex, useExtendedAction, &patternData, &maskData, &action, &extendedAction);
    }

    switch (inArgs[1])
    {
        case CPSS_EXMXPM_TTI_KEY_IPV4_E:
            wrCpssExMxPmTtiRule_KEY_IPV4_Get(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_MPLS_E:
            wrCpssExMxPmTtiRule_KEY_MPLS_Get(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_ETH_E:
            wrCpssExMxPmTtiRule_KEY_ETH_Get(inArgs,inFields,numFields,outArgs);
            break;

        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
            return CMD_AGENT_ERROR;
    }

    if (isMask)
        ruleIndex++;
#endif
    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ruleIndex = 0;

    /* first to get is mask */
    isMask = GT_TRUE;

    return wrCpssExMxPmTtiRuleGet(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* cpssExMxPmTtiRuleDefaultExtendedActionSet
*
* DESCRIPTION:
*       This function sets the global default extended action (used by standard
*       actions).
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       defaultExtendedActionPtr - points to the default TTI rule extended
*                       action parameters that applied on packet if packet's
*                       search key matched with masked pattern
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleDefaultExtendedActionSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#if 0
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_EXTENDED_ACTION_STC defaultExtendedAction;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    defaultExtendedAction.keepPreviousQoS=(GT_BOOL)inFields[0];
    defaultExtendedAction.trustUp=(GT_BOOL)inFields[1];
    defaultExtendedAction.trustDscp=(GT_BOOL)inFields[2];
    defaultExtendedAction.trustExp=(GT_BOOL)inFields[3];
    defaultExtendedAction.enableUpToUpRemapping=(GT_BOOL)inFields[4];
    defaultExtendedAction.enableDscpToDscpRemapping=(GT_BOOL)inFields[5];
    defaultExtendedAction.enableCfiToDpMapping=(GT_BOOL)inFields[6];
    defaultExtendedAction.up=(GT_U32)inFields[7];
    defaultExtendedAction.dscp=(GT_U32)inFields[8];
    defaultExtendedAction.exp=(GT_U32)inFields[9];
    defaultExtendedAction.tc=(GT_U32)inFields[10];
    defaultExtendedAction.dp=(CPSS_DP_LEVEL_ENT)inFields[11];
    defaultExtendedAction.modifyDscp=(GT_BOOL)inFields[12];
    defaultExtendedAction.modifyUp=(GT_BOOL)inFields[13];
    defaultExtendedAction.modifyExp=(GT_BOOL)inFields[14];
    defaultExtendedAction.qosPrecedence=(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)
                                        inFields[15];

    /* call cpss api function */
    result = cpssExMxPmTtiRuleDefaultExtendedActionSet(devNum, &defaultExtendedAction);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
#endif
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiRuleDefaultExtendedActionGet
*
* DESCRIPTION:
*       This function gets the global default extended action.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       defaultExtendedActionPtr - points to the default TTI rule extended
*                       action parameters that applied on packet if packet's
*                       search key matched with masked pattern
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleDefaultExtendedActionGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#if 0
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_EXTENDED_ACTION_STC defaultExtendedAction;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmTtiRuleDefaultExtendedActionGet(devNum, &defaultExtendedAction);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
    return CMD_OK;
}
    inFields[0]=defaultExtendedAction.keepPreviousQoS;
    inFields[1]=defaultExtendedAction.trustUp;
    inFields[2]=defaultExtendedAction.trustDscp;
    inFields[3]=defaultExtendedAction.trustExp;
    inFields[4]=defaultExtendedAction.enableUpToUpRemapping;
    inFields[5]=defaultExtendedAction.enableDscpToDscpRemapping;
    inFields[6]=defaultExtendedAction.enableCfiToDpMapping ;
    inFields[7]=defaultExtendedAction.up ;
    inFields[8]=defaultExtendedAction.dscp ;
    inFields[9]=defaultExtendedAction.exp ;
    inFields[10]=defaultExtendedAction.tc ;
    inFields[11]=defaultExtendedAction.dp ;
    inFields[12]=defaultExtendedAction.modifyDscp ;
    inFields[13]=defaultExtendedAction.modifyUp ;
    inFields[14]=defaultExtendedAction.modifyExp ;
    inFields[15]=defaultExtendedAction.qosPrecedence;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],
                inFields[9],  inFields[10],  inFields[11],
                inFields[12],  inFields[13],  inFields[14],
                inFields[15]);



    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
#endif
    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmTtiRuleDefaultExtendedActionGet
*
* DESCRIPTION:
*       This function gets the global default extended action.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       defaultExtendedActionPtr - points to the default TTI rule extended
*                       action parameters that applied on packet if packet's
*                       search key matched with masked pattern
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleDefaultExtendedActionGetNext
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
* cpssExMxPmTtiExceptionCmdSet
*
* DESCRIPTION:
*       This function sets the command for the various TTI exceptions.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       exceptionType - TTI exception type
*       cmd           - exception command; valid values:
*                           CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                           CPSS_PACKET_CMD_DROP_HARD_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiExceptionCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_EXCEPTION_ENT exceptionType;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_EXMXPM_TTI_EXCEPTION_ENT)inArgs[1];
    cmd = (CPSS_PACKET_CMD_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmTtiExceptionCmdSet(devNum, exceptionType, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiExceptionCmdGet
*
* DESCRIPTION:
*       This function gets the command for the various TTI exceptions.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       exceptionType - TTI exception type
*
* OUTPUTS:
*       cmdPtr        - points to exception command; valid values:
*                           CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                           CPSS_PACKET_CMD_DROP_HARD_E
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or exception type
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiExceptionCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_EXCEPTION_ENT exceptionType;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_EXMXPM_TTI_EXCEPTION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTtiExceptionCmdGet(devNum, exceptionType, &cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cmd);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiRuleMatchCounterSet
*
* DESCRIPTION:
*       This function sets TTI rule match counter value.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       counterIndex  - TTI match counter index (0..2)
*       value         - counter value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleMatchCounterSet
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
    GT_U32 value;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    counterIndex = (GT_U32)inArgs[1];
    value = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmTtiRuleMatchCounterSet(devNum, counterIndex, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiRuleMatchCounterGet
*
* DESCRIPTION:
*       This function gets TTI rule match counter value.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       counterIndex  - TTI match counter index (0..2)
*
* OUTPUTS:
*       valuePtr      - points to counter value
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or counter index
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleMatchCounterGet
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
    GT_U32 value;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    counterIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTtiRuleMatchCounterGet(devNum, counterIndex, &value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", value);

    return CMD_OK;
}
/******************tti external tcam configs table for puma 2 *********************************/
static struct
{   /*extTcam Rule params*/
    GT_U32                                    tcamId;
    GT_U32                                    ruleStartIndex;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT   ruleSize;
    GT_BOOL                                   ageEnable;
    /*extTcam action params */
    GT_U32                                    actionStartIndex;
    CPSS_EXMXPM_CONTROL_MEMORY_ENT            memoryType;

    GT_BOOL                                   valid;
    GT_U32                                    nextFree;
}extTcamConfigPm2Table[1024];


static GT_U32   firstExtTcamFree = 0;
static GT_BOOL  firsExtTcamRun = GT_TRUE;
/*******************************************************************************
* prvPm2TtiExtConfigGet (table command)
*
* DESCRIPTION:
*  The function searches for external tcam config  table entry by rule index
*  and returns index.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*
*       ruleIndex       - Index of the rule external TCAM.
*
* OUTPUTS:
*       tableIndex     - external TCAM table entry
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
static void prvPm2TtiExtConfigGet
(
    IN  GT_U32                  ruleIndex,
    OUT GT_U32                  *tableIndex
)
{
    GT_U32              index = 0;
    GT_BOOL             entryFound = GT_FALSE;

    while ((!entryFound) && index < 1024)
    {
        while (!extTcamConfigPm2Table[index].valid && index < 1024) index++;
        if ( (index < 1024) &&
             (extTcamConfigPm2Table[index].ruleStartIndex == ruleIndex))

            entryFound = GT_TRUE;
        else index++;
    }

    if (entryFound)
    {
        *tableIndex = index;
    }
    else
    {
        *tableIndex = INVALID_RULE_INDEX;
    }
}
/*******************************************************************************
* cpssExMxPmTtiExtConfigClear  (table command)
*
* DESCRIPTION:
*  The function initializes the table entries and sets all entries invalid.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       ruleIndex       - Index of the rule in the internal or external TCAM.
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*       The use of this function is for first initialization and for clearing all
*       antries in the table.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiExtConfigClear
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32          index = 0;
    firstExtTcamFree = 0;
    while (index < 1023)
    {
        extTcamConfigPm2Table[index].valid = GT_FALSE;
        extTcamConfigPm2Table[index].nextFree = index + 1;
        index++;
    }
    extTcamConfigPm2Table[index].nextFree = INVALID_RULE_INDEX;
    extTcamConfigPm2Table[index].valid = GT_FALSE;

    return CMD_OK;
}
/*******************************************************************************
* prvCpssExMxPmTtiExtConfigSet
*
* DESCRIPTION:
*       This function sets external TCAM table entry
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiExtConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32            ruleStartIndex;
    GT_U32            index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    ruleStartIndex=(GT_U32)inFields[0];

    if (ruleStartIndex > 1023)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Rule index has to be in range [0..1023].\n");

        return CMD_AGENT_ERROR;
    }
    if (firsExtTcamRun)
    {
        wrCpssExMxPmTtiExtConfigClear(inArgs,inFields,numFields,outArgs);
        firsExtTcamRun = GT_FALSE;
    }

    prvPm2TtiExtConfigGet(ruleStartIndex, &index);

    /* the rule wasn't previously in action table */
    if (index == INVALID_RULE_INDEX)
    {
        if(firstExtTcamFree == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
            return CMD_AGENT_ERROR;
        }

        index = firstExtTcamFree;
        firstExtTcamFree = extTcamConfigPm2Table[firstExtTcamFree].nextFree;
    }

    /* map input arguments to locals */

    extTcamConfigPm2Table[index].valid = GT_TRUE;
    extTcamConfigPm2Table[index].ruleStartIndex =ruleStartIndex;
    extTcamConfigPm2Table[index].ruleSize=(CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT)inFields[1];
    extTcamConfigPm2Table[index].tcamId=(GT_U32)inFields[2];
    extTcamConfigPm2Table[index].ageEnable=(GT_BOOL)inFields[3];
    extTcamConfigPm2Table[index].actionStartIndex=(GT_U32)inFields[4];
    extTcamConfigPm2Table[index].memoryType=(CPSS_EXMXPM_CONTROL_MEMORY_ENT)inFields[5];

  /* pack output arguments to galtis string */
   galtisOutput(outArgs, GT_OK, "");

   return CMD_OK;
}

static GT_U32 pm2ExtTcamIndex;
/********************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiExtConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    while(pm2ExtTcamIndex < 1024 && !extTcamConfigPm2Table[pm2ExtTcamIndex].valid)
        pm2ExtTcamIndex++;

    if (pm2ExtTcamIndex == 1024)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
   inFields[0]=extTcamConfigPm2Table[pm2ExtTcamIndex].ruleStartIndex;
   inFields[1]=extTcamConfigPm2Table[pm2ExtTcamIndex].ruleSize;
   inFields[2]=extTcamConfigPm2Table[pm2ExtTcamIndex].tcamId;
   inFields[3]=extTcamConfigPm2Table[pm2ExtTcamIndex].ageEnable;
   inFields[4]=extTcamConfigPm2Table[pm2ExtTcamIndex].actionStartIndex;
   inFields[5]=extTcamConfigPm2Table[pm2ExtTcamIndex].memoryType;

   pm2ExtTcamIndex++;

   fieldOutput("%d%d%d%d%d%d",
                inFields[0],  inFields[1],   inFields[2],   inFields[3],
                inFields[4],  inFields[5]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiExtConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    pm2ExtTcamIndex = 0;

    return wrCpssExMxPmTtiExtConfigGet(inArgs,inFields,numFields,outArgs);
}
/******************tti action table for puma 2 *********************************/


typedef struct
{
    CPSS_EXMXPM_TTI_ACTION_UNT          actionEntry;
    CPSS_EXMXPM_TTI_ACTION_TYPE_ENT     actionType;
    GT_U32                              ruleIndex;
    GT_U8                               devNum;
    GT_BOOL                             valid;
    GT_U32                              nextFree;
}WR_INTERNAL_ACTION_STC;

/*internal array*/
static WR_INTERNAL_ACTION_STC actionPm2Table[1024];

/*external Tcam array*/
static WR_INTERNAL_ACTION_STC actionExtPm2Table[1024];

static CPSS_EXMXPM_TTI_KEY_TYPE_ENT        typesOfIntEntry[1024];
static CPSS_EXMXPM_TTI_KEY_TYPE_ENT        typesOfExtEntry[1024];


static GT_U32   firstPm2IntFree = 0;
static GT_BOOL  firstPm2IntRun = GT_TRUE;

static GT_U32   firstPm2ExtFree = 0;
static GT_BOOL  firstPm2ExtRun = GT_TRUE;
/*******************************************************************************
* prvPm2TtiActionGet (table command)
*
* DESCRIPTION:
*  The function searches for action table entry by rule index and device number
*  and returns the actionEntry.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       ruleIndex       - Index of the rule in the internal or external TCAM.
*
* OUTPUTS:
*       tableIndex     - action table entry
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
static void prvPm2TtiActionGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  ruleIndex,
    IN  WR_INTERNAL_ACTION_STC  *actionTable,
    OUT GT_U32                  *tableIndex
)
{
    GT_U32              index = 0;
    GT_BOOL             entryFound = GT_FALSE;

    while ((!entryFound) && index < 1024)
    {
        while (!actionTable[index].valid && index < 1024) index++;
        if ( (index < 1024) &&
             (actionTable[index].ruleIndex == ruleIndex) &&
             (actionTable[index].devNum == devNum))
            entryFound = GT_TRUE;
        else index++;
    }

    if (entryFound)
    {
        *tableIndex = index;
    }
    else
    {
        *tableIndex = INVALID_RULE_INDEX;
    }
}
/*******************************************************************************
* cpssExMxPmTtiActionClear  (table command)
*
* DESCRIPTION:
*  The function initializes the table entries and sets all entries invalid.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       ruleIndex       - Index of the rule in the internal or external TCAM.
*
* OUTPUTS:
*       actionTable      - contains actionEntry, next free index and valid flag.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*       The use of this function is for first initialization and for clearing all
*       antries in the table.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiActionClear
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32          index = 0;
    WR_INTERNAL_ACTION_STC *actionTable;
    CPSS_EXMXPM_TCAM_TYPE_ENT           tcamType;


    tcamType=(CPSS_EXMXPM_TCAM_TYPE_ENT)inArgs[0];

    if(tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
    {
        firstPm2IntFree = 0;
        actionTable=actionPm2Table;
    }
     else
     {
         firstPm2ExtFree = 0;
         actionTable=actionExtPm2Table;

     }

    while (index < 1023)
    {
        actionTable[index].valid = GT_FALSE;
        actionTable[index].nextFree = index + 1;
        index++;
    }
    actionTable[index].nextFree = INVALID_RULE_INDEX;
    actionTable[index].valid = GT_FALSE;

    return CMD_OK;
}
/********************************************************************************/
static void prvCpssExMxPm2TtiRuleActionSet_STANDARD_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_U32 index,
    IN  WR_INTERNAL_ACTION_STC    *tablePtr
)
{
    CPSS_EXMXPM_TTI_ACTION_UNT          *actionPtr;
    CPSS_EXMXPM_OUTLIF_INFO_STC         *outlifPtr;

    actionPtr = &(tablePtr[index].actionEntry);
    outlifPtr=&(actionPtr->standard.outlif);

    /* Forwarding parameters */

    actionPtr->standard.command=(CPSS_PACKET_CMD_ENT)inFields[2];
    actionPtr->standard.redirectCommand=(CPSS_EXMXPM_TTI_REDIRECT_COMMAND_ENT)inFields[3];
    actionPtr->standard.userDefinedCpuCode=(CPSS_NET_RX_CPU_CODE_ENT)inFields[4];

       /*outlif*/
    cmdOsMemSet(outlifPtr, 0, sizeof(outlifPtr));

    outlifPtr->outlifType = (CPSS_EXMXPM_OUTLIF_TYPE_ENT)inFields[5];

    switch(outlifPtr->outlifType)
    {
    case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
        outlifPtr->outlifPointer.arpPtr=(GT_U32)inFields[6];
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
        outlifPtr->outlifPointer.ditPtr=(GT_U32)inFields[7];
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
        outlifPtr->outlifPointer.tunnelStartPtr.passengerPacketType=(CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[8];
        outlifPtr->outlifPointer.tunnelStartPtr.ptr=(GT_U32)inFields[9];
        break;
    default:
        break;
    }


    outlifPtr->interfaceInfo.type=(CPSS_INTERFACE_TYPE_ENT)inFields[10] ;

    switch(outlifPtr->interfaceInfo.type)
    {
    case CPSS_INTERFACE_PORT_E:
        outlifPtr->interfaceInfo.devPort.devNum = (GT_U8)inFields[11];
        outlifPtr->interfaceInfo.devPort.portNum = (GT_U8)inFields[12];
        CONVERT_DEV_PORT_DATA_MAC(outlifPtr->interfaceInfo.devPort.devNum,
            outlifPtr->interfaceInfo.devPort.portNum);

        break;

    case CPSS_INTERFACE_TRUNK_E:
        outlifPtr->interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[13];
        break;

    case CPSS_INTERFACE_VIDX_E:
        outlifPtr->interfaceInfo.vidx = (GT_U16)inFields[14];
        break;

    case CPSS_INTERFACE_VID_E:
        outlifPtr->interfaceInfo.vlanId = (GT_U16)inFields[15];
        break;
   case CPSS_INTERFACE_FABRIC_VIDX_E:
       /* outlifPtr->interfaceInfo.fabricVidx= (GT_U16)inFields[16];*/
        break;

    default:
        break;
    }

   actionPtr->standard.ipNextHopIndex= (GT_U32)inFields[16];

    /* Tunnel Terminate parameters */

   actionPtr->standard.tunnelTerminateEnable= (GT_BOOL)inFields[17];
   actionPtr->standard.passengerPacketType= (CPSS_EXMXPM_TTI_PASSENGER_TYPE_ENT)inFields[18];

   /* MPLS parameters */

   actionPtr->standard.mplsCmd= (CPSS_EXMXPM_TTI_MPLS_COMMAND_ENT)inFields[19];
   actionPtr->standard.exp=(GT_U32)inFields[20];
   actionPtr->standard.setExp=(GT_BOOL)inFields[21];
   actionPtr->standard.enableDecTtl=(GT_BOOL)inFields[22];
   actionPtr->standard.mplsLabel=(GT_U32)inFields[23];
   actionPtr->standard.ttl=(GT_U32)inFields[24];
   actionPtr->standard.copyTtlFromTunnelHeader=(GT_BOOL)inFields[25];

    /* VPLS parameters */

    actionPtr->standard.pwId=(GT_U32)inFields[26];
    actionPtr->standard.sourceIsPE=(GT_BOOL)inFields[28];
    actionPtr->standard.enableSourceLocalFiltering=(GT_BOOL)inFields[29];
    actionPtr->standard.floodDitPointer=(GT_U32)inFields[30];

    /* InLif/VID modifications and assignment */
    actionPtr->standard.inLifVidCmd=(CPSS_EXMXPM_TTI_VLAN_INLIF_COMMAND_ENT)inFields[32];
    actionPtr->standard.vlanPrecedence=(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[33];
    actionPtr->standard.nestedVlanEnable=(GT_BOOL)inFields[34];

    /* Policer parameters */
    actionPtr->standard.counterEnable=(GT_BOOL)inFields[35];
    actionPtr->standard.meterEnable=(GT_BOOL)inFields[36];
    actionPtr->standard.policerIndex=(GT_U32)inFields[37];

    /* Management and Debug parameters */
    actionPtr->standard.mirrorToIngressAnalyzerEnable=(GT_BOOL)inFields[38];

    /* InLif Attributes */
    actionPtr->standard.inLifVidId=(GT_U16)inFields[39];

    /* Misc parameters */
    actionPtr->standard.flowId=(GT_U32)inFields[40];
    actionPtr->standard.sstIdEnable=(GT_BOOL)inFields[41];
    actionPtr->standard.sstId=(GT_U32)inFields[42];




}
/********************************************************************************/
static void prvCpssExMxPm2TtiRuleActionSet_EXTENDED_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_U32 index,
    IN  WR_INTERNAL_ACTION_STC    *tablePtr
)
{
    CPSS_EXMXPM_TTI_ACTION_UNT          *actionPtr;
    CPSS_EXMXPM_OUTLIF_INFO_STC         *outlifPtr;

    actionPtr = &(tablePtr[index].actionEntry);
    outlifPtr=&(actionPtr->extended.actionStd.outlif);

    /* Forwarding parameters */

    actionPtr->extended.actionStd.command=(CPSS_PACKET_CMD_ENT)inFields[2];
    actionPtr->extended.actionStd.redirectCommand=(CPSS_EXMXPM_TTI_REDIRECT_COMMAND_ENT)inFields[3];
    actionPtr->extended.actionStd.userDefinedCpuCode=(CPSS_NET_RX_CPU_CODE_ENT)inFields[4];

       /*outlif*/
    cmdOsMemSet(outlifPtr, 0, sizeof(outlifPtr));

    outlifPtr->outlifType = (CPSS_EXMXPM_OUTLIF_TYPE_ENT)inFields[5];

    switch(outlifPtr->outlifType)
    {
    case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
        outlifPtr->outlifPointer.arpPtr=(GT_U32)inFields[6];
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
        outlifPtr->outlifPointer.ditPtr=(GT_U32)inFields[7];
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
        outlifPtr->outlifPointer.tunnelStartPtr.passengerPacketType=(CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[8];
        outlifPtr->outlifPointer.tunnelStartPtr.ptr=(GT_U32)inFields[9];
        break;
    default:
        break;
    }


    outlifPtr->interfaceInfo.type=(CPSS_INTERFACE_TYPE_ENT)inFields[10] ;

    switch(outlifPtr->interfaceInfo.type)
    {
    case CPSS_INTERFACE_PORT_E:
        outlifPtr->interfaceInfo.devPort.devNum = (GT_U8)inFields[11];
        outlifPtr->interfaceInfo.devPort.portNum = (GT_U8)inFields[12];
        CONVERT_DEV_PORT_DATA_MAC(outlifPtr->interfaceInfo.devPort.devNum,
            outlifPtr->interfaceInfo.devPort.portNum);

        break;

    case CPSS_INTERFACE_TRUNK_E:
        outlifPtr->interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[13];
        break;

    case CPSS_INTERFACE_VIDX_E:
        outlifPtr->interfaceInfo.vidx = (GT_U16)inFields[14];
        break;

    case CPSS_INTERFACE_VID_E:
        outlifPtr->interfaceInfo.vlanId = (GT_U16)inFields[15];
        break;
   case CPSS_INTERFACE_FABRIC_VIDX_E:
      /*  outlifPtr->interfaceInfo.fabricVidx= (GT_U16)inFields[16];*/
        break;

    default:
        break;
    }

   actionPtr->extended.actionStd.ipNextHopIndex= (GT_U32)inFields[16];

    /* Tunnel Terminate parameters */

   actionPtr->extended.actionStd.tunnelTerminateEnable= (GT_BOOL)inFields[17];
   actionPtr->extended.actionStd.passengerPacketType= (CPSS_EXMXPM_TTI_PASSENGER_TYPE_ENT)inFields[18];

   /* MPLS parameters */

   actionPtr->extended.actionStd.mplsCmd= (CPSS_EXMXPM_TTI_MPLS_COMMAND_ENT)inFields[19];
   actionPtr->extended.actionStd.exp=(GT_U32)inFields[20];
   actionPtr->extended.actionStd.setExp=(GT_BOOL)inFields[21];
   actionPtr->extended.actionStd.enableDecTtl=(GT_BOOL)inFields[22];
   actionPtr->extended.actionStd.mplsLabel=(GT_U32)inFields[23];
   actionPtr->extended.actionStd.ttl=(GT_U32)inFields[24];
   actionPtr->extended.actionStd.copyTtlFromTunnelHeader=(GT_BOOL)inFields[25];

    /* VPLS parameters */

    actionPtr->extended.actionStd.pwId=(GT_U32)inFields[26];
    actionPtr->extended.actionStd.sourceIsPE=(GT_BOOL)inFields[28];
    actionPtr->extended.actionStd.enableSourceLocalFiltering=(GT_BOOL)inFields[29];
    actionPtr->extended.actionStd.floodDitPointer=(GT_U32)inFields[30];

    /* InLif/VID modifications and assignment */
    actionPtr->extended.actionStd.inLifVidCmd=(CPSS_EXMXPM_TTI_VLAN_INLIF_COMMAND_ENT)inFields[32];
    actionPtr->extended.actionStd.vlanPrecedence=(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[33];
    actionPtr->extended.actionStd.nestedVlanEnable=(GT_BOOL)inFields[34];

    /* Policer parameters */
    actionPtr->extended.actionStd.counterEnable=(GT_BOOL)inFields[35];
    actionPtr->extended.actionStd.meterEnable=(GT_BOOL)inFields[36];
    actionPtr->extended.actionStd.policerIndex=(GT_U32)inFields[37];

    /* QoS parameters */
    actionPtr->extended.actionQosParams.keepPreviousQoS=(GT_BOOL)inFields[38];
    actionPtr->extended.actionQosParams.trustUp=(GT_BOOL)inFields[39];
    actionPtr->extended.actionQosParams.trustDscp=(GT_BOOL)inFields[40];
    actionPtr->extended.actionQosParams.trustExp=(GT_BOOL)inFields[41];
    actionPtr->extended.actionQosParams.enableUpToUpRemapping=(GT_BOOL)inFields[42];
    actionPtr->extended.actionQosParams.enableDscpToDscpRemapping=(GT_BOOL)inFields[43];
    actionPtr->extended.actionQosParams.enableCfiToDpMapping=(GT_BOOL)inFields[44];
    actionPtr->extended.actionQosParams.up=(GT_U32)inFields[45];
    actionPtr->extended.actionQosParams.dscp=(GT_U32)inFields[46];
    /*actionPtr->extended.exp=(GT_U32)inFields[48]; */
    actionPtr->extended.actionQosParams.tc=(GT_U32)inFields[47];
    actionPtr->extended.actionQosParams.dp=(CPSS_DP_LEVEL_ENT)inFields[48];
    actionPtr->extended.actionQosParams.modifyDscp=(GT_BOOL)inFields[49];
    actionPtr->extended.actionQosParams.modifyUp=(GT_BOOL)inFields[50];
    actionPtr->extended.actionQosParams.modifyExp=(GT_BOOL)inFields[51];
    actionPtr->extended.actionQosParams.qosPrecedence=(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[52];

    /* Management and Debug parameters */
    actionPtr->extended.mirrorToIngressAnalyzerEnable=(GT_BOOL)inFields[53];
    actionPtr->extended.matchCounterIndex=(CPSS_EXMXPM_TTI_MATCH_COUNTER_INDEX_ENT)inFields[54];

    /* InLif Attributes */
    actionPtr->extended.actionStd.inLifVidId=(GT_U16)inFields[55];
    actionPtr->extended.pclId1=(GT_U32)inFields[56];
    actionPtr->extended.forceUdbProfileId=(GT_BOOL)inFields[57];
    actionPtr->extended.udbProfileId=(GT_U32)inFields[58];
    actionPtr->extended.lookup1NonIpKeyType=(CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT)inFields[59];
    actionPtr->extended.lookup1Ipv4ArpKeyType=(CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT)inFields[60];
    actionPtr->extended.lookup1Ipv6KeyType=(CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT)inFields[61];
    actionPtr->extended.lookup1Mode=(CPSS_EXMXPM_TTI_LOOKUP_MODE_ENT)inFields[62];
    actionPtr->extended.lookup1Type=(CPSS_EXMXPM_PCL_LOOKUP_TYPE_ENT)inFields[63];
    actionPtr->extended.forceIpcl1Config=(GT_BOOL)inFields[64];
    actionPtr->extended.ipcl0LookupEnable=(GT_BOOL)inFields[65];

    /* Misc parameters */
    actionPtr->extended.actionStd.flowId=(GT_U32)inFields[66];
    actionPtr->extended.actionStd.sstIdEnable=(GT_BOOL)inFields[67];
    actionPtr->extended.actionStd.sstId=(GT_U32)inFields[68];





}
/********************************************************************************/
static void prvCpssExMxPm2TtiRuleActionSet_REDUCED_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_U32 index,
    IN  WR_INTERNAL_ACTION_STC    *tablePtr
)
{
    CPSS_EXMXPM_TTI_ACTION_UNT          *actionPtr;
    CPSS_EXMXPM_OUTLIF_INFO_STC         *outlifPtr;

    actionPtr = &(tablePtr[index].actionEntry);
    outlifPtr=&(actionPtr->reduced.outlif);

    /* Forwarding parameters */

    actionPtr->reduced.command=(CPSS_PACKET_CMD_ENT)inFields[2];
    actionPtr->reduced.redirectCommand=(CPSS_EXMXPM_TTI_REDIRECT_COMMAND_ENT)inFields[3];
    actionPtr->reduced.userDefinedCpuCode=(CPSS_NET_RX_CPU_CODE_ENT)inFields[4];

       /*outlif*/
    cmdOsMemSet(outlifPtr, 0, sizeof(outlifPtr));

    outlifPtr->outlifType = (CPSS_EXMXPM_OUTLIF_TYPE_ENT)inFields[5];

    switch(outlifPtr->outlifType)
    {
    case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
        outlifPtr->outlifPointer.arpPtr=(GT_U32)inFields[6];
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
        outlifPtr->outlifPointer.ditPtr=(GT_U32)inFields[7];
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
        outlifPtr->outlifPointer.tunnelStartPtr.passengerPacketType=(CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[8];
        outlifPtr->outlifPointer.tunnelStartPtr.ptr=(GT_U32)inFields[9];
        break;
    default:
        break;
    }


    outlifPtr->interfaceInfo.type=(CPSS_INTERFACE_TYPE_ENT)inFields[10] ;

    switch(outlifPtr->interfaceInfo.type)
    {
    case CPSS_INTERFACE_PORT_E:
        outlifPtr->interfaceInfo.devPort.devNum = (GT_U8)inFields[11];
        outlifPtr->interfaceInfo.devPort.portNum = (GT_U8)inFields[12];
        CONVERT_DEV_PORT_DATA_MAC(outlifPtr->interfaceInfo.devPort.devNum,
            outlifPtr->interfaceInfo.devPort.portNum);

        break;

    case CPSS_INTERFACE_TRUNK_E:
        outlifPtr->interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[13];
        break;

    case CPSS_INTERFACE_VIDX_E:
        outlifPtr->interfaceInfo.vidx = (GT_U16)inFields[14];
        break;

    case CPSS_INTERFACE_VID_E:
        outlifPtr->interfaceInfo.vlanId = (GT_U16)inFields[15];
        break;
   case CPSS_INTERFACE_FABRIC_VIDX_E:
       /* outlifPtr->interfaceInfo.fabricVidx= (GT_U16)inFields[16];*/
        break;

    default:
        break;
    }

   actionPtr->reduced.ipNextHopIndex= (GT_U32)inFields[16];

    /* Tunnel Terminate parameters */

   actionPtr->reduced.tunnelTerminateEnable= (GT_BOOL)inFields[17];
   actionPtr->reduced.passengerPacketType= (CPSS_EXMXPM_TTI_PASSENGER_TYPE_ENT)inFields[18];

   /* MPLS parameters */

   actionPtr->reduced.copyTtlFromTunnelHeader=(GT_BOOL)inFields[19];

    /* VPLS parameters */

    actionPtr->reduced.pwId=(GT_U32)inFields[20];

    actionPtr->reduced.sourceIsPE=(GT_BOOL)inFields[21];
    actionPtr->reduced.enableSourceLocalFiltering=(GT_BOOL)inFields[22];

    /* InLif/VID modifications and assignment */
    actionPtr->reduced.inLifVidCmd=(CPSS_EXMXPM_TTI_VLAN_INLIF_COMMAND_ENT)inFields[23];

    actionPtr->reduced.nestedVlanEnable=(GT_BOOL)inFields[24];

    /* QoS parameters */
    actionPtr->reduced.actionQosParams.qosParams.keepPreviousQoS=(GT_BOOL)inFields[25];
    actionPtr->reduced.actionQosParams.qosParams.trustUp=(GT_BOOL)inFields[26];
    actionPtr->reduced.actionQosParams.qosParams.trustDscp=(GT_BOOL)inFields[27];
    actionPtr->reduced.actionQosParams.qosParams.trustExp=(GT_BOOL)inFields[28];
    actionPtr->reduced.actionQosParams.qosParams.enableUpToUpRemapping=(GT_BOOL)inFields[29];
    actionPtr->reduced.actionQosParams.qosParams.enableDscpToDscpRemapping=(GT_BOOL)inFields[30];
    actionPtr->reduced.actionQosParams.qosParams.enableCfiToDpMapping=(GT_BOOL)inFields[31];
    actionPtr->reduced.actionQosParams.qosParams.up=(GT_U32)inFields[32];
    actionPtr->reduced.actionQosParams.qosParams.dscp=(GT_U32)inFields[33];
    actionPtr->reduced.actionQosParams.exp=(GT_U32)inFields[34];
    actionPtr->reduced.actionQosParams.qosParams.tc=(GT_U32)inFields[35];
    actionPtr->reduced.actionQosParams.qosParams.dp=(CPSS_DP_LEVEL_ENT)inFields[36];
    actionPtr->reduced.actionQosParams.qosParams.modifyDscp=(GT_BOOL)inFields[37];
    actionPtr->reduced.actionQosParams.qosParams.modifyUp=(GT_BOOL)inFields[38];
    actionPtr->reduced.actionQosParams.qosParams.modifyExp=(GT_BOOL)inFields[39];
    actionPtr->reduced.actionQosParams.qosParams.qosPrecedence=(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[40];


    /* InLif Attributes */
    actionPtr->reduced.inLifVidId=(GT_U16)inFields[41];


    /* Misc parameters */
    actionPtr->reduced.flowId=(GT_U32)inFields[42];



}
/*******************************************************************************
* cpssExMxPmTtiRuleActionUpdate
*
* DESCRIPTION:
*       This function updates rule action.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       tcamType      - TCAM type: internal or external
*       actionInfoPtr - (pointer to) the Action Layout
*       actionType    - TTI action type; valid values:
*                           CPSS_EXMXPM_TTI_ACTION_REDUCED_E
*                           CPSS_EXMXPM_TTI_ACTION_STANDARD_E
*                           CPSS_EXMXPM_TTI_ACTION_EXTENDED_E
*       actionPtr     - points to the TTI rule action that applied on packet
*                       if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
CMD_STATUS wrCpssExMxPm2TtiRuleActionSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_EXMXPM_TCAM_ACTION_INFO_UNT     actionInfo;
    GT_U32                              index;
    GT_U32                              ruleIndex;
    CPSS_EXMXPM_TTI_ACTION_TYPE_ENT     actionType;
    CPSS_EXMXPM_TCAM_TYPE_ENT           tcamType;
    CPSS_EXMXPM_TTI_ACTION_UNT          *actionPtr;
    WR_INTERNAL_ACTION_STC              *tablePtr=NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tcamType=(CPSS_EXMXPM_TCAM_TYPE_ENT)inArgs[0];

    devNum  =       (GT_U8)inFields[0];

    if(tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
     {
      actionInfo.internalTcamRuleStartIndex=(GT_U32)inFields[1];
      ruleIndex=actionInfo.internalTcamRuleStartIndex;
      }
    else
    {
        /*bind extTcam configs*/
        ruleIndex=(GT_U32)inFields[1];
        prvPm2TtiExtConfigGet(ruleIndex,&index);

            if (index ==INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nExternal Tcam configs  not set\n");

            return CMD_AGENT_ERROR;
        }

            actionInfo.externalTcamInfo.actionInfo.actionStartIndex=extTcamConfigPm2Table[index].actionStartIndex;
            actionInfo.externalTcamInfo.actionInfo.memoryType=extTcamConfigPm2Table[index].memoryType;
     }/*end of else*/



    if (ruleIndex > 1023)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Rule index has to be in range [0..1023].\n");

        return CMD_AGENT_ERROR;
    }
    if (tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E&&firstPm2IntRun)
    {
        wrCpssExMxPm2TtiActionClear(inArgs,inFields,numFields,outArgs);
        firstPm2IntRun = GT_FALSE;
    }

     if (tcamType==CPSS_EXMXPM_TCAM_TYPE_EXTERNAL_E&&firstPm2ExtRun)
    {
        wrCpssExMxPm2TtiActionClear(inArgs,inFields,numFields,outArgs);
        firstPm2ExtRun = GT_FALSE;
    }


   if (tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
   {

      prvPm2TtiActionGet((GT_U8)inFields[0], ruleIndex,actionPm2Table, &index);

      /* the rule wasn't previously in action table */
      if (index == INVALID_RULE_INDEX)
      {
          if(firstPm2IntFree == INVALID_RULE_INDEX)
          {
              /* pack output arguments to galtis string */
              galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
              return CMD_AGENT_ERROR;
          }

          index = firstPm2IntFree;
          firstPm2IntFree = actionPm2Table[firstPm2IntFree].nextFree;
      }
      tablePtr=actionPm2Table;
    }


   if (tcamType==CPSS_EXMXPM_TCAM_TYPE_EXTERNAL_E)
   {

      prvPm2TtiActionGet((GT_U8)inFields[0], ruleIndex,actionExtPm2Table, &index);

      /* the rule wasn't previously in action table */
      if (index == INVALID_RULE_INDEX)
      {
          if(firstPm2ExtFree == INVALID_RULE_INDEX)
          {
              /* pack output arguments to galtis string */
              galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
              return CMD_AGENT_ERROR;
          }

          index = firstPm2ExtFree;
          firstPm2ExtFree = actionExtPm2Table[firstPm2ExtFree].nextFree;
      }
      tablePtr=actionExtPm2Table;
    }


    /* map input arguments to locals */

    tablePtr[index].valid = GT_TRUE;
    tablePtr[index].ruleIndex = ruleIndex;
    tablePtr[index].devNum = devNum;
    actionPtr = &(tablePtr[index].actionEntry);
    actionType=(CPSS_EXMXPM_TTI_ACTION_TYPE_ENT)inArgs[1];


   /*swtich by action type*/

    switch(actionType){

    case CPSS_EXMXPM_TTI_ACTION_STANDARD_E:
        tablePtr[index].actionType=CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
        prvCpssExMxPm2TtiRuleActionSet_STANDARD_E(inFields,index,tablePtr);
        break;
    case CPSS_EXMXPM_TTI_ACTION_EXTENDED_E:
        tablePtr[index].actionType=CPSS_EXMXPM_TTI_ACTION_EXTENDED_E;
        prvCpssExMxPm2TtiRuleActionSet_EXTENDED_E(inFields,index,tablePtr);
        break;
    case CPSS_EXMXPM_TTI_ACTION_REDUCED_E:
        tablePtr[index].actionType=CPSS_EXMXPM_TTI_ACTION_REDUCED_E;
        prvCpssExMxPm2TtiRuleActionSet_REDUCED_E(inFields,index,tablePtr);
        break;
    default:
        break;

    }

  /* call cpss api function */
   result = cpssExMxPmTtiRuleActionUpdate(devNum,tcamType,&actionInfo,actionType,actionPtr);

   /* pack output arguments to galtis string */
   galtisOutput(outArgs, result, "");

   return CMD_OK;
}
/*******************************************************************************/

static  GT_U32   pm2index;
/********************************************************************************/
static CMD_STATUS prvCpssExMxPm2TtiRuleActionGet_STANDARD_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN GT_U32 index,
    IN WR_INTERNAL_ACTION_STC              *tablePtr
)
{
    CPSS_EXMXPM_TTI_ACTION_UNT          *actionPtr;
    CPSS_EXMXPM_OUTLIF_INFO_STC         *outlifPtr;
    GT_U8 tempPort;
    GT_U8 tempDevNum;

    actionPtr =&(tablePtr[index].actionEntry);
    outlifPtr=&(actionPtr->standard.outlif);

    /* Forwarding parameters */
    inFields[0]=tablePtr[index].devNum;
    inFields[1]=tablePtr[index].ruleIndex;
    inFields[2]=actionPtr->standard.command;
    inFields[3]=actionPtr->standard.redirectCommand;
    inFields[4]=actionPtr->standard.userDefinedCpuCode;

       /*outlif*/
    cmdOsMemSet(outlifPtr, 0, sizeof(outlifPtr));

    inFields[5]=outlifPtr->outlifType;

    switch(outlifPtr->outlifType)
    {
    case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
        inFields[6]=outlifPtr->outlifPointer.arpPtr;
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
        inFields[7]=outlifPtr->outlifPointer.ditPtr;
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
        inFields[8]=outlifPtr->outlifPointer.tunnelStartPtr.passengerPacketType;
        inFields[9]=outlifPtr->outlifPointer.tunnelStartPtr.ptr;
        break;
    default:
        break;
    }


    inFields[10]=outlifPtr->interfaceInfo.type ;

    switch(outlifPtr->interfaceInfo.type)
    {
    case CPSS_INTERFACE_PORT_E:
        tempDevNum=outlifPtr->interfaceInfo.devPort.devNum;
        tempPort=outlifPtr->interfaceInfo.devPort.portNum;

        /*  convert back*/
       CONVERT_BACK_DEV_PORT_DATA_MAC(tempDevNum,tempPort);

        inFields[11]=tempDevNum;
        inFields[12]=tempPort;



        break;

    case CPSS_INTERFACE_TRUNK_E:
        inFields[13]=outlifPtr->interfaceInfo.trunkId;
        break;

    case CPSS_INTERFACE_VIDX_E:
        inFields[14]=outlifPtr->interfaceInfo.vidx ;
        break;

    case CPSS_INTERFACE_VID_E:
        inFields[15]=outlifPtr->interfaceInfo.vlanId;
        break;
   case CPSS_INTERFACE_FABRIC_VIDX_E:
       /* inFields[16]=outlifPtr->interfaceInfo.fabricVidx;*/
        break;

    default:
        break;
    }

   inFields[16]=actionPtr->standard.ipNextHopIndex;

    /* Tunnel Terminate parameters */

   inFields[17]=actionPtr->standard.tunnelTerminateEnable;
   inFields[18]=actionPtr->standard.passengerPacketType;

   /* MPLS parameters */

   inFields[19]=actionPtr->standard.mplsCmd;
   inFields[20]=actionPtr->standard.exp;
   inFields[21]=actionPtr->standard.setExp;
   inFields[22]=actionPtr->standard.enableDecTtl;
   inFields[23]=actionPtr->standard.mplsLabel;
   inFields[24]=actionPtr->standard.ttl;
   inFields[25]=actionPtr->standard.copyTtlFromTunnelHeader;

    /* VPLS parameters */

    inFields[26]=actionPtr->standard.pwId;
    inFields[28]=actionPtr->standard.sourceIsPE;
    inFields[29]=actionPtr->standard.enableSourceLocalFiltering;
    inFields[30]=actionPtr->standard.floodDitPointer;

    /* InLif/VID modifications and assignment */
    inFields[32]=actionPtr->standard.inLifVidCmd;
    inFields[33]=actionPtr->standard.vlanPrecedence;
    inFields[34]=actionPtr->standard.nestedVlanEnable;

    /* Policer parameters */
    inFields[35]=actionPtr->standard.counterEnable;
    inFields[36]=actionPtr->standard.meterEnable;
    inFields[37]=actionPtr->standard.policerIndex;

    /* Management and Debug parameters */
    inFields[38]=actionPtr->standard.mirrorToIngressAnalyzerEnable;

    /* InLif Attributes */
    inFields[39]=actionPtr->standard.inLifVidId;

    /* Misc parameters */
    inFields[40]=actionPtr->standard.flowId;
    inFields[41]=actionPtr->standard.sstIdEnable;
    inFields[42]=actionPtr->standard.sstId;


       fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",

                inFields[0],  inFields[1],   inFields[2],   inFields[3],
                inFields[4],  inFields[5],   inFields[6],   inFields[7],
                inFields[8],  inFields[9],   inFields[10],  inFields[11],
                inFields[12], inFields[13],  inFields[14],  inFields[15],
                inFields[16], inFields[17],  inFields[18],  inFields[19],
                inFields[20], inFields[21],  inFields[22],  inFields[23],
                inFields[24], inFields[25],  inFields[26],  inFields[27],
                inFields[28], inFields[29],  inFields[30],  inFields[31],
                inFields[32], inFields[33],  inFields[34],  inFields[35],
                inFields[36], inFields[37],  inFields[38],  inFields[39],
                inFields[40], inFields[41],  inFields[42]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 0);

    pm2index++;

    return CMD_OK;
}
/********************************************************************************/
static CMD_STATUS prvCpssExMxPm2TtiRuleActionGet_EXTENDED_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN GT_U32 index,
    IN WR_INTERNAL_ACTION_STC              *tablePtr
)
{   CPSS_EXMXPM_TTI_ACTION_UNT          *actionPtr;
    CPSS_EXMXPM_OUTLIF_INFO_STC         *outlifPtr;
    GT_U8 tempPort;
    GT_U8 tempDevNum;

    actionPtr =&(tablePtr[index].actionEntry);
    outlifPtr=&(actionPtr->extended.actionStd.outlif);

    /* Forwarding parameters */
    inFields[0]=tablePtr[index].devNum;
    inFields[1]=tablePtr[index].ruleIndex;
    inFields[2]=actionPtr->extended.actionStd.command ;
    inFields[3]=actionPtr->extended.actionStd.redirectCommand;
    inFields[4]=actionPtr->extended.actionStd.userDefinedCpuCode;

        /*outlif*/
     cmdOsMemSet(outlifPtr, 0, sizeof(outlifPtr));

     inFields[5]=outlifPtr->outlifType;

     switch(outlifPtr->outlifType)
     {
     case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
         inFields[6]=outlifPtr->outlifPointer.arpPtr;
         break;
     case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
         inFields[7]=outlifPtr->outlifPointer.ditPtr;
     break;
     case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
         inFields[8]=outlifPtr->outlifPointer.tunnelStartPtr.passengerPacketType;
         inFields[9]=outlifPtr->outlifPointer.tunnelStartPtr.ptr;
         break;
     default:
         break;
     }


     inFields[10]=outlifPtr->interfaceInfo.type ;

     switch(outlifPtr->interfaceInfo.type)
     {
     case CPSS_INTERFACE_PORT_E:
         tempDevNum=outlifPtr->interfaceInfo.devPort.devNum;
         tempPort=outlifPtr->interfaceInfo.devPort.portNum;

         /*  convert back*/
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempDevNum,tempPort);

         inFields[11]=tempDevNum;
         inFields[12]=tempPort;



     break;

     case CPSS_INTERFACE_TRUNK_E:
         inFields[13]=outlifPtr->interfaceInfo.trunkId;
         break;

     case CPSS_INTERFACE_VIDX_E:
         inFields[14]=outlifPtr->interfaceInfo.vidx ;
         break;

     case CPSS_INTERFACE_VID_E:
         inFields[15]=outlifPtr->interfaceInfo.vlanId;
         break;
    case CPSS_INTERFACE_FABRIC_VIDX_E:
        /* inFields[16]=outlifPtr->interfaceInfo.fabricVidx;*/
         break;

     default:
         break;
     }

   inFields[16]=actionPtr->extended.actionStd.ipNextHopIndex;

        /* Tunnel Terminate parameters */

   inFields[17]=actionPtr->extended.actionStd.tunnelTerminateEnable;
   inFields[18]=actionPtr->extended.actionStd.passengerPacketType;

   /* MPLS parameters */

   inFields[19]=actionPtr->extended.actionStd.mplsCmd;
   inFields[20]=actionPtr->extended.actionStd.exp;
   inFields[21]=actionPtr->extended.actionStd.setExp;
   inFields[22]=actionPtr->extended.actionStd.enableDecTtl;
   inFields[23]=actionPtr->extended.actionStd.mplsLabel;
   inFields[24]=actionPtr->extended.actionStd.ttl;
   inFields[25]=actionPtr->extended.actionStd.copyTtlFromTunnelHeader;

    /* VPLS parameters */

    inFields[26]=actionPtr->extended.actionStd.pwId;
    inFields[28]=actionPtr->extended.actionStd.sourceIsPE;
    inFields[29]=actionPtr->extended.actionStd.enableSourceLocalFiltering;
    inFields[30]=actionPtr->extended.actionStd.floodDitPointer;


    /* InLif/VID modifications and assignment */
    inFields[32]=actionPtr->extended.actionStd.inLifVidCmd;
    inFields[33]=actionPtr->extended.actionStd.vlanPrecedence;
    inFields[34]=actionPtr->extended.actionStd.nestedVlanEnable;

    /* Policer parameters */
    inFields[35]=actionPtr->extended.actionStd.counterEnable;
    inFields[36]=actionPtr->extended.actionStd.meterEnable;
    inFields[37]=actionPtr->extended.actionStd.policerIndex ;

    /* QoS parameters */
    inFields[38]=actionPtr->extended.actionQosParams.keepPreviousQoS;
    inFields[39]=actionPtr->extended.actionQosParams.trustUp;
    inFields[40]=actionPtr->extended.actionQosParams.trustDscp;
    inFields[41]=actionPtr->extended.actionQosParams.trustExp;
    inFields[42]=actionPtr->extended.actionQosParams.enableUpToUpRemapping;
    inFields[43]=actionPtr->extended.actionQosParams.enableDscpToDscpRemapping;
    inFields[44]=actionPtr->extended.actionQosParams.enableCfiToDpMapping ;
    inFields[45]=actionPtr->extended.actionQosParams.up;
    inFields[46]=actionPtr->extended.actionQosParams.dscp;
    inFields[47]=actionPtr->extended.actionQosParams.tc;
    inFields[48]=actionPtr->extended.actionQosParams.dp;
    inFields[49]=actionPtr->extended.actionQosParams.modifyDscp;
    inFields[50]=actionPtr->extended.actionQosParams.modifyUp;
    inFields[51]=actionPtr->extended.actionQosParams.modifyExp;
    inFields[52]=actionPtr->extended.actionQosParams.qosPrecedence;
    /* Management and Debug parameters */
    inFields[53]=actionPtr->extended.mirrorToIngressAnalyzerEnable;
    inFields[54]=actionPtr->extended.matchCounterIndex;

    /* InLif Attributes */
    inFields[55]=actionPtr->extended.actionStd.inLifVidId;
    inFields[56]=actionPtr->extended.pclId1;
    inFields[57]=actionPtr->extended.forceUdbProfileId;
    inFields[58]=actionPtr->extended.udbProfileId;
    inFields[59]=actionPtr->extended.lookup1NonIpKeyType;
    inFields[60]=actionPtr->extended.lookup1Ipv4ArpKeyType;
    inFields[61]=actionPtr->extended.lookup1Ipv6KeyType;
    inFields[62]=actionPtr->extended.lookup1Mode;
    inFields[63]=actionPtr->extended.lookup1Type;
    inFields[64]=actionPtr->extended.forceIpcl1Config;
    inFields[65]=actionPtr->extended.ipcl0LookupEnable;

    /* Misc parameters */
    inFields[66]=actionPtr->extended.actionStd.flowId;
    inFields[67]=actionPtr->extended.actionStd.sstIdEnable;
    inFields[68]=actionPtr->extended.actionStd.sstId;


  fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",

                inFields[0],  inFields[1],   inFields[2],   inFields[3],
                inFields[4],  inFields[5],   inFields[6],   inFields[7],
                inFields[8],  inFields[9],   inFields[10],  inFields[11],
                inFields[12], inFields[13],  inFields[14],  inFields[15],
                inFields[16], inFields[17],  inFields[18],  inFields[19],
                inFields[20], inFields[21],  inFields[22],  inFields[23],
                inFields[24], inFields[25],  inFields[26],  inFields[27],
                inFields[28], inFields[29],  inFields[30],  inFields[31],
                inFields[32], inFields[33],  inFields[34],  inFields[35],
                inFields[36], inFields[37],  inFields[38],  inFields[39],
                inFields[40], inFields[41],  inFields[42],  inFields[43],
                inFields[44], inFields[45],  inFields[46],  inFields[47],
                inFields[48], inFields[49],  inFields[50],  inFields[51],
                inFields[52], inFields[53],  inFields[54],  inFields[55],
                inFields[56], inFields[57],  inFields[58],  inFields[59],
                inFields[60], inFields[61],  inFields[62],  inFields[63],
                inFields[32], inFields[33],  inFields[34],  inFields[35],
                inFields[64], inFields[65],  inFields[66],  inFields[67],
                inFields[68]);



    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 1);

    pm2index++;

    return CMD_OK;
}
/********************************************************************************/
static CMD_STATUS prvCpssExMxPm2TtiRuleActionGet_REDUCED_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN GT_U32 index,
    IN WR_INTERNAL_ACTION_STC              *tablePtr
)
{
    CPSS_EXMXPM_TTI_ACTION_UNT          *actionPtr;
    CPSS_EXMXPM_OUTLIF_INFO_STC         *outlifPtr;
    GT_U8 tempPort;
    GT_U8 tempDevNum;

    actionPtr =&(tablePtr[index].actionEntry);
    outlifPtr=&(actionPtr->reduced.outlif);

    /* Forwarding parameters */
    inFields[0]=tablePtr[index].devNum;
    inFields[1]=tablePtr[index].ruleIndex;

    inFields[2]=actionPtr->reduced.command ;
    inFields[3]=actionPtr->reduced.redirectCommand;
    inFields[4]=actionPtr->reduced.userDefinedCpuCode;

        /*outlif*/
     cmdOsMemSet(outlifPtr, 0, sizeof(outlifPtr));

     inFields[5]=outlifPtr->outlifType;

     switch(outlifPtr->outlifType)
     {
     case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
         inFields[6]=outlifPtr->outlifPointer.arpPtr;
         break;
     case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
         inFields[7]=outlifPtr->outlifPointer.ditPtr;
     break;
     case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
         inFields[8]=outlifPtr->outlifPointer.tunnelStartPtr.passengerPacketType;
         inFields[9]=outlifPtr->outlifPointer.tunnelStartPtr.ptr;
         break;
     default:
         break;
     }


     inFields[10]=outlifPtr->interfaceInfo.type ;

     switch(outlifPtr->interfaceInfo.type)
     {
     case CPSS_INTERFACE_PORT_E:
         tempDevNum=outlifPtr->interfaceInfo.devPort.devNum;
         tempPort=outlifPtr->interfaceInfo.devPort.portNum;

         /*  convert back*/
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempDevNum,tempPort);

         inFields[11]=tempDevNum;
         inFields[12]=tempPort;



     break;

     case CPSS_INTERFACE_TRUNK_E:
         inFields[13]=outlifPtr->interfaceInfo.trunkId;
         break;

     case CPSS_INTERFACE_VIDX_E:
         inFields[14]=outlifPtr->interfaceInfo.vidx ;
         break;

     case CPSS_INTERFACE_VID_E:
         inFields[15]=outlifPtr->interfaceInfo.vlanId;
         break;
    case CPSS_INTERFACE_FABRIC_VIDX_E:
        /* inFields[16]=outlifPtr->interfaceInfo.fabricVidx;*/
         break;

     default:
         break;
     }

   inFields[16]=actionPtr->reduced.ipNextHopIndex;

        /* Tunnel Terminate parameters */

   inFields[17]=actionPtr->reduced.tunnelTerminateEnable;
   inFields[18]=actionPtr->reduced.passengerPacketType;

   /* MPLS parameters */

    inFields[19]=actionPtr->reduced.copyTtlFromTunnelHeader;

    /* VPLS parameters */

    inFields[20]=actionPtr->reduced.pwId;

    inFields[21]=actionPtr->reduced.sourceIsPE;
    inFields[22]=actionPtr->reduced.enableSourceLocalFiltering;

    /* InLif/VID modifications and assignment */
    inFields[23]=actionPtr->reduced.inLifVidCmd;

    inFields[24]=actionPtr->reduced.nestedVlanEnable;

    /* QoS parameters */
    inFields[25]=actionPtr->reduced.actionQosParams.qosParams.keepPreviousQoS;
    inFields[26]=actionPtr->reduced.actionQosParams.qosParams.trustUp;
    inFields[27]=actionPtr->reduced.actionQosParams.qosParams.trustDscp;
    inFields[28]=actionPtr->reduced.actionQosParams.qosParams.trustExp;
    inFields[29]=actionPtr->reduced.actionQosParams.qosParams.enableUpToUpRemapping;
    inFields[30]=actionPtr->reduced.actionQosParams.qosParams.enableDscpToDscpRemapping;
    inFields[31]=actionPtr->reduced.actionQosParams.qosParams.enableCfiToDpMapping ;
    inFields[32]=actionPtr->reduced.actionQosParams.qosParams.up;
    inFields[33]=actionPtr->reduced.actionQosParams.qosParams.dscp;
    inFields[34]=actionPtr->reduced.actionQosParams.exp;
    inFields[35]=actionPtr->reduced.actionQosParams.qosParams.tc;
    inFields[36]=actionPtr->reduced.actionQosParams.qosParams.dp;
    inFields[37]=actionPtr->reduced.actionQosParams.qosParams.modifyDscp;
    inFields[38]=actionPtr->reduced.actionQosParams.qosParams.modifyUp;
    inFields[39]=actionPtr->reduced.actionQosParams.qosParams.modifyExp;
    inFields[40]=actionPtr->reduced.actionQosParams.qosParams.qosPrecedence;


    /* InLif Attributes */
    inFields[41]=actionPtr->reduced.inLifVidId;


    /* Misc parameters */
    inFields[42]=actionPtr->reduced.flowId;



       fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",

                inFields[0],  inFields[1],   inFields[2],   inFields[3],
                inFields[4],  inFields[5],   inFields[6],   inFields[7],
                inFields[8],  inFields[9],   inFields[10],  inFields[11],
                inFields[12], inFields[13],  inFields[14],  inFields[15],
                inFields[16], inFields[17],  inFields[18],  inFields[19],
                inFields[20], inFields[21],  inFields[22],  inFields[23],
                inFields[24], inFields[25],  inFields[26],  inFields[27],
                inFields[28], inFields[29],  inFields[30],  inFields[31],
                inFields[32], inFields[33],  inFields[34],  inFields[35],
                inFields[36], inFields[37],  inFields[38],  inFields[39],
                inFields[40], inFields[41],  inFields[42]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 2);

    pm2index++;

    return CMD_OK;
}
/*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiRuleActionGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_EXMXPM_TTI_ACTION_UNT          *actionPtr;
    CPSS_EXMXPM_TTI_ACTION_TYPE_ENT     actionType;
    WR_INTERNAL_ACTION_STC              *tablePtr;
    CPSS_EXMXPM_TCAM_TYPE_ENT           tcamType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tcamType=(CPSS_EXMXPM_TCAM_TYPE_ENT)inArgs[0];

    if(tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
    {

        tablePtr=actionPm2Table;
    }
     else
     {
         tablePtr=actionExtPm2Table;

     }

    while(pm2index < 1024 && !tablePtr[pm2index].valid)
        pm2index++;

    if (pm2index == 1024)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    actionPtr = &(tablePtr[pm2index].actionEntry);
    actionType=tablePtr[pm2index].actionType;

    /*swtich by action type*/

    switch(actionType){

    case CPSS_EXMXPM_TTI_ACTION_STANDARD_E:
        return prvCpssExMxPm2TtiRuleActionGet_STANDARD_E(inFields,outArgs,pm2index,tablePtr);
        break;
    case CPSS_EXMXPM_TTI_ACTION_EXTENDED_E:
        return prvCpssExMxPm2TtiRuleActionGet_EXTENDED_E(inFields,outArgs,pm2index,tablePtr);
        break;
    case CPSS_EXMXPM_TTI_ACTION_REDUCED_E:
        return prvCpssExMxPm2TtiRuleActionGet_REDUCED_E(inFields,outArgs,pm2index,tablePtr);
        break;
    default:
        break;

    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiRuleActionGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    pm2index = 0;

    return wrCpssExMxPm2TtiRuleActionGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiRuleActionDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32                              ruleIndex;
    GT_U8                               devNum;
    GT_U32                              index;
    GT_U32                              *firstFreePtr;
    WR_INTERNAL_ACTION_STC              *tablePtr;
    CPSS_EXMXPM_TCAM_TYPE_ENT           tcamType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum =  (GT_U8)inFields[0];
    ruleIndex = (GT_U32)inFields[1];
    tcamType=(CPSS_EXMXPM_TCAM_TYPE_ENT)inArgs[0];

    if(tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
    {
        firstFreePtr=&firstPm2IntFree;
        tablePtr=actionPm2Table;
    }
     else
     {
         firstFreePtr=&firstPm2ExtFree;
         tablePtr=actionExtPm2Table;

     }

    if (ruleIndex > 1023)
        return CMD_AGENT_ERROR;

    prvPm2TtiActionGet(devNum, ruleIndex,tablePtr, &index);

    /* the rule is not found */
    if (index == INVALID_RULE_INDEX)
    {
        galtisOutput(outArgs, GT_NO_SUCH, "%d", -1);
        return CMD_AGENT_ERROR;
    }

    tablePtr[index].valid = GT_FALSE;
    tablePtr[index].nextFree = *firstFreePtr;
    *firstFreePtr = index;

    /* pack output arguments to galtis string */

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}
/*******************************************************************************/

/* --- cpssExMxPm2TtiRule Table--- */
/*        table for Puma 2        */
/* tables cpssExMxPmTtiRule global variables */

static    CPSS_EXMXPM_TTI_RULE_UNT      maskPm2Data;
static    CPSS_EXMXPM_TTI_RULE_UNT      patternPm2Data;
static    CPSS_EXMXPM_TTI_KEY_TYPE_ENT  rulePm2Format;
static    GT_BOOL                       maskPm2_set = GT_FALSE; /* is mask set*/
static    GT_BOOL                       patternPm2_set = GT_FALSE; /* is pattern set*/
static    GT_BOOL                       valid_mask;
static    GT_BOOL                       valid_pattern;
static    GT_U32                        maskPm2_ruleIndex = 0;
static    GT_U32                        patternPm2_ruleIndex = 0;

/*******************************************************************************
* cpssExMxPmTtiRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       tcamType      - TCAM type: internal or external
*       ruleInfoPtr   - (pointer to) the Rule and Action Layouts
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_ETH_E
*                           CPSS_EXMXPM_TTI_KEY_MIM_E
*       actionType    - TTI action type; valid values:
*                           CPSS_EXMXPM_TTI_ACTION_REDUCED_E
*                           CPSS_EXMXPM_TTI_ACTION_STANDARD_E
*                           CPSS_EXMXPM_TTI_ACTION_EXTENDED_E
*       valid         - GT_TRUE - valid, GT_FALSE - invalid
*       maskPtr       - points to the rule's mask. The rule mask is "AND STYLED
*                       ONE". Mask bit's 0 means don't care bit (corresponding
*                       bit in the pattern is not used in the TCAM lookup).
*                       Mask bit's 1 means that corresponding bit in the pattern
*                       is using in the TCAM lookup.
*       patternPtr    - points to the rule's pattern
*       actionPtr     - points to the TTI rule action that applied on packet
*                       if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiRule_KEY_IPV4_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    rulePm2Format = CPSS_EXMXPM_TTI_KEY_IPV4_E;

    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
        maskPm2_ruleIndex = (GT_U32)inFields[0];
        valid_mask=(GT_BOOL)inFields[2];
        maskPm2Data.ipv4.common.pclId = (GT_U32)inFields[3];
        maskPm2Data.ipv4.common.srcIsTrunk = (GT_U32)inFields[4];
        maskPm2Data.ipv4.common.srcPortTrunk = (GT_U32)inFields[5];
        galtisMacAddr(&maskPm2Data.ipv4.common.mac, (GT_U8*)inFields[6]);
        maskPm2Data.ipv4.common.vid = (GT_U16)inFields[7];
        maskPm2Data.ipv4.common.isTagged = (GT_BOOL)inFields[8];
        maskPm2Data.ipv4.common.dsaSrcIsTrunk= (GT_BOOL)inFields[9];
        maskPm2Data.ipv4.common.dsaSrcPortTrunk= (GT_U8)inFields[10];
        maskPm2Data.ipv4.common.dsaSrcDevice= (GT_U8)inFields[11];
        maskPm2Data.ipv4.tunneltype = (GT_U32)inFields[12];
        galtisIpAddr(&maskPm2Data.ipv4.srcIp, (GT_U8*)inFields[13]);
        galtisIpAddr(&maskPm2Data.ipv4.destIp, (GT_U8*)inFields[14]);
        maskPm2Data.ipv4.isArp= (GT_BOOL)inFields[15];


        maskPm2_set = GT_TRUE;
    }
    else /* pattern */
    {
        patternPm2_ruleIndex = (GT_U32)inFields[0];
        valid_pattern=(GT_BOOL)inFields[2];
        patternPm2Data.ipv4.common.pclId = (GT_U32)inFields[3];
        patternPm2Data.ipv4.common.srcIsTrunk = (GT_U32)inFields[4];
        patternPm2Data.ipv4.common.srcPortTrunk = (GT_U32)inFields[5];
        galtisMacAddr(&patternPm2Data.ipv4.common.mac, (GT_U8*)inFields[6]);
        patternPm2Data.ipv4.common.vid = (GT_U16)inFields[7];
        patternPm2Data.ipv4.common.isTagged = (GT_BOOL)inFields[8];
        patternPm2Data.ipv4.common.dsaSrcIsTrunk= (GT_BOOL)inFields[9];
        patternPm2Data.ipv4.common.dsaSrcPortTrunk= (GT_U8)inFields[10];
        patternPm2Data.ipv4.common.dsaSrcDevice= (GT_U8)inFields[11];
        patternPm2Data.ipv4.tunneltype = (GT_U32)inFields[12];
        galtisIpAddr(&patternPm2Data.ipv4.srcIp, (GT_U8*)inFields[13]);
        galtisIpAddr(&patternPm2Data.ipv4.destIp, (GT_U8*)inFields[14]);
        patternPm2Data.ipv4.isArp= (GT_BOOL)inFields[15];

        patternPm2_set = GT_TRUE;
    }

    return CMD_OK;
}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiRule_KEY_MPLS_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    rulePm2Format = CPSS_EXMXPM_TTI_KEY_MPLS_E;

    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
        maskPm2_ruleIndex = (GT_U32)inFields[0];
        valid_mask=(GT_BOOL)inFields[2];
        maskPm2Data.mpls.common.pclId = (GT_U32)inFields[3];
        maskPm2Data.mpls.common.srcIsTrunk = (GT_U32)inFields[4];
        maskPm2Data.mpls.common.srcPortTrunk = (GT_U32)inFields[5];
        galtisMacAddr(&maskPm2Data.mpls.common.mac, (GT_U8*)inFields[6]);
        maskPm2Data.mpls.common.vid = (GT_U16)inFields[7];
        maskPm2Data.mpls.common.isTagged = (GT_BOOL)inFields[8];
        maskPm2Data.mpls.common.dsaSrcIsTrunk= (GT_BOOL)inFields[9];
        maskPm2Data.mpls.common.dsaSrcPortTrunk= (GT_U8)inFields[10];
        maskPm2Data.mpls.common.dsaSrcDevice= (GT_U8)inFields[11];

        maskPm2Data.mpls.label0=(GT_U32)inFields[12];
        maskPm2Data.mpls.exp0=(GT_U32)inFields[13];
        maskPm2Data.mpls.label1=(GT_U32)inFields[14];
        maskPm2Data.mpls.exp1= (GT_U32)inFields[15];
        maskPm2Data.mpls.label2=(GT_U32)inFields[16];
        maskPm2Data.mpls.exp2=(GT_U32)inFields[17];
        maskPm2Data.mpls.numOfLabels=(GT_U32)inFields[18];
        maskPm2Data.mpls.nlpAboveMPLS=(GT_U32)inFields[19];

        maskPm2_set = GT_TRUE;
    }
    else /* pattern */
    {
        patternPm2_ruleIndex = (GT_U32)inFields[0];
        valid_pattern=(GT_BOOL)inFields[2];
        patternPm2Data.mpls.common.pclId = (GT_U32)inFields[3];
        patternPm2Data.mpls.common.srcIsTrunk = (GT_U32)inFields[4];
        patternPm2Data.mpls.common.srcPortTrunk = (GT_U32)inFields[5];
        galtisMacAddr(&patternPm2Data.mpls.common.mac, (GT_U8*)inFields[6]);
        patternPm2Data.mpls.common.vid = (GT_U16)inFields[7];
        patternPm2Data.mpls.common.isTagged = (GT_BOOL)inFields[8];
        patternPm2Data.mpls.common.dsaSrcIsTrunk= (GT_BOOL)inFields[9];
        patternPm2Data.mpls.common.dsaSrcPortTrunk= (GT_U8)inFields[10];
        patternPm2Data.mpls.common.dsaSrcDevice= (GT_U8)inFields[11];

        patternPm2Data.mpls.label0=(GT_U32)inFields[12];
        patternPm2Data.mpls.exp0=(GT_U32)inFields[13];
        patternPm2Data.mpls.label1=(GT_U32)inFields[14];
        patternPm2Data.mpls.exp1= (GT_U32)inFields[15];
        patternPm2Data.mpls.label2=(GT_U32)inFields[16];
        patternPm2Data.mpls.exp2=(GT_U32)inFields[17];
        patternPm2Data.mpls.numOfLabels=(GT_U32)inFields[18];
        patternPm2Data.mpls.nlpAboveMPLS=(GT_U32)inFields[19];


        patternPm2_set = GT_TRUE;
    }

    return CMD_OK;
}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiRule_KEY_ETH_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    rulePm2Format = CPSS_EXMXPM_TTI_KEY_ETH_E;

    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
        maskPm2_ruleIndex = (GT_U32)inFields[0];
        valid_mask=(GT_BOOL)inFields[2];
        maskPm2Data.eth.common.pclId = (GT_U32)inFields[3];
        maskPm2Data.eth.common.srcIsTrunk = (GT_U32)inFields[4];
        maskPm2Data.eth.common.srcPortTrunk = (GT_U32)inFields[5];
        galtisMacAddr(&maskPm2Data.eth.common.mac, (GT_U8*)inFields[6]);
        maskPm2Data.eth.common.vid = (GT_U16)inFields[7];
        maskPm2Data.eth.common.isTagged = (GT_BOOL)inFields[8];
        maskPm2Data.eth.common.dsaSrcIsTrunk= (GT_BOOL)inFields[9];
        maskPm2Data.eth.common.dsaSrcPortTrunk= (GT_U8)inFields[10];
        maskPm2Data.eth.common.dsaSrcDevice= (GT_U8)inFields[11];


        maskPm2Data.eth.up0= (GT_U32)inFields[12];
        maskPm2Data.eth.cfi0= (GT_U32)inFields[13];
        maskPm2Data.eth.isVlan1Exists= (GT_BOOL)inFields[14];
        maskPm2Data.eth.vid1= (GT_U16)inFields[15];
        maskPm2Data.eth.up1= (GT_U32)inFields[16];
        maskPm2Data.eth.cfi1= (GT_U32)inFields[17];
        maskPm2Data.eth.etherType= (GT_U32)inFields[18];
        maskPm2Data.eth.macToMe= (GT_BOOL)inFields[19];

        maskPm2_set = GT_TRUE;
    }
    else /* pattern */
    {
        patternPm2_ruleIndex = (GT_U32)inFields[0];
        valid_pattern=(GT_BOOL)inFields[2];
        patternPm2Data.eth.common.pclId = (GT_U32)inFields[3];
        patternPm2Data.eth.common.srcIsTrunk = (GT_U32)inFields[4];
        patternPm2Data.eth.common.srcPortTrunk = (GT_U32)inFields[5];
        galtisMacAddr(&patternPm2Data.eth.common.mac, (GT_U8*)inFields[6]);
        patternPm2Data.eth.common.vid = (GT_U16)inFields[7];
        patternPm2Data.eth.common.isTagged = (GT_BOOL)inFields[8];
        patternPm2Data.eth.common.dsaSrcIsTrunk= (GT_BOOL)inFields[9];
        patternPm2Data.eth.common.dsaSrcPortTrunk= (GT_U8)inFields[10];
        patternPm2Data.eth.common.dsaSrcDevice= (GT_U8)inFields[11];


        patternPm2Data.eth.up0= (GT_U32)inFields[12];
        patternPm2Data.eth.cfi0= (GT_U32)inFields[13];
        patternPm2Data.eth.isVlan1Exists= (GT_BOOL)inFields[14];
        patternPm2Data.eth.vid1= (GT_U16)inFields[15];
        patternPm2Data.eth.up1= (GT_U32)inFields[16];
        patternPm2Data.eth.cfi1= (GT_U32)inFields[17];
        patternPm2Data.eth.etherType= (GT_U32)inFields[18];
        patternPm2Data.eth.macToMe= (GT_BOOL)inFields[19];


        patternPm2_set = GT_TRUE;
    }

    return CMD_OK;
}
/*********************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiRule_KEY_MIM_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    rulePm2Format = CPSS_EXMXPM_TTI_KEY_ETH_E;

    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
        maskPm2_ruleIndex = (GT_U32)inFields[0];
        valid_mask=(GT_BOOL)inFields[2];
        maskPm2Data.mim.common.pclId = (GT_U32)inFields[3];
        maskPm2Data.mim.common.srcIsTrunk = (GT_U32)inFields[4];
        maskPm2Data.mim.common.srcPortTrunk = (GT_U32)inFields[5];
        galtisMacAddr(&maskPm2Data.mim.common.mac, (GT_U8*)inFields[6]);
        maskPm2Data.mim.common.vid = (GT_U16)inFields[7];
        maskPm2Data.mim.common.isTagged = (GT_BOOL)inFields[8];
        maskPm2Data.mim.common.dsaSrcIsTrunk= (GT_BOOL)inFields[9];
        maskPm2Data.mim.common.dsaSrcPortTrunk= (GT_U8)inFields[10];
        maskPm2Data.mim.common.dsaSrcDevice= (GT_U8)inFields[11];

        maskPm2Data.mim.bUp= (GT_U32)inFields[12];
        maskPm2Data.mim.bDp= (GT_U32)inFields[13];
        maskPm2Data.mim.iSid= (GT_U32)inFields[14];
        maskPm2Data.mim.iUp= (GT_U32)inFields[15];
        maskPm2Data.mim.iDp= (GT_U32)inFields[16];
        maskPm2Data.mim.iRes1= (GT_U32)inFields[17];
        maskPm2Data.mim.iRes2= (GT_U32)inFields[18];



        maskPm2_set = GT_TRUE;
    }
    else /* pattern */
    {
        patternPm2_ruleIndex = (GT_U32)inFields[0];
        valid_pattern=(GT_BOOL)inFields[2];
        patternPm2Data.mim.common.pclId = (GT_U32)inFields[3];
        patternPm2Data.mim.common.srcIsTrunk = (GT_U32)inFields[4];
        patternPm2Data.mim.common.srcPortTrunk = (GT_U32)inFields[5];
        galtisMacAddr(&patternPm2Data.mim.common.mac, (GT_U8*)inFields[6]);
        patternPm2Data.mim.common.vid = (GT_U16)inFields[7];
        patternPm2Data.mim.common.isTagged = (GT_BOOL)inFields[8];
        patternPm2Data.mim.common.dsaSrcIsTrunk= (GT_BOOL)inFields[9];
        patternPm2Data.mim.common.dsaSrcPortTrunk= (GT_U8)inFields[10];
        patternPm2Data.mim.common.dsaSrcDevice= (GT_U8)inFields[11];

        patternPm2Data.mim.bUp= (GT_U32)inFields[12];
        patternPm2Data.mim.bDp= (GT_U32)inFields[13];
        patternPm2Data.mim.iSid= (GT_U32)inFields[14];
        patternPm2Data.mim.iUp= (GT_U32)inFields[15];
        patternPm2Data.mim.iDp= (GT_U32)inFields[16];
        patternPm2Data.mim.iRes1= (GT_U32)inFields[17];
        patternPm2Data.mim.iRes2= (GT_U32)inFields[18];

        patternPm2_set = GT_TRUE;
    }

    return CMD_OK;
}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiRuleSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&maskPm2Data , 0, sizeof(CPSS_EXMXPM_TTI_RULE_UNT));
    cmdOsMemSet(&patternPm2Data, 0, sizeof(CPSS_EXMXPM_TTI_RULE_UNT));

    maskPm2_set    = GT_FALSE;
    patternPm2_set = GT_FALSE;

    switch (inArgs[2])
    {
        case CPSS_EXMXPM_TTI_KEY_IPV4_E:
            wrCpssExMxPm2TtiRule_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_MPLS_E:
            wrCpssExMxPm2TtiRule_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_ETH_E:
            wrCpssExMxPm2TtiRule_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_MIM_E:
            wrCpssExMxPm2TtiRule_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
            break;
        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
            return CMD_AGENT_ERROR;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiRuleSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    GT_U32                              actionIndex;
    GT_U32                              index;
    CPSS_EXMXPM_TTI_ACTION_UNT          *actionPtr;
    CPSS_EXMXPM_TCAM_TYPE_ENT            tcamType;
    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT   ruleInfo;
    GT_BOOL                                 valid;
    WR_INTERNAL_ACTION_STC                  *tablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tcamType=(CPSS_EXMXPM_TCAM_TYPE_ENT)inArgs[1];
    /* Check Validity */
    if(inFields[1] > 1)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be MASK or PATTERN).\n");
        return CMD_AGENT_ERROR;
    }

    if(inFields[1] == 0 && maskPm2_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if(inFields[1] == 1 && patternPm2_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }


    switch (inArgs[2])
    {
        case CPSS_EXMXPM_TTI_KEY_IPV4_E:
            wrCpssExMxPm2TtiRule_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_MPLS_E:
            wrCpssExMxPm2TtiRule_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_ETH_E:
            wrCpssExMxPm2TtiRule_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_MIM_E:
            wrCpssExMxPm2TtiRule_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
            break;
        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
            return CMD_AGENT_ERROR;
    }

    if(maskPm2_set && patternPm2_set && (maskPm2_ruleIndex != patternPm2_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }


    if(tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
    {
       ruleInfo.internalTcamRuleStartIndex=(GT_U32)inFields[0];
       tablePtr=actionPm2Table;
    }

    else
    {   /*bind extTcam params*/
       prvPm2TtiExtConfigGet(inFields[0],&index);

       ruleInfo.externalTcamInfo.ruleInfo.tcamId=extTcamConfigPm2Table[index].tcamId;
       ruleInfo.externalTcamInfo.ruleInfo.ruleStartIndex=extTcamConfigPm2Table[index].ruleStartIndex;
       ruleInfo.externalTcamInfo.ruleInfo.ruleSize=extTcamConfigPm2Table[index].ruleSize;
       ruleInfo.externalTcamInfo.ruleInfo.ageEnable=extTcamConfigPm2Table[index].ageEnable;

       ruleInfo.externalTcamInfo.actionInfo.actionStartIndex=extTcamConfigPm2Table[index].actionStartIndex;
       ruleInfo.externalTcamInfo.actionInfo.memoryType=extTcamConfigPm2Table[index].memoryType;
       tablePtr=actionExtPm2Table;
    }

    /* Get the action */
    prvPm2TtiActionGet(devNum, inFields[0],tablePtr,&actionIndex);

    if(maskPm2_set && maskPm2_set && (actionIndex != INVALID_RULE_INDEX))
    {
        actionPtr = &(tablePtr[actionIndex].actionEntry);

        keyType = (CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[2];
        if(tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
          typesOfIntEntry[actionPm2Table[actionIndex].ruleIndex]=keyType;
        else
           typesOfExtEntry[actionExtPm2Table[actionIndex].ruleIndex]=keyType;

        if(valid_mask==valid_pattern)
             valid=valid_mask;
        else
            {
                /* pack output arguments to galtis string */
                 galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask and pattern valid  doesn't match \n");
                 return CMD_AGENT_ERROR;
             }

        /* call cpss api function */
        result = cpssExMxPmTtiRuleSet(devNum,tcamType,&ruleInfo,
                                       keyType,tablePtr[actionIndex].actionType,valid,
                                       &maskPm2Data,&patternPm2Data, actionPtr);




        maskPm2_set = GT_FALSE;
        patternPm2_set = GT_FALSE;

        cmdOsMemSet(&maskPm2Data , 0, sizeof(CPSS_EXMXPM_TTI_RULE_UNT));
        cmdOsMemSet(&patternPm2Data, 0, sizeof(CPSS_EXMXPM_TTI_RULE_UNT));
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiRuleEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if(maskPm2_set || patternPm2_set)
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action for specific
*       TCAM location according to the rule Key Type.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_ETH_E
                           CPSS_EXMXPM_TTI_KEY_MIM_E
*       ruleIndex     - Index of the rule in the internal or external TCAM.
*       actionType    - TTI action type; valid values:
*                           CPSS_EXMXPM_TTI_ACTION_REDUCED_E
*                           CPSS_EXMXPM_TTI_ACTION_STANDARD_E
*                           CPSS_EXMXPM_TTI_ACTION_EXTENDED_E
*       lookupMode    - TTI lookup mode; valid valuse:
*                           CPSS_EXMXPM_TTI_LOOKUP_MODE_EXT_TCAM_E
*                           CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E
*
* OUTPUTS:
*       patternPtr    - points to the rule's pattern
*       maskPtr       - points to the rule's mask. The rule mask is "AND STYLED
*                       ONE". Mask bit's 0 means don't care bit (corresponding
*                       bit in the pattern is not used in the TCAM lookup).
*                       Mask bit's 1 means that corresponding bit in the pattern
*                       is using in the TCAM lookup.
*       actionPtr     - points to the TTI rule action that applied on packet
*                       if packet's search key matched with masked pattern.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U32 rulePm2Index;
static GT_BOOL isPm2Mask;

static CMD_STATUS wrCpssExMxPm2TtiRule_KEY_IPV4_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    rulePm2Format = CPSS_EXMXPM_TTI_KEY_IPV4_E;

    if(isPm2Mask) /* mask */
    {
        inFields[0] = rulePm2Index;
        inFields[1] = 0;
        inFields[2] = valid_mask;
        inFields[3]= maskPm2Data.ipv4.common.pclId;
        inFields[4]= maskPm2Data.ipv4.common.srcIsTrunk;
        inFields[5]= maskPm2Data.ipv4.common.srcPortTrunk;
        inFields[7]= maskPm2Data.ipv4.common.vid;
        inFields[8]= maskPm2Data.ipv4.common.isTagged;
        inFields[9]= maskPm2Data.ipv4.common.dsaSrcIsTrunk;
        inFields[10]= maskPm2Data.ipv4.common.dsaSrcPortTrunk;
        inFields[11]=maskPm2Data.ipv4.common.dsaSrcDevice;
        inFields[12]=maskPm2Data.ipv4.tunneltype;
        inFields[15]=maskPm2Data.ipv4.isArp;

        isPm2Mask = GT_FALSE;

        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%4b%4b%d",
                    inFields[0], inFields[1],  inFields[2], inFields[3],
                    inFields[4],inFields[5], maskPm2Data.ipv4.common.mac.arEther, inFields[7],
                    inFields[8],inFields[9], inFields[10],inFields[11],inFields[12],
                    maskPm2Data.ipv4.srcIp.arIP, maskPm2Data.ipv4.destIp.arIP, inFields[15]);


    }
    else /* pattern */
    {
        inFields[0] = rulePm2Index;
        inFields[1] = 1;
        inFields[2] = valid_mask;
        inFields[3]= patternPm2Data.ipv4.common.pclId;
        inFields[4]= patternPm2Data.ipv4.common.srcIsTrunk;
        inFields[5]= patternPm2Data.ipv4.common.srcPortTrunk;
        inFields[7]= patternPm2Data.ipv4.common.vid;
        inFields[8]= patternPm2Data.ipv4.common.isTagged;
        inFields[9]= patternPm2Data.ipv4.common.dsaSrcIsTrunk;
        inFields[10]= patternPm2Data.ipv4.common.dsaSrcPortTrunk;
        inFields[11]=patternPm2Data.ipv4.common.dsaSrcDevice;
        inFields[12]=patternPm2Data.ipv4.tunneltype;
        inFields[15]=patternPm2Data.ipv4.isArp;

        isPm2Mask = GT_TRUE;

         fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%4b%4b%d",
                    inFields[0], inFields[1],  inFields[2], inFields[3],
                    inFields[4],inFields[5], patternPm2Data.ipv4.common.mac.arEther, inFields[7],
                    inFields[8],inFields[9], inFields[10],inFields[11],inFields[12],
                    patternPm2Data.ipv4.srcIp.arIP, patternPm2Data.ipv4.destIp.arIP, inFields[15]);
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", rulePm2Format);

    return CMD_OK;
}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiRule_KEY_MPLS_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    rulePm2Format = CPSS_EXMXPM_TTI_KEY_MPLS_E;

    if(isPm2Mask) /* mask */
    {
        inFields[0] = rulePm2Index;
        inFields[1] = 0;
        inFields[2] = valid_mask;

        inFields[3]=maskPm2Data.mpls.common.pclId;
        inFields[4]=maskPm2Data.mpls.common.srcIsTrunk;
        inFields[5]=maskPm2Data.mpls.common.srcPortTrunk;

        inFields[7]=maskPm2Data.mpls.common.vid;
        inFields[8]=maskPm2Data.mpls.common.isTagged;
        inFields[9]=maskPm2Data.mpls.common.dsaSrcIsTrunk;
        inFields[10]=maskPm2Data.mpls.common.dsaSrcPortTrunk;
        inFields[11]=maskPm2Data.mpls.common.dsaSrcDevice;

        inFields[12]=maskPm2Data.mpls.label0;
        inFields[13]=maskPm2Data.mpls.exp0;
        inFields[14]=maskPm2Data.mpls.label1;
        inFields[15]=maskPm2Data.mpls.exp1;
        inFields[16]=maskPm2Data.mpls.label2;
        inFields[17]=maskPm2Data.mpls.exp2;
        inFields[18]=maskPm2Data.mpls.numOfLabels;
        inFields[19]=maskPm2Data.mpls.nlpAboveMPLS;

        isPm2Mask = GT_FALSE;

        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4],inFields[5], maskPm2Data.mpls.common.mac.arEther,inFields[7],
                    inFields[8], inFields[9], inFields[10],  inFields[11],
                    inFields[12], inFields[13],inFields[14], inFields[15],
                    inFields[16], inFields[17],inFields[18],inFields[19]);
    }
    else /* pattern */
    {
        inFields[0] = rulePm2Index;
        inFields[1] = 1;
        inFields[2] = valid_mask;

        inFields[3]=patternPm2Data.mpls.common.pclId;
        inFields[4]=patternPm2Data.mpls.common.srcIsTrunk;
        inFields[5]=patternPm2Data.mpls.common.srcPortTrunk;

        inFields[7]=patternPm2Data.mpls.common.vid;
        inFields[8]=patternPm2Data.mpls.common.isTagged;
        inFields[9]=patternPm2Data.mpls.common.dsaSrcIsTrunk;
        inFields[10]=patternPm2Data.mpls.common.dsaSrcPortTrunk;
        inFields[11]=patternPm2Data.mpls.common.dsaSrcDevice;

        inFields[12]=patternPm2Data.mpls.label0;
        inFields[13]=patternPm2Data.mpls.exp0;
        inFields[14]=patternPm2Data.mpls.label1;
        inFields[15]=patternPm2Data.mpls.exp1;
        inFields[16]=patternPm2Data.mpls.label2;
        inFields[17]=patternPm2Data.mpls.exp2;
        inFields[18]=patternPm2Data.mpls.numOfLabels;
        inFields[19]=patternPm2Data.mpls.nlpAboveMPLS;

        isPm2Mask = GT_TRUE;

        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4],inFields[5], patternPm2Data.mpls.common.mac.arEther,inFields[7],
                    inFields[8], inFields[9], inFields[10],  inFields[11],
                    inFields[12], inFields[13],inFields[14], inFields[15],
                    inFields[16], inFields[17],inFields[18],inFields[19]);
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", rulePm2Format);
    return CMD_OK;

}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiRule_KEY_ETH_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    rulePm2Format = CPSS_EXMXPM_TTI_KEY_ETH_E;

    if(isPm2Mask) /* mask */
    {
        inFields[0] = rulePm2Index;
        inFields[1] = 0;

        isPm2Mask = GT_FALSE;

        inFields[2] = valid_mask;
        inFields[3]=maskPm2Data.eth.common.pclId;
        inFields[4]=maskPm2Data.eth.common.srcIsTrunk;
        inFields[5]=maskPm2Data.eth.common.srcPortTrunk;

        inFields[7]=maskPm2Data.eth.common.vid;
        inFields[8]=maskPm2Data.eth.common.isTagged;
        inFields[9]= maskPm2Data.eth.common.dsaSrcIsTrunk ;
        inFields[10]=maskPm2Data.eth.common.dsaSrcPortTrunk;
        inFields[11]=maskPm2Data.eth.common.dsaSrcDevice;


        inFields[12]=maskPm2Data.eth.up0;
        inFields[13]=maskPm2Data.eth.cfi0;
        inFields[14]=maskPm2Data.eth.isVlan1Exists;
        inFields[15]=maskPm2Data.eth.vid1;
        inFields[16]=maskPm2Data.eth.up1;
        inFields[17]=maskPm2Data.eth.cfi1;
        inFields[18]=maskPm2Data.eth.etherType;
        inFields[19]=maskPm2Data.eth.macToMe;


        isPm2Mask = GT_FALSE;

        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4],inFields[5], maskPm2Data.eth.common.mac.arEther,  inFields[7],
                    inFields[8], inFields[9], inFields[10],  inFields[11],
                    inFields[12],inFields[13],inFields[14], inFields[15],
                    inFields[16], inFields[17],inFields[18],inFields[19]);
    }
    else /* pattern */
    {
        inFields[0] = rulePm2Index;
        inFields[1] = 1;


        inFields[2] = valid_mask;
        inFields[3]=patternPm2Data.eth.common.pclId;
        inFields[4]=patternPm2Data.eth.common.srcIsTrunk;
        inFields[5]=patternPm2Data.eth.common.srcPortTrunk;

        inFields[7]=patternPm2Data.eth.common.vid;
        inFields[8]=patternPm2Data.eth.common.isTagged;
        inFields[9]= patternPm2Data.eth.common.dsaSrcIsTrunk ;
        inFields[10]=patternPm2Data.eth.common.dsaSrcPortTrunk;
        inFields[11]=patternPm2Data.eth.common.dsaSrcDevice;


        inFields[12]=patternPm2Data.eth.up0;
        inFields[13]=patternPm2Data.eth.cfi0;
        inFields[14]=patternPm2Data.eth.isVlan1Exists;
        inFields[15]=patternPm2Data.eth.vid1;
        inFields[16]=patternPm2Data.eth.up1;
        inFields[17]=patternPm2Data.eth.cfi1;
        inFields[18]=patternPm2Data.eth.etherType;
        inFields[19]=patternPm2Data.eth.macToMe;

        isPm2Mask = GT_TRUE;

       fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4],inFields[5], patternPm2Data.eth.common.mac.arEther,  inFields[7],
                    inFields[8], inFields[9], inFields[10],  inFields[11],
                    inFields[12],inFields[13],inFields[14], inFields[15],
                    inFields[16], inFields[17],inFields[18],inFields[19]);
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", rulePm2Format);
    return CMD_OK;
}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiRule_MIM_E_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    rulePm2Format = CPSS_EXMXPM_TTI_KEY_MIM_E;

    if(isPm2Mask) /* mask */
    {
        inFields[0] = rulePm2Index;
        inFields[1] = 0;

        isPm2Mask = GT_FALSE;
        inFields[2] = valid_mask;
        inFields[3]=maskPm2Data.mim.common.pclId ;
        inFields[4]=maskPm2Data.mim.common.srcIsTrunk ;
        inFields[5]=maskPm2Data.mim.common.srcPortTrunk ;

        inFields[7]=maskPm2Data.mim.common.vid ;
        inFields[8]=maskPm2Data.mim.common.isTagged;
        inFields[9]=maskPm2Data.mim.common.dsaSrcIsTrunk;
        inFields[10]=maskPm2Data.mim.common.dsaSrcPortTrunk;
        inFields[11]=maskPm2Data.mim.common.dsaSrcDevice;

        inFields[12]=maskPm2Data.mim.bUp;
        inFields[13]=maskPm2Data.mim.bDp;
        inFields[14]=maskPm2Data.mim.iSid;
        inFields[15]=maskPm2Data.mim.iUp;
        inFields[16]=maskPm2Data.mim.iDp;
        inFields[17]=maskPm2Data.mim.iRes1;
        inFields[18]=maskPm2Data.mim.iRes2;

        isPm2Mask = GT_FALSE;

        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4],inFields[5], maskPm2Data.mim.common.mac.arEther,  inFields[7],
                    inFields[8], inFields[9], inFields[10],  inFields[11],
                    inFields[12],inFields[13],inFields[14], inFields[15],
                    inFields[16], inFields[17],inFields[18]);
    }
    else /* pattern */
    {
        inFields[0] = rulePm2Index;
        inFields[1] = 1;

        inFields[2] = valid_mask;
        inFields[3]=patternPm2Data.mim.common.pclId ;
        inFields[4]=patternPm2Data.mim.common.srcIsTrunk ;
        inFields[5]=patternPm2Data.mim.common.srcPortTrunk ;

        inFields[7]=patternPm2Data.mim.common.vid ;
        inFields[8]=patternPm2Data.mim.common.isTagged;
        inFields[9]=patternPm2Data.mim.common.dsaSrcIsTrunk;
        inFields[10]=patternPm2Data.mim.common.dsaSrcPortTrunk;
        inFields[11]=patternPm2Data.mim.common.dsaSrcDevice;

        inFields[12]=patternPm2Data.mim.bUp;
        inFields[13]=patternPm2Data.mim.bDp;
        inFields[14]=patternPm2Data.mim.iSid;
        inFields[15]=patternPm2Data.mim.iUp;
        inFields[16]=patternPm2Data.mim.iDp;
        inFields[17]=patternPm2Data.mim.iRes1;
        inFields[18]=patternPm2Data.mim.iRes2;




        isPm2Mask = GT_TRUE;

        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4],inFields[5], patternPm2Data.mim.common.mac.arEther,  inFields[7],
                    inFields[8], inFields[9], inFields[10],  inFields[11],
                    inFields[12],inFields[13],inFields[14], inFields[15],
                    inFields[16], inFields[17],inFields[18]);
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", rulePm2Format);
    return CMD_OK;
}
/*******************************************************************************/

static CMD_STATUS wrCpssExMxPm2TtiRuleGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_EXMXPM_TCAM_TYPE_ENT           tcamType;
    CPSS_EXMXPM_TTI_ACTION_UNT          action;
    GT_U32                              actionIndex;
    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT   ruleInfo;
    WR_INTERNAL_ACTION_STC                  *tablePtr;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT             *typePtr;

    /* if isMask == GT_TRUE it is mask's turn (the first one to get), so there need to refresh the data
        else the data is updated and it is pattern's turn (was updated earlier)*/

     tcamType=(CPSS_EXMXPM_TCAM_TYPE_ENT)inArgs[1];

       if(tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
      {

         tablePtr=actionPm2Table;
         typePtr=typesOfIntEntry;
      }

      else
      {
         tablePtr=actionExtPm2Table;
         typePtr=typesOfExtEntry;
      }

    if (isPm2Mask)
    {
        /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];



        actionIndex = INVALID_RULE_INDEX;
        for(; actionIndex == INVALID_RULE_INDEX && rulePm2Index < 1024; rulePm2Index++)
            prvPm2TtiActionGet(devNum, rulePm2Index,tablePtr, &actionIndex);




        if (rulePm2Index == 1024)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_OK, "%d", -1);

            return CMD_OK;
        }

        rulePm2Index--;
        prvPm2TtiActionGet(devNum, rulePm2Index, tablePtr,&actionIndex);

        /*TODO:add external support*/

        ruleInfo.internalTcamRuleStartIndex=rulePm2Index;

        valid_mask=GT_FALSE;/*valid init*/
        valid_pattern=GT_FALSE;
        /* call cpss api function */
        result = cpssExMxPmTtiRuleGet(devNum,tcamType,
                                      &ruleInfo, typePtr[rulePm2Index],
                                      tablePtr[actionIndex].actionType,
                                      &valid_mask,
                                      &maskPm2Data,&patternPm2Data,
                                      &action);
    }


    switch (typePtr[rulePm2Index])
    {
        case CPSS_EXMXPM_TTI_KEY_IPV4_E:
            wrCpssExMxPm2TtiRule_KEY_IPV4_Get(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_MPLS_E:
            wrCpssExMxPm2TtiRule_KEY_MPLS_Get(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_ETH_E:
            wrCpssExMxPm2TtiRule_KEY_ETH_Get(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_TTI_KEY_MIM_E:
            wrCpssExMxPm2TtiRule_MIM_E_Get(inArgs,inFields,numFields,outArgs);
            break;

        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
            return CMD_AGENT_ERROR;
    }

    if (isPm2Mask)
        rulePm2Index++;

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiRuleGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    rulePm2Index = 0;

    /* first to get is mask */
    isPm2Mask = GT_TRUE;

    return wrCpssExMxPm2TtiRuleGet(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* cpssExMxPmTtiPortLookupModeSet
*
* DESCRIPTION:
*       This function sets the lookup mode for all key types (lookup mode
*       defines if the lookup would take place over internal or external TCAM).
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_ETH_E
*                           CPSS_EXMXPM_TTI_KEY_MIM_E
*       mode          - lookup mode; valid values:
*                           CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E
*                           CPSS_EXMXPM_TTI_LOOKUP_MODE_EXT_TCAM_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiPortLookupModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    CPSS_EXMXPM_TTI_LOOKUP_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    keyType = (CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[2];
    if (inArgs[3] == 1)
        mode = CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E;
    else
        mode = CPSS_EXMXPM_TTI_LOOKUP_MODE_EXT_TCAM_E;
    /*mode = (CPSS_EXMXPM_TTI_LOOKUP_MODE_ENT)inArgs[3];*/

    /* call cpss api function */
    result = cpssExMxPmTtiPortLookupModeSet(devNum, port, keyType, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiPortLookupModeGet
*
* DESCRIPTION:
*       This function gets the lookup mode for all key types (lookup mode
*       defines if the lookup would take place over internal or external TCAM).
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_ETH_E
*                           CPSS_EXMXPM_TTI_KEY_MIM_E
*
* OUTPUTS:
*       modePtr       - lookup mode; valid values:
*                           CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E
*                           CPSS_EXMXPM_TTI_LOOKUP_MODE_EXT_TCAM_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on data bad value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiPortLookupModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    CPSS_EXMXPM_TTI_LOOKUP_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    keyType = (CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmTtiPortLookupModeGet(devNum, port, keyType, &mode);

    /*fix due to Galtis db mistake

      in Galtis GUI:
      CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E=1
      CPSS_EXMXPM_TTI_LOOKUP_MODE_EXT_TCAM_E=0
    */

    if (mode== 1)
        mode = CPSS_EXMXPM_TTI_LOOKUP_MODE_INT_TCAM_E;
    else
        mode = CPSS_EXMXPM_TTI_LOOKUP_MODE_EXT_TCAM_E;



    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiPortLookupConfigSet
*
* DESCRIPTION:
*       This function sets the lookup configuration.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_ETH_E
*                           CPSS_EXMXPM_TTI_KEY_MIM_E
*       configPtr     - points to lookup configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiPortLookupConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    CPSS_EXMXPM_TTI_LOOKUP_CONFIG_STC config;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port= (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);


    keyType=(CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[2];
    config.actionType=(CPSS_EXMXPM_TTI_ACTION_TYPE_ENT)inArgs[3];
    config.pclId=(GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmTtiPortLookupConfigSet(devNum, port, keyType, &config);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiPortLookupConfigGet
*
* DESCRIPTION:
*       This function gets the lookup configuration.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_ETH_E
*                           CPSS_EXMXPM_TTI_KEY_MIM_E
*
* OUTPUTS:
*       configPtr     - points to lookup configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiPortLookupConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    CPSS_EXMXPM_TTI_LOOKUP_CONFIG_STC config;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port= (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    keyType=(CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[2];


    /* call cpss api function */
    result = cpssExMxPmTtiPortLookupConfigGet(devNum, port, keyType, &config);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",config.actionType,config.pclId);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiPortMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI MacToMe filter at the TTI
*       trigger phase (incoming packets would bypass the TTI lookup if their
*       DST MAC & VLAN doesn't fall in the MacToMe/VlanToMe range).
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_MIM_E
*       enable        - GT_TRUE: enable MacToMe check
*                       GT_FALSE: disable MacToMe check
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or key type
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiPortMacToMeEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    keyType = (CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmTtiPortMacToMeEnableSet(devNum, port, keyType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiPortMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of  the TTI
*       MacToMe filter.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_EXMXPM_TTI_KEY_IP_V4_E
*                           CPSS_EXMXPM_TTI_KEY_MPLS_E
*                           CPSS_EXMXPM_TTI_KEY_MIM_E
*
* OUTPUTS:
*       enablePtr     - points to enable/disable MacToMe check
*                       GT_TRUE: MacToMe check is enabled
*                       GT_FALSE: MacToMe check is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or key type
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiPortMacToMeEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT keyType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    keyType = (CPSS_EXMXPM_TTI_KEY_TYPE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmTtiPortMacToMeEnableGet(devNum, port, keyType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmTtiMimEtherTypeSet
*
* DESCRIPTION:
*       This function sets the Mac in Mac Ether type.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       ethType       - Ethernet type value (range 16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiMimEtherTypeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ethType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ethType = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTtiMimEtherTypeSet(devNum, ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiMimEtherTypeGet
*
* DESCRIPTION:
*       This function gets the Mac in Mac Ether type.

*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       ethTypePtr    - points to Ethernet type value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiMimEthTypeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ethType = 0;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmTtiMimEthTypeGet(devNum, &ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ethType);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmTtiMacToMeSet
*
* DESCRIPTION:
*       This function sets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MACTOME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MACTOME
*       lookup is performed and the internal flag is set accordingly.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - Index of mac and vlan in MacToMe table (0..127)
*       valid             - If the entry is valid
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiMacToMeSet
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
    CPSS_EXMXPM_TTI_MAC_VLAN_STC value;
    CPSS_EXMXPM_TTI_MAC_VLAN_STC mask;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum= (GT_U8)inArgs[0];
    entryIndex= (GT_U32)inArgs[1];
    valid=(GT_BOOL)inArgs[2];
    galtisMacAddr(&value.mac,(GT_U8*)inArgs[3]);
    value.vlanId=(GT_U16)inArgs[4];
    galtisMacAddr(&mask.mac,(GT_U8*)inArgs[5]);
    mask.vlanId=(GT_U16)inArgs[6];



    /* call cpss api function */
    result = cpssExMxPmTtiMacToMeSet(devNum, entryIndex, valid, &value, &mask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiMacToMeGet
*
* DESCRIPTION:
*       This function gets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MACTOME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MACTOME
*       lookup is performed and the internal flag is set accordingly.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - Index of mac and vlan in MacToMe table (0..127).
*
* OUTPUTS:
*       validPtr          - points if the entry is valid
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2TtiMacToMeGet
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
    CPSS_EXMXPM_TTI_MAC_VLAN_STC value;
    CPSS_EXMXPM_TTI_MAC_VLAN_STC mask;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum= (GT_U8)inArgs[0];
    entryIndex= (GT_U32)inArgs[1];



    /* call cpss api function */
    result = cpssExMxPmTtiMacToMeGet(devNum, entryIndex, &valid, &value, &mask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%6b%d%6b%d",valid,value.mac.arEther,value.vlanId,mask.mac.arEther,mask.vlanId);


    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiRuleDefaultActionSet
*
* DESCRIPTION:
*       This function sets the global default action (used by standard
*       actions).
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum           - device number
*       defaultQosParamsPtr - points to the default TTI rule action qos
*                       parameters that applied on packet if packet's
*                       search key matched with masked pattern
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleDefaultActionSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_ACTION_QOS_STC  defaultQosParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    defaultQosParams.qosParams.keepPreviousQoS = (GT_BOOL)inFields[0];
    defaultQosParams.qosParams.trustUp = (GT_BOOL)inFields[1];
    defaultQosParams.qosParams.trustDscp = (GT_BOOL)inFields[2];
    defaultQosParams.qosParams.trustExp = (GT_BOOL)inFields[3];
    defaultQosParams.qosParams.enableUpToUpRemapping = (GT_BOOL)inFields[4];
    defaultQosParams.qosParams.enableDscpToDscpRemapping = (GT_BOOL)inFields[5];
    defaultQosParams.qosParams.enableCfiToDpMapping = (GT_BOOL)inFields[6];
    defaultQosParams.qosParams.up = (GT_U32)inFields[7];
    defaultQosParams.qosParams.dscp = (GT_U32)inFields[8];
    defaultQosParams.qosParams.tc = (GT_U32)inFields[9];
    defaultQosParams.qosParams.dp = (CPSS_DP_LEVEL_ENT)inFields[10];
    defaultQosParams.qosParams.modifyDscp = (GT_BOOL)inFields[11];
    defaultQosParams.qosParams.modifyUp = (GT_BOOL)inFields[12];
    defaultQosParams.qosParams.modifyExp = (GT_BOOL)inFields[13];
    defaultQosParams.qosParams.qosPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[14];
    defaultQosParams.exp = (GT_U32)inFields[15];

    /* call cpss api function */
    result = cpssExMxPmTtiRuleDefaultActionSet(devNum, &defaultQosParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmTtiRuleDefaultActionGet
*
* DESCRIPTION:
*       This function gets the global default action qos parameters.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       defaultQosParamsPtr - points to the default TTI rule action qos
*                       parameters that applied on packet if packet's
*                       search key matched with masked pattern
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleDefaultActionGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TTI_ACTION_QOS_STC  defaultQosParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

   /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmTtiRuleDefaultActionGet(devNum, &defaultQosParams);

    if(result !=GT_OK)
    {
     galtisOutput(outArgs, result, "");
     return CMD_OK;
    }

    inFields[0] = defaultQosParams.qosParams.keepPreviousQoS;
    inFields[1] = defaultQosParams.qosParams.trustUp;
    inFields[2] = defaultQosParams.qosParams.trustDscp;
    inFields[3] = defaultQosParams.qosParams.trustExp;
    inFields[4] = defaultQosParams.qosParams.enableUpToUpRemapping;
    inFields[5] = defaultQosParams.qosParams.enableDscpToDscpRemapping;
    inFields[6] = defaultQosParams.qosParams.enableCfiToDpMapping;
    inFields[7] = defaultQosParams.qosParams.up;
    inFields[8] = defaultQosParams.qosParams.dscp;
    inFields[9] = defaultQosParams.qosParams.tc;
    inFields[10] = defaultQosParams.qosParams.dp;
    inFields[11] = defaultQosParams.qosParams.modifyDscp;
    inFields[12] = defaultQosParams.qosParams.modifyUp;
    inFields[13] = defaultQosParams.qosParams.modifyExp;
    inFields[14] = defaultQosParams.qosParams.qosPrecedence;
    inFields[15] = defaultQosParams.exp;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                                                    inFields[3],  inFields[4],  inFields[5],
                                                    inFields[6],  inFields[7],  inFields[8],
                                                    inFields[9],  inFields[10], inFields[11],
                                                    inFields[12], inFields[13], inFields[14],
                                                    inFields[15]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTtiRuleDefaultActionGet
*
* DESCRIPTION:
*       This function gets the global default action qos parameters.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       defaultQosParamsPtr - points to the default TTI rule action qos
*                       parameters that applied on packet if packet's
*                       search key matched with masked pattern
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleDefaultActionGetNext
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
* cpssExMxPmTtiRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in internal/external TCAM.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       tcamType      - TCAM type: internal or external
*       ruleInfoPtr   - (pointer to) the Rule and Action Layouts
*       valid         - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that when using external TCAM this operation requires reading,
*       modifying and wring back the entire rule
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleValidStatusSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_EXMXPM_TCAM_TYPE_ENT               tcamType;
    GT_U32                                  ruleStartIndex;
    GT_BOOL                                 valid;

    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT   ruleInfo;
    GT_U32                                  index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

   /* map input arguments to locals */
   devNum = (GT_U8)inArgs[0];
   tcamType = (CPSS_EXMXPM_TCAM_TYPE_ENT)inArgs[1];
   ruleStartIndex = (GT_U32)inArgs[2];
   valid = (GT_BOOL)inArgs[3];

   if(tcamType == CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
   {
       ruleInfo.internalTcamRuleStartIndex = ruleStartIndex;
   }
   else
   {
       prvPm2TtiExtConfigGet(ruleStartIndex,&index);
       if (index ==INVALID_RULE_INDEX)
       {
           /* pack output arguments to galtis string */
           galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nExternal Tcam configs  not set\n");

           return CMD_AGENT_ERROR;
       }

       /*bind extTcam params*/
       ruleInfo.externalTcamInfo.ruleInfo.tcamId=extTcamConfigPm2Table[index].tcamId;
       ruleInfo.externalTcamInfo.ruleInfo.ruleStartIndex=extTcamConfigPm2Table[index].ruleStartIndex;
       ruleInfo.externalTcamInfo.ruleInfo.ruleSize=extTcamConfigPm2Table[index].ruleSize;
       ruleInfo.externalTcamInfo.ruleInfo.ageEnable=extTcamConfigPm2Table[index].ageEnable;
    }

   /* call cpss api function */
    result = cpssExMxPmTtiRuleValidStatusSet(devNum,tcamType,&ruleInfo, valid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;

}

/*******************************************************************************
* cpssExMxPmTtiRuleValidStatusGet
*
* DESCRIPTION:
*       The function gets state (valid or not and rule size) of the
*       Rule in internal/external TCAM.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       tcamType      - TCAM type: internal or external
*       ruleInfoPtr   - (pointer to) the Rule and Action Layouts
*
*
* OUTPUTS:
*       validPtr      - (pointer to) GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTtiRuleValidStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_EXMXPM_TCAM_TYPE_ENT               tcamType;
    GT_U32                                  ruleStartIndex;
    GT_BOOL                                 valid;

    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT   ruleInfo;
    GT_U32                                  index;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tcamType = (CPSS_EXMXPM_TCAM_TYPE_ENT)inArgs[1];
    ruleStartIndex = (GT_U32)inArgs[2];

    if(tcamType == CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
    {
       ruleInfo.internalTcamRuleStartIndex = ruleStartIndex;
    }
    else
    {
       prvPm2TtiExtConfigGet(ruleStartIndex,&index);
       if (index ==INVALID_RULE_INDEX)
       {
           /* pack output arguments to galtis string */
           galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nExternal Tcam configs  not set\n");

           return CMD_AGENT_ERROR;
       }

       /*bind extTcam params*/
       ruleInfo.externalTcamInfo.ruleInfo.tcamId=extTcamConfigPm2Table[index].tcamId;
       ruleInfo.externalTcamInfo.ruleInfo.ruleStartIndex=extTcamConfigPm2Table[index].ruleStartIndex;
       ruleInfo.externalTcamInfo.ruleInfo.ruleSize=extTcamConfigPm2Table[index].ruleSize;
       ruleInfo.externalTcamInfo.ruleInfo.ageEnable=extTcamConfigPm2Table[index].ageEnable;
    }

    /* call cpss api function */
    result = cpssExMxPmTtiRuleValidStatusGet(devNum,tcamType,&ruleInfo,&valid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",valid);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmTtiLookupModeSet",
         &wrCpssExMxPmTtiLookupModeSet,
         2, 0},
        {"cpssExMxPmTtiLookupModeGet",
         &wrCpssExMxPmTtiLookupModeGet,
         1, 0},
        {"cpssExMxPmTtiLookupConfigSet",
         &wrCpssExMxPmTtiLookupConfigSet,
        5, 0},
        {"cpssExMxPmTtiLookupConfigGet",
         &wrCpssExMxPmTtiLookupConfigGet,
        2, 0},
        {"cpssExMxPmTtiMacModeSet",
         &wrCpssExMxPmTtiMacModeSet,
         3, 0},
        {"cpssExMxPmTtiMacModeGet",
         &wrCpssExMxPmTtiMacModeGet,
         2, 0},
        {"cpssExMxPmTtiIpv4McEnableSet",
         &wrCpssExMxPmTtiIpv4McEnableSet,
         2, 0},
        {"cpssExMxPmTtiIpv4McEnableGet",
         &wrCpssExMxPmTtiIpv4McEnableGet,
         1, 0},
        {"cpssExMxPmTtiIpv4GreEthTypeSet",
         &wrCpssExMxPmTtiIpv4GreEthTypeSet,
         3, 0},
        {"cpssExMxPmTtiIpv4GreEthTypeGet",
         &wrCpssExMxPmTtiIpv4GreEthTypeGet,
         2, 0},
        {"cpssExMxPmTtiMacToMeEnableSet",
         &wrCpssExMxPmTtiMacToMeEnableSet,
         3, 0},
        {"cpssExMxPmTtiMacToMeEnableGet",
         &wrCpssExMxPmTtiMacToMeEnableGet,
         2, 0},
        {"cpssExMxPmTtiMacToMeSet",
        &wrCpssExMxPmTtiMacToMeSet,
         5, 0},
        {"cpssExMxPmTtiMacToMeGet",
         &wrCpssExMxPmTtiMacToMeGet,
         1, 0},
        {"cpssExMxPmTtiPortLookupEnableSet",
         &wrCpssExMxPmTtiPortLookupEnableSet,
         4, 0},
        {"cpssExMxPmTtiPortLookupEnableGet",
         &wrCpssExMxPmTtiPortLookupEnableGet,
         3, 0},
        {"cpssExMxPmTtiPortIpv4OnlyTunneledEnableSet",
         &wrCpssExMxPmTtiPortIpv4OnlyTunneledEnableSet,
         3, 0},
        {"cpssExMxPmTtiPortIpv4OnlyTunneledEnableGet",
         &wrCpssExMxPmTtiPortIpv4OnlyTunneledEnableGet,
         2, 0},
        {"cpssExMxPmTtiRuleSetFirst",
         &wrCpssExMxPmTtiRuleSetFirst,
         2, 16},
        {"cpssExMxPmTtiRuleSetNext",
         &wrCpssExMxPmTtiRuleSetNext,
         2, 16},
        {"cpssExMxPmTtiRuleEndSet",
         &wrCpssExMxPmTtiRuleEndSet,
         0, 0},
        {"cpssExMxPmTtiRuleGetFirst",
         &wrCpssExMxPmTtiRuleGetFirst,
         1, 0},
        {"cpssExMxPmTtiRuleGetNext",
         &wrCpssExMxPmTtiRuleGet,
         1, 0},
        {"cpssExMxPmTtiRuleActionSet",
         &wrCpssExMxPmTtiRuleActionUpdate,
         0, 62},
        {"cpssExMxPmTtiRuleActionGetFirst",
         &wrCpssExMxPmTtiRuleActionGetFirst,
         0, 0},
        {"cpssExMxPmTtiRuleActionGetNext",
         &wrCpssExMxPmTtiRuleActionGet,
         0, 0},
        {"cpssExMxPmTtiRuleActionClear",
         &wrCpssExMxPmTtiActionClear,
         0, 0},
        {"cpssExMxPmTtiRuleActionDelete",
         &wrCpssExMxPmTtiRuleActionDelete,
         0, 2},
        {"cpssExMxPmTtiRuleDefaultExtendedActionSet",
         &wrCpssExMxPmTtiRuleDefaultExtendedActionSet,
         1, 16},
        {"cpssExMxPmTtiRuleDefaultExtendedActionGetFirst",
         &wrCpssExMxPmTtiRuleDefaultExtendedActionGetFirst,
         1, 0},
        {"cpssExMxPmTtiRuleDefaultExtendedActionGetNext",
         &wrCpssExMxPmTtiRuleDefaultExtendedActionGetNext,
         1, 0},
        {"cpssExMxPmTtiExceptionCmdSet",
         &wrCpssExMxPmTtiExceptionCmdSet,
         3, 0},
        {"cpssExMxPmTtiExceptionCmdGet",
         &wrCpssExMxPmTtiExceptionCmdGet,
         2, 0},
        {"cpssExMxPmTtiRuleMatchCounterSet",
         &wrCpssExMxPmTtiRuleMatchCounterSet,
         3, 0},
        {"cpssExMxPmTtiRuleMatchCounterGet",
         &wrCpssExMxPmTtiRuleMatchCounterGet,
         2, 0},
        {"cpssExMxPm2TtiRuleActionSet",
         &wrCpssExMxPm2TtiRuleActionSet,
         2, 69},
        {"cpssExMxPm2TtiRuleActionGetFirst",
         &wrCpssExMxPm2TtiRuleActionGetFirst,
         1, 0},
        {"cpssExMxPm2TtiRuleActionGetNext",
         &wrCpssExMxPm2TtiRuleActionGet,
         1, 0},
        {"cpssExMxPm2TtiRuleActionClear",
         &wrCpssExMxPm2TtiActionClear,
         1, 0},
        {"cpssExMxPm2TtiRuleActionDelete",
         &wrCpssExMxPm2TtiRuleActionDelete,
         0, 2},
        {"cpssExMxPm2TtiRuleSetFirst",
         &wrCpssExMxPm2TtiRuleSetFirst,
         3, 20},
        {"cpssExMxPm2TtiRuleSetNext",
         &wrCpssExMxPm2TtiRuleSetNext,
         3, 20},
        {"cpssExMxPm2TtiRuleEndSet",
         &wrCpssExMxPm2TtiRuleEndSet,
         3, 0},
        {"cpssExMxPm2TtiRuleGetFirst",
         &wrCpssExMxPm2TtiRuleGetFirst,
         2, 0},
        {"cpssExMxPm2TtiRuleGetNext",
         &wrCpssExMxPm2TtiRuleGet,
         2, 0},
        {"cpssExMxPm2TtiPortLookupModeSet",
         &wrCpssExMxPm2TtiPortLookupModeSet,
         4, 0},
        {"cpssExMxPm2TtiPortLookupModeGet",
         &wrCpssExMxPm2TtiPortLookupModeGet,
         3, 0},
        {"cpssExMxPm2TtiPortLookupConfigSet",
         &wrCpssExMxPm2TtiPortLookupConfigSet,
          5, 0},
        {"cpssExMxPm2TtiPortLookupConfigGet",
         &wrCpssExMxPm2TtiPortLookupConfigGet,
         3, 0},
        {"cpssExMxPm2TtiPortMacToMeEnableSet",
         &wrCpssExMxPm2TtiPortMacToMeEnableSet,
         4, 0},
        {"cpssExMxPm2TtiPortMacToMeEnableGet",
         &wrCpssExMxPm2TtiPortMacToMeEnableGet,
         3, 0},
        {"cpssExMxPm2TtiMimEtherTypeSet",
         &wrCpssExMxPm2TtiMimEtherTypeSet,
         2, 0},
        {"cpssExMxPm2TtiMimEthTypeGet",
         &wrCpssExMxPm2TtiMimEthTypeGet,
         1, 0},
        {"cpssExMxPm2TtiMacToMeSet",
         &wrCpssExMxPm2TtiMacToMeSet,
         7, 0},
        {"cpssExMxPm2TtiMacToMeGet",
         &wrCpssExMxPm2TtiMacToMeGet,
         2, 0},
        {"cpssExMxPmTtiExtTcamConfigSet",
         &wrCpssExMxPmTtiExtConfigSet,
         0,6},
        {"cpssExMxPmTtiExtTcamConfigGetFirst",
         &wrCpssExMxPmTtiExtConfigGetFirst,
         0,0},
        {"cpssExMxPmTtiExtTcamConfigGetNext",
         &wrCpssExMxPmTtiExtConfigGet,
         0,0},
        {"cpssExMxPmTtiRuleDefaultActionSet",
         &wrCpssExMxPmTtiRuleDefaultActionSet,
         1, 16},
        {"cpssExMxPmTtiRuleDefaultActionGetFirst",
         &wrCpssExMxPmTtiRuleDefaultActionGetFirst,
         1, 0},
        {"cpssExMxPmTtiRuleDefaultActionGetNext",
         &wrCpssExMxPmTtiRuleDefaultActionGetNext,
         1, 0},
        {"cpssExMxPmTtiRuleValidStatusSet",
         &wrCpssExMxPmTtiRuleValidStatusSet,
         4, 0},
        {"cpssExMxPmTtiRuleValidStatusGet",
         &wrCpssExMxPmTtiRuleValidStatusGet,
         3, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))


/*******************************************************************************
* cmdLibInitCpssExMxPmTti
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
GT_STATUS cmdLibInitCpssExMxPmTti
(
    GT_VOID
)
{
     return cmdInitLibrary(dbCommands, numCommands);
}

