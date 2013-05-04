/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmBrgVlan.c
*
* DESCRIPTION:
*       Wrapper functions for cpssExMxPmBrgVlan cpss.exMxPm functions
*
* DEPENDENCIES:
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
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgVlan.h>


/* ExMxPm max number of entries to be read from tables\classes */
#define  CPSS_EXMXPM_VLAN_MAX_ENTRY_CNS 4096


/* max index of entry that may be configured to support
   port protocol based VLANs and QoS */
#define CPSS_EXMXPM_PORT_PROT_MAX_ENTRY_CNS  (11)


/* cpssExMxPmBrgVlanEntry Table [DB+] ----    */
static GT_U16   vlanIdCnt;

/*
* cpssExMxPmBrgVlanEntryWrite
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW .
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       vlanId              - VLAN Id
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan
*       vlanInfoPtr         - (pointer to) VLAN specific information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is out
*                         of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       This function override the exist entry.
*
*******************************************************************************/
/*Puma 2 wrapper*/
static CMD_STATUS wrCpssExMxPm2BrgVlanEntryWrite

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;

    GT_U8                            devNum;
    GT_U16                           vlanId;
    CPSS_PORTS_BMP_STC               portsMembersPtr;
    CPSS_PORTS_BMP_STC               portsTaggingPtr;
    CPSS_EXMXPM_BRG_VLAN_INFO_STC    vlanInfoPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    vlanId = (GT_U16)inFields[0];

    portsMembersPtr.ports[0] = (GT_U32)inFields[1];
    portsMembersPtr.ports[1] = (GT_U32)inFields[2];
    portsTaggingPtr.ports[0] = (GT_U32)inFields[3];
    portsTaggingPtr.ports[1] = (GT_U32)inFields[4];

    vlanInfoPtr.cpuMember = (GT_BOOL)inFields[5];
    vlanInfoPtr.mirrorToTxAnalyzerEn = (GT_BOOL)inFields[6];
    vlanInfoPtr.ipv4IpmBrgMode = (CPSS_BRG_IPM_MODE_ENT)inFields[7];
    vlanInfoPtr.ipv6IpmBrgMode = (CPSS_BRG_IPM_MODE_ENT)inFields[8];
    vlanInfoPtr.ipv4IpmBrgEn = (GT_BOOL)inFields[9];
    vlanInfoPtr.ipv6IpmBrgEn = (GT_BOOL)inFields[10];
    vlanInfoPtr.stgId = (GT_U32)inFields[11];
    /*vlanInfoPtr.untaggedMruIdx = (GT_U32)inFields[12];*/
    vlanInfoPtr.egressUnregMcFilterCmd = (CPSS_UNREG_MC_EGR_FILTER_CMD_ENT)inFields[13];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanEntryWrite(devNum, vlanId, &portsMembersPtr,
                                         &portsTaggingPtr, &vlanInfoPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*Puma 1 wrapper*/
static CMD_STATUS wrCpssExMxPmBrgVlanEntryWrite

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;

    GT_U8                            devNum;
    GT_U16                           vlanId;
    CPSS_PORTS_BMP_STC               portsMembersPtr;
    CPSS_PORTS_BMP_STC               portsTaggingPtr;
    CPSS_EXMXPM_BRG_VLAN_INFO_STC    vlanInfoPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    vlanId = (GT_U16)inFields[0];

    portsMembersPtr.ports[0] = (GT_U32)inFields[1];
    portsMembersPtr.ports[1] = (GT_U32)inFields[2];
    portsTaggingPtr.ports[0] = (GT_U32)inFields[3];
    portsTaggingPtr.ports[1] = (GT_U32)inFields[4];

    vlanInfoPtr.cpuMember = (GT_BOOL)inFields[5];
    vlanInfoPtr.mirrorToTxAnalyzerEn = (GT_BOOL)inFields[6];
    vlanInfoPtr.ipv4IpmBrgMode = (CPSS_BRG_IPM_MODE_ENT)inFields[7];
    vlanInfoPtr.ipv6IpmBrgMode = (CPSS_BRG_IPM_MODE_ENT)inFields[8];
    vlanInfoPtr.ipv4IpmBrgEn = (GT_BOOL)inFields[9];
    vlanInfoPtr.ipv6IpmBrgEn = (GT_BOOL)inFields[10];
    vlanInfoPtr.stgId = (GT_U32)inFields[11];
    /*vlanInfoPtr.untaggedMruIdx = (GT_U32)inFields[12];*/
    vlanInfoPtr.egressUnregMcFilterCmd = CPSS_UNREG_MC_VLAN_FRWD_E;

    /* call cpss api function */
    result = cpssExMxPmBrgVlanEntryWrite(devNum, vlanId, &portsMembersPtr,
                                         &portsTaggingPtr, &vlanInfoPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanEntryRead
*
* DESCRIPTION:
*       Read vlan entry.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*
* OUTPUTS:
*       portsMembersPtr     - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr     - (pointer to) bmp of ports tagged in the vlan
*       vlanInfoPtr         - (pointer to) VLAN specific information
*       isValidPtr          - (pointer to) VLAN entry status
*                           GT_TRUE = Vlan is valid
*                           GT_FALSE = Vlan is not Valid
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
/*puma 2*/
static CMD_STATUS wrCpssExMxPm2BrgVlanEntryReadFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;

    GT_U8                           devNum;
    CPSS_PORTS_BMP_STC              portsMembersPtr;
    CPSS_PORTS_BMP_STC              portsTaggingPtr;
    CPSS_EXMXPM_BRG_VLAN_INFO_STC   vlanInfoPtr;
    GT_BOOL                         isValidPtr = GT_FALSE;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanIdCnt = 0;


    while((isValidPtr == GT_FALSE) && (vlanIdCnt < CPSS_EXMXPM_VLAN_MAX_ENTRY_CNS))
    {
        /* call cpss api function */
        result = cpssExMxPmBrgVlanEntryRead(devNum, vlanIdCnt, &portsMembersPtr,
                                  &portsTaggingPtr, &vlanInfoPtr, &isValidPtr);

        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
        vlanIdCnt++;
    }
    vlanIdCnt--;

    if(isValidPtr)
    {
        inFields[0] = vlanIdCnt;
        inFields[1] = portsMembersPtr.ports[0];
        inFields[2] = portsMembersPtr.ports[1];
        inFields[3] = portsTaggingPtr.ports[0];
        inFields[4] = portsTaggingPtr.ports[1];
        
        inFields[5] = vlanInfoPtr.cpuMember;
        inFields[6] = vlanInfoPtr.mirrorToTxAnalyzerEn;
        inFields[7] = vlanInfoPtr.ipv4IpmBrgMode;
        inFields[8] = vlanInfoPtr.ipv6IpmBrgMode;
        inFields[9] = vlanInfoPtr.ipv4IpmBrgEn;
        inFields[10] = vlanInfoPtr.ipv6IpmBrgEn;
        inFields[11] = vlanInfoPtr.stgId;
        inFields[12] = 0;/*vlanInfoPtr.untaggedMruIdx*/
        inFields[13] = vlanInfoPtr.egressUnregMcFilterCmd;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                              inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4],  inFields[5],
                              inFields[6],  inFields[7],  inFields[8],
                              inFields[9],  inFields[10], inFields[11],
                              inFields[12], inFields[13]);

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
       galtisOutput(outArgs, result, "");
    }
    return CMD_OK;
}


/*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2BrgVlanEntryReadNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;

    GT_U8                           devNum;
    CPSS_PORTS_BMP_STC              portsMembersPtr;
    CPSS_PORTS_BMP_STC              portsTaggingPtr;
    CPSS_EXMXPM_BRG_VLAN_INFO_STC   vlanInfoPtr;
    GT_BOOL                         isValidPtr = GT_FALSE;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(vlanIdCnt >= CPSS_EXMXPM_VLAN_MAX_ENTRY_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    while((isValidPtr == GT_FALSE) && (vlanIdCnt < CPSS_EXMXPM_VLAN_MAX_ENTRY_CNS))
    {
        vlanIdCnt++;
    	if(vlanIdCnt >= CPSS_EXMXPM_VLAN_MAX_ENTRY_CNS)
    	{
        	galtisOutput(outArgs, GT_OK, "%d", -1);
        	return CMD_OK;
    	}

        /* call cpss api function */
        result = cpssExMxPmBrgVlanEntryRead(devNum, vlanIdCnt, &portsMembersPtr,
                                  &portsTaggingPtr, &vlanInfoPtr, &isValidPtr);

        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
    }

    if(isValidPtr)
    {
        inFields[0] = vlanIdCnt;
        inFields[1] = portsMembersPtr.ports[0];
        inFields[2] = portsMembersPtr.ports[1];
        inFields[3] = portsTaggingPtr.ports[0];
        inFields[4] = portsTaggingPtr.ports[1];
        
        inFields[5] = vlanInfoPtr.cpuMember;
        inFields[6] = vlanInfoPtr.mirrorToTxAnalyzerEn;
        inFields[7] = vlanInfoPtr.ipv4IpmBrgMode;
        inFields[8] = vlanInfoPtr.ipv6IpmBrgMode;
        inFields[9] = vlanInfoPtr.ipv4IpmBrgEn;
        inFields[10] = vlanInfoPtr.ipv6IpmBrgEn;
        inFields[11] = vlanInfoPtr.stgId;
        inFields[12] = 0; /*vlanInfoPtr.untaggedMruIdx */
        inFields[13] = vlanInfoPtr.egressUnregMcFilterCmd;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                              inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4],  inFields[5],
                              inFields[6],  inFields[7],  inFields[8],
                              inFields[9],  inFields[10], inFields[11],
                              inFields[12], inFields[13]);

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, result, "");
    }
    return CMD_OK;
}
/*puma 1*/

static CMD_STATUS wrCpssExMxPmBrgVlanEntryReadFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;

    GT_U8                           devNum;
    CPSS_PORTS_BMP_STC              portsMembersPtr;
    CPSS_PORTS_BMP_STC              portsTaggingPtr;
    CPSS_EXMXPM_BRG_VLAN_INFO_STC   vlanInfoPtr;
    GT_BOOL                         isValidPtr = GT_FALSE;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanIdCnt = 0;


    while((isValidPtr == GT_FALSE) && (vlanIdCnt < CPSS_EXMXPM_VLAN_MAX_ENTRY_CNS))
    {
        /* call cpss api function */
        result = cpssExMxPmBrgVlanEntryRead(devNum, vlanIdCnt, &portsMembersPtr,
                                  &portsTaggingPtr, &vlanInfoPtr, &isValidPtr);

        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
        vlanIdCnt++;
    }
    vlanIdCnt--;

    if(isValidPtr)
    {
        inFields[0] = vlanIdCnt;
        inFields[1] = portsMembersPtr.ports[0];
        inFields[2] = portsMembersPtr.ports[1];
        inFields[3] = portsTaggingPtr.ports[0];
        inFields[4] = portsTaggingPtr.ports[1];
        
        inFields[5] = vlanInfoPtr.cpuMember;
        inFields[6] = vlanInfoPtr.mirrorToTxAnalyzerEn;
        inFields[7] = vlanInfoPtr.ipv4IpmBrgMode;
        inFields[8] = vlanInfoPtr.ipv6IpmBrgMode;
        inFields[9] = vlanInfoPtr.ipv4IpmBrgEn;
        inFields[10] = vlanInfoPtr.ipv6IpmBrgEn;
        inFields[11] = vlanInfoPtr.stgId;
        inFields[12] = 0; /*vlanInfoPtr.untaggedMruIdx */
        inFields[13] =GT_FALSE;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                              inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4],  inFields[5],
                              inFields[6],  inFields[7],  inFields[8],
                              inFields[9],  inFields[10], inFields[11],
                              inFields[12], inFields[13]);

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
       galtisOutput(outArgs, result, "");
    }
    return CMD_OK;
}


/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanEntryReadNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;

    GT_U8                           devNum;
    CPSS_PORTS_BMP_STC              portsMembersPtr;
    CPSS_PORTS_BMP_STC              portsTaggingPtr;
    CPSS_EXMXPM_BRG_VLAN_INFO_STC   vlanInfoPtr;
    GT_BOOL                         isValidPtr = GT_FALSE;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(vlanIdCnt >= CPSS_EXMXPM_VLAN_MAX_ENTRY_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    while((isValidPtr == GT_FALSE) && (vlanIdCnt < CPSS_EXMXPM_VLAN_MAX_ENTRY_CNS))
    {
        vlanIdCnt++;
    	if(vlanIdCnt >= CPSS_EXMXPM_VLAN_MAX_ENTRY_CNS)
    	{
        	galtisOutput(outArgs, GT_OK, "%d", -1);
        	return CMD_OK;
    	}

        /* call cpss api function */
        result = cpssExMxPmBrgVlanEntryRead(devNum, vlanIdCnt, &portsMembersPtr,
                                  &portsTaggingPtr, &vlanInfoPtr, &isValidPtr);

        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
    }

    if(isValidPtr)
    {
        inFields[0] = vlanIdCnt;
        inFields[1] = portsMembersPtr.ports[0];
        inFields[2] = portsMembersPtr.ports[1];
        inFields[3] = portsTaggingPtr.ports[0];
        inFields[4] = portsTaggingPtr.ports[1];
        
        inFields[5] = vlanInfoPtr.cpuMember;
        inFields[6] = vlanInfoPtr.mirrorToTxAnalyzerEn;
        inFields[7] = vlanInfoPtr.ipv4IpmBrgMode;
        inFields[8] = vlanInfoPtr.ipv6IpmBrgMode;
        inFields[9] = vlanInfoPtr.ipv4IpmBrgEn;
        inFields[10] = vlanInfoPtr.ipv6IpmBrgEn;
        inFields[11] = vlanInfoPtr.stgId;
        inFields[12] = 0; /*vlanInfoPtr.untaggedMruIdx */
        inFields[13] = GT_FALSE;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                              inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4],  inFields[5],
                              inFields[6],  inFields[7],  inFields[8],
                              inFields[9],  inFields[10], inFields[11],
                              inFields[12], inFields[13]);

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, result, "");
    }
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanEntryInvalidate  [DB]
*
* DESCRIPTION:
*       This function invalidates VLAN entry.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number.
*       vlanId     - VLAN id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanEntryInvalidate

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vlanId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanEntryInvalidate(devNum, vlanId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanEntryInvalidateAll

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    GT_U8    devNum;
    GT_U16   vlanId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    for(vlanId = 0; vlanId < 4096; vlanId++)
    {
        /* call cpss api function */
        result = cpssExMxPmBrgVlanEntryInvalidate(devNum, vlanId);
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanMemberSet    [DB+]
*
* DESCRIPTION:
*       Set specific member at  VLAN entry.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*       portNum         - port number
*       isMember        - GT_TRUE,  port would set as member
*                         GT_FALSE, port would set as not-member
*       isTagged        - GT_TRUE, to set the port as tagged
*                         GT_FALSE, to set the port as untagged
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanMemberSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vlanId;
    GT_U8    portNum;
    GT_BOOL  isMember;
    GT_BOOL  isTagged;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    isMember = (GT_BOOL)inArgs[3];
    isTagged = (GT_BOOL)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanMemberSet(devNum, vlanId, portNum,
                                        isMember, isTagged);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanCpuMemberSet [DB+]
*
* DESCRIPTION:
*       Add/Remove the CPU port from the VLAN port members.
*
* APPLICABLE DEVICES: ExMxPm devices only
*
* INPUTS:
*       devNum   - device number
*       vlanId   - vlan id
*       isMember - GT_TRUE  - CPU is a member of this VLAN.
*                  GT_FALSE - CPU isn't a member of this VLAN
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong input parameters
*       GT_NOT_SUPPORTED   - This request is not supportted.
*       GT_FAIL
*
* COMMENTS:
*       When CPU is a member, the CPU port would act as any other member port,
*       flooded packets would be flooded to it, but unicast packets that are
*       specifically sent to other member ports would not be sent to it.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanCpuMemberSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vlanId;
    GT_BOOL  isMember;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    isMember = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanCpuMemberSet(devNum, vlanId, isMember);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanPortAcceptFrameTypeSet   [DB+]
*
* DESCRIPTION:
*       Set port acceptable frame type.
*       There are three types of port configuration:
*        - Accept only Vlan tagged frames, deny untagged and priority tagged.
*        - Accept only untagged and priority tagged, deny tagged.
*        - Accept all frames.
*
* APPLICABLE DEVICES: ExMxPm devices only
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       frameType  -
*               CPSS_PORT_ACCEPT_FRAME_TAGGED_E    -
*                           All Untagged/Priority Tagged packets received on
*                           this port are discarded. Only Tagged accepted.
*               CPSS_PORT_ACCEPT_FRAME_ALL_E       -
*                           Both Tagged and Untagged packets are accepted
*                           on the port.
*               CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E  -
*                           All Tagged packets received on this port are
*                           discarded. Only Untagged/Priority Tagged accepted.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM- on wrong input parameters
*       GT_FAIL     - on error.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortAcceptFrameTypeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                               devNum;
    GT_U8                               portNum;
    CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     frameType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    frameType = (CPSS_PORT_ACCEPT_FRAME_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanPortAcceptFrameTypeSet(devNum, portNum, frameType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanPortAcceptFrameTypeGet   [DB+]
*
* DESCRIPTION:
*       Get port acceptable frame type.
*       There are three types of port configuration:
*        - Accept only Vlan tagged frames, deny untagged and priority tagged.
*        - Accept only untagged and priority tagged, deny tagged.
*        - Accept all frames.
*
* APPLICABLE DEVICES: ExMxPm devices only
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       frameTypePtr  - (pointer to)
*               CPSS_PORT_ACCEPT_FRAME_TAGGED_E    -
*                           All Untagged/Priority Tagged packets received on
*                           this port are discarded. Only Tagged accepted.
*               CPSS_PORT_ACCEPT_FRAME_ALL_E       -
*                           Both Tagged and Untagged packets are accepted
*                           on the port.
*               CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E  -
*                           All Tagged packets received on this port are
*                           discarded. Only Untagged/Priority Tagged accepted.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM- on wrong input parameters
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_FAIL     - on error.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortAcceptFrameTypeGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                               devNum;
    GT_U8                               portNum;
    CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     frameTypePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanPortAcceptFrameTypeGet(devNum, portNum, &frameTypePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", frameTypePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanPortPvidSet  [DB+]
*
* DESCRIPTION:
*       Set port's default VLAN Id.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       vlanId      - VLAN Id
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortPvidSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portNum;
    GT_U16   vlanId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    vlanId = (GT_U16)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanPortPvidSet(devNum, portNum, vlanId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanPortPvidGet  [DB+]
*
* DESCRIPTION:
*       Get port's default VLAN Id.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*
* OUTPUTS:
*       vidPtr   - (pointer to) default VLAN ID.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortPvidGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portNum;
    GT_U16   vidPtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanPortPvidGet(devNum, portNum, &vidPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vidPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanPortPvidPrecedenceSet    [DB+]
*
* DESCRIPTION:
*       Set Port VID Precedence.
*       Relevant for packets with assigned VLAN ID of Pvid.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       precedence  - precedence type
*                     soft: the PVID may be overriden by subsequent VLAN
*                           assignment mechanisms
*                     hard: the PVID cannot be overriden by subsequent VLAN
*                           assignment mechanisms
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM- on wrong input parameters
*       GT_HW_ERROR - on hardware error
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortPvidPrecedenceSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                        devNum;
    GT_U8                                        portNum;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  precedence;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    precedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanPortPvidPrecedenceSet(devNum, portNum, precedence);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanPortPvidPrecedenceGet    [DB+]
*
* DESCRIPTION:
*       Get the Port VID Precedence.
*       Relevant for packets with assigned VLAN ID of Pvid.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       precedencePtr  - (pointer to)precedence type - soft or hard
*                       soft: the PVID may be overriden by subsequent VLAN
*                             assignment mechanisms
*                       hard: the PVID cannot be overriden by subsequent VLAN
*                             assignment mechanisms
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong input parameters
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortPvidPrecedenceGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8                                        portNum;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  precedencePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanPortPvidPrecedenceGet(devNum, portNum,
                                                     &precedencePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", precedencePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanPortForcePvidEnableSet   [DB+]
*
* DESCRIPTION:
*       Set Port VID Assignment mode.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       enable   - GT_TRUE  - PVID is assigned to all packets.
*                  GT_FALSE - PVID is assigned to untagged or priority tagged
*                             packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM- on wrong input parameters
*       GT_HW_ERROR - on hardware error
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortForcePvidEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portNum;
    GT_BOOL  enable;


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
    result = cpssExMxPmBrgVlanPortForcePvidEnableSet(devNum, portNum,
                                                      enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanPortForcePvidEnableGet   [DB+]
*
* DESCRIPTION:
*       Get the Port VID Assignment mode.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - PVID is assigned to all packets.
*                    GT_FALSE - PVID is assigned to untagged or priority tagged
*                             packets
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong input parameters
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortForcePvidEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanPortForcePvidEnableGet(devNum, portNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/* cpssExMxPmBrgVlanProtocolEntry Table [DB+] */
static GT_U32   entryCnt;


/*******************************************************************************
* cpssExMxPmBrgVlanProtocolEntrySet
*
* DESCRIPTION:
*       Write etherType and encapsulation of Protocol based classification
*       for specific device.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       index       - entry number (0..11)
*       etherType   - Ether Type or DSAP/SSAP
*       encListPtr  - encapsulation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanProtocolEntrySet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    GT_U32                         entryNum;
    GT_U16                         etherType;
    CPSS_PROT_CLASS_ENCAP_STC      encListPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    entryNum = (GT_U32)inFields[0];
    etherType = (GT_U16)inFields[1];

    encListPtr.ethV2 = (GT_BOOL)inFields[2];
    encListPtr.nonLlcSnap = (GT_BOOL)inFields[3];
    encListPtr.llcSnap = (GT_BOOL)inFields[4];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanProtocolEntrySet(devNum, entryNum, etherType,
                                                          &encListPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanProtocolEntryGet
*
* DESCRIPTION:
*       Read etherType and encapsulation of Protocol based classification
*       for specific device.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*       index        - entry number (0..11)
*
* OUTPUTS:
*       etherTypePtr    - (pointer to) Ether Type or DSAP/SSAP
*       encListPtr      - (pointer to) encapsulation.
*       validEntryPtr   - (pointer to) validity bit
*                                   GT_TRUE - the entry is valid
*                                   GT_FALSE - the entry is invalid
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanProtocolEntryGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                      result;

    GT_U8                          devNum;
    GT_U16                         etherTypePtr;
    CPSS_PROT_CLASS_ENCAP_STC      encListPtr;
    GT_BOOL                        validEntryPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryCnt = 1;

    /* call cpss api function */
    result = cpssExMxPmBrgVlanProtocolEntryGet(devNum, entryCnt, &etherTypePtr,
                                              &encListPtr, &validEntryPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(validEntryPtr)
    {
        inFields[0] = entryCnt;
        inFields[1] = etherTypePtr;
        inFields[2] = encListPtr.ethV2;
        inFields[3] = encListPtr.nonLlcSnap;
        inFields[4] = encListPtr.llcSnap;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d", inFields[0], inFields[1],
                     inFields[2], inFields[3], inFields[4]);

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
       galtisOutput(outArgs, result, "");
    }
    return CMD_OK;
}


/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanProtocolEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    GT_U16                         etherTypePtr;
    CPSS_PROT_CLASS_ENCAP_STC      encListPtr;
    GT_BOOL                        validEntryPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryCnt++;

    if(entryCnt >= CPSS_EXMXPM_PORT_PROT_MAX_ENTRY_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxPmBrgVlanProtocolEntryGet(devNum, entryCnt, &etherTypePtr,
                                              &encListPtr, &validEntryPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(validEntryPtr)
    {
        inFields[0] = entryCnt;
        inFields[1] = etherTypePtr;
        inFields[2] = encListPtr.ethV2;
        inFields[3] = encListPtr.nonLlcSnap;
        inFields[4] = encListPtr.llcSnap;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d", inFields[0], inFields[1],
                     inFields[2], inFields[3], inFields[4]);

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
       galtisOutput(outArgs, result, "");
    }
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanProtocolEntryInvalidate
*
* DESCRIPTION:
*       Invalidates entry of Protocol based classification for specific device.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*       index        - entry number (0..11)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanProtocolEntryInvalidate

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U32   index;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    
    index = (GT_U32)inFields[0];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanProtocolEntryInvalidate(devNum, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanPortProtocolVidEnableSet [DB+]
*
* DESCRIPTION:
*       Enable/Disable Port Protocol VLAN assignment to packets received
*       on this port,according to it's Layer2 protocol
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_TRUE - Enable Port protocol VID assignment.
*                 GT_FALSE - Disable Port protocol VID assignment
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortProtocolVidEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portNum;
    GT_BOOL  enable;


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
    result = cpssExMxPmBrgVlanPortProtocolVidEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanPortProtocolVidEnableGet [DB+]
*
* DESCRIPTION:
*       Get the current state of Enable/Disable Port Protocol VLAN assignment
*       to packets received on this port,according to it's Layer2 protocol
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE - Port protocol VID assignment is enabled.
*                    GT_FALSE- Port protocol VID assignment is disabled.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or port
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortProtocolVidEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanPortProtocolVidEnableGet(devNum, portNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanPortProtocolQosEnableSet [DB+]
*
* DESCRIPTION:
*       Enable/Disable Port Protocol QoS Assignment to packets received on this
*       port, according to it's Layer2 protocol
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_TRUE - Enable Port protocol QOS assignment.
*                 GT_FALSE - Disable Port protocol QOS assignment
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or port
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortProtocolQosEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portNum;
    GT_BOOL  enable;


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
    result = cpssExMxPmBrgVlanPortProtocolQosEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanPortProtocolQosEnableGet [DB+]
*
* DESCRIPTION:
*       Get the current state of Enable/Disable Port Protocol QoS assignment
*       to packets received on this port,according to it's Layer2 protocol
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                   GT_TRUE - Port protocol QOS assignment is enabled.
*                   GT_FALSE - Port protocol QOS assignment is disabled
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or port
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortProtocolQosEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanPortProtocolQosEnableGet(devNum, portNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/* cpssExMxPmBrgVlanPortProtocolVidQosEntry Table   [DB+]*/
static GT_U32   entryNumCnt;

/*******************************************************************************
* cpssExMxPmBrgVlanPortProtocolVidQosEntrySet
*
* DESCRIPTION:
*       Write Port Protocol VID and QOS Assignment per port
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*       index               - Entry number (0..11)
*       vlanCfgPtr          - pointer to VID parameters
*       qosCfgPtr           - pointer to QoS parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       It is possible to update only VLAN or QOS parameters, when one
*       of those input pointers is NULL.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortProtocolVidQosEntrySet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                  result;

    GT_U8                                      devNum;
    GT_U8                                      port;
    GT_U32                                     entryNum;
    CPSS_EXMXPM_BRG_VLAN_PROT_CLASS_CFG_STC    vlanCfgPtr;
    CPSS_EXMXPM_QOS_ENTRY_STC                  qosCfgPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    entryNum = (GT_U32)inFields[0];

    vlanCfgPtr.vlanId = (GT_U16)inFields[1];
    vlanCfgPtr.vlanIdAssignCmd =
                              (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[2];
    vlanCfgPtr.vlanIdAssignPrecedence =
                       (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[3];


    qosCfgPtr.qosParamsModify.modifyTc = (GT_BOOL)inFields[4];
    qosCfgPtr.qosParamsModify.modifyUp = (GT_BOOL)inFields[5];
    qosCfgPtr.qosParamsModify.modifyDp = (GT_BOOL)inFields[6];
    qosCfgPtr.qosParamsModify.modifyDscp = (GT_BOOL)inFields[7];
    qosCfgPtr.qosParamsModify.modifyExp = (GT_BOOL)inFields[8];

    qosCfgPtr.qosParams.tc = (GT_U32)inFields[9];
    qosCfgPtr.qosParams.dp = (CPSS_DP_LEVEL_ENT)inFields[10];
    qosCfgPtr.qosParams.up = (GT_U32)inFields[11];
    qosCfgPtr.qosParams.dscp = (GT_U32)inFields[12];
    qosCfgPtr.qosParams.exp = (GT_U32)inFields[13];

    qosCfgPtr.qosPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[14];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(devNum, port, entryNum,
                                               &vlanCfgPtr, &qosCfgPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanPortProtocolVidQosEntryGet
*
* DESCRIPTION:
*       Get Port Protocol VID and QOS Assignment per port
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       portNum             - port number
*       index               - Entry number (0..11)
*
* OUTPUTS:
*       vlanCfgPtr          - (pointer to) VID parameters
*       qosCfgPtr           - (pointer to) QoS parameters
*       validEntryPtr       - (pointer to) validity bit
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*      It is possible to get only one of VLAN or QOS parameters by receiving
*      one of those pointers as NULL pointer.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortProtocolVidQosEntryGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                result;

    GT_U8                                    devNum;
    GT_U8                                    port;
    CPSS_EXMXPM_BRG_VLAN_PROT_CLASS_CFG_STC  vlanCfgPtr;
    CPSS_EXMXPM_QOS_ENTRY_STC                qosCfgPtr;
    GT_BOOL                                  validEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    entryNumCnt = 0;

    validEntry = GT_FALSE;

    while(!validEntry && entryNumCnt < 12)
    {
        /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
        result = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(devNum, port, entryNumCnt,
                                      &vlanCfgPtr, &qosCfgPtr, &validEntry);
        entryNumCnt++;
    }

    if (entryNumCnt == 12)
    {
        galtisOutput(outArgs, GT_EMPTY, "%d", -1);
        return CMD_OK;
    }

    entryNumCnt--;
    inFields[0] = entryNumCnt;
    inFields[1] = vlanCfgPtr.vlanId;
    inFields[2] = vlanCfgPtr.vlanIdAssignCmd;
    inFields[3] = vlanCfgPtr.vlanIdAssignPrecedence;

    inFields[4] = qosCfgPtr.qosParamsModify.modifyTc;
    inFields[5] = qosCfgPtr.qosParamsModify.modifyUp;
    inFields[6] = qosCfgPtr.qosParamsModify.modifyDp;
    inFields[7] = qosCfgPtr.qosParamsModify.modifyDscp;
    inFields[8] = qosCfgPtr.qosParamsModify.modifyExp;

    inFields[9] = qosCfgPtr.qosParams.tc;
    inFields[10] = qosCfgPtr.qosParams.dp;
    inFields[11] = qosCfgPtr.qosParams.up;
    inFields[12] = qosCfgPtr.qosParams.dscp;
    inFields[13] = qosCfgPtr.qosParams.exp;

    inFields[14] = qosCfgPtr.qosPrecedence;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                 inFields[0], inFields[1], inFields[2],
                 inFields[3], inFields[4], inFields[5],
                 inFields[6], inFields[7], inFields[8],
                 inFields[9], inFields[10], inFields[11],
                 inFields[12], inFields[13], inFields[14]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}


/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortProtocolVidQosEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                result;

    GT_U8                                    devNum;
    GT_U8                                    port;
    CPSS_EXMXPM_BRG_VLAN_PROT_CLASS_CFG_STC  vlanCfgPtr;
    CPSS_EXMXPM_QOS_ENTRY_STC                qosCfgPtr;
    GT_BOOL                                  validEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    if(entryNumCnt > CPSS_EXMXPM_VLAN_MAX_ENTRY_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    validEntry = GT_FALSE;

    while(!validEntry && entryNumCnt < 12)
    {
        entryNumCnt++;

        /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
        result = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(devNum, port, entryNumCnt,
                                      &vlanCfgPtr, &qosCfgPtr, &validEntry);
    }

    if(entryNumCnt == 12)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = entryNumCnt;
    inFields[1] = vlanCfgPtr.vlanId;
    inFields[2] = vlanCfgPtr.vlanIdAssignCmd;
    inFields[3] = vlanCfgPtr.vlanIdAssignPrecedence;
    
    inFields[4] = qosCfgPtr.qosParamsModify.modifyTc;
    inFields[5] = qosCfgPtr.qosParamsModify.modifyUp;
    inFields[6] = qosCfgPtr.qosParamsModify.modifyDp;
    inFields[7] = qosCfgPtr.qosParamsModify.modifyDscp;
    inFields[8] = qosCfgPtr.qosParamsModify.modifyExp;

    inFields[9] = qosCfgPtr.qosParams.tc;
    inFields[10] = qosCfgPtr.qosParams.dp;
    inFields[11] = qosCfgPtr.qosParams.up;
    inFields[12] = qosCfgPtr.qosParams.dscp;
    inFields[13] = qosCfgPtr.qosParams.exp;

    inFields[14] = qosCfgPtr.qosPrecedence;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                 inFields[0], inFields[1], inFields[2],
                 inFields[3], inFields[4], inFields[5],
                 inFields[6], inFields[7], inFields[8],
                 inFields[9], inFields[10], inFields[11],
                 inFields[12], inFields[13], inFields[14]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate
*
* DESCRIPTION:
*       Invalidate Port Protocol VID and QOS Assignment per port
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       index       - entry number (0...11)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM- on wrong input parameters
*       GT_HW_ERROR - on hardware error
*       GT_FAIL     - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portNum;
    GT_U32   index;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    index = (GT_U32)inFields[0];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate(devNum, portNum, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanPortIngressFilterEnableSet   [DB+]
*
* DESCRIPTION:
*       Enable/disable Ingress Filtering for specific port
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*       enable    - GT_TRUE, enable ingress filtering
*                   GT_FALSE, disable ingress filtering
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortIngressFilterEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portNum;
    GT_BOOL  enable;


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
    result = cpssExMxPmBrgVlanPortIngressFilterEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanPortIngressFilterEnableGet   [DB+]
*
* DESCRIPTION:
*       Get the current state of Ingress Filtering for specific port
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*
* OUTPUTS:
*       enablePtr    - (pointer to)
*                   GT_TRUE, enable ingress filtering
*                   GT_FALSE, disable ingress filtering
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortIngressFilterEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanPortIngressFilterEnableGet(devNum, portNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanInvalidFilterEnableSet   [DB+]
*
* DESCRIPTION:
*       Enable/Disable drop for packets with an invalid VLAN.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum    - device number
*       enable      GT_TRUE, enable invalid  filtering
*                   GT_FALSE, disable invalid filtering
*
* OUTPUTS:
*       None;
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanInvalidFilterEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanInvalidFilterEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanInvalidFilterEnableGet   [DB+]
*
* DESCRIPTION:
*       Get the current setting of invalid VLAN Filtering
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                   GT_TRUE, enable invalid  filtering
*                   GT_FALSE, disable invalid filtering
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanInvalidFilterEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enablePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanInvalidFilterEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanIngressEtherTypeSet  [DB+]
*
* DESCRIPTION:
*       Sets Ingress VLAN Ether Type and Range
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       etherTypeMode - mode of tagged vlan etherType. May be either
*                       CPSS_VLAN_ETHERTYPE0_E or CPSS_VLAN_ETHERTYPE1_E.
*                       See comment.
*       ethertype     - the EtherType to be recognized tagged packets. See
*                       comment.
*       maxVidRange   - Indicates the maximum VID that may be assigned to
*                       a packet received on a port with <Ingress VLANSel> set
*                       to 1.
*                       If the VID assigned to the packet is greater than this
*                       value, a security breach occurs and the packet is
*                       assigned with a hard drop or a soft drop command,
*                       according to the setting of <VLANRange DropMode>
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       If the vid of the incoming packet is greater or equal than the vlan
*       range value, the packet is filtered.
*
*       Configuration of vid and range for CPSS_VLAN_ETHERTYPE1_E should be done
*       only when nested vlan is used. User can alter the etherType field of
*       the encapsulating vlan by setting a number other then 0x8100 in the
*       parameter <ethertype>.
*
*       For most applications Ingress & Egress Ethertypes should be the same.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanIngressEtherTypeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                   devNum;
    CPSS_ETHER_MODE_ENT     etherTypeMode;
    GT_U16                  etherType;
    GT_U16                  maxVidRange;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    etherTypeMode = (CPSS_ETHER_MODE_ENT)inArgs[1];
    etherType = (GT_U16)inArgs[2];
    maxVidRange = (GT_U16)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanIngressEtherTypeSet(devNum, etherTypeMode,
                                                   etherType, maxVidRange);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanIngressEtherTypeGet  [DB+]
*
* DESCRIPTION:
*       Gets Ingress VLAN Ether Type and Range
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       etherTypeMode - mode of tagged vlan etherType. May be either
*                       CPSS_VLAN_ETHERTYPE0_E or CPSS_VLAN_ETHERTYPE1_E.
*                       See comment.
*
* OUTPUTS:
*       ethertypePtr  - the EtherType to be recognized tagged packets. See
*                          comment.
*       maxVidRangePtr  - (pointer to)the maximum VID that may be assigned to
*                       a packet received on a port with <Ingress VLANSel> set
*                       to 1.
*                       If the VID assigned to the packet is greater than this
*                       value, a security breach occurs and the packet is
*                       assigned with a hard drop or a soft drop command,
*                       according to the setting of <VLANRange DropMode>
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       If the vid of the incoming packet is greater or equal than the vlan
*       range value, the packet is filtered.
*
*       Configuration of vid and range for CPSS_VLAN_ETHERTYPE1_E should be done
*       only when nested vlan is used. User can alter the etherType field of
*       the encapsulating vlan by setting a number other then 0x8100 in the
*       parameter <ethertype>.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanIngressEtherTypeGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                   devNum;
    CPSS_ETHER_MODE_ENT     etherTypeMode;
    GT_U16                  etherTypePtr;
    GT_U16                  maxVidRangePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    etherTypeMode = (CPSS_ETHER_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanIngressEtherTypeGet(devNum, etherTypeMode,
                                                   &etherTypePtr, &maxVidRangePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", etherTypePtr, maxVidRangePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanEgressEtherTypeSet   [DB+]
*
* DESCRIPTION:
*       Sets egress VLAN Ether Type
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       etherTypeMode - Mode of tagged vlan etherType.
*                       May be either:CPSS_VLAN_ETHERTYPE0_E or
*                       CPSS_VLAN_ETHERTYPE1_E By default only value 0 would
*                       be used, unless nested VLANs are used.
*                       See comment.
*       ethertype     - the EtherType to be recognized tagged packets. See
*                       comment.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*       Configuration of vid and range for CPSS_VLAN_ETHERTYPE1_E should be done
*       only when nested vlan is used. User can alter the etherType field of
*       the encapsulating vlan by setting a number other then 0x8100 in the
*       parameter <ethertype>.
*
*       Applications should be aware that in most cases it is reasonable to
*       set the same EtherTypes for both Ingress & Egress directions.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanEgressEtherTypeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                   devNum;
    CPSS_ETHER_MODE_ENT     etherTypeMode;
    GT_U16                  etherType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    etherTypeMode = (CPSS_ETHER_MODE_ENT)inArgs[1];
    etherType = (GT_U16)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanEgressEtherTypeSet(devNum, etherTypeMode, etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanEgressEtherTypeGet   [DB+]
*
* DESCRIPTION:
*       Gets Ingress VLAN Ether Type
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       etherTypeMode - Mode of tagged vlan etherType.
*                       May be either:CPSS_VLAN_ETHERTYPE0_E or
*                       CPSS_VLAN_ETHERTYPE1_E By default only value 0 would
*                       be used, unless nested VLANs are used.
*                       See comment.
* OUTPUTS:
*       ethertypePtr     - the EtherType to be recognized tagged packets. See
*                          comment.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       Configuration of vid and range for CPSS_VLAN_ETHERTYPE1_E should be done
*       only when nested vlan is used. User can alter the etherType field of
*       the encapsulating vlan by setting a number other then 0x8100 in the
*       parameter <ethertype>.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanEgressEtherTypeGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                   devNum;
    CPSS_ETHER_MODE_ENT     etherTypeMode;
    GT_U16                  etherTypePtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    etherTypeMode = (CPSS_ETHER_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanEgressEtherTypeGet(devNum, etherTypeMode, &etherTypePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", etherTypePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanStgIdBind    [DB+]
*
* DESCRIPTION:
*       Bind VLAN to STP Id.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device Id
*       vlanId  - vlan Id
*       stgId   - STG Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM        - on wrong input parameters
*       GT_FAIL             - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanStgIdBind

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vlanId;
    GT_U16   stgId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    stgId = (GT_U16)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanStgIdBind(devNum, vlanId, stgId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanIpmBridgingEnableSet [DB+]
*
* DESCRIPTION:
*       IPv4/Ipv6 Multicast Bridging Enable
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*       devNum     - device id
*       vlanId  - Vlan Id
*       ipVer   - IPv4 or Ipv6
*       enable  - GT_TRUE - IPv4 multicast packets are bridged
*                   according to ipv4IpmBrgMode
*                 GT_FALSE - IPv4 multicast packets are bridged
*                   according to MAC DA
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong input parameters
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanIpmBridgingEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    GT_U16                      vlanId;
    CPSS_IP_PROTOCOL_STACK_ENT  ipVer;
    GT_BOOL                     enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    ipVer = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];


    /* call cpss api function */
    result = cpssExMxPmBrgVlanIpmBridgingEnableSet(devNum, vlanId, ipVer, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgVlanIpmBridgingModeSet   [DB+]
*
* DESCRIPTION:
*       Sets the IPM bridging mode of Vlan. Relevant when IPM Bridging enabled.
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*       devNum     - device id
*       vlanId  - Vlan Id
*       ipVer   - IPv4 or Ipv6
*       ipmMode - IPM bridging mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong input parameters
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanIpmBridgingModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    GT_U16                      vlanId;
    CPSS_IP_PROTOCOL_STACK_ENT  ipVer;
    CPSS_BRG_IPM_MODE_ENT       ipmMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    ipVer = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    ipmMode = (CPSS_BRG_IPM_MODE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanIpmBridgingModeSet(devNum, vlanId, ipVer, ipmMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet   [DB+]
*
* DESCRIPTION:
*     Set VLAN Maximum Receive Unit (MRU) profile index for a Port.
*     The packet length does not include the tag (tags).
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*
* APPLICABLE DEVICES: ExMxPm devices only
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*       index    - MRU profile index [0..7]
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong input parameters
*       GT_FAIL            - otherwise
*
* COMMENTS:
*       Note that packet exceeding either VLAN MRU or port VLAN MRU would
*       be dropped with HARD command
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portNum;
    GT_U32   index;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    index = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet(devNum, portNum, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet   [DB]    !!!recheck - done
*
* DESCRIPTION:
*     Get VLAN Maximum Receive Unit (MRU) profile index for a Port.
*     The packet length does not include the tag (tags).
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*
* APPLICABLE DEVICES: ExMxPm devices only
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number
*
* OUTPUTS:
*       indexPtr - (pointer to) MRU profile index [0..7]
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong input parameters
*       GT_BAD_PTR         - one of the parameters is NULL pointer
*       GT_FAIL            - otherwise
*
* COMMENTS:
*       Note that packet exceeding either VLAN MRU or port VLAN MRU would
*       be dropped with HARD command
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portNum;
    GT_U32   indexPtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet(devNum, portNum, &indexPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", indexPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanUntaggedMruProfileValueSet   [DB+]
*
* DESCRIPTION:
*     Set VLAN MRU (Maximum Receive Unit) profile's value for VLANs,
*     used by VLAN MRU and port VLAN MRU (the value itself is matched against
*     packet size, after striping any tag the packet may have).
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*
* APPLICABLE DEVICES: ExMxPm devices only
*
* INPUTS:
*       devNum- device number
*       index - MRU profile index [0..7]
*       mru   - MRU value in bytes [0..0xFFFF]
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong input parameters
*       GT_FAIL            - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanUntaggedMruProfileValueSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U32   index;
    GT_U32   mru;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    mru = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanUntaggedMruProfileValueSet(devNum, index, mru);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgVlanUntaggedMruProfileValueGet   [DB+]
*
* DESCRIPTION:
*     Get VLAN MRU (Maximum Receive Unit) profile's value for VLANs,
*     used by VLAN MRU and port VLAN MRU (the value itself is matched
*     against packet size, after striping any tag the packet may have).
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*
* APPLICABLE DEVICES: ExMxPm devices only
*
* INPUTS:
*       devNum   - device number
*       index    - MRU profile index
*
* OUTPUTS:
*       mruPtr - (pointer to) MRU value in bytes [0..0xFFFF]
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - on wrong input parameters
*       GT_BAD_PTR         - one of the parameters is NULL pointer
*       GT_FAIL            - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanUntaggedMruProfileValueGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U32   index;
    GT_U32   mruPtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    mruPtr = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanUntaggedMruProfileValueGet(devNum, index, &mruPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mruPtr);
    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmBrgVlanMirrorToTxAnalyzerEnableSet
*
* DESCRIPTION:
*       Enable/Disable egress mirroring on this Vlan.
*       When enabled, egress traffic on this Vlan is mirrored 
*       to the Tx Analyzer port.
*
* APPLICABLE DEVICES: All ExMxPm devices
*
* INPUTS:
*       devNum      - Device id
*       vlanId      - Vlan Id
*       enable      - GT_TRUE, enable egress mirroring
*                     GT_FALSE, disable egress mirroring
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanMirrorToTxAnalyzerEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16 vlanId;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanMirrorToTxAnalyzerEnableSet(devNum, vlanId, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmBrgVlanEgressUnregMcFilterCmdSet
*
* DESCRIPTION:
*       Set the Egress command for unregistered multicast packets on this Vlan.       
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                 - device number
*       vlanId                 - Vlan Id
*       egressUnregMcFilterCmd - The action applied on
*                                unregistered multicast packets.
*
* OUTPUTS:
*       None;
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanEgressUnregMcFilterCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16 vlanId;
    CPSS_UNREG_MC_EGR_FILTER_CMD_ENT egressUnregMcFilterCmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    egressUnregMcFilterCmd = (CPSS_UNREG_MC_EGR_FILTER_CMD_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanEgressUnregMcFilterCmdSet(devNum, vlanId, egressUnregMcFilterCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet
*
* DESCRIPTION:
*       Get the Egress command for unregistered multicast packets on this Vlan.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                 - device number
*       vlanId                 - Vlan Id
*
* OUTPUTS:
*       egressUnregMcFilterCmdPtr - (pointer to) The action applied on
*                                     unregistered multicast packets.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgVlanEgressUnregMcFilterCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16 vlanId;
    CPSS_UNREG_MC_EGR_FILTER_CMD_ENT egressUnregMcFilterCmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet(devNum, vlanId, &egressUnregMcFilterCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", egressUnregMcFilterCmd);

    

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    /* cpssExMxPmBrgVlanEntry */

     /*puma 2*/
    {"cpssExMxPm2BrgVlanEntrySet",
        &wrCpssExMxPm2BrgVlanEntryWrite,
        1, 14},

    {"cpssExMxPm2BrgVlanEntryGetFirst",
        &wrCpssExMxPm2BrgVlanEntryReadFirst,
        1, 0},

    {"cpssExMxPm2BrgVlanEntryGetNext",
        &wrCpssExMxPm2BrgVlanEntryReadNext,
        1, 0},

    /*puma 1*/

    {"cpssExMxPmBrgVlanEntrySet",
        &wrCpssExMxPmBrgVlanEntryWrite,
        1, 13},

    {"cpssExMxPmBrgVlanEntryGetFirst",
        &wrCpssExMxPmBrgVlanEntryReadFirst,
        1, 0},

    {"cpssExMxPmBrgVlanEntryGetNext",
        &wrCpssExMxPmBrgVlanEntryReadNext,
        1, 0},

    /* --- cpssExMxPmBrgVlanEntry */


    {"cpssExMxPmBrgVlanEntryInvalidate",
        &wrCpssExMxPmBrgVlanEntryInvalidate,
        2, 0},

    {"cpssExMxPmBrgVlanEntryInvalidateAll",
        &wrCpssExMxPmBrgVlanEntryInvalidateAll,
        1, 0},

    {"cpssExMxPmBrgVlanMemberSet",
        &wrCpssExMxPmBrgVlanMemberSet,
        5, 0},

    {"cpssExMxPmBrgVlanCpuMemberSet",
        &wrCpssExMxPmBrgVlanCpuMemberSet,
        3, 0},

    {"cpssExMxPmBrgVlanPortAcceptFrameTypeSet",
        &wrCpssExMxPmBrgVlanPortAcceptFrameTypeSet,
        3, 0},

    {"cpssExMxPmBrgVlanPortAcceptFrameTypeGet",
        &wrCpssExMxPmBrgVlanPortAcceptFrameTypeGet,
        2, 0},

    {"cpssExMxPmBrgVlanPortPvidSet",
        &wrCpssExMxPmBrgVlanPortPvidSet,
        3, 0},

    {"cpssExMxPmBrgVlanPortPvidGet",
        &wrCpssExMxPmBrgVlanPortPvidGet,
        2, 0},

    {"cpssExMxPmBrgVlanPortPvidPrecedenceSet",
        &wrCpssExMxPmBrgVlanPortPvidPrecedenceSet,
        3, 0},

    {"cpssExMxPmBrgVlanPortPvidPrecedenceGet",
        &wrCpssExMxPmBrgVlanPortPvidPrecedenceGet,
        2, 0},

    {"cpssExMxPmBrgVlanPortForcePvidEnableSet",
        &wrCpssExMxPmBrgVlanPortForcePvidEnableSet,
        3, 0},

    {"cpssExMxPmBrgVlanPortForcePvidEnableGet",
        &wrCpssExMxPmBrgVlanPortForcePvidEnableGet,
        2, 0},
    
    /*cpssExMxPmBrgVlanProtocolEntry Table*/


    {"cpssExMxPmBrgVlanProtocolEntrySet",            /*cpssDxChBrgVlanProtoClassSet*/
        &wrCpssExMxPmBrgVlanProtocolEntrySet,
        1, 5},

    {"cpssExMxPmBrgVlanProtocolEntryGetFirst",       /*cpssDxChBrgVlanProtoClassGetFirst*/
        &wrCpssExMxPmBrgVlanProtocolEntryGetFirst,
        1, 0},

    {"cpssExMxPmBrgVlanProtocolEntryGetNext",        /*cpssDxChBrgVlanProtoClassGetNext*/
        &wrCpssExMxPmBrgVlanProtocolEntryGetNext,    
        1, 0},


    {"cpssExMxPmBrgVlanProtocolEntryDelete",
        &wrCpssExMxPmBrgVlanProtocolEntryInvalidate,
        1, 1},
    
    /*cpssExMxPmBrgVlanProtocolEntry Table*/

    {"cpssExMxPmBrgVlanPortProtocolVidEnableSet",
        &wrCpssExMxPmBrgVlanPortProtocolVidEnableSet,
        3, 0},

    {"cpssExMxPmBrgVlanPortProtocolVidEnableGet",
        &wrCpssExMxPmBrgVlanPortProtocolVidEnableGet,
        2, 0},

    {"cpssExMxPmBrgVlanPortProtocolQosEnableSet",
        &wrCpssExMxPmBrgVlanPortProtocolQosEnableSet,
        3, 0},

    {"cpssExMxPmBrgVlanPortProtocolQosEnableGet",
        &wrCpssExMxPmBrgVlanPortProtocolQosEnableGet,
        2, 0},

    /* cpssExMxPmBrgVlanPortProtocolVidQosEntry */

    {"cpssExMxPmBrgVlanPortProtocolVidQosEntrySet",          /*cpssDxChBrgVlanPortProtoVlanQosSet*/
        &wrCpssExMxPmBrgVlanPortProtocolVidQosEntrySet,
        2, 15},

    {"cpssExMxPmBrgVlanPortProtocolVidQosEntryGetFirst",     /*cpssDxChBrgVlanPortProtoVlanQosGetFirst*/
        &wrCpssExMxPmBrgVlanPortProtocolVidQosEntryGetFirst,
        2, 0},

    {"cpssExMxPmBrgVlanPortProtocolVidQosEntryGetNext",      /*cpssDxChBrgVlanPortProtoVlanQosGetNext*/
        &wrCpssExMxPmBrgVlanPortProtocolVidQosEntryGetNext,
        2, 0},

    {"cpssExMxPmBrgVlanPortProtocolVidQosEntryDelete",
        &wrCpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate,
        2, 1},

    /* cpssExMxPmBrgVlanPortProtocolVidQosEntry */


    {"cpssExMxPmBrgVlanPortIngressFilterEnableSet",
        &wrCpssExMxPmBrgVlanPortIngressFilterEnableSet,
        3, 0},

    {"cpssExMxPmBrgVlanPortIngressFilterEnableGet",
        &wrCpssExMxPmBrgVlanPortIngressFilterEnableGet,
        2, 0},

    {"cpssExMxPmBrgVlanInvalidFilterEnableSet",
        &wrCpssExMxPmBrgVlanInvalidFilterEnableSet,
        2, 0},

    {"cpssExMxPmBrgVlanInvalidFilterEnableGet",
        &wrCpssExMxPmBrgVlanInvalidFilterEnableGet,
        1, 0},

    {"cpssExMxPmBrgVlanIngressEtherTypeSet",
        &wrCpssExMxPmBrgVlanIngressEtherTypeSet,
        4, 0},

    {"cpssExMxPmBrgVlanIngressEtherTypeGet",
        &wrCpssExMxPmBrgVlanIngressEtherTypeGet,
        2, 0},

    {"cpssExMxPmBrgVlanEgressEtherTypeSet",
        &wrCpssExMxPmBrgVlanEgressEtherTypeSet,
        3, 0},

    {"cpssExMxPmBrgVlanEgressEtherTypeGet",
        &wrCpssExMxPmBrgVlanEgressEtherTypeGet,
        2, 0},

    {"cpssExMxPmBrgVlanStgIdBind",
        &wrCpssExMxPmBrgVlanStgIdBind,
        3, 0},

    {"cpssExMxPmBrgVlanIpmBridgingEnableSet",
        &wrCpssExMxPmBrgVlanIpmBridgingEnableSet,
        4, 0},

    {"cpssExMxPmBrgVlanIpmBridgingModeSet",
        &wrCpssExMxPmBrgVlanIpmBridgingModeSet,
        4, 0},

    {"cpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet",
        &wrCpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet,
        3, 0},

    {"cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet",
        &wrCpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet,
        2, 0},

    {"cpssExMxPmBrgVlanUntaggedMruProfileValueSet",
        &wrCpssExMxPmBrgVlanUntaggedMruProfileValueSet,
        3, 0},

    {"cpssExMxPmBrgVlanUntaggedMruProfileValueGet",
        &wrCpssExMxPmBrgVlanUntaggedMruProfileValueGet,
        2, 0},

    {"cpssExMxPmBrgVlanMirrorToRxAnalyzerEnableSet",
         &wrCpssExMxPmBrgVlanMirrorToTxAnalyzerEnableSet,
         3, 0},

    {"cpssExMxPm2BrgVlanEgressUnregMcFilterCmdSet",
         &wrCpssExMxPmBrgVlanEgressUnregMcFilterCmdSet,
         3, 0},

    {"cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet",
         &wrCpssExMxPmBrgVlanEgressUnregMcFilterCmdGet,
         2, 0},


};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmBrgVlan
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
GT_STATUS cmdLibInitCpssExMxPmBrgVlan
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}



