/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfIpGen.c
*
* DESCRIPTION:
*       Generic API implementation for IP
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <utf/private/prvUtfHelpers.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

#include <trafficEngine/tgfTrafficEngine.h>
#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfIpGen.h>

#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

extern GT_STATUS appDemoDbEntryGet
(
    IN  GT_U8   *namePtr,
    OUT GT_U32  *valuePtr
);

/* the Route entry index for UC Route entry Table */
static GT_U32        prvTgfRouteEntryBaseIndex = 3;

/******************************************************************************\
 *                             Define section                                 *
\******************************************************************************/

/* total number of counters */
#define PRV_TGF_MAX_COUNTER_NUM_CNS    35

/* max counter field's name length */
#define PRV_TGF_MAX_FIELD_NAME_LEN_CNS 25


/******************************************************************************\
 *                     Private function's implementation                      *
\******************************************************************************/

#ifdef CHX_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToDxChIpUcRouteEntry
*
* DESCRIPTION:
*       Convert generic IP UC route entry into device specific IP UC route entry
*
* INPUTS:
*       ipUcRouteEntryPtr - (pointer to) IP UC route entry
*
* OUTPUTS:
*       dxChIpUcRouteEntryPtr - (pointer to) DxCh IP UC route entry
*
* RETURNS:
*       GT_OK           -   on OK
*       GT_BAD_PARAM    -   Invalid device id
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChIpUcRouteEntry
(
    IN  PRV_TGF_IP_UC_ROUTE_ENTRY_STC            *ipUcRouteEntryPtr,
    OUT CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC   *dxChIpUcRouteEntryPtr
)
{
    GT_STATUS        rc = GT_OK; /* return code */

    /* convert IP UC route entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, cmd);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, appSpecificCpuCodeEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, unicastPacketSipFilterEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, ingressMirror);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, countSet);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, trapMirrorArpBcEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, sipAccessLevel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, dipAccessLevel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, ICMPRedirectEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, scopeCheckingEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, siteId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, mtuProfileIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, nextHopVlanId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, ttlHopLimitDecEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, ttlHopLimDecOptionsExtChkByPass);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, qosProfileMarkingEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, qosProfileIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, qosPrecedence);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, modifyUp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, modifyDscp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, isTunnelStart);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, nextHopARPPointer);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, nextHopTunnelPointer);

    dxChIpUcRouteEntryPtr->cpuCodeIdx = ipUcRouteEntryPtr->cpuCodeIndex;

    cpssOsMemCpy((GT_VOID*) &(dxChIpUcRouteEntryPtr->nextHopInterface),
                 (GT_VOID*) &(ipUcRouteEntryPtr->nextHopInterface),
                 sizeof(ipUcRouteEntryPtr->nextHopInterface));

    if( CPSS_INTERFACE_PORT_E == ipUcRouteEntryPtr->nextHopInterface.type )
    {
        rc = prvUtfHwFromSwDeviceNumberGet(ipUcRouteEntryPtr->nextHopInterface.devPort.devNum,
                                      &(dxChIpUcRouteEntryPtr->nextHopInterface.devPort.devNum));
        if (rc != GT_OK)
        {
            /* ignore NOT OK.
              Some tests uses not existing devNum and it's OK for tests */
            rc = GT_OK;
        }
    }

    return rc;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericIpUcRouteEntry
*
* DESCRIPTION:
*       Convert device specific IP UC route entry into generic IP UC route entry
*
* INPUTS:
*       dxChIpUcRouteEntryPtr - (pointer to) DxCh IP UC route entry
*
* OUTPUTS:
*       ipUcRouteEntryPtr - (pointer to) IP UC route entry
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertDxChToGenericIpUcRouteEntry
(
    IN  CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC   *dxChIpUcRouteEntryPtr,
    OUT PRV_TGF_IP_UC_ROUTE_ENTRY_STC            *ipUcRouteEntryPtr
)
{
    /* convert IP UC route entry from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, cmd);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, appSpecificCpuCodeEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, unicastPacketSipFilterEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, ingressMirror);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, countSet);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, trapMirrorArpBcEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, sipAccessLevel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, dipAccessLevel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, ICMPRedirectEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, scopeCheckingEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, siteId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, mtuProfileIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, nextHopVlanId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, ttlHopLimitDecEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, ttlHopLimDecOptionsExtChkByPass);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, qosProfileMarkingEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, qosProfileIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, qosPrecedence);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, modifyUp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, modifyDscp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, isTunnelStart);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, nextHopARPPointer);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpUcRouteEntryPtr, ipUcRouteEntryPtr, nextHopTunnelPointer);

    ipUcRouteEntryPtr->cpuCodeIndex = dxChIpUcRouteEntryPtr->cpuCodeIdx;

    cpssOsMemCpy((GT_VOID*) &(ipUcRouteEntryPtr->nextHopInterface),
                 (GT_VOID*) &(dxChIpUcRouteEntryPtr->nextHopInterface),
                 sizeof(dxChIpUcRouteEntryPtr->nextHopInterface));

    if( CPSS_INTERFACE_PORT_E == dxChIpUcRouteEntryPtr->nextHopInterface.type )
    {
        prvUtfSwFromHwDeviceNumberGet(dxChIpUcRouteEntryPtr->nextHopInterface.devPort.devNum,
                                      &(ipUcRouteEntryPtr->nextHopInterface.devPort.devNum));
    }
}

/*******************************************************************************
* prvTgfConvertGenericToDxChIpMcRouteEntry
*
* DESCRIPTION:
*       Convert generic IP MC route entry into device specific IP UC route entry
*
* INPUTS:
*       ipMcRouteEntryPtr - (pointer to) IP MC route entry
*
* OUTPUTS:
*       dxChIpMcRouteEntryPtr - (pointer to) DxCh IP MC route entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChIpMcRouteEntry
(
    IN  PRV_TGF_IP_MC_ROUTE_ENTRY_STC            *ipMcRouteEntryPtr,
    OUT CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC          *dxChIpMcRouteEntryPtr
)
{
    /* convert IP MC route entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, cmd);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, appSpecificCpuCodeEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, ttlHopLimitDecEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, ttlHopLimDecOptionsExtChkByPass);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, ingressMirror);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, qosProfileMarkingEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, qosProfileIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, qosPrecedence);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, modifyUp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, modifyDscp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, countSet);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, multicastRPFCheckEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, multicastRPFVlan);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, RPFFailCommand);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, scopeCheckingEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, siteId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, mtuProfileIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, internalMLLPointer);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpMcRouteEntryPtr, ipMcRouteEntryPtr, externalMLLPointer);

    dxChIpMcRouteEntryPtr->cpuCodeIdx = ipMcRouteEntryPtr->cpuCodeIndex;

    /* convert mcRPFFailCmdMode into device specific format */
    switch (ipMcRouteEntryPtr->mcRPFFailCmdMode)
    {
        case PRV_TGF_IP_MC_ROUTE_ENTRY_RPF_FAIL_COMMAND_MODE_E:
            dxChIpMcRouteEntryPtr->multicastRPFFailCommandMode = CPSS_DXCH_IP_MULTICAST_ROUTE_ENTRY_RPF_FAIL_COMMAND_MODE_E;
            break;

        case PRV_TGF_IP_MC_MLL_RPF_FAIL_COMMAND_MODE_E:
            dxChIpMcRouteEntryPtr->multicastRPFFailCommandMode = CPSS_DXCH_IP_MULTICAST_MLL_RPF_FAIL_COMMAND_MODE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChRouteType
*
* DESCRIPTION:
*       Convert generic route type into device specific route type
*
* INPUTS:
*       routeType - route type
*
* OUTPUTS:
*       dxChRouteTypePtr - (pointer to) DxCh route type
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChRouteType
(
    IN  PRV_TGF_IP_ROUTE_ENTRY_METHOD_ENT       routeType,
    OUT CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT    *dxChRouteTypePtr
)
{
    /* convert routeType into device specific format */
    switch (routeType)
    {
        case PRV_TGF_IP_ECMP_ROUTE_ENTRY_GROUP_E:
            *dxChRouteTypePtr = CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E;
            break;

        case PRV_TGF_IP_COS_ROUTE_ENTRY_GROUP_E:
            *dxChRouteTypePtr = CPSS_DXCH_IP_QOS_ROUTE_ENTRY_GROUP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericRouteType
*
* DESCRIPTION:
*       Convert device specific route type into generic route type
*
* INPUTS:
*       dxChRouteType - DxCh route type
*
* OUTPUTS:
*       routeTypePtr - (pointer to) route type
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxChToGenericRouteType
(
    IN  CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT     dxChRouteType,
    OUT PRV_TGF_IP_ROUTE_ENTRY_METHOD_ENT      *routeTypePtr
)
{
    /* convert routeType from device specific format */
    switch (dxChRouteType)
    {
        case CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E:
            *routeTypePtr = PRV_TGF_IP_ECMP_ROUTE_ENTRY_GROUP_E;
            break;

        case CPSS_DXCH_IP_QOS_ROUTE_ENTRY_GROUP_E:
            *routeTypePtr = PRV_TGF_IP_COS_ROUTE_ENTRY_GROUP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChUcNextHopInfo
*
* DESCRIPTION:
*       Convert generic uc next hop info into DX specific uc next hop info
*
* INPUTS:
*       routingMode         - routing mode
*       defUcNextHopInfoPtr - (pointer to)  generic uc next hop info
*
* OUTPUTS:
*       dxChDefUcNextHopInfoPtr - (pointer to) DX uc next hop info
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChUcNextHopInfo
(
    IN  PRV_TGF_IP_ROUTING_MODE_ENT            routingMode,
    IN  PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *defUcNextHopInfoPtr,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT *dxChDefUcNextHopInfoPtr
)
{
    GT_STATUS rc = GT_OK;
    switch (routingMode)
    {
    case PRV_TGF_IP_ROUTING_MODE_PCL_ACTION_E:
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction,
                                       &defUcNextHopInfoPtr->pclIpUcAction,
                                       pktCmd);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction,
                                       &defUcNextHopInfoPtr->pclIpUcAction,
                                       actionStop);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction,
                                       &defUcNextHopInfoPtr->pclIpUcAction,
                                       bypassBridge);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction,
                                       &defUcNextHopInfoPtr->pclIpUcAction,
                                       bypassIngressPipe);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction,
                                       &defUcNextHopInfoPtr->pclIpUcAction,
                                       egressPolicy);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.lookupConfig,
                                       &defUcNextHopInfoPtr->pclIpUcAction.lookupConfig,
                                       ipclConfigIndex);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.lookupConfig,
                                       &defUcNextHopInfoPtr->pclIpUcAction.lookupConfig,
                                       pcl0_1OverrideConfigIndex);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.lookupConfig,
                                       &defUcNextHopInfoPtr->pclIpUcAction.lookupConfig,
                                       pcl1OverrideConfigIndex);

        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.mirror,
                                       &defUcNextHopInfoPtr->pclIpUcAction.mirror,
                                       cpuCode);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.mirror,
                                       &defUcNextHopInfoPtr->pclIpUcAction.mirror,
                                       mirrorTcpRstAndFinPacketsToCpu);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.mirror,
                                       &defUcNextHopInfoPtr->pclIpUcAction.mirror,
                                       mirrorToRxAnalyzerPort);

        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.matchCounter,
                                       &defUcNextHopInfoPtr->pclIpUcAction.matchCounter,
                                       enableMatchCount);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.matchCounter,
                                       &defUcNextHopInfoPtr->pclIpUcAction.matchCounter,
                                       matchCounterIndex);

        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.qos,
                                       &defUcNextHopInfoPtr->pclIpUcAction.qos,
                                       modifyUp);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.qos,
                                       &defUcNextHopInfoPtr->pclIpUcAction.qos,
                                       modifyDscp);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.qos.qos.ingress,
                                       &defUcNextHopInfoPtr->pclIpUcAction.qos,
                                       profileAssignIndex);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.qos.qos.ingress,
                                       &defUcNextHopInfoPtr->pclIpUcAction.qos,
                                       profileIndex);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.qos.qos.ingress,
                                       &defUcNextHopInfoPtr->pclIpUcAction.qos,
                                       profilePrecedence);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.redirect,
                                       &defUcNextHopInfoPtr->pclIpUcAction.redirect,
                                       redirectCmd);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.redirect.data,
                                       &defUcNextHopInfoPtr->pclIpUcAction.redirect.data,
                                       routerLttIndex);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.redirect.data,
                                       &defUcNextHopInfoPtr->pclIpUcAction.redirect.data,
                                       routerLttIndex);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.redirect.data,
                                       &defUcNextHopInfoPtr->pclIpUcAction.redirect.data,
                                       vrfId);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.policer,
                                       &defUcNextHopInfoPtr->pclIpUcAction.policer,
                                       policerId);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.policer,
                                       &defUcNextHopInfoPtr->pclIpUcAction.policer,
                                       policerEnable);

        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.sourceId,
                                       &defUcNextHopInfoPtr->pclIpUcAction.sourceId,
                                       assignSourceId);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.sourceId,
                                       &defUcNextHopInfoPtr->pclIpUcAction.sourceId,
                                       sourceIdValue);

        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                       &defUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                       arpDaIndex);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                       &defUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                       bypassTTLCheck);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                       &defUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                       decrementTTL);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                       &defUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                       doIpUcRoute);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                       &defUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                       icmpRedirectCheck);

        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.vlan,
                                       &defUcNextHopInfoPtr->pclIpUcAction.vlan,
                                       modifyVlan);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.vlan,
                                       &defUcNextHopInfoPtr->pclIpUcAction.vlan,
                                       nestedVlan);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.vlan,
                                       &defUcNextHopInfoPtr->pclIpUcAction.vlan,
                                       precedence);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.vlan,
                                       &defUcNextHopInfoPtr->pclIpUcAction.vlan,
                                       vlanId);
        break;
    case PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E:
        /* set route type */
        rc = prvTgfConvertGenericToDxChRouteType(defUcNextHopInfoPtr->ipLttEntry.routeType,
                                                 &dxChDefUcNextHopInfoPtr->ipLttEntry.routeType);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChRouteType FAILED, rc = [%d]", rc);

            return rc;
        }
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->ipLttEntry,
                                       &defUcNextHopInfoPtr->ipLttEntry,
                                       numOfPaths);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->ipLttEntry,
                                       &defUcNextHopInfoPtr->ipLttEntry,
                                       routeEntryBaseIndex);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->ipLttEntry,
                                       &defUcNextHopInfoPtr->ipLttEntry,
                                       ucRPFCheckEnable);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->ipLttEntry,
                                       &defUcNextHopInfoPtr->ipLttEntry,
                                       sipSaCheckMismatchEnable);
        PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->ipLttEntry,
                                       &defUcNextHopInfoPtr->ipLttEntry,
                                       ipv6MCGroupScopeLevel);
        break;
    default:
            return GT_BAD_PARAM;
    }

    return rc;
}


/*******************************************************************************
* prvTgfConvertGenericToDxChMcLttEntryInfo
*
* DESCRIPTION:
*       Convert generic mc LTT info into DX specific mc LTT info
*
* INPUTS:
*       defMcRouteLttEntryPtr - (pointer to) generic mc LTT info
*
* OUTPUTS:
*       dxChDefMcRouteLttEntryPtr - (pointer to) DX mc LTT info
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChMcLttEntryInfo
(
    IN  PRV_TGF_IP_LTT_ENTRY_STC             *defMcRouteLttEntryPtr,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC      *dxChDefMcRouteLttEntryPtr
)
{
    GT_STATUS rc = GT_OK;
    /* set route type */
    rc = prvTgfConvertGenericToDxChRouteType(defMcRouteLttEntryPtr->routeType,
                                             &dxChDefMcRouteLttEntryPtr->routeType);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChRouteType FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert ltt entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChDefMcRouteLttEntryPtr, defMcRouteLttEntryPtr, numOfPaths);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChDefMcRouteLttEntryPtr, defMcRouteLttEntryPtr, routeEntryBaseIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChDefMcRouteLttEntryPtr, defMcRouteLttEntryPtr, ucRPFCheckEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChDefMcRouteLttEntryPtr, defMcRouteLttEntryPtr, sipSaCheckMismatchEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChDefMcRouteLttEntryPtr, defMcRouteLttEntryPtr, ipv6MCGroupScopeLevel);
    return rc;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericMcLttEntryInfo
*
* DESCRIPTION:
*       Convert generic mc LTT info from DX specific mc LTT info
*
* INPUTS:
*       dxChDefMcRouteLttEntryPtr - (pointer to) DX mc LTT info
*
* OUTPUTS:
*       defMcRouteLttEntryPtr - (pointer to) generic mc LTT info
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxChToGenericMcLttEntryInfo
(
    IN  CPSS_DXCH_IP_LTT_ENTRY_STC      *dxChDefMcRouteLttEntryPtr,
    OUT PRV_TGF_IP_LTT_ENTRY_STC        *defMcRouteLttEntryPtr
)
{
    GT_STATUS rc = GT_OK;

    /* set route type */
    rc = prvTgfConvertDxChToGenericRouteType(dxChDefMcRouteLttEntryPtr->routeType,
                                             &(defMcRouteLttEntryPtr->routeType));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertDxChToGenericRouteType FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert ltt entry into device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChDefMcRouteLttEntryPtr, defMcRouteLttEntryPtr, numOfPaths);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChDefMcRouteLttEntryPtr, defMcRouteLttEntryPtr, routeEntryBaseIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChDefMcRouteLttEntryPtr, defMcRouteLttEntryPtr, ucRPFCheckEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChDefMcRouteLttEntryPtr, defMcRouteLttEntryPtr, sipSaCheckMismatchEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChDefMcRouteLttEntryPtr, defMcRouteLttEntryPtr, ipv6MCGroupScopeLevel);

    return rc;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericUcNextHopInfo
*
* DESCRIPTION:
*       Convert generic UC next hop info from DX specific UC next hop info
*
* INPUTS:
*       routingMode             - routing mode
*       dxChDefUcNextHopInfoPtr - (pointer to) DxCh UC next hop info
*
* OUTPUTS:
*       defUcNextHopInfoPtr - (pointer to) generic uc next hop info
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxChToGenericUcNextHopInfo
(
    IN  PRV_TGF_IP_ROUTING_MODE_ENT            routingMode,
    IN  CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT *dxChDefUcNextHopInfoPtr,
    OUT PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *defUcNextHopInfoPtr
)
{
    GT_STATUS rc = GT_OK;

    switch (routingMode)
    {
        case PRV_TGF_IP_ROUTING_MODE_PCL_ACTION_E:
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction,
                                           &defUcNextHopInfoPtr->pclIpUcAction,
                                           pktCmd);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction,
                                           &defUcNextHopInfoPtr->pclIpUcAction,
                                           actionStop);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction,
                                           &defUcNextHopInfoPtr->pclIpUcAction,
                                           bypassBridge);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction,
                                           &defUcNextHopInfoPtr->pclIpUcAction,
                                           bypassIngressPipe);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction,
                                           &defUcNextHopInfoPtr->pclIpUcAction,
                                           egressPolicy);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.lookupConfig,
                                           &defUcNextHopInfoPtr->pclIpUcAction.lookupConfig,
                                           ipclConfigIndex);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.lookupConfig,
                                           &defUcNextHopInfoPtr->pclIpUcAction.lookupConfig,
                                           pcl0_1OverrideConfigIndex);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.lookupConfig,
                                           &defUcNextHopInfoPtr->pclIpUcAction.lookupConfig,
                                           pcl1OverrideConfigIndex);
    
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.mirror,
                                           &defUcNextHopInfoPtr->pclIpUcAction.mirror,
                                           cpuCode);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.mirror,
                                           &defUcNextHopInfoPtr->pclIpUcAction.mirror,
                                           mirrorTcpRstAndFinPacketsToCpu);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.mirror,
                                           &defUcNextHopInfoPtr->pclIpUcAction.mirror,
                                           mirrorToRxAnalyzerPort);
    
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.matchCounter,
                                           &defUcNextHopInfoPtr->pclIpUcAction.matchCounter,
                                           enableMatchCount);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.matchCounter,
                                           &defUcNextHopInfoPtr->pclIpUcAction.matchCounter,
                                           matchCounterIndex);
    
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.qos,
                                           &defUcNextHopInfoPtr->pclIpUcAction.qos,
                                           modifyUp);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.qos,
                                           &defUcNextHopInfoPtr->pclIpUcAction.qos,
                                           modifyDscp);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.qos.qos.ingress,
                                           &defUcNextHopInfoPtr->pclIpUcAction.qos,
                                           profileAssignIndex);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.qos.qos.ingress,
                                           &defUcNextHopInfoPtr->pclIpUcAction.qos,
                                           profileIndex);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.qos.qos.ingress,
                                           &defUcNextHopInfoPtr->pclIpUcAction.qos,
                                           profilePrecedence);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.redirect,
                                           &defUcNextHopInfoPtr->pclIpUcAction.redirect,
                                           redirectCmd);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.redirect.data,
                                           &defUcNextHopInfoPtr->pclIpUcAction.redirect.data,
                                           routerLttIndex);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.redirect.data,
                                           &defUcNextHopInfoPtr->pclIpUcAction.redirect.data,
                                           routerLttIndex);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.redirect.data,
                                           &defUcNextHopInfoPtr->pclIpUcAction.redirect.data,
                                           vrfId);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.policer,
                                           &defUcNextHopInfoPtr->pclIpUcAction.policer,
                                           policerId);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.policer,
                                           &defUcNextHopInfoPtr->pclIpUcAction.policer,
                                           policerEnable);
    
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.sourceId,
                                           &defUcNextHopInfoPtr->pclIpUcAction.sourceId,
                                           assignSourceId);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.sourceId,
                                           &defUcNextHopInfoPtr->pclIpUcAction.sourceId,
                                           sourceIdValue);
    
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                           &defUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                           arpDaIndex);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                           &defUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                           bypassTTLCheck);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                           &defUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                           decrementTTL);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                           &defUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                           doIpUcRoute);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                           &defUcNextHopInfoPtr->pclIpUcAction.ipUcRoute,
                                           icmpRedirectCheck);
    
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.vlan,
                                           &defUcNextHopInfoPtr->pclIpUcAction.vlan,
                                           modifyVlan);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.vlan,
                                           &defUcNextHopInfoPtr->pclIpUcAction.vlan,
                                           nestedVlan);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.vlan,
                                           &defUcNextHopInfoPtr->pclIpUcAction.vlan,
                                           precedence);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->pclIpUcAction.vlan,
                                           &defUcNextHopInfoPtr->pclIpUcAction.vlan,
                                           vlanId);
            break;

        case PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E:
            /* set route type */
            rc = prvTgfConvertDxChToGenericRouteType(dxChDefUcNextHopInfoPtr->ipLttEntry.routeType,
                                                     &defUcNextHopInfoPtr->ipLttEntry.routeType);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertDxChToGenericRouteType FAILED, rc = [%d]", rc);
    
                return rc;
            }

            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->ipLttEntry,
                                           &defUcNextHopInfoPtr->ipLttEntry,
                                           numOfPaths);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->ipLttEntry,
                                           &defUcNextHopInfoPtr->ipLttEntry,
                                           routeEntryBaseIndex);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->ipLttEntry,
                                           &defUcNextHopInfoPtr->ipLttEntry,
                                           ucRPFCheckEnable);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->ipLttEntry,
                                           &defUcNextHopInfoPtr->ipLttEntry,
                                           sipSaCheckMismatchEnable);
            PRV_TGF_STC_D2S_FIELD_COPY_MAC(&dxChDefUcNextHopInfoPtr->ipLttEntry,
                                           &defUcNextHopInfoPtr->ipLttEntry,
                                           ipv6MCGroupScopeLevel);
            break;

        default:
            return GT_BAD_PARAM;
    }

    return rc;
}
#endif /* CHX_FAMILY */


#ifdef EXMXPM_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToExMxPmIpUcRouteEntry
*
* DESCRIPTION:
*       Convert generic IP UC route entry into device specific IP UC route entry
*
* INPUTS:
*       ipUcRouteEntryPtr - (pointer to) IP UC route entry
*
* OUTPUTS:
*       exMxPmIpUcRouteEntryPtr - (pointer to) ExMxPm IP UC route entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmIpUcRouteEntry
(
    IN  PRV_TGF_IP_UC_ROUTE_ENTRY_STC            *ipUcRouteEntryPtr,
    OUT CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_FORMAT_STC *exMxPmIpUcRouteEntryPtr
)
{
    /* convert IP UC route entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, cpuCodeIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, appSpecificCpuCodeEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, sipAccessLevel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, dipAccessLevel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, mtuProfileIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, nextHopVlanId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, bypassTtlExceptionCheckEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, ageRefresh);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, sipSaCheckEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, ttl);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, mplsLabel);

    exMxPmIpUcRouteEntryPtr->command                 = ipUcRouteEntryPtr->cmd;
    exMxPmIpUcRouteEntryPtr->ucPacketSipFilterEnable = ipUcRouteEntryPtr->unicastPacketSipFilterEnable;
    exMxPmIpUcRouteEntryPtr->mirrorToIngressAnalyzer = ipUcRouteEntryPtr->ingressMirror;
    exMxPmIpUcRouteEntryPtr->trapMirrorArpBcToCpu    = ipUcRouteEntryPtr->trapMirrorArpBcEnable;
    exMxPmIpUcRouteEntryPtr->icmpRedirectEnable      = ipUcRouteEntryPtr->ICMPRedirectEnable;
    exMxPmIpUcRouteEntryPtr->ipv6ScopeCheckEnable    = ipUcRouteEntryPtr->scopeCheckingEnable;
    exMxPmIpUcRouteEntryPtr->ipv6DestinationSiteId   = ipUcRouteEntryPtr->siteId;

    /* convert nextHop Interface into device specific format */
    switch (ipUcRouteEntryPtr->nextHopIf.outlifType)
    {
        case PRV_TGF_OUTLIF_TYPE_LL_E:
            exMxPmIpUcRouteEntryPtr->nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
            exMxPmIpUcRouteEntryPtr->nextHopIf.outlifPointer.arpPtr =
                ipUcRouteEntryPtr->nextHopIf.outlifPointer.arpPtr;

            break;

        case PRV_TGF_OUTLIF_TYPE_DIT_E:
            exMxPmIpUcRouteEntryPtr->nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_DIT_E;
            exMxPmIpUcRouteEntryPtr->nextHopIf.outlifPointer.ditPtr =
                ipUcRouteEntryPtr->nextHopIf.outlifPointer.ditPtr;

            break;

        case PRV_TGF_OUTLIF_TYPE_TUNNEL_E:
            {
                exMxPmIpUcRouteEntryPtr->nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E;

                /* convert passengerPacketType into device specific format */
                switch (ipUcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.passengerPacketType)
                {
                    case PRV_TGF_TS_PASSENGER_PACKET_TYPE_ETHERNET_E:
                        exMxPmIpUcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.passengerPacketType =
                            CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ETHERNET_E;
                        break;

                    case PRV_TGF_TS_PASSENGER_PACKET_TYPE_OTHER_E:
                        exMxPmIpUcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.passengerPacketType =
                            CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_OTHER_E;
                        break;

                    default:
                        return GT_BAD_PARAM;
                }

                exMxPmIpUcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.ptr =
                    ipUcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.ptr;
            }
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* set interface info */
    cpssOsMemCpy((GT_VOID*) &(exMxPmIpUcRouteEntryPtr->nextHopIf.interfaceInfo),
                 (GT_VOID*) &(ipUcRouteEntryPtr->nextHopIf.interfaceInfo),
                 sizeof(ipUcRouteEntryPtr->nextHopIf.interfaceInfo));

    /* set qos modify params */
    cpssOsMemCpy((GT_VOID*) &(exMxPmIpUcRouteEntryPtr->qosParamsModify),
                 (GT_VOID*) &(ipUcRouteEntryPtr->qosParamsModify),
                 sizeof(ipUcRouteEntryPtr->qosParamsModify));

    /* set qos params */
    cpssOsMemCpy((GT_VOID*) &(exMxPmIpUcRouteEntryPtr->qosParams),
                 (GT_VOID*) &(ipUcRouteEntryPtr->qosParams),
                 sizeof(ipUcRouteEntryPtr->qosParams));

    /* convert ucRpfOrIcmpRedirectIfMode into device specific format */
    switch (ipUcRouteEntryPtr->ucRpfOrIcmpRedirectIfMode)
    {
        case PRV_TGF_RPF_IF_MODE_DISABLED_E:
            exMxPmIpUcRouteEntryPtr->ucRpfOrIcmpRedirectIfMode = CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E;
            break;

        case PRV_TGF_RPF_IF_MODE_PORT_E:
            exMxPmIpUcRouteEntryPtr->ucRpfOrIcmpRedirectIfMode = CPSS_EXMXPM_RPF_IF_MODE_PORT_E;
            break;

        case PRV_TGF_RPF_IF_MODE_L2_VLAN_E:
            exMxPmIpUcRouteEntryPtr->ucRpfOrIcmpRedirectIfMode = CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert cntrSetIndex into device specific format */
    switch (ipUcRouteEntryPtr->countSet)
    {
        case CPSS_IP_CNT_SET0_E:
            exMxPmIpUcRouteEntryPtr->cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E;
            break;

        case CPSS_IP_CNT_SET1_E:
            exMxPmIpUcRouteEntryPtr->cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_1_E;
            break;

        case CPSS_IP_CNT_SET2_E:
            exMxPmIpUcRouteEntryPtr->cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_2_E;
            break;

        case CPSS_IP_CNT_SET3_E:
            exMxPmIpUcRouteEntryPtr->cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_3_E;
            break;

        case CPSS_IP_CNT_NO_SET_E:
            exMxPmIpUcRouteEntryPtr->cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_DISABLE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert hopLimitMode into device specific format */
    switch (ipUcRouteEntryPtr->hopLimitMode)
    {
        case PRV_TGF_IP_HOP_LIMIT_NOT_DECREMENTED_E:
            exMxPmIpUcRouteEntryPtr->hopLimitMode = CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
            break;

        case PRV_TGF_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E:
            exMxPmIpUcRouteEntryPtr->hopLimitMode = CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert ttlMode into device specific format */
    switch (ipUcRouteEntryPtr->ttlMode)
    {
        case PRV_TGF_IP_TTL_NOT_DECREMENTED_E:
            exMxPmIpUcRouteEntryPtr->ttlMode = CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
            break;

        case PRV_TGF_IP_TTL_DECREMENTED_BY_ONE_E:
            exMxPmIpUcRouteEntryPtr->ttlMode = CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ONE_E;
            break;

        case PRV_TGF_IP_TTL_DECREMENTED_BY_ENTRY_E:
            exMxPmIpUcRouteEntryPtr->ttlMode = CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ENTRY_E;
            break;

        case PRV_TGF_IP_TTL_COPY_ENTRY_E:
            exMxPmIpUcRouteEntryPtr->ttlMode = CPSS_EXMXPM_IP_TTL_COPY_ENTRY_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert mplsCommand into device specific format */
    switch (ipUcRouteEntryPtr->mplsCommand)
    {
        case PRV_TGF_MPLS_NOP_CMD_E:
            exMxPmIpUcRouteEntryPtr->mplsCommand = CPSS_EXMXPM_MPLS_NOP_CMD_E;
            break;

        case PRV_TGF_MPLS_SWAP_CMD_E:
            exMxPmIpUcRouteEntryPtr->mplsCommand = CPSS_EXMXPM_MPLS_SWAP_CMD_E;
            break;

        case PRV_TGF_MPLS_PUSH_CMD_E:
            exMxPmIpUcRouteEntryPtr->mplsCommand = CPSS_EXMXPM_MPLS_PUSH_CMD_E;
            break;

        case PRV_TGF_MPLS_PHP_CMD_E:
            exMxPmIpUcRouteEntryPtr->mplsCommand = CPSS_EXMXPM_MPLS_PHP_CMD_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertExMxPmToGenericIpUcRouteEntry
*
* DESCRIPTION:
*       Convert device specific IP UC route entry into generic IP UC route entry
*
* INPUTS:
*       exMxPmIpUcRouteEntryPtr - (pointer to) ExMxPm IP UC route entry
*
* OUTPUTS:
*       ipUcRouteEntryPtr - (pointer to) IP UC route entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertExMxPmToGenericIpUcRouteEntry
(
    IN  CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_FORMAT_STC *exMxPmIpUcRouteEntryPtr,
    OUT PRV_TGF_IP_UC_ROUTE_ENTRY_STC            *ipUcRouteEntryPtr
)
{
    /* convert IP UC route entry from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, cpuCodeIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, appSpecificCpuCodeEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, sipAccessLevel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, dipAccessLevel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, mtuProfileIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, nextHopVlanId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, bypassTtlExceptionCheckEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, ageRefresh);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, sipSaCheckEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, ttl);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpUcRouteEntryPtr, ipUcRouteEntryPtr, mplsLabel);

    ipUcRouteEntryPtr->cmd                          = exMxPmIpUcRouteEntryPtr->command;
    ipUcRouteEntryPtr->unicastPacketSipFilterEnable = exMxPmIpUcRouteEntryPtr->ucPacketSipFilterEnable;
    ipUcRouteEntryPtr->ingressMirror                = exMxPmIpUcRouteEntryPtr->mirrorToIngressAnalyzer;
    ipUcRouteEntryPtr->trapMirrorArpBcEnable        = exMxPmIpUcRouteEntryPtr->trapMirrorArpBcToCpu;
    ipUcRouteEntryPtr->ICMPRedirectEnable           = exMxPmIpUcRouteEntryPtr->icmpRedirectEnable;
    ipUcRouteEntryPtr->scopeCheckingEnable          = exMxPmIpUcRouteEntryPtr->ipv6ScopeCheckEnable;
    ipUcRouteEntryPtr->siteId                       = exMxPmIpUcRouteEntryPtr->ipv6DestinationSiteId;

    /* convert nextHop Interface from device specific format */
    switch (exMxPmIpUcRouteEntryPtr->nextHopIf.outlifType)
    {
        case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
            ipUcRouteEntryPtr->nextHopIf.outlifType = PRV_TGF_OUTLIF_TYPE_LL_E;
            ipUcRouteEntryPtr->nextHopIf.outlifPointer.arpPtr =
                exMxPmIpUcRouteEntryPtr->nextHopIf.outlifPointer.arpPtr;

            break;

        case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
            ipUcRouteEntryPtr->nextHopIf.outlifType = PRV_TGF_OUTLIF_TYPE_DIT_E;
            ipUcRouteEntryPtr->nextHopIf.outlifPointer.ditPtr =
                exMxPmIpUcRouteEntryPtr->nextHopIf.outlifPointer.ditPtr;

            break;

        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
            {
                ipUcRouteEntryPtr->nextHopIf.outlifType = PRV_TGF_OUTLIF_TYPE_TUNNEL_E;

                /* convert passengerPacketType into device specific format */
                switch (exMxPmIpUcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.passengerPacketType)
                {
                    case CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ETHERNET_E:
                        ipUcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.passengerPacketType =
                            PRV_TGF_TS_PASSENGER_PACKET_TYPE_ETHERNET_E;
                        break;

                    case CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_OTHER_E:
                        ipUcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.passengerPacketType =
                            PRV_TGF_TS_PASSENGER_PACKET_TYPE_OTHER_E;
                        break;

                    default:
                        return GT_BAD_PARAM;
                }

                ipUcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.ptr =
                    exMxPmIpUcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.ptr;
            }
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* set interface info */
    cpssOsMemCpy((GT_VOID*) &(ipUcRouteEntryPtr->nextHopIf.interfaceInfo),
                 (GT_VOID*) &(exMxPmIpUcRouteEntryPtr->nextHopIf.interfaceInfo),
                 sizeof(exMxPmIpUcRouteEntryPtr->nextHopIf.interfaceInfo));

    /* set qos modify params */
    cpssOsMemCpy((GT_VOID*) &(ipUcRouteEntryPtr->qosParamsModify),
                 (GT_VOID*) &(exMxPmIpUcRouteEntryPtr->qosParamsModify),
                 sizeof(exMxPmIpUcRouteEntryPtr->qosParamsModify));

    /* set qos params */
    cpssOsMemCpy((GT_VOID*) &(ipUcRouteEntryPtr->qosParams),
                 (GT_VOID*) &(exMxPmIpUcRouteEntryPtr->qosParams),
                 sizeof(exMxPmIpUcRouteEntryPtr->qosParams));

    /* convert ucRpfOrIcmpRedirectIfMode from device specific format */
    switch (exMxPmIpUcRouteEntryPtr->ucRpfOrIcmpRedirectIfMode)
    {
        case CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E:
            ipUcRouteEntryPtr->ucRpfOrIcmpRedirectIfMode = PRV_TGF_RPF_IF_MODE_DISABLED_E;
            break;

        case CPSS_EXMXPM_RPF_IF_MODE_PORT_E:
            ipUcRouteEntryPtr->ucRpfOrIcmpRedirectIfMode = PRV_TGF_RPF_IF_MODE_PORT_E;
            break;

        case CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E:
            ipUcRouteEntryPtr->ucRpfOrIcmpRedirectIfMode = PRV_TGF_RPF_IF_MODE_L2_VLAN_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert cntrSetIndex from device specific format */
    switch (exMxPmIpUcRouteEntryPtr->cntrSetIndex)
    {
        case CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E:
            ipUcRouteEntryPtr->countSet = CPSS_IP_CNT_SET0_E;
            break;

        case CPSS_EXMXPM_IP_COUNTER_SET_INDEX_1_E:
            ipUcRouteEntryPtr->countSet = CPSS_IP_CNT_SET1_E;
            break;

        case CPSS_EXMXPM_IP_COUNTER_SET_INDEX_2_E:
            ipUcRouteEntryPtr->countSet = CPSS_IP_CNT_SET2_E;
            break;

        case CPSS_EXMXPM_IP_COUNTER_SET_INDEX_3_E:
            ipUcRouteEntryPtr->countSet = CPSS_IP_CNT_SET3_E;
            break;

        case CPSS_EXMXPM_IP_COUNTER_DISABLE_E:
            ipUcRouteEntryPtr->countSet = CPSS_IP_CNT_NO_SET_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert hopLimitMode from device specific format */
    switch (exMxPmIpUcRouteEntryPtr->hopLimitMode)
    {
        case CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E:
            ipUcRouteEntryPtr->hopLimitMode = PRV_TGF_IP_HOP_LIMIT_NOT_DECREMENTED_E;
            break;

        case CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E:
            ipUcRouteEntryPtr->hopLimitMode = PRV_TGF_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert ttlMode from device specific format */
    switch (exMxPmIpUcRouteEntryPtr->ttlMode)
    {
        case CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E:
            ipUcRouteEntryPtr->ttlMode = PRV_TGF_IP_TTL_NOT_DECREMENTED_E;
            break;

        case CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ONE_E:
            ipUcRouteEntryPtr->ttlMode = PRV_TGF_IP_TTL_DECREMENTED_BY_ONE_E;
            break;

        case CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ENTRY_E:
            ipUcRouteEntryPtr->ttlMode = PRV_TGF_IP_TTL_DECREMENTED_BY_ENTRY_E;
            break;

        case CPSS_EXMXPM_IP_TTL_COPY_ENTRY_E:
            ipUcRouteEntryPtr->ttlMode = PRV_TGF_IP_TTL_COPY_ENTRY_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert mplsCommand from device specific format */
    switch (exMxPmIpUcRouteEntryPtr->mplsCommand)
    {
        case CPSS_EXMXPM_MPLS_NOP_CMD_E:
            ipUcRouteEntryPtr->mplsCommand = PRV_TGF_MPLS_NOP_CMD_E;
            break;

        case CPSS_EXMXPM_MPLS_SWAP_CMD_E:
            ipUcRouteEntryPtr->mplsCommand = PRV_TGF_MPLS_SWAP_CMD_E;
            break;

        case CPSS_EXMXPM_MPLS_PUSH_CMD_E:
            ipUcRouteEntryPtr->mplsCommand = PRV_TGF_MPLS_PUSH_CMD_E;
            break;

        case CPSS_EXMXPM_MPLS_PHP_CMD_E:
            ipUcRouteEntryPtr->mplsCommand = PRV_TGF_MPLS_PHP_CMD_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmIpMcRouteEntry
*
* DESCRIPTION:
*       Convert generic IP MC route entry into device specific IP MC route entry
*
* INPUTS:
*       ipMcRouteEntryPtr - (pointer to) IP MC route entry
*
* OUTPUTS:
*       exMxPmIpMcRouteEntryPtr - (pointer to) ExMxPm IP MC route entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmIpMcRouteEntry
(
    IN  PRV_TGF_IP_MC_ROUTE_ENTRY_STC            *ipMcRouteEntryPtr,
    OUT CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC        *exMxPmIpMcRouteEntryPtr
)
{
    /* convert IP MC route entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpMcRouteEntryPtr, ipMcRouteEntryPtr, bypassTtlExceptionCheckEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpMcRouteEntryPtr, ipMcRouteEntryPtr, cpuCodeIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpMcRouteEntryPtr, ipMcRouteEntryPtr, ageRefresh);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpMcRouteEntryPtr, ipMcRouteEntryPtr, ipv6ScopeLevel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpMcRouteEntryPtr, ipMcRouteEntryPtr, mtuProfileIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpMcRouteEntryPtr, ipMcRouteEntryPtr, appSpecificCpuCodeEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpMcRouteEntryPtr, ipMcRouteEntryPtr, ttl);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpMcRouteEntryPtr, ipMcRouteEntryPtr, mplsLabel);

    exMxPmIpMcRouteEntryPtr->command                 = ipMcRouteEntryPtr->cmd;
    exMxPmIpMcRouteEntryPtr->ipv6ScopeCheckEnable    = ipMcRouteEntryPtr->scopeCheckingEnable;
    exMxPmIpMcRouteEntryPtr->ipv6DestinationSiteId   = ipMcRouteEntryPtr->siteId;
    exMxPmIpMcRouteEntryPtr->mirrorToIngressAnalyzer = ipMcRouteEntryPtr->ingressMirror;

    /* set RPF interface */
    cpssOsMemCpy((GT_VOID*) &(exMxPmIpMcRouteEntryPtr->mcRpfIf),
                 (GT_VOID*) &(ipMcRouteEntryPtr->mcRpfIf),
                 sizeof(ipMcRouteEntryPtr->mcRpfIf));

    /* convert nextHop Interface into device specific format */
    switch (ipMcRouteEntryPtr->nextHopIf.outlifType)
    {
        case PRV_TGF_OUTLIF_TYPE_LL_E:
            exMxPmIpMcRouteEntryPtr->nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
            exMxPmIpMcRouteEntryPtr->nextHopIf.outlifPointer.arpPtr =
                ipMcRouteEntryPtr->nextHopIf.outlifPointer.arpPtr;

            break;

        case PRV_TGF_OUTLIF_TYPE_DIT_E:
            exMxPmIpMcRouteEntryPtr->nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_DIT_E;
            exMxPmIpMcRouteEntryPtr->nextHopIf.outlifPointer.ditPtr =
                ipMcRouteEntryPtr->nextHopIf.outlifPointer.ditPtr;

            break;

        case PRV_TGF_OUTLIF_TYPE_TUNNEL_E:
            {
                exMxPmIpMcRouteEntryPtr->nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E;

                /* convert passengerPacketType into device specific format */
                switch (ipMcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.passengerPacketType)
                {
                    case PRV_TGF_TS_PASSENGER_PACKET_TYPE_ETHERNET_E:
                        exMxPmIpMcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.passengerPacketType =
                            CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ETHERNET_E;
                        break;

                    case PRV_TGF_TS_PASSENGER_PACKET_TYPE_OTHER_E:
                        exMxPmIpMcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.passengerPacketType =
                            CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_OTHER_E;
                        break;

                    default:
                        return GT_BAD_PARAM;
                }

                exMxPmIpMcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.ptr =
                    ipMcRouteEntryPtr->nextHopIf.outlifPointer.tunnelStartPtr.ptr;
            }
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* set interface info */
    cpssOsMemCpy((GT_VOID*) &(exMxPmIpMcRouteEntryPtr->nextHopIf.interfaceInfo),
                 (GT_VOID*) &(ipMcRouteEntryPtr->nextHopIf.interfaceInfo),
                 sizeof(ipMcRouteEntryPtr->nextHopIf.interfaceInfo));

    /* set qos modify params */
    cpssOsMemCpy((GT_VOID*) &(exMxPmIpMcRouteEntryPtr->qosParamsModify),
                 (GT_VOID*) &(ipMcRouteEntryPtr->qosParamsModify),
                 sizeof(ipMcRouteEntryPtr->qosParamsModify));

    /* set qos params */
    cpssOsMemCpy((GT_VOID*) &(exMxPmIpMcRouteEntryPtr->qosParams),
                 (GT_VOID*) &(ipMcRouteEntryPtr->qosParams),
                 sizeof(ipMcRouteEntryPtr->qosParams));

    /* convert mcRpfIfMode into device specific format */
    switch (ipMcRouteEntryPtr->mcRpfIfMode)
    {
        case PRV_TGF_RPF_IF_MODE_DISABLED_E:
            exMxPmIpMcRouteEntryPtr->mcRpfIfMode = CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E;
            break;

        case PRV_TGF_RPF_IF_MODE_PORT_E:
            exMxPmIpMcRouteEntryPtr->mcRpfIfMode = CPSS_EXMXPM_RPF_IF_MODE_PORT_E;
            break;

        case PRV_TGF_RPF_IF_MODE_L2_VLAN_E:
            exMxPmIpMcRouteEntryPtr->mcRpfIfMode = CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert RPFFailCommand into device specific format */
    switch (ipMcRouteEntryPtr->RPFFailCommand)
    {
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            exMxPmIpMcRouteEntryPtr->mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_TRAP_TO_CPU_E;
            break;

        case CPSS_PACKET_CMD_DROP_HARD_E:
            exMxPmIpMcRouteEntryPtr->mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E;
            break;

        case CPSS_PACKET_CMD_DROP_SOFT_E:
            exMxPmIpMcRouteEntryPtr->mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_SOFT_E;
            break;

        case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            exMxPmIpMcRouteEntryPtr->mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_BRIDGE_AND_MIRROR_E;
            break;

        case CPSS_PACKET_CMD_BRIDGE_E:
            exMxPmIpMcRouteEntryPtr->mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_BRIDGE_E;
            break;

        case CPSS_PACKET_CMD_NONE_E:
            exMxPmIpMcRouteEntryPtr->mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DOWNSTREAM_INTERFACE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert cntrSetIndex into device specific format */
    switch (ipMcRouteEntryPtr->countSet)
    {
        case CPSS_IP_CNT_SET0_E:
            exMxPmIpMcRouteEntryPtr->cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E;
            break;

        case CPSS_IP_CNT_SET1_E:
            exMxPmIpMcRouteEntryPtr->cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_1_E;
            break;

        case CPSS_IP_CNT_SET2_E:
            exMxPmIpMcRouteEntryPtr->cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_2_E;
            break;

        case CPSS_IP_CNT_SET3_E:
            exMxPmIpMcRouteEntryPtr->cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_3_E;
            break;

        case CPSS_IP_CNT_NO_SET_E:
            exMxPmIpMcRouteEntryPtr->cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_DISABLE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert hopLimitMode into device specific format */
    switch (ipMcRouteEntryPtr->hopLimitMode)
    {
        case PRV_TGF_IP_HOP_LIMIT_NOT_DECREMENTED_E:
            exMxPmIpMcRouteEntryPtr->hopLimitMode = CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
            break;

        case PRV_TGF_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E:
            exMxPmIpMcRouteEntryPtr->hopLimitMode = CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert ttlMode into device specific format */
    switch (ipMcRouteEntryPtr->ttlMode)
    {
        case PRV_TGF_IP_TTL_NOT_DECREMENTED_E:
            exMxPmIpMcRouteEntryPtr->ttlMode = CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
            break;

        case PRV_TGF_IP_TTL_DECREMENTED_BY_ONE_E:
            exMxPmIpMcRouteEntryPtr->ttlMode = CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ONE_E;
            break;

        case PRV_TGF_IP_TTL_DECREMENTED_BY_ENTRY_E:
            exMxPmIpMcRouteEntryPtr->ttlMode = CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ENTRY_E;
            break;

        case PRV_TGF_IP_TTL_COPY_ENTRY_E:
            exMxPmIpMcRouteEntryPtr->ttlMode = CPSS_EXMXPM_IP_TTL_COPY_ENTRY_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert mplsCommand into device specific format */
    switch (ipMcRouteEntryPtr->mplsCommand)
    {
        case PRV_TGF_MPLS_NOP_CMD_E:
            exMxPmIpMcRouteEntryPtr->mplsCommand = CPSS_EXMXPM_MPLS_NOP_CMD_E;
            break;

        case PRV_TGF_MPLS_SWAP_CMD_E:
            exMxPmIpMcRouteEntryPtr->mplsCommand = CPSS_EXMXPM_MPLS_SWAP_CMD_E;
            break;

        case PRV_TGF_MPLS_PUSH_CMD_E:
            exMxPmIpMcRouteEntryPtr->mplsCommand = CPSS_EXMXPM_MPLS_PUSH_CMD_E;
            break;

        case PRV_TGF_MPLS_PHP_CMD_E:
            exMxPmIpMcRouteEntryPtr->mplsCommand = CPSS_EXMXPM_MPLS_PHP_CMD_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmRouteType
*
* DESCRIPTION:
*       Convert generic route type into device specific route type
*
* INPUTS:
*       routeType - route type
*
* OUTPUTS:
*       exMxPmRouteTypePtr - (pointer to) ExMxPm route type
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmRouteType
(
    IN  PRV_TGF_IP_ROUTE_ENTRY_METHOD_ENT       routeType,
    OUT CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT  *exMxPmRouteTypePtr
)
{
    /* convert routeType into device specific format */
    switch (routeType)
    {
        case PRV_TGF_IP_ECMP_ROUTE_ENTRY_GROUP_E:
            *exMxPmRouteTypePtr = CPSS_EXMXPM_IP_ECMP_ROUTE_ENTRY_GROUP_E;
            break;

        case PRV_TGF_IP_COS_ROUTE_ENTRY_GROUP_E:
            *exMxPmRouteTypePtr = CPSS_EXMXPM_IP_COS_ROUTE_ENTRY_GROUP_E;
            break;

        case PRV_TGF_IP_REGULAR_ROUTE_ENTRY_E:
            *exMxPmRouteTypePtr = CPSS_EXMXPM_IP_REGULAR_ROUTE_ENTRY_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}
#endif /* EXMXPM_FAMILY */


/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfIpRoutingModeGet
*
* DESCRIPTION:
*       Return the current routing mode
*
* INPUTS:
*       none
*
* OUTPUTS:
*       routingModePtr - the current routing mode
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on devNum not active.
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpRoutingModeGet
(
    OUT PRV_TGF_IP_ROUTING_MODE_ENT *routingModePtr
)
{
    GT_STATUS rc = GT_OK;
#ifdef CHX_FAMILY
    GT_U32 value;
    if ((appDemoDbEntryGet((GT_U8*)"usePolicyBasedRouting", &value) == GT_OK) && (value == GT_TRUE))
    {
        *routingModePtr = PRV_TGF_IP_ROUTING_MODE_PCL_ACTION_E;
    }
    else
        *routingModePtr = PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(routingModePtr);
    /* NOT IMPLEMENTED FOR PUMA */
    rc = GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

    return rc;
}

/*******************************************************************************
* prvTgfIpUcRouteEntriesWrite
*
* DESCRIPTION:
*    Writes an array of uc route entries to hw
*
* INPUTS:
*       devNum              - the device number
*       baseRouteEntryIndex - the index from which to write the array
*       routeEntriesArray   - the uc route entries array
*       numOfRouteEntries   - the number route entries in the array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on devNum not active.
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpUcRouteEntriesWrite
(
    IN GT_U32                         baseRouteEntryIndex,
    IN PRV_TGF_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray,
    IN GT_U32                         numOfRouteEntries
)
{
    GT_U8       devNum    = 0;
    GT_STATUS   rc, rc1   = GT_OK;
    GT_U32      entryIter = 0;
#ifdef CHX_FAMILY
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *dxChRouteEntriesArray;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC *exMxPmRouteEntriesArray;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* allocate memory */
    dxChRouteEntriesArray = cpssOsMalloc(numOfRouteEntries * sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));

    /* reset variables */
    cpssOsMemSet((GT_VOID*) dxChRouteEntriesArray, 0,
                 numOfRouteEntries * sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));

    /* convert IP UC route entry into device specific format */
    for (entryIter = 0; entryIter < numOfRouteEntries; entryIter++)
    {
        dxChRouteEntriesArray[entryIter].type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;

        /* convert IP UC route entry */
        rc = prvTgfConvertGenericToDxChIpUcRouteEntry(&routeEntriesArray[entryIter],
                                                 &(dxChRouteEntriesArray[entryIter].entry.regularEntry));

        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIpUcRouteEntry FAILED, rc = [%d]", rc);

            /* free allocated memory */
            cpssOsFree(dxChRouteEntriesArray);

            return rc;
        }
    }

    /* prepare device iterator */
    rc = prvUtfNextNotApplicableDeviceReset(&devNum, UTF_NONE_FAMILY_E);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextNotApplicableDeviceReset FAILED, rc = [%d]", rc);

        /* free allocated memory */
        cpssOsFree(dxChRouteEntriesArray);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChIpUcRouteEntriesWrite(devNum, baseRouteEntryIndex, dxChRouteEntriesArray, numOfRouteEntries);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpUcRouteEntriesWrite FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    /* free allocated memory */
    cpssOsFree(dxChRouteEntriesArray);

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* allocate memory */
    exMxPmRouteEntriesArray = cpssOsMalloc(numOfRouteEntries * sizeof(CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC));

    /* reset variables */
    cpssOsMemSet((GT_VOID*) exMxPmRouteEntriesArray, 0,
                 numOfRouteEntries * sizeof(CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC));

    /* convert IP UC route entry into device specific format */
    for (entryIter = 0; entryIter < numOfRouteEntries; entryIter++)
    {
        exMxPmRouteEntriesArray[entryIter].type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        /* convert IP UC route entry */
        rc = prvTgfConvertGenericToExMxPmIpUcRouteEntry(&routeEntriesArray[entryIter],
                                                        &(exMxPmRouteEntriesArray[entryIter].entry.regularEntry));
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIpUcRouteEntry FAILED, rc = [%d]", rc);

            /* free allocated memory */
            cpssOsFree(exMxPmRouteEntriesArray);

            return rc;
        }
    }

    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmIpUcRouteEntriesWrite(devNum, baseRouteEntryIndex, numOfRouteEntries, exMxPmRouteEntriesArray);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmIpUcRouteEntriesWrite FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    /* free allocated memory */
    cpssOsFree(exMxPmRouteEntriesArray);

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpUcRouteEntriesRead
*
* DESCRIPTION:
*    Reads an array of uc route entries from the hw
*
* INPUTS:
*       devNum              - the device number
*       baseRouteEntryIndex - the index from which to start reading
*       routeEntriesArray   - the uc route entries array
*       numOfRouteEntries   - the number route entries in the array
*
* OUTPUTS:
*       routeEntriesArray - the uc route entries array read
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on devNum not active
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpUcRouteEntriesRead
(
    IN    GT_U8                          devNum,
    IN    GT_U32                         baseRouteEntryIndex,
    INOUT PRV_TGF_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray,
    IN    GT_U32                         numOfRouteEntries
)
{
    GT_STATUS   rc        = GT_OK;
    GT_U32      entryIter = 0;
#ifdef CHX_FAMILY
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC *dxChRouteEntriesArray;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC *exMxPmRouteEntriesArray;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* allocate memory */
    dxChRouteEntriesArray = cpssOsMalloc(numOfRouteEntries * sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));

    /* reset variables */
    cpssOsMemSet((GT_VOID*) dxChRouteEntriesArray, 0,
                 numOfRouteEntries * sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));

    /* convert IP UC route entry into device specific format */
    for (entryIter = 0; entryIter < numOfRouteEntries; entryIter++)
    {
        dxChRouteEntriesArray[entryIter].type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;

        /* convert IP UC route entry */
        rc = prvTgfConvertGenericToDxChIpUcRouteEntry(&routeEntriesArray[entryIter],
                                                 &(dxChRouteEntriesArray[entryIter].entry.regularEntry));

        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIpUcRouteEntry FAILED, rc = [%d]", rc);

            /* free allocated memory */
            cpssOsFree(dxChRouteEntriesArray);

            return rc;
        }
    }

    /* call device specific API */
    rc = cpssDxChIpUcRouteEntriesRead(devNum, baseRouteEntryIndex, dxChRouteEntriesArray, numOfRouteEntries);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpUcRouteEntriesRead FAILED, rc = [%d]", rc);

        /* free allocated memory */
        cpssOsFree(dxChRouteEntriesArray);

        return rc;
    }

    /* convert IP UC route entry from device specific format */
    for (entryIter = 0; entryIter < numOfRouteEntries; entryIter++)
    {
        /* convert IP UC route entry */
        prvTgfConvertDxChToGenericIpUcRouteEntry(&(dxChRouteEntriesArray[entryIter].entry.regularEntry),
                                                 &routeEntriesArray[entryIter]);
    }

    /* free allocated memory */
    cpssOsFree(dxChRouteEntriesArray);

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* allocate memory */
    exMxPmRouteEntriesArray = cpssOsMalloc(numOfRouteEntries * sizeof(CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC));

    /* reset variables */
    cpssOsMemSet((GT_VOID*) exMxPmRouteEntriesArray, 0,
                 numOfRouteEntries * sizeof(CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC));

    /* convert IP UC route entry from device specific format */
    for (entryIter = 0; entryIter < numOfRouteEntries; entryIter++)
    {
        exMxPmRouteEntriesArray[entryIter].type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        /* convert IP UC route entry */
        rc = prvTgfConvertGenericToExMxPmIpUcRouteEntry(&routeEntriesArray[entryIter],
                                                        &(exMxPmRouteEntriesArray[entryIter].entry.regularEntry));
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIpUcRouteEntry FAILED, rc = [%d]", rc);

            /* free allocated memory */
            cpssOsFree(exMxPmRouteEntriesArray);

            return rc;
        }
    }

    /* call device specific API */
    rc = cpssExMxPmIpUcRouteEntriesRead(devNum, baseRouteEntryIndex, numOfRouteEntries, exMxPmRouteEntriesArray);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmIpUcRouteEntriesRead FAILED, rc = [%d]", rc);

        /* free allocated memory */
        cpssOsFree(exMxPmRouteEntriesArray);

        return rc;
    }

    /* convert IP UC route entry from device specific format */
    for (entryIter = 0; entryIter < numOfRouteEntries; entryIter++)
    {
        /* convert IP UC route entry */
        rc = prvTgfConvertExMxPmToGenericIpUcRouteEntry(&(exMxPmRouteEntriesArray[entryIter].entry.regularEntry),
                                                        &routeEntriesArray[entryIter]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertExMxPmToGenericIpUcRouteEntry FAILED, rc = [%d]", rc);

            /* free allocated memory */
            cpssOsFree(exMxPmRouteEntriesArray);

            return rc;
        }
    }

    /* free allocated memory */
    cpssOsFree(exMxPmRouteEntriesArray);

    return rc;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpRouterArpAddrWrite
*
* DESCRIPTION:
*    Write a ARP MAC address to the router ARP / Tunnel start Table
*
* INPUTS:
*       devNum         - the device number
*       routerArpIndex - The Arp Address index
*       arpMacAddrPtr  - the ARP MAC address to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on devNum not active.
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED - request is not supported for this device type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpRouterArpAddrWrite
(
    IN GT_U32                         routerArpIndex,
    IN GT_ETHERADDR                  *arpMacAddrPtr
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;


#ifdef CHX_FAMILY
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChIpRouterArpAddrWrite(devNum, routerArpIndex, arpMacAddrPtr);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpRouterArpAddrWrite FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmIpRouterArpAddrWrite(devNum, routerArpIndex, arpMacAddrPtr);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmIpRouterArpAddrWrite FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpRouterArpAddrRead
*
* DESCRIPTION:
*    Read a ARP MAC address from the router ARP / Tunnel start Table
*
* INPUTS:
*       devNum         - the device number
*       routerArpIndex - The Arp Address index
*
* OUTPUTS:
*       arpMacAddrPtr - the ARP MAC address to read
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on devNum not active
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED - request is not supported for this device type
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpRouterArpAddrRead
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        routerArpIndex,
    OUT GT_ETHERADDR                 *arpMacAddrPtr
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpRouterArpAddrRead(devNum, routerArpIndex, arpMacAddrPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmIpRouterArpAddrRead(devNum, routerArpIndex, arpMacAddrPtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv4UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv4 prefix
*
* INPUTS:
*       lpmDBId        - The LPM DB id
*       vrId           - The virtual router id
*       ipAddr         - The destination IP address of this prefix
*       prefixLen      - The number of bits that are actual valid in the ipAddr
*       nextHopInfoPtr - the route entry info accosiated with this UC prefix
*       override       - override an existing entry for this mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_OUT_OF_RANGE   - on prefix length is too big
*       GT_ERROR          - on the vrId was not created yet
*       GT_OUT_OF_CPU_MEM - on failed to allocate CPU memory
*       GT_OUT_OF_PP_MEM  - on failed to allocate TCAM memory
*       GT_FAIL           - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4UcPrefixAdd
(
    IN GT_U32                                lpmDBId,
    IN GT_U32                                vrId,
    IN GT_IPADDR                             ipAddr,
    IN GT_U32                                prefixLen,
    IN PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *nextHopInfoPtr,
    IN GT_BOOL                               override
)
{
    GT_STATUS   rc = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT dxChNextHopInfo;
    PRV_TGF_IP_ROUTING_MODE_ENT            routingMode;
    PRV_TGF_PCL_ACTION_STC                *pclIpUcActionPtr;
    PRV_TGF_IP_LTT_ENTRY_STC              *ipLttEntryPtr;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC exMxPmNextHopInfo;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    rc = prvTgfIpRoutingModeGet(&routingMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfIpRoutingModeGet FAILED, rc = [%d]\n", rc);

        return rc;
    }

    switch(routingMode)
    {
        case PRV_TGF_IP_ROUTING_MODE_PCL_ACTION_E:

            pclIpUcActionPtr = &nextHopInfoPtr->pclIpUcAction;

            rc = prvTgfConvertGenericToDxChRuleAction(pclIpUcActionPtr, &dxChNextHopInfo.pclIpUcAction);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChRuleAction FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E:

            ipLttEntryPtr = &nextHopInfoPtr->ipLttEntry;

            /* set route type */
            rc = prvTgfConvertGenericToDxChRouteType(ipLttEntryPtr->routeType, &(dxChNextHopInfo.ipLttEntry.routeType));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChRouteType FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert ltt entry into device specific format */
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChNextHopInfo.ipLttEntry), ipLttEntryPtr, numOfPaths);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChNextHopInfo.ipLttEntry), ipLttEntryPtr, routeEntryBaseIndex);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChNextHopInfo.ipLttEntry), ipLttEntryPtr, ucRPFCheckEnable);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChNextHopInfo.ipLttEntry), ipLttEntryPtr, sipSaCheckMismatchEnable);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChNextHopInfo.ipLttEntry), ipLttEntryPtr, ipv6MCGroupScopeLevel);

            break;

        default:

            return GT_BAD_PARAM;
    }

    /* call device specific API */
    rc = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &dxChNextHopInfo, override);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
#if 0
    ipLttEntryPtr = &nextHopInfoPtr->ipLttEntry;

    /* set route type */
    rc = prvTgfConvertGenericToExMxPmRouteType(ipLttEntryPtr->routeType, &(exMxPmNextHopInfo.routeEntryMethod));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmRouteType FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert tti action into device specific format */
    exMxPmNextHopInfo.routeEntryBaseMemAddr = ipLttEntryPtr->routeEntryBaseIndex;

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmNextHopInfo), ipLttEntryPtr, blockSize);
#endif
    /* avoid warnings */
    TGF_PARAM_NOT_USED(nextHopInfoPtr);

    /* call device specific API */
    rc = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen, &exMxPmNextHopInfo, override);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#else
    return rc;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv4UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv4 prefix in a Virtual Router for the specified LPM DB
*
* INPUTS:
*       lpmDBId   - The LPM DB id
*       vrId      - The virtual router id
*       ipAddr    - The destination IP address of the prefix
*       prefixLen - The number of bits that are actual valid in the ipAddr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - prefix length is too big
*       GT_ERROR        - vrId was not created yet
*       GT_NO_SUCH      - given prefix doesn't exitst in the VR
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4UcPrefixDel
(
    IN GT_U32                         lpmDBId,
    IN GT_U32                         vrId,
    IN GT_IPADDR                      ipAddr,
    IN GT_U32                         prefixLen
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpLpmIpv4UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmIpLpmIpv4UcPrefixDelete(lpmDBId, vrId, ipAddr, prefixLen);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv4UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv4 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* INPUTS:
*       lpmDBId - The LPM DB id
*       vrId    - The virtual router identifier
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId, vrId);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDBId, vrId);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv6UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv6 prefix
*
* INPUTS:
*       lpmDBId        - The LPM DB id
*       vrId           - The virtual router id
*       ipAddr         - The destination IP address of this prefix
*       prefixLen      - The number of bits that are actual valid in the ipAddr
*       nextHopInfoPtr - the route entry info accosiated with this UC prefix
*       override       - override an existing entry for this mask
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If prefix length is too big.
*       GT_ERROR                 - If the vrId was not created yet.
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv6UcPrefixAdd
(
    IN GT_U32                                lpmDBId,
    IN GT_U32                                vrId,
    IN GT_IPV6ADDR                           ipAddr,
    IN GT_U32                                prefixLen,
    IN PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *nextHopInfoPtr,
    IN GT_BOOL                               override,
    IN GT_BOOL                               defragmentationEnable
)
{
    GT_STATUS   rc = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT dxChNextHopInfo;
    PRV_TGF_IP_ROUTING_MODE_ENT            routingMode;
    PRV_TGF_PCL_ACTION_STC                *pclIpUcActionPtr;
    PRV_TGF_IP_LTT_ENTRY_STC              *ipLttEntryPtr;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC exMxPmNextHopInfo;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    rc = prvTgfIpRoutingModeGet(&routingMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfIpRoutingModeGet FAILED, rc = [%d]\n", rc);

        return rc;
    }

    switch(routingMode)
    {
        case PRV_TGF_IP_ROUTING_MODE_PCL_ACTION_E:

            pclIpUcActionPtr = &nextHopInfoPtr->pclIpUcAction;

            rc = prvTgfConvertGenericToDxChRuleAction(pclIpUcActionPtr, &dxChNextHopInfo.pclIpUcAction);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChRuleAction FAILED, rc = [%d]", rc);

                return rc;
            }

            break;

        case PRV_TGF_IP_ROUTING_MODE_IP_LTT_ENTRY_E:

            ipLttEntryPtr = &nextHopInfoPtr->ipLttEntry;

            /* convert routeType into device specific format */
            rc = prvTgfConvertGenericToDxChRouteType(ipLttEntryPtr->routeType, &(dxChNextHopInfo.ipLttEntry.routeType));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChRouteType FAILED, rc = [%d]", rc);

                return rc;
            }

            /* convert ltt entry into device specific format */
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChNextHopInfo.ipLttEntry), ipLttEntryPtr, numOfPaths);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChNextHopInfo.ipLttEntry), ipLttEntryPtr, routeEntryBaseIndex);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChNextHopInfo.ipLttEntry), ipLttEntryPtr, ucRPFCheckEnable);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChNextHopInfo.ipLttEntry), ipLttEntryPtr, sipSaCheckMismatchEnable);
            PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChNextHopInfo.ipLttEntry), ipLttEntryPtr, ipv6MCGroupScopeLevel);

            break;

        default:

            return GT_BAD_PARAM;
    }

    /* call device specific API */
    rc = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId,
                                        vrId,
                                        ipAddr,
                                        prefixLen,
                                        &dxChNextHopInfo,
                                        override,
                                        defragmentationEnable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
#if 0
    ipLttEntryPtr = &nextHopInfoPtr->ipLttEntry;

    /* convert routeType into device specific format */
    rc = prvTgfConvertGenericToExMxPmRouteType(ipLttEntryPtr->routeType, &(exMxPmNextHopInfo.routeEntryMethod));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmRouteType FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert tti action into device specific format */
    exMxPmNextHopInfo.routeEntryBaseMemAddr = ipLttEntryPtr->routeEntryBaseIndex;

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmNextHopInfo), ipLttEntryPtr, blockSize);
#endif
    /* avoid warnings */
    TGF_PARAM_NOT_USED(nextHopInfoPtr);
    TGF_PARAM_NOT_USED(defragmentationEnable);

    /* call device specific API */
    rc = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, &ipAddr, prefixLen, &exMxPmNextHopInfo, override);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#else
    return rc;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv6UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv6 prefix in a Virtual Router for the specified LPM DB
*
* INPUTS:
*       lpmDBId   - The LPM DB id
*       vrId      - The virtual router id
*       ipAddr    - The destination IP address of the prefix
*       prefixLen - The number of bits that are actual valid in the ipAddr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - prefix length is too big
*       GT_ERROR                 - vrId was not created yet
*       GT_NO_SUCH               - given prefix doesn't exitst in the VR
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv6UcPrefixDel
(
    IN GT_U32                         lpmDBId,
    IN GT_U32                         vrId,
    IN GT_IPV6ADDR                    ipAddr,
    IN GT_U32                         prefixLen
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpLpmIpv6UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmIpLpmIpv6UcPrefixDelete(lpmDBId, vrId, &ipAddr, prefixLen);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv6UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv6 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* INPUTS:
*       lpmDBId - The LPM DB id
*       vrId    - The virtual router identifier
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv6UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId, vrId);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDBId, vrId);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCountersIpSet
*
* DESCRIPTION:
*       Set route entry mode and reset IP couters
*
* INPUTS:
*       devNum       - device number
*       portNum      - port number
*       counterIndex - counter index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong parameter
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCountersIpSet
(
    IN GT_U8                          portNum,
    IN GT_U32                         counterIndex
)
{
    GT_STATUS   rc       = GT_OK;
    GT_U32      portIter = 0;
#ifdef CHX_FAMILY
    CPSS_DXCH_IP_CNT_SET_MODE_ENT              cntSetMode       = CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E;
    CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC interfaceModeCfg = {0};
    CPSS_DXCH_IP_COUNTER_SET_STC               ipCounters       = {0};
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_IP_COUNTER_BIND_MODE_ENT        bindMode = CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E;
    CPSS_EXMXPM_IP_COUNTER_SET_CFG_STC          exMxPmInterfaceModeCfg = {0};
    CPSS_EXMXPM_IP_COUNTER_SET_STC              ipCounters = {0};
#endif /* EXMXPM_FAMILY */


    /* find port index */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortsArray[portIter] == portNum)
        {
            break;
        }
    }

    /* check if port is valid */
    if (prvTgfPortsArray[portIter] != portNum)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", portNum);

        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* set counter mode */
    cntSetMode = CPSS_DXCH_IP_CNT_SET_ROUTE_ENTRY_MODE_E;

    /* set ROUTE_ENTRY mode for IP counters */
    interfaceModeCfg.portTrunkCntMode = CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E;
    interfaceModeCfg.ipMode           = CPSS_IP_PROTOCOL_IPV4_E;
    interfaceModeCfg.vlanMode         = CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E;
    interfaceModeCfg.devNum           = prvTgfDevsArray[portIter];
    interfaceModeCfg.portTrunk.port   = portNum;
    interfaceModeCfg.portTrunk.trunk  = 0;
    interfaceModeCfg.vlanId           = 0;

    rc = cpssDxChIpCntSetModeSet(prvTgfDevsArray[portIter], (CPSS_IP_CNT_SET_ENT) counterIndex,
                                 cntSetMode, &interfaceModeCfg);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpCntSetModeSet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* reset IP couters */
    rc = cpssDxChIpCntSet(prvTgfDevsArray[portIter], (CPSS_IP_CNT_SET_ENT) counterIndex, &ipCounters);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpCntSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* set counter bind mode */
    bindMode = CPSS_EXMXPM_IP_COUNTER_BIND_TO_ROUTE_ENTRY_E;

    /* set ROUTE_ENTRY mode for IP counters */
    exMxPmInterfaceModeCfg.portTrunkMode      = CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E;
    exMxPmInterfaceModeCfg.portTrunk.devPort.devNum = prvTgfDevsArray[portIter];
    exMxPmInterfaceModeCfg.portTrunk.devPort.port   = portNum;
    exMxPmInterfaceModeCfg.portTrunk.trunk    = 0;
    exMxPmInterfaceModeCfg.packetType         = CPSS_EXMXPM_IP_PACKET_TYPE_IPV4_COUNTER_MODE_E;
    exMxPmInterfaceModeCfg.vlanMode           = CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E;
    exMxPmInterfaceModeCfg.vlanId             = 0;

    rc = cpssExMxPmIpCounterConfigSet(prvTgfDevsArray[portIter], counterIndex, bindMode, &exMxPmInterfaceModeCfg);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmIpCounterConfigSet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* reset IP couters */
    rc = cpssExMxPmIpCounterSet(prvTgfDevsArray[portIter], counterIndex, &ipCounters);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmIpCounterSet FAILED, rc = [%d]", rc);

        return rc;
    }

    return rc;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfCountersIpGet
*
* DESCRIPTION:
*       This function gets the values of the various counters in the IP
*       Router Management Counter-Set.
*
* INPUTS:
*       devNum       - device number
*       counterIndex - counter index
*       enablePrint  - Enable/Disable output log
*
* OUTPUTS:
*       ipCountersPtr - (pointer to) IP counters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameter
*       GT_FAIL      - on error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCountersIpGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        counterIndex,
    IN  GT_BOOL                       enablePrint,
    OUT PRV_TGF_IP_COUNTER_SET_STC   *ipCountersPtr
)
{
    GT_STATUS   rc       = GT_OK;
    GT_BOOL     isZero   = GT_TRUE;
    GT_U32      cntIter  = 0;
    GT_U32      cntNext  = 0;
    GT_U32      cntCount = 0;
#ifdef CHX_FAMILY
    CPSS_DXCH_IP_COUNTER_SET_STC dxChIpCounters     = {0};
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_IP_COUNTER_SET_STC exMxPmIpCounters = {0};
#endif /* EXMXPM_FAMILY */
    GT_U8       ipCountersName[][PRV_TGF_MAX_FIELD_NAME_LEN_CNS] =
        {"inUcPkts", "inMcPkts", "inUcNonRoutedExcpPkts", "inUcNonRoutedNonExcpPkts",
         "inMcNonRoutedExcpPkts", "inMcNonRoutedNonExcpPkts", "inUcTrappedMirrorPkts",
         "inMcTrappedMirrorPkts", "mcRfpFailPkts", "outUcRoutedPkts"};


    /* get counter count */
    cntCount = sizeof(ipCountersName) / sizeof(ipCountersName[0]);

#ifdef CHX_FAMILY
    /* get counters */
    rc = cpssDxChIpCntGet(devNum, (CPSS_IP_CNT_SET_ENT) counterIndex, &dxChIpCounters);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpCntGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert counters from cpss */
    cpssOsMemCpy(ipCountersPtr, &dxChIpCounters, sizeof(dxChIpCounters));
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* get counters */
    rc = cpssExMxPmIpCounterGet(devNum, counterIndex, &exMxPmIpCounters);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmIpCounterGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert counters from cpss */
    cpssOsMemCpy(ipCountersPtr, &exMxPmIpCounters, sizeof(exMxPmIpCounters));
#endif /* EXMXPM_FAMILY */

    if (GT_TRUE == enablePrint)
    {
        /* print all not zero IP counters values */
        isZero = GT_TRUE;
        for (cntIter = 0; cntIter < cntCount; cntIter++)
        {
            /* next ip counter from CPSS_DXCH_IP_COUNTER_SET_STC */
            cntNext = *(cntIter + (GT_U32*) ipCountersPtr);

            if (cntNext != 0)
            {
                isZero = GT_FALSE;
                PRV_UTF_LOG2_MAC("  %s = %d\n", &ipCountersName[cntIter], cntNext);
            }
        }

        if (GT_TRUE == isZero)
        {
            PRV_UTF_LOG0_MAC("  All IP Counters are Zero\n");
        }

        PRV_UTF_LOG0_MAC("\n");
    }

    return rc;
}

/*******************************************************************************
* prvTgfIpPortRoutingEnable
*
* DESCRIPTION:
*       Enable multicast/unicast IPv4/v6 routing on a port
*
* INPUTS:
*       devNum     - the device number
*       portNum    - the port to enable on
*       ucMcEnable - routing type to enable Unicast/Multicast
*       protocol   - what type of traffic to enable ipv4 or ipv6 or both
*       enable     - enable IP routing for this port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfIpPortRoutingEnable
(
    IN GT_U8                          portNum,
    IN CPSS_IP_UNICAST_MULTICAST_ENT  ucMcEnable,
    IN CPSS_IP_PROTOCOL_STACK_ENT     protocol,
    IN GT_BOOL                        enable
)
{
    GT_U32  portIter = 0;
#ifdef EXMXPM_FAMILY
    GT_STATUS   rc = GT_OK;
#endif /* CHX_FAMILY */


    /* find port index */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortsArray[portIter] == portNum)
        {
            break;
        }
    }

    /* check if port is valid */
    if (prvTgfPortsArray[portIter] != portNum)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", portNum);

        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpPortRoutingEnable(prvTgfDevsArray[portIter], portNum,
                                       ucMcEnable, protocol, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* configured the InLIf Lookup mode */
    rc = cpssExMxPmInlifPortModeSet(prvTgfDevsArray[portIter], portNum,
                                    CPSS_EXMXPM_INLIF_PORT_MODE_PORT_E);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmInlifPortModeSet FAILED, rc = [%d]", rc);

        return rc;
    }

    switch (ucMcEnable)
    {
        case CPSS_IP_UNICAST_E:
            /* call device specific API */
            return cpssExMxPmInlifIpUcRouteEnableSet(prvTgfDevsArray[portIter],
                                                     CPSS_EXMXPM_INLIF_TYPE_PORT_E,
                                                     portNum, protocol, enable);
        case CPSS_IP_MULTICAST_E:
            /* call device specific API */
            return cpssExMxPmInlifIpMcRouteEnableSet(prvTgfDevsArray[portIter],
                                                     CPSS_EXMXPM_INLIF_TYPE_PORT_E,
                                                     portNum, protocol, enable);
        default:
            return GT_BAD_PARAM;
    }

#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpVlanRoutingEnable
*
* DESCRIPTION:
*       Enable/Disable IPv4/Ipv6 multicast/unicast Routing on Vlan
*
* INPUTS:
*       devNum     - device number
*       vlanId     - Vlan ID
*       ucMcEnable - routing type to enable Unicast/Multicast
*       protocol   - ipv4 or ipv6
*       enable     - enable\disable ip unicast routing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfIpVlanRoutingEnable
(
    IN GT_U16                         vlanId,
    IN CPSS_IP_UNICAST_MULTICAST_ENT  ucMcEnable,
    IN CPSS_IP_PROTOCOL_STACK_ENT     protocol,
    IN GT_BOOL                        enable
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;


#ifdef CHX_FAMILY
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        switch (ucMcEnable)
        {
            case CPSS_IP_UNICAST_E:
                /* call device specific API */
                rc = cpssDxChBrgVlanIpUcRouteEnable(devNum, vlanId, protocol, enable);
                if (GT_OK != rc)
                {
                    PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanIpUcRouteEnable FAILED, rc = [%d]", rc);

                    rc1 = rc;
                }

                break;

            case CPSS_IP_MULTICAST_E:
                /* call device specific API */
                rc = cpssDxChBrgVlanIpMcRouteEnable(devNum, vlanId, protocol, enable);
                if (GT_OK != rc)
                {
                    PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanIpMcRouteEnable FAILED, rc = [%d]", rc);

                    rc1 = rc;
                }

                break;

            default:
                rc1 = GT_BAD_PARAM;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        switch (ucMcEnable)
        {
            case CPSS_IP_UNICAST_E:
                /* call device specific API */
                rc = cpssExMxPmInlifIpUcRouteEnableSet(devNum, CPSS_EXMXPM_INLIF_TYPE_VLAN_E, vlanId, protocol, enable);
                if (GT_OK != rc)
                {
                    PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmInlifIpUcRouteEnableSet FAILED, rc = [%d]", rc);

                    rc1 = rc;
                }

                break;

            case CPSS_IP_MULTICAST_E:
                /* call device specific API */
                rc = cpssExMxPmInlifIpMcRouteEnableSet(devNum, CPSS_EXMXPM_INLIF_TYPE_VLAN_E, vlanId, protocol, enable);
                if (GT_OK != rc)
                {
                    PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmInlifIpMcRouteEnableSet FAILED, rc = [%d]", rc);

                    rc1 = rc;
                }

                break;

            default:
                rc1 = GT_BAD_PARAM;
        }
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfInlifEntrySet
*
* DESCRIPTION:
*       Set Inlif Entry
*
* INPUTS:
*       devNum        - device number
*       inlifType     - inlif type
*       inlifIndex    - inlif index
*       inlifEntryPtr - (pointer to) the Inlif Fields
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong input parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_FAIL      - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfInlifEntrySet
(
    IN PRV_TGF_INLIF_TYPE_ENT         inlifType,
    IN GT_U32                         inlifIndex,
    IN PRV_TGF_INLIF_ENTRY_STC       *inlifEntryPtr

)
{
#ifdef CHX_FAMILY
    TGF_PARAM_NOT_USED(inlifType);
    TGF_PARAM_NOT_USED(inlifIndex);
    TGF_PARAM_NOT_USED(inlifEntryPtr);

    return GT_OK;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    GT_U8                           devNum           = 0;
    GT_STATUS                       rc, rc1          = GT_OK;
    CPSS_EXMXPM_INLIF_TYPE_ENT      exMxPmInlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
    CPSS_EXMXPM_INLIF_ENTRY_STC     exMxPmInlifEntry = {0};


    /* convert Inlif type into device specific format */
    switch (inlifType)
    {
        case PRV_TGF_INLIF_TYPE_PORT_E:
            exMxPmInlifType  = CPSS_EXMXPM_INLIF_TYPE_PORT_E;
            break;

        case PRV_TGF_INLIF_TYPE_VLAN_E:
            exMxPmInlifType  = CPSS_EXMXPM_INLIF_TYPE_VLAN_E;
            break;

        case PRV_TGF_INLIF_TYPE_EXTERNAL_E:
            exMxPmInlifType  = CPSS_EXMXPM_INLIF_TYPE_EXTERNAL_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert Inlif entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, bridgeEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, autoLearnEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, naMessageToCpuEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, naStormPreventionEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, unkSaUcCommand);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, unkDaUcCommand);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, unkSaNotSecurBreachEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, untaggedMruIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, unregNonIpMcCommand);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, unregIpMcCommand);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, unregIpv4BcCommand);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, unregNonIpv4BcCommand);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, ipv4UcRouteEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, ipv4McRouteEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, ipv6UcRouteEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, ipv6McRouteEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, mplsRouteEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, vrfId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, ipv4IcmpRedirectEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, ipv6IcmpRedirectEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, bridgeRouterInterfaceEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, ipSecurityProfile);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, ipv4IgmpToCpuEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, ipv6IcmpToCpuEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, udpBcRelayEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, arpBcToCpuEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, arpBcToMeEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, ripv1MirrorEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, ipv4LinkLocalMcCommandEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, ipv6LinkLocalMcCommandEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, ipv6NeighborSolicitationEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, mirrorToAnalyzerEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmInlifEntry, inlifEntryPtr, mirrorToCpuEnable);

    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmInlifEntrySet(devNum, exMxPmInlifType, inlifIndex, &exMxPmInlifEntry);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmInlifEntrySet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*       This function adds a virtual router in system for specific LPM DB
*
* INPUTS:
*       lpmDBId                   - LPM DB id
*       vrId                      - virtual's router ID
*       defIpv4UcNextHopInfoPtr   - (pointer to) ipv4 uc next hop info
*       defIpv6UcNextHopInfoPtr   - (pointer to) ipv6 uc next hop info
*       defIpv4McRouteLttEntryPtr - (pointer to) ipv4 mc LTT entry info
*       defIpv6McRouteLttEntryPtr - (pointer to) ipv6 mc LTT entry info
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success on success
*       GT_NOT_FOUND      - on the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM - on failed to allocate CPU memory
*       GT_OUT_OF_PP_MEM  - on failed to allocate TCAM memory.
*       GT_BAD_STATE      - on the existing VR is not empty.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmVirtualRouterAdd
(
    IN GT_U32                                lpmDbId,
    IN GT_U32                                vrId,
    IN PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *defIpv4UcNextHopInfoPtr,
    IN PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *defIpv6UcNextHopInfoPtr,
    IN PRV_TGF_IP_LTT_ENTRY_STC             *defIpv4McRouteLttEntryPtr,
    IN PRV_TGF_IP_LTT_ENTRY_STC             *defIpv6McRouteLttEntryPtr
)
{
    GT_STATUS   rc = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC         vrConfigInfo = {0};
    PRV_TGF_IP_ROUTING_MODE_ENT            routingMode;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC    exMxPmVrConfig = {0};
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* get routing mode */
    rc =  prvTgfIpRoutingModeGet(&routingMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfIpRoutingModeGet FAILED, rc = [%d]", rc);
        return rc;
    }

    if (NULL != defIpv4UcNextHopInfoPtr)
    {
        rc = prvTgfConvertGenericToDxChUcNextHopInfo(routingMode,
                                                     defIpv4UcNextHopInfoPtr,
                                                     &vrConfigInfo.defIpv4UcNextHopInfo);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChUcNextHopInfo FAILED, rc = [%d]", rc);
            return rc;
        }
        vrConfigInfo.supportIpv4Uc = GT_TRUE;
    }
    if (NULL != defIpv6UcNextHopInfoPtr)
    {
        rc = prvTgfConvertGenericToDxChUcNextHopInfo(routingMode,
                                                     defIpv6UcNextHopInfoPtr,
                                                     &vrConfigInfo.defIpv6UcNextHopInfo);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChUcNextHopInfo FAILED, rc = [%d]", rc);
            return rc;
        }
        vrConfigInfo.supportIpv6Uc = GT_TRUE;
    }

    if (NULL != defIpv4McRouteLttEntryPtr)
    {
       rc = prvTgfConvertGenericToDxChMcLttEntryInfo(defIpv4McRouteLttEntryPtr,
                                                 &vrConfigInfo.defIpv4McRouteLttEntry);
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMcLttEntryInfo FAILED, rc = [%d]", rc);

           return rc;
       }
       vrConfigInfo.supportIpv4Mc = GT_TRUE;
    }
    if (NULL != defIpv6McRouteLttEntryPtr)
    {
       rc = prvTgfConvertGenericToDxChMcLttEntryInfo(defIpv6McRouteLttEntryPtr,
                                                     &vrConfigInfo.defIpv6McRouteLttEntry);
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMcLttEntryInfo FAILED, rc = [%d]", rc);

           return rc;
       }
       vrConfigInfo.supportIpv6Mc = GT_TRUE;
    }

    /* call device specific API */
    return cpssDxChIpLpmVirtualRouterAdd(lpmDbId, vrId,&vrConfigInfo);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* clear entry */
    cpssOsMemSet((GT_VOID*) &exMxPmVrConfig, 0, sizeof(exMxPmVrConfig));
    if (NULL != defIpv4UcNextHopInfoPtr)
    {
        exMxPmVrConfig.supportUcIpv4 = GT_TRUE;
        /* set IPv4 UC entry */
        exMxPmVrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr =
            defIpv4UcNextHopInfoPtr->ipLttEntry.routeEntryBaseIndex;
        exMxPmVrConfig.defaultUcIpv4RouteEntry.blockSize =
            defIpv4UcNextHopInfoPtr->ipLttEntry.numOfPaths;

        /* set route type */
        rc = prvTgfConvertGenericToExMxPmRouteType(defIpv4UcNextHopInfoPtr->ipLttEntry.routeType,
                                                   &(exMxPmVrConfig.defaultUcIpv4RouteEntry.routeEntryMethod));
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmRouteType FAILED, rc = [%d]", rc);
            return rc;
        }
    }
    if (NULL != defIpv6UcNextHopInfoPtr)
    {
        exMxPmVrConfig.supportUcIpv6 = GT_TRUE;

        /* set IPv6 UC entry */
        exMxPmVrConfig.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr =
            defIpv6UcNextHopInfoPtr->ipLttEntry.routeEntryBaseIndex;
        exMxPmVrConfig.defaultUcIpv6RouteEntry.blockSize =
            defIpv6UcNextHopInfoPtr->ipLttEntry.numOfPaths;

        /* set route type */
        rc = prvTgfConvertGenericToExMxPmRouteType(defIpv6UcNextHopInfoPtr->ipLttEntry.routeType,
                                                   &(exMxPmVrConfig.defaultUcIpv6RouteEntry.routeEntryMethod));
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmRouteType FAILED, rc = [%d]", rc);

            return rc;
        }
    }

    if (NULL !=defIpv4McRouteLttEntryPtr)
    {
        exMxPmVrConfig.supportMcIpv4 = GT_TRUE;

        /* set IPv4 MC entry */
        exMxPmVrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr =
            defIpv4McRouteLttEntryPtr->routeEntryBaseIndex;
        exMxPmVrConfig.defaultMcIpv4RouteEntry.blockSize =
            defIpv4McRouteLttEntryPtr->numOfPaths;

        /* set route type */
        rc = prvTgfConvertGenericToExMxPmRouteType(defIpv4McRouteLttEntryPtr->routeType,
                                                   &(exMxPmVrConfig.defaultMcIpv4RouteEntry.routeEntryMethod));
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmRouteType FAILED, rc = [%d]", rc);
            return rc;
        }
    }

    if (NULL != defIpv6McRouteLttEntryPtr)
    {
        exMxPmVrConfig.supportMcIpv6 = GT_TRUE;

        /* set IPv6 MC entry */
        exMxPmVrConfig.defaultMcIpv6RouteEntry.routeEntryBaseMemAddr =
            defIpv6McRouteLttEntryPtr->routeEntryBaseIndex;
        exMxPmVrConfig.defaultMcIpv6RouteEntry.blockSize =
            defIpv6McRouteLttEntryPtr->numOfPaths;

        /* set route type */
        rc = prvTgfConvertGenericToExMxPmRouteType(defIpv6McRouteLttEntryPtr->routeType,
                                                   &(exMxPmVrConfig.defaultMcIpv6RouteEntry.routeEntryMethod));
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmRouteType FAILED, rc = [%d]", rc);
            return rc;
        }
    }

    /* call device specific API */
    return cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &exMxPmVrConfig);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmVirtualRouterSharedAdd
*
* DESCRIPTION:
*       This function adds shared virtual router in system for specific LPM DB
*
* INPUTS:
*       lpmDBId                   - LPM DB id
*       vrId                      - virtual's router ID
*       defIpv4UcNextHopInfoPtr   - the route Ipv4 uc entry info
*       defIpv6UcNextHopInfoPtr   - the route Ipv6 uc entry info
*       defIpv4McRouteLttEntryPtr - the ipv4 mc LTT entry
*       defIpv6McRouteLttEntryPtr - the ipv6 mc LTT entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success on success
*       GT_NOT_FOUND      - on the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM - on failed to allocate CPU memory
*       GT_OUT_OF_PP_MEM  - on failed to allocate TCAM memory.
*       GT_BAD_STATE      - on the existing VR is not empty.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmVirtualRouterSharedAdd
(
    IN GT_U32                                lpmDbId,
    IN GT_U32                                vrId,
    IN PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *defIpv4UcNextHopInfoPtr,
    IN PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *defIpv6UcNextHopInfoPtr,
    IN PRV_TGF_IP_LTT_ENTRY_STC             *defIpv4McRouteLttEntryPtr,
    IN PRV_TGF_IP_LTT_ENTRY_STC             *defIpv6McRouteLttEntryPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC         vrConfigInfo = {0};
    PRV_TGF_IP_ROUTING_MODE_ENT            routingMode;
#endif /* CHX_FAMILY */


#ifdef CHX_FAMILY

    /* get routing mode */
    rc =  prvTgfIpRoutingModeGet(&routingMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfIpRoutingModeGet FAILED, rc = [%d]", rc);
        return rc;
    }

    if (NULL != defIpv4UcNextHopInfoPtr)
    {
        rc = prvTgfConvertGenericToDxChUcNextHopInfo(routingMode,
                                                     defIpv4UcNextHopInfoPtr,
                                                     &vrConfigInfo.defIpv4UcNextHopInfo);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChUcNextHopInfo FAILED, rc = [%d]", rc);
            return rc;
        }
        vrConfigInfo.supportIpv4Uc = GT_TRUE;
    }
    if (NULL != defIpv6UcNextHopInfoPtr)
    {
        rc = prvTgfConvertGenericToDxChUcNextHopInfo(routingMode,
                                                     defIpv6UcNextHopInfoPtr,
                                                     &vrConfigInfo.defIpv6UcNextHopInfo);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChUcNextHopInfo FAILED, rc = [%d]", rc);
            return rc;
        }
        vrConfigInfo.supportIpv6Uc = GT_TRUE;
    }

    if (NULL != defIpv4McRouteLttEntryPtr)
    {
       rc = prvTgfConvertGenericToDxChMcLttEntryInfo(defIpv4McRouteLttEntryPtr,
                                                 &vrConfigInfo.defIpv4McRouteLttEntry);
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMcLttEntryInfo FAILED, rc = [%d]", rc);

           return rc;
       }
       vrConfigInfo.supportIpv4Mc = GT_TRUE;
    }
    if (NULL != defIpv6McRouteLttEntryPtr)
    {
       rc = prvTgfConvertGenericToDxChMcLttEntryInfo(defIpv6McRouteLttEntryPtr,
                                                     &vrConfigInfo.defIpv6McRouteLttEntry);
       if (GT_OK != rc)
       {
           PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMcLttEntryInfo FAILED, rc = [%d]", rc);

           return rc;
       }
       vrConfigInfo.supportIpv6Mc = GT_TRUE;
    }

    /* call device specific API */
    return cpssDxChIpLpmVirtualRouterSharedAdd(lpmDbId, vrId, &vrConfigInfo);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(lpmDbId);
    TGF_PARAM_NOT_USED(vrId);
    TGF_PARAM_NOT_USED(defIpv4UcNextHopInfoPtr);
    TGF_PARAM_NOT_USED(defIpv6UcNextHopInfoPtr);
    TGF_PARAM_NOT_USED(defIpv4McRouteLttEntryPtr);
    TGF_PARAM_NOT_USED(defIpv6McRouteLttEntryPtr);
    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmVirtualRouterDel
*
* DESCRIPTION:
*       This function removes a virtual router in system for a specific LPM DB
*
* INPUTS:
*       lpmDBId - LPM DB id
*       vrId    - virtual's router ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success on success
*       GT_NOT_FOUND - on the LPM DB id is not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmVirtualRouterDel
(
    IN GT_U32                         lpmDbId,
    IN GT_U32                         vrId
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpLpmVirtualRouterDel(lpmDbId, vrId);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmIpLpmVirtualRouterDelete(lpmDbId, vrId);
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLttWrite
*
* DESCRIPTION:
*    Writes a LookUp Translation Table Entry
*
* INPUTS:
*       devNum       - device number
*       lttTtiRow    - the entry's row index in LTT table
*       lttTtiColumn - the entry's column index in LTT table
*       lttEntryPtr  - (pointer to) the lookup translation table entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong input parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLttWrite
(
    IN GT_U32                         lttTtiRow,
    IN GT_U32                         lttTtiColumn,
    IN PRV_TGF_IP_LTT_ENTRY_STC      *lttEntryPtr
)
{
#ifdef CHX_FAMILY
    GT_U8                       devNum  = 0;
    GT_STATUS                   rc, rc1 = GT_OK;
    CPSS_DXCH_IP_LTT_ENTRY_STC  dxChLttEntry;


    /* set LTT entry */
    cpssOsMemCpy((GT_VOID*) &dxChLttEntry, lttEntryPtr, sizeof(dxChLttEntry));

    /* set route type */
    rc = prvTgfConvertGenericToDxChRouteType(lttEntryPtr->routeType, &(dxChLttEntry.routeType));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmRouteType FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChIpLttWrite(devNum, lttTtiRow, lttTtiColumn, &dxChLttEntry);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpLttWrite FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(lttTtiRow);
    TGF_PARAM_NOT_USED(lttTtiColumn);
    TGF_PARAM_NOT_USED(lttEntryPtr);

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpv4PrefixSet
*
* DESCRIPTION:
*    Sets an ipv4 UC or MC prefix to the Router Tcam
*
* INPUTS:
*       devNum              - device number
*       routerTtiTcamRow    - TCAM row to set
*       routerTtiTcamColumn - TCAM column to set
*       prefixPtr           - (pointer to) prefix to set
*       maskPtr             - (pointer to) mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on fail
*       GT_BAD_PARAM - on devNum not active
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpv4PrefixSet
(
    IN GT_U32                         routerTtiTcamRow,
    IN GT_U32                         routerTtiTcamColumn,
    IN PRV_TGF_IPV4_PREFIX_STC       *prefixPtr,
    IN PRV_TGF_IPV4_PREFIX_STC       *maskPtr
)
{
#ifdef CHX_FAMILY
    GT_U8                       devNum  = 0;
    GT_STATUS                   rc, rc1 = GT_OK;
    CPSS_DXCH_IPV4_PREFIX_STC   dxChPrefix;
    CPSS_DXCH_IPV4_PREFIX_STC   dxChMask;


    /* convert Ipv4 prefix entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChPrefix, prefixPtr, vrId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChPrefix, prefixPtr, ipAddr);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChPrefix, prefixPtr, isMcSource);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChPrefix, prefixPtr, mcGroupIndexRow);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChPrefix, prefixPtr, mcGroupIndexColumn);
    
    /* convert Ipv4 mask entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChMask, maskPtr, vrId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChMask, maskPtr, ipAddr);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChMask, maskPtr, isMcSource);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChMask, maskPtr, mcGroupIndexRow);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChMask, maskPtr, mcGroupIndexColumn);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChIpv4PrefixSet(devNum, routerTtiTcamRow, routerTtiTcamColumn,
                                   &dxChPrefix, &dxChMask);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpv4PrefixSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(routerTtiTcamRow);
    TGF_PARAM_NOT_USED(routerTtiTcamColumn);
    TGF_PARAM_NOT_USED(prefixPtr);
    TGF_PARAM_NOT_USED(maskPtr);

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpv6PrefixSet
*
* DESCRIPTION:
*    Set an ipv6 UC or MC prefix to the Router Tcam
*
* INPUTS:
*       devNum           - device number
*       routerTtiTcamRow - TCAM row to set
*       prefixPtr        - (pointer to) prefix to set
*       maskPtr          - (pointer to) mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on fail
*       GT_BAD_PARAM - on devNum not active
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpv6PrefixSet
(
    IN GT_U32                  routerTtiTcamRow,
    IN PRV_TGF_IPV6_PREFIX_STC *prefixPtr,
    IN PRV_TGF_IPV6_PREFIX_STC *maskPtr
)
{
#ifdef CHX_FAMILY
    GT_U8                     devNum  = 0;
    GT_STATUS                 rc, rc1 = GT_OK;
    CPSS_DXCH_IPV6_PREFIX_STC dxChPrefix;
    CPSS_DXCH_IPV6_PREFIX_STC dxChMask;


    /* convert Ipv6 prefix entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChPrefix, prefixPtr, vrId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChPrefix, prefixPtr, ipAddr);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChPrefix, prefixPtr, isMcSource);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChPrefix, prefixPtr, mcGroupIndexRow);
    
    /* convert Ipv6 mask entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChMask, maskPtr, vrId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChMask, maskPtr, ipAddr);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChMask, maskPtr, isMcSource);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChMask, maskPtr, mcGroupIndexRow);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChIpv6PrefixSet(devNum, routerTtiTcamRow,
                                   &dxChPrefix, &dxChMask);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpv6PrefixSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(routerTtiTcamRow);
    TGF_PARAM_NOT_USED(prefixPtr);
    TGF_PARAM_NOT_USED(maskPtr);

    return GT_NOT_IMPLEMENTED;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpv4PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv4 UC or MC prefix in the Router Tcam
*
* INPUTS:
*       devNum              - the device number
*       routerTtiTcamRow    - the TCAM row to invalidate
*       routerTtiTcamColumn - the TCAM column to invalidate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on devNum not active
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpv4PrefixInvalidate
(
    IN GT_U32                         routerTtiTcamRow,
    IN GT_U32                         routerTtiTcamColumn
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChIpv4PrefixInvalidate(devNum, routerTtiTcamRow, routerTtiTcamColumn);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpv4PrefixInvalidate FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(routerTtiTcamRow);
    TGF_PARAM_NOT_USED(routerTtiTcamColumn);

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpv6PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv6 UC or MC prefix in the Router Tcam
*
* INPUTS:
*       devNum           - the device number
*       routerTtiTcamRow - the TCAM row to invalidate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on devNum not active
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpv6PrefixInvalidate
(
    IN GT_U32 routerTtiTcamRow
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChIpv6PrefixInvalidate(devNum, routerTtiTcamRow);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpv6PrefixInvalidate FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(routerTtiTcamRow);

    return GT_NOT_IMPLEMENTED;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpMcRouteEntriesWrite
*
* DESCRIPTION:
*       Write an array of MC route entries to hw
*
* INPUTS:
*       devNum          - device number
*       ucMcBaseIndex   - base Index in the Route entries table
*       numOfEntries    - number of route entries to write
*       entriesArrayPtr - MC route entry array to write
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
GT_STATUS prvTgfIpMcRouteEntriesWrite
(
    IN GT_U32                         ucMcBaseIndex,
    IN GT_U32                         numOfEntries,
    IN PRV_TGF_IP_MC_ROUTE_ENTRY_STC *entriesArrayPtr
)
{
    GT_U8       devNum    = 0;
    GT_STATUS   rc, rc1   = GT_OK;
    GT_U32      entryIter = 0;
#ifdef CHX_FAMILY
    CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC *dxChRouteEntriesArray;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC *exMxPmRouteEntriesArray;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* allocate memory */
    dxChRouteEntriesArray = cpssOsMalloc(numOfEntries * sizeof(CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC));

    /* reset variables */
    cpssOsMemSet((GT_VOID*) dxChRouteEntriesArray, 0,
                 numOfEntries * sizeof(CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC));

    /* convert IP MC route entry into device specific format */
    for (entryIter = 0; entryIter < numOfEntries; entryIter++)
    {
        /* convert IP MC route entry */
        rc = prvTgfConvertGenericToDxChIpMcRouteEntry(&entriesArrayPtr[entryIter],
                                                      &(dxChRouteEntriesArray[entryIter]));
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChIpMcRouteEntry FAILED, rc = [%d]", rc);

            /* free allocated memory */
            cpssOsFree(dxChRouteEntriesArray);

            return rc;
        }
    }

    /* prepare device iterator */
    rc = prvUtfNextNotApplicableDeviceReset(&devNum, UTF_NONE_FAMILY_E);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextNotApplicableDeviceReset FAILED, rc = [%d]", rc);

        /* free allocated memory */
        cpssOsFree(dxChRouteEntriesArray);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        for (entryIter = 0; entryIter < numOfEntries; entryIter++)
        {
            /* call device specific API */
            rc = cpssDxChIpMcRouteEntriesWrite(devNum, ucMcBaseIndex + entryIter, &(dxChRouteEntriesArray[entryIter]));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpMcRouteEntriesWrite FAILED, rc = [%d]", rc);

                rc1 = rc;
            }
        }
    }

    /* free allocated memory */
    cpssOsFree(dxChRouteEntriesArray);

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* allocate memory */
    exMxPmRouteEntriesArray = cpssOsMalloc(numOfEntries * sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC));

    /* reset variables */
    cpssOsMemSet((GT_VOID*) exMxPmRouteEntriesArray, 0,
                 numOfEntries * sizeof(CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC));

    /* convert IP MC route entry into device specific format */
    for (entryIter = 0; entryIter < numOfEntries; entryIter++)
    {
        /* convert IP MC route entry */
        rc = prvTgfConvertGenericToExMxPmIpMcRouteEntry(&entriesArrayPtr[entryIter],
                                                        &(exMxPmRouteEntriesArray[entryIter]));
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmIpMcRouteEntry FAILED, rc = [%d]", rc);

            /* free allocated memory */
            cpssOsFree(exMxPmRouteEntriesArray);

            return rc;
        }
    }

    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        /* free allocated memory */
        cpssOsFree(exMxPmRouteEntriesArray);

        return rc;
    }

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmIpMcRouteEntriesWrite(devNum, ucMcBaseIndex, numOfEntries, exMxPmRouteEntriesArray);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmIpMcRouteEntriesWrite FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    /* free allocated memory */
    cpssOsFree(exMxPmRouteEntriesArray);

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpMllPairWrite
*
* DESCRIPTION:
*    Write a Mc Link List (MLL) pair entry to hw
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairWriteForm  - the way to write the Mll pair
*       mllPairEntryPtr   - (pointer to) the Mc lisk list pair entry
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on devNum not active or invalid mllPairWriteForm.
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpMllPairWrite
(
    IN GT_U32                                   mllPairEntryIndex,
    IN PRV_TGF_IP_MLL_PAIR_READ_WRITE_FORM_ENT  mllPairWriteForm,
    IN PRV_TGF_IP_MLL_PAIR_STC                 *mllPairEntryPtr
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_FORM_ENT   dxChMllPairWriteForm;
    CPSS_DXCH_IP_MLL_PAIR_STC                   dxChMllPairEntry;


    /* convert mllPairWriteForm into device specific format */
    switch (mllPairWriteForm)
    {
        case PRV_TGF_IP_MLL_PAIR_READ_WRITE_FIRST_MLL_ONLY_E:
            dxChMllPairWriteForm = CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_FIRST_MLL_ONLY_E;
            break;

        case PRV_TGF_IP_MLL_PAIR_READ_WRITE_SECOND_MLL_NEXT_POINTER_ONLY_E:
            dxChMllPairWriteForm = CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_SECOND_MLL_NEXT_POINTER_ONLY_E;
            break;

        case PRV_TGF_IP_MLL_PAIR_READ_WRITE_WHOLE_E:
            dxChMllPairWriteForm = CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_WHOLE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* copy entry */
    cpssOsMemCpy((GT_VOID*) &dxChMllPairEntry,
                 (GT_VOID*) mllPairEntryPtr,
                 sizeof(dxChMllPairEntry));

    if(mllPairWriteForm == CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_WHOLE_E ||
        mllPairWriteForm == CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_FIRST_MLL_ONLY_E)
    {
        if( CPSS_INTERFACE_PORT_E == mllPairEntryPtr->firstMllNode.nextHopInterface.type )
        {
            rc = prvUtfHwFromSwDeviceNumberGet(mllPairEntryPtr->firstMllNode.nextHopInterface.devPort.devNum,
                                               &(dxChMllPairEntry.firstMllNode.nextHopInterface.devPort.devNum));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvUtfHwFromSwDeviceNumberGet FAILED, rc = [%d]", rc);

                return rc;
            }
        }
    }

    if(mllPairWriteForm == CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_WHOLE_E ||
        mllPairWriteForm ==
        CPSS_DXCH_IP_MLL_PAIR_READ_WRITE_SECOND_MLL_NEXT_POINTER_ONLY_E)
    {
        if( CPSS_INTERFACE_PORT_E == mllPairEntryPtr->secondMllNode.nextHopInterface.type )
        {
            rc = prvUtfHwFromSwDeviceNumberGet(mllPairEntryPtr->secondMllNode.nextHopInterface.devPort.devNum,
                                               &(dxChMllPairEntry.secondMllNode.nextHopInterface.devPort.devNum));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvUtfHwFromSwDeviceNumberGet FAILED, rc = [%d]", rc);

                return rc;
            }
        }
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChIpMLLPairWrite(devNum, mllPairEntryIndex, dxChMllPairWriteForm, &dxChMllPairEntry);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpMLLPairWrite FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(mllPairEntryIndex);
    TGF_PARAM_NOT_USED(mllPairWriteForm);
    TGF_PARAM_NOT_USED(mllPairEntryPtr);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmDBDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing LPM DB
*
* INPUTS:
*       lpmDBId   - the LPM DB id
*       devList   - the array of device ids to add to the LPM DB
*       numOfDevs - the number of device ids in the array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_NOT_FOUND - if the LPM DB id is not found.
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmDBDevListAdd
(
    IN GT_U32                         lpmDBId,
    IN GT_U8                          devList[],
    IN GT_U32                         numOfDevs
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpLpmDBDevListAdd(lpmDBId, devList, numOfDevs);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmIpLpmDbDevListAdd(lpmDBId, numOfDevs, devList);
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv4McEntryAdd
*
* DESCRIPTION:
*   Add IP MC route for a particular/all source and group address
*
* INPUTS:
*       lpmDBId             - LPM DB id
*       vrId                - virtual private network identifier
*       ipGroup             - IP MC group address
*       ipGroupPrefixLen    - number of bits that are actual valid in ipGroup
*       ipSrc               - root address for source base multi tree protocol
*       ipSrcPrefixLen      - number of bits that are actual valid in ipSrc
*       mcRouteLttEntryPtr  - LTT entry pointing to the MC route entry
*       override            - whether to override an mc Route pointer
*       defragmentationEnable - whether to enable performance costing
*                             de-fragmentation process
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success
*       GT_OUT_OF_RANGE    - if one of prefixes' lengths is too big
*       GT_ERROR           - if the virtual router does not exist
*       GT_OUT_OF_CPU_MEM  - if failed to allocate CPU memory
*       GT_OUT_OF_PP_MEM   - if failed to allocate TCAM memory
*       GT_NOT_IMPLEMENTED - if this request is not implemented
*       GT_FAIL            - otherwise
*       GT_BAD_PTR         - if one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4McEntryAdd
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPADDR                    ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPADDR                    ipSrc,
    IN GT_U32                       ipSrcPrefixLen,
    IN PRV_TGF_IP_LTT_ENTRY_STC    *mcRouteLttEntryPtr,
    IN GT_BOOL                      override,
    IN GT_BOOL                      defragmentationEnable
)
{
#ifdef CHX_FAMILY
    GT_STATUS                   rc = GT_OK;
    CPSS_DXCH_IP_LTT_ENTRY_STC  dxChMcRouteLttEntry = {0};


    /* set LTT entry */
    rc = prvTgfConvertGenericToDxChMcLttEntryInfo(mcRouteLttEntryPtr, &dxChMcRouteLttEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMcLttEntryInfo FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    return cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen,
                                     ipSrc, ipSrcPrefixLen, &dxChMcRouteLttEntry,
                                     override, defragmentationEnable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(lpmDBId);
    TGF_PARAM_NOT_USED(vrId);
    TGF_PARAM_NOT_USED(ipGroup);
    TGF_PARAM_NOT_USED(ipGroupPrefixLen);
    TGF_PARAM_NOT_USED(ipSrc);
    TGF_PARAM_NOT_USED(ipSrcPrefixLen);
    TGF_PARAM_NOT_USED(mcRouteLttEntryPtr);
    TGF_PARAM_NOT_USED(override);
    TGF_PARAM_NOT_USED(defragmentationEnable);

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv4McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success, or
*       GT_OUT_OF_RANGE - If one of prefixes' lengths is too big, or
*       GT_ERROR        - if the virtual router does not exist, or
*       GT_NOT_FOUND    - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4McEntryDel
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPADDR                    ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPADDR                    ipSrc,
    IN GT_U32                       ipSrcPrefixLen
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpLpmIpv4McEntryDel(lpmDBId, vrId, ipGroup, ipGroupPrefixLen,
                                     ipSrc, ipSrcPrefixLen);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(ipGroupPrefixLen);

    /* call device specific API */
    return cpssExMxPmIpLpmIpv4McEntryDelete(lpmDBId, vrId, ipGroup,
                                            ipSrc, ipSrcPrefixLen);
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv4UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the
*   information associated with it.
*
* INPUTS:
*       lpmDBId   - The LPM DB id.
*       vrId      - The virtual router id.
*       ipAddr    - The destination IP address to look for.
*       prefixLen - The number of bits that are actual valid in the
*                   ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr     - if found, this is the route entry info.
*                            accosiated with this UC prefix.
*       tcamRowIndexPtr    - if found, TCAM row index of this uc prefix.
*       tcamColumnIndexPtr - if found, TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK           - if the required entry was found.
*       GT_OUT_OF_RANGE - if prefix length is too big.
*       GT_BAD_PTR      - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND    - if the given ip prefix was not found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4UcPrefixSearch
(
    IN  GT_U32                               lpmDBId,
    IN  GT_U32                               vrId,
    IN  GT_IPADDR                            ipAddr,
    IN  GT_U32                               prefixLen,
    OUT PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *nextHopInfoPtr,
    OUT GT_U32                               *tcamRowIndexPtr,
    OUT GT_U32                               *tcamColumnIndexPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS                              rc = GT_OK;
    PRV_TGF_IP_ROUTING_MODE_ENT            routingMode;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT dxChnextHopInfo;

    /* get routing mode */
    rc =  prvTgfIpRoutingModeGet(&routingMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfIpRoutingModeGet FAILED, rc = [%d]", rc);
        return rc;
    }

    cpssOsMemSet(&dxChnextHopInfo, 0, sizeof(dxChnextHopInfo));

    /* call device specific API */
    rc = cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId,
                                        ipAddr, prefixLen,
                                        &dxChnextHopInfo,
                                        tcamRowIndexPtr, tcamColumnIndexPtr);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* convert Route entry */
    return prvTgfConvertDxChToGenericUcNextHopInfo(routingMode, &dxChnextHopInfo, nextHopInfoPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(lpmDBId);
    TGF_PARAM_NOT_USED(vrId);
    TGF_PARAM_NOT_USED(ipAddr);
    TGF_PARAM_NOT_USED(prefixLen);
    TGF_PARAM_NOT_USED(nextHopInfoPtr);
    TGF_PARAM_NOT_USED(tcamRowIndexPtr);
    TGF_PARAM_NOT_USED(tcamColumnIndexPtr);

    return GT_NOT_IMPLEMENTED;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv4McEntrySearch
*
* DESCRIPTION:
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc,ipGroup TCAM indexes
*
* INPUTS:
*       lpmDBId          - The LPM DB id.
*       vrId             - The virtual router Id.
*       ipGroup          - The ip Group address to get the next entry for.
*       ipGroupPrefixLen - The ip Group prefix len.
*       ipSrc            - The ip Source address to get the next entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr      - the LTT entry pointer pointing to the MC route
*                                 entry associated with this MC route.
*       tcamGroupRowIndexPtr    - pointer to TCAM group row  index.
*       tcamGroupColumnIndexPtr - pointer to TCAM group column  index.
*       tcamSrcRowIndexPtr      - pointer to TCAM source row  index.
*       tcamSrcColumnIndexPtr   - pointer to TCAM source column  index.
*
* RETURNS:
*       GT_OK           - if found.
*       GT_OUT_OF_RANGE - if prefix length is too big.
*       GT_BAD_PTR      - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND    - if the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPADDR                   ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPADDR                   ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT PRV_TGF_IP_LTT_ENTRY_STC    *mcRouteLttEntryPtr,
    OUT GT_U32                      *tcamGroupRowIndexPtr,
    OUT GT_U32                      *tcamGroupColumnIndexPtr,
    OUT GT_U32                      *tcamSrcRowIndexPtr,
    OUT GT_U32                      *tcamSrcColumnIndexPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS                   rc = GT_OK;
    CPSS_DXCH_IP_LTT_ENTRY_STC  dxChMcRouteLttEntry = {0};

    /* call device specific API */
    rc = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId,
                                        ipGroup, ipGroupPrefixLen,
                                        ipSrc, ipSrcPrefixLen,
                                        &dxChMcRouteLttEntry,
                                        tcamGroupRowIndexPtr, tcamGroupColumnIndexPtr,
                                        tcamSrcRowIndexPtr, tcamSrcColumnIndexPtr);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* convert LTT entry */
    return prvTgfConvertDxChToGenericMcLttEntryInfo(&dxChMcRouteLttEntry, mcRouteLttEntryPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(lpmDBId);
    TGF_PARAM_NOT_USED(vrId);
    TGF_PARAM_NOT_USED(ipGroup);
    TGF_PARAM_NOT_USED(ipGroupPrefixLen);
    TGF_PARAM_NOT_USED(ipSrc);
    TGF_PARAM_NOT_USED(ipSrcPrefixLen);
    TGF_PARAM_NOT_USED(mcRouteLttEntryPtr);
    TGF_PARAM_NOT_USED(tcamGroupRowIndexPtr);
    TGF_PARAM_NOT_USED(tcamGroupColumnIndexPtr);
    TGF_PARAM_NOT_USED(tcamSrcRowIndexPtr);
    TGF_PARAM_NOT_USED(tcamSrcColumnIndexPtr);

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv6UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the next
*   hop pointer associated with it and TCAM prefix index.
*
* INPUTS:
*       lpmDBId   - The LPM DB id.
*       vrId      - The virtual router id.
*       ipAddr    - The destination IP address to look for.
*       prefixLen - The number of bits that are actual valid in the
*                   ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr     - If  found, the route entry info accosiated with
*                            this UC prefix.
*       tcamRowIndexPtr    - if found, TCAM row index of this uc prefix.
*       tcamColumnIndexPtr - if found, TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK           - if the required entry was found.
*       GT_OUT_OF_RANGE - if prefix length is too big.
*       GT_BAD_PTR      - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND    - if the given ip prefix was not found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv6UcPrefixSearch
(
    IN  GT_U32                               lpmDBId,
    IN  GT_U32                               vrId,
    IN  GT_IPV6ADDR                          ipAddr,
    IN  GT_U32                               prefixLen,
    OUT PRV_TGF_IP_TCAM_ROUTE_ENTRY_INFO_UNT *nextHopInfoPtr,
    OUT GT_U32                               *tcamRowIndexPtr,
    OUT GT_U32                               *tcamColumnIndexPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS                              rc = GT_OK;
    PRV_TGF_IP_ROUTING_MODE_ENT            routingMode;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT dxChnextHopInfo;

    /* get routing mode */
    rc =  prvTgfIpRoutingModeGet(&routingMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfIpRoutingModeGet FAILED, rc = [%d]", rc);
        return rc;
    }

    cpssOsMemSet(&dxChnextHopInfo, 0, sizeof(dxChnextHopInfo));

    /* call device specific API */
    rc = cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId,
                                        ipAddr, prefixLen,
                                        &dxChnextHopInfo,
                                        tcamRowIndexPtr, tcamColumnIndexPtr);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* convert Route entry */
    return prvTgfConvertDxChToGenericUcNextHopInfo(routingMode, &dxChnextHopInfo, nextHopInfoPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(lpmDBId);
    TGF_PARAM_NOT_USED(vrId);
    TGF_PARAM_NOT_USED(ipAddr);
    TGF_PARAM_NOT_USED(prefixLen);
    TGF_PARAM_NOT_USED(nextHopInfoPtr);
    TGF_PARAM_NOT_USED(tcamRowIndexPtr);
    TGF_PARAM_NOT_USED(tcamColumnIndexPtr);

    return GT_NOT_IMPLEMENTED;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv6McEntrySearch
*
* DESCRIPTION:
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc,ipGroup TCAM indexes
*
* INPUTS:
*       lpmDBId          - The LPM DB id.
*       vrId             - The virtual router Id.
*       ipGroup          - The ip Group address to get the entry for.
*       ipGroupPrefixLen - ipGroup prefix length.
*       ipSrc            - The ip Source address to get the entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr   - the LTT entry pointer pointing to the MC route
*                              entry associated with this MC route.
*       tcamGroupRowIndexPtr - pointer to TCAM group row  index.
*       tcamSrcRowIndexPtr   - pointer to TCAM source row  index.
*
* RETURNS:
*       GT_OK           - if found.
*       GT_OUT_OF_RANGE - if one of prefix length is too big.
*       GT_BAD_PTR      - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND    - if the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv6McEntrySearch
(
    IN  GT_U32                   lpmDBId,
    IN  GT_U32                   vrId,
    IN  GT_IPV6ADDR              ipGroup,
    IN  GT_U32                   ipGroupPrefixLen,
    IN  GT_IPV6ADDR              ipSrc,
    IN  GT_U32                   ipSrcPrefixLen,
    OUT PRV_TGF_IP_LTT_ENTRY_STC *mcRouteLttEntryPtr,
    OUT GT_U32                   *tcamGroupRowIndexPtr,
    OUT GT_U32                   *tcamSrcRowIndexPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS                   rc = GT_OK;
    CPSS_DXCH_IP_LTT_ENTRY_STC  dxChMcRouteLttEntry = {0};

    /* call device specific API */
    rc = cpssDxChIpLpmIpv6McEntrySearch(lpmDBId, vrId,
                                        ipGroup, ipGroupPrefixLen,
                                        ipSrc, ipSrcPrefixLen,
                                        &dxChMcRouteLttEntry,
                                        tcamGroupRowIndexPtr,
                                        tcamSrcRowIndexPtr);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* convert LTT entry */
    return prvTgfConvertDxChToGenericMcLttEntryInfo(&dxChMcRouteLttEntry, mcRouteLttEntryPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(lpmDBId);
    TGF_PARAM_NOT_USED(vrId);
    TGF_PARAM_NOT_USED(ipGroup);
    TGF_PARAM_NOT_USED(ipGroupPrefixLen);
    TGF_PARAM_NOT_USED(ipSrc);
    TGF_PARAM_NOT_USED(ipSrcPrefixLen);
    TGF_PARAM_NOT_USED(mcRouteLttEntryPtr);
    TGF_PARAM_NOT_USED(tcamGroupRowIndexPtr);
    TGF_PARAM_NOT_USED(tcamSrcRowIndexPtr);

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv6McEntryAdd
*
* DESCRIPTION:
*   To add the multicast routing information for IP datagrams from a particular
*   source and addressed to a particular IP multicast group address for a
*   specific LPM DB.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual private network identifier.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*       mcRouteLttEntryPtr - the LTT entry pointing to the MC route entry
*                            associated with this MC route.
*       override        - weather to override the mcRoutePointerPtr for the
*                         given prefix
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented .
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = ipGroupPrefixLen = 0.
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv6McEntryAdd
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPV6ADDR                  ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPV6ADDR                  ipSrc,
    IN GT_U32                       ipSrcPrefixLen,
    IN PRV_TGF_IP_LTT_ENTRY_STC    *mcRouteLttEntryPtr,
    IN GT_BOOL                      override,
    IN GT_BOOL                      defragmentationEnable
)
{
#ifdef CHX_FAMILY
    GT_STATUS                   rc = GT_OK;
    CPSS_DXCH_IP_LTT_ENTRY_STC  dxChMcRouteLttEntry;


    /* set LTT en
     * try */
    cpssOsMemCpy((GT_VOID*) &dxChMcRouteLttEntry, mcRouteLttEntryPtr, sizeof(dxChMcRouteLttEntry));

    /* set route type */
    rc = prvTgfConvertGenericToDxChRouteType(mcRouteLttEntryPtr->routeType,
                                             &(dxChMcRouteLttEntry.routeType));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChRouteType FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    return cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen,
                                     ipSrc, ipSrcPrefixLen, &dxChMcRouteLttEntry,
                                     override, defragmentationEnable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(lpmDBId);
    TGF_PARAM_NOT_USED(vrId);
    TGF_PARAM_NOT_USED(ipGroup);
    TGF_PARAM_NOT_USED(ipGroupPrefixLen);
    TGF_PARAM_NOT_USED(ipSrc);
    TGF_PARAM_NOT_USED(ipSrcPrefixLen);
    TGF_PARAM_NOT_USED(mcRouteLttEntryPtr);
    TGF_PARAM_NOT_USED(override);
    TGF_PARAM_NOT_USED(defragmentationEnable);

    return GT_OK;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv4McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB
*
* INPUTS:
*       lpmDBId - LPM DB id
*       vrId    - virtual router identifier
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv4McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmIpLpmIpv4McEntriesFlush(lpmDBId, vrId);
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmIpv6McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB
*
* INPUTS:
*       lpmDBId - LPM DB id
*       vrId    - virtual router identifier
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmIpv6McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmIpLpmIpv6McEntriesFlush(lpmDBId, vrId);
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpSetMllCntInterface
*
* DESCRIPTION:
*      Sets a mll counter set's bounded inteface.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum          - the device number
*       mllCntSet       - the mll counter set out of the 2.
*       interfaceCfgPtr - the mll counter interface configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpSetMllCntInterface
(
    IN GT_U32                                    mllCntSet,
    IN PRV_TGF_IP_COUNTER_SET_INTERFACE_CFG_STC *interfaceCfgPtr
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC  dxChInterfaceCfg;

    /* reset variable */
    cpssOsMemSet(&dxChInterfaceCfg, 0, sizeof(dxChInterfaceCfg));

    /* convert Port/Trunk mode into device specific format */
    switch (interfaceCfgPtr->portTrunkCntMode)
    {
        case PRV_TGF_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E:
            dxChInterfaceCfg.portTrunkCntMode = CPSS_DXCH_IP_DISREGARD_PORT_TRUNK_CNT_MODE_E;
            break;

        case PRV_TGF_IP_PORT_CNT_MODE_E:
            dxChInterfaceCfg.portTrunkCntMode = CPSS_DXCH_IP_PORT_CNT_MODE_E;
            dxChInterfaceCfg.portTrunk.port   = interfaceCfgPtr->portTrunk.port;
            break;

        case PRV_TGF_IP_TRUNK_CNT_MODE_E:
            dxChInterfaceCfg.portTrunkCntMode = CPSS_DXCH_IP_TRUNK_CNT_MODE_E;
            dxChInterfaceCfg.portTrunk.trunk  = interfaceCfgPtr->portTrunk.trunk;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert the counter Set vlan mode into device specific format */
    switch (interfaceCfgPtr->vlanMode)
    {
        case PRV_TGF_IP_DISREGARD_VLAN_CNT_MODE_E:
            dxChInterfaceCfg.vlanMode = CPSS_DXCH_IP_DISREGARD_VLAN_CNT_MODE_E;
            break;

        case PRV_TGF_IP_USE_VLAN_CNT_MODE_E:
            dxChInterfaceCfg.vlanMode = CPSS_DXCH_IP_USE_VLAN_CNT_MODE_E;
            dxChInterfaceCfg.vlanId   = interfaceCfgPtr->vlanId;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert other fields */
    dxChInterfaceCfg.ipMode = interfaceCfgPtr->ipMode;
    dxChInterfaceCfg.devNum = interfaceCfgPtr->devNum;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChIpSetMllCntInterface(devNum, mllCntSet, &dxChInterfaceCfg);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpSetMllCntInterface FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(mllCntSet);
    TGF_PARAM_NOT_USED(interfaceCfgPtr);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpMllCntGet
*
* DESCRIPTION:
*      Get the mll counter.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum     - the device number.
*       mllCntSet  - the mll counter set out of the 2
*
* OUTPUTS:
*       mllOutMCPktsPtr - According to the configuration of this cnt set, The
*                      number of routed IP Multicast packets Duplicated by the
*                      MLL Engine and transmitted via this interface
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpMllCntGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  mllCntSet,
    OUT GT_U32  *mllOutMCPktsPtr
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpMllCntGet(devNum, mllCntSet, mllOutMCPktsPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(mllCntSet);
    TGF_PARAM_NOT_USED(mllOutMCPktsPtr);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpMllCntSet
*
* DESCRIPTION:
*      set an mll counter.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum          - the device number
*       mllCntSet       - the mll counter set out of the 2
*       mllOutMCPkts    - the counter value to set
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - Illegal parameter in function called
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpMllCntSet
(
    IN GT_U8    devNum,
    IN GT_U32   mllCntSet,
    IN GT_U32   mllOutMCPkts
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpMllCntSet(devNum, mllCntSet, mllOutMCPkts);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(mllCntSet);
    TGF_PARAM_NOT_USED(mllOutMCPkts);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpRouterMacSaBaseSet
*
* DESCRIPTION:
*       Sets 40 MSBs of Router MAC SA Base address on specified device.
*
* INPUTS:
*       devNum - the device number
*       macPtr - (pointer to)The system Mac address to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfIpRouterMacSaBaseSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *macPtr
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpRouterMacSaBaseSet(devNum, macPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmIpRouterMacSaBaseSet(devNum, macPtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpRouterMacSaModifyEnable
*
* DESCRIPTION:
*      Per Egress port bit Enable Routed packets MAC SA Modification
*
* INPUTS:
*       devNum   - the device number
*       portNum  - physical or CPU port number.
*       enable   - GT_FALSE: MAC SA Modification of routed packets is disabled
*                  GT_TRUE: MAC SA Modification of routed packets is enabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum/portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpRouterMacSaModifyEnable
(
    IN  GT_U8                      devNum,
    IN  GT_U8                      portNum,
    IN  GT_BOOL                    enable
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpRouterMacSaModifyEnable(
        devNum, portNum, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(enable);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpPortRouterMacSaLsbModeSet
*
* DESCRIPTION:
*       Sets the mode, per port, in which the device sets the packet's MAC SA
*       least significant bytes.
*
* INPUTS:
*       devNum          - the device number
*       portNum         - the port number
*       saLsbMode       - The MAC SA least-significant bit mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum/saLsbMode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The device 5 most significant bytes are set by cpssDxChIpRouterMacSaBaseSet().
*
*       The least significant bytes are set by:
*       Port mode is set by cpssDxChIpRouterPortMacSaLsbSet().
*       Vlan mode is set by cpssDxChIpRouterVlanMacSaLsbSet().
*
*******************************************************************************/
GT_STATUS prvTgfIpPortRouterMacSaLsbModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_MAC_SA_LSB_MODE_ENT    saLsbMode
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpPortRouterMacSaLsbModeSet(
        devNum, portNum, saLsbMode);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmIpPortRouterMacSaLsbModeSet(
        devNum, portNum, saLsbMode);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpRouterPortMacSaLsbSet
*
* DESCRIPTION:
*       Sets the 8 LSB Router MAC SA for this EGGRESS PORT.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*     devNum          - the device number
*     portNum         - Eggress Port number
*     saMac           - The 8 bits SA mac value to be written to the SA bits of
*                       routed packet if SA alteration mode is configured to
*                       take LSB according to Eggress Port number.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK                    - on success
*     GT_FAIL                  - on error.
*     GT_HW_ERROR              - on hardware error
*     GT_BAD_PARAM             - wrong devNum
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpRouterPortMacSaLsbSet
(
    IN GT_U8   devNum,
    IN GT_U8   portNum,
    IN GT_U8   saMac
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpRouterPortMacSaLsbSet(
        devNum, portNum, saMac);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmIpRouterPortMacSaLsbSet(
        devNum, portNum, saMac);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpRouterVlanMacSaLsbSet
*
* DESCRIPTION:
*       Sets the 8 LSB Router MAC SA for this VLAN.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*     devNum          - the device number
*     vlan            - VLAN Id
*     saMac           - The 8 bits SA mac value to be written to the SA bits of
*                       routed packet if SA alteration mode is configured to
*                       take LSB according to VLAN.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK                    - on success
*     GT_FAIL                  - on error.
*     GT_HW_ERROR              - on hardware error
*     GT_BAD_PARAM             - wrong devNum or vlanId
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfIpRouterVlanMacSaLsbSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlan,
    IN GT_U8   saMac
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChIpRouterVlanMacSaLsbSet(
        devNum, vlan, saMac);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmIpRouterVlanMacSaLsbSet(
        devNum, vlan, saMac);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}


/*******************************************************************************
* prvTgfIpRouterNextHopTableAgeBitsEntryRead
*
* DESCRIPTION:
*     read router next hop table age bits entry.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       activityBitPtr    - (pointer to) Age bit value of requested Next-hop entry.
*                           field. Range 0..1.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/

GT_STATUS prvTgfIpRouterNextHopTableAgeBitsEntryRead
(
    GT_U32     *activityBitPtr
)
{
#ifdef CHX_FAMILY
    GT_U32      rc = GT_OK;
    GT_U32      routerNextHopTableAgeBitsEntryIndex;
    GT_U32      routerNextHopTableAgeBitsEntryPtr;
    GT_U8       bitOffset;

    routerNextHopTableAgeBitsEntryIndex = prvTgfRouteEntryBaseIndex / 32;
    bitOffset = (GT_U8)(prvTgfRouteEntryBaseIndex % 32);

    /* call device specific API */
    rc = cpssDxChIpRouterNextHopTableAgeBitsEntryRead(prvTgfDevNum,
                                                      routerNextHopTableAgeBitsEntryIndex,
                                                      &routerNextHopTableAgeBitsEntryPtr);
    if(rc != GT_OK)
        return rc;

    *activityBitPtr = ((routerNextHopTableAgeBitsEntryPtr >> bitOffset) & 0x1);

    return rc;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmIpRouteEntryAgeRefreshGet(prvTgfDevNum, prvTgfRouteEntryBaseIndex, activityBitPtr);

#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmDBCapacityGet
*
* DESCRIPTION:
*   This function gets the current LPM DB allocation.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       lpmDBId                     - the LPM DB id.
*
* OUTPUTS:
*       indexesRangePtr             - the range of TCAM indexes availble for this
*                                     LPM DB (see explanation in
*                                     CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       partitionEnablePtr          - GT_TRUE:  the TCAM is partitioned according
*                                     to the capacityCfgPtr, any unused TCAM entries
*                                     were allocated to IPv4 UC entries.
*                                     GT_FALSE: TCAM entries are allocated on demand
*                                     while entries are guaranteed as specified
*                                     in capacityCfgPtr.
*       tcamLpmManagerCapcityCfgPtr - the current capacity configuration. when
*                                     partitionEnable in cpssDxChIpLpmDBCreate
*                                     was set to GT_TRUE this means current
*                                     prefixes partition, when this was set to
*                                     GT_FALSE this means the current guaranteed
*                                     prefixes allocation.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NO_RESOURCE           - failed to allocate resources needed to the
*                                  new configuration.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmDBCapacityGet
(
    IN  GT_U32                                     lpmDBId,
    OUT GT_BOOL                                    *partitionEnablePtr,
    OUT PRV_TGF_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    OUT PRV_TGF_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *tcamLpmManagerCapcityCfgPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc = GT_OK;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC dxChIndexesRange;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC dxChTcamLpmManagerCapcityCfg;

    /* reset variables */
    cpssOsMemSet(&dxChIndexesRange,             0, sizeof(dxChIndexesRange));
    cpssOsMemSet(&dxChTcamLpmManagerCapcityCfg, 0, sizeof(dxChTcamLpmManagerCapcityCfg));

    /* call device specific API */
    rc = cpssDxChIpLpmDBCapacityGet(lpmDBId,
                                    partitionEnablePtr,
                                    &dxChIndexesRange,
                                    &dxChTcamLpmManagerCapcityCfg);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpLpmDBCapacityGet FAILED, rc = [%d]", rc);
    }

    /* convert indexesRangePtr into device specific format */
    indexesRangePtr->firstIndex = dxChIndexesRange.firstIndex;
    indexesRangePtr->lastIndex  = dxChIndexesRange.lastIndex;

    /* convert the counter Set vlan mode into device specific format */
    tcamLpmManagerCapcityCfgPtr->numOfIpv4Prefixes         = dxChTcamLpmManagerCapcityCfg.numOfIpv4Prefixes;
    tcamLpmManagerCapcityCfgPtr->numOfIpv4McSourcePrefixes = dxChTcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes;
    tcamLpmManagerCapcityCfgPtr->numOfIpv6Prefixes         = dxChTcamLpmManagerCapcityCfg.numOfIpv6Prefixes;

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(lpmDBId);
    TGF_PARAM_NOT_USED(partitionEnablePtr);
    TGF_PARAM_NOT_USED(indexesRangePtr);
    TGF_PARAM_NOT_USED(tcamLpmManagerCapcityCfgPtr);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfIpLpmDBCapacityUpdate
*
* DESCRIPTION:
*   This function updates the initial LPM DB allocation.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       lpmDBId                     - the LPM DB id.
*       indexesRangePtr             - the range of TCAM indexes availble for this
*                                     LPM DB (see explanation in
*                                     CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       tcamLpmManagerCapcityCfgPtr - the new capacity configuration. when
*                                     partitionEnable in cpssDxChIpLpmDBCreate
*                                     was set to GT_TRUE this means new prefixes
*                                     partition, when this was set to GT_FALSE
*                                     this means the new prefixes guaranteed
*                                     allocation.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success
*       GT_BAD_PARAM                - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*       GT_NOT_FOUND                - if the LPM DB id is not found
*       GT_NO_RESOURCE              - failed to allocate resources needed to the
*                                     new configuration
*
* COMMENTS:
*       This API is relevant only when using TCAM Manager. This API is used for
*       updating only the capacity configuration of the LPM. for updating the
*       lines reservation for the TCAM Manger use cpssDxChTcamManagerRangeUpdate.
*
*******************************************************************************/
GT_STATUS prvTgfIpLpmDBCapacityUpdate
(
    IN GT_U32                                     lpmDBId,
    IN PRV_TGF_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    IN PRV_TGF_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *tcamLpmManagerCapcityCfgPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc = GT_OK;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC dxChIndexesRange;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC dxChTcamLpmManagerCapcityCfg;

    /* convert indexesRangePtr into device specific format */
    dxChIndexesRange.firstIndex = indexesRangePtr->firstIndex;
    dxChIndexesRange.lastIndex  = indexesRangePtr->lastIndex;

    /* convert the counter Set vlan mode into device specific format */
    dxChTcamLpmManagerCapcityCfg.numOfIpv4Prefixes         = tcamLpmManagerCapcityCfgPtr->numOfIpv4Prefixes;
    dxChTcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = tcamLpmManagerCapcityCfgPtr->numOfIpv4McSourcePrefixes;
    dxChTcamLpmManagerCapcityCfg.numOfIpv6Prefixes         = tcamLpmManagerCapcityCfgPtr->numOfIpv6Prefixes;

    /* call device specific API */
    rc = cpssDxChIpLpmDBCapacityUpdate(lpmDBId,
                                       &dxChIndexesRange,
                                       &dxChTcamLpmManagerCapcityCfg);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChIpLpmDBCapacityUpdate FAILED, rc = [%d]", rc);
    }

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(lpmDBId);
    TGF_PARAM_NOT_USED(indexesRangePtr);
    TGF_PARAM_NOT_USED(tcamLpmManagerCapcityCfgPtr);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

