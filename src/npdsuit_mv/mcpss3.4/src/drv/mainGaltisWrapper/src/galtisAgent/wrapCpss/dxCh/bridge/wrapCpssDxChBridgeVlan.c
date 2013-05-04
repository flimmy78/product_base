/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* wrapBridgeCpssDxCh.c
*
* DESCRIPTION:
*       Wrapper functions for Bridge cpss.dxCh functions
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
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpCtrl.h>

/* DxCh max number of entries to be read from tables\classes */
#define  CPSS_DXCH_VLAN_MAX_ENTRY_CNS 4096

/*******************************************************************************
* cpssDxChBrgVlanInit
*
* DESCRIPTION:
*       Initialize VLAN for specific device.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on system init error.
*       GT_OUT_OF_CPU_MEM - on malloc failed
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanInit

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgVlanInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanEntriesRangeWrite
*
* DESCRIPTION:
*       Function set multiple vlans with the same configuration
*       function needed for performances , when the device use "indirect" access
*       to the vlan table.
*
*       APPLICABLE DEVICES:  all DxCh devices
*
* INPUTS:
*       dev             - device number.
*       vlanId          - first VLAN Id
*       numOfEntries    - number of entries (vlans) to set
*       portsMembers    - (pointer to) bmp of ports members in vlan
*       portsTagging    - (pointer to) bmp of ports tagged in the vlan
*       vlanInfoPtr     - (pointer to) VLAN specific information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong vbMode
*       GT_NOT_SUPPORTED - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanEntriesRangeWrite

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
    GT_U32                           numOfEntries;
    CPSS_PORTS_BMP_STC               portsMembersPtr;
    CPSS_PORTS_BMP_STC               portsTaggingPtr;
    CPSS_DXCH_BRG_VLAN_INFO_STC      vlanInfoPtr;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    GT_U32                           i; /* loop iterator */

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    vlanId = (GT_U16)inFields[0];
    numOfEntries = (GT_U32)inFields[1];

    portsMembersPtr.ports[0] = (GT_U32)inFields[2];
    portsMembersPtr.ports[1] = (GT_U32)inFields[3];
    portsTaggingPtr.ports[0] = (GT_U32)inFields[4];
    portsTaggingPtr.ports[1] = (GT_U32)inFields[5];

    vlanInfoPtr.unkSrcAddrSecBreach = (GT_BOOL)inFields[6];
    vlanInfoPtr.unregNonIpMcastCmd = (CPSS_PACKET_CMD_ENT)inFields[7];
    vlanInfoPtr.unregIpv4McastCmd = (CPSS_PACKET_CMD_ENT)inFields[8];
    vlanInfoPtr.unregIpv6McastCmd = (CPSS_PACKET_CMD_ENT)inFields[9];
    vlanInfoPtr.unkUcastCmd = (CPSS_PACKET_CMD_ENT)inFields[10];
    vlanInfoPtr.unregIpv4BcastCmd = (CPSS_PACKET_CMD_ENT)inFields[11];
    vlanInfoPtr.unregNonIpv4BcastCmd = (CPSS_PACKET_CMD_ENT)inFields[12];
    vlanInfoPtr.ipv4IgmpToCpuEn = (GT_BOOL)inFields[13];
    vlanInfoPtr.mirrToRxAnalyzerEn = (GT_BOOL)inFields[14];
    vlanInfoPtr.ipv6IcmpToCpuEn = (GT_BOOL)inFields[15];
    vlanInfoPtr.ipCtrlToCpuEn = (CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT)inFields[16];
    vlanInfoPtr.ipv4IpmBrgMode = (CPSS_BRG_IPM_MODE_ENT)inFields[17];
    vlanInfoPtr.ipv6IpmBrgMode = (CPSS_BRG_IPM_MODE_ENT)inFields[18];
    vlanInfoPtr.ipv4IpmBrgEn = (GT_BOOL)inFields[19];
    vlanInfoPtr.ipv6IpmBrgEn = (GT_BOOL)inFields[20];
    vlanInfoPtr.ipv6SiteIdMode = (CPSS_IP_SITE_ID_ENT)inFields[21];
    vlanInfoPtr.ipv4UcastRouteEn = (GT_BOOL)inFields[22];
    vlanInfoPtr.ipv4McastRouteEn = (GT_BOOL)inFields[23];
    vlanInfoPtr.ipv6UcastRouteEn = (GT_BOOL)inFields[24];
    vlanInfoPtr.ipv6McastRouteEn = (GT_BOOL)inFields[25];
    vlanInfoPtr.stgId = (GT_U32)inFields[26];
    vlanInfoPtr.autoLearnDisable = (GT_BOOL)inFields[27];
    vlanInfoPtr.naMsgToCpuEn = (GT_BOOL)inFields[28];
    vlanInfoPtr.mruIdx = (GT_U32)inFields[29];
    vlanInfoPtr.bcastUdpTrapMirrEn = (GT_BOOL)inFields[30];

    if (numFields == 37)
    {
        vlanInfoPtr.vrfId = (GT_U32)inFields[31];
        vlanInfoPtr.floodVidx = (GT_U16)inFields[32];
        vlanInfoPtr.floodVidxMode = (CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT)inFields[33];
        vlanInfoPtr.portIsolationMode = (CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT)inFields[34];
        vlanInfoPtr.ucastLocalSwitchingEn = (GT_BOOL)inFields[35];
        vlanInfoPtr.mcastLocalSwitchingEn = (GT_BOOL)inFields[36];
    }
    else
    {
        vlanInfoPtr.vrfId = 0;
        vlanInfoPtr.floodVidx = 0xFFF;
        vlanInfoPtr.floodVidxMode = CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E;
        vlanInfoPtr.portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E;
        vlanInfoPtr.ucastLocalSwitchingEn = GT_FALSE;
        vlanInfoPtr.mcastLocalSwitchingEn = GT_FALSE;
    }

    for (i = 0; i < CPSS_MAX_PORTS_NUM_CNS; i++)
    {
        portsTaggingCmd.portsCmd[i] = ((portsTaggingPtr.ports[0] >> i) & 1)
                                    ? CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E
                                    : CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
    }

    /* call cpss api function */
    result = cpssDxChBrgVlanEntriesRangeWrite(devNum, vlanId, numOfEntries,
                                              &portsMembersPtr, &portsTaggingPtr,
                                              &vlanInfoPtr, &portsTaggingCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}




/*cpssDxChBrgVlan Table*/
static GT_U16   vlanIdCnt;

/*
* cpssDxChBrgVlanEntryWrite
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW .
*
* APPLICABLE DEVICES: All DxCh devices
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
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanEntryWrite

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
    CPSS_DXCH_BRG_VLAN_INFO_STC      vlanInfoPtr;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    GT_U32                           i; /* loop iterator */

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    vlanId = (GT_U16)inFields[0];

    portsMembersPtr.ports[0] = (GT_U32)inFields[1];
    portsMembersPtr.ports[1] = (GT_U32)inFields[2];
    portsTaggingPtr.ports[0] = (GT_U32)inFields[3];
    portsTaggingPtr.ports[1] = (GT_U32)inFields[4];

    CONVERT_DEV_PORTS_BMP_MAC(devNum,portsMembersPtr);
    CONVERT_DEV_PORTS_BMP_MAC(devNum,portsTaggingPtr);

    vlanInfoPtr.unkSrcAddrSecBreach = (GT_BOOL)inFields[5];
    vlanInfoPtr.unregNonIpMcastCmd = (CPSS_PACKET_CMD_ENT)inFields[6];
    vlanInfoPtr.unregIpv4McastCmd = (CPSS_PACKET_CMD_ENT)inFields[7];
    vlanInfoPtr.unregIpv6McastCmd = (CPSS_PACKET_CMD_ENT)inFields[8];
    vlanInfoPtr.unkUcastCmd = (CPSS_PACKET_CMD_ENT)inFields[9];
    vlanInfoPtr.unregIpv4BcastCmd = (CPSS_PACKET_CMD_ENT)inFields[10];
    vlanInfoPtr.unregNonIpv4BcastCmd = (CPSS_PACKET_CMD_ENT)inFields[11];
    vlanInfoPtr.ipv4IgmpToCpuEn = (GT_BOOL)inFields[12];
    vlanInfoPtr.mirrToRxAnalyzerEn = (GT_BOOL)inFields[13];
    vlanInfoPtr.ipv6IcmpToCpuEn = (GT_BOOL)inFields[14];
    vlanInfoPtr.ipCtrlToCpuEn = (CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT)inFields[15];
    vlanInfoPtr.ipv4IpmBrgMode = (CPSS_BRG_IPM_MODE_ENT)inFields[16];
    vlanInfoPtr.ipv6IpmBrgMode = (CPSS_BRG_IPM_MODE_ENT)inFields[17];
    vlanInfoPtr.ipv4IpmBrgEn = (GT_BOOL)inFields[18];
    vlanInfoPtr.ipv6IpmBrgEn = (GT_BOOL)inFields[19];
    vlanInfoPtr.ipv6SiteIdMode = (CPSS_IP_SITE_ID_ENT)inFields[20];
    vlanInfoPtr.ipv4UcastRouteEn = (GT_BOOL)inFields[21];
    vlanInfoPtr.ipv4McastRouteEn = (GT_BOOL)inFields[22];
    vlanInfoPtr.ipv6UcastRouteEn = (GT_BOOL)inFields[23];
    vlanInfoPtr.ipv6McastRouteEn = (GT_BOOL)inFields[24];
    vlanInfoPtr.stgId = (GT_U32)inFields[25];
    vlanInfoPtr.autoLearnDisable = (GT_BOOL)inFields[26];
    vlanInfoPtr.naMsgToCpuEn = (GT_BOOL)inFields[27];
    vlanInfoPtr.mruIdx = (GT_U32)inFields[28];
    vlanInfoPtr.bcastUdpTrapMirrEn = (GT_BOOL)inFields[29];
    if (numFields==30)
    {
        vlanInfoPtr.vrfId = 0;/* table don't support virtual router */
        vlanInfoPtr.floodVidx = 0xFFF;
        vlanInfoPtr.floodVidxMode = CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E;
        vlanInfoPtr.portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E;
        vlanInfoPtr.ucastLocalSwitchingEn = GT_TRUE;
        vlanInfoPtr.mcastLocalSwitchingEn = GT_TRUE;
    }

    else if (numFields==31)
    {/* table support virtual router */
        vlanInfoPtr.vrfId =(GT_U32)inFields[30];
        vlanInfoPtr.floodVidx = 0xFFF;
        vlanInfoPtr.floodVidxMode = CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E;
        vlanInfoPtr.portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E;
        vlanInfoPtr.ucastLocalSwitchingEn = GT_TRUE;
        vlanInfoPtr.mcastLocalSwitchingEn = GT_TRUE;
    }
    else if (numFields==36)
    {
        vlanInfoPtr.vrfId =(GT_U32)inFields[30];
        vlanInfoPtr.floodVidx = (GT_U16)inFields[31];
        vlanInfoPtr.floodVidxMode = (CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT)inFields[32];
        vlanInfoPtr.portIsolationMode = (CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT)inFields[33];
        vlanInfoPtr.ucastLocalSwitchingEn = (GT_BOOL)inFields[34];
        vlanInfoPtr.mcastLocalSwitchingEn = (GT_BOOL)inFields[35];
    }

    for (i = 0; i < CPSS_MAX_PORTS_NUM_CNS; i++)
    {
        portsTaggingCmd.portsCmd[i] = CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsTaggingPtr, i)
                                    ? CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E
                                    : CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
    }

    /* call cpss api function */
    result = cpssDxChBrgVlanEntryWrite(devNum,
                                       vlanId,
                                       &portsMembersPtr,
                                       &portsTaggingPtr,
                                       &vlanInfoPtr,
                                       &portsTaggingCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

static CMD_STATUS internal_wrCpssDxChBrgVlanEntryRead
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
IN  GT_BOOL vrfIdSupported,
OUT GT_8  outArgs[CMD_MAX_BUFFER]

)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_PORTS_BMP_STC              portsMembersPtr;
    CPSS_PORTS_BMP_STC              portsTaggingPtr;
    CPSS_DXCH_BRG_VLAN_INFO_STC     vlanInfoPtr;
    GT_BOOL                         isValidPtr = GT_FALSE;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    GT_U32                          i; /* loop iterator */

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    while ((isValidPtr == GT_FALSE) && (vlanIdCnt < CPSS_DXCH_VLAN_MAX_ENTRY_CNS))
    {
        /* call cpss api function */
        result = cpssDxChBrgVlanEntryRead(devNum, vlanIdCnt, &portsMembersPtr,
                                          &portsTaggingPtr, &vlanInfoPtr, &isValidPtr, &portsTaggingCmd);

        if (PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported == GT_TRUE)
        {
            portsTaggingPtr.ports[0] = portsTaggingPtr.ports[1] = 0;
            for (i = 0; i < CPSS_MAX_PORTS_NUM_CNS; i++)
            {
                if (portsTaggingCmd.portsCmd[i] != CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E)
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(&portsTaggingPtr,i);
                }
            }
        }

        if( (vlanIdCnt != 1) ||
            ((portsMembersPtr.ports[0] != PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[0]) &&
             (portsMembersPtr.ports[1] != PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[1])) )
        {
            CONVERT_BACK_DEV_PORTS_BMP_MAC(devNum,portsMembersPtr);
            CONVERT_BACK_DEV_PORTS_BMP_MAC(devNum,portsTaggingPtr);
        }

        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

        vlanIdCnt++;
    }

    if ((vlanIdCnt >= CPSS_DXCH_VLAN_MAX_ENTRY_CNS) &&
        (isValidPtr == GT_FALSE) /* need to check valid to catch valid entry 4095
                                 for this entry vlanIdCnt will be ==
                                 CPSS_DXCH_VLAN_MAX_ENTRY_CNS */)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }


    inFields[0] = vlanIdCnt - 1;/* the vlanIdCnt was already incremented */
    inFields[1] = portsMembersPtr.ports[0];
    inFields[2] = portsMembersPtr.ports[1];
    inFields[3] = portsTaggingPtr.ports[0];
    inFields[4] = portsTaggingPtr.ports[1];
    inFields[5] = vlanInfoPtr.unkSrcAddrSecBreach;
    inFields[6] = vlanInfoPtr.unregNonIpMcastCmd;
    inFields[7] = vlanInfoPtr.unregIpv4McastCmd;
    inFields[8] = vlanInfoPtr.unregIpv6McastCmd;
    inFields[9] = vlanInfoPtr.unkUcastCmd;
    inFields[10] = vlanInfoPtr.unregIpv4BcastCmd;
    inFields[11] = vlanInfoPtr.unregNonIpv4BcastCmd;
    inFields[12] = vlanInfoPtr.ipv4IgmpToCpuEn;
    inFields[13] = vlanInfoPtr.mirrToRxAnalyzerEn;
    inFields[14] = vlanInfoPtr.ipv6IcmpToCpuEn;
    inFields[15] = vlanInfoPtr.ipCtrlToCpuEn;
    inFields[16] = vlanInfoPtr.ipv4IpmBrgMode;
    inFields[17] = vlanInfoPtr.ipv6IpmBrgMode;
    inFields[18] = vlanInfoPtr.ipv4IpmBrgEn;
    inFields[19] = vlanInfoPtr.ipv6IpmBrgEn;
    inFields[20] = vlanInfoPtr.ipv6SiteIdMode;
    inFields[21] = vlanInfoPtr.ipv4UcastRouteEn;
    inFields[22] = vlanInfoPtr.ipv4McastRouteEn;
    inFields[23] = vlanInfoPtr.ipv6UcastRouteEn;
    inFields[24] = vlanInfoPtr.ipv6McastRouteEn;
    inFields[25] = vlanInfoPtr.stgId;
    inFields[26] = vlanInfoPtr.autoLearnDisable;
    inFields[27] = vlanInfoPtr.naMsgToCpuEn;
    inFields[28] = vlanInfoPtr.mruIdx;
    inFields[29] = vlanInfoPtr.bcastUdpTrapMirrEn;


    if (vrfIdSupported){

        inFields[30] = vlanInfoPtr.vrfId;
        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],  inFields[2],
                    inFields[3],  inFields[4],  inFields[5],
                    inFields[6],  inFields[7],  inFields[8],
                    inFields[9],  inFields[10], inFields[11],
                    inFields[12], inFields[13], inFields[14],
                    inFields[15], inFields[16], inFields[17],
                    inFields[18], inFields[19], inFields[20],
                    inFields[21], inFields[22], inFields[23],
                    inFields[24], inFields[25], inFields[26],
                    inFields[27], inFields[28], inFields[29],inFields[30]);


    }
    else{

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
            %d%d%d", inFields[0],  inFields[1],  inFields[2],
                    inFields[3],  inFields[4],  inFields[5],
                    inFields[6],  inFields[7],  inFields[8],
                    inFields[9],  inFields[10], inFields[11],
                    inFields[12], inFields[13], inFields[14],
                    inFields[15], inFields[16], inFields[17],
                    inFields[18], inFields[19], inFields[20],
                    inFields[21], inFields[22], inFields[23],
                    inFields[24], inFields[25], inFields[26],
                    inFields[27], inFields[28], inFields[29]);




    }

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

static CMD_STATUS internal_wrCpssDxChBrgVlanEntryExt1Read
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]

)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_PORTS_BMP_STC              portsMembersPtr;
    CPSS_PORTS_BMP_STC              portsTaggingPtr;
    CPSS_DXCH_BRG_VLAN_INFO_STC     vlanInfoPtr;
    GT_BOOL                         isValidPtr = GT_FALSE;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    GT_U32                          i; /* loop iterator */

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    while ((isValidPtr == GT_FALSE) && (vlanIdCnt < CPSS_DXCH_VLAN_MAX_ENTRY_CNS))
    {
        /* call cpss api function */
        result = cpssDxChBrgVlanEntryRead(devNum, vlanIdCnt, &portsMembersPtr,
                                          &portsTaggingPtr, &vlanInfoPtr, &isValidPtr, &portsTaggingCmd);

        if (PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported == GT_TRUE)
        {
            portsTaggingPtr.ports[0] = portsTaggingPtr.ports[1] = 0;
            for (i = 0; i < CPSS_MAX_PORTS_NUM_CNS; i++)
            {
                if (portsTaggingCmd.portsCmd[i] != CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E)
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(&portsTaggingPtr,i);
                }
            }
        }

        if( (vlanIdCnt != 1) ||
            ((portsMembersPtr.ports[0] != PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[0]) &&
             (portsMembersPtr.ports[1] != PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[1])) )
        {
            CONVERT_BACK_DEV_PORTS_BMP_MAC(devNum,portsMembersPtr);
            CONVERT_BACK_DEV_PORTS_BMP_MAC(devNum,portsTaggingPtr);
        }

        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

        vlanIdCnt++;
    }

    if ((vlanIdCnt >= CPSS_DXCH_VLAN_MAX_ENTRY_CNS) &&
        (isValidPtr == GT_FALSE) /* need to check valid to catch valid entry 4095
                                 for this entry vlanIdCnt will be ==
                                 CPSS_DXCH_VLAN_MAX_ENTRY_CNS */)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }


    inFields[0] = vlanIdCnt - 1;/* the vlanIdCnt was already incremented */
    inFields[1] = portsMembersPtr.ports[0];
    inFields[2] = portsMembersPtr.ports[1];
    inFields[3] = portsTaggingPtr.ports[0];
    inFields[4] = portsTaggingPtr.ports[1];
    inFields[5] = vlanInfoPtr.unkSrcAddrSecBreach;
    inFields[6] = vlanInfoPtr.unregNonIpMcastCmd;
    inFields[7] = vlanInfoPtr.unregIpv4McastCmd;
    inFields[8] = vlanInfoPtr.unregIpv6McastCmd;
    inFields[9] = vlanInfoPtr.unkUcastCmd;
    inFields[10] = vlanInfoPtr.unregIpv4BcastCmd;
    inFields[11] = vlanInfoPtr.unregNonIpv4BcastCmd;
    inFields[12] = vlanInfoPtr.ipv4IgmpToCpuEn;
    inFields[13] = vlanInfoPtr.mirrToRxAnalyzerEn;
    inFields[14] = vlanInfoPtr.ipv6IcmpToCpuEn;
    inFields[15] = vlanInfoPtr.ipCtrlToCpuEn;
    inFields[16] = vlanInfoPtr.ipv4IpmBrgMode;
    inFields[17] = vlanInfoPtr.ipv6IpmBrgMode;
    inFields[18] = vlanInfoPtr.ipv4IpmBrgEn;
    inFields[19] = vlanInfoPtr.ipv6IpmBrgEn;
    inFields[20] = vlanInfoPtr.ipv6SiteIdMode;
    inFields[21] = vlanInfoPtr.ipv4UcastRouteEn;
    inFields[22] = vlanInfoPtr.ipv4McastRouteEn;
    inFields[23] = vlanInfoPtr.ipv6UcastRouteEn;
    inFields[24] = vlanInfoPtr.ipv6McastRouteEn;
    inFields[25] = vlanInfoPtr.stgId;
    inFields[26] = vlanInfoPtr.autoLearnDisable;
    inFields[27] = vlanInfoPtr.naMsgToCpuEn;
    inFields[28] = vlanInfoPtr.mruIdx;
    inFields[29] = vlanInfoPtr.bcastUdpTrapMirrEn;
    inFields[30] = vlanInfoPtr.vrfId;
    inFields[31] = vlanInfoPtr.floodVidx;
    inFields[32] = vlanInfoPtr.floodVidxMode;
    inFields[33] = vlanInfoPtr.portIsolationMode;
    inFields[34] = vlanInfoPtr.ucastLocalSwitchingEn;
    inFields[35] = vlanInfoPtr.mcastLocalSwitchingEn;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],
                inFields[9],  inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17],
                inFields[18], inFields[19], inFields[20],
                inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26],
                inFields[27], inFields[28], inFields[29],
                inFields[30], inFields[31], inFields[32],
                inFields[33], inFields[34], inFields[35]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChBrgVlanEntryRead
*
* DESCRIPTION:
*       Read vlan entry.
*
* APPLICABLE DEVICES:  All DxCh devices
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
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanEntryReadFirst

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    vlanIdCnt = 0;

    return internal_wrCpssDxChBrgVlanEntryRead(inArgs,inFields,numFields,GT_FALSE,outArgs);
}


/*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanEntryReadNext
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internal_wrCpssDxChBrgVlanEntryRead(inArgs,inFields,numFields,GT_FALSE,outArgs);
}
/********************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanEntryExtReadFirst

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    vlanIdCnt = 0;

    return internal_wrCpssDxChBrgVlanEntryRead(inArgs,inFields,numFields,GT_TRUE,outArgs);
}


/*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanEntryExtReadNext
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internal_wrCpssDxChBrgVlanEntryRead(inArgs,inFields,numFields,GT_TRUE,outArgs);
}



/********************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanEntryExt1ReadFirst

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    vlanIdCnt = 0;

    return internal_wrCpssDxChBrgVlanEntryExt1Read(inArgs, inFields, numFields, outArgs);
}


/*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanEntryExt1ReadNext
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internal_wrCpssDxChBrgVlanEntryExt1Read(inArgs, inFields, numFields, outArgs);
}



/*******************************************************************************
* cpssDxChBrgVlanMemberAdd
*
* DESCRIPTION:
*       Add new port member to vlan entry. This function can be called only for
*       add port belongs to device that already member of the vlan.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*       portNum         - physical port number
*       isTagged        - GT_TRUE, to set the port as tagged member,
*                         GT_FALSE, to set the port as untagged
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*
* COMMENTS:
*       In case that added port belongs to device that is new device for vlan
*       other function is used - cpssDxChBrgVlanEntryWrite.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanMemberAdd
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_U8                         portNum;
    GT_BOOL                       isTagged;
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT taggingCmd;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    isTagged = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    taggingCmd = (isTagged == GT_TRUE) ? CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E : CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;

    /* call cpss api function */
    result = cpssDxChBrgVlanMemberAdd(devNum, vlanId, portNum, isTagged, taggingCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanMemberAdd
*
* DESCRIPTION:
*       Add new port member to vlan entry. This function can be called only for
*       add port belongs to device that already member of the vlan.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*       portNum         - port number
*       isTagged        - GT_TRUE, to set the port as tagged member,
*                         GT_FALSE, to set the port as untagged
*                         the relevant parameter for DxCh3 and beyond.
*       taggingCmd      - port tagging command
*                         the relevant parameter for DxChXcat and above.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vid or taggingCmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In case that added port belongs to device that is new device for vlan
*       other function is used - cpssDxChBrgVlanEntryWrite.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanMemberAdd1
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_U8                         portNum;
    GT_BOOL                       isTagged;
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT taggingCmd;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    isTagged = (GT_BOOL)inArgs[3];
    taggingCmd = (CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanMemberAdd(devNum, vlanId, portNum, isTagged, taggingCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanPortDelete
*
* DESCRIPTION:
*       Delete port member from vlan entry.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN Id
*       port        - port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vlanId
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortDelete

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_U8                         portNum;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    portNum = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortDelete(devNum, vlanId, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanMemberSet
*
* DESCRIPTION:
*       Set specific member at VLAN entry.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum          - device number
*       vlanId          - VLAN Id
*       portNum         - port number
*       isMember        - GT_TRUE,  port would set as member
*                         GT_FALSE, port would set as not-member
*       isTagged        - GT_TRUE, to set the port as tagged
*                         GT_FALSE, to set the port as untagged
*                         the relevant parameter for DxCh3 and beyond.
*       taggingCmd      - set port tagging command
*                         the relevant parameter for DxChXcat and above.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or taggingCmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanMemberSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_U8                         portNum;
    GT_BOOL                       isMember;
    GT_BOOL                       isTagged;
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT taggingCmd;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    isMember = (GT_BOOL)inArgs[3];
    isTagged = (GT_BOOL)inArgs[4];
    taggingCmd = (CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT)inArgs[5];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanMemberSet(devNum, vlanId, portNum, isMember, isTagged, taggingCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortVidGet
*
* DESCRIPTION:
*       Get port's default VLAN Id.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       port     - port number
*
* OUTPUTS:
*       vidPtr   - default VLAN ID.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - wrong devNum or port
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortVidGet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U8                         portNum;
    GT_U16                        vidPtr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortVidGet(devNum, portNum, &vidPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vidPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanPortVidSet
*
* DESCRIPTION:
*       Set port's default VLAN Id.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       port     - port number
*       vlanId    - VLAN Id
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM    - wrong devNum port, or vlanId
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortVidSet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U8                         portNum;
    GT_U16                        vlanId;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    vlanId = (GT_U16)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortVidSet(devNum, portNum, vlanId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanPortIngFltEnable
*
* DESCRIPTION:
*       Enable/disable Ingress Filtering for specific port
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum    - device number
*       port      - port number
*       enable    - GT_TRUE, enable ingress filtering
*                   GT_FALSE, disable ingress filtering
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM    - wrong devNum or port
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortIngFltEnable

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U8                         portNum;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortIngFltEnable(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanPortProtoClassVlanEnable
*
* DESCRIPTION:
*       Enable/Disable Port Protocol VLAN assignment to packets received
*       on this port,according to it's Layer2 protocol
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - device number
*       port    - port number
*       enable  - GT_TRUE for enable, otherwise GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or port
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortProtoClassVlanEnable

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U8                         portNum;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortProtoClassVlanEnable(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanPortProtoClassQosEnable
*
* DESCRIPTION:
*       Enable/Disable Port Protocol QoS Assignment to packets received on this
*       port, according to it's Layer2 protocol
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - device number
*       port    - port number
*       enable  - GT_TRUE for enable, otherwise GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or port
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortProtoClassQosEnable

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U8                         portNum;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortProtoClassQosEnable(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*cpssDxChBrgVlanProtoClass Table*/
static GT_U32   entryCnt;


/*******************************************************************************
* cpssDxChBrgVlanProtoClassSet
*
* DESCRIPTION:
*       Write etherType and encapsulation of Protocol based classification
*       for specific device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum      - device number
*       entryNum    - entry number (0..7)
*       etherType   - Ether Type or DSAP/SSAP
*       encListPtr  - encapsulation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanProtoClassSet

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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    entryNum = (GT_U32)inFields[0];
    etherType = (GT_U16)inFields[1];

    encListPtr.ethV2 = (GT_BOOL)inFields[2];
    encListPtr.nonLlcSnap = (GT_BOOL)inFields[3];
    encListPtr.llcSnap = (GT_BOOL)inFields[4];

    /* call cpss api function */
    result = cpssDxChBrgVlanProtoClassSet(devNum, entryNum, etherType,
                                          &encListPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanProtoClassGet
*
* DESCRIPTION:
*       Read etherType and encapsulation of Protocol based classification
*       for specific device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum       - device number
*       entryNum     - entry number
*
* OUTPUTS:
*       etherTypePtr - (pointer to) Ether Type or DSAP/SSAP
*       encListPtr   - (pointer to) encapsulation.
*       validEntryPtr - (pointer to) validity bit
*                                   GT_TRUE - the entry is valid
*                                   GT_FALSE - the entry is invalid
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanProtoClassGetFirst

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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryCnt = 0;

    /* call cpss api function */
    result = cpssDxChBrgVlanProtoClassGet(devNum, entryCnt, &etherTypePtr,
                                          &encListPtr, &validEntryPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if (validEntryPtr)
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
static CMD_STATUS wrCpssDxChBrgVlanProtoClassGetNext
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryCnt++;

    if (entryCnt > CPSS_DXCH_VLAN_MAX_ENTRY_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChBrgVlanProtoClassGet(devNum, entryCnt, &etherTypePtr,
                                          &encListPtr, &validEntryPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if (validEntryPtr)
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
* cpssDxChBrgVlanProtoClassInvalidate
*
* DESCRIPTION:
*       Invalidates entry of Protocol based classification for specific device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum       - device number
*       entryNum     - entry number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanProtoClassInvalidate

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U32                        entryNum;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    entryNum = (GT_U32)inFields[0];

    /* call cpss api function */
    result = cpssDxChBrgVlanProtoClassInvalidate(devNum, entryNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*cpssDxChBrgVlanProtoClass Table*/
static GT_U32   entryNumCnt;

/*******************************************************************************
* cpssDxChBrgVlanPortProtoVlanQosSet
*
* DESCRIPTION:
*       Write Port Protocol VID and QOS Assignment values for specific device
*       and port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum              - device number
*       port                - port number
*       entryNum            - entry number
*       vlanCfgPtr          - pointer to VID parameters
*       qosCfgPtr           - pointer to QoS parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM    - wrong devNum, port or vlanId
*
* COMMENTS:
*       It is possible to update only VLAN or QOS parameters, when one
*       of those input pointers is NULL.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortProtoVlanQosSet

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
    CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC      vlanCfgPtr;
    CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC       qosCfgPtr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
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

    qosCfgPtr.qosAssignCmd =  (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[4];
    qosCfgPtr.qosProfileId = (GT_U32)inFields[5];
    qosCfgPtr.qosAssignPrecedence =
    (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[6];
    qosCfgPtr.enableModifyUp = (GT_BOOL)inFields[7];
    qosCfgPtr.enableModifyDscp = (GT_BOOL)inFields[8];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortProtoVlanQosSet(devNum, port, entryNum,
                                                &vlanCfgPtr, &qosCfgPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrPrvCpssDxChBrgVlanPortProtoVlanQosGetNext
*
* DESCRIPTION:
*       Get Port Protocol VID and QOS Assignment values for specific device
*       and port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum              - device number
*       port                - port number
*       entryNum            - entry number
*
* OUTPUTS:
*       vlanCfgPtr          - pointer to VID parameters
*       qosCfgPtr           - pointer to QoS parameters
*       validEntryPtr       - pointer to validity bit
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM    - wrong devNum, port or vlanId
*
* COMMENTS:
*      It is possible to get only one of VLAN or QOS parameters by receiving
*      one of those pointers as NULL pointer.
*
*******************************************************************************/
static CMD_STATUS wrPrvCpssDxChBrgVlanPortProtoVlanQosGetNext

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
    CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC    vlanCfgPtr;
    CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC     qosCfgPtr;
    GT_BOOL                                  validEntry;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    validEntry = GT_FALSE;

    while (validEntry == GT_FALSE)
    {
        /* call cpss api function */
        result = cpssDxChBrgVlanPortProtoVlanQosGet(devNum, port, entryNumCnt,
                                                    &vlanCfgPtr, &qosCfgPtr, &validEntry);
    
        if (result != GT_OK)
        {
            if ((entryNumCnt > 7) && (result == GT_BAD_PARAM))
            {
                /* this is end of table */
                result = GT_OK;
            }

            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
        if (validEntry == GT_FALSE)
        {
            /* check next entry */
            entryNumCnt++;
        }
    }

    inFields[0] = entryNumCnt;
    inFields[1] = vlanCfgPtr.vlanId;
    inFields[2] = vlanCfgPtr.vlanIdAssignCmd;
    inFields[3] = vlanCfgPtr.vlanIdAssignPrecedence;
    inFields[4] = qosCfgPtr.qosAssignCmd;
    inFields[5] = qosCfgPtr.qosProfileId;
    inFields[6] = qosCfgPtr.qosAssignPrecedence;
    inFields[7] = qosCfgPtr.enableModifyUp;
    inFields[8] = qosCfgPtr.enableModifyDscp;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2],
                inFields[3], inFields[4], inFields[5],
                inFields[6], inFields[7], inFields[8]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortProtoVlanQosGet
*
* DESCRIPTION:
*       Get Port Protocol VID and QOS Assignment values for specific device
*       and port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum              - device number
*       port                - port number
*       entryNum            - entry number
*
* OUTPUTS:
*       vlanCfgPtr          - pointer to VID parameters
*       qosCfgPtr           - pointer to QoS parameters
*       validEntryPtr       - pointer to validity bit
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM    - wrong devNum, port or vlanId
*
* COMMENTS:
*      It is possible to get only one of VLAN or QOS parameters by receiving
*      one of those pointers as NULL pointer.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortProtoVlanQosGetFirst

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    entryNumCnt = 0;
    return wrPrvCpssDxChBrgVlanPortProtoVlanQosGetNext(inArgs, inFields,
                                                       numFields, outArgs);
}


/*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortProtoVlanQosGetNext
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    entryNumCnt++;

    return wrPrvCpssDxChBrgVlanPortProtoVlanQosGetNext(inArgs, inFields,
                                                       numFields, outArgs);
}


/*******************************************************************************
* cpssDxChBrgVlanPortProtoVlanQosInvalidate
*
* DESCRIPTION:
*       Invalidate protocol based QoS and VLAN assignment entry for
*       specific port
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum       - device number
*       port      - port number
*       entryNum  - entry number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortProtoVlanQosInvalidate

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U8                         portNum;
    GT_U32                        entryNum;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    entryNum = (GT_U32)inFields[0];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortProtoVlanQosInvalidate(devNum, portNum,
                                                       entryNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanLearningStateSet
*
* DESCRIPTION:
*       Sets state of VLAN based learning to specified VLAN on specified device.
*
* APPLICABLE DEVICES:  98DX2x5 only
*
* INPUTS:
*       devNum    - device number
*       vlanId    - vlan Id
*       status    - GT_TRUE for enable  or GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vlanId
*
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanLearningStateSet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        portNum;
    GT_BOOL                       status;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U16)inArgs[1];
    status = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanLearningStateSet(devNum, portNum, status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssDxChBrgVlanIsDevMember
*
* DESCRIPTION:
*       Checks if specified device has members belong to specified vlan
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE   - if port is member of vlan
*       GT_FALSE  - if port does not member of vlan
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vlanId
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanIsDevMember

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgVlanIsDevMember(devNum, vlanId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanToStpIdBind
*
* DESCRIPTION:
*       Bind VLAN to STP Id.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - device Id
*       vlanId  - vlan Id
*       stpId   - STP Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM        - wrong devNum or vlanId
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanToStpIdBind

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_U16                        stpId;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    stpId = (GT_U16)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanToStpIdBind(devNum, vlanId, stpId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanStpIdGet
*
* DESCRIPTION:
*       Read STP Id that bind to specified VLAN.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       vlanId  - vlan Id
*
* OUTPUTS:
*       stpIdPtr   - pointer to STP Id
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on error
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM        - wrong devNum or vlanId
*       GT_BAD_PTR          - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanStpIdGet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_U16                        stpIdPtr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgVlanStpIdGet(devNum, vlanId, &stpIdPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", stpIdPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanEtherTypeSet
*
* DESCRIPTION:
*       Sets vlan Ether Type mode and vlan range.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum        - device number
*       etherTypeMode - mode of tagged vlan etherType. May be either
*                       CPSS_VLAN_ETHERTYPE0_E or CPSS_VLAN_ETHERTYPE1_E.
*                       See comment.
*       ethertype     - the EtherType to be recognized tagged packets. See
*                       comment.
*       vidRange      - the maximum vlan range to accept for the specified
*                       etheType.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal vidRange value
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vidRange
*
* COMMENTS:
*       If the vid of the incoming packet is greater or equal (>=) than the vlan
*       range value, the packet is filtered.
*
*       Configuration of vid and range for CPSS_VLAN_ETHERTYPE1_E should be done
*       only when nested vlan is used. User can alter the etherType field of
*       the encapsulating vlan by setting a number other then 0x8100 in the
*       parameter <ethertype>.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanEtherTypeSet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    CPSS_ETHER_MODE_ENT           etherTypeMode;
    GT_U16                        etherType;
    GT_U16                        vidRange;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    etherTypeMode = (CPSS_ETHER_MODE_ENT)inArgs[1];
    etherType = (GT_U16)inArgs[2];
    vidRange = (GT_U16)inArgs[3];

    /* call cpss api function */
    result = cpssDxChBrgVlanEtherTypeSet(devNum, etherTypeMode,
                                         etherType, vidRange);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanTableInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN Table entries by writing 0 to the
*       first word.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - device number.
*       vid     - VLAN id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - otherwise
*       GT_BAD_PARAM     - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanTableInvalidate

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgVlanTableInvalidate(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanEntryInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN entry.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - device number.
*       vlanId     - VLAN id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - otherwise
*       GT_BAD_PARAM     - wrong devNum or vid
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanEntryInvalidate

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgVlanEntryInvalidate(devNum, vlanId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanMacLsbSet
*
* DESCRIPTION:
*       Sets the 8 LSB Router MAC SA for this VLAN.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*     devNum          - device number.
*     vlanId          - VLAN Id
*     saMac           - The 8 bits SA mac value to be written to the SA bits of
*                       routed packet if SA alteration mode is configured to
*                       take LSB according to VLAN id.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_SUPPORTED - if the feature does not supported by specified device
*     GT_FAIL          - on error.
*     GT_HW_ERROR      - on hardware error
*     GT_BAD_PARAM     - wrong devNum or vlanId
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanMacLsbSet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_U8                         saMac;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    saMac = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpRouterPortVlanMacSaLsbSet(devNum, vlanId, saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanUnkUnregFilterSet
*
* DESCRIPTION:
*       Set per VLAN filtering command for specified Unknown or Unregistered
*       packet type
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*             devNum                - device number.
*             vlanId                - VLAN ID
*             packetType            - packet type
*             cmd                   - command for the specified packet type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - wrong devNum or vlanId
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanUnkUnregFilterSet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;

    GT_U8                                   devNum;
    GT_U16                                  vlanId;
    CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT      packetType;
    CPSS_PACKET_CMD_ENT                     cmd;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    packetType = (CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT)inArgs[2];
    cmd = (CPSS_PACKET_CMD_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChBrgVlanUnkUnregFilterSet(devNum, vlanId, packetType, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanForcePvidEnable
*
* DESCRIPTION:
*       Set Port VID Assignment mode.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       port     - port number
*       forceEn  - GT_TRUE  - PVID is assigned to all packets.
*                  GT_FALSE - PVID is assigned to untagged or priority tagged
*                             packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanForcePvidEnable

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U8                         port;
    GT_BOOL                       enable;

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
    result = cpssDxChBrgVlanForcePvidEnable(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanForcePvidEnableGet
*
* DESCRIPTION:
*       Get Port VID Assignment mode.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - device number
*       port     - port number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE  - PVID is assigned to all packets.
*                     GT_FALSE - PVID is assigned to untagged or priority tagged
*                                packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanForcePvidEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;
    GT_U8                         devNum;
    GT_U8                         port;
    GT_BOOL                       enable;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChBrgVlanForcePvidEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;

}

/*******************************************************************************
* cpssDxChBrgVlanPortVidPrecedenceSet
*
* DESCRIPTION:
*       Set Port VID Precedence.
*       Relevant for packets with assigned VLAN ID of Pvid.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum      - device number
*       port        - port number
*       precedence  - precedence type - soft or hard
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortVidPrecedenceSet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;

    GT_U8                                          devNum;
    GT_U8                                          port;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT    precedence;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    precedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortVidPrecedenceSet(devNum, port, precedence);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortVidPrecedenceGet
*
* DESCRIPTION:
*       Get Port VID Precedence.
*       Relevant for packets with assigned VLAN ID of Pvid.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum      - device number
*       port        - port number
*
* OUTPUTS:
*       precedencePtr - (pointer to) precedence type - soft or hard
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortVidPrecedenceGet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;

    GT_U8                                          devNum;
    GT_U8                                          port;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT    precedence;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortVidPrecedenceGet(devNum, port, &precedence);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", precedence);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanIpUcRouteEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4/Ipv6 Unicast Routing on Vlan
*
* APPLIOCABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*       protocol    - ipv4 or ipv6
*       value       - GT_TRUE - enable ip unicast routing;
*                     GT_FALSE -  disable ip unicast routing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanIpUcRouteEnable

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    CPSS_IP_PROTOCOL_STACK_ENT    protocol;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    protocol = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChBrgVlanIpUcRouteEnable(devNum, vlanId, protocol, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanIpMcRouteEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4/Ipv6 Multicast Routing on Vlan
*
* APPLICABLE DEVICES: 98DX2x5 only
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan ID
*       protocol    - ipv4 or ipv6
*       value       - GT_TRUE - enable ip multicast routing;
*                     GT_FALSE -  disable ip multicast routing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanIpMcRouteEnable

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    CPSS_IP_PROTOCOL_STACK_ENT    protocol;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    protocol = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChBrgVlanIpMcRouteEnable(devNum, vlanId, protocol, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanNASecurEnable
*
* DESCRIPTION:
*      This function enables/disables per VLAN the generation of
*      security breach event for packets with unknown Source MAC addresses.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN Id
*       enable  - GT_TRUE - enable generation of security breach event
*                 GT_FALSE- disable generation of security breach event
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanNASecurEnable

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanNASecurEnable(devNum, vlanId, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanIgmpSnoopingEnable
*
* DESCRIPTION:
*       Enable/Disable IGMP trapping or mirroring to the CPU according to the
*       global setting.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum    - device number
*       vlanId - the VLAN-ID in which IGMP trapping is enabled/disabled.
*       enable - GT_TRUE - enable
*                GT_FALSE - disable
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on on bad device number
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanIgmpSnoopingEnable

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanIgmpSnoopingEnable(devNum, vlanId, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanIgmpSnoopingEnableGet
*
* DESCRIPTION:
*       Gets the status of IGMP trapping or mirroring to the CPU according to the
*       global setting.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       vlanId  - the VLAN-ID in which IGMP trapping is enabled/disabled.
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - enable
*                    GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanIgmpSnoopingEnableGet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgVlanIgmpSnoopingEnableGet(devNum, vlanId, &enable);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*****************************************************************************
* cpssDxChBrgVlanIpCntlToCpuSet
*
* DESCRIPTION:
*       Enable/disable IP control traffic trapping/mirroring to CPU.
*       When set, this enables the following control traffic to be eligible
*       for mirroring/trapping to the CPU:
*        - ARP
*        - IPv6 Neighbor Solicitation
*        - IPv4/v6 Control Protocols Running Over Link-Local Multicast
*        - RIPv1 MAC Broadcast
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN ID
*       ipCntrlType - IP control type
*                     DXCH devices support : CPSS_DXCH_BRG_IP_CTRL_NONE_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E
*                     DXCH2 devices support : like DXCH device ,and also
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV6_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on bad device number or IP control type
*       GT_NOT_SUPPORTED - request is not supported
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanIpCntlToCpuSet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    GT_U16                              vlanId;
    CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT      ipCntrlType;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    ipCntrlType = (CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanIpCntlToCpuSet(devNum, vlanId, ipCntrlType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*****************************************************************************
* cpssDxChBrgVlanIpCntlToCpuGet
*
* DESCRIPTION:
*       Gets IP control traffic trapping/mirroring to CPU status.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN ID
*
* OUTPUTS:
*       ipCntrlTypePtr - IP control type
*                     DXCH devices support : CPSS_DXCH_BRG_IP_CTRL_NONE_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E
*                     DXCH2 and above devices support: like DXCH device ,and also
*                          CPSS_DXCH_BRG_IP_CTRL_IPV4_E ,
*                          CPSS_DXCH_BRG_IP_CTRL_IPV6_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or IP control type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanIpCntlToCpuGet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    GT_U16                              vlanId;
    CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT      ipCntrlType;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgVlanIpCntlToCpuGet(devNum, vlanId, &ipCntrlType);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ipCntrlType);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanIpV6IcmpToCpuEnable
*
* DESCRIPTION:
*       Enable/Disable ICMPv6 trapping or mirroring to
*       the CPU, according to global ICMPv6 message type
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*        devNum      - device number
*        vlanId   - VLAN ID
*        enable   - GT_TRUE - enable
*                   GT_FALSE - disable
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on bad device number or vlanId
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanIpV6IcmpToCpuEnable

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanIpV6IcmpToCpuEnable(devNum, vlanId, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanIpV6IcmpToCpuEnableGet
*
* DESCRIPTION:
*       Gets status of ICMPv6 trapping or mirroring to
*       the CPU, according to global ICMPv6 message type
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*        devNum   - device number
*        vlanId   - VLAN ID
*
* OUTPUTS:
*        enablePtr   - GT_TRUE - enable
*                      GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanIpV6IcmpToCpuEnableGet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgVlanIpV6IcmpToCpuEnableGet(devNum, vlanId, &enable);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanUdpBcPktsToCpuEnable
*
* DESCRIPTION:
*     Enable or disable per Vlan the Broadcast UDP packets Mirror/Trap to the
*     CPU based on their destination UDP port.  Destination UDP port, command
*     (trap or mirror) and CPU code configured by
*     cpssDxChBrgVlanUdpBcPktsToCpuEnable
*
* APPLICABLE DEVICES:  98DX2x5 only
*
* INPUTS:
*       devNum - device number
*       vlanId -  vlan id
*       enable - Enable/disable UDP Trap Mirror.
*                GT_TRUE - enable trapping or mirroring, GT_FALSE - disable.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on bad device number or vlanId
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanUdpBcPktsToCpuEnable

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanUdpBcPktsToCpuEnable(devNum, vlanId, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanUdpBcPktsToCpuEnableGet
*
* DESCRIPTION:
*     Gets per Vlan if the Broadcast UDP packets are Mirror/Trap to the
*     CPU based on their destination UDP port.
*
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum - device number
*       vlanId -  vlan id
*
* OUTPUTS:
*       enablePtr - UDP Trap Mirror status
*                   GT_TRUE - enable trapping or mirroring, GT_FALSE - disable.

*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or vlanId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanUdpBcPktsToCpuEnableGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgVlanUdpBcPktsToCpuEnableGet(devNum, vlanId, &enable);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanIpv6SourceSiteIdSet
*
* DESCRIPTION:
*       Sets a vlan ipv6 site id
*
* APPLICABLE DEVICES:  98DX2x5 only
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - the vlan.
*       siteId          - the site id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on bad device number or vlanId
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanIpv6SourceSiteIdSet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    CPSS_IP_SITE_ID_ENT           siteId;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    siteId = (CPSS_IP_SITE_ID_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanIpv6SourceSiteIdSet(devNum, vlanId, siteId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanIpmBridgingEnable
*
* DESCRIPTION:
*       IPv4/Ipv6 Multicast Bridging Enable
*
* APPLICABLE DEVICES: All DxCh devices
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
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on bad device number or vlanId
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanIpmBridgingEnable

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    CPSS_IP_PROTOCOL_STACK_ENT    ipVer;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    ipVer = (GT_U8)inArgs[2];
    enable = (GT_U8)inArgs[3];

    /* call cpss api function */
    result = cpssDxChBrgVlanIpmBridgingEnable(devNum, vlanId, ipVer, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanIpmBridgingModeSet
*
* DESCRIPTION:
*       Sets the IPM bridging mode of Vlan. Relevant when IPM Bridging enabled.
*
* APPLICABLE DEVICES: All DxCh devices
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
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on bad device number or vlanId
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanIpmBridgingModeSet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    CPSS_IP_PROTOCOL_STACK_ENT    ipVer;
    CPSS_BRG_IPM_MODE_ENT         ipmMode;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    ipVer = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    ipmMode = (CPSS_BRG_IPM_MODE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChBrgVlanIpmBridgingModeSet(devNum, vlanId, ipVer, ipmMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanIngressMirrorEnable
*
* DESCRIPTION:
*       Enable/Disable ingress mirroring of packets to the Ingress Analyzer
*       port for packets assigned the given VLAN-ID.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum  - device id
*       vlanId  - VLAN ID which ingress VLAN mirroring is enabled/disabled..
*       enable  - GT_TRUE - Ingress mirrored traffic assigned to
*                          this Vlan to the analyzer port
*                 GT_FALSE - Don't ingress mirrored traffic assigned
*                          to this Vlan to the analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanIngressMirrorEnable

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_BOOL                         enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanIngressMirrorEnable(devNum, vlanId, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanPortAccFrameTypeSet
*
* DESCRIPTION:
*       Set port access frame type.
*       There are three types of port configuration:
*        - Admit only Vlan tagged frames
*        - Admit only tagged and priority tagged,
*        - Admit all frames.
*
* APPLICABLE DEVICES: 98DX2x5 devices only
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
*       GT_FAIL     - on error.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortAccFrameTypeSet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    GT_U8                               portNum;
    CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     frameType;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    frameType = (CPSS_PORT_ACCEPT_FRAME_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortAccFrameTypeSet(devNum, portNum, frameType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanPortAccFrameTypeGet
*
* DESCRIPTION:
*       Get port access frame type.
*       There are three types of port configuration:
*        - Admit only Vlan tagged frames
*        - Admit only tagged and priority tagged,
*        - Admit all frames.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*
* OUTPUTS:
*       frameTypePtr -
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
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortAccFrameTypeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    GT_U8                               portNum;
    CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     frameType;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortAccFrameTypeGet(devNum, portNum, &frameType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", frameType);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanMruProfileIdxSet
*
* DESCRIPTION:
*     Set Maximum Receive Unit MRU profile index for a VLAN.
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*     Value of MRU for profile is set by gvlnSetMruProfileValue.
*
* APPLICABLE DEVICES: 98DX2x5 devices only
*
* INPUTS:
*       devNum   - device number
*       vlanId   - vlan id
*       mruIndex - MRU profile index
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error
*       GT_BAD_PARAM       - on wrong input parameters
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanMruProfileIdxSet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_U32                        mruIndex;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    mruIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanMruProfileIdxSet(devNum, vlanId, mruIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanMruProfileValueSet
*
* DESCRIPTION:
*     Set MRU value for a VLAN MRU profile.
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*     gvlnSetMruProfileIdx set index of profile for a specific VLAN
*
* APPLICABLE DEVICES: 98DX2x5 devices only
*
* INPUTS:
*       devNum   - device number
*       mruIndex - MRU profile index
*       mruValue - MRU value in bytes
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error
*       GT_BAD_PARAM       - on wrong input parameters
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanMruProfileValueSet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U32                        mruIndex;
    GT_U32                        mruValue;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mruIndex = (GT_U32)inArgs[1];
    mruValue = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanMruProfileValueSet(devNum, mruIndex, mruValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanNaToCpuEnable
*
* DESCRIPTION:
*     Enable/Disable New Address (NA) Message Sending to CPU per VLAN  .
*     To send NA to CPU both VLAN and port must be set to send NA to CPU.
*
* APPLICABLE DEVICES: 98DX2x5 devices only
*
* INPUTS:
*       devNum   - device number
*       vlanId   - vlan id
*       enable   - GT_TRUE  - enable New Address Message Sending to CPU
*                  GT_FALSE - disable New Address Message Sending to CPU
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error
*       GT_BAD_PARAM       - on wrong input parameters
*       GT_NOT_SUPPORTED   - This request is not supportted.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanNaToCpuEnable

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_U16                        vlanId;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanNaToCpuEnable(devNum, vlanId, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanBridgingModeSet
*
* DESCRIPTION:
*     Set bridging mode either VLAN-aware (802.1Q) or VLAN-unaware (802.1D) mode
*     The device supports a VLAN-unaware mode for 802.1D bridging.
*     When this mode is enabled:
*      - In VLAN-unaware mode, the device does not perform any packet
*        modifications. Packets are always transmitted as-received, without any
*        modification (i.e., packets received tagged are transmitted tagged;
*        packets received untagged are transmitted untagged).
*      - Packets are implicitly assigned with VLAN-ID 1, regardless of
*        VLAN-assignment mechanisms.
*      - Packets are implicitly assigned a VIDX Multicast group index 0xFFF,
*        indicating that the packet flood domain is according to the VLAN-in
*        this case VLAN 1. Registered Multicast is not supported in this mode.
*      All other features are operational in this mode, including internal
*      packet QoS, trapping, filtering, policy, etc.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       brgMode  - bridge mode: IEEE 802.1Q bridge or IEEE 802.1D bridge
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error
*       GT_BAD_PARAM       - on wrong input parameters
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanBridgingModeSet

(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    CPSS_BRG_MODE_ENT             brgMode;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    brgMode = (CPSS_BRG_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgVlanBridgingModeSet(devNum, brgMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanVrfIdSet
*
* DESCRIPTION:
*       Sets vlan Virtual Router ID
*
* APPLICABLE DEVICES:  DXCH3 devices only
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - the vlan id.
*       vrfId           - the virtual router id (0..4095).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on bad device number or vlanId
*       GT_OUT_OF_RANGE - when vrfId is out of range
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanVrfIdSet
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
    GT_U32 vrfId;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    vrfId = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanVrfIdSet(devNum, vlanId, vrfId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortTranslationEnableSet
*
* DESCRIPTION:
*       Enable/Disable Vlan Translation per ingress or egress port.
*
* APPLICABLE DEVICES:  DXCH3 devices only
*
* INPUTS:
*       devNum          - the device number
*       port            - port number.
*       direction       - ingress or egress
*       enable          - enable/disable ingress/egress Vlan Translation.
*                         GT_TRUE: enable Vlan translation
*                         GT_FALSE: disable Vlan translation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on bad device number or port
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortTranslationEnableSet
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
    CPSS_DIRECTION_ENT direction;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    direction = (CPSS_DIRECTION_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortTranslationEnableSet(devNum, port, direction, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortTranslationEnableGet
*
* DESCRIPTION:
*       Get the status of Vlan Translation (Enable/Disable) per ingress or
*       egress port.
*
* APPLICABLE DEVICES:  DXCH3 devices only
*
* INPUTS:
*       devNum          - the device number
*       direction       - ingress or egress
*       port            - port number.
*
* OUTPUTS:
*       enablePtr       - (pointer to) ingress/egress Vlan Translation status
*                         GT_TRUE: enable Vlan translation
*                         GT_FALSE: disable Vlan translation
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on bad device number or port
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortTranslationEnableGet
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
    CPSS_DIRECTION_ENT direction;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    direction = (CPSS_DIRECTION_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortTranslationEnableGet(devNum, port, direction, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanTranslationEntryWrite
*
* DESCRIPTION:
*       Write an entry into Vlan Translation Table Ingress or Egress
*       This mapping avoids collisions between a VLAN-ID
*       used for a logical interface and a VLAN-ID used by the local network.
*
* APPLICABLE DEVICES:  DXCH3 devices only
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id, used as index in the Translation Table, 0..4095.
*       direction       - ingress or egress
*       transVlanId     - Translated Vlan ID, use as value in the
*                         Translation Table, 0..4095.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on bad device number or vlanId
*       GT_OUT_OF_RANGE - when transVlanId is out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanTranslationEntryWrite
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
    CPSS_DIRECTION_ENT direction;
    GT_U16 transVlanId;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    direction = (CPSS_DIRECTION_ENT)inArgs[2];
    transVlanId = (GT_U16)inArgs[3];

    /* call cpss api function */
    result = cpssDxChBrgVlanTranslationEntryWrite(devNum, vlanId, direction, transVlanId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanTranslationEntryRead
*
* DESCRIPTION:
*       Read an entry from Vlan Translation Table, Ingress or Egress.
*
* APPLICABLE DEVICES:  DXCH3 devices only
*
* INPUTS:
*       devNum          - the device number
*       vlanId          - VLAN id used as index in the Translation Table, 0..4095.
*       direction       - ingress or egress
*
* OUTPUTS:
*       transVlanIdPtr  - (pointer to) Translated Vlan ID, used as value in the
*                         Translation Table.
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on bad device number or vlanId
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanTranslationEntryRead
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
    CPSS_DIRECTION_ENT direction;
    GT_U16 transVlanId;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    direction = (CPSS_DIRECTION_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanTranslationEntryRead(devNum, vlanId, direction, &transVlanId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", transVlanId);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChBrgVlanValidCheckEnableSet
*
* DESCRIPTION:
*       Enable/disable check of "Valid" field in the VLAN entry
*       When check enable and VLAN entry is not valid then packets are dropped.
*       When check disable and VLAN entry is not valid then packets are
*       not dropped and processed like in case of valid VLAN.
*
* APPLICABLE DEVICES:  All DxCh2 and above
*
* INPUTS:
*       devNum    - physical device number
*       enable    - GT_TRUE  - Enable check of "Valid" field in the VLAN entry
*                   GT_FALSE - Disable check of "Valid" field in the VLAN entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanValidCheckEnableSet
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
    result = cpssDxChBrgVlanValidCheckEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanValidCheckEnableGet
*
* DESCRIPTION:
*       Get check status  of "Valid" field in the VLAN entry
*       When check enable and VLAN entry is not valid then packets are dropped.
*       When check disable and VLAN entry is not valid then packets are
*       not dropped and processed like in case of valid VLAN.
*
* APPLICABLE DEVICES:  All DxCh2 and above
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       enablePtr - pointer to check status  of "Valid" field in the VLAN entry.
*                 - GT_TRUE  - Enable check of "Valid" field in the VLAN entry
*                   GT_FALSE - Disable check of "Valid" field in the VLAN entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - failed to read from hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanValidCheckEnableGet
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
    result = cpssDxChBrgVlanValidCheckEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanTpidEntrySet
*
* DESCRIPTION:
*       Function sets TPID (Tag Protocol ID) table entry.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/egress direction
*       entryIndex  - entry index for TPID table (0-7)
*       etherType   - Tag Protocol ID value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, entryIndex, direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanTpidEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    CPSS_DIRECTION_ENT  direction;
    GT_U32              entryIndex;
    GT_U16              etherType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_DIRECTION_ENT)inArgs[1];
    entryIndex = (GT_U32)inArgs[2];
    etherType = (GT_U16)inArgs[3];

    /* call cpss api function */
    result = cpssDxChBrgVlanTpidEntrySet(devNum, direction, entryIndex, etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanTpidEntryGet
*
* DESCRIPTION:
*       Function gets TPID (Tag Protocol ID) table entry.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum      - device number
*       direction   - ingress/ egress direction
*       entryIndex  - entry index for TPID table (0-7)
*
* OUTPUTS:
*       etherTypePtr - (pointer to) Tag Protocol ID value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, entryIndex, direction
*       GT_BAD_PTR               - etherTypePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanTpidEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    CPSS_DIRECTION_ENT  direction;
    GT_U32              entryIndex;
    GT_U16              etherTypePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_DIRECTION_ENT)inArgs[1];
    entryIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanTpidEntryGet(devNum, direction, entryIndex, &etherTypePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", etherTypePtr);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortIngressTpidSet
*
* DESCRIPTION:
*       Function sets bitmap of TPID (Tag Protocol ID) table indexes per
*       ingress port.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       ethMode    - TAG0/TAG1 selector
*       tpidBmp    - bitmap represent 8 entries at the TPID Table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that for the ingress direction multiple TPID may be use to
*       identify Tag0 and Tag1. Each bit at the bitmap represent one of
*       the 8 entries at the TPID Table.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortIngressTpidSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    GT_U8               portNum;
    CPSS_ETHER_MODE_ENT ethMode;
    GT_U32              tpidBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ethMode = (CPSS_ETHER_MODE_ENT)inArgs[2];
    tpidBmp = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortIngressTpidSet(devNum, portNum, ethMode, tpidBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortIngressTpidGet
*
* DESCRIPTION:
*       Function gets bitmap of TPID (Tag Protocol ID) table indexes  per
*       ingress port.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       ethMode    - TAG0/TAG1 selector
*
* OUTPUTS:
*       tpidBmpPtr - (pointer to) bitmap represent 8 entries at the TPID Table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - tpidBmpPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Note that for the ingress direction multiple TPID may be use to
*       identify Tag0 and Tag1. Each bit at the bitmap represent one of
*       the 8 entries at the TPID Table.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortIngressTpidGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    GT_U8               portNum;
    CPSS_ETHER_MODE_ENT ethMode;
    GT_U32              tpidBmpPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ethMode = (CPSS_ETHER_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortIngressTpidGet(devNum, portNum, ethMode, &tpidBmpPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", tpidBmpPtr);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortEgressTpidSet
*
* DESCRIPTION:
*       Function sets index of TPID (Tag protocol ID) table per egress port.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number
*       ethMode        - TAG0/TAG1 selector
*       tpidEntryIndex - TPID table entry index (0-7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - illegal tpidEntryIndex (not 0-7)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortEgressTpidSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    GT_U8               portNum;
    CPSS_ETHER_MODE_ENT ethMode;
    GT_U32              tpidEntryIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ethMode = (CPSS_ETHER_MODE_ENT)inArgs[2];
    tpidEntryIndex = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortEgressTpidSet(devNum, portNum, ethMode, tpidEntryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanPortEgressTpidGet
*
* DESCRIPTION:
*       Function gets index of TPID (Tag Protocol ID) table per egress port.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       ethMode    - TAG0/TAG1 selector
*
* OUTPUTS:
*       tpidEntryIndexPtr - (pointer to) TPID table entry index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - tpidEntryIndexPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortEgressTpidGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    GT_U8               portNum;
    CPSS_ETHER_MODE_ENT ethMode;
    GT_U32              tpidEntryIndexPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ethMode = (CPSS_ETHER_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortEgressTpidGet(devNum, portNum, ethMode, &tpidEntryIndexPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", tpidEntryIndexPtr);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanRangeSet
*
* DESCRIPTION:
*       Function configures the valid VLAN Range.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*       vidRange  - VID range for VLAN filtering (0 - 4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - illegal vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanRangeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U16    vidRange;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vidRange = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgVlanRangeSet(devNum, vidRange);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanRangeGet
*
* DESCRIPTION:
*       Function gets the valid VLAN Range.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       vidRangePtr - (pointer to) VID range for VLAN filtering
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - vidRangePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanRangeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U16    vidRangePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgVlanRangeGet(devNum, &vidRangePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vidRangePtr);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanPortIsolationCmdSet
*
* DESCRIPTION:
*       Function sets port isolation command per egress VLAN.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN id
*       cmd     - port isolation command for given VID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or cmd
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanPortIsolationCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;
    GT_U8      devNum;
    GT_U16     vlanId;
    CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT  cmd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    cmd = (CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanPortIsolationCmdSet(devNum, vlanId, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanLocalSwitchingEnableSet
*
* DESCRIPTION:
*       Function sets local switching of Multicast, known and unknown Unicast,
*       and Broadcast traffic per VLAN.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum      - device number
*       vlanId      - VLAN id
*       trafficType - local switching traffic type
*       enable      - Enable/disable of local switching
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or trafficType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. To enable local switching of Multicast, unknown Unicast, and
*       Broadcast traffic, both this field in the VLAN entry and the
*       egress port configuration must be enabled for Multicast local switching
*       in function cpssDxChBrgPortEgressMcastLocalEnable.
*
*       2. To enable  local switching of known Unicast traffic, both this
*       field in the VLAN entry and the ingress port configuration must
*       be enabled for Unicast local switching in function
*       cpssDxChBrgGenUcLocalSwitchingEnable.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanLocalSwitchingEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;
    GT_U8   devNum;
    GT_U16  vlanId;
    CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT  trafficType;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    trafficType = (CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChBrgVlanLocalSwitchingEnableSet(devNum,vlanId, trafficType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanFloodVidxModeSet
*
* DESCRIPTION:
*       Function sets Flood VIDX and Flood VIDX Mode per VLAN.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum        - device number
*       vlanId        - VLAN id
*       floodVidx     - VIDX value (12bit) - VIDX value applied to Broadcast,
*                       Unregistered Multicast and unknown Unicast packets,
*                       depending on the VLAN entry field <Flood VIDX Mode>.
*       floodVidxMode - Flood VIDX Mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, vlanId or floodVidxMode
*       GT_OUT_OF_RANGE          - illegal floodVidx
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanFloodVidxModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    GT_U16                                  vlanId;
    GT_U16                                  floodVidx;
    CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT  floodVidxMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    floodVidx = (GT_U16)inArgs[2];
    floodVidxMode = (CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChBrgVlanFloodVidxModeSet(devNum, vlanId, floodVidx, floodVidxMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*cpssDxChBrgVlanTagCommand Table*/
static GT_U32   tagCmdCnt;

/*******************************************************************************
* cpssDxChBrgVlanTagCommandSet
*
* DESCRIPTION:
*       Biulds and writes tagging commands.
*
* APPLICABLE DEVICES:  All DxChXcat and above devices
*
* INPUTS:
*       devNum         - device number
*       vlanId         - VLAN Id
*       displayMembersOnly - display VLAN members only
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanTagCommandSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;
    GT_U8                          devNum;
    GT_U16                         vlanId;
    GT_BOOL                        displayMembersOnly;
    GT_U8                          portNum;
    CPSS_PORTS_BMP_STC             portsMembersPtr;
    CPSS_PORTS_BMP_STC             portsTaggingPtr;
    CPSS_DXCH_BRG_VLAN_INFO_STC    vlanInfoPtr;
    GT_BOOL                        isValidPtr;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmdPtr;
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT taggingCommand;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    displayMembersOnly = (GT_BOOL)inArgs[2];

    portNum = (GT_U8)inFields[0];
    taggingCommand = (CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT)inFields[1];

    if(portNum >= CPSS_MAX_PORTS_NUM_CNS)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChBrgVlanEntryRead(devNum,
                                      vlanId,
                                      &portsMembersPtr,
                                      &portsTaggingPtr,
                                      &vlanInfoPtr,
                                      &isValidPtr,
                                      &portsTaggingCmdPtr);

    if ((result != GT_OK) || (isValidPtr != GT_TRUE))
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    portsTaggingCmdPtr.portsCmd[portNum] = taggingCommand;

    result = cpssDxChBrgVlanEntryWrite(devNum,
                                       vlanId,
                                       &portsMembersPtr,
                                       &portsTaggingPtr,
                                       &vlanInfoPtr,
                                       &portsTaggingCmdPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanTagCommandGet
*
* DESCRIPTION:
*       Read tagging command per VLAN entry.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum       - device number
*       entryNum     - entry number
*
* OUTPUTS:
*       etherTypePtr - (pointer to) Ether Type or DSAP/SSAP
*       encListPtr   - (pointer to) encapsulation.
*       validEntryPtr - (pointer to) validity bit
*                                   GT_TRUE - the entry is valid
*                                   GT_FALSE - the entry is invalid
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanTagCommandGetFirst
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;
    GT_U8                          devNum;
    GT_U16                         vlanId;
    GT_BOOL                        displayMembersOnly;
    CPSS_PORTS_BMP_STC             portsMembersPtr;
    CPSS_PORTS_BMP_STC             portsTaggingPtr;
    CPSS_DXCH_BRG_VLAN_INFO_STC    vlanInfoPtr;
    GT_BOOL                        isValidPtr;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmdPtr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tagCmdCnt = 0;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    displayMembersOnly = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgVlanEntryRead(devNum,
                                      vlanId,
                                      &portsMembersPtr,
                                      &portsTaggingPtr,
                                      &vlanInfoPtr,
                                      &isValidPtr,
                                      &portsTaggingCmdPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if (isValidPtr == GT_TRUE)
    {
        inFields[0] = tagCmdCnt;
        inFields[1] = portsTaggingCmdPtr.portsCmd[tagCmdCnt];

        /* pack and output table fields */
        if ((displayMembersOnly == GT_TRUE) && (portsMembersPtr.ports[tagCmdCnt] == GT_FALSE))
        {
            /* do nothing*/
        }
        else
        {
            fieldOutput("%d%d", inFields[0], inFields[1]);
        }

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, result, "");
    }

    return CMD_OK;
}


/*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanTagCommandGetNext
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;
    GT_U8                          devNum;
    GT_U16                         vlanId;
    GT_BOOL                        displayMembersOnly;
    CPSS_PORTS_BMP_STC             portsMembersPtr;
    CPSS_PORTS_BMP_STC             portsTaggingPtr;
    CPSS_DXCH_BRG_VLAN_INFO_STC    vlanInfoPtr;
    GT_BOOL                        isValidPtr;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmdPtr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    displayMembersOnly = (GT_BOOL)inArgs[2];

    tagCmdCnt++;

    if (tagCmdCnt >= CPSS_MAX_PORTS_NUM_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChBrgVlanEntryRead(devNum,
                                      vlanId,
                                      &portsMembersPtr,
                                      &portsTaggingPtr,
                                      &vlanInfoPtr,
                                      &isValidPtr,
                                      &portsTaggingCmdPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if (isValidPtr == GT_TRUE)
    {
        inFields[0] = tagCmdCnt;
        inFields[1] = portsTaggingCmdPtr.portsCmd[tagCmdCnt];

        /* pack and output table fields */
        if ((displayMembersOnly == GT_TRUE) && (portsMembersPtr.ports[tagCmdCnt] == GT_FALSE))
        {
            /* do nothing*/
        }
        else
        {
            fieldOutput("%d%d", inFields[0], inFields[1]);
        }

        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, result, "");
    }

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
*
* DESCRIPTION:
*       Set Tag1 removal mode from the egress port Tag State if Tag1 VID
*       is assigned a value of 0.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum        - device number
*       mode          - Vlan Tag1 Removal mode when Tag1 VID=0
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           dev;
    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev    = (GT_U8)inArgs[0];
    mode   = (CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet(dev, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet
*
* DESCRIPTION:
*       Get Tag1 removal mode from the egress port Tag State when Tag1 VID
*       is assigned a value of 0.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       modePtr       - (pointer to) Vlan Tag1 Removal mode when Tag1 VID=0
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           dev;
    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev    = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet(dev, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanForceNewDsaToCpuEnableSet
*
* DESCRIPTION:
*       Enable / Disable preserving the original VLAN tag
*       and VLAN EtherType (or tags),
*       and including the new VLAN Id assignment in the TO_CPU DSA tag
*       for tagged packets that are sent to the CPU.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum        - device number.
*       enable        - GT_TRUE - Double Tag to CPU: Packets sent to the
*                                 CPU whose Vlan Id was modified by the device,
*                                 with a DSA tag that contains the newly
*                                 assigned Vlan Id and the original Vlan tag
*                                 (or tags) that is preserved after the DSA tag.
*                     - GT_FALSE - If the original packet contained a Vlan tag,
*                                  it is removed and the DSA tag contains
*                                  the newly assigned Vlan Id.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanForceNewDsaToCpuEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       dev;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev    = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgVlanForceNewDsaToCpuEnableSet(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanForceNewDsaToCpuEnableGet
*
* DESCRIPTION:
*       Get Force New DSA To Cpu mode for enabling / disabling
*       preserving the original VLAN tag and VLAN EtherType (or tags),
*       and including the new VLAN Id assignment in the TO_CPU DSA tag
*       for tagged packets that are sent to the CPU.

*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum        - device number.
* OUTPUTS:
*       enablePtr     - pointer to Force New DSA To Cpu mode.
*                     - GT_TRUE - Double Tag to CPU: Packets sent to the
*                                 CPU whose Vlan Id was modified by the device,
*                                 with a DSA tag that contains the newly
*                                 assigned Vlan Id and the original Vlan tag
*                                 (or tags) that is preserved after the DSA tag.
*                     - GT_FALSE - If the original packet contained a Vlan tag,
*                                  it is removed and the DSA tag contains
*                                  the newly assigned Vlan Id.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanForceNewDsaToCpuEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       dev;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev    = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgVlanForceNewDsaToCpuEnableGet(dev, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChBrgVlanKeepVlan1EnableSet
*
* DESCRIPTION:
*       Enable/Disable keeping VLAN1 in the packet, for packets received with VLAN1,
*       even-though the tag-state of this egress-port is configured in the
*       VLAN-table to "untagged" or "VLAN0".
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       up              - VLAN tag 0 User Priority
*                         [0..7].
*       enable          - GT_TRUE: If the packet is received with VLAN1 and
*                                  VLAN Tag state is "VLAN0" or "untagged"
*                                  then VLAN1 is not removed from the packet.
*                         GT_FALSE: Tag state assigned by VLAN is preserved.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or up
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanKeepVlan1EnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       dev;
    GT_U8       portNum;
    GT_U8       up;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev    = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    up = (GT_U8)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    CONVERT_DEV_PORT_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanKeepVlan1EnableSet(dev, portNum, up, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanKeepVlan1EnableGet
*
* DESCRIPTION:
*       Get status of keeping VLAN1 in the packet, for packets received with VLAN1.
*       (Enabled/Disabled).
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*       up              - VLAN tag 0 User Priority
*                         [0..7].
*
* OUTPUTS:
*       enablePtr       - GT_TRUE: If the packet is received with VLAN1 and
*                                  VLAN Tag state is "VLAN0" or "untagged"
*                                  then VLAN1 is not removed from the packet.
*                         GT_FALSE: Tag state assigned by VLAN is preserved.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, port or up
*       GT_BAD_PTR               - modePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanKeepVlan1EnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       dev;
    GT_U8       portNum;
    GT_U8       up;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev    = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    up = (GT_U8)inArgs[2];

    CONVERT_DEV_PORT_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChBrgVlanKeepVlan1EnableGet(dev, portNum, up, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

    {"cpssDxChBrgVlanInit",
        &wrCpssDxChBrgVlanInit,
        1, 0},

    {"cpssDxChBrgVlanEntriesRangeSet",
        &wrCpssDxChBrgVlanEntriesRangeWrite,
        1, 31},

    {"cpssDxChBrgVlanEntriesRangeExt1Set",
        &wrCpssDxChBrgVlanEntriesRangeWrite,
        1, 36},

    {"cpssDxChBrgVlanEntrySet",
        &wrCpssDxChBrgVlanEntryWrite,
        1, 30},

    {"cpssDxChBrgVlanEntryGetFirst",
        &wrCpssDxChBrgVlanEntryReadFirst,
        1, 0},

    {"cpssDxChBrgVlanEntryGetNext",
        &wrCpssDxChBrgVlanEntryReadNext,
        1, 0},

    {"cpssDxChBrgVlanEntryExtSet",
        &wrCpssDxChBrgVlanEntryWrite,
        1, 31},

    {"cpssDxChBrgVlanEntryExtGetFirst",
        &wrCpssDxChBrgVlanEntryExtReadFirst,
        1, 0},

    {"cpssDxChBrgVlanEntryExtGetNext",
        &wrCpssDxChBrgVlanEntryExtReadNext,
        1, 0},

    {"cpssDxChBrgVlanEntryExt1Set",
        &wrCpssDxChBrgVlanEntryWrite,
        1, 36},

    {"cpssDxChBrgVlanEntryExt1GetFirst",
        &wrCpssDxChBrgVlanEntryExt1ReadFirst,
        1, 0},

    {"cpssDxChBrgVlanEntryExt1GetNext",
        &wrCpssDxChBrgVlanEntryExt1ReadNext,
        1, 0},

    {"cpssDxChBrgVlanMemberAdd",
        &wrCpssDxChBrgVlanMemberAdd,
        4, 0},

    {"cpssDxChBrgVlanMemberAdd1",
        &wrCpssDxChBrgVlanMemberAdd1,
        5, 0},

    {"cpssDxChBrgVlanPortDelete",
        &wrCpssDxChBrgVlanPortDelete,
        3, 0},

    {"cpssDxChBrgVlanMemberSet",
        &wrCpssDxChBrgVlanMemberSet,
        6, 0},

    {"cpssDxChBrgVlanPortVidGet",
        &wrCpssDxChBrgVlanPortVidGet,
        2, 0},

    {"cpssDxChBrgVlanPortVidSet",
        &wrCpssDxChBrgVlanPortVidSet,
        3, 0},

    {"cpssDxChBrgVlanPortIngFltEnable",
        &wrCpssDxChBrgVlanPortIngFltEnable,
        3, 0},

    {"cpssDxChBrgVlanPortProtoClassVlanEnable",
        &wrCpssDxChBrgVlanPortProtoClassVlanEnable,
        3, 0},

    {"cpssDxChBrgVlanPortProtoClassQosEnable",
        &wrCpssDxChBrgVlanPortProtoClassQosEnable,
        3, 0},

    {"cpssDxChBrgVlanProtoClassSet",
        &wrCpssDxChBrgVlanProtoClassSet,
        1, 5},

    {"cpssDxChBrgVlanProtoClassGetFirst",
        &wrCpssDxChBrgVlanProtoClassGetFirst,
        1, 0},

    {"cpssDxChBrgVlanProtoClassGetNext",
        &wrCpssDxChBrgVlanProtoClassGetNext,
        1, 0},

    {"cpssDxChBrgVlanProtoClassDelete",
        &wrCpssDxChBrgVlanProtoClassInvalidate,
        1, 1},

    {"cpssDxChBrgVlanPortProtoVlanQosSet",
        &wrCpssDxChBrgVlanPortProtoVlanQosSet,
        2, 9},

    {"cpssDxChBrgVlanPortProtoVlanQosGetFirst",
        &wrCpssDxChBrgVlanPortProtoVlanQosGetFirst,
        2, 0},

    {"cpssDxChBrgVlanPortProtoVlanQosGetNext",
        &wrCpssDxChBrgVlanPortProtoVlanQosGetNext,
        2, 0},

    {"cpssDxChBrgVlanPortProtoVlanQosDelete",
        &wrCpssDxChBrgVlanPortProtoVlanQosInvalidate,
        2, 1},

    {"cpssDxChBrgVlanLearningStateSet",
        &wrCpssDxChBrgVlanLearningStateSet,
        3, 0},

    {"cpssDxChBrgVlanIsDevMember",
        &wrCpssDxChBrgVlanIsDevMember,
        2, 0},

    {"cpssDxChBrgVlanToStpIdBind",
        &wrCpssDxChBrgVlanToStpIdBind,
        3, 0},

    {"cpssDxChBrgVlanStpIdGet",
        &wrCpssDxChBrgVlanStpIdGet,
        2, 0},

    {"cpssDxChBrgVlanEtherTypeSet",
        &wrCpssDxChBrgVlanEtherTypeSet,
        4, 0},

    {"cpssDxChBrgVlanTableInvalidate",
        &wrCpssDxChBrgVlanTableInvalidate,
        1, 0},

    {"cpssDxChBrgVlanEntryInvalidate",
        &wrCpssDxChBrgVlanEntryInvalidate,
        2, 0},

    {"cpssDxChBrgVlanMacLsbSet",
        &wrCpssDxChBrgVlanMacLsbSet,
        3, 0},

    {"cpssDxChBrgVlanUnkUnregFilterSet",
        &wrCpssDxChBrgVlanUnkUnregFilterSet,
        4, 0},

    {"cpssDxChBrgVlanForcePvidEnable",
        &wrCpssDxChBrgVlanForcePvidEnable,
        3, 0},

    {"cpssDxChBrgVlanForcePvidEnableGet",
        &wrCpssDxChBrgVlanForcePvidEnableGet,
        2, 0},

    {"cpssDxChBrgVlanPortVidPrecedenceSet",
        &wrCpssDxChBrgVlanPortVidPrecedenceSet,
        3, 0},

    {"cpssDxChBrgVlanPortVidPrecedenceGet",
        &wrCpssDxChBrgVlanPortVidPrecedenceGet,
        2, 0},

    {"cpssDxChBrgVlanIpUcRouteEnable",
        &wrCpssDxChBrgVlanIpUcRouteEnable,
        4, 0},

    {"cpssDxChBrgVlanIpMcRouteEnable",
        &wrCpssDxChBrgVlanIpMcRouteEnable,
        4, 0},

    {"cpssDxChBrgVlanNASecurEnable",
        &wrCpssDxChBrgVlanNASecurEnable,
        3, 0},

    {"cpssDxChBrgVlanIgmpSnoopingEnableGet",
        &wrCpssDxChBrgVlanIgmpSnoopingEnableGet,
        2, 0},

    {"cpssDxChBrgVlanIgmpSnoopingEnable",
        &wrCpssDxChBrgVlanIgmpSnoopingEnable,
        3, 0},

    {"cpssDxChBrgVlanIpCntlToCpuSet",
        &wrCpssDxChBrgVlanIpCntlToCpuSet,
        3, 0},

    {"cpssDxChBrgVlanIpCntlToCpuGet",
        &wrCpssDxChBrgVlanIpCntlToCpuGet,
        2, 0},

    {"cpssDxChBrgVlanIpV6IcmpToCpuEnable",
        &wrCpssDxChBrgVlanIpV6IcmpToCpuEnable,
        3, 0},

    {"cpssDxChBrgVlanIpV6IcmpToCpuEnableGet",
        &wrCpssDxChBrgVlanIpV6IcmpToCpuEnableGet,
        2, 0},

    {"cpssDxChBrgVlanUdpBcPktsToCpuEnable",
        &wrCpssDxChBrgVlanUdpBcPktsToCpuEnable,
        3, 0},

    {"cpssDxChBrgVlanUdpBcPktsToCpuEnableGet",
        &wrCpssDxChBrgVlanUdpBcPktsToCpuEnableGet,
        2, 0},

    {"cpssDxChBrgVlanIpv6SourceSiteIdSet",
        &wrCpssDxChBrgVlanIpv6SourceSiteIdSet,
        3, 0},

    {"cpssDxChBrgVlanMacLsbSet",
        &wrCpssDxChBrgVlanMacLsbSet,
        3, 0},

    {"cpssDxChBrgVlanIpmBridgingEnable",
        &wrCpssDxChBrgVlanIpmBridgingEnable,
        4, 0},

    {"cpssDxChBrgVlanIpmBridgingModeSet",
        &wrCpssDxChBrgVlanIpmBridgingModeSet,
        4, 0},

    {"cpssDxChBrgVlanIngressMirrorEnable",
        &wrCpssDxChBrgVlanIngressMirrorEnable,
        3, 0},

    {"cpssDxChBrgVlanPortAccFrameTypeGet",
        &wrCpssDxChBrgVlanPortAccFrameTypeGet,
        2, 0},

    {"cpssDxChBrgVlanPortAccFrameTypeSet",
        &wrCpssDxChBrgVlanPortAccFrameTypeSet,
        3, 0},

    {"cpssDxChBrgVlanMruProfileIdxSet",
        &wrCpssDxChBrgVlanMruProfileIdxSet,
        3, 0},

    {"cpssDxChBrgVlanMruProfileValueSet",
        &wrCpssDxChBrgVlanMruProfileValueSet,
        3, 0},

    {"cpssDxChBrgVlanNaToCpuEnable",
        &wrCpssDxChBrgVlanNaToCpuEnable,
        3, 0},

    {"cpssDxChBrgVlanBridgingModeSet",
        &wrCpssDxChBrgVlanBridgingModeSet,
        2, 0},

    {"cpssDxChBrgVlanVrfIdSet",
        &wrCpssDxChBrgVlanVrfIdSet,
        3, 0},

    {"cpssDxChBrgVlanPortTranslationEnableSet",
        &wrCpssDxChBrgVlanPortTranslationEnableSet,
        4, 0},

    {"cpssDxChBrgVlanPortTranslationEnableGet",
        &wrCpssDxChBrgVlanPortTranslationEnableGet,
        3, 0},

    {"cpssDxChBrgVlanTranslationEntryWrite",
        &wrCpssDxChBrgVlanTranslationEntryWrite,
        4, 0},

    {"cpssDxChBrgVlanTranslationEntryRead",
        &wrCpssDxChBrgVlanTranslationEntryRead,
        3, 0},
    {"cpssDxChBrgVlanValidCheckEnableSet",
         &wrCpssDxChBrgVlanValidCheckEnableSet,
         2, 0},
    {"cpssDxChBrgVlanValidCheckEnableGet",
         &wrCpssDxChBrgVlanValidCheckEnableGet,
         1, 0},

    {"cpssDxChBrgVlanTpidEntrySet",
         &wrCpssDxChBrgVlanTpidEntrySet,
         4, 0},

    {"cpssDxChBrgVlanTpidEntryGet",
         &wrCpssDxChBrgVlanTpidEntryGet,
         3, 0},

    {"cpssDxChBrgVlanPortIngressTpidSet",
         &wrCpssDxChBrgVlanPortIngressTpidSet,
         4, 0},

    {"cpssDxChBrgVlanPortIngressTpidGet",
         &wrCpssDxChBrgVlanPortIngressTpidGet,
         3, 0},

    {"cpssDxChBrgVlanPortEgressTpidSet",
         &wrCpssDxChBrgVlanPortEgressTpidSet,
         4, 0},

    {"cpssDxChBrgVlanPortEgressTpidGet",
         &wrCpssDxChBrgVlanPortEgressTpidGet,
         3, 0},

    {"cpssDxChBrgVlanRangeSet",
         &wrCpssDxChBrgVlanRangeSet,
         2, 0},

    {"cpssDxChBrgVlanRangeGet",
         &wrCpssDxChBrgVlanRangeGet,
         1, 0},

    {"cpssDxChBrgVlanPortIsolationCmdSet",
         &wrCpssDxChBrgVlanPortIsolationCmdSet,
         3, 0},

    {"cpssDxChBrgVlanLocalSwitchingEnableSet",
         &wrCpssDxChBrgVlanLocalSwitchingEnableSet,
         4, 0},

    {"cpssDxChBrgVlanFloodVidxModeSet",
         &wrCpssDxChBrgVlanFloodVidxModeSet,
         4, 0},

    {"cpssDxChBrgVlanTagCommandSet",
        &wrCpssDxChBrgVlanTagCommandSet,
        3, 2},

    {"cpssDxChBrgVlanTagCommandGetFirst",
        &wrCpssDxChBrgVlanTagCommandGetFirst,
        3, 0},

    {"cpssDxChBrgVlanTagCommandGetNext",
        &wrCpssDxChBrgVlanTagCommandGetNext,
        3, 0},

    {"cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet",
        &wrCpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet,
        2, 0},

    {"cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet",
        &wrCpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet,
        1, 0},

    {"cpssDxChBrgVlanForceNewDsaToCpuEnableSet",
        &wrCpssDxChBrgVlanForceNewDsaToCpuEnableSet,
        2, 0},

    {"cpssDxChBrgVlanForceNewDsaToCpuEnableGet",
        &wrCpssDxChBrgVlanForceNewDsaToCpuEnableGet,
        1, 0},

    {"cpssDxChBrgVlanKeepVlan1EnableSet",
        &wrCpssDxChBrgVlanKeepVlan1EnableSet,
        4, 0},

    {"cpssDxChBrgVlanKeepVlan1EnableGet",
        &wrCpssDxChBrgVlanKeepVlan1EnableGet,
        3, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChBridgeVlan
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
GT_STATUS cmdLibInitCpssDxChBridgeVlan
(
GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


