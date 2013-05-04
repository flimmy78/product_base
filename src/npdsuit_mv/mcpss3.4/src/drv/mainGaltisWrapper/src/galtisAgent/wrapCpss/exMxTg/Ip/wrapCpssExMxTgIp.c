/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxTgIp.c
*
* DESCRIPTION:
*       Wrapper functions for Ip cpss functions for Tiger.
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
#include <cpss/exMx/exMxTg/ip/cpssExMxTgIp.h>
#include <cpss/exMx/exMxTg/ip/cpssExMxTgIpCtrl.h>



/*cpssExMxTgIpMcRule Table*/
static GT_U32 gRuleIndexGet;
#define MAX_NUM_OF_RULES 2048
/*******************************************************************************
* cpssExMxTgIpMcRuleSet
*
* DESCRIPTION:
*   The function sets the IP MC Rule with Mask, Pattern and Action
*
*
* INPUTS:
*       dev             - device number
*      ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case.
*                          IP MC Rules and Policy tules are used same TCAM.
*                          Rule index for IP MC Rule rules may be in the range
*                          from 0 up to 2048.
*
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*
*       patternPtr       - rule pattern.
*
*       ipMcActionPtr   - IP MC action rule action that applied on packet if
*                          packet's IP MC search key matched with masked pattern.
*
*
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
*       To copy or invalidate IP MC Rule use the general PCL API:
*       cpssExMxPclRuleInvalidate and cpssExMxPclRuleCopy
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgIpMcRuleSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                             dev;
    GT_U32                            ruleIndex;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       maskPtr;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       patternPtr;
    CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC ipMcActionPtr;
    GT_BYTE_ARRY                     bArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    ruleIndex = (GT_U32)inFields[0];
    maskPtr.pclId = (GT_U8)inFields[1];
    maskPtr.fwdEnable = (GT_U8)inFields[2];
    maskPtr.mngEnable = (GT_U8)inFields[3];
    maskPtr.portLifNum = (GT_U8)inFields[4];
    galtisBArray(&bArr,(GT_U8*)inFields[5]);
    maskPtr.vid = (GT_U16)inFields[6];

    cmdOsMemCpy(maskPtr.dip, bArr.data, 16);

    patternPtr.pclId = (GT_U8)inFields[7];
    patternPtr.fwdEnable = (GT_U8)inFields[8];
    patternPtr.mngEnable = (GT_U8)inFields[9];
    patternPtr.portLifNum = (GT_U8)inFields[10];
    galtisBArray(&bArr,(GT_U8*)inFields[11]);
    patternPtr.vid = (GT_U16)inFields[12];

    cmdOsMemCpy(patternPtr.dip, bArr.data, 16);

    ipMcActionPtr.command = (CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_ENT)inFields[13];
    ipMcActionPtr.destScope =
                      (CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_SCOPE_ENT)inFields[14];

    if(inFields[13] == 0)
    {
        ipMcActionPtr.dest.ssmNode.type =
                      (CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_TYPE_ENT)inFields[15];
        ipMcActionPtr.dest.ssmNode.lpmIndex = (GT_U32)inFields[16];
    }

    if(inFields[13] == 1)
    {
#ifdef NATAN
        ipMcActionPtr.dest.asmRoute.routeEntryMethod =
                 (CPSS_EXMX_TG_PCL_IP_MC_ACTION_ROUTING_METHOD_ENT)inFields[17];
        ipMcActionPtr.dest.asmRoute.routeEntryBaseMemAddr = (GT_U32)inFields[18];
        ipMcActionPtr.dest.asmRoute.blockSize = (GT_U8)inFields[19];
#endif
    }


/* call cpss api function */
    result = cpssExMxTgIpMcRuleSet(dev, ruleIndex, GT_TRUE, &maskPtr,
                                   &patternPtr, &ipMcActionPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxTgIpMcRuleGetFirst
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*   The function gets the IP MC Rule valid, Mask, Pattern and Action.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       dev             - device number
*      ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case.
*                          IP MC Rules and Policy tules are used same TCAM.
*                          Rule index for IP MC Rule rules may be in the range
*                          from 0 up to 2048.
*
* OUTPUTS:
*       isValidPtr       - weather to set the rule as valid or invalid. in the
*                         case of invalid rule , the maskPtr, patternPtr and
*                         ipMcActionPtr can be NULL since they are irrelevant.
*
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*
*       patternPtr       - rule pattern.
*
*       ipMcActionPtr   - IP MC action rule action that applied on packet if
*                          packet's IP MC search key matched with masked pattern.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_STATE   - the rule invalid
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgIpMcRuleGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                             dev;
    GT_BOOL                           isValid;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       maskPtr;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       patternPtr;
    CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC ipMcActionPtr;
    GT_BYTE_ARRY                      bArr;

    GT_U8                             dataStr[16];
    bArr.data  =  dataStr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    gRuleIndexGet = 0;

    do
    {
        /* call cpss api function */
        result = cpssExMxTgIpMcRuleGet(dev, gRuleIndexGet, &isValid, &maskPtr,
                                   &patternPtr, &ipMcActionPtr);
        gRuleIndexGet++;
    }while(!isValid);

    inFields[0] = gRuleIndexGet;
    inFields[1] = maskPtr.pclId;
    inFields[2] = maskPtr.fwdEnable;
    inFields[3] = maskPtr.mngEnable;
    inFields[4] = maskPtr.portLifNum;

    cmdOsMemCpy(bArr.data, maskPtr.dip, 16);
    bArr.length = 16;
    inFields[5] = (GT_U32)galtisBArrayOut(&bArr);

    inFields[6] = maskPtr.vid;
    inFields[7] = patternPtr.pclId;
    inFields[8] = patternPtr.fwdEnable;
    inFields[9] = patternPtr.mngEnable;
    inFields[10] = patternPtr.portLifNum;


    cmdOsMemCpy(bArr.data, patternPtr.dip, 16);
    bArr.length = 16;
    inFields[11] = (GT_U32)galtisBArrayOut(&bArr);

    inFields[12] = patternPtr.vid;
    inFields[13] = ipMcActionPtr.command;
    inFields[14] = ipMcActionPtr.destScope;
    inFields[15] = ipMcActionPtr.dest.ssmNode.type;
    inFields[16] = ipMcActionPtr.dest.ssmNode.lpmIndex;
    inFields[17] = ipMcActionPtr.dest.asmRoute.routeEntryMethod;
    inFields[18] = ipMcActionPtr.dest.asmRoute.routeEntryBaseMemAddr;
    inFields[19] = ipMcActionPtr.dest.asmRoute.blockSize;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgIpMcRuleGetNext
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*   The function gets the IP MC Rule valid, Mask, Pattern and Action.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       dev             - device number
*      ruleIndex        - index of the rule in the TCAM. The rule index defines
*                          order of action resolution in the cases of multiple
*                          rules match with packet's search key. Action of the
*                          matched rule with lowest index is taken in this case.
*                          IP MC Rules and Policy tules are used same TCAM.
*                          Rule index for IP MC Rule rules may be in the range
*                          from 0 up to 2048.
*
* OUTPUTS:
*       isValidPtr       - weather to set the rule as valid or invalid. in the
*                         case of invalid rule , the maskPtr, patternPtr and
*                         ipMcActionPtr can be NULL since they are irrelevant.
*
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*
*       patternPtr       - rule pattern.
*
*       ipMcActionPtr   - IP MC action rule action that applied on packet if
*                          packet's IP MC search key matched with masked pattern.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_STATE   - the rule invalid
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgIpMcRuleGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                             dev;
    GT_BOOL                           isValid;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       maskPtr;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       patternPtr;
    CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC ipMcActionPtr;
    GT_BYTE_ARRY                      bArr;

    GT_U8                             dataStr[16];
    bArr.data  =  dataStr;
    bArr.length = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;



    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    do
    {
        /* call cpss api function */
        result = cpssExMxTgIpMcRuleGet(dev, gRuleIndexGet, &isValid, &maskPtr,
                                   &patternPtr, &ipMcActionPtr);
        gRuleIndexGet++;

        if (gRuleIndexGet >= MAX_NUM_OF_RULES)
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
    }while(!isValid);

    inFields[0] = gRuleIndexGet;
    inFields[1] = maskPtr.pclId;
    inFields[2] = maskPtr.fwdEnable;
    inFields[3] = maskPtr.mngEnable;
    inFields[4] = maskPtr.portLifNum;

    cmdOsMemCpy(bArr.data, maskPtr.dip, 16);
    bArr.length = 16;
    inFields[5] = (GT_U32)galtisBArrayOut(&bArr);

    inFields[6] = maskPtr.vid;
    inFields[7] = patternPtr.pclId;
    inFields[8] = patternPtr.fwdEnable;
    inFields[9] = patternPtr.mngEnable;
    inFields[10] = patternPtr.portLifNum;


    cmdOsMemCpy(bArr.data, patternPtr.dip, 16);
    bArr.length = 16;
    inFields[11] = (GT_U32)galtisBArrayOut(&bArr);

    inFields[12] = patternPtr.vid;
    inFields[13] = ipMcActionPtr.command;
    inFields[14] = ipMcActionPtr.destScope;
    inFields[15] = ipMcActionPtr.dest.ssmNode.type;
    inFields[16] = ipMcActionPtr.dest.ssmNode.lpmIndex;
    inFields[17] = ipMcActionPtr.dest.asmRoute.routeEntryMethod;
    inFields[18] = ipMcActionPtr.dest.asmRoute.routeEntryBaseMemAddr;
    inFields[19] = ipMcActionPtr.dest.asmRoute.blockSize;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxTgIpMcRuleActionUpdate
*
* DESCRIPTION:
*   The function updates the IP MC Rule  Action
*
*
* INPUTS:
*       dev             - device number
*       ruleIndex       - index of the rule in the TCAM.
*                         See cpssExMxTgIpMcRuleSet.
*
*       actionPtr       - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
*       ipMcActionPtr   - IP MC action rule action that applied on packet if
*                          packet's IP MC search key matched with masked pattern.
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
CMD_STATUS wrCpssExMxTgIpMcRuleActionUpdate

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                             dev;
    GT_U32                            ruleIndex;
    CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC ipMcActionPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inArgs[1];

    ipMcActionPtr.command =  (CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_ENT)inArgs[2];
    ipMcActionPtr.destScope =
                       (CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_SCOPE_ENT)inArgs[3];

    if(inArgs[2] == 0)
    {
        ipMcActionPtr.dest.ssmNode.type =
                       (CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_TYPE_ENT)inArgs[4];
        ipMcActionPtr.dest.ssmNode.lpmIndex = (GT_U32)inArgs[5];
    }

    if(inArgs[2] == 1)
    {
#ifdef NATAN
        ipMcActionPtr.dest.asmRoute.routeEntryMethod =
                  (CPSS_EXMX_TG_PCL_IP_MC_ACTION_ROUTING_METHOD_ENT)inArgs[6];
        ipMcActionPtr.dest.asmRoute.routeEntryBaseMemAddr = (GT_U32)inArgs[7];
        ipMcActionPtr.dest.asmRoute.blockSize = (GT_U8)inArgs[8];
#endif
    }


/* call cpss api function */
    result = cpssExMxTgIpMcRuleActionUpdate(dev, ruleIndex, &ipMcActionPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgIpMcRuleActionGet
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*   The function gets the IP MC Rule Action
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       dev             - device number
*       ruleIndex       - index of the rule in the TCAM.
*                         See cpssExMxTgPclIpMcRuleSet.
*
*
* OUTPUTS:
*       ipMcActionPtr   - IP MC action rule action that applied on packet if
*                          packet's IP MC search key matched with masked pattern.
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
CMD_STATUS wrCpssExMxTgIpMcRuleActionGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                             dev;
    GT_U32                            ruleIndex;
    CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC ipMcActionPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgIpMcRuleActionGet(dev, ruleIndex, &ipMcActionPtr);

    if(result != GT_OK)
    {
        return CMD_AGENT_ERROR;
    }

    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d",
                 ipMcActionPtr.command,
                 ipMcActionPtr.destScope,
                 ipMcActionPtr.dest.ssmNode.type,
                 ipMcActionPtr.dest.ssmNode.lpmIndex,
                 ipMcActionPtr.dest.asmRoute.routeEntryMethod,
                 ipMcActionPtr.dest.asmRoute.routeEntryBaseMemAddr,
                 ipMcActionPtr.dest.asmRoute.blockSize);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgIpv6McPclIdSet
*
* DESCRIPTION:
*   The function set PCL ID for IPV6 Multicast Group lookup key
*
*
* INPUTS:
*       dev             - device number
*       pclId           - PCL Id
*
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
CMD_STATUS wrCpssExMxTgIpv6McPclIdSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  dev;
    GT_U32 pclId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    pclId = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgIpv6McPclIdSet(dev, pclId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/* up to here was formerly implemented in PCL */

/*******************************************************************************
* cpssExMxTgIpv6McDefaultActionIndexSet
*
* DESCRIPTION:
*    Function Relevant mode : All modes
*       sets the index for action that will be selected if IPV6 Multicast TCAM
*       lookup will not match any rule.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       dev             - device number
*       actionIndex     - number of Action in action table
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
CMD_STATUS wrCpssExMxTgIpv6McDefaultActionIndexSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  dev;
    GT_U32 actionIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    actionIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgIpv6McDefaultActionIndexSet(dev, actionIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*ctrl*/

/*******************************************************************************
* cpssExMxTgIpMcMirrorCpuCodeEnable
*
* DESCRIPTION:
*       Function Relevant mode: All modes
*       enable/diable the mc mirror cpu code.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       devNum  - the device number.
*       enable  - weather to enable the mc mirror cpu code.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - not supported on one or more device in the system
*
* COMMENTS:
*       This feature is not supported on all devices
*
*
* GalTis:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgIpMcMirrorCpuCodeEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgIpMcMirrorCpuCodeEnable(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgIpCtrlEcmpHashModeSet
*
* DESCRIPTION:
*       Function Relevant mode: All modes
*       Sets ECMP hash mode on a specific device.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       devNum       - the device number.
*       protocolStack - types of IP protocol stack to set.
*       ecmpHashMode - the ecmp hash mode. wether it takes into account the next
*                      header protocol or not.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED - The library was not initialized.
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgIpCtrlEcmpHashModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                dev;
    CPSS_IP_PROTOCOL_STACK_ENT           protocolStack;
    CPSS_EXMX_TG_IP_ECMP_HASH_MODE_ENT   ecmpHashMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    ecmpHashMode =  (CPSS_EXMX_TG_IP_ECMP_HASH_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxTgIpCtrlEcmpHashModeSet(dev, protocolStack, ecmpHashMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgIpCtrlCosRoutingModeSet
*
* DESCRIPTION:
*       Function Relevant mode: All modes
*       Sets Cos routing hash mode on a specific device.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       devNum       - the device number.
*       cosRouteMode - the cos routing hash mode. wether it uses a module or LSB
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED - The library was not initialized.
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgIpCtrlCosRoutingModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                          dev;
    CPSS_EXMX_TG_IP_COS_ROUTING_HASH_MODE_ENT      cosRouteMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    cosRouteMode = (CPSS_EXMX_TG_IP_COS_ROUTING_HASH_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgIpCtrlCosRoutingModeSet(dev, cosRouteMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgIpCtrlMcMacMatchEnable
*
* DESCRIPTION:
*       Function Relevant mode: All modes
*       Enable/Disable verification of match between destination mcast MAC
*       address and IP destination IPM address (group). Used in Illegal Address
*       exception checking.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       devNum        - the device number
*       protocolStack - types of IP protocol stack to set.
*       enable        - weather to enable the check
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED - The library was not initialized.
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgIpCtrlMcMacMatchEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           dev;
    CPSS_IP_PROTOCOL_STACK_ENT      protocolStack;
    GT_BOOL                         enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxTgIpCtrlMcMacMatchEnable(dev, protocolStack, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgIpCtrlL2L3ByteCountMatchEnable
*
* DESCRIPTION:
*       Function Relevant mode: All modes
*       Enable/Disable verification of match between byte counts in L3 and L2
*       headers on a specific device. Used in Bad Header exception checking.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       devNum        - the device number.
*       protocolStack - types of IP protocol stack to set.
*       enable        - weather to enable the check
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED - The library was not initialized.
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgIpCtrlL2L3ByteCountMatchEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           dev;
    CPSS_IP_PROTOCOL_STACK_ENT      protocolStack;
    GT_BOOL                         enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxTgIpCtrlL2L3ByteCountMatchEnable(dev, protocolStack, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgIpv6AddrPrefixScopeSet
*
* DESCRIPTION:
*       Function Relevant mode: All modes
*       Defines a prefix of a scope type.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       dev              - physical device number
*       prefix           - an IPv6 address prefix
*       prefixLen        - length of the prefix (0 - 16)
*       addressScope     - type of the address scope spanned by the prefix
*       prefixScopeIndex - index of the new prefix scope entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_BAD_PARAM       - case that the prefix length is out of range.
*       GT_NOT_SUPPORTED - if working with a PP that doesn't support IPv6
*
* COMMENTS:
*       Configures an entry in the prefix look up table
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgIpv6AddrPrefixScopeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           dev;
    GT_IPV6ADDR                     prefix;
    GT_U32                          prefixLen;
    CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT addressScope;
    GT_U32                          prefixScopeIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    galtisIpv6Addr(&prefix, (GT_U8*)inArgs[1]);
    prefixLen = (GT_U32)inArgs[2];
    addressScope = (CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT)inArgs[3];
    prefixScopeIndex = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssExMxTgIpv6AddrPrefixScopeSet(dev, prefix, prefixLen,
                                              addressScope, prefixScopeIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgIpv6UcBorderCrossCmdSet
*
* DESCRIPTION:
*       Function Relevant mode: All modes
*       Define an action for a type of border crossing
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       dev              - physical device number
*       addressScopeSrc  - IPv6 address scope of source address
*       addressScopeDest - IPv6 address scope of destination address
*       borderCrossed    - GT_TRUE if source site ID is diffrent to destination
*                          site ID
*       cmd              - action to be done on a packet that crosses the border
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success.
*       GT_FAIL          - on error.
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_NO_RESOURCE - if failed to allocate CPU memory
*       GT_NOT_SUPPORTED - if working with a PP that doesn't support IPv6
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgIpv6UcBorderCrossCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           dev;
    CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT addressScopeSrc;
    CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT addressScopeDest;
    GT_BOOL                         borderCrossed;
    CPSS_EXMX_IP_EXCP_CMD_ENT       cmd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    addressScopeSrc = (CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT)inArgs[1];
    addressScopeDest = (CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT)inArgs[2];
    borderCrossed = (GT_BOOL)inArgs[3];
    cmd = (CPSS_EXMX_IP_EXCP_CMD_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssExMxTgIpv6UcBorderCrossCmdSet(dev, addressScopeSrc,
                                               addressScopeDest, borderCrossed,
                                               cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgIpv6McBorderCrossCmdSet
*
* DESCRIPTION:
*       Function Relevant mode: All modes
*       Define an action for a type of border crossing.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       dev              - physical device number
*       addressScopeSrc  - IPv6 address scope of source address
*       addressScopeDest - IPv6 address scope of destination address
*       borderCrossed    - GT_TRUE if source site ID is equal to destination site ID
*       cmd              - action to be done on a packet that crosses the border
*       mllSelectionRule - rule for choosing MLL for IPv6 Multicast propogation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success.
*       GT_FAIL          - on error.
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_NO_RESOURCE - if failed to allocate CPU memory
*
* COMMENTS:
*       Configures an entry in the prefix look up table
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTgIpv6McBorderCrossCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                       dev;
    CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT             addressScopeSrc;
    CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT             addressScopeDest;
    GT_BOOL                                     borderCrossed;
    CPSS_EXMX_IP_EXCP_CMD_ENT                   cmd;
    CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_ENT    mllSelectionRule;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    addressScopeSrc = (CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT)inArgs[1];
    addressScopeDest = (CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT)inArgs[2];
    borderCrossed = (GT_BOOL)inArgs[3];
    cmd = (CPSS_EXMX_IP_EXCP_CMD_ENT)inArgs[4];
    mllSelectionRule = (CPSS_EXMX_TG_IPV6_MLL_SELECTION_RULE_ENT)inArgs[5];

    /* call cpss api function */
    result = cpssExMxTgIpv6McBorderCrossCmdSet(dev, addressScopeSrc,
                                               addressScopeDest, borderCrossed,
                                               cmd, mllSelectionRule);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxTgIpMcRule2Set",
        &wrCpssExMxTgIpMcRuleSet,
        1, 20},

    {"cpssExMxTgIpMcRule2GetFirst",
        &wrCpssExMxTgIpMcRuleGetFirst,
        1, 0},

    {"cpssExMxTgIpMcRule2GetNext",
        &wrCpssExMxTgIpMcRuleGetNext,
        1, 0},

    {"cpssExMxTgIpMcRuleActionUpdate",
        &wrCpssExMxTgIpMcRuleActionUpdate,
        9, 0},

    {"cpssExMxTgIpMcRuleActionGet",
        &wrCpssExMxTgIpMcRuleActionGet,
        2, 0},

    {"cpssExMxTgIpv6McPclIdSet",
        &wrCpssExMxTgIpv6McPclIdSet,
        2, 0},

    {"cpssExMxTgIpv6McDefaultActionIndexSet",
        &wrCpssExMxTgIpv6McDefaultActionIndexSet,
        2, 0},

    /*ctrl*/
    {"cpssExMxTgIpMcMirrorCpuCodeEnable",
        &wrCpssExMxTgIpMcMirrorCpuCodeEnable,
        2, 0},

    {"cpssExMxTgIpCtrlEcmpHashModeSet",
        &wrCpssExMxTgIpCtrlEcmpHashModeSet,
        3, 0},

    {"cpssExMxTgIpCtrlCosRoutingModeSet",
        &wrCpssExMxTgIpCtrlCosRoutingModeSet,
        2, 0},

    {"cpssExMxTgIpCtrlMcMacMatchEnable",
        &wrCpssExMxTgIpCtrlMcMacMatchEnable,
        3, 0},

    {"cpssExMxTgIpCtrlL2L3ByteCountMatchEnable",
        &wrCpssExMxTgIpCtrlL2L3ByteCountMatchEnable,
        3, 0},

    {"cpssExMxTgIpv6AddrPrefixScopeSet",
        &wrCpssExMxTgIpv6AddrPrefixScopeSet,
        5, 0},

    {"cpssExMxTgIpv6UcBorderCrossCmdSet",
        &wrCpssExMxTgIpv6UcBorderCrossCmdSet,
        5, 0},

    {"cpssExMxTgIpv6McBorderCrossCmdSet",
        &wrCpssExMxTgIpv6McBorderCrossCmdSet,
        6, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxTgIp
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
GT_STATUS cmdLibInitCpssExMxTgIp
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


