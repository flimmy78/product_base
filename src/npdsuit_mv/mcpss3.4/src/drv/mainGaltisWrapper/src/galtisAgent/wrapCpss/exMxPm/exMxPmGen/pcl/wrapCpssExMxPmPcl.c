/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmPcl.c
*
* DESCRIPTION:
*       Wrapper functions for cpssExMxPmPcl cpss.exMxPm functions
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
#include <cpss/exMxPm/exMxPmGen/pcl/cpssExMxPmPcl.h>
#include <cpss/generic/pcl/cpssPcl.h>
#include <cpss/exMxPm/exMxPmGen/intTcam/cpssExMxPmIntTcam.h>
#include <cpss/exMxPm/exMxPmGen/extTcam/cpssExMxPmExternalTcam.h>
#include <cpss/exMxPm/exMxPmGen/cpssExMxPmTypes.h>
#include <galtisAgent/wrapCpss/exMxPm/exMxPmGen/pcl/prvCmdCpssExMxPmPcl.h>

/* number of UDBs in entry */
#define PRV_EXMXPM_PCL_UDBS_NUM_CNS  23

#define PRV_EXMXPM_PCL_STUB_PRINT_MAC() \
    cmdOsPrintf("*** The wrapper is STUB ***\n")

/* Use external or internal TCAM and Action Table */
static GT_BOOL useExternalTcam = GT_FALSE;

static WR_PCL_ACTION_STC actionIntTable[ACTION_TABLE_SIZE];
static WR_PCL_ACTION_STC actionExtTable[ACTION_TABLE_SIZE];

static GT_U32   firstIntFree = 0;
static GT_BOOL  firstIntRun = GT_TRUE;

static GT_U32   firstExtFree = 0;
static GT_BOOL  firstExtRun = GT_TRUE;

/*************Pcl external Tcam configuration Table******************/

static WR_CPSS_EXMXPM_PCL_EXT_TCAM_CFG_STC extTcamConfigPclTable[EXT_TCAM_CFG_TABLE_ZISE];


/* devNum for management commands that does not have such argument/field*/
static GT_U8    devNum_ExtTcamPcl = 0;

static GT_U32   firstFree_ExtTcamPcl = 0;
static GT_BOOL  firsRun_ExtTcamPcl= GT_TRUE;

/* tables cpssExMxPmPclIngressRule and cpssExMxPmPclEgressRule global variables */
static    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT      maskData;
static    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT      patternData;
static    CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT ruleFormat;
static    GT_BOOL                              mask_set = GT_FALSE; /* is mask set*/
static    GT_BOOL                              pattern_set = GT_FALSE; /* is pattern set*/
static    GT_U32                               mask_ruleIndex = 0;
static    GT_U32                               pattern_ruleIndex = 0;

static CPSS_EXMXPM_PCL_ACTION_UNT wrPclEntry;

/* UDB table entry representing table */
static CPSS_EXMXPM_PCL_UDB_TABLE_ENTRY_STC udbTableEntry;
static GT_U32                              udbTableCurrentIndex;

static GT_U32 pm2ExtTcamPclIndex;

/*table number in Galtis db:
Ingress Std =0
IngressExt  =1
EgressStd   =2
EgressExt   =3
IngressUdb  =4
*/

static GT_U8 tableNum=0;

static  GT_U32   acionEntryIndex;

/* GT_TRUE for old table, GT_FALSE for extended */
static GT_BOOL wrCpssExMxPmPclIngressRuleWrite_patched = GT_FALSE;

/************** external access to internal data *******************************/
CPSS_EXMXPM_PCL_RULE_FORMAT_UNT      *wrCpssExMxPmPcl_maskDataPtr    = &maskData;
CPSS_EXMXPM_PCL_RULE_FORMAT_UNT      *wrCpssExMxPmPcl_patternDataPtr = &patternData;
CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT *wrCpssExMxPmPcl_ruleFormatPtr   = &ruleFormat;
GT_BOOL *wrCpssExMxPmPcl_useExternalTcamPtr = &useExternalTcam;
WR_PCL_ACTION_STC *wrCpssExMxPmPcl_actionIntTable = actionIntTable;
WR_PCL_ACTION_STC *wrCpssExMxPmPcl_actionExtTable = actionExtTable;
WR_CPSS_EXMXPM_PCL_EXT_TCAM_CFG_STC *wrCpssExMxPmPcl_extTcamConfigPclTablePtr =
    extTcamConfigPclTable;


/*******************************************************************************
* utilCpssExMxPmPclExtTcamUseSet  (util)
*
* DESCRIPTION:
*  The function sets the global default for multiple TCAM and Action
*  commands and tables to reffer to internal or to external TCAM.
*
* INPUTS:
*       extTcamUse - HT_TRUE - external, GT_FALSE - internal
*
* OUTPUTS:
*       none
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID utilCpssExMxPmPclExtTcamUseSet
(
    IN  GT_BOOL extTcamUse
)
{
    useExternalTcam = extTcamUse;
}

/*******************************************************************************
* utilCpssExMxPmPclExtTcamCfgDevNumSet  (table util)
*
* DESCRIPTION:
*  The function sets device number of current external rules.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       none
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID utilCpssExMxPmPclExtTcamCfgDevNumSet
(
    IN  GT_U8 devNum
)
{
    devNum_ExtTcamPcl = devNum;
}

/*******************************************************************************
* wrCpssExMxPmPclExtConfigIndexGet (table utilite)
*
* DESCRIPTION:
*  The function searches for external tcam config  table entry by rule index
*  and returns index.
*
*
*
* INPUTS:
*
*       ruleIndex       - Index of the rule external TCAM.
*
* OUTPUTS:
*       tableIndex     - external TCAM table entry
*
* RETURNS:
*         none
*
* COMMENTS:
*
*******************************************************************************/
void wrCpssExMxPmPclExtConfigIndexGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  ruleIndex,
    OUT GT_U32                  *tableIndex
)
{
    GT_U32              tcamExtCfgIndex = 0;
    GT_BOOL             entryFound = GT_FALSE;

    while ((!entryFound) && (tcamExtCfgIndex < EXT_TCAM_CFG_TABLE_ZISE))
    {
        while ((extTcamConfigPclTable[tcamExtCfgIndex].valid == GT_FALSE)
               && (tcamExtCfgIndex < EXT_TCAM_CFG_TABLE_ZISE))
        {
            tcamExtCfgIndex++;
        }
        if ( (tcamExtCfgIndex < EXT_TCAM_CFG_TABLE_ZISE) &&
             (extTcamConfigPclTable[tcamExtCfgIndex].devNum == devNum) &&
             (extTcamConfigPclTable[tcamExtCfgIndex].ruleStartIndex == ruleIndex))
        {
            entryFound = GT_TRUE;
        }
        else
        {
            tcamExtCfgIndex++;
        }
    }

    if (entryFound)
    {
        *tableIndex = tcamExtCfgIndex;
    }
    else
    {
        *tableIndex = INVALID_RULE_INDEX;
    }
}

/*******************************************************************************
* wrCpssExMxPmPclActionTableIndexGet (table command)
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
void wrCpssExMxPmPclActionTableIndexGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  ruleIndex,
    IN  WR_PCL_ACTION_STC      *actionTable,
    OUT GT_U32                  *tableIndex
)
{
    GT_U32              index = 0;
    GT_BOOL             entryFound = GT_FALSE;

    while ((!entryFound) && (index < ACTION_TABLE_SIZE))
    {
        while (!actionTable[index].valid && index < ACTION_TABLE_SIZE) index++;
        if ( (index < ACTION_TABLE_SIZE) &&
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
* wrCpssExMxPmPclActionIndexGet (table command)
*
* DESCRIPTION:
*  The function searches for action table entry by rule index and device number
*  and returns the actionEntry.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       ruleIndex       - index of the rule in the TCAM. See cpssExMxPclRuleSet.
*
* OUTPUTS:
*       actionEntry     - contains actionEntry, next free index and valid flag.
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void wrCpssExMxPmPclActionIndexGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  ruleIndex,
    OUT GT_U32                  *tableIndex
)
{
    WR_PCL_ACTION_STC*  actionTable;

    actionTable =
        (useExternalTcam == GT_FALSE)
        ? actionIntTable : actionExtTable;

    wrCpssExMxPmPclActionTableIndexGet(
        devNum, ruleIndex, actionTable, tableIndex);
}

/*******************************************************************************
* utilCpssExMxPm2PclActionTableInitOrCheck  (table util)
*
* DESCRIPTION:
*  The function checks the action table initialization an if it not done
*  initializes the table entries and sets all entries invalid.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       tcamType - tcam Type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None.
*
* COMMENTS:
*       The use of this function is for first initialization and for clearing all
*       antries in the table.
*
*******************************************************************************/
static GT_VOID utilCpssExMxPm2PclActionTableInitOrCheck
(
    IN  CPSS_EXMXPM_TCAM_TYPE_ENT  tcamType
)
{
    GT_U32          index = 0;
    WR_PCL_ACTION_STC *actionTable;

    if (tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
    {
        if (firstIntRun == GT_FALSE)
        {
            return;
        }
        firstIntRun = GT_FALSE;
        firstIntFree = 0;
        actionTable=actionIntTable;
    }
    else
    {
        if (firstExtRun == GT_FALSE)
        {
            return;
        }
        firstExtRun = GT_FALSE;
        firstExtFree = 0;
        actionTable=actionExtTable;
    }

    while (index < (ACTION_TABLE_SIZE - 1))
    {
        actionTable[index].valid = GT_FALSE;
        actionTable[index].nextFree = index + 1;
        index++;
    }
    actionTable[index].nextFree = INVALID_RULE_INDEX;
    actionTable[index].valid = GT_FALSE;
}

/*******************************************************************************
* cpssExMxPmPclInit
*
* DESCRIPTION:
*   The function initializes the device for following configuration
*   and using Policy engine
*
* APPLICABLE DEVICES:  All ExMxPm devices
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
*       GT_BAD_PARAM   - on wrong device number
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclInit

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* there is no Init function - return OK */
    result = GT_OK;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    PRV_EXMXPM_PCL_STUB_PRINT_MAC();
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclPolicyEnableSet
*
* DESCRIPTION:
*    Enables Ingress or Egress Policy.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*    devNum           - device number
*    direction        - Policy direction either Ingress or Egress
*    enable           - enable Policy
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
*       GT_HW_ERROR        - on hardware error
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclPolicyEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PCL_DIRECTION_ENT  direction;
    GT_BOOL             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPclPolicyEnableSet(devNum, direction, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclPolicyEnableGet
*
* DESCRIPTION:
*    Get status (Enable or Disable) of Ingress or Egress Policy.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*    devNum           - device number
*    direction        - Policy direction either Ingress or Egress
*
* OUTPUTS:
*    enablePtr        - enable Policy
*                       GT_TRUE  - enable
*                       GT_FALSE - disable
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on enablePtr == NULL
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclPolicyEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PCL_DIRECTION_ENT  direction;
    GT_BOOL             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPclPolicyEnableGet(devNum, direction, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclPortIngressPolicyEnableSet
*
* DESCRIPTION:
*    Enable or disable ingress policy per port .
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*    enable          - GT_TRUE - Ingress Policy enable,
*                      GT_FALSE - Ingress Policy disable
*
* OUTPUTS:
*    none
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclPortIngressPolicyEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_BOOL             enable;

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
    result = cpssExMxPmPclPortIngressPolicyEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclPortIngressPolicyEnableGet
*
* DESCRIPTION:
*    Retrieves if ingress policy per port enable or disable .
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*
* OUTPUTS:
*    enablePtr       - GT_TRUE - Ingress Policy enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_BAD_PTR         - on null pointer
*       GT_FAIL            - otherwise.
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclPortIngressPolicyEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_BOOL             enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmPclPortIngressPolicyEnableGet(devNum, port, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclPortLookupCfgTblAccessModeSet
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* DESCRIPTION:
*    Configures VLAN/PORT access mode to Ingress or Egress PCL
*    configuration table per lookup.
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
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclPortLookupCfgTblAccessModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    CPSS_PCL_DIRECTION_ENT  direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT  lookupNum;
    CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_ENT  mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[2];
    lookupNum = (CPSS_PCL_LOOKUP_NUMBER_ENT)inArgs[3];
    mode = (CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_ENT)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmPclPortLookupCfgTblAccessModeSet(devNum, port, direction, lookupNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclPortLookupCfgTblAccessModeGet
*
* DESCRIPTION:
*    Gets VLAN/PORT access mode to Ingress or Egress PCL
*    configuration table per lookup.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*    devNum          - device number
*    port            - port number
*    direction       - Policy direction:
*                      Ingress or Egress
*    lookupNum       - Lookup number:
*                      lookup0 or lookup1
*
* OUTPUTS:
*    modePtr         - PCL Configuration Table access mode
*                      Port or VLAN ID
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_BAD_PTR         - on null pointer
*       GT_FAIL            - otherwise.
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclPortLookupCfgTblAccessModeGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    CPSS_PCL_DIRECTION_ENT  direction;
    CPSS_PCL_LOOKUP_NUMBER_ENT  lookupNum;
    CPSS_EXMXPM_PCL_PORT_LOOKUP_CFG_TBL_ACC_MODE_ENT  modePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[2];
    lookupNum = (CPSS_PCL_LOOKUP_NUMBER_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmPclPortLookupCfgTblAccessModeGet(devNum, port, direction, lookupNum, &modePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", modePtr);
    return CMD_OK;
}


/* cpssExMxPmPclCfgTblAccessMode    Table   */

/*
* cpssExMxPmPclCfgTblAccessModeSet  (Table)
*
* DESCRIPTION:
*    Configures global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*    devNum          - device number
*    direction       - Policy direction:
*                      Ingress or Egress
*    accModePtr      - global configuration of access mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on NULL pointer value
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclCfgTblAccessModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(inFields[0] == 0)
    {
        /* only Local ports supported */
        result = GT_OK;
    }
    else
        result = GT_NOT_SUPPORTED;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    PRV_EXMXPM_PCL_STUB_PRINT_MAC();
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclCfgTblAccessModeGet  (Table)
*
* DESCRIPTION:
*    Gets global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*    devNum          - device number
*    direction       - Policy direction:
*                      Ingress or Egress
* OUTPUTS:
*    accModePtr      - global configuration of access mode
*
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on NULL pointer value
*       GT_BAD_STATE       - HW values that never written by SW
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclCfgTblAccessModeGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    inFields[0] = 0;
    inFields[1] = 0;
    inFields[2] = 0;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, GT_OK, "%f");
    PRV_EXMXPM_PCL_STUB_PRINT_MAC();
    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmPclCfgTblAccessModeGetNext  (Table)
*
* DESCRIPTION:
*    Gets global access mode to Ingress or Egress PCL configuration tables.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*    devNum          - device number
*    direction       - Policy direction:
*                      Ingress or Egress
* OUTPUTS:
*    accModePtr      - global configuration of access mode
*
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on NULL pointer value
*       GT_BAD_STATE       - HW values that never written by SW
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclCfgTblAccessModeGetNext
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
* cpssExMxPmPclEgressPortPacketTypeSet
*
* DESCRIPTION:
*   Enables/disables Egress PCL (EPCL) for given packet type on the given port
*
* APPLICABLE DEVICES: All ExMxPm devices
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
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*   - after reset EPCL disabled for all packet types on all ports
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclEgressPortPacketTypeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_BOOL             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = (enable) ? GT_OK : GT_NOT_SUPPORTED;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    PRV_EXMXPM_PCL_STUB_PRINT_MAC();
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclEgressPortPacketTypeGet
*
* DESCRIPTION:
*   Get status (enable or disable) of Egress PCL (EPCL) for given packet type
*   on the given port
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*   devNum        - device number
*   port          - port number
*   pktType       - packet type to enable/disable EPCL for it
*
* OUTPUTS:
*   enablePtr     - enable EPCL for specific packet type
*                   GT_TRUE - enable
*                   GT_FALSE - disable
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on NULL pointer value
*
* COMMENTS:
*   - after reset EPCL disabled for all packet types on all ports
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclEgressPortPacketTypeGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U8               devNum;
    GT_U8               port;
    GT_BOOL             enablePtr = GT_TRUE;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d", enablePtr);
    PRV_EXMXPM_PCL_STUB_PRINT_MAC();
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclRuleMatchCounterGet
*
* DESCRIPTION:
*       Get rule matching counters.
*       The counter is read/write (not clear on read)
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - Device number.
*       counterIndex - Counter Index (0..31)
*
* OUTPUTS:
*       counterPtr   - pointer to the counter value.
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - otherwise
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_BAD_PTR         - on NULL pointer value
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclRuleMatchCounterGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    GT_U8               devNum;
    GT_U32              counterIndex;
    GT_U32              counterPtr = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    counterIndex = (GT_U32)inArgs[1];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", counterPtr);
    PRV_EXMXPM_PCL_STUB_PRINT_MAC();
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclRuleMatchCounterSet
*
* DESCRIPTION:
*       Set rule matching counters.
*       To reset value of counter use counterValue = 0
*       The counter is read/write (not clear on read)
*
* APPLICABLE DEVICES:  All ExMxPm devices
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
*       GT_BAD_PARAM     - one of the input parameters is not valid.
*       GT_FAIL          - otherwise
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclRuleMatchCounterSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              counterIndex;
    GT_U32              counterValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    counterIndex = (GT_U32)inArgs[1];
    counterValue = (GT_U32)inArgs[2];

    result = GT_NOT_SUPPORTED;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    PRV_EXMXPM_PCL_STUB_PRINT_MAC();
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclRuleStateGet
*
* DESCRIPTION:
*   The function gets state (valid or not and rule size) of the Policy Rule
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssExMxPclRuleSet.
*
* OUTPUTS:
*       sizePtr         - the determinated rule size
*       validPtr        - GT_TRUE - rule valid, GT_FALSE - rule invalid
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclRuleStateGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_EXMXPM_INT_TCAM_RULE_SIZE_ENT  ruleSize;
    GT_U32              ruleIndex;
    CPSS_EXMXPM_INT_TCAM_RULE_SIZE_ENT  size;
    GT_BOOL             valid;
    GT_BOOL             ageEnable;
    GT_U32              patternOrX[20];
    GT_U32              maskOrY[20];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_PCL_RULE_SIZE_ENT)inArgs[1];
    ruleIndex = (GT_U32)inArgs[2];

    if (useExternalTcam == GT_FALSE)
    {
        /* call cpss api function */
        result = cpssExMxPmIntTcamRuleStateGet(
            devNum, ruleIndex, &size, &valid);
    }
    else
    {
        GT_U32 index;

        wrCpssExMxPmPclExtConfigIndexGet(devNum, ruleIndex, &index);
        if (index == INVALID_RULE_INDEX)
        {
            galtisOutput(outArgs, GT_FAIL, "%d%d", -1, -1);
            return CMD_OK;
        }

        /* size is one of command results */
        size = extTcamConfigPclTable[index].ruleSize;

        result = cpssExMxPmExternalTcamRuleRead(
            devNum,
            extTcamConfigPclTable[index].tcamId,
            extTcamConfigPclTable[index].ruleStartIndex,
            size, (sizeof(patternOrX) * 8),
            CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_DATA_MASK_AND_LOGIC_E,
            &valid, &ageEnable, patternOrX, maskOrY);
        if (result == GT_OK)
        {
            galtisOutput(outArgs, result, "%d%d", -1, -1);
            return CMD_OK;
        }

    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", size, valid);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclActionClear  (table command)
*
* DESCRIPTION:
*  The function initializes the table entries and sets all entries invalid.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       ruleIndex       - index of the rule in the TCAM. See cpssExMxPclRuleSet.
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
static CMD_STATUS wrCpssExMxPmPclActionClear
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if (useExternalTcam == GT_FALSE)
    {
        firstIntRun = GT_TRUE;
        utilCpssExMxPm2PclActionTableInitOrCheck(
            CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E);
    }
    else
    {
        firstExtRun = GT_TRUE;
        utilCpssExMxPm2PclActionTableInitOrCheck(
            CPSS_EXMXPM_TCAM_TYPE_EXTERNAL_E);
    }

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclActionDelete (table command)
*
* DESCRIPTION:
*  The function finds action table entry by rule index and device number,
*  deletes the actionEntry and updates the list.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       ruleIndex       - index of the rule in the TCAM. See cpssExMxPclRuleSet.
*
* OUTPUTS:
*       actionEntry     - contains actionEntry, device number, next free index and valid flag.
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
static CMD_STATUS wrCpssExMxPmPclActionDelete
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

    devNum =  (GT_U8)inFields[0];
    ruleIndex = (GT_U32)inFields[1];
    if (ruleIndex > 1023)
        return CMD_AGENT_ERROR;

    wrCpssExMxPmPclActionIndexGet(devNum, ruleIndex, &index);

    /* the rule is not found */
    if (index == INVALID_RULE_INDEX)
    {
        galtisOutput(outArgs, GT_NO_SUCH, "%d", -1);
        return CMD_AGENT_ERROR;
    }


    if (useExternalTcam == GT_FALSE)
    {
        actionIntTable[index].valid = GT_FALSE;
        actionIntTable[index].nextFree = firstIntFree;
        firstIntFree = index;
    }
    else
    {
        actionExtTable[index].valid = GT_FALSE;
        actionExtTable[index].nextFree = firstExtFree;
        firstExtFree = index;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

#define PRV_CNV_OLD_TO_NEW_OUT_LIF_TYPE_MAC(_old) \
   (((_old) == 0) ? CPSS_EXMXPM_OUTLIF_TYPE_LL_E :     \
    (((_old) == 1) ? CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E : CPSS_EXMXPM_OUTLIF_TYPE_DIT_E))

/*******************************************************************************
* cpssExMxPmPclActionSet (table command)
*
* DESCRIPTION:
*   The function sets action entry at the action table for the use of cpssExMxPmPclIngressRule/
*   cpssExMxPmPclEgressRule table.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
**       ruleIndex       - index of the rule in the TCAM. See cpssExMxPclRuleSet.
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
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclActionSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_EXMXPM_PCL_ACTION_UNT         *actionPtr;
    GT_U32                              ruleIndex;
    GT_U8                               devNum;
    GT_U32                              index;
    CPSS_PCL_RULE_SIZE_ENT              ruleSize;
    GT_STATUS                           result;
    CPSS_EXMXPM_TCAM_ACTION_INFO_UNT    actionInfo;
    CPSS_EXMXPM_TCAM_TYPE_ENT           tcamType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum  =       (GT_U8)inFields[0];
    ruleIndex =     (GT_U32)inFields[1];

    if (useExternalTcam == GT_FALSE)
    {
        utilCpssExMxPm2PclActionTableInitOrCheck(
            CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E);
    }
    else
    {
        utilCpssExMxPm2PclActionTableInitOrCheck(
            CPSS_EXMXPM_TCAM_TYPE_EXTERNAL_E);
    }

    wrCpssExMxPmPclActionIndexGet(devNum, ruleIndex, &index);

    /* the rule wasn't previously in action table */
    if (index == INVALID_RULE_INDEX)
    {
        if (useExternalTcam == GT_FALSE)
        {
            if (firstIntFree == INVALID_RULE_INDEX)
            {
                /* pack output arguments to galtis string */
                galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
                return CMD_AGENT_ERROR;
            }
            index = firstIntFree;
            firstIntFree = actionIntTable[firstIntFree].nextFree;

        }
        else
        {
            if (firstExtFree == INVALID_RULE_INDEX)
            {
                /* pack output arguments to galtis string */
                galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
                return CMD_AGENT_ERROR;
            }
            index = firstExtFree;
            firstExtFree = actionExtTable[firstExtFree].nextFree;
        }
    }

    if (useExternalTcam == GT_FALSE)
    {
        actionIntTable[index].valid = GT_TRUE;
        actionIntTable[index].ruleIndex = ruleIndex;
        actionIntTable[index].devNum = devNum;
        actionPtr = &(actionIntTable[index].actionEntry);
    }
    else
    {
        actionExtTable[index].valid = GT_TRUE;
        actionExtTable[index].ruleIndex = ruleIndex;
        actionExtTable[index].devNum = devNum;
        actionPtr = &(actionExtTable[index].actionEntry);
    }



    actionPtr->ingressStandard.pktCmd = (CPSS_PACKET_CMD_ENT)inFields[4];
    actionPtr->ingressStandard.mirror.cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inFields[5];
    actionPtr->ingressStandard.mirror.ingressPolicyAnalyzerNumber = inFields[6];
   /* actionPtr->ingressStandard.matchCounter.enableMatchCount = (GT_BOOL)inFields[7];
    actionPtr->ingressStandard.matchCounter.matchCounterIndex = (GT_U32)inFields[8];  */
    actionPtr->ingressStandard.qos.qosParams.tc = (GT_U32)inFields[9];
    actionPtr->ingressStandard.qos.qosParams.dp = (CPSS_DP_LEVEL_ENT)inFields[10];
    actionPtr->ingressStandard.qos.qosParams.up = (GT_U32)inFields[11];
    actionPtr->ingressStandard.qos.qosParams.dscp = (GT_U32)inFields[12];
    actionPtr->ingressStandard.qos.qosParams.exp = (GT_U32)inFields[13];
    actionPtr->ingressStandard.qos.qosParamsModify.modifyTc = (GT_BOOL)inFields[14];
    actionPtr->ingressStandard.qos.qosParamsModify.modifyUp = (GT_BOOL)inFields[15];
    actionPtr->ingressStandard.qos.qosParamsModify.modifyDp = (GT_BOOL)inFields[16];
    actionPtr->ingressStandard.qos.qosParamsModify.modifyDscp = (GT_BOOL)inFields[17];
    actionPtr->ingressStandard.qos.qosParamsModify.modifyExp = (GT_BOOL)inFields[18];
    actionPtr->ingressStandard.qos.qosPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[19];
    actionPtr->ingressStandard.vlan.precedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[20];
    actionPtr->ingressStandard.vlan.nestedVlan = (GT_BOOL)inFields[21];
    actionPtr->ingressStandard.vlan.vlanCmd = (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[22];
    actionPtr->ingressStandard.vlan.vidOrInLif = (GT_U16)inFields[23];
    actionPtr->ingressStandard.redirect.redirectCmd = (CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_ENT)inFields[24];
    actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inFields[25];
    actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.devPort.devNum = (GT_U8)inFields[26];
    actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.devPort.portNum = (GT_U8)inFields[27];
    actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[28];
    actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.vidx = (GT_U16)inFields[29];
    actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.vlanId = (GT_U16)inFields[30];
    actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.outlifType = PRV_CNV_OLD_TO_NEW_OUT_LIF_TYPE_MAC((GT_U16)inFields[31]);
    actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.outlifPointer.tunnelStartPtr.ptr = (GT_U32)inFields[32];
    actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.outlifPointer.ditPtr = (GT_U32)inFields[33];
    actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.outlifPointer.tunnelStartPtr.passengerPacketType= (GT_U32)inFields[34];
    actionPtr->ingressStandard.redirect.data.outIf.vntL2Echo = (GT_BOOL)inFields[35];
    if (actionPtr->ingressStandard.redirect.redirectCmd == CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
    {
        /* union member, relvant for redirection to route entry only */
       actionPtr->ingressStandard.redirect.data.ipNextHopEntryIndex = (GT_U32)inFields[36];
    }
   /* actionPtr->ingressStandard.fa.flowId = (GT_U32)inFields[37]; */
    actionPtr->ingressStandard.policer.activateCounter = (GT_BOOL)inFields[38];
    actionPtr->ingressStandard.policer.activateMeter = (GT_BOOL)inFields[39];
    actionPtr->ingressStandard.policer.policerPtr = (GT_U32)inFields[40];

    if (inFields[2] == GT_FALSE)
    {
        /* no Update */
        galtisOutput(outArgs, GT_OK, "");
        return CMD_OK;
    }

    if (useExternalTcam == GT_FALSE)
    {
        tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
        actionInfo.internalTcamRuleStartIndex = ruleIndex;

    }
    else
    {
        tcamType = CPSS_EXMXPM_TCAM_TYPE_EXTERNAL_E;

        wrCpssExMxPmPclExtConfigIndexGet(devNum, ruleIndex, &index);
        if (index == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nExternal Tcam configs  not set\n");

            return CMD_AGENT_ERROR;
        }

        actionInfo.externalTcamInfo.actionInfo.actionStartIndex=extTcamConfigPclTable[index].actionStartIndex;
        actionInfo.externalTcamInfo.actionInfo.memoryType=extTcamConfigPclTable[index].memoryType;
    }


    /* call cpss api function */
    ruleSize = (CPSS_PCL_RULE_SIZE_ENT)inFields[3];
    result = cpssExMxPmPclRuleActionSet(
        devNum,
        tcamType,
        &actionInfo,
        CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E,
        actionPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclActionGetFirst (table command)
*
* DESCRIPTION:
*   The function gets action entry from the action table
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
**       ruleIndex       - index of the rule in the TCAM. See cpssExMxPclRuleSet.
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
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclActionGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    WR_PCL_ACTION_STC*                   actionTable =
        (useExternalTcam == GT_FALSE) ? actionIntTable : actionExtTable;
    CPSS_EXMXPM_PCL_ACTION_UNT           *actionPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    acionEntryIndex = 0;
    while(acionEntryIndex < ACTION_TABLE_SIZE && !actionTable[acionEntryIndex].valid)
        acionEntryIndex++;

    if (acionEntryIndex == ACTION_TABLE_SIZE)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    actionPtr = &(actionTable[acionEntryIndex].actionEntry);
    inFields[0] = actionTable[acionEntryIndex].devNum;
    inFields[1] = actionTable[acionEntryIndex].ruleIndex;


    /* inFields[2] == isUpdate and inFields[3] == ruleSize is needed only in set mode */
    inFields[4] = actionPtr->ingressStandard.pktCmd;
    inFields[5] = actionPtr->ingressStandard.mirror.cpuCode;
    inFields[6] = actionPtr->ingressStandard.mirror.ingressPolicyAnalyzerNumber;
    inFields[7] = 0;
    inFields[8] = 0;
    inFields[9] = actionPtr->ingressStandard.qos.qosParams.tc;
    inFields[10] = actionPtr->ingressStandard.qos.qosParams.dp;
    inFields[11] = actionPtr->ingressStandard.qos.qosParams.up ;
    inFields[12] = actionPtr->ingressStandard.qos.qosParams.dscp;
    inFields[13] = actionPtr->ingressStandard.qos.qosParams.exp;
    inFields[14] = actionPtr->ingressStandard.qos.qosParamsModify.modifyTc;
    inFields[15] = actionPtr->ingressStandard.qos.qosParamsModify.modifyUp;
    inFields[16] = actionPtr->ingressStandard.qos.qosParamsModify.modifyDp;
    inFields[17] = actionPtr->ingressStandard.qos.qosParamsModify.modifyDscp;
    inFields[18] = actionPtr->ingressStandard.qos.qosParamsModify.modifyExp;
    inFields[19] = actionPtr->ingressStandard.qos.qosPrecedence;
    inFields[20] = actionPtr->ingressStandard.vlan.precedence;
    inFields[21] = actionPtr->ingressStandard.vlan.nestedVlan;
    inFields[22] = actionPtr->ingressStandard.vlan.vlanCmd;
    inFields[23] = actionPtr->ingressStandard.vlan.vidOrInLif;
    inFields[24] = actionPtr->ingressStandard.redirect.redirectCmd;
    inFields[25] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.outlifType;
    inFields[26] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.devPort.devNum;
    inFields[27] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.devPort.portNum;
    inFields[28] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.trunkId;
    inFields[29] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.vidx;
    inFields[30] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.vlanId;
    inFields[31] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.outlifType;
    inFields[32] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.outlifPointer.tunnelStartPtr.ptr;
    inFields[33] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.outlifPointer.ditPtr;
    inFields[34] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.outlifPointer.tunnelStartPtr.passengerPacketType;
    inFields[35] = actionPtr->ingressStandard.redirect.data.outIf.vntL2Echo;
    inFields[36] = actionPtr->ingressStandard.redirect.data.ipNextHopEntryIndex;
    inFields[37] = 0;
    inFields[38] = actionPtr->ingressStandard.policer.activateCounter;
    inFields[39] = actionPtr->ingressStandard.policer.activateMeter;
    inFields[40] = actionPtr->ingressStandard.policer.policerPtr;

    acionEntryIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],
                inFields[2],  inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],  inFields[9],
                inFields[10], inFields[11], inFields[12], inFields[13],
                inFields[14], inFields[15], inFields[16], inFields[17],
                inFields[18], inFields[19], inFields[20], inFields[21],
                inFields[22], inFields[23], inFields[24], inFields[25],
                inFields[26], inFields[27], inFields[28], inFields[29],
                inFields[30], inFields[31], inFields[32], inFields[33],
                inFields[34], inFields[35], inFields[36], inFields[37],
                inFields[38], inFields[39], inFields[40]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclActionGetNext (table command)
*
* DESCRIPTION:
*   The function gets action entry from the action table
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
**       ruleIndex       - index of the rule in the TCAM. See cpssExMxPclRuleSet.
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
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclActionGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    WR_PCL_ACTION_STC*                   actionTable =
        (useExternalTcam == GT_FALSE) ? actionIntTable : actionExtTable;
    CPSS_EXMXPM_PCL_ACTION_UNT          *actionPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    while(acionEntryIndex < ACTION_TABLE_SIZE && !actionTable[acionEntryIndex].valid)
        acionEntryIndex++;

    if (acionEntryIndex >= ACTION_TABLE_SIZE)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    actionPtr = &(actionTable[acionEntryIndex].actionEntry);
    inFields[0] = actionTable[acionEntryIndex].devNum;
    inFields[1] = actionTable[acionEntryIndex].ruleIndex;
    /* inFields[2] == isUpdate and inFields[3] == ruleSize is needed only in set mode */
    inFields[4] = actionPtr->ingressStandard.pktCmd;
    inFields[5] = actionPtr->ingressStandard.mirror.cpuCode;
    inFields[6] = actionPtr->ingressStandard.mirror.ingressPolicyAnalyzerNumber;
    inFields[7] = 0;
    inFields[8] = 0;
    inFields[9] = actionPtr->ingressStandard.qos.qosParams.tc;
    inFields[10] = actionPtr->ingressStandard.qos.qosParams.dp;
    inFields[11] = actionPtr->ingressStandard.qos.qosParams.up ;
    inFields[12] = actionPtr->ingressStandard.qos.qosParams.dscp;
    inFields[13] = actionPtr->ingressStandard.qos.qosParams.exp;
    inFields[14] = actionPtr->ingressStandard.qos.qosParamsModify.modifyTc;
    inFields[15] = actionPtr->ingressStandard.qos.qosParamsModify.modifyUp;
    inFields[16] = actionPtr->ingressStandard.qos.qosParamsModify.modifyDp;
    inFields[17] = actionPtr->ingressStandard.qos.qosParamsModify.modifyDscp;
    inFields[18] = actionPtr->ingressStandard.qos.qosParamsModify.modifyExp;
    inFields[19] = actionPtr->ingressStandard.qos.qosPrecedence;
    inFields[20] = actionPtr->ingressStandard.vlan.precedence;
    inFields[21] = actionPtr->ingressStandard.vlan.nestedVlan;
    inFields[22] = actionPtr->ingressStandard.vlan.vlanCmd;
    inFields[23] = actionPtr->ingressStandard.vlan.vidOrInLif;
    inFields[24] = actionPtr->ingressStandard.redirect.redirectCmd;
    inFields[25] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.outlifType;
    inFields[26] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.devPort.devNum;
    inFields[27] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.devPort.portNum;
    inFields[28] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.trunkId;
    inFields[29] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.vidx;
    inFields[30] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.interfaceInfo.vlanId;
    inFields[31] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.outlifType;
    inFields[32] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.outlifPointer.tunnelStartPtr.ptr;
    inFields[33] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.outlifPointer.ditPtr;
    inFields[34] = actionPtr->ingressStandard.redirect.data.outIf.outLifInfo.outlifPointer.tunnelStartPtr.passengerPacketType;
    inFields[35] = actionPtr->ingressStandard.redirect.data.outIf.vntL2Echo;
    inFields[36] = actionPtr->ingressStandard.redirect.data.ipNextHopEntryIndex;
    inFields[37] = 0;
    inFields[38] = actionPtr->ingressStandard.policer.activateCounter;
    inFields[39] = actionPtr->ingressStandard.policer.activateMeter;
    inFields[40] = actionPtr->ingressStandard.policer.policerPtr;

    acionEntryIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],
                inFields[2],  inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],  inFields[9],
                inFields[10], inFields[11], inFields[12], inFields[13],
                inFields[14], inFields[15], inFields[16], inFields[17],
                inFields[18], inFields[19], inFields[20], inFields[21],
                inFields[22], inFields[23], inFields[24], inFields[25],
                inFields[26], inFields[27], inFields[28], inFields[29],
                inFields[30], inFields[31], inFields[32], inFields[33],
                inFields[34], inFields[35], inFields[36], inFields[37],
                inFields[38], inFields[39], inFields[40]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmPclIngressRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
* APPLICABLE DEVICES:  All ExMxPm devices
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
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*                          The format of mask is defined by ruleFormat
*
*       patternPtr       - rule pattern.
*                          The format of pattern is defined by ruleFormat
*
*       actionPtr        - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_BAD_PTR     - on NULL pointer value
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressRule_STD_NOT_IP_WriteFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_BYTE_ARRY                       maskBArr, patternBArr;

    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleIngrStandardNotIp.commonIngrStd.pclId = (GT_U16)inFields[2];
        maskData.ruleIngrStandardNotIp.commonIngrStd.srcPort = (GT_U8)inFields[3];
        maskData.ruleIngrStandardNotIp.commonIngrStdPkt.isTagged = (GT_U8)inFields[4];
        maskData.ruleIngrStandardNotIp.commonIngrStd.vid = (GT_U16)inFields[5];
        maskData.ruleIngrStandardNotIp.commonIngrStd.up = (GT_U8)inFields[6];
        maskData.ruleIngrStandardNotIp.commonIngrStd.isIp = (GT_U8)inFields[7];
        maskData.ruleIngrStandardNotIp.commonIngrStdPkt.isL2Valid = (GT_U8)inFields[8];
        maskData.ruleIngrStandardNotIp.commonIngrStd.macToMe = (GT_U8)inFields[9];
        maskData.ruleIngrStandardNotIp.commonIngrStdPkt.isUdbValid = (GT_U8)inFields[10];
        maskData.ruleIngrStandardNotIp.commonIngrStd.isIpv4 = (GT_BOOL)inFields[11];
        maskData.ruleIngrStandardNotIp.l2EncapType = (GT_U8)inFields[12];
        maskData.ruleIngrStandardNotIp.ethType = (GT_U16)inFields[13];
        maskData.ruleIngrStandardNotIp.commonIngrStdPkt.isArp = (GT_U8)inFields[14];

        galtisMacAddr(&maskData.ruleIngrStandardNotIp.macDa, (GT_U8*)inFields[15]);
        galtisMacAddr(&maskData.ruleIngrStandardNotIp.macSa, (GT_U8*)inFields[16]);
        galtisBArray(&maskBArr,(GT_U8*)inFields[17]);
        cmdOsMemCpy(
            maskData.ruleIngrStandardNotIp.udb, maskBArr.data,
            maskBArr.length);


        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleIngrStandardNotIp.commonIngrStd.pclId = (GT_U16)inFields[2];
        patternData.ruleIngrStandardNotIp.commonIngrStd.srcPort = (GT_U8)inFields[3];
        patternData.ruleIngrStandardNotIp.commonIngrStdPkt.isTagged = (GT_U8)inFields[4];
        patternData.ruleIngrStandardNotIp.commonIngrStd.vid = (GT_U16)inFields[5];
        patternData.ruleIngrStandardNotIp.commonIngrStd.up = (GT_U8)inFields[6];
        patternData.ruleIngrStandardNotIp.commonIngrStd.isIp = (GT_U8)inFields[7];
        patternData.ruleIngrStandardNotIp.commonIngrStdPkt.isL2Valid = (GT_U8)inFields[8];
        patternData.ruleIngrStandardNotIp.commonIngrStd.macToMe = (GT_U8)inFields[9];
        patternData.ruleIngrStandardNotIp.commonIngrStdPkt.isUdbValid = (GT_U8)inFields[10];
        patternData.ruleIngrStandardNotIp.commonIngrStd.isIpv4 = (GT_BOOL)inFields[11];
        patternData.ruleIngrStandardNotIp.l2EncapType = (GT_U8)inFields[12];
        patternData.ruleIngrStandardNotIp.ethType = (GT_U16)inFields[13];
        patternData.ruleIngrStandardNotIp.commonIngrStdPkt.isArp = (GT_U8)inFields[14];

        galtisMacAddr(&patternData.ruleIngrStandardNotIp.macDa, (GT_U8*)inFields[15]);
        galtisMacAddr(&patternData.ruleIngrStandardNotIp.macSa, (GT_U8*)inFields[16]);
        galtisBArray(&patternBArr,(GT_U8*)inFields[17]);
        cmdOsMemCpy(
            patternData.ruleIngrStandardNotIp.udb, patternBArr.data,
            patternBArr.length);


        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}


/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressRule_STD_NOT_IP_WriteNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_BYTE_ARRY                       maskBArr, patternBArr;

    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;

    /* map input arguments to locals */
    /* inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1 */

    if(inFields[1] == 0) /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];



        maskData.ruleIngrStandardNotIp.commonIngrStd.pclId = (GT_U16)inFields[2];
        maskData.ruleIngrStandardNotIp.commonIngrStd.srcPort = (GT_U8)inFields[3];
        maskData.ruleIngrStandardNotIp.commonIngrStdPkt.isTagged = (GT_U8)inFields[4];
        maskData.ruleIngrStandardNotIp.commonIngrStd.vid = (GT_U16)inFields[5];
        maskData.ruleIngrStandardNotIp.commonIngrStd.up = (GT_U8)inFields[6];
        maskData.ruleIngrStandardNotIp.commonIngrStd.isIp = (GT_U8)inFields[7];
        maskData.ruleIngrStandardNotIp.commonIngrStdPkt.isL2Valid = (GT_U8)inFields[8];
        maskData.ruleIngrStandardNotIp.commonIngrStd.macToMe = (GT_U8)inFields[9];
        maskData.ruleIngrStandardNotIp.commonIngrStdPkt.isUdbValid = (GT_U8)inFields[10];
        maskData.ruleIngrStandardNotIp.commonIngrStd.isIpv4 = (GT_BOOL)inFields[11];
        maskData.ruleIngrStandardNotIp.l2EncapType = (GT_U8)inFields[12];
        maskData.ruleIngrStandardNotIp.ethType = (GT_U16)inFields[13];
        maskData.ruleIngrStandardNotIp.commonIngrStdPkt.isArp = (GT_U8)inFields[14];

        galtisMacAddr(&maskData.ruleIngrStandardNotIp.macDa, (GT_U8*)inFields[15]);
        galtisMacAddr(&maskData.ruleIngrStandardNotIp.macSa, (GT_U8*)inFields[16]);
        galtisBArray(&maskBArr,(GT_U8*)inFields[17]);
        cmdOsMemCpy(
            maskData.ruleIngrStandardNotIp.udb, maskBArr.data,
            maskBArr.length);


        mask_set = GT_TRUE;

    }
    else  /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleIngrStandardNotIp.commonIngrStd.pclId = (GT_U16)inFields[2];
        patternData.ruleIngrStandardNotIp.commonIngrStd.srcPort = (GT_U8)inFields[3];
        patternData.ruleIngrStandardNotIp.commonIngrStdPkt.isTagged = (GT_U8)inFields[4];
        patternData.ruleIngrStandardNotIp.commonIngrStd.vid = (GT_U16)inFields[5];
        patternData.ruleIngrStandardNotIp.commonIngrStd.up = (GT_U8)inFields[6];
        patternData.ruleIngrStandardNotIp.commonIngrStd.isIp = (GT_U8)inFields[7];
        patternData.ruleIngrStandardNotIp.commonIngrStdPkt.isL2Valid = (GT_U8)inFields[8];
        patternData.ruleIngrStandardNotIp.commonIngrStd.macToMe = (GT_U8)inFields[9];
        patternData.ruleIngrStandardNotIp.commonIngrStdPkt.isUdbValid = (GT_U8)inFields[10];
        patternData.ruleIngrStandardNotIp.commonIngrStd.isIpv4 = (GT_BOOL)inFields[11];
        patternData.ruleIngrStandardNotIp.l2EncapType = (GT_U8)inFields[12];
        patternData.ruleIngrStandardNotIp.ethType = (GT_U16)inFields[13];
        patternData.ruleIngrStandardNotIp.commonIngrStdPkt.isArp = (GT_U8)inFields[14];

        galtisMacAddr(&patternData.ruleIngrStandardNotIp.macDa, (GT_U8*)inFields[15]);
        galtisMacAddr(&patternData.ruleIngrStandardNotIp.macSa, (GT_U8*)inFields[16]);
        galtisBArray(&patternBArr,(GT_U8*)inFields[17]);
        cmdOsMemCpy(
            patternData.ruleIngrStandardNotIp.udb, patternBArr.data,
            patternBArr.length);


        pattern_set = GT_TRUE;
    }


    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressRule_STD_IP_L2_QOS_WriteFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_BYTE_ARRY                       maskBArr, patternBArr;

    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;

    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) { /* mask */





        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.pclId = (GT_U16)inFields[2];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.srcPort = (GT_U8)inFields[3];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isTagged = (GT_U8)inFields[4];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.vid = (GT_U16)inFields[5];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.up = (GT_U8)inFields[6];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.isIp = (GT_U8)inFields[7];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isL2Valid = (GT_U8)inFields[8];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isUdbValid = (GT_U8)inFields[9];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.macToMe = (GT_U8)inFields[10];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.isIpv4 = (GT_U8)inFields[11];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.ipProtocol = (GT_U8)inFields[12];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.dscp = (GT_U8)inFields[13];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.isL4Valid = (GT_U8)inFields[14];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.l4Byte2 = (GT_U8)inFields[15];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.l4Byte3 = (GT_U8)inFields[16];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.ipHeaderOk = (GT_U8)inFields[17];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.isIpv4Fragment = (GT_U8)inFields[18];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isArp = (GT_U8)inFields[19];
        maskData.ruleIngrStandardIpL2Qos.isIpv6HopByHop = (GT_U8)inFields[20];
        galtisMacAddr(&maskData.ruleIngrStandardIpL2Qos.macDa, (GT_U8*)inFields[21]);
        galtisMacAddr(&maskData.ruleIngrStandardIpL2Qos.macSa, (GT_U8*)inFields[22]);
        galtisBArray(&maskBArr,(GT_U8*)inFields[23]);
        cmdOsMemCpy(
            maskData.ruleIngrStandardIpL2Qos.udb, maskBArr.data,
            maskBArr.length);
        maskData.ruleIngrStandardIpL2Qos.isIpv6Eh = (GT_U8)inFields[24];
        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.pclId = (GT_U16)inFields[2];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.srcPort = (GT_U8)inFields[3];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isTagged = (GT_U8)inFields[4];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.vid = (GT_U16)inFields[5];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.up = (GT_U8)inFields[6];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.isIp = (GT_U8)inFields[7];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isL2Valid = (GT_U8)inFields[8];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isUdbValid = (GT_U8)inFields[9];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.macToMe = (GT_U8)inFields[10];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.isIpv4 = (GT_U8)inFields[11];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.ipProtocol = (GT_U8)inFields[12];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.dscp = (GT_U8)inFields[13];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.isL4Valid = (GT_U8)inFields[14];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.l4Byte2 = (GT_U8)inFields[15];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.l4Byte3 = (GT_U8)inFields[16];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.ipHeaderOk = (GT_U8)inFields[17];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.isIpv4Fragment = (GT_U8)inFields[18];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isArp = (GT_U8)inFields[19];
        patternData.ruleIngrStandardIpL2Qos.isIpv6HopByHop = (GT_U8)inFields[20];
        galtisMacAddr(&patternData.ruleIngrStandardIpL2Qos.macDa, (GT_U8*)inFields[21]);
        galtisMacAddr(&patternData.ruleIngrStandardIpL2Qos.macSa, (GT_U8*)inFields[22]);
        galtisBArray(&patternBArr,(GT_U8*)inFields[23]);
        cmdOsMemCpy(
            patternData.ruleIngrStandardIpL2Qos.udb, patternBArr.data,
            patternBArr.length);
        patternData.ruleIngrStandardIpL2Qos.isIpv6Eh = (GT_U8)inFields[24];
        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressRule_STD_IP_L2_QOS_WriteNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_BYTE_ARRY                       maskBArr, patternBArr;


    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;

    /* map input arguments to locals */
    /* inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1 */

    if(inFields[1] == 0) { /* mask */
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.pclId = (GT_U16)inFields[2];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.srcPort = (GT_U8)inFields[3];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isTagged = (GT_U8)inFields[4];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.vid = (GT_U16)inFields[5];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.up = (GT_U8)inFields[6];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.isIp = (GT_U8)inFields[7];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isL2Valid = (GT_U8)inFields[8];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isUdbValid = (GT_U8)inFields[9];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.macToMe = (GT_U8)inFields[10];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.isIpv4 = (GT_U8)inFields[11];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.ipProtocol = (GT_U8)inFields[12];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.dscp = (GT_U8)inFields[13];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.isL4Valid = (GT_U8)inFields[14];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.l4Byte2 = (GT_U8)inFields[15];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.l4Byte3 = (GT_U8)inFields[16];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.ipHeaderOk = (GT_U8)inFields[17];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.isIpv4Fragment = (GT_U8)inFields[18];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isArp = (GT_U8)inFields[19];
        maskData.ruleIngrStandardIpL2Qos.isIpv6HopByHop = (GT_U8)inFields[20];
        galtisMacAddr(&maskData.ruleIngrStandardIpL2Qos.macDa, (GT_U8*)inFields[21]);
        galtisMacAddr(&maskData.ruleIngrStandardIpL2Qos.macSa, (GT_U8*)inFields[22]);
        galtisBArray(&maskBArr,(GT_U8*)inFields[23]);
        cmdOsMemCpy(
            maskData.ruleIngrStandardIpL2Qos.udb, maskBArr.data,
            maskBArr.length);
        maskData.ruleIngrStandardIpL2Qos.isIpv6Eh = (GT_U8)inFields[24];
        mask_set = GT_TRUE;
    }
    else  /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.pclId = (GT_U16)inFields[2];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.srcPort = (GT_U8)inFields[3];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isTagged = (GT_U8)inFields[4];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.vid = (GT_U16)inFields[5];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.up = (GT_U8)inFields[6];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.isIp = (GT_U8)inFields[7];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isL2Valid = (GT_U8)inFields[8];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isUdbValid = (GT_U8)inFields[9];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.macToMe = (GT_U8)inFields[10];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.isIpv4 = (GT_U8)inFields[11];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.ipProtocol = (GT_U8)inFields[12];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.dscp = (GT_U8)inFields[13];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.isL4Valid = (GT_U8)inFields[14];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.l4Byte2 = (GT_U8)inFields[15];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.l4Byte3 = (GT_U8)inFields[16];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.ipHeaderOk = (GT_U8)inFields[17];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.isIpv4Fragment = (GT_U8)inFields[18];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isArp = (GT_U8)inFields[19];
        patternData.ruleIngrStandardIpL2Qos.isIpv6HopByHop = (GT_U8)inFields[20];
        galtisMacAddr(&patternData.ruleIngrStandardIpL2Qos.macDa, (GT_U8*)inFields[21]);
        galtisMacAddr(&patternData.ruleIngrStandardIpL2Qos.macSa, (GT_U8*)inFields[22]);
        galtisBArray(&patternBArr,(GT_U8*)inFields[23]);
        cmdOsMemCpy(
            patternData.ruleIngrStandardIpL2Qos.udb, patternBArr.data,
            patternBArr.length);
        patternData.ruleIngrStandardIpL2Qos.isIpv6Eh = (GT_U8)inFields[24];
        pattern_set = GT_TRUE;

    }


    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressRule_STD_IPV4_L4_WriteFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_BYTE_ARRY                       maskBArr, patternBArr;


    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E;
    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0) { /* mask */
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.pclId = (GT_U16)inFields[2];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.srcPort = (GT_U8)inFields[3];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isTagged = (GT_U8)inFields[4];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.vid = (GT_U16)inFields[5];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.up = (GT_U8)inFields[6];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.isIp = (GT_U8)inFields[7];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isL2Valid = (GT_U8)inFields[8];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isUdbValid = (GT_U8)inFields[9];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.macToMe = (GT_U8)inFields[10];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.isIpv4 = (GT_U8)inFields[11];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.ipProtocol = (GT_U8)inFields[12];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.dscp = (GT_U8)inFields[13];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.isL4Valid = (GT_U8)inFields[14];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.l4Byte2 = (GT_U8)inFields[15];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.l4Byte3 = (GT_U8)inFields[16];
        maskData.ruleIngrStandardIpv4L4.l4Byte0 = (GT_U8)inFields[17];
        maskData.ruleIngrStandardIpv4L4.l4Byte1 = (GT_U8)inFields[18];
        maskData.ruleIngrStandardIpv4L4.l4Byte13 = (GT_U8)inFields[19];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.ipHeaderOk = (GT_U8)inFields[20];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.isIpv4Fragment = (GT_U8)inFields[21];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isArp = (GT_BOOL)inFields[22];
        galtisIpAddr(&maskData.ruleIngrStandardIpv4L4.sip, (GT_U8*)inFields[23]);
        galtisIpAddr(&maskData.ruleIngrStandardIpv4L4.dip, (GT_U8*)inFields[24]);
        galtisBArray(&maskBArr,(GT_U8*)inFields[25]);
        cmdOsMemCpy(
            maskData.ruleIngrStandardIpv4L4.udb, maskBArr.data,
            maskBArr.length);
        maskData.ruleIngrStandardIpv4L4.isBc = (GT_U8)inFields[26];
        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.pclId = (GT_U16)inFields[2];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.srcPort = (GT_U8)inFields[3];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isTagged = (GT_U8)inFields[4];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.vid = (GT_U16)inFields[5];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.up = (GT_U8)inFields[6];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.isIp = (GT_U8)inFields[7];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isL2Valid = (GT_U8)inFields[8];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isUdbValid = (GT_U8)inFields[9];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.macToMe = (GT_U8)inFields[10];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.isIpv4 = (GT_U8)inFields[11];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.ipProtocol = (GT_U8)inFields[12];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.dscp = (GT_U8)inFields[13];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.isL4Valid = (GT_U8)inFields[14];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.l4Byte2 = (GT_U8)inFields[15];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.l4Byte3 = (GT_U8)inFields[16];
        patternData.ruleIngrStandardIpv4L4.l4Byte0 = (GT_U8)inFields[17];
        patternData.ruleIngrStandardIpv4L4.l4Byte1 = (GT_U8)inFields[18];
        patternData.ruleIngrStandardIpv4L4.l4Byte13 = (GT_U8)inFields[19];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.ipHeaderOk = (GT_U8)inFields[20];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.isIpv4Fragment = (GT_U8)inFields[21];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isArp = (GT_BOOL)inFields[22];
        galtisIpAddr(&patternData.ruleIngrStandardIpv4L4.sip, (GT_U8*)inFields[23]);
        galtisIpAddr(&patternData.ruleIngrStandardIpv4L4.dip, (GT_U8*)inFields[24]);
        galtisBArray(&patternBArr,(GT_U8*)inFields[25]);
        cmdOsMemCpy(
            patternData.ruleIngrStandardIpv4L4.udb, patternBArr.data,
            patternBArr.length);
        patternData.ruleIngrStandardIpv4L4.isBc = (GT_U8)inFields[26];
        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}

/******************************************************************************/

static CMD_STATUS wrCpssExMxPmPclIngressRule_STD_IPV4_L4_WriteNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_BYTE_ARRY                       maskBArr, patternBArr;


    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E;

    /* map input arguments to locals */
    /* inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1 */

    if(inFields[1] == 0) /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.pclId = (GT_U16)inFields[2];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.srcPort = (GT_U8)inFields[3];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isTagged = (GT_U8)inFields[4];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.vid = (GT_U16)inFields[5];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.up = (GT_U8)inFields[6];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.isIp = (GT_U8)inFields[7];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isL2Valid = (GT_U8)inFields[8];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isUdbValid = (GT_U8)inFields[9];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.macToMe = (GT_U8)inFields[10];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.isIpv4 = (GT_U8)inFields[11];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.ipProtocol = (GT_U8)inFields[12];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.dscp = (GT_U8)inFields[13];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.isL4Valid = (GT_U8)inFields[14];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.l4Byte2 = (GT_U8)inFields[15];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.l4Byte3 = (GT_U8)inFields[16];
        maskData.ruleIngrStandardIpv4L4.l4Byte0 = (GT_U8)inFields[17];
        maskData.ruleIngrStandardIpv4L4.l4Byte1 = (GT_U8)inFields[18];
        maskData.ruleIngrStandardIpv4L4.l4Byte13 = (GT_U8)inFields[19];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.ipHeaderOk = (GT_U8)inFields[20];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.isIpv4Fragment = (GT_U8)inFields[21];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isArp = (GT_BOOL)inFields[22];
        galtisIpAddr(&maskData.ruleIngrStandardIpv4L4.sip, (GT_U8*)inFields[23]);
        galtisIpAddr(&maskData.ruleIngrStandardIpv4L4.dip, (GT_U8*)inFields[24]);
        galtisBArray(&maskBArr,(GT_U8*)inFields[25]);
            cmdOsMemCpy(
            maskData.ruleIngrStandardIpv4L4.udb, maskBArr.data,
            maskBArr.length);
        maskData.ruleIngrStandardIpv4L4.isBc = (GT_U8)inFields[26];
        mask_set = GT_TRUE;
    }
    else  /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.pclId = (GT_U16)inFields[2];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.srcPort = (GT_U8)inFields[3];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isTagged = (GT_U8)inFields[4];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.vid = (GT_U16)inFields[5];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.up = (GT_U8)inFields[6];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.isIp = (GT_U8)inFields[7];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isL2Valid = (GT_U8)inFields[8];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isUdbValid = (GT_U8)inFields[9];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.macToMe = (GT_U8)inFields[10];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.isIpv4 = (GT_U8)inFields[11];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.ipProtocol = (GT_U8)inFields[12];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.dscp = (GT_U8)inFields[13];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.isL4Valid = (GT_U8)inFields[14];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.l4Byte2 = (GT_U8)inFields[15];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.l4Byte3 = (GT_U8)inFields[16];
        patternData.ruleIngrStandardIpv4L4.l4Byte0 = (GT_U8)inFields[17];
        patternData.ruleIngrStandardIpv4L4.l4Byte1 = (GT_U8)inFields[18];
        patternData.ruleIngrStandardIpv4L4.l4Byte13 = (GT_U8)inFields[19];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.ipHeaderOk = (GT_U8)inFields[20];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.isIpv4Fragment = (GT_U8)inFields[21];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isArp = (GT_BOOL)inFields[22];
        galtisIpAddr(&patternData.ruleIngrStandardIpv4L4.sip, (GT_U8*)inFields[23]);
        galtisIpAddr(&patternData.ruleIngrStandardIpv4L4.dip, (GT_U8*)inFields[24]);
        galtisBArray(&patternBArr,(GT_U8*)inFields[25]);
            cmdOsMemCpy(
                patternData.ruleIngrStandardIpv4L4.udb, patternBArr.data,
                patternBArr.length);
        patternData.ruleIngrStandardIpv4L4.isBc = (GT_U8)inFields[26];
        pattern_set = GT_TRUE;
    }


    return CMD_OK;
}



/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressRule_EXT_NOT_IPV6_WriteFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_BYTE_ARRY                       maskBArr, patternBArr;
    GT_IPADDR                          wrkIpAddr;

    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_NOT_IPV6_E;


    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0)  /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.pclId = (GT_U16)inFields[2];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.srcPort = (GT_U8)inFields[3];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isTagged = (GT_U8)inFields[4];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.vid = (GT_U16)inFields[5];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.up = (GT_U8)inFields[6];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.isIp = (GT_U8)inFields[7];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isL2Valid = (GT_U8)inFields[8];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isUdbValid = (GT_U8)inFields[9];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.macToMe = (GT_U8)inFields[10];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.isIpv6 = (GT_U8)inFields[11];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.ipProtocol = (GT_U8)inFields[12];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.dscp = (GT_U8)inFields[13];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isL4Valid = (GT_U8)inFields[14];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte0 = (GT_U8)inFields[15];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte1 = (GT_U8)inFields[16];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte2 = (GT_U8)inFields[17];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte3 = (GT_U8)inFields[18];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte13 = (GT_U8)inFields[19];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.ipHeaderOk = (GT_U8)inFields[20];

        galtisIpAddr(&wrkIpAddr, (GT_U8*)inFields[21]);
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[0] = wrkIpAddr.arIP[0];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[1] = wrkIpAddr.arIP[1];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[2] = wrkIpAddr.arIP[2];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[3] = wrkIpAddr.arIP[3];
        galtisIpAddr(&wrkIpAddr, (GT_U8*)inFields[22]);
        maskData.ruleIngrExtendedNotIpv6.dipBits31_0[0]                  = wrkIpAddr.arIP[0];
        maskData.ruleIngrExtendedNotIpv6.dipBits31_0[1]                  = wrkIpAddr.arIP[1];
        maskData.ruleIngrExtendedNotIpv6.dipBits31_0[2]                  = wrkIpAddr.arIP[2];
        maskData.ruleIngrExtendedNotIpv6.dipBits31_0[3]                  = wrkIpAddr.arIP[3];
        maskData.ruleIngrExtendedNotIpv6.ethType = (GT_U16)inFields[23];
        maskData.ruleIngrExtendedNotIpv6.l2Encapsulation0 = (GT_U8)inFields[24];
        galtisMacAddr(&maskData.ruleIngrExtendedNotIpv6.macDa, (GT_U8*)inFields[25]);
        galtisMacAddr(&maskData.ruleIngrExtendedNotIpv6.macSa, (GT_U8*)inFields[26]);
        maskData.ruleIngrExtendedNotIpv6.isIpv4Fragment = (GT_BOOL)inFields[27];
        galtisBArray(&maskBArr,(GT_U8*)inFields[28]);
        cmdOsMemCpy(
            maskData.ruleIngrExtendedNotIpv6.udb, maskBArr.data,
            maskBArr.length);
        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.pclId = (GT_U16)inFields[2];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.srcPort = (GT_U8)inFields[3];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isTagged = (GT_U8)inFields[4];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.vid = (GT_U16)inFields[5];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.up = (GT_U8)inFields[6];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.isIp = (GT_U8)inFields[7];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isL2Valid = (GT_U8)inFields[8];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isUdbValid = (GT_U8)inFields[9];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.macToMe = (GT_U8)inFields[10];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.isIpv6 = (GT_U8)inFields[11];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.ipProtocol = (GT_U8)inFields[12];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.dscp = (GT_U8)inFields[13];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isL4Valid = (GT_U8)inFields[14];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte0 = (GT_U8)inFields[15];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte1 = (GT_U8)inFields[16];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte2 = (GT_U8)inFields[17];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte3 = (GT_U8)inFields[18];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte13 = (GT_U8)inFields[19];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.ipHeaderOk = (GT_U8)inFields[20];

        galtisIpAddr(&wrkIpAddr, (GT_U8*)inFields[21]);
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[0] = wrkIpAddr.arIP[0];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[1] = wrkIpAddr.arIP[1];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[2] = wrkIpAddr.arIP[2];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[3] = wrkIpAddr.arIP[3];
        galtisIpAddr(&wrkIpAddr, (GT_U8*)inFields[22]);
        patternData.ruleIngrExtendedNotIpv6.dipBits31_0[0]                  = wrkIpAddr.arIP[0];
        patternData.ruleIngrExtendedNotIpv6.dipBits31_0[1]                  = wrkIpAddr.arIP[1];
        patternData.ruleIngrExtendedNotIpv6.dipBits31_0[2]                  = wrkIpAddr.arIP[2];
        patternData.ruleIngrExtendedNotIpv6.dipBits31_0[3]                  = wrkIpAddr.arIP[3];
        patternData.ruleIngrExtendedNotIpv6.ethType = (GT_U16)inFields[23];
        patternData.ruleIngrExtendedNotIpv6.l2Encapsulation0 = (GT_U8)inFields[24];
        galtisMacAddr(&patternData.ruleIngrExtendedNotIpv6.macDa, (GT_U8*)inFields[25]);
        galtisMacAddr(&patternData.ruleIngrExtendedNotIpv6.macSa, (GT_U8*)inFields[26]);
        patternData.ruleIngrExtendedNotIpv6.isIpv4Fragment = (GT_BOOL)inFields[27];
        galtisBArray(&patternBArr,(GT_U8*)inFields[28]);
        cmdOsMemCpy(
            patternData.ruleIngrExtendedNotIpv6.udb, patternBArr.data,
            patternBArr.length);
        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}



/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressRule_EXT_NOT_IPV6_WriteNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_BYTE_ARRY                       maskBArr, patternBArr;
    GT_IPADDR                          wrkIpAddr;


    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_NOT_IPV6_E;

    /* map input arguments to locals */
    /* inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1 */

    if(inFields[1] == 0)  /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.pclId = (GT_U16)inFields[2];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.srcPort = (GT_U8)inFields[3];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isTagged = (GT_U8)inFields[4];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.vid = (GT_U16)inFields[5];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.up = (GT_U8)inFields[6];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.isIp = (GT_U8)inFields[7];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isL2Valid = (GT_U8)inFields[8];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isUdbValid = (GT_U8)inFields[9];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.macToMe = (GT_U8)inFields[10];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.isIpv6 = (GT_U8)inFields[11];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExt.ipProtocol = (GT_U8)inFields[12];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.dscp = (GT_U8)inFields[13];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isL4Valid = (GT_U8)inFields[14];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte0 = (GT_U8)inFields[15];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte1 = (GT_U8)inFields[16];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte2 = (GT_U8)inFields[17];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte3 = (GT_U8)inFields[18];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte13 = (GT_U8)inFields[19];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.ipHeaderOk = (GT_U8)inFields[20];

        galtisIpAddr(&wrkIpAddr, (GT_U8*)inFields[21]);
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[0] = wrkIpAddr.arIP[0];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[1] = wrkIpAddr.arIP[1];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[2] = wrkIpAddr.arIP[2];
        maskData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[3] = wrkIpAddr.arIP[3];
        galtisIpAddr(&wrkIpAddr, (GT_U8*)inFields[22]);
        maskData.ruleIngrExtendedNotIpv6.dipBits31_0[0]                  = wrkIpAddr.arIP[0];
        maskData.ruleIngrExtendedNotIpv6.dipBits31_0[1]                  = wrkIpAddr.arIP[1];
        maskData.ruleIngrExtendedNotIpv6.dipBits31_0[2]                  = wrkIpAddr.arIP[2];
        maskData.ruleIngrExtendedNotIpv6.dipBits31_0[3]                  = wrkIpAddr.arIP[3];
        maskData.ruleIngrExtendedNotIpv6.ethType = (GT_U16)inFields[23];
        maskData.ruleIngrExtendedNotIpv6.l2Encapsulation0 = (GT_U8)inFields[24];
        galtisMacAddr(&maskData.ruleIngrExtendedNotIpv6.macDa, (GT_U8*)inFields[25]);
        galtisMacAddr(&maskData.ruleIngrExtendedNotIpv6.macSa, (GT_U8*)inFields[26]);
        maskData.ruleIngrExtendedNotIpv6.isIpv4Fragment = (GT_BOOL)inFields[27];
        galtisBArray(&maskBArr,(GT_U8*)inFields[28]);
        cmdOsMemCpy(
            maskData.ruleIngrExtendedNotIpv6.udb, maskBArr.data,
            maskBArr.length);
        mask_set = GT_TRUE;
    }
    else  /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.pclId = (GT_U16)inFields[2];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.srcPort = (GT_U8)inFields[3];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isTagged = (GT_U8)inFields[4];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.vid = (GT_U16)inFields[5];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.up = (GT_U8)inFields[6];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.isIp = (GT_U8)inFields[7];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isL2Valid = (GT_U8)inFields[8];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isUdbValid = (GT_U8)inFields[9];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.macToMe = (GT_U8)inFields[10];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.isIpv6 = (GT_U8)inFields[11];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExt.ipProtocol = (GT_U8)inFields[12];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.dscp = (GT_U8)inFields[13];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.isL4Valid = (GT_U8)inFields[14];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte0 = (GT_U8)inFields[15];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte1 = (GT_U8)inFields[16];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte2 = (GT_U8)inFields[17];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte3 = (GT_U8)inFields[18];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.l4Byte13 = (GT_U8)inFields[19];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.ipHeaderOk = (GT_U8)inFields[20];


        galtisIpAddr(&wrkIpAddr, (GT_U8*)inFields[21]);
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[0] = wrkIpAddr.arIP[0];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[1] = wrkIpAddr.arIP[1];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[2] = wrkIpAddr.arIP[2];
        patternData.ruleIngrExtendedNotIpv6.commonIngrExtPkt.sipBits31_0[3] = wrkIpAddr.arIP[3];
        galtisIpAddr(&wrkIpAddr, (GT_U8*)inFields[22]);
        patternData.ruleIngrExtendedNotIpv6.dipBits31_0[0]                  = wrkIpAddr.arIP[0];
        patternData.ruleIngrExtendedNotIpv6.dipBits31_0[1]                  = wrkIpAddr.arIP[1];
        patternData.ruleIngrExtendedNotIpv6.dipBits31_0[2]                  = wrkIpAddr.arIP[2];
        patternData.ruleIngrExtendedNotIpv6.dipBits31_0[3]                  = wrkIpAddr.arIP[3];
        patternData.ruleIngrExtendedNotIpv6.ethType = (GT_U16)inFields[23];
        patternData.ruleIngrExtendedNotIpv6.l2Encapsulation0 = (GT_U8)inFields[24];
        galtisMacAddr(&patternData.ruleIngrExtendedNotIpv6.macDa, (GT_U8*)inFields[25]);
        galtisMacAddr(&patternData.ruleIngrExtendedNotIpv6.macSa, (GT_U8*)inFields[26]);
        patternData.ruleIngrExtendedNotIpv6.isIpv4Fragment = (GT_BOOL)inFields[27];
        galtisBArray(&patternBArr,(GT_U8*)inFields[28]);
        cmdOsMemCpy(
            patternData.ruleIngrExtendedNotIpv6.udb, patternBArr.data,
            patternBArr.length);
        pattern_set = GT_TRUE;
    }


    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressRule_EXT_IPV6_L2_WriteFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY                       maskBArr, patternBArr;

    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L2_E;

    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0)  /* mask */
    {

        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.pclId = (GT_U16)inFields[2];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.srcPort = (GT_U8)inFields[3];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isTagged = (GT_U8)inFields[4];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.vid = (GT_U16)inFields[5];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.up = (GT_U8)inFields[6];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.isIp = (GT_U8)inFields[7];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isL2Valid = (GT_U8)inFields[8];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isUdbValid = (GT_U8)inFields[9];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.macToMe = (GT_U8)inFields[10];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.isIpv6 = (GT_U8)inFields[11];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.ipProtocol = (GT_U8)inFields[12];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.dscp = (GT_U8)inFields[13];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isL4Valid = (GT_U8)inFields[14];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte0 = (GT_U8)inFields[15];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte1 = (GT_U8)inFields[16];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte2 = (GT_U8)inFields[17];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte3 = (GT_U8)inFields[18];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte13 = (GT_U8)inFields[19];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.ipHeaderOk = (GT_U8)inFields[20];
        galtisBArray(&maskBArr,(GT_U8*)inFields[21]);
        cmdOsMemCpy(
            maskData.ruleIngrExtendedIpv6L2.sipBits127_32,
            maskBArr.data, 12);
        cmdOsMemCpy(
            maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.sipBits31_0,
            (maskBArr.data + 12), 4);

    /*     maskData.ruleIngrExtendedIpv6L2.sipBits127_32[0], (GT_U8)inFields[21]); */
        maskData.ruleIngrExtendedIpv6L2.dipBits127_120 = (GT_U8)inFields[22];
        maskData.ruleIngrExtendedIpv6L2.isIpv6Eh = (GT_U8)inFields[23];
        maskData.ruleIngrExtendedIpv6L2.isIpv6HopByHop = (GT_BOOL)inFields[24];
        galtisMacAddr(&maskData.ruleIngrExtendedIpv6L2.macDa, (GT_U8*)inFields[25]);
        galtisMacAddr(&maskData.ruleIngrExtendedIpv6L2.macSa, (GT_U8*)inFields[26]);
        galtisBArray(&maskBArr,(GT_U8*)inFields[27]);
        cmdOsMemCpy(
            maskData.ruleIngrExtendedIpv6L2.udb, maskBArr.data,
            maskBArr.length);
        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.pclId = (GT_U16)inFields[2];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.srcPort = (GT_U8)inFields[3];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isTagged = (GT_U8)inFields[4];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.vid = (GT_U16)inFields[5];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.up = (GT_U8)inFields[6];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.isIp = (GT_U8)inFields[7];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isL2Valid = (GT_U8)inFields[8];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isUdbValid = (GT_U8)inFields[9];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.macToMe = (GT_U8)inFields[10];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.isIpv6 = (GT_U8)inFields[11];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.ipProtocol = (GT_U8)inFields[12];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.dscp = (GT_U8)inFields[13];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isL4Valid = (GT_U8)inFields[14];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte0 = (GT_U8)inFields[15];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte1 = (GT_U8)inFields[16];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte2 = (GT_U8)inFields[17];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte3 = (GT_U8)inFields[18];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte13 = (GT_U8)inFields[19];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.ipHeaderOk = (GT_U8)inFields[20];
        galtisBArray(&patternBArr,(GT_U8*)inFields[21]);
        cmdOsMemCpy(
            patternData.ruleIngrExtendedIpv6L2.sipBits127_32,
            patternBArr.data, 12);
        cmdOsMemCpy(
            patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.sipBits31_0,
            (patternBArr.data + 12), 4);
        patternData.ruleIngrExtendedIpv6L2.dipBits127_120 = (GT_U8)inFields[22];
        patternData.ruleIngrExtendedIpv6L2.isIpv6Eh = (GT_U8)inFields[23];
        patternData.ruleIngrExtendedIpv6L2.isIpv6HopByHop = (GT_BOOL)inFields[24];
        galtisMacAddr(&patternData.ruleIngrExtendedIpv6L2.macDa, (GT_U8*)inFields[25]);
        galtisMacAddr(&patternData.ruleIngrExtendedIpv6L2.macSa, (GT_U8*)inFields[26]);
        galtisBArray(&patternBArr,(GT_U8*)inFields[27]);
        cmdOsMemCpy(
            patternData.ruleIngrExtendedIpv6L2.udb, patternBArr.data,
            patternBArr.length);

        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}


/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressRule_EXT_IPV6_L2_WriteNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY                       maskBArr, patternBArr;


    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L2_E;

    /* map input arguments to locals */
    /* inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1 */

    if(inFields[1] == 0)  /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.pclId = (GT_U16)inFields[2];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.srcPort = (GT_U8)inFields[3];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isTagged = (GT_U8)inFields[4];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.vid = (GT_U16)inFields[5];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.up = (GT_U8)inFields[6];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.isIp = (GT_U8)inFields[7];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isL2Valid = (GT_U8)inFields[8];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isUdbValid = (GT_U8)inFields[9];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.macToMe = (GT_U8)inFields[10];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.isIpv6 = (GT_U8)inFields[11];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExt.ipProtocol = (GT_U8)inFields[12];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.dscp = (GT_U8)inFields[13];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isL4Valid = (GT_U8)inFields[14];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte0 = (GT_U8)inFields[15];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte1 = (GT_U8)inFields[16];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte2 = (GT_U8)inFields[17];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte3 = (GT_U8)inFields[18];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte13 = (GT_U8)inFields[19];
        maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.ipHeaderOk = (GT_U8)inFields[20];
        galtisBArray(&maskBArr,(GT_U8*)inFields[21]);
        cmdOsMemCpy(
            maskData.ruleIngrExtendedIpv6L2.sipBits127_32,
            maskBArr.data, 12);
        cmdOsMemCpy(
            maskData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.sipBits31_0,
            (maskBArr.data + 12), 4);
        maskData.ruleIngrExtendedIpv6L2.dipBits127_120 = (GT_U8)inFields[22];
        maskData.ruleIngrExtendedIpv6L2.isIpv6Eh = (GT_U8)inFields[23];
        maskData.ruleIngrExtendedIpv6L2.isIpv6HopByHop = (GT_BOOL)inFields[24];
        galtisMacAddr(&maskData.ruleIngrExtendedIpv6L2.macDa, (GT_U8*)inFields[25]);
        galtisMacAddr(&maskData.ruleIngrExtendedIpv6L2.macSa, (GT_U8*)inFields[26]);
        galtisBArray(&maskBArr,(GT_U8*)inFields[27]);
        cmdOsMemCpy(
            maskData.ruleIngrExtendedIpv6L2.udb, maskBArr.data,
            maskBArr.length);
        mask_set = GT_TRUE;
    }
    else  /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.pclId = (GT_U16)inFields[2];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.srcPort = (GT_U8)inFields[3];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isTagged = (GT_U8)inFields[4];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.vid = (GT_U16)inFields[5];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.up = (GT_U8)inFields[6];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.isIp = (GT_U8)inFields[7];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isL2Valid = (GT_U8)inFields[8];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isUdbValid = (GT_U8)inFields[9];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.macToMe = (GT_U8)inFields[10];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.isIpv6 = (GT_U8)inFields[11];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExt.ipProtocol = (GT_U8)inFields[12];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.dscp = (GT_U8)inFields[13];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.isL4Valid = (GT_U8)inFields[14];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte0 = (GT_U8)inFields[15];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte1 = (GT_U8)inFields[16];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte2 = (GT_U8)inFields[17];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte3 = (GT_U8)inFields[18];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.l4Byte13 = (GT_U8)inFields[19];
        patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.ipHeaderOk = (GT_U8)inFields[20];
        galtisBArray(&patternBArr,(GT_U8*)inFields[21]);
        cmdOsMemCpy(
            patternData.ruleIngrExtendedIpv6L2.sipBits127_32,
            patternBArr.data, 12);
        cmdOsMemCpy(
            patternData.ruleIngrExtendedIpv6L2.commonIngrExtPkt.sipBits31_0,
            (patternBArr.data + 12), 4);
        patternData.ruleIngrExtendedIpv6L2.dipBits127_120 = (GT_U8)inFields[22];
        patternData.ruleIngrExtendedIpv6L2.isIpv6Eh = (GT_U8)inFields[23];
        patternData.ruleIngrExtendedIpv6L2.isIpv6HopByHop = (GT_BOOL)inFields[24];
        galtisMacAddr(&patternData.ruleIngrExtendedIpv6L2.macDa, (GT_U8*)inFields[25]);
        galtisMacAddr(&patternData.ruleIngrExtendedIpv6L2.macSa, (GT_U8*)inFields[26]);
        galtisBArray(&patternBArr,(GT_U8*)inFields[27]);
        cmdOsMemCpy(
            patternData.ruleIngrExtendedIpv6L2.udb, patternBArr.data,
            patternBArr.length);

        pattern_set = GT_TRUE;
    }


    return CMD_OK;
}


/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressRule_EXT_IPV6_L4_WriteFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY                       maskBArr, patternBArr;


    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L4_E;

    /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[1] == 0)  /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.pclId = (GT_U16)inFields[2];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.srcPort = (GT_U8)inFields[3];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isTagged = (GT_U8)inFields[4];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.vid = (GT_U16)inFields[5];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.up = (GT_U8)inFields[6];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.isIp = (GT_U8)inFields[7];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isL2Valid = (GT_U8)inFields[8];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isUdbValid = (GT_U8)inFields[9];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.macToMe = (GT_U8)inFields[10];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.isIpv6 = (GT_U8)inFields[11];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.ipProtocol = (GT_U8)inFields[12];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.dscp = (GT_U8)inFields[13];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isL4Valid = (GT_U8)inFields[14];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte0 = (GT_U8)inFields[15];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte1 = (GT_U8)inFields[16];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte2 = (GT_U8)inFields[17];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte3 = (GT_U8)inFields[18];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte13 = (GT_U8)inFields[19];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.ipHeaderOk = (GT_U8)inFields[20];
        /*galtisIpv6Addr(&maskData.ruleIngrExtendedIpv6L4.sip, (GT_U8*)inFields[21]);*/
        if(inFields[21])
        {
            galtisBArray(&maskBArr,(GT_U8*)inFields[21]);
            cmdOsMemCpy(
                maskData.ruleIngrExtendedIpv6L4.sipBits127_32,
                maskBArr.data, 12);
        }
        else
        {
            cmdOsMemSet(maskData.ruleIngrExtendedIpv6L4.sipBits127_32, 0, 12);
        }
        galtisIpv6Addr(&maskData.ruleIngrExtendedIpv6L4.dip, (GT_U8*)inFields[22]);
        maskData.ruleIngrExtendedIpv6L4.isIpv6Eh = (GT_U8)inFields[23];
        maskData.ruleIngrExtendedIpv6L4.isIpv6HopByHop = (GT_U8)inFields[24];

        if (wrCpssExMxPmPclIngressRuleWrite_patched == GT_FALSE)
        {
             galtisBArray(&maskBArr,(GT_U8*)inFields[25]);
             cmdOsMemCpy(
                maskData.ruleIngrExtendedIpv6L4.udb, maskBArr.data,
                3);
        }
        else
        {
            /* error in GUI - 32-bit value instead of hex-array */
            /* used as UDB{15:12}                               */
            maskData.ruleIngrExtendedIpv6L4.udb[12] =
                (GT_U8)(((GT_U32)inFields[25] >> 24) & 0xFF);
            maskData.ruleIngrExtendedIpv6L4.udb[13] =
                (GT_U8)(((GT_U32)inFields[25] >> 16) & 0xFF);
            maskData.ruleIngrExtendedIpv6L4.udb[14] =
                (GT_U8)(((GT_U32)inFields[25] >> 8) & 0xFF);
            maskData.ruleIngrExtendedIpv6L4.udb[15] =
                (GT_U8)((GT_U32)inFields[25] & 0xFF);
        }

        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.pclId = (GT_U16)inFields[2];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.srcPort = (GT_U8)inFields[3];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isTagged = (GT_U8)inFields[4];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.vid = (GT_U16)inFields[5];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.up = (GT_U8)inFields[6];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.isIp = (GT_U8)inFields[7];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isL2Valid = (GT_U8)inFields[8];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isUdbValid = (GT_U8)inFields[9];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.macToMe = (GT_U8)inFields[10];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.isIpv6 = (GT_U8)inFields[11];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.ipProtocol = (GT_U8)inFields[12];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.dscp = (GT_U8)inFields[13];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isL4Valid = (GT_U8)inFields[14];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte0 = (GT_U8)inFields[15];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte1 = (GT_U8)inFields[16];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte2 = (GT_U8)inFields[17];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte3 = (GT_U8)inFields[18];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte13 = (GT_U8)inFields[19];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.ipHeaderOk = (GT_U8)inFields[20];
        /*galtisIpv6Addr(&patternData.ruleIngrExtendedIpv6L4.sip, (GT_U8*)inFields[21]);*/
        if(inFields[21])
        {
            galtisBArray(&patternBArr,(GT_U8*)inFields[21]);
            cmdOsMemCpy(
                patternData.ruleIngrExtendedIpv6L4.sipBits127_32,
                patternBArr.data, 12);
        }
        else
        {
            cmdOsMemSet(patternData.ruleIngrExtendedIpv6L4.sipBits127_32, 0, 12);
        }

        galtisIpv6Addr(&patternData.ruleIngrExtendedIpv6L4.dip, (GT_U8*)inFields[22]);
        patternData.ruleIngrExtendedIpv6L4.isIpv6Eh = (GT_U8)inFields[23];
        patternData.ruleIngrExtendedIpv6L4.isIpv6HopByHop = (GT_U8)inFields[24];

        if (wrCpssExMxPmPclIngressRuleWrite_patched == GT_FALSE)
        {
             galtisBArray(&patternBArr,(GT_U8*)inFields[25]);
             cmdOsMemCpy(
                patternData.ruleIngrExtendedIpv6L4.udb, patternBArr.data,
                3);
        }
        else
        {
            /* error in GUI - 32-bit value instead of hex-array */
            /* used as UDB{15:12}                               */
            patternData.ruleIngrExtendedIpv6L4.udb[12] =
                (GT_U8)(((GT_U32)inFields[25] >> 24) & 0xFF);
            patternData.ruleIngrExtendedIpv6L4.udb[13] =
                (GT_U8)(((GT_U32)inFields[25] >> 16) & 0xFF);
            patternData.ruleIngrExtendedIpv6L4.udb[14] =
                (GT_U8)(((GT_U32)inFields[25] >> 8) & 0xFF);
            patternData.ruleIngrExtendedIpv6L4.udb[15] =
                (GT_U8)((GT_U32)inFields[25] & 0xFF);
        }

        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}


/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressRule_EXT_IPV6_L4_WriteNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_BYTE_ARRY                       maskBArr, patternBArr;


    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L4_E;

    /* map input arguments to locals */
    /* inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1 */

    if(inFields[1] == 0)  /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.pclId = (GT_U16)inFields[2];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.srcPort = (GT_U8)inFields[3];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isTagged = (GT_U8)inFields[4];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.vid = (GT_U16)inFields[5];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.up = (GT_U8)inFields[6];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.isIp = (GT_U8)inFields[7];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isL2Valid = (GT_U8)inFields[8];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isUdbValid = (GT_U8)inFields[9];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.macToMe = (GT_U8)inFields[10];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.isIpv6 = (GT_U8)inFields[11];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExt.ipProtocol = (GT_U8)inFields[12];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.dscp = (GT_U8)inFields[13];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isL4Valid = (GT_U8)inFields[14];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte0 = (GT_U8)inFields[15];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte1 = (GT_U8)inFields[16];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte2 = (GT_U8)inFields[17];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte3 = (GT_U8)inFields[18];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte13 = (GT_U8)inFields[19];
        maskData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.ipHeaderOk = (GT_U8)inFields[20];
        /*galtisIpv6Addr(&maskData.ruleIngrExtendedIpv6L4.sip, (GT_U8*)inFields[21]);*/
        if(inFields[21])
        {
            galtisBArray(&maskBArr,(GT_U8*)inFields[21]);
            cmdOsMemCpy(
                maskData.ruleIngrExtendedIpv6L4.sipBits127_32,
                maskBArr.data, 12);
        }
        else
        {
            cmdOsMemSet(maskData.ruleIngrExtendedIpv6L4.sipBits127_32, 0, 12);
        }
        galtisIpv6Addr(&maskData.ruleIngrExtendedIpv6L4.dip, (GT_U8*)inFields[22]);
        maskData.ruleIngrExtendedIpv6L4.isIpv6Eh = (GT_U8)inFields[23];
        maskData.ruleIngrExtendedIpv6L4.isIpv6HopByHop = (GT_U8)inFields[24];

        if (wrCpssExMxPmPclIngressRuleWrite_patched == GT_FALSE)
        {
             galtisBArray(&maskBArr,(GT_U8*)inFields[25]);
             cmdOsMemCpy(
                maskData.ruleIngrExtendedIpv6L4.udb, maskBArr.data,
                3);
        }
        else
        {
            /* error in GUI - 32-bit value instead of hex-array */
            /* used as UDB{15:12}                               */
            maskData.ruleIngrExtendedIpv6L4.udb[12] =
                (GT_U8)(((GT_U32)inFields[25] >> 24) & 0xFF);
            maskData.ruleIngrExtendedIpv6L4.udb[13] =
                (GT_U8)(((GT_U32)inFields[25] >> 16) & 0xFF);
            maskData.ruleIngrExtendedIpv6L4.udb[14] =
                (GT_U8)(((GT_U32)inFields[25] >> 8) & 0xFF);
            maskData.ruleIngrExtendedIpv6L4.udb[15] =
                (GT_U8)((GT_U32)inFields[25] & 0xFF);
        }

        mask_set = GT_TRUE;
    }
    else  /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.pclId = (GT_U16)inFields[2];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.srcPort = (GT_U8)inFields[3];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isTagged = (GT_U8)inFields[4];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.vid = (GT_U16)inFields[5];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.up = (GT_U8)inFields[6];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.isIp = (GT_U8)inFields[7];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isL2Valid = (GT_U8)inFields[8];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isUdbValid = (GT_U8)inFields[9];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.macToMe = (GT_U8)inFields[10];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.isIpv6 = (GT_U8)inFields[11];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExt.ipProtocol = (GT_U8)inFields[12];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.dscp = (GT_U8)inFields[13];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.isL4Valid = (GT_U8)inFields[14];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte0 = (GT_U8)inFields[15];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte1 = (GT_U8)inFields[16];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte2 = (GT_U8)inFields[17];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte3 = (GT_U8)inFields[18];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.l4Byte13 = (GT_U8)inFields[19];
        patternData.ruleIngrExtendedIpv6L4.commonIngrExtPkt.ipHeaderOk = (GT_U8)inFields[20];
        /*galtisIpv6Addr(&patternData.ruleIngrExtendedIpv6L4.sip, (GT_U8*)inFields[21]);*/
        if(inFields[21])
        {
            galtisBArray(&patternBArr,(GT_U8*)inFields[21]);
            cmdOsMemCpy(
                patternData.ruleIngrExtendedIpv6L4.sipBits127_32,
                patternBArr.data, 12);
        }
        else
        {
            cmdOsMemSet(patternData.ruleIngrExtendedIpv6L4.sipBits127_32, 0, 12);
        }
        galtisIpv6Addr(&patternData.ruleIngrExtendedIpv6L4.dip, (GT_U8*)inFields[22]);
        patternData.ruleIngrExtendedIpv6L4.isIpv6Eh = (GT_U8)inFields[23];
        patternData.ruleIngrExtendedIpv6L4.isIpv6HopByHop = (GT_U8)inFields[24];

        if (wrCpssExMxPmPclIngressRuleWrite_patched == GT_FALSE)
        {
             galtisBArray(&patternBArr,(GT_U8*)inFields[25]);
             cmdOsMemCpy(
                patternData.ruleIngrExtendedIpv6L4.udb, patternBArr.data,
                3);
        }
        else
        {
            /* error in GUI - 32-bit value instead of hex-array */
            /* used as UDB{15:12}                               */
            patternData.ruleIngrExtendedIpv6L4.udb[12] =
                (GT_U8)(((GT_U32)inFields[25] >> 24) & 0xFF);
            patternData.ruleIngrExtendedIpv6L4.udb[13] =
                (GT_U8)(((GT_U32)inFields[25] >> 16) & 0xFF);
            patternData.ruleIngrExtendedIpv6L4.udb[14] =
                (GT_U8)(((GT_U32)inFields[25] >> 8) & 0xFF);
            patternData.ruleIngrExtendedIpv6L4.udb[15] =
                (GT_U8)((GT_U32)inFields[25] & 0xFF);
        }

        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}

/*********************************************************************************/
static CMD_STATUS  wrCpssExMxPmPclIngressRuleWriteFirst
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

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));

    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;

    switch (inArgs[1])
    {
        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E:
            wrCpssExMxPmPclIngressRule_STD_NOT_IP_WriteFirst(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E:
            wrCpssExMxPmPclIngressRule_STD_IP_L2_QOS_WriteFirst(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E:
            wrCpssExMxPmPclIngressRule_STD_IPV4_L4_WriteFirst(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_NOT_IPV6_E:
            wrCpssExMxPmPclIngressRule_EXT_NOT_IPV6_WriteFirst(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L2_E:
            wrCpssExMxPmPclIngressRule_EXT_IPV6_L2_WriteFirst(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L4_E:
            wrCpssExMxPmPclIngressRule_EXT_IPV6_L4_WriteFirst(
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

static CMD_STATUS  wrCpssExMxPmPclIngressRuleWriteFirst_patched
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CMD_STATUS cmdStatus;

    wrCpssExMxPmPclIngressRuleWrite_patched = GT_TRUE;

    cmdStatus = wrCpssExMxPmPclIngressRuleWriteFirst(
        inArgs, inFields, numFields, outArgs);

    wrCpssExMxPmPclIngressRuleWrite_patched = GT_FALSE;

    return cmdStatus;
}

/*************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressRuleWriteNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result = GT_OK;
    GT_U8           devNum;
    CPSS_EXMXPM_PCL_ACTION_UNT         *actionPtr;
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

    if ((mask_set == GT_FALSE) && (pattern_set == GT_FALSE))
    {
        cmdOsMemSet(&maskData , 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));
        cmdOsMemSet(&patternData, 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    switch (inArgs[1])
    {
       case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E:
            wrCpssExMxPmPclIngressRule_STD_NOT_IP_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;
        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E:
            wrCpssExMxPmPclIngressRule_STD_IP_L2_QOS_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;
        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E:
            wrCpssExMxPmPclIngressRule_STD_IPV4_L4_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;
        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_NOT_IPV6_E:
            wrCpssExMxPmPclIngressRule_EXT_NOT_IPV6_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;
        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L2_E:
            wrCpssExMxPmPclIngressRule_EXT_IPV6_L2_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;
        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L4_E:
            wrCpssExMxPmPclIngressRule_EXT_IPV6_L4_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;
        default:
            /* pack output arguments to galtis string */
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
    wrCpssExMxPmPclActionIndexGet(devNum, ruleIndex, &actionIndex);

    if (actionIndex >= INVALID_RULE_INDEX)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Action table entry is not set.\n");
        return CMD_AGENT_ERROR;

    }

    if ((mask_set != GT_FALSE) && (pattern_set != GT_FALSE))
    {

        CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT  ruleInfo;
        CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType;
        WR_PCL_ACTION_STC*                     actionTablePtr;
        GT_U32                                 index;

        if (useExternalTcam == GT_FALSE)
        {
            tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
            ruleInfo.internalTcamRuleStartIndex=mask_ruleIndex;
            actionTablePtr = actionIntTable;
        }
        else
        {   /*bind extTcam params*/
            tcamType = CPSS_EXMXPM_TCAM_TYPE_EXTERNAL_E;
            wrCpssExMxPmPclExtConfigIndexGet(devNum, mask_ruleIndex, &index);
            if (index == INVALID_RULE_INDEX)
            {
                galtisOutput(outArgs, GT_FAIL, "%d", -1);
                return CMD_OK;
            }

            ruleInfo.externalTcamInfo.ruleInfo.tcamId=extTcamConfigPclTable[index].tcamId;
            ruleInfo.externalTcamInfo.ruleInfo.ruleStartIndex=extTcamConfigPclTable[index].ruleStartIndex;
            ruleInfo.externalTcamInfo.ruleInfo.ruleSize=extTcamConfigPclTable[index].ruleSize;
            ruleInfo.externalTcamInfo.ruleInfo.ageEnable=extTcamConfigPclTable[index].ageEnable;

            ruleInfo.externalTcamInfo.actionInfo.actionStartIndex=extTcamConfigPclTable[index].actionStartIndex;
            ruleInfo.externalTcamInfo.actionInfo.memoryType=extTcamConfigPclTable[index].memoryType;
            actionTablePtr = actionExtTable;
        }



        actionPtr = &(actionTablePtr[actionIndex].actionEntry);
        /* call cpss api function */
        result = cpssExMxPmPclRuleSet(
            devNum,tcamType,&ruleInfo,
            ruleFormat, actionTablePtr[actionIndex].actionFormat, GT_TRUE /*valid*/,
            &maskData, &patternData, actionPtr);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        if(result!=GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

static CMD_STATUS  wrCpssExMxPmPclIngressRuleWriteNext_patched
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CMD_STATUS cmdStatus;

    wrCpssExMxPmPclIngressRuleWrite_patched = GT_TRUE;

    cmdStatus = wrCpssExMxPmPclIngressRuleWriteNext(
        inArgs, inFields, numFields, outArgs);

    wrCpssExMxPmPclIngressRuleWrite_patched = GT_FALSE;

    return cmdStatus;
}

/*************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressRuleWriteEnd

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


/* --- cpssExMxPmPclRuleEgress Table--- */

/*******************************************************************************
* cpssExMxPmPclEgressRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule.
*
*       ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case
*                          With reference to IngrIngrStandard and Extended rules
*                          indexes, the partitioning is as follows:
*                          - IngrIngrStandard rules.
*                            Rule index may be in the range from 0 up to 1023.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 511.
*                          Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n <= 511
*                            is placed in the space of two standard rules with
*                            indexes n and n + 512.
*
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*                          The format of mask is defined by ruleFormat
*
*       patternPtr       - rule pattern.
*                          The format of pattern is defined by ruleFormat
*
*       actionPtr        - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_BAD_PTR     - on NULL pointer value
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclEgressRule_STD_NOT_IP_WriteNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;

    /* map input arguments to locals */
    /* inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1 */

    if(inFields[1] == 0)  /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleEgrStandardNotIp.commonEgr.pclId = (GT_U16)inFields[2];
        maskData.ruleEgrStandardNotIp.commonEgr.srcPort = (GT_U8)inFields[3];
        maskData.ruleEgrStandardNotIp.commonEgr.isTagged = (GT_U8)inFields[4];
        maskData.ruleEgrStandardNotIp.commonEgr.up = (GT_U8)inFields[5];
        maskData.ruleEgrStandardNotIp.commonEgr.vid = (GT_U16)inFields[6];
        maskData.ruleEgrStandardNotIp.commonEgr.isIp = (GT_U8)inFields[7];
        maskData.ruleEgrStandardNotIp.commonEgrStd.isL2Valid = (GT_U8)inFields[8];

        maskData.ruleEgrStandardNotIp.ethType = (GT_U16)inFields[9];
        maskData.ruleEgrStandardNotIp.commonEgrStd.isArp = (GT_U8)inFields[10];
        maskData.ruleEgrStandardNotIp.commonEgrStd.isIpv4 = (GT_U8)inFields[11];
        maskData.ruleEgrStandardNotIp.l2EncapType = (GT_U8)inFields[12];
        galtisMacAddr(&maskData.ruleEgrStandardNotIp.macDa, (GT_U8*)inFields[13]);
        galtisMacAddr(&maskData.ruleEgrStandardNotIp.macSa, (GT_U8*)inFields[14]);

        maskData.ruleEgrStandardNotIp.commonEgrStd.packetCmd = (GT_U8)inFields[15];
        maskData.ruleEgrStandardNotIp.commonEgr.isMpls = (GT_U8)inFields[16];
        maskData.ruleEgrStandardNotIp.commonEgrStd.isIpOrMplsRouted = (GT_U8)inFields[17];
        /* errata isUnknown changed to isKnown (mask - as is) */
        maskData.ruleEgrStandardNotIp.commonEgrStd.egrFilterRegOrCpuCode7 = (GT_U8)inFields[18];

        /* handle multiplexed fields.
          Compose these fields as OR of all possible logical fields.
          The responsibility of RDE test is fill in not relevant fields as ZEROs.
         */

         /*
         * trgOrCpuCode0_6OrCosOrSniff - case(Key<PacketCmd>):
         *                    Forward: {SrcTrunkID[7],TrgPort}
         *                    ToCPU: CPU_CODE[6:0]
         *                    FromCPU: {1'b0, EgressFilterEn, TC[2:0], DP[1:0]}
         *                    ToAnalyzer:  {6'b0, RxSniff}
         */
        maskData.ruleEgrStandardNotIp.commonEgrStd.trgOrCpuCode0_6OrCosOrSniff = (GT_U8) (
            /* Forward packet command */
                /* trgPort is absent */
              ((inFields[21] >> 7) & 1) |  /* srcTrunk ID bit 7 */
            /* To CPU */
              (inFields[22] & 0x7f)     |  /* CPU Code 0:6 */
            /*  From CPU */
              (inFields[26] & 3)        |   /* DP */
              ((inFields[25] & 7) << 2) |   /* TC */
              ((inFields[27] & 1) << 5) |   /* egrFilterEnable*/
            /*  To analizer */
              ((inFields[29]) & 1));           /* RxSniff */

        /*
         * srcTrgOrTxMirror - if ToCPU: this bits indicate SrcTrg.
         *                    Else:
         *                    If Conf<EnableEgressMirrorAsForward> == 0:
         *                    Reserved
         *                    If Conf<EnableEgressMirrorAsForward> == 1:
         *                    From CPU: 0
         *                    Else this bit is used to identify EgressMirrored
        */
        maskData.ruleEgrStandardNotIp.commonEgrStd.srcTrgOrTxMirror =
            /* TO CPU */
            (GT_U8)(inFields[23]);  /* srcTrg */
            /* NOT TO CPU - txMirror absent */

        /*
         * srcInfo          - case(Key<PacketCmd>):
         *                    Forward and SrcIsTrunk is 1: SrcTrunkID[6:0]
         *                    Forward and  SrcIsTrunk is 0: SrcDev[6:0]
         *                    ToCPU: SrcDev[6:0]
         *                    FromCPU: SrcDev[6:0]
         *                    ToAnalyzer: SrcDev[6:0]
        */
         maskData.ruleEgrStandardNotIp.commonEgrStd.srcInfo = (GT_U8)(
             /* Forward */
             (inFields[21] & 0x7F)      | /* SrcTrunkID[6:0]*/
             (inFields[20] & 0x7F)      | /* SrcDev[6:0] */
             /* TO CPU */
             (inFields[24] & 0x7F)      | /* SrcDev[6:0] */
             /* FROM CPU */
             (inFields[28] & 0x7F)      | /* SrcDev[6:0] */
             /* TO Analyzer */
             (inFields[30] & 0x7F));       /* SrcDev[6:0] */

   /*   maskData.ruleEgrStandardNotIp.egress.isVidx = (GT_U8)inFields[19]; */

        maskData.ruleEgrStandardNotIp.commonEgrStd.sourceId = (GT_U8)inFields[31];
        maskData.ruleEgrStandardNotIp.commonEgrStd.srcIsTrunk = (GT_U8)inFields[32];
        mask_set = GT_TRUE;
    }
    else  /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleEgrStandardNotIp.commonEgr.pclId = (GT_U16)inFields[2];
        patternData.ruleEgrStandardNotIp.commonEgr.srcPort = (GT_U8)inFields[3];
        patternData.ruleEgrStandardNotIp.commonEgr.isTagged = (GT_U8)inFields[4];
        patternData.ruleEgrStandardNotIp.commonEgr.up = (GT_U8)inFields[5];
        patternData.ruleEgrStandardNotIp.commonEgr.vid = (GT_U16)inFields[6];
        patternData.ruleEgrStandardNotIp.commonEgr.isIp = (GT_U8)inFields[7];
        patternData.ruleEgrStandardNotIp.commonEgrStd.isL2Valid = (GT_U8)inFields[8];

        patternData.ruleEgrStandardNotIp.ethType = (GT_U16)inFields[9];
        patternData.ruleEgrStandardNotIp.commonEgrStd.isArp = (GT_U8)inFields[10];
        patternData.ruleEgrStandardNotIp.commonEgrStd.isIpv4 = (GT_U8)inFields[11];
        patternData.ruleEgrStandardNotIp.l2EncapType = (GT_U8)inFields[12];
        galtisMacAddr(&patternData.ruleEgrStandardNotIp.macDa, (GT_U8*)inFields[13]);
        galtisMacAddr(&patternData.ruleEgrStandardNotIp.macSa, (GT_U8*)inFields[14]);

        patternData.ruleEgrStandardNotIp.commonEgrStd.packetCmd = (GT_U8)inFields[15];
        patternData.ruleEgrStandardNotIp.commonEgr.isMpls = (GT_U8)inFields[16];
        patternData.ruleEgrStandardNotIp.commonEgrStd.isIpOrMplsRouted = (GT_U8)inFields[17];
        /* errata isUnknown changed to isKnown */
        patternData.ruleEgrStandardNotIp.commonEgrStd.egrFilterRegOrCpuCode7 = (~ (GT_U8)inFields[18]) & 1;
   /*   patternData.ruleEgrStandardNotIp.egress.isVidx = (GT_U8)inFields[19]; */

        /* handle multiplexed fields.
          Compose these fields as OR of all possible logical fields.
          The responsibility of RDE test is fill in not relevant fields as ZEROs.
         */

         /*
         * trgOrCpuCode0_6OrCosOrSniff - case(Key<PacketCmd>):
         *                    Forward: {SrcTrunkID[7],TrgPort}
         *                    ToCPU: CPU_CODE[6:0]
         *                    FromCPU: {1'b0, EgressFilterEn, TC[2:0], DP[1:0]}
         *                    ToAnalyzer:  {6'b0, RxSniff}
         */
        patternData.ruleEgrStandardNotIp.commonEgrStd.trgOrCpuCode0_6OrCosOrSniff = (GT_U8) (
            /* Forward packet command */
                /* trgPort is absent */
              ((inFields[21] >> 7) & 1) |  /* srcTrunk ID bit 7 */
            /* To CPU */
              (inFields[22] & 0x7f)     |  /* CPU Code 0:6 */
            /*  From CPU */
              (inFields[26] & 3)        |   /* DP */
              ((inFields[25] & 7) << 2) |   /* TC */
              ((inFields[27] & 1) << 5) |   /* egrFilterEnable*/
            /*  To analizer */
              ((inFields[29]) & 1));           /* RxSniff */

        /*
         * srcTrgOrTxMirror - if ToCPU: this bits indicate SrcTrg.
         *                    Else:
         *                    If Conf<EnableEgressMirrorAsForward> == 0:
         *                    Reserved
         *                    If Conf<EnableEgressMirrorAsForward> == 1:
         *                    From CPU: 0
         *                    Else this bit is used to identify EgressMirrored
        */
        patternData.ruleEgrStandardNotIp.commonEgrStd.srcTrgOrTxMirror =
            /* TO CPU */
            (GT_U8)(inFields[23]);  /* srcTrg */
            /* NOT TO CPU - txMirror absent */

        /*
         * srcInfo          - case(Key<PacketCmd>):
         *                    Forward and SrcIsTrunk is 1: SrcTrunkID[6:0]
         *                    Forward and  SrcIsTrunk is 0: SrcDev[6:0]
         *                    ToCPU: SrcDev[6:0]
         *                    FromCPU: SrcDev[6:0]
         *                    ToAnalyzer: SrcDev[6:0]
        */
         patternData.ruleEgrStandardNotIp.commonEgrStd.srcInfo = (GT_U8)(
             /* Forward */
             (inFields[21] & 0x7F)      | /* SrcTrunkID[6:0]*/
             (inFields[20] & 0x7F)      | /* SrcDev[6:0] */
             /* TO CPU */
             (inFields[24] & 0x7F)      | /* SrcDev[6:0] */
             /* FROM CPU */
             (inFields[28] & 0x7F)      | /* SrcDev[6:0] */
             /* TO Analyzer */
             (inFields[30] & 0x7F));       /* SrcDev[6:0] */

        patternData.ruleEgrStandardNotIp.commonEgrStd.sourceId = (GT_U8)inFields[31];
        patternData.ruleEgrStandardNotIp.commonEgrStd.srcIsTrunk = (GT_U8)inFields[32];
        pattern_set = GT_TRUE;
    }

    if ((mask_set == GT_TRUE) && (pattern_set == GT_TRUE))
    {
        /* due to errata the pattern.isKnown = (~ pattern.isUnknown) */
        /* for don't care case must be set to 0                       */
        patternData.ruleEgrStandardNotIp.commonEgrStd.egrFilterRegOrCpuCode7 &=
            maskData.ruleEgrStandardNotIp.commonEgrStd.egrFilterRegOrCpuCode7;
    }

    return CMD_OK;
}

/********************************************************************/
static CMD_STATUS wrCpssExMxPmPclEgressRule_STD_IP_L2_QOS_WriteNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY                       maskBArr, patternBArr;

    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IP_L2_QOS_E;

    /* map input arguments to locals */
    /* inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1 */

    if(inFields[1] == 0)  /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleEgrStandardIpL2Qos.commonEgr.pclId = (GT_U16)inFields[2];
        maskData.ruleEgrStandardIpL2Qos.commonEgr.srcPort = (GT_U8)inFields[3];
        maskData.ruleEgrStandardIpL2Qos.commonEgr.isTagged = (GT_U8)inFields[4];
        maskData.ruleEgrStandardIpL2Qos.commonEgr.up = (GT_U8)inFields[5];
        maskData.ruleEgrStandardIpL2Qos.commonEgr.vid = (GT_U16)inFields[6];
        maskData.ruleEgrStandardIpL2Qos.commonEgr.isIp = (GT_U8)inFields[7];
        maskData.ruleEgrStandardIpL2Qos.commonEgrStd.isL2Valid = (GT_U8)inFields[8];

        maskData.ruleEgrStandardIpL2Qos.commonEgrStdIp.ipProtocol = (GT_U8)inFields[9];
        maskData.ruleEgrStandardIpL2Qos.commonEgrStd.isArp = (GT_U8)inFields[10];
        maskData.ruleEgrStandardIpL2Qos.commonEgrStd.isIpv4 = (GT_U8)inFields[11];
        maskData.ruleEgrStandardIpL2Qos.commonEgrStdIp.dscp = (GT_U8)inFields[12];
        maskData.ruleEgrStandardIpL2Qos.commonEgrStdIp.isIpv4Fragment = (GT_U8)inFields[13];
        maskData.ruleEgrStandardIpL2Qos.commonEgrStdIp.isL4Valid = (GT_U8)inFields[14];
        maskData.ruleEgrStandardIpL2Qos.commonEgrStdIp.l4Byte2 = (GT_U8)inFields[15];
        maskData.ruleEgrStandardIpL2Qos.commonEgrStdIp.l4Byte3 = (GT_U8)inFields[16];
        maskData.ruleEgrStandardIpL2Qos.commonEgrStdIp.l4Byte13 = (GT_U8)inFields[17];

        maskData.ruleEgrStandardIpL2Qos.commonEgrStd.packetCmd = (GT_U8)inFields[18];
        maskData.ruleEgrStandardIpL2Qos.commonEgr.isMpls = (GT_U8)inFields[19];
        maskData.ruleEgrStandardIpL2Qos.commonEgrStd.isIpOrMplsRouted = (GT_U8)inFields[20];
        /* errata isUnknown changed to isKnown (mask - as is) */
        maskData.ruleEgrStandardIpL2Qos.commonEgrStd.egrFilterRegOrCpuCode7 = (GT_U8)inFields[21];
  /*    maskData.ruleEgrStandardIpL2Qos.egress.isVidx = (GT_U8)inFields[22];   */

        /* handle multiplexed fields.
          Compose these fields as OR of all possible logical fields.
          The responsibility of RDE test is fill in not relevant fields as ZEROs.
         */

         /*
         * trgOrCpuCode0_6OrCosOrSniff - case(Key<PacketCmd>):
         *                    Forward: {SrcTrunkID[7],TrgPort}
         *                    ToCPU: CPU_CODE[6:0]
         *                    FromCPU: {1'b0, EgressFilterEn, TC[2:0], DP[1:0]}
         *                    ToAnalyzer:  {6'b0, RxSniff}
         */
        maskData.ruleEgrStandardIpL2Qos.commonEgrStd.trgOrCpuCode0_6OrCosOrSniff = (GT_U8) (
            /* Forward packet command */
                /* trgPort is absent */
              ((inFields[24] >> 7) & 1) |  /* srcTrunk ID bit 7 */
            /* To CPU */
              (inFields[25] & 0x7f)     |  /* CPU Code 0:6 */
            /*  From CPU */
              (inFields[29] & 3)        |   /* DP */
              ((inFields[28] & 7) << 2) |   /* TC */
              ((inFields[30] & 1) << 5) |   /* egrFilterEnable*/
            /*  To analizer */
              ((inFields[32]) & 1));           /* RxSniff */

        /*
         * srcTrgOrTxMirror - if ToCPU: this bits indicate SrcTrg.
         *                    Else:
         *                    If Conf<EnableEgressMirrorAsForward> == 0:
         *                    Reserved
         *                    If Conf<EnableEgressMirrorAsForward> == 1:
         *                    From CPU: 0
         *                    Else this bit is used to identify EgressMirrored
        */
        maskData.ruleEgrStandardIpL2Qos.commonEgrStd.srcTrgOrTxMirror =
            /* TO CPU */
            (GT_U8)(inFields[26]);  /* srcTrg */
            /* NOT TO CPU - txMirror absent */

        /*
         * srcInfo          - case(Key<PacketCmd>):
         *                    Forward and SrcIsTrunk is 1: SrcTrunkID[6:0]
         *                    Forward and  SrcIsTrunk is 0: SrcDev[6:0]
         *                    ToCPU: SrcDev[6:0]
         *                    FromCPU: SrcDev[6:0]
         *                    ToAnalyzer: SrcDev[6:0]
        */
         maskData.ruleEgrStandardIpL2Qos.commonEgrStd.srcInfo = (GT_U8)(
             /* Forward */
             (inFields[24] & 0x7F)      | /* SrcTrunkID[6:0]*/
             (inFields[23] & 0x7F)      | /* SrcDev[6:0] */
             /* TO CPU */
             (inFields[27] & 0x7F)      | /* SrcDev[6:0] */
             /* FROM CPU */
             (inFields[31] & 0x7F)      | /* SrcDev[6:0] */
             /* TO Analyzer */
             (inFields[33] & 0x7F));       /* SrcDev[6:0] */

        maskData.ruleEgrStandardIpL2Qos.commonEgrStd.sourceId = (GT_U8)inFields[34];
        maskData.ruleEgrStandardIpL2Qos.commonEgrStd.srcIsTrunk = (GT_U8)inFields[35];

        galtisBArray(&maskBArr,(GT_U8*)inFields[36]);

        cmdOsMemCpy(
            maskData.ruleEgrStandardIpL2Qos.commonEgrStdIp.dipBits31_0, maskBArr.data,
            maskBArr.length);

   /*     maskData.ruleEgrStandardIpL2Qos.isIpv6Eh = (GT_U8)inFields[37];
        maskData.ruleEgrStandardIpL2Qos.isIpv6HopByHop = (GT_U8)inFields[38];
        maskData.ruleEgrStandardIpL2Qos.tcpUdpComparator = (GT_U8)inFields[39]; */
         galtisMacAddr(&maskData.ruleEgrStandardIpL2Qos.macDa, (GT_U8*)inFields[40]);
        mask_set = GT_TRUE;
    }
    else  /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleEgrStandardIpL2Qos.commonEgr.pclId = (GT_U16)inFields[2];
        patternData.ruleEgrStandardIpL2Qos.commonEgr.srcPort = (GT_U8)inFields[3];
        patternData.ruleEgrStandardIpL2Qos.commonEgr.isTagged = (GT_U8)inFields[4];
        patternData.ruleEgrStandardIpL2Qos.commonEgr.up = (GT_U8)inFields[5];
        patternData.ruleEgrStandardIpL2Qos.commonEgr.vid = (GT_U16)inFields[6];
        patternData.ruleEgrStandardIpL2Qos.commonEgr.isIp = (GT_U8)inFields[7];
        patternData.ruleEgrStandardIpL2Qos.commonEgrStd.isL2Valid = (GT_U8)inFields[8];

        patternData.ruleEgrStandardIpL2Qos.commonEgrStdIp.ipProtocol = (GT_U8)inFields[9];
        patternData.ruleEgrStandardIpL2Qos.commonEgrStd.isArp = (GT_U8)inFields[10];
        patternData.ruleEgrStandardIpL2Qos.commonEgrStd.isIpv4 = (GT_U8)inFields[11];
        patternData.ruleEgrStandardIpL2Qos.commonEgrStdIp.dscp = (GT_U8)inFields[12];
        patternData.ruleEgrStandardIpL2Qos.commonEgrStdIp.isIpv4Fragment = (GT_U8)inFields[13];
        patternData.ruleEgrStandardIpL2Qos.commonEgrStdIp.isL4Valid = (GT_U8)inFields[14];
        patternData.ruleEgrStandardIpL2Qos.commonEgrStdIp.l4Byte2 = (GT_U8)inFields[15];
        patternData.ruleEgrStandardIpL2Qos.commonEgrStdIp.l4Byte3 = (GT_U8)inFields[16];
        patternData.ruleEgrStandardIpL2Qos.commonEgrStdIp.l4Byte13 = (GT_U8)inFields[17];

        patternData.ruleEgrStandardIpL2Qos.commonEgrStd.packetCmd = (GT_U8)inFields[18];
        patternData.ruleEgrStandardIpL2Qos.commonEgr.isMpls = (GT_U8)inFields[19];
        patternData.ruleEgrStandardIpL2Qos.commonEgrStd.isIpOrMplsRouted = (GT_U8)inFields[20];
        /* errata isUnknown changed to isKnown*/
        patternData.ruleEgrStandardIpL2Qos.commonEgrStd.egrFilterRegOrCpuCode7 = (~ (GT_U8)inFields[21]) & 1;
    /*  patternData.ruleEgrStandardIpL2Qos.egress.isVidx = (GT_U8)inFields[22];    */

        /* handle multiplexed fields.
          Compose these fields as OR of all possible logical fields.
          The responsibility of RDE test is fill in not relevant fields as ZEROs.
         */

         /*
         * trgOrCpuCode0_6OrCosOrSniff - case(Key<PacketCmd>):
         *                    Forward: {SrcTrunkID[7],TrgPort}
         *                    ToCPU: CPU_CODE[6:0]
         *                    FromCPU: {1'b0, EgressFilterEn, TC[2:0], DP[1:0]}
         *                    ToAnalyzer:  {6'b0, RxSniff}
         */
        patternData.ruleEgrStandardIpL2Qos.commonEgrStd.trgOrCpuCode0_6OrCosOrSniff = (GT_U8) (
            /* Forward packet command */
                /* trgPort is absent */
              ((inFields[24] >> 7) & 1) |  /* srcTrunk ID bit 7 */
            /* To CPU */
              (inFields[25] & 0x7f)     |  /* CPU Code 0:6 */
            /*  From CPU */
              (inFields[29] & 3)        |   /* DP */
              ((inFields[28] & 7) << 2) |   /* TC */
              ((inFields[30] & 1) << 5) |   /* egrFilterEnable*/
            /*  To analizer */
              ((inFields[32]) & 1));           /* RxSniff */

        /*
         * srcTrgOrTxMirror - if ToCPU: this bits indicate SrcTrg.
         *                    Else:
         *                    If Conf<EnableEgressMirrorAsForward> == 0:
         *                    Reserved
         *                    If Conf<EnableEgressMirrorAsForward> == 1:
         *                    From CPU: 0
         *                    Else this bit is used to identify EgressMirrored
        */
        patternData.ruleEgrStandardIpL2Qos.commonEgrStd.srcTrgOrTxMirror =
            /* TO CPU */
            (GT_U8)(inFields[26]);  /* srcTrg */
            /* NOT TO CPU - txMirror absent */

        /*
         * srcInfo          - case(Key<PacketCmd>):
         *                    Forward and SrcIsTrunk is 1: SrcTrunkID[6:0]
         *                    Forward and  SrcIsTrunk is 0: SrcDev[6:0]
         *                    ToCPU: SrcDev[6:0]
         *                    FromCPU: SrcDev[6:0]
         *                    ToAnalyzer: SrcDev[6:0]
        */
         patternData.ruleEgrStandardIpL2Qos.commonEgrStd.srcInfo = (GT_U8)(
             /* Forward */
             (inFields[24] & 0x7F)      | /* SrcTrunkID[6:0]*/
             (inFields[23] & 0x7F)      | /* SrcDev[6:0] */
             /* TO CPU */
             (inFields[27] & 0x7F)      | /* SrcDev[6:0] */
             /* FROM CPU */
             (inFields[31] & 0x7F)      | /* SrcDev[6:0] */
             /* TO Analyzer */
             (inFields[33] & 0x7F));       /* SrcDev[6:0] */

      /*  patternData.ruleEgrStandardIpL2Qos.egress.pktTypeDep.fwdFromPort.srcDevId = (GT_U8)inFields[23];
        patternData.ruleEgrStandardIpL2Qos.egress.pktTypeDep.fwdFromTrunk.srcTrunkId = (GT_U8)inFields[24];
        patternData.ruleEgrStandardIpL2Qos.egress.pktTypeDep.toCpu.cpuCode = (GT_U8)inFields[25];
        patternData.ruleEgrStandardIpL2Qos.egress.pktTypeDep.toCpu.srcTrg = (GT_U8)inFields[26];
        patternData.ruleEgrStandardIpL2Qos.egress.pktTypeDep.toCpu.srcDevId = (GT_U8)inFields[27];
        patternData.ruleEgrStandardIpL2Qos.egress.pktTypeDep.fromCpu.tc = (GT_U8)inFields[28];
        patternData.ruleEgrStandardIpL2Qos.egress.pktTypeDep.fromCpu.dp = (GT_U8)inFields[29];
        patternData.ruleEgrStandardIpL2Qos.egress.pktTypeDep.fromCpu.egrFilterEn = (GT_U8)inFields[30];
        patternData.ruleEgrStandardIpL2Qos.egress.pktTypeDep.fromCpu.srcDevId = (GT_U8)inFields[31];
        patternData.ruleEgrStandardIpL2Qos.egress.pktTypeDep.toAnalyzer.rxSniffer = (GT_U8)inFields[32];
        patternData.ruleEgrStandardIpL2Qos.egress.pktTypeDep.toAnalyzer.srcDevId = (GT_U8)inFields[33]; */

        patternData.ruleEgrStandardIpL2Qos.commonEgrStd.sourceId = (GT_U8)inFields[34];
        patternData.ruleEgrStandardIpL2Qos.commonEgrStd.srcIsTrunk = (GT_U8)inFields[35];

        galtisBArray(&patternBArr,(GT_U8*)inFields[36]);

        cmdOsMemCpy(
            patternData.ruleEgrStandardIpL2Qos.commonEgrStdIp.dipBits31_0, patternBArr.data,
            patternBArr.length);

   /*     patternData.ruleEgrStandardIpL2Qos.isIpv6Eh = (GT_U8)inFields[37];
        patternData.ruleEgrStandardIpL2Qos.isIpv6HopByHop = (GT_U8)inFields[38];
        patternData.ruleEgrStandardIpL2Qos.tcpUdpComparator = (GT_U8)inFields[39]; */
        galtisMacAddr(&patternData.ruleEgrStandardIpL2Qos.macDa, (GT_U8*)inFields[40]);
        pattern_set = GT_TRUE;
    }

    if ((mask_set == GT_TRUE) && (pattern_set == GT_TRUE))
    {
        /* due to errata the pattern.isKnown = (~ pattern.isUnknown) */
        /* for don't care case must be set to 0                       */
        patternData.ruleEgrStandardIpL2Qos.commonEgrStd.egrFilterRegOrCpuCode7 &=
            maskData.ruleEgrStandardIpL2Qos.commonEgrStd.egrFilterRegOrCpuCode7;
    }

    return CMD_OK;
}


/********************************************************************/
static CMD_STATUS wrCpssExMxPmPclEgressRule_STD_IPV4_L4_WriteNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_IPADDR  tmpIpv4Addr;

    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IPV4_L4_E;

    /* map input arguments to locals */
    /* inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1 */

    if(inFields[1] == 0)  /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleEgrStandardIpv4L4.commonEgr.pclId = (GT_U16)inFields[2];
        maskData.ruleEgrStandardIpv4L4.commonEgr.srcPort = (GT_U8)inFields[3];
        maskData.ruleEgrStandardIpv4L4.commonEgr.isTagged = (GT_U8)inFields[4];
        maskData.ruleEgrStandardIpv4L4.commonEgr.up = (GT_U8)inFields[5];
        maskData.ruleEgrStandardIpv4L4.commonEgr.vid = (GT_U16)inFields[6];
        maskData.ruleEgrStandardIpv4L4.commonEgr.isIp = (GT_U8)inFields[7];
        maskData.ruleEgrStandardIpv4L4.commonEgrStd.isL2Valid = (GT_U8)inFields[8];

        maskData.ruleEgrStandardIpv4L4.commonEgrStdIp.ipProtocol = (GT_U8)inFields[9];
        maskData.ruleEgrStandardIpv4L4.commonEgrStd.isArp = (GT_U8)inFields[10];
        maskData.ruleEgrStandardIpv4L4.commonEgrStd.isIpv4 = (GT_U8)inFields[11];
        maskData.ruleEgrStandardIpv4L4.commonEgrStdIp.dscp = (GT_U8)inFields[12];
        maskData.ruleEgrStandardIpv4L4.commonEgrStdIp.isIpv4Fragment = (GT_U8)inFields[13];
        maskData.ruleEgrStandardIpv4L4.commonEgrStdIp.isL4Valid = (GT_U8)inFields[14];
        maskData.ruleEgrStandardIpv4L4.commonEgrStdIp.l4Byte2 = (GT_U8)inFields[15];
        maskData.ruleEgrStandardIpv4L4.commonEgrStdIp.l4Byte3 = (GT_U8)inFields[16];
        maskData.ruleEgrStandardIpv4L4.l4Byte0 = (GT_U8)inFields[17];
        maskData.ruleEgrStandardIpv4L4.l4Byte1 = (GT_U8)inFields[18];
        maskData.ruleEgrStandardIpv4L4.commonEgrStdIp.l4Byte13 = (GT_U8)inFields[19];

        maskData.ruleEgrStandardIpv4L4.commonEgrStd.packetCmd = (GT_U8)inFields[20];
        maskData.ruleEgrStandardIpv4L4.commonEgr.isMpls = (GT_U8)inFields[21];
        maskData.ruleEgrStandardIpv4L4.commonEgrStd.isIpOrMplsRouted = (GT_U8)inFields[22];
        /* errata isUnknown changed to isKnown (mask - as is) */
        maskData.ruleEgrStandardIpv4L4.commonEgrStd.egrFilterRegOrCpuCode7 = (GT_U8)inFields[23];
  /*    maskData.ruleEgrStandardIpv4L4.egress.isVidx = (GT_U8)inFields[24];  */

        /* handle multiplexed fields.
          Compose these fields as OR of all possible logical fields.
          The responsibility of RDE test is fill in not relevant fields as ZEROs.
         */

         /*
         * trgOrCpuCode0_6OrCosOrSniff - case(Key<PacketCmd>):
         *                    Forward: {SrcTrunkID[7],TrgPort}
         *                    ToCPU: CPU_CODE[6:0]
         *                    FromCPU: {1'b0, EgressFilterEn, TC[2:0], DP[1:0]}
         *                    ToAnalyzer:  {6'b0, RxSniff}
         */
        maskData.ruleEgrStandardIpv4L4.commonEgrStd.trgOrCpuCode0_6OrCosOrSniff = (GT_U8) (
            /* Forward packet command */
                /* trgPort is absent */
              ((inFields[26] >> 7) & 1) |  /* srcTrunk ID bit 7 */
            /* To CPU */
              (inFields[27] & 0x7f)     |  /* CPU Code 0:6 */
            /*  From CPU */
              (inFields[31] & 3)        |   /* DP */
              ((inFields[30] & 7) << 2) |   /* TC */
              ((inFields[32] & 1) << 5) |   /* egrFilterEnable*/
            /*  To analizer */
              ((inFields[34]) & 1));           /* RxSniff */

        /*
         * srcTrgOrTxMirror - if ToCPU: this bits indicate SrcTrg.
         *                    Else:
         *                    If Conf<EnableEgressMirrorAsForward> == 0:
         *                    Reserved
         *                    If Conf<EnableEgressMirrorAsForward> == 1:
         *                    From CPU: 0
         *                    Else this bit is used to identify EgressMirrored
        */
        maskData.ruleEgrStandardIpv4L4.commonEgrStd.srcTrgOrTxMirror =
            /* TO CPU */
            (GT_U8)(inFields[28]);  /* srcTrg */
            /* NOT TO CPU - txMirror absent */

        /*
         * srcInfo          - case(Key<PacketCmd>):
         *                    Forward and SrcIsTrunk is 1: SrcTrunkID[6:0]
         *                    Forward and  SrcIsTrunk is 0: SrcDev[6:0]
         *                    ToCPU: SrcDev[6:0]
         *                    FromCPU: SrcDev[6:0]
         *                    ToAnalyzer: SrcDev[6:0]
        */
         maskData.ruleEgrStandardIpv4L4.commonEgrStd.srcInfo = (GT_U8)(
             /* Forward */
             (inFields[26] & 0x7F)      | /* SrcTrunkID[6:0]*/
             (inFields[25] & 0x7F)      | /* SrcDev[6:0] */
             /* TO CPU */
             (inFields[29] & 0x7F)      | /* SrcDev[6:0] */
             /* FROM CPU */
             (inFields[33] & 0x7F)      | /* SrcDev[6:0] */
             /* TO Analyzer */
             (inFields[35] & 0x7F));       /* SrcDev[6:0] */

        maskData.ruleEgrStandardIpv4L4.commonEgrStd.sourceId = (GT_U8)inFields[36];
        maskData.ruleEgrStandardIpv4L4.commonEgrStd.srcIsTrunk = (GT_U8)inFields[37];

        galtisIpAddr(&tmpIpv4Addr, (GT_U8*)inFields[38]);

        cmdOsMemCpy(maskData.ruleEgrStandardIpv4L4.commonEgrStdIp.dipBits31_0,
                 tmpIpv4Addr.arIP,
                 sizeof(maskData.ruleEgrStandardIpv4L4.commonEgrStdIp.dipBits31_0));

        galtisIpAddr(&maskData.ruleEgrStandardIpv4L4.sip, (GT_U8*)inFields[39]);

        maskData.ruleEgrStandardIpv4L4.commonEgrStdIp.tcpUdpPortComparators = (GT_U8)inFields[40];
        maskData.ruleEgrStandardIpv4L4.isBc = (GT_U8)inFields[41];
        mask_set = GT_TRUE;
    }
    else  /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleEgrStandardIpv4L4.commonEgr.pclId = (GT_U16)inFields[2];
        patternData.ruleEgrStandardIpv4L4.commonEgr.srcPort = (GT_U8)inFields[3];
        patternData.ruleEgrStandardIpv4L4.commonEgr.isTagged = (GT_U8)inFields[4];
        patternData.ruleEgrStandardIpv4L4.commonEgr.up = (GT_U8)inFields[5];
        patternData.ruleEgrStandardIpv4L4.commonEgr.vid = (GT_U16)inFields[6];
        patternData.ruleEgrStandardIpv4L4.commonEgr.isIp = (GT_U8)inFields[7];
        patternData.ruleEgrStandardIpv4L4.commonEgrStd.isL2Valid = (GT_U8)inFields[8];

        patternData.ruleEgrStandardIpv4L4.commonEgrStdIp.ipProtocol = (GT_U8)inFields[9];
        patternData.ruleEgrStandardIpv4L4.commonEgrStd.isArp = (GT_U8)inFields[10];
        patternData.ruleEgrStandardIpv4L4.commonEgrStd.isIpv4 = (GT_U8)inFields[11];
        patternData.ruleEgrStandardIpv4L4.commonEgrStdIp.dscp = (GT_U8)inFields[12];
        patternData.ruleEgrStandardIpv4L4.commonEgrStdIp.isIpv4Fragment = (GT_U8)inFields[13];
        patternData.ruleEgrStandardIpv4L4.commonEgrStdIp.isL4Valid = (GT_U8)inFields[14];
        patternData.ruleEgrStandardIpv4L4.commonEgrStdIp.l4Byte2 = (GT_U8)inFields[15];
        patternData.ruleEgrStandardIpv4L4.commonEgrStdIp.l4Byte3 = (GT_U8)inFields[16];
        patternData.ruleEgrStandardIpv4L4.l4Byte0 = (GT_U8)inFields[17];
        patternData.ruleEgrStandardIpv4L4.l4Byte1 = (GT_U8)inFields[18];
        patternData.ruleEgrStandardIpv4L4.commonEgrStdIp.l4Byte13 = (GT_U8)inFields[19];

        patternData.ruleEgrStandardIpv4L4.commonEgrStd.packetCmd = (GT_U8)inFields[20];
        patternData.ruleEgrStandardIpv4L4.commonEgr.isMpls = (GT_U8)inFields[21];
        patternData.ruleEgrStandardIpv4L4.commonEgrStd.isIpOrMplsRouted = (GT_U8)inFields[22];
        /* errata isUnknown changed to isKnown */
        patternData.ruleEgrStandardIpv4L4.commonEgrStd.egrFilterRegOrCpuCode7 = (~ (GT_U8)inFields[23]) & 1;
  /*    patternData.ruleEgrStandardIpv4L4.egress.isVidx = (GT_U8)inFields[24];  */

        /* handle multiplexed fields.
          Compose these fields as OR of all possible logical fields.
          The responsibility of RDE test is fill in not relevant fields as ZEROs.
         */

         /*
         * trgOrCpuCode0_6OrCosOrSniff - case(Key<PacketCmd>):
         *                    Forward: {SrcTrunkID[7],TrgPort}
         *                    ToCPU: CPU_CODE[6:0]
         *                    FromCPU: {1'b0, EgressFilterEn, TC[2:0], DP[1:0]}
         *                    ToAnalyzer:  {6'b0, RxSniff}
         */
        patternData.ruleEgrStandardIpv4L4.commonEgrStd.trgOrCpuCode0_6OrCosOrSniff = (GT_U8) (
            /* Forward packet command */
                /* trgPort is absent */
              ((inFields[26] >> 7) & 1) |  /* srcTrunk ID bit 7 */
            /* To CPU */
              (inFields[27] & 0x7f)     |  /* CPU Code 0:6 */
            /*  From CPU */
              (inFields[31] & 3)        |   /* DP */
              ((inFields[30] & 7) << 2) |   /* TC */
              ((inFields[32] & 1) << 5) |   /* egrFilterEnable*/
            /*  To analizer */
              ((inFields[34]) & 1));           /* RxSniff */

        /*
         * srcTrgOrTxMirror - if ToCPU: this bits indicate SrcTrg.
         *                    Else:
         *                    If Conf<EnableEgressMirrorAsForward> == 0:
         *                    Reserved
         *                    If Conf<EnableEgressMirrorAsForward> == 1:
         *                    From CPU: 0
         *                    Else this bit is used to identify EgressMirrored
        */
        patternData.ruleEgrStandardIpv4L4.commonEgrStd.srcTrgOrTxMirror =
            /* TO CPU */
            (GT_U8)(inFields[28]);  /* srcTrg */
            /* NOT TO CPU - txMirror absent */

        /*
         * srcInfo          - case(Key<PacketCmd>):
         *                    Forward and SrcIsTrunk is 1: SrcTrunkID[6:0]
         *                    Forward and  SrcIsTrunk is 0: SrcDev[6:0]
         *                    ToCPU: SrcDev[6:0]
         *                    FromCPU: SrcDev[6:0]
         *                    ToAnalyzer: SrcDev[6:0]
        */
         patternData.ruleEgrStandardIpv4L4.commonEgrStd.srcInfo = (GT_U8)(
             /* Forward */
             (inFields[26] & 0x7F)      | /* SrcTrunkID[6:0]*/
             (inFields[25] & 0x7F)      | /* SrcDev[6:0] */
             /* TO CPU */
             (inFields[29] & 0x7F)      | /* SrcDev[6:0] */
             /* FROM CPU */
             (inFields[33] & 0x7F)      | /* SrcDev[6:0] */
             /* TO Analyzer */
             (inFields[35] & 0x7F));       /* SrcDev[6:0] */

        patternData.ruleEgrStandardIpv4L4.commonEgrStd.sourceId = (GT_U8)inFields[36];
        patternData.ruleEgrStandardIpv4L4.commonEgrStd.srcIsTrunk = (GT_U8)inFields[37];

        galtisIpAddr(&tmpIpv4Addr, (GT_U8*)inFields[38]);

        cmdOsMemCpy(patternData.ruleEgrStandardIpv4L4.commonEgrStdIp.dipBits31_0,
                    tmpIpv4Addr.arIP,
                    sizeof(patternData.ruleEgrStandardIpv4L4.commonEgrStdIp.dipBits31_0));

        galtisIpAddr(&patternData.ruleEgrStandardIpv4L4.sip, (GT_U8*)inFields[39]);

        patternData.ruleEgrStandardIpv4L4.commonEgrStdIp.tcpUdpPortComparators = (GT_U8)inFields[40];
        patternData.ruleEgrStandardIpv4L4.isBc = (GT_U8)inFields[41];
        pattern_set = GT_TRUE;
    }

    if ((mask_set == GT_TRUE) && (pattern_set == GT_TRUE))
    {
        /* due to errata the pattern.isKnown = (~ pattern.isUnknown) */
        /* for don't care case must be set to 0                       */
        patternData.ruleEgrStandardIpv4L4.commonEgrStd.egrFilterRegOrCpuCode7 &=
            maskData.ruleEgrStandardIpv4L4.commonEgrStd.egrFilterRegOrCpuCode7;
    }

    return CMD_OK;
}


/********************************************************************/
static CMD_STATUS wrCpssExMxPmPclEgressRule_EXT_NOT_IPV6_WriteNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_IPADDR  tmpIpv4Addr;

    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_E;

    /* map input arguments to locals */
    /* inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1 */

    if(inFields[1] == 0)  /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleEgrExtendedNotIpv6.commonEgr.pclId = (GT_U16)inFields[2];
        maskData.ruleEgrExtendedNotIpv6.commonEgr.srcPort = (GT_U8)inFields[3];
        maskData.ruleEgrExtendedNotIpv6.commonEgr.isTagged = (GT_U8)inFields[4];
        maskData.ruleEgrExtendedNotIpv6.commonEgr.up = (GT_U8)inFields[5];
        maskData.ruleEgrExtendedNotIpv6.commonEgr.vid = (GT_U16)inFields[6];
        maskData.ruleEgrExtendedNotIpv6.commonEgr.isIp = (GT_U8)inFields[7];
        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.isL2Valid = (GT_U8)inFields[8];

        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.ipProtocol = (GT_U8)inFields[9];
        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.isIpv6 = (GT_U8)inFields[10];
        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.dscp = (GT_U8)inFields[11];
        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.isL4Valid = (GT_U8)inFields[12];
        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.l4Byte0 = (GT_U8)inFields[13];
        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.l4Byte1 = (GT_U8)inFields[14];
        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.l4Byte13 = (GT_U8)inFields[15];
        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.l4Byte2 = (GT_U8)inFields[16];
        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.l4Byte3 = (GT_U8)inFields[17];

        maskData.ruleEgrExtendedNotIpv6.ethType = (GT_U16)inFields[18];
        maskData.ruleEgrExtendedNotIpv6.l2EncapType = (GT_U8)inFields[19];
        galtisMacAddr(&maskData.ruleEgrExtendedNotIpv6.macDa, (GT_U8*)inFields[20]);
        galtisMacAddr(&maskData.ruleEgrExtendedNotIpv6.macSa, (GT_U8*)inFields[21]);

        galtisIpAddr(&tmpIpv4Addr, (GT_U8*)inFields[22]);

        cmdOsMemCpy(maskData.ruleEgrExtendedNotIpv6.dipBits31_0,
                    tmpIpv4Addr.arIP,
                    sizeof(maskData.ruleEgrExtendedNotIpv6.dipBits31_0));

        galtisIpAddr(&tmpIpv4Addr, (GT_U8*)inFields[23]);

        cmdOsMemCpy(maskData.ruleEgrExtendedNotIpv6.commonEgrExt.sipBits31_0,
                    tmpIpv4Addr.arIP,
                    sizeof(maskData.ruleEgrExtendedNotIpv6.commonEgrExt.sipBits31_0));

        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.packetCmd = (GT_U8)inFields[24];
   /*     maskData.ruleEgrExtendedNotIpv6.commonEgr.isIpOrMplsRouted = (GT_U8)inFields[25];
         errata isUnknown changed to isKnown (mask - as is) */
        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.egrFilterRegOrCpuCode7 = (GT_U8)inFields[26];
  /*    maskData.ruleEgrExtendedNotIpv6.egress.isVidx = (GT_U8)inFields[27];  */

        /* handle multiplexed fields.
          Compose these fields as OR of all possible logical fields.
          The responsibility of RDE test is fill in not relevant fields as ZEROs.
         */

         /*
         * trgOrCpuCode0_6OrCosOrSniff - case(Key<PacketCmd>):
         *                    Forward: {SrcTrunkID[7],TrgPort}
         *                    ToCPU: CPU_CODE[6:0]
         *                    FromCPU: {1'b0, EgressFilterEn, TC[2:0], DP[1:0]}
         *                    ToAnalyzer:  {6'b0, RxSniff}
         */
        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.trgOrCpuCode0_6OrCosOrSniff = (GT_U8) (
            /* Forward packet command */
                /* trgPort is absent */
              ((inFields[29] >> 7) & 1) |  /* srcTrunk ID bit 7 */
            /* To CPU */
              (inFields[30] & 0x7f)     |  /* CPU Code 0:6 */
            /*  From CPU */
              (inFields[34] & 3)        |   /* DP */
              ((inFields[33] & 7) << 2) |   /* TC */
              ((inFields[35] & 1) << 5) |   /* egrFilterEnable*/
            /*  To analizer */
              ((inFields[37]) & 1));           /* RxSniff */

        /*
         * srcTrgOrTxMirror - if ToCPU: this bits indicate SrcTrg.
         *                    Else:
         *                    If Conf<EnableEgressMirrorAsForward> == 0:
         *                    Reserved
         *                    If Conf<EnableEgressMirrorAsForward> == 1:
         *                    From CPU: 0
         *                    Else this bit is used to identify EgressMirrored
        */
        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.srcTrgOrTxMirror =
            /* TO CPU */
            (GT_U8)(inFields[31]);  /* srcTrg */
            /* NOT TO CPU - txMirror absent */

        /*
         * srcInfo          - case(Key<PacketCmd>):
         *                    Forward and SrcIsTrunk is 1: SrcTrunkID[6:0]
         *                    Forward and  SrcIsTrunk is 0: SrcDev[6:0]
         *                    ToCPU: SrcDev[6:0]
         *                    FromCPU: SrcDev[6:0]
         *                    ToAnalyzer: SrcDev[6:0]
        */
         maskData.ruleEgrExtendedNotIpv6.commonEgrExt.srcInfo = (GT_U8)(
             /* Forward */
             (inFields[29] & 0x7F)      | /* SrcTrunkID[6:0]*/
             (inFields[28] & 0x7F)      | /* SrcDev[6:0] */
             /* TO CPU */
             (inFields[32] & 0x7F)      | /* SrcDev[6:0] */
             /* FROM CPU */
             (inFields[36] & 0x7F)      | /* SrcDev[6:0] */
             /* TO Analyzer */
             (inFields[38] & 0x7F));       /* SrcDev[6:0] */

        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.sourceId = (GT_U8)inFields[39];
        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.srcIsTrunk = (GT_U8)inFields[40];
        maskData.ruleEgrExtendedNotIpv6.commonEgr.isMpls = (GT_U8)inFields[41];

        maskData.ruleEgrExtendedNotIpv6.commonEgrExt.tcpUdpPortComparators = (GT_U8)inFields[42];
        maskData.ruleEgrExtendedNotIpv6.isIpv4Fragment = (GT_U8)inFields[43];
        mask_set = GT_TRUE;
    }
    else  /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleEgrExtendedNotIpv6.commonEgr.pclId = (GT_U16)inFields[2];
        patternData.ruleEgrExtendedNotIpv6.commonEgr.srcPort = (GT_U8)inFields[3];
        patternData.ruleEgrExtendedNotIpv6.commonEgr.isTagged = (GT_U8)inFields[4];
        patternData.ruleEgrExtendedNotIpv6.commonEgr.up = (GT_U8)inFields[5];
        patternData.ruleEgrExtendedNotIpv6.commonEgr.vid = (GT_U16)inFields[6];
        patternData.ruleEgrExtendedNotIpv6.commonEgr.isIp = (GT_U8)inFields[7];
        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.isL2Valid = (GT_U8)inFields[8];

        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.ipProtocol = (GT_U8)inFields[9];
        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.isIpv6 = (GT_U8)inFields[10];
        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.dscp = (GT_U8)inFields[11];
        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.isL4Valid = (GT_U8)inFields[12];
        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.l4Byte0 = (GT_U8)inFields[13];
        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.l4Byte1 = (GT_U8)inFields[14];
        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.l4Byte13 = (GT_U8)inFields[15];
        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.l4Byte2 = (GT_U8)inFields[16];
        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.l4Byte3 = (GT_U8)inFields[17];

        patternData.ruleEgrExtendedNotIpv6.ethType = (GT_U16)inFields[18];
        patternData.ruleEgrExtendedNotIpv6.l2EncapType = (GT_U8)inFields[19];
        galtisMacAddr(&patternData.ruleEgrExtendedNotIpv6.macDa, (GT_U8*)inFields[20]);
        galtisMacAddr(&patternData.ruleEgrExtendedNotIpv6.macSa, (GT_U8*)inFields[21]);

        galtisIpAddr(&tmpIpv4Addr, (GT_U8*)inFields[22]);

        cmdOsMemCpy(patternData.ruleEgrExtendedNotIpv6.dipBits31_0,
                    tmpIpv4Addr.arIP,
                    sizeof(patternData.ruleEgrExtendedNotIpv6.dipBits31_0));

        galtisIpAddr(&tmpIpv4Addr, (GT_U8*)inFields[23]);

        cmdOsMemCpy(patternData.ruleEgrExtendedNotIpv6.commonEgrExt.sipBits31_0,
                    tmpIpv4Addr.arIP,
                    sizeof(patternData.ruleEgrExtendedNotIpv6.commonEgrExt.sipBits31_0));


        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.packetCmd = (GT_U8)inFields[24];
   /*     patternData.ruleEgrExtendedNotIpv6.commonEgr.isIpOrMplsRouted = (GT_U8)inFields[25];
         errata isUnknown changed to isKnown */
        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.egrFilterRegOrCpuCode7 = (~ (GT_U8)inFields[26]) & 1;
   /*   patternData.ruleEgrExtendedNotIpv6.egress.isVidx = (GT_U8)inFields[27];  */

        /* handle multiplexed fields.
          Compose these fields as OR of all possible logical fields.
          The responsibility of RDE test is fill in not relevant fields as ZEROs.
         */

         /*
         * trgOrCpuCode0_6OrCosOrSniff - case(Key<PacketCmd>):
         *                    Forward: {SrcTrunkID[7],TrgPort}
         *                    ToCPU: CPU_CODE[6:0]
         *                    FromCPU: {1'b0, EgressFilterEn, TC[2:0], DP[1:0]}
         *                    ToAnalyzer:  {6'b0, RxSniff}
         */
        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.trgOrCpuCode0_6OrCosOrSniff = (GT_U8) (
            /* Forward packet command */
                /* trgPort is absent */
              ((inFields[29] >> 7) & 1) |  /* srcTrunk ID bit 7 */
            /* To CPU */
              (inFields[30] & 0x7f)     |  /* CPU Code 0:6 */
            /*  From CPU */
              (inFields[34] & 3)        |   /* DP */
              ((inFields[33] & 7) << 2) |   /* TC */
              ((inFields[35] & 1) << 5) |   /* egrFilterEnable*/
            /*  To analizer */
              ((inFields[37]) & 1));           /* RxSniff */

        /*
         * srcTrgOrTxMirror - if ToCPU: this bits indicate SrcTrg.
         *                    Else:
         *                    If Conf<EnableEgressMirrorAsForward> == 0:
         *                    Reserved
         *                    If Conf<EnableEgressMirrorAsForward> == 1:
         *                    From CPU: 0
         *                    Else this bit is used to identify EgressMirrored
        */
        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.srcTrgOrTxMirror =
            /* TO CPU */
            (GT_U8)(inFields[31]);  /* srcTrg */
            /* NOT TO CPU - txMirror absent */

        /*
         * srcInfo          - case(Key<PacketCmd>):
         *                    Forward and SrcIsTrunk is 1: SrcTrunkID[6:0]
         *                    Forward and  SrcIsTrunk is 0: SrcDev[6:0]
         *                    ToCPU: SrcDev[6:0]
         *                    FromCPU: SrcDev[6:0]
         *                    ToAnalyzer: SrcDev[6:0]
        */
         patternData.ruleEgrExtendedNotIpv6.commonEgrExt.srcInfo = (GT_U8)(
             /* Forward */
             (inFields[29] & 0x7F)      | /* SrcTrunkID[6:0]*/
             (inFields[28] & 0x7F)      | /* SrcDev[6:0] */
             /* TO CPU */
             (inFields[32] & 0x7F)      | /* SrcDev[6:0] */
             /* FROM CPU */
             (inFields[36] & 0x7F)      | /* SrcDev[6:0] */
             /* TO Analyzer */
             (inFields[38] & 0x7F));       /* SrcDev[6:0] */

    /*    patternData.ruleEgrExtendedNotIpv6.egress.pktTypeDep.fwdFromPort.srcDevId = (GT_U8)inFields[28];
        patternData.ruleEgrExtendedNotIpv6.egress.pktTypeDep.fwdFromTrunk.srcTrunkId = (GT_U8)inFields[29];
        patternData.ruleEgrExtendedNotIpv6.egress.pktTypeDep.toCpu.cpuCode = (GT_U8)inFields[30];
        patternData.ruleEgrExtendedNotIpv6.egress.pktTypeDep.toCpu.srcTrg = (GT_U8)inFields[31];
        patternData.ruleEgrExtendedNotIpv6.egress.pktTypeDep.toCpu.srcDevId = (GT_U8)inFields[32];
        patternData.ruleEgrExtendedNotIpv6.egress.pktTypeDep.fromCpu.tc = (GT_U8)inFields[33];
        patternData.ruleEgrExtendedNotIpv6.egress.pktTypeDep.fromCpu.dp = (GT_U8)inFields[34];
        patternData.ruleEgrExtendedNotIpv6.egress.pktTypeDep.fromCpu.egrFilterEn = (GT_U8)inFields[35];
        patternData.ruleEgrExtendedNotIpv6.egress.pktTypeDep.fromCpu.srcDevId = (GT_U8)inFields[36];
        patternData.ruleEgrExtendedNotIpv6.egress.pktTypeDep.toAnalyzer.rxSniffer = (GT_U8)inFields[37];
        patternData.ruleEgrExtendedNotIpv6.egress.pktTypeDep.toAnalyzer.srcDevId = (GT_U8)inFields[38]; */

        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.sourceId = (GT_U8)inFields[39];
        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.srcIsTrunk = (GT_U8)inFields[40];
        patternData.ruleEgrExtendedNotIpv6.commonEgr.isMpls = (GT_U8)inFields[41];

        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.tcpUdpPortComparators = (GT_U8)inFields[42];
        patternData.ruleEgrExtendedNotIpv6.isIpv4Fragment = (GT_U8)inFields[43];
        pattern_set = GT_TRUE;
    }

    if ((mask_set == GT_TRUE) && (pattern_set == GT_TRUE))
    {
        /* due to errata the pattern.isKnown = (~ pattern.isUnknown) */
        /* for don't care case must be set to 0                       */
        patternData.ruleEgrExtendedNotIpv6.commonEgrExt.egrFilterRegOrCpuCode7 &=
            maskData.ruleEgrExtendedNotIpv6.commonEgrExt.egrFilterRegOrCpuCode7;
    }

    return CMD_OK;
}

/********************************************************************/
static CMD_STATUS wrCpssExMxPmPclEgressRule_EXT_IPV6_L2_WriteNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_IPV6ADDR   tmpIpv6Addr;

    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L2_E;

    /* map input arguments to locals */
    /* inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1 */

    if(inFields[1] == 0)  /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleEgrExtendedIpv6L2.commonEgr.pclId = (GT_U16)inFields[2];
        maskData.ruleEgrExtendedIpv6L2.commonEgr.srcPort = (GT_U8)inFields[3];
        maskData.ruleEgrExtendedIpv6L2.commonEgr.isTagged = (GT_U8)inFields[4];
        maskData.ruleEgrExtendedIpv6L2.commonEgr.up = (GT_U8)inFields[5];
        maskData.ruleEgrExtendedIpv6L2.commonEgr.vid = (GT_U16)inFields[6];
        maskData.ruleEgrExtendedIpv6L2.commonEgr.isIp = (GT_U8)inFields[7];
        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.isL2Valid = (GT_U8)inFields[8];

        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.isIpv6 = (GT_U8)inFields[9];
        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.ipProtocol = (GT_U8)inFields[10];
        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.dscp = (GT_U8)inFields[11];
        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.isL4Valid = (GT_U8)inFields[12];
        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.l4Byte0 = (GT_U8)inFields[13];
        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.l4Byte1 = (GT_U8)inFields[14];
        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.l4Byte13 = (GT_U8)inFields[15];
        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.l4Byte2 = (GT_U8)inFields[16];
        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.l4Byte3 = (GT_U8)inFields[17];

        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.packetCmd = (GT_U8)inFields[18];
        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.isIpOrMplsRouted = (GT_U8)inFields[19];
        /* errata isUnknown changed to isKnown (mask - as is) */
        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.egrFilterRegOrCpuCode7 = (GT_U8)inFields[20];
     /* maskData.ruleEgrExtendedIpv6L2.egress.isVidx = (GT_U8)inFields[21];  */

        /* handle multiplexed fields.
          Compose these fields as OR of all possible logical fields.
          The responsibility of RDE test is fill in not relevant fields as ZEROs.
         */

         /*
         * trgOrCpuCode0_6OrCosOrSniff - case(Key<PacketCmd>):
         *                    Forward: {SrcTrunkID[7],TrgPort}
         *                    ToCPU: CPU_CODE[6:0]
         *                    FromCPU: {1'b0, EgressFilterEn, TC[2:0], DP[1:0]}
         *                    ToAnalyzer:  {6'b0, RxSniff}
         */
        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.trgOrCpuCode0_6OrCosOrSniff = (GT_U8) (
            /* Forward packet command */
                /* trgPort is absent */
              ((inFields[23] >> 7) & 1) |  /* srcTrunk ID bit 7 */
            /* To CPU */
              (inFields[24] & 0x7f)     |  /* CPU Code 0:6 */
            /*  From CPU */
              (inFields[28] & 3)        |   /* DP */
              ((inFields[27] & 7) << 2) |   /* TC */
              ((inFields[29] & 1) << 5) |   /* egrFilterEnable*/
            /*  To analizer */
              ((inFields[31]) & 1));           /* RxSniff */

        /*
         * srcTrgOrTxMirror - if ToCPU: this bits indicate SrcTrg.
         *                    Else:
         *                    If Conf<EnableEgressMirrorAsForward> == 0:
         *                    Reserved
         *                    If Conf<EnableEgressMirrorAsForward> == 1:
         *                    From CPU: 0
         *                    Else this bit is used to identify EgressMirrored
        */
        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.srcTrgOrTxMirror =
            /* TO CPU */
            (GT_U8)(inFields[25]);  /* srcTrg */
            /* NOT TO CPU - txMirror absent */

        /*
         * srcInfo          - case(Key<PacketCmd>):
         *                    Forward and SrcIsTrunk is 1: SrcTrunkID[6:0]
         *                    Forward and  SrcIsTrunk is 0: SrcDev[6:0]
         *                    ToCPU: SrcDev[6:0]
         *                    FromCPU: SrcDev[6:0]
         *                    ToAnalyzer: SrcDev[6:0]
        */
         maskData.ruleEgrExtendedIpv6L2.commonEgrExt.srcInfo = (GT_U8)(
             /* Forward */
             (inFields[23] & 0x7F)      | /* SrcTrunkID[6:0]*/
             (inFields[22] & 0x7F)      | /* SrcDev[6:0] */
             /* TO CPU */
             (inFields[26] & 0x7F)      | /* SrcDev[6:0] */
             /* FROM CPU */
             (inFields[30] & 0x7F)      | /* SrcDev[6:0] */
             /* TO Analyzer */
             (inFields[32] & 0x7F));       /* SrcDev[6:0] */

        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.sourceId = (GT_U8)inFields[33];
        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.srcIsTrunk = (GT_U8)inFields[34];
        maskData.ruleEgrExtendedIpv6L2.commonEgr.isMpls = (GT_U8)inFields[35];

        galtisMacAddr(&maskData.ruleEgrExtendedIpv6L2.macDa, (GT_U8*)inFields[36]);
        galtisMacAddr(&maskData.ruleEgrExtendedIpv6L2.macSa, (GT_U8*)inFields[37]);

        maskData.ruleEgrExtendedIpv6L2.commonEgrExt.tcpUdpPortComparators = (GT_U8)inFields[38];
        maskData.ruleEgrExtendedIpv6L2.dipBits127_120 = (GT_U8)inFields[39];

        galtisIpv6Addr(&tmpIpv6Addr, (GT_U8*)inFields[40]);

        /* copy MSBs of IPv6 SA */
        cmdOsMemCpy(maskData.ruleEgrExtendedIpv6L2.sipBits127_32,
                     tmpIpv6Addr.arIP,
                     sizeof(maskData.ruleEgrExtendedIpv6L2.sipBits127_32));

        /* copy LSBs of IPv6 SA */
        cmdOsMemCpy(maskData.ruleEgrExtendedIpv6L2.commonEgrExt.sipBits31_0,
                     &tmpIpv6Addr.arIP[12],
                     sizeof(maskData.ruleEgrExtendedIpv6L2.commonEgrExt.sipBits31_0));


        mask_set = GT_TRUE;
    }
    else  /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleEgrExtendedIpv6L2.commonEgr.pclId = (GT_U16)inFields[2];
        patternData.ruleEgrExtendedIpv6L2.commonEgr.srcPort = (GT_U8)inFields[3];
        patternData.ruleEgrExtendedIpv6L2.commonEgr.isTagged = (GT_U8)inFields[4];
        patternData.ruleEgrExtendedIpv6L2.commonEgr.up = (GT_U8)inFields[5];
        patternData.ruleEgrExtendedIpv6L2.commonEgr.vid = (GT_U16)inFields[6];
        patternData.ruleEgrExtendedIpv6L2.commonEgr.isIp = (GT_U8)inFields[7];
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.isL2Valid = (GT_U8)inFields[8];

        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.isIpv6 = (GT_U8)inFields[9];
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.ipProtocol = (GT_U8)inFields[10];
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.dscp = (GT_U8)inFields[11];
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.isL4Valid = (GT_U8)inFields[12];
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.l4Byte0 = (GT_U8)inFields[13];
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.l4Byte1 = (GT_U8)inFields[14];
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.l4Byte13 = (GT_U8)inFields[15];
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.l4Byte2 = (GT_U8)inFields[16];
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.l4Byte3 = (GT_U8)inFields[17];

        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.packetCmd = (GT_U8)inFields[18];
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.isIpOrMplsRouted = (GT_U8)inFields[19];
        /* errata isUnknown changed to isKnown */
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.egrFilterRegOrCpuCode7 = (~ (GT_U8)inFields[20]) & 1;
    /*  patternData.ruleEgrExtendedIpv6L2.egress.isVidx = (GT_U8)inFields[21]; */

        /* handle multiplexed fields.
          Compose these fields as OR of all possible logical fields.
          The responsibility of RDE test is fill in not relevant fields as ZEROs.
         */

         /*
         * trgOrCpuCode0_6OrCosOrSniff - case(Key<PacketCmd>):
         *                    Forward: {SrcTrunkID[7],TrgPort}
         *                    ToCPU: CPU_CODE[6:0]
         *                    FromCPU: {1'b0, EgressFilterEn, TC[2:0], DP[1:0]}
         *                    ToAnalyzer:  {6'b0, RxSniff}
         */
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.trgOrCpuCode0_6OrCosOrSniff = (GT_U8) (
            /* Forward packet command */
                /* trgPort is absent */
              ((inFields[23] >> 7) & 1) |  /* srcTrunk ID bit 7 */
            /* To CPU */
              (inFields[24] & 0x7f)     |  /* CPU Code 0:6 */
            /*  From CPU */
              (inFields[28] & 3)        |   /* DP */
              ((inFields[27] & 7) << 2) |   /* TC */
              ((inFields[29] & 1) << 5) |   /* egrFilterEnable*/
            /*  To analizer */
              ((inFields[31]) & 1));           /* RxSniff */

        /*
         * srcTrgOrTxMirror - if ToCPU: this bits indicate SrcTrg.
         *                    Else:
         *                    If Conf<EnableEgressMirrorAsForward> == 0:
         *                    Reserved
         *                    If Conf<EnableEgressMirrorAsForward> == 1:
         *                    From CPU: 0
         *                    Else this bit is used to identify EgressMirrored
        */
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.srcTrgOrTxMirror =
            /* TO CPU */
            (GT_U8)(inFields[25]);  /* srcTrg */
            /* NOT TO CPU - txMirror absent */

        /*
         * srcInfo          - case(Key<PacketCmd>):
         *                    Forward and SrcIsTrunk is 1: SrcTrunkID[6:0]
         *                    Forward and  SrcIsTrunk is 0: SrcDev[6:0]
         *                    ToCPU: SrcDev[6:0]
         *                    FromCPU: SrcDev[6:0]
         *                    ToAnalyzer: SrcDev[6:0]
        */
         patternData.ruleEgrExtendedIpv6L2.commonEgrExt.srcInfo = (GT_U8)(
             /* Forward */
             (inFields[23] & 0x7F)      | /* SrcTrunkID[6:0]*/
             (inFields[22] & 0x7F)      | /* SrcDev[6:0] */
             /* TO CPU */
             (inFields[26] & 0x7F)      | /* SrcDev[6:0] */
             /* FROM CPU */
             (inFields[30] & 0x7F)      | /* SrcDev[6:0] */
             /* TO Analyzer */
             (inFields[32] & 0x7F));       /* SrcDev[6:0] */

        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.sourceId = (GT_U8)inFields[33];
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.srcIsTrunk = (GT_U8)inFields[34];
        patternData.ruleEgrExtendedIpv6L2.commonEgr.isMpls = (GT_U8)inFields[35];

        galtisMacAddr(&patternData.ruleEgrExtendedIpv6L2.macDa, (GT_U8*)inFields[36]);
        galtisMacAddr(&patternData.ruleEgrExtendedIpv6L2.macSa, (GT_U8*)inFields[37]);

        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.tcpUdpPortComparators = (GT_U8)inFields[38];
        patternData.ruleEgrExtendedIpv6L2.dipBits127_120 = (GT_U8)inFields[39];

        galtisIpv6Addr(&tmpIpv6Addr, (GT_U8*)inFields[40]);

        /* copy MSBs of IPv6 SA */
        cmdOsMemCpy(patternData.ruleEgrExtendedIpv6L2.sipBits127_32,
                     tmpIpv6Addr.arIP,
                     sizeof(patternData.ruleEgrExtendedIpv6L2.sipBits127_32));

        /* copy LSBs of IPv6 SA */
        cmdOsMemCpy(patternData.ruleEgrExtendedIpv6L2.commonEgrExt.sipBits31_0,
                     &tmpIpv6Addr.arIP[12],
                     sizeof(patternData.ruleEgrExtendedIpv6L2.commonEgrExt.sipBits31_0));

        pattern_set = GT_TRUE;
    }

    if ((mask_set == GT_TRUE) && (pattern_set == GT_TRUE))
    {
         /*  due to errata the pattern.isKnown = (~ pattern.isUnknown)
            for don't care case must be set to 0                       */
        patternData.ruleEgrExtendedIpv6L2.commonEgrExt.egrFilterRegOrCpuCode7 &=
            maskData.ruleEgrExtendedIpv6L2.commonEgrExt.egrFilterRegOrCpuCode7;
    }

    return CMD_OK;
}

/********************************************************************/
static CMD_STATUS wrCpssExMxPmPclEgressRule_EXT_IPV6_L4_WriteNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_IPV6ADDR   tmpIpv6Addr;

    ruleFormat = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L4_E;

    /* map input arguments to locals */
    /* inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1 */

    if(inFields[1] == 0)   /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];
        maskData.ruleEgrExtendedIpv6L4.commonEgr.pclId = (GT_U16)inFields[2];
        maskData.ruleEgrExtendedIpv6L4.commonEgr.srcPort = (GT_U8)inFields[3];
        maskData.ruleEgrExtendedIpv6L4.commonEgr.isTagged = (GT_U8)inFields[4];
        maskData.ruleEgrExtendedIpv6L4.commonEgr.up = (GT_U8)inFields[5];
        maskData.ruleEgrExtendedIpv6L4.commonEgr.vid = (GT_U16)inFields[6];
        maskData.ruleEgrExtendedIpv6L4.commonEgr.isIp = (GT_U8)inFields[7];
        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.isL2Valid = (GT_U8)inFields[8];

        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.isIpv6 = (GT_U8)inFields[9];
        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.ipProtocol = (GT_U8)inFields[10];
        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.dscp = (GT_U8)inFields[11];
        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.isL4Valid = (GT_U8)inFields[12];
        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.l4Byte0 = (GT_U8)inFields[13];
        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.l4Byte1 = (GT_U8)inFields[14];
        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.l4Byte13 = (GT_U8)inFields[15];
        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.l4Byte2 = (GT_U8)inFields[16];
        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.l4Byte3 = (GT_U8)inFields[17];

        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.packetCmd = (GT_U8)inFields[18];
        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.isIpOrMplsRouted = (GT_U8)inFields[19];
        /* errata isUnknown changed to isKnown (mask - as is) */
        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.egrFilterRegOrCpuCode7 = (GT_U8)inFields[20];
   /*   maskData.ruleEgrExtendedIpv6L4.egress.isVidx = (GT_U8)inFields[21];   */

        /* handle multiplexed fields.
          Compose these fields as OR of all possible logical fields.
          The responsibility of RDE test is fill in not relevant fields as ZEROs.
         */

         /*
         * trgOrCpuCode0_6OrCosOrSniff - case(Key<PacketCmd>):
         *                    Forward: {SrcTrunkID[7],TrgPort}
         *                    ToCPU: CPU_CODE[6:0]
         *                    FromCPU: {1'b0, EgressFilterEn, TC[2:0], DP[1:0]}
         *                    ToAnalyzer:  {6'b0, RxSniff}
         */
        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.trgOrCpuCode0_6OrCosOrSniff = (GT_U8) (
            /* Forward packet command */
                /* trgPort is absent */
              ((inFields[23] >> 7) & 1) |  /* srcTrunk ID bit 7 */
            /* To CPU */
              (inFields[24] & 0x7f)     |  /* CPU Code 0:6 */
            /*  From CPU */
              (inFields[28] & 3)        |   /* DP */
              ((inFields[27] & 7) << 2) |   /* TC */
              ((inFields[29] & 1) << 5) |   /* egrFilterEnable*/
            /*  To analizer */
              ((inFields[37]) & 1));           /* RxSniff */

        /*
         * srcTrgOrTxMirror - if ToCPU: this bits indicate SrcTrg.
         *                    Else:
         *                    If Conf<EnableEgressMirrorAsForward> == 0:
         *                    Reserved
         *                    If Conf<EnableEgressMirrorAsForward> == 1:
         *                    From CPU: 0
         *                    Else this bit is used to identify EgressMirrored
        */
        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.srcTrgOrTxMirror =
            /* TO CPU */
            (GT_U8)(inFields[25]);  /* srcTrg */
            /* NOT TO CPU - txMirror absent */

        /*
         * srcInfo          - case(Key<PacketCmd>):
         *                    Forward and SrcIsTrunk is 1: SrcTrunkID[6:0]
         *                    Forward and  SrcIsTrunk is 0: SrcDev[6:0]
         *                    ToCPU: SrcDev[6:0]
         *                    FromCPU: SrcDev[6:0]
         *                    ToAnalyzer: SrcDev[6:0]
        */
         maskData.ruleEgrExtendedIpv6L4.commonEgrExt.srcInfo = (GT_U8)(
             /* Forward */
             (inFields[23] & 0x7F)      | /* SrcTrunkID[6:0]*/
             (inFields[22] & 0x7F)      | /* SrcDev[6:0] */
             /* TO CPU */
             (inFields[26] & 0x7F)      | /* SrcDev[6:0] */
             /* FROM CPU */
             (inFields[30] & 0x7F)      | /* SrcDev[6:0] */
             /* TO Analyzer */
             (inFields[32] & 0x7F));       /* SrcDev[6:0] */

        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.sourceId = (GT_U8)inFields[33];
        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.srcIsTrunk = (GT_U8)inFields[34];
        maskData.ruleEgrExtendedIpv6L4.commonEgr.isMpls = (GT_U8)inFields[35];

        maskData.ruleEgrExtendedIpv6L4.commonEgrExt.tcpUdpPortComparators = (GT_U8)inFields[36];

        galtisIpv6Addr(&maskData.ruleEgrExtendedIpv6L4.dip, (GT_U8*)inFields[37]);

        galtisIpv6Addr(&tmpIpv6Addr, (GT_U8*)inFields[38]);

        /* copy MSBs of IPv6 SA */
        cmdOsMemCpy(maskData.ruleEgrExtendedIpv6L4.sipBits127_32,
                     tmpIpv6Addr.arIP,
                     sizeof(maskData.ruleEgrExtendedIpv6L4.sipBits127_32));

        /* copy LSBs of IPv6 SA */
        cmdOsMemCpy(maskData.ruleEgrExtendedIpv6L4.commonEgrExt.sipBits31_0,
                     &tmpIpv6Addr.arIP[12],
                     sizeof(maskData.ruleEgrExtendedIpv6L4.commonEgrExt.sipBits31_0));

        mask_set = GT_TRUE;
    }
    else  /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternData.ruleEgrExtendedIpv6L4.commonEgr.pclId = (GT_U16)inFields[2];
        patternData.ruleEgrExtendedIpv6L4.commonEgr.srcPort = (GT_U8)inFields[3];
        patternData.ruleEgrExtendedIpv6L4.commonEgr.isTagged = (GT_U8)inFields[4];
        patternData.ruleEgrExtendedIpv6L4.commonEgr.up = (GT_U8)inFields[5];
        patternData.ruleEgrExtendedIpv6L4.commonEgr.vid = (GT_U16)inFields[6];
        patternData.ruleEgrExtendedIpv6L4.commonEgr.isIp = (GT_U8)inFields[7];
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.isL2Valid = (GT_U8)inFields[8];

        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.isIpv6 = (GT_U8)inFields[9];
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.ipProtocol = (GT_U8)inFields[10];
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.dscp = (GT_U8)inFields[11];
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.isL4Valid = (GT_U8)inFields[12];
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.l4Byte0 = (GT_U8)inFields[13];
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.l4Byte1 = (GT_U8)inFields[14];
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.l4Byte13 = (GT_U8)inFields[15];
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.l4Byte2 = (GT_U8)inFields[16];
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.l4Byte3 = (GT_U8)inFields[17];

        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.packetCmd = (GT_U8)inFields[18];
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.isIpOrMplsRouted = (GT_U8)inFields[19];
        /* errata isUnknown changed to isKnown */
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.egrFilterRegOrCpuCode7 = (~ (GT_U8)inFields[20]) & 1;
     /* patternData.ruleEgrExtendedIpv6L4.egress.isVidx = (GT_U8)inFields[21]; */

        /* handle multiplexed fields.
          Compose these fields as OR of all possible logical fields.
          The responsibility of RDE test is fill in not relevant fields as ZEROs.
         */

         /*
         * trgOrCpuCode0_6OrCosOrSniff - case(Key<PacketCmd>):
         *                    Forward: {SrcTrunkID[7],TrgPort}
         *                    ToCPU: CPU_CODE[6:0]
         *                    FromCPU: {1'b0, EgressFilterEn, TC[2:0], DP[1:0]}
         *                    ToAnalyzer:  {6'b0, RxSniff}
         */
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.trgOrCpuCode0_6OrCosOrSniff = (GT_U8) (
            /* Forward packet command */
                /* trgPort is absent */
              ((inFields[23] >> 7) & 1) |  /* srcTrunk ID bit 7 */
            /* To CPU */
              (inFields[24] & 0x7f)     |  /* CPU Code 0:6 */
            /*  From CPU */
              (inFields[28] & 3)        |   /* DP */
              ((inFields[27] & 7) << 2) |   /* TC */
              ((inFields[29] & 1) << 5) |   /* egrFilterEnable*/
            /*  To analizer */
              ((inFields[37]) & 1));           /* RxSniff */

        /*
         * srcTrgOrTxMirror - if ToCPU: this bits indicate SrcTrg.
         *                    Else:
         *                    If Conf<EnableEgressMirrorAsForward> == 0:
         *                    Reserved
         *                    If Conf<EnableEgressMirrorAsForward> == 1:
         *                    From CPU: 0
         *                    Else this bit is used to identify EgressMirrored
        */
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.srcTrgOrTxMirror =
            /* TO CPU */
            (GT_U8)(inFields[25]);  /* srcTrg */
            /* NOT TO CPU - txMirror absent */

        /*
         * srcInfo          - case(Key<PacketCmd>):
         *                    Forward and SrcIsTrunk is 1: SrcTrunkID[6:0]
         *                    Forward and  SrcIsTrunk is 0: SrcDev[6:0]
         *                    ToCPU: SrcDev[6:0]
         *                    FromCPU: SrcDev[6:0]
         *                    ToAnalyzer: SrcDev[6:0]
        */
         patternData.ruleEgrExtendedIpv6L4.commonEgrExt.srcInfo = (GT_U8)(
             /* Forward */
             (inFields[23] & 0x7F)      | /* SrcTrunkID[6:0]*/
             (inFields[22] & 0x7F)      | /* SrcDev[6:0] */
             /* TO CPU */
             (inFields[26] & 0x7F)      | /* SrcDev[6:0] */
             /* FROM CPU */
             (inFields[30] & 0x7F)      | /* SrcDev[6:0] */
             /* TO Analyzer */
             (inFields[32] & 0x7F));       /* SrcDev[6:0] */

        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.sourceId = (GT_U8)inFields[33];
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.srcIsTrunk = (GT_U8)inFields[34];
        patternData.ruleEgrExtendedIpv6L4.commonEgr.isMpls = (GT_U8)inFields[35];

        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.tcpUdpPortComparators = (GT_U8)inFields[36];

        galtisIpv6Addr(&patternData.ruleEgrExtendedIpv6L4.dip, (GT_U8*)inFields[37]);

        galtisIpv6Addr(&tmpIpv6Addr, (GT_U8*)inFields[38]);

        /* copy MSBs of IPv6 SA */
        cmdOsMemCpy(patternData.ruleEgrExtendedIpv6L4.sipBits127_32,
                     tmpIpv6Addr.arIP,
                     sizeof(patternData.ruleEgrExtendedIpv6L4.sipBits127_32));

        /* copy LSBs of IPv6 SA */
        cmdOsMemCpy(patternData.ruleEgrExtendedIpv6L4.commonEgrExt.sipBits31_0,
                     &tmpIpv6Addr.arIP[12],
                     sizeof(patternData.ruleEgrExtendedIpv6L4.commonEgrExt.sipBits31_0));

        pattern_set = GT_TRUE;
    }

    if ((mask_set == GT_TRUE) && (pattern_set == GT_TRUE))
    {
        /* due to errata the pattern.isKnown = (~ pattern.isUnknown) *
         for don't care case must be set to 0                       */
        patternData.ruleEgrExtendedIpv6L4.commonEgrExt.egrFilterRegOrCpuCode7 &=
            maskData.ruleEgrExtendedIpv6L4.commonEgrExt.egrFilterRegOrCpuCode7;
    }

    return CMD_OK;
}

/**********************************************************************************/
static CMD_STATUS wrCpssExMxPmPclEgressRuleWriteFirst
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

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));

    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;

    switch (inArgs[1] + 6)
    {
        case CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E:
            wrCpssExMxPmPclEgressRule_STD_NOT_IP_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IP_L2_QOS_E:
            wrCpssExMxPmPclEgressRule_STD_IP_L2_QOS_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IPV4_L4_E:
            wrCpssExMxPmPclEgressRule_STD_IPV4_L4_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_E:
            wrCpssExMxPmPclEgressRule_EXT_NOT_IPV6_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L2_E:
            wrCpssExMxPmPclEgressRule_EXT_IPV6_L2_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L4_E:
            wrCpssExMxPmPclEgressRule_EXT_IPV6_L4_WriteNext(
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

/**********************************************************************************/
static CMD_STATUS wrCpssExMxPmPclEgressRuleWriteNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result = GT_OK;
    GT_U8                               devNum;
    CPSS_EXMXPM_PCL_ACTION_UNT         *actionPtr;
    GT_U32                              actionIndex;
    GT_U32                              ruleIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */

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
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of Egress {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if(inFields[1] == 1 && pattern_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of Egress {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if ((mask_set == GT_FALSE) && (pattern_set == GT_FALSE))
    {
        cmdOsMemSet(&maskData , 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));
        cmdOsMemSet(&patternData, 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    switch (inArgs[1] + 6)
    {
        case CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E:
            wrCpssExMxPmPclEgressRule_STD_NOT_IP_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IP_L2_QOS_E:
            wrCpssExMxPmPclEgressRule_STD_IP_L2_QOS_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IPV4_L4_E:
            wrCpssExMxPmPclEgressRule_STD_IPV4_L4_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_E:
            wrCpssExMxPmPclEgressRule_EXT_NOT_IPV6_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L2_E:
            wrCpssExMxPmPclEgressRule_EXT_IPV6_L2_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L4_E:
            wrCpssExMxPmPclEgressRule_EXT_IPV6_L4_WriteNext(
                inArgs, inFields ,numFields ,outArgs);
            break;

        default:
            /* pack output arguments to galtis string */
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
    wrCpssExMxPmPclActionIndexGet(devNum, ruleIndex, &actionIndex);

    if (actionIndex == INVALID_RULE_INDEX)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Action table entry is not set.\n");
        return CMD_AGENT_ERROR;

    }

    if ((mask_set != GT_FALSE) && (pattern_set != GT_FALSE))
    {

        CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT  ruleInfo;
        CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType;
        WR_PCL_ACTION_STC*                     actionTablePtr;
        GT_U32                                 index;

        if (useExternalTcam == GT_FALSE)
        {
            tcamType = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
            ruleInfo.internalTcamRuleStartIndex=mask_ruleIndex;
            actionTablePtr = actionIntTable;
        }
        else
        {   /*bind extTcam params*/
            tcamType = CPSS_EXMXPM_TCAM_TYPE_EXTERNAL_E;
            wrCpssExMxPmPclExtConfigIndexGet(devNum, mask_ruleIndex, &index);
            if (index == INVALID_RULE_INDEX)
            {
                galtisOutput(outArgs, GT_FAIL, "%d", -1);
                return CMD_OK;
            }

            ruleInfo.externalTcamInfo.ruleInfo.tcamId=extTcamConfigPclTable[index].tcamId;
            ruleInfo.externalTcamInfo.ruleInfo.ruleStartIndex=extTcamConfigPclTable[index].ruleStartIndex;
            ruleInfo.externalTcamInfo.ruleInfo.ruleSize=extTcamConfigPclTable[index].ruleSize;
            ruleInfo.externalTcamInfo.ruleInfo.ageEnable=extTcamConfigPclTable[index].ageEnable;

            ruleInfo.externalTcamInfo.actionInfo.actionStartIndex=extTcamConfigPclTable[index].actionStartIndex;
            ruleInfo.externalTcamInfo.actionInfo.memoryType=extTcamConfigPclTable[index].memoryType;
            actionTablePtr = actionExtTable;
        }



        actionPtr = &(actionTablePtr[actionIndex].actionEntry);
        /* call cpss api function */
        result = cpssExMxPmPclRuleSet(
            devNum,tcamType,&ruleInfo,
            ruleFormat, actionTablePtr[actionIndex].actionFormat, GT_TRUE /*valid*/,
            &maskData, &patternData, actionPtr);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        if(result!=GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;

}
/*******************************************************************************/

static CMD_STATUS wrCpssExMxPmPclEgressRuleWriteEnd

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


/*******************************************************************************
* cpssExMxPmPclRuleInvalidate
*
* DESCRIPTION:
*   The function invalidates the Policy Rule
*
* APPLICABLE DEVICES:  All ExMxPm devices
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
static CMD_STATUS wrCpssExMxPmPclRuleInvalidate

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PCL_RULE_SIZE_ENT  ruleSize;
    GT_U32              ruleIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_PCL_RULE_SIZE_ENT)inArgs[1];
    ruleIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmIntTcamRuleValidStatusSet(devNum, ruleIndex, GT_FALSE);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclRuleValidStatusSet
*
* DESCRIPTION:
*   The function invalidates the Policy Rule
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*       ruleIndex       - index of the rule in the TCAM. See cpssExMxPclRuleSet.
*       valid           - new rule status: GT_TRUE - valid, GT_FALSE - invalid
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
static CMD_STATUS wrCpssExMxPmPclRuleValidStatusSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PCL_RULE_SIZE_ENT  ruleSize;
    GT_U32              ruleIndex;
    GT_BOOL             valid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_PCL_RULE_SIZE_ENT)inArgs[1];
    ruleIndex = (GT_U32)inArgs[2];
    valid = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmIntTcamRuleValidStatusSet(devNum, ruleIndex, valid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclRuleCopy
*
* DESCRIPTION:
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first cpssExMxPmPclRuleCopy should be
*  called. And after this cpssExMxPmPclRuleInvalidate should be used to
*  invalidate Rule in old position
*
* APPLICABLE DEVICES:  All ExMxPm devices
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
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclRuleCopy

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PCL_RULE_SIZE_ENT  ruleSize;
    GT_U32              ruleSrcIndex;
    GT_U32              ruleDstIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_PCL_RULE_SIZE_ENT)inArgs[1];
    ruleSrcIndex = (GT_U32)inArgs[2];
    ruleDstIndex = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmIntTcamRuleAndActionCopy(devNum, ruleSize, ruleSrcIndex, ruleDstIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclCfgTblSet(table command)
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
static CMD_STATUS wrCpssExMxPmPclCfgTblSet

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
    CPSS_EXMXPM_PCL_LOOKUP_CFG_STC  lookupCfgPtr;
    GT_U32                          correctBase, otherBase, otherMax, value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* default values */
    lookupCfgPtr.externalLookup = GT_FALSE;
    lookupCfgPtr.lookupType     = CPSS_EXMXPM_PCL_LOOKUP_TYPE_SINGLE_E;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    interfaceInfoPtr.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[1];

    switch (interfaceInfoPtr.type)
    {
    case CPSS_INTERFACE_PORT_E:
        interfaceInfoPtr.devPort.devNum = (GT_U8)inArgs[2];
        interfaceInfoPtr.devPort.portNum = (GT_U8)inArgs[3];
        break;

    case CPSS_INTERFACE_TRUNK_E:
        interfaceInfoPtr.trunkId = (GT_TRUNK_ID)inArgs[4];
        break;

    case CPSS_INTERFACE_VIDX_E:
        interfaceInfoPtr.vidx = (GT_U16)inArgs[5];
        break;

    case CPSS_INTERFACE_VID_E:
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
                                (CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT)inFields[2];
    lookupCfgPtr.groupKeyTypes.ipv4Key =
                                (CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT)inFields[3];
    lookupCfgPtr.groupKeyTypes.ipv6Key =
                                (CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT)inFields[4];

    /* convert ingress and engess constants according to direction */
    if (direction == CPSS_PCL_DIRECTION_INGRESS_E)
    {
        correctBase = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;
        otherBase   = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;
    }
    else
    {
        correctBase = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;
        otherBase   = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;
    }
    otherMax = otherBase + 6;

    value = lookupCfgPtr.groupKeyTypes.nonIpKey;
    if ((value >= otherBase) && (value < otherMax))
    {
        lookupCfgPtr.groupKeyTypes.nonIpKey = correctBase + (value - otherBase);
    }

    value = lookupCfgPtr.groupKeyTypes.ipv4Key;
    if ((value >= otherBase) && (value < otherMax))
    {
        lookupCfgPtr.groupKeyTypes.ipv4Key = correctBase + (value - otherBase);
    }

    value = lookupCfgPtr.groupKeyTypes.ipv6Key;
    if ((value >= otherBase) && (value < otherMax))
    {
        lookupCfgPtr.groupKeyTypes.ipv6Key = correctBase + (value - otherBase);
    }

    /* end of conversion */

    /* call cpss api function */
    result = cpssExMxPmPclCfgTblSet(devNum, &interfaceInfoPtr, direction,
                                              lookupNum, &lookupCfgPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmPclCfgTblGet(table command)
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
static CMD_STATUS wrCpssExMxPmPclCfgTblGet

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
    CPSS_EXMXPM_PCL_LOOKUP_CFG_STC    lookupCfgPtr;
    static GT_U32                   isNext = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(isNext == 1)
    {
        isNext = 0;
        /* there is no GetNext for wrapper */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    isNext = 1;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    interfaceInfoPtr.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[1];

    switch (interfaceInfoPtr.type)
    {
    case CPSS_INTERFACE_PORT_E:
        interfaceInfoPtr.devPort.devNum = (GT_U8)inArgs[2];
        interfaceInfoPtr.devPort.portNum = (GT_U8)inArgs[3];
        break;

    case CPSS_INTERFACE_TRUNK_E:
        interfaceInfoPtr.trunkId = (GT_TRUNK_ID)inArgs[4];
        break;

    case CPSS_INTERFACE_VIDX_E:
        interfaceInfoPtr.vidx = (GT_U16)inArgs[5];
        break;

    case CPSS_INTERFACE_VID_E:
        interfaceInfoPtr.vlanId = (GT_U16)inArgs[6];
        break;

    default:
        break;
    }

    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[7];
    lookupNum = (CPSS_PCL_LOOKUP_NUMBER_ENT)inArgs[8];


    result = cpssExMxPmPclCfgTblGet(devNum, &interfaceInfoPtr, direction,
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

/*******************************************************************************
* cpssExMxPmPclCfgTblV1Set(table command)
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
static CMD_STATUS wrCpssExMxPmPclCfgTblV1Set

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
    CPSS_EXMXPM_PCL_LOOKUP_CFG_STC  lookupCfgPtr;
    GT_U32                          correctBase, otherBase, otherMax, value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    interfaceInfoPtr.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[1];

    switch (interfaceInfoPtr.type)
    {
    case CPSS_INTERFACE_PORT_E:
        interfaceInfoPtr.devPort.devNum = (GT_U8)inArgs[2];
        interfaceInfoPtr.devPort.portNum = (GT_U8)inArgs[3];
        break;

    case CPSS_INTERFACE_TRUNK_E:
        interfaceInfoPtr.trunkId = (GT_TRUNK_ID)inArgs[4];
        break;

    case CPSS_INTERFACE_VIDX_E:
        interfaceInfoPtr.vidx = (GT_U16)inArgs[5];
        break;

    case CPSS_INTERFACE_VID_E:
        interfaceInfoPtr.vlanId = (GT_U16)inArgs[6];
        break;

    default:
        break;
    }

    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[7];
    lookupNum = (CPSS_PCL_LOOKUP_NUMBER_ENT)inArgs[8];


    lookupCfgPtr.enableLookup = (GT_BOOL)inFields[0];
    lookupCfgPtr.externalLookup = (GT_BOOL)inFields[1];
    lookupCfgPtr.lookupType = (CPSS_EXMXPM_PCL_LOOKUP_TYPE_ENT)inFields[2];

    lookupCfgPtr.pclId = (GT_U32)inFields[3];
    lookupCfgPtr.groupKeyTypes.nonIpKey =
                                (CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT)inFields[4];
    lookupCfgPtr.groupKeyTypes.ipv4Key =
                                (CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT)inFields[5];
    lookupCfgPtr.groupKeyTypes.ipv6Key =
                                (CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT)inFields[6];

    /* convert ingress and engess constants according to direction */
    if (direction == CPSS_PCL_DIRECTION_INGRESS_E)
    {
        correctBase = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;
        otherBase   = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;
    }
    else
    {
        correctBase = CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E;
        otherBase   = CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;
    }
    otherMax = otherBase + 6;

    value = lookupCfgPtr.groupKeyTypes.nonIpKey;
    if ((value >= otherBase) && (value < otherMax))
    {
        lookupCfgPtr.groupKeyTypes.nonIpKey = correctBase + (value - otherBase);
    }

    value = lookupCfgPtr.groupKeyTypes.ipv4Key;
    if ((value >= otherBase) && (value < otherMax))
    {
        lookupCfgPtr.groupKeyTypes.ipv4Key = correctBase + (value - otherBase);
    }

    value = lookupCfgPtr.groupKeyTypes.ipv6Key;
    if ((value >= otherBase) && (value < otherMax))
    {
        lookupCfgPtr.groupKeyTypes.ipv6Key = correctBase + (value - otherBase);
    }

    /* end of conversion */

    /* call cpss api function */
    result = cpssExMxPmPclCfgTblSet(devNum, &interfaceInfoPtr, direction,
                                              lookupNum, &lookupCfgPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmPclCfgTblV1Get(table command)
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
static CMD_STATUS wrCpssExMxPmPclCfgTblV1Get

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
    CPSS_EXMXPM_PCL_LOOKUP_CFG_STC    lookupCfgPtr;
    static GT_U32                   isNext = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(isNext == 1)
    {
        isNext = 0;
        /* there is no GetNext for wrapper */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    isNext = 1;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    interfaceInfoPtr.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[1];

    switch (interfaceInfoPtr.type)
    {
    case CPSS_INTERFACE_PORT_E:
        interfaceInfoPtr.devPort.devNum = (GT_U8)inArgs[2];
        interfaceInfoPtr.devPort.portNum = (GT_U8)inArgs[3];
        break;

    case CPSS_INTERFACE_TRUNK_E:
        interfaceInfoPtr.trunkId = (GT_TRUNK_ID)inArgs[4];
        break;

    case CPSS_INTERFACE_VIDX_E:
        interfaceInfoPtr.vidx = (GT_U16)inArgs[5];
        break;

    case CPSS_INTERFACE_VID_E:
        interfaceInfoPtr.vlanId = (GT_U16)inArgs[6];
        break;

    default:
        break;
    }

    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[7];
    lookupNum = (CPSS_PCL_LOOKUP_NUMBER_ENT)inArgs[8];


    result = cpssExMxPmPclCfgTblGet(devNum, &interfaceInfoPtr, direction,
                                              lookupNum, &lookupCfgPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = lookupCfgPtr.enableLookup;
    inFields[1] = lookupCfgPtr.externalLookup;
    inFields[2] = lookupCfgPtr.lookupType;
    inFields[3] = lookupCfgPtr.pclId;
    inFields[4] = lookupCfgPtr.groupKeyTypes.nonIpKey;
    inFields[5] = lookupCfgPtr.groupKeyTypes.ipv4Key;
    inFields[6] = lookupCfgPtr.groupKeyTypes.ipv6Key;


    /* pack and output table fields */
    fieldOutput(
        "%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
        inFields[3], inFields[4], inFields[5], inFields[6]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*       offsetType   - the type of offset (see CPSS_EXMXPM_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - otherwise
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclUserDefinedByteSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;
    GT_U8                                devNum;
    GT_U32                               udbIndex;
    GT_U32                               udbEntryIndex;
    CPSS_EXMXPM_PCL_UDB_TABLE_ENTRY_STC  udbEntry;
    GT_U8                                ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum        = (GT_U8)inArgs[0];
    udbEntryIndex = ((GT_U32)inArgs[2]  >> 8);
    udbIndex      = ((GT_U32)inArgs[2] & 0xFF);

    if ((23 <= udbIndex)
        || (32 <= udbEntryIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    result = cpssExMxPmPclUdbTableEntryGet(
        devNum, udbEntryIndex, &udbEntry);
    if (result != GT_OK)
    {
        for (ii = 0; (ii < 23); ii++)
        {
            udbEntry.udbCfg[ii].valid =  GT_FALSE;
            udbEntry.udbCfg[ii].offsetType = CPSS_EXMXPM_PCL_OFFSET_L2_E;
            udbEntry.udbCfg[ii].offset  = 0;
            udbEntry.udbCfg[ii].tcpUdpComparatorsEnable = GT_FALSE;
        }
    }
    /* any case, wrapper does not support other values */
    udbEntry.inlifIdEnable   = GT_FALSE;
    udbEntry.profileIdEnable = GT_FALSE;

    udbEntry.udbCfg[udbIndex].valid      =  GT_TRUE;
    udbEntry.udbCfg[udbIndex].offsetType = (GT_U32)inArgs[3];
    udbEntry.udbCfg[udbIndex].offset     = (GT_U32)inArgs[4];
    udbEntry.udbCfg[udbIndex].tcpUdpComparatorsEnable = GT_FALSE;
    if (udbEntry.udbCfg[udbIndex].offsetType == 4 /* comparator*/)
    {
        udbEntry.udbCfg[udbIndex].tcpUdpComparatorsEnable = GT_TRUE;
        udbEntry.udbCfg[udbIndex].offsetType = CPSS_EXMXPM_PCL_OFFSET_L2_E;
        udbEntry.udbCfg[udbIndex].offset     = 0;
    }

    /* call cpss api function */
    result = cpssExMxPmPclUdbTableEntrySet(
        devNum, udbEntryIndex, &udbEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*       ruleFormat   - rule format
*       udbIndex     - index of User Defined Byte to configure.
*                      See format of rules to known indexes of UDBs
*
* OUTPUTS:
*       offsetType   - the type of offset (see CPSS_EXMXPM_PCL_OFFSET_TYPE_ENT)
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - otherwise
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclUserDefinedByteGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;
    GT_U8                                devNum;
    GT_U32                               udbIndex;
    GT_U32                               udbEntryIndex;
    CPSS_EXMXPM_PCL_UDB_TABLE_ENTRY_STC  udbEntry;
    GT_U8                                ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum        = (GT_U8)inArgs[0];
    udbEntryIndex = ((GT_U32)inArgs[2]  >> 8);
    udbIndex      = ((GT_U32)inArgs[2] & 0xFF);

    if ((23 <= udbIndex)
        || (32 <= udbEntryIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    /* default values */
    for (ii = 0; (ii < 23); ii++)
    {
        udbEntry.udbCfg[ii].valid =  GT_FALSE;
        udbEntry.udbCfg[ii].offsetType = CPSS_EXMXPM_PCL_OFFSET_L2_E;
        udbEntry.udbCfg[ii].offset  = 0;
        udbEntry.udbCfg[ii].tcpUdpComparatorsEnable = GT_FALSE;
    }
    udbEntry.inlifIdEnable   = GT_FALSE;
    udbEntry.profileIdEnable = GT_FALSE;

    result = cpssExMxPmPclUdbTableEntryGet(devNum, udbEntryIndex, &udbEntry);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d",
        ((udbEntry.udbCfg[udbIndex].tcpUdpComparatorsEnable == GT_FALSE)
            ? udbEntry.udbCfg[udbIndex].offsetType : 4 /* comparator*/),
        udbEntry.udbCfg[udbIndex].offset,
        udbEntry.udbCfg[udbIndex].valid
        );
    return CMD_OK;
}

/*
 * Typedef: enum WR_CPSS_EXMXPM_PCL_UDB_USE_ENT
 *
 * Description: The UDB using mode - coded with each UDB in the wrapper's table
 *
 * Enumerations:
 *    WR_CPSS_EXMXPM_PCL_UDB_USE_REGULAR_E     - UDB used according to offset-type and offset
 *    WR_CPSS_EXMXPM_PCL_UDB_USE_COMPARATOR_E  - UDB used as TCP/UDB port comparators result
 *                                               relevant only for UDBs 5,11,14,18,20
 *    WR_CPSS_EXMXPM_PCL_UDB_USE_INLIF_E       - UDB14 is inLif[14:8], UDB13 is inLif[7:0]
 *                                               For Get - relevant only for UDB13
 *                                               For Set - understood near any UDB,
 *                                               but this treated as "regular"
 *    WR_CPSS_EXMXPM_PCL_UDB_USE_UDB_ENTRY_INDEX_E
 *                                             - UDB15 is UDB CFG table entry index[4:0]
 *                                               For Get - relevant only for UDB15
 *                                               For Set - understood near any UDB,
 *                                               but this treated as "regular"
 */
typedef enum
{
    WR_CPSS_EXMXPM_PCL_UDB_USE_REGULAR_E,
    WR_CPSS_EXMXPM_PCL_UDB_USE_COMPARATOR_E,
    WR_CPSS_EXMXPM_PCL_UDB_USE_INLIF_E,
    WR_CPSS_EXMXPM_PCL_UDB_USE_UDB_ENTRY_INDEX_E

} WR_CPSS_EXMXPM_PCL_UDB_USE_ENT;

/*
 * typedef: struct WR_CPSS_EXMXPM_PCL_UDB_CONFIG_STC
 *
 * Description:
 *      User Defined Byte configuration.
 *
 * Fields:
 *      udbIndex     - UDB Index 0..22
 *      valid        - GT_TRUE - valid, GT_FALSE - invalid
 *      offsetType   - the type of offset (see CPSS_EXMXPM_PCL_OFFSET_TYPE_ENT)
 *      offset       - The offset of the user-defined byte, in bytes,from the
 *                            place in the packet indicated by the offset type
 *
 * Comment:
 */
typedef struct
{
    GT_U32                           udbIndex;
    GT_BOOL                          valid;
    CPSS_EXMXPM_PCL_OFFSET_TYPE_ENT  offsetType;
    GT_U32                           offset;
    WR_CPSS_EXMXPM_PCL_UDB_USE_ENT   udbUse;
} WR_CPSS_EXMXPM_PCL_UDB_CONFIG_STC;

/*******************************************************************************
* utilCpssExMxPmPclUdbCmdSet
*
* DESCRIPTION:
*    Updates the "udbTableEntry" using the First/Next table Set datas
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*    linePtr    - First/Next table Set data
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
* COMMENTS:
*
*******************************************************************************/
static void utilCpssExMxPmPclUdbCmdSet
(
    IN  WR_CPSS_EXMXPM_PCL_UDB_CONFIG_STC *linePtr
)
{
    if (linePtr->udbIndex > 22)
    {
        cmdOsPrintf("*** wrong UDB index %d ***\n", linePtr->udbIndex);
        return;
    }

    udbTableEntry.udbCfg[linePtr->udbIndex].valid = linePtr->valid;
    udbTableEntry.udbCfg[linePtr->udbIndex].offsetType = linePtr->offsetType;
    udbTableEntry.udbCfg[linePtr->udbIndex].offset = linePtr->offset;
    udbTableEntry.udbCfg[linePtr->udbIndex].tcpUdpComparatorsEnable =
        (linePtr->udbUse == WR_CPSS_EXMXPM_PCL_UDB_USE_COMPARATOR_E)
         ? GT_TRUE : GT_FALSE;

    if (linePtr->udbUse == WR_CPSS_EXMXPM_PCL_UDB_USE_INLIF_E)
    {
        udbTableEntry.inlifIdEnable = GT_TRUE;
    }

    if (linePtr->udbUse == WR_CPSS_EXMXPM_PCL_UDB_USE_UDB_ENTRY_INDEX_E)
    {
        udbTableEntry.profileIdEnable = GT_TRUE;
    }
}

/*******************************************************************************
* utilCpssExMxPmPclUdbCmdGet
*
* DESCRIPTION:
*    Retrievs  the First/Next table Get datas from the "udbTableEntry"
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*    linePtr->udbIndex - the UDB index the date related to must be set
*                        by the caller
*
*
* OUTPUTS:
*    linePtr    - First/Next table Get data
*
* RETURNS:
*       None
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*       GT_HW_ERROR        - on hardware error
* COMMENTS:
*
*******************************************************************************/
static void utilCpssExMxPmPclUdbCmdGet
(
    IN  WR_CPSS_EXMXPM_PCL_UDB_CONFIG_STC *linePtr
)
{
    if (linePtr->udbIndex > 22)
    {
        cmdOsPrintf("*** wrong UDB index %d ***\n", linePtr->udbIndex);
        return;
    }

    linePtr->valid  = udbTableEntry.udbCfg[linePtr->udbIndex].valid;
    linePtr->offsetType  = udbTableEntry.udbCfg[linePtr->udbIndex].offsetType;
    linePtr->offset  = udbTableEntry.udbCfg[linePtr->udbIndex].offset;
    linePtr->udbUse =
        (udbTableEntry.udbCfg[linePtr->udbIndex].tcpUdpComparatorsEnable
         == GT_FALSE)
        ? WR_CPSS_EXMXPM_PCL_UDB_USE_REGULAR_E
        : WR_CPSS_EXMXPM_PCL_UDB_USE_COMPARATOR_E;

    if ((linePtr->udbIndex == 13)
        && (udbTableEntry.inlifIdEnable != GT_FALSE))
    {
        linePtr->udbUse = WR_CPSS_EXMXPM_PCL_UDB_USE_INLIF_E;
    }

    if ((linePtr->udbIndex == 15)
        && (udbTableEntry.profileIdEnable != GT_FALSE))
    {
        linePtr->udbUse = WR_CPSS_EXMXPM_PCL_UDB_USE_UDB_ENTRY_INDEX_E;
    }
}

/*******************************************************************************
* wrCpssExMxPmPclUdbTableEntryV1SetFirst
*
* DESCRIPTION:
*   The function configures the entry of UDB CFG table
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                - device number
*       entryIndex            - number of the entry of UDB CFG table
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_IGNORANT_E mode
*                use an appropriate PROFILE-ID table entry[4:0] value
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENHANCED_E mode
*                (PROFILE-ID table entry[2:0] << 2) + paket-type where
*                paket-type is: IPV4 - 0, IPV6 - 1, ETH_OTHER - 2, MPLS - 3
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_STANDARD_E mode
*                (PROFILE-ID table entry[1:0] << 3) + paket-type where
*                paket-type is: IPV4_TCP - 0, IPV4_UDP - 1, MPLS - 2, IPV4_FRAG - 3
*                IPV4_OTHER - 4, UDE - 5, ETH_OTHER - 6, IPV6 - 7
*       udbCfgPtr             - (pointer to)entry of UDB CFG table configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_OUT_OF_RANGE          - on UDB offset > 127
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclUdbTableEntryV1SetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    WR_CPSS_EXMXPM_PCL_UDB_CONFIG_STC tabLine;
    GT_U32                            ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* default values */
    for (ii = 0; (ii < 23); ii++)
    {
        udbTableEntry.udbCfg[ii].valid =  GT_FALSE;
        udbTableEntry.udbCfg[ii].offsetType = CPSS_EXMXPM_PCL_OFFSET_L2_E;
        udbTableEntry.udbCfg[ii].offset  = 0;
        udbTableEntry.udbCfg[ii].tcpUdpComparatorsEnable = GT_FALSE;
    }
    udbTableEntry.inlifIdEnable   = GT_FALSE;
    udbTableEntry.profileIdEnable = GT_FALSE;

    /* map input arguments to locals */
    tabLine.udbIndex   = (GT_U32)inFields[0];
    tabLine.valid      = (GT_BOOL)inFields[1];
    tabLine.offsetType = (CPSS_EXMXPM_PCL_OFFSET_TYPE_ENT)inFields[2];
    tabLine.offset     = (GT_U32)inFields[3];
    tabLine.udbUse     = (WR_CPSS_EXMXPM_PCL_UDB_USE_ENT)inFields[4];

    if (tabLine.udbIndex > 22)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    utilCpssExMxPmPclUdbCmdSet(&tabLine);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmPclUdbTableEntryV1SetNext
*
* DESCRIPTION:
*   The function configures the entry of UDB CFG table
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                - device number
*       entryIndex            - number of the entry of UDB CFG table
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_IGNORANT_E mode
*                use an appropriate PROFILE-ID table entry[4:0] value
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENHANCED_E mode
*                (PROFILE-ID table entry[2:0] << 2) + paket-type where
*                paket-type is: IPV4 - 0, IPV6 - 1, ETH_OTHER - 2, MPLS - 3
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_STANDARD_E mode
*                (PROFILE-ID table entry[1:0] << 3) + paket-type where
*                paket-type is: IPV4_TCP - 0, IPV4_UDP - 1, MPLS - 2, IPV4_FRAG - 3
*                IPV4_OTHER - 4, UDE - 5, ETH_OTHER - 6, IPV6 - 7
*       udbCfgPtr             - (pointer to)entry of UDB CFG table configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_OUT_OF_RANGE          - on UDB offset > 127
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclUdbTableEntryV1SetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    WR_CPSS_EXMXPM_PCL_UDB_CONFIG_STC tabLine;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    tabLine.udbIndex   = (GT_U32)inFields[0];
    tabLine.valid      = (GT_BOOL)inFields[1];
    tabLine.offsetType = (CPSS_EXMXPM_PCL_OFFSET_TYPE_ENT)inFields[2];
    tabLine.offset     = (GT_U32)inFields[3];
    tabLine.udbUse     = (WR_CPSS_EXMXPM_PCL_UDB_USE_ENT)inFields[4];

    if (tabLine.udbIndex > 22)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    utilCpssExMxPmPclUdbCmdSet(&tabLine);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclUdbTableEntryV1SetEnd
*
* DESCRIPTION:
*   The function configures the entry of UDB CFG table
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                - device number
*       entryIndex            - number of the entry of UDB CFG table
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_IGNORANT_E mode
*                use an appropriate PROFILE-ID table entry[4:0] value
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENHANCED_E mode
*                (PROFILE-ID table entry[2:0] << 2) + paket-type where
*                paket-type is: IPV4 - 0, IPV6 - 1, ETH_OTHER - 2, MPLS - 3
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_STANDARD_E mode
*                (PROFILE-ID table entry[1:0] << 3) + paket-type where
*                paket-type is: IPV4_TCP - 0, IPV4_UDP - 1, MPLS - 2, IPV4_FRAG - 3
*                IPV4_OTHER - 4, UDE - 5, ETH_OTHER - 6, IPV6 - 7
*       udbCfgPtr             - (pointer to)entry of UDB CFG table configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_OUT_OF_RANGE          - on UDB offset > 127
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclUdbTableEntryV1SetEnd

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    entryIndex;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    entryIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPclUdbTableEntrySet(
        devNum, entryIndex, &udbTableEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmPclUdbTableEntryV1GetFirst
*
* DESCRIPTION:
*   The function gets the entry of UDB CFG table
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                - device number
*       entryIndex            - number of the entry of UDB CFG table
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_IGNORANT_E mode
*                use an appropriate PROFILE-ID table entry[4:0] value
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENHANCED_E mode
*                (PROFILE-ID table entry[2:0] << 2) + paket-type where
*                paket-type is: IPV4 - 0, IPV6 - 1, ETH_OTHER - 2, MPLS - 3
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_STANDARD_E mode
*                (PROFILE-ID table entry[1:0] << 3) + paket-type where
*                paket-type is: IPV4_TCP - 0, IPV4_UDP - 1, MPLS - 2, IPV4_FRAG - 3
*                IPV4_OTHER - 4, UDE - 5, ETH_OTHER - 6, IPV6 - 7
*
* OUTPUTS:
*       udbCfgPtr             - (pointer to)entry of UDB CFG table configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_OUT_OF_RANGE          - on UDB offset > 127
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclUdbTableEntryV1GetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    WR_CPSS_EXMXPM_PCL_UDB_CONFIG_STC tabLine;
    GT_STATUS                         result;
    GT_U8                             devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];

    udbTableCurrentIndex = 0;

    /* map input arguments to locals */
    tabLine.udbIndex   = udbTableCurrentIndex;
    udbTableCurrentIndex ++;
    /* defaults */
    tabLine.valid      = GT_FALSE;
    tabLine.offsetType = CPSS_EXMXPM_PCL_OFFSET_L2_E;
    tabLine.offset     = 0;
    tabLine.udbUse     = WR_CPSS_EXMXPM_PCL_UDB_USE_REGULAR_E;

    /* call cpss api function */
    result = cpssExMxPmPclUdbTableEntryGet(
        devNum, tabLine.udbIndex, &udbTableEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    utilCpssExMxPmPclUdbCmdGet(&tabLine);

    fieldOutput(
        "%d%d%d%d%d",
        tabLine.udbIndex, tabLine.valid, tabLine.offsetType,
        tabLine.offset, tabLine.udbUse);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmPclUdbTableEntryV1GetNext
*
* DESCRIPTION:
*   The function gets the entry of UDB CFG table
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                - device number
*       entryIndex            - number of the entry of UDB CFG table
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_IGNORANT_E mode
*                use an appropriate PROFILE-ID table entry[4:0] value
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENHANCED_E mode
*                (PROFILE-ID table entry[2:0] << 2) + paket-type where
*                paket-type is: IPV4 - 0, IPV6 - 1, ETH_OTHER - 2, MPLS - 3
*            for CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_STANDARD_E mode
*                (PROFILE-ID table entry[1:0] << 3) + paket-type where
*                paket-type is: IPV4_TCP - 0, IPV4_UDP - 1, MPLS - 2, IPV4_FRAG - 3
*                IPV4_OTHER - 4, UDE - 5, ETH_OTHER - 6, IPV6 - 7
*
* OUTPUTS:
*       udbCfgPtr             - (pointer to)entry of UDB CFG table configuration
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_OUT_OF_RANGE          - on UDB offset > 127
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclUdbTableEntryV1GetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    WR_CPSS_EXMXPM_PCL_UDB_CONFIG_STC tabLine;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    tabLine.udbIndex   = udbTableCurrentIndex;
    udbTableCurrentIndex ++;
    /* defaults */
    tabLine.valid      = GT_FALSE;
    tabLine.offsetType = CPSS_EXMXPM_PCL_OFFSET_L2_E;
    tabLine.offset     = 0;
    tabLine.udbUse     = WR_CPSS_EXMXPM_PCL_UDB_USE_REGULAR_E;

    if (tabLine.udbIndex > 22)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    utilCpssExMxPmPclUdbCmdGet(&tabLine);

    fieldOutput(
        "%d%d%d%d%d",
        tabLine.udbIndex, tabLine.valid, tabLine.offsetType,
        tabLine.offset, tabLine.udbUse);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclInvalidUdbCmdSet
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
* APPLICABLE DEVICES:  All ExMxPm devices
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
*       GT_HW_ERROR        - on hardware error
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclInvalidUdbCmdSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_EXMXPM_UDB_ERROR_CMD_ENT  udbErrorCmd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    udbErrorCmd = (CPSS_EXMXPM_UDB_ERROR_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPclInvalidUdbCmdSet(devNum, udbErrorCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclInvalidUdbCmdGet
*
* DESCRIPTION:
*    Get the command that is applied when the policy key <User-Defined>
*    field cannot be extracted from the packet due to lack of header
*    depth (i.e, the field resides deeper than 128 bytes into the packet).
*    This command is NOT applied when the policy key <User-Defined>
*    field cannot be extracted due to the offset being relative to a layer
*    start point that does not exist in the packet. (e.g. the offset is relative
*    to the IP header but the packet is a non-IP).
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*    devNum         - device number
*
*
* OUTPUTS:
*    udbErrorCmdPtr - command applied to a packet:
*                      continue lookup, trap to CPU, hard drop or soft drop
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_BAD_PTR         - on null pointer
*       GT_FAIL            - otherwise.
*       GT_HW_ERROR        - on hardware error
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclInvalidUdbCmdGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_EXMXPM_UDB_ERROR_CMD_ENT  udbErrorCmdPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPclInvalidUdbCmdGet(devNum, &udbErrorCmdPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", udbErrorCmdPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclTwoLookupsCpuCodeResolutionSet
*
* DESCRIPTION:
*    Sets which lookup CPU Code will be taken
*    if both lookups has action commands
*    are either both TRAP or both MIRROR To CPU
*
* APPLICABLE DEVICES:  All ExMxPm devices
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
*       GT_HW_ERROR        - on hardware error
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclTwoLookupsCpuCodeResolutionSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    /* call cpss api function */
    result = GT_NOT_SUPPORTED;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    PRV_EXMXPM_PCL_STUB_PRINT_MAC();
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclTwoLookupsCpuCodeResolutionGet
*
* DESCRIPTION:
*    Gets which lookup CPU Code will be taken
*    if both lookups has action commands
*    are either both TRAP or both MIRROR To CPU
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*    devNum               - device number
*
* OUTPUTS:
*    lookupNumPtr         - lookup number from which the CPU Code is selected
*                           when action commands are either both TRAP or
*                           both MIRROR To CPU
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*       GT_HW_ERROR        - on hardware error
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclTwoLookupsCpuCodeResolutionGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    result = GT_NOT_SUPPORTED;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", 0);
    PRV_EXMXPM_PCL_STUB_PRINT_MAC();
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclTcpUdpPortComparatorSet
*
* DESCRIPTION:
*        Configure TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES: All ExMxPm devices
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
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclTcpUdpPortComparatorSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PCL_DIRECTION_ENT  direction;
    CPSS_L4_PROTOCOL_ENT  l4Protocol;
    GT_U32              entryIndex;
    CPSS_L4_PROTOCOL_PORT_TYPE_ENT  l4PortType;
    CPSS_COMPARE_OPERATOR_ENT  compareOperator;
    GT_U16              value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[1];
    l4Protocol = (CPSS_L4_PROTOCOL_ENT)inArgs[2];
    entryIndex = (GT_U32)inArgs[3];
    l4PortType = (CPSS_L4_PROTOCOL_PORT_TYPE_ENT)inArgs[4];
    compareOperator = (CPSS_COMPARE_OPERATOR_ENT)inArgs[5];
    value = (GT_U16)inArgs[6];

    /* call cpss api function */
    result = cpssExMxPmPclTcpUdpPortComparatorSet(devNum, direction, l4Protocol, entryIndex, l4PortType, compareOperator, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclTcpUdpPortComparatorGet
*
* DESCRIPTION:
*        Gets Configuration of the TCP or UDP Port Comparator entry
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    l4Protocol        - protocol, TCP or UDP
*    entryIndex        - entry index (0-7)
*
* OUTPUTS:
*    l4PortTypePtr     - TCP/UDP port type, source or destination port
*    compareOperatorPtr- compare operator FALSE, LTE, GTE, NEQ
*    valuePtr          - 16 bit value to compare with
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_FAIL            - otherwise.
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclTcpUdpPortComparatorGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PCL_DIRECTION_ENT  direction;
    CPSS_L4_PROTOCOL_ENT  l4Protocol;
    GT_U32              entryIndex;
    CPSS_L4_PROTOCOL_PORT_TYPE_ENT  l4PortTypePtr;
    CPSS_COMPARE_OPERATOR_ENT  compareOperatorPtr;
    GT_U16              valuePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PCL_DIRECTION_ENT)inArgs[1];
    l4Protocol = (CPSS_L4_PROTOCOL_ENT)inArgs[2];
    entryIndex = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmPclTcpUdpPortComparatorGet(devNum, direction, l4Protocol, entryIndex, &l4PortTypePtr, &compareOperatorPtr, &valuePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", l4PortTypePtr, compareOperatorPtr, valuePtr);
    return CMD_OK;
}

/*********************************puma 2 specific wrappers***********************************/

/*******************************************************************************
* cpssExMxPmPclUserDefinedEtherTypeSet
*
* DESCRIPTION:
*    Sets EtherType of packets to be classified
*    as User Defined Ethertype (UDE) packets.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - device number
*       etherType         - ethernet type, range 0-0xFFFF
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       OUT_OF_RANGE             - etherType > 0xFFFF
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclUserDefinedEtherTypeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 etherType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    etherType = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPclUserDefinedEtherTypeSet(devNum, etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmPclUserDefinedEtherTypeGet
*
* DESCRIPTION:
*    Gets EtherType of packets to be classified
*    as User Defined Ethertype (UDE) packets.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - device number
* OUTPUTS:
*       etherTypePtr      - (pointer to) ethernet type, range 0-0xFFFF
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - on NULL pointer value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclUserDefinedEtherTypeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 etherType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPclUserDefinedEtherTypeGet(devNum, &etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", etherType);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmPclProfileAssignmentModeSet
*
* DESCRIPTION:
*   The function sets the profile assignment mode.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       mode     - profile assignment mode
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclProfileAssignmentModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPclProfileAssignmentModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclProfileAssignmentModeGet
*
* DESCRIPTION:
*   The function gets the profile assignment mode.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*
*
* OUTPUTS:
*       modePtr  - (pointrer to) assignment mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - null pointer in parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclProfileAssignmentModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_PCL_PROFILE_ASSIGNMENT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPclProfileAssignmentModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmPclEgressLookupTypeSet
*
* DESCRIPTION:
*    Sets Egress Policy Lookup type either external TCAM or internal TCAM .
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                  - device number
*       notIpPktLookupExternal  - not IP Packets Lookup
*                                 GT_TRUE  - in external TCAM,
*                                 GT_FALSE - in internal TCAM
*       ipv4PktLookupExternal   - IPV4 Packets Lookup
*                                 GT_TRUE  - in external TCAM,
*                                 GT_FALSE - in internal TCAM
*       ipv6PktLookupExternal   - IPV6 Packets Lookup
*                                 GT_TRUE  - in external TCAM,
*                                 GT_FALSE - in internal TCAM
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclEgressLookupTypeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL notIpPktLookupExternal;
    GT_BOOL ipv4PktLookupExternal;
    GT_BOOL ipv6PktLookupExternal;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    notIpPktLookupExternal = (GT_BOOL)inArgs[1];
    ipv4PktLookupExternal = (GT_BOOL)inArgs[2];
    ipv6PktLookupExternal = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmPclEgressLookupTypeSet(devNum, notIpPktLookupExternal, ipv4PktLookupExternal, ipv6PktLookupExternal);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclEgressLookupTypeGet
*
* DESCRIPTION:
*    Gets Egress Policy Lookup type either external TCAM or internal TCAM .
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                  - device number
* OUTPUTS:
*       notIpPktLookupExternalPtr - (pointer to) not IP Packets Lookup
*                                 GT_TRUE  - in external TCAM,
*                                 GT_FALSE - in internal TCAM
*       ipv4PktLookupExternalPtr  - (pointer to) IPV4 Packets Lookup
*                                 GT_TRUE  - in external TCAM,
*                                 GT_FALSE - in internal TCAM
*       ipv6PktLookupExternalPtr  - (pointer to) IPV6 Packets Lookup
*                                 GT_TRUE  - in external TCAM,
*                                 GT_FALSE - in internal TCAM
*
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - one of the input parameters is not valid.
*       GT_BAD_PTR               - on NULL pointer value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclEgressLookupTypeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL notIpPktLookupExternal;
    GT_BOOL ipv4PktLookupExternal;
    GT_BOOL ipv6PktLookupExternal;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPclEgressLookupTypeGet(devNum, &notIpPktLookupExternal, &ipv4PktLookupExternal, &ipv6PktLookupExternal);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", notIpPktLookupExternal, ipv4PktLookupExternal, ipv6PktLookupExternal);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmPclIngressPolicyEngineCfgSet
*
* DESCRIPTION:
*   The function sets the configuration of Ingress Policy engine.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum             - device number
*       pclEngineCfgPtr - (pointer to) configuration of Ingress Policy engine
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressPolicyEngineCfgSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_PCL_INGRESS_POLICY_ENGINE_CFG_STC pclEngineCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    pclEngineCfg.lookup0_0_actionFormat=(CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT)inArgs[1];
    pclEngineCfg.lookup0_1_actionFormat=(CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT)inArgs[2];
    pclEngineCfg.lookup1_0_actionFormat=(CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT)inArgs[3];
    pclEngineCfg.lookup1_1_actionFormat=(CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT)inArgs[4];

    pclEngineCfg.externalUdbStandardKeyMode=(CPSS_EXMXPM_PCL_EXTERNAL_UDB_STANDARD_KEY_MODE_ENT)inArgs[5];
    pclEngineCfg.externalUdbExtendedKeyMode= (CPSS_EXMXPM_PCL_EXTERNAL_UDB_STANDARD_KEY_MODE_ENT)inArgs[6];

    /* call cpss api function */
    result = cpssExMxPmPclIngressPolicyEngineCfgSet(devNum, &pclEngineCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclIngressPolicyEngineCfgGet
*
* DESCRIPTION:
*   The function gets the configuration of Ingress Policy engine.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum             - device number
*
* OUTPUTS:
*       pclEngineCfgPtr - (pointer to) configuration of Ingress Policy engine
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclIngressPolicyEngineCfgGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_PCL_INGRESS_POLICY_ENGINE_CFG_STC pclEngineCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPclIngressPolicyEngineCfgGet(devNum, &pclEngineCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d",pclEngineCfg.lookup0_0_actionFormat,
                 pclEngineCfg.lookup0_1_actionFormat,pclEngineCfg.lookup1_0_actionFormat,
                 pclEngineCfg.lookup1_1_actionFormat,pclEngineCfg.externalUdbStandardKeyMode,
                 pclEngineCfg.externalUdbExtendedKeyMode);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmPclProfileIdTblSet
*
* DESCRIPTION:
*   The function sets the profile Id for port or VLAN.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       interfaceInfoPtr - (pointer to) port or VLAN interface info
*       profileId        - profile ID 0..31
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       OUT_OF_RANGE             - out of range value (profileId > 31)
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclProfileIdTblSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_INTERFACE_INFO_STC interfaceInfo;
    GT_U32 profileId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileId=(GT_U32)inArgs[1];

    interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[2];

    switch (interfaceInfo.type)
    {
    case CPSS_INTERFACE_PORT_E:
        interfaceInfo.devPort.devNum = (GT_U8)inArgs[3];
        interfaceInfo.devPort.portNum = (GT_U8)inArgs[4];
        break;

    case CPSS_INTERFACE_TRUNK_E:
        interfaceInfo.trunkId = (GT_TRUNK_ID)inArgs[5];
        break;

    case CPSS_INTERFACE_VIDX_E:
        interfaceInfo.vidx = (GT_U16)inArgs[6];
        break;

    case CPSS_INTERFACE_VID_E:
        interfaceInfo.vlanId = (GT_U16)inArgs[7];
        break;
    case CPSS_INTERFACE_DEVICE_E:
        interfaceInfo.devNum=(GT_U8)inArgs[8];
        break;

    case CPSS_INTERFACE_FABRIC_VIDX_E:
        interfaceInfo.fabricVidx=(GT_U16)inArgs[9];
        break;


    default:
        break;
    }



    /* call cpss api function */
    result = cpssExMxPmPclProfileIdTblSet(devNum, &interfaceInfo, profileId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPclProfileIdTblGet
*
* DESCRIPTION:
*   The function gets the profile Id for port or VLAN.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       interfaceInfoPtr - (pointer to) port or VLAN interface info
*
* OUTPUTS:
*       profileIdPtr     - (pointer to) profile ID 0..31
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclProfileIdTblGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_INTERFACE_INFO_STC interfaceInfo;
    GT_U32 profileId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[1];

    switch (interfaceInfo.type)
    {
    case CPSS_INTERFACE_PORT_E:
        interfaceInfo.devPort.devNum = (GT_U8)inArgs[2];
        interfaceInfo.devPort.portNum = (GT_U8)inArgs[3];
        break;

    case CPSS_INTERFACE_TRUNK_E:
        interfaceInfo.trunkId = (GT_TRUNK_ID)inArgs[4];
        break;

    case CPSS_INTERFACE_VIDX_E:
        interfaceInfo.vidx = (GT_U16)inArgs[5];
        break;

    case CPSS_INTERFACE_VID_E:
        interfaceInfo.vlanId = (GT_U16)inArgs[6];
        break;
    case CPSS_INTERFACE_DEVICE_E:
        interfaceInfo.devNum=(GT_U8)inArgs[7];
        break;

    case CPSS_INTERFACE_FABRIC_VIDX_E:
        interfaceInfo.fabricVidx=(GT_U16)inArgs[8];
        break;


    default:
        break;
    }

    /* call cpss api function */
    result = cpssExMxPmPclProfileIdTblGet(devNum, &interfaceInfo, &profileId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",profileId);

    return CMD_OK;
}

/*******************************************************************************
* wrUtilCpssExMxPmPclExtTcamUseSet  (util)
*
* DESCRIPTION:
*  The function sets the global default for multiple TCAM and Action
*  commands and tables to reffer to internal or to external TCAM.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrUtilCpssExMxPmPclExtTcamUseSet
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

    utilCpssExMxPmPclExtTcamUseSet((GT_BOOL)inArgs[0]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

   return CMD_OK;
}

/*******************************************************************************
* wrUtilCpssExMxPmPclExtTcamCfgDevNumSet  (table util)
*
* DESCRIPTION:
*  The function sets device number of current external rules.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrUtilCpssExMxPmPclExtTcamCfgDevNumSet
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

    utilCpssExMxPmPclExtTcamCfgDevNumSet((GT_U8)inArgs[0]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

   return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmPclExtConfigClear  (table command)
*
* DESCRIPTION:
*  The function initializes the table entries and sets all entries invalid.
*
*
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
static CMD_STATUS utilCpssExMxPmPclExtConfigClear
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32          index = 0;
    firstFree_ExtTcamPcl = 0;
    while (index < (EXT_TCAM_CFG_TABLE_ZISE - 1))
    {
        extTcamConfigPclTable[index].valid = GT_FALSE;
        extTcamConfigPclTable[index].nextFree = index + 1;
        index++;
    }
    extTcamConfigPclTable[index].nextFree = INVALID_RULE_INDEX;
    extTcamConfigPclTable[index].valid = GT_FALSE;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmPclExtConfigSet
*
* DESCRIPTION:
*       This function sets external TCAM table entry (wrapper level)
*
*
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
static CMD_STATUS wrCpssExMxPmPclExtConfigSet
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

    if (firsRun_ExtTcamPcl)
    {
        utilCpssExMxPmPclExtConfigClear(inArgs,inFields,numFields,outArgs);
        firsRun_ExtTcamPcl = GT_FALSE;
    }

    wrCpssExMxPmPclExtConfigIndexGet(devNum_ExtTcamPcl, ruleStartIndex, &index);

    /* the rule wasn't previously in action table */
    if (index == INVALID_RULE_INDEX)
    {
        if(firsRun_ExtTcamPcl == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
            return CMD_AGENT_ERROR;
        }

        index = firstFree_ExtTcamPcl;
        firstFree_ExtTcamPcl = extTcamConfigPclTable[firstFree_ExtTcamPcl].nextFree;
    }

    /* map input arguments to locals */

    extTcamConfigPclTable[index].valid = GT_TRUE;
    extTcamConfigPclTable[index].ruleStartIndex = ruleStartIndex;
    extTcamConfigPclTable[index].devNum = devNum_ExtTcamPcl;
    extTcamConfigPclTable[index].ruleSize=(CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT)inFields[1];
    extTcamConfigPclTable[index].tcamId=(GT_U32)inFields[2];
    extTcamConfigPclTable[index].ageEnable=(GT_BOOL)inFields[3];
    extTcamConfigPclTable[index].actionStartIndex=(GT_U32)inFields[4];
    extTcamConfigPclTable[index].memoryType=(CPSS_EXMXPM_CONTROL_MEMORY_ENT)inFields[5];

  /* pack output arguments to galtis string */
   galtisOutput(outArgs, GT_OK, "");

   return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmPclExtConfigGet
*
* DESCRIPTION:
*       This function gets external TCAM table entry (wrapper level)
*
*
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
static CMD_STATUS wrCpssExMxPmPclExtConfigGet
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


    while ((pm2ExtTcamPclIndex < EXT_TCAM_CFG_TABLE_ZISE)

          && ((extTcamConfigPclTable[pm2ExtTcamPclIndex].valid == GT_FALSE)
              || (extTcamConfigPclTable[pm2ExtTcamPclIndex].devNum
                  != devNum_ExtTcamPcl)))
        pm2ExtTcamPclIndex++;

    if (pm2ExtTcamPclIndex >= EXT_TCAM_CFG_TABLE_ZISE)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
   inFields[0]=extTcamConfigPclTable[pm2ExtTcamPclIndex].ruleStartIndex;
   inFields[1]=extTcamConfigPclTable[pm2ExtTcamPclIndex].ruleSize;
   inFields[2]=extTcamConfigPclTable[pm2ExtTcamPclIndex].tcamId;
   inFields[3]=extTcamConfigPclTable[pm2ExtTcamPclIndex].ageEnable;
   inFields[4]=extTcamConfigPclTable[pm2ExtTcamPclIndex].actionStartIndex;
   inFields[5]=extTcamConfigPclTable[pm2ExtTcamPclIndex].memoryType;

   pm2ExtTcamPclIndex++;

   fieldOutput("%d%d%d%d%d%d",
                inFields[0],  inFields[1],   inFields[2],   inFields[3],
                inFields[4],  inFields[5]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmPclExtConfigGetFirst
*
* DESCRIPTION:
*       This function gets first external TCAM table entry (wrapper level)
*
*
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
static CMD_STATUS wrCpssExMxPmPclExtConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    pm2ExtTcamPclIndex = 0;

    return wrCpssExMxPmPclExtConfigGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* prvWrSetOutlifInfo  (table command)
*
* DESCRIPTION:
*  The function initializes CPSS_EXMXPM_OUTLIF_INFO_STC
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       inFields    - Galtis iput stream
*       index       - inFields offset
*
* OUTPUTS:
*       outlifPtr   - interface to set
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static void prvWrSetOutlifInfo
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_U32 index,
    OUT  CPSS_EXMXPM_OUTLIF_INFO_STC    *outlifPtr
)
{
    outlifPtr->outlifType=(CPSS_EXMXPM_OUTLIF_TYPE_ENT)inFields[index];

    if(outlifPtr->outlifType==CPSS_EXMXPM_OUTLIF_TYPE_LL_E)
       outlifPtr->outlifPointer.arpPtr=(GT_U32)inFields[index+1];

    if(outlifPtr->outlifType==CPSS_EXMXPM_OUTLIF_TYPE_DIT_E)
       outlifPtr->outlifPointer.ditPtr=(GT_U32)inFields[index+2];

    if(outlifPtr->outlifType==CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E)
    {
      outlifPtr->outlifPointer.tunnelStartPtr.passengerPacketType=(CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[index+3];
      outlifPtr->outlifPointer.tunnelStartPtr.ptr=(GT_U32)inFields[index+4];
    }

    outlifPtr->interfaceInfo.type=(CPSS_INTERFACE_TYPE_ENT)inFields[index+5] ;

    switch(outlifPtr->interfaceInfo.type)
    {
    case CPSS_INTERFACE_PORT_E:
        outlifPtr->interfaceInfo.devPort.devNum = (GT_U8)inFields[index+6];
        outlifPtr->interfaceInfo.devPort.portNum = (GT_U8)inFields[index+7];

        CONVERT_DEV_PORT_DATA_MAC(outlifPtr->interfaceInfo.devPort.devNum,
            outlifPtr->interfaceInfo.devPort.portNum);

        break;

    case CPSS_INTERFACE_TRUNK_E:
        outlifPtr->interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[index+8];
        break;

    case CPSS_INTERFACE_VIDX_E:
        outlifPtr->interfaceInfo.vidx = (GT_U16)inFields[index+9];
        break;

    case CPSS_INTERFACE_VID_E:
        outlifPtr->interfaceInfo.vlanId = (GT_U16)inFields[index+10];
        break;
   case CPSS_INTERFACE_DEVICE_E:
        outlifPtr->interfaceInfo.devNum= (GT_U8)inFields[index+11];
        break;

    default:
        break;
    }


}
/*******************************************************************************
* prvWrGetOutlifInfo  (table command)
*
* DESCRIPTION:
*  The function puts CPSS_EXMXPM_OUTLIF_INFO_STC into inFields
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       inFields    - Galtis iput stream
*       index       - inFields offset
*       outlifPtr   - interface to set
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
*
*******************************************************************************/
static void prvWrGetOutlifInfo
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_U32 index,
    IN  CPSS_EXMXPM_OUTLIF_INFO_STC    *outlifPtr
)
{
    inFields[index]=outlifPtr->outlifType;

    if(outlifPtr->outlifType==CPSS_EXMXPM_OUTLIF_TYPE_LL_E)
       inFields[index+1]=outlifPtr->outlifPointer.arpPtr;

    if(outlifPtr->outlifType==CPSS_EXMXPM_OUTLIF_TYPE_DIT_E)
       inFields[index+2]=outlifPtr->outlifPointer.ditPtr;

    if(outlifPtr->outlifType==CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E)
    {
      inFields[index+3]=outlifPtr->outlifPointer.tunnelStartPtr.passengerPacketType;
      inFields[index+4]=outlifPtr->outlifPointer.tunnelStartPtr.ptr;
    }

    inFields[index+5]=outlifPtr->interfaceInfo.type ;

    switch(outlifPtr->interfaceInfo.type)
    {
    case CPSS_INTERFACE_PORT_E:
        inFields[index+6]=outlifPtr->interfaceInfo.devPort.devNum ;
        inFields[index+7]=outlifPtr->interfaceInfo.devPort.portNum ;

        break;

    case CPSS_INTERFACE_TRUNK_E:
        inFields[index+8]=outlifPtr->interfaceInfo.trunkId ;
        break;

    case CPSS_INTERFACE_VIDX_E:
        inFields[index+9]=outlifPtr->interfaceInfo.vidx ;
        break;

    case CPSS_INTERFACE_VID_E:
        inFields[index+10]=outlifPtr->interfaceInfo.vlanId;
        break;
   case CPSS_INTERFACE_DEVICE_E:
        inFields[index+11]=outlifPtr->interfaceInfo.devNum ;
        break;

    default:
        break;
    }


}
/*******************************************************************************
* prvCpssExMxPm2PclActionSet_INGRESS_STANDARD_E  (table command)
*
* DESCRIPTION:
*  The function initializes the table entry for
*      CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_STC
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       index       - Index of the rule in the internal or external TCAM.
*       tablePtr    - pointer to action table(wrapper level)
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
*
*******************************************************************************/
static void prvCpssExMxPm2PclActionSet_INGRESS_STANDARD_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_U32 index,
    IN  WR_PCL_ACTION_STC    *tablePtr
)
{
    CPSS_EXMXPM_PCL_ACTION_UNT          *actionPtr;

    tablePtr[index].actionFormat=CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E;
    actionPtr = &(tablePtr[index].actionEntry);
    actionPtr->ingressStandard.pktCmd=(CPSS_PACKET_CMD_ENT)inFields[1];
    actionPtr->ingressStandard.actionStop=(GT_BOOL)inFields[2];

    /*mirror*/
    actionPtr->ingressStandard.mirror.cpuCode=(CPSS_NET_RX_CPU_CODE_ENT)inFields[3];
    actionPtr->ingressStandard.mirror.ingressPolicyAnalyzerNumber = inFields[4];

    /*qos*/

      /*dp*/
    actionPtr->ingressStandard.qos.qosParamsModify.modifyDp=(GT_BOOL)inFields[5];
    actionPtr->ingressStandard.qos.qosParams.dp=(CPSS_DP_LEVEL_ENT)inFields[6];
      /*up*/
    actionPtr->ingressStandard.qos.qosParamsModify.modifyUp=(GT_BOOL)inFields[7];
    actionPtr->ingressStandard.qos.qosParams.up=(GT_U32)inFields[8];
      /*dscp*/
    actionPtr->ingressStandard.qos.qosParamsModify.modifyDscp=(GT_BOOL)inFields[9];
    actionPtr->ingressStandard.qos.qosParams.dscp=(GT_U32)inFields[10];
       /*exp*/
    actionPtr->ingressStandard.qos.qosParamsModify.modifyExp=(GT_BOOL)inFields[11];
    actionPtr->ingressStandard.qos.qosParams.exp=(GT_U32)inFields[12];
      /*tc*/
    actionPtr->ingressStandard.qos.qosParamsModify.modifyTc=(GT_BOOL)inFields[13];
    actionPtr->ingressStandard.qos.qosParams.tc=(GT_U32)inFields[14];
    /*attribute*/
    actionPtr->ingressStandard.qos.qosPrecedence=(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[15];

    /*vlan*/
    actionPtr->ingressStandard.vlan.precedence=(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[16];
    actionPtr->ingressStandard.vlan.nestedVlan=(GT_BOOL)inFields[17];
    actionPtr->ingressStandard.vlan.vlanCmd=(CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[18];
    actionPtr->ingressStandard.vlan.vidOrInLif=(GT_U32)inFields[19];

    /*Redirect*/
    actionPtr->ingressStandard.redirect.redirectCmd=(CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_ENT)inFields[20];

    if(actionPtr->ingressStandard.redirect.redirectCmd==CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
       actionPtr->ingressStandard.redirect.data.ipNextHopEntryIndex=(GT_U32)inFields[21];
    else if (actionPtr->ingressStandard.redirect.redirectCmd==CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
     {
      actionPtr->ingressStandard.redirect.data.outIf.vntL2Echo=(GT_BOOL)inFields[22];
      prvWrSetOutlifInfo(inFields,23,&(actionPtr->ingressStandard.redirect.data.outIf.outLifInfo));
     }

    /*policer*/

     actionPtr->ingressStandard.policer.activateCounter=(GT_BOOL)inFields[35];
     actionPtr->ingressStandard.policer.activateMeter=(GT_BOOL)inFields[36];
     actionPtr->ingressStandard.policer.policerPtr=(GT_U32)inFields[37];

     /*source id*/
     actionPtr->ingressStandard.sourceId.assignSourceId=(GT_BOOL)inFields[38];
     actionPtr->ingressStandard.sourceId.sourceIdValue=(GT_U32)inFields[39];

     /*fabric*/
     actionPtr->ingressStandard.fabric.flowId=(GT_U32)inFields[40];

}
/*******************************************************************************
* prvCpssExMxPm2PclActionSet_INGRESS_REDUCED_E  (table command)
*
* DESCRIPTION:
*  The function initializes the table entry for
*      CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_STC
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       index       - Index of the rule in the internal or external TCAM.
*       tablePtr    - pointer to action table(wrapper level)
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
*
*******************************************************************************/
static void prvCpssExMxPm2PclActionSet_INGRESS_REDUCED_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_U32 index,
    IN  WR_PCL_ACTION_STC    *tablePtr
)
{
    CPSS_EXMXPM_PCL_ACTION_UNT          *actionPtr;

    tablePtr[index].actionFormat=CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E;
    actionPtr = &(tablePtr[index].actionEntry);

    actionPtr->ingressReduced.pktCmd=(CPSS_PACKET_CMD_ENT)inFields[1];

     /*mirror*/
    actionPtr->ingressReduced.mirror.cpuCode=(CPSS_NET_RX_CPU_CODE_ENT)inFields[2];
    actionPtr->ingressReduced.mirror.mirrorToIngressPolicyAnalyzer = (GT_BOOL)inFields[3];

    /*qos*/

      /*dp*/
    actionPtr->ingressReduced.qos.qosParamsModify.modifyDp=(GT_BOOL)inFields[4];
    actionPtr->ingressReduced.qos.qosParams.dp=(CPSS_DP_LEVEL_ENT)inFields[5];
      /*up*/
    actionPtr->ingressReduced.qos.qosParamsModify.modifyUp=(GT_BOOL)inFields[6];
    actionPtr->ingressReduced.qos.qosParams.up=(GT_U32)inFields[7];
      /*dscp*/
    actionPtr->ingressReduced.qos.qosParamsModify.modifyDscp=(GT_BOOL)inFields[8];
    actionPtr->ingressReduced.qos.qosParams.dscp=(GT_U32)inFields[9];
       /*exp*/
    actionPtr->ingressReduced.qos.qosParamsModify.modifyExp=(GT_BOOL)inFields[10];
    actionPtr->ingressReduced.qos.qosParams.exp=(GT_U32)inFields[11];
      /*tc*/
    actionPtr->ingressReduced.qos.qosParamsModify.modifyTc=(GT_BOOL)inFields[12];
    actionPtr->ingressReduced.qos.qosParams.tc=(GT_U32)inFields[13];
    /*attribute*/
    actionPtr->ingressReduced.qos.qosPrecedence=(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[14];

    /*vlan*/
    actionPtr->ingressReduced.vlan.precedence=(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[15];
    actionPtr->ingressReduced.vlan.nestedVlan=(GT_BOOL)inFields[16];
    actionPtr->ingressReduced.vlan.vlanCmd=(CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[17];
    actionPtr->ingressReduced.vlan.vidOrInLif=(GT_U32)inFields[18];

    /*Redirect*/
    actionPtr->ingressReduced.redirect.redirectCmd=(CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_ENT)inFields[19];

    if(actionPtr->ingressReduced.redirect.redirectCmd==CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
       actionPtr->ingressReduced.redirect.data.ipNextHopEntryIndex=(GT_U32)inFields[20];
    else if (actionPtr->ingressReduced.redirect.redirectCmd==CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
     {
      actionPtr->ingressReduced.redirect.data.outIf.vntL2Echo=(GT_BOOL)inFields[21];
      prvWrSetOutlifInfo(inFields,22,&(actionPtr->ingressReduced.redirect.data.outIf.outLifInfo));
     }

    /*policer*/

     actionPtr->ingressReduced.policer.activateCounter=(GT_BOOL)inFields[34];
     actionPtr->ingressReduced.policer.activateMeter=(GT_BOOL)inFields[35];
     actionPtr->ingressReduced.policer.policerPtr=(GT_U32)inFields[36];

     /*source id*/
     actionPtr->ingressReduced.sourceId.assignSourceId=(GT_BOOL)inFields[37];
     actionPtr->ingressReduced.sourceId.sourceIdValue=(GT_U32)inFields[38];



}
/*******************************************************************************
* prvCpssExMxPm2PclActionSet__EGRESS_E  (table command)
*
* DESCRIPTION:
*  The function initializes the table entry for
*      CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_STC
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       index       - Index of the rule in the internal or external TCAM.
*       tablePtr    - pointer to action table(wrapper level)
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
*
*******************************************************************************/
static void prvCpssExMxPm2PclActionSet__EGRESS_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_U32 index,
    IN  WR_PCL_ACTION_STC    *tablePtr
)
{
    CPSS_EXMXPM_PCL_ACTION_UNT          *actionPtr;

    tablePtr[index].actionFormat=CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E;
    actionPtr = &(tablePtr[index].actionEntry);

    actionPtr->egress.pktCmd=(CPSS_PACKET_CMD_ENT)inFields[1];
    actionPtr->egress.actionStop=(GT_BOOL)inFields[2];

    /*qos*/

      /*dp*/
    actionPtr->egress.qos.qosParamsModify.modifyDp=(GT_BOOL)inFields[3];
    actionPtr->egress.qos.qosParams.dp=(CPSS_DP_LEVEL_ENT)inFields[4];
      /*up*/
    actionPtr->egress.qos.qosParamsModify.modifyUp=(GT_BOOL)inFields[5];
    actionPtr->egress.qos.qosParams.up=(GT_U32)inFields[6];
      /*dscp*/
    actionPtr->egress.qos.qosParamsModify.modifyDscp=(GT_BOOL)inFields[7];
    actionPtr->egress.qos.qosParams.dscp=(GT_U32)inFields[8];
       /*exp*/
    actionPtr->egress.qos.qosParamsModify.modifyExp=(GT_BOOL)inFields[9];
    actionPtr->egress.qos.qosParams.exp=(GT_U32)inFields[10];
      /*tc*/
    actionPtr->egress.qos.qosParamsModify.modifyTc=(GT_BOOL)inFields[11];
    actionPtr->egress.qos.qosParams.tc=(GT_U32)inFields[12];
    /*attribute*/
    actionPtr->egress.qos.qosPrecedence=(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[13];

    /*vlan*/
    actionPtr->egress.vlan.precedence=(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[14];
    actionPtr->egress.vlan.nestedVlan=(GT_BOOL)inFields[15];
    actionPtr->egress.vlan.vlanCmd=(CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[16];
    actionPtr->egress.vlan.vidOrInLif=(GT_U32)inFields[17];


    /*policer*/

     actionPtr->egress.policer.activateCounter=(GT_BOOL)inFields[18];
     actionPtr->egress.policer.activateMeter=(GT_BOOL)inFields[19];
     actionPtr->egress.policer.policerPtr=(GT_U32)inFields[20];





}
/*******************************************************************************
* cpssExMxPmPclRuleActionSet
*
* DESCRIPTION:
*   The function writes the Rule Action to internal or external Action table
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamType       - TCAM type: internal or external
*       actionInfoPtr  - (pointer to) the Action Layout
*       actionFormat   - format of the Rule action (standard, reduced, egress).
*       actionPtr      - (pointer to) Policy rule action that applied on packet
*                         if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*********************************************************************/
static CMD_STATUS wrCpssExMxPm2PclActionSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)

{   GT_STATUS                           result=GT_OK;
    GT_U8                               devNum;
    CPSS_EXMXPM_TCAM_ACTION_INFO_UNT     actionInfo;
    GT_U32                              index;
    GT_U32                              ruleIndex;
    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT   actionType;
    CPSS_EXMXPM_TCAM_TYPE_ENT           tcamType;
    CPSS_EXMXPM_PCL_ACTION_UNT          *actionPtr;
    WR_PCL_ACTION_STC                   *tablePtr=NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tcamType=(CPSS_EXMXPM_TCAM_TYPE_ENT)inArgs[2];

    devNum  =       (GT_U8)inArgs[0];

    if(tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
    {
        actionInfo.internalTcamRuleStartIndex=(GT_U32)inArgs[1];
        ruleIndex=actionInfo.internalTcamRuleStartIndex;
    }
    else
    {
        /*bind extTcam configs*/
        ruleIndex=(GT_U32)inArgs[1];
        wrCpssExMxPmPclExtConfigIndexGet(devNum, ruleIndex,&index);

        if (index ==INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nExternal Tcam configs  not set\n");

            return CMD_AGENT_ERROR;
        }

        actionInfo.externalTcamInfo.actionInfo.actionStartIndex=extTcamConfigPclTable[index].actionStartIndex;
        actionInfo.externalTcamInfo.actionInfo.memoryType=extTcamConfigPclTable[index].memoryType;
     }/*end of else*/

    utilCpssExMxPm2PclActionTableInitOrCheck(tcamType);

     if (tcamType == CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
     {

        wrCpssExMxPmPclActionTableIndexGet((GT_U8)inArgs[0], ruleIndex, actionIntTable, &index);

        /* the rule wasn't previously in action table */
        if (index == INVALID_RULE_INDEX)
        {
            if(firstIntFree == INVALID_RULE_INDEX)
            {
                /* pack output arguments to galtis string */
                galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
                return CMD_AGENT_ERROR;
            }

            index = firstIntFree;
            firstIntFree = actionIntTable[firstIntFree].nextFree;
        }
        tablePtr=actionIntTable;
      }


     if (tcamType==CPSS_EXMXPM_TCAM_TYPE_EXTERNAL_E)
     {

        wrCpssExMxPmPclActionTableIndexGet((GT_U8)inArgs[0], ruleIndex,actionExtTable, &index);

        /* the rule wasn't previously in action table */
        if (index == INVALID_RULE_INDEX)
        {
            if(firstExtFree == INVALID_RULE_INDEX)
            {
                /* pack output arguments to galtis string */
                galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
                return CMD_AGENT_ERROR;
            }

            index = firstExtFree;
            firstExtFree = actionExtTable[firstExtFree].nextFree;
        }
        tablePtr=actionExtTable;
      }


   /* map input arguments to locals */

    tablePtr[index].valid = GT_TRUE;
    tablePtr[index].ruleIndex = ruleIndex;
    tablePtr[index].devNum = devNum;
    actionPtr = &(tablePtr[index].actionEntry);
    actionType=(CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT)inArgs[4];


   /*swtich by action type*/

    switch(actionType){

    case CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E:

        prvCpssExMxPm2PclActionSet_INGRESS_STANDARD_E(inFields,index,tablePtr);
        break;
    case CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E:

        prvCpssExMxPm2PclActionSet_INGRESS_REDUCED_E(inFields,index,tablePtr);
        break;
    case CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E:

        prvCpssExMxPm2PclActionSet__EGRESS_E(inFields,index,tablePtr);
        break;
    default:
        break;

    }

    if(inFields[0])

  /* call cpss api function */
   result = cpssExMxPmPclRuleActionSet(devNum,tcamType,&actionInfo,actionType,actionPtr);

   /* pack output arguments to galtis string */
   galtisOutput(outArgs, result, "");

   return CMD_OK;
}

/*******************************************************************************
* prvCpssExMxPm2PclActionGet_INGRESS_STANDARD_E  (table command)
*
* DESCRIPTION:
*  The function gets the table entry for
*      CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_STC
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*      inFields
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
*
*******************************************************************************/
static void prvCpssExMxPm2PclActionGet_INGRESS_STANDARD_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_EXMXPM_PCL_ACTION_UNT          *actionPtr;
    actionPtr = &wrPclEntry;

    inFields[1]=actionPtr->ingressStandard.pktCmd;
    inFields[2]=actionPtr->ingressStandard.actionStop;

    /*mirror*/
    inFields[3]=actionPtr->ingressStandard.mirror.cpuCode;
    inFields[4]=actionPtr->ingressStandard.mirror.ingressPolicyAnalyzerNumber;

    /*qos*/

      /*dp*/
    inFields[5]=actionPtr->ingressStandard.qos.qosParamsModify.modifyDp;
    inFields[6]=actionPtr->ingressStandard.qos.qosParams.dp;
      /*up*/
    inFields[7]=actionPtr->ingressStandard.qos.qosParamsModify.modifyUp;
    inFields[8]=actionPtr->ingressStandard.qos.qosParams.up;
      /*dscp*/
    inFields[9]=actionPtr->ingressStandard.qos.qosParamsModify.modifyDscp;
    inFields[10]=actionPtr->ingressStandard.qos.qosParams.dscp;
       /*exp*/
    inFields[11]=actionPtr->ingressStandard.qos.qosParamsModify.modifyExp;
    inFields[12]=actionPtr->ingressStandard.qos.qosParams.exp;
      /*tc*/
    inFields[13]=actionPtr->ingressStandard.qos.qosParamsModify.modifyTc;
    inFields[14]=actionPtr->ingressStandard.qos.qosParams.tc;
    /*attribute*/
    inFields[15]=actionPtr->ingressStandard.qos.qosPrecedence;

    /*vlan*/
    inFields[16]=actionPtr->ingressStandard.vlan.precedence;
    inFields[17]=actionPtr->ingressStandard.vlan.nestedVlan;
    inFields[18]=actionPtr->ingressStandard.vlan.vlanCmd;
    inFields[19]=actionPtr->ingressStandard.vlan.vidOrInLif;

    /*Redirect*/
    inFields[20]=actionPtr->ingressStandard.redirect.redirectCmd;

    if(actionPtr->ingressStandard.redirect.redirectCmd==CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
       inFields[21]=actionPtr->ingressStandard.redirect.data.ipNextHopEntryIndex;
    else if (actionPtr->ingressStandard.redirect.redirectCmd==CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
     {
      inFields[22]=actionPtr->ingressStandard.redirect.data.outIf.vntL2Echo;
      prvWrGetOutlifInfo(inFields,23,&(actionPtr->ingressStandard.redirect.data.outIf.outLifInfo));
     }

    /*policer*/

     inFields[35]=actionPtr->ingressStandard.policer.activateCounter;
     inFields[36]=actionPtr->ingressStandard.policer.activateMeter;
     inFields[37]=actionPtr->ingressStandard.policer.policerPtr;

     /*source id*/
     inFields[38]=actionPtr->ingressStandard.sourceId.assignSourceId;
     inFields[39]=actionPtr->ingressStandard.sourceId.sourceIdValue;

     /*fabric*/
     inFields[40]=actionPtr->ingressStandard.fabric.flowId;

     fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",

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
                inFields[40]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 0);

}
/*******************************************************************************
* prvCpssExMxPm2PclActionGet_INGRESS_REDUCED_E  (table command)
*
* DESCRIPTION:
*  The function gets the table entry for
*      CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_STC
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*      inFields
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
*
*******************************************************************************/
static void prvCpssExMxPm2PclActionGet_INGRESS_REDUCED_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]

)
{
    CPSS_EXMXPM_PCL_ACTION_UNT          *actionPtr;
    actionPtr = &wrPclEntry;

    inFields[1]=actionPtr->ingressReduced.pktCmd;

     /*mirror*/
    inFields[2]=actionPtr->ingressReduced.mirror.cpuCode;
    inFields[3]=actionPtr->ingressReduced.mirror.mirrorToIngressPolicyAnalyzer;

    /*qos*/

      /*dp*/
    inFields[4]=actionPtr->ingressReduced.qos.qosParamsModify.modifyDp;
    inFields[5]=actionPtr->ingressReduced.qos.qosParams.dp;
      /*up*/
    inFields[6]=actionPtr->ingressReduced.qos.qosParamsModify.modifyUp;
    inFields[7]=actionPtr->ingressReduced.qos.qosParams.up;
      /*dscp*/
    inFields[8]=actionPtr->ingressReduced.qos.qosParamsModify.modifyDscp;
    inFields[9]=actionPtr->ingressReduced.qos.qosParams.dscp;
       /*exp*/
    inFields[10]=actionPtr->ingressReduced.qos.qosParamsModify.modifyExp;
    inFields[11]=actionPtr->ingressReduced.qos.qosParams.exp;
      /*tc*/
    inFields[12]=actionPtr->ingressReduced.qos.qosParamsModify.modifyTc;
    inFields[13]=actionPtr->ingressReduced.qos.qosParams.tc;
    /*attribute*/
    inFields[14]=actionPtr->ingressReduced.qos.qosPrecedence;

    /*vlan*/
    inFields[15]=actionPtr->ingressReduced.vlan.precedence;
    inFields[16]=actionPtr->ingressReduced.vlan.nestedVlan;
    inFields[17]=actionPtr->ingressReduced.vlan.vlanCmd;
    inFields[18]=actionPtr->ingressReduced.vlan.vidOrInLif;

    /*Redirect*/
    inFields[19]=actionPtr->ingressReduced.redirect.redirectCmd;

    if(actionPtr->ingressReduced.redirect.redirectCmd==CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
       inFields[20]=actionPtr->ingressReduced.redirect.data.ipNextHopEntryIndex;
    else if (actionPtr->ingressReduced.redirect.redirectCmd==CPSS_EXMXPM_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
     {
      inFields[21]=actionPtr->ingressReduced.redirect.data.outIf.vntL2Echo;
      prvWrGetOutlifInfo(inFields,22,&(actionPtr->ingressReduced.redirect.data.outIf.outLifInfo));
     }

    /*policer*/

     inFields[34]=actionPtr->ingressReduced.policer.activateCounter;
     inFields[35]=actionPtr->ingressReduced.policer.activateMeter;
     inFields[36]=actionPtr->ingressReduced.policer.policerPtr;

     /*source id*/
     inFields[37]=actionPtr->ingressReduced.sourceId.assignSourceId;
     inFields[38]=actionPtr->ingressReduced.sourceId.sourceIdValue;

     fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",

                inFields[0],  inFields[1],   inFields[2],   inFields[3],
                inFields[4],  inFields[5],   inFields[6],   inFields[7],
                inFields[8],  inFields[9],   inFields[10],  inFields[11],
                inFields[12], inFields[13],  inFields[14],  inFields[15],
                inFields[16], inFields[17],  inFields[18],  inFields[19],
                inFields[20], inFields[21],  inFields[22],  inFields[23],
                inFields[24], inFields[25],  inFields[26],  inFields[27],
                inFields[28], inFields[29],  inFields[30],  inFields[31],
                inFields[32], inFields[33],  inFields[34],  inFields[35],
                inFields[36], inFields[37],  inFields[38]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 1);



}

/*******************************************************************************
* prvCpssExMxPm2PclActionGet__EGRESS_E  (table command)
*
* DESCRIPTION:
*  The function gets the table entry for
*      CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_STC
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       inFields
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
*
*******************************************************************************/
static void prvCpssExMxPm2PclActionGet__EGRESS_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]

)
{
    CPSS_EXMXPM_PCL_ACTION_UNT          *actionPtr;

    actionPtr = &wrPclEntry;

    inFields[1]=actionPtr->egress.pktCmd;
    inFields[2]=actionPtr->egress.actionStop;

    /*qos*/

      /*dp*/
    inFields[3]=actionPtr->egress.qos.qosParamsModify.modifyDp;
    inFields[4]=actionPtr->egress.qos.qosParams.dp;
      /*up*/
    inFields[5]=actionPtr->egress.qos.qosParamsModify.modifyUp;
    inFields[6]=actionPtr->egress.qos.qosParams.up;
      /*dscp*/
    inFields[7]=actionPtr->egress.qos.qosParamsModify.modifyDscp;
    inFields[8]=actionPtr->egress.qos.qosParams.dscp;
       /*exp*/
    inFields[9]=actionPtr->egress.qos.qosParamsModify.modifyExp;
    inFields[10]=actionPtr->egress.qos.qosParams.exp;
      /*tc*/
    inFields[11]=actionPtr->egress.qos.qosParamsModify.modifyTc;
    inFields[12]=actionPtr->egress.qos.qosParams.tc;
    /*attribute*/
    inFields[13]=actionPtr->egress.qos.qosPrecedence;

    /*vlan*/
    inFields[14]=actionPtr->egress.vlan.precedence;
    inFields[15]=actionPtr->egress.vlan.nestedVlan;
    inFields[16]=actionPtr->egress.vlan.vlanCmd;
    inFields[17]=actionPtr->egress.vlan.vidOrInLif;


    /*policer*/

     inFields[18]=actionPtr->egress.policer.activateCounter;
     inFields[19]=actionPtr->egress.policer.activateMeter;
     inFields[20]=actionPtr->egress.policer.policerPtr;


     fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",

                inFields[0],  inFields[1],   inFields[2],   inFields[3],
                inFields[4],  inFields[5],   inFields[6],   inFields[7],
                inFields[8],  inFields[9],   inFields[10],  inFields[11],
                inFields[12], inFields[13],  inFields[14],  inFields[15],
                inFields[16], inFields[17],  inFields[18],  inFields[19],
                inFields[20]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 2);




}
/*******************************************************************************
* wrcpssExMxPm2PclRuleActionGetEntry
*
* DESCRIPTION:
*   The function reads the Rule Action from internal or external Action table
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamType       - TCAM type: internal or external
*       actionInfoPtr  - (pointer to) the Action Layout
*       actionFormat   - format of the Rule action (standard, reduced, egress).
*
* OUTPUTS:
*       actionPtr      - (pointer to) Policy rule action that applied on packet
*                         if packet's search key matched with masked pattern.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPm2PclRuleActionGetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   GT_STATUS                           result=GT_OK;
    GT_U8                               devNum;
    GT_U32                              ruleIndex;
    GT_U32                              index;
    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT   typeToRefresh;
    CPSS_EXMXPM_TCAM_TYPE_ENT           tcamType;
    CPSS_EXMXPM_TCAM_ACTION_INFO_UNT    actionInfo;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(inFields,0,CMD_MAX_FIELDS);

    devNum=(GT_U8)inArgs[0];
    tcamType=(CPSS_EXMXPM_TCAM_TYPE_ENT)inArgs[2];
    typeToRefresh=(CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT)inArgs[3];


    if(tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
     {
      actionInfo.internalTcamRuleStartIndex=(GT_U32)inArgs[1];
      ruleIndex=actionInfo.internalTcamRuleStartIndex;
      }
    else
    {
        /*bind extTcam configs*/
        ruleIndex=(GT_U32)inArgs[1];
        wrCpssExMxPmPclExtConfigIndexGet(devNum, ruleIndex, &index);

            if (index ==INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nExternal Tcam configs  not set\n");

            return CMD_AGENT_ERROR;
        }

            actionInfo.externalTcamInfo.actionInfo.actionStartIndex=extTcamConfigPclTable[index].actionStartIndex;
            actionInfo.externalTcamInfo.actionInfo.memoryType=extTcamConfigPclTable[index].memoryType;
     }/*end of else*/

    /* call cpss api function */
     result = cpssExMxPmPclRuleActionGet(devNum,
                                         tcamType,
                                         &actionInfo,
                                         typeToRefresh,
                                         &wrPclEntry);

      if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }


     inFields[2]=GT_FALSE;

      /*swtich by action type*/

       switch(typeToRefresh){

       case CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_STANDARD_E:

           prvCpssExMxPm2PclActionGet_INGRESS_STANDARD_E(inFields,outArgs);
           break;
       case CPSS_EXMXPM_PCL_ACTION_FORMAT_INGRESS_REDUCED_E:

           prvCpssExMxPm2PclActionGet_INGRESS_REDUCED_E(inFields,outArgs);
           break;
       case CPSS_EXMXPM_PCL_ACTION_FORMAT_EGRESS_E:

           prvCpssExMxPm2PclActionGet__EGRESS_E(inFields,outArgs);
           break;
       default:
           break;

       }

    return GT_OK;

}
/*******************************************************************************
* wrCpssExMxPmPclRuleActionGetNext
*
* DESCRIPTION:
*   This is stub wrapper
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
***********************************************************************************/
static CMD_STATUS wrCpssExMxPmPclRuleActionGetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    galtisOutput(outArgs, GT_OK, "%d",-1);
    return CMD_OK;
}
/*******************************************************************************
* wrCpssExMxPm2PclRule_ING_STD_NOT_IP_E_Set
*
* DESCRIPTION:
*   The function sets  the Policy Rule Mask or  Pattern from Galtis
*          for   CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       none
*
*
* OUTPUTS:
*       none
*
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2PclRule_ING_STD_NOT_IP_E_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   GT_BYTE_ARRY                       maskBArr, patternBArr;
   /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[0] == 0) /* mask */
    {
        mask_ruleIndex = (GT_U32)inArgs[2];

        /*commonIngrStd*/
        maskData.ruleIngrStandardNotIp.commonIngrStd.pclId= (GT_U16)inFields[2];
        maskData.ruleIngrStandardNotIp.commonIngrStd.macToMe=(GT_U8)inFields[3];
        maskData.ruleIngrStandardNotIp.commonIngrStd.srcPort=(GT_U8)inFields[4];
        maskData.ruleIngrStandardNotIp.commonIngrStd.vid=(GT_U16)inFields[5];
        maskData.ruleIngrStandardNotIp.commonIngrStd.up=(GT_U8)inFields[6];
        maskData.ruleIngrStandardNotIp.commonIngrStd.isIpv4=(GT_U8)inFields[7];
        maskData.ruleIngrStandardNotIp.commonIngrStd.isIp=(GT_U8)inFields[8];

        /*commonIngrStdPkt*/
        maskData.ruleIngrStandardNotIp.commonIngrStdPkt.isTagged=(GT_U8)inFields[9];
        maskData.ruleIngrStandardNotIp.commonIngrStdPkt.isArp=(GT_U8)inFields[10];
        maskData.ruleIngrStandardNotIp.commonIngrStdPkt.isUdbValid=(GT_U8)inFields[11];
        maskData.ruleIngrStandardNotIp.commonIngrStdPkt.isL2Valid=(GT_U8)inFields[12];

        maskData.ruleIngrStandardNotIp.l2EncapType=(GT_U8)inFields[13];
        maskData.ruleIngrStandardNotIp.ethType=(GT_U16)inFields[14];

        galtisMacAddr(&maskData.ruleIngrStandardNotIp.macDa, (GT_U8*)inFields[15]);
        galtisMacAddr(&maskData.ruleIngrStandardNotIp.macSa, (GT_U8*)inFields[16]);

        galtisBArray(&maskBArr,(GT_U8*)inFields[17]);
        cmdOsMemCpy(
            maskData.ruleIngrStandardNotIp.udb, maskBArr.data,
            maskBArr.length);

        maskData.ruleIngrStandardNotIp.udb15Dup=(GT_U8)inFields[18];

        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inArgs[2];
        /*commonIngrStd*/
        patternData.ruleIngrStandardNotIp.commonIngrStd.pclId= (GT_U16)inFields[2];
        patternData.ruleIngrStandardNotIp.commonIngrStd.macToMe=(GT_U8)inFields[3];
        patternData.ruleIngrStandardNotIp.commonIngrStd.srcPort=(GT_U8)inFields[4];
        patternData.ruleIngrStandardNotIp.commonIngrStd.vid=(GT_U16)inFields[5];
        patternData.ruleIngrStandardNotIp.commonIngrStd.up=(GT_U8)inFields[6];
        patternData.ruleIngrStandardNotIp.commonIngrStd.isIpv4=(GT_U8)inFields[7];
        patternData.ruleIngrStandardNotIp.commonIngrStd.isIp=(GT_U8)inFields[8];

        /*commonIngrStdPkt*/
        patternData.ruleIngrStandardNotIp.commonIngrStdPkt.isTagged=(GT_U8)inFields[9];
        patternData.ruleIngrStandardNotIp.commonIngrStdPkt.isArp=(GT_U8)inFields[10];
        patternData.ruleIngrStandardNotIp.commonIngrStdPkt.isUdbValid=(GT_U8)inFields[11];
        patternData.ruleIngrStandardNotIp.commonIngrStdPkt.isL2Valid=(GT_U8)inFields[12];

        patternData.ruleIngrStandardNotIp.l2EncapType=(GT_U8)inFields[13];
        patternData.ruleIngrStandardNotIp.ethType=(GT_U16)inFields[14];

        galtisMacAddr(&patternData.ruleIngrStandardNotIp.macDa, (GT_U8*)inFields[15]);
        galtisMacAddr(&patternData.ruleIngrStandardNotIp.macSa, (GT_U8*)inFields[16]);

        galtisBArray(&patternBArr,(GT_U8*)inFields[17]);
        cmdOsMemCpy(
            patternData.ruleIngrStandardNotIp.udb, patternBArr.data,
            patternBArr.length);

        patternData.ruleIngrStandardNotIp.udb15Dup=(GT_U8)inFields[18];

        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}
/*******************************************************************************
* wrCpssExMxPm2PclRule_ING_STD_IP_L2_QOS_E_Set
*
* DESCRIPTION:
*   The function sets  the Policy Rule Mask or  Pattern from Galtis
*          for   CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       none
*
*
* OUTPUTS:
*       none
*
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2PclRule_ING_STD_IP_L2_QOS_E_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   GT_BYTE_ARRY                       maskBArr, patternBArr;
   /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[0] == 0) /* mask */
    {
        mask_ruleIndex = (GT_U32)inArgs[2];

        /*commonIngrStd*/
        maskData.ruleIngrStandardNotIp.commonIngrStd.pclId= (GT_U16)inFields[2];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.macToMe=(GT_U8)inFields[3];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.srcPort=(GT_U8)inFields[4];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.vid=(GT_U16)inFields[5];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.up=(GT_U8)inFields[6];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.isIpv4=(GT_U8)inFields[7];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStd.isIp=(GT_U8)inFields[8];

        /*commonIngrStdPkt*/
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isTagged=(GT_U8)inFields[9];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isArp=(GT_U8)inFields[10];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isUdbValid=(GT_U8)inFields[11];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isL2Valid=(GT_U8)inFields[12];

         /*commonIngrStdIp*/
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.ipHeaderOk=(GT_U8)inFields[13];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.isIpv4Fragment=(GT_U8)inFields[14];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.ipProtocol=(GT_U8)inFields[15];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.dscp=(GT_U8)inFields[16];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.isL4Valid=(GT_U8)inFields[17];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.l4Byte2=(GT_U8)inFields[18];
        maskData.ruleIngrStandardIpL2Qos.commonIngrStdIp.l4Byte3=(GT_U8)inFields[19];

        galtisMacAddr(&maskData.ruleIngrStandardIpL2Qos.macDa, (GT_U8*)inFields[20]);
        galtisMacAddr(&maskData.ruleIngrStandardIpL2Qos.macSa, (GT_U8*)inFields[21]);

        maskData.ruleIngrStandardIpL2Qos.isIpv6Eh=(GT_U8)inFields[22];
        maskData.ruleIngrStandardIpL2Qos.isIpv6HopByHop=(GT_U8)inFields[23];

        galtisBArray(&maskBArr,(GT_U8*)inFields[24]);
        cmdOsMemCpy(
            maskData.ruleIngrStandardIpL2Qos.udb, maskBArr.data,
            maskBArr.length);



        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inArgs[2];

        /*commonIngrStd*/
        patternData.ruleIngrStandardNotIp.commonIngrStd.pclId= (GT_U16)inFields[2];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.macToMe=(GT_U8)inFields[3];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.srcPort=(GT_U8)inFields[4];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.vid=(GT_U16)inFields[5];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.up=(GT_U8)inFields[6];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.isIpv4=(GT_U8)inFields[7];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStd.isIp=(GT_U8)inFields[8];

        /*commonIngrStdPkt*/
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isTagged=(GT_U8)inFields[9];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isArp=(GT_U8)inFields[10];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isUdbValid=(GT_U8)inFields[11];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdPkt.isL2Valid=(GT_U8)inFields[12];

         /*commonIngrStdIp*/
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.ipHeaderOk=(GT_U8)inFields[13];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.isIpv4Fragment=(GT_U8)inFields[14];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.ipProtocol=(GT_U8)inFields[15];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.dscp=(GT_U8)inFields[16];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.isL4Valid=(GT_U8)inFields[17];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.l4Byte2=(GT_U8)inFields[18];
        patternData.ruleIngrStandardIpL2Qos.commonIngrStdIp.l4Byte3=(GT_U8)inFields[19];

        galtisMacAddr(&patternData.ruleIngrStandardIpL2Qos.macDa, (GT_U8*)inFields[20]);
        galtisMacAddr(&patternData.ruleIngrStandardIpL2Qos.macSa, (GT_U8*)inFields[21]);

        patternData.ruleIngrStandardIpL2Qos.isIpv6Eh=(GT_U8)inFields[22];
        patternData.ruleIngrStandardIpL2Qos.isIpv6HopByHop=(GT_U8)inFields[23];

        galtisBArray(&patternBArr,(GT_U8*)inFields[24]);
        cmdOsMemCpy(
            patternData.ruleIngrStandardIpL2Qos.udb, patternBArr.data,
            patternBArr.length);

        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}
/*******************************************************************************
* wrCpssExMxPm2PclRule_ING_STD_IPV4_L4_E_Set
*
* DESCRIPTION:
*   The function sets  the Policy Rule Mask or  Pattern from Galtis
*          for   CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       none
*
*
* OUTPUTS:
*       none
*
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2PclRule_ING_STD_IPV4_L4_E_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY                       maskBArr, patternBArr;
    GT_ETHERADDR                       isBcData;

   /* map input arguments to locals */
    /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */
    if(inFields[0] == 0) /* mask */
    {
        mask_ruleIndex = (GT_U32)inArgs[2];

        /*commonIngrStd*/
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.pclId= (GT_U16)inFields[2];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.macToMe=(GT_U8)inFields[3];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.srcPort=(GT_U8)inFields[4];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.vid=(GT_U16)inFields[5];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.up=(GT_U8)inFields[6];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.isIpv4=(GT_U8)inFields[7];
        maskData.ruleIngrStandardIpv4L4.commonIngrStd.isIp=(GT_U8)inFields[8];

        /*commonIngrStdPkt*/
        maskData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isTagged=(GT_U8)inFields[9];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isArp=(GT_U8)inFields[10];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isUdbValid=(GT_U8)inFields[11];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isL2Valid=(GT_U8)inFields[12];

         /*commonIngrStdIp*/
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.ipHeaderOk=(GT_U8)inFields[13];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.isIpv4Fragment=(GT_U8)inFields[14];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.ipProtocol=(GT_U8)inFields[15];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.dscp=(GT_U8)inFields[16];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.isL4Valid=(GT_U8)inFields[17];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.l4Byte2=(GT_U8)inFields[18];
        maskData.ruleIngrStandardIpv4L4.commonIngrStdIp.l4Byte3=(GT_U8)inFields[19];

        /* field is defined like MAC address - array of 6 bytes.
           use first byte for the field */
        galtisMacAddr(&isBcData, (GT_U8*)inFields[20]);
        maskData.ruleIngrStandardIpv4L4.isBc= isBcData.arEther[0];

        galtisIpAddr(&maskData.ruleIngrStandardIpv4L4.sip, (GT_U8*)inFields[21]);
        galtisIpAddr(&maskData.ruleIngrStandardIpv4L4.dip, (GT_U8*)inFields[22]);

        maskData.ruleIngrStandardIpv4L4.l4Byte0=(GT_U8)inFields[23];
        maskData.ruleIngrStandardIpv4L4.l4Byte1=(GT_U8)inFields[24];
        maskData.ruleIngrStandardIpv4L4.l4Byte13=(GT_U8)inFields[25];

        galtisBArray(&maskBArr,(GT_U8*)inFields[26]);
        cmdOsMemCpy(
            maskData.ruleIngrStandardIpv4L4.udb, maskBArr.data,
            maskBArr.length);

        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inArgs[2];

        /*commonIngrStd*/
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.pclId= (GT_U16)inFields[2];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.macToMe=(GT_U8)inFields[3];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.srcPort=(GT_U8)inFields[4];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.vid=(GT_U16)inFields[5];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.up=(GT_U8)inFields[6];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.isIpv4=(GT_U8)inFields[7];
        patternData.ruleIngrStandardIpv4L4.commonIngrStd.isIp=(GT_U8)inFields[8];

        /*commonIngrStdPkt*/
        patternData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isTagged=(GT_U8)inFields[9];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isArp=(GT_U8)inFields[10];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isUdbValid=(GT_U8)inFields[11];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdPkt.isL2Valid=(GT_U8)inFields[12];

         /*commonIngrStdIp*/
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.ipHeaderOk=(GT_U8)inFields[13];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.isIpv4Fragment=(GT_U8)inFields[14];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.ipProtocol=(GT_U8)inFields[15];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.dscp=(GT_U8)inFields[16];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.isL4Valid=(GT_U8)inFields[17];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.l4Byte2=(GT_U8)inFields[18];
        patternData.ruleIngrStandardIpv4L4.commonIngrStdIp.l4Byte3=(GT_U8)inFields[19];

        /* field is defined like MAC address - array of 6 bytes.
           use first byte for the field */
        galtisMacAddr(&isBcData, (GT_U8*)inFields[20]);
        patternData.ruleIngrStandardIpv4L4.isBc=isBcData.arEther[0];

        galtisIpAddr(&patternData.ruleIngrStandardIpv4L4.sip, (GT_U8*)inFields[21]);
        galtisIpAddr(&patternData.ruleIngrStandardIpv4L4.dip, (GT_U8*)inFields[22]);

        patternData.ruleIngrStandardIpv4L4.l4Byte0=(GT_U8)inFields[23];
        patternData.ruleIngrStandardIpv4L4.l4Byte1=(GT_U8)inFields[24];
        patternData.ruleIngrStandardIpv4L4.l4Byte13=(GT_U8)inFields[25];

        galtisBArray(&patternBArr,(GT_U8*)inFields[26]);
        cmdOsMemCpy(
            patternData.ruleIngrStandardIpv4L4.udb, patternBArr.data,
            patternBArr.length);


        pattern_set = GT_TRUE;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPm2PclRuleSetFirst
*
* DESCRIPTION:
*   The function sets  the Policy Rule Mask or  Pattern
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       tcamType       - TCAM type: internal or external
*       ruleInfoPtr    - (pointer to) the Rule and Action Layouts
*       ruleFormat     - format of the Rule.
*
*
* OUTPUTS:
*
*
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2PclRuleSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   GT_U32 typeToSet;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    typeToSet=(GT_U32)inArgs[4];

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));

    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;

    switch(tableNum)
    {
    case 0:
        switch (typeToSet)
        {
        case 0:
            ruleFormat= CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;
            break;
        case 1:
            ruleFormat=CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;
            break;
        case 2:
            ruleFormat=CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E;
            break;
        default:
            break;

        }
        break;
    default:
        break;

    }
    switch (ruleFormat)
    {
        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E:
            wrCpssExMxPm2PclRule_ING_STD_NOT_IP_E_Set(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E:
            wrCpssExMxPm2PclRule_ING_STD_IP_L2_QOS_E_Set(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E:
            wrCpssExMxPm2PclRule_ING_STD_IPV4_L4_E_Set(inArgs,inFields,numFields,outArgs);
            break;


        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong PCL RULE FORMAT.\n");
            return CMD_AGENT_ERROR;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}
/*******************************************************************************
* wrCpssExMxPm2PclRuleSetNext
*
* DESCRIPTION:
*   The function sets  the Policy Rule Mask or  Pattern,cheks if Mask and Pattern set.
*   Binds external Tcam configs if   tcamType==CPSS_EXMXPM_TCAM_TYPE_EXTERNAL_E.
*   Calls cpssExMxPmPclRuleSet API.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       tcamType       - TCAM type: internal or external
*       ruleInfoPtr    - (pointer to) the Rule and Action Layouts
*       ruleFormat     - format of the Rule.
*
*
* OUTPUTS:
*
*
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2PclRuleSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                  devNum;
    GT_U32                                 typeToSet;
    GT_U32                                 index;
    CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType;
    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT  ruleInfo;
    WR_PCL_ACTION_STC                      *tablePtr;
    GT_BOOL                                valid;
    CPSS_EXMXPM_PCL_ACTION_UNT             *actionPtr;
    GT_STATUS                              result=GT_OK;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

     /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tcamType=(CPSS_EXMXPM_TCAM_TYPE_ENT)inArgs[1];
    mask_ruleIndex=(GT_U32)inArgs[2];
    typeToSet=(GT_U32)inArgs[4];

    if(inFields[0] == 0 && mask_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if(inFields[0] == 1 && pattern_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if ((mask_set == GT_FALSE) && (pattern_set == GT_FALSE))
    {
        cmdOsMemSet(&maskData , 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));
        cmdOsMemSet(&patternData, 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));
    }

    switch(tableNum)
    {
    case 0:
        switch (typeToSet)
        {
        case 0:
            ruleFormat= CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;
            break;
        case 1:
            ruleFormat=CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;
            break;
        case 2:
            ruleFormat=CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E;
            break;
        default:
            break;

        }
        break;
      /* TODO:add support for other tables*/
    default:
        break;

    }
    switch (ruleFormat)
    {
        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E:
            wrCpssExMxPm2PclRule_ING_STD_NOT_IP_E_Set(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E:
            wrCpssExMxPm2PclRule_ING_STD_IP_L2_QOS_E_Set(inArgs,inFields,numFields,outArgs);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E:
            wrCpssExMxPm2PclRule_ING_STD_IPV4_L4_E_Set(inArgs,inFields,numFields,outArgs);
            break;


        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong PCL RULE FORMAT.\n");
            return CMD_AGENT_ERROR;
    }

    if(tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
    {
       ruleInfo.internalTcamRuleStartIndex=mask_ruleIndex;
       tablePtr=actionIntTable;
    }

    else
    {   /*bind extTcam params*/
       wrCpssExMxPmPclExtConfigIndexGet(devNum, mask_ruleIndex, &index);

       ruleInfo.externalTcamInfo.ruleInfo.tcamId=extTcamConfigPclTable[index].tcamId;
       ruleInfo.externalTcamInfo.ruleInfo.ruleStartIndex=extTcamConfigPclTable[index].ruleStartIndex;
       ruleInfo.externalTcamInfo.ruleInfo.ruleSize=extTcamConfigPclTable[index].ruleSize;
       ruleInfo.externalTcamInfo.ruleInfo.ageEnable=extTcamConfigPclTable[index].ageEnable;

       ruleInfo.externalTcamInfo.actionInfo.actionStartIndex=extTcamConfigPclTable[index].actionStartIndex;
       ruleInfo.externalTcamInfo.actionInfo.memoryType=extTcamConfigPclTable[index].memoryType;
       tablePtr=actionExtTable;
    }


    /* Get the action */
    wrCpssExMxPmPclActionTableIndexGet(devNum, mask_ruleIndex,tablePtr,&index);

    if(mask_set && pattern_set && (index != INVALID_RULE_INDEX))
    {
        actionPtr = &(tablePtr[index].actionEntry);
        valid=(GT_BOOL)inFields[1];
        /* call cpss api function */
        result = cpssExMxPmPclRuleSet(devNum,tcamType,&ruleInfo,
                                       ruleFormat,tablePtr[index].actionFormat,valid,
                                       &maskData,&patternData, actionPtr);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        if(result!=GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }



    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;


}
static GT_BOOL isMaskPcl;
static GT_BOOL isLast;
/*******************************************************************************
* wrCpssExMxPm2PclRule_ING_STD_NOT_IP_E_Get
*
* DESCRIPTION:
*   The function gets  the Policy Rule Mask or  Pattern from Galtis
*          for   CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       none
*
*
* OUTPUTS:
*       none
*
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2PclRule_ING_STD_NOT_IP_E_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    GT_BOOL   valid
)
{   GT_BYTE_ARRY                       dataBArr;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT    *dataPtr;
    GT_U8                              *dataStr;


    if(isMaskPcl) /* mask */
    {
      dataPtr=&maskData;
      isMaskPcl = GT_FALSE;
      inFields[0]=0;
    }
    else/*pattern*/
    {
      dataPtr=&patternData;
      isMaskPcl=GT_TRUE;
      inFields[0]=1;
      isLast=GT_TRUE;
    }
    inFields[1]=valid;

    /*commonIngrStd*/
   inFields[2]=dataPtr->ruleIngrStandardNotIp.commonIngrStd.pclId;
   inFields[3]=dataPtr->ruleIngrStandardNotIp.commonIngrStd.macToMe;
   inFields[4]=dataPtr->ruleIngrStandardNotIp.commonIngrStd.srcPort;
   inFields[5]=dataPtr->ruleIngrStandardNotIp.commonIngrStd.vid;
   inFields[6]=dataPtr->ruleIngrStandardNotIp.commonIngrStd.up;
   inFields[7]=dataPtr->ruleIngrStandardNotIp.commonIngrStd.isIpv4;
   inFields[8]=dataPtr->ruleIngrStandardNotIp.commonIngrStd.isIp;

   /*commonIngrStdPkt*/
   inFields[9]=dataPtr->ruleIngrStandardNotIp.commonIngrStdPkt.isTagged;
   inFields[10]=dataPtr->ruleIngrStandardNotIp.commonIngrStdPkt.isArp;
   inFields[11]=dataPtr->ruleIngrStandardNotIp.commonIngrStdPkt.isUdbValid;
   inFields[12]=dataPtr->ruleIngrStandardNotIp.commonIngrStdPkt.isL2Valid;

   inFields[13]=dataPtr->ruleIngrStandardNotIp.l2EncapType;
   inFields[14]=dataPtr->ruleIngrStandardNotIp.ethType;

   dataBArr.data=dataPtr->ruleIngrStandardNotIp.udb;
   dataBArr.length=18;

   inFields[18]=dataPtr->ruleIngrStandardNotIp.udb15Dup;

   dataStr=galtisBArrayOut(&dataBArr);

   fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%6b%6b%s%d",
                    inFields[0], inFields[1],  inFields[2], inFields[3],
                    inFields[4],inFields[5], inFields[6], inFields[7],
                    inFields[8],inFields[9], inFields[10],inFields[11],inFields[12],
                    inFields[13], inFields[14], dataPtr->ruleIngrStandardNotIp.macDa.arEther,
                    dataPtr->ruleIngrStandardNotIp.macSa.arEther,dataStr,
                    inFields[18]);

     /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 0);

   return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPm2PclRule_ING_STD_IP_L2_QOS_E_Get
*
* DESCRIPTION:
*   The function gets  the Policy Rule Mask or  Pattern from Galtis
*          for   CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       none
*
*
* OUTPUTS:
*       none
*
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2PclRule_ING_STD_IP_L2_QOS_E_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL valid
)
{   GT_BYTE_ARRY                       dataBArr;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT    *dataPtr;
    GT_U8                              *dataStr;


    if(isMaskPcl) /* mask */
    {
      dataPtr=&maskData;
      isMaskPcl = GT_FALSE;
      inFields[0]=0;
    }
    else/*pattern*/
    {
      dataPtr=&patternData;
      isMaskPcl=GT_TRUE;
      inFields[0]=1;
      isLast=GT_TRUE;
    }
    inFields[1]=valid;

    /*commonIngrStd*/
    inFields[2]=dataPtr->ruleIngrStandardNotIp.commonIngrStd.pclId;
    inFields[3]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStd.macToMe;
    inFields[4]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStd.srcPort;
    inFields[5]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStd.vid;
    inFields[6]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStd.up;
    inFields[7]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStd.isIpv4;
    inFields[8]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStd.isIp;

    /*commonIngrStdPkt*/
    inFields[9]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStdPkt.isTagged;
    inFields[10]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStdPkt.isArp;
    inFields[11]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStdPkt.isUdbValid;
    inFields[12]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStdPkt.isL2Valid;

    /*commonIngrStdIp*/
    inFields[13]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStdIp.ipHeaderOk;
    inFields[14]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStdIp.isIpv4Fragment;
    inFields[15]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStdIp.ipProtocol;
    inFields[16]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStdIp.dscp;
    inFields[17]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStdIp.isL4Valid;
    inFields[18]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStdIp.l4Byte2;
    inFields[19]=dataPtr->ruleIngrStandardIpL2Qos.commonIngrStdIp.l4Byte3;

    inFields[22]=dataPtr->ruleIngrStandardIpL2Qos.isIpv6Eh;
    inFields[23]=dataPtr->ruleIngrStandardIpL2Qos.isIpv6HopByHop;

    dataBArr.data=dataPtr->ruleIngrStandardIpL2Qos.udb;
    dataBArr.length=20;
    dataStr=galtisBArrayOut(&dataBArr);

   fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%6b%6b%d%d%s",
                    inFields[0], inFields[1],  inFields[2], inFields[3],
                    inFields[4],inFields[5], inFields[6], inFields[7],
                    inFields[8],inFields[9], inFields[10],inFields[11],inFields[12],
                    inFields[13], inFields[14],inFields[15],inFields[16],
                    inFields[17],inFields[18],inFields[19],
                    dataPtr->ruleIngrStandardIpL2Qos.macDa.arEther,
                    dataPtr->ruleIngrStandardIpL2Qos.macSa.arEther,
                    inFields[22],inFields[23],dataStr);


     /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 1);

    return CMD_OK;

}
/*******************************************************************************
* wrCpssExMxPm2PclRule_ING_STD_IPV4_L4_E_Get
*
* DESCRIPTION:
*   The function gets  the Policy Rule Mask or  Pattern from Galtis
*          for   CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       none
*
*
* OUTPUTS:
*       none
*
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2PclRule_ING_STD_IPV4_L4_E_Get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL valid
)
{
    GT_BYTE_ARRY                       dataBArr;
    CPSS_EXMXPM_PCL_RULE_FORMAT_UNT    *dataPtr;
    GT_U8                              *dataStr;
    GT_U8                               isBcArr[6];

    isBcArr[1] = isBcArr[2] = isBcArr[3] = isBcArr[4] = isBcArr[5] = 0;

    if(isMaskPcl) /* mask */
    {
      dataPtr=&maskData;
      isMaskPcl = GT_FALSE;
      inFields[0]=0;
    }
    else/*pattern*/
    {
      dataPtr=&patternData;
      isMaskPcl=GT_TRUE;
      inFields[0]=1;
      isLast=GT_TRUE;
    }
    inFields[1]=valid;


    /*commonIngrStd*/
    inFields[2]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStd.pclId ;
    inFields[3]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStd.macToMe;
    inFields[4]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStd.srcPort;
    inFields[5]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStd.vid;
    inFields[6]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStd.up;
    inFields[7]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStd.isIpv4;
    inFields[8]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStd.isIp;

    /*commonIngrStdPkt*/
    inFields[9]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStdPkt.isTagged;
    inFields[10]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStdPkt.isArp;
    inFields[11]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStdPkt.isUdbValid;
    inFields[12]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStdPkt.isL2Valid;

     /*commonIngrStdIp*/
    inFields[13]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStdIp.ipHeaderOk;
    inFields[14]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStdIp.isIpv4Fragment;
    inFields[15]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStdIp.ipProtocol;
    inFields[16]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStdIp.dscp;
    inFields[17]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStdIp.isL4Valid;
    inFields[18]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStdIp.l4Byte2;
    inFields[19]=dataPtr->ruleIngrStandardIpv4L4.commonIngrStdIp.l4Byte3;


    isBcArr[0]=dataPtr->ruleIngrStandardIpv4L4.isBc;

    inFields[23]=dataPtr->ruleIngrStandardIpv4L4.l4Byte0;
    inFields[24]=dataPtr->ruleIngrStandardIpv4L4.l4Byte1;
    inFields[25]=dataPtr->ruleIngrStandardIpv4L4.l4Byte13;



    dataBArr.data=dataPtr->ruleIngrStandardIpv4L4.udb;
    dataBArr.length=23;
    dataStr=galtisBArrayOut(&dataBArr);

   fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%6b%4b%4b%d%d%d%s",
                    inFields[0], inFields[1],  inFields[2], inFields[3],
                    inFields[4],inFields[5], inFields[6], inFields[7],
                    inFields[8],inFields[9], inFields[10],inFields[11],inFields[12],
                    inFields[13], inFields[14],inFields[15],inFields[16],
                    inFields[17],inFields[18],inFields[19],isBcArr,
                    dataPtr->ruleIngrStandardIpv4L4.sip.arIP,
                    dataPtr->ruleIngrStandardIpv4L4.dip.arIP,
                    inFields[23],inFields[24],inFields[25],dataStr);


     /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 2);

    return CMD_OK;

}
/*******************************************************************************
* wrCpssExMxPm2PclRuleGetEntry
*
* DESCRIPTION:
*   The function gets  the Policy Rule Mask and Pattern
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       tcamType       - TCAM type: internal or external
*       ruleInfoPtr    - (pointer to) the Rule and Action Layouts
*       ruleFormat     - format of the Rule.
*
*
* OUTPUTS:
*
*
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2PclRuleGetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                  devNum;
    GT_U32                                 typeToGet;
    GT_U32                                 index;
    CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType;
    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT  ruleInfo;
    GT_BOOL                                valid;
    WR_PCL_ACTION_STC                      *tablePtr;
    CPSS_EXMXPM_PCL_ACTION_UNT             action;
    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT      actionFormat;
    GT_STATUS                              result=GT_OK;

    /* if isMaskPcl == GT_TRUE it is mask's turn (the first one to get), so there need to refresh the data
        else the data is updated and it is pattern's turn (was updated earlier)*/

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(isLast)
     {
        galtisOutput(outArgs, GT_OK, "%d",-1);
        return CMD_OK;

     }

     /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tcamType=(CPSS_EXMXPM_TCAM_TYPE_ENT)inArgs[1];
    mask_ruleIndex=(GT_U32)inArgs[2];
    typeToGet=(GT_U32)inArgs[3];


    if(tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
    {
       ruleInfo.internalTcamRuleStartIndex=mask_ruleIndex;
       tablePtr=actionIntTable;

    }

    else
    {   /*bind extTcam params*/
       wrCpssExMxPmPclExtConfigIndexGet(devNum, mask_ruleIndex, &index);

       ruleInfo.externalTcamInfo.ruleInfo.tcamId=extTcamConfigPclTable[index].tcamId;
       ruleInfo.externalTcamInfo.ruleInfo.ruleStartIndex=extTcamConfigPclTable[index].ruleStartIndex;
       ruleInfo.externalTcamInfo.ruleInfo.ruleSize=extTcamConfigPclTable[index].ruleSize;
       ruleInfo.externalTcamInfo.ruleInfo.ageEnable=extTcamConfigPclTable[index].ageEnable;

       ruleInfo.externalTcamInfo.actionInfo.actionStartIndex=extTcamConfigPclTable[index].actionStartIndex;
       ruleInfo.externalTcamInfo.actionInfo.memoryType=extTcamConfigPclTable[index].memoryType;
       tablePtr=actionExtTable;

    }
     /* get rule format */
    switch(tableNum)
    {
    case 0:
        switch (typeToGet)
        {
        case 0:
            ruleFormat= CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E;
            break;
        case 1:
            ruleFormat=CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E;
            break;
        case 2:
            ruleFormat=CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E;
            break;
        default:
            break;

        }
        break;
      /* TODO:add support for other tables*/
    default:
        break;

    }

    /*get action format*/
    wrCpssExMxPmPclActionTableIndexGet(devNum, mask_ruleIndex,tablePtr,&index);
    actionFormat=tablePtr[index].actionFormat;

    /*call cpss api function*/
    result = cpssExMxPmPclRuleGet(devNum,tcamType,&ruleInfo,
                                       ruleFormat,actionFormat,&valid,
                                       &maskData,&patternData,&action);


    switch (ruleFormat)
    {
        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E:
            wrCpssExMxPm2PclRule_ING_STD_NOT_IP_E_Get(inArgs,inFields,numFields,outArgs,valid);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E:
            wrCpssExMxPm2PclRule_ING_STD_IP_L2_QOS_E_Get(inArgs,inFields,numFields,outArgs,valid);
            break;

        case CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E:
            wrCpssExMxPm2PclRule_ING_STD_IPV4_L4_E_Get(inArgs,inFields,numFields,outArgs,valid);
            break;


        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong PCL RULE FORMAT.\n");
            return CMD_AGENT_ERROR;
    }

     return CMD_OK;

}
/*******************************************************************************
* wrCpssExMxPm2PclRuleGetFirst
*
* DESCRIPTION:
*   The function gets  the Policy Rule Mask and Pattern
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       tcamType       - TCAM type: internal or external
*       ruleInfoPtr    - (pointer to) the Rule and Action Layouts
*       ruleFormat     - format of the Rule.
*
*
* OUTPUTS:
*
*
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2PclRuleGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    /* first to get is mask */
    isMaskPcl = GT_TRUE;
    isLast=GT_FALSE;
    return wrCpssExMxPm2PclRuleGetEntry(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* wrCpssExMxPm2PclIngressStdRuleWriteFirst
*
* DESCRIPTION:
*   The function sets table number and call generic wrapper to set
*   the Policy Rule Mask or  Pattern.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       none
*
*
* OUTPUTS:
*       none
*
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2PclIngressStdRuleWriteFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    tableNum=0;
    return wrCpssExMxPm2PclRuleSetFirst(inArgs,inFields,numFields,outArgs);

}

/*******************************************************************************
* wrCpssExMxPm2PclIngressStdRuleWriteNext
*
* DESCRIPTION:
*   The function sets table number and call generic wrapper to set
*   the Policy Rule Mask or  Pattern
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       none
*
*
* OUTPUTS:
*       none
*
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2PclIngressStdRuleWriteNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    tableNum=0;
    return wrCpssExMxPm2PclRuleSetNext(inArgs,inFields,numFields,outArgs);

}
/*******************************************************************************
* wrCpssExMxPm2PclIngressStdRuleReadFirst
*
* DESCRIPTION:
*   The function gets table number and call generic wrapper to set
*   the Policy Rule Mask or  Pattern.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       none
*
*
* OUTPUTS:
*       none
*
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2PclIngressStdRuleReadFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    tableNum=0;
    return wrCpssExMxPm2PclRuleGetFirst(inArgs,inFields,numFields,outArgs);

}

/*******************************************************************************
* wrCpssExMxPm2PclIngressStdRuleReadNext
*
* DESCRIPTION:
*   The function gets table number and call generic wrapper to set
*   the Policy Rule Mask or  Pattern
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       none
*
*
* OUTPUTS:
*       none
*
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2PclIngressStdRuleReadNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    tableNum=0;
    return wrCpssExMxPm2PclRuleGetEntry(inArgs,inFields,numFields,outArgs);

}

/* RULE BY FIELD SET */

#define PRV_MAC(_element_name, _element_expose)                \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                               \
    CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_INGRESS_STANDARD_STC,   \
    _element_name, _element_expose, NULL)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonIngrStd[] =
{
    PRV_MAC(pclId,   "pclId"),
    PRV_MAC(macToMe, "macToMe"),
    PRV_MAC(srcPort, "srcPort"),
    PRV_MAC(vid,     "vid"),
    PRV_MAC(up,      "up"),
    PRV_MAC(isIpv4,  "isIpv4"),
    PRV_MAC(isIp,    "isIp"),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose)                         \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                        \
    CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_INGRESS_STANDARD_PKT_TYPE_STC,   \
    _element_name, _element_expose, NULL)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonIngrStdPkt[] =
{
    PRV_MAC(isTagged,   "isTagged"),
    PRV_MAC(isArp,      "isArp"),
    PRV_MAC(isUdbValid, "isUdbValid"),
    PRV_MAC(isL2Valid,  "isL2Valid"),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose)                   \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                  \
    CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_INGRESS_STANDARD_IP_STC,   \
    _element_name, _element_expose, NULL)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonIngrStdIp[] =
{
    PRV_MAC(ipHeaderOk,       "ipHeaderOk"),
    PRV_MAC(isIpv4Fragment,   "isIpv4Fragment"),
    PRV_MAC(ipProtocol,       "ipProtocol"),
    PRV_MAC(dscp,             "dscp"),
    PRV_MAC(isL4Valid,        "isL4Valid"),
    PRV_MAC(l4Byte2,          "l4Byte2"),
    PRV_MAC(l4Byte3,          "l4Byte3"),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)        \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                     \
    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_STC,         \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrStandardNotIp[] =
{
    PRV_MAC(commonIngrStd,     "commonIngrStd",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrStd),
    PRV_MAC(commonIngrStdPkt,  "commonIngrStdPkt",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrStdPkt),
    PRV_MAC(l2EncapType,  "l2EncapType", NULL),
    PRV_MAC(ethType,      "ethType",     NULL),
    PRV_MAC(macDa,        "macDa",       NULL),
    PRV_MAC(macSa,        "macSa",       NULL),
    PRV_MAC(udb,          "udb",         NULL),
    PRV_MAC(udb15Dup,     "udb15Dup",    NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_NOT_IP_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)           \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                        \
    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_STC,         \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrStandardIpL2Qos[] =
{
    PRV_MAC(commonIngrStd,     "commonIngrStd",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrStd),
    PRV_MAC(commonIngrStdPkt,  "commonIngrStdPkt",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrStdPkt),
    PRV_MAC(commonIngrStdPkt,  "commonIngrStdIp",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrStdIp),
    PRV_MAC(macDa,           "macDa",          NULL),
    PRV_MAC(macSa,           "macSa",          NULL),
    PRV_MAC(isIpv6Eh,        "isIpv6Eh",       NULL),
    PRV_MAC(isIpv6HopByHop,  "isIpv6HopByHop", NULL),
    PRV_MAC(udb,             "udb",            NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IP_L2_QOS_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)           \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                        \
    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_STC,           \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrStandardIpv4L4[] =
{
    PRV_MAC(commonIngrStd,     "commonIngrStd",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrStd),
    PRV_MAC(commonIngrStdPkt,  "commonIngrStdPkt",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrStdPkt),
    PRV_MAC(commonIngrStdPkt,  "commonIngrStdIp",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrStdIp),
    PRV_MAC(isBc,            "isBc",           NULL),
    PRV_MAC(sip,             "sip",            NULL),
    PRV_MAC(dip,             "dip",            NULL),
    PRV_MAC(l4Byte0,         "l4Byte0",        NULL),
    PRV_MAC(l4Byte1,         "l4Byte1",        NULL),
    PRV_MAC(l4Byte13,        "l4Byte13",       NULL),
    PRV_MAC(udb,             "udb",            NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_IPV4_L4_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose)      \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                     \
    CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_EGRESS_STC,   \
    _element_name, _element_expose, NULL)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonEgr[] =
{
    PRV_MAC(pclId,      "pclId"),
    PRV_MAC(isMpls,     "isMpls"),
    PRV_MAC(srcPort,    "srcPort"),
    PRV_MAC(isTagged,   "isTagged"),
    PRV_MAC(vid,        "vid"),
    PRV_MAC(up,         "up"),
    PRV_MAC(isIp,       "isIp"),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose)               \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                              \
    CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_EGRESS_STANDARD_STC,   \
    _element_name, _element_expose, NULL)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonEgrStd[] =
{
    PRV_MAC(isIpv4,                          "isIpv4"),
    PRV_MAC(isArp,                           "isArp"),
    PRV_MAC(isL2Valid,                       "isL2Valid"),
    PRV_MAC(packetCmd,                        "packetCmd"),
    PRV_MAC(trgOrCpuCode0_6OrCosOrSniff,      "trgOrCpuCode0_6OrCosOrSniff"),
    PRV_MAC(srcTrgOrTxMirror,                 "srcTrgOrTxMirror"),
    PRV_MAC(sourceId,                         "sourceId"),
    PRV_MAC(srcInfo,                          "srcInfo"),
    PRV_MAC(srcIsTrunk,                       "srcIsTrunk"),
    PRV_MAC(isIpOrMplsRouted,                 "isIpOrMplsRouted"),
    PRV_MAC(egrFilterRegOrCpuCode7,           "egrFilterRegOrCpuCode7"),
    PRV_MAC(flowId_Extrn,                     "flowId_Extrn"),
    PRV_MAC(trgPort_Extrn,                    "trgPort_Extrn"),
    PRV_MAC(srcDev8_7_Extrn,                  "srcDev8_7_Extrn"),
    PRV_MAC(exp_Extrn,                        "exp_Extrn"),
    PRV_MAC(egrFilterReg_Extrn,               "egrFilterReg_Extrn"),
    PRV_MAC(macSa_Extrn,                      "macSa_Extrn"),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose)                  \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                 \
    CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_EGRESS_STANDARD_IP_STC,   \
    _element_name, _element_expose, NULL)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonEgrStdIp[] =
{
    PRV_MAC(ipProtocol,             "ipProtocol"),
    PRV_MAC(dscp,                   "dscp"),
    PRV_MAC(isIpv4Fragment,         "isIpv4Fragment"),
    PRV_MAC(isL4Valid,              "isL4Valid"),
    PRV_MAC(l4Byte2,                "l4Byte2"),
    PRV_MAC(l4Byte3,                "l4Byte3"),
    PRV_MAC(l4Byte13,               "l4Byte13"),
    PRV_MAC(tcpUdpPortComparators,  "tcpUdpPortComparators"),
    PRV_MAC(dipBits31_0,            "dipBits31_0"),
    PRV_MAC(tosBits1_0_Extrn,       "tosBits1_0_Extrn"),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)         \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                      \
    CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_STC,           \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_EgrStandardNotIp[] =
{
    PRV_MAC(commonEgr,     "commonEgr",
        wrCpssExMxPmPclRuleFieldDsc_commonEgr),
    PRV_MAC(commonEgrStd,  "commonEgrStd",
        wrCpssExMxPmPclRuleFieldDsc_commonEgrStd),
    PRV_MAC(l2EncapType,  "l2EncapType",     NULL),
    PRV_MAC(ethType,      "ethType",         NULL),
    PRV_MAC(macDa,        "macDa",           NULL),
    PRV_MAC(macSa,         "macSa",          NULL),
    PRV_MAC(isBc_Extrn,  "isBc_Extrn",       NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_NOT_IP_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)         \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                      \
    CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IP_L2_QOS_STC,        \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_EgrStandardIpL2Qos[] =
{
    PRV_MAC(commonEgr,       "commonEgr",
        wrCpssExMxPmPclRuleFieldDsc_commonEgr),
    PRV_MAC(commonEgrStd,    "commonEgrStd",
        wrCpssExMxPmPclRuleFieldDsc_commonEgrStd),
    PRV_MAC(commonEgrStdIp,  "commonEgrStdIp",
        wrCpssExMxPmPclRuleFieldDsc_commonEgrStdIp),
    PRV_MAC(macDa,        "macDa",           NULL),
    PRV_MAC(sipBits31_0,  "sipBits31_0",     NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IP_L2_QOS_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)         \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                      \
    CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IPV4_L4_STC,          \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_EgrStandardIpv4L4[] =
{
    PRV_MAC(commonEgr,       "commonEgr",
        wrCpssExMxPmPclRuleFieldDsc_commonEgr),
    PRV_MAC(commonEgrStd,    "commonEgrStd",
        wrCpssExMxPmPclRuleFieldDsc_commonEgrStd),
    PRV_MAC(commonEgrStdIp,  "commonEgrStdIp",
        wrCpssExMxPmPclRuleFieldDsc_commonEgrStdIp),
    PRV_MAC(isBc,       "isBc",          NULL),
    PRV_MAC(sip,        "sip",           NULL),
    PRV_MAC(l4Byte0,    "l4Byte0",       NULL),
    PRV_MAC(l4Byte1,    "l4Byte1",       NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_STANDARD_IPV4_L4_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose)                  \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                 \
    CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_INGRESS_EXTENDED_STC,     \
    _element_name, _element_expose, NULL)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonIngrExt[] =
{
    PRV_MAC(pclId,       "pclId"),
    PRV_MAC(macToMe,     "macToMe"),
    PRV_MAC(srcPort,     "srcPort"),
    PRV_MAC(vid,         "vid"),
    PRV_MAC(up,          "up"),
    PRV_MAC(isIpv6,      "isIpv6"),
    PRV_MAC(isIp,        "isIp"),
    PRV_MAC(ipProtocol,  "ipProtocol"),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose)                       \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                      \
    CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_INGRESS_EXTENDED_PKT_TYPE_STC, \
    _element_name, _element_expose, NULL)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonIngrExtPkt[] =
{
    PRV_MAC(isUdbValid,    "isUdbValid"),
    PRV_MAC(isL2Valid,     "isL2Valid"),
    PRV_MAC(isTagged,      "isTagged"),
    PRV_MAC(ipHeaderOk,    "ipHeaderOk"),
    PRV_MAC(sipBits31_0,   "sipBits31_0"),
    PRV_MAC(dscp,          "dscp"),
    PRV_MAC(isL4Valid,     "isL4Valid"),
    PRV_MAC(l4Byte0,       "l4Byte0"),
    PRV_MAC(l4Byte1,       "l4Byte1"),
    PRV_MAC(l4Byte2,       "l4Byte2"),
    PRV_MAC(l4Byte3,       "l4Byte3"),
    PRV_MAC(l4Byte13,      "l4Byte13"),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)         \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                      \
    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_NOT_IPV6_STC,        \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrExtendedNotIpv6[] =
{
    PRV_MAC(commonIngrExt,       "commonIngrExt",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrExt),
    PRV_MAC(commonIngrExtPkt,    "commonIngrExtPkt",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrExtPkt),
    PRV_MAC(dipBits31_0,    "dipBits31_0",             NULL),
    PRV_MAC(l2Encapsulation0,    "l2Encapsulation0",   NULL),
    PRV_MAC(ethType,             "ethType",            NULL),
    PRV_MAC(macSa,               "macSa",              NULL),
    PRV_MAC(macDa,               "macDa",              NULL),
    PRV_MAC(isIpv4Fragment,      "isIpv4Fragment",     NULL),
    PRV_MAC(udb,                 "udb",                NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_NOT_IPV6_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)         \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                      \
    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L2_STC,         \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrExtendedIpv6L2[] =
{
    PRV_MAC(commonIngrExt,       "commonIngrExt",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrExt),
    PRV_MAC(commonIngrExtPkt,    "commonIngrExtPkt",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrExtPkt),
    PRV_MAC(isIpv6Eh,          "isIpv6Eh",          NULL),
    PRV_MAC(isIpv6HopByHop,    "isIpv6HopByHop",    NULL),
    PRV_MAC(macDa,             "macDa",             NULL),
    PRV_MAC(macSa,             "macSa",             NULL),
    PRV_MAC(sipBits127_32,     "sipBits127_32",     NULL),
    PRV_MAC(dipBits127_120,    "dipBits127_120",    NULL),
    PRV_MAC(udb,               "udb",               NULL),
    PRV_MAC(udb11Dup,          "udb11Dup",          NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L2_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)         \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                      \
    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L4_STC,         \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrExtendedIpv6L4[] =
{
    PRV_MAC(commonIngrExt,       "commonIngrExt",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrExt),
    PRV_MAC(commonIngrExtPkt,    "commonIngrExtPkt",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrExtPkt),
    PRV_MAC(isIpv6Eh,          "isIpv6Eh",          NULL),
    PRV_MAC(isIpv6HopByHop,    "isIpv6HopByHop",    NULL),
    PRV_MAC(sipBits127_32,     "sipBits127_32",     NULL),
    PRV_MAC(dip,               "dip",               NULL),
    PRV_MAC(udb,               "udb",               NULL),
    PRV_MAC(udb12Dup,          "udb12Dup",          NULL),
    PRV_MAC(udb13Dup,          "udb13Dup",          NULL),
    PRV_MAC(udb14Dup,          "udb14Dup",          NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_IPV6_L4_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose)             \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                            \
    CPSS_EXMXPM_PCL_RULE_FORMAT_COMMON_EGRESS_EXTENDED_STC, \
    _element_name, _element_expose, NULL)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonEgrExt[] =
{
    PRV_MAC(isIpv6,                         "isIpv6"),
    PRV_MAC(isArp,                          "isArp"),
    PRV_MAC(isL2Valid,                      "isL2Valid"),
    PRV_MAC(packetCmd,                      "packetCmd"),
    PRV_MAC(trgOrCpuCode0_6OrCosOrSniff,    "trgOrCpuCode0_6OrCosOrSniff"),
    PRV_MAC(srcTrgOrTxMirror,               "srcTrgOrTxMirror"),
    PRV_MAC(sourceId,                       "sourceId"),
    PRV_MAC(srcInfo,                        "srcInfo"),
    PRV_MAC(srcIsTrunk,                     "srcIsTrunk"),
    PRV_MAC(isIpOrMplsRouted,               "isIpOrMplsRouted"),
    PRV_MAC(egrFilterRegOrCpuCode7,         "egrFilterRegOrCpuCode7"),
    PRV_MAC(trgPort_Extrn,                  "trgPort_Extrn"),
    PRV_MAC(srcDev8_7,                      "srcDev8_7"),
    PRV_MAC(exp_Extrn,                      "exp_Extrn"),
    PRV_MAC(egrFilterReg,                   "egrFilterReg"),
    PRV_MAC(tcpUdpPortComparators,          "tcpUdpPortComparators"),
    PRV_MAC(sipBits31_0,                    "sipBits31_0"),
    PRV_MAC(ttl_Extrn,                      "ttl_Extrn"),
    PRV_MAC(tosBits1_0_Extrn,               "tosBits1_0_Extrn"),
    PRV_MAC(ipProtocol,                     "ipProtocol"),
    PRV_MAC(dscp,                           "dscp"),
    PRV_MAC(isL4Valid,                      "isL4Valid"),
    PRV_MAC(l4Byte0,                        "l4Byte0"),
    PRV_MAC(l4Byte1,                        "l4Byte1"),
    PRV_MAC(l4Byte2,                        "l4Byte2"),
    PRV_MAC(l4Byte3,                        "l4Byte3"),
    PRV_MAC(l4Byte13,                       "l4Byte13"),
    PRV_MAC(flowId_Extrn,                   "flowId_Extrn"),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)         \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                      \
    CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_STC,         \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_EgrExtendedNotIpv6[] =
{
    PRV_MAC(commonEgr,       "commonEgr",
        wrCpssExMxPmPclRuleFieldDsc_commonEgr),
    PRV_MAC(commonEgrExt,    "commonEgrExt",
        wrCpssExMxPmPclRuleFieldDsc_commonEgrExt),
    PRV_MAC(l2EncapType,       "l2EncapType",      NULL),
    PRV_MAC(ethType,           "ethType",          NULL),
    PRV_MAC(macDa,             "macDa",            NULL),
    PRV_MAC(macSa,             "macSa",            NULL),
    PRV_MAC(isIpv4Fragment,    "isIpv4Fragment",   NULL),
    PRV_MAC(ipv4Options_Ext,   "ipv4Options_Ext",  NULL),
    PRV_MAC(dipBits31_0,       "dipBits31_0",      NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_NOT_IPV6_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)         \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                      \
    CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L2_STC,          \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_EgrExtendedIpv6L2[] =
{
    PRV_MAC(commonEgr,       "commonEgr",
        wrCpssExMxPmPclRuleFieldDsc_commonEgr),
    PRV_MAC(commonEgrExt,    "commonEgrExt",
        wrCpssExMxPmPclRuleFieldDsc_commonEgrExt),
    PRV_MAC(macDa,             "macDa",            NULL),
    PRV_MAC(macSa,             "macSa",            NULL),
    PRV_MAC(isIpv6Eh_Extrn,    "isIpv6Eh_Extrn",   NULL),
    PRV_MAC(sipBits127_32,     "sipBits127_32",    NULL),
    PRV_MAC(dipBits127_120,    "dipBits127_120",   NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L2_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)         \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                      \
    CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L4_STC,          \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_EgrExtendedIpv6L4[] =
{
    PRV_MAC(commonEgr,       "commonEgr",
        wrCpssExMxPmPclRuleFieldDsc_commonEgr),
    PRV_MAC(commonEgrExt,    "commonEgrExt",
        wrCpssExMxPmPclRuleFieldDsc_commonEgrExt),
    PRV_MAC(isIpv6Eh_Extrn,    "isIpv6Eh_Extrn",   NULL),
    PRV_MAC(sipBits127_32,     "sipBits127_32",    NULL),
    PRV_MAC(dip,               "dip",              NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_EGRESS_EXTENDED_IPV6_L4_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose)             \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                            \
    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC,     \
    _element_name, _element_expose, NULL)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonIngrUdb[] =
{
    PRV_MAC(pktTagging,         "pktTagging"),
    PRV_MAC(l3OffsetInvalid,    "l3OffsetInvalid"),
    PRV_MAC(l4ProtocolType,     "l4ProtocolType"),
    PRV_MAC(pktType,            "pktType"),
    PRV_MAC(ipHeaderOk,         "ipHeaderOk"),
    PRV_MAC(macDaType,          "macDaType"),
    PRV_MAC(l4OffsetInalid,     "l4OffsetInalid"),
    PRV_MAC(l2Encapsulation,    "l2Encapsulation"),
    PRV_MAC(isIpv6Eh,           "isIpv6Eh"),
    PRV_MAC(isIpv6HopByHop,     "isIpv6HopByHop"),
    PRV_MAC(isUdbValid,         "isUdbValid"),
    PRV_MAC(isL2Valid,          "isL2Valid"),
    PRV_MAC(dscpOrExp,          "dscpOrExp"),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)         \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                      \
    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC,             \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrStandardUdb[] =
{
    PRV_MAC(commonIngrStd,    "commonIngrStd",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrStd),
    PRV_MAC(commonIngrUdb,    "commonIngrUdb",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrUdb),
    PRV_MAC(udb,             "udb",            NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)         \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                      \
    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC,             \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrExtendedUdb[] =
{
    PRV_MAC(commonIngrExt,    "commonIngrExt",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrExt),
    PRV_MAC(commonIngrUdb,    "commonIngrUdb",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrUdb),
    PRV_MAC(macSaOrSipBits79_32,     "macSaOrSipBits79_32",    NULL),
    PRV_MAC(macDaOrSipBits127_80,    "macDaOrSipBits127_80",   NULL),
    PRV_MAC(sipBits31_0,             "sipBits31_0",            NULL),
    PRV_MAC(dipBits31_0,             "dipBits31_0",            NULL),
    PRV_MAC(dipBits127_112,          "dipBits127_112",         NULL),
    PRV_MAC(udb,                     "udb",                    NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

#undef PRV_MAC
#define PRV_MAC(_element_name, _element_expose, _subtree_ptr)         \
    GT_UTIL_STC_ELEMENT_DSC_MAC(                                      \
    CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTERNAL_80B_STC,             \
    _element_name, _element_expose, _subtree_ptr)

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrExternalUdb80B[] =
{
    PRV_MAC(commonIngrExt,    "commonIngrExt",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrExt),
    PRV_MAC(commonIngrUdb,    "commonIngrUdb",
        wrCpssExMxPmPclRuleFieldDsc_commonIngrUdb),
    PRV_MAC(ethType,     "ethType",    NULL),
    PRV_MAC(macDa,       "macDa",      NULL),
    PRV_MAC(macSa,       "macSa",      NULL),
    PRV_MAC(sip,         "sip",        NULL),
    PRV_MAC(dip,         "dip",        NULL),
    PRV_MAC(l4Byte0,     "l4Byte0",    NULL),
    PRV_MAC(l4Byte1,     "l4Byte1",    NULL),
    PRV_MAC(l4Byte2,     "l4Byte2",    NULL),
    PRV_MAC(l4Byte3,     "l4Byte3",    NULL),
    PRV_MAC(l4Byte13,    "l4Byte13",   NULL),
    PRV_MAC(udb,         "udb",        NULL),
    PRV_PM_PCL_RULE_FMT_HOLDER_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_INGRESS_EXTERNAL_80B_E),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_Union[] =
{
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleIngrStandardNotIp, "ruleIngrStandardNotIp",
        wrCpssExMxPmPclRuleFieldDsc_IngrStandardNotIp),
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleIngrStandardIpL2Qos, "ruleIngrStandardIpL2Qos",
        wrCpssExMxPmPclRuleFieldDsc_IngrStandardIpL2Qos),
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleIngrStandardIpv4L4, "ruleIngrStandardIpv4L4",
        wrCpssExMxPmPclRuleFieldDsc_IngrStandardIpv4L4),
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleEgrStandardNotIp, "ruleEgrStandardNotIp",
        wrCpssExMxPmPclRuleFieldDsc_EgrStandardNotIp),
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleEgrStandardIpL2Qos, "ruleEgrStandardIpL2Qos",
        wrCpssExMxPmPclRuleFieldDsc_EgrStandardIpL2Qos),
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleEgrStandardIpv4L4, "ruleEgrStandardIpv4L4",
        wrCpssExMxPmPclRuleFieldDsc_EgrStandardIpv4L4),
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleIngrExtendedNotIpv6, "ruleIngrExtendedNotIpv6",
        wrCpssExMxPmPclRuleFieldDsc_IngrExtendedNotIpv6),
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleIngrExtendedIpv6L2, "ruleIngrExtendedIpv6L2",
        wrCpssExMxPmPclRuleFieldDsc_IngrExtendedIpv6L2),
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleIngrExtendedIpv6L4, "ruleIngrExtendedIpv6L4",
        wrCpssExMxPmPclRuleFieldDsc_IngrExtendedIpv6L4),
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleEgrExtendedNotIpv6, "ruleEgrExtendedNotIpv6",
        wrCpssExMxPmPclRuleFieldDsc_EgrExtendedNotIpv6),
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleEgrExtendedIpv6L2, "ruleEgrExtendedIpv6L2",
        wrCpssExMxPmPclRuleFieldDsc_EgrExtendedIpv6L2),
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleEgrExtendedIpv6L4, "ruleEgrExtendedIpv6L4",
        wrCpssExMxPmPclRuleFieldDsc_EgrExtendedIpv6L4),
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleIngrStandardUdb, "ruleIngrStandardUdb",
        wrCpssExMxPmPclRuleFieldDsc_IngrStandardUdb),
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleIngrExtendedUdb, "ruleIngrExtendedUdb",
        wrCpssExMxPmPclRuleFieldDsc_IngrExtendedUdb),
    GT_UTIL_STC_ELEMENT_DSC_MAC(
        CPSS_EXMXPM_PCL_RULE_FORMAT_UNT,
        ruleIngrExternalUdb80B, "ruleIngrExternalUdb80B",
        wrCpssExMxPmPclRuleFieldDsc_IngrExternalUdb80B),
    GT_UTIL_STC_ELEMENT_DSC_END_LIST_CNS
};

/*******************************************************************************
* wrCpssExMxPmPclRuleByFieldsSetNext
*
* DESCRIPTION:
*   The function accomulates the Policy Rule data to static
*   Mask and Pattern for the wrCpssExMxPmPclRuleByFieldsSetEnd
*   that calls cpssExMxPmPclRuleSet API.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       tcamType       - TCAM type: internal or external
*       ruleInfoPtr    - (pointer to) the Rule and Action Layouts
*       ruleFormat     - format of the Rule.
*
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclRuleByFieldsSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         rc;
    GT_U8                             *pathStr; /* the field path */
    GT_UTIL_HEX_DATA_STORE_FORMAT_ENT storeFormat;
    GT_U32                            length;   /* length in bytes */
    GT_U32                            offsetInField;
    GT_U8                             *maskDataStr;
    GT_U8                             *patternDataStr;
    GT_U32                            offset;
    GT_UTIL_STC_ELEMENT_DSC_STC       *foundDscPtr;

    pathStr        = (GT_U8*)                            inFields[0];
    storeFormat    = (GT_UTIL_HEX_DATA_STORE_FORMAT_ENT) inFields[1];
    length         = (GT_U32)                            inFields[2];
    offsetInField  = (GT_U32)                            inFields[3];
    maskDataStr    = (GT_U8*)                            inFields[4];
    patternDataStr = (GT_U8*)                            inFields[5];

    rc = utilFindStructureElenentDescriptorByPath(
        wrCpssExMxPmPclRuleFieldDsc_Union,
        0 /*dscEntrySize*/,
        '.' /*nameSeparator*/,
        pathStr, &foundDscPtr, &offset);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, GT_NOT_FOUND, "");
        return CMD_OK;
    }

    rc = utilHexStringToStcField(
        maskDataStr, length, storeFormat,
        ((GT_U8*)&maskData + offset + offsetInField));
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    rc = utilHexStringToStcField(
        patternDataStr, length, storeFormat,
        ((GT_U8*)&patternData + offset + offsetInField));
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmPclRuleByFieldsSetFirst
*
* DESCRIPTION:
*   The function accomulates the Policy Rule data to static
*   Mask and Pattern for the wrCpssExMxPmPclRuleByFieldsSetEnd
*   that calls cpssExMxPmPclRuleSet API.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       tcamType       - TCAM type: internal or external
*       ruleInfoPtr    - (pointer to) the Rule and Action Layouts
*       ruleFormat     - format of the Rule.
*
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclRuleByFieldsSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         rc = GT_OK;
    GT_U8                             *pathStr; /* the field path */
    GT_U8                             formatPath[PRV_PM_PCL_RULE_FMT_PATH_SIZE_CNS];
    GT_U32                            fmtPathMaxLen;
    GT_U32                            fmtPathLen;
    GT_U32                            fmtPathPostfixLen;
    GT_U32                            offset;
    GT_UTIL_STC_ELEMENT_DSC_STC       *foundDscPtr;

    pathStr        = (GT_U8*)                            inFields[0];

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_EXMXPM_PCL_RULE_FORMAT_UNT));

    /* detect the rule format by PASS */
    fmtPathPostfixLen = cmdOsStrlen(PRV_PM_PCL_RULE_FMT_NAME_CNS);
    fmtPathMaxLen =
        PRV_PM_PCL_RULE_FMT_PATH_SIZE_CNS - 2 - fmtPathPostfixLen;

    /* calculate first name length */
    for (fmtPathLen = 0;
          ((pathStr[fmtPathLen] != '.') && (pathStr[fmtPathLen] != 0));
          fmtPathLen++) {};

    if (fmtPathLen > fmtPathMaxLen)
    {
        galtisOutput(outArgs, GT_NOT_FOUND, "");
        return CMD_OK;
    }

    /* compose path to search format */
    cmdOsMemCpy(&(formatPath[0]), pathStr, fmtPathLen);
    formatPath[fmtPathLen] = '.';
    cmdOsMemCpy(
        &(formatPath[fmtPathLen + 1 ]),
        PRV_PM_PCL_RULE_FMT_NAME_CNS, fmtPathPostfixLen);
    formatPath[fmtPathLen + 1 + fmtPathPostfixLen] = 0;

    rc = utilFindStructureElenentDescriptorByPath(
        wrCpssExMxPmPclRuleFieldDsc_Union,
        0 /*dscEntrySize*/,
        '.' /*nameSeparator*/,
        formatPath, &foundDscPtr, &offset);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, GT_NOT_FOUND, "");
        return CMD_OK;
    }

    ruleFormat =
        (CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT)foundDscPtr->offset;

    return wrCpssExMxPmPclRuleByFieldsSetNext(
        inArgs, inFields, numFields, outArgs);
}

/*******************************************************************************
* wrCpssExMxPmPclRuleByFieldsSetEnd
*
* DESCRIPTION:
*   The function sets the Policy Rule.
*   Mask and Pattern contents accomulated by previous First and Next calls.
*   Binds external Tcam configs if
*   tcamType==CPSS_EXMXPM_TCAM_TYPE_EXTERNAL_E.
*   Calls cpssExMxPmPclRuleSet API.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       tcamType       - TCAM type: internal or external
*       ruleInfoPtr    - (pointer to) the Rule and Action Layouts
*       ruleFormat     - format of the Rule.
*
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer value
*       GT_BAD_STATE             - on found rule size
*                                  not matches the "ruleFormat"
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPclRuleByFieldsSetEnd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                  devNum;
    GT_U32                                 ruleIndex;
    GT_U32                                 index;
    GT_BOOL                                valid;
    CPSS_EXMXPM_TCAM_TYPE_ENT              tcamType;
    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT  ruleInfo;
    WR_PCL_ACTION_STC                      *tablePtr;
    CPSS_EXMXPM_PCL_ACTION_UNT             *actionPtr;
    GT_STATUS                              result=GT_OK;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

     /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];
    tcamType  =(CPSS_EXMXPM_TCAM_TYPE_ENT)inArgs[1];
    ruleIndex =(GT_U32)inArgs[2];
    valid     = (GT_BOOL)inArgs[3];

    if(tcamType==CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E)
    {
       ruleInfo.internalTcamRuleStartIndex = ruleIndex;
       tablePtr = actionIntTable;
    }
    else
    {   /*bind extTcam params*/
       wrCpssExMxPmPclExtConfigIndexGet(devNum, ruleIndex, &index);
       if (index == INVALID_RULE_INDEX)
       {
           galtisOutput(outArgs, GT_NOT_FOUND, "");
           return CMD_OK;
       }

       ruleInfo.externalTcamInfo.ruleInfo.tcamId=extTcamConfigPclTable[index].tcamId;
       ruleInfo.externalTcamInfo.ruleInfo.ruleStartIndex=extTcamConfigPclTable[index].ruleStartIndex;
       ruleInfo.externalTcamInfo.ruleInfo.ruleSize=extTcamConfigPclTable[index].ruleSize;
       ruleInfo.externalTcamInfo.ruleInfo.ageEnable=extTcamConfigPclTable[index].ageEnable;

       ruleInfo.externalTcamInfo.actionInfo.actionStartIndex=extTcamConfigPclTable[index].actionStartIndex;
       ruleInfo.externalTcamInfo.actionInfo.memoryType=extTcamConfigPclTable[index].memoryType;
       tablePtr = actionExtTable;
    }


    /* Get the action */
    wrCpssExMxPmPclActionTableIndexGet(devNum, ruleIndex, tablePtr, &index);
    if (index == INVALID_RULE_INDEX)
    {
        galtisOutput(outArgs, GT_NOT_FOUND, "");
        return CMD_OK;
    }

    actionPtr = &(tablePtr[index].actionEntry);

    /* call cpss api function */
    result = cpssExMxPmPclRuleSet(
        devNum, tcamType, &ruleInfo,
        ruleFormat, tablePtr[index].actionFormat, valid,
        &maskData, &patternData, actionPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxPmPclInit",
        &wrCpssExMxPmPclInit,
        1, 0},

    {"cpssExMxPmPclPolicyEnableSet",
        &wrCpssExMxPmPclPolicyEnableSet,
        3, 0},

    {"cpssExMxPmPclPolicyEnableGet",
        &wrCpssExMxPmPclPolicyEnableGet,
        2, 0},

    {"cpssExMxPmPclPortIngressPolicyEnableSet",
        &wrCpssExMxPmPclPortIngressPolicyEnableSet,
        3, 0},

    {"cpssExMxPmPclPortIngressPolicyEnableGet",
        &wrCpssExMxPmPclPortIngressPolicyEnableGet,
        2, 0},

    {"cpssExMxPmPclPortLookupCfgTblAccessModeSet",
        &wrCpssExMxPmPclPortLookupCfgTblAccessModeSet,
        5, 0},

    {"cpssExMxPmPclPortLookupCfgTblAccessModeGet",
        &wrCpssExMxPmPclPortLookupCfgTblAccessModeGet,
        4, 0},

    /* --- cpssExMxPmPclCfgTblAccessMode    Table */

    {"cpssExMxPmPclCfgTblAccessModeSet",
        &wrCpssExMxPmPclCfgTblAccessModeSet,
        2, 3},

    {"cpssExMxPmPclCfgTblAccessModeGetFirst",
        &wrCpssExMxPmPclCfgTblAccessModeGetFirst,
        2, 0},

    {"cpssExMxPmPclCfgTblAccessModeGetNext",
        &wrCpssExMxPmPclCfgTblAccessModeGetNext,
        2, 0},

    /* --- cpssExMxPmPclCfgTblAccessMode    Table */

    {"cpssExMxPmPclEgressPortPacketTypeSet",
        &wrCpssExMxPmPclEgressPortPacketTypeSet,
        4, 0},

    {"cpssExMxPmPclEgressPortPacketTypeGet",
        &wrCpssExMxPmPclEgressPortPacketTypeGet,
        3, 0},

    {"cpssExMxPmPclRuleMatchCounterGet",
        &wrCpssExMxPmPclRuleMatchCounterGet,
        2, 0},

    {"cpssExMxPmPclRuleMatchCounterSet",
        &wrCpssExMxPmPclRuleMatchCounterSet,
        3, 0},

    {"cpssExMxPmPclRuleStateGet",
        &wrCpssExMxPmPclRuleStateGet,
        3, 0},
    /* --- cpssExMxPmPclAction Table --- */

    {"cpssExMxPmPclActionClear",
        &wrCpssExMxPmPclActionClear,
        0, 0},

    {"cpssExMxPmPclActionDelete",
        &wrCpssExMxPmPclActionDelete,
        0, 2},

    { "cpssExMxPmPclActionGetFirst",
        &wrCpssExMxPmPclActionGetFirst,
        0, 0},

    {"cpssExMxPmPclActionGetNext",
        &wrCpssExMxPmPclActionGetNext,
        0, 0},

    {"cpssExMxPmPclActionSet",
        &wrCpssExMxPmPclActionSet,
        0, 41},

    /* --- cpssExMxPmPclIngressRule Table --- */

    {"cpssExMxPmPclIngressRuleSetFirst",
        &wrCpssExMxPmPclIngressRuleWriteFirst_patched,
        2, 30},

    {"cpssExMxPmPclIngressRuleSetNext",
        &wrCpssExMxPmPclIngressRuleWriteNext_patched,
        2, 30},

    {"cpssExMxPmPclIngressRuleEndSet",
        &wrCpssExMxPmPclIngressRuleWriteEnd,
        2, 0},

    /* --- cpssExMxPmPclIngressRuleExt Table --- */
    /* this is the updated cpssExMxPmPclIngressRule table*/

    {"cpssExMxPmPclIngressRuleExtSetFirst",
        &wrCpssExMxPmPclIngressRuleWriteFirst,
        2, 30},

    {"cpssExMxPmPclIngressRuleExtSetNext",
        &wrCpssExMxPmPclIngressRuleWriteNext,
        2, 30},

    {"cpssExMxPmPclIngressRuleExtEndSet",
        &wrCpssExMxPmPclIngressRuleWriteEnd,
        2, 0},


    /* --- cpssExMxPmPclEgressRule Table --- */

    {"cpssExMxPmPclEgressRuleSetFirst",
        &wrCpssExMxPmPclEgressRuleWriteFirst,
        2, 45},

    {"cpssExMxPmPclEgressRuleSetNext",
        &wrCpssExMxPmPclEgressRuleWriteNext,
        2, 45},

    {"cpssExMxPmPclEgressRuleEndSet",
        &wrCpssExMxPmPclEgressRuleWriteEnd,
        2, 0},

    {"cpssExMxPmPclRuleInvalidate",
        &wrCpssExMxPmPclRuleInvalidate,
        3, 0},

    {"cpssExMxPmPclRuleValidStatusSet",
        &wrCpssExMxPmPclRuleValidStatusSet,
        4, 0},

    {"cpssExMxPmPclRuleCopy",
        &wrCpssExMxPmPclRuleCopy,
        4, 0},

    {"cpssExMxPmPclCfgTblSet",
        &wrCpssExMxPmPclCfgTblSet,
        9, 5},

    {"cpssExMxPmPclCfgTblGetFirst",
        &wrCpssExMxPmPclCfgTblGet,
        9, 0/*5*/},

    {"cpssExMxPmPclCfgTblGetNext",
        &wrCpssExMxPmPclCfgTblGet,
        9, 0/*5*/},

    /* cpssExMxPmPclCfgTblV1 */
    {"cpssExMxPmPclCfgTblV1Set",
        &wrCpssExMxPmPclCfgTblV1Set,
        9, 7},

    {"cpssExMxPmPclCfgTblV1GetFirst",
        &wrCpssExMxPmPclCfgTblV1Get,
        9, 0/*7*/},

    {"cpssExMxPmPclCfgTblV1GetNext",
        &wrCpssExMxPmPclCfgTblV1Get,
        9, 0/*7*/},

    /* cpssExMxPmPclCfgTblV2 - different GUI only */
    {"cpssExMxPmPclCfgTblV2Set",
        &wrCpssExMxPmPclCfgTblV1Set,
        9, 7},

    {"cpssExMxPmPclCfgTblV2GetFirst",
        &wrCpssExMxPmPclCfgTblV1Get,
        9, 0/*7*/},

    {"cpssExMxPmPclCfgTblV2GetNext",
        &wrCpssExMxPmPclCfgTblV1Get,
        9, 0/*7*/},

    {"cpssExMxPmPclUserDefinedByteSet",
        &wrCpssExMxPmPclUserDefinedByteSet,
        5, 0},

    {"cpssExMxPmPclUserDefinedByteGet",
        &wrCpssExMxPmPclUserDefinedByteGet,
        3, 0},

    {"cpssExMxPmPclInvalidUdbCmdSet",
        &wrCpssExMxPmPclInvalidUdbCmdSet,
        2, 0},

    {"cpssExMxPmPclInvalidUdbCmdGet",
        &wrCpssExMxPmPclInvalidUdbCmdGet,
        1, 0},

    {"cpssExMxPmPclTwoLookupsCpuCodeResolutionSet",
        &wrCpssExMxPmPclTwoLookupsCpuCodeResolutionSet,
        2, 0},

    {"cpssExMxPmPclTwoLookupsCpuCodeResolutionGet",
        &wrCpssExMxPmPclTwoLookupsCpuCodeResolutionGet,
        1, 0},

    {"cpssExMxPmPclTcpUdpPortComparatorSet",
        &wrCpssExMxPmPclTcpUdpPortComparatorSet,
        7, 0},

    {"cpssExMxPmPclTcpUdpPortComparatorGet",
        &wrCpssExMxPmPclTcpUdpPortComparatorGet,
        4, 0},
    /***********************************puma 2 specific api wrappers****************/
    {"cpssExMxPm2PclUserDefinedEtherTypeSet",
         &wrCpssExMxPmPclUserDefinedEtherTypeSet,
         2, 0},
    {"cpssExMxPm2PclUserDefinedEtherTypeGet",
         &wrCpssExMxPmPclUserDefinedEtherTypeGet,
         1, 0},
     {"cpssExMxPm2PclProfileAssignmentModeSet",
         &wrCpssExMxPmPclProfileAssignmentModeSet,
         2, 0},
     {"cpssExMxPm2PclProfileAssignmentModeGet",
         &wrCpssExMxPmPclProfileAssignmentModeGet,
         1, 0},
     {"cpssExMxPm2PclEgressLookupTypeSet",
         &wrCpssExMxPmPclEgressLookupTypeSet,
         4, 0},
     {"cpssExMxPm2PclEgressLookupTypeGet",
         &wrCpssExMxPmPclEgressLookupTypeGet,
         1, 0},
      {"cpssExMxPm2PclIngressPolicyEngineCfgSet",
         &wrCpssExMxPmPclIngressPolicyEngineCfgSet,
         7, 0},
      {"cpssExMxPm2PclIngressPolicyEngineCfgGet",
         &wrCpssExMxPmPclIngressPolicyEngineCfgGet,
         1, 0},
      {"cpssExMxPm2PclProfileIdTblSet",
         &wrCpssExMxPmPclProfileIdTblSet,
         10, 0},
      {"cpssExMxPm2PclProfileIdTblGet",
         &wrCpssExMxPmPclProfileIdTblGet,
         9, 0},
      {"utilCpssExMxPmPclExtTcamUseSet",
         &wrUtilCpssExMxPmPclExtTcamUseSet,
         1,0},
      {"utilCpssExMxPmPclExtTcamCfgDevNumSet",
         &wrUtilCpssExMxPmPclExtTcamCfgDevNumSet,
         1,0},
      {"cpssExMxPmPclExtTcamConfigSet",
         &wrCpssExMxPmPclExtConfigSet,
         0,6},
      {"cpssExMxPmPclExtTcamConfigGetFirst",
         &wrCpssExMxPmPclExtConfigGetFirst,
         0,0},
      {"cpssExMxPmPclExtTcamConfigGetNext",
         &wrCpssExMxPmPclExtConfigGet,
         0,0},

      {"cpssExMxPm2PclActionSet",
        &wrCpssExMxPm2PclActionSet,
        5, 41},
      {"cpssExMxPm2PclActionGetFirst",
        &wrcpssExMxPm2PclRuleActionGetEntry,
        4, 0},
      {"cpssExMxPm2PclActionGetNext",
        &wrCpssExMxPmPclRuleActionGetNext,
        0, 0},

      /* IngressStd rule table */
      {"cpssExMxPm2PclIngressStdRuleSetFirst",
        &wrCpssExMxPm2PclIngressStdRuleWriteFirst,
        5, 30},

      {"cpssExMxPm2PclIngressStdRuleSetNext",
        &wrCpssExMxPm2PclIngressStdRuleWriteNext,
        5, 30},

      {"cpssExMxPm2PclIngressStdRuleEndSet",
        &wrCpssExMxPmPclIngressRuleWriteEnd,
        5, 0},

      {"cpssExMxPm2PclIngressStdRuleGetFirst",
        &wrCpssExMxPm2PclIngressStdRuleReadFirst,
        5, 0},

      {"cpssExMxPm2PclIngressStdRuleGetNext",
        &wrCpssExMxPm2PclIngressStdRuleReadNext,
        5, 0},

      /* UserDefind Byte table */
      {"cpssExMxPmPclUdbTableEntryV1SetFirst",
        &wrCpssExMxPmPclUdbTableEntryV1SetFirst,
        2, 5},

      {"cpssExMxPmPclUdbTableEntryV1SetNext",
        &wrCpssExMxPmPclUdbTableEntryV1SetNext,
        2, 5},

      {"cpssExMxPmPclUdbTableEntryV1EndSet",
        &wrCpssExMxPmPclUdbTableEntryV1SetEnd,
        2, 0},

      {"cpssExMxPmPclUdbTableEntryV1GetFirst",
        &wrCpssExMxPmPclUdbTableEntryV1GetFirst,
        2, 0},

      {"cpssExMxPmPclUdbTableEntryV1GetNext",
        &wrCpssExMxPmPclUdbTableEntryV1GetNext,
        2, 0},

      /* rule by fields table */
      {"cpssExMxPmPclRuleByFieldsSetFirst",
        &wrCpssExMxPmPclRuleByFieldsSetFirst,
        4, 6},

      {"cpssExMxPmPclRuleByFieldsSetNext",
        &wrCpssExMxPmPclRuleByFieldsSetNext,
        4, 6},

      {"cpssExMxPmPclRuleByFieldsEndSet",
        &wrCpssExMxPmPclRuleByFieldsSetEnd,
        4, 0},


    /* CMD_END */

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))


/*******************************************************************************
* cmdLibInitCpssExMxPmPcl
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
GT_STATUS cmdLibInitCpssExMxPmPcl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


