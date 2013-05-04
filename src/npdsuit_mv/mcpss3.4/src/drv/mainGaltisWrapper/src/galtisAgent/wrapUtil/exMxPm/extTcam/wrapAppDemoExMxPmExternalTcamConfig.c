/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapAppDemoExMxPmExternalTcamConfig.c
*
* DESCRIPTION:
*       wrappers for appDemoExMxPmExternalTcamConfig.c
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

/*******************************************************************************
* appDemoExMxPmExternalTcamRuleIndexByLenAndLookupGet
*
* DESCRIPTION:
*       The function gets the rule start index and the maximum number of
*       rules that describe the external TCAM rule area appropriate to
*       a rule size and lookup number combination.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       ruleSize            - size of rule entry.
*       lookupIndex         - the number of lookup.
*
* OUTPUTS:
*       ruleStartIndexPtr   - (pointer to) rule start index number of the area.
*       maxRuleNumPtr       - (pointer to) the maximal number of rules that can
*                             be inserted into the area.
*       linesPerRulePtr     - (pointer to) how many external TCAM lines a rule
*                             of ruleSize type consumes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrAppDemoExMxPmExternalTcamRuleIndexByLenAndLookupGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT ruleSize;
    GT_U32 lookupIndex;
    GT_U32 ruleStartIndex;
    GT_U32 maxRuleNum;
    GT_U32 linesPerRule;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT)inArgs[1];
    lookupIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cmdAppExMxPmExternalTcamRuleIndexByLenAndLookupGet(devNum, ruleSize, lookupIndex, &ruleStartIndex, &maxRuleNum, &linesPerRule);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", ruleStartIndex, maxRuleNum, linesPerRule);

    return CMD_OK;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamActionIndexGet
*
* DESCRIPTION:
*       The function gets the line in control memory area allocated for external
*       TCAM actions for a defined rule line in external TCAM.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       ruleSize            - size of rule entry.
*       lookupIndex         - the number of lookup.
*       ruleStartIndex      - rule start index.
*
* OUTPUTS:
*       actionStartIndexPtr - (pointer to) action start index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrAppDemoExMxPmExternalTcamActionIndexGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT ruleSize;
    GT_U32 lookupIndex;
    GT_U32 ruleStartIndex;
    GT_U32 actionStartIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT)inArgs[1];
    lookupIndex = (GT_U32)inArgs[2];
    ruleStartIndex = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cmdAppExMxPmExternalTcamActionIndexGet(devNum, ruleSize, lookupIndex, ruleStartIndex, &actionStartIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", actionStartIndex);

    return CMD_OK;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupSet
*
* DESCRIPTION:
*       This function sets the value of a number of rules requested for a
*       device, client, key and lookup combination later to be used and
*       overwrite the default system value.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       tcamClient          - EXMXPM device External TCAM unit client.
*       clientKeyType       - The selected key type enumeration per client:
*                             please refer to Functional Spec for key types number.
*       lookupIndex         - The number of lookup.
*       requestedRulesNum   - the number of rules requested (will overwrite the
*                             default value).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NO_RESOURCE           - no place to save the request
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to effect the system external TCAM configuration this
*       function must be called before cpssInitSystem!!!
*
*******************************************************************************/
static CMD_STATUS wrAppDemoExMxPmExternalTcamClientKeyLookupSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT tcamClient;
    GT_U32 clientKeyType;
    GT_U32 lookupIndex;
    GT_U32 requestedRulesNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tcamClient = (CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT)inArgs[1];
    clientKeyType = (GT_U32)inArgs[2];
    lookupIndex = (GT_U32)inArgs[3];
    requestedRulesNum = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cmdAppExMxPmExternalTcamClientKeyLookupSet(devNum, tcamClient, clientKeyType, lookupIndex, requestedRulesNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupGet
*
* DESCRIPTION:
*       This function gets the value of a number of rules requested for a
*       device, client, key and lookup combination later to be used and
*       overwrite the default system value.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                  - device number
*       tcamClient              - EXMXPM device External TCAM unit client.
*       clientKeyType           - The selected key type enumeration per client:
*                                 please refer to Functional Spec for key types number.
*       lookupIndex             - The number of lookup.
*
* OUTPUTS:
*       requestedRulesNumPtr    - (pointer to) the number of rules requested.
*       validPtr                - (pointer to) indication whether a match to the
*                                 device, client, key and lookup was found.
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrAppDemoExMxPmExternalTcamClientKeyLookupGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT tcamClient;
    GT_U32 clientKeyType;
    GT_U32 lookupIndex;
    GT_U32 requestedRulesNum;
    GT_BOOL valid;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tcamClient = (CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT)inArgs[1];
    clientKeyType = (GT_U32)inArgs[2];
    lookupIndex = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cmdAppExMxPmExternalTcamClientKeyLookupGet(devNum, tcamClient, clientKeyType, lookupIndex, &requestedRulesNum, &valid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", requestedRulesNum, valid);

    return CMD_OK;
}
/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupSetByIndex
*
* DESCRIPTION:
*       This function sets an entry (specified by index) in the database
*       of values to overwrite the default of maximum number of rules for
*       device, client, key and lookup combination.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       entryIndex  - entry index in database.
*       entryPtr    - (pointer to) database entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrAppDemoExMxPmExternalTcamClientKeyLookupSetByIndex
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 entryIndex;
    APP_DEMO_EXTERNAL_TCAM_OVERWRITTEN_DEFAULT_STC entry;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    entryIndex=(GT_U32)inFields[0];
    entry.devNum=(GT_U8)inFields[1];
    entry.tcamClient=(CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT)inFields[2];
    entry.clientKeyType=(GT_U32)inFields[3];
    entry.lookupIndex=(GT_U32)inFields[4];
    entry.requestedRulesNum=(GT_U32)inFields[5];
    entry.inUse=(GT_U32)inFields[6];


    /* call cpss api function */
    result = cmdAppExMxPmExternalTcamClientKeyLookupSetByIndex(entryIndex, &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
static GT_U32 entryIndex;
static GT_U32 minEntryIndex;
static GT_U32 maxEntryIndex;
/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupGetByIndex
*
* DESCRIPTION:
*       This function gets an entry (specified by index) from the database
*       of values to overwrite the default of maximum number of rules for
*       device, client, key and lookup combination.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       entryIndex  - entry index in database.
*
* OUTPUTS:
*       entryPtr    - (pointer to) database entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrAppDemoExMxPmExternalTcamClientKeyLookupGetByIndex
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    APP_DEMO_EXTERNAL_TCAM_OVERWRITTEN_DEFAULT_STC entry;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (entryIndex>maxEntryIndex)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cmdAppExMxPmExternalTcamClientKeyLookupGetByIndex(entryIndex, &entry);

    if (result != GT_OK)
   {
       galtisOutput(outArgs, result, "");
       return CMD_OK;
   }

    inFields[0]=entryIndex;
    inFields[1]=entry.devNum;
    inFields[2]=entry.tcamClient;
    inFields[3]=entry.clientKeyType;
    inFields[4]=entry.lookupIndex;
    inFields[5]=entry.requestedRulesNum;
    inFields[6]=entry.inUse;

     /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d",inFields[0], inFields[1], inFields[2],
                 inFields[3],inFields[4], inFields[5],inFields[6]);



    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}
/********************************************************************************/
static CMD_STATUS wrAppDemoExMxPmExternalTcamClientKeyLookupGetByIndexGetFirst
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


   minEntryIndex=(GT_U32)inArgs[0];
   maxEntryIndex=(GT_U32)inArgs[1];
   entryIndex=minEntryIndex;

   return wrAppDemoExMxPmExternalTcamClientKeyLookupGetByIndex(inArgs,inFields,numFields,outArgs);



}
/*******************************************************************************/
static CMD_STATUS wrAppDemoExMxPmExternalTcamClientKeyLookupGetByIndexGetNext
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

    entryIndex++;

    return wrAppDemoExMxPmExternalTcamClientKeyLookupGetByIndex(inArgs,inFields,numFields,outArgs);
}
/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

        {"appDemoExMxPmExternalTcmRuleIndexByLenAndLookupGet",
         &wrAppDemoExMxPmExternalTcamRuleIndexByLenAndLookupGet,
         3, 0},
        {"appDemoExMxPmExternalTcamActionIndexGet",
         &wrAppDemoExMxPmExternalTcamActionIndexGet,
         4, 0},
        {"appDemoExMxPmExternalTcamClientKeyLookupSet",
         &wrAppDemoExMxPmExternalTcamClientKeyLookupSet,
         5, 0},
        {"appDemoExMxPmExternalTcamClientKeyLookupGet",
         &wrAppDemoExMxPmExternalTcamClientKeyLookupGet,
         4, 0},
        {"appDemoExMxPmExternalTcamRuleNumSet",
         &wrAppDemoExMxPmExternalTcamClientKeyLookupSetByIndex,
         0, 7},
        {"appDemoExMxPmExternalTcamRuleNumGetFirst",
         &wrAppDemoExMxPmExternalTcamClientKeyLookupGetByIndexGetFirst,
         2, 0},
        {"appDemoExMxPmExternalTcamRuleNumGetNext",
         &wrAppDemoExMxPmExternalTcamClientKeyLookupGetByIndexGetNext,
         0, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitAppDemoExMxPmExternalTcamConfig
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
GT_STATUS cmdLibInitAppDemoExMxPmExternalTcamConfig
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

