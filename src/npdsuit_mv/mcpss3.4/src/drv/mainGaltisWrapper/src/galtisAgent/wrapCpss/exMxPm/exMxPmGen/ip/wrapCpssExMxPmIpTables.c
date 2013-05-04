/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmIpTables.c
*
* DESCRIPTION:
*
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
#include <cpss/generic/ip/cpssIpTypes.h>
#include <cpss/exMxPm/exMxPmGen/cpssExMxPmTypes.h>
#include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpTypes.h>
#include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpTables.h>
#include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpCtrl.h>
#include <cpss/exMxPm/exMxPmGen/dit/cpssExMxPmDitTypes.h>
#include <cpss/exMxPm/exMxPmGen/dit/cpssExMxPmDit.h>

/****************cpssExMxPmIpUcRouteEntries Table************/

/*set*/
static CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC*        gIPUCrouteEntryArrayPtr = NULL;
static GT_U32                                  gIPUCnumOfEntries = 0;

/*get*/


static CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC*
                       gIPUCgetRouteEntryArrayPtr= NULL;
static GT_U32          gIPUCgetRouteEntryCurrIndx = 0;
static GT_U32          gIPUCgetNumOfEntries;

/*******************************************************************************
* cpssExMxPmIpUcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of uc route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                  - device number
*       ucMcRouteEntryBaseIndex - The base Index in the Route entries table
*                                 range (0..4095). The entries will be written
*                                 from this base on.
*       numOfRouteEntries       - The number route entries to write.
*       routeEntriesArrayPtr    - The uc route entries array
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       This table is shared between unicast and multicast route entries. Each
*       entry can hold one unicast route entry or one multicast route entry.
*       The function loops over the entries.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpUcRouteEntriesWriteSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32      ind;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    gIPUCnumOfEntries = 0;/*reset on first*/

    /* map input arguments to locals */
    if(gIPUCrouteEntryArrayPtr == NULL)
    {
        gIPUCrouteEntryArrayPtr = (CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC*)
                    cmdOsMalloc(sizeof(CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC));
    }
    if(gIPUCrouteEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gIPUCnumOfEntries)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gIPUCrouteEntryArrayPtr[ind].type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.command=
                                 (CPSS_PACKET_CMD_ENT)inFields[1];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.cpuCodeIndex =
                                  (GT_U32)inFields[2];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.appSpecificCpuCodeEnable =
                                                          (GT_BOOL)inFields[3];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.ucPacketSipFilterEnable =
                                                          (GT_BOOL)inFields[4];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.ucRpfOrIcmpRedirectIfMode =
                                      (CPSS_EXMXPM_RPF_IF_MODE_ENT)inFields[5];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.hopLimitMode =
                                (GT_TRUE == (GT_BOOL)inFields[6])?
                                CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.ttlMode =
                                (GT_TRUE == (GT_BOOL)inFields[6])?
                                CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.ttl = 0;

    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.bypassTtlExceptionCheckEnable =
                                                          (GT_BOOL)inFields[7];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.mirrorToIngressAnalyzer =
                                                          (GT_BOOL)inFields[8];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyDp =
                                                          (GT_BOOL)inFields[9];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyDscp =
                                                         (GT_BOOL)inFields[10];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyExp =
                                                         (GT_BOOL)inFields[11];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyTc=
                                                         (GT_BOOL)inFields[12];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyUp =
                                                         (GT_BOOL)inFields[13];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParams.dp = (CPSS_DP_LEVEL_ENT)inFields[14];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParams.dscp = (GT_U32)inFields[15];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParams.exp = (GT_U32)inFields[16];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParams.tc = (GT_U32)inFields[17];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParams.up= (GT_U32)inFields[18];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.cntrSetIndex =
                      (CPSS_EXMXPM_IP_COUNTER_SET_INDEX_ENT)inFields[19];

    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.trapMirrorArpBcToCpu =(GT_BOOL)inFields[20];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.sipAccessLevel = (GT_U32)inFields[21];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.dipAccessLevel = (GT_U32)inFields[22];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.icmpRedirectEnable = (GT_BOOL)inFields[23];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.sipSaCheckEnable = (GT_BOOL)inFields[24];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.ipv6ScopeCheckEnable = (GT_BOOL)inFields[25];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.ipv6DestinationSiteId =
                                            (CPSS_IP_SITE_ID_ENT)inFields[26];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.mtuProfileIndex = (GT_U32)inFields[27];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.ageRefresh = (GT_U16)inFields[28];

    if (GT_TRUE == (GT_BOOL)inFields[29])
    {
        gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E;
        gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifPointer.tunnelStartPtr.ptr=(GT_U32)inFields[38];
        gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifPointer.tunnelStartPtr.passengerPacketType=
                                                (CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[39];
    }
    else
    {
        gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifPointer.arpPtr =(GT_U32)inFields[37];
    }

    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum =(GT_U8)inFields[30];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum =(GT_U8)inFields[31];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.trunkId=(GT_TRUNK_ID)inFields[32];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inFields[33];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.vidx = (GT_U16)inFields[34];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx = (GT_U16)inFields[34];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.vlanId = (GT_U16)inFields[35];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopVlanId = (GT_U16)inFields[36];

    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.mplsCommand = CPSS_EXMXPM_MPLS_NOP_CMD_E;
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.mplsLabel = 0;
    gIPUCnumOfEntries++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmIpUcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of uc route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                  - device number
*       ucMcRouteEntryBaseIndex - The base Index in the Route entries table
*                                 range (0..4095). The entries will be written
*                                 from this base on.
*       numOfRouteEntries       - The number route entries to write.
*       routeEntriesArrayPtr    - The uc route entries array
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       This table is shared between unicast and multicast route entries. Each
*       entry can hold one unicast route entry or one multicast route entry.
*       The function loops over the entries.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpUcRouteEntriesWriteSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    gIPUCrouteEntryArrayPtr = (CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC*)
             cmdOsRealloc(gIPUCrouteEntryArrayPtr,
             sizeof(CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC) * (gIPUCnumOfEntries+1));

    if(gIPUCrouteEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }
    ind = (GT_U32)inFields[0];

    if(ind > gIPUCnumOfEntries)
    {
      galtisOutput(outArgs, GT_BAD_VALUE,"");
      return CMD_OK;
    }

    gIPUCrouteEntryArrayPtr[ind].type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.command=
                                 (CPSS_PACKET_CMD_ENT)inFields[1];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.cpuCodeIndex =
                                  (GT_U32)inFields[2];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.appSpecificCpuCodeEnable =
                                                          (GT_BOOL)inFields[3];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.ucPacketSipFilterEnable =
                                                          (GT_BOOL)inFields[4];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.ucRpfOrIcmpRedirectIfMode =
                                      (CPSS_EXMXPM_RPF_IF_MODE_ENT)inFields[5];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.hopLimitMode =
                                (GT_TRUE == (GT_BOOL)inFields[6])?
                                CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.ttlMode =
                                (GT_TRUE == (GT_BOOL)inFields[6])?
                                CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.ttl = 0;
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.bypassTtlExceptionCheckEnable =
                                                          (GT_BOOL)inFields[7];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.mirrorToIngressAnalyzer =
                                                          (GT_BOOL)inFields[8];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyDp =
                                                          (GT_BOOL)inFields[9];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyDscp =
                                                         (GT_BOOL)inFields[10];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyExp =
                                                         (GT_BOOL)inFields[11];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyTc=
                                                         (GT_BOOL)inFields[12];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyUp =
                                                         (GT_BOOL)inFields[13];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParams.dp = (CPSS_DP_LEVEL_ENT)inFields[14];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParams.dscp = (GT_U32)inFields[15];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParams.exp = (GT_U32)inFields[16];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParams.tc = (GT_U32)inFields[17];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.qosParams.up= (GT_U32)inFields[18];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.cntrSetIndex =
                      (CPSS_EXMXPM_IP_COUNTER_SET_INDEX_ENT)inFields[19];

    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.trapMirrorArpBcToCpu =(GT_BOOL)inFields[20];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.sipAccessLevel = (GT_U32)inFields[21];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.dipAccessLevel = (GT_U32)inFields[22];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.icmpRedirectEnable = (GT_BOOL)inFields[23];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.sipSaCheckEnable = (GT_BOOL)inFields[24];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.ipv6ScopeCheckEnable = (GT_BOOL)inFields[25];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.ipv6DestinationSiteId =
                                            (CPSS_IP_SITE_ID_ENT)inFields[26];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.mtuProfileIndex = (GT_U32)inFields[27];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.ageRefresh = (GT_U16)inFields[28];

    if (GT_TRUE == (GT_BOOL)inFields[29])
    {
        gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E;
        gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifPointer.tunnelStartPtr.ptr=(GT_U32)inFields[38];
        gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifPointer.tunnelStartPtr.passengerPacketType=
                                      (CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[39];
    }
    else
    {
        gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifPointer.arpPtr =(GT_U32)inFields[37];
    }

    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum =(GT_U8)inFields[30];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum =(GT_U8)inFields[31];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.trunkId=(GT_TRUNK_ID)inFields[32];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inFields[33];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.vidx = (GT_U16)inFields[34];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx = (GT_U16)inFields[34];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.vlanId = (GT_U16)inFields[35];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.nextHopVlanId = (GT_U16)inFields[36];
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.mplsCommand = CPSS_EXMXPM_MPLS_NOP_CMD_E;
    gIPUCrouteEntryArrayPtr[ind].entry.regularEntry.mplsLabel = 0;

    gIPUCnumOfEntries++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpUcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of uc route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                  - device number
*       ucMcRouteEntryBaseIndex - The base Index in the Route entries table
*                                 range (0..4095). The entries will be written
*                                 from this base on.
*       numOfRouteEntries       - The number route entries to write.
*       routeEntriesArrayPtr    - The uc route entries array
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       This table is shared between unicast and multicast route entries. Each
*       entry can hold one unicast route entry or one multicast route entry.
*       The function loops over the entries.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpUcRouteEntriesWriteEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8    devNum;
    GT_U32   ucMcRouteEntryBaseIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    ucMcRouteEntryBaseIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
   result =  cpssExMxPmIpUcRouteEntriesWrite(devNum,ucMcRouteEntryBaseIndex,
                                           gIPUCnumOfEntries,
                                           gIPUCrouteEntryArrayPtr);
   if (result != GT_OK)
   {
       galtisOutput(outArgs, result, "");
       return CMD_OK;
   }


   cmdOsFree(gIPUCrouteEntryArrayPtr);
   gIPUCrouteEntryArrayPtr = NULL;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmIpUcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of uc route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                  - device number
*       ucMcRouteEntryBaseIndex - The base Index in the Route entries table
*                                 range (0..4095). The entries will be written
*                                 from this base on.
*       numOfRouteEntries       - The number route entries to write.
*       routeEntriesArrayPtr    - The uc route entries array
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       This table is shared between unicast and multicast route entries. Each
*       entry can hold one unicast route entry or one multicast route entry.
*       The function loops over the entries.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpUcRouteEntriesWriteCancelSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

   cmdOsFree(gIPUCrouteEntryArrayPtr);
   gIPUCrouteEntryArrayPtr = NULL;

    galtisOutput(outArgs, GT_BAD_VALUE,"");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmIpUcRouteEntriesRead
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Read an array of uc route entries from the hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                  - device number
*       ucMcRouteEntryBaseIndex - The base Index in the Route entries table
*                                 range (0..4095). The entries will be read
*                                 from this base on.
*       numOfRouteEntries       - The number route entries to read.
*
*
* OUTPUTS:
*       routeEntriesArrayPtr    - The uc route entries array to read into.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       This table is shared between unicast and multicast route entries. Each
*       entry can hold one unicast route entry or one multicast route entry.
*       The function loops over the entries.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpUcRouteEntriesReadGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    ucMcRouteEntryBaseIndex;
    GT_U32    ind, i;
    GT_U32    numToRefreash;/*number of entries to get from table*/

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gIPUCgetRouteEntryCurrIndx = 0;/*reset on first*/
    ind = gIPUCgetRouteEntryCurrIndx;


    /* map input arguments to locals */
     devNum= (GT_U8)inArgs[0];
     ucMcRouteEntryBaseIndex=(GT_U32)inArgs[1];
     numToRefreash=(GT_U32)inArgs[2];
     gIPUCgetNumOfEntries=numToRefreash;

     if(gIPUCgetRouteEntryArrayPtr == NULL)
    {
        gIPUCgetRouteEntryArrayPtr = (CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC*)
                    cmdOsMalloc(sizeof(CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC)*numToRefreash);
    }
    if(gIPUCgetRouteEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

   /* call cpss api function */
    cmdOsMemSet(gIPUCgetRouteEntryArrayPtr,0,
                sizeof(CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC) * gIPUCgetNumOfEntries);

    for (i=0; i<gIPUCgetNumOfEntries; i++)
    {
        gIPUCgetRouteEntryArrayPtr[i].type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;
    }

    result = cpssExMxPmIpUcRouteEntriesRead(devNum, ucMcRouteEntryBaseIndex, gIPUCgetNumOfEntries, gIPUCgetRouteEntryArrayPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] =gIPUCgetRouteEntryCurrIndx;
    inFields[1] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.command;
    inFields[2] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.cpuCodeIndex;
    inFields[3] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.appSpecificCpuCodeEnable;
    inFields[4] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.ucPacketSipFilterEnable;
    inFields[5] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.ucRpfOrIcmpRedirectIfMode;
    inFields[6] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.hopLimitMode ;
    inFields[7] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.bypassTtlExceptionCheckEnable;
    inFields[8] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.mirrorToIngressAnalyzer;
    inFields[9] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyDp;
    inFields[10]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyDscp;
    inFields[11]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyExp;
    inFields[12]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyTc;
    inFields[13]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyUp;
    inFields[14]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParams.dp;
    inFields[15]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParams.dscp;
    inFields[16]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParams.exp ;
    inFields[17]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParams.tc ;
    inFields[18]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParams.up;
    inFields[19]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.cntrSetIndex;
    inFields[20]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.trapMirrorArpBcToCpu;
    inFields[21]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.sipAccessLevel;
    inFields[22]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.dipAccessLevel;
    inFields[23]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.icmpRedirectEnable;
    inFields[24]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.sipSaCheckEnable;
    inFields[25]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.ipv6ScopeCheckEnable;
    inFields[26]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.ipv6DestinationSiteId ;
    inFields[27]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.mtuProfileIndex;
    inFields[28]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.ageRefresh;
    inFields[30]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum;
    inFields[31]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum;
    inFields[32]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.trunkId;

    switch (gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifType)
    {
        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
            inFields[29]=GT_TRUE;
            break;
        default:
            inFields[29]=GT_FALSE;
    }

    inFields[33]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.type;
    if(gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.type == CPSS_INTERFACE_FABRIC_VIDX_E)
        inFields[34]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx;
    else
        inFields[34]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.vidx;
    inFields[35]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.vlanId;
    inFields[36]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopVlanId;
    inFields[37]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifPointer.arpPtr;
    inFields[38]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifPointer.tunnelStartPtr.ptr;
    inFields[39]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifPointer.tunnelStartPtr.passengerPacketType;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                ,inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29], inFields[30],inFields[31],
                inFields[32], inFields[33], inFields[34],inFields[35],
                inFields[36], inFields[37], inFields[38],inFields[39]);

    galtisOutput(outArgs, result, "%f");

    gIPUCgetRouteEntryCurrIndx++;

    return CMD_OK;

}
/*******************************************************************************
* cpssExMxPmIpUcRouteEntriesRead
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Read an array of uc route entries from the hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                  - device number
*       ucMcRouteEntryBaseIndex - The base Index in the Route entries table
*                                 range (0..4095). The entries will be read
*                                 from this base on.
*       numOfRouteEntries       - The number route entries to read.
*
*
* OUTPUTS:
*       routeEntriesArrayPtr    - The uc route entries array to read into.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       This table is shared between unicast and multicast route entries. Each
*       entry can hold one unicast route entry or one multicast route entry.
*       The function loops over the entries.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpUcRouteEntriesReadGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32    ind = gIPUCgetRouteEntryCurrIndx;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(gIPUCgetRouteEntryCurrIndx >= gIPUCgetNumOfEntries)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    /* map input arguments to locals */

    inFields[0] =gIPUCgetRouteEntryCurrIndx;
    inFields[1] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.command;
    inFields[2] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.cpuCodeIndex;
    inFields[3] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.appSpecificCpuCodeEnable;
    inFields[4] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.ucPacketSipFilterEnable;
    inFields[5] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.ucRpfOrIcmpRedirectIfMode;
    inFields[6] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.hopLimitMode ;
    inFields[7] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.bypassTtlExceptionCheckEnable;
    inFields[8] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.mirrorToIngressAnalyzer;
    inFields[9] =gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyDp;
    inFields[10]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyDscp;
    inFields[11]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyExp;
    inFields[12]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyTc;
    inFields[13]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParamsModify.modifyUp;
    inFields[14]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParams.dp;
    inFields[15]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParams.dscp;
    inFields[16]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParams.exp;
    inFields[17]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParams.tc;
    inFields[18]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.qosParams.up;
    inFields[19]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.cntrSetIndex;
    inFields[20]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.trapMirrorArpBcToCpu;
    inFields[21]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.sipAccessLevel;
    inFields[22]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.dipAccessLevel;
    inFields[23]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.icmpRedirectEnable;
    inFields[24]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.sipSaCheckEnable;
    inFields[25]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.ipv6ScopeCheckEnable;
    inFields[26]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.ipv6DestinationSiteId;
    inFields[27]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.mtuProfileIndex;
    inFields[28]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.ageRefresh;
    inFields[30]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum;
    inFields[31]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum;
    inFields[32]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.trunkId;

    switch (gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifType)
    {
        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
            inFields[29]=GT_TRUE;
            break;
        default:
            inFields[29]=GT_FALSE;
    }

    inFields[33]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.type;
    if(gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.type == CPSS_INTERFACE_FABRIC_VIDX_E)
        inFields[34]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx;
    else
        inFields[34]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.vidx;
    inFields[35]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.interfaceInfo.vlanId;
    inFields[36]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopVlanId;
    inFields[37]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifPointer.arpPtr;
    inFields[38]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifPointer.tunnelStartPtr.ptr;
    inFields[39]=gIPUCgetRouteEntryArrayPtr[ind].entry.regularEntry.nextHopIf.outlifPointer.tunnelStartPtr.passengerPacketType;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                ,inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29], inFields[30],inFields[31],
                inFields[32], inFields[33], inFields[34],inFields[35],
                inFields[36], inFields[37], inFields[38],inFields[39]);


    galtisOutput(outArgs, GT_OK, "%f");

    gIPUCgetRouteEntryCurrIndx++;

    return CMD_OK;
}




/***************** cpssExMxPmIpUcEcmpRpfEntries Table *********************/

/* set */

static CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC   ipSetEcmpRpfEntry;
static GT_U32                              ucSetEcmpRpfIfIndex = 0;

/* get */

static CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC   ipGetEcmpRpfEntry;
static GT_U32                              ucGetEcmpRpfIfIndex = 0;

/*******************************************************************************
* cpssExMxPmIpUcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write Ecmp Rpf Entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                  - device number
*       ucMcRouteEntryBaseIndex - The base Index in the Route entries table
*                                 range (0..4095).
*       ecmpRpfEntryPtr         - The ECMP RPF entry
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpEcmpRpfEntriesWriteSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lineIndex=0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* start with first uc ecmp rpfIf */
    ucSetEcmpRpfIfIndex = 0;

    /* map input arguments to locals */
    lineIndex = (GT_U32)inFields[0];
    if(lineIndex != ucSetEcmpRpfIfIndex)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d",-1);
        return CMD_OK;
    }
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIcmpMode = (CPSS_EXMXPM_RPF_IF_MODE_ENT)inFields[1];
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.sipFilterEnable = (GT_BOOL)inFields[2];
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.sipAccessLevel = (GT_U32)inFields[3];
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[lineIndex].valid =  (GT_BOOL)inFields[4];
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[lineIndex].interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inFields[5];
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[lineIndex].interfaceInfo.devPort.devNum = (GT_U8)inFields[6];
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[lineIndex].interfaceInfo.devPort.portNum = (GT_U8)inFields[7];
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[lineIndex].interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[8];
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[lineIndex].interfaceInfo.vlanId = (GT_U16)inFields[9];

    ucSetEcmpRpfIfIndex++;

    /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmIpUcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write Ecmp Rpf Entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                  - device number
*       ucMcRouteEntryBaseIndex - The base Index in the Route entries table
*                                 range (0..4095).
*       ecmpRpfEntryPtr         - The ECMP RPF entry
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpEcmpRpfEntriesWriteSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lineIndex=0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* verify that no more than 8 rpfIf are added (ecmp support max of 8 rpfIf) */
    if (ucSetEcmpRpfIfIndex >= 8)
    {
        galtisOutput(outArgs, GT_OUT_OF_RANGE, "%d",-1);
        return CMD_OK;
    }

    lineIndex = (GT_U32)inFields[0];
    if(lineIndex != ucSetEcmpRpfIfIndex)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d",-1);
        return CMD_OK;
    }

    /* NOTE THAT rpfIcmpMode, sipFilterEnable & sipAccessLevel ARE TAKEN FROM */
    /* FIRST ENTRY ONLY AND IGNORED FOR THE REST OF THE ENTRIES               */

    /* map input arguments to locals */
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[lineIndex].valid =  (GT_BOOL)inFields[4];
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[lineIndex].interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inFields[5];
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[lineIndex].interfaceInfo.devPort.devNum = (GT_U8)inFields[6];
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[lineIndex].interfaceInfo.devPort.portNum = (GT_U8)inFields[7];
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[lineIndex].interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[8];
    ipSetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[lineIndex].interfaceInfo.vlanId = (GT_U16)inFields[9];

    ucSetEcmpRpfIfIndex++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmIpUcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write Ecmp Rpf Entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                  - device number
*       ucMcRouteEntryBaseIndex - The base Index in the Route entries table
*                                 range (0..4095).
*       ecmpRpfEntryPtr         - The ECMP RPF entry
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpEcmpRpfEntriesWriteEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    GT_U32                                  ucMcRouteEntryBaseIndex;
    GT_U32                                  numOfRouteEntries;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ucMcRouteEntryBaseIndex = (GT_U32)inArgs[1];
    numOfRouteEntries = 1; /* this wrapper allows only one ecmp route entry at a time */

    /* fill all ecmp rpfIf that were not entered explicitly to non valid */
    for ( ; ucSetEcmpRpfIfIndex < 8 ; ucSetEcmpRpfIfIndex++)
    {
        ipSetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[ucSetEcmpRpfIfIndex].valid = GT_FALSE;
    }

    /* this wrapper is used only to add ecmp route entry */
    ipSetEcmpRpfEntry.type = CPSS_EXMXPM_IP_UC_ECMP_RPF_E;


    /* call CPSS API */
    result = cpssExMxPmIpUcRouteEntriesWrite(devNum,
                                             ucMcRouteEntryBaseIndex,
                                             numOfRouteEntries,
                                             &ipSetEcmpRpfEntry);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmIpUcRouteEntriesRead
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Read Ecmp Rpf Entries from hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                  - device number
*       ucMcRouteEntryBaseIndex - The base Index in the Route entries table
*                                 range (0..4095).
*
* OUTPUTS:
*       ecmpRpfEntryPtr         - The ECMP RPF entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpEcmpRpfEntriesReadGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    GT_U32                                  ucMcRouteEntryBaseIndex;
    GT_U32                                  numOfRouteEntries;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ucMcRouteEntryBaseIndex = (GT_U32)inArgs[1];
    numOfRouteEntries = 1; /* this wrapper allows only one ecmp route entry at a time */

    /* call the CPSS API */
    ipGetEcmpRpfEntry.type = CPSS_EXMXPM_IP_UC_ECMP_RPF_E;
    result = cpssExMxPmIpUcRouteEntriesRead(devNum,
                                            ucMcRouteEntryBaseIndex,
                                            numOfRouteEntries,
                                            &ipGetEcmpRpfEntry);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    ucGetEcmpRpfIfIndex = 0;

    /* map input arguments to locals */
    inFields[0] = ucGetEcmpRpfIfIndex;
    inFields[1] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIcmpMode;
    inFields[2] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.sipFilterEnable;
    inFields[3] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.sipAccessLevel;
    inFields[4] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[ucGetEcmpRpfIfIndex].valid;
    inFields[5] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[ucGetEcmpRpfIfIndex].interfaceInfo.type;
    inFields[6] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[ucGetEcmpRpfIfIndex].interfaceInfo.devPort.devNum;
    inFields[7] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[ucGetEcmpRpfIfIndex].interfaceInfo.devPort.portNum;
    inFields[8] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[ucGetEcmpRpfIfIndex].interfaceInfo.trunkId;
    inFields[9] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[ucGetEcmpRpfIfIndex].interfaceInfo.vlanId;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],  inFields[3],
                inFields[4],  inFields[5],  inFields[6],  inFields[7],
                inFields[8],  inFields[9]);

    ucGetEcmpRpfIfIndex++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmIpUcRouteEntriesRead
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Read Ecmp Rpf Entries from hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                  - device number
*       ucMcRouteEntryBaseIndex - The base Index in the Route entries table
*                                 range (0..4095).
*
* OUTPUTS:
*       ecmpRpfEntryPtr         - The ECMP RPF entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpEcmpRpfEntriesReadGetNext
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

    /* check if more lines to output */
    if (ucGetEcmpRpfIfIndex >= 8)
    {
        /* return the error code */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    inFields[0] = ucGetEcmpRpfIfIndex;
    inFields[1] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIcmpMode;
    inFields[2] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.sipFilterEnable;
    inFields[3] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.sipAccessLevel;
    inFields[4] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[ucGetEcmpRpfIfIndex].valid;
    inFields[5] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[ucGetEcmpRpfIfIndex].interfaceInfo.type;
    inFields[6] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[ucGetEcmpRpfIfIndex].interfaceInfo.devPort.devNum;
    inFields[7] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[ucGetEcmpRpfIfIndex].interfaceInfo.devPort.portNum;
    inFields[8] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[ucGetEcmpRpfIfIndex].interfaceInfo.trunkId;
    inFields[9] = ipGetEcmpRpfEntry.entry.ecmpRpfCheck.rpfIf[ucGetEcmpRpfIfIndex].interfaceInfo.vlanId;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],  inFields[3],
                inFields[4],  inFields[5],  inFields[6],  inFields[7],
                inFields[8],  inFields[9]);

    ucGetEcmpRpfIfIndex++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


 /****************cpssExMxPmIpMcRouteEntries Table************/

/*set*/
static CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC*        gIPUCrouteMcEntryArrayPtr = NULL;
static GT_U32                                  gIPUCnumOfMcEntries = 0;

/*get*/


static CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC*
                       gIPUCgetRouteMcEntryArrayPtr= NULL;
static GT_U32          gIPUCgetRouteMcEntryCurrIndx = 0;
static GT_U32          gIPUCgetNumOfMcEntries;

/*******************************************************************************
* cpssExMxPmIpMcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of MC route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be written
*                                from this base on.
*       numOfRouteEntries      - The number of route entries to write.
*       routeEntriesArrayPtr   - The MC route entry array to write.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesWriteSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    gIPUCnumOfMcEntries = 0;/*reset on first*/

    if(gIPUCrouteMcEntryArrayPtr == NULL)
    {
        gIPUCrouteMcEntryArrayPtr = (CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC*)
                    cmdOsMalloc(sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC));
    }
    if(gIPUCrouteMcEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gIPUCnumOfMcEntries)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }
    gIPUCrouteMcEntryArrayPtr[ind].command=
                                 (CPSS_PACKET_CMD_ENT)inFields[1];
    gIPUCrouteMcEntryArrayPtr[ind].cpuCodeIndex =
                                  (GT_U32)inFields[2];
    gIPUCrouteMcEntryArrayPtr[ind].appSpecificCpuCodeEnable =
                                                          (GT_BOOL)inFields[3];
    gIPUCrouteMcEntryArrayPtr[ind].hopLimitMode =
                                (GT_TRUE == (GT_BOOL)inFields[4])?
                                CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
    gIPUCrouteMcEntryArrayPtr[ind].ttlMode =
                                (GT_TRUE == (GT_BOOL)inFields[4])?
                                CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
    gIPUCrouteMcEntryArrayPtr[ind].ttl = 0;

    gIPUCrouteMcEntryArrayPtr[ind].bypassTtlExceptionCheckEnable =
                                                          (GT_BOOL)inFields[5];
    gIPUCrouteMcEntryArrayPtr[ind].mirrorToIngressAnalyzer =
                                                          (GT_BOOL)inFields[6];

    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyDp =
                                                          (GT_BOOL)inFields[7];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyDscp =
                                                         (GT_BOOL)inFields[8];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyExp =
                                                         (GT_BOOL)inFields[9];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyTc=
                                                         (GT_BOOL)inFields[10];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyUp =
                                                         (GT_BOOL)inFields[11];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.dp = (CPSS_DP_LEVEL_ENT)inFields[12];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.dscp = (GT_U32)inFields[13];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.exp = (GT_U32)inFields[14];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.tc = (GT_U32)inFields[15];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.up= (GT_U32)inFields[16];
    gIPUCrouteMcEntryArrayPtr[ind].cntrSetIndex =
                      (CPSS_EXMXPM_IP_COUNTER_SET_INDEX_ENT)inFields[17];

    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIfMode =(CPSS_EXMXPM_RPF_IF_MODE_ENT)inFields[18];
    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.type = CPSS_INTERFACE_VID_E;
    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.vlanId = (GT_U16)inFields[19];

    if (inFields[20] == 0) /* previous rpf command mode - now obsolete */
    {
        switch ((CPSS_PACKET_CMD_ENT)inFields[21])
        {
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_TRAP_TO_CPU_E;
                break;
            case CPSS_PACKET_CMD_DROP_HARD_E:
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand =CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E;
                break;
            case CPSS_PACKET_CMD_DROP_SOFT_E:
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand =CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_SOFT_E;
                break;
            case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand =CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_BRIDGE_AND_MIRROR_E;
                break;
            case CPSS_PACKET_CMD_BRIDGE_E:
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand =CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_BRIDGE_E;
                break;
            default:
                galtisOutput(outArgs, GT_BAD_STATE, "");
                break;
        }
    }
    else
    {
        gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DOWNSTREAM_INTERFACE_E;
    }

    gIPUCrouteMcEntryArrayPtr[ind].ipv6ScopeCheckEnable = (GT_BOOL)inFields[22];
    gIPUCrouteMcEntryArrayPtr[ind].ipv6ScopeLevel = (GT_U32)inFields[23];

    gIPUCrouteMcEntryArrayPtr[ind].ipv6DestinationSiteId =
                                                     (CPSS_IP_SITE_ID_ENT)inFields[24];
    gIPUCrouteMcEntryArrayPtr[ind].mtuProfileIndex = (GT_U32)inFields[25];
    gIPUCrouteMcEntryArrayPtr[ind].ageRefresh =(GT_BOOL)inFields[26];

    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_DIT_E;
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.outlifPointer.ditPtr =(GT_U32)inFields[27];

    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.vidx = 0xFFF;

    gIPUCrouteMcEntryArrayPtr[ind].mplsCommand = CPSS_EXMXPM_MPLS_NOP_CMD_E;
    gIPUCrouteMcEntryArrayPtr[ind].mplsLabel = 0;

    gIPUCnumOfMcEntries++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpMcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of MC route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be written
*                                from this base on.
*       numOfRouteEntries      - The number of route entries to write.
*       routeEntriesArrayPtr   - The MC route entry array to write.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesWriteSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    gIPUCrouteMcEntryArrayPtr = (CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC*)
       cmdOsRealloc(gIPUCrouteMcEntryArrayPtr,
       sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC) * (gIPUCnumOfMcEntries+1));

      if(gIPUCrouteMcEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }
    ind = (GT_U32)inFields[0];

    if(ind > gIPUCnumOfMcEntries)
    {
      galtisOutput(outArgs, GT_BAD_VALUE,"");
      return CMD_OK;
    }
    gIPUCrouteMcEntryArrayPtr[ind].command=
                                 (CPSS_PACKET_CMD_ENT)inFields[1];
    gIPUCrouteMcEntryArrayPtr[ind].cpuCodeIndex =
                                  (GT_U32)inFields[2];
    gIPUCrouteMcEntryArrayPtr[ind].appSpecificCpuCodeEnable =
                                                          (GT_BOOL)inFields[3];
    gIPUCrouteMcEntryArrayPtr[ind].hopLimitMode =
                                (GT_TRUE == (GT_BOOL)inFields[4])?
                                CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
    gIPUCrouteMcEntryArrayPtr[ind].ttlMode =
                                (GT_TRUE == (GT_BOOL)inFields[4])?
                                CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
    gIPUCrouteMcEntryArrayPtr[ind].ttl = 0;
    gIPUCrouteMcEntryArrayPtr[ind].bypassTtlExceptionCheckEnable =

        (GT_BOOL)inFields[5];
    gIPUCrouteMcEntryArrayPtr[ind].mirrorToIngressAnalyzer =
                                                          (GT_BOOL)inFields[6];

    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyDp =
                                                          (GT_BOOL)inFields[7];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyDscp =
                                                         (GT_BOOL)inFields[8];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyExp =
                                                         (GT_BOOL)inFields[9];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyTc=
                                                         (GT_BOOL)inFields[10];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyUp =
                                                         (GT_BOOL)inFields[11];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.dp = (CPSS_DP_LEVEL_ENT)inFields[12];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.dscp = (GT_U32)inFields[13];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.exp = (GT_U32)inFields[14];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.tc = (GT_U32)inFields[15];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.up= (GT_U32)inFields[16];
    gIPUCrouteMcEntryArrayPtr[ind].cntrSetIndex =
                      (CPSS_EXMXPM_IP_COUNTER_SET_INDEX_ENT)inFields[17];

    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIfMode =(CPSS_EXMXPM_RPF_IF_MODE_ENT)inFields[18];
    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.type = CPSS_INTERFACE_VID_E;
    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.vlanId = (GT_U16)inFields[19];

    if (inFields[20] == 0) /* previous rpf command mode - now obsolete */
    {
        switch ((CPSS_PACKET_CMD_ENT)inFields[21])
        {
            case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_TRAP_TO_CPU_E;
                break;
            case CPSS_PACKET_CMD_DROP_HARD_E:
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E;
                break;
            case CPSS_PACKET_CMD_DROP_SOFT_E:
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_SOFT_E;
                break;
            case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_BRIDGE_AND_MIRROR_E;
                break;
            case CPSS_PACKET_CMD_BRIDGE_E:
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_BRIDGE_E;
                break;
            default:
                galtisOutput(outArgs, GT_BAD_STATE, "");
                break;
        }
    }
    else
    {
        gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DOWNSTREAM_INTERFACE_E;
    }

    gIPUCrouteMcEntryArrayPtr[ind].ipv6ScopeCheckEnable = (GT_BOOL)inFields[22];
    gIPUCrouteMcEntryArrayPtr[ind].ipv6ScopeLevel = (GT_U32)inFields[23];

    gIPUCrouteMcEntryArrayPtr[ind].ipv6DestinationSiteId =
                                                     (CPSS_IP_SITE_ID_ENT)inFields[24];
    gIPUCrouteMcEntryArrayPtr[ind].mtuProfileIndex = (GT_U32)inFields[25];
    gIPUCrouteMcEntryArrayPtr[ind].ageRefresh =(GT_BOOL)inFields[26];

    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_DIT_E;
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.outlifPointer.ditPtr =(GT_U32)inFields[27];

    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.vidx = 0xFFF;

    gIPUCrouteMcEntryArrayPtr[ind].mplsCommand = CPSS_EXMXPM_MPLS_NOP_CMD_E;
    gIPUCrouteMcEntryArrayPtr[ind].mplsLabel = 0;

    gIPUCnumOfMcEntries++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpMcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of MC route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be written
*                                from this base on.
*       numOfRouteEntries      - The number of route entries to write.
*       routeEntriesArrayPtr   - The MC route entry array to write.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/

static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesWriteEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8    devNum;
    GT_U32   ucMcRouteEntryBaseIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    ucMcRouteEntryBaseIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
   result =  cpssExMxPmIpMcRouteEntriesWrite(devNum,ucMcRouteEntryBaseIndex,
                                           gIPUCnumOfMcEntries,
                                           gIPUCrouteMcEntryArrayPtr);
   if (result != GT_OK)
   {
       galtisOutput(outArgs, result, "");
       return CMD_OK;
   }


   cmdOsFree(gIPUCrouteMcEntryArrayPtr);
   gIPUCrouteMcEntryArrayPtr = NULL;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmIpMcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of MC route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be written
*                                from this base on.
*       numOfRouteEntries      - The number of route entries to write.
*       routeEntriesArrayPtr   - The MC route entry array to write.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/

static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesWriteCancelSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsFree(gIPUCrouteMcEntryArrayPtr);
    gIPUCrouteMcEntryArrayPtr = NULL;

    galtisOutput(outArgs, GT_BAD_VALUE,"");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpMcRouteEntriesRead
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Read an array of MC route entries from the hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be read
*                                from this base on.
*       numOfRouteEntries      - The number route entries to read.
*
* OUTPUTS:
*       routeEntriesArrayPtr   - The MC route entry array to read into.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesReadGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ucMcRouteEntryBaseIndex;
    GT_U32  ind;
    GT_U32 numToRefreash;/*number of entries to get from table*/

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gIPUCgetRouteMcEntryCurrIndx = 0;/*reset on first*/
    ind = gIPUCgetRouteMcEntryCurrIndx;


    /* map input arguments to locals */
     devNum= (GT_U8)inArgs[0];
     ucMcRouteEntryBaseIndex=(GT_U32)inArgs[1];
     numToRefreash=(GT_U32)inArgs[2];
     gIPUCgetNumOfMcEntries=numToRefreash;

     if(gIPUCgetRouteMcEntryArrayPtr == NULL)
    {
        gIPUCgetRouteMcEntryArrayPtr = (CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC*)
                    cmdOsMalloc(sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC)*numToRefreash);
    }
    if(gIPUCgetRouteMcEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

  /* call cpss api function */
    cmdOsMemSet(gIPUCgetRouteMcEntryArrayPtr,0,
                sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC) * gIPUCgetNumOfMcEntries);

    /* call cpss api function */
    result = cpssExMxPmIpMcRouteEntriesRead(devNum, ucMcRouteEntryBaseIndex, gIPUCgetNumOfMcEntries, gIPUCgetRouteMcEntryArrayPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gIPUCgetRouteMcEntryCurrIndx;

    inFields[1]=gIPUCgetRouteMcEntryArrayPtr[ind].command;

    inFields[2]=gIPUCgetRouteMcEntryArrayPtr[ind].cpuCodeIndex;

    inFields[3]=gIPUCgetRouteMcEntryArrayPtr[ind].appSpecificCpuCodeEnable;

    inFields[4] = (gIPUCgetRouteMcEntryArrayPtr[ind].hopLimitMode == CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E)?GT_TRUE:GT_FALSE;

    inFields[5]= gIPUCgetRouteMcEntryArrayPtr[ind].bypassTtlExceptionCheckEnable;

    inFields[6] =gIPUCgetRouteMcEntryArrayPtr[ind].mirrorToIngressAnalyzer;

    inFields[7]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyDp;

    inFields[8]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyDscp;

    inFields[9]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyExp;

    inFields[10]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyTc;

    inFields[11]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyUp;

    inFields[12] = gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.dp;

    inFields[13]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.dscp;

    inFields[14] =gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.exp;

    inFields[15]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.tc;

    inFields[16]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.up;

    inFields[17]=gIPUCgetRouteMcEntryArrayPtr[ind].cntrSetIndex;

    inFields[18]=gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIfMode;

    switch (gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.type)
    {
        case CPSS_INTERFACE_VID_E:
            inFields[19] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.vlanId;
            break;
        case CPSS_INTERFACE_PORT_E:
            inFields[19] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.devPort.portNum;
            break;
        case CPSS_INTERFACE_TRUNK_E:
            inFields[19] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.trunkId;
            break;
        default:
            inFields[19]=0;
            break;
    }

    /* previous rpf fail command mode - now obsolete */
    if (gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfFailCommand == CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DOWNSTREAM_INTERFACE_E)
    {
        inFields[20] = 1;
        inFields[21] = CPSS_PACKET_CMD_BRIDGE_E;
    }
    else
    {
        inFields[20] = 0;
        switch (gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfFailCommand)
        {
            case CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_TRAP_TO_CPU_E:
                inFields[21] = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
                break;
            case CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E:
                inFields[21] = CPSS_PACKET_CMD_DROP_HARD_E;
                break;
            case CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_SOFT_E:
                inFields[21] = CPSS_PACKET_CMD_DROP_SOFT_E;
                break;
            case CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_BRIDGE_AND_MIRROR_E:
                inFields[21] = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
                break;
            case CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_BRIDGE_E:
                inFields[21] = CPSS_PACKET_CMD_BRIDGE_E;
                break;
            default:
                inFields[21] = CPSS_PACKET_CMD_NONE_E;
        }
    }

    inFields[22]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6ScopeCheckEnable;

    inFields[23]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6ScopeLevel;

    inFields[24]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6DestinationSiteId;

    inFields[25]=gIPUCgetRouteMcEntryArrayPtr[ind].mtuProfileIndex;

    inFields[26]=gIPUCgetRouteMcEntryArrayPtr[ind].ageRefresh;

    inFields[27]=gIPUCgetRouteMcEntryArrayPtr[ind].nextHopIf.outlifPointer.ditPtr;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                ,inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29]);

    galtisOutput(outArgs, result, "%f");

    gIPUCgetRouteMcEntryCurrIndx++;

    return CMD_OK;
}

 /*******************************************************************************
* cpssExMxPmIpMcRouteEntriesRead
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Read an array of MC route entries from the hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be read
*                                from this base on.
*       numOfRouteEntries      - The number route entries to read.
*
* OUTPUTS:
*       routeEntriesArrayPtr   - The MC route entry array to read into.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/
 static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesReadGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32    ind = gIPUCgetRouteMcEntryCurrIndx;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(gIPUCgetRouteMcEntryCurrIndx >= gIPUCgetNumOfMcEntries)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    /* map input arguments to locals */


    inFields[0] = gIPUCgetRouteMcEntryCurrIndx;

    inFields[1]=gIPUCgetRouteMcEntryArrayPtr[ind].command;

    inFields[2]=gIPUCgetRouteMcEntryArrayPtr[ind].cpuCodeIndex;

    inFields[3]=gIPUCgetRouteMcEntryArrayPtr[ind].appSpecificCpuCodeEnable;

    inFields[4] = (gIPUCgetRouteMcEntryArrayPtr[ind].hopLimitMode == CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E)?GT_TRUE:GT_FALSE;

    inFields[5]= gIPUCgetRouteMcEntryArrayPtr[ind].bypassTtlExceptionCheckEnable;

    inFields[6] =gIPUCgetRouteMcEntryArrayPtr[ind].mirrorToIngressAnalyzer;

    inFields[7]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyDp;

    inFields[8]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyDscp;

    inFields[9]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyExp;

    inFields[10]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyTc;

    inFields[11]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyUp;

    inFields[12] = gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.dp;

    inFields[13]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.dscp;

    inFields[14] =gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.exp;

    inFields[15]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.tc;

    inFields[16]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.up;

    inFields[17]=gIPUCgetRouteMcEntryArrayPtr[ind].cntrSetIndex;

    inFields[18]=gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIfMode;

    switch (gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.type)
    {
        case CPSS_INTERFACE_VID_E:
            inFields[19] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.vlanId;
            break;
        case CPSS_INTERFACE_PORT_E:
            inFields[19] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.devPort.portNum;
            break;
        case CPSS_INTERFACE_TRUNK_E:
            inFields[19] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.trunkId;
            break;
        default:
            inFields[19]=0;
            break;
    }

    /* previous rpf fail command mode - now obsolete */
    if (gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfFailCommand == CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DOWNSTREAM_INTERFACE_E)
    {
        inFields[20] = 1;
        inFields[21] = CPSS_PACKET_CMD_BRIDGE_E;
    }
    else
    {
        inFields[20] = 0;
        switch (gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfFailCommand)
        {
            case CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_TRAP_TO_CPU_E:
                inFields[21] = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
                break;
            case CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E:
                inFields[21] = CPSS_PACKET_CMD_DROP_HARD_E;
                break;
            case CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_SOFT_E:
                inFields[21] = CPSS_PACKET_CMD_DROP_SOFT_E;
                break;
            case CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_BRIDGE_AND_MIRROR_E:
                inFields[21] = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
                break;
            case CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_BRIDGE_E:
                inFields[21] = CPSS_PACKET_CMD_BRIDGE_E;
                break;
            default:
                inFields[21] = CPSS_PACKET_CMD_NONE_E;
        }
    }

    inFields[22]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6ScopeCheckEnable;

    inFields[23]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6ScopeLevel;

    inFields[24]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6DestinationSiteId;

    inFields[25]=gIPUCgetRouteMcEntryArrayPtr[ind].mtuProfileIndex;

    inFields[26]=gIPUCgetRouteMcEntryArrayPtr[ind].ageRefresh;

    inFields[27]=gIPUCgetRouteMcEntryArrayPtr[ind].nextHopIf.outlifPointer.ditPtr;



/* pack and output table fields */
fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
            ,inFields[0], inFields[1], inFields[2], inFields[3],
            inFields[4], inFields[5], inFields[6], inFields[7],
            inFields[8], inFields[9], inFields[10], inFields[11],
            inFields[12], inFields[13], inFields[14], inFields[15],
            inFields[16], inFields[17], inFields[18], inFields[19],
            inFields[20], inFields[21], inFields[22], inFields[23],
            inFields[24], inFields[25], inFields[26], inFields[27],
            inFields[28], inFields[29]);



    galtisOutput(outArgs, GT_OK, "%f");

    gIPUCgetRouteMcEntryCurrIndx++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpMcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of MC route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be written
*                                from this base on.
*       numOfRouteEntries      - The number of route entries to write.
*       routeEntriesArrayPtr   - The MC route entry array to write.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesWriteSetFirst_1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    gIPUCnumOfMcEntries = 0;/*reset on first*/

    if(gIPUCrouteMcEntryArrayPtr == NULL)
    {
        gIPUCrouteMcEntryArrayPtr = (CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC*)
                    cmdOsMalloc(sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC));
    }
    if(gIPUCrouteMcEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gIPUCnumOfMcEntries)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }
    gIPUCrouteMcEntryArrayPtr[ind].command=
                                 (CPSS_PACKET_CMD_ENT)inFields[1];
    gIPUCrouteMcEntryArrayPtr[ind].cpuCodeIndex =
                                  (GT_U32)inFields[2];
    gIPUCrouteMcEntryArrayPtr[ind].appSpecificCpuCodeEnable =
                                                          (GT_BOOL)inFields[3];
    gIPUCrouteMcEntryArrayPtr[ind].hopLimitMode =
                                (GT_TRUE == (GT_BOOL)inFields[4])?
                                CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
    gIPUCrouteMcEntryArrayPtr[ind].ttlMode =
                                (GT_TRUE == (GT_BOOL)inFields[4])?
                                CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
    gIPUCrouteMcEntryArrayPtr[ind].ttl = 0;

    gIPUCrouteMcEntryArrayPtr[ind].bypassTtlExceptionCheckEnable =
                                                          (GT_BOOL)inFields[5];
    gIPUCrouteMcEntryArrayPtr[ind].mirrorToIngressAnalyzer =
                                                          (GT_BOOL)inFields[6];

    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyDp =
                                                          (GT_BOOL)inFields[7];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyDscp =
                                                         (GT_BOOL)inFields[8];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyExp =
                                                         (GT_BOOL)inFields[9];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyTc=
                                                         (GT_BOOL)inFields[10];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyUp =
                                                         (GT_BOOL)inFields[11];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.dp = (CPSS_DP_LEVEL_ENT)inFields[12];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.dscp = (GT_U32)inFields[13];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.exp = (GT_U32)inFields[14];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.tc = (GT_U32)inFields[15];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.up= (GT_U32)inFields[16];
    gIPUCrouteMcEntryArrayPtr[ind].cntrSetIndex =
                      (CPSS_EXMXPM_IP_COUNTER_SET_INDEX_ENT)inFields[17];

    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIfMode =(CPSS_EXMXPM_RPF_IF_MODE_ENT)inFields[18];

    switch ((gIPUCrouteMcEntryArrayPtr[ind].mcRpfIfMode))
        {
            case CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E:
            case CPSS_EXMXPM_RPF_IF_MODE_VLAN_INLIF_E:
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.type = CPSS_INTERFACE_VID_E;
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.vlanId = (GT_U16)inFields[19];
                break;
            case CPSS_EXMXPM_RPF_IF_MODE_PORT_E:
                if ((GT_U16)inFields[19] & 0x1)
                {
                    /* trunk */
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.type = CPSS_INTERFACE_TRUNK_E;
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.trunkId = ((GT_U16)inFields[19] >> 1) & 0xff;
                }
                else
                {
                    /* port */
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.type = CPSS_INTERFACE_PORT_E;
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.devPort.portNum = ((GT_U16)inFields[19] >> 1) & 0x3f;
                }
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand =CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E;
                break;
            case CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E:
                break;
            default:
                galtisOutput(outArgs, GT_BAD_STATE, "");
                break;
        }

    gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand = (CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_ENT)inFields[20];

    gIPUCrouteMcEntryArrayPtr[ind].ipv6ScopeCheckEnable = (GT_BOOL)inFields[21];
    gIPUCrouteMcEntryArrayPtr[ind].ipv6ScopeLevel = (GT_U32)inFields[22];

    gIPUCrouteMcEntryArrayPtr[ind].ipv6DestinationSiteId =
                                                     (CPSS_IP_SITE_ID_ENT)inFields[23];
    gIPUCrouteMcEntryArrayPtr[ind].mtuProfileIndex = (GT_U32)inFields[24];
    gIPUCrouteMcEntryArrayPtr[ind].ageRefresh =(GT_BOOL)inFields[25];

    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_DIT_E;
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.outlifPointer.ditPtr =(GT_U32)inFields[26];

    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.vidx = 0xFFF;

    gIPUCrouteMcEntryArrayPtr[ind].mplsCommand = CPSS_EXMXPM_MPLS_NOP_CMD_E;
    gIPUCrouteMcEntryArrayPtr[ind].mplsLabel = 0;

    gIPUCnumOfMcEntries++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpMcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of MC route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be written
*                                from this base on.
*       numOfRouteEntries      - The number of route entries to write.
*       routeEntriesArrayPtr   - The MC route entry array to write.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesWriteSetNext_1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    gIPUCrouteMcEntryArrayPtr = (CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC*)
       cmdOsRealloc(gIPUCrouteMcEntryArrayPtr,
       sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC) * (gIPUCnumOfMcEntries+1));

      if(gIPUCrouteMcEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }
    ind = (GT_U32)inFields[0];

    if(ind > gIPUCnumOfMcEntries)
    {
      galtisOutput(outArgs, GT_BAD_VALUE,"");
      return CMD_OK;
    }
    gIPUCrouteMcEntryArrayPtr[ind].command=
                                 (CPSS_PACKET_CMD_ENT)inFields[1];
    gIPUCrouteMcEntryArrayPtr[ind].cpuCodeIndex =
                                  (GT_U32)inFields[2];
    gIPUCrouteMcEntryArrayPtr[ind].appSpecificCpuCodeEnable =
                                                          (GT_BOOL)inFields[3];
    gIPUCrouteMcEntryArrayPtr[ind].hopLimitMode =
                                (GT_TRUE == (GT_BOOL)inFields[4])?
                                CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
    gIPUCrouteMcEntryArrayPtr[ind].ttlMode =
                                (GT_TRUE == (GT_BOOL)inFields[4])?
                                CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
    gIPUCrouteMcEntryArrayPtr[ind].ttl = 0;
    gIPUCrouteMcEntryArrayPtr[ind].bypassTtlExceptionCheckEnable =

        (GT_BOOL)inFields[5];
    gIPUCrouteMcEntryArrayPtr[ind].mirrorToIngressAnalyzer =
                                                          (GT_BOOL)inFields[6];

    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyDp =
                                                          (GT_BOOL)inFields[7];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyDscp =
                                                         (GT_BOOL)inFields[8];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyExp =
                                                         (GT_BOOL)inFields[9];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyTc=
                                                         (GT_BOOL)inFields[10];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyUp =
                                                         (GT_BOOL)inFields[11];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.dp = (CPSS_DP_LEVEL_ENT)inFields[12];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.dscp = (GT_U32)inFields[13];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.exp = (GT_U32)inFields[14];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.tc = (GT_U32)inFields[15];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.up= (GT_U32)inFields[16];
    gIPUCrouteMcEntryArrayPtr[ind].cntrSetIndex =
                      (CPSS_EXMXPM_IP_COUNTER_SET_INDEX_ENT)inFields[17];

    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIfMode =(CPSS_EXMXPM_RPF_IF_MODE_ENT)inFields[18];

    switch ((gIPUCrouteMcEntryArrayPtr[ind].mcRpfIfMode))
        {
            case CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E:
            case CPSS_EXMXPM_RPF_IF_MODE_VLAN_INLIF_E:
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.type = CPSS_INTERFACE_VID_E;
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.vlanId = (GT_U16)inFields[19];
                break;
            case CPSS_EXMXPM_RPF_IF_MODE_PORT_E:
                if ((GT_U16)inFields[19] & 0x1)
                {
                    /* trunk */
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.type = CPSS_INTERFACE_TRUNK_E;
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.trunkId = ((GT_U16)inFields[19] >> 1) & 0xff;
                }
                else
                {
                    /* port */
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.type = CPSS_INTERFACE_PORT_E;
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.devPort.portNum = ((GT_U16)inFields[19] >> 1) & 0x3f;
                }
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand =CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E;
                break;
            case CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E:
                break;
            default:
                galtisOutput(outArgs, GT_BAD_STATE, "");
                break;
        }

    gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand = (CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_ENT)inFields[20];

    gIPUCrouteMcEntryArrayPtr[ind].ipv6ScopeCheckEnable = (GT_BOOL)inFields[21];
    gIPUCrouteMcEntryArrayPtr[ind].ipv6ScopeLevel = (GT_U32)inFields[22];

    gIPUCrouteMcEntryArrayPtr[ind].ipv6DestinationSiteId =
                                                     (CPSS_IP_SITE_ID_ENT)inFields[23];
    gIPUCrouteMcEntryArrayPtr[ind].mtuProfileIndex = (GT_U32)inFields[24];
    gIPUCrouteMcEntryArrayPtr[ind].ageRefresh =(GT_BOOL)inFields[25];

    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_DIT_E;
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.outlifPointer.ditPtr =(GT_U32)inFields[26];

    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.vidx = 0xFFF;

    gIPUCrouteMcEntryArrayPtr[ind].mplsCommand = CPSS_EXMXPM_MPLS_NOP_CMD_E;
    gIPUCrouteMcEntryArrayPtr[ind].mplsLabel = 0;

    gIPUCnumOfMcEntries++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmIpMcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of MC route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be written
*                                from this base on.
*       numOfRouteEntries      - The number of route entries to write.
*       routeEntriesArrayPtr   - The MC route entry array to write.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/

static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesWriteEndSet_1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8    devNum;
    GT_U32   ucMcRouteEntryBaseIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    ucMcRouteEntryBaseIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
   result =  cpssExMxPmIpMcRouteEntriesWrite(devNum,ucMcRouteEntryBaseIndex,
                                           gIPUCnumOfMcEntries,
                                           gIPUCrouteMcEntryArrayPtr);
   if (result != GT_OK)
   {
       galtisOutput(outArgs, result, "");
       return CMD_OK;
   }


   cmdOsFree(gIPUCrouteMcEntryArrayPtr);
   gIPUCrouteMcEntryArrayPtr = NULL;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmIpMcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of MC route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be written
*                                from this base on.
*       numOfRouteEntries      - The number of route entries to write.
*       routeEntriesArrayPtr   - The MC route entry array to write.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/

static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesWriteCancelSet_1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsFree(gIPUCrouteMcEntryArrayPtr);
    gIPUCrouteMcEntryArrayPtr = NULL;

    galtisOutput(outArgs, GT_BAD_VALUE,"");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmIpMcRouteEntriesRead
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Read an array of MC route entries from the hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be read
*                                from this base on.
*       numOfRouteEntries      - The number route entries to read.
*
* OUTPUTS:
*       routeEntriesArrayPtr   - The MC route entry array to read into.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesReadGetFirst_1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ucMcRouteEntryBaseIndex;
    GT_U32  ind;
    GT_U32 numToRefreash;/*number of entries to get from table*/

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gIPUCgetRouteMcEntryCurrIndx = 0;/*reset on first*/
    ind = gIPUCgetRouteMcEntryCurrIndx;


    /* map input arguments to locals */
     devNum= (GT_U8)inArgs[0];
     ucMcRouteEntryBaseIndex=(GT_U32)inArgs[1];
     numToRefreash=(GT_U32)inArgs[2];
     gIPUCgetNumOfMcEntries=numToRefreash;

     if(gIPUCgetRouteMcEntryArrayPtr == NULL)
    {
        gIPUCgetRouteMcEntryArrayPtr = (CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC*)
                    cmdOsMalloc(sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC)*numToRefreash);
    }
    if(gIPUCgetRouteMcEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

  /* call cpss api function */
    cmdOsMemSet(gIPUCgetRouteMcEntryArrayPtr,0,
                sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC) * gIPUCgetNumOfMcEntries);

    /* call cpss api function */
    result = cpssExMxPmIpMcRouteEntriesRead(devNum, ucMcRouteEntryBaseIndex, gIPUCgetNumOfMcEntries, gIPUCgetRouteMcEntryArrayPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gIPUCgetRouteMcEntryCurrIndx;

    inFields[1]=gIPUCgetRouteMcEntryArrayPtr[ind].command;

    inFields[2]=gIPUCgetRouteMcEntryArrayPtr[ind].cpuCodeIndex;

    inFields[3]=gIPUCgetRouteMcEntryArrayPtr[ind].appSpecificCpuCodeEnable;

    inFields[4] = (gIPUCgetRouteMcEntryArrayPtr[ind].hopLimitMode == CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E)?GT_TRUE:GT_FALSE;

    inFields[5]= gIPUCgetRouteMcEntryArrayPtr[ind].bypassTtlExceptionCheckEnable;

    inFields[6] =gIPUCgetRouteMcEntryArrayPtr[ind].mirrorToIngressAnalyzer;

    inFields[7]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyDp;

    inFields[8]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyDscp;

    inFields[9]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyExp;

    inFields[10]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyTc;

    inFields[11]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyUp;

    inFields[12] = gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.dp;

    inFields[13]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.dscp;

    inFields[14] =gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.exp;

    inFields[15]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.tc;

    inFields[16]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.up;

    inFields[17]=gIPUCgetRouteMcEntryArrayPtr[ind].cntrSetIndex;

    inFields[18]=gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIfMode;

    switch (gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.type)
    {
        case CPSS_INTERFACE_VID_E:
            inFields[19] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.vlanId;
            break;
        case CPSS_INTERFACE_PORT_E:
            inFields[19] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.devPort.portNum << 1;
            break;
        case CPSS_INTERFACE_TRUNK_E:
            inFields[19] = (gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.trunkId << 1)| 0x1;
            break;
        default:
            inFields[19]=0;
            break;
    }
    inFields[20] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfFailCommand;
    /* previous rpf fail command mode - now obsolete */

    inFields[21]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6ScopeCheckEnable;

    inFields[22]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6ScopeLevel;

    inFields[23]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6DestinationSiteId;

    inFields[24]=gIPUCgetRouteMcEntryArrayPtr[ind].mtuProfileIndex;

    inFields[25]=gIPUCgetRouteMcEntryArrayPtr[ind].ageRefresh;

    inFields[26]=gIPUCgetRouteMcEntryArrayPtr[ind].nextHopIf.outlifPointer.ditPtr;
    inFields[27] = 0;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                ,inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26],inFields[27]);

    galtisOutput(outArgs, result, "%f");

    gIPUCgetRouteMcEntryCurrIndx++;

    return CMD_OK;

}
 /*******************************************************************************
* cpssExMxPmIpMcRouteEntriesRead
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Read an array of MC route entries from the hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be read
*                                from this base on.
*       numOfRouteEntries      - The number route entries to read.
*
* OUTPUTS:
*       routeEntriesArrayPtr   - The MC route entry array to read into.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/
 static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesReadGetNext_1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32    ind = gIPUCgetRouteMcEntryCurrIndx;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(gIPUCgetRouteMcEntryCurrIndx >= gIPUCgetNumOfMcEntries)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    /* map input arguments to locals */


    inFields[0] = gIPUCgetRouteMcEntryCurrIndx;

    inFields[1]=gIPUCgetRouteMcEntryArrayPtr[ind].command;

    inFields[2]=gIPUCgetRouteMcEntryArrayPtr[ind].cpuCodeIndex;

    inFields[3]=gIPUCgetRouteMcEntryArrayPtr[ind].appSpecificCpuCodeEnable;

    inFields[4] = (gIPUCgetRouteMcEntryArrayPtr[ind].hopLimitMode == CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E)?GT_TRUE:GT_FALSE;

    inFields[5]= gIPUCgetRouteMcEntryArrayPtr[ind].bypassTtlExceptionCheckEnable;

    inFields[6] =gIPUCgetRouteMcEntryArrayPtr[ind].mirrorToIngressAnalyzer;

    inFields[7]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyDp;

    inFields[8]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyDscp;

    inFields[9]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyExp;

    inFields[10]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyTc;

    inFields[11]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyUp;

    inFields[12] = gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.dp;

    inFields[13]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.dscp;

    inFields[14] =gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.exp;

    inFields[15]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.tc;

    inFields[16]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.up;

    inFields[17]=gIPUCgetRouteMcEntryArrayPtr[ind].cntrSetIndex;

    inFields[18]=gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIfMode;

    switch (gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.type)
    {
        case CPSS_INTERFACE_VID_E:
            inFields[19] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.vlanId;
            break;
        case CPSS_INTERFACE_PORT_E:
            inFields[19] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.devPort.portNum << 1;
            break;
        case CPSS_INTERFACE_TRUNK_E:
            inFields[19] = (gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.trunkId << 1)| 0x1;
            break;
        default:
            inFields[19]=0;
            break;
    }

    inFields[20] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfFailCommand;
    /* previous rpf fail command mode - now obsolete */

    inFields[21]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6ScopeCheckEnable;

    inFields[22]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6ScopeLevel;

    inFields[23]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6DestinationSiteId;

    inFields[24]=gIPUCgetRouteMcEntryArrayPtr[ind].mtuProfileIndex;

    inFields[25]=gIPUCgetRouteMcEntryArrayPtr[ind].ageRefresh;

    inFields[26]=gIPUCgetRouteMcEntryArrayPtr[ind].nextHopIf.outlifPointer.ditPtr;
    inFields[27] = 0;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                ,inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26],inFields[27]);

    galtisOutput(outArgs, GT_OK, "%f");

    gIPUCgetRouteMcEntryCurrIndx++;

    return CMD_OK;
}

/******************************************************************************
* cpssExMxPmIpMcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of MC route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be written
*                                from this base on.
*       numOfRouteEntries      - The number of route entries to write.
*       routeEntriesArrayPtr   - The MC route entry array to write.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesWriteSetFirst_2
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32      ind;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    gIPUCnumOfMcEntries = 0;/*reset on first*/

    if(gIPUCrouteMcEntryArrayPtr == NULL)
    {
        gIPUCrouteMcEntryArrayPtr = (CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC*)
                    cmdOsMalloc(sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC));
    }
    if(gIPUCrouteMcEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gIPUCnumOfMcEntries)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }
    gIPUCrouteMcEntryArrayPtr[ind].command=
                                 (CPSS_PACKET_CMD_ENT)inFields[1];
    gIPUCrouteMcEntryArrayPtr[ind].cpuCodeIndex =
                                  (GT_U32)inFields[2];
    gIPUCrouteMcEntryArrayPtr[ind].appSpecificCpuCodeEnable =
                                                          (GT_BOOL)inFields[3];
    gIPUCrouteMcEntryArrayPtr[ind].hopLimitMode =
                                (GT_TRUE == (GT_BOOL)inFields[4])?
                                CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
    gIPUCrouteMcEntryArrayPtr[ind].ttlMode =
                                (GT_TRUE == (GT_BOOL)inFields[4])?
                                CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
    gIPUCrouteMcEntryArrayPtr[ind].ttl = 0;

    gIPUCrouteMcEntryArrayPtr[ind].bypassTtlExceptionCheckEnable =
                                                          (GT_BOOL)inFields[5];
    gIPUCrouteMcEntryArrayPtr[ind].mirrorToIngressAnalyzer =
                                                          (GT_BOOL)inFields[6];

    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyDp =
                                                          (GT_BOOL)inFields[7];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyDscp =
                                                         (GT_BOOL)inFields[8];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyExp =
                                                         (GT_BOOL)inFields[9];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyTc=
                                                         (GT_BOOL)inFields[10];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyUp =
                                                         (GT_BOOL)inFields[11];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.dp = (CPSS_DP_LEVEL_ENT)inFields[12];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.dscp = (GT_U32)inFields[13];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.exp = (GT_U32)inFields[14];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.tc = (GT_U32)inFields[15];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.up= (GT_U32)inFields[16];
    gIPUCrouteMcEntryArrayPtr[ind].cntrSetIndex =
                      (CPSS_EXMXPM_IP_COUNTER_SET_INDEX_ENT)inFields[17];

    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIfMode =(CPSS_EXMXPM_RPF_IF_MODE_ENT)inFields[18];

    switch ((gIPUCrouteMcEntryArrayPtr[ind].mcRpfIfMode))
        {
            case CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E:
            case CPSS_EXMXPM_RPF_IF_MODE_VLAN_INLIF_E:
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.type = CPSS_INTERFACE_VID_E;
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.vlanId = (GT_U16)inFields[19];
                break;
            case CPSS_EXMXPM_RPF_IF_MODE_PORT_E:
                if ((GT_U16)inFields[19] & 0x1)
                {
                    /* trunk */
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.type = CPSS_INTERFACE_TRUNK_E;
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.trunkId = ((GT_U16)inFields[19] >> 1) & 0xff;
                }
                else
                {
                    /* port */
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.type = CPSS_INTERFACE_PORT_E;
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.devPort.portNum = ((GT_U16)inFields[19] >> 1) & 0x3f;
                }
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand =CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E;
                break;
            case CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E:
                break;
            default:
                galtisOutput(outArgs, GT_BAD_STATE, "");
                break;
        }

    gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand = (CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_ENT)inFields[20];

    gIPUCrouteMcEntryArrayPtr[ind].ipv6ScopeCheckEnable = (GT_BOOL)inFields[21];
    gIPUCrouteMcEntryArrayPtr[ind].ipv6ScopeLevel = (GT_U32)inFields[22];

    gIPUCrouteMcEntryArrayPtr[ind].ipv6DestinationSiteId =
                                                     (CPSS_IP_SITE_ID_ENT)inFields[23];
    gIPUCrouteMcEntryArrayPtr[ind].mtuProfileIndex = (GT_U32)inFields[24];
    gIPUCrouteMcEntryArrayPtr[ind].ageRefresh =(GT_BOOL)inFields[25];

    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_DIT_E;
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.outlifPointer.ditPtr =(GT_U32)inFields[26];

    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inFields[27];
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.devPort.devNum =(GT_U8)inFields[28];
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.devPort.portNum =(GT_U8)inFields[29];
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.trunkId =(GT_TRUNK_ID)inFields[30];
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.vidx = (GT_U16)inFields[31];
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.fabricVidx = (GT_U16)inFields[32];
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.vlanId = (GT_U16)inFields[33];

    gIPUCrouteMcEntryArrayPtr[ind].mplsCommand = CPSS_EXMXPM_MPLS_NOP_CMD_E;
    gIPUCrouteMcEntryArrayPtr[ind].mplsLabel = 0;

    gIPUCnumOfMcEntries++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpMcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of MC route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be written
*                                from this base on.
*       numOfRouteEntries      - The number of route entries to write.
*       routeEntriesArrayPtr   - The MC route entry array to write.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesWriteSetNext_2
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     GT_U32      ind;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    gIPUCrouteMcEntryArrayPtr = (CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC*)
       cmdOsRealloc(gIPUCrouteMcEntryArrayPtr,
       sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC) * (gIPUCnumOfMcEntries+1));

      if(gIPUCrouteMcEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }
    ind = (GT_U32)inFields[0];

    if(ind > gIPUCnumOfMcEntries)
    {
      galtisOutput(outArgs, GT_BAD_VALUE,"");
      return CMD_OK;
    }
    gIPUCrouteMcEntryArrayPtr[ind].command=
                                 (CPSS_PACKET_CMD_ENT)inFields[1];
    gIPUCrouteMcEntryArrayPtr[ind].cpuCodeIndex =
                                  (GT_U32)inFields[2];
    gIPUCrouteMcEntryArrayPtr[ind].appSpecificCpuCodeEnable =
                                                          (GT_BOOL)inFields[3];
    gIPUCrouteMcEntryArrayPtr[ind].hopLimitMode =
                                (GT_TRUE == (GT_BOOL)inFields[4])?
                                CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
    gIPUCrouteMcEntryArrayPtr[ind].ttlMode =
                                (GT_TRUE == (GT_BOOL)inFields[4])?
                                CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ONE_E:
                                CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
    gIPUCrouteMcEntryArrayPtr[ind].ttl = 0;
    gIPUCrouteMcEntryArrayPtr[ind].bypassTtlExceptionCheckEnable =

        (GT_BOOL)inFields[5];
    gIPUCrouteMcEntryArrayPtr[ind].mirrorToIngressAnalyzer =
                                                          (GT_BOOL)inFields[6];

    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyDp =
                                                          (GT_BOOL)inFields[7];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyDscp =
                                                         (GT_BOOL)inFields[8];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyExp =
                                                         (GT_BOOL)inFields[9];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyTc=
                                                         (GT_BOOL)inFields[10];
    gIPUCrouteMcEntryArrayPtr[ind].qosParamsModify.modifyUp =
                                                         (GT_BOOL)inFields[11];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.dp = (CPSS_DP_LEVEL_ENT)inFields[12];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.dscp = (GT_U32)inFields[13];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.exp = (GT_U32)inFields[14];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.tc = (GT_U32)inFields[15];
    gIPUCrouteMcEntryArrayPtr[ind].qosParams.up= (GT_U32)inFields[16];
    gIPUCrouteMcEntryArrayPtr[ind].cntrSetIndex =
                      (CPSS_EXMXPM_IP_COUNTER_SET_INDEX_ENT)inFields[17];

    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIfMode =(CPSS_EXMXPM_RPF_IF_MODE_ENT)inFields[18];

    switch ((gIPUCrouteMcEntryArrayPtr[ind].mcRpfIfMode))
        {
            case CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E:
            case CPSS_EXMXPM_RPF_IF_MODE_VLAN_INLIF_E:
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.type = CPSS_INTERFACE_VID_E;
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.vlanId = (GT_U16)inFields[19];
                break;
            case CPSS_EXMXPM_RPF_IF_MODE_PORT_E:
                if ((GT_U16)inFields[19] & 0x1)
                {
                    /* trunk */
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.type = CPSS_INTERFACE_TRUNK_E;
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.trunkId = ((GT_U16)inFields[19] >> 1) & 0xff;
                }
                else
                {
                    /* port */
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.type = CPSS_INTERFACE_PORT_E;
                    gIPUCrouteMcEntryArrayPtr[ind].mcRpfIf.devPort.portNum = ((GT_U16)inFields[19] >> 1) & 0x3f;
                }
                gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand =CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E;
                break;
            case CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E:
                break;
            default:
                galtisOutput(outArgs, GT_BAD_STATE, "");
                break;
        }

    gIPUCrouteMcEntryArrayPtr[ind].mcRpfFailCommand = (CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_ENT)inFields[20];

    gIPUCrouteMcEntryArrayPtr[ind].ipv6ScopeCheckEnable = (GT_BOOL)inFields[21];
    gIPUCrouteMcEntryArrayPtr[ind].ipv6ScopeLevel = (GT_U32)inFields[22];

    gIPUCrouteMcEntryArrayPtr[ind].ipv6DestinationSiteId =
                                                     (CPSS_IP_SITE_ID_ENT)inFields[23];
    gIPUCrouteMcEntryArrayPtr[ind].mtuProfileIndex = (GT_U32)inFields[24];
    gIPUCrouteMcEntryArrayPtr[ind].ageRefresh =(GT_BOOL)inFields[25];

    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_DIT_E;
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.outlifPointer.ditPtr =(GT_U32)inFields[26];

    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inFields[27];
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.devPort.devNum =(GT_U8)inFields[28];
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.devPort.portNum =(GT_U8)inFields[29];
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.trunkId =(GT_TRUNK_ID)inFields[30];
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.vidx = (GT_U16)inFields[31];
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.fabricVidx = (GT_U16)inFields[32];
    gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.vlanId = (GT_U16)inFields[33];

    gIPUCrouteMcEntryArrayPtr[ind].mplsCommand = CPSS_EXMXPM_MPLS_NOP_CMD_E;
    gIPUCrouteMcEntryArrayPtr[ind].mplsLabel = 0;

    gIPUCnumOfMcEntries++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmIpMcRouteEntriesWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Write an array of MC route entries to hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be written
*                                from this base on.
*       numOfRouteEntries      - The number of route entries to write.
*       routeEntriesArrayPtr   - The MC route entry array to write.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesWriteEndSet_2
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8    devNum;
    GT_U32   ucMcRouteEntryBaseIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    ucMcRouteEntryBaseIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
   result =  cpssExMxPmIpMcRouteEntriesWrite(devNum,ucMcRouteEntryBaseIndex,
                                           gIPUCnumOfMcEntries,
                                           gIPUCrouteMcEntryArrayPtr);
   if (result != GT_OK)
   {
       galtisOutput(outArgs, result, "");
       return CMD_OK;
   }

   cmdOsFree(gIPUCrouteMcEntryArrayPtr);
   gIPUCrouteMcEntryArrayPtr = NULL;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmIpMcRouteEntriesRead
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Read an array of MC route entries from the hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be read
*                                from this base on.
*       numOfRouteEntries      - The number route entries to read.
*
* OUTPUTS:
*       routeEntriesArrayPtr   - The MC route entry array to read into.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesReadGetFirst_2
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ucMcRouteEntryBaseIndex;
    GT_U32  ind;
    GT_U32 numToRefreash;/*number of entries to get from table*/

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gIPUCgetRouteMcEntryCurrIndx = 0;/*reset on first*/
    ind = gIPUCgetRouteMcEntryCurrIndx;

    /* map input arguments to locals */
     devNum= (GT_U8)inArgs[0];
     ucMcRouteEntryBaseIndex=(GT_U32)inArgs[1];
     numToRefreash=(GT_U32)inArgs[2];
     gIPUCgetNumOfMcEntries=numToRefreash;

     if(gIPUCgetRouteMcEntryArrayPtr == NULL)
    {
        gIPUCgetRouteMcEntryArrayPtr = (CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC*)
                    cmdOsMalloc(sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC)*numToRefreash);
    }
    if(gIPUCgetRouteMcEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

  /* call cpss api function */
    cmdOsMemSet(gIPUCgetRouteMcEntryArrayPtr,0,
                sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC) * gIPUCgetNumOfMcEntries);

    /* call cpss api function */
    result = cpssExMxPmIpMcRouteEntriesRead(devNum, ucMcRouteEntryBaseIndex, gIPUCgetNumOfMcEntries, gIPUCgetRouteMcEntryArrayPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gIPUCgetRouteMcEntryCurrIndx;

    inFields[1]=gIPUCgetRouteMcEntryArrayPtr[ind].command;

    inFields[2]=gIPUCgetRouteMcEntryArrayPtr[ind].cpuCodeIndex;

    inFields[3]=gIPUCgetRouteMcEntryArrayPtr[ind].appSpecificCpuCodeEnable;

    inFields[4] = (gIPUCgetRouteMcEntryArrayPtr[ind].hopLimitMode == CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E)?GT_TRUE:GT_FALSE;

    inFields[5]= gIPUCgetRouteMcEntryArrayPtr[ind].bypassTtlExceptionCheckEnable;

    inFields[6] =gIPUCgetRouteMcEntryArrayPtr[ind].mirrorToIngressAnalyzer;

    inFields[7]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyDp;

    inFields[8]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyDscp;

    inFields[9]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyExp;

    inFields[10]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyTc;

    inFields[11]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyUp;

    inFields[12] = gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.dp;

    inFields[13]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.dscp;

    inFields[14] =gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.exp;

    inFields[15]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.tc;

    inFields[16]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.up;

    inFields[17]=gIPUCgetRouteMcEntryArrayPtr[ind].cntrSetIndex;

    inFields[18]=gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIfMode;

    switch (gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.type)
    {
        case CPSS_INTERFACE_VID_E:
            inFields[19] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.vlanId;
            break;
        case CPSS_INTERFACE_PORT_E:
            inFields[19] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.devPort.portNum << 1;
            break;
        case CPSS_INTERFACE_TRUNK_E:
            inFields[19] = (gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.trunkId << 1)| 0x1;
            break;
        default:
            inFields[19]=0;
            break;
    }
    inFields[20] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfFailCommand;
    /* previous rpf fail command mode - now obsolete */

    inFields[21]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6ScopeCheckEnable;

    inFields[22]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6ScopeLevel;

    inFields[23]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6DestinationSiteId;

    inFields[24]=gIPUCgetRouteMcEntryArrayPtr[ind].mtuProfileIndex;

    inFields[25]=gIPUCgetRouteMcEntryArrayPtr[ind].ageRefresh;

    inFields[26]=gIPUCgetRouteMcEntryArrayPtr[ind].nextHopIf.outlifPointer.ditPtr;

    inFields[27]=gIPUCgetRouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.type;

    inFields[28]=gIPUCgetRouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.devPort.devNum;

    inFields[29]=gIPUCgetRouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.devPort.portNum;

    inFields[30]=gIPUCgetRouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.trunkId;

    inFields[31]=gIPUCgetRouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.vidx;

    inFields[32]=gIPUCgetRouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.fabricVidx;

    inFields[33]=gIPUCgetRouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.vlanId;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                ,inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26],inFields[27],
                inFields[28],inFields[29],inFields[30],inFields[31],
                inFields[32],inFields[33]);

    galtisOutput(outArgs, result, "%f");

    gIPUCgetRouteMcEntryCurrIndx++;

    return CMD_OK;
}
 /*******************************************************************************
* cpssExMxPmIpMcRouteEntriesRead
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Read an array of MC route entries from the hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                 - device number
*       ucMcRouteEntryBaseIndex- The base Index in the Route entries table
*                                range (0..4095). The entries will be read
*                                from this base on.
*       numOfRouteEntries      - The number route entries to read.
*
* OUTPUTS:
*       routeEntriesArrayPtr   - The MC route entry array to read into.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       See comments on cpssExMxPmIpUcRouteEntriesWrite
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpMcRouteEntriesReadGetNext_2
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32    ind = gIPUCgetRouteMcEntryCurrIndx;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(gIPUCgetRouteMcEntryCurrIndx >= gIPUCgetNumOfMcEntries)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    /* map input arguments to locals */
    inFields[0] = gIPUCgetRouteMcEntryCurrIndx;

    inFields[1]=gIPUCgetRouteMcEntryArrayPtr[ind].command;

    inFields[2]=gIPUCgetRouteMcEntryArrayPtr[ind].cpuCodeIndex;

    inFields[3]=gIPUCgetRouteMcEntryArrayPtr[ind].appSpecificCpuCodeEnable;

    inFields[4] = (gIPUCgetRouteMcEntryArrayPtr[ind].hopLimitMode == CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E)?GT_TRUE:GT_FALSE;

    inFields[5]= gIPUCgetRouteMcEntryArrayPtr[ind].bypassTtlExceptionCheckEnable;

    inFields[6] =gIPUCgetRouteMcEntryArrayPtr[ind].mirrorToIngressAnalyzer;

    inFields[7]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyDp;

    inFields[8]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyDscp;

    inFields[9]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyExp;

    inFields[10]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyTc;

    inFields[11]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParamsModify.modifyUp;

    inFields[12] = gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.dp;

    inFields[13]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.dscp;

    inFields[14] =gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.exp;

    inFields[15]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.tc;

    inFields[16]=gIPUCgetRouteMcEntryArrayPtr[ind].qosParams.up;

    inFields[17]=gIPUCgetRouteMcEntryArrayPtr[ind].cntrSetIndex;

    inFields[18]=gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIfMode;

    switch (gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.type)
    {
        case CPSS_INTERFACE_VID_E:
            inFields[19] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.vlanId;
            break;
        case CPSS_INTERFACE_PORT_E:
            inFields[19] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.devPort.portNum << 1;
            break;
        case CPSS_INTERFACE_TRUNK_E:
            inFields[19] = (gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfIf.trunkId << 1)| 0x1;
            break;
        default:
            inFields[19]=0;
            break;
    }

    inFields[20] = gIPUCgetRouteMcEntryArrayPtr[ind].mcRpfFailCommand;
    /* previous rpf fail command mode - now obsolete */

    inFields[21]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6ScopeCheckEnable;

    inFields[22]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6ScopeLevel;

    inFields[23]=gIPUCgetRouteMcEntryArrayPtr[ind].ipv6DestinationSiteId;

    inFields[24]=gIPUCgetRouteMcEntryArrayPtr[ind].mtuProfileIndex;

    inFields[25]=gIPUCgetRouteMcEntryArrayPtr[ind].ageRefresh;

    inFields[26]=gIPUCgetRouteMcEntryArrayPtr[ind].nextHopIf.outlifPointer.ditPtr;

    inFields[27]=gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.type;

    inFields[28]=gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.devPort.devNum;

    inFields[29]=gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.devPort.portNum;

    inFields[30]=gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.trunkId;

    inFields[31]=gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.vidx;

    inFields[32]=gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.fabricVidx;

    inFields[33]=gIPUCrouteMcEntryArrayPtr[ind].nextHopIf.interfaceInfo.vlanId;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                ,inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26],inFields[27],
                inFields[28],inFields[29],inFields[30],inFields[31],
                inFields[32],inFields[33]);

    galtisOutput(outArgs, GT_OK, "%f");

    gIPUCgetRouteMcEntryCurrIndx++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpRouteEntryAgeRefreshSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Set age/refresh field of next-hop route entry.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                  - device number
*       ucMcRouteEntryIndex     - The index in the Route entries table
*                                 range (0..4095).
*       ageRefresh              - Age/refresh value to set. Range 0..1.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpRouteEntryAgeRefreshSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ucMcRouteEntryIndex;
    GT_U32 ageRefresh;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ucMcRouteEntryIndex = (GT_U32)inArgs[1];
    ageRefresh = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmIpRouteEntryAgeRefreshSet(devNum, ucMcRouteEntryIndex, ageRefresh);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpRouteEntryAgeRefreshGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Set age/refresh field of next-hop route entry.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                  - device number
*       ucMcRouteEntryIndex     - The index in the Route entries table
*                                 range (0..4095).
*
* OUTPUTS:
*       ageRefreshPtr           - Age/refresh value of requested Next-hop entry.
*                                 field. Range 0..1.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpRouteEntryAgeRefreshGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ucMcRouteEntryIndex;
    GT_U32 ageRefresh;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ucMcRouteEntryIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpRouteEntryAgeRefreshGet(devNum, ucMcRouteEntryIndex, &ageRefresh);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ageRefresh);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpRouterArpAddrWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Writes a ARP MAC address to the router ARP / Tunnel start Table.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number
*       routerArpIndex  - The Arp Address index (to be inserted later
*                         in the UC Route entry Arp nextHopARPPointer
*                         field) range (0..4095).
*       arpMacAddrPtr   - the ARP MAC address to write
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on routerArpIndex bigger then 4095 or
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       pay attention that the router ARP table is shard with a tunnel start
*       table, each tunnel start entry takes 4 ARP Address.
*       that is Tunnel start <n> takes ARP addresses <4n>,<4n+1>,<4n+2>,
*       <4n+3>
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpRouterArpAddrWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 routerArpIndex;
    GT_ETHERADDR arpMacAddr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    routerArpIndex = (GT_U32)inArgs[1];
    galtisMacAddr(&arpMacAddr, (GT_U8*)inArgs[2]);

    /* call cpss api function */
    result = cpssExMxPmIpRouterArpAddrWrite(devNum, routerArpIndex, &arpMacAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpRouterArpAddrRead
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Reads a ARP MAC address from the router ARP / Tunnel start Table.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number
*       routerArpIndex  - The Arp Address index (to be inserted later in the
*                         UC Route entry Arp nextHopARPPointer field) range
*                         (0..4095).
*
*
* OUTPUTS:
*       arpMacAddrPtr   - the ARP MAC address
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on routerArpIndex bigger then 4095 or
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       see cpssExMxPmIpRouterArpAddrWrite
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpRouterArpAddrRead
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 routerArpIndex;
    GT_ETHERADDR arpMacAddr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    routerArpIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpRouterArpAddrRead(devNum, routerArpIndex, &arpMacAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result,"%6b", arpMacAddr.arEther);

    return CMD_OK;
}

 /**************Table: cpssExMxPmDitIpMcPair****************/

GT_U32  gDitPairEntryIndex;
GT_U32  gMaxDitPairEntryIndex;


/*******************************************************************************
* cpssExMxPmIpDitPairWrite
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Writes a Downstream interface Table (DIT) pair entry to hw according
*       to the scheme :
*           1) First part only
*           2) Second part + next pointer only
*           3) Whole DIT pair.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       ditPairEntryIndex - The DIT Pair entry index.
*       ditPairWriteForm  - The way to write the DIT pair, first part only/
*                           second + next pointer only/ whole DIT pair.
*       ditPairEntryPtr   - The downstream interface pair entry
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_BAD_PARAM      - on illegal parameter
*       GT_BAD_PTR        - on NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDitPairWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                        result;
    GT_U8                                            devNum;
    GT_U32                                           ditPairEntryIndex;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_MODE_ENT   ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_WHOLE_E;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_STC                   ditPairEntry;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    ditPairEntryIndex           = (GT_U32)inFields[0];

    switch (inFields[1])
    {
        case 2:
            ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E;
            break;
        case 3:
            ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_SECOND_DIT_NEXT_POINTER_ONLY_E;
            break;
        case 4:
            ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_WHOLE_E;
            break;
        default:
            galtisOutput(outArgs, GT_BAD_PARAM, "");
            return CMD_OK;
    }

    ditPairEntry.ditNextPointer = (GT_U32)inFields[2];

    if((ditPairWriteForm == CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E) ||
       (ditPairWriteForm == CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_WHOLE_E))
    {
        ditPairEntry.firstDitNode.ditRpfFailCommand           = (CPSS_PACKET_CMD_ENT)inFields[3];
        ditPairEntry.firstDitNode.isTunnelStart               = (GT_BOOL)inFields[4];
        ditPairEntry.firstDitNode.nextHopTunnelPointer        = (GT_U32)inFields[5];
        ditPairEntry.firstDitNode.nextHopIf.type              = (CPSS_INTERFACE_TYPE_ENT)inFields[6];
        ditPairEntry.firstDitNode.nextHopIf.devPort.devNum    = (GT_U8)inFields[7];
        ditPairEntry.firstDitNode.nextHopIf.devPort.portNum   = (GT_U8)inFields[8];
        ditPairEntry.firstDitNode.nextHopIf.trunkId           = (GT_TRUNK_ID)inFields[9];
        ditPairEntry.firstDitNode.nextHopIf.vidx              = (GT_U16)inFields[10];
        ditPairEntry.firstDitNode.nextHopIf.vlanId            = (GT_U16)inFields[11];
        ditPairEntry.firstDitNode.nextHopVlanId               = (GT_U16)inFields[12];
        ditPairEntry.firstDitNode.ttlHopLimitThreshold        = (GT_U32)inFields[13];
        ditPairEntry.firstDitNode.excludeSourceVlanIf         = (GT_BOOL)inFields[14];
        ditPairEntry.firstDitNode.isLast                      = (GT_BOOL)inFields[15];

    }

     if((ditPairWriteForm == CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_SECOND_DIT_NEXT_POINTER_ONLY_E) ||
       (ditPairWriteForm == CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_WHOLE_E))
    {
        ditPairEntry.secondDitNode.ditRpfFailCommand          = (CPSS_PACKET_CMD_ENT)inFields[16];
        ditPairEntry.secondDitNode.isTunnelStart              = (GT_BOOL)inFields[17];
        ditPairEntry.secondDitNode.nextHopTunnelPointer       = (GT_U32)inFields[18];
        ditPairEntry.secondDitNode.nextHopIf.type             = (CPSS_INTERFACE_TYPE_ENT)inFields[19];
        ditPairEntry.secondDitNode.nextHopIf.devPort.devNum   = (GT_U8)inFields[20];
        ditPairEntry.secondDitNode.nextHopIf.devPort.portNum  = (GT_U8)inFields[21];
        ditPairEntry.secondDitNode.nextHopIf.trunkId          = (GT_TRUNK_ID)inFields[22];
        ditPairEntry.secondDitNode.nextHopIf.vidx             = (GT_U16)inFields[23];
        ditPairEntry.secondDitNode.nextHopIf.vlanId           = (GT_U16)inFields[24];
        ditPairEntry.secondDitNode.nextHopVlanId              = (GT_U16)inFields[25];
        ditPairEntry.secondDitNode.ttlHopLimitThreshold       = (GT_U32)inFields[26];
        ditPairEntry.secondDitNode.excludeSourceVlanIf        = (GT_BOOL)inFields[27];
        ditPairEntry.secondDitNode.isLast                     = (GT_BOOL)inFields[28];
     }

    /* call cpss api function */
    result = cpssExMxPmDitIpMcPairWrite(devNum, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpDitPairRead
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Reads a Downstream Interface Table pair entry from the hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       ditPairEntryIndex - The DIT Pair entry index.
*
* OUTPUTS:
*       ditPairEntryPtr   - The downstream interface pair entry
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_BAD_PARAM      - on illegal parameter
*       GT_BAD_PTR        - on NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDitPairReadEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;

    CPSS_EXMXPM_DIT_IP_MC_PAIR_STC  ditPairEntry;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

     devNum = (GT_U8)inArgs[0];

     if(gDitPairEntryIndex > gMaxDitPairEntryIndex)
    {
      galtisOutput(outArgs, GT_OK, "%d", -1);
      return CMD_OK;
    }


    /* call cpss api function */
    result = cpssExMxPmDitIpMcPairRead(devNum, gDitPairEntryIndex, CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_WHOLE_E, &ditPairEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] =gDitPairEntryIndex;
    inFields[1] = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_WHOLE_E ;
    inFields[2] =ditPairEntry.ditNextPointer;
    inFields[3] =ditPairEntry.firstDitNode.ditRpfFailCommand;
    inFields[4] =ditPairEntry.firstDitNode.isTunnelStart;
    inFields[5] =ditPairEntry.firstDitNode.nextHopTunnelPointer;
    inFields[6] =ditPairEntry.firstDitNode.nextHopIf.type;
    inFields[7] =ditPairEntry.firstDitNode.nextHopIf.devPort.devNum;
    inFields[8] =ditPairEntry.firstDitNode.nextHopIf.devPort.portNum;
    inFields[9] =ditPairEntry.firstDitNode.nextHopIf.trunkId;
    inFields[10]=ditPairEntry.firstDitNode.nextHopIf.vidx;
    inFields[11]=ditPairEntry.firstDitNode.nextHopIf.vlanId;
    inFields[12]=ditPairEntry.firstDitNode.nextHopVlanId;
    inFields[13]=ditPairEntry.firstDitNode.ttlHopLimitThreshold ;
    inFields[14]=ditPairEntry.firstDitNode.excludeSourceVlanIf ;
    inFields[15]=ditPairEntry.firstDitNode.isLast ;
    inFields[16]=ditPairEntry.secondDitNode.ditRpfFailCommand;
    inFields[17]=ditPairEntry.secondDitNode.isTunnelStart;
    inFields[18]=ditPairEntry.secondDitNode.nextHopTunnelPointer;
    inFields[19]=ditPairEntry.secondDitNode.nextHopIf.type;
    inFields[20]=ditPairEntry.secondDitNode.nextHopIf.devPort.devNum;
    inFields[21]=ditPairEntry.secondDitNode.nextHopIf.devPort.portNum;
    inFields[22]=ditPairEntry.secondDitNode.nextHopIf.trunkId;
    inFields[23]=ditPairEntry.secondDitNode.nextHopIf.vidx;
    inFields[24]=ditPairEntry.secondDitNode.nextHopIf.vlanId;
    inFields[25]=ditPairEntry.secondDitNode.nextHopVlanId;
    inFields[26]=ditPairEntry.secondDitNode.ttlHopLimitThreshold;
    inFields[27]=ditPairEntry.secondDitNode.excludeSourceVlanIf;
    inFields[28]=ditPairEntry.secondDitNode.isLast;
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
            ,inFields[0], inFields[1], inFields[2], inFields[3],
            inFields[4], inFields[5], inFields[6], inFields[7],
            inFields[8], inFields[9], inFields[10], inFields[11],
            inFields[12], inFields[13], inFields[14], inFields[15],
            inFields[16], inFields[17], inFields[18], inFields[19],
            inFields[20], inFields[21], inFields[22], inFields[23],
            inFields[24], inFields[25], inFields[26],inFields[27],
            inFields[28]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmIpDitPairRead
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Reads a Downstream Interface Table pair entry from the hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       ditPairEntryIndex - The DIT Pair entry index.
*
* OUTPUTS:
*       ditPairEntryPtr   - The downstream interface pair entry
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_BAD_PARAM      - on illegal parameter
*       GT_BAD_PTR        - on NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDitPairReadGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

     gDitPairEntryIndex     =(GT_U32)inArgs[1];/*get min and max entry*/
     gMaxDitPairEntryIndex  =(GT_U32)inArgs[2];
     return wrCpssExMxPmIpDitPairReadEntry(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* cpssExMxPmIpDitPairRead
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Reads a Downstream Interface Table pair entry from the hw.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       ditPairEntryIndex - The DIT Pair entry index.
*
* OUTPUTS:
*       ditPairEntryPtr   - The downstream interface pair entry
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_BAD_PARAM      - on illegal parameter
*       GT_BAD_PTR        - on NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDitPairReadGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    gDitPairEntryIndex++;
    return wrCpssExMxPmIpDitPairReadEntry(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssExMxPmIpDitPairIsLastSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       The Downstream Interface table is organized as a linked list of pairs
*       of DIT entries. Each DIT entry contains a <Last> bit indicating whether
*       this entry is the last entry in the list.
*       This function writes <isLast> field to first or second DIT entry in
*       a pair.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       ditPairEntryIndex - The DIT Pair entry index.
*       ditPairWriteForm  - The way to write the DIT pair, isLast field of
*                           first part or second part of DIT pair.
*       isLast            - Value to set to isLast field
*                             - 0: DIT is not last
*                             - 1: DIT is last
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDitPairLastSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ditPairEntryIndex;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_LAST_FIELD_WRITE_MODE_ENT ditPairWriteForm;
    GT_U32 isLast;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum              = (GT_U8)inArgs[0];
    ditPairEntryIndex   = (GT_U32)inArgs[1];
    ditPairWriteForm    = (CPSS_EXMXPM_DIT_IP_MC_PAIR_LAST_FIELD_WRITE_MODE_ENT)inArgs[2];
    isLast              = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmDitIpMcPairLastSet(devNum, ditPairEntryIndex, ditPairWriteForm, isLast);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/* amount TCAM rows */
#define PRV_CPSS_EXMXPM_INT_TCAM_RULE_ROWS_AMOUNT_CNS 2048

static GT_BOOL rulesBitArray[PRV_CPSS_EXMXPM_INT_TCAM_RULE_ROWS_AMOUNT_CNS];
static GT_U32  ruleIndex;

/*******************************************************************************
* cpssExMxPmIpv6McRuleSet
*
* DESCRIPTION:
*       Function Relevant mode : Low Level API mode
*       Sets the IPv6 MC Rule with Mask, Pattern and Action
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       ruleIndex     - index of the rule in the TCAM.
*                       IPv6 MC Rules and Policy rules use the same TCAM.
*       isValid       - validity state of the rule
*                       GT_TRUE:  entry is valid and will be used in TCAM lookup
*                       GT_FALSE: entry is not valid
*       maskPtr       - (pointer to) rule mask. The rule mask is AND styled one.
*                       Mask bit 0 means don't care bit (corresponding bit in
*                       the pattern is not used in the TCAM lookup).
*                       Mask bit 1 means that corresponding bit in the
*                       pattern is used in the TCAM lookup.
*       patternPtr    - (pointer to) rule pattern.
*       actionPtr     - (pointer to) IPv6 MC action rule action that applied on packet if
*                       packet's IP MC search key is matched with masked pattern.
*                       supported action pointer type are:
*                       - CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E
*                       - CPSS_EXMXPM_IP_COMPRESSED_1_NODE_PTR_TYPE_E
*                       - CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To copy or invalidate IPv6 MC Rule use the general PCL API
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpv6McRuleSet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                                result;
    GT_U8                                    devNum;
    CPSS_EXMXPM_IPV6_MC_KEY_STC              mask;
    CPSS_EXMXPM_IPV6_MC_KEY_STC              pattern;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC action;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleIndex = inFields[0];
    mask.pclId = inFields[1];
    galtisIpv6Addr((GT_IPV6ADDR*)&mask.dip, (GT_U8*)inFields[2]);
    mask.inlifId = (GT_U32)inFields[3];
    mask.vrId = (GT_U32)inFields[4];
    pattern.pclId = inFields[5];
    galtisIpv6Addr((GT_IPV6ADDR*)&pattern.dip, (GT_U8*)inFields[6]);
    pattern.inlifId = (GT_U32)inFields[7];
    pattern.vrId = (GT_U32)inFields[8];
    action.pointerType = inFields[9];

    if (action.pointerType != CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
    {
        action.pointerData.nextNodePointer.nextPointer = inFields[10];
        action.pointerData.nextNodePointer.range5Index = inFields[11];
    }
    else
    {
        action.pointerData.routeEntryPointer.routeEntryBaseMemAddr = (GT_U32)inFields[12];
        action.pointerData.routeEntryPointer.blockSize = (GT_U32)inFields[13];
        action.pointerData.routeEntryPointer.routeEntryMethod = inFields[14];
    }

    /* call cpss api function */
    result = cpssExMxPmIpv6McRuleSet(devNum,
                                    ruleIndex,
                                    GT_TRUE,
                                    &mask,
                                    &pattern,
                                    &action);

    if (result == GT_OK)
    {
        /*mark the presence of IPv6 MC Rule in the rules array*/
        rulesBitArray[ruleIndex] = GT_TRUE;
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

static CMD_STATUS wrCpssExMxPmIpv6McRuleDelete
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                                result;
    GT_U8                                    devNum;
    CPSS_EXMXPM_IPV6_MC_KEY_STC              mask;
    CPSS_EXMXPM_IPV6_MC_KEY_STC              pattern;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC action;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ruleIndex = inFields[0];
    mask.pclId = inFields[1];
    galtisIpv6Addr((GT_IPV6ADDR*)&mask.dip, (GT_U8*)inFields[2]);
    mask.inlifId = (GT_U32)inFields[3];
    mask.vrId = (GT_U32)inFields[4];
    pattern.pclId = inFields[5];
    galtisIpv6Addr((GT_IPV6ADDR*)&pattern.dip, (GT_U8*)inFields[6]);
    pattern.inlifId = (GT_U32)inFields[7];
    pattern.vrId = (GT_U32)inFields[8];
    action.pointerType = inFields[9];

    if (action.pointerType != CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
    {
        action.pointerData.nextNodePointer.nextPointer = inFields[10];
        action.pointerData.nextNodePointer.range5Index = inFields[11];
    }
    else
    {
        action.pointerData.routeEntryPointer.routeEntryBaseMemAddr = (GT_U32)inFields[12];
        action.pointerData.routeEntryPointer.blockSize = (GT_U32)inFields[13];
        action.pointerData.routeEntryPointer.routeEntryMethod = inFields[14];
    }

    /* call cpss api function */
    result = cpssExMxPmIpv6McRuleSet(devNum,
                                    ruleIndex,
                                    GT_FALSE,
                                    &mask,
                                    &pattern,
                                    &action);
    if (result == GT_OK)
    {
        /*mark the presence of IPv6 MC Rule in the rules array*/
        rulesBitArray[ruleIndex] = GT_FALSE;
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

static CMD_STATUS wrCpssExMxPmIpv6McRuleClearAll
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                                result;
    GT_U8                                    devNum;
    GT_BOOL                                  valid;
    CPSS_EXMXPM_IPV6_MC_KEY_STC              mask;
    CPSS_EXMXPM_IPV6_MC_KEY_STC              pattern;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC action;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    for (ruleIndex = 0 ; ruleIndex < PRV_CPSS_EXMXPM_INT_TCAM_RULE_ROWS_AMOUNT_CNS; ++ruleIndex)
    {
        if (rulesBitArray[ruleIndex] == GT_TRUE)
        {
            /* call cpss api function */
            result = cpssExMxPmIpv6McRuleGet(devNum,
                                            ruleIndex,
                                            &valid,
                                            &mask,
                                            &pattern,
                                            &action);

            if (result != GT_OK)
            {
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }

            valid = GT_FALSE;

            if (action.pointerType != CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
            {
                action.pointerData.nextNodePointer.nextPointer = 0;
                action.pointerData.nextNodePointer.range5Index = 0;
            }
            else
            {
                action.pointerData.routeEntryPointer.routeEntryBaseMemAddr = 0;
                action.pointerData.routeEntryPointer.blockSize = 0;
                action.pointerData.routeEntryPointer.routeEntryMethod = 0;
            }

            /* call cpss api function */
            result = cpssExMxPmIpv6McRuleSet(devNum,
                                           ruleIndex,
                                           valid,
                                           &mask,
                                           &pattern,
                                           &action);
            if (result != GT_OK)
            {
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }

            /*mark the presence of IPv6 MC Rule in the rules array*/
            rulesBitArray[ruleIndex] = GT_FALSE;
        }
    }

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpv6McRuleGet
*
* DESCRIPTION:
*       Function Relevant mode : Low Level API mode
*       Gets the IPv6 MC Rule: Mask, Pattern and Action
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       ruleIndex     - index of the rule in the TCAM.
*                       IPv6 MC Rules and Policy rules use the same TCAM.
*
* OUTPUTS:
*       isValidPtr    - (pointer to) validity state of the rule
*                       GT_TRUE:  entry is valid and will be used in TCAM lookup
*                       GT_FALSE: entry is not valid
*       maskPtr       - (pointer to) rule mask. The rule mask is AND styled one.
*                       Mask bit 0 means don't care bit (corresponding bit in
*                       the pattern is not used in the TCAM lookup).
*                       Mask bit 1 means that corresponding bit in the
*                       pattern is used in the TCAM lookup.
*       patternPtr    - (pointer to) rule pattern.
*       actionPtr     - (pointer to) IPv6 MC action rule action that applied on packet if
*                       packet's IP MC search key is matched with masked pattern.
*                       supported action pointer type are:
*                       - CPSS_EXMXPM_IP_REGULAR_NODE_PTR_TYPE_E
*                       - CPSS_EXMXPM_IP_COMPRESSED_1_NODE_PTR_TYPE_E
*                       - CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To copy or invalidate IPv6 MC Rule use the general PCL API
*
*******************************************************************************/
static CMD_STATUS prvCpssExMxPmIpv6McRuleGetEntry
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                                result;
    GT_U8                                    devNum;
    GT_BOOL                                  valid;
    CPSS_EXMXPM_IPV6_MC_KEY_STC              mask;
    CPSS_EXMXPM_IPV6_MC_KEY_STC              pattern;
    CPSS_EXMXPM_IP_LPM_NODE_NEXT_POINTER_STC action;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    for ( ; ruleIndex < PRV_CPSS_EXMXPM_INT_TCAM_RULE_ROWS_AMOUNT_CNS; ++ruleIndex)
    {
        if (rulesBitArray[ruleIndex] == GT_TRUE)
            break;
    }

    if (ruleIndex >= PRV_CPSS_EXMXPM_INT_TCAM_RULE_ROWS_AMOUNT_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }


    /* call cpss api function */
    result = cpssExMxPmIpv6McRuleGet(devNum,
                                    ruleIndex,
                                    &valid,
                                    &mask,
                                    &pattern,
                                    &action);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = ruleIndex;
    inFields[1] = mask.pclId;
    inFields[3] = mask.inlifId;
    inFields[4] = mask.vrId;
    inFields[5] = pattern.pclId;
    inFields[7] = pattern.inlifId;
    inFields[8] = pattern.vrId;
    inFields[9] = action.pointerType;

    if (action.pointerType != CPSS_EXMXPM_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
    {
        inFields[10] = action.pointerData.nextNodePointer.nextPointer;
        inFields[11] = action.pointerData.nextNodePointer.range5Index;

        /* pack and output table fields */
        fieldOutput("%d%d%16B%d%d%d%16B%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1], mask.dip,inFields[3],
                    inFields[4], inFields[5], pattern.dip, inFields[7],
                    inFields[8], inFields[9], inFields[10], inFields[11],0,0,0);
    }
    else
    {
        inFields[12] = action.pointerData.routeEntryPointer.routeEntryBaseMemAddr;
        inFields[13] = action.pointerData.routeEntryPointer.blockSize;
        inFields[14] = action.pointerData.routeEntryPointer.routeEntryMethod;

        /* pack and output table fields */
        fieldOutput("%d%d%16B%d%d%d%16B%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1], mask.dip,inFields[3],
                    inFields[4], inFields[5], pattern.dip, inFields[7],
                    inFields[8], inFields[9], 0, 0, inFields[12], inFields[13],
                    inFields[14]);
    }


    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssExMxPmIpv6McRuleGetFirst
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    ruleIndex = 0;

    return prvCpssExMxPmIpv6McRuleGetEntry(inArgs, inFields, numFields, outArgs);
}

static CMD_STATUS wrCpssExMxPmIpv6McRuleGetNext
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    ruleIndex++;

    return prvCpssExMxPmIpv6McRuleGetEntry(inArgs, inFields, numFields, outArgs);
}




/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmIpUcRouteEntriesSetFirst",
         &wrCpssExMxPmIpUcRouteEntriesWriteSetFirst,
         0, 41},
        {"cpssExMxPmIpUcRouteEntriesSetNext",
         &wrCpssExMxPmIpUcRouteEntriesWriteSetNext,
         0, 41},
        {"cpssExMxPmIpUcRouteEntriesEndSet",
         &wrCpssExMxPmIpUcRouteEntriesWriteEndSet,
         2, 0},
        {"cpssExMxPmIpUcRouteEntriesCancelSet",
         &wrCpssExMxPmIpUcRouteEntriesWriteCancelSet,
         0, 0},
        {"cpssExMxPmIpUcRouteEntriesGetFirst",
         &wrCpssExMxPmIpUcRouteEntriesReadGetFirst,
         3, 0},
        {"cpssExMxPmIpUcRouteEntriesGetNext",
         &wrCpssExMxPmIpUcRouteEntriesReadGetNext,
         0, 0},
        /* different enumeration in nextHopIf.type(field 33)*/
        {"cpssExMxPmIpUcRouteEntries_1SetFirst",
         &wrCpssExMxPmIpUcRouteEntriesWriteSetFirst,
         0, 41},
        {"cpssExMxPmIpUcRouteEntries_1SetNext",
         &wrCpssExMxPmIpUcRouteEntriesWriteSetNext,
         0, 41},
        {"cpssExMxPmIpUcRouteEntries_1EndSet",
         &wrCpssExMxPmIpUcRouteEntriesWriteEndSet,
         2, 0},
        {"cpssExMxPmIpUcRouteEntries_1CancelSet",
         &wrCpssExMxPmIpUcRouteEntriesWriteCancelSet,
         0, 0},
        {"cpssExMxPmIpUcRouteEntries_1GetFirst",
         &wrCpssExMxPmIpUcRouteEntriesReadGetFirst,
         3, 0},
        {"cpssExMxPmIpUcRouteEntries_1GetNext",
         &wrCpssExMxPmIpUcRouteEntriesReadGetNext,
         0, 0},
        {"cpssExMxPmIpEcmpRpfEntriesSetFirst",
         &wrCpssExMxPmIpEcmpRpfEntriesWriteSetFirst,
         0, 10},
        {"cpssExMxPmIpEcmpRpfEntriesSetNext",
         &wrCpssExMxPmIpEcmpRpfEntriesWriteSetNext,
         0, 10},
        {"cpssExMxPmIpEcmpRpfEntriesEndSet",
         &wrCpssExMxPmIpEcmpRpfEntriesWriteEndSet,
         2, 0},
        {"cpssExMxPmIpEcmpRpfEntriesGetFirst",
         &wrCpssExMxPmIpEcmpRpfEntriesReadGetFirst,
         2, 0},
        {"cpssExMxPmIpEcmpRpfEntriesGetNext",
         &wrCpssExMxPmIpEcmpRpfEntriesReadGetNext,
         0, 0},
         {"cpssExMxPmIpMcRouteEntriesSetFirst",
         &wrCpssExMxPmIpMcRouteEntriesWriteSetFirst,
         0, 29},
        {"cpssExMxPmIpMcRouteEntriesSetNext",
         &wrCpssExMxPmIpMcRouteEntriesWriteSetNext,
         0, 29},
        {"cpssExMxPmIpMcRouteEntriesEndSet",
         &wrCpssExMxPmIpMcRouteEntriesWriteEndSet,
         2, 0},
        {"cpssExMxPmIpMcRouteEntriesCancelSet",
         &wrCpssExMxPmIpMcRouteEntriesWriteEndSet,
         0, 0},
        {"cpssExMxPmIpMcRouteEntriesGetFirst",
         &wrCpssExMxPmIpMcRouteEntriesReadGetFirst,
         3, 0},
        {"cpssExMxPmIpMcRouteEntriesGetNext",
         &wrCpssExMxPmIpMcRouteEntriesReadGetNext,
         0, 0},
        {"cpssExMxPmIpMcRouteEntriesCancelSet",
         &wrCpssExMxPmIpMcRouteEntriesWriteCancelSet,
         0, 0},

        {"cpssExMxPmIpMcRouteEntries_1SetFirst",
         &wrCpssExMxPmIpMcRouteEntriesWriteSetFirst_1,
         0, 29},
        {"cpssExMxPmIpMcRouteEntries_1SetNext",
         &wrCpssExMxPmIpMcRouteEntriesWriteSetNext_1,
         0, 29},
        {"cpssExMxPmIpMcRouteEntries_1EndSet",
         &wrCpssExMxPmIpMcRouteEntriesWriteEndSet_1,
         2, 0},
        {"cpssExMxPmIpMcRouteEntries_1GetFirst",
         &wrCpssExMxPmIpMcRouteEntriesReadGetFirst_1,
         3, 0},
        {"cpssExMxPmIpMcRouteEntries_1GetNext",
         &wrCpssExMxPmIpMcRouteEntriesReadGetNext_1,
         0, 0},
        {"cpssExMxPmIpMcRouteEntries_1CancelSet",
         &wrCpssExMxPmIpMcRouteEntriesWriteCancelSet_1,
         0, 0},
        /* support for nexthop interface info added*/
        {"cpssExMxPmIpMcRouteEntries_2SetFirst",
         &wrCpssExMxPmIpMcRouteEntriesWriteSetFirst_2,
         0, 34},
        {"cpssExMxPmIpMcRouteEntries_2SetNext",
         &wrCpssExMxPmIpMcRouteEntriesWriteSetNext_2,
         0, 34},
        {"cpssExMxPmIpMcRouteEntries_2EndSet",
         &wrCpssExMxPmIpMcRouteEntriesWriteEndSet_2,
         2, 0},
        {"cpssExMxPmIpMcRouteEntries_2GetFirst",
         &wrCpssExMxPmIpMcRouteEntriesReadGetFirst_2,
         3, 0},
        {"cpssExMxPmIpMcRouteEntries_2GetNext",
         &wrCpssExMxPmIpMcRouteEntriesReadGetNext_2,
         0, 0},
        {"cpssExMxPmIpMcRouteEntries_2CancelSet",
         &wrCpssExMxPmIpMcRouteEntriesWriteCancelSet_1,
         0, 0},
        /*end of new mc commands for route entry next hop interface info*/

        {"cpssExMxPmIpRouteEntryAgeRefreshSet",
         &wrCpssExMxPmIpRouteEntryAgeRefreshSet,
         3, 0},
        {"cpssExMxPmIpRouteEntryAgeRefreshGet",
         &wrCpssExMxPmIpRouteEntryAgeRefreshGet,
         2, 0},
        {"cpssExMxPmIpRouterArpAddrWrite",
         &wrCpssExMxPmIpRouterArpAddrWrite,
         3, 0},
        {"cpssExMxPmIpRouterArpAddrRead",
         &wrCpssExMxPmIpRouterArpAddrRead,
         2, 0},
        {"cpssExMxPmIpDitPairSet",
         &wrCpssExMxPmIpDitPairWrite,
         1, 29},
        {"cpssExMxPmIpDitPairGetFirst",
         &wrCpssExMxPmIpDitPairReadGetFirst,
         3, 0},
        {"cpssExMxPmIpDitPairGetNext",
         &wrCpssExMxPmIpDitPairReadGetNext,
         3, 0},
        {"cpssExMxPmIpDitPairLastSet",
         &wrCpssExMxPmIpDitPairLastSet,
         4, 0},
        {"cpssExMxPmIpv6McRuleSet",
         &wrCpssExMxPmIpv6McRuleSet,
         1, 15},
        {"cpssExMxPmIpv6McRuleDelete",
         wrCpssExMxPmIpv6McRuleDelete,
         1, 15},
        {"cpssExMxPmIpv6McRuleGetFirst",
         wrCpssExMxPmIpv6McRuleGetFirst,
         1, 0},
        {"cpssExMxPmIpv6McRuleGetNext",
         wrCpssExMxPmIpv6McRuleGetNext,
         1, 0},
        {"cpssExMxPmIpv6McRuleClear",
         wrCpssExMxPmIpv6McRuleClearAll,
         1, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmIpTables
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
GT_STATUS cmdLibInitCpssExMxPmIpTables
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

