/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxTgTunnelTerm.c
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
#include <cpss/exMx/exMxTg/tunnel/cpssExMxTgTunnelTerm.h>

/*******************************************************************************
* cpssExMxTgTunnelTermInterfaceModeSet
*
* DESCRIPTION:
*       This function sets tunnel terminate interface mode
*
* APPLICABLE DEVICES:
*       EX1x6.
*
* INPUTS:
*       devNum        - device number
*       mode          - tunnel terminate interface mode. valid values:
*                       CPSS_EXMXTG_TUNNEL_TERM_INTERFACE_REGISTER_MODE_E, 
*                       CPSS_EXMXTG_TUNNEL_TERM_INTERFACE_ROUTE_ENTRY_MODE_E    
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
*       It is not applicable for expedited tunnel termination.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelTermInterfaceModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXTG_TUNNEL_TERM_INTERFACE_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXTG_TUNNEL_TERM_INTERFACE_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgTunnelTermInterfaceModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgTunnelTermInterfaceModeGet
*
* DESCRIPTION:
*       This function gets tunnel terminate interface mode.
*
* APPLICABLE DEVICES:
*       EX1x6.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       modePtr       - pointer to tunnel terminate interface mode.Valid values:
*                       CPSS_EXMXTG_TUNNEL_TERM_INTERFACE_REGISTER_MODE_E, 
*                       CPSS_EXMXTG_TUNNEL_TERM_INTERFACE_ROUTE_ENTRY_MODE_E    
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*       GT_FAIL       - on error
*
* COMMENTS:
*       It is not applicable for expedited tunnel termination.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelTermInterfaceModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXTG_TUNNEL_TERM_INTERFACE_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxTgTunnelTermInterfaceModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgTunnelTermOutLifSet
*
* DESCRIPTION:
*       This function sets output logical interface and pointer to ARP entry in
*       IP Tunnel Terminate Registers.
*
* APPLICABLE DEVICES:
*       EX1x6.
*
* INPUTS:
*       devNum        - device number
*       outlifPtr     - pointer to output logical interface 
*       arpIndex      - ARP entry index containing MAC address
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or mode
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       It is not applicable for expedited tunnel termination.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelTermOutLifSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMX_IP_OUTLIF_CONFIG_STC outlif;
    GT_U32 arpIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    outlif.outlifType = CPSS_EXMX_IP_OUTLIF_LL_E;
    outlif.outlifInfo.linkLayerOutLif.vid = (GT_U16)inFields[0];
    outlif.outlifInfo.linkLayerOutLif.useVidx = (GT_BOOL)inFields[1];
    if(outlif.outlifInfo.linkLayerOutLif.useVidx)
    {
        /* vidx */
        outlif.outlifInfo.linkLayerOutLif.vidxOrTrunkPort.vidx = (GT_U16)inFields[2];
    }
    else
    {
        outlif.outlifInfo.linkLayerOutLif.vidxOrTrunkPort.tarInfo.tarIsTrunk = (GT_BOOL)inFields[3];
        if(outlif.outlifInfo.linkLayerOutLif.vidxOrTrunkPort.tarInfo.tarIsTrunk) 
            /* trunk */
            outlif.outlifInfo.linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.trunkInfo.tarTrunk = (GT_U8)inFields[4];
        else
        {
            /* port */
            outlif.outlifInfo.linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarDev = (GT_U8)inFields[5];
            outlif.outlifInfo.linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarPort = (GT_U8)inFields[6];
        }
    }
    arpIndex = inFields[7];
    
    /* call cpss api function */
    result = cpssExMxTgTunnelTermOutLifSet(devNum, &outlif, arpIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgTunnelTermOutLifGet
*
* DESCRIPTION:
*       This function gets output logical interface and pointer to ARP entry in
*       IP Tunnel Terminate Registers.
*
* APPLICABLE DEVICES:
*       EX1x6.
*
* INPUTS:
*       devNum        - device number
*        
*
* OUTPUTS:
*       outlifPtr     - pointer to output logical interface
*       arpIndexPtr   - pointer to ARP entry index containing MAC address
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or mode
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       It is not applicable for expedited tunnel termination.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelTermOutLifGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMX_IP_OUTLIF_CONFIG_STC outlif;
    GT_U32 arpIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
 
    /* init with zeros */
    cmdOsMemSet(&outlif, 0, sizeof(outlif));

    /* call cpss api function */
    result = cpssExMxTgTunnelTermOutLifGet(devNum, &outlif, &arpIndex);

    inFields[0] = outlif.outlifType;
    inFields[1] = outlif.outlifInfo.linkLayerOutLif.vid;
    inFields[2] = outlif.outlifInfo.linkLayerOutLif.useVidx;
    if(outlif.outlifInfo.linkLayerOutLif.useVidx)
    {
        /* vidx */
        inFields[3] = outlif.outlifInfo.linkLayerOutLif.vidxOrTrunkPort.vidx;
    }
    else
    {
        inFields[3] = outlif.outlifInfo.linkLayerOutLif.vidxOrTrunkPort.tarInfo.tarIsTrunk;
        if(outlif.outlifInfo.linkLayerOutLif.vidxOrTrunkPort.tarInfo.tarIsTrunk) 
            /* trunk */
            inFields[4] = outlif.outlifInfo.linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.trunkInfo.tarTrunk;
        else
        {
            /* port */
            inFields[4] = outlif.outlifInfo.linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarDev;
            inFields[5] = outlif.outlifInfo.linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarPort;
        }
    }
    inFields[6] = arpIndex;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelTermOutLifGetNext
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
* cpssExMxTgTunnelTermExpeditedTableEntrySet
*
* DESCRIPTION:
*       Configures an expedited tunnel termination entry at a specified 
*       expedited tunnel index.
*
* APPLICABLE DEVICES:
*       EX1x6.
*
* INPUTS:
*       devNum                    - device number
*       expTunnelIndex            - index of expedited tunnel. 
*                                   The index range is 0...7 .
*       expTunnelTermEntryCnfgPtr - pointer to the structure of configure parameters 
*                                   of the expedited tunnel termination entry.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or mode
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       The expedited termination is applicable for IPv6-over-IPv4(GRE) tunnel.
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelTermExpeditedTableEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 expTunnelIndex;
    CPSS_EXMX_TG_TUNNEL_TERM_EXPEDITED_ENTRY_STC expTunnelTermEntryCnfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    expTunnelIndex = inFields[0];
    galtisIpAddr(&expTunnelTermEntryCnfg.destIp,(GT_U8*)inFields[1]);
    galtisIpAddr(&expTunnelTermEntryCnfg.srcIp,(GT_U8*)inFields[2]);
    expTunnelTermEntryCnfg.srcIpPrefixLength = inFields[3];
    expTunnelTermEntryCnfg.vlanId = (GT_U16)inFields[4];
    expTunnelTermEntryCnfg.failVlanCmd = (CPSS_EXMXTG_TUNNEL_TERM_EXPEDITED_IPV4_CMD_ENT)inFields[5];
    
    /* call cpss api function */
    result = cpssExMxTgTunnelTermExpeditedTableEntrySet(devNum, expTunnelIndex, &expTunnelTermEntryCnfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgTunnelTermExpeditedTableEntryGet
*
* DESCRIPTION:
*       Get an expedited tunnel termination entry at a specified 
*       expedited tunnel index.
*
* APPLICABLE DEVICES:
*       EX1x6.
*
* INPUTS:
*       devNum                    - device number
*       expTunnelIndex            - index of expedited tunnel. 
*                                   The index range is 0...7 .
*       
* OUTPUTS:
*       expTunnelTermEntryCnfgPtr - pointer to the structure of configure parameters 
*                                   of the expedited tunnel termination entry.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or mode
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       The expedited termination is applicable for IPv6-over-IPv4(GRE) tunnel.
*******************************************************************************/
static GT_U32 expTunnelIndex;

static CMD_STATUS wrCpssExMxTgTunnelTermExpeditedTableEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMX_TG_TUNNEL_TERM_EXPEDITED_ENTRY_STC expTunnelTermEntryCnfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(expTunnelIndex > 7)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxTgTunnelTermExpeditedTableEntryGet(devNum, expTunnelIndex, &expTunnelTermEntryCnfg);

    inFields[0] = expTunnelIndex;
    inFields[3] = expTunnelTermEntryCnfg.srcIpPrefixLength;
    inFields[4] = expTunnelTermEntryCnfg.vlanId;
    inFields[5] = expTunnelTermEntryCnfg.failVlanCmd;

    expTunnelIndex++;

    /* pack and output table fields */
    fieldOutput("%d%4b%4b%d%d%d",
                inFields[0], expTunnelTermEntryCnfg.destIp.arIP, expTunnelTermEntryCnfg.srcIp.arIP, 
                inFields[3], inFields[4], inFields[5]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}
/*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelTermExpeditedTableEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    expTunnelIndex = 0;
    return wrCpssExMxTgTunnelTermExpeditedTableEntryGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelTermExpeditedTableEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssExMxTgTunnelTermExpeditedTableEntryGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssExMxTgTunnelTermExpeditedExeptionCmdSet
*
* DESCRIPTION:
*       This function sets the command that should be taken if  
*       packet identified by expedited tunnel termination mechanism is found to
*       be incorrect.
*
* APPLICABLE DEVICES:
*       EX1x6.
*
* INPUTS:
*       devNum          - device number
*       failPacketCmd   - tunnel associated packet fail command. Valid values:
*                         CPSS_EXMXTG_TUNNEL_TERM_EXPEDITED_EXCEPTION_CMD_DONOTHING_E,
*                         CPSS_EXMXTG_TUNNEL_TERM_EXPEDITED_EXCEPTION_CMD_DROP_E,
*                         CPSS_EXMXTG_TUNNEL_TERM_EXPEDITED_EXCEPTION_CMD_TRAP_E                    
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
*       The expedited termination is applicable for IPv6-over-IPv4(GRE) tunnel.
*       Having triggered the expedited tunnel termination, the packet is verified
*       for correctness. If the packet is found to be incorrect, it is treated 
*       according to the <BadHdrInTnlCmd> field.
*       If the packet is trapped to the CPU, it is sent 
*       with CPU code <EXP_TNL_BAD_IPV4_HDR>.
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelTermExpeditedExeptionCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXTG_TUNNEL_TERM_EXPEDITED_EXCEPTION_CMD_ENT failPacketCmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    failPacketCmd = (CPSS_EXMXTG_TUNNEL_TERM_EXPEDITED_EXCEPTION_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgTunnelTermExpeditedExeptionCmdSet(devNum, failPacketCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgTunnelTermExpeditedExeptionCmdGet
*
* DESCRIPTION:
*       This function gets the command that should be taken if  
*       packet identified by expedited tunnel termination mechanism is found to
*       be incorrect.
*
* APPLICABLE DEVICES:
*       EX1x6.
*
* INPUTS:
*       devNum            - device number
*       
*
* OUTPUTS:
*       failPacketCmdPtr - pointer to tunnel associated packet fail command.
*                         valid values:
*                         CPSS_EXMXTG_TUNNEL_TERM_EXPEDITED_EXCEPTION_CMD_DONOTHING_E,
*                         CPSS_EXMXTG_TUNNEL_TERM_EXPEDITED_EXCEPTION_CMD_DROP_E,
*                         CPSS_EXMXTG_TUNNEL_TERM_EXPEDITED_EXCEPTION_CMD_TRAP_E                    
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or mode
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*       GT_FAIL       - on error
*
* COMMENTS:
*       The expedited termination is applicable for IPv6-over-IPv4(GRE) tunnel.
*       Having triggered the expedited tunnel termination, the packet is verified
*       for correctness. If the packet is found to be incorrect, it is treated 
*       according to the <BadHdrInTnlCmd> field.
*       If the packet is trapped to the CPU, it is sent 
*       with CPU code <EXP_TNL_BAD_IPV4_HDR>.
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelTermExpeditedExeptionCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXTG_TUNNEL_TERM_EXPEDITED_EXCEPTION_CMD_ENT failPacketCmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxTgTunnelTermExpeditedExeptionCmdGet(devNum, &failPacketCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", failPacketCmd);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgTunnelTermExpeditedPacketCounterGet
*
* DESCRIPTION:
*       This function gets Number of IPv6-over-IPv4 packets identified for 
*       expedited tunnel termination.
*
* APPLICABLE DEVICES:
*       EX1x6.
*
* INPUTS:
*       devNum            - device number
*       counterSetIndex   - counter set index.  The index range is 0...2 .
*
* OUTPUTS:
*       packetNumberPtr - pointer to number of packets identified for 
*       expedited tunnel termination.                  
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or mode
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       The expedited termination is applicable for IPv6-over-IPv4(GRE) tunnel.
*       
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelTermExpeditedPacketCounterGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 counterSetIndex;
    GT_U32 packetNumber;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    counterSetIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgTunnelTermExpeditedPacketCounterGet(devNum, counterSetIndex, &packetNumber);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", packetNumber);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgTunnelTermPolicyRedirectModeSet
*
* DESCRIPTION:
*       This function sets Policy Redirect Mode bit. This bit controls the 
*       alteration of the packet’s VID when the packet is redirected
*       by the Policy Action Command.
*
* APPLICABLE DEVICES:
*       EX1x6.
*
* INPUTS:
*       devNum        - device number
*       mode          - VID alteration mode. valid values:
*                       CPSS_EXMXTG_TUNNEL_TERM_PCL_VID_ALTERATION_MODE_E, 
*                       CPSS_EXMXTG_TUNNEL_TERM_PCL_ENABLED_VID_ALTERATION_MODE_E    
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or mode
*       GT_HW_ERROR   - on hardware error
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelTermPolicyRedirectModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXTG_TUNNEL_TERM_PCL_VID_ALTERATION_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXTG_TUNNEL_TERM_PCL_VID_ALTERATION_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgTunnelTermPolicyRedirectModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgTunnelTermPolicyRedirectModeGet
*
* DESCRIPTION:
*       This function sets Policy Redirect Mode bit. This bit controls the 
*       alteration of the packet’s VID when the packet is redirected
*       by the Policy Action Command.
*
* APPLICABLE DEVICES:
*       EX1x6.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       modePtr       - VID alteration mode. valid values:
*                       CPSS_EXMXTG_TUNNEL_TERM_PCL_VID_ALTERATION_MODE_E, 
*                       CPSS_EXMXTG_TUNNEL_TERM_PCL_ENABLED_VID_ALTERATION_MODE_E    
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*       GT_FAIL       - on error
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelTermPolicyRedirectModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXTG_TUNNEL_TERM_PCL_VID_ALTERATION_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxTgTunnelTermPolicyRedirectModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgTunnelTermExpeditedPacketCounterSet
*
* DESCRIPTION:
*       This function sets counter of IPv6-over-IPv4 packets identified for 
*       expedited tunnel termination.
*
* APPLICABLE DEVICES:
*       EX1x6.
*
* INPUTS:
*       devNum            - device number
*       counterSetIndex   - counter set index.  The index range is 0...2 .
*       counterInitValue  - initial value for expedited packet counter                  
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
*       The expedited termination is applicable for IPv6-over-IPv4(GRE) tunnel.
*       
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgTunnelTermExpeditedPacketCounterSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 counterSetIndex;
    GT_U32 counterInitValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    counterSetIndex = inArgs[1];
    counterInitValue = inArgs[2];

    /* call cpss api function */
    result = cpssExMxTgTunnelTermExpeditedPacketCounterSet(devNum, counterSetIndex, counterInitValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxTgTunnelTermInterfaceModeSet",
         &wrCpssExMxTgTunnelTermInterfaceModeSet,
         2, 0},
        {"cpssExMxTgTunnelTermInterfaceModeGet",
         &wrCpssExMxTgTunnelTermInterfaceModeGet,
         1, 0},
        {"cpssExMxTgTunnelTermOutLifSet",
         &wrCpssExMxTgTunnelTermOutLifSet,
         1, 8},
        {"cpssExMxTgTunnelTermOutLifGetFirst",
         &wrCpssExMxTgTunnelTermOutLifGetFirst,
         1, 0},
        {"cpssExMxTgTunnelTermOutLifGetNext",
         &wrCpssExMxTgTunnelTermOutLifGetNext,
         1, 0},
        {"cpssExMxTgTunnelTermExpeditedTableEntrySet",
         &wrCpssExMxTgTunnelTermExpeditedTableEntrySet,
         1, 6},
        {"cpssExMxTgTunnelTermExpeditedTableEntryGetFirst",
         &wrCpssExMxTgTunnelTermExpeditedTableEntryGetFirst,
         1, 0},
        {"cpssExMxTgTunnelTermExpeditedTableEntryGetNext",
         &wrCpssExMxTgTunnelTermExpeditedTableEntryGetNext,
         1, 0},
        {"cpssExMxTgTunnelTermExpeditedExeptionCmdSet",
         &wrCpssExMxTgTunnelTermExpeditedExeptionCmdSet,
         2, 0},
        {"cpssExMxTgTunnelTermExpeditedExeptionCmdGet",
         &wrCpssExMxTgTunnelTermExpeditedExeptionCmdGet,
         1, 0},
        {"cpssExMxTgTunnelTermExpeditedPacketCounterGet",
         &wrCpssExMxTgTunnelTermExpeditedPacketCounterGet,
         2, 0},
        {"cpssExMxTgTunnelTermPolicyRedirectModeSet",
         &wrCpssExMxTgTunnelTermPolicyRedirectModeSet,
         2, 0},
        {"cpssExMxTgTunnelTermPolicyRedirectModeGet",
         &wrCpssExMxTgTunnelTermPolicyRedirectModeGet,
         1, 0},
        {"cpssExMxTgTunnelTermExpeditedPacketCounterSet",
         &wrCpssExMxTgTunnelTermExpeditedPacketCounterSet,
         3, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxTgTunnelTerm
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
GT_STATUS cmdLibInitCpssExMxTgTunnelTerm
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

