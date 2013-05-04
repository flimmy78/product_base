/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChPcl.c
*
* DESCRIPTION:
*       Wrapper functions for Pcl cpss.dxCh functions.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

/* Feature specific includes. */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/generic/pcl/cpssPcl.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPcl.h>

/* forward declaration */
GT_STATUS utilCpssDxChPclRuleIncrementalSequenceSet
(
    IN GT_U8                              devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT checkRuleFormat,
    IN GT_U32                             ruleIndexBase,
    IN GT_U32                             ruleIndexIncrement,
    IN GT_U32                             rulesAmount,
    IN char                               *field_full_name,
    IN GT_U32                             elementStart, /* start array element */
    IN GT_U32                             increment
);

/*******************************************************************************
* wrCpssDxChPclGenericDymmyGetNext
*
* DESCRIPTION:
*   The function used to return "No Next Table Entry"
*
* COMMENTS: Generic
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclGenericDymmyGetNext
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

/* support for multi port groups */

/*******************************************************************************
* pclMultiPortGroupsBmpGet
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
static void pclMultiPortGroupsBmpGet
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

/* default mode - backward compatible     */
/* the new command  will set these values */
static GT_U32  ruleSet_ruleOptionsBmp = 0;
static GT_U32  ruleSet_portListBmpMask = 0;
static GT_U32  ruleSet_portListBmpPattern = 0;

/*******************************************************************************
* wrUtil_cpssDxChPclRuleOptionsSet
*
* DESCRIPTION:
*       Set Rule write options.
*
* INPUTS:
*       ruleOptionsBmp      - Bitmap of rule write options
*                             bit0 (i.e. 1) - write rule invalid.
*                             bit1 (i.e. 2) - write rule using port list.
*       portListBmpMask     - port List Bitmap Mask
*                             relevant only if port list option specified.
*       portListBmpPattern  - port List Bitmap Pattern
*                             relevant only if port list option specified.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrUtil_cpssDxChPclRuleOptionsSet
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

    ruleSet_ruleOptionsBmp     = (GT_U32)inArgs[0];
    ruleSet_portListBmpMask    = (GT_U32)inArgs[1];
    ruleSet_portListBmpPattern = (GT_U32)inArgs[2];

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclRuleSet
(
    IN GT_U8                              devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    IN CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclRuleSet(
            devNum, ruleFormat, ruleIndex,
            ruleSet_ruleOptionsBmp,
            maskPtr, patternPtr, actionPtr);
    }
    else
    {
        return cpssDxChPclPortGroupRuleSet(
            devNum, pgBmp, ruleFormat, ruleIndex,
            ruleSet_ruleOptionsBmp,
            maskPtr, patternPtr, actionPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclRuleActionUpdate(
            devNum, ruleSize, ruleIndex, actionPtr);
    }
    else
    {
        return cpssDxChPclPortGroupRuleActionUpdate(
            devNum, pgBmp, ruleSize, ruleIndex, actionPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN  GT_U32                             ruleIndex,
    IN  CPSS_PCL_DIRECTION_ENT             direction,
    OUT CPSS_DXCH_PCL_ACTION_STC           *actionPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclRuleActionGet(
            devNum, ruleSize, ruleIndex, direction, actionPtr);
    }
    else
    {
        return cpssDxChPclPortGroupRuleActionGet(
            devNum, pgBmp, ruleSize, ruleIndex, direction, actionPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclRuleInvalidate
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclRuleInvalidate(
            devNum, ruleSize, ruleIndex);
    }
    else
    {
        return cpssDxChPclPortGroupRuleInvalidate(
            devNum, pgBmp, ruleSize, ruleIndex);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclRuleValidStatusSet
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleIndex,
    IN GT_BOOL                            valid
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclRuleValidStatusSet(
            devNum, ruleSize, ruleIndex, valid);
    }
    else
    {
        return cpssDxChPclPortGroupRuleValidStatusSet(
            devNum, pgBmp, ruleSize, ruleIndex, valid);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclRuleStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT CPSS_PCL_RULE_SIZE_ENT        *ruleSizePtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclRuleStateGet(
            devNum, ruleIndex, validPtr, ruleSizePtr);
    }
    else
    {
        return cpssDxChPclPortGroupRuleAnyStateGet(
            devNum, pgBmp, CPSS_PCL_RULE_SIZE_STD_E,
            ruleIndex, validPtr, ruleSizePtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclRuleCopy
(
    IN GT_U8                              devNum,
    IN CPSS_PCL_RULE_SIZE_ENT             ruleSize,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclRuleCopy(
            devNum, ruleSize, ruleSrcIndex, ruleDstIndex);
    }
    else
    {
        return cpssDxChPclPortGroupRuleCopy(
            devNum, pgBmp, ruleSize, ruleSrcIndex, ruleDstIndex);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclRuleGet
(
    IN  GT_U8                  devNum,
    IN  CPSS_PCL_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                 ruleIndex,
    OUT GT_U32                 maskArr[],
    OUT GT_U32                 patternArr[],
    OUT GT_U32                 actionArr[]
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclRuleGet(
            devNum, ruleSize, ruleIndex,
            maskArr, patternArr, actionArr);
    }
    else
    {
        return cpssDxChPclPortGroupRuleGet(
            devNum, pgBmp, ruleSize, ruleIndex,
            maskArr, patternArr, actionArr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclCfgTblSet
(
    IN GT_U8                           devNum,
    IN CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN CPSS_PCL_DIRECTION_ENT          direction,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclCfgTblSet(
            devNum, interfaceInfoPtr,
            direction, lookupNum, lookupCfgPtr);
    }
    else
    {
        return cpssDxChPclPortGroupCfgTblSet(
            devNum, pgBmp, interfaceInfoPtr,
            direction, lookupNum, lookupCfgPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclCfgTblGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_INTERFACE_INFO_STC        *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    OUT CPSS_DXCH_PCL_LOOKUP_CFG_STC    *lookupCfgPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclCfgTblGet(
            devNum, interfaceInfoPtr,
            direction, lookupNum, lookupCfgPtr);
    }
    else
    {
        return cpssDxChPclPortGroupCfgTblGet(
            devNum, pgBmp, interfaceInfoPtr,
            direction, lookupNum, lookupCfgPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclCfgTblEntryGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum,
    IN  GT_U32                          entryIndex,
    OUT GT_U32                          *pclCfgTblEntryPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclCfgTblEntryGet(
            devNum, direction, lookupNum,
            entryIndex, pclCfgTblEntryPtr);
    }
    else
    {
        return cpssDxChPclPortGroupCfgTblEntryGet(
            devNum, pgBmp, direction, lookupNum,
            entryIndex, pclCfgTblEntryPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclTcpUdpPortComparatorSet
(
    IN GT_U8                             devNum,
    IN CPSS_PCL_DIRECTION_ENT            direction,
    IN CPSS_L4_PROTOCOL_ENT              l4Protocol,
    IN GT_U8                             entryIndex,
    IN CPSS_L4_PROTOCOL_PORT_TYPE_ENT    l4PortType,
    IN CPSS_COMPARE_OPERATOR_ENT         compareOperator,
    IN GT_U16                            value
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxCh2PclTcpUdpPortComparatorSet(
            devNum, direction, l4Protocol,
            entryIndex, l4PortType, compareOperator, value);
    }
    else
    {
        return cpssDxChPclPortGroupTcpUdpPortComparatorSet(
            devNum, pgBmp, direction, l4Protocol,
            entryIndex, l4PortType, compareOperator, value);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclLookupCfgPortListEnableSet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    IN  GT_BOOL                       enable
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclLookupCfgPortListEnableSet(
            devNum, direction, lookupNum,
            subLookupNum, enable);
    }
    else
    {
        return cpssDxChPclPortGroupLookupCfgPortListEnableSet(
            devNum, pgBmp, direction, lookupNum,
            subLookupNum, enable);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclLookupCfgPortListEnableGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum,
    OUT GT_BOOL                       *enablePtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclLookupCfgPortListEnableGet(
            devNum, direction, lookupNum,
            subLookupNum, enablePtr);
    }
    else
    {
        return cpssDxChPclPortGroupLookupCfgPortListEnableGet(
            devNum, pgBmp, direction, lookupNum,
            subLookupNum, enablePtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclUserDefinedByteSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN GT_U32                               udbIndex,
    IN CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType,
    IN GT_U8                                offset
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclUserDefinedByteSet(
            devNum, ruleFormat, packetType,
            udbIndex, offsetType, offset);
    }
    else
    {
        return cpssDxChPclPortGroupUserDefinedByteSet(
            devNum, pgBmp, ruleFormat, packetType,
            udbIndex, offsetType, offset);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclUserDefinedByteGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat,
    IN  CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType,
    IN  GT_U32                               udbIndex,
    OUT CPSS_DXCH_PCL_OFFSET_TYPE_ENT        *offsetTypePtr,
    OUT GT_U8                                *offsetPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclUserDefinedByteGet(
            devNum, ruleFormat, packetType,
            udbIndex, offsetTypePtr, offsetPtr);
    }
    else
    {
        return cpssDxChPclPortGroupUserDefinedByteGet(
            devNum, pgBmp, ruleFormat, packetType,
            udbIndex, offsetTypePtr, offsetPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclOverrideUserDefinedBytesSet
(
    IN  GT_U8                          devNum,
    IN  CPSS_DXCH_PCL_OVERRIDE_UDB_STC *udbOverridePtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclOverrideUserDefinedBytesSet(
            devNum, udbOverridePtr);
    }
    else
    {
        return cpssDxChPclPortGroupOverrideUserDefinedBytesSet(
            devNum, pgBmp, udbOverridePtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclOverrideUserDefinedBytesGet
(
    IN  GT_U8                          devNum,
    OUT CPSS_DXCH_PCL_OVERRIDE_UDB_STC *udbOverridePtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclOverrideUserDefinedBytesGet(
            devNum, udbOverridePtr);
    }
    else
    {
        return cpssDxChPclPortGroupOverrideUserDefinedBytesGet(
            devNum, pgBmp, udbOverridePtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet
(
    IN  GT_U8                                     devNum,
    IN  CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet(
            devNum, udbOverTrunkHashPtr);
    }
    else
    {
        return cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashSet(
            devNum, pgBmp, udbOverTrunkHashPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet
(
    IN  GT_U8                                     devNum,
    OUT CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC *udbOverTrunkHashPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet(
            devNum, udbOverTrunkHashPtr);
    }
    else
    {
        return cpssDxChPclPortGroupOverrideUserDefinedBytesByTrunkHashGet(
            devNum, pgBmp, udbOverTrunkHashPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclEgressKeyFieldsVidUpModeSet
(
    IN  GT_U8                                      devNum,
    IN  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclEgressKeyFieldsVidUpModeSet(
            devNum, vidUpMode);
    }
    else
    {
        return cpssDxChPclPortGroupEgressKeyFieldsVidUpModeSet(
            devNum, pgBmp, vidUpMode);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChPclEgressKeyFieldsVidUpModeGet
(
    IN   GT_U8                                      devNum,
    OUT  CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   *vidUpModePtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    pclMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChPclEgressKeyFieldsVidUpModeGet(
            devNum, vidUpModePtr);
    }
    else
    {
        return cpssDxChPclPortGroupEgressKeyFieldsVidUpModeGet(
            devNum, pgBmp, vidUpModePtr);
    }
}

/*******************************************************************************
* cpssDxChPclInit
*
* DESCRIPTION:
*   The function initializes the device for following configuration
*   and using Policy engine
*
*
* INPUTS:
*       devNum             - device number
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclInit

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
    result = cpssDxChPclInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* prvCpssDxChPclUDBIndexBaseGet
*
* DESCRIPTION:
*   The function gets UDB index base by the key format
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       ruleFormat   - rule format
*
* OUTPUTS:
*       indexBasePtr - (pointer to) UDB index base
*
* RETURNS:
*       GT_OK        -     on success
*       GT_BAD_PARAM -     otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS prvCpssDxChPclUDBIndexBaseGet
(
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT    ruleFormat,
    OUT GT_U32                                *indexBasePtr
)
{
    switch (ruleFormat)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            *indexBasePtr = 15;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            *indexBasePtr = 18;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            *indexBasePtr = 20;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
            *indexBasePtr = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            *indexBasePtr = 0;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            *indexBasePtr = 6;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            *indexBasePtr = 12;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
            *indexBasePtr = 0;
            break;
        default: return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* pg_wrap_cpssDxChPclUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*       pktType      - packet type
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*       offsetType   - the type of offset (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_UDB_CFG_STC
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclUserDefinedByteSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                             result;
    GT_U32                                i;
    GT_U8                                 devNum;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT    ruleFormat;
    GT_U32                                udbIndex;
    CPSS_DXCH_PCL_OFFSET_TYPE_ENT         offsetType;
    GT_U8                                 offset;
    GT_U32                                udbIndexBase;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleFormat = (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inArgs[1];
    udbIndex = (GT_U32)inArgs[2];
    offsetType = (CPSS_DXCH_PCL_OFFSET_TYPE_ENT)inArgs[3];
    offset = (GT_U8)inArgs[4];

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* Ch1-3                  */
        /* call cpss api function */
        result = pg_wrap_cpssDxChPclUserDefinedByteSet(
            devNum, ruleFormat,
            /* for DxCh1-3 parameter ignored     */
            /* for DxChXCat - another command    */
            CPSS_DXCH_PCL_PACKET_TYPE_ETHERNET_OTHER_E,
            udbIndex, offsetType, offset);
    }
    else
    {
        /* XCatA1 and above */

        result = prvCpssDxChPclUDBIndexBaseGet(
            ruleFormat, &udbIndexBase);
        if (result != GT_OK)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, result, "");
            return CMD_OK;
        }

        /* configure UDB in all 8 templates */
        for (i = 8; (i > 0); i--)
        {
            result = pg_wrap_cpssDxChPclUserDefinedByteSet(
                devNum, ruleFormat,
                (CPSS_DXCH_PCL_PACKET_TYPE_ENT)(i - 1),
                (udbIndexBase + udbIndex), offsetType, offset);
        }
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* pg_wrap_cpssDxChPclUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte configuration
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*       pktType      - packet type
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetType   - the type of offset (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_UDB_CFG_STC
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclUserDefinedByteGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                             result;
    GT_U8                                 devNum;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT    ruleFormat;
    CPSS_DXCH_PCL_PACKET_TYPE_ENT         pktType;
    GT_U32                                udbIndex;
    CPSS_DXCH_PCL_OFFSET_TYPE_ENT         offsetType;
    GT_U8                                 offset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    ruleFormat = (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inArgs[1];
    pktType    = (CPSS_DXCH_PCL_PACKET_TYPE_ENT)inArgs[2];
    udbIndex   = (GT_U32)inArgs[3];
    offsetType = 0;
    offset     = 0;

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclUserDefinedByteGet(
        devNum, ruleFormat, pktType,
        udbIndex, &offsetType, &offset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", offsetType, offset);
    return CMD_OK;
}

/* Table: cpssDxChPclRule (Union Table)
*
* Description:
*     PCL Configuration table
*
* Fields:
*       maskData          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*                          The format of mask is defined by ruleFormat
*
*       patternData       - rule pattern.
*                          The format of pattern is defined by ruleFormat
*
*       actionPtr        - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
*
*       See include file cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h for more details.
*
*
* Comments:
*
*
*
*/


/* table cpssDxChPclRule global variables */

static    CPSS_DXCH_PCL_RULE_FORMAT_UNT      maskData;
static    CPSS_DXCH_PCL_RULE_FORMAT_UNT      patternData;
static    CPSS_DXCH_PCL_ACTION_STC           actionData;
static    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
/* last action stored by pg_wrap_cpssDxChPclRuleSet */
static    CPSS_DXCH_PCL_ACTION_STC           *lastActionPtr;

/* only words 10..20 should be used */
/* use GT_32 to be with same type as --> GT_32 inFields[CMD_MAX_FIELDS] */
static GT_32   ruleDataWords[50];

/*******************************************************************************
* actionRetrieve
*
* DESCRIPTION:
*   Retrueves the RULE Action data from the table fields
*
*
* INPUTS:
*       inputFields          - source input fields array
*       baseFieldIndex       - index of first field related to the action
*
* OUTPUTS:
*       actionPtr            - he retrieved CPSS action
*
* RETURNS:
*       None
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static void actionRetrieve

(
    IN  GT_32                       inputFields[],
    IN  GT_32                       baseFieldIndex,
    OUT CPSS_DXCH_PCL_ACTION_STC    *actionPtr
)
{
    GT_32 *inFields;

    inFields = &(inputFields[baseFieldIndex]);

    /* retrieve command from Galtis Drop Down List */
    switch (inFields[0])
    {
        case 0:
            actionPtr->pktCmd = CPSS_PACKET_CMD_FORWARD_E;
            break;
        case 1:
            actionPtr->pktCmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
            break;
        case 2:
            actionPtr->pktCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;
        case 3:
            actionPtr->pktCmd = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        case 4:
            actionPtr->pktCmd = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        default:
            actionPtr->pktCmd = (CPSS_PACKET_CMD_ENT)0xFF;
            cmdOsPrintf(" *** WRONG PACKET COMMAND ***\n");
            break;
    }

    actionPtr->mirror.cpuCode = (GT_U32)inFields[1];
    actionPtr->mirror.mirrorToRxAnalyzerPort = (GT_BOOL)inFields[2];
    actionPtr->matchCounter.enableMatchCount = (GT_BOOL)inFields[3];
    actionPtr->matchCounter.matchCounterIndex = (GT_U32)inFields[4];
    actionPtr->egressPolicy = (GT_BOOL)inFields[5];
    actionPtr->qos.modifyDscp =
                            (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[6];
    actionPtr->qos.modifyUp =
                            (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[7];

    switch (inFields[5])
    {
    case 0:
        actionPtr->qos.qos.ingress.profileIndex = (GT_U32)inFields[8];
        actionPtr->qos.qos.ingress.profileAssignIndex = (GT_BOOL)inFields[9];
        actionPtr->qos.qos.ingress.profilePrecedence =
                      (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[10];
        break;

    case 1:
        actionPtr->qos.qos.egress.dscp = (GT_U8)inFields[11];
        actionPtr->qos.qos.egress.up = (GT_U8)inFields[12];
        break;

    default:
        break;
    }

    actionPtr->redirect.redirectCmd =
                            (CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT)inFields[13];

    if (inFields[13] == 1)
    {
        actionPtr->redirect.data.outIf.outInterface.type =
                                          (CPSS_INTERFACE_TYPE_ENT)inFields[14];

        switch (inFields[14])
        {
        case 0:
            actionPtr->redirect.data.outIf.outInterface.devPort.devNum =
                                                            (GT_U8)inFields[15];
            actionPtr->redirect.data.outIf.outInterface.devPort.portNum =
                                                            (GT_U8)inFields[16];

            CONVERT_DEV_PORT_DATA_MAC(actionPtr->redirect.data.outIf.outInterface.devPort.devNum,
                                 actionPtr->redirect.data.outIf.outInterface.devPort.portNum);

            break;

        case 1:
            actionPtr->redirect.data.outIf.outInterface.trunkId =
                                                      (GT_TRUNK_ID)inFields[17];
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->redirect.data.outIf.outInterface.trunkId);
            break;

        case 2:
            actionPtr->redirect.data.outIf.outInterface.vidx =
                                                           (GT_U16)inFields[18];
            break;

        case 3:
            actionPtr->redirect.data.outIf.outInterface.vlanId =
                                                           (GT_U16)inFields[19];
            break;

        default:
            break;
        }

        actionPtr->redirect.data.outIf.vntL2Echo = (GT_BOOL)inFields[20];
        actionPtr->redirect.data.outIf.tunnelStart = (GT_BOOL)inFields[21];
        actionPtr->redirect.data.outIf.tunnelPtr = (GT_U32)inFields[22];
    }

    if (inFields[13] == 2)
    {
        actionPtr->redirect.data.routerLttIndex = (GT_U32)inFields[23];
    }

    actionPtr->policer.policerEnable = (GT_BOOL)inFields[24];
    actionPtr->policer.policerId = (GT_U32)inFields[25];

    /*actionPtr->vlan.egressTaggedModify = (GT_BOOL)inFields[26];*/
    actionPtr->vlan.modifyVlan =
                             (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[27];
    actionPtr->vlan.nestedVlan = (GT_BOOL)inFields[28];
    actionPtr->vlan.vlanId = (GT_U16)inFields[29];
    actionPtr->vlan.precedence =
                      (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[30];

    actionPtr->ipUcRoute.doIpUcRoute = (GT_BOOL)inFields[31];
    actionPtr->ipUcRoute.arpDaIndex = (GT_U32)inFields[32];
    actionPtr->ipUcRoute.decrementTTL = (GT_BOOL)inFields[33];
    actionPtr->ipUcRoute.bypassTTLCheck = (GT_BOOL)inFields[34];
    actionPtr->ipUcRoute.icmpRedirectCheck = (GT_BOOL)inFields[35];

}

/*******************************************************************************
* pg_wrap_cpssDxChPclRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule.
*
*       ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case
*                          With reference to Standard and Extended rules
*                          indexes, the partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1023.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 511.
*                          Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n <= 511
*                            is placed in the space of two standard rules with
*                            indexes n and n + 512.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleSet_STD_NOT_IP
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U32                             ruleIndex;
    GT_BYTE_ARRY                       maskBArr, patternBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    ruleIndex = (GT_U32)inFields[0];
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;

    maskData.ruleStdNotIp.common.portListBmp = ruleSet_portListBmpMask;

    maskData.ruleStdNotIp.common.pclId = (GT_U16)inFields[2];
    CONVERT_PCL_MASK_PCL_ID_TEST_TO_CPSS_MAC(maskData.ruleStdNotIp.common.pclId);
    maskData.ruleStdNotIp.common.sourcePort = (GT_U8)inFields[3];
    maskData.ruleStdNotIp.common.isTagged = (GT_U8)inFields[4];
    maskData.ruleStdNotIp.common.vid = (GT_U16)inFields[5];
    maskData.ruleStdNotIp.common.up = (GT_U8)inFields[6];
    maskData.ruleStdNotIp.common.qosProfile = (GT_U8)inFields[7];
    maskData.ruleStdNotIp.common.isIp = (GT_U8)inFields[8];
    maskData.ruleStdNotIp.common.isL2Valid = (GT_U8)inFields[9];
    maskData.ruleStdNotIp.common.isUdbValid = (GT_U8)inFields[10];
    maskData.ruleStdNotIp.isIpv4 = (GT_U8)inFields[11];
    maskData.ruleStdNotIp.etherType = (GT_U16)inFields[12];
    maskData.ruleStdNotIp.isArp = (GT_BOOL)inFields[13];
    maskData.ruleStdNotIp.l2Encap = (GT_BOOL)inFields[14];
    galtisMacAddr(&maskData.ruleStdNotIp.macDa, (GT_U8*)inFields[15]);
    galtisMacAddr(&maskData.ruleStdNotIp.macSa, (GT_U8*)inFields[16]);
    galtisBArray(&maskBArr,(GT_U8*)inFields[17]);

    cmdOsMemCpy(
        maskData.ruleStdNotIp.udb, maskBArr.data,
        MIN(3, maskBArr.length));

    patternData.ruleStdNotIp.common.portListBmp = ruleSet_portListBmpPattern;

    patternData.ruleStdNotIp.common.pclId = (GT_U16)inFields[18];
    patternData.ruleStdNotIp.common.sourcePort = (GT_U8)inFields[19];
    patternData.ruleStdNotIp.common.isTagged = (GT_U8)inFields[20];
    patternData.ruleStdNotIp.common.vid = (GT_U16)inFields[21];
    patternData.ruleStdNotIp.common.up = (GT_U8)inFields[22];
    patternData.ruleStdNotIp.common.qosProfile = (GT_U8)inFields[23];
    patternData.ruleStdNotIp.common.isIp = (GT_U8)inFields[24];
    patternData.ruleStdNotIp.common.isL2Valid = (GT_U8)inFields[25];
    patternData.ruleStdNotIp.common.isUdbValid = (GT_U8)inFields[26];
    patternData.ruleStdNotIp.isIpv4 = (GT_U8)inFields[27];
    patternData.ruleStdNotIp.etherType = (GT_U16)inFields[28];
    patternData.ruleStdNotIp.isArp = (GT_BOOL)inFields[29];
    patternData.ruleStdNotIp.l2Encap = (GT_BOOL)inFields[30];
    galtisMacAddr(&patternData.ruleStdNotIp.macDa, (GT_U8*)inFields[31]);
    galtisMacAddr(&patternData.ruleStdNotIp.macSa, (GT_U8*)inFields[32]);
    galtisBArray(&patternBArr,(GT_U8*)inFields[33]);

    cmdOsMemCpy(
        patternData.ruleStdNotIp.udb, patternBArr.data,
        MIN(3, patternBArr.length));

    actionRetrieve(inFields, 34, &actionData);

    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleStdNotIp.common.sourcePort);
    devNum=(GT_U8)inArgs[0];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleStdNotIp.common.sourcePort);

    lastActionPtr = &actionData;

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, &maskData,
                                                &patternData, &actionData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleSet_STD_IP_L2_QOS

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U32                             ruleIndex;
    GT_BYTE_ARRY                       maskBArr, patternBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    ruleIndex = (GT_U32)inFields[0];
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;

    maskData.ruleStdIpL2Qos.common.portListBmp = ruleSet_portListBmpMask;

    maskData.ruleStdIpL2Qos.common.pclId = (GT_U16)inFields[2];
    CONVERT_PCL_MASK_PCL_ID_TEST_TO_CPSS_MAC(maskData.ruleStdIpL2Qos.common.pclId);
    maskData.ruleStdIpL2Qos.common.sourcePort = (GT_U8)inFields[3];
    maskData.ruleStdIpL2Qos.common.isTagged = (GT_U8)inFields[4];
    maskData.ruleStdIpL2Qos.common.vid = (GT_U16)inFields[5];
    maskData.ruleStdIpL2Qos.common.up = (GT_U8)inFields[6];
    maskData.ruleStdIpL2Qos.common.qosProfile = (GT_U8)inFields[7];
    maskData.ruleStdIpL2Qos.common.isIp = (GT_U8)inFields[8];
    maskData.ruleStdIpL2Qos.common.isL2Valid = (GT_U8)inFields[9];
    maskData.ruleStdIpL2Qos.common.isUdbValid = (GT_U8)inFields[10];
    maskData.ruleStdIpL2Qos.commonStdIp.isIpv4 = (GT_U8)inFields[11];
    maskData.ruleStdIpL2Qos.commonStdIp.ipProtocol = (GT_U8)inFields[12];
    maskData.ruleStdIpL2Qos.commonStdIp.dscp = (GT_U8)inFields[13];
    maskData.ruleStdIpL2Qos.commonStdIp.isL4Valid = (GT_U8)inFields[14];
    maskData.ruleStdIpL2Qos.commonStdIp.l4Byte2 = (GT_U8)inFields[15];
    maskData.ruleStdIpL2Qos.commonStdIp.l4Byte3 = (GT_U8)inFields[16];
    maskData.ruleStdIpL2Qos.commonStdIp.ipHeaderOk = (GT_U8)inFields[17];
    maskData.ruleStdIpL2Qos.commonStdIp.ipv4Fragmented = (GT_U8)inFields[18];
    maskData.ruleStdIpL2Qos.isArp = (GT_BOOL)inFields[19];
    maskData.ruleStdIpL2Qos.isIpv6ExtHdrExist = (GT_BOOL)inFields[20];
    maskData.ruleStdIpL2Qos.isIpv6HopByHop = (GT_BOOL)inFields[21];
    galtisMacAddr(&maskData.ruleStdIpL2Qos.macDa, (GT_U8*)inFields[22]);
    galtisMacAddr(&maskData.ruleStdIpL2Qos.macSa, (GT_U8*)inFields[23]);
    galtisBArray(&maskBArr,(GT_U8*)inFields[24]);

    cmdOsMemCpy(
        maskData.ruleStdIpL2Qos.udb, maskBArr.data,
        MIN(2, maskBArr.length));

    patternData.ruleStdIpL2Qos.common.portListBmp = ruleSet_portListBmpPattern;

    patternData.ruleStdIpL2Qos.common.pclId = (GT_U16)inFields[25];
    patternData.ruleStdIpL2Qos.common.sourcePort = (GT_U8)inFields[26];
    patternData.ruleStdIpL2Qos.common.isTagged = (GT_U8)inFields[27];
    patternData.ruleStdIpL2Qos.common.vid = (GT_U16)inFields[28];
    patternData.ruleStdIpL2Qos.common.up = (GT_U8)inFields[29];
    patternData.ruleStdIpL2Qos.common.qosProfile = (GT_U8)inFields[30];
    patternData.ruleStdIpL2Qos.common.isIp = (GT_U8)inFields[31];
    patternData.ruleStdIpL2Qos.common.isL2Valid = (GT_U8)inFields[32];
    patternData.ruleStdIpL2Qos.common.isUdbValid = (GT_U8)inFields[33];
    patternData.ruleStdIpL2Qos.commonStdIp.isIpv4 = (GT_U8)inFields[34];
    patternData.ruleStdIpL2Qos.commonStdIp.ipProtocol = (GT_U8)inFields[35];
    patternData.ruleStdIpL2Qos.commonStdIp.dscp = (GT_U8)inFields[36];
    patternData.ruleStdIpL2Qos.commonStdIp.isL4Valid = (GT_U8)inFields[37];
    patternData.ruleStdIpL2Qos.commonStdIp.l4Byte2 = (GT_U8)inFields[38];
    patternData.ruleStdIpL2Qos.commonStdIp.l4Byte3 = (GT_U8)inFields[39];
    patternData.ruleStdIpL2Qos.commonStdIp.ipHeaderOk = (GT_U8)inFields[40];
    patternData.ruleStdIpL2Qos.commonStdIp.ipv4Fragmented = (GT_U8)inFields[41];
    patternData.ruleStdIpL2Qos.isArp = (GT_BOOL)inFields[42];
    patternData.ruleStdIpL2Qos.isIpv6ExtHdrExist = (GT_BOOL)inFields[43];
    patternData.ruleStdIpL2Qos.isIpv6HopByHop = (GT_BOOL)inFields[44];
    galtisMacAddr(&patternData.ruleStdIpL2Qos.macDa, (GT_U8*)inFields[45]);
    galtisMacAddr(&patternData.ruleStdIpL2Qos.macSa, (GT_U8*)inFields[46]);
    galtisBArray(&patternBArr,(GT_U8*)inFields[47]);

    cmdOsMemCpy(
        patternData.ruleStdIpL2Qos.udb, patternBArr.data,
        MIN(2, patternBArr.length));

    actionRetrieve(inFields, 48, &actionData);

    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleStdNotIp.common.sourcePort);
    devNum = (GT_U8)inArgs[0];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleStdNotIp.common.sourcePort);

    lastActionPtr = &actionData;

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, &maskData,
                                                &patternData, &actionData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleSet_EXT_NOT_IPV6

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U32                             ruleIndex;
    GT_BYTE_ARRY                       maskBArr, patternBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    ruleIndex = (GT_U32)inFields[0];
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;

    maskData.ruleExtNotIpv6.common.portListBmp = ruleSet_portListBmpMask;

    maskData.ruleExtNotIpv6.common.pclId = (GT_U16)inFields[2];
    maskData.ruleExtNotIpv6.common.sourcePort = (GT_U8)inFields[3];
    maskData.ruleExtNotIpv6.common.isTagged = (GT_U8)inFields[4];
    maskData.ruleExtNotIpv6.common.vid = (GT_U16)inFields[5];
    maskData.ruleExtNotIpv6.common.up = (GT_U8)inFields[6];
    maskData.ruleExtNotIpv6.common.qosProfile = (GT_U8)inFields[7];
    maskData.ruleExtNotIpv6.common.isIp = (GT_U8)inFields[8];
    maskData.ruleExtNotIpv6.common.isL2Valid = (GT_U8)inFields[9];
    maskData.ruleExtNotIpv6.common.isUdbValid = (GT_U8)inFields[10];
    maskData.ruleExtNotIpv6.commonExt.isIpv6 = (GT_U8)inFields[11];
    maskData.ruleExtNotIpv6.commonExt.ipProtocol = (GT_U8)inFields[12];
    maskData.ruleExtNotIpv6.commonExt.dscp = (GT_U8)inFields[13];
    maskData.ruleExtNotIpv6.commonExt.isL4Valid = (GT_U8)inFields[14];
    maskData.ruleExtNotIpv6.commonExt.l4Byte0 = (GT_U8)inFields[15];
    maskData.ruleExtNotIpv6.commonExt.l4Byte1 = (GT_U8)inFields[16];
    maskData.ruleExtNotIpv6.commonExt.l4Byte2 = (GT_U8)inFields[17];
    maskData.ruleExtNotIpv6.commonExt.l4Byte3 = (GT_U8)inFields[18];
    maskData.ruleExtNotIpv6.commonExt.l4Byte13 = (GT_U8)inFields[19];
    maskData.ruleExtNotIpv6.commonExt.ipHeaderOk = (GT_U8)inFields[20];
    galtisIpAddr(&maskData.ruleExtNotIpv6.sip, (GT_U8*)inFields[21]);
    galtisIpAddr(&maskData.ruleExtNotIpv6.dip, (GT_U8*)inFields[22]);
    maskData.ruleExtNotIpv6.etherType = (GT_U16)inFields[23];
    maskData.ruleExtNotIpv6.l2Encap = (GT_U8)inFields[24];
    galtisMacAddr(&maskData.ruleExtNotIpv6.macDa, (GT_U8*)inFields[25]);
    galtisMacAddr(&maskData.ruleExtNotIpv6.macSa, (GT_U8*)inFields[26]);
    maskData.ruleExtNotIpv6.ipv4Fragmented = (GT_BOOL)inFields[27];
    galtisBArray(&maskBArr,(GT_U8*)inFields[28]);

    cmdOsMemCpy(
        maskData.ruleExtNotIpv6.udb, maskBArr.data,
        MIN(6, maskBArr.length));

    patternData.ruleExtNotIpv6.common.portListBmp = ruleSet_portListBmpPattern;

    patternData.ruleExtNotIpv6.common.pclId = (GT_U16)inFields[29];
    patternData.ruleExtNotIpv6.common.sourcePort = (GT_U8)inFields[30];
    patternData.ruleExtNotIpv6.common.isTagged = (GT_U8)inFields[31];
    patternData.ruleExtNotIpv6.common.vid = (GT_U16)inFields[32];
    patternData.ruleExtNotIpv6.common.up = (GT_U8)inFields[33];
    patternData.ruleExtNotIpv6.common.qosProfile = (GT_U8)inFields[34];
    patternData.ruleExtNotIpv6.common.isIp = (GT_U8)inFields[35];
    patternData.ruleExtNotIpv6.common.isL2Valid = (GT_U8)inFields[36];
    patternData.ruleExtNotIpv6.common.isUdbValid = (GT_U8)inFields[37];
    patternData.ruleExtNotIpv6.commonExt.isIpv6 = (GT_U8)inFields[38];
    patternData.ruleExtNotIpv6.commonExt.ipProtocol = (GT_U8)inFields[39];
    patternData.ruleExtNotIpv6.commonExt.dscp = (GT_U8)inFields[40];
    patternData.ruleExtNotIpv6.commonExt.isL4Valid = (GT_U8)inFields[40];
    patternData.ruleExtNotIpv6.commonExt.l4Byte0 = (GT_U8)inFields[42];
    patternData.ruleExtNotIpv6.commonExt.l4Byte1 = (GT_U8)inFields[43];
    patternData.ruleExtNotIpv6.commonExt.l4Byte2 = (GT_U8)inFields[44];
    patternData.ruleExtNotIpv6.commonExt.l4Byte3 = (GT_U8)inFields[45];
    patternData.ruleExtNotIpv6.commonExt.l4Byte13 = (GT_U8)inFields[46];
    patternData.ruleExtNotIpv6.commonExt.ipHeaderOk = (GT_U8)inFields[47];
    galtisIpAddr(&patternData.ruleExtNotIpv6.sip, (GT_U8*)inFields[48]);
    galtisIpAddr(&patternData.ruleExtNotIpv6.dip, (GT_U8*)inFields[49]);
    patternData.ruleExtNotIpv6.etherType = (GT_U16)inFields[50];
    patternData.ruleExtNotIpv6.l2Encap = (GT_U8)inFields[51];
    galtisMacAddr(&patternData.ruleExtNotIpv6.macDa, (GT_U8*)inFields[52]);
    galtisMacAddr(&patternData.ruleExtNotIpv6.macSa, (GT_U8*)inFields[53]);
    patternData.ruleExtNotIpv6.ipv4Fragmented = (GT_BOOL)inFields[54];
    galtisBArray(&patternBArr,(GT_U8*)inFields[55]);

    cmdOsMemCpy(
        patternData.ruleExtNotIpv6.udb, patternBArr.data,
        MIN(6, patternBArr.length));

    actionRetrieve(inFields, 56, &actionData);

    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleStdNotIp.common.sourcePort);
    devNum = (GT_U8)inArgs[0];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleStdNotIp.common.sourcePort);

    lastActionPtr = &actionData;

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, &maskData,
                                                &patternData, &actionData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleSet_EXT_IPV6_L2

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U32                             ruleIndex;
    GT_BYTE_ARRY                       maskBArr, patternBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    ruleIndex = (GT_U32)inFields[0];
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;

    maskData.ruleExtIpv6L2.common.portListBmp = ruleSet_portListBmpMask;

    maskData.ruleExtIpv6L2.common.pclId = (GT_U16)inFields[2];
    maskData.ruleExtIpv6L2.common.sourcePort = (GT_U8)inFields[3];
    maskData.ruleExtIpv6L2.common.isTagged = (GT_U8)inFields[4];
    maskData.ruleExtIpv6L2.common.vid = (GT_U16)inFields[5];
    maskData.ruleExtIpv6L2.common.up = (GT_U8)inFields[6];
    maskData.ruleExtIpv6L2.common.qosProfile = (GT_U8)inFields[7];
    maskData.ruleExtIpv6L2.common.isIp = (GT_U8)inFields[8];
    maskData.ruleExtIpv6L2.common.isL2Valid = (GT_U8)inFields[9];
    maskData.ruleExtIpv6L2.common.isUdbValid = (GT_U8)inFields[10];
    maskData.ruleExtIpv6L2.commonExt.isIpv6 = (GT_U8)inFields[11];
    maskData.ruleExtIpv6L2.commonExt.ipProtocol = (GT_U8)inFields[12];
    maskData.ruleExtIpv6L2.commonExt.dscp = (GT_U8)inFields[13];
    maskData.ruleExtIpv6L2.commonExt.isL4Valid = (GT_U8)inFields[14];
    maskData.ruleExtIpv6L2.commonExt.l4Byte0 = (GT_U8)inFields[15];
    maskData.ruleExtIpv6L2.commonExt.l4Byte1 = (GT_U8)inFields[16];
    maskData.ruleExtIpv6L2.commonExt.l4Byte2 = (GT_U8)inFields[17];
    maskData.ruleExtIpv6L2.commonExt.l4Byte3 = (GT_U8)inFields[18];
    maskData.ruleExtIpv6L2.commonExt.l4Byte13 = (GT_U8)inFields[19];
    maskData.ruleExtIpv6L2.commonExt.ipHeaderOk = (GT_U8)inFields[20];
    galtisIpv6Addr(&maskData.ruleExtIpv6L2.sip, (GT_U8*)inFields[21]);
    maskData.ruleExtIpv6L2.dipBits127to120 = (GT_U8)inFields[22];
    maskData.ruleExtIpv6L2.isIpv6ExtHdrExist = (GT_BOOL)inFields[23];
    maskData.ruleExtIpv6L2.isIpv6HopByHop = (GT_BOOL)inFields[24];
    galtisMacAddr(&maskData.ruleExtIpv6L2.macDa, (GT_U8*)inFields[25]);
    galtisMacAddr(&maskData.ruleExtIpv6L2.macSa, (GT_U8*)inFields[26]);
    galtisBArray(&maskBArr,(GT_U8*)inFields[27]);

    cmdOsMemCpy(
        maskData.ruleExtIpv6L2.udb, maskBArr.data,
        MIN(6, maskBArr.length));

    patternData.ruleExtIpv6L2.common.portListBmp = ruleSet_portListBmpPattern;

    patternData.ruleExtIpv6L2.common.pclId = (GT_U16)inFields[28];
    patternData.ruleExtIpv6L2.common.sourcePort = (GT_U8)inFields[29];
    patternData.ruleExtIpv6L2.common.isTagged = (GT_U8)inFields[30];
    patternData.ruleExtIpv6L2.common.vid = (GT_U16)inFields[31];
    patternData.ruleExtIpv6L2.common.up = (GT_U8)inFields[32];
    patternData.ruleExtIpv6L2.common.qosProfile = (GT_U8)inFields[33];
    patternData.ruleExtIpv6L2.common.isIp = (GT_U8)inFields[34];
    patternData.ruleExtIpv6L2.common.isL2Valid = (GT_U8)inFields[35];
    patternData.ruleExtIpv6L2.common.isUdbValid = (GT_U8)inFields[36];
    patternData.ruleExtIpv6L2.commonExt.isIpv6 = (GT_U8)inFields[37];
    patternData.ruleExtIpv6L2.commonExt.ipProtocol = (GT_U8)inFields[38];
    patternData.ruleExtIpv6L2.commonExt.dscp = (GT_U8)inFields[39];
    patternData.ruleExtIpv6L2.commonExt.isL4Valid = (GT_U8)inFields[40];
    patternData.ruleExtIpv6L2.commonExt.l4Byte0 = (GT_U8)inFields[41];
    patternData.ruleExtIpv6L2.commonExt.l4Byte1 = (GT_U8)inFields[42];
    patternData.ruleExtIpv6L2.commonExt.l4Byte2 = (GT_U8)inFields[43];
    patternData.ruleExtIpv6L2.commonExt.l4Byte3 = (GT_U8)inFields[44];
    patternData.ruleExtIpv6L2.commonExt.l4Byte13 = (GT_U8)inFields[45];
    patternData.ruleExtIpv6L2.commonExt.ipHeaderOk = (GT_U8)inFields[46];
    galtisIpv6Addr(&patternData.ruleExtIpv6L2.sip, (GT_U8*)inFields[47]);
    patternData.ruleExtIpv6L2.dipBits127to120 = (GT_U8)inFields[48];
    patternData.ruleExtIpv6L2.isIpv6ExtHdrExist = (GT_BOOL)inFields[49];
    patternData.ruleExtIpv6L2.isIpv6HopByHop = (GT_BOOL)inFields[50];
    galtisMacAddr(&patternData.ruleExtIpv6L2.macDa, (GT_U8*)inFields[51]);
    galtisMacAddr(&patternData.ruleExtIpv6L2.macSa, (GT_U8*)inFields[52]);
    galtisBArray(&patternBArr,(GT_U8*)inFields[53]);

    cmdOsMemCpy(
        patternData.ruleExtIpv6L2.udb, patternBArr.data,
        MIN(6, patternBArr.length));

    actionRetrieve(inFields, 54, &actionData);

    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleStdNotIp.common.sourcePort);
    devNum = (GT_U8)inArgs[0];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleStdNotIp.common.sourcePort);

    lastActionPtr = &actionData;

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, &maskData,
                                                &patternData, &actionData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleSet_EXT_IPV6_L4

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U32                             ruleIndex;
    GT_BYTE_ARRY                       maskBArr, patternBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    ruleIndex = (GT_U32)inFields[0];
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E;

    maskData.ruleExtIpv6L4.common.portListBmp = ruleSet_portListBmpMask;

    maskData.ruleExtIpv6L4.common.pclId = (GT_U16)inFields[2];
    maskData.ruleExtIpv6L4.common.sourcePort = (GT_U8)inFields[3];
    maskData.ruleExtIpv6L4.common.isTagged = (GT_U8)inFields[4];
    maskData.ruleExtIpv6L4.common.vid = (GT_U16)inFields[5];
    maskData.ruleExtIpv6L4.common.up = (GT_U8)inFields[6];
    maskData.ruleExtIpv6L4.common.qosProfile = (GT_U8)inFields[7];
    maskData.ruleExtIpv6L4.common.isIp = (GT_U8)inFields[8];
    maskData.ruleExtIpv6L4.common.isL2Valid = (GT_U8)inFields[9];
    maskData.ruleExtIpv6L4.common.isUdbValid = (GT_U8)inFields[10];
    maskData.ruleExtIpv6L4.commonExt.isIpv6 = (GT_U8)inFields[11];
    maskData.ruleExtIpv6L4.commonExt.ipProtocol = (GT_U8)inFields[12];
    maskData.ruleExtIpv6L4.commonExt.dscp = (GT_U8)inFields[13];
    maskData.ruleExtIpv6L4.commonExt.isL4Valid = (GT_U8)inFields[14];
    maskData.ruleExtIpv6L4.commonExt.l4Byte0 = (GT_U8)inFields[15];
    maskData.ruleExtIpv6L4.commonExt.l4Byte1 = (GT_U8)inFields[16];
    maskData.ruleExtIpv6L4.commonExt.l4Byte2 = (GT_U8)inFields[17];
    maskData.ruleExtIpv6L4.commonExt.l4Byte3 = (GT_U8)inFields[18];
    maskData.ruleExtIpv6L4.commonExt.l4Byte13 = (GT_U8)inFields[19];
    maskData.ruleExtIpv6L4.commonExt.ipHeaderOk = (GT_U8)inFields[20];
    galtisIpv6Addr(&maskData.ruleExtIpv6L4.sip, (GT_U8*)inFields[21]);
    galtisIpv6Addr(&maskData.ruleExtIpv6L4.dip, (GT_U8*)inFields[22]);
    maskData.ruleExtIpv6L4.isIpv6ExtHdrExist = (GT_U8)inFields[23];
    maskData.ruleExtIpv6L4.isIpv6HopByHop = (GT_U8)inFields[24];
    galtisBArray(&maskBArr,(GT_U8*)inFields[25]);

    cmdOsMemCpy(
        maskData.ruleExtIpv6L4.udb, maskBArr.data,
        MIN(3, maskBArr.length));

    patternData.ruleExtIpv6L4.common.portListBmp = ruleSet_portListBmpPattern;

    patternData.ruleExtIpv6L4.common.pclId = (GT_U16)inFields[26];
    patternData.ruleExtIpv6L4.common.sourcePort = (GT_U8)inFields[27];
    patternData.ruleExtIpv6L4.common.isTagged = (GT_BOOL)inFields[28];
    patternData.ruleExtIpv6L4.common.vid = (GT_U16)inFields[29];
    patternData.ruleExtIpv6L4.common.up = (GT_U8)inFields[30];
    patternData.ruleExtIpv6L4.common.qosProfile = (GT_U8)inFields[31];
    patternData.ruleExtIpv6L4.common.isIp = (GT_BOOL)inFields[32];
    patternData.ruleExtIpv6L4.common.isL2Valid = (GT_BOOL)inFields[33];
    patternData.ruleExtIpv6L4.common.isUdbValid = (GT_BOOL)inFields[34];
    patternData.ruleExtIpv6L4.commonExt.isIpv6 = (GT_U8)inFields[35];
    patternData.ruleExtIpv6L4.commonExt.ipProtocol = (GT_U8)inFields[36];
    patternData.ruleExtIpv6L4.commonExt.dscp = (GT_U8)inFields[37];
    patternData.ruleExtIpv6L4.commonExt.isL4Valid = (GT_BOOL)inFields[38];
    patternData.ruleExtIpv6L4.commonExt.l4Byte0 = (GT_U8)inFields[39];
    patternData.ruleExtIpv6L4.commonExt.l4Byte1 = (GT_U8)inFields[40];
    patternData.ruleExtIpv6L4.commonExt.l4Byte2 = (GT_U8)inFields[41];
    patternData.ruleExtIpv6L4.commonExt.l4Byte3 = (GT_U8)inFields[42];
    patternData.ruleExtIpv6L4.commonExt.l4Byte13 = (GT_U8)inFields[43];
    patternData.ruleExtIpv6L4.commonExt.ipHeaderOk = (GT_BOOL)inFields[44];
    galtisIpv6Addr(&patternData.ruleExtIpv6L4.sip, (GT_U8*)inFields[45]);
    galtisIpv6Addr(&patternData.ruleExtIpv6L4.dip, (GT_U8*)inFields[46]);
    patternData.ruleExtIpv6L4.isIpv6ExtHdrExist = (GT_BOOL)inFields[47];
    patternData.ruleExtIpv6L4.isIpv6HopByHop = (GT_BOOL)inFields[48];
    galtisBArray(&patternBArr,(GT_U8*)inFields[49]);

    cmdOsMemCpy(
        patternData.ruleExtIpv6L4.udb, patternBArr.data,
        MIN(3, patternBArr.length));

    actionRetrieve(inFields, 50, &actionData);

    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleStdNotIp.common.sourcePort);
    devNum = (GT_U8)inArgs[0];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleStdNotIp.common.sourcePort);

    lastActionPtr = &actionData;

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, &maskData,
                                                &patternData, &actionData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleSet_STD_IPV6_DIP

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U32                             ruleIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    ruleIndex = (GT_U32)inFields[0];

    maskData.ruleStdIpv6Dip.common.portListBmp = ruleSet_portListBmpMask;

    maskData.ruleStdIpv6Dip.common.pclId = (GT_U16)inFields[2];
    CONVERT_PCL_MASK_PCL_ID_TEST_TO_CPSS_MAC(maskData.ruleStdIpv6Dip.common.pclId);
    maskData.ruleStdIpv6Dip.common.sourcePort = (GT_U8)inFields[3];
    maskData.ruleStdIpv6Dip.common.isTagged = (GT_U8)inFields[4];
    maskData.ruleStdIpv6Dip.common.vid = (GT_U16)inFields[5];
    maskData.ruleStdIpv6Dip.common.up = (GT_U8)inFields[6];
    maskData.ruleStdIpv6Dip.common.qosProfile = (GT_U8)inFields[7];
    maskData.ruleStdIpv6Dip.common.isIp = (GT_U8)inFields[8];
    maskData.ruleStdIpv6Dip.common.isL2Valid = (GT_U8)inFields[9];
    maskData.ruleStdIpv6Dip.common.isUdbValid = (GT_U8)inFields[10];
    maskData.ruleStdIpv6Dip.commonStdIp.isIpv4 = (GT_U8)inFields[11];
    maskData.ruleStdIpv6Dip.commonStdIp.ipProtocol = (GT_U8)inFields[12];
    maskData.ruleStdIpv6Dip.commonStdIp.dscp = (GT_U8)inFields[13];
    maskData.ruleStdIpv6Dip.commonStdIp.isL4Valid = (GT_U8)inFields[14];
    maskData.ruleStdIpv6Dip.commonStdIp.l4Byte2 = (GT_U8)inFields[15];
    maskData.ruleStdIpv6Dip.commonStdIp.l4Byte3 = (GT_U8)inFields[16];
    maskData.ruleStdIpv6Dip.commonStdIp.ipHeaderOk = (GT_U8)inFields[17];
    maskData.ruleStdIpv6Dip.commonStdIp.ipv4Fragmented = (GT_U8)inFields[18];
    maskData.ruleStdIpv6Dip.isArp = (GT_BOOL)inFields[19];
    maskData.ruleStdIpv6Dip.isIpv6ExtHdrExist = (GT_U8)inFields[20];
    maskData.ruleStdIpv6Dip.isIpv6HopByHop = (GT_U8)inFields[21];
    galtisIpv6Addr(&maskData.ruleStdIpv6Dip.dip, (GT_U8*)inFields[22]);

    patternData.ruleStdIpv6Dip.common.portListBmp = ruleSet_portListBmpPattern;

    patternData.ruleStdIpv6Dip.common.pclId = (GT_U16)inFields[23];
    patternData.ruleStdIpv6Dip.common.sourcePort = (GT_U8)inFields[24];
    patternData.ruleStdIpv6Dip.common.isTagged = (GT_U8)inFields[25];
    patternData.ruleStdIpv6Dip.common.vid = (GT_U16)inFields[26];
    patternData.ruleStdIpv6Dip.common.up = (GT_U8)inFields[27];
    patternData.ruleStdIpv6Dip.common.qosProfile = (GT_U8)inFields[28];
    patternData.ruleStdIpv6Dip.common.isIp = (GT_U8)inFields[29];
    patternData.ruleStdIpv6Dip.common.isL2Valid = (GT_U8)inFields[30];
    patternData.ruleStdIpv6Dip.common.isUdbValid = (GT_U8)inFields[31];
    patternData.ruleStdIpv6Dip.commonStdIp.isIpv4 = (GT_U8)inFields[32];
    patternData.ruleStdIpv6Dip.commonStdIp.ipProtocol = (GT_U8)inFields[33];
    patternData.ruleStdIpv6Dip.commonStdIp.dscp = (GT_U8)inFields[34];
    patternData.ruleStdIpv6Dip.commonStdIp.isL4Valid = (GT_U8)inFields[35];
    patternData.ruleStdIpv6Dip.commonStdIp.l4Byte2 = (GT_U8)inFields[36];
    patternData.ruleStdIpv6Dip.commonStdIp.l4Byte3 = (GT_U8)inFields[37];
    patternData.ruleStdIpv6Dip.commonStdIp.ipHeaderOk = (GT_U8)inFields[38];
    patternData.ruleStdIpv6Dip.commonStdIp.ipv4Fragmented = (GT_U8)inFields[39];
    patternData.ruleStdIpv6Dip.isArp = (GT_BOOL)inFields[40];
    patternData.ruleStdIpv6Dip.isIpv6ExtHdrExist = (GT_U8)inFields[41];
    patternData.ruleStdIpv6Dip.isIpv6HopByHop = (GT_U8)inFields[42];
    galtisIpv6Addr(&patternData.ruleStdIpv6Dip.dip, (GT_U8*)inFields[43]);

    actionRetrieve(inFields, 44, &actionData);

    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleStdNotIp.common.sourcePort);
    devNum = (GT_U8)inArgs[0];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleStdNotIp.common.sourcePort);

    lastActionPtr = &actionData;

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclRuleSet(devNum,
                              CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E,
                                  ruleIndex, &maskData, &patternData, &actionData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleSet_STD_IPV4_L4

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U32                             ruleIndex;
    GT_BYTE_ARRY                       maskBArr, patternBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    ruleIndex = (GT_U32)inFields[0];
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;

    maskData.ruleStdIpv4L4.common.portListBmp = ruleSet_portListBmpMask;

    maskData.ruleStdIpv4L4.common.pclId = (GT_U16)inFields[2];
    CONVERT_PCL_MASK_PCL_ID_TEST_TO_CPSS_MAC(maskData.ruleStdIpv4L4.common.pclId);
    maskData.ruleStdIpv4L4.common.sourcePort = (GT_U8)inFields[3];
    maskData.ruleStdIpv4L4.common.isTagged = (GT_U8)inFields[4];
    maskData.ruleStdIpv4L4.common.vid = (GT_U16)inFields[5];
    maskData.ruleStdIpv4L4.common.up = (GT_U8)inFields[6];
    maskData.ruleStdIpv4L4.common.qosProfile = (GT_U8)inFields[7];
    maskData.ruleStdIpv4L4.common.isIp = (GT_U8)inFields[8];
    maskData.ruleStdIpv4L4.common.isL2Valid = (GT_U8)inFields[9];
    maskData.ruleStdIpv4L4.common.isUdbValid = (GT_U8)inFields[10];
    maskData.ruleStdIpv4L4.commonStdIp.isIpv4 = (GT_U8)inFields[11];
    maskData.ruleStdIpv4L4.commonStdIp.ipProtocol = (GT_U8)inFields[12];
    maskData.ruleStdIpv4L4.commonStdIp.dscp = (GT_U8)inFields[13];
    maskData.ruleStdIpv4L4.commonStdIp.isL4Valid = (GT_U8)inFields[14];
    maskData.ruleStdIpv4L4.commonStdIp.l4Byte2 = (GT_U8)inFields[15];
    maskData.ruleStdIpv4L4.commonStdIp.l4Byte3 = (GT_U8)inFields[16];
    maskData.ruleStdIpv4L4.commonStdIp.ipHeaderOk = (GT_U8)inFields[17];
    maskData.ruleStdIpv4L4.commonStdIp.ipv4Fragmented = (GT_U8)inFields[18];
    maskData.ruleStdIpv4L4.isArp = (GT_BOOL)inFields[19];
    maskData.ruleStdIpv4L4.isBc = (GT_BOOL)inFields[20];
    galtisIpAddr(&maskData.ruleStdIpv4L4.sip, (GT_U8*)inFields[21]);
    galtisIpAddr(&maskData.ruleStdIpv4L4.dip, (GT_U8*)inFields[22]);
    maskData.ruleStdIpv4L4.l4Byte0 = (GT_U8)inFields[23];
    maskData.ruleStdIpv4L4.l4Byte1 = (GT_U8)inFields[24];
    maskData.ruleStdIpv4L4.l4Byte13 = (GT_U8)inFields[25];
    galtisBArray(&maskBArr,(GT_U8*)inFields[26]);

    cmdOsMemCpy(
        maskData.ruleStdIpv4L4.udb, maskBArr.data,
        MIN(3, maskBArr.length));

    patternData.ruleStdIpv4L4.common.portListBmp = ruleSet_portListBmpPattern;

    patternData.ruleStdIpv4L4.common.pclId = (GT_U16)inFields[27];
    patternData.ruleStdIpv4L4.common.sourcePort = (GT_U8)inFields[28];
    patternData.ruleStdIpv4L4.common.isTagged = (GT_U8)inFields[29];
    patternData.ruleStdIpv4L4.common.vid = (GT_U16)inFields[30];
    patternData.ruleStdIpv4L4.common.up = (GT_U8)inFields[31];
    patternData.ruleStdIpv4L4.common.qosProfile = (GT_U8)inFields[32];
    patternData.ruleStdIpv4L4.common.isIp = (GT_U8)inFields[33];
    patternData.ruleStdIpv4L4.common.isL2Valid = (GT_U8)inFields[34];
    patternData.ruleStdIpv4L4.common.isUdbValid = (GT_U8)inFields[35];
    patternData.ruleStdIpv4L4.commonStdIp.isIpv4 = (GT_U8)inFields[36];
    patternData.ruleStdIpv4L4.commonStdIp.ipProtocol = (GT_U8)inFields[37];
    patternData.ruleStdIpv4L4.commonStdIp.dscp = (GT_U8)inFields[38];
    patternData.ruleStdIpv4L4.commonStdIp.isL4Valid = (GT_U8)inFields[39];
    patternData.ruleStdIpv4L4.commonStdIp.l4Byte2 = (GT_U8)inFields[40];
    patternData.ruleStdIpv4L4.commonStdIp.l4Byte3 = (GT_U8)inFields[41];
    patternData.ruleStdIpv4L4.commonStdIp.ipHeaderOk = (GT_U8)inFields[42];
    patternData.ruleStdIpv4L4.commonStdIp.ipv4Fragmented = (GT_U8)inFields[43];
    patternData.ruleStdIpv4L4.isArp = (GT_BOOL)inFields[44];
    patternData.ruleStdIpv4L4.isBc = (GT_BOOL)inFields[45];
    galtisIpAddr(&patternData.ruleStdIpv4L4.sip, (GT_U8*)inFields[46]);
    galtisIpAddr(&patternData.ruleStdIpv4L4.dip, (GT_U8*)inFields[47]);
    patternData.ruleStdIpv4L4.l4Byte0 = (GT_U8)inFields[48];
    patternData.ruleStdIpv4L4.l4Byte1 = (GT_U8)inFields[49];
    patternData.ruleStdIpv4L4.l4Byte13 = (GT_U8)inFields[50];
    galtisBArray(&patternBArr,(GT_U8*)inFields[51]);

    cmdOsMemCpy(
        patternData.ruleStdIpv4L4.udb, patternBArr.data,
        MIN(3, patternBArr.length));

    actionRetrieve(inFields, 52, &actionData);

    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleStdNotIp.common.sourcePort);
    devNum = (GT_U8)inArgs[0];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleStdNotIp.common.sourcePort);

    lastActionPtr = &actionData;

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, &maskData,
                                                &patternData, &actionData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}




/* Table: cpssDxChPclRuleAction
*
* Description:
*     PCL Configuration table
*
* Fields:
*      pktCmd       -  packet command (forward, mirror-to-cpu,
*                      hard-drop, soft-drop, or trap-to-cpu)
*      mirror       -  packet mirroring configuration
*           mirrorToRxAnalyzerPort
*                        - Enables mirroring the packet to
*                          the ingress analyzer port.
*                          GT_FALSE = Packet is not mirrored to ingress
*                                     analyzer port.
*                          GT_TRUE = Packet is mirrored to ingress analyzer port.
*           cpuCode      - The CPU code assigned to packets
*                          Mirrored to CPU or Trapped to CPU due
*                          to a match in the Policy rule entry
*      matchCounter -  match counter configuration
*           enableMatchCount
*                        - Enables the binding of this
*                          policy action entry to the Policy Rule Match
*                          Counter<n> (0<=n<32) indexed by matchCounterIndex
*                          GT_FALSE  = Match counter binding is disabled.
*                          GT_TRUE = Match counter binding is enabled.
*
*           matchCounterIndex
*                        - A index one of the 32 Policy Rule Match Counter<n>
*                          (0<=n<32) The counter is incremented for every packet
*                          satisfying both of the following conditions:
*                          - Matching this rule.
*                          - The previous packet command is not hard drop.
*      qos          -  packet QoS attributes modification configuration
*           egressPolicy - GT_TRUE  - Action is used for the Egress Policy
*                          GT_FALSE - Action is used for the Ingress Policy
*           modifyDscp   - For Ingress Policy:
*                          The Modify DSCP QoS attribute of the packet.
*                          Enables modification of the packet's DSCP field.
*                          Only relevant if QoS precedence of the previous
*                          QoS assignment mechanisms (Port, Protocol Based
*                          QoS, and previous matching rule) is Soft.
*                          Relevant for IPv4/IPv6 packets, only.
*                          ModifyDSCP enables the following:
*                          - Keep previous DSCP modification command.
*                          - Enable modification of the DSCP field in
*                            the packet.
*                          - Disable modification of the DSCP field in
*                            the packet.
*
*                          For Egress Policy:
*                          Enables modification of the IP Header DSCP field
*                          to egress.dscp of the transmitted packet.
*                          ModifyDSCP enables the following:
*                          - Keep previous packet DSCP setting.
*                          - Modify the Packet's DSCP to <egress.dscp>.
*
*           modifyUp     - For Ingress Policy:
*                          The Modify UP QoS attribute of the packet.
*                          Enables modification of the packet's
*                          802.1p User Priority field.
*                          Only relevant if QoS precedence of the previous
*                          QoS assignment mechanisms  (Port, Protocol Based
*                          QoS, and previous matching rule) is Soft.
*                          ModifyUP enables the following:
*                          - Keep previous QoS attribute <ModifyUP> setting.
*                          - Set the QoS attribute <modifyUP> to 1.
*                          - Set the QoS attribute <modifyUP> to 0.
*
*                          For Egress Policy:
*                          Enables the modification of the 802.1p User
*                          Priority field to egress.up of packet
*                          transmitted with a VLAN tagged.
*                          ModifyUP enables the following:
*                          - Keep previous QoS attribute <ModifyUP> setting.
*                          - Modify the Packet's UP to <egress.up>.
*
*           egress.dscp  - The DSCP field set to the transmitted packets.
*                          Relevant for Egress Policy only.
*           egress.up    - The 802.1p UP field set to the transmitted packets.
*                          Relevant for Egress Policy only.
*        ingress members:
*           profileIndex - The QoS Profile Attribute of the packet.
*                          Only relevant if the QoS precedence of the
*                          previous QoS Assignment Mechanisms (Port,
*                          Protocol Based QoS, and previous matching rule)
*                          is Soft and profileAssignIndex is set
*                          to GT_TRUE.the QoSProfile is used to index the
*                          QoSProfile to QoS Table Entry<n> (0<=n<72)
*                          and map the QoS Parameters for the packet,
*                          which are TC, DP, UP and DSCP
*                          Valid Range - 0 through 71
*           profileAssignIndex
*                        - Enable marking of QoS Profile Attribute of
*                          the packet.
*                          GT_TRUE - Assign <profileIndex> to the packet.
*                          GT_FALSE - Preserve previous QoS Profile setting.
*           profilePrecedence
*                        - Marking of the QoSProfile Precedence.
*                          Setting this bit locks the QoS parameters setting
*                          from being modified by subsequent QoS
*                          assignment engines in the ingress pipe.
*                          QoSPrecedence enables the following:
*                          - QoS precedence is soft and the packet's QoS
*                            parameters may be overridden by subsequent
*                            QoS assignment engines.
*                          - QoS precedence is hard and the packet's QoS
*                            parameters setting is performed until
*                            this stage is locked. It cannot be overridden
*                            by subsequent QoS assignment engines.
*      redirect     -  packet Policy redirection configuration
*           redirectCmd  - redirection command
*
*           data.outIf   - out interface redirection data
*                          relevant for CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_IF_E
*                          packet redirected to output interface
*           outInterface - output interface (port, trunk, VID, VIDX)
*           vntL2Echo    - Enables Virtual Network Tester Layer 2 Echo
*                          GT_TRUE - swap MAC SA and MAC DA in redirected packet
*                          GT_FALSE - don't swap MAC SA and MAC DA
*           tunnelStart  - Indicates this action is a Tunnel Start point
*                          GT_TRUE - Packet is redirected to an Egress Interface
*                                    and is tunneled as an  Ethernet-over-MPLS.
*                          GT_FALSE - Packet is redirected to an Egress
*                                     Interface and is not tunneled.
*           tunnelPtr    - the pointer to "Tunnel-start" entry
*                          (relevant only if tunnelStart == GT_TRUE)
*
*           data.routerLttIndex
*                        - index of IP router Lookup Translation Table entry
*                          relevant for CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E
*      policer      -  packet Policing configuration
*           policerEnable
*                        - GT_TRUE - policer enable,
*                          GT_FALSE - policer disable
*           policerId    - policers table entry index
*      vlan         -  packet VLAN modification configuration
*           egressTaggedModify - the field deleted
*                        - VLAN ID Modification either for Egress or Ingress
*                          Policy
*                        - GT_FALSE - Ingress Policy.
*                                     Use <modifyVlan> command,
*                        - GT_TRUE  - Egress Policy.
*                                     Use VLAN ID assignment to tagged packets.
*                                     (for Ingress Policy this is
*                                     ASSIGN_FOR_UNTAGGED_E command)
*           modifyVlan   - VLAN id modification command
*
*           nestedVlan   - When this field is set to GT_TRUE, this rule matching
*                          flow is defined as an access flow. The VID of all
*                          packets received on this flow is discarded and they
*                          are assigned with a VID using the device's VID
*                          assignment algorithms, as if they are untagged. When
*                          a packet received on an access flow is transmitted
*                          via a core port or a Cascading port, a VLAN tag is
*                          added to the packet (on top of the existing VLAN tag,
*                          if any). The VID field is the VID assigned to the
*                          packet as a result of all VLAN assignment algorithms.
*                          The 802.1p User Priority field of this added tag may
*                          be one of the following, depending on the ModifyUP
*                          QoS parameter set to the packet at the end of the
*                          Ingress pipe:
*                          - If ModifyUP is GT_TRUE, it is the UP extracted
*                            from the QoSProfile to QoS Table Entry<n>
*                          - If ModifyUP is GT_FALSE, it is the original packet
*                            802.1p User Priority field if it is tagged and is
*                            UP if the original packet is untagged.
*
*           vlanId       - VLAN id used for VLAN id modification if command
*                          not CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E or
*                          egressTaggedModify = GT_TRUE
*           precedence   - The VLAN Assignment precedence for the subsequent
*                          VLAN assignment mechanism, which is the Policy engine
*                          next policy-pass rule. Only relevant if the
*                          VID precedence set by the previous VID assignment
*                          mechanisms (Port, Protocol Based VLANs, and previous
*                          matching rule) is Soft.
*                          - Soft precedence The VID assignment can be
*                            overridden by the subsequent VLAN assignment
*                            mechanism, which is the Policy engine.
*                          - Hard precedence The VID assignment is locked to the last
*                            VLAN assigned to the packet and cannot be overridden.
*      ipUcRoute    -  special 98DX2X3 and 98DX1x7 Ip Unicast Route action
*           doIpUcRoute  - Configure IP Unicast Routing Actions
*                          GT_TRUE - the action used for IP unicast routing
*                          GT_FALSE - the action is not used for IP unicast
*                                     routing, all data of the structure
*                                      ignored.
*           arpDaIndex   - Route Entry ARP Index to the ARP Table (10 bit)
*           decrementTTL - Decrement IPv4 <TTL> or IPv6 <Hop Limit> enable
*                          GT_TRUE - TTL Decrement for routed packets is enabled
*                          GT_FALSE - TTL Decrement for routed packets is disabled
*           bypassTTLCheck
*                        - Bypass Router engine TTL and Options Check
*                          GT_TRUE - the router engine bypasses the
*                                    IPv4 TTL/Option check and the IPv6 Hop
*                                    Limit/Hop-by-Hop check. This is used for
*                                    IP-TO-ME host entries, where the packet
*                                    is destined to this device
*                          GT_FALSE - the check is not bypassed
*           icmpRedirectCheck
*                        - ICMP Redirect Check Enable
*                          GT_TRUE - the router engine checks if the next hop
*                                    VLAN is equal to the packet VLAN
*                                    assignment, and if so, the packet is
*                                    mirrored to the CPU, in order to send an
*                                    ICMP Redirect message back to the sender.
*                          GT_FALSE - the check disabled parameters
*                                     configuration.
*
*
* Comments:
*
*   - for 98DX2X5 - full support,
*   - for 98DX2x3,98DX2x0,98DX2x7, 98DX1x6, 98DX1x7 and so on supports
*     Ingress direction only with 32 ports per device and the
*     non-local-device-entries-segment-base == 0
*
*
*/
/*******************************************************************************
* pg_wrap_cpssDxChPclRuleActionUpdate(table command)
*
* DESCRIPTION:
*   The function updates the RULE Action
*
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See pg_wrap_cpssDxChPclRuleSet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleActionUpdate

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PCL_RULE_SIZE_ENT      ruleSize;
    GT_U32                      ruleIndex;
    CPSS_DXCH_PCL_ACTION_STC    action;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* to support the not seted/geted members of structures */
    cpssOsMemSet(&action, 0, sizeof(action));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_PCL_RULE_SIZE_ENT)inArgs[1];
    ruleIndex = (GT_U32)inArgs[2];


    actionRetrieve(inFields, 0, &action);

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclRuleActionUpdate(devNum, ruleSize, ruleIndex,
                                                          &action);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* pg_wrap_cpssDxChPclRuleActionGet
*
* DESCRIPTION:
*   The function gets the Rule Action
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See pg_wrap_cpssDxChPclRuleSet.
*       direction       - Policy direction:
*                         Ingress or Egress
*                         Needed for parsing
*
* OUTPUTS:
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleActionGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PCL_RULE_SIZE_ENT      ruleSize;
    GT_U32                      ruleIndex;
    CPSS_PCL_DIRECTION_ENT      direction;
    CPSS_DXCH_PCL_ACTION_STC    action;
    GT_U8  __Dev,__Port; /* Dummy for converting. */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* to support the not seted/geted members of structures */
    cpssOsMemSet(&action, 0, sizeof(action));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_PCL_RULE_SIZE_ENT)inArgs[1];
    ruleIndex = (CPSS_PCL_DIRECTION_ENT)inArgs[2];
    direction = (GT_U32)inArgs[3];


    result = pg_wrap_cpssDxChPclRuleActionGet(devNum, ruleSize, ruleIndex,
                                            direction, &action);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = action.pktCmd;
    inFields[1] = action.mirror.cpuCode;
    inFields[2] = action.mirror.mirrorToRxAnalyzerPort;
    inFields[3] = action.matchCounter.enableMatchCount;
    inFields[4] = action.matchCounter.matchCounterIndex;
    inFields[5] = action.egressPolicy;
    inFields[6] = action.qos.modifyDscp;
    inFields[7] = action.qos.modifyUp;
    inFields[8] = action.qos.qos.ingress.profileIndex;
    inFields[9] = action.qos.qos.ingress.profileAssignIndex;
    inFields[10] = action.qos.qos.ingress.profilePrecedence;
    inFields[11] = action.qos.qos.egress.dscp;
    inFields[12] = action.qos.qos.egress.up;
    inFields[13] = action.redirect.data.outIf.outInterface.type;
    __Dev  = action.redirect.data.outIf.outInterface.devPort.devNum;
    __Port = action.redirect.data.outIf.outInterface.devPort.portNum;

    CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev, __Port) ;
    inFields[15] = __Dev  ;
    inFields[16] = __Port ;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(action.redirect.data.outIf.outInterface.trunkId);
    inFields[17] = action.redirect.data.outIf.outInterface.trunkId;
    inFields[18] = action.redirect.data.outIf.outInterface.vidx;
    inFields[19] = action.redirect.data.outIf.outInterface.vlanId;
    inFields[20] = action.redirect.data.outIf.vntL2Echo;
    inFields[21] = action.redirect.data.outIf.tunnelStart;
    inFields[22] = action.redirect.data.outIf.tunnelPtr;
    inFields[23] = action.redirect.data.routerLttIndex;
    inFields[24] = action.policer.policerEnable;
    inFields[25] = action.policer.policerId;
    inFields[26] = GT_FALSE; /* actionPtr.vlan.egressTaggedModify - this field does not exist anymore */
    inFields[27] = action.vlan.modifyVlan;
    inFields[28] = action.vlan.nestedVlan;
    inFields[29] = action.vlan.vlanId;
    inFields[30] = action.vlan.precedence;
    inFields[31] = action.ipUcRoute.doIpUcRoute;
    inFields[32] = action.ipUcRoute.arpDaIndex;
    inFields[33] = action.ipUcRoute.decrementTTL;
    inFields[34] = action.ipUcRoute.bypassTTLCheck;
    inFields[35] = action.ipUcRoute.icmpRedirectCheck;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],  inFields[6],
                inFields[7],  inFields[8],  inFields[9],  inFields[10],
                inFields[11], inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17], inFields[18],
                inFields[19], inFields[20], inFields[21], inFields[22],
                inFields[23], inFields[24], inFields[25], inFields[26],
                inFields[27], inFields[28], inFields[29], inFields[30],
                inFields[31], inFields[32], inFields[33], inFields[34],
                inFields[35]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* pg_wrap_cpssDxChPclRuleInvalidate
*
* DESCRIPTION:
*   The function invalidates the Policy Rule
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssExMxPclRuleSet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleInvalidate

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                        devNum;
    CPSS_PCL_RULE_SIZE_ENT       ruleSize;
    GT_U32                       ruleIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_PCL_RULE_SIZE_ENT)inArgs[1];
    ruleIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclRuleInvalidate(devNum, ruleSize, ruleIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* pg_wrap_cpssDxChPclRuleCopy
*
* DESCRIPTION:
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first pg_wrap_cpssDxChPclRuleCopy should be
*  called. And after this cpssExMxPclRuleInvalidate should be used to invalidate
*  Rule in old position
*
*
* INPUTS:
*       devNum           - device number
*       ruleSize         - size of Rule.
*       ruleSrcIndex     - index of the rule in the TCAM from which pattern,
*                           mask and action are taken.
*       ruleDstIndex     - index of the rule in the TCAM to which pattern,
*                           mask and action are placed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleCopy

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                        devNum;
    CPSS_PCL_RULE_SIZE_ENT       ruleSize;
    GT_U32                       ruleSrcIndex;
    GT_U32                       ruleDstIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_PCL_RULE_SIZE_ENT)inArgs[1];
    ruleSrcIndex = (GT_U32)inArgs[2];
    ruleDstIndex = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclRuleCopy(devNum, ruleSize, ruleSrcIndex, ruleDstIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPclRuleMatchCounterGet
*
* DESCRIPTION:
*       Get or get and reset rule matching counters depending on the reset
*       parameter.
*
* INPUTS:
*       devNum       - Device number.
*       counterIndex - Counter Id
*
* OUTPUTS:
*       counterPtr   - pointer to the counter value.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - otherwize
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleMatchCounterGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     devNum;
    GT_U32    counterIndex;
    GT_U32    counterPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    counterIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPclRuleMatchCounterGet(devNum, counterIndex, &counterPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", counterPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPclRuleMatchCounterSet
*
* DESCRIPTION:
*       Set rule matching counters.
*       To reset value of counter use counterValue = 0
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum       - Device number.
*       counterIndex - Counter Index (0..31)
*       counterValue - counter value.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleMatchCounterSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     devNum;
    GT_U32    counterIndex;
    GT_U32    counterValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    counterIndex = (GT_U32)inArgs[1];
    counterValue = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPclRuleMatchCounterSet(devNum, counterIndex, counterValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPclPortIngressPolicyEnable
*
* DESCRIPTION:
*    Enables/disables ingress policy per port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*    enable          - GT_TRUE - Ingress Policy enable, GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclPortIngressPolicyEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_U8      port;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChPclPortIngressPolicyEnable(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPclPortIngressPolicyEnableGet
*
* DESCRIPTION:
*    Get the Enable/Disable ingress policy status per port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*
* OUTPUTS:
*    enable          - Pointer to ingress policy status.
*                      GT_TRUE - Ingress Policy is enabled.
*                      GT_FALSE - Ingress Policy is disabled.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_FAIL                  - otherwise.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclPortIngressPolicyEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                    devNum;
    GT_U8                    port;
    GT_BOOL                  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPclPortIngressPolicyEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPclPortLookupCfgTabAccessModeSet
*
* APPLICABLE DEVICES:  All DxCh devices
*
* DESCRIPTION:
*    Configures VLAN/PORT access mode to Ingress or Egress PCL
*    configuration table perlookup.
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*    direction       - Policy direction:
*                      Ingress or Egress
*    lookupNum       - Lookup number:
*                      lookup0 or lookup1
*    mode            - PCL Configuration Table access mode
*                      Port or VLAN ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*
* COMMENTS:
*   - for 98DX2X5 - full support,
*   - for 98DX2x3,98DX2x0,98DX2x7, 98DX1x6, 98DX1x7 and so on supports
*     Ingress direction only and set same access type
*     for both lookups regardless the <lookupNum> parameter value
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclPortLookupCfgTabAccessModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                         result;

    GT_U8                                             devNum;
    GT_U8                                             port;
    CPSS_PCL_DIRECTION_ENT                            direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT                        lookupNum;
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT    mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[2];
    lookupNum = (CPSS_PCL_LOOKUP_NUMBER_ENT)inArgs[3];
    mode = (CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChPclPortLookupCfgTabAccessModeSet(
        devNum, port, direction, lookupNum,
        0 /*subLookupNum*/, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPclPortLookupCfgTabAccessModeSet_V1
*
* APPLICABLE DEVICES:  All DxCh devices
*
* DESCRIPTION:
*    Configures VLAN/PORT access mode to Ingress or Egress PCL
*    configuration table perlookup.
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*    direction       - Policy direction:
*                      Ingress or Egress
*    lookupNum       - Lookup number:
*                      lookup0 or lookup1
*    subLookupNum    - sub lookup number for lookup.
*                      Supported only for  DxChXCat and above devices.
*                      Ignored for other devices.
*                      DxChXCat devices supports two sub lookups only for
*                      ingress lookup 0.
*                      All other lookups have not sub lookups.
*    mode            - PCL Configuration Table access mode
*                      Port or VLAN ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*
* COMMENTS:
*   - for CH2 and above- full support,
*   - for CH1 supports
*     Ingress direction only and set same access type
*     for both lookups regardless the <lookupNum> parameter value
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclPortLookupCfgTabAccessModeSet_V1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                         result;

    GT_U8                                             devNum;
    GT_U8                                             port;
    CPSS_PCL_DIRECTION_ENT                            direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT                        lookupNum;
    GT_U32                                            subLookupNum;
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT    mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum       = (GT_U8)inArgs[0];
    port         = (GT_U8)inArgs[1];
    direction    = (CPSS_PCL_DIRECTION_ENT)inArgs[2];
    lookupNum    = (CPSS_PCL_LOOKUP_NUMBER_ENT)inArgs[3];
    subLookupNum = (GT_U32)inArgs[4];
    mode         = (CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT)inArgs[5];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChPclPortLookupCfgTabAccessModeSet(
        devNum, port, direction, lookupNum,
        subLookupNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/* Table: cpssDxChPclCfgTblAccessMode
*
* Description:
*     PCL Configuration table
*
* Fields:
*    ipclAccMode     - Ingress Policy local/non-local mode
*    ipclMaxDevPorts - Ingress Policy support 32/64 port remote devices
*    ipclDevPortBase - Ingress Policy Configuration table access base
*    epclAccMode     - Egress PCL local/non-local mode
*    epclMaxDevPorts - Egress PCL support 32/64 port remote devices
*    epclDevPortBase - Egress PCL Configuration table access base
*
*
* Comments:
*
*   - for 98DX2X5 - full support,
*   - for 98DX2x3,98DX2x0,98DX2x7, 98DX1x6, 98DX1x7 and so on supports
*     Ingress direction only with 32 ports per device and the
*     non-local-device-entries-segment-base == 0
*
*
*/

/*******************************************************************************
* cpssDxChPclCfgTblAccessModeSet(table command)
*
* DESCRIPTION:
*    Configures global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*    devNum          - device number
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclCfgTblAccessModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                                  result;
    GT_U8                                      devNum;
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC      accModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];


    accModePtr.ipclAccMode = (CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT)inFields[0];
    accModePtr.ipclMaxDevPorts =
                           (CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT)inFields[1];
    accModePtr.ipclDevPortBase =
                           (CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT)inFields[2];
    accModePtr.epclAccMode = (CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_ENT)inFields[3];
    accModePtr.epclMaxDevPorts =
                           (CPSS_DXCH_PCL_CFG_TBL_MAX_DEV_PORTS_ENT)inFields[4];
    accModePtr.epclDevPortBase =
                           (CPSS_DXCH_PCL_CFG_TBL_DEV_PORT_BASE_ENT)inFields[5];


    /* call cpss api function */
    result = cpssDxChPclCfgTblAccessModeSet(devNum, &accModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPclCfgTblAccessModeGet(table command)
*
* DESCRIPTION:
*    Get global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*    devNum          - device number
*
* OUTPUTS:
*    accModePtr      - global configuration of access mode
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclCfgTblAccessModeGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                                  result;
    GT_U8                                      devNum;
    CPSS_DXCH_PCL_CFG_TBL_ACCESS_MODE_STC      accModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];


    result = cpssDxChPclCfgTblAccessModeGet(devNum, &accModePtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = accModePtr.ipclAccMode;
    inFields[1] = accModePtr.ipclMaxDevPorts;
    inFields[2] = accModePtr.ipclDevPortBase;
    inFields[3] = accModePtr.epclAccMode;
    inFields[4] = accModePtr.epclMaxDevPorts;
    inFields[5] = accModePtr.epclDevPortBase;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                inFields[3], inFields[4], inFields[5]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/* Table: cpssDxChPclCfgTbl
*
* Description:
*     PCL Configuration table
*
* Fields:
*    enableLookup           - Enable Lookup
*                             GT_TRUE - enable Lookup
*                             GT_FALSE - disable Lookup
*    pclId                  - PCL-ID bits in the TCAM search key.
*    groupKeyTypes.nonIpKey - type of TCAM search key for NON-IP packets.
*    groupKeyTypes.ipv4Key  - type of TCAM search key for IPV4 packets
*    groupKeyTypes.ipv6Key  - type of TCAM search key for IPV6 packets
*
*
* Comments:
*
*       For 98DX2X5 - full support (beside the Stadard IPV6 DIP key)
*       For 98DX2X3 - ingerss only, keys for not-IP, IPV4 and IPV6 must
*           be of the same size, Stadard IPV6 DIP key allowed only on lookup1
*
*
*/

/*******************************************************************************
* pg_wrap_cpssDxChPclCfgTblSet(table command)
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: either port or VLAN
*          devPort - info about the {dev,port} - relevant to CPSS_INTERFACE_PORT_E
*             devNum - the device number
*             portNum - the port number
*          trunkId - info about the {trunkId} - relevant to CPSS_INTERFACE_TRUNK_E
*          vidx   -  info about the {vidx} - relevant to CPSS_INTERFACE_VIDX_E
*          vlanId -  info about the {vid}  - relevant to CPSS_INTERFACE_VID_E
*
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
* COMMENTS:
*
*
*********************************************************************************/
static CMD_STATUS wrCpssDxChPclCfgTblSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_INTERFACE_INFO_STC         interfaceInfoPtr;
    CPSS_PCL_DIRECTION_ENT          direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfgPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* to support the not seted/geted members of structures */
    cpssOsMemSet(&interfaceInfoPtr, 0, sizeof(interfaceInfoPtr));
    cpssOsMemSet(&lookupCfgPtr, 0, sizeof(lookupCfgPtr));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    interfaceInfoPtr.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[1];

    switch (inArgs[1])
    {
    case 0:
        interfaceInfoPtr.devPort.devNum = (GT_U8)inArgs[2];
        interfaceInfoPtr.devPort.portNum = (GT_U8)inArgs[3];
        CONVERT_DEV_PORT_DATA_MAC(interfaceInfoPtr.devPort.devNum ,
                             interfaceInfoPtr.devPort.portNum);
        break;

    case 1:
        interfaceInfoPtr.trunkId = (GT_TRUNK_ID)inArgs[4];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfoPtr.trunkId);
        break;

    case 2:
        interfaceInfoPtr.vidx = (GT_U16)inArgs[5];
        break;

    case 3:
        interfaceInfoPtr.vlanId = (GT_U16)inArgs[6];
        break;

    default:
        break;
    }

    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[7];
    lookupNum = (CPSS_PCL_LOOKUP_NUMBER_ENT)inArgs[8];


    lookupCfgPtr.enableLookup = (GT_BOOL)inFields[0];
    lookupCfgPtr.pclId = (GT_U32)inFields[1];
    lookupCfgPtr.groupKeyTypes.nonIpKey =
                                (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inFields[2];
    lookupCfgPtr.groupKeyTypes.ipv4Key =
                                (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inFields[3];
    lookupCfgPtr.groupKeyTypes.ipv6Key =
                                (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inFields[4];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclCfgTblSet(devNum, &interfaceInfoPtr, direction,
                                              lookupNum, &lookupCfgPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* pg_wrap_cpssDxChPclCfgTblGet(table command)
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: either port or VLAN
*          devPort - info about the {dev,port} - relevant to CPSS_INTERFACE_PORT_E
*             devNum - the device number
*             portNum - the port number
*          trunkId - info about the {trunkId} - relevant to CPSS_INTERFACE_TRUNK_E
*          vidx   -  info about the {vidx} - relevant to CPSS_INTERFACE_VIDX_E
*          vlanId -  info about the {vid}  - relevant to CPSS_INTERFACE_VID_E
*
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*
* OUTPUTS:
*       lookupCfgPtr      - lookup configuration
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclCfgTblGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_INTERFACE_INFO_STC         interfaceInfoPtr;
    CPSS_PCL_DIRECTION_ENT          direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfgPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* to support the not seted/geted members of structures */
    cpssOsMemSet(&interfaceInfoPtr, 0, sizeof(interfaceInfoPtr));
    cpssOsMemSet(&lookupCfgPtr, 0, sizeof(lookupCfgPtr));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    interfaceInfoPtr.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[1];

    switch (inArgs[1])
    {
        case 0:
            interfaceInfoPtr.devPort.devNum = (GT_U8)inArgs[2];
            interfaceInfoPtr.devPort.portNum = (GT_U8)inArgs[3];
            CONVERT_DEV_PORT_DATA_MAC(interfaceInfoPtr.devPort.devNum ,
                                 interfaceInfoPtr.devPort.portNum);
            break;

        case 1:
            interfaceInfoPtr.trunkId = (GT_TRUNK_ID)inArgs[4];
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfoPtr.trunkId);
            break;

        case 2:
            interfaceInfoPtr.vidx = (GT_U16)inArgs[5];
            break;

        case 3:
            interfaceInfoPtr.vlanId = (GT_U16)inArgs[6];
            break;

        default:
            break;
    }

    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[7];
    lookupNum = (CPSS_PCL_LOOKUP_NUMBER_ENT)inArgs[8];


    result = pg_wrap_cpssDxChPclCfgTblGet(devNum, &interfaceInfoPtr, direction,
                                              lookupNum, &lookupCfgPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = lookupCfgPtr.enableLookup;
    inFields[1] = lookupCfgPtr.pclId;
    inFields[2] = lookupCfgPtr.groupKeyTypes.nonIpKey;
    inFields[3] = lookupCfgPtr.groupKeyTypes.ipv4Key;
    inFields[4] = lookupCfgPtr.groupKeyTypes.ipv6Key;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                           inFields[3], inFields[4]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


/*****************Table:cpssDxCh3PclCfgTbl **************************************/

/*******************************************************************************
* pg_wrap_cpssDxChPclCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: either port or VLAN
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       For DxCh2 and above - full support (beside the Stadard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be of the same size, Stadard IPV6 DIP key allowed only on lookup1
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PclCfgTblSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_INTERFACE_INFO_STC         interfaceInfo;
    CPSS_PCL_DIRECTION_ENT          direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* to support the not seted/geted members of structures */
    cpssOsMemSet(&interfaceInfo, 0, sizeof(interfaceInfo));
    cpssOsMemSet(&lookupCfg, 0, sizeof(lookupCfg));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[1];

    switch (inArgs[1])
    {
        case 0:
            interfaceInfo.devPort.devNum = (GT_U8)inArgs[2];
            interfaceInfo.devPort.portNum = (GT_U8)inArgs[3];
            CONVERT_DEV_PORT_DATA_MAC(interfaceInfo.devPort.devNum ,
                                 interfaceInfo.devPort.portNum);
            break;

        case 1:
            interfaceInfo.trunkId = (GT_TRUNK_ID)inArgs[4];
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);
            break;

        case 2:
            interfaceInfo.vidx = (GT_U16)inArgs[5];
            break;

        case 3:
            interfaceInfo.vlanId = (GT_U16)inArgs[6];
            break;

        default:
            break;
    }

    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[7];
    lookupNum = (CPSS_PCL_LOOKUP_NUMBER_ENT)inArgs[8];


    lookupCfg.enableLookup = (GT_BOOL)inFields[0];
    lookupCfg.dualLookup   = (GT_BOOL)inFields[1];
    lookupCfg.pclId = (GT_U32)inFields[2];
    lookupCfg.groupKeyTypes.nonIpKey =
                                (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inFields[3];
    lookupCfg.groupKeyTypes.ipv4Key =
                                (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inFields[4];
    lookupCfg.groupKeyTypes.ipv6Key =
                                (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inFields[5];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclCfgTblSet(devNum, &interfaceInfo, direction,
                                              lookupNum, &lookupCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* pg_wrap_cpssDxChPclCfgTblGet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: either port or VLAN
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       lookupCfgPtr      - lookup configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be the same size, Standard IPV6 DIP key allowed only on lookup1
*
*********************************************************************************/
static CMD_STATUS wrCpssDxCh3PclCfgTblGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_INTERFACE_INFO_STC         interfaceInfo;
    CPSS_PCL_DIRECTION_ENT          direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* to support the not seted/geted members of structures */
    cpssOsMemSet(&interfaceInfo, 0, sizeof(interfaceInfo));
    cpssOsMemSet(&lookupCfg, 0, sizeof(lookupCfg));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[1];

    switch (inArgs[1])
    {
        case 0:
            interfaceInfo.devPort.devNum = (GT_U8)inArgs[2];
            interfaceInfo.devPort.portNum = (GT_U8)inArgs[3];
            CONVERT_DEV_PORT_DATA_MAC(interfaceInfo.devPort.devNum ,
                                 interfaceInfo.devPort.portNum);
            break;

        case 1:
            interfaceInfo.trunkId = (GT_TRUNK_ID)inArgs[4];
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);
            break;

        case 2:
            interfaceInfo.vidx = (GT_U16)inArgs[5];
            break;

        case 3:
            interfaceInfo.vlanId = (GT_U16)inArgs[6];
            break;

        default:
            break;
    }

    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[7];
    lookupNum = (CPSS_PCL_LOOKUP_NUMBER_ENT)inArgs[8];


    result = pg_wrap_cpssDxChPclCfgTblGet(devNum, &interfaceInfo, direction,
                                              lookupNum, &lookupCfg);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = lookupCfg.enableLookup;
    inFields[1] = lookupCfg.dualLookup;
    inFields[2] = lookupCfg.pclId;
    inFields[3] = lookupCfg.groupKeyTypes.nonIpKey;
    inFields[4] = lookupCfg.groupKeyTypes.ipv4Key;
    inFields[5] = lookupCfg.groupKeyTypes.ipv6Key;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                           inFields[3], inFields[4],inFields[5]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChXCatPclCfgTblSet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:  All XCAT devices
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: either port or VLAN
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       For DxCh2 and above - full support (beside the Stadard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be of the same size, Stadard IPV6 DIP key allowed only on lookup1
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChXCatPclCfgTblSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_INTERFACE_INFO_STC         interfaceInfo;
    CPSS_PCL_DIRECTION_ENT          direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* to support the not seted/geted members of structures */
    cpssOsMemSet(&interfaceInfo, 0, sizeof(interfaceInfo));
    cpssOsMemSet(&lookupCfg, 0, sizeof(lookupCfg));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[1];

    switch (inArgs[1])
    {
        case 0:
            interfaceInfo.devPort.devNum = (GT_U8)inArgs[2];
            interfaceInfo.devPort.portNum = (GT_U8)inArgs[3];
            CONVERT_DEV_PORT_DATA_MAC(interfaceInfo.devPort.devNum ,
                                 interfaceInfo.devPort.portNum);
            break;

        case 1:
            interfaceInfo.trunkId = (GT_TRUNK_ID)inArgs[4];
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);
            break;

        case 2:
            interfaceInfo.vidx = (GT_U16)inArgs[5];
            break;

        case 3:
            interfaceInfo.vlanId = (GT_U16)inArgs[6];
            break;

        case 4:
            interfaceInfo.index  = (GT_U32)inArgs[7];
            break;

        default:
            break;
    }

    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[7];
    lookupNum = (CPSS_PCL_LOOKUP_NUMBER_ENT)inArgs[8];


    lookupCfg.enableLookup = (GT_BOOL)inFields[0];
    lookupCfg.dualLookup   = (GT_BOOL)inFields[1];
    lookupCfg.pclId        = (GT_U32)inFields[2];
    lookupCfg.pclIdL01      = (GT_U32)inFields[3];
    lookupCfg.groupKeyTypes.nonIpKey =
        (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inFields[4];
    lookupCfg.groupKeyTypes.ipv4Key =
        (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inFields[5];
    lookupCfg.groupKeyTypes.ipv6Key =
        (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inFields[6];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclCfgTblSet(devNum, &interfaceInfo, direction,
                                              lookupNum, &lookupCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChXCatPclCfgTblGet
*
* DESCRIPTION:
*    PCL Configuration table entry's lookup configuration for interface.
*
* APPLICABLE DEVICES:  All XCAT devices
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: either port or VLAN
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       lookupCfgPtr      - lookup configuration
*
* OUTPUTS:
*       lookupCfgPtr      - lookup configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*       For DxCh2 and above - full support (beside the Standard IPV6 DIP key)
*       For DxCh1 - ingress only, keys for not-IP, IPV4 and IPV6 must
*           be the same size, Standard IPV6 DIP key allowed only on lookup1
*
*********************************************************************************/
static CMD_STATUS wrCpssDxChXCatPclCfgTblGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_INTERFACE_INFO_STC         interfaceInfo;
    CPSS_PCL_DIRECTION_ENT          direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT      lookupNum;
    CPSS_DXCH_PCL_LOOKUP_CFG_STC    lookupCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* to support the not seted/geted members of structures */
    cpssOsMemSet(&interfaceInfo, 0, sizeof(interfaceInfo));
    cpssOsMemSet(&lookupCfg, 0, sizeof(lookupCfg));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[1];

    switch (inArgs[1])
    {
        case 0:
            interfaceInfo.devPort.devNum = (GT_U8)inArgs[2];
            interfaceInfo.devPort.portNum = (GT_U8)inArgs[3];
            CONVERT_DEV_PORT_DATA_MAC(interfaceInfo.devPort.devNum ,
                                 interfaceInfo.devPort.portNum);
            break;

        case 1:
            interfaceInfo.trunkId = (GT_TRUNK_ID)inArgs[4];
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceInfo.trunkId);
            break;

        case 2:
            interfaceInfo.vidx = (GT_U16)inArgs[5];
            break;

        case 3:
            interfaceInfo.vlanId = (GT_U16)inArgs[6];
            break;

        case 4:
            interfaceInfo.index  = (GT_U32)inArgs[7];
            break;

        default:
            break;
    }

    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[7];
    lookupNum = (CPSS_PCL_LOOKUP_NUMBER_ENT)inArgs[8];


    result = pg_wrap_cpssDxChPclCfgTblGet(devNum, &interfaceInfo, direction,
                                              lookupNum, &lookupCfg);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = lookupCfg.enableLookup;
    inFields[1] = lookupCfg.dualLookup;
    inFields[2] = lookupCfg.pclId;
    inFields[3] = lookupCfg.pclIdL01;
    inFields[4] = lookupCfg.groupKeyTypes.nonIpKey;
    inFields[5] = lookupCfg.groupKeyTypes.ipv4Key;
    inFields[6] = lookupCfg.groupKeyTypes.ipv6Key;


    /* pack and output table fields */
    fieldOutput(
        "%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
        inFields[3], inFields[4], inFields[5], inFields[6]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPclIngressPolicyEnable
*
* DESCRIPTION:
*    Enables Ingress Policy.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*    devNum           - device number
*    enable           - enable ingress policy
*                      GT_TRUE  - enable,
*                      GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclIngressPolicyEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPclIngressPolicyEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPclTcamRuleSizeModeSet
*
* DESCRIPTION:
*    Set TCAM Rules size mode.
*    The rules TCAM may be configured to contain short rules (24 bytes), long
*    rules (48 bytes), or a mix of short rules and long rules.
*    The rule size may be set by global configuration or may be defined
*    dynamical according to PCL Configuration table settings.
*
* APPLICABLE DEVICES: 98DX2x3,98DX2x0,98DX2x7, 98DX1x6, 98DX1x7  devices
*
* INPUTS:
*    devNum               - device number
*    mixedRuleSizeMode    - TCAM rule size mode
*                           GT_TRUE  - mixed Rule Size Mode, rule size defined
*                                      by PCL configuration table
*                           GT_FALSE - Rule size defined by ruleSize parameter
*    ruleSize             - Rule size for not mixed mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclTcamRuleSizeModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8                     devNum;
    GT_BOOL                   mixedRuleSizeMode;
    CPSS_PCL_RULE_SIZE_ENT    ruleSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mixedRuleSizeMode = (GT_BOOL)inArgs[1];
    ruleSize = (CPSS_PCL_RULE_SIZE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPclTcamRuleSizeModeSet(devNum, mixedRuleSizeMode,
                                                             ruleSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPclTwoLookupsCpuCodeResolution
*
* DESCRIPTION:
*    Resolve the result CPU Code if both lookups has action commands
*    are either both TRAP or both MIRROR To CPU
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*    devNum               - device number
*    lookupNum            - lookup number from which the CPU Code is selected
*                           when action commands are either both TRAP or
*                           both MIRROR To CPU
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclTwoLookupsCpuCodeResolution

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    lookupNum = (CPSS_PCL_LOOKUP_NUMBER_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPclTwoLookupsCpuCodeResolution(devNum, lookupNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPclInvalidUdbCmdSet
*
* DESCRIPTION:
*    Set the command that is applied when the policy key <User-Defined>
*    field cannot be extracted from the packet due to lack of header
*    depth (i.e, the field resides deeper than 128 bytes into the packet).
*    This command is NOT applied when the policy key <User-Defined>
*    field cannot be extracted due to the offset being relative to a layer
*    start point that does not exist in the packet. (e.g. the offset is relative
*    to the IP header but the packet is a non-IP).
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*    devNum         - device number
*    udbErrorCmd    - command applied to a packet:
*                      continue lookup, trap to CPU, hard drop or soft drop
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclInvalidUdbCmdSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    CPSS_DXCH_UDB_ERROR_CMD_ENT    udbErrorCmd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    udbErrorCmd = (CPSS_DXCH_UDB_ERROR_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPclInvalidUdbCmdSet(devNum, udbErrorCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPclIpLengthCheckModeSet
*
* DESCRIPTION:
*    Set the mode of checking IP packet length.
*    To determine if an IP packet is a valid IP packet, one of the checks is a
*    length check to find out if the total IP length field reported in the
*    IP header is less or equal to the packet's length.
*    This function determines the check mode.
*    The results of this check sets the policy key <IP Header OK>.
*    The result is also used by the router engine to determine whether
*    to forward or trap/drop the packet.
*    There are two check modes:
*     1. For IPv4: ip_total_length <= packet's byte count
*        For IPv6: ip_total_length + 40 <= packet's byte count
*     2. For IPv4: ip_total_length + L3 Offset + 4 (CRC) <= packet's byte count,
*        For IPv6: ip_total_length + 40 +L3 Offset + 4 (CRC) <= packet's
*        byte count
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*    devNum           - device number
*    mode             - IP packet length checking mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclIpLengthCheckModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                 result;

    GT_U8                                     devNum;
    CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT    mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_DXCH_PCL_IP_LENGTH_CHECK_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPclIpLengthCheckModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* pg_wrap_cpssDxChPclRuleStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum         - device number
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleStateGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    GT_U32                    ruleIndex;
    GT_BOOL                   validPtr;
    CPSS_PCL_RULE_SIZE_ENT    ruleSizePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclRuleStateGet(devNum, ruleIndex, &validPtr,
                                                     &ruleSizePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", validPtr, ruleSizePtr);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPclPolicyRuleGetFirst
*
* DESCRIPTION:
*   The function gets the Policy Rule Mask, Pattern and Action in in te rowIndex
*   in Hw format.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum           - device number
*       ruleSize         - size of the Rule.
*       ruleIndex        - index of the rule in the TCAM.
*
* OUTPUTS:
*       mask             - rule mask          - 12  words.
*       pattern          - rule pattern       - 12  words.
*       action           - Policy rule action - 3 words.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclPolicyRuleGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PCL_RULE_SIZE_ENT      ruleSize;
    GT_U32                      ruleIndex;
    GT_U32                      maskPtr[18];
    GT_U32                      patternPtr[18];
    GT_U32                      actionPtr[4];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_PCL_RULE_SIZE_ENT)inArgs[1];
    ruleIndex = (GT_U32)inArgs[2];

    /* clean pattern and mask */
    cmdOsMemSet(maskPtr, 0, sizeof(maskPtr));
    cmdOsMemSet(patternPtr, 0, sizeof(patternPtr));

    result = pg_wrap_cpssDxChPclRuleGet(devNum, ruleSize, ruleIndex,
                                maskPtr, patternPtr, actionPtr);
    inFields[0] = maskPtr[0];
    inFields[1] = maskPtr[1];
    inFields[2] = maskPtr[2];
    inFields[3] = maskPtr[3];
    inFields[4] = maskPtr[4];
    inFields[5] = maskPtr[5];
    inFields[6] = maskPtr[6];
    inFields[7] = maskPtr[7];
    inFields[8] = maskPtr[8];
    inFields[9] = maskPtr[9];
    inFields[10] = maskPtr[10];
    inFields[11] = maskPtr[11];

    inFields[12] = patternPtr[0];
    inFields[13] = patternPtr[1];
    inFields[14] = patternPtr[2];
    inFields[15] = patternPtr[3];
    inFields[16] = patternPtr[4];
    inFields[17] = patternPtr[5];
    inFields[18] = patternPtr[6];
    inFields[19] = patternPtr[7];
    inFields[20] = patternPtr[8];
    inFields[21] = patternPtr[9];
    inFields[22] = patternPtr[10];
    inFields[23] = patternPtr[11];

    inFields[24] = actionPtr[0];
    inFields[25] = actionPtr[1];
    inFields[26] = actionPtr[2];


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* pg_wrap_cpssDxChPclCfgTblEntryGet
*
* DESCRIPTION:
*    Gets the PCL configuration table entry in Hw format.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*    devNum          - device number
*    direction       - Ingress Policy or Egress Policy
*                        (CPSS_PCL_DIRECTION_ENT member),
*    entryIndex      - PCL configuration table entry Index
*
* OUTPUTS:
*    pclCfgTblEntryPtr    - pcl Cfg Tbl Entry
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*       GT_BAD_STATE       - in case of already bound pclHwId with same
*                            combination of slot/direction
* COMMENTS:
*       for 98DX2X3 ingess direction only
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclCfgTblEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    CPSS_PCL_DIRECTION_ENT    direction;
    GT_U32                    entryIndex;
    GT_U32                    pclCfgTblEntry[4];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[1];
    entryIndex = (GT_U32)inArgs[2];

    pclCfgTblEntry[0] = 0;
    pclCfgTblEntry[1] = 0;
    pclCfgTblEntry[2] = 0;
    pclCfgTblEntry[3] = 0;

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclCfgTblEntryGet(
        devNum, direction,
        CPSS_PCL_LOOKUP_0_E,
        entryIndex,
        &(pclCfgTblEntry[0]));

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%X%X%X%X",
        pclCfgTblEntry[0], pclCfgTblEntry[1],
        pclCfgTblEntry[2], pclCfgTblEntry[3]);
    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChPclLookupCfgPortListEnableSet
*
* DESCRIPTION:
*        The function enables/disables using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*    enable            - GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclLookupCfgPortListEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                      devNum;
    CPSS_PCL_DIRECTION_ENT                     direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT                 lookupNum;
    GT_U32                                     subLookupNum;
    GT_BOOL                                    enable;
    GT_STATUS                                  result;

    devNum       = (GT_U8)                      inArgs[0];
    direction    = (CPSS_PCL_DIRECTION_ENT)     inArgs[1];
    lookupNum    = (CPSS_PCL_LOOKUP_NUMBER_ENT) inArgs[2];
    subLookupNum = (GT_U32)                     inArgs[3];
    enable       = (GT_U32)                     inArgs[4];

    result = pg_wrap_cpssDxChPclLookupCfgPortListEnableSet(
        devNum, direction, lookupNum, subLookupNum, enable);

    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPclLookupCfgPortListEnableGet
*
* DESCRIPTION:
*        The function gets enable/disable state of
*        using port-list in search keys.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress CPSS_PCL_LOOKUP_0_E
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
*
* OUTPUTS:
*    enablePtr         - (pointer to)
*                        GT_TRUE  - enable port-list in search key
*                        GT_FALSE - disable port-list in search key
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclLookupCfgPortListEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                      devNum;
    CPSS_PCL_DIRECTION_ENT                     direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT                 lookupNum;
    GT_U32                                     subLookupNum;
    GT_BOOL                                    enable;
    GT_STATUS                                  result;

    devNum       = (GT_U8)                      inArgs[0];
    direction    = (CPSS_PCL_DIRECTION_ENT)     inArgs[1];
    lookupNum    = (CPSS_PCL_LOOKUP_NUMBER_ENT) inArgs[2];
    subLookupNum = (GT_U32)                     inArgs[3];

    result = pg_wrap_cpssDxChPclLookupCfgPortListEnableGet(
        devNum, direction, lookupNum, subLookupNum, &enable);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    galtisOutput(
        outArgs, GT_OK, "%d", enable);

    return CMD_OK;
}

/* 98DX2x5 only API */

/*******************************************************************************
* cpssDxCh2PclEgressPolicyEnable
*
* DESCRIPTION:
*    Enables Egress Policy.
*
* APPLICABLE DEVICES: 98DX2x5 devices
*
* INPUTS:
*    devNum           - device number
*    enable           - enable Egress Policy
*                       GT_TRUE  - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh2PclEgressPolicyEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxCh2PclEgressPolicyEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxCh2EgressPclPacketTypesSet
*
* DESCRIPTION:
*   Enables/disables Egress PCL (EPCL) for set of packet types on port
*
* APPLICABLE DEVICES: 98DX2X5 devices
*
* INPUTS:
*   devNum        - device number
*   port          - port number
*   pktType       - packet type to enable/disable EPCL for it
*   enable        - enable EPCL for specific packet type
*                   GT_TRUE - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*
* COMMENTS:
*   - after reset EPCL disabled for all packet types on all ports
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh2EgressPclPacketTypesSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8                                devNum;
    GT_U8                                port;
    CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT    pktType;
    GT_BOOL                              enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    pktType = (CPSS_DXCH_PCL_EGRESS_PKT_TYPE_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxCh2EgressPclPacketTypesSet(devNum, port, pktType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* pg_wrap_cpssDxChPclTcpUdpPortComparatorSet
*
* DESCRIPTION:
*        Configure TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES: 98DX2X5 devices
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (0-7)
*    l4PortType        - TCP/UDP port type, source or destination port
*    compareOperator   - compare operator FALSE, LTE, GTE, NEQ
*    value             - 16 bit value to compare with
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh2PclTcpUdpPortComparatorSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8                              devNum;
    CPSS_PCL_DIRECTION_ENT             direction;
    CPSS_L4_PROTOCOL_ENT               l4Protocol;
    GT_U8                              entryIndex;
    CPSS_L4_PROTOCOL_PORT_TYPE_ENT     l4PortType;
    CPSS_COMPARE_OPERATOR_ENT          compareOperator;
    GT_U16                             value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[1];
    l4Protocol = (CPSS_L4_PROTOCOL_ENT)inArgs[2];
    entryIndex = (GT_U8)inArgs[3];
    l4PortType = (CPSS_L4_PROTOCOL_PORT_TYPE_ENT)inArgs[4];
    compareOperator = (CPSS_COMPARE_OPERATOR_ENT)inArgs[5];
    value = (GT_U16)inArgs[6];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclTcpUdpPortComparatorSet(devNum, direction, l4Protocol,
                                                        entryIndex, l4PortType,
                                                        compareOperator, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* pg_wrap_cpssDxChPclRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*   Switches rule-format specific functions according to
*   the field amonunt
*
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule.
*
*       ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case
*                          With reference to Standard and Extended rules
*                          indexes, the partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 1023.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 511.
*                          Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n <= 511
*                            is placed in the space of two standard rules with
*                            indexes n and n + 512.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    cpssOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));

    /* tho old version of this function switched specific functions */
    /* by amount of fields, the table resides only in this comment  */
    /* don't remove it                                              */
    /* 70 - wrCpssDxChPclRuleSet_STD_NOT_IP                         */
    /* 84 - wrCpssDxChPclRuleSet_STD_IP_L2_QOS                      */
    /* 88 - wrCpssDxChPclRuleSet_STD_IPV4_L4                        */
    /* 80 - wrCpssDxChPclRuleSet_STD_IPV6_DIP                       */
    /* 92 - wrCpssDxChPclRuleSet_EXT_NOT_IPV6                       */
    /* 90 - wrCpssDxChPclRuleSet_EXT_IPV6_L2                        */
    /* 86 - wrCpssDxChPclRuleSet_EXT_IPV6_L4                        */

    /* switch by rule format in inFields[1]               */
    /* egress formats not supported, ingress used instead */
    switch (inFields[1])
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
            return wrCpssDxChPclRuleSet_STD_NOT_IP(
                inArgs, inFields ,numFields ,outArgs);
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
            return wrCpssDxChPclRuleSet_STD_IP_L2_QOS(
                inArgs, inFields ,numFields ,outArgs);
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
            return wrCpssDxChPclRuleSet_STD_IPV4_L4(
                inArgs, inFields ,numFields ,outArgs);
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
            return wrCpssDxChPclRuleSet_STD_IPV6_DIP(
                inArgs, inFields ,numFields ,outArgs);
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
            return wrCpssDxChPclRuleSet_EXT_NOT_IPV6(
                inArgs, inFields ,numFields ,outArgs);
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
            return wrCpssDxChPclRuleSet_EXT_IPV6_L2(
                inArgs, inFields ,numFields ,outArgs);
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
            return wrCpssDxChPclRuleSet_EXT_IPV6_L4(
                inArgs, inFields ,numFields ,outArgs);
        default: return CMD_AGENT_ERROR;
    }
}

/*******************************************************************************
* pg_wrap_cpssDxChPclRuleValidStatusSet
*
* DESCRIPTION:
*     Validates/Invalidates the Policy rule.
*        The validation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Valid indication set.
*           The function does not check content of the rule before
*           write it back to TCAM
*        The invalidation of the rule is performed by next steps:
*        1. Retrieve the content of the rule from PP TCAM
*        2. Write content back to TCAM with Invalid indication set.
*        If the given the rule found already in needed valid state
*        no write done. If the given the rule found with size
*        different from the given rule-size an error code returned.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM.
*       valid           - new rule status: GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_STATE   - if in TCAM found rule of size different
*                        from the specified
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleValidStatusSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PCL_RULE_SIZE_ENT ruleSize;
    GT_U32 ruleIndex;
    GT_BOOL valid;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_PCL_RULE_SIZE_ENT)inArgs[1];
    ruleIndex = (GT_U32)inArgs[2];
    valid = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPclRuleValidStatusSet(devNum, ruleSize, ruleIndex, valid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPclEgressForRemoteTunnelStartEnableSet
*
* DESCRIPTION:
*       Enable/Disable the Egress PCL processing for the packets,
*       whith Tunnel Start on another device.
*       These packets are ingessed whith DSA Tag contains source port 60.
*
* APPLICABLE DEVICES: DxCh2 devices only
*
* INPUTS:
*       devNum         - device number
*       enable         - enable Remote Tunnel Start Packets Egress Pcl
*                        GT_TRUE  - enable
*                        GT_FALSE - disable
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclEgressForRemoteTunnelStartEnableSet
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
    result = cpssDxChPclEgressForRemoteTunnelStartEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPclEgressForRemoteTunnelStartEnableGet
*
* DESCRIPTION:
*       Gets Enable/Disable of the Egress PCL processing for the packets,
*       which Tunnel Start on another device.
*       These packets are ingessed with DSA Tag contains source port 60.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       enablePtr      - (pointer to) enable Remote Tunnel Start Packets Egress Pcl
*                        GT_TRUE  - enable
*                        GT_FALSE - disable
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclEgressForRemoteTunnelStartEnableGet
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
    result = cpssDxChPclEgressForRemoteTunnelStartEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet
*
* DESCRIPTION:
*       Sets Egress Policy Configuration Table Access Mode for
*       Tunnel Start packets
*
* APPLICABLE DEVICES: DxCh3 and above
*
* INPUTS:
*       devNum         - device number
*       cfgTabAccMode  - PCL Configuration Table access mode
*                        Port or VLAN ID
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT cfgTabAccMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum        = (GT_U8)inArgs[0];
    cfgTabAccMode = (CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT)inArgs[1];

    /* call cpss api function */
        result = cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet(
            devNum, cfgTabAccMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
*
* DESCRIPTION:
*       Gets Egress Policy Configuration Table Access Mode for
*       Tunnel Start packets
*
* APPLICABLE DEVICES: DxCh3 and above
*
* INPUTS:
*       devNum             - device number
* OUTPUTS:
*       cfgTabAccModePtr   - (pointer to) PCL Configuration Table access mode
*                            Port or VLAN ID
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PCL_PORT_LOOKUP_CFG_TAB_ACC_MODE_ENT cfgTabAccMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum        = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet(
        devNum, &cfgTabAccMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cfgTabAccMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PclTunnelTermForceVlanModeEnableSet
*
* DESCRIPTION:
*   The function enables/disables forcing of the PCL ID configuration
*   for all TT packets according to the VLAN assignment.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum         - device number
*       enable         - force TT packets assigned to PCL
*                        configuration table entry
*                        GT_TRUE  - By VLAN
*                        GT_FALSE - accordind to ingress port
*                                   per lookup settings
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PclTunnelTermForceVlanModeEnableSet
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
    result = cpssDxCh3PclTunnelTermForceVlanModeEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxCh3PclTunnelTermForceVlanModeEnableGet
*
* DESCRIPTION:
*   The function gets enable/disable of the forcing of the PCL ID configuration
*   for all TT packets according to the VLAN assignment.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       enablePtr      - (pointer to) force TT packets assigned to PCL
*                        configuration table entry
*                        GT_TRUE  - By VLAN
*                        GT_FALSE - accordind to ingress port
*                                   per lookup settings
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PclTunnelTermForceVlanModeEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable=GT_FALSE;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxCh3PclTunnelTermForceVlanModeEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*************table cpssDxCh3PclRuleAction********************/

/* global variables */

/*actionTable-wrapper level table*/

#define ACTION_TAB_SIZE 64

static struct
{
    CPSS_DXCH_PCL_ACTION_STC  actionEntry;
    GT_U32                      ruleIndex;
    GT_BOOL                     valid;
    GT_U32                      nextFree;
}actionTable[ACTION_TAB_SIZE];

static GT_U32   firstFree = 0;
static GT_BOOL  firstRun = GT_TRUE;
#define INVALID_RULE_INDEX 0x7FFFFFF0
/*******************************************************************************
* wrCpssDxChPclActionClear  (internal table command)
*
* DESCRIPTION:
*  Clears the wrapper level action table
*  The function initializes the actionTable entries and sets all entries invalid.
*  Done in wrapper level.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*       The use of this function is for first initialization and for clearing all
*       entries in the table.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PclActionClear
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32          index = 0;
    firstFree = 0;
    while (index < ACTION_TAB_SIZE)
    {
        actionTable[index].valid = GT_FALSE;
        actionTable[index].nextFree = index + 1;
        index++;
    }
    /* override*/
    actionTable[ACTION_TAB_SIZE - 1].nextFree = INVALID_RULE_INDEX;

    return CMD_OK;
}

/*******************************************************************************
* pclDxCh3ActionGet (table command)
*
* DESCRIPTION:
*  The function searches for action entry in internal wrapper table  by rule index
*  and returns the actionEntry.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       ruleIndex       - index of the rule
*
* OUTPUTS:
*       tableIndex     - index of entry in actionTable(wrappper level table) that
*                        contains ruleIndex
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
void pclDxCh3ActionGet
(
    IN  GT_U32                  ruleIndex,
    OUT GT_U32                  *tableIndex
)
{
    GT_U32              index;

    for (index = 0; (index < ACTION_TAB_SIZE); index++)
    {
        if (actionTable[index].ruleIndex != ruleIndex)
            continue;

        *tableIndex = index;
        return;
    }

    *tableIndex = INVALID_RULE_INDEX;
 }

/*******************************************************************************
* cpssDxCh3PclActionDelete (table command)
*
* DESCRIPTION:
*  Delete from wrapper level table.
*  The function finds action table entry by rule index ,
*  deletes the actionEntry and updates the list.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       ruleIndex       - index of the rule
*
* OUTPUTS:
*       none
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
static CMD_STATUS wrCpssDxCh3PclActionDelete
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
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum =  (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inArgs[1];

    pclDxCh3ActionGet( ruleIndex, &index);

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
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxCh3PclActionSet (table command)
*
* DESCRIPTION:
*   The function sets action entry at the action table for the use of cpssDxCh3PclIngressRule/
*   cpssDxCh3PclEgressRule table.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       ruleIndex       - index of the rule in the TCAM.
*
* OUTPUTS:
*       actionTable      - contains actionEntry and valid flag.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
* if inFields[0]==1 then action enry is also written to hardware if not it written
* only to wrapper level table
*
*******************************************************************************/
static CMD_STATUS wrCpssDxCh3PclActionSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_PCL_ACTION_STC           *actionPtr;
    GT_U32                              ruleIndex;
    GT_U8                               devNum;
    GT_U32                              index;
    CPSS_PCL_RULE_SIZE_ENT              ruleSize;
    GT_STATUS                           result;
    GT_BOOL                             update;
    GT_U32                              i;
    GT_BOOL                             isXCatActionTable;
    GT_BOOL                             isLionActionTable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    isXCatActionTable = (numFields < 51) ? GT_FALSE : GT_TRUE;
    isLionActionTable = (numFields < 54) ? GT_FALSE : GT_TRUE;

    devNum  =       (GT_U8)inArgs[0];
    ruleIndex =     (GT_U32)inArgs[1];

    if (firstRun)
    {
        wrCpssDxCh3PclActionClear(inArgs,inFields,numFields,outArgs);
        firstRun = GT_FALSE;
    }

    pclDxCh3ActionGet( ruleIndex, &index);

    /* the rule wasn't previously in action table */
    if (index == INVALID_RULE_INDEX)
    {
        if (firstFree == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
            return CMD_AGENT_ERROR;
        }

        index = firstFree;
    }

    cpssOsMemSet(
        &(actionTable[index].actionEntry), 0,
        sizeof(actionTable[index].actionEntry));

    actionPtr = &(actionTable[index].actionEntry);

    /* field index */
    i = 0;

    update = (GT_BOOL)inFields[i++];

    actionPtr->pktCmd        = (CPSS_PACKET_CMD_ENT)inFields[i++];
    actionPtr->actionStop    =(GT_BOOL)inFields[i++];
    actionPtr->egressPolicy  = (GT_BOOL)inFields[i++];
    actionPtr->mirror.cpuCode=(CPSS_NET_RX_CPU_CODE_ENT)inFields[i++];
    actionPtr->mirror.mirrorToRxAnalyzerPort=(GT_BOOL)inFields[i++];
    actionPtr->matchCounter.enableMatchCount=(GT_BOOL)inFields[i++];
    actionPtr->matchCounter.matchCounterIndex=(GT_U32)inFields[i++];

    /* WA - GUI defines the field modifyDscp as BOOL instead of Enum */
    switch (inFields[i++])
    {
        case (GT_32)GT_FALSE:
            actionPtr->qos.modifyDscp =
                CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
            break;
        case (GT_32)GT_TRUE:
            actionPtr->qos.modifyDscp =
                CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            break;
        default:
            /* the case reserved for Galtis GUI that must     */
            /* be fixed by appending "DISABLE_MODIFY" to list */
            /* {"GT_FALSE", "GT_TRUE", "DISABLE_MODIFY"}      */
            actionPtr->qos.modifyDscp =
                CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            break;
    }

    actionPtr->qos.modifyUp =
        (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[i++];

    switch (actionPtr->egressPolicy)
    {
        case GT_FALSE:
            actionPtr->qos.qos.ingress.profileIndex = (GT_U32)inFields[i];
            actionPtr->qos.qos.ingress.profileAssignIndex = (GT_BOOL)inFields[i+1];
            actionPtr->qos.qos.ingress.profilePrecedence =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[i+2];
            break;

        default:
        case GT_TRUE:
            actionPtr->qos.qos.egress.dscp = (GT_U8)inFields[i+3];
            actionPtr->qos.qos.egress.up = (GT_U8)inFields[i+4];
            break;

    }

    /* bypass the switch */
    i += 5;

    actionPtr->redirect.redirectCmd =
        (CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT)inFields[i++];

    if (actionPtr->redirect.redirectCmd ==
        CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
    {
        actionPtr->redirect.data.outIf.outInterface.type =
            (CPSS_INTERFACE_TYPE_ENT)inFields[i];

        switch (actionPtr->redirect.data.outIf.outInterface.type)
        {
        case CPSS_INTERFACE_PORT_E:
            actionPtr->redirect.data.outIf.outInterface.devPort.devNum =
                (GT_U8)inFields[i+1];
            actionPtr->redirect.data.outIf.outInterface.devPort.portNum =
                (GT_U8)inFields[i+2];

            CONVERT_DEV_PORT_DATA_MAC(
                actionPtr->redirect.data.outIf.outInterface.devPort.devNum,
                actionPtr->redirect.data.outIf.outInterface.devPort.portNum);

            break;

        case CPSS_INTERFACE_TRUNK_E:
            actionPtr->redirect.data.outIf.outInterface.trunkId =
                (GT_TRUNK_ID)inFields[i+3];
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->redirect.data.outIf.outInterface.trunkId);
            break;

        case CPSS_INTERFACE_VIDX_E:
            actionPtr->redirect.data.outIf.outInterface.vidx =
                (GT_U16)inFields[i+4];
            break;

        case CPSS_INTERFACE_VID_E:
            actionPtr->redirect.data.outIf.outInterface.vlanId =
                (GT_U16)inFields[i+5];
            break;

        default:
            break;
        }
        actionPtr->redirect.data.outIf.vntL2Echo   = (GT_BOOL)inFields[i+6];
        actionPtr->redirect.data.outIf.tunnelStart = (GT_BOOL)inFields[i+7];
        actionPtr->redirect.data.outIf.tunnelPtr   = (GT_U32)inFields[i+8];
        actionPtr->redirect.data.outIf.tunnelType  =
            (CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ENT)inFields[i+9];
        if (isLionActionTable != GT_FALSE)
        {
            actionPtr->redirect.data.outIf.arpPtr = (GT_U32)inFields[i+10];
            actionPtr->redirect.data.outIf.modifyMacDa = (GT_BOOL)inFields[i+11];
            actionPtr->redirect.data.outIf.modifyMacSa = (GT_BOOL)inFields[i+12];
        }
        /* following - not implemented "redirectInterfaceType" */
    }

    /* bypass the "if" body */
    i += 11;
    if (isLionActionTable != GT_FALSE)
    {
        i += 3;
    }



    if (actionPtr->redirect.redirectCmd ==
        CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
    {
        actionPtr->redirect.data.routerLttIndex = (GT_U32)inFields[i++];
    }
    else
    {
        /* bypass the "if" body */
        i ++;
    }


    if (actionPtr->redirect.redirectCmd ==
        CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E)
    {
        actionPtr->redirect.data.vrfId = (GT_U32)inFields[i++];
    }
    else
    {
        /* bypass the "if" body */
        i ++;
    }

    actionPtr->policer.policerEnable = (GT_BOOL)inFields[i++];
    actionPtr->policer.policerId = (GT_U32)inFields[i++];

    actionPtr->vlan.modifyVlan =
        (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[i++];
    actionPtr->vlan.nestedVlan = (GT_BOOL)inFields[i++];
    actionPtr->vlan.vlanId = (GT_U16)inFields[i++];
    actionPtr->vlan.precedence =
        (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[i++];

    actionPtr->ipUcRoute.doIpUcRoute = (GT_BOOL)inFields[i++];
    actionPtr->ipUcRoute.arpDaIndex = (GT_U32)inFields[i++];
    actionPtr->ipUcRoute.decrementTTL = (GT_BOOL)inFields[i++];
    actionPtr->ipUcRoute.bypassTTLCheck = (GT_BOOL)inFields[i++];
    actionPtr->ipUcRoute.icmpRedirectCheck = (GT_BOOL)inFields[i++];

    actionPtr->sourceId.assignSourceId = (GT_BOOL)inFields[i++];
    actionPtr->sourceId.sourceIdValue  = (GT_U32)inFields[i++];

    if (isXCatActionTable != GT_FALSE)
    {
        actionPtr->bypassBridge                           = (GT_BOOL)inFields[i++];
        actionPtr->bypassIngressPipe                      = (GT_BOOL)inFields[i++];
        actionPtr->lookupConfig.ipclConfigIndex           = (GT_U32)inFields[i++];
        actionPtr->lookupConfig.pcl0_1OverrideConfigIndex = (GT_BOOL)inFields[i++];
        actionPtr->lookupConfig.pcl1OverrideConfigIndex   = (GT_BOOL)inFields[i++];
        i++; /*redundant mirror.CpuCode */
        i++; /*redundant  mirror.mirrorToRxAnalyzerPort */
        actionPtr->mirror.mirrorTcpRstAndFinPacketsToCpu  = (GT_BOOL)inFields[i++];
        i++; /*redundant  policer.policerEnable */
    }

    if (update != GT_FALSE) /*Write entry to hardware or not*/
    {
        ruleSize=(CPSS_PCL_RULE_SIZE_ENT)inArgs[2];
        /*call api function*/
        result = pg_wrap_cpssDxChPclRuleActionUpdate(devNum, ruleSize, ruleIndex, actionPtr);
    }
    else
    {
        actionTable[index].valid = GT_TRUE;
        actionTable[index].ruleIndex = ruleIndex;
        firstFree = actionTable[firstFree].nextFree;
        result = GT_OK;
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxCh3PclActionGetFirst (table command)
*
* DESCRIPTION:
*   The function gets action entry from the action table in hardware
*  (not in wrapper level table)
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See pg_wrap_cpssDxChPclRuleSet.
*       direction       - Policy direction:
*                         Ingress or Egress
*                         Needed for parsing
*
* OUTPUTS:
*       actionEntry      - actionTable - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern
*                          valid - valid / invalid flag.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
***********************************************************************************/
static CMD_STATUS wrCpssDxCh3PclActionGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PCL_RULE_SIZE_ENT      ruleSize;
    GT_U32                      ruleIndex;
    CPSS_PCL_DIRECTION_ENT      direction;
    CPSS_DXCH_PCL_ACTION_STC    action;
    GT_U32                      i;
    GT_BOOL                     isXCatActionTable;
    GT_BOOL                     isLionActionTable;
    GT_U32                      actualNumFields;
    GT_U8  __Dev,__Port; /* Dummy for converting. */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    isXCatActionTable = (numFields < 51) ? GT_FALSE : GT_TRUE;
    isLionActionTable = (numFields < 54) ? GT_FALSE : GT_TRUE;

    /* to support the not seted/geted members of structures */
    cpssOsMemSet(&action, 0, sizeof(action));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleIndex = (CPSS_PCL_DIRECTION_ENT)inArgs[1];
    ruleSize = (CPSS_PCL_RULE_SIZE_ENT)inArgs[2];
    direction = (GT_U32)inArgs[3];


    result = pg_wrap_cpssDxChPclRuleActionGet(devNum, ruleSize, ruleIndex,
                                            direction, &action);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    i = 0;

    inFields[i++]=GT_TRUE;
    inFields[i++]=action.pktCmd;
    inFields[i++]=action.actionStop ;
    inFields[i++]=action.egressPolicy ;
    inFields[i++]=action.mirror.cpuCode;
    inFields[i++]=action.mirror.mirrorToRxAnalyzerPort;
    inFields[i++]=action.matchCounter.enableMatchCount;
    inFields[i++]=action.matchCounter.matchCounterIndex;
    inFields[i++]=action.qos.modifyDscp ;
    inFields[i++]=action.qos.modifyUp;
    inFields[i++]=action.qos.qos.ingress.profileIndex;
    inFields[i++]=action.qos.qos.ingress.profileAssignIndex;
    inFields[i++]=action.qos.qos.ingress.profilePrecedence;
    inFields[i++]=action.qos.qos.egress.dscp;
    inFields[i++]=action.qos.qos.egress.up;
    inFields[i++]=action.redirect.redirectCmd;
    inFields[i++]=action.redirect.data.outIf.outInterface.type;
    __Dev  = action.redirect.data.outIf.outInterface.devPort.devNum;
    __Port = action.redirect.data.outIf.outInterface.devPort.portNum;

    CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev, __Port) ;
    inFields[i++] = __Dev  ;
    inFields[i++] = __Port ;
    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(action.redirect.data.outIf.outInterface.trunkId);
    inFields[i++]=action.redirect.data.outIf.outInterface.trunkId;
    inFields[i++]=action.redirect.data.outIf.outInterface.vidx;
    inFields[i++]=action.redirect.data.outIf.outInterface.vlanId;
    inFields[i++]= action.redirect.data.outIf.vntL2Echo;
    inFields[i++]= action.redirect.data.outIf.tunnelStart;
    inFields[i++]= action.redirect.data.outIf.tunnelPtr;
    inFields[i++]= action.redirect.data.outIf.tunnelType;
    if (isLionActionTable != GT_FALSE)
    {
        inFields[i++] = action.redirect.data.outIf.arpPtr;
        inFields[i++] = action.redirect.data.outIf.modifyMacDa;
        inFields[i++] = action.redirect.data.outIf.modifyMacSa;
    }
    inFields[i++]= 0 /* not implemented "redirectInterfaceType" */;
    inFields[i++]=action.redirect.data.routerLttIndex;
    inFields[i++]=action.redirect.data.vrfId ;
    inFields[i++]=action.policer.policerEnable;
    inFields[i++]=action.policer.policerId;
    inFields[i++]=action.vlan.modifyVlan;
    inFields[i++]=action.vlan.nestedVlan;
    inFields[i++]=action.vlan.vlanId;
    inFields[i++]=action.vlan.precedence;
    inFields[i++]=action.ipUcRoute.doIpUcRoute;
    inFields[i++]=action.ipUcRoute.arpDaIndex ;
    inFields[i++]=action.ipUcRoute.decrementTTL;
    inFields[i++]=action.ipUcRoute.bypassTTLCheck;
    inFields[i++]=action.ipUcRoute.icmpRedirectCheck;
    inFields[i++]=action.sourceId.assignSourceId;
    inFields[i++]=action.sourceId.sourceIdValue ;

    if (isXCatActionTable != GT_FALSE)
    {
        inFields[i++] = action.bypassBridge;
        inFields[i++] = action.bypassIngressPipe;
        inFields[i++] = action.lookupConfig.ipclConfigIndex;
        inFields[i++] = action.lookupConfig.pcl0_1OverrideConfigIndex;
        inFields[i++] = action.lookupConfig.pcl1OverrideConfigIndex;
        inFields[i++] = 0; /*redundant mirror.CpuCode */
        inFields[i++] = 0; /*redundant  mirror.mirrorToRxAnalyzerPort */
        inFields[i++] = action.mirror.mirrorTcpRstAndFinPacketsToCpu;
        inFields[i++] = 0; /*redundant  policer.policerEnable */
    }

    actualNumFields = i;

    /* pack and output table fields */
    for (i = 0; (i < actualNumFields); i++)
    {
        fieldOutput("%d", inFields[i]);
        fieldOutputSetAppendMode();
    }

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxCh3PclActionGet_Ch3
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxCh3PclActionGet(
        inArgs,inFields, 42 /*numFields*/, outArgs);
}

static CMD_STATUS wrCpssDxCh3PclActionGet_XCat
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxCh3PclActionGet(
        inArgs,inFields, 51 /*numFields*/, outArgs);
}

/* tables cpssDxCh3PclIngressRule and cpssDxCh3PclEgressRule global variables */

static    GT_BOOL                              mask_set = GT_FALSE; /* is mask set*/
static    GT_BOOL                              pattern_set = GT_FALSE; /* is pattern set*/
static    GT_U32                               mask_ruleIndex = 0;
static    GT_U32                               pattern_ruleIndex = 0;
/********************cpssDxCh3PclIngressRule********************************/

static CMD_STATUS wrCpssDxCh3PclIngressRule_STD_NOT_IP_Write
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8           devNum;
    GT_BYTE_ARRY                       maskBArr, patternBArr;
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    devNum=(GT_U8)inArgs[0];

    if(inFields[1] == 0) /* mask */
    {
    mask_ruleIndex = (GT_U32)inFields[0];

    maskData.ruleStdNotIp.common.portListBmp = ruleSet_portListBmpMask;

    maskData.ruleStdNotIp.common.pclId = (GT_U16)inFields[2];
    CONVERT_PCL_MASK_PCL_ID_TEST_TO_CPSS_MAC(maskData.ruleStdNotIp.common.pclId);
    maskData.ruleStdNotIp.common.macToMe=(GT_U8)inFields[3];
    maskData.ruleStdNotIp.common.sourcePort = (GT_U8)inFields[4];
    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleStdNotIp.common.sourcePort);


    maskData.ruleStdNotIp.common.isTagged = (GT_U8)inFields[5];
    maskData.ruleStdNotIp.common.vid = (GT_U16)inFields[6];
    maskData.ruleStdNotIp.common.up = (GT_U8)inFields[7];
    maskData.ruleStdNotIp.common.qosProfile = (GT_U8)inFields[8];
    maskData.ruleStdNotIp.common.isIp = (GT_U8)inFields[9];
    maskData.ruleStdNotIp.common.isL2Valid = (GT_U8)inFields[10];
    maskData.ruleStdNotIp.common.isUdbValid = (GT_U8)inFields[11];
    maskData.ruleStdNotIp.isIpv4 = (GT_U8)inFields[12];
    maskData.ruleStdNotIp.etherType = (GT_U16)inFields[13];
    maskData.ruleStdNotIp.isArp = (GT_BOOL)inFields[14];
    maskData.ruleStdNotIp.l2Encap = (GT_BOOL)inFields[15];
    galtisMacAddr(&maskData.ruleStdNotIp.macDa, (GT_U8*)inFields[16]);
    galtisMacAddr(&maskData.ruleStdNotIp.macSa, (GT_U8*)inFields[17]);
    galtisBArray(&maskBArr,(GT_U8*)inFields[18]);

    cmdOsMemCpy(
            maskData.ruleStdNotIp.udb, maskBArr.data,
            maskBArr.length);

        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];

        patternData.ruleStdNotIp.common.portListBmp = ruleSet_portListBmpPattern;

        patternData.ruleStdNotIp.common.pclId = (GT_U16)inFields[2];
        patternData.ruleStdNotIp.common.macToMe=(GT_U8)inFields[3];
        patternData.ruleStdNotIp.common.sourcePort = (GT_U8)inFields[4];

        devNum=(GT_U8)inArgs[0];
        CONVERT_DEV_PORT_MAC(devNum, patternData.ruleStdNotIp.common.sourcePort);
        inArgs[0]=devNum;

        patternData.ruleStdNotIp.common.isTagged = (GT_U8)inFields[5];
        patternData.ruleStdNotIp.common.vid = (GT_U16)inFields[6];
        patternData.ruleStdNotIp.common.up = (GT_U8)inFields[7];
        patternData.ruleStdNotIp.common.qosProfile = (GT_U8)inFields[8];
        patternData.ruleStdNotIp.common.isIp = (GT_U8)inFields[9];
        patternData.ruleStdNotIp.common.isL2Valid = (GT_U8)inFields[10];
        patternData.ruleStdNotIp.common.isUdbValid = (GT_U8)inFields[11];
        patternData.ruleStdNotIp.isIpv4 = (GT_U8)inFields[12];
        patternData.ruleStdNotIp.etherType = (GT_U16)inFields[13];
        patternData.ruleStdNotIp.isArp = (GT_BOOL)inFields[14];
        patternData.ruleStdNotIp.l2Encap = (GT_BOOL)inFields[15];
        galtisMacAddr(&patternData.ruleStdNotIp.macDa, (GT_U8*)inFields[16]);
        galtisMacAddr(&patternData.ruleStdNotIp.macSa, (GT_U8*)inFields[17]);
        galtisBArray(&patternBArr,(GT_U8*)inFields[18]);

       cmdOsMemCpy(
        patternData.ruleStdNotIp.udb, patternBArr.data,
        patternBArr.length);


        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}
/*********************************************************************************/

static CMD_STATUS wrCpssDxCh3PclIngressRule_STD_IP_L2_QOS_Write
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8           devNum;
    GT_BYTE_ARRY                       maskBArr, patternBArr;
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    devNum=(GT_U8)inArgs[0];

    if(inFields[1] == 0) /* mask */
    {
    mask_ruleIndex = (GT_U32)inFields[0];

    maskData.ruleStdIpL2Qos.common.portListBmp = ruleSet_portListBmpMask;

    maskData.ruleStdIpL2Qos.common.pclId = (GT_U16)inFields[2];
    CONVERT_PCL_MASK_PCL_ID_TEST_TO_CPSS_MAC(maskData.ruleStdIpL2Qos.common.pclId);
    maskData.ruleStdIpL2Qos.common.macToMe=(GT_U8)inFields[3];
    maskData.ruleStdIpL2Qos.common.sourcePort = (GT_U8)inFields[4];

    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleStdIpL2Qos.common.sourcePort);

    maskData.ruleStdIpL2Qos.common.isTagged = (GT_U8)inFields[5];
    maskData.ruleStdIpL2Qos.common.vid = (GT_U16)inFields[6];
    maskData.ruleStdIpL2Qos.common.up = (GT_U8)inFields[7];
    maskData.ruleStdIpL2Qos.common.qosProfile = (GT_U8)inFields[8];
    maskData.ruleStdIpL2Qos.common.isIp = (GT_U8)inFields[9];
    maskData.ruleStdIpL2Qos.common.isL2Valid = (GT_U8)inFields[10];
    maskData.ruleStdIpL2Qos.common.isUdbValid = (GT_U8)inFields[11];
    maskData.ruleStdIpL2Qos.commonStdIp.isIpv4 = (GT_U8)inFields[12];
    maskData.ruleStdIpL2Qos.commonStdIp.ipProtocol = (GT_U8)inFields[13];
    maskData.ruleStdIpL2Qos.commonStdIp.dscp = (GT_U8)inFields[14];
    maskData.ruleStdIpL2Qos.commonStdIp.isL4Valid = (GT_U8)inFields[15];
    maskData.ruleStdIpL2Qos.commonStdIp.l4Byte2 = (GT_U8)inFields[16];
    maskData.ruleStdIpL2Qos.commonStdIp.l4Byte3 = (GT_U8)inFields[17];
    maskData.ruleStdIpL2Qos.commonStdIp.ipHeaderOk = (GT_U8)inFields[18];
    maskData.ruleStdIpL2Qos.commonStdIp.ipv4Fragmented = (GT_U8)inFields[19];
    maskData.ruleStdIpL2Qos.isArp = (GT_BOOL)inFields[20];
    maskData.ruleStdIpL2Qos.isIpv6ExtHdrExist = (GT_BOOL)inFields[21];
    maskData.ruleStdIpL2Qos.isIpv6HopByHop = (GT_BOOL)inFields[22];
    galtisMacAddr(&maskData.ruleStdIpL2Qos.macDa, (GT_U8*)inFields[23]);
    galtisMacAddr(&maskData.ruleStdIpL2Qos.macSa, (GT_U8*)inFields[24]);
    galtisBArray(&maskBArr,(GT_U8*)inFields[25]);

    if(maskBArr.length > 2)
    {
        maskBArr.length = 2;
    }

    cmdOsMemCpy(
            maskData.ruleStdIpL2Qos.udb, maskBArr.data,
            maskBArr.length);

        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];

        patternData.ruleStdIpL2Qos.common.portListBmp = ruleSet_portListBmpPattern;

        patternData.ruleStdIpL2Qos.common.pclId = (GT_U16)inFields[2];
        patternData.ruleStdIpL2Qos.common.macToMe=(GT_U8)inFields[3];
        patternData.ruleStdIpL2Qos.common.sourcePort = (GT_U8)inFields[4];

        devNum=(GT_U8)inArgs[0];
        CONVERT_DEV_PORT_MAC(devNum, patternData.ruleStdIpL2Qos.common.sourcePort);
        inArgs[0]=devNum;

        patternData.ruleStdIpL2Qos.common.isTagged = (GT_U8)inFields[5];
        patternData.ruleStdIpL2Qos.common.vid = (GT_U16)inFields[6];
        patternData.ruleStdIpL2Qos.common.up = (GT_U8)inFields[7];
        patternData.ruleStdIpL2Qos.common.qosProfile = (GT_U8)inFields[8];
        patternData.ruleStdIpL2Qos.common.isIp = (GT_U8)inFields[9];
        patternData.ruleStdIpL2Qos.common.isL2Valid = (GT_U8)inFields[10];
        patternData.ruleStdIpL2Qos.common.isUdbValid = (GT_U8)inFields[11];
        patternData.ruleStdIpL2Qos.commonStdIp.isIpv4 = (GT_U8)inFields[12];
        patternData.ruleStdIpL2Qos.commonStdIp.ipProtocol = (GT_U8)inFields[13];
        patternData.ruleStdIpL2Qos.commonStdIp.dscp = (GT_U8)inFields[14];
        patternData.ruleStdIpL2Qos.commonStdIp.isL4Valid = (GT_U8)inFields[15];
        patternData.ruleStdIpL2Qos.commonStdIp.l4Byte2 = (GT_U8)inFields[16];
        patternData.ruleStdIpL2Qos.commonStdIp.l4Byte3 = (GT_U8)inFields[17];
        patternData.ruleStdIpL2Qos.commonStdIp.ipHeaderOk = (GT_U8)inFields[18];
        patternData.ruleStdIpL2Qos.commonStdIp.ipv4Fragmented = (GT_U8)inFields[19];
        patternData.ruleStdIpL2Qos.isArp = (GT_BOOL)inFields[20];
        patternData.ruleStdIpL2Qos.isIpv6ExtHdrExist = (GT_BOOL)inFields[21];
        patternData.ruleStdIpL2Qos.isIpv6HopByHop = (GT_BOOL)inFields[22];
        galtisMacAddr(&patternData.ruleStdIpL2Qos.macDa, (GT_U8*)inFields[23]);
        galtisMacAddr(&patternData.ruleStdIpL2Qos.macSa, (GT_U8*)inFields[24]);
        galtisBArray(&patternBArr,(GT_U8*)inFields[25]);

        if(patternBArr.length > 2)
        {
            patternBArr.length = 2;
        }

        cmdOsMemCpy(
            patternData.ruleStdIpL2Qos.udb, patternBArr.data,
            patternBArr.length);

        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}
/**********************************************************************************/

static CMD_STATUS wrCpssDxCh3PclIngressRule_STD_IPV4_L4_Write
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8           devNum;
    GT_BYTE_ARRY                       maskBArr, patternBArr;
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
    mask_ruleIndex = (GT_U32)inFields[0];

    maskData.ruleStdIpv4L4.common.portListBmp = ruleSet_portListBmpMask;

    maskData.ruleStdIpv4L4.common.pclId = (GT_U16)inFields[2];
    CONVERT_PCL_MASK_PCL_ID_TEST_TO_CPSS_MAC(maskData.ruleStdIpv4L4.common.pclId);
    maskData.ruleStdIpv4L4.common.macToMe=(GT_U8)inFields[3];
    maskData.ruleStdIpv4L4.common.sourcePort = (GT_U8)inFields[4];

    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleStdIpv4L4.common.sourcePort);

    maskData.ruleStdIpv4L4.common.isTagged = (GT_U8)inFields[5];
    maskData.ruleStdIpv4L4.common.vid = (GT_U16)inFields[6];
    maskData.ruleStdIpv4L4.common.up = (GT_U8)inFields[7];
    maskData.ruleStdIpv4L4.common.qosProfile = (GT_U8)inFields[8];
    maskData.ruleStdIpv4L4.common.isIp = (GT_U8)inFields[9];
    maskData.ruleStdIpv4L4.common.isL2Valid = (GT_U8)inFields[10];
    maskData.ruleStdIpv4L4.common.isUdbValid = (GT_U8)inFields[11];
    maskData.ruleStdIpv4L4.commonStdIp.isIpv4 = (GT_U8)inFields[12];
    maskData.ruleStdIpv4L4.commonStdIp.ipProtocol = (GT_U8)inFields[13];
    maskData.ruleStdIpv4L4.commonStdIp.dscp = (GT_U8)inFields[14];
    maskData.ruleStdIpv4L4.commonStdIp.isL4Valid = (GT_U8)inFields[15];
    maskData.ruleStdIpv4L4.commonStdIp.l4Byte2 = (GT_U8)inFields[16];
    maskData.ruleStdIpv4L4.commonStdIp.l4Byte3 = (GT_U8)inFields[17];
    maskData.ruleStdIpv4L4.commonStdIp.ipHeaderOk = (GT_U8)inFields[18];
    maskData.ruleStdIpv4L4.commonStdIp.ipv4Fragmented = (GT_U8)inFields[19];
    maskData.ruleStdIpv4L4.isArp = (GT_BOOL)inFields[20];
    maskData.ruleStdIpv4L4.isBc = (GT_BOOL)inFields[21];
    galtisIpAddr(&maskData.ruleStdIpv4L4.sip, (GT_U8*)inFields[22]);
    galtisIpAddr(&maskData.ruleStdIpv4L4.dip, (GT_U8*)inFields[23]);
    maskData.ruleStdIpv4L4.l4Byte0 = (GT_U8)inFields[24];
    maskData.ruleStdIpv4L4.l4Byte1 = (GT_U8)inFields[25];
    maskData.ruleStdIpv4L4.l4Byte13 = (GT_U8)inFields[26];
    galtisBArray(&maskBArr,(GT_U8*)inFields[27]);
    cmdOsMemCpy(
            maskData.ruleStdIpv4L4.udb, maskBArr.data,
            maskBArr.length);

        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
    pattern_ruleIndex = (GT_U32)inFields[0];

    patternData.ruleStdIpv4L4.common.portListBmp = ruleSet_portListBmpPattern;

    patternData.ruleStdIpv4L4.common.pclId = (GT_U16)inFields[2];
    patternData.ruleStdIpv4L4.common.macToMe=(GT_U8)inFields[3];
    patternData.ruleStdIpv4L4.common.sourcePort = (GT_U8)inFields[4];

    devNum=(GT_U8)inArgs[0];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleStdIpv4L4.common.sourcePort);
    inArgs[0]=devNum;

    patternData.ruleStdIpv4L4.common.isTagged = (GT_U8)inFields[5];
    patternData.ruleStdIpv4L4.common.vid = (GT_U16)inFields[6];
    patternData.ruleStdIpv4L4.common.up = (GT_U8)inFields[7];
    patternData.ruleStdIpv4L4.common.qosProfile = (GT_U8)inFields[8];
    patternData.ruleStdIpv4L4.common.isIp = (GT_U8)inFields[9];
    patternData.ruleStdIpv4L4.common.isL2Valid = (GT_U8)inFields[10];
    patternData.ruleStdIpv4L4.common.isUdbValid = (GT_U8)inFields[11];
    patternData.ruleStdIpv4L4.commonStdIp.isIpv4 = (GT_U8)inFields[12];
    patternData.ruleStdIpv4L4.commonStdIp.ipProtocol = (GT_U8)inFields[13];
    patternData.ruleStdIpv4L4.commonStdIp.dscp = (GT_U8)inFields[14];
    patternData.ruleStdIpv4L4.commonStdIp.isL4Valid = (GT_U8)inFields[15];
    patternData.ruleStdIpv4L4.commonStdIp.l4Byte2 = (GT_U8)inFields[16];
    patternData.ruleStdIpv4L4.commonStdIp.l4Byte3 = (GT_U8)inFields[17];
    patternData.ruleStdIpv4L4.commonStdIp.ipHeaderOk = (GT_U8)inFields[18];
    patternData.ruleStdIpv4L4.commonStdIp.ipv4Fragmented = (GT_U8)inFields[19];
    patternData.ruleStdIpv4L4.isArp = (GT_BOOL)inFields[20];
    patternData.ruleStdIpv4L4.isBc = (GT_BOOL)inFields[21];
    galtisIpAddr(&patternData.ruleStdIpv4L4.sip, (GT_U8*)inFields[22]);
    galtisIpAddr(&patternData.ruleStdIpv4L4.dip, (GT_U8*)inFields[23]);
    patternData.ruleStdIpv4L4.l4Byte0 = (GT_U8)inFields[24];
    patternData.ruleStdIpv4L4.l4Byte1 = (GT_U8)inFields[25];
    patternData.ruleStdIpv4L4.l4Byte13 = (GT_U8)inFields[26];
    galtisBArray(&patternBArr,(GT_U8*)inFields[27]);

        cmdOsMemCpy(
            patternData.ruleStdIpv4L4.udb, patternBArr.data,
            patternBArr.length);

        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}

/**********************************************************************************/

static CMD_STATUS wrCpssDxCh3PclIngressRule_STD_IPV6_DIP_Write
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U8           devNum;
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
    mask_ruleIndex = (GT_U32)inFields[0];

    maskData.ruleStdIpv6Dip.common.portListBmp = ruleSet_portListBmpMask;

    maskData.ruleStdIpv6Dip.common.pclId = (GT_U16)inFields[2];
    CONVERT_PCL_MASK_PCL_ID_TEST_TO_CPSS_MAC(maskData.ruleStdIpv6Dip.common.pclId);
    maskData.ruleStdIpv6Dip.common.macToMe=(GT_U8)inFields[3];
    maskData.ruleStdIpv6Dip.common.sourcePort = (GT_U8)inFields[4];

    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleStdIpv6Dip.common.sourcePort);

    maskData.ruleStdIpv6Dip.common.isTagged = (GT_U8)inFields[5];
    maskData.ruleStdIpv6Dip.common.vid = (GT_U16)inFields[6];
    maskData.ruleStdIpv6Dip.common.up = (GT_U8)inFields[7];
    maskData.ruleStdIpv6Dip.common.qosProfile = (GT_U8)inFields[8];
    maskData.ruleStdIpv6Dip.common.isIp = (GT_U8)inFields[9];
    maskData.ruleStdIpv6Dip.common.isL2Valid = (GT_U8)inFields[10];
    maskData.ruleStdIpv6Dip.common.isUdbValid = (GT_U8)inFields[11];
    maskData.ruleStdIpv6Dip.commonStdIp.isIpv4 = (GT_U8)inFields[12];
    maskData.ruleStdIpv6Dip.commonStdIp.ipProtocol = (GT_U8)inFields[13];
    maskData.ruleStdIpv6Dip.commonStdIp.dscp = (GT_U8)inFields[14];
    maskData.ruleStdIpv6Dip.commonStdIp.isL4Valid = (GT_U8)inFields[15];
    maskData.ruleStdIpv6Dip.commonStdIp.l4Byte2 = (GT_U8)inFields[16];
    maskData.ruleStdIpv6Dip.commonStdIp.l4Byte3 = (GT_U8)inFields[17];
    maskData.ruleStdIpv6Dip.commonStdIp.ipHeaderOk = (GT_U8)inFields[18];
    maskData.ruleStdIpv6Dip.commonStdIp.ipv4Fragmented = (GT_U8)inFields[19];
    maskData.ruleStdIpv6Dip.isArp = (GT_BOOL)inFields[20];
    maskData.ruleStdIpv6Dip.isIpv6ExtHdrExist = (GT_U8)inFields[21];
    maskData.ruleStdIpv6Dip.isIpv6HopByHop = (GT_U8)inFields[22];
    galtisIpv6Addr(&maskData.ruleStdIpv6Dip.dip, (GT_U8*)inFields[23]);

    mask_set = GT_TRUE;
    }
    else /* pattern */
    {
    pattern_ruleIndex = (GT_U32)inFields[0];

    patternData.ruleStdIpv6Dip.common.portListBmp = ruleSet_portListBmpPattern;

    patternData.ruleStdIpv6Dip.common.pclId = (GT_U16)inFields[2];
    patternData.ruleStdIpv6Dip.common.macToMe=(GT_U8)inFields[3];
    patternData.ruleStdIpv6Dip.common.sourcePort = (GT_U8)inFields[4];

    devNum=(GT_U8)inArgs[0];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleStdIpv6Dip.common.sourcePort);
    inArgs[0]=devNum;

    patternData.ruleStdIpv6Dip.common.isTagged = (GT_U8)inFields[5];
    patternData.ruleStdIpv6Dip.common.vid = (GT_U16)inFields[6];
    patternData.ruleStdIpv6Dip.common.up = (GT_U8)inFields[7];
    patternData.ruleStdIpv6Dip.common.qosProfile = (GT_U8)inFields[8];
    patternData.ruleStdIpv6Dip.common.isIp = (GT_U8)inFields[9];
    patternData.ruleStdIpv6Dip.common.isL2Valid = (GT_U8)inFields[10];
    patternData.ruleStdIpv6Dip.common.isUdbValid = (GT_U8)inFields[11];
    patternData.ruleStdIpv6Dip.commonStdIp.isIpv4 = (GT_U8)inFields[12];
    patternData.ruleStdIpv6Dip.commonStdIp.ipProtocol = (GT_U8)inFields[13];
    patternData.ruleStdIpv6Dip.commonStdIp.dscp = (GT_U8)inFields[14];
    patternData.ruleStdIpv6Dip.commonStdIp.isL4Valid = (GT_U8)inFields[15];
    patternData.ruleStdIpv6Dip.commonStdIp.l4Byte2 = (GT_U8)inFields[16];
    patternData.ruleStdIpv6Dip.commonStdIp.l4Byte3 = (GT_U8)inFields[17];
    patternData.ruleStdIpv6Dip.commonStdIp.ipHeaderOk = (GT_U8)inFields[18];
    patternData.ruleStdIpv6Dip.commonStdIp.ipv4Fragmented = (GT_U8)inFields[19];
    patternData.ruleStdIpv6Dip.isArp = (GT_BOOL)inFields[20];
    patternData.ruleStdIpv6Dip.isIpv6ExtHdrExist = (GT_U8)inFields[21];
    patternData.ruleStdIpv6Dip.isIpv6HopByHop = (GT_U8)inFields[22];
    galtisIpv6Addr(&patternData.ruleStdIpv6Dip.dip, (GT_U8*)inFields[23]);

    pattern_set = GT_TRUE;
    }

    return CMD_OK;
}
/*********************************************************************************/

static CMD_STATUS wrCpssDxCh3PclIngressRule_EXT_NOT_IPV6_Write
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY                       maskBArr, patternBArr;
    GT_U8           devNum;
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
    mask_ruleIndex = (GT_U32)inFields[0];

    maskData.ruleExtNotIpv6.common.portListBmp = ruleSet_portListBmpMask;

    maskData.ruleExtNotIpv6.common.pclId = (GT_U16)inFields[2];
    maskData.ruleExtNotIpv6.common.macToMe=(GT_U8)inFields[3];
    maskData.ruleExtNotIpv6.common.sourcePort = (GT_U8)inFields[4];

    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleExtNotIpv6.common.sourcePort);

    maskData.ruleExtNotIpv6.common.isTagged = (GT_U8)inFields[5];
    maskData.ruleExtNotIpv6.common.vid = (GT_U16)inFields[6];
    maskData.ruleExtNotIpv6.common.up = (GT_U8)inFields[7];
    maskData.ruleExtNotIpv6.common.qosProfile = (GT_U8)inFields[8];
    maskData.ruleExtNotIpv6.common.isIp = (GT_U8)inFields[9];
    maskData.ruleExtNotIpv6.common.isL2Valid = (GT_U8)inFields[10];
    maskData.ruleExtNotIpv6.common.isUdbValid = (GT_U8)inFields[11];
    maskData.ruleExtNotIpv6.commonExt.isIpv6 = (GT_U8)inFields[12];
    maskData.ruleExtNotIpv6.commonExt.ipProtocol = (GT_U8)inFields[13];
    maskData.ruleExtNotIpv6.commonExt.dscp = (GT_U8)inFields[14];
    maskData.ruleExtNotIpv6.commonExt.isL4Valid = (GT_U8)inFields[15];
    maskData.ruleExtNotIpv6.commonExt.l4Byte0 = (GT_U8)inFields[16];
    maskData.ruleExtNotIpv6.commonExt.l4Byte1 = (GT_U8)inFields[17];
    maskData.ruleExtNotIpv6.commonExt.l4Byte2 = (GT_U8)inFields[18];
    maskData.ruleExtNotIpv6.commonExt.l4Byte3 = (GT_U8)inFields[19];
    maskData.ruleExtNotIpv6.commonExt.l4Byte13 = (GT_U8)inFields[20];
    maskData.ruleExtNotIpv6.commonExt.ipHeaderOk = (GT_U8)inFields[21];
    galtisIpAddr(&maskData.ruleExtNotIpv6.sip, (GT_U8*)inFields[22]);
    galtisIpAddr(&maskData.ruleExtNotIpv6.dip, (GT_U8*)inFields[23]);
    maskData.ruleExtNotIpv6.etherType = (GT_U16)inFields[24];
    maskData.ruleExtNotIpv6.l2Encap = (GT_U8)inFields[25];
    galtisMacAddr(&maskData.ruleExtNotIpv6.macDa, (GT_U8*)inFields[26]);
    galtisMacAddr(&maskData.ruleExtNotIpv6.macSa, (GT_U8*)inFields[27]);
    maskData.ruleExtNotIpv6.ipv4Fragmented = (GT_BOOL)inFields[28];
    galtisBArray(&maskBArr,(GT_U8*)inFields[29]);

    cmdOsMemCpy(
            maskData.ruleExtNotIpv6.udb, maskBArr.data,
            maskBArr.length);

    mask_set = GT_TRUE;
    }
    else /* pattern */
    {
    pattern_ruleIndex = (GT_U32)inFields[0];

    patternData.ruleExtNotIpv6.common.portListBmp = ruleSet_portListBmpPattern;

    patternData.ruleExtNotIpv6.common.pclId = (GT_U16)inFields[2];
    patternData.ruleExtNotIpv6.common.macToMe=(GT_U8)inFields[3];
    patternData.ruleExtNotIpv6.common.sourcePort = (GT_U8)inFields[4];

    devNum=(GT_U8)inArgs[0];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleExtNotIpv6.common.sourcePort);
    inArgs[0]=devNum;

    patternData.ruleExtNotIpv6.common.isTagged = (GT_U8)inFields[5];
    patternData.ruleExtNotIpv6.common.vid = (GT_U16)inFields[6];
    patternData.ruleExtNotIpv6.common.up = (GT_U8)inFields[7];
    patternData.ruleExtNotIpv6.common.qosProfile = (GT_U8)inFields[8];
    patternData.ruleExtNotIpv6.common.isIp = (GT_U8)inFields[9];
    patternData.ruleExtNotIpv6.common.isL2Valid = (GT_U8)inFields[10];
    patternData.ruleExtNotIpv6.common.isUdbValid = (GT_U8)inFields[11];
    patternData.ruleExtNotIpv6.commonExt.isIpv6 = (GT_U8)inFields[12];
    patternData.ruleExtNotIpv6.commonExt.ipProtocol = (GT_U8)inFields[13];
    patternData.ruleExtNotIpv6.commonExt.dscp = (GT_U8)inFields[14];
    patternData.ruleExtNotIpv6.commonExt.isL4Valid = (GT_U8)inFields[15];
    patternData.ruleExtNotIpv6.commonExt.l4Byte0 = (GT_U8)inFields[16];
    patternData.ruleExtNotIpv6.commonExt.l4Byte1 = (GT_U8)inFields[17];
    patternData.ruleExtNotIpv6.commonExt.l4Byte2 = (GT_U8)inFields[18];
    patternData.ruleExtNotIpv6.commonExt.l4Byte3 = (GT_U8)inFields[19];
    patternData.ruleExtNotIpv6.commonExt.l4Byte13 = (GT_U8)inFields[20];
    patternData.ruleExtNotIpv6.commonExt.ipHeaderOk = (GT_U8)inFields[21];
    galtisIpAddr(&patternData.ruleExtNotIpv6.sip, (GT_U8*)inFields[22]);
    galtisIpAddr(&patternData.ruleExtNotIpv6.dip, (GT_U8*)inFields[23]);
    patternData.ruleExtNotIpv6.etherType = (GT_U16)inFields[24];
    patternData.ruleExtNotIpv6.l2Encap = (GT_U8)inFields[25];
    galtisMacAddr(&patternData.ruleExtNotIpv6.macDa, (GT_U8*)inFields[26]);
    galtisMacAddr(&patternData.ruleExtNotIpv6.macSa, (GT_U8*)inFields[27]);
    patternData.ruleExtNotIpv6.ipv4Fragmented = (GT_BOOL)inFields[28];
    galtisBArray(&patternBArr,(GT_U8*)inFields[29]);

    cmdOsMemCpy(
            patternData.ruleExtNotIpv6.udb, patternBArr.data,
            patternBArr.length);
    pattern_set = GT_TRUE;
    }

    return CMD_OK;
}
/**********************************************************************************/

static CMD_STATUS wrCpssDxCh3PclIngressRule_EXT_IPV6_L2_Write
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY                       maskBArr, patternBArr;
    GT_U8           devNum;
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
    mask_ruleIndex = (GT_U32)inFields[0];

    maskData.ruleExtIpv6L2.common.portListBmp = ruleSet_portListBmpMask;

    maskData.ruleExtIpv6L2.common.pclId = (GT_U16)inFields[2];
    maskData.ruleExtIpv6L2.common.macToMe=(GT_U8)inFields[3];
    maskData.ruleExtIpv6L2.common.sourcePort = (GT_U8)inFields[4];

    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleExtIpv6L2.common.sourcePort);

    maskData.ruleExtIpv6L2.common.isTagged = (GT_U8)inFields[5];
    maskData.ruleExtIpv6L2.common.vid = (GT_U16)inFields[6];
    maskData.ruleExtIpv6L2.common.up = (GT_U8)inFields[7];
    maskData.ruleExtIpv6L2.common.qosProfile = (GT_U8)inFields[8];
    maskData.ruleExtIpv6L2.common.isIp = (GT_U8)inFields[9];
    maskData.ruleExtIpv6L2.common.isL2Valid = (GT_U8)inFields[10];
    maskData.ruleExtIpv6L2.common.isUdbValid = (GT_U8)inFields[11];
    maskData.ruleExtIpv6L2.commonExt.isIpv6 = (GT_U8)inFields[12];
    maskData.ruleExtIpv6L2.commonExt.ipProtocol = (GT_U8)inFields[13];
    maskData.ruleExtIpv6L2.commonExt.dscp = (GT_U8)inFields[14];
    maskData.ruleExtIpv6L2.commonExt.isL4Valid = (GT_U8)inFields[15];
    maskData.ruleExtIpv6L2.commonExt.l4Byte0 = (GT_U8)inFields[16];
    maskData.ruleExtIpv6L2.commonExt.l4Byte1 = (GT_U8)inFields[17];
    maskData.ruleExtIpv6L2.commonExt.l4Byte2 = (GT_U8)inFields[18];
    maskData.ruleExtIpv6L2.commonExt.l4Byte3 = (GT_U8)inFields[19];
    maskData.ruleExtIpv6L2.commonExt.l4Byte13 = (GT_U8)inFields[20];
    maskData.ruleExtIpv6L2.commonExt.ipHeaderOk = (GT_U8)inFields[21];
    galtisIpv6Addr(&maskData.ruleExtIpv6L2.sip, (GT_U8*)inFields[22]);
    maskData.ruleExtIpv6L2.dipBits127to120 = (GT_U8)inFields[23];
    maskData.ruleExtIpv6L2.isIpv6ExtHdrExist = (GT_BOOL)inFields[24];
    maskData.ruleExtIpv6L2.isIpv6HopByHop = (GT_BOOL)inFields[25];
    galtisMacAddr(&maskData.ruleExtIpv6L2.macDa, (GT_U8*)inFields[26]);
    galtisMacAddr(&maskData.ruleExtIpv6L2.macSa, (GT_U8*)inFields[27]);
    galtisBArray(&maskBArr,(GT_U8*)inFields[28]);

    cmdOsMemCpy(
            maskData.ruleExtIpv6L2.udb, maskBArr.data,
            maskBArr.length);

    mask_set = GT_TRUE;
    }
    else /* pattern */
    {
    pattern_ruleIndex = (GT_U32)inFields[0];

    patternData.ruleExtIpv6L2.common.portListBmp = ruleSet_portListBmpPattern;

    patternData.ruleExtIpv6L2.common.pclId = (GT_U16)inFields[2];
    patternData.ruleExtIpv6L2.common.macToMe=(GT_U8)inFields[3];
    patternData.ruleExtIpv6L2.common.sourcePort = (GT_U8)inFields[4];

    devNum=(GT_U8)inArgs[0];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleExtIpv6L2.common.sourcePort);
    inArgs[0]=devNum;

    patternData.ruleExtIpv6L2.common.isTagged = (GT_U8)inFields[5];
    patternData.ruleExtIpv6L2.common.vid = (GT_U16)inFields[6];
    patternData.ruleExtIpv6L2.common.up = (GT_U8)inFields[7];
    patternData.ruleExtIpv6L2.common.qosProfile = (GT_U8)inFields[8];
    patternData.ruleExtIpv6L2.common.isIp = (GT_U8)inFields[9];
    patternData.ruleExtIpv6L2.common.isL2Valid = (GT_U8)inFields[10];
    patternData.ruleExtIpv6L2.common.isUdbValid = (GT_U8)inFields[11];
    patternData.ruleExtIpv6L2.commonExt.isIpv6 = (GT_U8)inFields[12];
    patternData.ruleExtIpv6L2.commonExt.ipProtocol = (GT_U8)inFields[13];
    patternData.ruleExtIpv6L2.commonExt.dscp = (GT_U8)inFields[14];
    patternData.ruleExtIpv6L2.commonExt.isL4Valid = (GT_U8)inFields[15];
    patternData.ruleExtIpv6L2.commonExt.l4Byte0 = (GT_U8)inFields[16];
    patternData.ruleExtIpv6L2.commonExt.l4Byte1 = (GT_U8)inFields[17];
    patternData.ruleExtIpv6L2.commonExt.l4Byte2 = (GT_U8)inFields[18];
    patternData.ruleExtIpv6L2.commonExt.l4Byte3 = (GT_U8)inFields[19];
    patternData.ruleExtIpv6L2.commonExt.l4Byte13 = (GT_U8)inFields[20];
    patternData.ruleExtIpv6L2.commonExt.ipHeaderOk = (GT_U8)inFields[21];
    galtisIpv6Addr(&patternData.ruleExtIpv6L2.sip, (GT_U8*)inFields[22]);
    patternData.ruleExtIpv6L2.dipBits127to120 = (GT_U8)inFields[23];
    patternData.ruleExtIpv6L2.isIpv6ExtHdrExist = (GT_BOOL)inFields[24];
    patternData.ruleExtIpv6L2.isIpv6HopByHop = (GT_BOOL)inFields[25];
    galtisMacAddr(&patternData.ruleExtIpv6L2.macDa, (GT_U8*)inFields[26]);
    galtisMacAddr(&patternData.ruleExtIpv6L2.macSa, (GT_U8*)inFields[27]);
    galtisBArray(&patternBArr,(GT_U8*)inFields[28]);

    cmdOsMemCpy(
            patternData.ruleExtIpv6L2.udb, patternBArr.data,
            patternBArr.length);

    pattern_set = GT_TRUE;
    }

    return CMD_OK;
}
/**********************************************************************************/

static CMD_STATUS wrCpssDxCh3PclIngressRule_EXT_IPV6_L4_Write
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY                       maskBArr, patternBArr;
    GT_U8           devNum;
    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
    mask_ruleIndex = (GT_U32)inFields[0];

    maskData.ruleExtIpv6L4.common.portListBmp = ruleSet_portListBmpMask;

    maskData.ruleExtIpv6L4.common.pclId = (GT_U16)inFields[2];
    maskData.ruleExtIpv6L4.common.macToMe=(GT_U8)inFields[3];
    maskData.ruleExtIpv6L4.common.sourcePort = (GT_U8)inFields[4];

    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleExtIpv6L4.common.sourcePort);

    maskData.ruleExtIpv6L4.common.isTagged = (GT_U8)inFields[5];
    maskData.ruleExtIpv6L4.common.vid = (GT_U16)inFields[6];
    maskData.ruleExtIpv6L4.common.up = (GT_U8)inFields[7];
    maskData.ruleExtIpv6L4.common.qosProfile = (GT_U8)inFields[8];
    maskData.ruleExtIpv6L4.common.isIp = (GT_U8)inFields[9];
    maskData.ruleExtIpv6L4.common.isL2Valid = (GT_U8)inFields[10];
    maskData.ruleExtIpv6L4.common.isUdbValid = (GT_U8)inFields[11];
    maskData.ruleExtIpv6L4.commonExt.isIpv6 = (GT_U8)inFields[12];
    maskData.ruleExtIpv6L4.commonExt.ipProtocol = (GT_U8)inFields[13];
    maskData.ruleExtIpv6L4.commonExt.dscp = (GT_U8)inFields[14];
    maskData.ruleExtIpv6L4.commonExt.isL4Valid = (GT_U8)inFields[15];
    maskData.ruleExtIpv6L4.commonExt.l4Byte0 = (GT_U8)inFields[16];
    maskData.ruleExtIpv6L4.commonExt.l4Byte1 = (GT_U8)inFields[17];
    maskData.ruleExtIpv6L4.commonExt.l4Byte2 = (GT_U8)inFields[18];
    maskData.ruleExtIpv6L4.commonExt.l4Byte3 = (GT_U8)inFields[19];
    maskData.ruleExtIpv6L4.commonExt.l4Byte13 = (GT_U8)inFields[20];
    maskData.ruleExtIpv6L4.commonExt.ipHeaderOk = (GT_U8)inFields[21];
    galtisIpv6Addr(&maskData.ruleExtIpv6L4.sip, (GT_U8*)inFields[22]);
    galtisIpv6Addr(&maskData.ruleExtIpv6L4.dip, (GT_U8*)inFields[23]);
    maskData.ruleExtIpv6L4.isIpv6ExtHdrExist = (GT_U8)inFields[24];
    maskData.ruleExtIpv6L4.isIpv6HopByHop = (GT_U8)inFields[25];
    galtisBArray(&maskBArr,(GT_U8*)inFields[26]);

    cmdOsMemCpy(
            maskData.ruleExtIpv6L4.udb, maskBArr.data,
            maskBArr.length);

    mask_set = GT_TRUE;
    }
    else /* pattern */
    {
    pattern_ruleIndex = (GT_U32)inFields[0];

    patternData.ruleExtIpv6L4.common.portListBmp = ruleSet_portListBmpPattern;

    patternData.ruleExtIpv6L4.common.pclId = (GT_U16)inFields[2];
    patternData.ruleExtIpv6L4.common.macToMe=(GT_U8)inFields[3];
    patternData.ruleExtIpv6L4.common.sourcePort = (GT_U8)inFields[4];

    devNum=(GT_U8)inArgs[0];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleExtIpv6L4.common.sourcePort);
    inArgs[0]=devNum;

    patternData.ruleExtIpv6L4.common.isTagged = (GT_U8)inFields[5];
    patternData.ruleExtIpv6L4.common.vid = (GT_U16)inFields[6];
    patternData.ruleExtIpv6L4.common.up = (GT_U8)inFields[7];
    patternData.ruleExtIpv6L4.common.qosProfile = (GT_U8)inFields[8];
    patternData.ruleExtIpv6L4.common.isIp = (GT_U8)inFields[9];
    patternData.ruleExtIpv6L4.common.isL2Valid = (GT_U8)inFields[10];
    patternData.ruleExtIpv6L4.common.isUdbValid = (GT_U8)inFields[11];
    patternData.ruleExtIpv6L4.commonExt.isIpv6 = (GT_U8)inFields[12];
    patternData.ruleExtIpv6L4.commonExt.ipProtocol = (GT_U8)inFields[13];
    patternData.ruleExtIpv6L4.commonExt.dscp = (GT_U8)inFields[14];
    patternData.ruleExtIpv6L4.commonExt.isL4Valid = (GT_U8)inFields[15];
    patternData.ruleExtIpv6L4.commonExt.l4Byte0 = (GT_U8)inFields[16];
    patternData.ruleExtIpv6L4.commonExt.l4Byte1 = (GT_U8)inFields[17];
    patternData.ruleExtIpv6L4.commonExt.l4Byte2 = (GT_U8)inFields[18];
    patternData.ruleExtIpv6L4.commonExt.l4Byte3 = (GT_U8)inFields[19];
    patternData.ruleExtIpv6L4.commonExt.l4Byte13 = (GT_U8)inFields[20];
    patternData.ruleExtIpv6L4.commonExt.ipHeaderOk = (GT_U8)inFields[21];
    galtisIpv6Addr(&patternData.ruleExtIpv6L4.sip, (GT_U8*)inFields[22]);
    galtisIpv6Addr(&patternData.ruleExtIpv6L4.dip, (GT_U8*)inFields[23]);
    patternData.ruleExtIpv6L4.isIpv6ExtHdrExist = (GT_U8)inFields[24];
    patternData.ruleExtIpv6L4.isIpv6HopByHop = (GT_U8)inFields[25];
    galtisBArray(&patternBArr,(GT_U8*)inFields[26]);


    cmdOsMemCpy(
            patternData.ruleExtIpv6L4.udb, patternBArr.data,
            patternBArr.length);


    pattern_set = GT_TRUE;
    }

    return CMD_OK;
}
/*********************************************************************************/

static CMD_STATUS  wrCpssDxCh3PclIngressRuleWriteFirst

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

    cpssOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));


    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;

    switch (inArgs[1])
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            wrCpssDxCh3PclIngressRule_STD_NOT_IP_Write(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            wrCpssDxCh3PclIngressRule_STD_IP_L2_QOS_Write(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            wrCpssDxCh3PclIngressRule_STD_IPV4_L4_Write(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
            wrCpssDxCh3PclIngressRule_STD_IPV6_DIP_Write(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            wrCpssDxCh3PclIngressRule_EXT_NOT_IPV6_Write(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            wrCpssDxCh3PclIngressRule_EXT_IPV6_L2_Write(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            wrCpssDxCh3PclIngressRule_EXT_IPV6_L4_Write(
                inArgs, inFields ,numFields ,outArgs);
            break;

        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong PCL RULE FORMAT.\n");
            return CMD_AGENT_ERROR;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}
/*************************************************************************/
static CMD_STATUS wrCpssDxCh3PclIngressRuleWriteNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result = GT_OK;
    GT_U8           devNum;
    CPSS_DXCH_PCL_ACTION_STC         *actionPtr;
    GT_U32                              actionIndex;
    GT_U32                              ruleIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


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
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of Ingress {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if(inFields[1] == 1 && pattern_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of Ingress {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }



    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    switch (inArgs[1])
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            wrCpssDxCh3PclIngressRule_STD_NOT_IP_Write(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            wrCpssDxCh3PclIngressRule_STD_IP_L2_QOS_Write(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            wrCpssDxCh3PclIngressRule_STD_IPV4_L4_Write(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
            wrCpssDxCh3PclIngressRule_STD_IPV6_DIP_Write(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            wrCpssDxCh3PclIngressRule_EXT_NOT_IPV6_Write(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            wrCpssDxCh3PclIngressRule_EXT_IPV6_L2_Write(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            wrCpssDxCh3PclIngressRule_EXT_IPV6_L4_Write(
                inArgs, inFields ,numFields ,outArgs);
            break;

        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong PCL RULE FORMAT.\n");
            return CMD_AGENT_ERROR;
    }
    if(mask_set && pattern_set && (mask_ruleIndex != pattern_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }

    /* Get the action */
    ruleIndex = inFields[0];
    pclDxCh3ActionGet( ruleIndex, &actionIndex);

    if(actionIndex == INVALID_RULE_INDEX || !(actionTable[actionIndex].valid))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Action table entry is not set.\n");
        return CMD_AGENT_ERROR;

    }

    if(mask_set && pattern_set)
    {
         /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];

        actionPtr = &(actionTable[actionIndex].actionEntry);
        lastActionPtr = actionPtr;

        /* call cpss api function */
        result = pg_wrap_cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, &maskData,
                                                    &patternData, actionPtr);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        /* don't clear pattern and mask - could be reused */

    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*************************************************************************/
static CMD_STATUS wrCpssDxCh3PclIngressRuleWriteEnd

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
  if(mask_set || pattern_set)
      /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/****************************Ingress UDB stylesd keys Table*******************/

/* returns reached field index */
static GT_U32 prvWrCpssDxChXCatPclUdbCommonSet
(
    IN  GT_32                                            inFields[],
    IN  GT_32                                            startFieldIndex,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC *comPtr
)
{
    GT_U32 i; /* field index */

    i = startFieldIndex;

    comPtr->pclId           = (GT_U16) inFields[i++];
    comPtr->macToMe         = (GT_U8)  inFields[i++];
    comPtr->sourcePort      = (GT_U8)  inFields[i++];
    comPtr->vid             = (GT_U16) inFields[i++];
    comPtr->up              = (GT_U8)  inFields[i++];
    comPtr->isIp            = (GT_U8)  inFields[i++];
    comPtr->dscpOrExp       = (GT_U8)  inFields[i++];
    comPtr->isL2Valid       = (GT_U8)  inFields[i++];
    comPtr->isUdbValid      = (GT_U8)  inFields[i++];
    comPtr->pktTagging      = (GT_U8)  inFields[i++];
    comPtr->l3OffsetInvalid = (GT_U8)  inFields[i++];
    comPtr->l4ProtocolType  = (GT_U8)  inFields[i++];
    comPtr->pktType         = (GT_U8)  inFields[i++];
    comPtr->ipHeaderOk      = (GT_U8)  inFields[i++];
    comPtr->macDaType       = (GT_U8)  inFields[i++];
    comPtr->l4OffsetInalid  = (GT_U8)  inFields[i++];
    comPtr->l2Encapsulation = (GT_U8)  inFields[i++];
    comPtr->isIpv6Eh        = (GT_U8)  inFields[i++];
    comPtr->isIpv6HopByHop  = (GT_U8)  inFields[i++];

    /* set port list */
    comPtr->portListBmp = (inFields[1] == 0) ? ruleSet_portListBmpMask : ruleSet_portListBmpPattern;

    return i;
}

/* returns reached field index */
static GT_U32 prvWrCpssDxChXCatPclUdbStdSet
(
    IN  GT_32                                              inFields[],
    IN  GT_32                                              startFieldIndex,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC *keyPtr
)
{
    GT_U32 i;           /* field index */
    GT_BYTE_ARRY bArr;  /* binary data */

    i = prvWrCpssDxChXCatPclUdbCommonSet(
        inFields,
        startFieldIndex,
        &(keyPtr->commonIngrUdb));

    keyPtr->isIpv4 = (GT_BOOL)inFields[i++];

    galtisBArray(&bArr,(GT_U8*)inFields[i++]);
    cmdOsMemCpy(keyPtr->udb, bArr.data, bArr.length);
    galtisBArrayFree(&bArr);

    return i;
}

/* returns reached field index */
static GT_U32 prvWrCpssDxChXCatPclUdbExtSet
(
    IN  GT_32                                              inFields[],
    IN  GT_32                                              startFieldIndex,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC *keyPtr
)
{
    GT_U32 i;           /* field index */
    GT_BYTE_ARRY bArr;  /* binary data */
    GT_ETHERADDR mac;   /* mac address */
    GT_IPADDR    ipAddr;/* IP address  */

    i = prvWrCpssDxChXCatPclUdbCommonSet(
        inFields,
        startFieldIndex,
        &(keyPtr->commonIngrUdb));

    keyPtr->isIpv6 = (GT_BOOL)inFields[i++];

    keyPtr->ipProtocol  = (GT_U8)  inFields[i++];

    galtisIpAddr(&ipAddr, (GT_U8*)inFields[i++]);
    cmdOsMemCpy(keyPtr->sipBits31_0, ipAddr.arIP, 4);

    galtisMacAddr(&mac,  (GT_U8*)inFields[i++]);
    cmdOsMemCpy(keyPtr->sipBits79_32orMacSa, mac.arEther, 6);

    galtisMacAddr(&mac, (GT_U8*)inFields[i++]);
    cmdOsMemCpy(keyPtr->sipBits127_80orMacDa, mac.arEther, 6);

    galtisIpAddr(&ipAddr, (GT_U8*)inFields[i++]);
    cmdOsMemCpy(keyPtr->dipBits127_112, ipAddr.arIP, 2);

    galtisIpAddr(&ipAddr, (GT_U8*)inFields[i++]);
    cmdOsMemCpy(keyPtr->dipBits31_0, ipAddr.arIP, 4);

    galtisBArray(&bArr,(GT_U8*)inFields[i++]);
    cmdOsMemCpy(keyPtr->udb, bArr.data, bArr.length);
    galtisBArrayFree(&bArr);

    return i;
}

static CMD_STATUS  wrCpssDxChPclXCatUDBRuleWriteNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_PCL_RULE_FORMAT_UNT *untPtr;
    GT_STATUS                     result = GT_OK;
    GT_U8                         devNum;
    CPSS_DXCH_PCL_ACTION_STC      *actionPtr;
    GT_U32                        actionIndex;
    GT_U32                        ruleIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
        if (mask_set != GT_FALSE)
        {
            galtisOutput(outArgs, GT_BAD_STATE, "");
            return CMD_OK;
        }
        mask_ruleIndex = (GT_U32)inFields[0];
        mask_set    = GT_TRUE;
        untPtr = &maskData;
    }
    else
    {
        if (pattern_set != GT_FALSE)
        {
            galtisOutput(outArgs, GT_BAD_STATE, "");
            return CMD_OK;
        }
        pattern_ruleIndex = (GT_U32)inFields[0];
        pattern_set = GT_TRUE;
        untPtr = &patternData;
    }

    switch (inArgs[1])
    {
        case 0:
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E;
            prvWrCpssDxChXCatPclUdbStdSet(
                inFields, 2, &(untPtr->ruleIngrStdUdb));

            if(inFields[1] == 0) /* mask */
            {
                CONVERT_PCL_MASK_PCL_ID_TEST_TO_CPSS_MAC(maskData.ruleIngrStdUdb.commonIngrUdb.pclId);
            }

            break;

        case 1:
            ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E;
            prvWrCpssDxChXCatPclUdbExtSet(
                inFields, 2, &(untPtr->ruleIngrExtUdb));
            break;

        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong PCL RULE FORMAT.\n");
            return CMD_AGENT_ERROR;
    }

    if (mask_set && pattern_set)
    {
        if (mask_ruleIndex != pattern_ruleIndex)
        {
            galtisOutput(outArgs, GT_BAD_STATE, "");
            return CMD_OK;
        }

        ruleIndex = mask_ruleIndex;

         /* map input arguments to locals */
        devNum  = (GT_U8)inArgs[0];

        pclDxCh3ActionGet(ruleIndex, &actionIndex);

        if(actionIndex == INVALID_RULE_INDEX || !(actionTable[actionIndex].valid))
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Action table entry is not set.\n");
            return CMD_AGENT_ERROR;

        }

        actionPtr = &(actionTable[actionIndex].actionEntry);
        lastActionPtr = actionPtr;

        /* call cpss api function */
        result = pg_wrap_cpssDxChPclRuleSet(
            devNum, ruleFormat, ruleIndex,
            &maskData, &patternData, actionPtr);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS  wrCpssDxChPclXCatUDBRuleWriteFirst
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

    cpssOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cpssOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));


    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;

    return wrCpssDxChPclXCatUDBRuleWriteNext(
        inArgs, inFields, numFields, outArgs);

}

static void epclCommonEgrPktTypeInfoSet
(
    INOUT CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC *commonPtr,
    IN GT_32   fieldsArray[]/* relevant fields are 10..20*/
)
{
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC *patternCommonPtr =
        &patternData.ruleEgrStdNotIp.common;/* all egress rules starts with the 'common'*/
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC *maskCommonPtr =
        &maskData.ruleEgrStdNotIp.common;/* all egress rules starts with the 'common'*/
    GT_TRUNK_ID trunkId;

    switch(patternCommonPtr->egrPacketType){/* switch according to patternData */
    case 0:
        commonPtr->egrPktType.toCpu.cpuCode=(GT_U8)fieldsArray[10];
        commonPtr->egrPktType.toCpu.srcTrg=(GT_U8)fieldsArray[11];
        break;
    case 1:
        commonPtr->egrPktType.fromCpu.tc=(GT_U8)fieldsArray[12];
        commonPtr->egrPktType.fromCpu.dp=(GT_U8)fieldsArray[13];
        commonPtr->egrPktType.fromCpu.egrFilterEnable=(GT_U8)fieldsArray[14];
        break;
    case 2:
        commonPtr->egrPktType.toAnalyzer.rxSniff=(GT_U8)fieldsArray[15];
        break;
    case 3:
        commonPtr->egrPktType.fwdData.qosProfile=(GT_U8)fieldsArray[16];
        commonPtr->egrPktType.fwdData.srcTrunkId=(GT_U8)fieldsArray[17];
        commonPtr->egrPktType.fwdData.srcIsTrunk=(GT_U8)fieldsArray[18];
        commonPtr->egrPktType.fwdData.isUnknown=(GT_U8)fieldsArray[19];
        commonPtr->egrPktType.fwdData.isRouted=(GT_U8)fieldsArray[20];

        trunkId = commonPtr->egrPktType.fwdData.srcTrunkId;
        if(commonPtr == maskCommonPtr)
        {
            /* we do the mask now */
            gtPclMaskTrunkConvertFromTestToCpss(
                maskCommonPtr->egrPktType.fwdData.srcIsTrunk,
                patternCommonPtr->egrPktType.fwdData.srcIsTrunk,
                &trunkId);
        }
        else
        {
            /* we do the pattern now */
            gtPclPatternTrunkConvertFromTestToCpss(
                maskCommonPtr->egrPktType.fwdData.srcIsTrunk,
                patternCommonPtr->egrPktType.fwdData.srcIsTrunk,
                &trunkId);
        }

        commonPtr->egrPktType.fwdData.srcTrunkId = (GT_U8)trunkId;


        break;
    }

}


/****************************Egress Table*************************************/
static CMD_STATUS wrCpssDxCh3PclEgressRule_EGRESS_STD_NOT_IP_E
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8           devNum;
    GT_U32   ii;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    devNum=(GT_U8)inArgs[0];

    if(inFields[1] == 0) /* mask */
    {
    mask_ruleIndex = (GT_U32)inFields[0];

    maskData.ruleEgrStdNotIp.common.portListBmp = ruleSet_portListBmpMask;

    /*start of common*/
    maskData.ruleEgrStdNotIp.common.pclId = (GT_U16)inFields[2];
    maskData.ruleEgrStdNotIp.common.sourcePort = (GT_U8)inFields[3];
    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleStdNotIp.common.sourcePort);


    maskData.ruleEgrStdNotIp.common.isTagged = (GT_U8)inFields[4];
    maskData.ruleEgrStdNotIp.common.vid = (GT_U16)inFields[5];
    maskData.ruleEgrStdNotIp.common.up = (GT_U8)inFields[6];

    maskData.ruleEgrStdNotIp.common.isIp = (GT_U8)inFields[7];
    maskData.ruleEgrStdNotIp.common.isL2Valid = (GT_U8)inFields[8];
    maskData.ruleEgrStdNotIp.common.egrPacketType=(GT_U8)inFields[9];

    /*egrPacketType
      0 - packet to CPU
      1 - packet from CPU
      2 - packet to ANALYZER
      3 - forward DATA packet
    */

    if(pattern_set==GT_TRUE)
    {
        epclCommonEgrPktTypeInfoSet(&maskData.ruleEgrStdNotIp.common,
            inFields);
        epclCommonEgrPktTypeInfoSet(&patternData.ruleEgrStdNotIp.common,
            ruleDataWords);
    }
    else
    {
        /* save the info from mask and set it properly when the pattern is set */
        for(ii = 10 ; ii <= 20 ; ii++)
        {
            ruleDataWords[ii] = inFields[ii];
        }
    }

    maskData.ruleEgrStdNotIp.common.srcDev= (GT_U8)inFields[21];
    maskData.ruleEgrStdNotIp.common.sourceId=(GT_U8)inFields[22];
    maskData.ruleEgrStdNotIp.common.isVidx=(GT_U8)inFields[23];

    /*end of common*/

    maskData.ruleEgrStdNotIp.isIpv4 = (GT_U8)inFields[24];
    maskData.ruleEgrStdNotIp.etherType = (GT_U16)inFields[25];
    maskData.ruleEgrStdNotIp.isArp = (GT_U8)inFields[26];
    maskData.ruleEgrStdNotIp.l2Encap = (GT_BOOL)inFields[27];
    galtisMacAddr(&maskData.ruleEgrStdNotIp.macDa, (GT_U8*)inFields[28]);
    galtisMacAddr(&maskData.ruleEgrStdNotIp.macSa, (GT_U8*)inFields[29]);


    mask_set = GT_TRUE;
    }
    else /* pattern */
    {
    pattern_ruleIndex = (GT_U32)inFields[0];

    patternData.ruleEgrStdNotIp.common.portListBmp = ruleSet_portListBmpPattern;

    /*start of common*/
    patternData.ruleEgrStdNotIp.common.pclId = (GT_U16)inFields[2];
    patternData.ruleEgrStdNotIp.common.sourcePort = (GT_U8)inFields[3];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleStdNotIp.common.sourcePort);


    patternData.ruleEgrStdNotIp.common.isTagged = (GT_U8)inFields[4];
    patternData.ruleEgrStdNotIp.common.vid = (GT_U16)inFields[5];
    patternData.ruleEgrStdNotIp.common.up = (GT_U8)inFields[6];

    patternData.ruleEgrStdNotIp.common.isIp = (GT_U8)inFields[7];
    patternData.ruleEgrStdNotIp.common.isL2Valid = (GT_U8)inFields[8];
    patternData.ruleEgrStdNotIp.common.egrPacketType=(GT_U8)inFields[9];

    /*egrPacketType
      0 - packet to CPU
      1 - packet from CPU
      2 - packet to ANALYZER
      3 - forward DATA packet
    */

    if(mask_set == GT_TRUE)
    {
        epclCommonEgrPktTypeInfoSet(&maskData.ruleEgrStdNotIp.common,
            ruleDataWords);
        epclCommonEgrPktTypeInfoSet(&patternData.ruleEgrStdNotIp.common,
            inFields);
    }
    else
    {
        /* save the info from mask and set it properly when the pattern is set */
        for(ii = 10 ; ii <= 20 ; ii++)
        {
            ruleDataWords[ii] = inFields[ii];
        }
    }

    patternData.ruleEgrStdNotIp.common.srcDev= (GT_U8)inFields[21];
    patternData.ruleEgrStdNotIp.common.sourceId=(GT_U8)inFields[22];
    patternData.ruleEgrStdNotIp.common.isVidx=(GT_U8)inFields[23];

    /*end of common*/

    patternData.ruleEgrStdNotIp.isIpv4 = (GT_U8)inFields[24];
    patternData.ruleEgrStdNotIp.etherType = (GT_U16)inFields[25];
    patternData.ruleEgrStdNotIp.isArp = (GT_U8)inFields[26];
    patternData.ruleEgrStdNotIp.l2Encap = (GT_BOOL)inFields[27];
    galtisMacAddr(&patternData.ruleEgrStdNotIp.macDa, (GT_U8*)inFields[28]);
    galtisMacAddr(&patternData.ruleEgrStdNotIp.macSa, (GT_U8*)inFields[29]);


    pattern_set = GT_TRUE;
    }

    return CMD_OK;
}
static CMD_STATUS wrCpssDxCh3PclEgressRule_EGRESS_STD_IP_L2_QOS_E
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8           devNum;
    GT_BYTE_ARRY                       maskBArr, patternBArr;
    GT_U32   ii;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    devNum=(GT_U8)inArgs[0];

    if(inFields[1] == 0) /* mask */
    {
    mask_ruleIndex = (GT_U32)inFields[0];

    maskData.ruleEgrStdIpL2Qos.common.portListBmp = ruleSet_portListBmpMask;

    /*start of common*/
    maskData.ruleEgrStdIpL2Qos.common.pclId = (GT_U16)inFields[2];
    maskData.ruleEgrStdIpL2Qos.common.sourcePort = (GT_U8)inFields[3];
    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleEgrStdIpL2Qos.common.sourcePort);


    maskData.ruleEgrStdIpL2Qos.common.isTagged = (GT_U8)inFields[4];
    maskData.ruleEgrStdIpL2Qos.common.vid = (GT_U16)inFields[5];
    maskData.ruleEgrStdIpL2Qos.common.up = (GT_U8)inFields[6];

    maskData.ruleEgrStdIpL2Qos.common.isIp = (GT_U8)inFields[7];
    maskData.ruleEgrStdIpL2Qos.common.isL2Valid = (GT_U8)inFields[8];
    maskData.ruleEgrStdIpL2Qos.common.egrPacketType=(GT_U8)inFields[9];

    /*egrPacketType
      0 - packet to CPU
      1 - packet from CPU
      2 - packet to ANALYZER
      3 - forward DATA packet
    */

    if(pattern_set==GT_TRUE)
    {
        epclCommonEgrPktTypeInfoSet(&maskData.ruleEgrStdIpL2Qos.common,
            inFields);
        epclCommonEgrPktTypeInfoSet(&patternData.ruleEgrStdIpL2Qos.common,
            ruleDataWords);
    }
    else
    {
        /* save the info from mask and set it properly when the pattern is set */
        for(ii = 10 ; ii <= 20 ; ii++)
        {
            ruleDataWords[ii] = inFields[ii];
        }
    }

    maskData.ruleEgrStdIpL2Qos.common.srcDev= (GT_U8)inFields[21];
    maskData.ruleEgrStdIpL2Qos.common.sourceId=(GT_U8)inFields[22];
    maskData.ruleEgrStdIpL2Qos.common.isVidx=(GT_U8)inFields[23];

    /*end of common*/




    maskData.ruleEgrStdIpL2Qos.commonStdIp.isIpv4 = (GT_U8)inFields[24];
    maskData.ruleEgrStdIpL2Qos.commonStdIp.ipProtocol = (GT_U8)inFields[25];
    maskData.ruleEgrStdIpL2Qos.commonStdIp.dscp = (GT_U8)inFields[26];
    maskData.ruleEgrStdIpL2Qos.commonStdIp.isL4Valid = (GT_U8)inFields[27];
    maskData.ruleEgrStdIpL2Qos.commonStdIp.l4Byte2 = (GT_U8)inFields[28];
    maskData.ruleEgrStdIpL2Qos.commonStdIp.l4Byte3 = (GT_U8)inFields[29];
    maskData.ruleEgrStdIpL2Qos.commonStdIp.ipv4Fragmented = (GT_U8)inFields[30];
    maskData.ruleEgrStdIpL2Qos.commonStdIp.egrTcpUdpPortComparator= (GT_U8)inFields[31];
    maskData.ruleEgrStdIpL2Qos.isArp = (GT_U8)inFields[32];
    galtisMacAddr(&maskData.ruleEgrStdIpL2Qos.macDa, (GT_U8*)inFields[33]);
    maskData.ruleEgrStdIpL2Qos.l4Byte13=(GT_U8)inFields[34];
    galtisBArray(&maskBArr,(GT_U8*)inFields[35]);
    cmdOsMemCpy(
            maskData.ruleEgrStdIpL2Qos.dipBits0to31, maskBArr.data,
            maskBArr.length);

        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
    pattern_ruleIndex = (GT_U32)inFields[0];

    patternData.ruleEgrStdIpL2Qos.common.portListBmp = ruleSet_portListBmpPattern;

    /*start of common*/
    patternData.ruleEgrStdIpL2Qos.common.pclId = (GT_U16)inFields[2];
    patternData.ruleEgrStdIpL2Qos.common.sourcePort = (GT_U8)inFields[3];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleEgrStdIpL2Qos.common.sourcePort);


    patternData.ruleEgrStdIpL2Qos.common.isTagged = (GT_U8)inFields[4];
    patternData.ruleEgrStdIpL2Qos.common.vid = (GT_U16)inFields[5];
    patternData.ruleEgrStdIpL2Qos.common.up = (GT_U8)inFields[6];

    patternData.ruleEgrStdIpL2Qos.common.isIp = (GT_U8)inFields[7];
    patternData.ruleEgrStdIpL2Qos.common.isL2Valid = (GT_U8)inFields[8];
    patternData.ruleEgrStdIpL2Qos.common.egrPacketType=(GT_U8)inFields[9];

    /*egrPacketType
      0 - packet to CPU
      1 - packet from CPU
      2 - packet to ANALYZER
      3 - forward DATA packet
    */

    if(mask_set == GT_TRUE)
    {
        epclCommonEgrPktTypeInfoSet(&maskData.ruleEgrStdIpL2Qos.common,
            ruleDataWords);
        epclCommonEgrPktTypeInfoSet(&patternData.ruleEgrStdIpL2Qos.common,
            inFields);
    }
    else
    {
        /* save the info from mask and set it properly when the pattern is set */
        for(ii = 10 ; ii <= 20 ; ii++)
        {
            ruleDataWords[ii] = inFields[ii];
        }
    }

    patternData.ruleEgrStdIpL2Qos.common.srcDev= (GT_U8)inFields[21];
    patternData.ruleEgrStdIpL2Qos.common.sourceId=(GT_U8)inFields[22];
    patternData.ruleEgrStdIpL2Qos.common.isVidx=(GT_U8)inFields[23];

    /*end of common*/




    patternData.ruleEgrStdIpL2Qos.commonStdIp.isIpv4 = (GT_U8)inFields[24];
    patternData.ruleEgrStdIpL2Qos.commonStdIp.ipProtocol = (GT_U8)inFields[25];
    patternData.ruleEgrStdIpL2Qos.commonStdIp.dscp = (GT_U8)inFields[26];
    patternData.ruleEgrStdIpL2Qos.commonStdIp.isL4Valid = (GT_U8)inFields[27];
    patternData.ruleEgrStdIpL2Qos.commonStdIp.l4Byte2 = (GT_U8)inFields[28];
    patternData.ruleEgrStdIpL2Qos.commonStdIp.l4Byte3 = (GT_U8)inFields[29];
    patternData.ruleEgrStdIpL2Qos.commonStdIp.ipv4Fragmented = (GT_U8)inFields[30];
    patternData.ruleEgrStdIpL2Qos.commonStdIp.egrTcpUdpPortComparator= (GT_U8)inFields[31];
    patternData.ruleEgrStdIpL2Qos.isArp = (GT_U8)inFields[32];
    galtisMacAddr(&patternData.ruleEgrStdIpL2Qos.macDa, (GT_U8*)inFields[33]);
    patternData.ruleEgrStdIpL2Qos.l4Byte13=(GT_U8)inFields[34];
    galtisBArray(&patternBArr,(GT_U8*)inFields[35]);
    cmdOsMemCpy(
            patternData.ruleEgrStdIpL2Qos.dipBits0to31, patternBArr.data,
            patternBArr.length);

        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}

static CMD_STATUS wrCpssDxCh3PclEgressRule_EGRESS_STD_IPV4_L4_E
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8           devNum;
    GT_U32   ii;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
    mask_ruleIndex = (GT_U32)inFields[0];

    maskData.ruleEgrStdIpv4L4.common.portListBmp = ruleSet_portListBmpMask;

    /*start of common*/
    maskData.ruleEgrStdIpv4L4.common.pclId = (GT_U16)inFields[2];
    maskData.ruleEgrStdIpv4L4.common.sourcePort = (GT_U8)inFields[3];
    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleEgrStdIpv4L4.common.sourcePort);


    maskData.ruleEgrStdIpv4L4.common.isTagged = (GT_U8)inFields[4];
    maskData.ruleEgrStdIpv4L4.common.vid = (GT_U16)inFields[5];
    maskData.ruleEgrStdIpv4L4.common.up = (GT_U8)inFields[6];

    maskData.ruleEgrStdIpv4L4.common.isIp = (GT_U8)inFields[7];
    maskData.ruleEgrStdIpv4L4.common.isL2Valid = (GT_U8)inFields[8];
    maskData.ruleEgrStdIpv4L4.common.egrPacketType=(GT_U8)inFields[9];

    /*egrPacketType
      0 - packet to CPU
      1 - packet from CPU
      2 - packet to ANALYZER
      3 - forward DATA packet
    */

    if(pattern_set==GT_TRUE)
    {
        epclCommonEgrPktTypeInfoSet(&maskData.ruleEgrStdIpv4L4.common,
            inFields);
        epclCommonEgrPktTypeInfoSet(&patternData.ruleEgrStdIpv4L4.common,
            ruleDataWords);
    }
    else
    {
        /* save the info from mask and set it properly when the pattern is set */
        for(ii = 10 ; ii <= 20 ; ii++)
        {
            ruleDataWords[ii] = inFields[ii];
        }
    }

    maskData.ruleEgrStdIpv4L4.common.srcDev= (GT_U8)inFields[21];
    maskData.ruleEgrStdIpv4L4.common.sourceId=(GT_U8)inFields[22];
    maskData.ruleEgrStdIpv4L4.common.isVidx=(GT_U8)inFields[23];

    /*end of common*/

    maskData.ruleEgrStdIpv4L4.commonStdIp.isIpv4 = (GT_U8)inFields[24];
    maskData.ruleEgrStdIpv4L4.commonStdIp.ipProtocol = (GT_U8)inFields[25];
    maskData.ruleEgrStdIpv4L4.commonStdIp.dscp = (GT_U8)inFields[26];
    maskData.ruleEgrStdIpv4L4.commonStdIp.isL4Valid = (GT_U8)inFields[27];
    maskData.ruleEgrStdIpv4L4.commonStdIp.l4Byte2 = (GT_U8)inFields[28];
    maskData.ruleEgrStdIpv4L4.commonStdIp.l4Byte3 = (GT_U8)inFields[29];
    maskData.ruleEgrStdIpv4L4.commonStdIp.ipv4Fragmented = (GT_U8)inFields[30];
    maskData.ruleEgrStdIpv4L4.commonStdIp.egrTcpUdpPortComparator= (GT_U8)inFields[31];
    maskData.ruleEgrStdIpv4L4.isArp = (GT_U8)inFields[32];
    maskData.ruleEgrStdIpv4L4.isBc = (GT_U8)inFields[33];
    galtisIpAddr(&maskData.ruleEgrStdIpv4L4.sip, (GT_U8*)inFields[34]);
    galtisIpAddr(&maskData.ruleEgrStdIpv4L4.dip, (GT_U8*)inFields[35]);
    maskData.ruleEgrStdIpv4L4.l4Byte0 = (GT_U8)inFields[36];
    maskData.ruleEgrStdIpv4L4.l4Byte1 = (GT_U8)inFields[37];
    maskData.ruleEgrStdIpv4L4.l4Byte13 = (GT_U8)inFields[38];


    mask_set = GT_TRUE;
    }
    else /* pattern */
    {
   pattern_ruleIndex = (GT_U32)inFields[0];

   patternData.ruleEgrStdIpv4L4.common.portListBmp = ruleSet_portListBmpPattern;

   /*start of common*/
    patternData.ruleEgrStdIpv4L4.common.pclId = (GT_U16)inFields[2];
    patternData.ruleEgrStdIpv4L4.common.sourcePort = (GT_U8)inFields[3];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleEgrStdIpv4L4.common.sourcePort);


    patternData.ruleEgrStdIpv4L4.common.isTagged = (GT_U8)inFields[4];
    patternData.ruleEgrStdIpv4L4.common.vid = (GT_U16)inFields[5];
    patternData.ruleEgrStdIpv4L4.common.up = (GT_U8)inFields[6];

    patternData.ruleEgrStdIpv4L4.common.isIp = (GT_U8)inFields[7];
    patternData.ruleEgrStdIpv4L4.common.isL2Valid = (GT_U8)inFields[8];
    patternData.ruleEgrStdIpv4L4.common.egrPacketType=(GT_U8)inFields[9];

    /*egrPacketType
      0 - packet to CPU
      1 - packet from CPU
      2 - packet to ANALYZER
      3 - forward DATA packet
    */

    if(mask_set == GT_TRUE)
    {
        epclCommonEgrPktTypeInfoSet(&maskData.ruleEgrStdIpv4L4.common,
            ruleDataWords);
        epclCommonEgrPktTypeInfoSet(&patternData.ruleEgrStdIpv4L4.common,
            inFields);
    }
    else
    {
        /* save the info from mask and set it properly when the pattern is set */
        for(ii = 10 ; ii <= 20 ; ii++)
        {
            ruleDataWords[ii] = inFields[ii];
        }
    }


    patternData.ruleEgrStdIpv4L4.common.srcDev= (GT_U8)inFields[21];
    patternData.ruleEgrStdIpv4L4.common.sourceId=(GT_U8)inFields[22];
    patternData.ruleEgrStdIpv4L4.common.isVidx=(GT_U8)inFields[23];

    /*end of common*/

    patternData.ruleEgrStdIpv4L4.commonStdIp.isIpv4 = (GT_U8)inFields[24];
    patternData.ruleEgrStdIpv4L4.commonStdIp.ipProtocol = (GT_U8)inFields[25];
    patternData.ruleEgrStdIpv4L4.commonStdIp.dscp = (GT_U8)inFields[26];
    patternData.ruleEgrStdIpv4L4.commonStdIp.isL4Valid = (GT_U8)inFields[27];
    patternData.ruleEgrStdIpv4L4.commonStdIp.l4Byte2 = (GT_U8)inFields[28];
    patternData.ruleEgrStdIpv4L4.commonStdIp.l4Byte3 = (GT_U8)inFields[29];
    patternData.ruleEgrStdIpv4L4.commonStdIp.ipv4Fragmented = (GT_U8)inFields[30];
    patternData.ruleEgrStdIpv4L4.commonStdIp.egrTcpUdpPortComparator= (GT_U8)inFields[31];
    patternData.ruleEgrStdIpv4L4.isArp = (GT_U8)inFields[32];
    patternData.ruleEgrStdIpv4L4.isBc = (GT_U8)inFields[33];
    galtisIpAddr(&patternData.ruleEgrStdIpv4L4.sip, (GT_U8*)inFields[34]);
    galtisIpAddr(&patternData.ruleEgrStdIpv4L4.dip, (GT_U8*)inFields[35]);
    patternData.ruleEgrStdIpv4L4.l4Byte0 = (GT_U8)inFields[36];
    patternData.ruleEgrStdIpv4L4.l4Byte1 = (GT_U8)inFields[37];
    patternData.ruleEgrStdIpv4L4.l4Byte13 = (GT_U8)inFields[38];


    pattern_set = GT_TRUE;
    }

    return CMD_OK;
}
/**********************************************************************************/

static CMD_STATUS wrCpssDxCh3PclEgressRule_EGRESS_EXT_NOT_IPV6_E
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U8           devNum;
    GT_U32   ii;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
    mask_ruleIndex = (GT_U32)inFields[0];

    maskData.ruleEgrExtNotIpv6.common.portListBmp = ruleSet_portListBmpMask;

    /*start of common*/
    maskData.ruleEgrExtNotIpv6.common.pclId = (GT_U16)inFields[2];
    maskData.ruleEgrExtNotIpv6.common.sourcePort = (GT_U8)inFields[3];
    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleEgrExtNotIpv6.common.sourcePort);


    maskData.ruleEgrExtNotIpv6.common.isTagged = (GT_U8)inFields[4];
    maskData.ruleEgrExtNotIpv6.common.vid = (GT_U16)inFields[5];
    maskData.ruleEgrExtNotIpv6.common.up = (GT_U8)inFields[6];

    maskData.ruleEgrExtNotIpv6.common.isIp = (GT_U8)inFields[7];
    maskData.ruleEgrExtNotIpv6.common.isL2Valid = (GT_U8)inFields[8];
    maskData.ruleEgrExtNotIpv6.common.egrPacketType=(GT_U8)inFields[9];

    /*egrPacketType
      0 - packet to CPU
      1 - packet from CPU
      2 - packet to ANALYZER
      3 - forward DATA packet
    */

    if(pattern_set==GT_TRUE)
    {
        epclCommonEgrPktTypeInfoSet(&maskData.ruleEgrExtNotIpv6.common,
            inFields);
        epclCommonEgrPktTypeInfoSet(&patternData.ruleEgrExtNotIpv6.common,
            ruleDataWords);
    }
    else
    {
        /* save the info from mask and set it properly when the pattern is set */
        for(ii = 10 ; ii <= 20 ; ii++)
        {
            ruleDataWords[ii] = inFields[ii];
        }
    }

    maskData.ruleEgrExtNotIpv6.common.srcDev= (GT_U8)inFields[21];
    maskData.ruleEgrExtNotIpv6.common.sourceId=(GT_U8)inFields[22];
    maskData.ruleEgrExtNotIpv6.common.isVidx=(GT_U8)inFields[23];

    /*end of common*/

    maskData.ruleEgrExtNotIpv6.commonExt.isIpv6 = (GT_U8)inFields[24];
    maskData.ruleEgrExtNotIpv6.commonExt.ipProtocol = (GT_U8)inFields[25];
    maskData.ruleEgrExtNotIpv6.commonExt.dscp = (GT_U8)inFields[26];
    maskData.ruleEgrExtNotIpv6.commonExt.isL4Valid = (GT_U8)inFields[27];
    maskData.ruleEgrExtNotIpv6.commonExt.l4Byte0 = (GT_U8)inFields[28];
    maskData.ruleEgrExtNotIpv6.commonExt.l4Byte1 = (GT_U8)inFields[29];
    maskData.ruleEgrExtNotIpv6.commonExt.l4Byte2 = (GT_U8)inFields[30];
    maskData.ruleEgrExtNotIpv6.commonExt.l4Byte3 = (GT_U8)inFields[31];
    maskData.ruleEgrExtNotIpv6.commonExt.l4Byte13 = (GT_U8)inFields[32];
    maskData.ruleEgrExtNotIpv6.commonExt.egrTcpUdpPortComparator = (GT_U8)inFields[33];
    galtisIpAddr(&maskData.ruleEgrExtNotIpv6.sip, (GT_U8*)inFields[34]);
    galtisIpAddr(&maskData.ruleEgrExtNotIpv6.dip, (GT_U8*)inFields[35]);
    maskData.ruleEgrExtNotIpv6.etherType = (GT_U16)inFields[36];
    maskData.ruleEgrExtNotIpv6.l2Encap = (GT_U8)inFields[37];
    galtisMacAddr(&maskData.ruleEgrExtNotIpv6.macDa, (GT_U8*)inFields[38]);
    galtisMacAddr(&maskData.ruleEgrExtNotIpv6.macSa, (GT_U8*)inFields[39]);
    maskData.ruleEgrExtNotIpv6.ipv4Fragmented = (GT_U8)inFields[40];


    mask_set = GT_TRUE;
    }
    else /* pattern */
    {
    pattern_ruleIndex = (GT_U32)inFields[0];

    patternData.ruleEgrExtNotIpv6.common.portListBmp = ruleSet_portListBmpPattern;

 /*start of common*/
    patternData.ruleEgrExtNotIpv6.common.pclId = (GT_U16)inFields[2];
    patternData.ruleEgrExtNotIpv6.common.sourcePort = (GT_U8)inFields[3];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleEgrExtNotIpv6.common.sourcePort);


    patternData.ruleEgrExtNotIpv6.common.isTagged = (GT_U8)inFields[4];
    patternData.ruleEgrExtNotIpv6.common.vid = (GT_U16)inFields[5];
    patternData.ruleEgrExtNotIpv6.common.up = (GT_U8)inFields[6];

    patternData.ruleEgrExtNotIpv6.common.isIp = (GT_U8)inFields[7];
    patternData.ruleEgrExtNotIpv6.common.isL2Valid = (GT_U8)inFields[8];
    patternData.ruleEgrExtNotIpv6.common.egrPacketType=(GT_U8)inFields[9];

    /*egrPacketType
      0 - packet to CPU
      1 - packet from CPU
      2 - packet to ANALYZER
      3 - forward DATA packet
    */

    if(mask_set == GT_TRUE)
    {
        epclCommonEgrPktTypeInfoSet(&maskData.ruleEgrExtNotIpv6.common,
            ruleDataWords);
        epclCommonEgrPktTypeInfoSet(&patternData.ruleEgrExtNotIpv6.common,
            inFields);
    }
    else
    {
        /* save the info from mask and set it properly when the pattern is set */
        for(ii = 10 ; ii <= 20 ; ii++)
        {
            ruleDataWords[ii] = inFields[ii];
        }
    }

    patternData.ruleEgrExtNotIpv6.common.srcDev= (GT_U8)inFields[21];
    patternData.ruleEgrExtNotIpv6.common.sourceId=(GT_U8)inFields[22];
    patternData.ruleEgrExtNotIpv6.common.isVidx=(GT_U8)inFields[23];

    /*end of common*/

    patternData.ruleEgrExtNotIpv6.commonExt.isIpv6 = (GT_U8)inFields[24];
    patternData.ruleEgrExtNotIpv6.commonExt.ipProtocol = (GT_U8)inFields[25];
    patternData.ruleEgrExtNotIpv6.commonExt.dscp = (GT_U8)inFields[26];
    patternData.ruleEgrExtNotIpv6.commonExt.isL4Valid = (GT_U8)inFields[27];
    patternData.ruleEgrExtNotIpv6.commonExt.l4Byte0 = (GT_U8)inFields[28];
    patternData.ruleEgrExtNotIpv6.commonExt.l4Byte1 = (GT_U8)inFields[29];
    patternData.ruleEgrExtNotIpv6.commonExt.l4Byte2 = (GT_U8)inFields[30];
    patternData.ruleEgrExtNotIpv6.commonExt.l4Byte3 = (GT_U8)inFields[31];
    patternData.ruleEgrExtNotIpv6.commonExt.l4Byte13 = (GT_U8)inFields[32];
    patternData.ruleEgrExtNotIpv6.commonExt.egrTcpUdpPortComparator = (GT_U8)inFields[33];
    galtisIpAddr(&patternData.ruleEgrExtNotIpv6.sip, (GT_U8*)inFields[34]);
    galtisIpAddr(&patternData.ruleEgrExtNotIpv6.dip, (GT_U8*)inFields[35]);
    patternData.ruleEgrExtNotIpv6.etherType = (GT_U16)inFields[36];
    patternData.ruleEgrExtNotIpv6.l2Encap = (GT_U8)inFields[37];
    galtisMacAddr(&patternData.ruleEgrExtNotIpv6.macDa, (GT_U8*)inFields[38]);
    galtisMacAddr(&patternData.ruleEgrExtNotIpv6.macSa, (GT_U8*)inFields[39]);
    patternData.ruleEgrExtNotIpv6.ipv4Fragmented = (GT_U8)inFields[40];



    pattern_set = GT_TRUE;
    }

    return CMD_OK;
}

static CMD_STATUS wrCpssDxCh3PclEgressRule_EGRESS_EXT_IPV6_L2_E
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U8           devNum;
    GT_U32   ii;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
    mask_ruleIndex = (GT_U32)inFields[0];

    maskData.ruleEgrExtIpv6L2.common.portListBmp = ruleSet_portListBmpMask;

    /*start of common*/
    maskData.ruleEgrExtIpv6L2.common.pclId = (GT_U16)inFields[2];
    maskData.ruleEgrExtIpv6L2.common.sourcePort = (GT_U8)inFields[3];
    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleEgrExtIpv6L2.common.sourcePort);


    maskData.ruleEgrExtIpv6L2.common.isTagged = (GT_U8)inFields[4];
    maskData.ruleEgrExtIpv6L2.common.vid = (GT_U16)inFields[5];
    maskData.ruleEgrExtIpv6L2.common.up = (GT_U8)inFields[6];

    maskData.ruleEgrExtIpv6L2.common.isIp = (GT_U8)inFields[7];
    maskData.ruleEgrExtIpv6L2.common.isL2Valid = (GT_U8)inFields[8];
    maskData.ruleEgrExtIpv6L2.common.egrPacketType=(GT_U8)inFields[9];

    /*egrPacketType
      0 - packet to CPU
      1 - packet from CPU
      2 - packet to ANALYZER
      3 - forward DATA packet
    */

    if(pattern_set==GT_TRUE)
    {
        epclCommonEgrPktTypeInfoSet(&maskData.ruleEgrExtIpv6L2.common,
            inFields);
        epclCommonEgrPktTypeInfoSet(&patternData.ruleEgrExtIpv6L2.common,
            ruleDataWords);
    }
    else
    {
        /* save the info from mask and set it properly when the pattern is set */
        for(ii = 10 ; ii <= 20 ; ii++)
        {
            ruleDataWords[ii] = inFields[ii];
        }
    }


    maskData.ruleEgrExtIpv6L2.common.srcDev= (GT_U8)inFields[21];
    maskData.ruleEgrExtIpv6L2.common.sourceId=(GT_U8)inFields[22];
    maskData.ruleEgrExtIpv6L2.common.isVidx=(GT_U8)inFields[23];

    /*end of common*/

    maskData.ruleEgrExtIpv6L2.commonExt.isIpv6 = (GT_U8)inFields[24];
    maskData.ruleEgrExtIpv6L2.commonExt.ipProtocol = (GT_U8)inFields[25];
    maskData.ruleEgrExtIpv6L2.commonExt.dscp = (GT_U8)inFields[26];
    maskData.ruleEgrExtIpv6L2.commonExt.isL4Valid = (GT_U8)inFields[27];
    maskData.ruleEgrExtIpv6L2.commonExt.l4Byte0 = (GT_U8)inFields[28];
    maskData.ruleEgrExtIpv6L2.commonExt.l4Byte1 = (GT_U8)inFields[29];
    maskData.ruleEgrExtIpv6L2.commonExt.l4Byte2 = (GT_U8)inFields[30];
    maskData.ruleEgrExtIpv6L2.commonExt.l4Byte3 = (GT_U8)inFields[31];
    maskData.ruleEgrExtIpv6L2.commonExt.l4Byte13 = (GT_U8)inFields[32];
    maskData.ruleEgrExtIpv6L2.commonExt.egrTcpUdpPortComparator = (GT_U8)inFields[33];
    galtisIpv6Addr(&maskData.ruleEgrExtIpv6L2.sip, (GT_U8*)inFields[34]);
    maskData.ruleEgrExtIpv6L2.dipBits127to120 = (GT_U8)inFields[35];

    galtisMacAddr(&maskData.ruleEgrExtIpv6L2.macDa, (GT_U8*)inFields[36]);
    galtisMacAddr(&maskData.ruleEgrExtIpv6L2.macSa, (GT_U8*)inFields[37]);




    mask_set = GT_TRUE;
    }
    else /* pattern */
    {
    pattern_ruleIndex = (GT_U32)inFields[0];

    patternData.ruleEgrExtIpv6L2.common.portListBmp = ruleSet_portListBmpPattern;

    /*start of common*/
    patternData.ruleEgrExtIpv6L2.common.pclId = (GT_U16)inFields[2];
    patternData.ruleEgrExtIpv6L2.common.sourcePort = (GT_U8)inFields[3];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleEgrExtIpv6L2.common.sourcePort);


    patternData.ruleEgrExtIpv6L2.common.isTagged = (GT_U8)inFields[4];
    patternData.ruleEgrExtIpv6L2.common.vid = (GT_U16)inFields[5];
    patternData.ruleEgrExtIpv6L2.common.up = (GT_U8)inFields[6];

    patternData.ruleEgrExtIpv6L2.common.isIp = (GT_U8)inFields[7];
    patternData.ruleEgrExtIpv6L2.common.isL2Valid = (GT_U8)inFields[8];
    patternData.ruleEgrExtIpv6L2.common.egrPacketType=(GT_U8)inFields[9];

    /*egrPacketType
      0 - packet to CPU
      1 - packet from CPU
      2 - packet to ANALYZER
      3 - forward DATA packet
    */

    if(mask_set == GT_TRUE)
    {
        epclCommonEgrPktTypeInfoSet(&maskData.ruleEgrExtIpv6L2.common,
            ruleDataWords);
        epclCommonEgrPktTypeInfoSet(&patternData.ruleEgrExtIpv6L2.common,
            inFields);
    }
    else
    {
        /* save the info from mask and set it properly when the pattern is set */
        for(ii = 10 ; ii <= 20 ; ii++)
        {
            ruleDataWords[ii] = inFields[ii];
        }
    }

    patternData.ruleEgrExtIpv6L2.common.srcDev= (GT_U8)inFields[21];
    patternData.ruleEgrExtIpv6L2.common.sourceId=(GT_U8)inFields[22];
    patternData.ruleEgrExtIpv6L2.common.isVidx=(GT_U8)inFields[23];

    /*end of common*/

    patternData.ruleEgrExtIpv6L2.commonExt.isIpv6 = (GT_U8)inFields[24];
    patternData.ruleEgrExtIpv6L2.commonExt.ipProtocol = (GT_U8)inFields[25];
    patternData.ruleEgrExtIpv6L2.commonExt.dscp = (GT_U8)inFields[26];
    patternData.ruleEgrExtIpv6L2.commonExt.isL4Valid = (GT_U8)inFields[27];
    patternData.ruleEgrExtIpv6L2.commonExt.l4Byte0 = (GT_U8)inFields[28];
    patternData.ruleEgrExtIpv6L2.commonExt.l4Byte1 = (GT_U8)inFields[29];
    patternData.ruleEgrExtIpv6L2.commonExt.l4Byte2 = (GT_U8)inFields[30];
    patternData.ruleEgrExtIpv6L2.commonExt.l4Byte3 = (GT_U8)inFields[31];
    patternData.ruleEgrExtIpv6L2.commonExt.l4Byte13 = (GT_U8)inFields[32];
    patternData.ruleEgrExtIpv6L2.commonExt.egrTcpUdpPortComparator = (GT_U8)inFields[33];
    galtisIpv6Addr(&patternData.ruleEgrExtIpv6L2.sip, (GT_U8*)inFields[34]);
    patternData.ruleEgrExtIpv6L2.dipBits127to120 = (GT_U8)inFields[35];

    galtisMacAddr(&patternData.ruleEgrExtIpv6L2.macDa, (GT_U8*)inFields[36]);
    galtisMacAddr(&patternData.ruleEgrExtIpv6L2.macSa, (GT_U8*)inFields[37]);


    pattern_set = GT_TRUE;
    }

    return CMD_OK;
}
static CMD_STATUS wrCpssDxCh3PclEgressRule_EGRESS_EXT_IPV6_L4_E
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8           devNum;
    GT_U32   ii;

    ruleFormat = CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
    mask_ruleIndex = (GT_U32)inFields[0];

    maskData.ruleEgrExtIpv6L4.common.portListBmp = ruleSet_portListBmpMask;

   /*start of common*/
    maskData.ruleEgrExtIpv6L4.common.pclId = (GT_U16)inFields[2];
    maskData.ruleEgrExtIpv6L4.common.sourcePort = (GT_U8)inFields[3];
    CONVERT_DEV_PORT_MAC(devNum, maskData.ruleEgrExtIpv6L2.common.sourcePort);


    maskData.ruleEgrExtIpv6L4.common.isTagged = (GT_U8)inFields[4];
    maskData.ruleEgrExtIpv6L4.common.vid = (GT_U16)inFields[5];
    maskData.ruleEgrExtIpv6L4.common.up = (GT_U8)inFields[6];

    maskData.ruleEgrExtIpv6L4.common.isIp = (GT_U8)inFields[7];
    maskData.ruleEgrExtIpv6L4.common.isL2Valid = (GT_U8)inFields[8];
    maskData.ruleEgrExtIpv6L4.common.egrPacketType=(GT_U8)inFields[9];

    /*egrPacketType
      0 - packet to CPU
      1 - packet from CPU
      2 - packet to ANALYZER
      3 - forward DATA packet
    */

    if(pattern_set==GT_TRUE)
    {
        epclCommonEgrPktTypeInfoSet(&maskData.ruleEgrExtIpv6L4.common,
            inFields);
        epclCommonEgrPktTypeInfoSet(&patternData.ruleEgrExtIpv6L4.common,
            ruleDataWords);
    }
    else
    {
        /* save the info from mask and set it properly when the pattern is set */
        for(ii = 10 ; ii <= 20 ; ii++)
        {
            ruleDataWords[ii] = inFields[ii];
        }
    }


    maskData.ruleEgrExtIpv6L4.common.srcDev= (GT_U8)inFields[21];
    maskData.ruleEgrExtIpv6L4.common.sourceId=(GT_U8)inFields[22];
    maskData.ruleEgrExtIpv6L4.common.isVidx=(GT_U8)inFields[23];

    /*end of common*/

    maskData.ruleEgrExtIpv6L4.commonExt.isIpv6 = (GT_U8)inFields[24];
    maskData.ruleEgrExtIpv6L4.commonExt.ipProtocol = (GT_U8)inFields[25];
    maskData.ruleEgrExtIpv6L4.commonExt.dscp = (GT_U8)inFields[26];
    maskData.ruleEgrExtIpv6L4.commonExt.isL4Valid = (GT_U8)inFields[27];
    maskData.ruleEgrExtIpv6L4.commonExt.l4Byte0 = (GT_U8)inFields[28];
    maskData.ruleEgrExtIpv6L4.commonExt.l4Byte1 = (GT_U8)inFields[29];
    maskData.ruleEgrExtIpv6L4.commonExt.l4Byte2 = (GT_U8)inFields[30];
    maskData.ruleEgrExtIpv6L4.commonExt.l4Byte3 = (GT_U8)inFields[31];
    maskData.ruleEgrExtIpv6L4.commonExt.l4Byte13 = (GT_U8)inFields[32];
    maskData.ruleEgrExtIpv6L4.commonExt.egrTcpUdpPortComparator = (GT_U8)inFields[33];

    galtisIpv6Addr(&maskData.ruleEgrExtIpv6L4.sip, (GT_U8*)inFields[34]);
    galtisIpv6Addr(&maskData.ruleEgrExtIpv6L4.dip, (GT_U8*)inFields[35]);


    mask_set = GT_TRUE;
    }
    else /* pattern */
    {
    pattern_ruleIndex = (GT_U32)inFields[0];

    patternData.ruleEgrExtIpv6L4.common.portListBmp = ruleSet_portListBmpPattern;

    /*start of common*/
    patternData.ruleEgrExtIpv6L4.common.pclId = (GT_U16)inFields[2];
    patternData.ruleEgrExtIpv6L4.common.sourcePort = (GT_U8)inFields[3];
    CONVERT_DEV_PORT_MAC(devNum, patternData.ruleEgrExtIpv6L2.common.sourcePort);


    patternData.ruleEgrExtIpv6L4.common.isTagged = (GT_U8)inFields[4];
    patternData.ruleEgrExtIpv6L4.common.vid = (GT_U16)inFields[5];
    patternData.ruleEgrExtIpv6L4.common.up = (GT_U8)inFields[6];

    patternData.ruleEgrExtIpv6L4.common.isIp = (GT_U8)inFields[7];
    patternData.ruleEgrExtIpv6L4.common.isL2Valid = (GT_U8)inFields[8];
    patternData.ruleEgrExtIpv6L4.common.egrPacketType=(GT_U8)inFields[9];

    /*egrPacketType
      0 - packet to CPU
      1 - packet from CPU
      2 - packet to ANALYZER
      3 - forward DATA packet
    */

    if(mask_set == GT_TRUE)
    {
        epclCommonEgrPktTypeInfoSet(&maskData.ruleEgrExtIpv6L4.common,
            ruleDataWords);
        epclCommonEgrPktTypeInfoSet(&patternData.ruleEgrExtIpv6L4.common,
            inFields);
    }
    else
    {
        /* save the info from mask and set it properly when the pattern is set */
        for(ii = 10 ; ii <= 20 ; ii++)
        {
            ruleDataWords[ii] = inFields[ii];
        }
    }


    patternData.ruleEgrExtIpv6L4.common.srcDev= (GT_U8)inFields[21];
    patternData.ruleEgrExtIpv6L4.common.sourceId=(GT_U8)inFields[22];
    patternData.ruleEgrExtIpv6L4.common.isVidx=(GT_U8)inFields[23];

    /*end of common*/

    patternData.ruleEgrExtIpv6L4.commonExt.isIpv6 = (GT_U8)inFields[24];
    patternData.ruleEgrExtIpv6L4.commonExt.ipProtocol = (GT_U8)inFields[25];
    patternData.ruleEgrExtIpv6L4.commonExt.dscp = (GT_U8)inFields[26];
    patternData.ruleEgrExtIpv6L4.commonExt.isL4Valid = (GT_U8)inFields[27];
    patternData.ruleEgrExtIpv6L4.commonExt.l4Byte0 = (GT_U8)inFields[28];
    patternData.ruleEgrExtIpv6L4.commonExt.l4Byte1 = (GT_U8)inFields[29];
    patternData.ruleEgrExtIpv6L4.commonExt.l4Byte2 = (GT_U8)inFields[30];
    patternData.ruleEgrExtIpv6L4.commonExt.l4Byte3 = (GT_U8)inFields[31];
    patternData.ruleEgrExtIpv6L4.commonExt.l4Byte13 = (GT_U8)inFields[32];
    patternData.ruleEgrExtIpv6L4.commonExt.egrTcpUdpPortComparator = (GT_U8)inFields[33];

    galtisIpv6Addr(&patternData.ruleEgrExtIpv6L4.sip, (GT_U8*)inFields[34]);
    galtisIpv6Addr(&patternData.ruleEgrExtIpv6L4.dip, (GT_U8*)inFields[35]);

    pattern_set = GT_TRUE;
    }

    return CMD_OK;
}
/*********************************************************************************/

static CMD_STATUS  wrCpssDxCh3PclEgressRuleWriteFirst

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

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_PCL_RULE_FORMAT_UNT));


    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;
   switch (inArgs[1]+7)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
            wrCpssDxCh3PclEgressRule_EGRESS_STD_NOT_IP_E(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
            wrCpssDxCh3PclEgressRule_EGRESS_STD_IP_L2_QOS_E(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
            wrCpssDxCh3PclEgressRule_EGRESS_STD_IPV4_L4_E(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
            wrCpssDxCh3PclEgressRule_EGRESS_EXT_NOT_IPV6_E(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
            wrCpssDxCh3PclEgressRule_EGRESS_EXT_IPV6_L2_E(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
            wrCpssDxCh3PclEgressRule_EGRESS_EXT_IPV6_L4_E(
                inArgs, inFields ,numFields ,outArgs);
            break;

       default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong PCL RULE FORMAT.\n");
            return CMD_AGENT_ERROR;
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}
/*************************************************************************/
static CMD_STATUS wrCpssDxCh3PclEgressRuleWriteNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result = GT_OK;
    GT_U8           devNum;
    CPSS_DXCH_PCL_ACTION_STC         *actionPtr;
    GT_U32                              actionIndex;
    GT_U32                              ruleIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


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
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of Ingress {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if(inFields[1] == 1 && pattern_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of Ingress {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }



    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    switch (inArgs[1]+7)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
            wrCpssDxCh3PclEgressRule_EGRESS_STD_NOT_IP_E(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
            wrCpssDxCh3PclEgressRule_EGRESS_STD_IP_L2_QOS_E(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
            wrCpssDxCh3PclEgressRule_EGRESS_STD_IPV4_L4_E(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
            wrCpssDxCh3PclEgressRule_EGRESS_EXT_NOT_IPV6_E(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
            wrCpssDxCh3PclEgressRule_EGRESS_EXT_IPV6_L2_E(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
            wrCpssDxCh3PclEgressRule_EGRESS_EXT_IPV6_L4_E(
                inArgs, inFields ,numFields ,outArgs);
            break;

       default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong PCL RULE FORMAT.\n");
            return CMD_AGENT_ERROR;
    }
    if(mask_set && pattern_set && (mask_ruleIndex != pattern_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }

    /* Get the action */
    ruleIndex = inFields[0];
    pclDxCh3ActionGet( ruleIndex, &actionIndex);

    if(actionIndex == INVALID_RULE_INDEX || !(actionTable[actionIndex].valid))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Action table entry is not set.\n");
        return CMD_AGENT_ERROR;

    }

    if(mask_set && pattern_set)
    {
         /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];

        actionPtr = &(actionTable[actionIndex].actionEntry);
        lastActionPtr = actionPtr;

        /* call cpss api function */
        result = pg_wrap_cpssDxChPclRuleSet(devNum, ruleFormat, ruleIndex, &maskData,
                                                    &patternData, actionPtr);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        /* maskData, patternData, lastActionPtr cleared only at */
        /* wrCpssDxCh3PclEgressRuleWriteFirst                   */
        /* Keeped for use in utilites                           */
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*************************************************************************/
static CMD_STATUS wrCpssDxCh3PclEgressRuleWriteEnd

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
  if(mask_set || pattern_set)
      /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/****************************end of egress*****************************/

/* XCat UDB Table */

/*******************************************************************************
* wrCpssDxChPclUserDefinedByteTableSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for and above DxChXCat devices
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*       offsetType   - the type of offset (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_OUT_OF_RANGE          - parameter value more then HW bit field
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclUserDefinedByteTableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                                devNum;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat;
    CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType;
    GT_U32                               udbIndex;
    CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType;
    GT_U8                                offset;

    devNum      = (GT_U8)inArgs[0];
    ruleFormat  = (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inArgs[1];
    packetType  = (CPSS_DXCH_PCL_PACKET_TYPE_ENT)inArgs[2];
    udbIndex    = (GT_U32)inFields[0];
    offsetType  = (CPSS_DXCH_PCL_OFFSET_TYPE_ENT)inFields[1];
    offset      = (GT_U8)inFields[2];

    rc =  pg_wrap_cpssDxChPclUserDefinedByteSet(
        devNum, ruleFormat, packetType,
        udbIndex, offsetType, offset);

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/* index for Get first/next */
static GT_U32  udbTableIndex;

/*******************************************************************************
* wrCpssDxChPclUserDefinedByteTableGetNext
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for DxChXCat and above devices
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*                         (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclUserDefinedByteTableGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                                devNum;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT   ruleFormat;
    CPSS_DXCH_PCL_PACKET_TYPE_ENT        packetType;
    CPSS_DXCH_PCL_OFFSET_TYPE_ENT        offsetType;
    GT_U8                                offset;

    devNum      = (GT_U8)inArgs[0];
    ruleFormat  = (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inArgs[1];
    packetType  = (CPSS_DXCH_PCL_PACKET_TYPE_ENT)inArgs[2];
    offsetType  = (CPSS_DXCH_PCL_OFFSET_TYPE_ENT)0;
    offset      = (GT_U8)0;

    rc =  pg_wrap_cpssDxChPclUserDefinedByteGet(
        devNum, ruleFormat, packetType,
        udbTableIndex, &offsetType, &offset);

    if ((rc != GT_OK) && (udbTableIndex != 0))
    {
        /* notify "no records more" */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    fieldOutput("%d%d%d", udbTableIndex, offsetType, offset);
    galtisOutput(outArgs, rc, "%f");

    /* increment index */
    udbTableIndex ++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPclUserDefinedByteTableGetFirst
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*                      Relevant for DxCh1, DxCh2, DxCh3 devices
*       packetType   - packet Type
*                      Relevant for DxChXCat and above devices
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*                         (see CPSS_DXCH_PCL_OFFSET_TYPE_ENT)
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       See comments to CPSS_DXCH_PCL_OFFSET_TYPE_ENT
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclUserDefinedByteTableGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    udbTableIndex = 0;

    return wrCpssDxChPclUserDefinedByteTableGetNext(
        inArgs, inFields, numFields, outArgs);
}

/*******************************************************************************
* wrCpssDxChPclOverrideUserDefinedBytesSet
*
* DESCRIPTION:
*   The function sets Override User Defined Bytes
*
* APPLICABLE DEVICES:  All DxChXCat devices
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclOverrideUserDefinedBytesSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                    devNum;
    CPSS_DXCH_PCL_OVERRIDE_UDB_STC           udbOverride;
    GT_STATUS                                result;
    GT_U32                                   i;

    i = 0;
    cpssOsMemSet(&udbOverride, 0, sizeof(udbOverride));

    devNum = (GT_U8)inArgs[i++];
    udbOverride.vrfIdLsbEnableStdNotIp      = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdMsbEnableStdNotIp      = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdLsbEnableStdIpL2Qos    = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdMsbEnableStdIpL2Qos    = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdLsbEnableStdIpv4L4     = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdMsbEnableStdIpv4L4     = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdLsbEnableStdUdb        = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdMsbEnableStdUdb        = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdLsbEnableExtNotIpv6    = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdMsbEnableExtNotIpv6    = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdLsbEnableExtIpv6L2     = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdMsbEnableExtIpv6L2     = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdLsbEnableExtIpv6L4     = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdMsbEnableExtIpv6L4     = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdLsbEnableExtUdb        = (GT_BOOL)inArgs[i++];
    udbOverride.vrfIdMsbEnableExtUdb        = (GT_BOOL)inArgs[i++];
    udbOverride.qosProfileEnableStdUdb      = (GT_BOOL)inArgs[i++];
    udbOverride.qosProfileEnableExtUdb      = (GT_BOOL)inArgs[i++];

    result = pg_wrap_cpssDxChPclOverrideUserDefinedBytesSet(
        devNum, &udbOverride);

    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPclOverrideUserDefinedBytesGet
*
* DESCRIPTION:
*   The function gets Override User Defined Bytes
*
* APPLICABLE DEVICES:  All DxChXCat devices
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclOverrideUserDefinedBytesGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                    devNum;
    CPSS_DXCH_PCL_OVERRIDE_UDB_STC           udbOverride;
    GT_STATUS                                result;

    devNum = (GT_U8)inArgs[0];
    cpssOsMemSet(&udbOverride, 0, sizeof(udbOverride));

    result = pg_wrap_cpssDxChPclOverrideUserDefinedBytesGet(
        devNum, &udbOverride);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    galtisOutput(
        outArgs, GT_OK, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
        udbOverride.vrfIdLsbEnableStdNotIp,
        udbOverride.vrfIdMsbEnableStdNotIp,
        udbOverride.vrfIdLsbEnableStdIpL2Qos,
        udbOverride.vrfIdMsbEnableStdIpL2Qos,
        udbOverride.vrfIdLsbEnableStdIpv4L4,
        udbOverride.vrfIdMsbEnableStdIpv4L4,
        udbOverride.vrfIdLsbEnableStdUdb,
        udbOverride.vrfIdMsbEnableStdUdb,
        udbOverride.vrfIdLsbEnableExtNotIpv6,
        udbOverride.vrfIdMsbEnableExtNotIpv6,
        udbOverride.vrfIdLsbEnableExtIpv6L2,
        udbOverride.vrfIdMsbEnableExtIpv6L2,
        udbOverride.vrfIdLsbEnableExtIpv6L4,
        udbOverride.vrfIdMsbEnableExtIpv6L4,
        udbOverride.vrfIdLsbEnableExtUdb,
        udbOverride.vrfIdMsbEnableExtUdb,
        udbOverride.qosProfileEnableStdUdb,
        udbOverride.qosProfileEnableExtUdb);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet
*
* DESCRIPTION:
*   The function sets overriding of  User Defined Bytes
*   by packets Trunk Hash value.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum              - device number
*       udbOverTrunkHashPtr - (pointer to) UDB override trunk hash structure
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclOverrideUserDefinedBytesByTrunkHashSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                      devNum;
    CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC  udbOverTrunkHash;
    GT_STATUS                                  result;
    GT_U32                                     i;

    i = 0;
    cpssOsMemSet(&udbOverTrunkHash, 0, sizeof(udbOverTrunkHash));

    devNum = (GT_U8)inArgs[i++];
    udbOverTrunkHash.trunkHashEnableStdNotIp      = (GT_BOOL)inArgs[i++];
    udbOverTrunkHash.trunkHashEnableStdIpv4L4     = (GT_BOOL)inArgs[i++];
    udbOverTrunkHash.trunkHashEnableStdUdb        = (GT_BOOL)inArgs[i++];
    udbOverTrunkHash.trunkHashEnableExtNotIpv6    = (GT_BOOL)inArgs[i++];
    udbOverTrunkHash.trunkHashEnableExtIpv6L2     = (GT_BOOL)inArgs[i++];
    udbOverTrunkHash.trunkHashEnableExtIpv6L4     = (GT_BOOL)inArgs[i++];
    udbOverTrunkHash.trunkHashEnableExtUdb        = (GT_BOOL)inArgs[i++];

    result = pg_wrap_cpssDxChPclOverrideUserDefinedBytesByTrunkHashSet(
        devNum, &udbOverTrunkHash);

    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet
*
* DESCRIPTION:
*   The function gets overriding of  User Defined Bytes
*   by packets Trunk Hash value.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum              - device number
* OUTPUTS:
*       udbOverTrunkHashPtr - (pointer to) UDB override trunk hash structure
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclOverrideUserDefinedBytesByTrunkHashGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                      devNum;
    CPSS_DXCH_PCL_OVERRIDE_UDB_TRUNK_HASH_STC  udbOverTrunkHash;
    GT_STATUS                                  result;

    devNum = (GT_U8)inArgs[0];
    cpssOsMemSet(&udbOverTrunkHash, 0, sizeof(udbOverTrunkHash));

    result = pg_wrap_cpssDxChPclOverrideUserDefinedBytesByTrunkHashGet(
        devNum, &udbOverTrunkHash);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    galtisOutput(
        outArgs, GT_OK, "%d%d%d%d%d%d%d",
        udbOverTrunkHash.trunkHashEnableStdNotIp,
        udbOverTrunkHash.trunkHashEnableStdIpv4L4,
        udbOverTrunkHash.trunkHashEnableStdUdb,
        udbOverTrunkHash.trunkHashEnableExtNotIpv6,
        udbOverTrunkHash.trunkHashEnableExtIpv6L2,
        udbOverTrunkHash.trunkHashEnableExtIpv6L4,
        udbOverTrunkHash.trunkHashEnableExtUdb);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPclUdeEtherTypeSet
*
* DESCRIPTION:
*       This function sets UDE Ethertype.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index, range 0..5
*       ethType       - Ethertype value (range 0..0xFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_OUT_OF_RANGE          - on out of range parameter value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclUdeEtherTypeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8           devNum;
    GT_U32          index;
    GT_U32          ethType;
    GT_STATUS       result;

    devNum   = (GT_U8) inArgs[0];
    index    = (GT_U32)inArgs[1];
    ethType  = (GT_U32)inArgs[2];

    result = cpssDxChPclUdeEtherTypeSet(
        devNum, index, ethType);

    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPclUdeEtherTypeGet
*
* DESCRIPTION:
*       This function gets the UDE Ethertype.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum        - device number
*       index         - UDE Ethertype index, range 0..5
*
* OUTPUTS:
*       ethTypePtr    - points to Ethertype value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclUdeEtherTypeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8           devNum;
    GT_U32          index;
    GT_U32          ethType;
    GT_STATUS       result;

    devNum   = (GT_U8) inArgs[0];
    index    = (GT_U32)inArgs[1];

    result = cpssDxChPclUdeEtherTypeGet(
        devNum, index, &ethType);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    galtisOutput(
        outArgs, GT_OK, "%d", ethType);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPclEgressKeyFieldsVidUpModeSet
*
* DESCRIPTION:
*       Sets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum         - device number
*       vidUpMode      - VID and UP key fields content mode
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclEgressKeyFieldsVidUpModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                      devNum;
    CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode;
    GT_STATUS                                  result;

    devNum    = (GT_U8) inArgs[0];
    vidUpMode = (CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT)inArgs[1];

    result = pg_wrap_cpssDxChPclEgressKeyFieldsVidUpModeSet(
        devNum, vidUpMode);

    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPclEgressKeyFieldsVidUpModeGet
*
* DESCRIPTION:
*       Gets Egress Policy VID and UP key fields content mode
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum             - device number
* OUTPUTS:
*       vidUpModePtr       - (pointer to) VID and UP key fields
*                            calculation mode
*
* RETURNS :
*       GT_OK          - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclEgressKeyFieldsVidUpModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                      devNum;
    CPSS_DXCH_PCL_EGRESS_KEY_VID_UP_MODE_ENT   vidUpMode;
    GT_STATUS                                  result;

    devNum = (GT_U8)inArgs[0];

    result = pg_wrap_cpssDxChPclEgressKeyFieldsVidUpModeGet(
        devNum, &vidUpMode);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    galtisOutput(
        outArgs, GT_OK, "%d", vidUpMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPclRuleAnyStateGet
*
* DESCRIPTION:
*       Get state (valid or not) of the rule and it's size
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum         - device number
*       ruleSize       - size of rule
*       ruleIndex      - index of rule
* OUTPUTS:
*       validPtr       -  rule's validity
*                         GT_TRUE  - rule valid
*                         GT_FALSE - rule invalid
*       ruleSizePtr    -  rule's size
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclRuleAnyStateGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PCL_RULE_SIZE_ENT ruleSizeIn;
    GT_U32 ruleIndex;
    GT_BOOL valid;
    CPSS_PCL_RULE_SIZE_ENT ruleSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleSizeIn = (CPSS_PCL_RULE_SIZE_ENT)inArgs[1];
    ruleIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPclRuleAnyStateGet(devNum, ruleSizeIn, ruleIndex, &valid, &ruleSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", valid, ruleSize);

    return CMD_OK;
}

/*******************************************************************************
* utilCpssDxChPclRuleIncrementalSequenceSet
*
* DESCRIPTION:
*     Writes sequence of rules.
*     The Rules calculated by incremental modification of last written rule
*
* INPUTS:
*     devNum             - device number
*     checkRuleFormat    - the format of last written rule
*     ruleIndexBase      - rule Index Base
*     ruleIndexIncrement - rule Index Increment
*     rulesAmount        - rules Amount
*                          the i-th rule index is
*                          ruleIndexBase + (i * ruleIndexIncrement)
*     field_full_name    - the composed name of incremented field
*                          names used in CPSS_DXCH_PCL_RULE_FORMAT_..._STC
*                          sructure separated by '.'.For example:
*                         "common.sourcePort" or "macDa"
*     elementStart       - for scalar field - 0; for array the index of
*                          incremented array element
*     increment          - value
*
*
* OUTPUTS:
*     None.
*
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     Only network ordered byte array supported
*     Example (start from 1-th element, carry of overflow bit)
*     checkRuleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E
*     field_full_name = macDa
*     elementStart    = 1
*     increment       = 0x01020080
*     MAC_DA states     00:00:00:00:00:00
*                       00:01:02:00:80:00
*                       00:02:04:01:00:00
*                       00:03:06:01:80:00
*                       00:04:08:02:00:00
*
*******************************************************************************/
static CMD_STATUS wrUtilCpssDxChPclRuleIncrementalSequenceSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          rc;
    GT_U8                              devNum;
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT checkRuleFormat;
    GT_U32                             ruleIndexBase;
    GT_U32                             ruleIndexIncrement;
    GT_U32                             rulesAmount;
    char                               *field_full_name;
    GT_U32                             elementStart;
    GT_U32                             increment;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum                = (GT_U8) inArgs[0];
    checkRuleFormat       = (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)inArgs[1];
    ruleIndexBase         = (GT_U32)inArgs[2];
    ruleIndexIncrement    = (GT_U32)inArgs[3];
    rulesAmount           = (GT_U32)inArgs[4];
    field_full_name       = (char*) inArgs[5];
    elementStart          = (GT_U32)inArgs[6];
    increment             = (GT_U32)inArgs[7];

    rc = utilCpssDxChPclRuleIncrementalSequenceSet(
        devNum, checkRuleFormat,
        ruleIndexBase, ruleIndexIncrement, rulesAmount,
        field_full_name, elementStart,  increment);

    galtisOutput(outArgs, rc, "");
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPclL3L4ParsingOverMplsEnableSet
*
* DESCRIPTION:
*       If enabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as IP packets for key selection and UDB usage.
*       If disabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as MPLS packets for key selection and UDB usage.
*
* APPLICABLE DEVICES:  All DxChXcat and above devices
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - enable, GT_FALSE - disable
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclL3L4ParsingOverMplsEnableSet
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
    result = cpssDxChPclL3L4ParsingOverMplsEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPclL3L4ParsingOverMplsEnableGet
*
* DESCRIPTION:
*       If enabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as IP packets for key selection and UDB usage.
*       If disabled, non-Tunnel-Terminated IPvx over MPLS packets are treated
*       by Ingress PCL as MPLS packets for key selection and UDB usage.
*
* APPLICABLE DEVICES:  All DxChXcat and above devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPclL3L4ParsingOverMplsEnableGet
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
    result = cpssDxChPclL3L4ParsingOverMplsEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChPclInit",
        &wrCpssDxChPclInit,
        1, 0},

    {"cpssDxChPclUserDefinedByteSet",
        &wrCpssDxChPclUserDefinedByteSet,
        5, 0},

    {"cpssDxChPclUserDefinedByteGet",
        &wrCpssDxChPclUserDefinedByteGet,
        4, 0},

    {"cpssDxChPclL3L4ParsingOverMplsEnableSet",
        &wrCpssDxChPclL3L4ParsingOverMplsEnableSet,
        2, 0},

    {"cpssDxChPclL3L4ParsingOverMplsEnableGet",
        &wrCpssDxChPclL3L4ParsingOverMplsEnableGet,
        1, 0},

    /* call for union table DxChPclRule */

    {"cpssDxChPclRuleSet",
        &wrCpssDxChPclRuleSet,
        2, 100}, /* the fields number is variable (see the function) */

/* end call for union table DxChPclRule */

    {"cpssDxChPclRuleActionSet",
        &wrCpssDxChPclRuleActionUpdate,
        3, 36},

    {"cpssDxChPclRuleActionGetFirst",
        &wrCpssDxChPclRuleActionGet,
        4, 0},

    {"cpssDxChPclRuleActionGetNext",
        &genericTableGetLast,
        4, 0},

    {"cpssDxChPclRuleInvalidate",
        &wrCpssDxChPclRuleInvalidate,
        3, 0},

    {"cpssDxChPclRuleCopy",
        &wrCpssDxChPclRuleCopy,
        4, 0},

    {"cpssDxChPclRuleMatchCounterGet",
        &wrCpssDxChPclRuleMatchCounterGet,
        2, 0},

    {"cpssDxChPclRuleMatchCounterSet",
        &wrCpssDxChPclRuleMatchCounterSet,
        3, 0},

    {"cpssDxChPclPortIngressPolicyEnable",
        &wrCpssDxChPclPortIngressPolicyEnable,
        3, 0},

    {"cpssDxChPclPortIngressPolicyEnableGet",
        &wrCpssDxChPclPortIngressPolicyEnableGet,
        2, 0},

    {"cpssDxChPclPortLookupCfgTabAccessModeSet",
        &wrCpssDxChPclPortLookupCfgTabAccessModeSet,
        5, 0},

    {"cpssDxChPclPortLookupCfgTabAccessModeSet_v1",
        &wrCpssDxChPclPortLookupCfgTabAccessModeSet_V1,
        6, 0},

    {"cpssDxChPclCfgTblAccessModeSet",
        &wrCpssDxChPclCfgTblAccessModeSet,
        1, 5},

    {"cpssDxChPclCfgTblAccessModeGetFirst",
        &wrCpssDxChPclCfgTblAccessModeGetFirst,
        1, 0},

    {"cpssDxChPclCfgTblAccessModeGetNext",
        &wrCpssDxChPclGenericDymmyGetNext,
        1, 0},

    {"cpssDxChPclCfgTblSet",
        &wrCpssDxChPclCfgTblSet,
        9, 5},

    {"cpssDxChPclCfgTblGetFirst",
        &wrCpssDxChPclCfgTblGet,
        9, 0},

    {"cpssDxChPclCfgTblGetNext",
        &wrCpssDxChPclGenericDymmyGetNext,
        9, 0},

    {"cpssDxCh3PclCfgTblSet",
        &wrCpssDxCh3PclCfgTblSet,
        9, 6},

    {"cpssDxCh3PclCfgTblGetFirst",
        &wrCpssDxCh3PclCfgTblGet,
        9, 0},

    {"cpssDxCh3PclCfgTblGetNext",
        &wrCpssDxChPclGenericDymmyGetNext,
        9, 0},

    {"cpssDxChXCatPclCfgTblSet",
        &wrCpssDxChXCatPclCfgTblSet,
        9, 7},

    {"cpssDxChXCatPclCfgTblGetFirst",
        &wrCpssDxChXCatPclCfgTblGet,
        9, 0},

    {"cpssDxChXCatPclCfgTblGetNext",
        &wrCpssDxChPclGenericDymmyGetNext,
        9, 0},

    {"cpssDxChPclIngressPolicyEnable",
        &wrCpssDxChPclIngressPolicyEnable,
        2, 0},

    {"cpssDxChPclTcamRuleSizeModeSet",
        &wrCpssDxChPclTcamRuleSizeModeSet,
        3, 0},

    {"cpssDxChPclTwoLookupsCpuCodeResolution",
        &wrCpssDxChPclTwoLookupsCpuCodeResolution,
        2, 0},

    {"cpssDxChPclInvalidUdbCmdSet",
        &wrCpssDxChPclInvalidUdbCmdSet,
        2, 0},

    {"cpssDxChPclIpLengthCheckModeSet",
        &wrCpssDxChPclIpLengthCheckModeSet,
        2, 0},

    {"cpssDxChPclRuleStateGet",
        &wrCpssDxChPclRuleStateGet,
        2, 0},

    {"cpssDxChPclCfgTblEntryGet",
        &wrCpssDxChPclCfgTblEntryGet,
        3, 0},

    {"cpssDxChPclLookupCfgPortListEnableSet",
        &wrCpssDxChPclLookupCfgPortListEnableSet,
        5, 0},

    {"cpssDxChPclLookupCfgPortListEnableGet",
        &wrCpssDxChPclLookupCfgPortListEnableGet,
        4, 0},

    {"util_cpssDxChPclRuleOptionsSet",
        &wrUtil_cpssDxChPclRuleOptionsSet,
        3, 0},

    {"cpssDxCh2PclEgressPolicyEnable",
        &wrCpssDxCh2PclEgressPolicyEnable,
        2, 0},

    {"cpssDxCh2EgressPclPacketTypesSet",
        &wrCpssDxCh2EgressPclPacketTypesSet,
        4, 0},

    {"cpssDxCh2PclTcpUdpPortComparatorSet",
        &wrCpssDxCh2PclTcpUdpPortComparatorSet,
        7, 0},

    {"cpssDxChPclPolicyRuleGetFirst",
        &wrCpssDxChPclPolicyRuleGetFirst,
        3, 0},

    {"cpssDxChPclPolicyRuleGetNext",
        &wrCpssDxChPclGenericDymmyGetNext,
        3, 0},


    {"cpssDxChPclRuleValidStatusSet",
         &wrCpssDxChPclRuleValidStatusSet,
         4, 0},

    {"cpssDxChPclEgressForRemoteTunnelStartEnableSet",
         &wrCpssDxChPclEgressForRemoteTunnelStartEnableSet,
         2, 0},

    {"cpssDxChPclEgressForRemoteTunnelStartEnableGet",
         &wrCpssDxChPclEgressForRemoteTunnelStartEnableGet,
         1, 0},

    {"cpssDxChPclEgressTunnelStartPktCfgTabAccessModeSet",
         &wrCpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeSet,
         2, 0},

    {"cpssDxChPclEgressTunnelStartPktCfgTabAccesModeGet",
         &wrCpssDxChPclEgressTunnelStartPacketsCfgTabAccessModeGet,
         1, 0},

    {"cpssDxCh3PclTunnelTermForceVlanModeEnableSet",
         &wrCpssDxCh3PclTunnelTermForceVlanModeEnableSet,
         2, 0},

    {"cpssDxCh3PclTunnelTermForceVlanModeEnableGet",
         &wrCpssDxCh3PclTunnelTermForceVlanModeEnableGet,
         1, 0},

    {"cpssDxChPclOverrideUserDefinedBytesSet",
         &wrCpssDxChPclOverrideUserDefinedBytesSet,
         19, 0},

    {"cpssDxChPclOverrideUserDefinedBytesGet",
         &wrCpssDxChPclOverrideUserDefinedBytesGet,
         1, 0},

    {"cpssDxChPclOverUDB_ByTrunkHashSet",
         &wrCpssDxChPclOverrideUserDefinedBytesByTrunkHashSet,
         8, 0},

    {"cpssDxChPclOverUDB_ByTrunkHashGet",
         &wrCpssDxChPclOverrideUserDefinedBytesByTrunkHashGet,
         1, 0},

    {"cpssDxChPclUdeEtherTypeSet",
         &wrCpssDxChPclUdeEtherTypeSet,
         3, 0},

    {"cpssDxChPclUdeEtherTypeGet",
         &wrCpssDxChPclUdeEtherTypeGet,
         2, 0},

    {"cpssDxChPclEgrKeyVidUpModeSet",
         &wrCpssDxChPclEgressKeyFieldsVidUpModeSet,
         2, 0},

    {"cpssDxChPclEgrKeyVidUpModeGet",
         &wrCpssDxChPclEgressKeyFieldsVidUpModeGet,
         1, 0},

    {"cpssDxCh3PclActionSet",
        &wrCpssDxCh3PclActionSet,
        3, 42},

    {"cpssDxCh3PclActionGetFirst",
        &wrCpssDxCh3PclActionGet_Ch3,
        4, 0},

    {"cpssDxCh3PclActionGetNext",
        &genericTableGetLast,
        0, 0},

    {"cpssDxCh3PclActionClear",
        &wrCpssDxCh3PclActionClear,
        0, 0},

    {"cpssDxCh3PclActionDelete",
        &wrCpssDxCh3PclActionDelete,
        2, 0},
     /* --- cpssDxChXCatActionTable --- */

    {"cpssDxChXCatActionTableSet",
        &wrCpssDxCh3PclActionSet,
        3, 51},

    {"cpssDxChXCatActionTableGetFirst",
        &wrCpssDxCh3PclActionGet_XCat,
        4, 0},

    {"cpssDxChXCatActionTableGetNext",
        &genericTableGetLast,
        0, 0},

    {"cpssDxChXCatActionTableClear",
        &wrCpssDxCh3PclActionClear,
        0, 0},

    {"cpssDxChXCatActionTableDelete",
        &wrCpssDxCh3PclActionDelete,
        2, 0},

     /* --- cpssDxChLionActionTable --- */

    {"cpssDxChLionActionTableSet",
        &wrCpssDxCh3PclActionSet,
        3, 51},

    {"cpssDxChLionActionTableGetFirst",
        &wrCpssDxCh3PclActionGet_XCat,
        4, 0},

    {"cpssDxChLionActionTableGetNext",
        &genericTableGetLast,
        0, 0},

    {"cpssDxChLionctionTableClear",
        &wrCpssDxCh3PclActionClear,
        0, 0},

    {"cpssDxChLionActionTableDelete",
        &wrCpssDxCh3PclActionDelete,
        2, 0},

    /* --- cpssDxCh3PclIngressRule Table --- */

    {"cpssDxCh3PclIngressRuleSetFirst",
        &wrCpssDxCh3PclIngressRuleWriteFirst,
        2, 30},

    {"cpssDxCh3PclIngressRuleSetNext",
        &wrCpssDxCh3PclIngressRuleWriteNext,
        2, 30},

    {"cpssDxCh3PclIngressRuleEndSet",
        &wrCpssDxCh3PclIngressRuleWriteEnd,
        2, 0},
    /**/

    {"cpssDxChPclXCatUDBRuleSetFirst",
        &wrCpssDxChPclXCatUDBRuleWriteFirst,
        2, 30},

    {"cpssDxChPclXCatUDBRuleSetNext",
        &wrCpssDxChPclXCatUDBRuleWriteNext,
        2, 30},

    {"cpssDxChPclXCatUDBRuleEndSet",
        &wrCpssDxCh3PclIngressRuleWriteEnd,
        2, 0},

    /* --- cpssDxCh3PclEgressRule Table --- */

    {"cpssDxCh3PclEgressRuleSetFirst",
        &wrCpssDxCh3PclEgressRuleWriteFirst,
        2, 41},

    {"cpssDxCh3PclEgressRuleSetNext",
        &wrCpssDxCh3PclEgressRuleWriteNext,
        2, 41},

    {"cpssDxCh3PclEgressRuleEndSet",
        &wrCpssDxCh3PclEgressRuleWriteEnd,
        2, 0},

    {"utilCpssDxChPclRuleIncrementalSequenceSet",
        &wrUtilCpssDxChPclRuleIncrementalSequenceSet,
        8, 0},

    {"cpssDxChPclUserDefinedByteTableSet",
        &wrCpssDxChPclUserDefinedByteTableSet,
        3, 3},

    {"cpssDxChPclUserDefinedByteTableGetNext",
        &wrCpssDxChPclUserDefinedByteTableGetNext,
        3, 0},

    {"cpssDxChPclUserDefinedByteTableGetFirst",
        &wrCpssDxChPclUserDefinedByteTableGetFirst,
        3, 0},

    {"cpssDxChPclRuleAnyStateGet",
         &wrCpssDxChPclRuleAnyStateGet,
         3, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChPcl
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
GT_STATUS cmdLibInitCpssDxChPcl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


/********************************* UTILITES *************************************/

/*
 * typedef: struct SUBFIELD_DATA_STC
 *
 * Description:
 *      Rule Subfield descriptor
 *
 * Fields:
 *      name           -  subfield name
 *      subfieldOffset -  offset of subfield from the structure origin
 *      subfieldSize   -  size of subfield
 *      elementsAmount -  amount of elements (for not-array - 1)
 *      nodePtr        -  address of substructure table
 *                        (for not-substructure NULL)
 *
 * Comment:
 */
typedef struct __SUBFIELD_DATA
{
    char             *name;            /* name in structure         */
    GT_U32           subfieldOffset;   /* offset in bytes           */
    GT_U32           subfieldSize;     /* size in bytes             */
    GT_U32           elementsAmount;   /* Not 1 means array         */
    struct __SUBFIELD_DATA  *nodePtr;  /* descpiptor of subsructure */
} SUBFIELD_DATA_STC;

#define SUBFIELD_NULL_BASED(_struct, _member) ((_struct*)0)->_member

#define SUBFIELD_OFFSET(_struct, _member) \
    (int)((char*)&(SUBFIELD_NULL_BASED(_struct, _member)) - (char*)0)

#define SUBFIELD_SIZE(_struct, _member) \
    sizeof(SUBFIELD_NULL_BASED(_struct, _member))

/* for array */
#define SUBFIELD_ELEMENT_AMOUNT(_struct, _member) \
    (SUBFIELD_SIZE(_struct, _member) / SUBFIELD_SIZE(_struct, _member[0]))

#define SUBFIELD_END {(char*)0, 0, 0, 0, (SUBFIELD_DATA_STC*)0}

#define SUBFIELD_SCALAR(_struct, _member)   \
    {                                       \
        #_member,                           \
        SUBFIELD_OFFSET(_struct, _member),  \
        SUBFIELD_SIZE(_struct, _member),    \
        1,                                  \
        (SUBFIELD_DATA_STC*)0               \
    }

#define SUBFIELD_SCALAR_DEEP(_struct, _branch, _member)    \
    {                                                      \
        #_member,                                          \
        SUBFIELD_OFFSET(_struct, _branch._member),        \
        SUBFIELD_SIZE(_struct, _branch._member),          \
        1,                                                 \
        (SUBFIELD_DATA_STC*)0                              \
    }

#define SUBFIELD_NODE(_struct, _member, _scriptPtr)     \
    {                                                   \
        #_member,                                       \
        SUBFIELD_OFFSET(_struct, _member),              \
        SUBFIELD_SIZE(_struct, _member),                \
        1,                                              \
        _scriptPtr                                      \
    }

/* for GT_IPV6ADDR member      _hidden_path = ".arIP"    */
/* for GT_ETHERADDR member     _hidden_path = ".arEther" */
#define SUBFIELD_ARRAY(_struct, _member, _hidden_path)            \
    {                                                             \
        #_member,                                                 \
        SUBFIELD_OFFSET(_struct, _member),          \
        SUBFIELD_SIZE(_struct, _member),            \
        SUBFIELD_ELEMENT_AMOUNT(_struct, _member._hidden_path),  \
        (SUBFIELD_DATA_STC*)0                                     \
    }

#define SUBFIELD_ARRAY_PURE(_struct, _member)       \
    {                                               \
        #_member,                                   \
        SUBFIELD_OFFSET(_struct, _member),          \
        SUBFIELD_SIZE(_struct, _member),            \
        SUBFIELD_ELEMENT_AMOUNT(_struct, _member),  \
        (SUBFIELD_DATA_STC*)0                       \
    }

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_COMMON_STC[] =
{
    SUBFIELD_SCALAR(_STR, pclId),
    SUBFIELD_SCALAR(_STR, sourcePort),
    SUBFIELD_SCALAR(_STR, vid),
    SUBFIELD_SCALAR(_STR, up),
    SUBFIELD_SCALAR(_STR, qosProfile),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_COMMON_EXT_STC[] =
{
    SUBFIELD_SCALAR(_STR, ipProtocol),
    SUBFIELD_SCALAR(_STR, dscp),
    SUBFIELD_SCALAR(_STR, l4Byte0),
    SUBFIELD_SCALAR(_STR, l4Byte1),
    SUBFIELD_SCALAR(_STR, l4Byte2),
    SUBFIELD_SCALAR(_STR, l4Byte3),
    SUBFIELD_SCALAR(_STR, l4Byte13),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_COMMON_STD_IP_STC[] =
{
    SUBFIELD_SCALAR(_STR, ipProtocol),
    SUBFIELD_SCALAR(_STR, dscp),
    SUBFIELD_SCALAR(_STR, l4Byte2),
    SUBFIELD_SCALAR(_STR, l4Byte3),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_STD_NOT_IP_STC[] =
{
    SUBFIELD_NODE(_STR, common, subfields_RULE_FORMAT_COMMON_STC),
    SUBFIELD_SCALAR(_STR, etherType),
    SUBFIELD_ARRAY(_STR, macDa, arEther),
    SUBFIELD_ARRAY(_STR, macSa, arEther),
    SUBFIELD_SCALAR(_STR, udb[0]),
    SUBFIELD_SCALAR(_STR, udb[1]),
    SUBFIELD_SCALAR(_STR, udb[2]),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_STD_IP_L2_QOS_STC[] =
{
    SUBFIELD_NODE(_STR, common, subfields_RULE_FORMAT_COMMON_STC),
    SUBFIELD_NODE(_STR, commonStdIp, subfields_RULE_FORMAT_COMMON_STD_IP_STC),
    SUBFIELD_ARRAY(_STR, macDa, arEther),
    SUBFIELD_ARRAY(_STR, macSa, arEther),
    SUBFIELD_SCALAR(_STR, udb[0]),
    SUBFIELD_SCALAR(_STR, udb[1]),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_STD_IPV4_L4_STC[] =
{
    SUBFIELD_NODE(_STR, common, subfields_RULE_FORMAT_COMMON_STC),
    SUBFIELD_NODE(_STR, commonStdIp, subfields_RULE_FORMAT_COMMON_STD_IP_STC),
    SUBFIELD_ARRAY(_STR, sip, arIP),
    SUBFIELD_ARRAY(_STR, dip, arIP),
    SUBFIELD_SCALAR(_STR, l4Byte0),
    SUBFIELD_SCALAR(_STR, l4Byte1),
    SUBFIELD_SCALAR(_STR, l4Byte13),
    SUBFIELD_SCALAR(_STR, udb[0]),
    SUBFIELD_SCALAR(_STR, udb[1]),
    SUBFIELD_SCALAR(_STR, udb[2]),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_EXT_NOT_IPV6_STC[] =
{
    SUBFIELD_NODE(_STR, common, subfields_RULE_FORMAT_COMMON_STC),
    SUBFIELD_NODE(_STR, commonExt, subfields_RULE_FORMAT_COMMON_EXT_STC),
    SUBFIELD_ARRAY(_STR, sip, arIP),
    SUBFIELD_ARRAY(_STR, dip, arIP),
    SUBFIELD_SCALAR(_STR, etherType),
    SUBFIELD_ARRAY(_STR, macDa, arEther),
    SUBFIELD_ARRAY(_STR, macSa, arEther),
    SUBFIELD_SCALAR(_STR, udb[0]),
    SUBFIELD_SCALAR(_STR, udb[1]),
    SUBFIELD_SCALAR(_STR, udb[2]),
    SUBFIELD_SCALAR(_STR, udb[3]),
    SUBFIELD_SCALAR(_STR, udb[4]),
    SUBFIELD_SCALAR(_STR, udb[5]),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_EXT_IPV6_L2_STC[] =
{
    SUBFIELD_NODE(_STR, common, subfields_RULE_FORMAT_COMMON_STC),
    SUBFIELD_NODE(_STR, commonExt, subfields_RULE_FORMAT_COMMON_EXT_STC),
    SUBFIELD_ARRAY(_STR, sip, arIP),
    SUBFIELD_SCALAR(_STR, dipBits127to120),
    SUBFIELD_ARRAY(_STR, macDa, arEther),
    SUBFIELD_ARRAY(_STR, macSa, arEther),
    SUBFIELD_SCALAR(_STR, udb[0]),
    SUBFIELD_SCALAR(_STR, udb[1]),
    SUBFIELD_SCALAR(_STR, udb[2]),
    SUBFIELD_SCALAR(_STR, udb[3]),
    SUBFIELD_SCALAR(_STR, udb[4]),
    SUBFIELD_SCALAR(_STR, udb[5]),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_EXT_IPV6_L4_STC[] =
{
    SUBFIELD_NODE(_STR, common, subfields_RULE_FORMAT_COMMON_STC),
    SUBFIELD_NODE(_STR, commonExt, subfields_RULE_FORMAT_COMMON_EXT_STC),
    SUBFIELD_ARRAY(_STR, sip, arIP),
    SUBFIELD_ARRAY(_STR, dip, arIP),
    SUBFIELD_SCALAR(_STR, udb[0]),
    SUBFIELD_SCALAR(_STR, udb[1]),
    SUBFIELD_SCALAR(_STR, udb[2]),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_EGR_COMMON_STC[] =
{
    SUBFIELD_SCALAR(_STR, pclId),
    SUBFIELD_SCALAR(_STR, sourcePort),
    SUBFIELD_SCALAR(_STR, vid),
    SUBFIELD_SCALAR(_STR, up),
    SUBFIELD_SCALAR(_STR, egrPacketType),
    SUBFIELD_SCALAR_DEEP(_STR, egrPktType.toCpu, cpuCode),
    SUBFIELD_SCALAR_DEEP(_STR, egrPktType.fromCpu, tc),
    SUBFIELD_SCALAR_DEEP(_STR, egrPktType.fromCpu, dp),
    SUBFIELD_SCALAR_DEEP(_STR, egrPktType.fwdData, qosProfile),
    SUBFIELD_SCALAR_DEEP(_STR, egrPktType.fwdData, srcTrunkId),
    SUBFIELD_SCALAR(_STR, srcDev),
    SUBFIELD_SCALAR(_STR, sourceId),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_EGR_COMMON_EXT_STC[] =
{
    SUBFIELD_SCALAR(_STR, ipProtocol),
    SUBFIELD_SCALAR(_STR, dscp),
    SUBFIELD_SCALAR(_STR, l4Byte0),
    SUBFIELD_SCALAR(_STR, l4Byte1),
    SUBFIELD_SCALAR(_STR, l4Byte2),
    SUBFIELD_SCALAR(_STR, l4Byte3),
    SUBFIELD_SCALAR(_STR, l4Byte13),
    SUBFIELD_SCALAR(_STR, egrTcpUdpPortComparator),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_EGR_COMMON_STD_IP_STC[] =
{
    SUBFIELD_SCALAR(_STR, ipProtocol),
    SUBFIELD_SCALAR(_STR, dscp),
    SUBFIELD_SCALAR(_STR, l4Byte2),
    SUBFIELD_SCALAR(_STR, l4Byte3),
    SUBFIELD_SCALAR(_STR, egrTcpUdpPortComparator),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_NOT_IP_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_EGR_STD_NOT_IP_STC[] =
{
    SUBFIELD_NODE(_STR, common, subfields_RULE_FORMAT_EGR_COMMON_STC),
    SUBFIELD_SCALAR(_STR, etherType),
    SUBFIELD_ARRAY(_STR, macDa, arEther),
    SUBFIELD_ARRAY(_STR, macSa, arEther),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC[] =
{
    SUBFIELD_NODE(_STR, common, subfields_RULE_FORMAT_EGR_COMMON_STC),
    SUBFIELD_NODE(_STR, commonStdIp, subfields_RULE_FORMAT_EGR_COMMON_STD_IP_STC),
    SUBFIELD_ARRAY_PURE(_STR, dipBits0to31),
    SUBFIELD_SCALAR(_STR, l4Byte13),
    SUBFIELD_ARRAY(_STR, macDa, arEther),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_EGR_STD_IPV4_L4_STC[] =
{
    SUBFIELD_NODE(_STR, common, subfields_RULE_FORMAT_EGR_COMMON_STC),
    SUBFIELD_NODE(_STR, commonStdIp, subfields_RULE_FORMAT_EGR_COMMON_STD_IP_STC),
    SUBFIELD_ARRAY(_STR, sip, arIP),
    SUBFIELD_ARRAY(_STR, dip, arIP),
    SUBFIELD_SCALAR(_STR, l4Byte0),
    SUBFIELD_SCALAR(_STR, l4Byte1),
    SUBFIELD_SCALAR(_STR, l4Byte13),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC[] =
{
    SUBFIELD_NODE(_STR, common, subfields_RULE_FORMAT_EGR_COMMON_STC),
    SUBFIELD_NODE(_STR, commonExt, subfields_RULE_FORMAT_EGR_COMMON_EXT_STC),
    SUBFIELD_ARRAY(_STR, sip, arIP),
    SUBFIELD_ARRAY(_STR, dip, arIP),
    SUBFIELD_SCALAR(_STR, etherType),
    SUBFIELD_ARRAY(_STR, macDa, arEther),
    SUBFIELD_ARRAY(_STR, macSa, arEther),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_EGR_EXT_IPV6_L2_STC[] =
{
    SUBFIELD_NODE(_STR, common, subfields_RULE_FORMAT_EGR_COMMON_STC),
    SUBFIELD_NODE(_STR, commonExt, subfields_RULE_FORMAT_EGR_COMMON_EXT_STC),
    SUBFIELD_ARRAY(_STR, sip, arIP),
    SUBFIELD_SCALAR(_STR, dipBits127to120),
    SUBFIELD_ARRAY(_STR, macDa, arEther),
    SUBFIELD_ARRAY(_STR, macSa, arEther),
    SUBFIELD_END
};

#undef _STR
#define _STR CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L4_STC
static SUBFIELD_DATA_STC subfields_RULE_FORMAT_EGR_EXT_IPV6_L4_STC[] =
{
    SUBFIELD_NODE(_STR, common, subfields_RULE_FORMAT_EGR_COMMON_STC),
    SUBFIELD_NODE(_STR, commonExt, subfields_RULE_FORMAT_EGR_COMMON_EXT_STC),
    SUBFIELD_ARRAY(_STR, sip, arIP),
    SUBFIELD_ARRAY(_STR, dip, arIP),
    SUBFIELD_END
};

/*******************************************************************************
* prvUtilSubfieldRootGet
*
* DESCRIPTION:
*     Retrieves root subfield descriptors table by rule format
*
* INPUTS:
*     checkRuleFormat    - rule format
*
* OUTPUTS:
*     None
*
*
* RETURNS:
*     subfield descriptor address - on success.
*     NULL                        - on failure.
*
* COMMENTS:
*
*******************************************************************************/
static SUBFIELD_DATA_STC* prvUtilSubfieldRootGet
(
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT checkRuleFormat
)
{
    switch (checkRuleFormat)
    {
        default: break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            return subfields_RULE_FORMAT_STD_NOT_IP_STC;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            return subfields_RULE_FORMAT_STD_IP_L2_QOS_STC;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            return subfields_RULE_FORMAT_STD_IPV4_L4_STC;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            return subfields_RULE_FORMAT_EXT_NOT_IPV6_STC;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            return subfields_RULE_FORMAT_EXT_IPV6_L2_STC;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            return subfields_RULE_FORMAT_EXT_IPV6_L4_STC;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
            return subfields_RULE_FORMAT_EGR_STD_NOT_IP_STC;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
            return subfields_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
            return subfields_RULE_FORMAT_EGR_STD_IPV4_L4_STC;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
            return subfields_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
            return subfields_RULE_FORMAT_EGR_EXT_IPV6_L2_STC;

        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
            return subfields_RULE_FORMAT_EGR_EXT_IPV6_L4_STC;
    }

    return (SUBFIELD_DATA_STC*)0;
}

/*******************************************************************************
* prvUtilSubfieldGet
*
* DESCRIPTION:
*     Retrieves subfield descriptor and calculates total offset
*
* INPUTS:
*     checkRuleFormat    - rule format
*     field_full_name    - the composed name of incremented field
*                          names used in CPSS_DXCH_PCL_RULE_FORMAT_..._STC
*                          sructure separated by '.'.For example:
*                         "common.sourcePort" or "macDa"
*
* OUTPUTS:
*     offsetPtr         - field offset from the rule origin
*
*
* RETURNS:
*     subfield descriptor address - on success.
*     NULL                        - on failure.
*
* COMMENTS:
*
*******************************************************************************/
static SUBFIELD_DATA_STC* prvUtilSubfieldGet
(
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT checkRuleFormat,
    IN  char                               *full_name,
    OUT GT_U32                              *offsetPtr
)
{
    SUBFIELD_DATA_STC* sfPtr;
    char*              path = full_name;
    GT_U32             name_len;

    *offsetPtr = 0;

    sfPtr = prvUtilSubfieldRootGet(checkRuleFormat);

    while (sfPtr != (SUBFIELD_DATA_STC*)0)
    {
        /* measure length of the current name in the path */
        for (name_len = 0;
              ((path[name_len] != '.') && (path[name_len] != 0));
              name_len ++) {};

        /* lookm for name from the path in the fields list */
        for (; (sfPtr->name != 0); sfPtr ++)
        {
            /* another length */
            if (sfPtr->name[name_len] != 0)
                continue;

            /* another contents */
            if (cmdOsMemCmp(sfPtr->name, path, name_len) != 0)
                continue;

            /* found */
            break;
        }

        /* not found */
        if (sfPtr->name == 0)
        {
            /* end of list reached */
            return (SUBFIELD_DATA_STC*)0;
        }

        /* accomulate offset */
        *offsetPtr += sfPtr->subfieldOffset;

        /* end of path */
        if (path[name_len] == 0)
        {
            return sfPtr;
        }

        /* down to substructure */
        sfPtr = sfPtr->nodePtr;

        /* to the next name in the path */
        path += (name_len + 1);
    }

    return (SUBFIELD_DATA_STC*)0;
}

/*******************************************************************************
* prvUtilSubfieldIncrement
*
* DESCRIPTION:
*     Increments the field
*
* INPUTS:
*     fieldPtr      - field address
*     fieldSizeof   - size of field in bytes
*     elementAmount - amount of elements in array, 1 for scalar
*     elementStart  - index of array element to increment from
*     increment     - increment
*
* OUTPUTS:
*     checkRuleFormat  - check Rule Format
*
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     Supported only network oredered byte array
*     The elementStart is offset from the last array element
*     see testUtilCpssDxChPclRuleIncrementalSequenceSet's comment
*
*******************************************************************************/
static GT_STATUS prvUtilSubfieldIncrement
(
    INOUT GT_U8              *fieldPtr,
    IN    GT_U32             fieldSizeof,
    IN    GT_U32             elementAmount,
    IN    GT_U32             elementStart,
    IN    GT_U32             increment
)
{
    GT_U32 index;
    GT_U32 maxIndex;
    GT_U32 realIndex;
    GT_U32 data;
    GT_U32 carryIndex;

    /* array */
    if (elementAmount != 1)
    {
        if (elementAmount != fieldSizeof)
        {
            /* only byte network ordered array supported */
            return GT_FAIL;
        }

        if (elementStart >= elementAmount)
        {
            /* start byte out of array */
            return GT_FAIL;
        }

        /* increment is 32 bits == 4 bytes */
        maxIndex = 3;
        if ((elementStart + maxIndex) >= elementAmount)
        {
            maxIndex = elementAmount - elementStart - 1;
        }

        for (index = 0; (index <= maxIndex); index ++)
        {
            /* network ordered array */
            realIndex = elementAmount - elementStart - 1 - index;

            data = ((increment >> (index * 8)) & 0xFF)
                + (GT_U32)(fieldPtr[realIndex]);

            fieldPtr[realIndex] = (GT_U8)data;

            /* carry */
            for (carryIndex = realIndex;
                  ((data & 0x0100) && (carryIndex > 0));
                  carryIndex --)
            {
                data = (GT_U32)(fieldPtr[carryIndex - 1]) + 1;
                fieldPtr[carryIndex - 1] = (GT_U8)data;
            }
        }

        return GT_OK;
    }

    /* SCALARS 1,2,4 byte-sized */
    if (elementStart)
    {
        return GT_FAIL;
    }

    switch (fieldSizeof)
    {
        case 1:
            *(GT_U8*)fieldPtr  += (GT_U8)increment;
            break;
        case 2:
            *(GT_U16*)fieldPtr += (GT_U16)increment;
            break;
        case 4:
            *(GT_U32*)fieldPtr += (GT_U32)increment;
            break;
        default: return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* utilCpssDxChPclRuleIncrementalSequenceSet
*
* DESCRIPTION:
*     Writes sequence of rules.
*     The Rules calculated by incremental modification of last written rule
*
* INPUTS:
*     devNum             - device number
*     checkRuleFormat    - the format of last written rule
*     ruleIndexBase      - rule Index Base
*     ruleIndexIncrement - rule Index Increment
*     rulesAmount        - rules Amount
*                          the i-th rule index is
*                          ruleIndexBase + (i * ruleIndexIncrement)
*     field_full_name    - the composed name of incremented field
*                          names used in CPSS_DXCH_PCL_RULE_FORMAT_..._STC
*                          sructure separated by '.'.For example:
*                         "common.sourcePort" or "macDa"
*     elementStart       - for scalar field - 0; for array the index of
*                          incremented array element
*     increment          - value
*
*
* OUTPUTS:
*     None.
*
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     Only network ordered byte array supported
*     Example (start from 1-th element, carry of overflow bit)
*     checkRuleFormat = CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E
*     field_full_name = macDa
*     elementStart    = 1
*     increment       = 0x01020080
*     MAC_DA states     00:00:00:00:00:00
*                       00:01:02:00:80:00
*                       00:02:04:01:00:00
*                       00:03:06:01:80:00
*                       00:04:08:02:00:00
*
*******************************************************************************/
GT_STATUS utilCpssDxChPclRuleIncrementalSequenceSet
(
    IN GT_U8                              devNum,
    IN CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT checkRuleFormat,
    IN GT_U32                             ruleIndexBase,
    IN GT_U32                             ruleIndexIncrement,
    IN GT_U32                             rulesAmount,
    IN char                               *field_full_name,
    IN GT_U32                             elementStart, /* start array element */
    IN GT_U32                             increment
)
{
    GT_STATUS          result;
    SUBFIELD_DATA_STC* sfPtr;
    GT_U32             offset;
    GT_U32             i;
    GT_U8              *fieldPtr;

    if (checkRuleFormat != ruleFormat)
    {
        return GT_BAD_STATE;
    }

    sfPtr = prvUtilSubfieldGet(
        checkRuleFormat, field_full_name, &offset);
    if (sfPtr == 0)
    {
        return GT_FAIL;
    }

    fieldPtr = (GT_U8*)&patternData + offset;

    for (i = 0; (i < rulesAmount); i++)
    {
         /* call cpss api function */
        result = pg_wrap_cpssDxChPclRuleSet(
            devNum, checkRuleFormat,
            (ruleIndexBase + (i * ruleIndexIncrement)),
            &maskData, &patternData, lastActionPtr);
        if (result != GT_OK)
        {
            return result;
        }

        result = prvUtilSubfieldIncrement(
            fieldPtr,
            sfPtr->subfieldSize, sfPtr->elementsAmount,
            elementStart, increment);
        if (result != GT_OK)
        {
            return result;
        }
   }

    return GT_OK;
}
