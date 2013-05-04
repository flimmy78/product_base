/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChTti.c
*
* DESCRIPTION:
*       DxCh TTI wrappers
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>

#include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>


/* support for multi port groups */

/*******************************************************************************
* ttiMultiPortGroupsBmpGet
*
* DESCRIPTION:
*       Get the portGroupsBmp for multi port groups device.
*       when 'enabled' --> wrappers will use the APIs
*       with portGroupsBmp parameter
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - (pointer to)enable / disable the use of APIs with portGroupsBmp parameter.
*       portGroupsBmpPtr - (pointer to)port groups bmp , relevant only when enable = GT_TRUE
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*******************************************************************************/
static void ttiMultiPortGroupsBmpGet
(
    IN   GT_U8               devNum,
    OUT  GT_BOOL             *enablePtr,
    OUT  GT_PORT_GROUPS_BMP  *portGroupsBmpPtr
)
{
    /* default */
    *enablePtr  = GT_FALSE;
    *portGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if (0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return;
    }

    utilMultiPortGroupsBmpGet(devNum, enablePtr, portGroupsBmpPtr);
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiMacToMeSet
(
    IN  GT_U8                              devNum,
    IN  GT_U32                             entryIndex,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *valuePtr,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC         *maskPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiMacToMeSet(
            devNum, entryIndex, valuePtr, maskPtr);
    }
    else
    {
        return cpssDxChTtiPortGroupMacToMeSet(
            devNum, pgBmp, entryIndex, valuePtr, maskPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiMacToMeGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              entryIndex,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *valuePtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC          *maskPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiMacToMeGet(
            devNum, entryIndex, valuePtr, maskPtr);
    }
    else
    {
        return cpssDxChTtiPortGroupMacToMeGet(
            devNum, pgBmp, entryIndex, valuePtr, maskPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiMacModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_DXCH_TTI_MAC_MODE_ENT        macMode
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiMacModeSet(
            devNum, keyType, macMode);
    }
    else
    {
        return cpssDxChTtiPortGroupMacModeSet(
            devNum, pgBmp, keyType, macMode);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiMacModeGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *macModePtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiMacModeGet(
            devNum, keyType, macModePtr);
    }
    else
    {
        return cpssDxChTtiPortGroupMacModeGet(
            devNum, pgBmp, keyType, macModePtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiRuleSet(
            devNum, routerTtiTcamRow, keyType,
            patternPtr, maskPtr, actionType, actionPtr);
    }
    else
    {
        return cpssDxChTtiPortGroupRuleSet(
            devNum, pgBmp, routerTtiTcamRow, keyType,
            patternPtr, maskPtr, actionType, actionPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiRuleGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiRuleGet(
            devNum, routerTtiTcamRow, keyType,
            patternPtr, maskPtr, actionType, actionPtr);
    }
    else
    {
        return cpssDxChTtiPortGroupRuleGet(
            devNum, pgBmp, routerTtiTcamRow, keyType,
            patternPtr, maskPtr, actionType, actionPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiRuleActionUpdate(
            devNum, routerTtiTcamRow, actionType, actionPtr);
    }
    else
    {
        return cpssDxChTtiPortGroupRuleActionUpdate(
            devNum, pgBmp, routerTtiTcamRow, actionType, actionPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  GT_BOOL                             valid
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiRuleValidStatusSet(
            devNum, routerTtiTcamRow, valid);
    }
    else
    {
        return cpssDxChTtiPortGroupRuleValidStatusSet(
            devNum, pgBmp, routerTtiTcamRow, valid);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiRuleValidStatusGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    OUT GT_BOOL                             *validPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiRuleValidStatusGet(
            devNum, routerTtiTcamRow, validPtr);
    }
    else
    {
        return cpssDxChTtiPortGroupRuleValidStatusGet(
            devNum, pgBmp, routerTtiTcamRow, validPtr);
    }
}


/******************Table:cpssDxChTtiMacToMe*****************************************/

static GT_U8 macToMeTableIndex;

/*******************************************************************************
* cpssDxChTtiMacToMeSet
*
* DESCRIPTION:
*       This function sets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MAC2ME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MAC2ME
*       lookup is performed and the internal flag is set accordingly.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum            - device number
*       mac2MeIndex       - Index of mac and vlan in Mac2Me table (0..7)
*       macVlanPtr        - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* OUTPUTS:
*       None.
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
static CMD_STATUS wrCpssDxChTtiMacToMeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mac2MeIndex;
    CPSS_DXCH_TTI_MAC_VLAN_STC macVlan;
    CPSS_DXCH_TTI_MAC_VLAN_STC mask;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mac2MeIndex=(GT_U8)inFields[0];
    galtisMacAddr(&macVlan.mac,(GT_U8*)inFields[1]);
    macVlan.vlanId=(GT_U16)inFields[2];
    galtisMacAddr(&mask.mac,(GT_U8*)inFields[3]);
    mask.vlanId=(GT_U16)inFields[4];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiMacToMeSet(devNum, mac2MeIndex, &macVlan, &mask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMacToMeGet
*
* DESCRIPTION:
*       This function sets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MAC2ME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MAC2ME
*       lookup is performed and the internal flag is set accordingly.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum            - device number
*       mac2MeIndex       - Index of mac and vlan in Mac2Me table (0..7).
*
* OUTPUTS:
*       macVlanPtr        - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
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
static CMD_STATUS wrCpssDxChTtiMacToMeGetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;

    CPSS_DXCH_TTI_MAC_VLAN_STC macVlan;
    CPSS_DXCH_TTI_MAC_VLAN_STC mask;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (macToMeTableIndex>7)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* initialize macVlan and mask */
    cmdOsMemSet(&macVlan , 0, sizeof(macVlan));
    cmdOsMemSet(&mask , 0, sizeof(mask));

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiMacToMeGet(devNum, macToMeTableIndex, &macVlan, &mask);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0]= macToMeTableIndex;
    inFields[2]=macVlan.vlanId;
    inFields[4]=mask.vlanId;


    fieldOutput("%d%6b%d%6b%d", inFields[0],macVlan.mac.arEther,inFields[2],mask.mac.arEther,inFields[4]);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/**********************************************************************************/

static CMD_STATUS wrCpssDxChTtiMacToMeGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    macToMeTableIndex=0;

    return wrCpssDxChTtiMacToMeGetEntry(inArgs,inFields,numFields,outArgs);
}
/**********************************************************************************/

static CMD_STATUS wrCpssDxChTtiMacToMeGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    macToMeTableIndex++;

    return wrCpssDxChTtiMacToMeGetEntry(inArgs,inFields,numFields,outArgs);

}
/*******************************************************************************
* cpssDxChTtiPortLookupEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for the specified key
*       type at the port.
*
* APPLICABLE DEVICES:
*       All DXCH devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IP_V4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
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
static CMD_STATUS wrCpssDxChTtiPortLookupEnableSet
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
    CPSS_DXCH_TTI_KEY_TYPE_ENT keyType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTtiPortLookupEnableSet(devNum, port, keyType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortLookupEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of  the
*       TTI lookup for the specified key type.
*
* APPLICABLE DEVICES:
*       All DXCH devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IP_V4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
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
static CMD_STATUS wrCpssDxChTtiPortLookupEnableGet
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
    CPSS_DXCH_TTI_KEY_TYPE_ENT keyType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTtiPortLookupEnableGet(devNum, port, keyType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyTunneledEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only tunneled
*       packets received on port.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
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
static CMD_STATUS wrCpssDxChTtiPortIpv4OnlyTunneledEnableSet
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
    result = cpssDxChTtiPortIpv4OnlyTunneledEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyTunneledEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only tunneled packets received on port.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
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
static CMD_STATUS wrCpssDxChTtiPortIpv4OnlyTunneledEnableGet
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
    result = cpssDxChTtiPortIpv4OnlyTunneledEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyMac2MeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: enable IPv4 TTI lookup only for mac to me packets
*                       GT_FALSE: disable IPv4 TTI lookup only for mac to me packets
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
static CMD_STATUS wrCpssDxChTtiPortIpv4OnlyMac2MeEnableSet
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
    result = cpssDxChTtiPortIpv4OnlyMacToMeEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyMac2MeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       mac to me packets
*                       GT_TRUE: IPv4 TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: IPv4 TTI lookup only for mac to me packets is disabled
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
static CMD_STATUS wrCpssDxChTtiPortIpv4OnlyMac2MeEnableGet
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
    result = cpssDxChTtiPortIpv4OnlyMacToMeEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiIpv4McEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for IPv4 multicast
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
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
static CMD_STATUS wrCpssDxChTtiIpv4McEnableSet
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
    result = cpssDxChTtiIpv4McEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiIpv4McEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of TTI lookup for
*       IPv4 multicast (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
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
static CMD_STATUS wrCpssDxChTtiIpv4McEnableGet
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
    result = cpssDxChTtiIpv4McEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortMplsOnlyMac2MeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the MPLS TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: enable MPLS TTI lookup only for mac to me packets
*                       GT_FALSE: disable MPLS TTI lookup only for mac to me packets
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
static CMD_STATUS wrCpssDxChTtiPortMplsOnlyMac2MeEnableSet
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
    result = cpssDxChTtiPortMplsOnlyMacToMeEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortMplsOnlyMac2MeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       MPLS TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       mac to me packets
*                       GT_TRUE: MPLS TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: MPLS TTI lookup only for mac to me packets is disabled
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
static CMD_STATUS wrCpssDxChTtiPortMplsOnlyMac2MeEnableGet
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
    result = cpssDxChTtiPortMplsOnlyMacToMeEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortMimOnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the MIM TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE:  enable MIM TTI lookup only for mac to me packets
*                       GT_FALSE: disable MIM TTI lookup only for mac to me packets
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
static CMD_STATUS wrCpssDxChTtiPortMimOnlyMacToMeEnableSet
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
    result = cpssDxChTtiPortMimOnlyMacToMeEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortMimOnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       MIM TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable MIM TTI lookup only for
*                       mac to me packets
*                       GT_TRUE:  MIM TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: MIM TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPortMimOnlyMacToMeEnableGet
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
    result = cpssDxChTtiPortMimOnlyMacToMeEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*********************************/
/* cpssDxChTtiRuleAction Table */
/*********************************/

/* table cpssDxChTtiRuleAction global variables */
#define ACTION_TABLE_SIZE 1024

/* Database to hold TTI actions that has been set.
   Non valid entries maintain link list of free table entries */
typedef struct
{
    GT_BOOL                         valid;          /* weather this entry is valid           */
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType;     /* action type                           */
    CPSS_DXCH_TTI_ACTION_UNT        actionEntry;    /* action data                           */
    GT_U32                          ruleIndex;      /* rule index associated with the action */
    GT_U8                           devNum;         /* device associated with the action     */
    GT_U32                          nextFree;       /* in case not valid, next free entry    */
}ttiActionDB;

static ttiActionDB ttiActionDatabase[ACTION_TABLE_SIZE];

/* index of first free TTI action entry in the action table */
static GT_U32   firstFree = 0;
/* weather TTI action was used before or not (used to invalidate all entries
   on first time the table is used) */
static GT_BOOL  firstRun = GT_TRUE;
/* indicated invalid rule entry */
#define INVALID_RULE_INDEX 0x7FFFFFF0
/* index to go over TTI actions */
static  GT_U32   ttiActionGetIndex;

/*******************************************************************************
* ttiActionDatabaseGet
*
* DESCRIPTION:
*  The function searches the action entries database for action entry that match
*  the rule index and device number and returns the actionEntry.
*
*******************************************************************************/
static void ttiActionDatabaseGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  ruleIndex,
    OUT GT_U32                  *tableIndex
)
{
    GT_U32              index = 0;
    GT_BOOL             entryFound = GT_FALSE;

    while ((!entryFound) && (index < ACTION_TABLE_SIZE))
    {
        /* get next valid entry */
        while ((ttiActionDatabase[index].valid == GT_FALSE) && (index < ACTION_TABLE_SIZE))
            index++;
        /* check if the entry match the device number and rule index */
        if ((index < ACTION_TABLE_SIZE) &&
            (ttiActionDatabase[index].ruleIndex == ruleIndex) &&
            (ttiActionDatabase[index].devNum == devNum))
        {
            entryFound = GT_TRUE;
        }
        else
        {
            index++;
        }
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
* ttiActionDatabaseInit
*
* DESCRIPTION:
*  The function initializes the TTI action database (set all entries as not
*  valid and create free list of all non valid entries).
*
*******************************************************************************/
static CMD_STATUS ttiActionDatabaseInit(void)
{
    GT_U32      index = 0;

    firstFree = 0;

    while (index < ACTION_TABLE_SIZE-1)
    {
        ttiActionDatabase[index].valid = GT_FALSE;
        ttiActionDatabase[index].nextFree = index + 1;
        index++;
    }
    ttiActionDatabase[index].nextFree = INVALID_RULE_INDEX;
    ttiActionDatabase[index].valid = GT_FALSE;

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiRuleActionUpdate
*
* DESCRIPTION:
*       This function updates rule action.
*
* APPLICABLE DEVICES:
*       All DXCH devices.
*
* INPUTS:
*       devNum        - device number
*       ruleIndex     - Index of the rule in the TCAM (0..3k-1).
*       actionPtr     - points to the TTI rule action that applied on packet
*                       if packet's search key matched with masked pattern.
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
static CMD_STATUS wrCpssDxChTtiRuleActionUpdate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result = GT_OK;
    GT_U8                               devNum;
    GT_U32                              ruleIndex;
    GT_U32                              index;
    CPSS_DXCH_TTI_ACTION_UNT            *actionPtr;
    GT_BOOL                             isNewIndex = GT_FALSE;
    ttiActionDB                         ttiTempAction = {0};

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)inFields[0];
    ruleIndex = (GT_U32)inFields[1];
    if (ruleIndex > 3071)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Rule index has to be in range [0..3071].\n");

        return CMD_AGENT_ERROR;
    }

    /* in case it is first usage of TTI action database, initialize it */
    if (firstRun)
    {
        ttiActionDatabaseInit();
        firstRun = GT_FALSE;
    }

    /* look for the action in the TTI action database */
    ttiActionDatabaseGet((GT_U8)inFields[0], inFields[1], &index);

    /* the action wasn't in the TTI action database */
    if (index == INVALID_RULE_INDEX)
    {
        if (firstFree == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
            return CMD_AGENT_ERROR;
        }

        index = firstFree;
        firstFree = ttiActionDatabase[firstFree].nextFree;
        isNewIndex = GT_TRUE;
    }

    /* this is update of an existing action:
       save the old action parameters */
    if (isNewIndex == GT_FALSE)
    {
        cpssOsMemCpy(&ttiTempAction,&ttiActionDatabase[index],sizeof(ttiTempAction));
    }

    /* set action entry in TTI action database as valid */
    ttiActionDatabase[index].valid      = GT_TRUE;
    ttiActionDatabase[index].ruleIndex  = ruleIndex;
    ttiActionDatabase[index].devNum     = devNum;
    ttiActionDatabase[index].actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

    actionPtr = &(ttiActionDatabase[index].actionEntry);

    /* fill action fields */
    actionPtr->type1.tunnelTerminate                  = (GT_BOOL)inFields[2];
    actionPtr->type1.passengerPacketType              = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inFields[3];
    actionPtr->type1.copyTtlFromTunnelHeader          = (GT_BOOL)inFields[4];
    actionPtr->type1.command                          = (CPSS_PACKET_CMD_ENT)inFields[5];
    actionPtr->type1.redirectCommand                  = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inFields[6];
    actionPtr->type1.egressInterface.type             = (CPSS_INTERFACE_TYPE_ENT)inFields[7];
    actionPtr->type1.egressInterface.devPort.devNum   = (GT_U8)inFields[8];
    actionPtr->type1.egressInterface.devPort.portNum  = (GT_U8)inFields[9];
    /* Override Device and Port */
    CONVERT_DEV_PORT_DATA_MAC(actionPtr->type1.egressInterface.devPort.devNum, actionPtr->type1.egressInterface.devPort.portNum);
    actionPtr->type1.egressInterface.trunkId          = (GT_TRUNK_ID)inFields[10];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type1.egressInterface.trunkId);
    actionPtr->type1.egressInterface.vidx             = (GT_U16)inFields[11];
    actionPtr->type1.egressInterface.vlanId           = (GT_U16)inFields[12];

    actionPtr->type1.tunnelStart                      = (GT_BOOL)inFields[13];
    actionPtr->type1.tunnelStartPtr                   = (GT_U32)inFields[14];
    actionPtr->type1.routerLookupPtr                  = (GT_U32)inFields[15];
    actionPtr->type1.vrfId                            = (GT_U32)inFields[16];
    /* the fields targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
       from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be ignored */
    actionPtr->type1.useVidx                          = (GT_BOOL)inFields[20];


    actionPtr->type1.sourceIdSetEnable                = (GT_BOOL)inFields[21];
    actionPtr->type1.sourceId                         = (GT_U32)inFields[22];
    actionPtr->type1.vlanCmd                          = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[23];
    actionPtr->type1.vlanId                           = (GT_U16)inFields[24];
    actionPtr->type1.vlanPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[25];
    actionPtr->type1.nestedVlanEnable                 = (GT_BOOL)inFields[26];
    actionPtr->type1.bindToPolicer                    = (GT_BOOL)inFields[27];
    actionPtr->type1.policerIndex                     = (GT_U32)inFields[28];
    actionPtr->type1.qosPrecedence                    = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[29];
    actionPtr->type1.qosTrustMode                     = (CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT)inFields[30];
    actionPtr->type1.qosProfile                       = (GT_U32)inFields[31];
    actionPtr->type1.modifyUpEnable                   = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inFields[32];
    actionPtr->type1.modifyDscpEnable                 = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inFields[33];
    actionPtr->type1.up                               = (GT_U32)inFields[34];
    actionPtr->type1.remapDSCP                        = (GT_BOOL)inFields[35];
    actionPtr->type1.mirrorToIngressAnalyzerEnable    = (GT_BOOL)inFields[36];
    actionPtr->type1.userDefinedCpuCode               = (CPSS_NET_RX_CPU_CODE_ENT)inFields[37];
    actionPtr->type1.activateCounter                  = (GT_BOOL)inFields[38];
    actionPtr->type1.counterIndex                     = (GT_U32)inFields[39];
    actionPtr->type1.vntl2Echo                        = (GT_BOOL)inFields[40];
    actionPtr->type1.bridgeBypass                     = (GT_BOOL)inFields[41];
    actionPtr->type1.actionStop                       = (GT_BOOL)inFields[42];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiRuleActionUpdate(devNum,ruleIndex,ttiActionDatabase[index].actionType,actionPtr);

    /* if the rule action didn't succeed:  */
    if (result != GT_OK)
    {
        /* if this is a new action remove it from the database */
        if (isNewIndex)
        {
            ttiActionDatabase[index].valid = GT_FALSE;
            ttiActionDatabase[index].nextFree = firstFree;
            firstFree = index;
        }
        /* if this is an update for existing action restore previous data */
        else
        {
            cpssOsMemCpy(&ttiActionDatabase[index],&ttiTempAction,sizeof(ttiTempAction));
        }
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleActionGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_ACTION_UNT    *actionPtr;
    GT_U8                       tempDev,tempPort; /* used for port,dev converting */

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* get next valid TTI action entry that is from action type 1 */
    while (ttiActionGetIndex < ACTION_TABLE_SIZE)
    {
        if ((ttiActionDatabase[ttiActionGetIndex].valid == GT_TRUE) &&
        (ttiActionDatabase[ttiActionGetIndex].actionType == CPSS_DXCH_TTI_ACTION_TYPE1_ENT))
            break;
        else
            ttiActionGetIndex++;
    }

    if (ttiActionGetIndex == ACTION_TABLE_SIZE)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    actionPtr = &(ttiActionDatabase[ttiActionGetIndex].actionEntry);

    inFields[0]  = ttiActionDatabase[ttiActionGetIndex].devNum;
    inFields[1]  = ttiActionDatabase[ttiActionGetIndex].ruleIndex;
    inFields[2]  = actionPtr->type1.tunnelTerminate;
    inFields[3]  = actionPtr->type1.passengerPacketType;
    inFields[4]  = actionPtr->type1.copyTtlFromTunnelHeader;
    inFields[5]  = actionPtr->type1.command;
    inFields[6]  = actionPtr->type1.redirectCommand;
    inFields[7]  = actionPtr->type1.egressInterface.type;
    tempDev      = actionPtr->type1.egressInterface.devPort.devNum;
    tempPort     = actionPtr->type1.egressInterface.devPort.portNum;
    CONVERT_BACK_DEV_PORT_DATA_MAC(tempDev,tempPort);
    inFields[8]  = tempDev;
    inFields[9]  = tempPort;
    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr->type1.egressInterface.trunkId);
    inFields[10] = actionPtr->type1.egressInterface.trunkId;
    inFields[11] = actionPtr->type1.egressInterface.vidx;
    inFields[12] = actionPtr->type1.egressInterface.vlanId;
    inFields[13] = actionPtr->type1.tunnelStart;
    inFields[14] = actionPtr->type1.tunnelStartPtr;
    inFields[15] = actionPtr->type1.routerLookupPtr;
    inFields[16] = actionPtr->type1.vrfId;
    /* the fields targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
       from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be hardcoded  */
    inFields[17] = GT_FALSE;
    inFields[18] = 0;
    inFields[19] = 0;
    inFields[20] = actionPtr->type1.useVidx;
    inFields[21] = actionPtr->type1.sourceIdSetEnable;
    inFields[22] = actionPtr->type1.sourceId;
    inFields[23] = actionPtr->type1.vlanCmd;
    inFields[24] = actionPtr->type1.vlanId;
    inFields[25] = actionPtr->type1.vlanPrecedence;
    inFields[26] = actionPtr->type1.nestedVlanEnable;
    inFields[27] = actionPtr->type1.bindToPolicer;
    inFields[28] = actionPtr->type1.policerIndex;
    inFields[29] = actionPtr->type1.qosPrecedence;
    inFields[30] = actionPtr->type1.qosTrustMode;
    inFields[31] = actionPtr->type1.qosProfile;
    inFields[32] = actionPtr->type1.modifyUpEnable;
    inFields[33] = actionPtr->type1.modifyDscpEnable;
    inFields[34] = actionPtr->type1.up;
    inFields[35] = actionPtr->type1.remapDSCP;
    inFields[36] = actionPtr->type1.mirrorToIngressAnalyzerEnable;
    inFields[37] = actionPtr->type1.userDefinedCpuCode;
    inFields[38] = actionPtr->type1.activateCounter;
    inFields[39] = actionPtr->type1.counterIndex;
    inFields[40] = actionPtr->type1.vntl2Echo;
    inFields[41] = actionPtr->type1.bridgeBypass;
    inFields[42] = actionPtr->type1.actionStop;

    ttiActionGetIndex++;

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
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleActionGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ttiActionGetIndex = 0;

    return wrCpssDxChTtiRuleActionGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleActionDelete
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

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum =  (GT_U8)inFields[0];
    ruleIndex = (GT_U32)inFields[1];

    ttiActionDatabaseGet(devNum, ruleIndex, &index);

    /* the rule is not found */
    if (index == INVALID_RULE_INDEX)
    {
        galtisOutput(outArgs, GT_NO_SUCH, "%d", -1);
        return CMD_AGENT_ERROR;
    }

    ttiActionDatabase[index].valid = GT_FALSE;
    ttiActionDatabase[index].nextFree = firstFree;
    firstFree = index;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleActionClear
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ttiActionDatabaseInit();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/***********************************/
/* cpssDxChTtiRuleAction_1 Table */
/***********************************/

/*******************************************************************************
* wrCpssDxChTtiRuleAction_1Update
*
* DESCRIPTION:
*       This function updates rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_1Update
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result = GT_OK;
    GT_U8                               devNum;
    GT_U32                              ruleIndex;
    GT_U32                              index;
    CPSS_DXCH_TTI_ACTION_UNT            *actionPtr;
    GT_BOOL                             isNewIndex = GT_FALSE;
    ttiActionDB                         ttiTempAction = {0};

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)inFields[0];
    ruleIndex = (GT_U32)inFields[1];
    if (ruleIndex > 3071)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Rule index has to be in range [0..3071].\n");

        return CMD_AGENT_ERROR;
    }

    /* in case it is first usage of TTI action database, initialize it */
    if (firstRun)
    {
        ttiActionDatabaseInit();
        firstRun = GT_FALSE;
    }

    /* look for the action in the TTI action database */
    ttiActionDatabaseGet(devNum, ruleIndex, &index);

    /* the action wasn't in the TTI action database */
    if (index == INVALID_RULE_INDEX)
    {
        if (firstFree == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
            return CMD_AGENT_ERROR;
        }

        index = firstFree;
        firstFree = ttiActionDatabase[firstFree].nextFree;
        isNewIndex = GT_TRUE;
    }

    /* this is update of an existing action:
       save the old action parameters */
    if (isNewIndex == GT_FALSE)
    {
        cpssOsMemCpy(&ttiTempAction,&ttiActionDatabase[index],sizeof(ttiTempAction));
    }

    /* set action entry in TTI action database as valid */
    ttiActionDatabase[index].valid      = GT_TRUE;
    ttiActionDatabase[index].ruleIndex  = ruleIndex;
    ttiActionDatabase[index].devNum     = devNum;
    ttiActionDatabase[index].actionType = (CPSS_DXCH_TTI_ACTION_TYPE_ENT)inArgs[0];

    actionPtr = &(ttiActionDatabase[index].actionEntry);

    /* fill action fields */
    switch (ttiActionDatabase[index].actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:

        actionPtr->type1.tunnelTerminate                  = (GT_BOOL)inFields[2];
        actionPtr->type1.passengerPacketType              = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inFields[3];
        actionPtr->type1.copyTtlFromTunnelHeader          = (GT_BOOL)inFields[4];
        actionPtr->type1.command                          = (CPSS_PACKET_CMD_ENT)inFields[5];
        actionPtr->type1.redirectCommand                  = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inFields[6];
        actionPtr->type1.egressInterface.type             = (CPSS_INTERFACE_TYPE_ENT)inFields[7];
        actionPtr->type1.egressInterface.devPort.devNum   = (GT_U8)inFields[8];
        actionPtr->type1.egressInterface.devPort.portNum  = (GT_U8)inFields[9];
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(actionPtr->type1.egressInterface.devPort.devNum, actionPtr->type1.egressInterface.devPort.portNum);
        actionPtr->type1.egressInterface.trunkId          = (GT_TRUNK_ID)inFields[10];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type1.egressInterface.trunkId);
        actionPtr->type1.egressInterface.vidx             = (GT_U16)inFields[11];
        actionPtr->type1.egressInterface.vlanId           = (GT_U16)inFields[12];
        actionPtr->type1.tunnelStart                      = (GT_BOOL)inFields[13];
        actionPtr->type1.tunnelStartPtr                   = (GT_U32)inFields[14];
        actionPtr->type1.routerLookupPtr                  = (GT_U32)inFields[15];
        actionPtr->type1.vrfId                            = (GT_U32)inFields[16];
        /* the fields targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
           from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be ignored */
        actionPtr->type1.useVidx                          = (GT_BOOL)inFields[20];
        actionPtr->type1.sourceIdSetEnable                = (GT_BOOL)inFields[21];
        actionPtr->type1.sourceId                         = (GT_U32)inFields[22];
        actionPtr->type1.vlanCmd                          = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[23];
        actionPtr->type1.vlanId                           = (GT_U16)inFields[24];
        actionPtr->type1.vlanPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[25];
        actionPtr->type1.nestedVlanEnable                 = (GT_BOOL)inFields[26];
        actionPtr->type1.bindToPolicer                    = (GT_BOOL)inFields[27];
        actionPtr->type1.policerIndex                     = (GT_U32)inFields[28];
        actionPtr->type1.qosPrecedence                    = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[29];
        actionPtr->type1.qosTrustMode                     = (CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT)inFields[30];
        actionPtr->type1.qosProfile                       = (GT_U32)inFields[31];
        actionPtr->type1.modifyUpEnable                   = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inFields[32];
        actionPtr->type1.modifyDscpEnable                 = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inFields[33];
        actionPtr->type1.up                               = (GT_U32)inFields[34];
        actionPtr->type1.remapDSCP                        = (GT_BOOL)inFields[35];
        actionPtr->type1.mirrorToIngressAnalyzerEnable    = (GT_BOOL)inFields[36];
        actionPtr->type1.userDefinedCpuCode               = (CPSS_NET_RX_CPU_CODE_ENT)inFields[37];
        actionPtr->type1.vntl2Echo                        = (GT_BOOL)inFields[38];
        actionPtr->type1.bridgeBypass                     = (GT_BOOL)inFields[39];
        actionPtr->type1.actionStop                       = (GT_BOOL)inFields[40];
        actionPtr->type1.activateCounter                  = (GT_BOOL)inFields[41];
        actionPtr->type1.counterIndex                     = (GT_U32)inFields[42];
        break;

    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:

        actionPtr->type2.tunnelTerminate                 = (GT_BOOL)inFields[2];
        actionPtr->type2.ttPassengerPacketType           = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inFields[3];
        actionPtr->type2.tsPassengerPacketType           = (CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT)inFields[4];
        actionPtr->type2.copyTtlFromTunnelHeader         = (GT_BOOL)inFields[5];
        actionPtr->type2.mplsCommand                     = (CPSS_DXCH_TTI_MPLS_CMD_ENT)inFields[6];
        actionPtr->type2.mplsTtl                         = (GT_U32)inFields[7];
        actionPtr->type2.enableDecrementTtl              = (GT_BOOL)inFields[8];
        actionPtr->type2.command                         = (CPSS_PACKET_CMD_ENT)inFields[9];
        actionPtr->type2.redirectCommand                 = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inFields[10];
        actionPtr->type2.egressInterface.type            = (CPSS_INTERFACE_TYPE_ENT)inFields[11];
        actionPtr->type2.egressInterface.devPort.devNum  = (GT_U8)inFields[12];
        actionPtr->type2.egressInterface.devPort.portNum = (GT_U8)inFields[13];
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(actionPtr->type2.egressInterface.devPort.devNum, actionPtr->type2.egressInterface.devPort.portNum);
        actionPtr->type2.egressInterface.trunkId         = (GT_TRUNK_ID)inFields[14];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type2.egressInterface.trunkId);
        actionPtr->type2.egressInterface.vidx            = (GT_U16)inFields[15];
        actionPtr->type2.egressInterface.vlanId          = (GT_U16)inFields[16];
        actionPtr->type2.arpPtr                          = (GT_U32)inFields[17];
        actionPtr->type2.tunnelStart                     = (GT_BOOL)inFields[18];
        actionPtr->type2.tunnelStartPtr                  = (GT_U32)inFields[19];
        actionPtr->type2.routerLttPtr                    = (GT_U32)inFields[20];
        actionPtr->type2.vrfId                           = (GT_U32)inFields[21];
        actionPtr->type2.sourceIdSetEnable               = (GT_BOOL)inFields[22];
        actionPtr->type2.sourceId                        = (GT_U32)inFields[23];
        actionPtr->type2.tag0VlanCmd                     = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[24];
        actionPtr->type2.tag0VlanId                      = (GT_U16)inFields[25];
        actionPtr->type2.tag1VlanCmd                     = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[26];
        actionPtr->type2.tag1VlanId                      = (GT_U16)inFields[27];
        actionPtr->type2.tag0VlanPrecedence              = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[28];
        actionPtr->type2.nestedVlanEnable                = (GT_BOOL)inFields[29];
        actionPtr->type2.bindToPolicerMeter              = (GT_BOOL)inFields[30];
        actionPtr->type2.bindToPolicer                   = (GT_BOOL)inFields[31];
        actionPtr->type2.policerIndex                    = (GT_U32)inFields[32];
        actionPtr->type2.qosPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[33];
        actionPtr->type2.keepPreviousQoS                 = (GT_BOOL)inFields[34];
        actionPtr->type2.trustUp                         = (GT_BOOL)inFields[35];
        actionPtr->type2.trustDscp                       = (GT_BOOL)inFields[36];
        actionPtr->type2.trustExp                        = (GT_BOOL)inFields[37];
        actionPtr->type2.qosProfile                      = (GT_U32)inFields[38];
        actionPtr->type2.modifyTag0Up                    = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inFields[39];
        actionPtr->type2.tag1UpCommand                   = (CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT)inFields[40];
        actionPtr->type2.modifyDscp                      = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inFields[41];
        actionPtr->type2.tag0Up                          = (GT_U32)inFields[42];
        actionPtr->type2.tag1Up                          = (GT_U32)inFields[43];
        actionPtr->type2.remapDSCP                       = (GT_BOOL)inFields[44];
        actionPtr->type2.pcl0OverrideConfigIndex         = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[45];
        actionPtr->type2.pcl0_1OverrideConfigIndex       = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[46];
        actionPtr->type2.pcl1OverrideConfigIndex         = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[47];
        actionPtr->type2.iPclConfigIndex                 = (GT_U32)inFields[48];
        actionPtr->type2.mirrorToIngressAnalyzerEnable   = (GT_BOOL)inFields[49];
        actionPtr->type2.userDefinedCpuCode              = (CPSS_NET_RX_CPU_CODE_ENT)inFields[50];
        actionPtr->type2.bindToCentralCounter            = (GT_BOOL)inFields[51];
        actionPtr->type2.centralCounterIndex             = (GT_U32)inFields[52];
        actionPtr->type2.vntl2Echo                       = (GT_BOOL)inFields[53];
        actionPtr->type2.bridgeBypass                    = (GT_BOOL)inFields[54];
        actionPtr->type2.ingressPipeBypass               = (GT_BOOL)inFields[55];
        actionPtr->type2.actionStop                      = (GT_BOOL)inFields[56];
        break;

    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_AGENT_ERROR;
    }

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiRuleActionUpdate(devNum,ruleIndex,ttiActionDatabase[index].actionType,actionPtr);

    /* if the rule action didn't succeed:  */
    if (result != GT_OK)
    {
        /* if this is a new action remove it from the database */
        if (isNewIndex)
        {
            ttiActionDatabase[index].valid = GT_FALSE;
            ttiActionDatabase[index].nextFree = firstFree;
            firstFree = index;
        }
        /* if this is an update for existing action restore previous data */
        else
        {
            cpssOsMemCpy(&ttiActionDatabase[index],&ttiTempAction,sizeof(ttiTempAction));
        }
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleAction_1Get
*
* DESCRIPTION:
*       This function gets single rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_1Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_ACTION_UNT    *actionPtr;
    GT_U8                       tempDev,tempPort; /* used for port,dev converting */

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* get next valid TTI action entry */
    while ((ttiActionGetIndex < ACTION_TABLE_SIZE) &&
           (ttiActionDatabase[ttiActionGetIndex].valid == GT_FALSE))
        ttiActionGetIndex++;

    if (ttiActionGetIndex == ACTION_TABLE_SIZE)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    actionPtr = &(ttiActionDatabase[ttiActionGetIndex].actionEntry);

    switch (ttiActionDatabase[ttiActionGetIndex].actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:

        inArgs[0] = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

        inFields[0]  = ttiActionDatabase[ttiActionGetIndex].devNum;
        inFields[1]  = ttiActionDatabase[ttiActionGetIndex].ruleIndex;
        inFields[2]  = actionPtr->type1.tunnelTerminate;
        inFields[3]  = actionPtr->type1.passengerPacketType;
        inFields[4]  = actionPtr->type1.copyTtlFromTunnelHeader;
        inFields[5]  = actionPtr->type1.command;
        inFields[6]  = actionPtr->type1.redirectCommand;
        inFields[7]  = actionPtr->type1.egressInterface.type;
        tempDev      = actionPtr->type1.egressInterface.devPort.devNum;
        tempPort     = actionPtr->type1.egressInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempDev,tempPort);
        inFields[8]  = tempDev;
        inFields[9]  = tempPort;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr->type1.egressInterface.trunkId);
        inFields[10] = actionPtr->type1.egressInterface.trunkId;
        inFields[11] = actionPtr->type1.egressInterface.vidx;
        inFields[12] = actionPtr->type1.egressInterface.vlanId;
        inFields[13] = actionPtr->type1.tunnelStart;
        inFields[14] = actionPtr->type1.tunnelStartPtr;
        inFields[15] = actionPtr->type1.routerLookupPtr;
        inFields[16] = actionPtr->type1.vrfId;
        /* the fields targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
           from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be hardcoded  */
        inFields[17] = GT_FALSE;
        inFields[18] = 0;
        inFields[19] = 0;
        inFields[20] = actionPtr->type1.useVidx;
        inFields[21] = actionPtr->type1.sourceIdSetEnable;
        inFields[22] = actionPtr->type1.sourceId;
        inFields[23] = actionPtr->type1.vlanCmd;
        inFields[24] = actionPtr->type1.vlanId;
        inFields[25] = actionPtr->type1.vlanPrecedence;
        inFields[26] = actionPtr->type1.nestedVlanEnable;
        inFields[27] = actionPtr->type1.bindToPolicer;
        inFields[28] = actionPtr->type1.policerIndex;
        inFields[29] = actionPtr->type1.qosPrecedence;
        inFields[30] = actionPtr->type1.qosTrustMode;
        inFields[31] = actionPtr->type1.qosProfile;
        inFields[32] = actionPtr->type1.modifyUpEnable;
        inFields[33] = actionPtr->type1.modifyDscpEnable;
        inFields[34] = actionPtr->type1.up;
        inFields[35] = actionPtr->type1.remapDSCP;
        inFields[36] = actionPtr->type1.mirrorToIngressAnalyzerEnable;
        inFields[37] = actionPtr->type1.userDefinedCpuCode;
        inFields[38] = actionPtr->type1.vntl2Echo;
        inFields[39] = actionPtr->type1.bridgeBypass;
        inFields[40] = actionPtr->type1.actionStop;
        inFields[41] = actionPtr->type1.activateCounter;
        inFields[42] = actionPtr->type1.counterIndex;

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
        break;

    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:

        inArgs[0] = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        inFields[0]  = ttiActionDatabase[ttiActionGetIndex].devNum;
        inFields[1]  = ttiActionDatabase[ttiActionGetIndex].ruleIndex;
        inFields[2]  = actionPtr->type2.tunnelTerminate;
        inFields[3]  = actionPtr->type2.ttPassengerPacketType;
        inFields[4]  = actionPtr->type2.tsPassengerPacketType;
        inFields[5]  = actionPtr->type2.copyTtlFromTunnelHeader;
        inFields[6]  = actionPtr->type2.mplsCommand;
        inFields[7]  = actionPtr->type2.mplsTtl;
        inFields[8]  = actionPtr->type2.enableDecrementTtl;
        inFields[9]  = actionPtr->type2.command;
        inFields[10] = actionPtr->type2.redirectCommand;
        inFields[11] = actionPtr->type2.egressInterface.type;
        tempDev      = actionPtr->type2.egressInterface.devPort.devNum;
        tempPort     = actionPtr->type2.egressInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempDev,tempPort);
        inFields[12] = tempDev;
        inFields[13] = tempPort;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr->type2.egressInterface.trunkId);
        inFields[14] = actionPtr->type2.egressInterface.trunkId;
        inFields[15] = actionPtr->type2.egressInterface.vidx;
        inFields[16] = actionPtr->type2.egressInterface.vlanId;
        inFields[17] = actionPtr->type2.arpPtr;
        inFields[18] = actionPtr->type2.tunnelStart;
        inFields[19] = actionPtr->type2.tunnelStartPtr;
        inFields[20] = actionPtr->type2.routerLttPtr;
        inFields[21] = actionPtr->type2.vrfId;
        inFields[22] = actionPtr->type2.sourceIdSetEnable;
        inFields[23] = actionPtr->type2.sourceId;
        inFields[24] = actionPtr->type2.tag0VlanCmd;
        inFields[25] = actionPtr->type2.tag0VlanId;
        inFields[26] = actionPtr->type2.tag1VlanCmd;
        inFields[27] = actionPtr->type2.tag1VlanId;
        inFields[28] = actionPtr->type2.tag0VlanPrecedence;
        inFields[29] = actionPtr->type2.nestedVlanEnable;
        inFields[30] = actionPtr->type2.bindToPolicerMeter;
        inFields[31] = actionPtr->type2.bindToPolicer;
        inFields[32] = actionPtr->type2.policerIndex;
        inFields[33] = actionPtr->type2.qosPrecedence;
        inFields[34] = actionPtr->type2.keepPreviousQoS;
        inFields[35] = actionPtr->type2.trustUp;
        inFields[36] = actionPtr->type2.trustDscp;
        inFields[37] = actionPtr->type2.trustExp;
        inFields[38] = actionPtr->type2.qosProfile;
        inFields[39] = actionPtr->type2.modifyTag0Up;
        inFields[40] = actionPtr->type2.tag1UpCommand;
        inFields[41] = actionPtr->type2.modifyDscp;
        inFields[42] = actionPtr->type2.tag0Up;
        inFields[43] = actionPtr->type2.tag1Up;
        inFields[44] = actionPtr->type2.remapDSCP;
        inFields[45] = actionPtr->type2.pcl0OverrideConfigIndex;
        inFields[46] = actionPtr->type2.pcl0_1OverrideConfigIndex;
        inFields[47] = actionPtr->type2.pcl1OverrideConfigIndex;
        inFields[48] = actionPtr->type2.iPclConfigIndex;
        inFields[49] = actionPtr->type2.mirrorToIngressAnalyzerEnable;
        inFields[50] = actionPtr->type2.userDefinedCpuCode;
        inFields[51] = actionPtr->type2.bindToCentralCounter;
        inFields[52] = actionPtr->type2.centralCounterIndex;
        inFields[53] = actionPtr->type2.vntl2Echo;
        inFields[54] = actionPtr->type2.bridgeBypass;
        inFields[55] = actionPtr->type2.ingressPipeBypass;
        inFields[56] = actionPtr->type2.actionStop;

        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
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
                    inFields[56]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d%f", 1);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI ACTION FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    ttiActionGetIndex++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleAction_1GetFirst
*
* DESCRIPTION:
*       This function gets first rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_1GetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ttiActionGetIndex = 0;

    return wrCpssDxChTtiRuleAction_1Get(inArgs,inFields,numFields,outArgs);
}

/*************************/
/* cpssDxChTtiRule Table */
/*************************/

/* tables cpssExMxPmTtiRule global variables */

static  CPSS_DXCH_TTI_RULE_UNT          maskData;
static  CPSS_DXCH_TTI_RULE_UNT          patternData;
static  CPSS_DXCH_TTI_KEY_TYPE_ENT      ruleFormat;
static  GT_BOOL                         mask_set = GT_FALSE;    /* is mask set   */
static  GT_BOOL                         pattern_set = GT_FALSE; /* is pattern set*/
static  GT_U32                          mask_ruleIndex = 0;
static  GT_U32                          pattern_ruleIndex = 0;

/* variables for TTI rule table entries get */

/* table to hold the rule type of each added rule */
static  CPSS_DXCH_TTI_KEY_TYPE_ENT      ttiRuleFormat[3072];
/* table to hold the action type for each rule entry set (used in rule entry get) */
static  CPSS_DXCH_TTI_ACTION_TYPE_ENT   ttiRuleActionType[3072];

static  GT_U32                          ruleIndex;
static  GT_U32                          ruleIndexMaxGet;
static  GT_BOOL                         isMask;

/* convert trunk fields of test for mask ,pattern of cpss */
static void ttiRuleTrunkInfoFromTestToCpss
(
    INOUT CPSS_DXCH_TTI_RULE_COMMON_STC *commonPtr,
    IN GT_U32                  srcPortTrunk,
    IN GT_U8                   dsaSrcPortTrunk
)
{
    CPSS_DXCH_TTI_RULE_COMMON_STC *patternCommonPtr =
        &patternData.ipv4.common;/* all tti rules starts with the 'common'*/
    CPSS_DXCH_TTI_RULE_COMMON_STC *maskCommonPtr =
        &maskData.ipv4.common;/* all tti rules starts with the 'common'*/
    GT_TRUNK_ID trunkId;

    /*srcPortTrunk*/
    {
        commonPtr->srcPortTrunk = srcPortTrunk;

        trunkId = (GT_TRUNK_ID)commonPtr->srcPortTrunk;
        if(commonPtr == maskCommonPtr)
        {
            /* we do the mask now */
            gtPclMaskTrunkConvertFromTestToCpss(
                maskCommonPtr->srcIsTrunk,
                patternCommonPtr->srcIsTrunk,
                &trunkId);
        }
        else
        {
            /* we do the pattern now */
            gtPclPatternTrunkConvertFromTestToCpss(
                maskCommonPtr->srcIsTrunk,
                patternCommonPtr->srcIsTrunk,
                &trunkId);
        }

        commonPtr->srcPortTrunk = trunkId;
    }

    /*dsaSrcPortTrunk*/
    {
        commonPtr->dsaSrcPortTrunk = dsaSrcPortTrunk;

        trunkId = commonPtr->dsaSrcPortTrunk;
        if(commonPtr == maskCommonPtr)
        {
            /* we do the mask now */
            gtPclMaskTrunkConvertFromTestToCpss(
                maskCommonPtr->dsaSrcIsTrunk,
                patternCommonPtr->dsaSrcIsTrunk,
                &trunkId);
        }
        else
        {
            /* we do the pattern now */
            gtPclPatternTrunkConvertFromTestToCpss(
                maskCommonPtr->dsaSrcIsTrunk,
                patternCommonPtr->dsaSrcIsTrunk,
                &trunkId);
        }

        commonPtr->dsaSrcPortTrunk = (GT_U8)trunkId;
    }


    return;
}

/* convert trunk fields of test for mask ,pattern of cpss */
static void ttiRuleTrunkInfoFromTestToCpss_1
(
    IN GT_U32       pattern
)
{
    if(pattern == 0)/*mask*/
    {
        if(pattern_set == GT_FALSE)
        {
            return;
        }
        /* we do mask now and pattern already done*/
        /* we can get full info */
    }
    else
    {
        if(pattern_set == GT_FALSE)
        {
            return;
        }

        /* we do pattern now and mask already done*/
        /* we can get full info */
    }

    ttiRuleTrunkInfoFromTestToCpss(
    &maskData.ipv4.common, /* all keys start with common */
    maskData.ipv4.common.srcPortTrunk,
    maskData.ipv4.common.dsaSrcPortTrunk);

    ttiRuleTrunkInfoFromTestToCpss(
    &patternData.ipv4.common, /* all keys start with common */
    patternData.ipv4.common.srcPortTrunk,
    patternData.ipv4.common.dsaSrcPortTrunk);
}


/* convert trunk fields of mask ,pattern back to test value */
static void ttiRuleTrunkInfoFromCpssToTest
(
    INOUT CPSS_DXCH_TTI_RULE_COMMON_STC *commonPtr
)
{
    CPSS_DXCH_TTI_RULE_COMMON_STC *patternCommonPtr =
        &patternData.ipv4.common;/* all tti rules starts with the 'common'*/
    CPSS_DXCH_TTI_RULE_COMMON_STC *maskCommonPtr =
        &maskData.ipv4.common;/* all tti rules starts with the 'common'*/
    GT_TRUNK_ID trunkId;

    {
        trunkId = (GT_TRUNK_ID)commonPtr->srcPortTrunk;
        if(commonPtr == maskCommonPtr)
        {
            /* we do the mask now */
            gtPclMaskTrunkConvertFromCpssToTest(
                maskCommonPtr->srcIsTrunk,
                patternCommonPtr->srcIsTrunk,
                &trunkId);
        }
        else
        {
            /* we do the pattern now */
            gtPclPatternTrunkConvertFromCpssToTest(
                maskCommonPtr->srcIsTrunk,
                patternCommonPtr->srcIsTrunk,
                &trunkId);
        }

        commonPtr->srcPortTrunk = trunkId;
    }

    {
        trunkId = commonPtr->dsaSrcPortTrunk;
        if(commonPtr == maskCommonPtr)
        {
            /* we do the mask now */
            gtPclMaskTrunkConvertFromCpssToTest(
                maskCommonPtr->dsaSrcIsTrunk,
                patternCommonPtr->dsaSrcIsTrunk,
                &trunkId);
        }
        else
        {
            /* we do the pattern now */
            gtPclPatternTrunkConvertFromCpssToTest(
                maskCommonPtr->dsaSrcIsTrunk,
                patternCommonPtr->dsaSrcIsTrunk,
                &trunkId);
        }

        commonPtr->dsaSrcPortTrunk = (GT_U8)trunkId;
    }


    return;
}


/* convert trunk fields of test for mask ,pattern of cpss */
static void ttiRuleTrunkInfoFromCpssToTest_1
(
    void
)
{
    ttiRuleTrunkInfoFromCpssToTest(
    &maskData.ipv4.common);

    ttiRuleTrunkInfoFromCpssToTest(
    &patternData.ipv4.common);
}


/*******************************************************************************
* wrCpssDxChTtiRule_KEY_IPV4_Set
*
* DESCRIPTION:
*       This function maps input arguments to IPv4 TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_KEY_IPV4_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_U8 port;

    ruleFormat = CPSS_DXCH_TTI_KEY_IPV4_E;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr        = &maskData;
        mask_ruleIndex = (GT_U32)inFields[0];
    }
    else /* pattern */
    {
        rulePtr           = &patternData;
        pattern_ruleIndex = (GT_U32)inFields[0];
    }

    rulePtr->ipv4.common.pclId           = (GT_U32)inFields[2];
    rulePtr->ipv4.common.srcIsTrunk      = (GT_U32)inFields[3];
    rulePtr->ipv4.common.srcPortTrunk    = (GT_U32)inFields[4];

    if (rulePtr->ipv4.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_U8)rulePtr->ipv4.common.srcPortTrunk;

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum,port);

        rulePtr->ipv4.common.srcPortTrunk = port;
    }

    galtisMacAddr(&rulePtr->ipv4.common.mac,(GT_U8*)inFields[5]);
    rulePtr->ipv4.common.vid             = (GT_U16)inFields[6];
    rulePtr->ipv4.common.isTagged        = (GT_BOOL)inFields[7];
    rulePtr->ipv4.tunneltype             = (GT_U32)inFields[8];
    galtisIpAddr(&rulePtr->ipv4.srcIp,(GT_U8*)inFields[9]);
    galtisIpAddr(&rulePtr->ipv4.destIp,(GT_U8*)inFields[10]);
    rulePtr->ipv4.isArp                  = (GT_BOOL)inFields[11];
    rulePtr->ipv4.common.dsaSrcIsTrunk   = (GT_BOOL)inFields[12];
    rulePtr->ipv4.common.dsaSrcPortTrunk =(GT_U8)inFields[13];
    rulePtr->ipv4.common.dsaSrcDevice    = (GT_U8)inFields[14];

    /* convert the trunk fields */
    ttiRuleTrunkInfoFromTestToCpss_1(inFields[1]);

    if (inFields[1] == 0) /* mask */
    {
        mask_set       = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_set       = GT_TRUE;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_KEY_MPLS_Set
*
* DESCRIPTION:
*       This function maps input arguments to MPLS TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_KEY_MPLS_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_U8 port;

    ruleFormat = CPSS_DXCH_TTI_KEY_MPLS_E;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr        = &maskData;
        mask_ruleIndex = (GT_U32)inFields[0];
    }
    else /* pattern */
    {
        rulePtr           = &patternData;
        pattern_ruleIndex = (GT_U32)inFields[0];
    }

    rulePtr->mpls.common.pclId           = (GT_U32)inFields[2];
    rulePtr->mpls.common.srcIsTrunk      = (GT_U32)inFields[3];
    rulePtr->mpls.common.srcPortTrunk    = (GT_U32)inFields[4];

    if (rulePtr->mpls.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_U8)rulePtr->mpls.common.srcPortTrunk;

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum,port);

        rulePtr->mpls.common.srcPortTrunk = port;
    }

    galtisMacAddr(&rulePtr->mpls.common.mac,(GT_U8*)inFields[5]);
    rulePtr->mpls.common.vid             = (GT_U16)inFields[6];
    rulePtr->mpls.common.isTagged        = (GT_BOOL)inFields[7];
    rulePtr->mpls.label0                 = (GT_U32)inFields[8];
    rulePtr->mpls.exp0                   = (GT_U32)inFields[9];
    rulePtr->mpls.label1                 = (GT_U32)inFields[10];
    rulePtr->mpls.exp1                   = (GT_U32)inFields[11];
    rulePtr->mpls.label2                 = (GT_U32)inFields[12];
    rulePtr->mpls.exp2                   = (GT_U32)inFields[13];
    rulePtr->mpls.numOfLabels            = (GT_U32)inFields[14];
    rulePtr->mpls.protocolAboveMPLS      = (GT_U32)inFields[15];
    rulePtr->mpls.common.dsaSrcIsTrunk   = (GT_BOOL)inFields[16];
    rulePtr->mpls.common.dsaSrcPortTrunk = (GT_U8)inFields[17];
    rulePtr->mpls.common.dsaSrcDevice    = (GT_U8)inFields[18];

    /* convert the trunk fields */
    ttiRuleTrunkInfoFromTestToCpss_1(inFields[1]);

    if (inFields[1] == 0) /* mask */
    {
        mask_set       = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_set       = GT_TRUE;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_KEY_ETH_Set
*
* DESCRIPTION:
*       This function maps input arguments to ETH TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_KEY_ETH_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_U8 port;

    ruleFormat = CPSS_DXCH_TTI_KEY_ETH_E;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr        = &maskData;
        mask_ruleIndex = (GT_U32)inFields[0];
    }
    else /* pattern */
    {
        rulePtr           = &patternData;
        pattern_ruleIndex = (GT_U32)inFields[0];
    }

    rulePtr->eth.common.pclId           = (GT_U32)inFields[2];
    rulePtr->eth.common.srcIsTrunk      = (GT_U32)inFields[3];
    rulePtr->eth.common.srcPortTrunk    = (GT_U32)inFields[4];

    if (rulePtr->eth.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_U8)rulePtr->eth.common.srcPortTrunk;

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum,port);

        rulePtr->eth.common.srcPortTrunk = port;
    }

    galtisMacAddr(&rulePtr->eth.common.mac,(GT_U8*)inFields[5]);
    rulePtr->eth.common.vid             = (GT_U16)inFields[6];
    rulePtr->eth.common.isTagged        = (GT_BOOL)inFields[7];
    rulePtr->eth.up0                    = inFields[8];
    rulePtr->eth.cfi0                   = inFields[9];
    rulePtr->eth.isVlan1Exists          = (GT_BOOL)inFields[10];
    rulePtr->eth.vid1                   = (GT_U16)inFields[11];
    rulePtr->eth.up1                    = inFields[12];
    rulePtr->eth.cfi1                   = inFields[13];
    rulePtr->eth.etherType              = (GT_U32)inFields[14];
    rulePtr->eth.macToMe                = (GT_BOOL)inFields[15];
    rulePtr->eth.common.dsaSrcIsTrunk   = inFields[16];
    rulePtr->eth.common.dsaSrcPortTrunk = (GT_U8)inFields[17];
    rulePtr->eth.common.dsaSrcDevice    = (GT_U8)inFields[18];

    /* convert the trunk fields */
    ttiRuleTrunkInfoFromTestToCpss_1(inFields[1]);

    if (inFields[1] == 0) /* mask */
    {
        mask_set       = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_set       = GT_TRUE;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleSetFirst
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleSetFirst
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

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));

    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;


    switch (inArgs[1])
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleSetNext
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result = GT_OK;
    GT_U8                       devNum;
    GT_U32                      ruleIndex;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType;
    GT_U32                      actionIndex;

    CPSS_DXCH_TTI_ACTION_UNT    *actionPtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Check Validity */
    if (inFields[1] > 1)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be MASK or PATTERN).\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 0) && mask_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 1) && pattern_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inFields[0];

    switch (inArgs[1])
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (mask_set && pattern_set && (mask_ruleIndex != pattern_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }


    /* Get the action */
    ttiActionDatabaseGet(devNum,ruleIndex,&actionIndex);

    if ((mask_set && pattern_set) && (actionIndex != INVALID_RULE_INDEX))
    {
        actionPtr = &(ttiActionDatabase[actionIndex].actionEntry);

        keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];
        ttiRuleFormat[ruleIndex] = keyType;

        /* call cpss api function */
        result = pg_wrap_cpssDxChTtiRuleSet(devNum,ruleIndex,keyType,&patternData,&maskData,ttiActionDatabase[actionIndex].actionType,actionPtr);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
        cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleEndSet
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if (mask_set || pattern_set)
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_KEY_IPV4_Get
*
* DESCRIPTION:
*       This function maps IPv4 TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_KEY_IPV4_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_U8 port;

    ruleFormat = CPSS_DXCH_TTI_KEY_IPV4_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->ipv4.common.pclId;
    inFields[3]  = rulePtr->ipv4.common.srcIsTrunk;
    inFields[4]  = rulePtr->ipv4.common.srcPortTrunk;

    if (rulePtr->ipv4.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_U8)rulePtr->ipv4.common.srcPortTrunk;

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum,port);

        inFields[4] = port;
    }

    inFields[6]  = rulePtr->ipv4.common.vid;
    inFields[7]  = rulePtr->ipv4.common.isTagged;
    inFields[8]  = rulePtr->ipv4.tunneltype;
    inFields[11] = rulePtr->ipv4.isArp;
    inFields[12] = rulePtr->ipv4.common.dsaSrcIsTrunk;
    inFields[13] = rulePtr->ipv4.common.dsaSrcPortTrunk;
    inFields[14] = rulePtr->ipv4.common.dsaSrcDevice;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%4b%4b%d%d%d%d",
                inFields[0], inFields[1],  inFields[2], inFields[3],
                inFields[4], rulePtr->ipv4.common.mac.arEther, inFields[6],
                inFields[7], inFields[8], rulePtr->ipv4.srcIp.arIP,
                rulePtr->ipv4.destIp.arIP, inFields[11], inFields[12],inFields[13],inFields[14]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_KEY_MPLS_Get
*
* DESCRIPTION:
*       This function maps MPLS TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_KEY_MPLS_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_U8 port;

    ruleFormat = CPSS_DXCH_TTI_KEY_MPLS_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->mpls.common.pclId;
    inFields[3]  = rulePtr->mpls.common.srcIsTrunk;
    inFields[4]  = rulePtr->mpls.common.srcPortTrunk;

    if (rulePtr->mpls.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_U8)rulePtr->mpls.common.srcPortTrunk;

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum,port);

        inFields[4] = port;
    }

    inFields[6]  = rulePtr->mpls.common.vid;
    inFields[7]  = rulePtr->mpls.common.isTagged;
    inFields[8]  = rulePtr->mpls.label0;
    inFields[9]  = rulePtr->mpls.exp0 ;
    inFields[10] = rulePtr->mpls.label1;
    inFields[11] = rulePtr->mpls.exp1;
    inFields[12] = rulePtr->mpls.label2;
    inFields[13] = rulePtr->mpls.exp2;
    inFields[14] = rulePtr->mpls.numOfLabels;
    inFields[15] = rulePtr->mpls.protocolAboveMPLS;
    inFields[16] = rulePtr->mpls.common.dsaSrcIsTrunk;
    inFields[17] = rulePtr->mpls.common.dsaSrcPortTrunk;
    inFields[18] = rulePtr->mpls.common.dsaSrcDevice;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->mpls.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11], inFields[12],inFields[13], inFields[14],
                inFields[15],inFields[16],inFields[17],inFields[18]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;

}

/*******************************************************************************
* wrCpssDxChTtiRule_KEY_ETH_Get
*
* DESCRIPTION:
*       This function maps ETH TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_KEY_ETH_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_U8 port;

    ruleFormat = CPSS_DXCH_TTI_KEY_ETH_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->eth.common.pclId;
    inFields[3]  = rulePtr->eth.common.srcIsTrunk;
    inFields[4]  = rulePtr->eth.common.srcPortTrunk;

    if (rulePtr->eth.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_U8)rulePtr->eth.common.srcPortTrunk;

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum,port);

        inFields[4] = port;
    }

    inFields[6]  = rulePtr->eth.common.vid;
    inFields[7]  = rulePtr->eth.common.isTagged;
    inFields[8]  = rulePtr->eth.up0;
    inFields[9]  = rulePtr->eth.cfi0;
    inFields[10] = rulePtr->eth.isVlan1Exists;
    inFields[11] = rulePtr->eth.vid1;
    inFields[12] = rulePtr->eth.up1;
    inFields[13] = rulePtr->eth.cfi1;
    inFields[14] = rulePtr->eth.etherType;
    inFields[15] = rulePtr->eth.macToMe;
    inFields[16] = rulePtr->eth.common.dsaSrcIsTrunk;
    inFields[17] = rulePtr->eth.common.dsaSrcPortTrunk;
    inFields[18] = rulePtr->eth.common.dsaSrcDevice;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->eth.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11],inFields[12],inFields[13], inFields[14],
                    inFields[15],inFields[16],inFields[17],inFields[18]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChTtiRuleGet
*
* DESCRIPTION:
*       This function gets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    GT_U32                      actionIndex;

    /* if isMask == GT_TRUE it is mask's turn (the first one to get), so there need to refresh the data
    else the data is updated and it is pattern's turn (was updated earlier)*/
    if (isMask)
    {
        /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];
        keyType = inArgs[1];

        actionIndex = INVALID_RULE_INDEX;
        for (; actionIndex == INVALID_RULE_INDEX && ruleIndex < 3072; ruleIndex++)
            ttiActionDatabaseGet(devNum,ruleIndex,&actionIndex);

        if (ruleIndex == 3072)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_OK, "%d", -1);

            return CMD_OK;
        }

        if (actionIndex == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_OK, "%d", -1);

            return CMD_OK;
        }

        ruleIndex--;

        /* call cpss api function */
        result = pg_wrap_cpssDxChTtiRuleGet(devNum,ruleIndex,keyType,&patternData,&maskData,ttiActionDatabase[actionIndex].actionType,&action);
        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
    }

    switch (ttiRuleFormat[ruleIndex])
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_KEY_IPV4_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_KEY_MPLS_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_KEY_ETH_Get(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (isMask)
        ruleIndex++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleGetFirst
*
* DESCRIPTION:
*       This function gets first TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleGetFirst
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

    return wrCpssDxChTtiRuleGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_IPV4_Set
*
* DESCRIPTION:
*       This function maps input arguments to IPv4 TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_IPV4_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* IPv4 key in TtiRule_1 table is the same as in TtiRule table */
    return wrCpssDxChTtiRule_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_MPLS_Set
*
* DESCRIPTION:
*       This function maps input arguments to MPLS TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_MPLS_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* MPLS key in TtiRule_1 table is the same as in TtiRule table */
    return wrCpssDxChTtiRule_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_ETH_Set
*
* DESCRIPTION:
*       This function maps input arguments to ETH TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_ETH_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_U8 port;

    ruleFormat = CPSS_DXCH_TTI_KEY_ETH_E;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr        = &maskData;
        mask_ruleIndex = (GT_U32)inFields[0];
    }
    else /* pattern */
    {
        rulePtr           = &patternData;
        pattern_ruleIndex = (GT_U32)inFields[0];
    }

    rulePtr->eth.common.pclId           = (GT_U32)inFields[2];
    rulePtr->eth.common.srcIsTrunk      = (GT_U32)inFields[3];
    rulePtr->eth.common.srcPortTrunk    = (GT_U32)inFields[4];

    if (rulePtr->eth.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_U8)rulePtr->eth.common.srcPortTrunk;

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum,port);

        rulePtr->eth.common.srcPortTrunk = port;
    }

    galtisMacAddr(&rulePtr->eth.common.mac,(GT_U8*)inFields[5]);
    rulePtr->eth.common.vid             = (GT_U16)inFields[6];
    rulePtr->eth.common.isTagged        = (GT_BOOL)inFields[7];
    rulePtr->eth.up0                    = inFields[8];
    rulePtr->eth.cfi0                   = inFields[9];
    rulePtr->eth.isVlan1Exists          = (GT_BOOL)inFields[10];
    rulePtr->eth.vid1                   = (GT_U16)inFields[11];
    rulePtr->eth.up1                    = inFields[12];
    rulePtr->eth.cfi1                   = inFields[13];
    rulePtr->eth.etherType              = (GT_U32)inFields[14];
    rulePtr->eth.macToMe                = (GT_BOOL)inFields[15];
    rulePtr->eth.common.dsaSrcIsTrunk   = inFields[16];
    rulePtr->eth.common.dsaSrcPortTrunk = (GT_U8)inFields[17];
    rulePtr->eth.common.dsaSrcDevice    = (GT_U8)inFields[18];
    rulePtr->eth.srcId                  = (GT_U32)inFields[19];
    rulePtr->eth.dsaQosProfile          = (GT_U32)inFields[20];

    /* convert the trunk fields */
    ttiRuleTrunkInfoFromTestToCpss_1(inFields[1]);


    if (inFields[1] == 0) /* mask */
    {
        mask_set       = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_set       = GT_TRUE;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_MIM_Set
*
* DESCRIPTION:
*       This function maps input arguments to MIM TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_MIM_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_U8 port;

    ruleFormat = CPSS_DXCH_TTI_KEY_MIM_E;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr        = &maskData;
        mask_ruleIndex = (GT_U32)inFields[0];
    }
    else /* pattern */
    {
        rulePtr           = &patternData;
        pattern_ruleIndex = (GT_U32)inFields[0];
    }

    rulePtr->mim.common.pclId           = (GT_U32)inFields[2];
    rulePtr->mim.common.srcIsTrunk      = (GT_BOOL)inFields[3];
    rulePtr->mim.common.srcPortTrunk    = (GT_U32)inFields[4];

    if (rulePtr->mim.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_U8)rulePtr->mim.common.srcPortTrunk;

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum,port);

        rulePtr->mim.common.srcPortTrunk = port;
    }

    galtisMacAddr(&rulePtr->mim.common.mac,(GT_U8*)inFields[5]);
    rulePtr->mim.common.vid             = (GT_U16)inFields[6];
    rulePtr->mim.common.isTagged        = (GT_BOOL)inFields[7];
    rulePtr->mim.common.dsaSrcIsTrunk   = (GT_BOOL)inFields[8];
    rulePtr->mim.common.dsaSrcPortTrunk = (GT_U8)inFields[9];
    rulePtr->mim.common.dsaSrcDevice    = (GT_U8)inFields[10];
    rulePtr->mim.bUp                    = (GT_U32)inFields[11];
    rulePtr->mim.bDp                    = (GT_U32)inFields[12];
    rulePtr->mim.iSid                   = (GT_U32)inFields[13];
    rulePtr->mim.iUp                    = (GT_U32)inFields[14];
    rulePtr->mim.iDp                    = (GT_U32)inFields[15];
    rulePtr->mim.iRes1                  = (GT_U32)inFields[16];
    rulePtr->mim.iRes2                  = (GT_U32)inFields[17];

    /* convert the trunk fields */
    ttiRuleTrunkInfoFromTestToCpss_1(inFields[1]);


    if (inFields[1] == 0) /* mask */
    {
        mask_set       = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_set       = GT_TRUE;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1SetFirst
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1SetFirst
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

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));

    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;

    switch (inArgs[3]) /* key type tab */
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_1_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_1_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_1_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_1_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1SetNext
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1SetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;
    GT_U8                           devNum;
    GT_U32                          ruleIndex;
    CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType;
    GT_U32                          actionIndex;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType;
    CPSS_DXCH_TTI_ACTION_UNT        *actionPtr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Check Validity */
    if (inFields[1] > 1)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be MASK or PATTERN).\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 0) && mask_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 1) && pattern_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inFields[0];

    switch (inArgs[3]) /* key type tab */
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_1_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_1_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_1_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_1_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (mask_set && pattern_set && (mask_ruleIndex != pattern_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }


    /* Get the action */
    ttiActionDatabaseGet(devNum,ruleIndex,&actionIndex);

    if ((mask_set && pattern_set) && (actionIndex != INVALID_RULE_INDEX))
    {
        actionPtr                    = &(ttiActionDatabase[actionIndex].actionEntry);
        keyType                      = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[3];
        ttiRuleFormat[ruleIndex]     = keyType;
        actionType                   = ttiActionDatabase[actionIndex].actionType;
        ttiRuleActionType[ruleIndex] = actionType;

        /* call cpss api function */
        result = pg_wrap_cpssDxChTtiRuleSet(devNum,ruleIndex,keyType,&patternData,&maskData,actionType,actionPtr);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
        cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1EndSet
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1EndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if (mask_set || pattern_set)
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_IPV4_Get
*
* DESCRIPTION:
*       This function maps IPv4 TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_IPV4_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* IPv4 key in TtiRule_1 table is the same as in TtiRule table */
    return wrCpssDxChTtiRule_KEY_IPV4_Get(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_MPLS_Get
*
* DESCRIPTION:
*       This function maps MPLS TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_MPLS_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* MPLS key in TtiRule_1 table is the same as in TtiRule table */
    return wrCpssDxChTtiRule_KEY_MPLS_Get(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_ETH_Get
*
* DESCRIPTION:
*       This function maps ETH TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_ETH_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_U8 port;

    ruleFormat = CPSS_DXCH_TTI_KEY_ETH_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->eth.common.pclId;
    inFields[3]  = rulePtr->eth.common.srcIsTrunk;
    inFields[4]  = rulePtr->eth.common.srcPortTrunk;

    if (rulePtr->eth.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_U8)rulePtr->eth.common.srcPortTrunk;

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum,port);

        inFields[4] = port;
    }

    inFields[6]  = rulePtr->eth.common.vid;
    inFields[7]  = rulePtr->eth.common.isTagged;
    inFields[8]  = rulePtr->eth.up0;
    inFields[9]  = rulePtr->eth.cfi0;
    inFields[10] = rulePtr->eth.isVlan1Exists;
    inFields[11] = rulePtr->eth.vid1;
    inFields[12] = rulePtr->eth.up1;
    inFields[13] = rulePtr->eth.cfi1;
    inFields[14] = rulePtr->eth.etherType;
    inFields[15] = rulePtr->eth.macToMe;
    inFields[16] = rulePtr->eth.common.dsaSrcIsTrunk;
    inFields[17] = rulePtr->eth.common.dsaSrcPortTrunk;
    inFields[18] = rulePtr->eth.common.dsaSrcDevice;
    inFields[19] = rulePtr->eth.srcId;
    inFields[20] = rulePtr->eth.dsaQosProfile;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->eth.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11],inFields[12],inFields[13], inFields[14],
                inFields[15],inFields[16],inFields[17],inFields[18],
                inFields[19],inFields[20]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_MIM_Get
*
* DESCRIPTION:
*       This function maps MIM TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_MIM_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_U8 port;

    ruleFormat = CPSS_DXCH_TTI_KEY_MIM_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2] = rulePtr->mim.common.pclId;
    inFields[3] = rulePtr->mim.common.srcIsTrunk;
    inFields[4] = rulePtr->mim.common.srcPortTrunk;

    if (rulePtr->mim.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_U8)rulePtr->mim.common.srcPortTrunk;

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum,port);

        inFields[4] = port;
    }

    inFields[6] = rulePtr->mim.common.vid;
    inFields[7] = rulePtr->mim.common.isTagged;
    inFields[8] = rulePtr->mim.common.dsaSrcIsTrunk;
    inFields[9] = rulePtr->mim.common.dsaSrcPortTrunk;
    inFields[10] = rulePtr->mim.common.dsaSrcDevice;
    inFields[11] = rulePtr->mim.bUp;
    inFields[12] = rulePtr->mim.bDp;
    inFields[13] = rulePtr->mim.iSid;
    inFields[14] = rulePtr->mim.iUp;
    inFields[15] = rulePtr->mim.iDp;
    inFields[16] = rulePtr->mim.iRes1;
    inFields[17] = rulePtr->mim.iRes2;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->eth.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11],inFields[12],inFields[13], inFields[14],
                inFields[15],inFields[16],inFields[17]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1Get
*
* DESCRIPTION:
*       This function gets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    GT_BOOL                     ruleValid;

    /* if isMask == GT_TRUE it is mask's turn (the first one to get), so there need to refresh the data
    else the data is updated and it is pattern's turn (was updated earlier)*/
    if (isMask)
    {
        /* check if requested refresh range is already refreshed */
        if ((ruleIndex >= 3072) || (ruleIndex > ruleIndexMaxGet))
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];
        keyType = inArgs[1];

        /* look for another valid rule within the range */
        for ((ruleValid = GT_FALSE) ; ((ruleValid == GT_FALSE) && (ruleIndex < 3072) && (ruleIndex <= ruleIndexMaxGet)) ; ruleIndex++)
        {
            /* check if the rule is valid */
            result = pg_wrap_cpssDxChTtiRuleValidStatusGet(devNum,ruleIndex,&ruleValid);
            if (result != GT_OK)
            {
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }
        }

        /* check if no valid rule was found in the requested range */
        if ((ruleValid == GT_FALSE))
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        /* valid rule was found in the requested range */
        ruleIndex--;

        /* call cpss api function, note that it is assumed that rule action
           type is not modified after setting the rule */
        result = pg_wrap_cpssDxChTtiRuleGet(devNum,ruleIndex,ttiRuleFormat[ruleIndex],&patternData,&maskData,ttiRuleActionType[ruleIndex],&action);
    }

    switch (ttiRuleFormat[ruleIndex])
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_1_KEY_IPV4_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_1_KEY_MPLS_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_1_KEY_ETH_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_1_KEY_MIM_Get(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (isMask)
        ruleIndex++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1GetFirst
*
* DESCRIPTION:
*       This function gets first TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1GetFirst
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

    /* check if the application request 0 entries */
    if (inArgs[2] == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    ruleIndex       = (GT_U32)inArgs[1];
    ruleIndexMaxGet = ruleIndex + (GT_U32)inArgs[2] - 1;

    return wrCpssDxChTtiRule_1Get(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChTtiMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IP_V4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*       macMode       - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
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
static CMD_STATUS wrCpssDxChTtiMacModeSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT keyType;
    CPSS_DXCH_TTI_MAC_MODE_ENT macMode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];
    macMode = (CPSS_DXCH_TTI_MAC_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiMacModeSet(devNum, keyType, macMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*       All DXCH devices.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IP_V4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*
* OUTPUTS:
*       macModePtr    - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
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
static CMD_STATUS wrCpssDxChTtiMacModeGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT keyType;
    CPSS_DXCH_TTI_MAC_MODE_ENT macMode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiMacModeGet(devNum, keyType, &macMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", macMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPclIdSet
*
* DESCRIPTION:
*       This function sets the PCL ID for the specified key type. The PCL ID
*       is used to distinguish between different TTI keys in the TCAM.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E
*       pclId         - PCL ID value (10 bits)
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
*       The TTI PCL ID values are only relevant for the TTI TCAM and are not
*       related to the PCL ID values in the PCL TCAM.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPclIdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType;
    GT_U32                      pclId;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum  = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];
    pclId   = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiPclIdSet(devNum,keyType,pclId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPclIdGet
*
* DESCRIPTION:
*       This function gets the PCL ID for the specified key type. The PCL ID
*       is used to distinguish between different TTI keys in the TCAM.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E
*
* OUTPUTS:
*       pclIdPtr      - (points to) PCL ID value (10 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The TTI PCL ID values are only relevant for the TTI TCAM and are not
*       related to the PCL ID values in the PCL TCAM.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPclIdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType;
    GT_U32                      pclId;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum  = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiPclIdGet(devNum,keyType,&pclId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", pclId);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiIpv4GreEthTypeSet
*
* DESCRIPTION:
*       This function sets the IPv4 GRE protocol Ethernet type. Two Ethernet
*       types are supported, used by TTI for Ethernet over GRE tunnels
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type; valid values:
*                           CPSS_DXCH_TTI_IPV4_GRE0_E
*                           CPSS_DXCH_TTI_IPV4_GRE1_E
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
static CMD_STATUS wrCpssDxChTtiIpv4GreEthTypeSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_TTI_IPV4_GRE_TYPE_ENT greTunnelType;
    GT_U32 ethType;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    greTunnelType = (CPSS_DXCH_TTI_IPV4_GRE_TYPE_ENT)inArgs[1];
    ethType = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiIpv4GreEthTypeSet(devNum, greTunnelType, ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiIpv4GreEthTypeGet
*
* DESCRIPTION:
*       This function gets the IPv4 GRE protocol Ethernet type. Two Ethernet
*       types are supported, used by TTI for Ethernet over GRE tunnels
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type; valid values:
*                           CPSS_DXCH_TTI_IPV4_GRE0_E
*                           CPSS_DXCH_TTI_IPV4_GRE1_E
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
static CMD_STATUS wrCpssDxChTtiIpv4GreEthTypeGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_TTI_IPV4_GRE_TYPE_ENT greTunnelType;
    GT_U32 ethType;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    greTunnelType = (CPSS_DXCH_TTI_IPV4_GRE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiIpv4GreEthTypeGet(devNum, greTunnelType, &ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ethType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMimEthTypeSet
*
* DESCRIPTION:
*       This function sets the MIM Ethernet type.
*
* APPLICABLE DEVICES: DxChXcat and above.
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
*      specialEthertypes is used to identify tunnel for incoming packets in
*      the TTI.
*      routerHdrAltIEthertype is used for outgoing tunnel in the header alteration.
*      Those registers are configured to have the same value.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMimEthTypeSet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ethType = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiMimEthTypeSet(devNum, ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMimEthTypeGet
*
* DESCRIPTION:
*       This function gets the MIM Ethernet type.
*
* APPLICABLE DEVICES: DxChXcat and above.
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
static CMD_STATUS wrCpssDxChTtiMimEthTypeGet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTtiMimEthTypeGet(devNum, &ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ethType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMplsEthTypeSet
*
* DESCRIPTION:
*       This function sets the MPLS Ethernet type.
*
* APPLICABLE DEVICES: DxChXcat A1 and above.
*
* INPUTS:
*       devNum        - device number
*       ucMcSet       - weather to set it for unicast packets or multicast.
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
*      mplsEthertypes is used to identify tunnel for incoming packets in
*      the TTI.
*      hdrAltMplsEthertype is used for outgoing tunnel in the header alteration.
*      Those registers are configured to have the same value.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMplsEthTypeSet
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
    CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ucMcSet = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[1];
    ethType = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiMplsEthTypeSet(devNum, ucMcSet, ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMplsEthTypeGet
*
* DESCRIPTION:
*       This function gets the MPLS Ethernet type.
*
* APPLICABLE DEVICES: DxChXcat A1 and above.
*
* INPUTS:
*       devNum        - device number
*       ucMcSet       - weather to get it for unicast packets or multicast.
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
static CMD_STATUS wrCpssDxChTtiMplsEthTypeGet
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
    CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ucMcSet = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiMplsEthTypeGet(devNum, ucMcSet, &ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ethType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiExceptionCmdSet
*
* DESCRIPTION:
*       Set tunnel termination exception command.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum        - device number
*       exceptionType - tunnel termination exception type to set command for
*       command       - command to set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*
* COMMENTS:
*       Commands for the different exceptions are:
*
*       CPSS_DXCH_TTI_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiExceptionCmdSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_TTI_EXCEPTION_ENT exceptionType;
    CPSS_PACKET_CMD_ENT command;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_DXCH_TTI_EXCEPTION_ENT)inArgs[1];
    command = (CPSS_PACKET_CMD_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiExceptionCmdSet(devNum, exceptionType, command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiExceptionCmdGet
*
* DESCRIPTION:
*       Get tunnel termination exception command.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*
* OUTPUTS:
*       commandPtr    - points to the command for the exception type
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_PTR  - on NULL pointer
*       GT_FAIL     - on error.
*
* COMMENTS:
*       Commands for the different exceptions are:
*
**       CPSS_DXCH_TTI_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiExceptionCmdGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_TTI_EXCEPTION_ENT exceptionType;
    CPSS_PACKET_CMD_ENT command;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_DXCH_TTI_EXCEPTION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiExceptionCmdGet(devNum, exceptionType, &command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", command);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChTtiRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM (0..3k-1).
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
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
static CMD_STATUS wrCpssDxChTtiRuleValidStatusSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    IN GT_U8   devNum;
    IN GT_U32  routerTtiTcamRow;
    IN GT_BOOL valid;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum           = (GT_U8)inArgs[0];
    routerTtiTcamRow = (GT_U32)inArgs[1];
    valid            = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiRuleValidStatusSet(devNum, routerTtiTcamRow, valid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChTtiRuleValidStatusGet
*
* DESCRIPTION:
*       This function returns the valid status of the rule in TCAM
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM (0..3k-1).
*
* OUTPUTS:
*       validPtr          - GT_TRUE - valid, GT_FALSE - invalid
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
static CMD_STATUS wrCpssDxChTtiRuleValidStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    IN  GT_U8   devNum;
    IN  GT_U32  routerTtiTcamRow;
    OUT GT_BOOL valid;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum           = (GT_U8)inArgs[0];
    routerTtiTcamRow = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiRuleValidStatusGet(devNum, routerTtiTcamRow, &valid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", valid);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_IPV4_Set
*
* DESCRIPTION:
*       This function maps input arguments to IPv4 TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_IPV4_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr = &maskData;
    }
    else /* pattern */
    {
        rulePtr = &patternData;
    }

    /* IPv4 key in TtiRule_2 table is the same as in TtiRule table except for field sourcePortGroupId */
    wrCpssDxChTtiRule_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
    rulePtr->ipv4.common.sourcePortGroupId = (GT_U32)inFields[15];

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_MPLS_Set
*
* DESCRIPTION:
*       This function maps input arguments to MPLS TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_MPLS_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr = &maskData;
    }
    else /* pattern */
    {
        rulePtr = &patternData;
    }

    /* MPLS key in TtiRule_2 table is the same as in TtiRule table except for field sourcePortGroupId */
    wrCpssDxChTtiRule_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
    rulePtr->mpls.common.sourcePortGroupId = (GT_U32)inFields[19];

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_ETH_Set
*
* DESCRIPTION:
*       This function maps input arguments to ETH TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_ETH_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr = &maskData;
    }
    else /* pattern */
    {
        rulePtr = &patternData;
    }

    /* ETH key in TtiRule_2 table is the same as in TtiRule_1 table except for field sourcePortGroupId */
    wrCpssDxChTtiRule_1_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
    rulePtr->eth.common.sourcePortGroupId = (GT_U32)inFields[21];

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_MIM_Set
*
* DESCRIPTION:
*       This function maps input arguments to MIM TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_MIM_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr = &maskData;
    }
    else /* pattern */
    {
        rulePtr = &patternData;
    }

    /* MIM key in TtiRule_2 table is the same as in TtiRule_1 table except for field sourcePortGroupId */
    wrCpssDxChTtiRule_1_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
    rulePtr->mim.common.sourcePortGroupId = (GT_U32)inFields[18];

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2SetFirst
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2SetFirst
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

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));

    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;

    switch (inArgs[3]) /* key type tab */
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_2_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_2_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2SetNext
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2SetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;
    GT_U8                           devNum;
    GT_U32                          ruleIndex;
    CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType;
    GT_U32                          actionIndex;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType;
    CPSS_DXCH_TTI_ACTION_UNT        *actionPtr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Check Validity */
    if (inFields[1] > 1)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be MASK or PATTERN).\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 0) && mask_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 1) && pattern_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inFields[0];

    switch (inArgs[3]) /* key type tab */
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_2_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_2_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (mask_set && pattern_set && (mask_ruleIndex != pattern_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }


    /* Get the action */
    ttiActionDatabaseGet(devNum,ruleIndex,&actionIndex);

    if ((mask_set && pattern_set) && (actionIndex != INVALID_RULE_INDEX))
    {
        actionPtr                    = &(ttiActionDatabase[actionIndex].actionEntry);
        keyType                      = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[3];
        ttiRuleFormat[ruleIndex]     = keyType;
        actionType                   = ttiActionDatabase[actionIndex].actionType;
        ttiRuleActionType[ruleIndex] = actionType;

        /* call cpss api function */
        result = pg_wrap_cpssDxChTtiRuleSet(devNum,ruleIndex,keyType,&patternData,&maskData,actionType,actionPtr);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
        cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2EndSet
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2EndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if (mask_set || pattern_set)
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_IPV4_Get
*
* DESCRIPTION:
*       This function maps IPv4 TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_IPV4_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_U8 port;

    ruleFormat = CPSS_DXCH_TTI_KEY_IPV4_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->ipv4.common.pclId;
    inFields[3]  = rulePtr->ipv4.common.srcIsTrunk;
    inFields[4]  = rulePtr->ipv4.common.srcPortTrunk;

    if (rulePtr->ipv4.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_U8)rulePtr->ipv4.common.srcPortTrunk;

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum,port);

        inFields[4] = port;
    }

    inFields[6]  = rulePtr->ipv4.common.vid;
    inFields[7]  = rulePtr->ipv4.common.isTagged;
    inFields[8]  = rulePtr->ipv4.tunneltype;
    inFields[11] = rulePtr->ipv4.isArp;
    inFields[12] = rulePtr->ipv4.common.dsaSrcIsTrunk;
    inFields[13] = rulePtr->ipv4.common.dsaSrcPortTrunk;
    inFields[14] = rulePtr->ipv4.common.dsaSrcDevice;
    inFields[15] = rulePtr->ipv4.common.sourcePortGroupId;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%4b%4b%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2], inFields[3],
                inFields[4], rulePtr->ipv4.common.mac.arEther, inFields[6],
                inFields[7], inFields[8], rulePtr->ipv4.srcIp.arIP,
                rulePtr->ipv4.destIp.arIP, inFields[11], inFields[12],
                inFields[13],inFields[14],inFields[15]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_MPLS_Get
*
* DESCRIPTION:
*       This function maps MPLS TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_MPLS_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_U8 port;

    ruleFormat = CPSS_DXCH_TTI_KEY_MPLS_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->mpls.common.pclId;
    inFields[3]  = rulePtr->mpls.common.srcIsTrunk;
    inFields[4]  = rulePtr->mpls.common.srcPortTrunk;

    if (rulePtr->mpls.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_U8)rulePtr->mpls.common.srcPortTrunk;

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum,port);

        inFields[4] = port;
    }

    inFields[6]  = rulePtr->mpls.common.vid;
    inFields[7]  = rulePtr->mpls.common.isTagged;
    inFields[8]  = rulePtr->mpls.label0;
    inFields[9]  = rulePtr->mpls.exp0 ;
    inFields[10] = rulePtr->mpls.label1;
    inFields[11] = rulePtr->mpls.exp1;
    inFields[12] = rulePtr->mpls.label2;
    inFields[13] = rulePtr->mpls.exp2;
    inFields[14] = rulePtr->mpls.numOfLabels;
    inFields[15] = rulePtr->mpls.protocolAboveMPLS;
    inFields[16] = rulePtr->mpls.common.dsaSrcIsTrunk;
    inFields[17] = rulePtr->mpls.common.dsaSrcPortTrunk;
    inFields[18] = rulePtr->mpls.common.dsaSrcDevice;
    inFields[19] = rulePtr->mpls.common.sourcePortGroupId;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->mpls.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11], inFields[12],inFields[13], inFields[14],
                inFields[15],inFields[16],inFields[17],inFields[18],inFields[19]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_ETH_Get
*
* DESCRIPTION:
*       This function maps ETH TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_ETH_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_U8 port;

    ruleFormat = CPSS_DXCH_TTI_KEY_ETH_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->eth.common.pclId;
    inFields[3]  = rulePtr->eth.common.srcIsTrunk;
    inFields[4]  = rulePtr->eth.common.srcPortTrunk;

    if (rulePtr->eth.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_U8)rulePtr->eth.common.srcPortTrunk;

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum,port);

        inFields[4] = port;
    }

    inFields[6]  = rulePtr->eth.common.vid;
    inFields[7]  = rulePtr->eth.common.isTagged;
    inFields[8]  = rulePtr->eth.up0;
    inFields[9]  = rulePtr->eth.cfi0;
    inFields[10] = rulePtr->eth.isVlan1Exists;
    inFields[11] = rulePtr->eth.vid1;
    inFields[12] = rulePtr->eth.up1;
    inFields[13] = rulePtr->eth.cfi1;
    inFields[14] = rulePtr->eth.etherType;
    inFields[15] = rulePtr->eth.macToMe;
    inFields[16] = rulePtr->eth.common.dsaSrcIsTrunk;
    inFields[17] = rulePtr->eth.common.dsaSrcPortTrunk;
    inFields[18] = rulePtr->eth.common.dsaSrcDevice;
    inFields[19] = rulePtr->eth.srcId;
    inFields[20] = rulePtr->eth.dsaQosProfile;
    inFields[21] = rulePtr->eth.common.sourcePortGroupId;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->eth.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11],inFields[12],inFields[13], inFields[14],
                inFields[15],inFields[16],inFields[17],inFields[18],
                inFields[19],inFields[20], inFields[21]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_MIM_Get
*
* DESCRIPTION:
*       This function maps MIM TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_MIM_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_U8 port;

    ruleFormat = CPSS_DXCH_TTI_KEY_MIM_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2] = rulePtr->mim.common.pclId;
    inFields[3] = rulePtr->mim.common.srcIsTrunk;
    inFields[4] = rulePtr->mim.common.srcPortTrunk;

    if (rulePtr->mim.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_U8)rulePtr->mim.common.srcPortTrunk;

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum,port);

        inFields[4] = port;
    }

    inFields[6] = rulePtr->mim.common.vid;
    inFields[7] = rulePtr->mim.common.isTagged;
    inFields[8] = rulePtr->mim.common.dsaSrcIsTrunk;
    inFields[9] = rulePtr->mim.common.dsaSrcPortTrunk;
    inFields[10] = rulePtr->mim.common.dsaSrcDevice;
    inFields[11] = rulePtr->mim.bUp;
    inFields[12] = rulePtr->mim.bDp;
    inFields[13] = rulePtr->mim.iSid;
    inFields[14] = rulePtr->mim.iUp;
    inFields[15] = rulePtr->mim.iDp;
    inFields[16] = rulePtr->mim.iRes1;
    inFields[17] = rulePtr->mim.iRes2;
    inFields[18] = rulePtr->mim.common.sourcePortGroupId;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->eth.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11],inFields[12],inFields[13], inFields[14],
                inFields[15],inFields[16],inFields[17],inFields[18]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2Get
*
* DESCRIPTION:
*       This function gets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    GT_BOOL                     ruleValid;

    /* if isMask == GT_TRUE it is mask's turn (the first one to get), so there need to refresh the data
    else the data is updated and it is pattern's turn (was updated earlier)*/
    if (isMask)
    {
        /* check if requested refresh range is already refreshed */
        if ((ruleIndex >= 3072) || (ruleIndex > ruleIndexMaxGet))
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];
        keyType = inArgs[1];

        /* look for another valid rule within the range */
        for ((ruleValid = GT_FALSE) ; ((ruleValid == GT_FALSE) && (ruleIndex < 3072) && (ruleIndex <= ruleIndexMaxGet)) ; ruleIndex++)
        {
            /* check if the rule is valid */
            result = pg_wrap_cpssDxChTtiRuleValidStatusGet(devNum,ruleIndex,&ruleValid);
            if (result != GT_OK)
            {
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }
        }

        /* check if no valid rule was found in the requested range */
        if ((ruleValid == GT_FALSE))
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        /* valid rule was found in the requested range */
        ruleIndex--;

        /* call cpss api function, note that it is assumed that rule action
           type is not modified after setting the rule */
        result = pg_wrap_cpssDxChTtiRuleGet(devNum,ruleIndex,ttiRuleFormat[ruleIndex],&patternData,&maskData,ttiRuleActionType[ruleIndex],&action);
    }

    switch (ttiRuleFormat[ruleIndex])
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_2_KEY_MPLS_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_2_KEY_MIM_Get(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (isMask)
        ruleIndex++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2GetFirst
*
* DESCRIPTION:
*       This function gets first TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2GetFirst
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

    /* check if the application request 0 entries */
    if (inArgs[2] == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    ruleIndex       = (GT_U32)inArgs[1];
    ruleIndexMaxGet = ruleIndex + (GT_U32)inArgs[2] - 1;

    return wrCpssDxChTtiRule_2Get(inArgs,inFields,numFields,outArgs);
}

/***********************************/
/* cpssDxChTtiRuleAction_2 Table */
/***********************************/

/*******************************************************************************
* wrCpssDxChTtiRuleAction_2Update
*
* DESCRIPTION:
*       This function updates rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_2Update
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result = GT_OK;
    GT_U8                               devNum;
    GT_U32                              ruleIndex;
    GT_U32                              index;
    CPSS_DXCH_TTI_ACTION_UNT            *actionPtr;
    GT_BOOL                             isNewIndex = GT_FALSE;
    ttiActionDB                         ttiTempAction = {0};

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)inFields[0];
    ruleIndex = (GT_U32)inFields[1];

    if (ruleIndex > PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerAndTunnelTermTcam)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Rule index is not in the range .\n");

        return CMD_AGENT_ERROR;
    }

    /* in case it is first usage of TTI action database, initialize it */
    if (firstRun)
    {
        ttiActionDatabaseInit();
        firstRun = GT_FALSE;
    }

    /* look for the action in the TTI action database */
    ttiActionDatabaseGet(devNum, ruleIndex, &index);

    /* the action wasn't in the TTI action database */
    if (index == INVALID_RULE_INDEX)
    {
        if (firstFree == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
            return CMD_AGENT_ERROR;
        }

        index = firstFree;
        firstFree = ttiActionDatabase[firstFree].nextFree;
        isNewIndex = GT_TRUE;
    }

    /* this is update of an existing action:
       save the old action parameters */
    if (isNewIndex == GT_FALSE)
    {
        cpssOsMemCpy(&ttiTempAction,&ttiActionDatabase[index],sizeof(ttiTempAction));
    }

    /* set action entry in TTI action database as valid */
    ttiActionDatabase[index].valid      = GT_TRUE;
    ttiActionDatabase[index].ruleIndex  = ruleIndex;
    ttiActionDatabase[index].devNum     = devNum;
    ttiActionDatabase[index].actionType = (CPSS_DXCH_TTI_ACTION_TYPE_ENT)inArgs[0];

    actionPtr = &(ttiActionDatabase[index].actionEntry);

    /* fill action fields */
    switch (ttiActionDatabase[index].actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:

        actionPtr->type1.tunnelTerminate                  = (GT_BOOL)inFields[2];
        actionPtr->type1.passengerPacketType              = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inFields[3];
        actionPtr->type1.copyTtlFromTunnelHeader          = (GT_BOOL)inFields[4];
        actionPtr->type1.command                          = (CPSS_PACKET_CMD_ENT)inFields[5];
        actionPtr->type1.redirectCommand                  = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inFields[6];
        actionPtr->type1.egressInterface.type             = (CPSS_INTERFACE_TYPE_ENT)inFields[7];
        actionPtr->type1.egressInterface.devPort.devNum   = (GT_U8)inFields[8];
        actionPtr->type1.egressInterface.devPort.portNum  = (GT_U8)inFields[9];
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(actionPtr->type1.egressInterface.devPort.devNum, actionPtr->type1.egressInterface.devPort.portNum);
        actionPtr->type1.egressInterface.trunkId          = (GT_TRUNK_ID)inFields[10];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type1.egressInterface.trunkId);
        actionPtr->type1.egressInterface.vidx             = (GT_U16)inFields[11];
        actionPtr->type1.egressInterface.vlanId           = (GT_U16)inFields[12];
        actionPtr->type1.tunnelStart                      = (GT_BOOL)inFields[13];
        actionPtr->type1.tunnelStartPtr                   = (GT_U32)inFields[14];
        actionPtr->type1.routerLookupPtr                  = (GT_U32)inFields[15];
        actionPtr->type1.vrfId                            = (GT_U32)inFields[16];
        /* the fields targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
           from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be ignored */
        actionPtr->type1.useVidx                          = (GT_BOOL)inFields[20];
        actionPtr->type1.sourceIdSetEnable                = (GT_BOOL)inFields[21];
        actionPtr->type1.sourceId                         = (GT_U32)inFields[22];
        actionPtr->type1.vlanCmd                          = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[23];
        actionPtr->type1.vlanId                           = (GT_U16)inFields[24];
        actionPtr->type1.vlanPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[25];
        actionPtr->type1.nestedVlanEnable                 = (GT_BOOL)inFields[26];
        actionPtr->type1.bindToPolicer                    = (GT_BOOL)inFields[27];
        actionPtr->type1.policerIndex                     = (GT_U32)inFields[28];
        actionPtr->type1.qosPrecedence                    = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[29];
        actionPtr->type1.qosTrustMode                     = (CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT)inFields[30];
        actionPtr->type1.qosProfile                       = (GT_U32)inFields[31];
        actionPtr->type1.modifyUpEnable                   = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inFields[32];
        actionPtr->type1.modifyDscpEnable                 = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inFields[33];
        actionPtr->type1.up                               = (GT_U32)inFields[34];
        actionPtr->type1.remapDSCP                        = (GT_BOOL)inFields[35];
        actionPtr->type1.mirrorToIngressAnalyzerEnable    = (GT_BOOL)inFields[36];
        actionPtr->type1.userDefinedCpuCode               = (CPSS_NET_RX_CPU_CODE_ENT)inFields[37];
        actionPtr->type1.vntl2Echo                        = (GT_BOOL)inFields[38];
        actionPtr->type1.bridgeBypass                     = (GT_BOOL)inFields[39];
        actionPtr->type1.actionStop                       = (GT_BOOL)inFields[40];
        actionPtr->type1.activateCounter                  = (GT_BOOL)inFields[41];
        actionPtr->type1.counterIndex                     = (GT_U32)inFields[42];
        break;

    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:

        actionPtr->type2.tunnelTerminate                 = (GT_BOOL)inFields[2];
        actionPtr->type2.ttPassengerPacketType           = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inFields[3];
        actionPtr->type2.tsPassengerPacketType           = (CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT)inFields[4];
        actionPtr->type2.copyTtlFromTunnelHeader         = (GT_BOOL)inFields[5];
        actionPtr->type2.mplsCommand                     = (CPSS_DXCH_TTI_MPLS_CMD_ENT)inFields[6];
        actionPtr->type2.mplsTtl                         = (GT_U32)inFields[7];
        actionPtr->type2.enableDecrementTtl              = (GT_BOOL)inFields[8];
        actionPtr->type2.command                         = (CPSS_PACKET_CMD_ENT)inFields[9];
        actionPtr->type2.redirectCommand                 = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inFields[10];
        actionPtr->type2.egressInterface.type            = (CPSS_INTERFACE_TYPE_ENT)inFields[11];
        actionPtr->type2.egressInterface.devPort.devNum  = (GT_U8)inFields[12];
        actionPtr->type2.egressInterface.devPort.portNum = (GT_U8)inFields[13];
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(actionPtr->type2.egressInterface.devPort.devNum, actionPtr->type2.egressInterface.devPort.portNum);
        actionPtr->type2.egressInterface.trunkId         = (GT_TRUNK_ID)inFields[14];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type2.egressInterface.trunkId);
        actionPtr->type2.egressInterface.vidx            = (GT_U16)inFields[15];
        actionPtr->type2.egressInterface.vlanId          = (GT_U16)inFields[16];
        actionPtr->type2.arpPtr                          = (GT_U32)inFields[17];
        actionPtr->type2.tunnelStart                     = (GT_BOOL)inFields[18];
        actionPtr->type2.tunnelStartPtr                  = (GT_U32)inFields[19];
        actionPtr->type2.routerLttPtr                    = (GT_U32)inFields[20];
        actionPtr->type2.vrfId                           = (GT_U32)inFields[21];
        actionPtr->type2.sourceIdSetEnable               = (GT_BOOL)inFields[22];
        actionPtr->type2.sourceId                        = (GT_U32)inFields[23];
        actionPtr->type2.tag0VlanCmd                     = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[24];
        actionPtr->type2.tag0VlanId                      = (GT_U16)inFields[25];
        actionPtr->type2.tag1VlanCmd                     = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[26];
        actionPtr->type2.tag1VlanId                      = (GT_U16)inFields[27];
        actionPtr->type2.tag0VlanPrecedence              = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[28];
        actionPtr->type2.nestedVlanEnable                = (GT_BOOL)inFields[29];
        actionPtr->type2.bindToPolicerMeter              = (GT_BOOL)inFields[30];
        actionPtr->type2.bindToPolicer                   = (GT_BOOL)inFields[31];
        actionPtr->type2.policerIndex                    = (GT_U32)inFields[32];
        actionPtr->type2.qosPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[33];
        actionPtr->type2.keepPreviousQoS                 = (GT_BOOL)inFields[34];
        actionPtr->type2.trustUp                         = (GT_BOOL)inFields[35];
        actionPtr->type2.trustDscp                       = (GT_BOOL)inFields[36];
        actionPtr->type2.trustExp                        = (GT_BOOL)inFields[37];
        actionPtr->type2.qosProfile                      = (GT_U32)inFields[38];
        actionPtr->type2.modifyTag0Up                    = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inFields[39];
        actionPtr->type2.tag1UpCommand                   = (CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT)inFields[40];
        actionPtr->type2.modifyDscp                      = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inFields[41];
        actionPtr->type2.tag0Up                          = (GT_U32)inFields[42];
        actionPtr->type2.tag1Up                          = (GT_U32)inFields[43];
        actionPtr->type2.remapDSCP                       = (GT_BOOL)inFields[44];
        actionPtr->type2.pcl0OverrideConfigIndex         = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[45];
        actionPtr->type2.pcl0_1OverrideConfigIndex       = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[46];
        actionPtr->type2.pcl1OverrideConfigIndex         = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[47];
        actionPtr->type2.iPclConfigIndex                 = (GT_U32)inFields[48];
        actionPtr->type2.mirrorToIngressAnalyzerEnable   = (GT_BOOL)inFields[49];
        actionPtr->type2.userDefinedCpuCode              = (CPSS_NET_RX_CPU_CODE_ENT)inFields[50];
        actionPtr->type2.bindToCentralCounter            = (GT_BOOL)inFields[51];
        actionPtr->type2.centralCounterIndex             = (GT_U32)inFields[52];
        actionPtr->type2.vntl2Echo                       = (GT_BOOL)inFields[53];
        actionPtr->type2.bridgeBypass                    = (GT_BOOL)inFields[54];
        actionPtr->type2.ingressPipeBypass               = (GT_BOOL)inFields[55];
        actionPtr->type2.actionStop                      = (GT_BOOL)inFields[56];
        actionPtr->type2.hashMaskIndex                   = (GT_U32)inFields[57];
        actionPtr->type2.modifyMacSa                     = (GT_BOOL)inFields[58];
        actionPtr->type2.modifyMacDa                     = (GT_BOOL)inFields[59];
        actionPtr->type2.ResetSrcPortGroupId             = (GT_BOOL)inFields[60];
        actionPtr->type2.multiPortGroupTtiEnable         = (GT_BOOL)inFields[61];

        break;

    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_AGENT_ERROR;
    }

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiRuleActionUpdate(devNum,ruleIndex,ttiActionDatabase[index].actionType,actionPtr);

    /* if the rule action didn't succeed:  */
    if (result != GT_OK)
    {
        /* if this is a new action remove it from the database */
        if (isNewIndex)
        {
            ttiActionDatabase[index].valid = GT_FALSE;
            ttiActionDatabase[index].nextFree = firstFree;
            firstFree = index;
        }
        /* if this is an update for existing action restore previous data */
        else
        {
            cpssOsMemCpy(&ttiActionDatabase[index],&ttiTempAction,sizeof(ttiTempAction));
        }
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleAction_2Get
*
* DESCRIPTION:
*       This function gets single rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_2Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_ACTION_UNT    *actionPtr;
    GT_U8                       tempDev,tempPort; /* used for port,dev converting */

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* get next valid TTI action entry */
    while ((ttiActionGetIndex < ACTION_TABLE_SIZE) &&
           (ttiActionDatabase[ttiActionGetIndex].valid == GT_FALSE))
        ttiActionGetIndex++;

    if (ttiActionGetIndex == ACTION_TABLE_SIZE)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    actionPtr = &(ttiActionDatabase[ttiActionGetIndex].actionEntry);

    switch (ttiActionDatabase[ttiActionGetIndex].actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:

        inArgs[0] = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

        inFields[0]  = ttiActionDatabase[ttiActionGetIndex].devNum;
        inFields[1]  = ttiActionDatabase[ttiActionGetIndex].ruleIndex;
        inFields[2]  = actionPtr->type1.tunnelTerminate;
        inFields[3]  = actionPtr->type1.passengerPacketType;
        inFields[4]  = actionPtr->type1.copyTtlFromTunnelHeader;
        inFields[5]  = actionPtr->type1.command;
        inFields[6]  = actionPtr->type1.redirectCommand;
        inFields[7]  = actionPtr->type1.egressInterface.type;
        tempDev      = actionPtr->type1.egressInterface.devPort.devNum;
        tempPort     = actionPtr->type1.egressInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempDev,tempPort);
        inFields[8]  = tempDev;
        inFields[9]  = tempPort;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr->type1.egressInterface.trunkId);
        inFields[10] = actionPtr->type1.egressInterface.trunkId;
        inFields[11] = actionPtr->type1.egressInterface.vidx;
        inFields[12] = actionPtr->type1.egressInterface.vlanId;
        inFields[13] = actionPtr->type1.tunnelStart;
        inFields[14] = actionPtr->type1.tunnelStartPtr;
        inFields[15] = actionPtr->type1.routerLookupPtr;
        inFields[16] = actionPtr->type1.vrfId;
        /* the fields targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
           from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be hardcoded  */
        inFields[17] = GT_FALSE;
        inFields[18] = 0;
        inFields[19] = 0;
        inFields[20] = actionPtr->type1.useVidx;
        inFields[21] = actionPtr->type1.sourceIdSetEnable;
        inFields[22] = actionPtr->type1.sourceId;
        inFields[23] = actionPtr->type1.vlanCmd;
        inFields[24] = actionPtr->type1.vlanId;
        inFields[25] = actionPtr->type1.vlanPrecedence;
        inFields[26] = actionPtr->type1.nestedVlanEnable;
        inFields[27] = actionPtr->type1.bindToPolicer;
        inFields[28] = actionPtr->type1.policerIndex;
        inFields[29] = actionPtr->type1.qosPrecedence;
        inFields[30] = actionPtr->type1.qosTrustMode;
        inFields[31] = actionPtr->type1.qosProfile;
        inFields[32] = actionPtr->type1.modifyUpEnable;
        inFields[33] = actionPtr->type1.modifyDscpEnable;
        inFields[34] = actionPtr->type1.up;
        inFields[35] = actionPtr->type1.remapDSCP;
        inFields[36] = actionPtr->type1.mirrorToIngressAnalyzerEnable;
        inFields[37] = actionPtr->type1.userDefinedCpuCode;
        inFields[38] = actionPtr->type1.vntl2Echo;
        inFields[39] = actionPtr->type1.bridgeBypass;
        inFields[40] = actionPtr->type1.actionStop;
        inFields[41] = actionPtr->type1.activateCounter;
        inFields[42] = actionPtr->type1.counterIndex;

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
        break;

    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:

        inArgs[0] = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        inFields[0]  = ttiActionDatabase[ttiActionGetIndex].devNum;
        inFields[1]  = ttiActionDatabase[ttiActionGetIndex].ruleIndex;
        inFields[2]  = actionPtr->type2.tunnelTerminate;
        inFields[3]  = actionPtr->type2.ttPassengerPacketType;
        inFields[4]  = actionPtr->type2.tsPassengerPacketType;
        inFields[5]  = actionPtr->type2.copyTtlFromTunnelHeader;
        inFields[6]  = actionPtr->type2.mplsCommand;
        inFields[7]  = actionPtr->type2.mplsTtl;
        inFields[8]  = actionPtr->type2.enableDecrementTtl;
        inFields[9]  = actionPtr->type2.command;
        inFields[10] = actionPtr->type2.redirectCommand;
        inFields[11] = actionPtr->type2.egressInterface.type;
        tempDev      = actionPtr->type2.egressInterface.devPort.devNum;
        tempPort     = actionPtr->type2.egressInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempDev,tempPort);
        inFields[12] = tempDev;
        inFields[13] = tempPort;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr->type2.egressInterface.trunkId);
        inFields[14] = actionPtr->type2.egressInterface.trunkId;
        inFields[15] = actionPtr->type2.egressInterface.vidx;
        inFields[16] = actionPtr->type2.egressInterface.vlanId;
        inFields[17] = actionPtr->type2.arpPtr;
        inFields[18] = actionPtr->type2.tunnelStart;
        inFields[19] = actionPtr->type2.tunnelStartPtr;
        inFields[20] = actionPtr->type2.routerLttPtr;
        inFields[21] = actionPtr->type2.vrfId;
        inFields[22] = actionPtr->type2.sourceIdSetEnable;
        inFields[23] = actionPtr->type2.sourceId;
        inFields[24] = actionPtr->type2.tag0VlanCmd;
        inFields[25] = actionPtr->type2.tag0VlanId;
        inFields[26] = actionPtr->type2.tag1VlanCmd;
        inFields[27] = actionPtr->type2.tag1VlanId;
        inFields[28] = actionPtr->type2.tag0VlanPrecedence;
        inFields[29] = actionPtr->type2.nestedVlanEnable;
        inFields[30] = actionPtr->type2.bindToPolicerMeter;
        inFields[31] = actionPtr->type2.bindToPolicer;
        inFields[32] = actionPtr->type2.policerIndex;
        inFields[33] = actionPtr->type2.qosPrecedence;
        inFields[34] = actionPtr->type2.keepPreviousQoS;
        inFields[35] = actionPtr->type2.trustUp;
        inFields[36] = actionPtr->type2.trustDscp;
        inFields[37] = actionPtr->type2.trustExp;
        inFields[38] = actionPtr->type2.qosProfile;
        inFields[39] = actionPtr->type2.modifyTag0Up;
        inFields[40] = actionPtr->type2.tag1UpCommand;
        inFields[41] = actionPtr->type2.modifyDscp;
        inFields[42] = actionPtr->type2.tag0Up;
        inFields[43] = actionPtr->type2.tag1Up;
        inFields[44] = actionPtr->type2.remapDSCP;
        inFields[45] = actionPtr->type2.pcl0OverrideConfigIndex;
        inFields[46] = actionPtr->type2.pcl0_1OverrideConfigIndex;
        inFields[47] = actionPtr->type2.pcl1OverrideConfigIndex;
        inFields[48] = actionPtr->type2.iPclConfigIndex;
        inFields[49] = actionPtr->type2.mirrorToIngressAnalyzerEnable;
        inFields[50] = actionPtr->type2.userDefinedCpuCode;
        inFields[51] = actionPtr->type2.bindToCentralCounter;
        inFields[52] = actionPtr->type2.centralCounterIndex;
        inFields[53] = actionPtr->type2.vntl2Echo;
        inFields[54] = actionPtr->type2.bridgeBypass;
        inFields[55] = actionPtr->type2.ingressPipeBypass;
        inFields[56] = actionPtr->type2.actionStop;
        inFields[57] = actionPtr->type2.hashMaskIndex;
        inFields[58] = actionPtr->type2.modifyMacSa;
        inFields[59] = actionPtr->type2.modifyMacDa;
        inFields[60] = actionPtr->type2.ResetSrcPortGroupId;
        inFields[61] = actionPtr->type2.multiPortGroupTtiEnable;

        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
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
        galtisOutput(outArgs, GT_OK, "%d%f", 1);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI ACTION FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    ttiActionGetIndex++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleAction_2GetFirst
*
* DESCRIPTION:
*       This function gets first rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_2GetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ttiActionGetIndex = 0;

    return wrCpssDxChTtiRuleAction_2Get(inArgs,inFields,numFields,outArgs);
}


/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChTtiMacToMeSet",
        &wrCpssDxChTtiMacToMeSet,
        1, 5},
    {"cpssDxChTtiMacToMeGetFirst",
        &wrCpssDxChTtiMacToMeGetFirst,
        1, 0},
    {"cpssDxChTtiMacToMeGetNext",
        &wrCpssDxChTtiMacToMeGetNext,
        1, 0},
    {"cpssDxChTtiPortLookupEnableSet",
        &wrCpssDxChTtiPortLookupEnableSet,
        4, 0},
    {"cpssDxChTtiPortLookupEnableGet",
        &wrCpssDxChTtiPortLookupEnableGet,
        3, 0},
    {"cpssDxChTtiPortIpv4OnlyTunneledEnableSet",
        &wrCpssDxChTtiPortIpv4OnlyTunneledEnableSet,
        3, 0},
    {"cpssDxChTtiPortIpv4OnlyTunneledEnableGet",
        &wrCpssDxChTtiPortIpv4OnlyTunneledEnableGet,
        2, 0},
    {"cpssDxChTtiPortIpv4OnlyMac2MeEnableSet",
        &wrCpssDxChTtiPortIpv4OnlyMac2MeEnableSet,
        3, 0},
    {"cpssDxChTtiPortIpv4OnlyMac2MeEnableGet",
        &wrCpssDxChTtiPortIpv4OnlyMac2MeEnableGet,
        2, 0},
    {"cpssDxChTtiIpv4McEnableSet",
        &wrCpssDxChTtiIpv4McEnableSet,
        2, 0},
    {"cpssDxChTtiIpv4McEnableGet",
        &wrCpssDxChTtiIpv4McEnableGet,
        1, 0},
    {"cpssDxChTtiPortMplsOnlyMac2MeEnableSet",
        &wrCpssDxChTtiPortMplsOnlyMac2MeEnableSet,
        3, 0},
    {"cpssDxChTtiPortMplsOnlyMac2MeEnableGet",
        &wrCpssDxChTtiPortMplsOnlyMac2MeEnableGet,
        2, 0},
    {"cpssDxChTtiPortMimOnlyMacToMeEnableSet",
        &wrCpssDxChTtiPortMimOnlyMacToMeEnableSet,
        3, 0},
    {"cpssDxChTtiPortMimOnlyMacToMeEnableGet",
        &wrCpssDxChTtiPortMimOnlyMacToMeEnableGet,
        2, 0},


    {"cpssDxChTtiRuleSetFirst",
        &wrCpssDxChTtiRuleSetFirst,
        2, 19},
    {"cpssDxChTtiRuleSetNext",
        &wrCpssDxChTtiRuleSetNext,
        2, 19},
    {"cpssDxChTtiRuleEndSet",
        &wrCpssDxChTtiRuleEndSet,
        0, 0},
    {"cpssDxChTtiRuleGetFirst",
        &wrCpssDxChTtiRuleGetFirst,
        1, 0},
    {"cpssDxChTtiRuleGetNext",
        &wrCpssDxChTtiRuleGet,
        1, 0},

    {"cpssDxChTtiRuleActionSet",
        &wrCpssDxChTtiRuleActionUpdate,
        0, 43},
    {"cpssDxChTtiRuleActionGetFirst",
        &wrCpssDxChTtiRuleActionGetFirst,
        0, 0},
    {"cpssDxChTtiRuleActionGetNext",
        &wrCpssDxChTtiRuleActionGet,
        0, 0},
    {"cpssDxChTtiRuleActionClear",
        &wrCpssDxChTtiRuleActionClear,
        0, 0},
    {"cpssDxChTtiRuleActionDelete",
        &wrCpssDxChTtiRuleActionDelete,
        0, 2},

    {"cpssDxChTtiRule_1SetFirst",
        &wrCpssDxChTtiRule_1SetFirst,
        4, 19},
    {"cpssDxChTtiRule_1SetNext",
        &wrCpssDxChTtiRule_1SetNext,
        4, 19},
    {"cpssDxChTtiRule_1EndSet",
        &wrCpssDxChTtiRule_1EndSet,
        0, 0},
    {"cpssDxChTtiRule_1GetFirst",
        &wrCpssDxChTtiRule_1GetFirst,
        4, 0},
    {"cpssDxChTtiRule_1GetNext",
        &wrCpssDxChTtiRule_1Get,
        4, 0},

    {"cpssDxChTtiRuleAction_1Set",
        &wrCpssDxChTtiRuleAction_1Update,
        1, 60},
    {"cpssDxChTtiRuleAction_1GetFirst",
        &wrCpssDxChTtiRuleAction_1GetFirst,
        1, 0},
    {"cpssDxChTtiRuleAction_1GetNext",
        &wrCpssDxChTtiRuleAction_1Get,
        1, 0},
    {"cpssDxChTtiRuleAction_1Clear",
        &wrCpssDxChTtiRuleActionClear,
        1, 0},
    {"cpssDxChTtiRuleAction_1Delete",
        &wrCpssDxChTtiRuleActionDelete,
        1, 2},

    {"cpssDxChTtiMacModeSet",
        &wrCpssDxChTtiMacModeSet,
        3, 0},
    {"cpssDxChTtiMacModeGet",
        &wrCpssDxChTtiMacModeGet,
        2, 0},
    {"cpssDxChTtiPclIdSet",
        &wrCpssDxChTtiPclIdSet,
        3, 0},
    {"cpssDxChTtiPclIdGet",
        &wrCpssDxChTtiPclIdGet,
        2, 0},
    {"cpssDxChTtiIpv4GreEthTypeSet",
        &wrCpssDxChTtiIpv4GreEthTypeSet,
        3, 0},
    {"cpssDxChTtiIpv4GreEthTypeGet",
        &wrCpssDxChTtiIpv4GreEthTypeGet,
        2, 0},
    {"cpssDxChTtiMimEthTypeSet",
        &wrCpssDxChTtiMimEthTypeSet,
        2, 0},
    {"cpssDxChTtiMimEthTypeGet",
        &wrCpssDxChTtiMimEthTypeGet,
        1, 0},
    {"cpssDxChTtiMplsEthTypeSet",
        &wrCpssDxChTtiMplsEthTypeSet,
        3, 0},
    {"cpssDxChTtiMplsEthTypeGet",
        &wrCpssDxChTtiMplsEthTypeGet,
        2, 0},
    {"cpssDxChTtiExceptionCmdSet",
        &wrCpssDxChTtiExceptionCmdSet,
        3, 0},
    {"cpssDxChTtiExceptionCmdGet",
        &wrCpssDxChTtiExceptionCmdGet,
        2, 0},

    {"cpssDxChTtiRuleValidStatusSet",
        &wrCpssDxChTtiRuleValidStatusSet,
        3, 0},
    {"cpssDxChTtiRuleValidStatusGet",
        &wrCpssDxChTtiRuleValidStatusGet,
        2, 0},

    {"cpssDxChTtiRule_2SetFirst",
        &wrCpssDxChTtiRule_2SetFirst,
        4, 20},
    {"cpssDxChTtiRule_2SetNext",
        &wrCpssDxChTtiRule_2SetNext,
        4, 20},
    {"cpssDxChTtiRule_2EndSet",
        &wrCpssDxChTtiRule_2EndSet,
        0, 0},
    {"cpssDxChTtiRule_2GetFirst",
        &wrCpssDxChTtiRule_2GetFirst,
        4, 0},
    {"cpssDxChTtiRule_2GetNext",
        &wrCpssDxChTtiRule_2Get,
        4, 0},

    {"cpssDxChTtiRuleAction_2Set",
        &wrCpssDxChTtiRuleAction_2Update,
        1, 65},
    {"cpssDxChTtiRuleAction_2GetFirst",
        &wrCpssDxChTtiRuleAction_2GetFirst,
        1, 0},
    {"cpssDxChTtiRuleAction_2GetNext",
        &wrCpssDxChTtiRuleAction_2Get,
        1, 0},
    {"cpssDxChTtiRuleAction_2Clear",
        &wrCpssDxChTtiRuleActionClear,
        1, 0},
    {"cpssDxChTtiRuleAction_2Delete",
        &wrCpssDxChTtiRuleActionDelete,
        1, 2},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChTti
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
GT_STATUS cmdLibInitCpssDxChTti
(
GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

