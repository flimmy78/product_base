/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPcl.c
*
* DESCRIPTION:
*       Wrapper functions for Pcl cpss functions
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/


/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/exMx/exMxGen/pcl/cpssExMxPcl.h>


/*******************************************************************************
* cpssExMxPclInit
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
static CMD_STATUS wrCpssExMxPclInit
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
    result = cpssExMxPclInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPclTcamFlush
*
* DESCRIPTION:
*   The function resets the TCAM to the same state
*   as after power-on and initialization:
*    - all entries are set to Standard rules - relevant only for 98EX1x6.
*    - all entries are set to INVALID state.
*   The IP MC rules also reseted.
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
*       For 98EX1X5 the flush implemented by optimal SW algorithm
*       For 98EX1X6, 98MX6x5 and 98MX6x8 the flush implemented by ASIC
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPclTcamFlush
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
    result = cpssExMxPclTcamFlush(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/******************************************************************************
* Table: PclTemplate
*
* Description:
*     pcl template configuration
*
* Fields:
*      offsetType   - the type of offset (see CPSS_EXMX_PCL_OFFSET_TYPE_ENT)
*      offset       - The offset of the user-defined byte, in bytes,from the
*                     place in the packet indicated by the offset type
*
*
* Comments:
*
* Profiles and templates number:
*   98EX1x6 may work in one of three modes of profile
*   set by cpssExMxTgPclProfileSizeSet:
*    - 8 templates per profile, 2 profiles per device.
*      In this mode first seven template types including
*      CPSS_EXMX_PCL_TEMPLATE_IPV6_E are used to configure profile.
*    - 4 templates per profile, 4 profiles per device.
*      In this mode only 4 templates are used:
*        CPSS_EXMX_PCL_TEMPLATE_IPV4_E, CPSS_EXMX_PCL_TEMPLATE_IPV6_E
*        CPSS_EXMX_PCL_TEMPLATE_MPLS_E and CPSS_EXMX_PCL_TEMPLATE_ETH_OTHER_E
*    - 1 template per profile, 16 profiles per device.
*      In this mode only CPSS_EXMX_PCL_TEMPLATE_ANY_E is used.
*
*   98EX1x5, 98MX6x5 and 98MX6x8 may work in the only mode of profile
*    - 8 templates per profile, 2 profiles per device.
*      In this mode first seven template types including
*      CPSS_EXMX_PCL_TEMPLATE_IPV6_E are used to configure profile.
*
* Include VID, InLif number and Profile Id:
*   98EX1x6
*     - InLif number replaces user-defined bytes 13 and 14.
*     - Include VID not supported
*     - Profile id replaces user-defined byte 15
*   98EX1x5
*     - InLif number or VID are two bytes placed in the key in the UDBs after
*       last ones defined by udbAmount.
*     - The sum of VID/InLif number two bytes and number of used-defined bytes
*       should be less then or equal 16.
*     - Include Profile Id not supported
*
*/

/* table PclTemplate global variables */

static GT_U8                                devNum;
static GT_U8                                profileId;
static CPSS_EXMX_PCL_TEMPLATE_ENT           keyTemplate;
static GT_U32                               udbAmount;
static GT_U8                                udbCount;
static CPSS_EXMX_PCL_UDB_CFG_STC            *udbCfg = NULL;
static CPSS_EXMX_PCL_INCLUDE_VLAN_INLIF_ENT includeVlanOrInLifNum;
static GT_BOOL                              includeProfileId;


/*******************************************************************************
* cpssExMxPclTemplateSetFirst
*
* DESCRIPTION:
*   The function configures the template of the profile
*
*
* INPUTS:
*       devNum                - device number
*       profileId             - number of profile (as must be written to LIF)
*       keyTemplate           - the key template secified by packet type
*       includeVlanOrInLifNum - include InLif number or VLAN id flag to UDBs
*                               of the key. See comments below.
*       includeProfileId      - GT_TRUE - include profile Id (8bit) to UDB
*                                         of the key. see comments below.
*                               GT_FALSE - not include profile Id (8bit) to UDB
*                                          of the key.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*     GT_OK       - on success
*     GT_FAIL     - on failure
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
* COMMENTS:
*
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPclTemplateSetFirst
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

    devNum = (GT_U8)inArgs[0];
    profileId = (GT_U8)inArgs[1];
    keyTemplate = (CPSS_EXMX_PCL_TEMPLATE_ENT)inArgs[2];
    includeVlanOrInLifNum = (CPSS_EXMX_PCL_INCLUDE_VLAN_INLIF_ENT)inArgs[3];
    includeProfileId = (GT_BOOL)inArgs[4];

    {
        if(numFields < 2)
            return CMD_FIELD_UNDERFLOW;
        if(numFields > 2)
            return CMD_FIELD_OVERFLOW;

        if (udbCfg != NULL)
        {
            cmdOsFree(udbCfg);
            udbCfg = NULL;
        }

        udbCfg = cmdOsMalloc(sizeof(CPSS_EXMX_PCL_UDB_CFG_STC));

        if (udbCfg == NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }

        udbCount = 0;

        udbCfg[udbCount].offsetType =(CPSS_EXMX_PCL_OFFSET_TYPE_ENT)inFields[0];
        udbCfg[udbCount].offset = (GT_U8)inFields[1];

        udbCount++ ;
        galtisOutput(outArgs, GT_OK, "");
    }
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPclTemplateSetNext
*
* DESCRIPTION:
*   The function configures the template of the profile
*
*
* INPUTS:
*       devNum                - device number
*       profileId             - number of profile (as must be written to LIF)
*       keyTemplate           - the key template secified by packet type
*       udbAmount             - amount of User Defined Bytes to configure
*       includeVlanOrInLifNum - include InLif number or VLAN id flag to UDBs
*                               of the key. See comments below.
*       includeProfileId      - GT_TRUE - include profile Id (8bit) to UDB
*                                         of the key. see comments below.
*                               GT_FALSE - not include profile Id (8bit) to UDB
*                                          of the key.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*     GT_OK       - on success
*     GT_FAIL     - on failure
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
* COMMENTS:
*
*     none
*
*******************************************************************************/

static CMD_STATUS wrCpssExMxPclTemplateSetNext

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

    devNum = (GT_U8)inArgs[0];
    profileId = (GT_U8)inArgs[1];
    keyTemplate = (CPSS_EXMX_PCL_TEMPLATE_ENT)inArgs[2];
    includeVlanOrInLifNum = (CPSS_EXMX_PCL_INCLUDE_VLAN_INLIF_ENT)inArgs[3];
    includeProfileId = (GT_BOOL)inArgs[4];

    {
        if(numFields < 2)
            return CMD_FIELD_UNDERFLOW;
        if(numFields > 2)
            return CMD_FIELD_OVERFLOW;


        if (udbCount == 0)
        {
            galtisOutput(outArgs, GT_FAIL, "");
            return CMD_OK;
        }

        if (udbCfg == NULL)
        {
            galtisOutput(outArgs, GT_FAIL, "");
            return CMD_OK;
        }

        udbCfg = cmdOsRealloc(udbCfg, (udbCount + 1) *
                 sizeof(CPSS_EXMX_PCL_UDB_CFG_STC));

        if (udbCfg == NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }


        udbCfg[udbCount].offsetType =(CPSS_EXMX_PCL_OFFSET_TYPE_ENT)inFields[0];
        udbCfg[udbCount].offset = (GT_U8)inFields[1];

        udbCount++ ;
        galtisOutput(outArgs, GT_OK, "");
    }
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPclTemplateEndSet
*
* DESCRIPTION:
*   The function configures the template of the profile
*
*
* INPUTS:
*       devNum                - device number
*       profileId             - number of profile (as must be written to LIF)
*       keyTemplate           - the key template secified by packet type
*       udbAmount             - amount of User Defined Bytes to configure
*       includeVlanOrInLifNum - include InLif number or VLAN id flag to UDBs
*                               of the key. See comments below.
*       includeProfileId      - GT_TRUE - include profile Id (8bit) to UDB
*                                         of the key. see comments below.
*                               GT_FALSE - not include profile Id (8bit) to UDB
*                                          of the key.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*     GT_OK       - on success
*     GT_FAIL     - on failure
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
* COMMENTS:
*
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPclTemplateEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    profileId = (GT_U8)inArgs[1];
    keyTemplate = (CPSS_EXMX_PCL_TEMPLATE_ENT)inArgs[2];
    includeVlanOrInLifNum = (CPSS_EXMX_PCL_INCLUDE_VLAN_INLIF_ENT)inArgs[3];
    includeProfileId = (GT_BOOL)inArgs[4];

    udbAmount = udbCount;
    {
        if (udbCount == 0)
        {
            galtisOutput(outArgs, GT_OK, "");
            return CMD_OK;
        }
        if (udbCfg == NULL)
        {
            galtisOutput(outArgs, GT_FAIL, "");
            return CMD_OK;
        }

        status = cpssExMxPclTemplateSet(devNum, profileId, keyTemplate,
                 udbAmount, udbCfg, includeVlanOrInLifNum, includeProfileId);
        if (status != GT_OK)
        {
            galtisOutput(outArgs, status, "");
            return CMD_OK;
        }

        cmdOsFree(udbCfg);
        udbCfg = NULL;
        udbAmount = 0;

        galtisOutput(outArgs, status, "");
    }
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPclTemplateCancelSet (table command)
*
* DESCRIPTION:
*     Cancel operations on PclTemplate table
*
* INPUTS:
*     none
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPclTemplateCancelSet
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

    cmdOsFree(udbCfg);
    udbCfg = NULL;
    udbAmount = 0;

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPclTemplateDefaultActionSet
*
* DESCRIPTION:
*   The function configures the default Action for one of templates
*
*
* INPUTS:
*       devNum          - device number
*       profileId       - number of profile (as must be written to LIF)
*       keyTemplate     - the key template secified by packet type
*       action       - default action of a template
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
static CMD_STATUS wrCpssExMxPclTemplateDefaultActionSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                   status;
    GT_U8                       devNum;
    GT_U8                       profileId;
    CPSS_EXMX_PCL_TEMPLATE_ENT  keyTemplate;
    CPSS_EXMX_POLICY_ACTION_STC action;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    profileId = (GT_U8)inFields[0];
    keyTemplate = (CPSS_EXMX_PCL_TEMPLATE_ENT)inFields[1];

    action.pktCmd = (CPSS_EXMX_POLICY_ACTION_PKT_CMD_ENT)inFields[2];
    action.mirror.mirrorToCpu = (GT_BOOL)inFields[3];
    action.mirror.mirrorToRxAnalyzerPort = (GT_BOOL)inFields[4];
    action.qos.markCmd = (CPSS_EXMX_POLICY_ACTION_MARK_CMD_ENT)inFields[5];
    action.qos.markDscp = (GT_BOOL)inFields[6];
    action.qos.dscp = (GT_U8)inFields[7];
    action.qos.markTc = (GT_BOOL)inFields[8];
    action.qos.tc = (GT_U8)inFields[9];
    action.qos.markDp = (GT_BOOL)inFields[10];
    action.qos.dp = (GT_U8)inFields[11];
    action.qos.markUp = (GT_BOOL)inFields[12];
    action.qos.up = (GT_U8)inFields[13];
    action.redirect.redirectCmd =
                            (CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_ENT)inFields[14];
    action.redirect.redirectVid = (GT_U16)inFields[15];

    if(inFields[14] == 0)
    {
        action.redirect.redirectData.outInterface.type =
                                          (CPSS_INTERFACE_TYPE_ENT)inFields[16];
        action.redirect.redirectData.outInterface.devPort.devNum =
                                                            (GT_U8)inFields[17];
        action.redirect.redirectData.outInterface.devPort.portNum =
                                                            (GT_U8)inFields[18];
        action.redirect.redirectData.outInterface.trunkId =
                                                      (GT_TRUNK_ID)inFields[19];
        action.redirect.redirectData.outInterface.vidx =
                                                           (GT_U16)inFields[20];
        action.redirect.redirectData.outInterface.vlanId =
                                                           (GT_U16)inFields[21];
    }

    if(inFields[14] == 1)
    {
        action.redirect.redirectData.routeEntryPointer =
                                                           (GT_U32)inFields[22];
    }

    if(inFields[14] == 2)
    {
        action.redirect.redirectData.mplsNhlfePointer = (GT_U32)inFields[23];
    }

    action.policer.policerMode =
                            (CPSS_EXMX_POLICY_ACTION_POLICER_MODE_ENT)inFields[24];
    action.policer.policerId = (GT_U32)inFields[25];
    action.fa.setLbi = (GT_BOOL)inFields[26];
    action.fa.lbi = (GT_U8)inFields[27];
    action.fa.flowId = (GT_U32)inFields[28];
    action.vlan.modifyVlan = (CPSS_EXMX_POLICY_ACTION_VLAN_CMD_ENT)inFields[29];
    action.vlan.nestedVlan = (GT_BOOL)inFields[30];
    action.vlan.vlanId = (GT_U16)inFields[31];


    /* call cpss api function */
    status = cpssExMxPclTemplateDefaultActionSet(devNum, profileId,
                                           keyTemplate, &action);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPclRuleSet
*
* DESCRIPTION:
*   The function sets the Policy Rule Mask, Pattern and Action
*
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule.
*                           For 98EX1x6 devices supported formats are:
*                           - CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E,
*                           - CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E,
*                           - CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_SIP_E.
*                           For 98EX1x5 devices supported format is:
*                           - CPSS_EXMX_PCL_RULE_FORMAT_TD_E.
*
*       ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case
*                          98EX1x5 devices:
*                           - rule index may be in the range from 0 up to 1023
*                           - Policy Control List has granularity of 8 rules
*                          98EX1x6 devices:
*                          The TCAM is constructed from two identical blocks,
*                          referred to as TCAM0 and TCAM1. The two blocks
*                          are placed in a serial configuration, therefore,
*                          logically, TCAM0 is searched first and only
*                          if there is a miss,the search continues to TCAM1.
*                          With reference to Standard and Extended rules
*                          indexes, the partitioning is as follows:
*                          - Standard rules.
*                            Rule index may be in the range from 0 up to 2047.
*                            Rules 0-1023 are in the TCAM0.
*                            Rules 1024-2047 are in the TCAM1.
*                          - Extended rules.
*                            Rule index may be in the range from 0 up to 1023.
*                            Rules 0-511 are in the TCAM0.
*                            Rules 512-1023 are in the TCAM1.
*                          Extended rule consumes the space of two standard
*                            rules:
*                          - Extended rule with index  0 <= n <= 511 (TCAM0)
*                            is placed in the space of two standard rules with
*                            indexes n and n + 512.
*                          - Extended rule with index  512 <= n <= 1023 (TCAM1)
*                            is placed in the space of two standard rules with
*                            ndexes n + 512 and n + 1024.
*                            E.g. set of extended rule to index 58 prohibits
*                            to set two standard rules to indexes 58 and 570,
*                            and extended rule 700 prohibits to set two
*                            standard rules to indexes 1212 and 1724.
*                          Policy Control List has granularity of 1 rule
*                          98MX6x5 and 98MX6x8 devices:
*                           - rule index may be in the range from 0 up to 1023
*                           - Policy Control List has granularity of 1 rule
*
*       mask          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*                          The format of mask is defined by ruleFormat
*
*       pattern       - rule pattern.
*                          The format of pattern is defined by ruleFormat
*
*       action        - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
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
*/


/******************************************************************************/

static CMD_STATUS wrCpssExMxPclRule_TG_STD_Set

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          status;
    GT_U8                              devNum;
    GT_U32                             ruleIndex;
    CPSS_EXMX_POLICY_ACTION_STC        action;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT      mask;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT      pattern;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    ruleIndex = (GT_U32)inFields[0];

    mask.tgStd.pclId = (GT_U8)inFields[1];
    mask.tgStd.pktType = (GT_U8)inFields[2];
    mask.tgStd.ipHdrOk = (GT_U8)inFields[3];
    mask.tgStd.routedOrBridged = (GT_U8)inFields[4];
    mask.tgStd.macDaType = (GT_U8)inFields[5];
    mask.tgStd.macDaFound = (GT_U8)inFields[6];
    mask.tgStd.pktTagging = (GT_U8)inFields[7];
    mask.tgStd.tcpUdpIcmp = (GT_U8)inFields[8];
    mask.tgStd.ipv6ExtHeaderExists = (GT_U8)inFields[9];
    mask.tgStd.vid = (GT_U16)inFields[10];
    mask.tgStd.up = (GT_U8)inFields[11];
    mask.tgStd.portInLifNum = (GT_U8)inFields[12];
    mask.tgStd.udbValid = (GT_U8)inFields[13];
    mask.tgStd.l4Invalid = (GT_U8)inFields[14];
    mask.tgStd.l2Encap = (GT_U8)inFields[15];
    mask.tgStd.dscp = (GT_U8)inFields[16];

    inFields[17] = cmdOsHtonl(inFields[17]);
    inFields[18] = cmdOsHtonl(inFields[18]);
    inFields[19] = cmdOsHtonl(inFields[19]);
    inFields[20] = cmdOsHtonl(inFields[20]);

    cmdOsMemCpy(mask.tgStd.udb, &inFields[17], 16);

    pattern.tgStd.pclId = (GT_U8)inFields[21];
    pattern.tgStd.pktType = (GT_U8)inFields[22];
    pattern.tgStd.ipHdrOk = (GT_U8)inFields[23];
    pattern.tgStd.routedOrBridged = (GT_U8)inFields[24];
    pattern.tgStd.macDaType = (GT_U8)inFields[25];
    pattern.tgStd.macDaFound = (GT_U8)inFields[26];
    pattern.tgStd.pktTagging = (GT_U8)inFields[27];
    pattern.tgStd.tcpUdpIcmp = (GT_U8)inFields[28];
    pattern.tgStd.ipv6ExtHeaderExists = (GT_U8)inFields[29];
    pattern.tgStd.vid = (GT_U16)inFields[30];
    pattern.tgStd.up = (GT_U8)inFields[31];
    pattern.tgStd.portInLifNum = (GT_U8)inFields[32];
    pattern.tgStd.udbValid = (GT_U8)inFields[33];
    pattern.tgStd.l4Invalid = (GT_U8)inFields[34];
    pattern.tgStd.l2Encap = (GT_U8)inFields[35];
    pattern.tgStd.dscp = (GT_U8)inFields[36];

    inFields[37] = cmdOsHtonl(inFields[37]);
    inFields[38] = cmdOsHtonl(inFields[38]);
    inFields[39] = cmdOsHtonl(inFields[39]);
    inFields[40] = cmdOsHtonl(inFields[40]);

    cmdOsMemCpy(pattern.tgStd.udb, &inFields[37], 16);

    action.pktCmd = (CPSS_EXMX_POLICY_ACTION_PKT_CMD_ENT)inFields[41];
    action.mirror.mirrorToCpu = (GT_BOOL)inFields[42];
    action.mirror.mirrorToRxAnalyzerPort = (GT_BOOL)inFields[43];
    action.qos.markCmd = (CPSS_EXMX_POLICY_ACTION_MARK_CMD_ENT)inFields[44];
    action.qos.markDscp = (GT_BOOL)inFields[45];
    action.qos.dscp = (GT_U8)inFields[46];
    action.qos.markTc = (GT_BOOL)inFields[47];
    action.qos.tc = (GT_U8)inFields[48];
    action.qos.markDp = (GT_BOOL)inFields[49];
    action.qos.dp = (GT_U8)inFields[50];
    action.qos.markUp = (GT_BOOL)inFields[51];
    action.qos.up = (GT_U8)inFields[52];
    action.redirect.redirectCmd =
                            (CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_ENT)inFields[53];
    action.redirect.redirectVid = (GT_U16)inFields[54];

    if(inFields[53] == 0)
    {
        action.redirect.redirectData.outInterface.type =
                                          (CPSS_INTERFACE_TYPE_ENT)inFields[55];
        action.redirect.redirectData.outInterface.devPort.devNum =
                                                            (GT_U8)inFields[56];
        action.redirect.redirectData.outInterface.devPort.portNum =
                                                            (GT_U8)inFields[57];
        action.redirect.redirectData.outInterface.trunkId =
                                                      (GT_TRUNK_ID)inFields[58];
        action.redirect.redirectData.outInterface.vidx =
                                                           (GT_U16)inFields[59];
        action.redirect.redirectData.outInterface.vlanId =
                                                           (GT_U16)inFields[60];
    }

    if(inFields[53] == 1)
    {
        action.redirect.redirectData.routeEntryPointer =
                                                           (GT_U32)inFields[61];
    }

    if(inFields[53] == 2)
    {
        action.redirect.redirectData.mplsNhlfePointer = (GT_U32)inFields[62];
    }

    action.policer.policerMode =
                            (CPSS_EXMX_POLICY_ACTION_POLICER_MODE_ENT)inFields[63];
    action.policer.policerId = (GT_U32)inFields[64];
    action.fa.setLbi = (GT_BOOL)inFields[65];
    action.fa.lbi = (GT_U8)inFields[66];
    action.fa.flowId = (GT_U32)inFields[67];
    action.vlan.modifyVlan = (CPSS_EXMX_POLICY_ACTION_VLAN_CMD_ENT)inFields[68];
    action.vlan.nestedVlan = (GT_BOOL)inFields[69];
    action.vlan.vlanId = (GT_U16)inFields[70];

    /* call cpss api function */
    status = cpssExMxPclRuleSet(devNum, CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E,
                              ruleIndex, &mask, &pattern, &action);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}




/**********************************************************************/



static CMD_STATUS wrCpssExMxPclRule_TG_EXT_MAC_Set

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          status;
    GT_U8                              devNum;
    GT_U32                             ruleIndex;
    CPSS_EXMX_POLICY_ACTION_STC        action;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT      mask;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT      pattern;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];



    ruleIndex = (GT_U32)inFields[0];

    mask.tgExtMac.stdRule.pclId = (GT_U8)inFields[1];
    mask.tgExtMac.stdRule.pktType = (GT_U8)inFields[2];
    mask.tgExtMac.stdRule.ipHdrOk = (GT_U8)inFields[3];
    mask.tgExtMac.stdRule.routedOrBridged = (GT_U8)inFields[4];
    mask.tgExtMac.stdRule.macDaType = (GT_U8)inFields[5];
    mask.tgExtMac.stdRule.macDaFound = (GT_U8)inFields[6];
    mask.tgExtMac.stdRule.pktTagging = (GT_U8)inFields[7];
    mask.tgExtMac.stdRule.tcpUdpIcmp = (GT_U8)inFields[8];
    mask.tgExtMac.stdRule.ipv6ExtHeaderExists = (GT_U8)inFields[9];
    mask.tgExtMac.stdRule.vid = (GT_U16)inFields[10];
    mask.tgExtMac.stdRule.up = (GT_U8)inFields[11];
    mask.tgExtMac.stdRule.portInLifNum = (GT_U8)inFields[12];
    mask.tgExtMac.stdRule.udbValid = (GT_U8)inFields[13];
    mask.tgExtMac.stdRule.l4Invalid = (GT_U8)inFields[14];
    mask.tgExtMac.stdRule.l2Encap = (GT_U8)inFields[15];
    mask.tgExtMac.stdRule.dscp = (GT_U8)inFields[16];

    inFields[17] = cmdOsHtonl(inFields[17]);
    inFields[18] = cmdOsHtonl(inFields[18]);
    inFields[19] = cmdOsHtonl(inFields[19]);
    inFields[20] = cmdOsHtonl(inFields[20]);
    inFields[21] = cmdOsHtonl(inFields[21]);

    cmdOsMemCpy(mask.tgStd.udb, &inFields[17], 16);

    cmdOsMemCpy(mask.tgExtMac.dipBits0to31, &inFields[21], 4);

    inFields[22] = (GT_U32)cmdOsHtons((GT_U16)(inFields[22]));

    cmdOsMemCpy(mask.tgExtMac.dipBits112to127, &inFields[22], 2);

    mask.tgExtMac.ipProtocol = (GT_U8)inFields[23];
    galtisMacAddr(&mask.tgExtMac.macDa, (GT_U8*)inFields[24]);
    galtisMacAddr(&mask.tgExtMac.macSa, (GT_U8*)inFields[25]);

    inFields[26] = cmdOsHtonl(inFields[26]);

    cmdOsMemCpy(mask.tgExtMac.sipBits0to31, &inFields[26], 4);

    pattern.tgExtMac.stdRule.pclId = (GT_U8)inFields[27];
    pattern.tgExtMac.stdRule.pktType = (GT_U8)inFields[28];
    pattern.tgExtMac.stdRule.ipHdrOk = (GT_U8)inFields[29];
    pattern.tgExtMac.stdRule.routedOrBridged = (GT_U8)inFields[30];
    pattern.tgExtMac.stdRule.macDaType = (GT_U8)inFields[31];
    pattern.tgExtMac.stdRule.macDaFound = (GT_U8)inFields[32];
    pattern.tgExtMac.stdRule.pktTagging = (GT_U8)inFields[33];
    pattern.tgExtMac.stdRule.tcpUdpIcmp = (GT_U8)inFields[34];
    pattern.tgExtMac.stdRule.ipv6ExtHeaderExists = (GT_U8)inFields[35];
    pattern.tgExtMac.stdRule.vid = (GT_U16)inFields[36];
    pattern.tgExtMac.stdRule.up = (GT_U8)inFields[37];
    pattern.tgExtMac.stdRule.portInLifNum = (GT_U8)inFields[38];
    pattern.tgExtMac.stdRule.udbValid = (GT_U8)inFields[39];
    pattern.tgExtMac.stdRule.l4Invalid = (GT_U8)inFields[40];
    pattern.tgExtMac.stdRule.l2Encap = (GT_U8)inFields[41];
    pattern.tgExtMac.stdRule.dscp = (GT_U8)inFields[42];

    inFields[43] = cmdOsHtonl(inFields[43]);
    inFields[44] = cmdOsHtonl(inFields[44]);
    inFields[45] = cmdOsHtonl(inFields[45]);
    inFields[46] = cmdOsHtonl(inFields[46]);
    inFields[47] = cmdOsHtonl(inFields[47]);

    cmdOsMemCpy(pattern.tgExtMac.stdRule.udb, &inFields[43], 16);

    cmdOsMemCpy(pattern.tgExtMac.dipBits0to31, &inFields[47], 4);

    inFields[48] = (GT_U32)cmdOsHtons((GT_U16)(inFields[48]));

    cmdOsMemCpy(pattern.tgExtMac.dipBits112to127, &inFields[48], 2);

    pattern.tgExtMac.ipProtocol = (GT_U8)inFields[49];
    galtisMacAddr(&pattern.tgExtMac.macDa, (GT_U8*)inFields[50]);
    galtisMacAddr(&pattern.tgExtMac.macSa, (GT_U8*)inFields[51]);

    inFields[52] = cmdOsHtonl(inFields[52]);

    cmdOsMemCpy(pattern.tgExtMac.sipBits0to31, &inFields[52], 4);

    action.pktCmd = (CPSS_EXMX_POLICY_ACTION_PKT_CMD_ENT)inFields[53];
    action.mirror.mirrorToCpu = (GT_BOOL)inFields[54];
    action.mirror.mirrorToRxAnalyzerPort = (GT_BOOL)inFields[55];
    action.qos.markCmd = (CPSS_EXMX_POLICY_ACTION_MARK_CMD_ENT)inFields[56];
    action.qos.markDscp = (GT_BOOL)inFields[57];
    action.qos.dscp = (GT_U8)inFields[58];
    action.qos.markTc = (GT_BOOL)inFields[59];
    action.qos.tc = (GT_U8)inFields[60];
    action.qos.markDp = (GT_BOOL)inFields[61];
    action.qos.dp = (GT_U8)inFields[62];
    action.qos.markUp = (GT_BOOL)inFields[63];
    action.qos.up = (GT_U8)inFields[64];
    action.redirect.redirectCmd =
                            (CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_ENT)inFields[65];
    action.redirect.redirectVid = (GT_U16)inFields[66];

    if(inFields[65] == 0)
    {
        action.redirect.redirectData.outInterface.type =
                                          (CPSS_INTERFACE_TYPE_ENT)inFields[67];
        action.redirect.redirectData.outInterface.devPort.devNum =
                                                            (GT_U8)inFields[68];
        action.redirect.redirectData.outInterface.devPort.portNum =
                                                            (GT_U8)inFields[69];
        action.redirect.redirectData.outInterface.trunkId =
                                                      (GT_TRUNK_ID)inFields[70];
        action.redirect.redirectData.outInterface.vidx =
                                                           (GT_U16)inFields[71];
        action.redirect.redirectData.outInterface.vlanId =
                                                           (GT_U16)inFields[72];
    }

    if(inFields[65] == 1)
    {
        action.redirect.redirectData.routeEntryPointer =
                                                           (GT_U32)inFields[73];
    }

    if(inFields[65] == 2)
    {
        action.redirect.redirectData.mplsNhlfePointer = (GT_U32)inFields[74];
    }

    action.policer.policerMode =
                            (CPSS_EXMX_POLICY_ACTION_POLICER_MODE_ENT)inFields[75];
    action.policer.policerId = (GT_U32)inFields[76];
    action.fa.setLbi = (GT_BOOL)inFields[77];
    action.fa.lbi = (GT_U8)inFields[78];
    action.fa.flowId = (GT_U32)inFields[79];
    action.vlan.modifyVlan = (CPSS_EXMX_POLICY_ACTION_VLAN_CMD_ENT)inFields[80];
    action.vlan.nestedVlan = (GT_BOOL)inFields[81];
    action.vlan.vlanId = (GT_U16)inFields[82];

    /* call cpss api function */
    status = cpssExMxPclRuleSet(devNum, CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E,
                                  ruleIndex, &mask, &pattern, &action);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}


/**********************************************************************/


static CMD_STATUS wrCpssExMxPclRule_TD_STC_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          status;
    GT_U8                              devNum;
    GT_U32                             ruleIndex;
    CPSS_EXMX_POLICY_ACTION_STC        action;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT      mask;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT      pattern;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];



    ruleIndex = (GT_U32)inFields[0];

    mask.tdRule.pktType = (GT_U8)inFields[1];
    mask.tdRule.routedOrBridged = (GT_U8)inFields[2];
    mask.tdRule.macDaType = (GT_U8)inFields[3];
    mask.tdRule.macDaFound = (GT_U8)inFields[4];

    inFields[5] = cmdOsHtonl(inFields[5]);
    inFields[6] = cmdOsHtonl(inFields[6]);
    inFields[7] = cmdOsHtonl(inFields[7]);
    inFields[8] = cmdOsHtonl(inFields[8]);

    cmdOsMemCpy(mask.tdRule.udb, &inFields[5], 16);

    pattern.tdRule.pktType = (GT_U8)inFields[9];
    pattern.tdRule.routedOrBridged = (GT_U8)inFields[10];
    pattern.tdRule.macDaType = (GT_U8)inFields[11];
    pattern.tdRule.macDaFound = (GT_U8)inFields[12];

    inFields[13] = cmdOsHtonl(inFields[13]);
    inFields[14] = cmdOsHtonl(inFields[14]);
    inFields[15] = cmdOsHtonl(inFields[15]);
    inFields[16] = cmdOsHtonl(inFields[16]);

    cmdOsMemCpy(pattern.tdRule.udb, &inFields[13], 16);

    action.pktCmd = (CPSS_EXMX_POLICY_ACTION_PKT_CMD_ENT)inFields[17];
    action.mirror.mirrorToCpu = (GT_BOOL)inFields[18];
    action.mirror.mirrorToRxAnalyzerPort = (GT_BOOL)inFields[19];
    action.qos.markCmd = (CPSS_EXMX_POLICY_ACTION_MARK_CMD_ENT)inFields[20];
    action.qos.markDscp = (GT_BOOL)inFields[21];
    action.qos.dscp = (GT_U8)inFields[22];
    action.qos.markTc = (GT_BOOL)inFields[23];
    action.qos.tc = (GT_U8)inFields[24];
    action.qos.markDp = (GT_BOOL)inFields[25];
    action.qos.dp = (GT_U8)inFields[26];
    action.qos.markUp = (GT_BOOL)inFields[27];
    action.qos.up = (GT_U8)inFields[28];
    action.redirect.redirectCmd =
                            (CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_ENT)inFields[29];
    action.redirect.redirectVid = (GT_U16)inFields[30];

    if(inFields[29] == 0)
    {
        action.redirect.redirectData.outInterface.type =
                                          (CPSS_INTERFACE_TYPE_ENT)inFields[31];
        action.redirect.redirectData.outInterface.devPort.devNum =
                                                            (GT_U8)inFields[32];
        action.redirect.redirectData.outInterface.devPort.portNum =
                                                            (GT_U8)inFields[33];
        action.redirect.redirectData.outInterface.trunkId =
                                                      (GT_TRUNK_ID)inFields[34];
        action.redirect.redirectData.outInterface.vidx =
                                                           (GT_U16)inFields[35];
        action.redirect.redirectData.outInterface.vlanId =
                                                           (GT_U16)inFields[36];
    }

    if(inFields[29] == 1)
    {
        action.redirect.redirectData.routeEntryPointer =
                                                           (GT_U32)inFields[37];
    }

    if(inFields[29] == 2)
    {
        action.redirect.redirectData.mplsNhlfePointer = (GT_U32)inFields[38];
    }

    action.policer.policerMode =
                            (CPSS_EXMX_POLICY_ACTION_POLICER_MODE_ENT)inFields[39];
    action.policer.policerId = (GT_U32)inFields[40];
    action.fa.setLbi = (GT_BOOL)inFields[41];
    action.fa.lbi = (GT_U8)inFields[42];
    action.fa.flowId = (GT_U32)inFields[43];
    action.vlan.modifyVlan = (CPSS_EXMX_POLICY_ACTION_VLAN_CMD_ENT)inFields[44];
    action.vlan.nestedVlan = (GT_BOOL)inFields[45];
    action.vlan.vlanId = (GT_U16)inFields[46];

    /* call cpss api function */
    status = cpssExMxPclRuleSet(devNum, CPSS_EXMX_PCL_RULE_FORMAT_TD_E,
                          ruleIndex, &mask, &pattern, &action);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}



/**********************************************************************/


static CMD_STATUS wrCpssExMxPclRule_TG_EXT_SIP_Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          status;
    GT_U8                              devNum;
    GT_U32                             ruleIndex;
    CPSS_EXMX_POLICY_ACTION_STC        action;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT      mask;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT      pattern;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];



    ruleIndex = (GT_U32)inFields[0];

    mask.tgExtSip.stdRule.pclId = (GT_U8)inFields[1];
    mask.tgExtSip.stdRule.pktType = (GT_U8)inFields[2];
    mask.tgExtSip.stdRule.ipHdrOk = (GT_U8)inFields[3];
    mask.tgExtSip.stdRule.routedOrBridged = (GT_U8)inFields[4];
    mask.tgExtSip.stdRule.macDaType = (GT_U8)inFields[5];
    mask.tgExtSip.stdRule.macDaFound = (GT_U8)inFields[6];
    mask.tgExtSip.stdRule.pktTagging = (GT_U8)inFields[7];
    mask.tgExtSip.stdRule.tcpUdpIcmp = (GT_U8)inFields[8];
    mask.tgExtSip.stdRule.ipv6ExtHeaderExists = (GT_U8)inFields[9];
    mask.tgExtSip.stdRule.vid = (GT_U16)inFields[10];
    mask.tgExtSip.stdRule.up = (GT_U8)inFields[11];
    mask.tgExtSip.stdRule.portInLifNum = (GT_U8)inFields[12];
    mask.tgExtSip.stdRule.udbValid = (GT_U8)inFields[13];
    mask.tgExtSip.stdRule.l4Invalid = (GT_U8)inFields[14];
    mask.tgExtSip.stdRule.l2Encap = (GT_U8)inFields[15];
    mask.tgExtSip.stdRule.dscp = (GT_U8)inFields[16];

    inFields[17] = cmdOsHtonl(inFields[17]);
    inFields[18] = cmdOsHtonl(inFields[18]);
    inFields[19] = cmdOsHtonl(inFields[19]);
    inFields[20] = cmdOsHtonl(inFields[20]);
    inFields[21] = cmdOsHtonl(inFields[21]);

    cmdOsMemCpy(mask.tgExtSip.stdRule.udb, &inFields[17], 16);

    cmdOsMemCpy(mask.tgExtSip.dipBits0to31, &inFields[21], 4);

    inFields[22] = (GT_U32)cmdOsHtons((GT_U16)(inFields[22]));

    cmdOsMemCpy(mask.tgExtSip.dipBits112to127, &inFields[22], 2);

    mask.tgExtSip.ipProtocol = (GT_U8)inFields[23];

    inFields[24] = cmdOsHtonl(inFields[24]);

    cmdOsMemCpy(mask.tgExtSip.sip, &inFields[24], 16);

    pattern.tgExtSip.stdRule.pclId = (GT_U8)inFields[28];
    pattern.tgExtSip.stdRule.pktType = (GT_U8)inFields[29];
    pattern.tgExtSip.stdRule.ipHdrOk = (GT_U8)inFields[30];
    pattern.tgExtSip.stdRule.routedOrBridged = (GT_U8)inFields[31];
    pattern.tgExtSip.stdRule.macDaType = (GT_U8)inFields[32];
    pattern.tgExtSip.stdRule.macDaFound = (GT_U8)inFields[33];
    pattern.tgExtSip.stdRule.pktTagging = (GT_U8)inFields[34];
    pattern.tgExtSip.stdRule.tcpUdpIcmp = (GT_U8)inFields[35];
    pattern.tgExtSip.stdRule.ipv6ExtHeaderExists = (GT_U8)inFields[36];
    pattern.tgExtSip.stdRule.vid = (GT_U16)inFields[37];
    pattern.tgExtSip.stdRule.up = (GT_U8)inFields[38];
    pattern.tgExtSip.stdRule.portInLifNum = (GT_U8)inFields[39];
    pattern.tgExtSip.stdRule.udbValid = (GT_U8)inFields[40];
    pattern.tgExtSip.stdRule.l4Invalid = (GT_U8)inFields[41];
    pattern.tgExtSip.stdRule.l2Encap = (GT_U8)inFields[42];
    pattern.tgExtSip.stdRule.dscp = (GT_U8)inFields[43];

    inFields[44] = cmdOsHtonl(inFields[44]);
    inFields[45] = cmdOsHtonl(inFields[45]);
    inFields[46] = cmdOsHtonl(inFields[46]);
    inFields[47] = cmdOsHtonl(inFields[47]);
    inFields[48] = cmdOsHtonl(inFields[48]);

    cmdOsMemCpy(pattern.tgExtSip.stdRule.udb, &inFields[44], 16);

    cmdOsMemCpy(pattern.tgExtSip.dipBits0to31, &inFields[48], 4);

    inFields[49] = (GT_U32)cmdOsHtons((GT_U16)(inFields[49]));

    cmdOsMemCpy(pattern.tgExtSip.dipBits112to127, &inFields[49], 2);

    pattern.tgExtSip.ipProtocol = (GT_U8)inFields[50];

    inFields[51] = cmdOsHtonl(inFields[51]);
    inFields[52] = cmdOsHtonl(inFields[52]);
    inFields[53] = cmdOsHtonl(inFields[53]);
    inFields[54] = cmdOsHtonl(inFields[54]);

    cmdOsMemCpy(pattern.tgExtSip.sip, &inFields[51], 16);

    action.pktCmd = (CPSS_EXMX_POLICY_ACTION_PKT_CMD_ENT)inFields[55];
    action.mirror.mirrorToCpu = (GT_BOOL)inFields[56];
    action.mirror.mirrorToRxAnalyzerPort = (GT_BOOL)inFields[57];
    action.qos.markCmd = (CPSS_EXMX_POLICY_ACTION_MARK_CMD_ENT)inFields[58];
    action.qos.markDscp = (GT_BOOL)inFields[59];
    action.qos.dscp = (GT_U8)inFields[60];
    action.qos.markTc = (GT_BOOL)inFields[61];
    action.qos.tc = (GT_U8)inFields[62];
    action.qos.markDp = (GT_BOOL)inFields[63];
    action.qos.dp = (GT_U8)inFields[64];
    action.qos.markUp = (GT_BOOL)inFields[65];
    action.qos.up = (GT_U8)inFields[66];
    action.redirect.redirectCmd =
                           (CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_ENT)inFields[67];
    action.redirect.redirectVid = (GT_U16)inFields[68];

    if(inFields[67] == 0)
    {
        action.redirect.redirectData.outInterface.type =
                                          (CPSS_INTERFACE_TYPE_ENT)inFields[69];
        action.redirect.redirectData.outInterface.devPort.devNum =
                                                            (GT_U8)inFields[70];
        action.redirect.redirectData.outInterface.devPort.portNum =
                                                            (GT_U8)inFields[71];
        action.redirect.redirectData.outInterface.trunkId =
                                                      (GT_TRUNK_ID)inFields[72];
        action.redirect.redirectData.outInterface.vidx =
                                                           (GT_U16)inFields[73];
        action.redirect.redirectData.outInterface.vlanId =
                                                           (GT_U16)inFields[74];
    }

    if(inFields[67] == 1)
    {
        action.redirect.redirectData.routeEntryPointer =
                                                           (GT_U32)inFields[75];
    }

    if(inFields[67] == 2)
    {
        action.redirect.redirectData.mplsNhlfePointer = (GT_U32)inFields[76];
    }

    action.policer.policerMode =
                           (CPSS_EXMX_POLICY_ACTION_POLICER_MODE_ENT)inFields[77];
    action.policer.policerId = (GT_U32)inFields[78];
    action.fa.setLbi = (GT_BOOL)inFields[79];
    action.fa.lbi = (GT_U8)inFields[80];
    action.fa.flowId = (GT_U32)inFields[81];
    action.vlan.modifyVlan =
                               (CPSS_EXMX_POLICY_ACTION_VLAN_CMD_ENT)inFields[82];
    action.vlan.nestedVlan = (GT_BOOL)inFields[83];
    action.vlan.vlanId = (GT_U16)inFields[84];

    /* call cpss api function */
    status = cpssExMxPclRuleSet(devNum, CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_SIP_E,
                                  ruleIndex, &mask, &pattern, &action);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}


static CMD_STATUS wrCpssExMxPclRuleSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* fields number (saved here only) */
    /* wrCpssExMxPclRule_TG_STD_Set     - 71 */
    /* wrCpssExMxPclRule_TG_EXT_MAC_Set - 83 */
    /* wrCpssExMxPclRule_TG_EXT_SIP_Set - 85 */
    /* wrCpssExMxPclRule_TD_STC_Set     - 47 */

    switch(inArgs[1])
    {
        case 0:
            return wrCpssExMxPclRule_TG_STD_Set(
                inArgs, inFields, numFields, outArgs);
            break;

        case 1:
            return wrCpssExMxPclRule_TG_EXT_MAC_Set(
                inArgs, inFields, numFields, outArgs);
            break;

        case 2:
            return wrCpssExMxPclRule_TG_EXT_SIP_Set(
                inArgs, inFields, numFields, outArgs);
        break;

        case 3:
            return wrCpssExMxPclRule_TD_STC_Set(
                inArgs, inFields, numFields, outArgs);
            break;

        default:
            return CMD_AGENT_ERROR;
    }

}

/*******************************************************************************
* cpssExMxPclRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the RULE Action
*
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*                         Size of the rule defined by rule's format:
*                           For 98EX1x6 rule formats:
*  - CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E - use CPSS_EXMX_PCL_RULE_SIZE_STD_E
*  - CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E - use  CPSS_EXMX_PCL_RULE_SIZE_EXT_E
*  - CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_SIP_E - use  CPSS_EXMX_PCL_RULE_SIZE_EXT_E
*                           98EX1x5, 98MX6x5 and 98MX6x8 devices supports only
*                            CPSS_EXMX_PCL_RULE_SIZE_STD_E
*
*       ruleIndex       - index of the rule in the TCAM. See cpssExMxPclRuleSet.
*
*       action       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
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
static CMD_STATUS wrCpssExMxPclRuleActionUpdate

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                   status;
    GT_U8                       devNum;
    CPSS_EXMX_PCL_RULE_SIZE_ENT ruleSize;
    GT_U32                      ruleIndex;
    CPSS_EXMX_POLICY_ACTION_STC action;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];


    ruleSize = (CPSS_EXMX_PCL_RULE_SIZE_ENT)inFields[0];
    ruleIndex = inFields[1];

    action.pktCmd = (CPSS_EXMX_POLICY_ACTION_PKT_CMD_ENT)inFields[2];
    action.mirror.mirrorToCpu = (GT_BOOL)inFields[3];
    action.mirror.mirrorToRxAnalyzerPort = (GT_BOOL)inFields[4];
    action.qos.markCmd = (CPSS_EXMX_POLICY_ACTION_MARK_CMD_ENT)inFields[5];
    action.qos.markDscp = (GT_BOOL)inFields[6];
    action.qos.dscp = (GT_U8)inFields[7];
    action.qos.markTc = (GT_BOOL)inFields[8];
    action.qos.tc = (GT_U8)inFields[9];
    action.qos.markDp = (GT_BOOL)inFields[10];
    action.qos.dp = (GT_U8)inFields[11];
    action.qos.markUp = (GT_BOOL)inFields[12];
    action.qos.up = (GT_U8)inFields[13];
    action.redirect.redirectCmd =
                            (CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_ENT)inFields[14];
    action.redirect.redirectVid = (GT_U16)inFields[15];
    if(inFields[14] == 0)
    {
        action.redirect.redirectData.outInterface.type =
                                          (CPSS_INTERFACE_TYPE_ENT)inFields[16];
        action.redirect.redirectData.outInterface.devPort.devNum =
                                                            (GT_U8)inFields[17];
        action.redirect.redirectData.outInterface.devPort.portNum =
                                                            (GT_U8)inFields[18];
        action.redirect.redirectData.outInterface.trunkId =
                                                      (GT_TRUNK_ID)inFields[19];
        action.redirect.redirectData.outInterface.vidx =
                                                           (GT_U16)inFields[20];
        action.redirect.redirectData.outInterface.vlanId =
                                                           (GT_U16)inFields[21];
    }

    if(inFields[14] == 1)
    {
        action.redirect.redirectData.routeEntryPointer =
                                                           (GT_U32)inFields[22];
    }

    if(inFields[14] == 2)
    {
        action.redirect.redirectData.mplsNhlfePointer = (GT_U32)inFields[23];
    }

    action.policer.policerMode =
                            (CPSS_EXMX_POLICY_ACTION_POLICER_MODE_ENT)inFields[24];
    action.policer.policerId = (GT_U32)inFields[25];
    action.fa.setLbi = (GT_BOOL)inFields[26];
    action.fa.lbi = (GT_U8)inFields[27];
    action.fa.flowId = (GT_U32)inFields[28];
    action.vlan.modifyVlan = (CPSS_EXMX_POLICY_ACTION_VLAN_CMD_ENT)inFields[29];
    action.vlan.nestedVlan = (GT_BOOL)inFields[30];
    action.vlan.vlanId = (GT_U16)inFields[31];


        /* call cpss api function */
        status = cpssExMxPclRuleActionUpdate(devNum, ruleSize, ruleIndex,
                                                              &action);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPclRuleInvalidate
*
* DESCRIPTION:
*   The function invalidates the Policy Rule
*
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - size of Rule.
*                         Size of the rule defined by rule's format:
*                           For 98EX1x6 rule formats:
*  - CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E - use CPSS_EXMX_PCL_RULE_SIZE_STD_E
*  - CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E - use  CPSS_EXMX_PCL_RULE_SIZE_EXT_E
*  - CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_SIP_E - use  CPSS_EXMX_PCL_RULE_SIZE_EXT_E
*                           98EX1x5, 98MX6x5 and 98MX6x8 devices supports only
*                            CPSS_EXMX_PCL_RULE_SIZE_STD_E
*
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
*       The 98EX1X6 for IP MC Rule invalidation the ruleSize should be
*       CPSS_EXMX_PCL_RULE_SIZE_STD_E
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPclRuleInvalidate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                                devNum;
    CPSS_EXMX_PCL_RULE_SIZE_ENT ruleSize;
    GT_U32                                                ruleIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_EXMX_PCL_RULE_SIZE_ENT)inArgs[1];
    ruleIndex = (GT_U32)inArgs[2];


    /* call cpss api function */
    result = cpssExMxPclRuleInvalidate(devNum, ruleSize, ruleIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPclRuleCopy
*
* DESCRIPTION:
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first cpssExMxPclRuleCopy should be
*  called. And after this cpssExMxPclRuleInvalidate should be used to invalidate
*  Rule in old position
*
*
* INPUTS:
*       devNum           - device number
*       ruleSize        - size of Rule.
*                         Size of the rule defined by rule's format:
*                           For 98EX1x6 rule formats:
*  - CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E - use CPSS_EXMX_PCL_RULE_SIZE_STD_E
*  - CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E - use  CPSS_EXMX_PCL_RULE_SIZE_EXT_E
*  - CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_SIP_E - use  CPSS_EXMX_PCL_RULE_SIZE_EXT_E
*                           98EX1x5, 98MX6x5 and 98MX6x8 devices supports only
*                            CPSS_EXMX_PCL_RULE_SIZE_STD_E
*
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
*       The 98EX1X6 for IP MC Rule copy the ruleSize should be
*       CPSS_EXMX_PCL_RULE_SIZE_STD_E
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPclRuleCopy
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                                devNum;
    CPSS_EXMX_PCL_RULE_SIZE_ENT ruleSize;
    GT_U32                                                ruleSrcIndex;
    GT_U32                                                ruleDstIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_EXMX_PCL_RULE_SIZE_ENT)inArgs[1];
    ruleSrcIndex = (GT_U32)inArgs[2];
    ruleDstIndex = (GT_U32)inArgs[3];


    /* call cpss api function */
    result = cpssExMxPclRuleCopy(devNum, ruleSize, ruleSrcIndex, ruleDstIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPclEngineModeSet
*
* DESCRIPTION:
*       Set Active, Drop, Bypass and Pause Policy Engine global mode.
*
* INPUTS:
*       devNum             - Device number.
*       policyEngineMode   - Policy Engine global mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong parameters
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - for devices of families
*                          that does not support this feature
*
* COMMENTS:
*       The global PCL configuration supported only on
*        several PP types.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPclEngineModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                                  devNum;
    CPSS_EXMX_PCL_ENGINE_MODE_ENT pclEngineMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    pclEngineMode = (CPSS_EXMX_PCL_ENGINE_MODE_ENT)inArgs[1];


    /* call cpss api function */
    result = cpssExMxPclEngineModeSet(devNum, pclEngineMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPclTwoLookupsActionSelectSet
*
* DESCRIPTION:
*       Configure Policy TCAM engine two lookups actions selection table.
*
* INPUTS:
*       devNum                - Device number.
*       actionSelectionPtr    - Policy TCAM engine two lookups actions
*                               selection table.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong parameters
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - for devices of families
*                          that does not support this feature
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPclTwoLookupsActionSelectSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                                                                   devNum;
    CPSS_EXMX_PCL_TWO_LOOKUPS_ACTION_SELECTION_STC actionSelectionPtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    actionSelectionPtr.packetCmd =
                              (CPSS_EXMX_PCL_ACTION_SELECT_TYPE_ENT)inFields[0];
    actionSelectionPtr.policer =
                              (CPSS_EXMX_PCL_ACTION_SELECT_TYPE_ENT)inFields[1];
    actionSelectionPtr.cosMarking =
                              (CPSS_EXMX_PCL_ACTION_SELECT_TYPE_ENT)inFields[2];
    actionSelectionPtr.redirection =
                              (CPSS_EXMX_PCL_ACTION_SELECT_TYPE_ENT)inFields[3];
    actionSelectionPtr.mirrorToCpu =
                              (CPSS_EXMX_PCL_ACTION_SELECT_TYPE_ENT)inFields[4];
    actionSelectionPtr.mirrorToRxAnalyzer =
                              (CPSS_EXMX_PCL_ACTION_SELECT_TYPE_ENT)inFields[5];
    actionSelectionPtr.lbi =  (CPSS_EXMX_PCL_ACTION_SELECT_TYPE_ENT)inFields[6];
    actionSelectionPtr.nestedVlan =
                              (CPSS_EXMX_PCL_ACTION_SELECT_TYPE_ENT)inFields[7];
    actionSelectionPtr.modifyVlan =
                              (CPSS_EXMX_PCL_ACTION_SELECT_TYPE_ENT)inFields[8];
    actionSelectionPtr.flowId =
                              (CPSS_EXMX_PCL_ACTION_SELECT_TYPE_ENT)inFields[9];


    /* call cpss api function */
    result = cpssExMxPclTwoLookupsActionSelectSet(devNum, &actionSelectionPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPclRulePolicerActionUpdate
*
* DESCRIPTION:
*       Updates Policer mode and Policer index in the Policy Rule action.
*       The function is used to disconnect Policer entry from action
*       before update of Policer entry. And connect Policer entry to action
*       after update of Policer entry.
*
* INPUTS:
*       devNum           - device number.
*       ruleSize        - size of Rule.
*                         Size of the rule defined by rule's format:
*                           For 98EX1x6 rule formats:
*  - CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E - use CPSS_EXMX_PCL_RULE_SIZE_STD_E
*  - CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E - use  CPSS_EXMX_PCL_RULE_SIZE_EXT_E
*  - CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_SIP_E - use  CPSS_EXMX_PCL_RULE_SIZE_EXT_E
*                           98EX1x5, 98MX6x5 and 98MX6x8 devices supports only
*                            CPSS_EXMX_PCL_RULE_SIZE_STD_E
*
*       ruleIndex        - index of the rule in the TCAM. See cpssExMxPclRuleSet.
*
*       policerMode        - Policer mode
*       policerId          - Policers table entry index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong parameters
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - for devices of families
*                          that does not support this feature
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPclRulePolicerActionUpdate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                                          result;

    GT_U8                                                                  devNum;
    CPSS_EXMX_PCL_RULE_SIZE_ENT                          ruleSize;
    GT_U32                                                                  ruleIndex;
    CPSS_EXMX_POLICY_ACTION_POLICER_MODE_ENT policerMode;
    GT_U32                                                                  policerId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleSize = (CPSS_EXMX_PCL_RULE_SIZE_ENT)inArgs[1];
    ruleIndex = (GT_U32)inArgs[2];
    policerMode = (CPSS_EXMX_POLICY_ACTION_POLICER_MODE_ENT)inArgs[3];
    policerId = (GT_U32)inArgs[4];


    /* call cpss api function */
    result = cpssExMxPclRulePolicerActionUpdate(devNum, ruleSize,
                                                           ruleIndex, policerMode, policerId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxPclInit",
        &wrCpssExMxPclInit,
        1, 0},

    {"cpssExMxPclTcamFlush",
        &wrCpssExMxPclTcamFlush,
        1, 0},

    /*  call for table pclTemplate  */

    {"cpssExMxPclTemplateSetFirst",
        &wrCpssExMxPclTemplateSetFirst,
        5, 2},

    {"cpssExMxPclTemplateSetNext",
        &wrCpssExMxPclTemplateSetNext,
        5, 2},

    {"cpssExMxPclTemplateEndSet",
        &wrCpssExMxPclTemplateEndSet,
        5, 0},

    {"cpssExMxPclTemplateCancelSet",
        &wrCpssExMxPclTemplateCancelSet,
        0, 0},

    /*  end call for table pclTemplate  */

    {"cpssExMxPclTemplateDefaultActionSet",
        &wrCpssExMxPclTemplateDefaultActionSet,
        1, 32},

/*****************************************/
    {"cpssExMxPclRuleSet",
        &wrCpssExMxPclRuleSet,
        2, 100},
/*********************************************/

    {"cpssExMxPclRuleActionSet",
        &wrCpssExMxPclRuleActionUpdate,
        1, 32},

    {"cpssExMxPclRuleInvalidate",
        &wrCpssExMxPclRuleInvalidate,
        3, 0},

    {"cpssExMxPclRuleCopy",
        &wrCpssExMxPclRuleCopy,
        4, 0},

    {"cpssExMxPclEngineModeSet",
        &wrCpssExMxPclEngineModeSet,
        2, 0},

    {"cpssExMxPclTwoLookupsActionSelectSet",
        &wrCpssExMxPclTwoLookupsActionSelectSet,
        1, 10},

    {"cpssExMxPclRulePolicerActionUpdate",
        &wrCpssExMxPclRulePolicerActionUpdate,
        5, 0}


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
GT_STATUS cmdLibInitCpssExMxPcl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


