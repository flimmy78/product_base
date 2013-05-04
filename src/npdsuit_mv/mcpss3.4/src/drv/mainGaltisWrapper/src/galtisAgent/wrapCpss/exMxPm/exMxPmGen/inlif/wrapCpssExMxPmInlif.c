/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmInlif.c
*
* DESCRIPTION:
*       wrappers for cpssExMxPmInlif.c
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
#include <cpss/exMxPm/exMxPmGen/inlif/cpssExMxPmInlif.h>
#include <cpss/exMxPm/exMxPmGen/inlif/cpssExMxPmInlifTypes.h>

/*******************************************************************************
* wrCpssExMxPm2InlifIndexToType
*
* DESCRIPTION:
*       Convert old inlif index to inlif type
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       inlifIndexOld    - Inlif Index (0..65535)
*
* OUTPUTS:
*       inlifTypePtr     - pointer to inlif type port/vlan/external
*       inlifIndexNewPtr - pointer to new inlif index
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong input parameters
*
* COMMENTS:
*       conversion from old inLif setting to new inLif setting:
*
*       OLD SETTING:
*       ============
*       internal inLif table
*       --------------------
*       if inLif index (4096..4159) - inLif ID is per port
*       access the inLif table with index: 4K + port num as index
*
*       if inLif index (0..4095) - inLif ID is per vlan
*       access the inLif table with VLAN ID as index
*
*       external inLif table
*       --------------------
*       if inLif index (4160..65535) - inLif ID is per vlan (TTI)
*       access the inLif table with index: inLif ID - 4K + 64
*
*       NEW SETTING:
*       ============
*       port - access the internal inLif table with index: (0..63)
*
*       vlan - access the internal inLif table with index: (0..4095)
*
*       external - access the external inLif table with index: (4096..65535)
*******************************************************************************/
/*puma 2*/
static CMD_STATUS wrCpssExMxPm2InlifIndexToType
(
IN  GT_U32                       inlifIndexOld,
OUT CPSS_EXMXPM_INLIF_TYPE_ENT  *inlifTypePtr,
OUT GT_U32                      *inlifIndexNewPtr
)
{
    if (inlifIndexOld <= 4095)
    {
        *inlifIndexNewPtr = inlifIndexOld;
        *inlifTypePtr = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
    } else if (inlifIndexOld >= 4096 && inlifIndexOld <= 4159)
    {
        *inlifIndexNewPtr = inlifIndexOld - 4096;
        *inlifTypePtr = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    } else if (inlifIndexOld >= 4160 && inlifIndexOld <= 65535)
    {
        *inlifIndexNewPtr = inlifIndexOld;
        *inlifTypePtr = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
    } else
        return GT_BAD_PARAM;


    return GT_OK;
}

/*******************************************************************************
* cpssExMxPmInlifPortModeSet
*
* DESCRIPTION:
*       The function configured the InLIf Lookup mode for the Ingress port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       inlifmode   - InLIF Lookup Mode
*
* OUTPUTS:
*           none.
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
static CMD_STATUS wrCpssExMxPmInlifPortModeSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    CPSS_EXMXPM_INLIF_PORT_MODE_ENT inlifMode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    inlifMode = (CPSS_EXMXPM_INLIF_PORT_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmInlifPortModeSet(devNum, portNum, inlifMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifPortModeGet
*
* DESCRIPTION:
*       Get the InLIf Lookup mode for the Ingress port.
*       Each port is independent and can work in every mode.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       inlifModePtr   - (pointer to)InLIF Lookup Mode
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
static CMD_STATUS wrCpssExMxPmInlifPortModeGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    CPSS_EXMXPM_INLIF_PORT_MODE_ENT inlifMode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmInlifPortModeGet(devNum, portNum, &inlifMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", inlifMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifIntForceVlanModeOnTtSet
*
* DESCRIPTION:
*       Enable/Disable the Force-Vlan-Mode-on-TTI.
*       Enabling it means that all packets that have a hit at the TTI
*       (Tunnel-Termination-Interface) lookup are implicitly assgined
*       a per-VLAN inlif (no matter what is the port mode).
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE = Force: If TTI rule hit, IPCL Configuration
*                       table index - packet VLAN assignment
*                     GT_FALSE = Don't Force: Doesn't effect the existing
*                        Policy Configuration table access logic
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
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifIntForceVlanModeOnTtiSet
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
    result = cpssExMxPmInlifIntForceVlanModeOnTtSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifIntForceVlanModeOnTtGet
*
* DESCRIPTION:
*       Get the Force-Vlan-Mode-on-TTI status.
*       Enable means that all packets that have a hit at the TTI
*       (Tunnel-Termination-Interface) lookup are implicitly assgined
*       a per-VLAN inlif (no matter what is the port mode).
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_TRUE = Force: If TTI rule hit, IPCL Configuration
*                       table index - packet VLAN assignment
*                     GT_FALSE = Don't Force: Doesn't effect the existing
*                        Policy Configuration table access logic
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
static CMD_STATUS wrCpssExMxPmInlifIntForceVlanModeOnTtiGet
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
    result = cpssExMxPmInlifIntForceVlanModeOnTtGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifEntrySet
*
* DESCRIPTION:
*       Set Inlif Entry
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       inlifIndexPtr  - (pointer to) The Inlif Index.
*       inlifEntryPtr  - (pointer to) The Inlif Fields.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       Supported for PORT, VLAN and TTI
*
*******************************************************************************/
/*puma 1 backward compatibility*/
static CMD_STATUS wrCpssExMxPmInlifEntrySet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntry;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifIndex=(GT_U32)inFields[0];

    cmdOsMemSet(&inlifEntry, 0, sizeof(inlifEntry));

    switch (inArgs[4])
    {

        case 0:

            inlifEntry.bridgeEnable=(GT_BOOL)inFields[1];
            inlifEntry.unkSaNotSecurBreachEnable=(GT_BOOL)inFields[2];
            inlifEntry.naStormPreventionEnable=(GT_BOOL)inFields[3];
            inlifEntry.autoLearnEnable=(GT_BOOL)inFields[4];
            inlifEntry.naMessageToCpuEnable=(GT_BOOL)inFields[5];
            inlifEntry.unkSaUcCommand=(CPSS_PACKET_CMD_ENT)inFields[6];
            inlifEntry.unkDaUcCommand=(CPSS_PACKET_CMD_ENT)inFields[7];
            inlifEntry.unregNonIpMcCommand=(CPSS_PACKET_CMD_ENT)inFields[8];
            inlifEntry.unregIpMcCommand=(CPSS_PACKET_CMD_ENT)inFields[9];
            inlifEntry.unregIpv4BcCommand=(CPSS_PACKET_CMD_ENT)inFields[11];
            inlifEntry.unregNonIpv4BcCommand=(CPSS_PACKET_CMD_ENT)inFields[12];
            inlifEntry.ipv4IgmpToCpuEnable=(GT_BOOL)inFields[13];
            inlifEntry.arpBcToCpuEnable=(GT_BOOL)inFields[15];
            inlifEntry.ipv4LinkLocalMcCommandEnable=(GT_BOOL)inFields[16];
            inlifEntry.ipv6LinkLocalMcCommandEnable=(GT_BOOL)inFields[17];
            inlifEntry.arpBcToMeEnable=(GT_BOOL)inFields[19];
            inlifEntry.ripv1MirrorEnable=(GT_BOOL)inFields[20];
            inlifEntry.ipv4UcRouteEnable=(GT_BOOL)inFields[22];
            inlifEntry.ipv4McRouteEnable=(GT_BOOL)inFields[23];
            inlifEntry.ipv6UcRouteEnable=(GT_BOOL)inFields[24];
            inlifEntry.ipv6McRouteEnable=(GT_BOOL)inFields[25];
            inlifEntry.ipv4IcmpRedirectEnable=(GT_BOOL)inFields[26];
            inlifEntry.ipv6IcmpRedirectEnable=(GT_BOOL)inFields[27];
            inlifEntry.bridgeRouterInterfaceEnable=(GT_BOOL)inFields[28];
            inlifEntry.ipv6NeighborSolicitationEnable=(GT_BOOL)inFields[29];
            inlifEntry.ipSecurityProfile=(GT_U32)inFields[31];


            break;
        case 1:

            inlifEntry.bridgeEnable=(GT_BOOL)inFields[1];
            inlifEntry.unkSaNotSecurBreachEnable=(GT_BOOL)inFields[2];
            inlifEntry.naStormPreventionEnable=(GT_BOOL)inFields[3];
            inlifEntry.autoLearnEnable=(GT_BOOL)inFields[4];
            inlifEntry.naMessageToCpuEnable=(GT_BOOL)inFields[5];
            inlifEntry.unkSaUcCommand=(CPSS_PACKET_CMD_ENT)inFields[6];
            inlifEntry.unkDaUcCommand=(CPSS_PACKET_CMD_ENT)inFields[7];
            inlifEntry.unregNonIpMcCommand=(CPSS_PACKET_CMD_ENT)inFields[8];
            inlifEntry.unregIpMcCommand=(CPSS_PACKET_CMD_ENT)inFields[9];
            inlifEntry.unregIpv4BcCommand=(CPSS_PACKET_CMD_ENT)inFields[11];
            inlifEntry.unregNonIpv4BcCommand=(CPSS_PACKET_CMD_ENT)inFields[12];
            inlifEntry.ipv4IgmpToCpuEnable=(GT_BOOL)inFields[13];
            inlifEntry.arpBcToCpuEnable=(GT_BOOL)inFields[15];
            inlifEntry.ipv4LinkLocalMcCommandEnable=(GT_BOOL)inFields[16];
            inlifEntry.ipv6LinkLocalMcCommandEnable=(GT_BOOL)inFields[17];
            inlifEntry.arpBcToMeEnable=(GT_BOOL)inFields[19];
            inlifEntry.ripv1MirrorEnable=(GT_BOOL)inFields[20];
            inlifEntry.ipv4UcRouteEnable=(GT_BOOL)inFields[22];
            inlifEntry.ipv4McRouteEnable=(GT_BOOL)inFields[23];
            inlifEntry.ipv6UcRouteEnable=(GT_BOOL)inFields[24];
            inlifEntry.ipv6McRouteEnable=(GT_BOOL)inFields[25];
            inlifEntry.ipv4IcmpRedirectEnable=(GT_BOOL)inFields[26];
            inlifEntry.ipv6IcmpRedirectEnable=(GT_BOOL)inFields[27];
            inlifEntry.bridgeRouterInterfaceEnable=(GT_BOOL)inFields[28];
            inlifEntry.ipv6NeighborSolicitationEnable=(GT_BOOL)inFields[29];
            inlifEntry.ipSecurityProfile=(GT_U32)inFields[31];


            break;
        case 2:

            inlifEntry.bridgeEnable=(GT_BOOL)inFields[1];
            inlifEntry.ipv4IgmpToCpuEnable=(GT_BOOL)inFields[2];
            inlifEntry.ripv1MirrorEnable=(GT_BOOL)inFields[4];
            inlifEntry.vrfId=(GT_U32)inFields[5];

            break;
        default:
            return GT_BAD_PARAM;
    }

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifEntrySet(devNum, inlifType, inlifIndexNew, &inlifEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifEntrySet
*
* DESCRIPTION:
*       Set Inlif Entry
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       inlifIndex     - Inlif Index (0..65535)
*       inlifEntryPtr  - (pointer to) The Inlif Fields.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for PORT, VLAN and TTI inlif entry settings
*
*       internal inLif table
*       --------------------
*       if inLif index (4096..4159) - inLif ID is per port
*       access the inLif table with index: 4K + port num as index
*
*       if inLif index (0..4095) - inLif ID is per vlan
*       access the inLif table with VLAN ID as index
*
*       external inLif table
*       --------------------
*       if inLif index (4160..65535) - inLif ID is per vlan (TTI)
*       access the inLif table with index: inLif ID - 4K + 64
*
*******************************************************************************/
/*puma 2*/
static CMD_STATUS wrCpssExMxPm2InlifEntrySet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntry;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    inlifIndex=(GT_32)inFields[0];

    /* Bridge Section */

    inlifEntry.bridgeEnable=(GT_BOOL)inFields[1];
    inlifEntry.autoLearnEnable=(GT_BOOL)inFields[2];
    inlifEntry.naMessageToCpuEnable=(GT_BOOL)inFields[3];
    inlifEntry.naStormPreventionEnable=(GT_BOOL)inFields[4];
    inlifEntry.unkSaUcCommand=(CPSS_PACKET_CMD_ENT)inFields[5];
    inlifEntry.unkDaUcCommand=(CPSS_PACKET_CMD_ENT)inFields[6];
    inlifEntry.unkSaNotSecurBreachEnable=(GT_BOOL)inFields[7];
    inlifEntry.untaggedMruIndex=(GT_U32)inFields[8];
    inlifEntry.unregNonIpMcCommand=(CPSS_PACKET_CMD_ENT)inFields[9];
    inlifEntry.unregIpMcCommand=(CPSS_PACKET_CMD_ENT)inFields[10];
    inlifEntry.unregIpv4BcCommand=(CPSS_PACKET_CMD_ENT)inFields[11];
    inlifEntry.unregNonIpv4BcCommand=(CPSS_PACKET_CMD_ENT)inFields[12];

    /* Router Section */

    inlifEntry.ipv4UcRouteEnable=(GT_BOOL)inFields[13];
    inlifEntry.ipv4McRouteEnable=(GT_BOOL)inFields[14];
    inlifEntry.ipv6UcRouteEnable=(GT_BOOL)inFields[15];
    inlifEntry.ipv6McRouteEnable=(GT_BOOL)inFields[16];
    inlifEntry.mplsRouteEnable=(GT_BOOL)inFields[17];
    inlifEntry.vrfId=(GT_U32)inFields[18];

    /* SCT Section */
    inlifEntry.ipv4IcmpRedirectEnable=(GT_BOOL)inFields[19];
    inlifEntry.ipv6IcmpRedirectEnable=(GT_BOOL)inFields[20];
    inlifEntry.bridgeRouterInterfaceEnable=(GT_BOOL)inFields[21];
    inlifEntry.ipSecurityProfile=(GT_U32)inFields[22];
    inlifEntry.ipv4IgmpToCpuEnable=(GT_BOOL)inFields[23];
    inlifEntry.ipv6IcmpToCpuEnable=(GT_BOOL)inFields[24];
    inlifEntry.udpBcRelayEnable=(GT_BOOL)inFields[25];
    inlifEntry.arpBcToCpuEnable=(GT_BOOL)inFields[26];
    inlifEntry.arpBcToMeEnable=(GT_BOOL)inFields[27];
    inlifEntry.ripv1MirrorEnable=(GT_BOOL)inFields[28];
    inlifEntry.ipv4LinkLocalMcCommandEnable=(GT_BOOL)inFields[29];
    inlifEntry.ipv6LinkLocalMcCommandEnable=(GT_BOOL)inFields[30];
    inlifEntry.ipv6NeighborSolicitationEnable=(GT_BOOL)inFields[31];

    /* Misc Section */
    inlifEntry.mirrorToAnalyzerEnable=(GT_BOOL)inFields[32];
    inlifEntry.mirrorToCpuEnable=(GT_BOOL)inFields[33];

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifEntrySet(devNum, inlifType, inlifIndexNew, &inlifEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;


}

static GT_U32                gIndGet;
static GT_U32                maxIndex;
/*******************************************************************************
* cpssExMxPmInlifEntryGet
*
* DESCRIPTION:
*       Get Inlif Entry
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       inlifIndexPtr  - (pointer to) The Inlif Index.
*
* OUTPUTS:
*       inlifEntryPtr  - (pointer to) The Inlif Fields.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       Supported for PORT, VLAN and TTI
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifEntryGetEntry
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntry;
    GT_U8 type;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    type=0;
    devNum = (GT_U8)inArgs[0];

    inlifIndex=gIndGet;

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifEntryGet(devNum, inlifType, inlifIndexNew, &inlifEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    if (gIndGet>maxIndex)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0]=inlifIndex;

/* pack table fields */

    switch (type)
    {

        case 0:

            inFields[1]=inlifEntry.bridgeEnable;
            inFields[2]=inlifEntry.unkSaNotSecurBreachEnable;
            inFields[3]=inlifEntry.naStormPreventionEnable;
            inFields[4]=inlifEntry.autoLearnEnable;
            inFields[5]=inlifEntry.naMessageToCpuEnable;
            inFields[6]=inlifEntry.unkSaUcCommand;
            inFields[7]=inlifEntry.unkDaUcCommand;
            inFields[8]=inlifEntry.unregNonIpMcCommand;
            inFields[9]=inlifEntry.unregIpMcCommand;
            inFields[10]=0;
            inFields[11]=inlifEntry.unregIpv4BcCommand;
            inFields[12]=inlifEntry.unregNonIpv4BcCommand;
            inFields[13]=inlifEntry.ipv4IgmpToCpuEnable;
            inFields[14]=0;
            inFields[15]=inlifEntry.arpBcToCpuEnable;
            inFields[16]=inlifEntry.ipv4LinkLocalMcCommandEnable;
            inFields[17]=inlifEntry.ipv6LinkLocalMcCommandEnable;
            inFields[18]=0;
            inFields[19]=inlifEntry.arpBcToMeEnable;
            inFields[20]=inlifEntry.ripv1MirrorEnable;
            inFields[21]=0;
            inFields[22]=inlifEntry.ipv4UcRouteEnable;
            inFields[23]=inlifEntry.ipv4McRouteEnable;
            inFields[24]=inlifEntry.ipv6UcRouteEnable;
            inFields[25]=inlifEntry.ipv6McRouteEnable;
            inFields[26]=inlifEntry.ipv4IcmpRedirectEnable;
            inFields[27]=inlifEntry.ipv6IcmpRedirectEnable;
            inFields[28]=inlifEntry.bridgeRouterInterfaceEnable;
            inFields[29]=inlifEntry.ipv6NeighborSolicitationEnable;
            inFields[30]=0;
            inFields[31]=inlifEntry.ipSecurityProfile;


            break;
        case 1:

            inFields[1]=inlifEntry.bridgeEnable;
            inFields[2]=inlifEntry.unkSaNotSecurBreachEnable;
            inFields[3]=inlifEntry.naStormPreventionEnable;
            inFields[4]=inlifEntry.autoLearnEnable;
            inFields[5]=inlifEntry.naMessageToCpuEnable;
            inFields[6]=inlifEntry.unkSaUcCommand;
            inFields[7]=inlifEntry.unkDaUcCommand;
            inFields[8]=inlifEntry.unregNonIpMcCommand;
            inFields[9]=inlifEntry.unregIpMcCommand;
            inFields[10]=0;
            inFields[11]=inlifEntry.unregIpv4BcCommand;
            inFields[12]=inlifEntry.unregNonIpv4BcCommand;
            inFields[13]=inlifEntry.ipv4IgmpToCpuEnable;
            inFields[14]=0;
            inFields[15]=inlifEntry.arpBcToCpuEnable;
            inFields[16]=inlifEntry.ipv4LinkLocalMcCommandEnable;
            inFields[17]=inlifEntry.ipv6LinkLocalMcCommandEnable;
            inFields[18]=0;
            inFields[19]=inlifEntry.arpBcToMeEnable;
            inFields[20]=inlifEntry.ripv1MirrorEnable;
            inFields[21]=0;
            inFields[22]=inlifEntry.ipv4UcRouteEnable;
            inFields[23]=inlifEntry.ipv4McRouteEnable;
            inFields[24]=inlifEntry.ipv6UcRouteEnable;
            inFields[25]=inlifEntry.ipv6McRouteEnable;
            inFields[26]=inlifEntry.ipv4IcmpRedirectEnable;
            inFields[27]=inlifEntry.ipv6IcmpRedirectEnable;
            inFields[28]=inlifEntry.bridgeRouterInterfaceEnable;
            inFields[29]=inlifEntry.ipv6NeighborSolicitationEnable;
            inFields[30]=0;
            inFields[31]=inlifEntry.ipSecurityProfile;


            break;


        case 2:

            inFields[1]=inlifEntry.bridgeEnable;
            inFields[2]=inlifEntry.ipv4IgmpToCpuEnable;
            inFields[3]=0;
            inFields[4]=inlifEntry.ripv1MirrorEnable;
            inFields[5]=inlifEntry.vrfId;

            break;
        default:
            return GT_BAD_PARAM;
    }

/*  output table fields */
    switch (type)
    {

        case 0:
        case 1:

            fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],
                        inFields[1],  inFields[2],inFields[3],  inFields[4],inFields[5],  inFields[6],
                        inFields[7],  inFields[8],inFields[9],  inFields[10],inFields[11],  inFields[12],
                        inFields[13],  inFields[14],inFields[15],  inFields[16],inFields[17],  inFields[18],
                        inFields[19],  inFields[20],inFields[21],  inFields[22],inFields[23],  inFields[24],
                        inFields[25],  inFields[26],inFields[27],  inFields[28],inFields[29],  inFields[30],
                        inFields[31]);

            break;


        case 2:

            fieldOutput("%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],inFields[3],
                        inFields[4],  inFields[5]);

            break;
        default:
            return GT_BAD_PARAM;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%f", 0);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmInlifEntryGet
*
* DESCRIPTION:
*       Get Inlif Entry
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       inlifIndexPtr  - (pointer to) The Inlif Index.
*
* OUTPUTS:
*       inlifEntryPtr  - (pointer to) The Inlif Fields.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       Supported for PORT, VLAN and TTI
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifEntryGetFirst
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gIndGet =(GT_U32) inArgs[1]; /*get min and max entry*/
    maxIndex=(GT_U32)inArgs[2];
    return wrCpssExMxPmInlifEntryGetEntry(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* cpssExMxPmInlifEntryGet
*
* DESCRIPTION:
*       Get Inlif Entry
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       inlifIndexPtr  - (pointer to) The Inlif Index.
*
* OUTPUTS:
*       inlifEntryPtr  - (pointer to) The Inlif Fields.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       Supported for PORT, VLAN and TTI
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifEntryGetNext
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gIndGet++;/* go to next index */
    return wrCpssExMxPmInlifEntryGetEntry(inArgs,inFields,numFields,outArgs);
}

static GT_U32                currentInlifIndex;
static GT_U32                maxInlifIndex;
/*******************************************************************************
* cpssExMxPmInlifEntryGet
*
* DESCRIPTION:
*       Get Inlif Entry
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       inlifIndex     - Inlif Index (0..65535)
*
* OUTPUTS:
*       inlifEntryPtr  - (pointer to) The Inlif Fields.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
       Supported for PORT, VLAN and TTI inlif entry settings
*
*       internal inLif table
*       --------------------
*       if inLif index (4096..4159) - inLif ID is per port
*       access the inLif table with index: 4K + port num as index
*
*       if inLif index (0..4095) - inLif ID is per vlan
*       access the inLif table with VLAN ID as index
*
*       external inLif table
*       --------------------
*       if inLif index (4160..65535) - inLif ID is per vlan (TTI)
*       access the inLif table with index: inLif ID - 4K + 64
*
*******************************************************************************/
/*puma 2*/
static CMD_STATUS wrCpssExMxPm2InlifEntryGetEntry
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    CPSS_EXMXPM_INLIF_ENTRY_STC inlifEntry;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */


    devNum = (GT_U8)inArgs[0];

    inlifIndex=currentInlifIndex;

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifEntryGet(devNum, inlifType, inlifIndexNew, &inlifEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    if (currentInlifIndex>maxInlifIndex)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0]=inlifIndex;

    /* Bridge Section */

    inFields[1]=inlifEntry.bridgeEnable;
    inFields[2]=inlifEntry.autoLearnEnable;
    inFields[3]=inlifEntry.naMessageToCpuEnable;
    inFields[4]=inlifEntry.naStormPreventionEnable;
    inFields[5]=inlifEntry.unkSaUcCommand;
    inFields[6]=inlifEntry.unkDaUcCommand;
    inFields[7]=inlifEntry.unkSaNotSecurBreachEnable;
    inFields[8]=inlifEntry.untaggedMruIndex;
    inFields[9]=inlifEntry.unregNonIpMcCommand;
    inFields[10]=inlifEntry.unregIpMcCommand;
    inFields[11]=inlifEntry.unregIpv4BcCommand;
    inFields[12]=inlifEntry.unregNonIpv4BcCommand;

    /* Router Section */

    inFields[13]=inlifEntry.ipv4UcRouteEnable;
    inFields[14]=inlifEntry.ipv4McRouteEnable;
    inFields[15]=inlifEntry.ipv6UcRouteEnable;
    inFields[16]=inlifEntry.ipv6McRouteEnable;
    inFields[17]=inlifEntry.mplsRouteEnable;
    inFields[18]=inlifEntry.vrfId;

    /* SCT Section */
    inFields[19]=inlifEntry.ipv4IcmpRedirectEnable;
    inFields[20]=inlifEntry.ipv6IcmpRedirectEnable;
    inFields[21]=inlifEntry.bridgeRouterInterfaceEnable;
    inFields[22]=inlifEntry.ipSecurityProfile;
    inFields[23]=inlifEntry.ipv4IgmpToCpuEnable;
    inFields[24]=inlifEntry.ipv6IcmpToCpuEnable;
    inFields[25]=inlifEntry.udpBcRelayEnable;
    inFields[26]=inlifEntry.arpBcToCpuEnable;
    inFields[27]=inlifEntry.arpBcToMeEnable;
    inFields[28]=inlifEntry.ripv1MirrorEnable;
    inFields[29]=inlifEntry.ipv4LinkLocalMcCommandEnable;
    inFields[30]=inlifEntry.ipv6LinkLocalMcCommandEnable;
    inFields[31]=inlifEntry.ipv6NeighborSolicitationEnable;

    /* Misc Section */
    inFields[32]=inlifEntry.mirrorToAnalyzerEnable;
    inFields[33]=inlifEntry.mirrorToCpuEnable;


/* pack table fields */


    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],
                inFields[1],  inFields[2],inFields[3],  inFields[4],inFields[5],  inFields[6],
                inFields[7],  inFields[8],inFields[9],  inFields[10],inFields[11],  inFields[12],
                inFields[13],  inFields[14],inFields[15],  inFields[16],inFields[17],  inFields[18],
                inFields[19],  inFields[20],inFields[21],  inFields[22],inFields[23],  inFields[24],
                inFields[25],  inFields[26],inFields[27],  inFields[28],inFields[29],  inFields[30],
                inFields[31], inFields[32], inFields[33]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/************************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifEntryGetFirst
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    currentInlifIndex =(GT_U32) inArgs[1]; /*get min and max entry*/
    maxInlifIndex=(GT_U32)inArgs[2];
    return wrCpssExMxPm2InlifEntryGetEntry(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************/

static CMD_STATUS wrCpssExMxPm2InlifEntryGetNext
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    currentInlifIndex++;/* go to next index */
    return wrCpssExMxPm2InlifEntryGetEntry(inArgs,inFields,numFields,outArgs);
}


/*******************************************************************************
* cpssExMxPmInlifBridgeEnableSet
*
* DESCRIPTION:
*       This function Enable/Disable the bridge at the specified Inlif
*       When disabling this, incoming packets would bypass the ingress bridge engine.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifIndexPtr - (pointer to) The Inlif Index.
*       enable        - Enable/disable bridge.
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
*       Supported for PORT, VLAN and TTI
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifBridgeEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_BOOL enable;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifIndex=(GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifBridgeEnableSet(devNum, inlifType, inlifIndexNew, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifUnkSaNotSecurBreachEnableSet
*
* DESCRIPTION:
*       Set the Inlif's unknown-source-address-isn't-security event state
*       (enable/disable), enabling this feature means that unkown Source
*       MAC address at the FDB is not considered as security breach and does
*       not generate a security breach event.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifIndexPtr - (pointer to) The Inlif Index.
*       enable        - GT_TRUE = unknown SA isn't security breach.
*                       GT_FALSE = unknown SA is security breach.
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
*       Supported for PORT and VLAN
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifUnkSaNotSecurBreachEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifIndex=(GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(devNum, inlifType, inlifIndexNew, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifNaStormPreventionEnableSet
*
* DESCRIPTION:
*      Set the Inlif's NA storm prevention to CPU state (enable/disable),
*      enabling this feature would prevent the PP to repeat sending same NA
*      message when packets are storming the PP from same source address.
*      Only relevant in controlled address learning mode (Auto Learn is Disabled).
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number

*       enable      - GT_TRUE = NA storm prevention enabled.
*                     GT_FALSE = NA storm prevention disabled.
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
*       Supported for PORT and VLAN
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifNaStormPreventionEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifIndex=(GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifNaStormPreventionEnableSet(devNum, inlifType, inlifIndexNew, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifAutoLearnEnableSet
*
* DESCRIPTION:
*       Disable/Enable Automatic Learning.
*       When enabled, the device learns the new source MAC Addresses received
*       on this interface automatically and, according to the setting for <NA
*       Message To Cpu Enable>, sends or does not send NA messages to the
*       CPU.
*       When disabled the device does not learn the new source MAC Addresses
*       received on this interface automatically.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifIndexPtr - (pointer to) The Inlif Index.
*       enable        - GT_TRUE = auto-learning enabled.
*                       GT_FALSE = auto-learning disabled.
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
*       Supported for PORT and VLAN
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifAutoLearnEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifIndex=(GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifAutoLearnEnableSet(devNum, inlifType, inlifIndexNew, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifNaToCpuEnableSet
*
* DESCRIPTION:
*       Set the Inlif's NA messages to CPU state,
*       when new/relocated SA MAC addresses are encountered.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifIndexPtr - (pointer to) The Inlif Index.
*       enable        - GT_TRUE:Enable Sending New Source MAC address
*                               message to the CPU
*                       GT_FALSE:Disable Sending New Source MAC address
*                                message to the CPU
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
*       Supported for PORT and VLAN
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifNaToCpuEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifIndex=(GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifNaToCpuEnableSet(devNum, inlifType, inlifIndexNew, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifUnkUnregAddrCmdSet
*
* DESCRIPTION:
*       This function set the Inlif's unknown/unregistered addresses command
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifIndexPtr - (pointer to) The Inlif Index.
*       addrType      - Supported unkown/unreg addresses types
*       cmd           - Flood:Flood the packet according to its VLAN assignment.
*                       Mirror:Mirror the packet to the CPU with specific CPU
*                             code for Unknown Unicast.
*                       Trap: Trap the packet to the CPU with specific CPU
*                             code for Unknown Unicast.
*                       Hard Drop: Hard Drop the packet.
*                       Soft Drop: Soft Drop the packet.
*
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
*       Supported for PORT and VLAN
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifUnkUnregAddrCmdSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    CPSS_EXMXPM_INLIF_UNK_UNREG_ADDR_ENT addrType;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    inlifIndex=(GT_U32)inArgs[2];
    addrType=(CPSS_EXMXPM_INLIF_UNK_UNREG_ADDR_ENT)inArgs[3];
    cmd=(CPSS_PACKET_CMD_ENT)inArgs[4];

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifUnkUnregAddrCmdSet(devNum, inlifType, inlifIndexNew, addrType, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;

}

/*******************************************************************************
* cpssExMxPmInlifControlTrafficEnableSet
*
* DESCRIPTION:
*      Set the Inlif's relevant contol traffic state (enabled/disabled),
*      enabling specific control traffic means that it would be either
*      MIRROR/TRAPed to the CPU.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifIndexPtr - (pointer to) The Inlif Index.
*       controlTraffic- Inlif Control packets type
*       enable        - GT_TRUE = control traffic is enabled
*                       (PP would act according to specific commands at the SCT)
*                       GT_FALSE = control traffic is handled as any other packets.
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
*       Supported for PORT and VLAN
*       For CPSS_EXMXPM_INLIF_CTRL_IP_V4_IGMP_E,
*           CPSS_EXMXPM_INLIF_CTRL_RIP_V1_E,
*           CPSS_EXMXPM_INLIF_CTRL_IP_V6_ICMP_E
*           supported also TTI
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifControlTrafficEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    CPSS_EXMXPM_INLIF_CTRL_TRAFFIC_ENT controlTraffic;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifIndex=(GT_U32)inArgs[2];
    controlTraffic=(CPSS_EXMXPM_INLIF_CTRL_TRAFFIC_ENT)inArgs[3];
    enable = (GT_BOOL)inArgs[4];

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifControlTrafficEnableSet(devNum, inlifType, inlifIndexNew, controlTraffic, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifIpUcRouteEnableSet
*
* DESCRIPTION:
*   Set IPv4/IPv6 Unicast Routing on the Interface
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifIndexPtr - (pointer to) The Inlif Index.
*       ipVer         - IP Version (IPv4 or IPv6)
*       enable        - GT_TRUE = Enable.
*                       GT_FALSE = Disable.
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
*       Supported for PORT and VLAN
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifIpUcRouteEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    CPSS_IP_PROTOCOL_STACK_ENT ipVer;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifIndex=(GT_U32)inArgs[2];
    ipVer=(CPSS_IP_PROTOCOL_STACK_ENT)inArgs[3];
    enable = (GT_BOOL)inArgs[4];

    if ((GT_U32)inArgs[1] == 0) /* port mode -> add offset to port area */
    {
        inlifIndex += 4096;
    }

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifIpUcRouteEnableSet(devNum, inlifType, inlifIndexNew, ipVer, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifIpMcRouteEnableSet
*
* DESCRIPTION:
*   Set IPv4/IPv6 Multicast Routing state on the interface.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifIndexPtr - (pointer to) The Inlif Index.
*       ipVer         - IP Version (IPv4 or IPv6)
*       enable        - GT_TRUE = Enable.
*                       GT_FALSE = Disable.
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
*       Supported for PORT and VLAN
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifIpMcRouteEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    CPSS_IP_PROTOCOL_STACK_ENT ipVer;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    inlifIndex=(GT_U32)inArgs[2];
    ipVer=(CPSS_IP_PROTOCOL_STACK_ENT)inArgs[3];
    enable = (GT_BOOL)inArgs[4];

    if ((GT_U32)inArgs[1] == 0) /* port mode -> add offset to port area */
    {
        inlifIndex += 4096;
    }

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifIpMcRouteEnableSet(devNum, inlifType, inlifIndexNew, ipVer, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPmInlifBridgeRouterInterfaceEnabledSet
*
* DESCRIPTION:
*       Set "Bridged Router Interface" (BRI), state.
*       When enabled unicast IP packets will trigger the router regardless
*       of the MAC DA Entry <DaRoute> bit and "MAC to ME" mechanism
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifIndexPtr - (pointer to) The Inlif Index.
*       enable        - GT_TRUE = Enable.
*                       GT_FALSE = Disable.
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
*       Supported for PORT and VLAN
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifBridgeRouterInterfaceEnabledSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    inlifIndex=(GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(devNum, inlifType, inlifIndexNew, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifIpSecurityProfileSet
*
* DESCRIPTION:
*      Set the Profile Index used to access the router's security table and
*      fetch the SCT control bits.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       inlifIndexPtr   - (pointer to) The Inlif Index.
*       profileIndex    - profile Index value [0..15]
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
*       Supported for PORT and VLAN
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifIpSecurityProfileSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    GT_32 profileIndex;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifIndex=(GT_U32)inArgs[2];
    profileIndex = (GT_32)inArgs[3];

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifIpSecurityProfileSet(devNum, inlifType, inlifIndexNew, profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifVrIdSet
*
* DESCRIPTION:
*       Determines the Virtual Router ID of the packet. <Virtual Router ID> is
*       used by routing engine. This field is relevant for Ethernet interface
*       and IPv4/6-OverX tunnel terminated packets.
*
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       inlifIndexPtr   - (pointer to) The Inlif Index.
*       vpnId           - Virtual Router ID value
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
*       Supported for TTI
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifVrIdSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    GT_U32 vrId;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifIndex=(GT_U32)inArgs[2];
    vrId = (GT_32)inArgs[3];

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifVrfIdSet(devNum, inlifType, inlifIndexNew, vrId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmInlifPortDefaultInlifIdSet
*
* DESCRIPTION:
*       The function configure the port default InLIF-ID
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       inlifId     - InLIF ID (0..16K-1)
*
* OUTPUTS:
*           none.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The port InLIF is used only if the port InLIF mode is Per-Port InLIF
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifPortDefaultInlifIdSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 inlifId;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType; /* dummy */


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    inlifId = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    wrCpssExMxPm2InlifIndexToType(inlifId,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifPortDefaultInlifIdSet(devNum, portNum, inlifIndexNew);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifPortDefaultInlifIdGet
*
* DESCRIPTION:
*       Get the default InLIf ID for the Ingress port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       inlifIdPtr  - (pointer to) InLIF ID (0..16K-1)
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The port InLIF is used only if the port InLIF mode is Per-Port InLIF
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifPortDefaultInlifIdGet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 inlifIndexNew;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmInlifPortDefaultInlifIdGet(devNum, portNum, &inlifIndexNew);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", inlifIndexNew);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmInlifMirrorToAnalyzerEnableSet
*
* DESCRIPTION:
*       If set the packet is mirrored to the Ingress Analyzer port
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifIndex    - Inlif Index.
*       enable        - GT_TRUE = Enable.
*                       GT_FALSE = Disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifMirrorToAnalyzerEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifIndex = (GT_U32)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifMirrorToAnalyzerEnableSet(devNum, inlifType, inlifIndexNew, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmInlifMirrorToCpuEnableSet
*
* DESCRIPTION:
*       If set the packet is mirrored to the CPU
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifIndex    - Inlif Index.
*       enable        - GT_TRUE = Enable.
*                       GT_FALSE = Disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifMirrorToCpuEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifIndex = (GT_U32)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifMirrorToCpuEnableSet(devNum, inlifType, inlifIndexNew, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmInlifMplsRouteEnableSet
*
* DESCRIPTION:
*       Enable\disable MPLS switching on the interface
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifIndex    - Inlif Index.
*       enable        - GT_TRUE = Enable.
*                       GT_FALSE = Disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmInlifMplsRouteEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inlifIndex;
    GT_U32 inlifIndexNew;
    CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifIndex = (GT_U32)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    wrCpssExMxPm2InlifIndexToType(inlifIndex,&inlifType,&inlifIndexNew);

    /* call cpss api function */
    result = cpssExMxPmInlifMplsRouteEnableSet(devNum, inlifType, inlifIndexNew, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/************************************************************************/
/********** the follwing are updates made by Benny Schlesinger **********/
/************************************************************************/

/*******************************************************************************
* cpssExMxPmInlifBridgeEnableSet
*
* DESCRIPTION:
*       This function enable/disable the ingress pipe Bridge engine.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type port/vlan/external
*       inlifIndex    - inlif index in the table
*       enable        - Enable/disable bridge.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifBridgeEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    IN GT_U8                        devNum;
    IN CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType;
    IN GT_U32                       inlifIndex;
    IN GT_BOOL                      enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmInlifBridgeEnableSet(devNum, inlifType, inlifIndex, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifUnkSaNotSecurBreachEnableSet
*
* DESCRIPTION:
*       Set the Inlif's unknown-source-address-isn't-security event state
*       (enable/disable),
*       Disable = Packets with unknown Source MAC Addresses generate
*       security breach events
*       Enable = Packets with unknown Source MAC Address are
*       processed according to the normal Source Address learning rules
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type port/vlan/external
*       inlifIndex    - inlif index in the table
*       enable        - GT_TRUE = unknown SA isn't security breach.
*                       GT_FALSE = unknown SA is security breach.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifUnkSaNotSecurBreachEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    IN GT_U8                        devNum;
    IN CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType;
    IN GT_U32                       inlifIndex;
    IN GT_BOOL                      enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmInlifUnkSaNotSecurBreachEnableSet(devNum, inlifType, inlifIndex, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifNaStormPreventionEnableSet
*
* DESCRIPTION:
*      Set the Inlif's NA storm prevention to CPU state (enable/disable),
*      enabling this feature would prevent the PP to repeat sending same NA
*      message when packets are storming the PP from same source address.
*      Only relevant in controlled address learning mode (Auto Learn is Disabled).
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       inlifType   - inlif type port/vlan/external
*       inlifIndex  - inlif index in the table
*       enable      - GT_TRUE = NA storm prevention enabled.
*                     GT_FALSE = NA storm prevention disabled.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifNaStormPreventionEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)

{
    GT_STATUS                       result;

    IN GT_U8                        devNum;
    IN CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType;
    IN GT_U32                       inlifIndex;
    IN GT_BOOL                      enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum     = (GT_U8)inArgs[0];
    inlifType  = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    enable     = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmInlifNaStormPreventionEnableSet(devNum, inlifType, inlifIndex, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifAutoLearnEnableSet
*
* DESCRIPTION:
*       Set the Automatic Learning (Disable/Enable).
*       When enabled, the device learns the new source MAC Addresses received
*       on this interface automatically and, according to the setting for <NA
*       Message To Cpu Enable>, sends or does not send NA messages to the
*       CPU.
*       When disabled the device does not learn the new source MAC Addresses
*       received on this interface automatically.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type port/vlan/external
*       inlifIndex    - inlif index in the table
*       enable        - GT_TRUE = auto-learning enabled.
*                       GT_FALSE = auto-learning disabled.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifAutoLearnEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    IN GT_U8                        devNum;
    IN CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType;
    IN GT_U32                       inlifIndex;
    IN GT_BOOL                      enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmInlifAutoLearnEnableSet(devNum, inlifType, inlifIndex, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifNaToCpuEnableSet
*
* DESCRIPTION:
*       Set the Inlif's NA messages to CPU state,
*       when new/relocated SA MAC addresses are encountered.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type port/vlan/external
*       inlifIndex    - inlif index in the table
*       enable        - GT_TRUE:Enable Sending New Source MAC address
*                               message to the CPU
*                       GT_FALSE:Disable Sending New Source MAC address
*                                message to the CPU
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifNaToCpuEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    IN GT_U8                        devNum;
    IN CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType;
    IN GT_U32                       inlifIndex;
    IN GT_BOOL                      enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmInlifNaToCpuEnableSet(devNum, inlifType, inlifIndex, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifUnkUnregAddrCmdSet
*
* DESCRIPTION:
*       This function set the Inlif's unknown/unregistered addresses command
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type port/vlan/external
*       inlifIndex    - inlif index in the table
*       addrType      - Supported unkown/unreg addresses types
*       cmd           - Flood:Flood the packet according to its VLAN assignment.
*                       valid options:
*                           CPSS_PACKET_CMD_FORWARD_E
*                           CPSS_PACKET_CMD_BRIDGE_E
*                       Mirror:Mirror the packet to the CPU with specific CPU
*                             code for Unknown Unicast.
*                       Trap: Trap the packet to the CPU with specific CPU
*                             code for Unknown Unicast.
*                       Hard Drop: Hard Drop the packet.
*                       Soft Drop: Soft Drop the packet.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifUnkUnregAddrCmdSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    IN  GT_U8                                   devNum;
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT              inlifType;
    IN  GT_U32                                  inlifIndex;
    IN  CPSS_EXMXPM_INLIF_UNK_UNREG_ADDR_ENT    addrType;
    IN  CPSS_PACKET_CMD_ENT                     cmd;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    addrType = (CPSS_EXMXPM_INLIF_UNK_UNREG_ADDR_ENT)inArgs[3];
    cmd = (CPSS_PACKET_CMD_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmInlifUnkUnregAddrCmdSet(devNum, inlifType, inlifIndex, addrType, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifUntaggedMruIndexSet
*
* DESCRIPTION:
*       Set The Maximum Receive Unit (MRU) profile index assigned
*       to a packet associated with the InLIF
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type port/vlan/external
*       inlifIndex    - inlif index in the table
*       mruIndex      - MRU profile index, 3-bit.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCssExMxPm2InlifUntaggedMruIndexSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    IN  GT_U8                       devNum;
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    IN  GT_U32                      inlifIndex;
    IN  GT_U32                      mruIndex;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    mruIndex = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmInlifUntaggedMruIndexSet(devNum, inlifType, inlifIndex, mruIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifIpUcRouteEnableSet
*
* DESCRIPTION:
*   Set IPv4/IPv6 Unicast Routing on the Interface
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type port/vlan/external
*       inlifIndex    - inlif index in the table
*       ipVer         - IP Version (IPv4 or IPv6)
*       enable        - GT_TRUE = Enable.
*                       GT_FALSE = Disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifIpUcRouteEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    IN  GT_U8                          devNum;
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT     inlifType;
    IN  GT_U32                         inlifIndex;
    IN  CPSS_IP_PROTOCOL_STACK_ENT     ipVer;
    IN  GT_BOOL                        enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    ipVer = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[3];
    enable = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmInlifIpUcRouteEnableSet(devNum, inlifType, inlifIndex, ipVer, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifIpMcRouteEnableSet
*
* DESCRIPTION:
*       Set IPv4/IPv6 Multicast Routing state on the interface.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type port/vlan/external
*       inlifIndex    - inlif index in the table
*       ipVer         - IP Version (IPv4 or IPv6)
*       enable        - GT_TRUE = Enable.
*                       GT_FALSE = Disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifIpMcRouteEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    IN  GT_U8                          devNum;
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT     inlifType;
    IN  GT_U32                         inlifIndex;
    IN  CPSS_IP_PROTOCOL_STACK_ENT     ipVer;
    IN  GT_BOOL                        enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    ipVer = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[3];
    enable = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmInlifIpMcRouteEnableSet(devNum, inlifType, inlifIndex, ipVer, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifMplsRouteEnableSet
*
* DESCRIPTION:
*       Set the MPLS switching on the interface (Enable\disable)
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type port/vlan/external
*       inlifIndex    - inlif index in the table
*       enable        - GT_TRUE = Enable.
*                       GT_FALSE = Disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifMplsRouteEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    IN  GT_U8                       devNum;
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    IN  GT_U32                      inlifIndex;
    IN  GT_BOOL                     enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmInlifMplsRouteEnableSet(devNum, inlifType, inlifIndex, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifVrfIdSet
*
* DESCRIPTION:
*       Determines the Virtual Router ID of the packet.
*
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       inlifType       - inlif type port/vlan/external
*       inlifIndex      - inlif index in the table
*       vrfId           - Virtual Router ID value
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifVrfIdSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    IN  GT_U8                       devNum;
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    IN  GT_U32                      inlifIndex;
    IN  GT_U32                      vrfId;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    vrfId = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmInlifVrfIdSet(devNum, inlifType, inlifIndex, vrfId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifBridgeRouterInterfaceEnabledSet
*
* DESCRIPTION:
*       Set "Bridged Router Interface" (BRI), state.
*       When enabled unicast IP packets will trigger the router regardless
*       of the MAC DA Entry <DaRoute> bit and "MAC to ME" mechanism
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type port/vlan/external
*       inlifIndex    - inlif index in the table
*       enable        - GT_TRUE = Enable.
*                       GT_FALSE = Disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifBridgeRouterInterfaceEnabledSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    IN  GT_U8                       devNum;
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    IN  GT_U32                      inlifIndex;
    IN  GT_BOOL                     enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmInlifBridgeRouterInterfaceEnabledSet(devNum, inlifType, inlifIndex, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifIpSecurityProfileSet
*
* DESCRIPTION:
*      Set the Profile Index used to access the router's security table and
*      fetch the SCT control bits.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       inlifType       - inlif type port/vlan/external
*       inlifIndex      - inlif index in the table
*       profileIndex    - profile Index value [0..15]
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifIpSecurityProfileSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    IN  GT_U8                       devNum;
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    IN  GT_U32                      inlifIndex;
    IN  GT_U32                      profileIndex;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    profileIndex = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmInlifIpSecurityProfileSet(devNum, inlifType, inlifIndex, profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifControlTrafficEnableSet
*
* DESCRIPTION:
*      Set the Inlif's relevant contol traffic state (enabled/disabled),
*      enabling specific control traffic means that it would be either
*      MIRROR/TRAPed to the CPU.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type port/vlan/external
*       inlifIndex    - inlif index in the table
*       controlTraffic- Inlif Control packets type
*       enable        - GT_TRUE = control traffic is enabled
*                       (PP would act according to specific commands at the SCT)
*                       GT_FALSE = control traffic is handled as any other packets.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifControlTrafficEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    IN  GT_U8                               devNum;
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT          inlifType;
    IN  GT_U32                              inlifIndex;
    IN  CPSS_EXMXPM_INLIF_CTRL_TRAFFIC_ENT  controlTraffic;
    IN  GT_BOOL                             enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    controlTraffic = (CPSS_EXMXPM_INLIF_CTRL_TRAFFIC_ENT)inArgs[3];
    enable = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmInlifControlTrafficEnableSet(devNum, inlifType,inlifIndex, controlTraffic, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifMirrorToAnalyzerEnableSet
*
* DESCRIPTION:
*       Enable/Disable mirroring to the Ingress Analyzer port of packets
*       associated with the interface
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type port/vlan/external
*       inlifIndex    - inlif index in the table
*       enable        - GT_TRUE = Enable.
*                       GT_FALSE = Disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifMirrorToAnalyzerEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    IN  GT_U8                       devNum;
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    IN  GT_U32                      inlifIndex;
    IN  GT_BOOL                     enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmInlifMirrorToAnalyzerEnableSet(devNum, inlifType, inlifIndex, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmInlifMirrorToCpuEnableSet
*
* DESCRIPTION:
*       Enable/Disable mirroring to the cpu of packets associated
*       with the interface
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type port/vlan/external
*       inlifIndex    - inlif index in the table
*       enable        - GT_TRUE = Enable.
*                       GT_FALSE = Disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifMirrorToCpuEnableSet
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    IN  GT_U8                       devNum;
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT  inlifType;
    IN  GT_U32                      inlifIndex;
    IN  GT_BOOL                     enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    inlifType = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    inlifIndex = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmInlifMirrorToCpuEnableSet(devNum, inlifType, inlifIndex, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/********** Table: cpssExMxPmInlifEntry_2 **********/
static GT_U32 InlifEntryIndex;
static GT_U32 InlifEntryMax;
/*******************************************************************************
* cpssExMxPm2InlifEntrySet
*
* DESCRIPTION:
*       Set Inlif Entry
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       inlifType      - inlif type port/vlan/external
*       inlifIndex     - (0..63) for inLif type port or
*                        (0..4095) for inLif type vlan or
*                        (4096..65535) for inLif type external
*       inlifEntryPtr  - (pointer to) The Inlif Fields.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for PORT, VLAN and EXTERNAL inlif entry settings
*
*       port - access the internal inLif table with index: (0..63)
*
*       vlan - access the internal inLif table with index: (0..4095)
*
*       external - access the external inLif table with index: (4096..65535)
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2InlifEntrySet_1
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    IN GT_U8                        devNum;
    IN CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType;
    IN GT_U32                       inlifIndex;

    CPSS_EXMXPM_INLIF_ENTRY_STC     inlifEntry;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    inlifType  = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];

    inlifIndex = (GT_U32)inFields[0];

    /*** mapping table fields to 'inlifEntry' structure ***/
    /* Bridge Section */
    inlifEntry.bridgeEnable              =      (GT_BOOL)inFields[1];
    inlifEntry.autoLearnEnable           =      (GT_BOOL)inFields[2];
    inlifEntry.naMessageToCpuEnable      =      (GT_BOOL)inFields[3];
    inlifEntry.naStormPreventionEnable   =      (GT_BOOL)inFields[4];
    inlifEntry.unkSaUcCommand            =      (CPSS_PACKET_CMD_ENT)inFields[5];
    inlifEntry.unkDaUcCommand            =      (CPSS_PACKET_CMD_ENT)inFields[6];
    inlifEntry.unkSaNotSecurBreachEnable =      (GT_BOOL)inFields[7];
    inlifEntry.untaggedMruIndex          =      (GT_U32)inFields[8];
    inlifEntry.unregNonIpMcCommand       =      (CPSS_PACKET_CMD_ENT)inFields[9];
    inlifEntry.unregIpMcCommand          =      (CPSS_PACKET_CMD_ENT)inFields[10];
    inlifEntry.unregIpv4BcCommand        =      (CPSS_PACKET_CMD_ENT)inFields[11];
    inlifEntry.unregNonIpv4BcCommand     =      (CPSS_PACKET_CMD_ENT)inFields[12];

    /* Router Section */
    inlifEntry.ipv4UcRouteEnable = (GT_BOOL)inFields[13];
    inlifEntry.ipv4McRouteEnable = (GT_BOOL)inFields[14];
    inlifEntry.ipv6UcRouteEnable = (GT_BOOL)inFields[15];
    inlifEntry.ipv6McRouteEnable = (GT_BOOL)inFields[16];
    inlifEntry.mplsRouteEnable   = (GT_BOOL)inFields[17];
    inlifEntry.vrfId             = (GT_U32)inFields[18];

    /* SCT Section */
    inlifEntry.ipv4IcmpRedirectEnable         =   (GT_BOOL)inFields[19];
    inlifEntry.ipv6IcmpRedirectEnable         =   (GT_BOOL)inFields[20];
    inlifEntry.bridgeRouterInterfaceEnable    =   (GT_BOOL)inFields[21];
    inlifEntry.ipSecurityProfile              =   (GT_U32)inFields[22];
    inlifEntry.ipv4IgmpToCpuEnable            =   (GT_BOOL)inFields[23];
    inlifEntry.ipv6IcmpToCpuEnable            =   (GT_BOOL)inFields[24];
    inlifEntry.udpBcRelayEnable               =   (GT_BOOL)inFields[25];
    inlifEntry.arpBcToCpuEnable               =   (GT_BOOL)inFields[26];
    inlifEntry.arpBcToMeEnable                =   (GT_BOOL)inFields[27];
    inlifEntry.ripv1MirrorEnable              =   (GT_BOOL)inFields[28];
    inlifEntry.ipv4LinkLocalMcCommandEnable   =   (GT_BOOL)inFields[29];
    inlifEntry.ipv6LinkLocalMcCommandEnable   =   (GT_BOOL)inFields[30];
    inlifEntry.ipv6NeighborSolicitationEnable =   (GT_BOOL)inFields[31];

    /* Misc Section */
    inlifEntry.mirrorToAnalyzerEnable = (GT_BOOL)inFields[32];
    inlifEntry.mirrorToCpuEnable      = (GT_BOOL)inFields[33];


    /* call cpss api function */
    result = cpssExMxPmInlifEntrySet(devNum, inlifType, inlifIndex, &inlifEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmInlifEntryGet
*
* DESCRIPTION:
*       Get Inlif Entry
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - device number
*       inlifType      - inlif type port/vlan/external
*       inlifIndex     - (0..63) for inLif type port or
*                        (0..4095) for inLif type vlan or
*                        (4096..65535) for inLif type external
*
* OUTPUTS:
*       inlifEntryPtr  - (pointer to) The Inlif Fields.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong input parameters
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for PORT, VLAN and EXTERNAL inlif entry settings
*
*       port - access the internal inLif table with index: (0..63)
*
*       vlan - access the internal inLif table with index: (0..4095)
*
*       external - access the external inLif table with index: (4096..65535)
*
*******************************************************************************/
CMD_STATUS cpssExMxPm2InlifEntryGetFirst_1
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;

    IN  GT_U8                        devNum;
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType;

    IN  GT_U32                       rangeStart;
    IN  GT_U32                       rangeEnd;

    OUT CPSS_EXMXPM_INLIF_ENTRY_STC  inlifEntry;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    inlifType  = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];
    rangeStart = (GT_U32)inArgs[2];
    rangeEnd   = (GT_U32)inArgs[3];

    InlifEntryIndex = rangeStart; /* reset on first */
    InlifEntryMax   = rangeEnd;

    /* call cpss API function */
    result = cpssExMxPmInlifEntryGet(devNum, inlifType, InlifEntryIndex, &inlifEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    if (InlifEntryIndex > InlifEntryMax)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

/* pack table fields */
    inFields[0]  = InlifEntryIndex;

    /* Bridge Section */
    inFields[1]  = inlifEntry.bridgeEnable;
    inFields[2]  = inlifEntry.autoLearnEnable;
    inFields[3]  = inlifEntry.naMessageToCpuEnable;
    inFields[4]  = inlifEntry.naStormPreventionEnable;
    inFields[5]  = inlifEntry.unkSaUcCommand;
    inFields[6]  = inlifEntry.unkDaUcCommand;
    inFields[7]  = inlifEntry.unkSaNotSecurBreachEnable;
    inFields[8]  = inlifEntry.untaggedMruIndex;
    inFields[9]  = inlifEntry.unregNonIpMcCommand;
    inFields[10] = inlifEntry.unregIpMcCommand;
    inFields[11] = inlifEntry.unregIpv4BcCommand;
    inFields[12] = inlifEntry.unregNonIpv4BcCommand;

    /* Router Section */
    inFields[13] = inlifEntry.ipv4UcRouteEnable;
    inFields[14] = inlifEntry.ipv4McRouteEnable;
    inFields[15] = inlifEntry.ipv6UcRouteEnable;
    inFields[16] = inlifEntry.ipv6McRouteEnable;
    inFields[17] = inlifEntry.mplsRouteEnable;
    inFields[18] = inlifEntry.vrfId;

    /* SCT Section */
    inFields[19] = inlifEntry.ipv4IcmpRedirectEnable;
    inFields[20] = inlifEntry.ipv6IcmpRedirectEnable;
    inFields[21] = inlifEntry.bridgeRouterInterfaceEnable;
    inFields[22] = inlifEntry.ipSecurityProfile;
    inFields[23] = inlifEntry.ipv4IgmpToCpuEnable;
    inFields[24] = inlifEntry.ipv6IcmpToCpuEnable;
    inFields[25] = inlifEntry.udpBcRelayEnable;
    inFields[26] = inlifEntry.arpBcToCpuEnable;
    inFields[27] = inlifEntry.arpBcToMeEnable;
    inFields[28] = inlifEntry.ripv1MirrorEnable;
    inFields[29] = inlifEntry.ipv4LinkLocalMcCommandEnable;
    inFields[30] = inlifEntry.ipv6LinkLocalMcCommandEnable;
    inFields[31] = inlifEntry.ipv6NeighborSolicitationEnable;

    /* Misc Section */
    inFields[32] = inlifEntry.mirrorToAnalyzerEnable;
    inFields[33] = inlifEntry.mirrorToCpuEnable;


/*  output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3], inFields[4],
                inFields[5], inFields[6], inFields[7],  inFields[8],inFields[9],
                inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23], inFields[24],
                inFields[25], inFields[26], inFields[27], inFields[28], inFields[29],
                inFields[30], inFields[31], inFields[32], inFields[33]
                );

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%f", 0);

    InlifEntryIndex++;

    return CMD_OK;
}

static CMD_STATUS wrCpssExMxPm2InlifGetNext_1
(
IN  GT_32 inArgs[CMD_MAX_ARGS],
IN  GT_32 inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    IN  GT_U8                        devNum;
    IN  CPSS_EXMXPM_INLIF_TYPE_ENT   inlifType;

    IN  GT_U32                       rangeStart;
    IN  GT_U32                       rangeEnd;

    OUT CPSS_EXMXPM_INLIF_ENTRY_STC  inlifEntry;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    inlifType  = (CPSS_EXMXPM_INLIF_TYPE_ENT)inArgs[1];

    rangeStart = (GT_U32)inArgs[2];
    rangeEnd   = (GT_U32)inArgs[3];

    if (InlifEntryIndex > rangeEnd)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss API function */
    result = cpssExMxPmInlifEntryGet(devNum, inlifType, InlifEntryIndex, &inlifEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

/* pack table fields */
    inFields[0]  = InlifEntryIndex;

    /* Bridge Section */
    inFields[1]  = inlifEntry.bridgeEnable;
    inFields[2]  = inlifEntry.autoLearnEnable;
    inFields[3]  = inlifEntry.naMessageToCpuEnable;
    inFields[4]  = inlifEntry.naStormPreventionEnable;
    inFields[5]  = inlifEntry.unkSaUcCommand;
    inFields[6]  = inlifEntry.unkDaUcCommand;
    inFields[7]  = inlifEntry.unkSaNotSecurBreachEnable;
    inFields[8]  = inlifEntry.untaggedMruIndex;
    inFields[9]  = inlifEntry.unregNonIpMcCommand;
    inFields[10] = inlifEntry.unregIpMcCommand;
    inFields[11] = inlifEntry.unregIpv4BcCommand;
    inFields[12] = inlifEntry.unregNonIpv4BcCommand;

    /* Router Section */
    inFields[13] = inlifEntry.ipv4UcRouteEnable;
    inFields[14] = inlifEntry.ipv4McRouteEnable;
    inFields[15] = inlifEntry.ipv6UcRouteEnable;
    inFields[16] = inlifEntry.ipv6McRouteEnable;
    inFields[17] = inlifEntry.mplsRouteEnable;
    inFields[18] = inlifEntry.vrfId;

    /* SCT Section */
    inFields[19] = inlifEntry.ipv4IcmpRedirectEnable;
    inFields[20] = inlifEntry.ipv6IcmpRedirectEnable;
    inFields[21] = inlifEntry.bridgeRouterInterfaceEnable;
    inFields[22] = inlifEntry.ipSecurityProfile;
    inFields[23] = inlifEntry.ipv4IgmpToCpuEnable;
    inFields[24] = inlifEntry.ipv6IcmpToCpuEnable;
    inFields[25] = inlifEntry.udpBcRelayEnable;
    inFields[26] = inlifEntry.arpBcToCpuEnable;
    inFields[27] = inlifEntry.arpBcToMeEnable;
    inFields[28] = inlifEntry.ripv1MirrorEnable;
    inFields[29] = inlifEntry.ipv4LinkLocalMcCommandEnable;
    inFields[30] = inlifEntry.ipv6LinkLocalMcCommandEnable;
    inFields[31] = inlifEntry.ipv6NeighborSolicitationEnable;

    /* Misc Section */
    inFields[32] = inlifEntry.mirrorToAnalyzerEnable;
    inFields[33] = inlifEntry.mirrorToCpuEnable;

    /*  output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3], inFields[4],
                inFields[5], inFields[6], inFields[7],  inFields[8],inFields[9],
                inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23], inFields[24],
                inFields[25], inFields[26], inFields[27], inFields[28], inFields[29],
                inFields[30], inFields[31], inFields[32], inFields[33]
                );

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%f", 0);

    InlifEntryIndex++;

    return CMD_OK;
}




/**** database initialization **************************************/


static CMD_COMMAND dbCommands[] =
{

    {"cpssExMxPmInlifPortModeSet",
        &wrCpssExMxPmInlifPortModeSet,
        3, 0},
    {"cpssExMxPmInlifPortModeGet",
        &wrCpssExMxPmInlifPortModeGet,
        2, 0},
    {"cpssExMxPmInlifIntForceVlanModeOnTtiSet",
        &wrCpssExMxPmInlifIntForceVlanModeOnTtiSet,
        2, 0},
    {"cpssExMxPmInlifIntForceVlanModeOnTtiGet",
        &wrCpssExMxPmInlifIntForceVlanModeOnTtiGet,
        1, 0},
    {"cpssExMxPmInlifEntry_1Set",
        &wrCpssExMxPmInlifEntrySet,
        5, 32},
    {"cpssExMxPmInlifEntry_1GetFirst",
        &wrCpssExMxPmInlifEntryGetFirst,
        5, 0},
    {"cpssExMxPmInlifEntry_1GetNext",
        &wrCpssExMxPmInlifEntryGetNext,
        5, 0},
    {"cpssExMxPmInlifBridgeEnableSet",
        &wrCpssExMxPmInlifBridgeEnableSet,
        4, 0},

    {"cpssExMxPmInlifUnkSaNotSecurBreachEnableSet",
        &wrCpssExMxPmInlifUnkSaNotSecurBreachEnableSet,
        4, 0},

    {"cpssExMxPmInlifNaStormPreventionEnableSet",
        &wrCpssExMxPmInlifNaStormPreventionEnableSet,
        4, 0},

    {"cpssExMxPmInlifAutoLearnEnableSet",
        &wrCpssExMxPmInlifAutoLearnEnableSet,
        4, 0},

    {"cpssExMxPmInlifNaToCpuEnableSet",
        &wrCpssExMxPmInlifNaToCpuEnableSet,
        4, 0},

    {"cpssExMxPmInlifUnkUnregAddrCmdSet",
        &wrCpssExMxPmInlifUnkUnregAddrCmdSet,
        5, 0},
    {"cpssExMxPm2InlifUnkUnregAddrCmdSet",
        &wrCpssExMxPmInlifUnkUnregAddrCmdSet,
        5, 0},

    {"cpssExMxPmInlifControlTrafficEnableSet",
        &wrCpssExMxPmInlifControlTrafficEnableSet,
        5, 0},
    {"cpssExMxPm2InlifControlTrafficEnableSet",
        &wrCpssExMxPmInlifControlTrafficEnableSet,
        5, 0},

    {"cpssExMxPmInlifIpUcRouteEnableSet",
        &wrCpssExMxPmInlifIpUcRouteEnableSet,
        5, 0},

    {"cpssExMxPmInlifIpMcRouteEnableSet",
        &wrCpssExMxPmInlifIpMcRouteEnableSet,
        5, 0},


    {"cpssExMxPmInlifBridgeRouterInterfaceEnabledSet",
        &wrCpssExMxPmInlifBridgeRouterInterfaceEnabledSet,
        4, 0},


    {"cpssExMxPmInlifIpSecurityProfileSet",
        &wrCpssExMxPmInlifIpSecurityProfileSet,
        4, 0},
    {"cpssExMxPmInlifVrIdSet",
        &wrCpssExMxPmInlifVrIdSet,
        4, 0},


    /*puma2*/
    {"cpssExMxPm2InlifEntrySet",
        &wrCpssExMxPm2InlifEntrySet,
        1, 34},
    {"cpssExMxPm2InlifEntryGetFirst",
        &wrCpssExMxPm2InlifEntryGetFirst,
        3, 0},
    {"cpssExMxPm2InlifEntryGetNext",
        &wrCpssExMxPm2InlifEntryGetNext,
        3, 0},
    {"cpssExMxPm2InlifPortDefaultInlifIdSet",
        &wrCpssExMxPmInlifPortDefaultInlifIdSet,
        3, 0},
    {"cpssExMxPm2InlifPortDefaultInlifIdGet",
        &wrCpssExMxPmInlifPortDefaultInlifIdGet,
        2, 0},
    {"cpssExMxPmInlifMirrorToAnalyzerEnableSet",
        &wrCpssExMxPmInlifMirrorToAnalyzerEnableSet,
        3, 0},
    {"cpssExMxPmInlifMirrorToCpuEnableSet",
        &wrCpssExMxPmInlifMirrorToCpuEnableSet,
        3, 0},
    {"cpssExMxPm2InlifMplsRouteEnableSet",
        &wrCpssExMxPmInlifMplsRouteEnableSet,
        3, 0},

/* updates for Puma 2 */
    {"cpssExMxPm2InlifBridgeEnable_2Set",
        &wrCpssExMxPm2InlifBridgeEnableSet,
        4, 0},
    {"cpssExMxPm2InlifUnkSaNotSecurBreachEnable_2Set",
        &wrCpssExMxPm2InlifUnkSaNotSecurBreachEnableSet,
        4, 0},
    {"cpssExMxPm2InlifNaStormPreventionEnable_2Set",
        &wrCpssExMxPm2InlifNaStormPreventionEnableSet,
        4, 0},
    {"cpssExMxPm2InlifAutoLearnEnable_2Set",
        &wrCpssExMxPm2InlifAutoLearnEnableSet,
        4, 0},
    {"cpssExMxPm2InlifNaToCpuEnable_2Set",
        &wrCpssExMxPm2InlifNaToCpuEnableSet,
        4, 0},
    {"cpssExMxPm2InlifUnkUnregAddrCmd_2Set",
        &wrCpssExMxPm2InlifUnkUnregAddrCmdSet,
        5, 0},
    {"cpssExMxPm2InlifUntaggedMruIndex_2Set",
        &wrCssExMxPm2InlifUntaggedMruIndexSet,
        4, 0},
    {"cpssExMxPm2InlifIpUcRouteEnable_2Set",
        &wrCpssExMxPm2InlifIpUcRouteEnableSet,
        5, 0},
    {"cpssExMxPm2InlifIpMcRouteEnable_2Set",
        &wrCpssExMxPm2InlifIpMcRouteEnableSet,
        5, 0},
    {"cpssExMxPm2InlifMplsRouteEnable_2Set",
        &wrCpssExMxPm2InlifMplsRouteEnableSet,
        4, 0},
    {"cpssExMxPm2InlifVrfId_2Set",
        &wrCpssExMxPm2InlifVrfIdSet,
        4, 0},
    {"cpssExMxPm2InlifBridgeRouterInterfaceEnabled_2Set",
        &wrCpssExMxPm2InlifBridgeRouterInterfaceEnabledSet,
        4, 0},
    {"cpssExMxPm2InlifIpSecurityProfile_2Set",
        &wrCpssExMxPm2InlifIpSecurityProfileSet,
        4, 0},
    {"cpssExMxPm2InlifControlTrafficEnable_2Set",
        &wrCpssExMxPm2InlifControlTrafficEnableSet,
        5, 0},
    {"cpssExMxPm2InlifMirrorToAnalyzerEnable_2Set",
        &wrCpssExMxPm2InlifMirrorToAnalyzerEnableSet,
        4, 0},
    {"cpssExMxPm2InlifMirrorToCpuEnable_2Set",
        &wrCpssExMxPm2InlifMirrorToCpuEnableSet,
        4, 0},

    /********** Table for: cpssExMxPmInlifEntry **********/
    {"cpssExMxPm2InlifEntry_2Set",
        &wrCpssExMxPm2InlifEntrySet_1,
        4, 34},
    {"cpssExMxPm2InlifEntry_2GetFirst",
        &cpssExMxPm2InlifEntryGetFirst_1,
        4, 0},
    {"cpssExMxPm2InlifEntry_2GetNext",
        &wrCpssExMxPm2InlifGetNext_1,
        4, 0},
    /****** end of Table: cpssExMxPmInlifEntry *******/
};



#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmInlif
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
GT_STATUS cmdLibInitCpssExMxPmInlif
(
GT_VOID
)
{

    return cmdInitLibrary(dbCommands, numCommands);

}

