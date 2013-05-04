/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmBrgSrcId.c
*
* DESCRIPTION:
*       Wrapper functions for cpssExMxPmBrgSrcId cpss.exMxPm functions
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
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgSrcId.h>


/* ExMxPm max number of entries to be read from tables\classes */
#define  CPSS_EXMXPM_SOURCE_ID_MAX_ENTRY_CNS 32


/*******************************************************************************
* cpssExMxPmBrgSrcIdGroupPortAdd    [DB]
*
* DESCRIPTION:
*         Add a port to the Source ID group. Packets assigned with this Source ID
*         may be forwarded to this port.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID  number that the port is added to.
*       portNum  - port number (may be CPU port).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, portNum, sourceId
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSrcIdGroupPortAdd

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              sourceId;
    GT_U8               portNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    sourceId = (GT_U32)inArgs[1];
    portNum = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgSrcIdGroupPortAdd(devNum, sourceId, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSrcIdGroupPortDelete [DB]
*
* DESCRIPTION:
*         Delete a port from Source ID group. Packets assigned with this
*         Source ID will not be forwarded to this port.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum         - device number
*       sourceId       - Source ID  number that the port is added to.
*       portNum        - port number (may be  CPU port).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, portNum, sourceId
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSrcIdGroupPortDelete

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              sourceId;
    GT_U8               portNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    sourceId = (GT_U32)inArgs[1];
    portNum = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgSrcIdGroupPortDelete(devNum, sourceId, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/* cpssExMxPmBrgSrcIdGroupEntry     Table   [DB] */
/* same as cpssDxChBrgSrcIdGroupEntry Table*/
static GT_U32   sourceIdCnt;


/*
* cpssExMxPmBrgSrcIdGroupEntrySet
*
* DESCRIPTION:
*        Set entry in Source ID Egress Filtering table.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum         - device number
*       sourceId       - Source ID  number.
*       cpuSrcIdMember - GT_TRUE - CPU is member of of Src ID group.
*                        GT_FALSE - CPU isn't member of of Src ID group.
*       portBitmapPtr  - pointer to the bitmap of ports that are
*                        Source ID Members of specified PP device.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, sourceId, ports bitmap value
*       GT_BAD_PTR       - portsMembersPtr is NULL pointer
*       GT_OUT_OF_RANGE - length of portsMembersPtr or is out of range
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSrcIdGroupEntrySet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              sourceId;
    GT_BOOL             cpuSrcIdMember;
    CPSS_PORTS_BMP_STC  portsMembersPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    sourceId = (GT_U32)inFields[0];
    cpuSrcIdMember = (GT_BOOL)inFields[1];

    portsMembersPtr.ports[0] = (GT_U32)inFields[2];
    portsMembersPtr.ports[1] = (GT_U32)inFields[3];

    /* call cpss api function */
    result = cpssExMxPmBrgSrcIdGroupEntrySet(devNum, sourceId, cpuSrcIdMember,
                                              &portsMembersPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSrcIdGroupEntryGet
*
* DESCRIPTION:
*        Get entry in Source ID Egress Filtering table.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum            - device number
*       sourceId          - Source ID number.
*
* OUTPUTS:
*       cpuSrcIdMemberPtr - GT_TRUE - CPU is member of of Src ID group.
*                           GT_FALSE - CPU isn't member of of Src ID group.
*       portBitmapPtr     - pointer to the bitmap of ports are Source ID Members
*                           of specified PP device.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum, sourceId
*       GT_HW_ERROR       - on hardware error
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSrcIdGroupEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    GT_BOOL             cpuSrcIdMemberPtr;
    CPSS_PORTS_BMP_STC  portsMembersPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsBzero((char *)&portsMembersPtr, sizeof(portsMembersPtr));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    sourceIdCnt = 0;

    /* call cpss api function */
    result = cpssExMxPmBrgSrcIdGroupEntryGet(devNum, sourceIdCnt,
                                             &cpuSrcIdMemberPtr, 
                                             &portsMembersPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = sourceIdCnt;
    inFields[1] = cpuSrcIdMemberPtr;
    inFields[2] = portsMembersPtr.ports[0];
    inFields[3] = portsMembersPtr.ports[1];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1],
                            inFields[2], inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSrcIdGroupEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_BOOL                         cpuSrcIdMemberPtr;
    CPSS_PORTS_BMP_STC              portsMembersPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsBzero((char *)&portsMembersPtr, sizeof(portsMembersPtr));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    sourceIdCnt++;

    if(sourceIdCnt >= CPSS_EXMXPM_SOURCE_ID_MAX_ENTRY_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxPmBrgSrcIdGroupEntryGet(devNum, sourceIdCnt,
                                             &cpuSrcIdMemberPtr,
                                             &portsMembersPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = sourceIdCnt;
    inFields[1] = cpuSrcIdMemberPtr;
    inFields[2] = portsMembersPtr.ports[0];
    inFields[3] = portsMembersPtr.ports[1];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1],
                            inFields[2], inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgSrcIdPortDefaultSrcIdSet [DB]
*
* DESCRIPTION:
*       Configure Port's Default Source ID.
*       The Source ID is used for source based egress filtering.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       srcId           - Port's Default Source ID [0..31]
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum, portNum, defaultSrcId
*       GT_HW_ERROR       - on hardware error
*       GT_OUT_OF_RANGE   - srcId > 31
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSrcIdPortDefaultSrcIdSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               portNum;
    GT_U32              srcId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    srcId = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgSrcIdPortDefaultSrcIdSet(devNum, portNum, srcId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSrcIdPortDefaultSrcIdGet [DB]
*
* DESCRIPTION:
*       Get configuration of Port's Default Source ID.
*       The Source ID is used for Source based egress filtering.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number
*
* OUTPUTS:
*       srcIdPtr          - Port's Default Source ID [0..31]
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum, portNum
*       GT_HW_ERROR       - on hardware error
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSrcIdPortDefaultSrcIdGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               portNum;
    GT_U32              srcIdPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgSrcIdPortDefaultSrcIdGet(devNum, portNum, &srcIdPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", srcIdPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSrcIdUcEgressFilterEnableSet     [DB]
*
* DESCRIPTION:
*     Enable or disable Source ID egress filter for unicast packets.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE -  enable Source ID filtering on unicast packets.
*                             Unicast packet is dropped if egress port is not
*                             member in the Source ID group.
*                - GT_FALSE - disable Source ID filtering on unicast packets.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSrcIdUcEgressFilterEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_BOOL             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgSrcIdUcEgressFilterEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSrcIdUcEgressFilterEnableGet     [DB]
*
* DESCRIPTION:
*     Get Source ID egress filter configuration for unicast packets.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE -  enable Source ID filtering on unicast packets.
*                             Unicast packet is dropped if egress port is not
*                             member in the Source ID group.
*                   - GT_FALSE - disable Source ID filtering on unicast packets.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSrcIdUcEgressFilterEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_BOOL             enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgSrcIdUcEgressFilterEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet [DB]
*
* DESCRIPTION:
*       Per Egress Port enable or disable Source ID egress filter for unicast
*       packets.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (may be  CPU port).
*       enable   - GT_TRUE - enable Source ID filtering on unicast packets
*                            forwarded to this port. Unicast packet is dropped
*                            if egress port is not member in the Source ID group.
*                - GT_FALSE - disable Source ID filtering on unicast packets
*                             forwarded to this port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, portNum
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               portNum;
    GT_BOOL             enable;

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
    result = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet(devNum, portNum,
                                                           enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet     [DB]
*
* DESCRIPTION:
*       Get Per Egress Port Source ID egress filter configuration for unicast
*       packets.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (may be  CPU port).
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - enable Source ID filtering on unicast packets
*                            forwarded to this port. Unicast packet is dropped
*                            if egress port is not member in the Source ID group.
*                   - GT_FALSE - disable Source ID filtering on unicast packets
*                             forwarded to this port.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - wrong devNum, portNum
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               portNum;
    GT_BOOL             enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet(devNum, portNum,
                                                           &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSrcIdPortAssignModeSet   [DB]
*
* DESCRIPTION:
*         Set Source ID Assignment mode for non-DSA tagged packets
*         per Ingress Port.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*       mode      - the assignment mode of the packet Source ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum, portNum, mode
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSrcIdPortAssignModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               portNum;
    CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    mode = (CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgSrcIdPortAssignModeSet(devNum, portNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgSrcIdPortAssignModeGet   [DB]  [bad cpss func name!]
*
* DESCRIPTION:
*         Get Source ID Assignment mode for non-DSA tagged packets
*         per Ingress Port.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*
* OUTPUTS:
*       modePtr   - the assignment mode of the packet Source ID
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong devNum, portNum
*       GT_HW_ERROR       - on hardware error
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgSrcIdPortAssignModeGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               portNum;
    CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  modePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgSrcIdPortAssignModeGet(devNum, portNum,
                                                       &modePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", modePtr);
    return CMD_OK;
}



/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

    {"cpssExMxPmBrgSrcIdGroupPortAdd",
        &wrCpssExMxPmBrgSrcIdGroupPortAdd,
        3, 0},

    {"cpssExMxPmBrgSrcIdGroupPortDelete",
        &wrCpssExMxPmBrgSrcIdGroupPortDelete,
        3, 0},

    /* ---  cpssExMxPmBrgSrcIdGroupEntry    table  (same as DxCh) */

    {"cpssExMxPmBrgSrcIdGroupEntrySet",
        &wrCpssExMxPmBrgSrcIdGroupEntrySet,
        1, 4},

    {"cpssExMxPmBrgSrcIdGroupEntryGetFirst",
        &wrCpssExMxPmBrgSrcIdGroupEntryGetFirst,
        1, 0},

    {"cpssExMxPmBrgSrcIdGroupEntryGetNext",
        &wrCpssExMxPmBrgSrcIdGroupEntryGetNext,
        1, 0},

    /* ---  cpssExMxPmBrgSrcIdGroupEntry    table  (same as DxCh) */


    {"cpssExMxPmBrgSrcIdPortDefaultSrcIdSet",
        &wrCpssExMxPmBrgSrcIdPortDefaultSrcIdSet,
        3, 0},

    {"cpssExMxPmBrgSrcIdPortDefaultSrcIdGet",
        &wrCpssExMxPmBrgSrcIdPortDefaultSrcIdGet,
        2, 0},

    {"cpssExMxPmBrgSrcIdUcEgressFilterEnableSet",
        &wrCpssExMxPmBrgSrcIdUcEgressFilterEnableSet,
        2, 0},

    {"cpssExMxPmBrgSrcIdUcEgressFilterEnableGet",
        &wrCpssExMxPmBrgSrcIdUcEgressFilterEnableGet,
        1, 0},

    {"cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet",
        &wrCpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet,
        3, 0},

    {"cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet",
        &wrCpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet,
        2, 0},

    {"cpssExMxPmBrgSrcIdPortAssignModeSet",
        &wrCpssExMxPmBrgSrcIdPortAssignModeSet,
        3, 0},

    {"cpssExMxPmBrgSrcIdPortAssignModeGet",
        &wrCpssExMxPmBrgSrcIdPortAssignModeGet,
        2, 0},

    /* CMD_END */
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmBrgSrcId
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
GT_STATUS cmdLibInitCpssExMxPmBrgSrcId
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


