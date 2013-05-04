/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfTunnelGen.c
*
* DESCRIPTION:
*       Generic API implementation for Tunnel
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
#include <common/tgfTunnelGen.h>

#ifdef CHX_FAMILY
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#endif /*CHX_FAMILY*/
/******************************************************************************\
 *                              Marco definitions                             *
\******************************************************************************/

#ifdef CHX_FAMILY
/* convert QoS mark modes into device specific format */
#define PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(dstMarkMode, srcMarkMode)            \
    do                                                                         \
    {                                                                          \
        switch (srcMarkMode)                                                   \
        {                                                                      \
            case PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E:                       \
                dstMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;    \
                break;                                                         \
                                                                               \
            case PRV_TGF_TUNNEL_START_MARK_FROM_PACKET_QOS_PROFILE_E:          \
                dstMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E;\
                break;                                                         \
                                                                               \
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)

/* convert QoS mark modes from device specific format */
#define PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(dstMarkMode, srcMarkMode)            \
    do                                                                         \
    {                                                                          \
        switch (dstMarkMode)                                                   \
        {                                                                      \
            case CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E:                 \
                srcMarkMode = PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E;          \
                break;                                                         \
                                                                               \
            case CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_PACKET_QOS_PROFILE_E:    \
                srcMarkMode = PRV_TGF_TUNNEL_START_MARK_FROM_PACKET_QOS_PROFILE_E; \
                break;                                                         \
                                                                               \
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)

/* convert TTI key type into device specific format */
#define PRV_TGF_S2D_KEY_TYPE_CONVERT_MAC(dstKeyType, srcKeyType)               \
    do                                                                         \
    {                                                                          \
        switch (srcKeyType)                                                    \
        {                                                                      \
            case PRV_TGF_TTI_KEY_IPV4_E:                                       \
                dstKeyType = CPSS_DXCH_TTI_KEY_IPV4_E;                         \
                break;                                                         \
                                                                               \
            case PRV_TGF_TTI_KEY_MPLS_E:                                       \
                dstKeyType = CPSS_DXCH_TTI_KEY_MPLS_E;                         \
                break;                                                         \
                                                                               \
            case PRV_TGF_TTI_KEY_ETH_E:                                        \
                dstKeyType = CPSS_DXCH_TTI_KEY_ETH_E;                          \
                break;                                                         \
                                                                               \
            case PRV_TGF_TTI_KEY_MIM_E:                                        \
                dstKeyType = CPSS_DXCH_TTI_KEY_MIM_E;                          \
                break;                                                         \
                                                                               \
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)

/* convert TTI key type from device specific format */
#define PRV_TGF_D2S_KEY_TYPE_CONVERT_MAC(dstKeyType, srcKeyType)               \
    do                                                                         \
    {                                                                          \
        switch (dstKeyType)                                                    \
        {                                                                      \
            case CPSS_DXCH_TTI_KEY_IPV4_E:                                     \
                srcKeyType = PRV_TGF_TTI_KEY_IPV4_E;                           \
                break;                                                         \
                                                                               \
            case CPSS_DXCH_TTI_KEY_MPLS_E:                                     \
                srcKeyType = PRV_TGF_TTI_KEY_MPLS_E;                           \
                break;                                                         \
                                                                               \
            case CPSS_DXCH_TTI_KEY_ETH_E:                                      \
                srcKeyType = PRV_TGF_TTI_KEY_ETH_E;                            \
                break;                                                         \
                                                                               \
            case CPSS_DXCH_TTI_KEY_MIM_E:                                      \
                srcKeyType = PRV_TGF_TTI_KEY_MIM_E;                            \
                break;                                                         \
                                                                               \
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)

/* convert MAC mode into device specific format */
#define PRV_TGF_S2D_MAC_MODE_CONVERT_MAC(dstMacMode, srcMacMode)               \
    do                                                                         \
    {                                                                          \
        switch (srcMacMode)                                                    \
        {                                                                      \
            case PRV_TGF_TTI_MAC_MODE_DA_E:                                    \
                dstMacMode = CPSS_DXCH_TTI_MAC_MODE_DA_E;                      \
                break;                                                         \
                                                                               \
            case PRV_TGF_TTI_MAC_MODE_SA_E:                                    \
                dstMacMode = CPSS_DXCH_TTI_MAC_MODE_SA_E;                      \
                break;                                                         \
                                                                               \
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)

/* convert MAC mode from device specific format */
#define PRV_TGF_D2S_MAC_MODE_CONVERT_MAC(dstMacMode, srcMacMode)               \
    do                                                                         \
    {                                                                          \
        switch (dstMacMode)                                                    \
        {                                                                      \
            case CPSS_DXCH_TTI_MAC_MODE_DA_E:                                  \
                srcMacMode = PRV_TGF_TTI_MAC_MODE_DA_E;                        \
                break;                                                         \
                                                                               \
            case CPSS_DXCH_TTI_MAC_MODE_SA_E:                                  \
                srcMacMode = PRV_TGF_TTI_MAC_MODE_SA_E;                        \
                break;                                                         \
                                                                               \
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
/* convert QoS mark modes into device specific format */
#define PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(dstMarkMode, srcMarkMode)            \
    do                                                                         \
    {                                                                          \
        switch (srcMarkMode)                                                   \
        {                                                                      \
            case PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E:                       \
                dstMarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_ENTRY_E;      \
                break;                                                         \
                                                                               \
            case PRV_TGF_TUNNEL_START_MARK_FROM_INGRESS_PIPE_E:                \
                dstMarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_INGRESS_PIPE_E;\
                break;                                                         \
                                                                               \
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)

/* convert QoS mark modes from device specific format */
#define PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(dstMarkMode, srcMarkMode)            \
    do                                                                         \
    {                                                                          \
        switch (dstMarkMode)                                                   \
        {                                                                      \
            case CPSS_EXMXPM_TUNNEL_START_MARK_FROM_ENTRY_E:                   \
                srcMarkMode = PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E;          \
                break;                                                         \
                                                                               \
            case CPSS_EXMXPM_TUNNEL_START_MARK_FROM_INGRESS_PIPE_E:            \
                srcMarkMode = PRV_TGF_TUNNEL_START_MARK_FROM_INGRESS_PIPE_E;   \
                break;                                                         \
                                                                               \
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)

/* convert TTI key type into device specific format */
#define PRV_TGF_S2D_KEY_TYPE_CONVERT_MAC(dstKeyType, srcKeyType)               \
    do                                                                         \
    {                                                                          \
        switch (srcKeyType)                                                    \
        {                                                                      \
            case PRV_TGF_TTI_KEY_IPV4_E:                                       \
                dstKeyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;                       \
                break;                                                         \
                                                                               \
            case PRV_TGF_TTI_KEY_MPLS_E:                                       \
                dstKeyType = CPSS_EXMXPM_TTI_KEY_MPLS_E;                       \
                break;                                                         \
                                                                               \
            case PRV_TGF_TTI_KEY_ETH_E:                                        \
                dstKeyType = CPSS_EXMXPM_TTI_KEY_ETH_E;                        \
                break;                                                         \
                                                                               \
            case PRV_TGF_TTI_KEY_MIM_E:                                        \
                dstKeyType = CPSS_EXMXPM_TTI_KEY_MIM_E;                        \
                break;                                                         \
                                                                               \
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)

/* convert TTI key type from device specific format */
#define PRV_TGF_D2S_KEY_TYPE_CONVERT_MAC(dstKeyType, srcKeyType)               \
    do                                                                         \
    {                                                                          \
        switch (dstKeyType)                                                    \
        {                                                                      \
            case CPSS_EXMXPM_TTI_KEY_IPV4_E:                                   \
                srcKeyType = PRV_TGF_TTI_KEY_IPV4_E;                           \
                break;                                                         \
                                                                               \
            case CPSS_EXMXPM_TTI_KEY_MPLS_E:                                   \
                srcKeyType = PRV_TGF_TTI_KEY_MPLS_E;                           \
                break;                                                         \
                                                                               \
            case CPSS_EXMXPM_TTI_KEY_ETH_E:                                    \
                srcKeyType = PRV_TGF_TTI_KEY_ETH_E;                            \
                break;                                                         \
                                                                               \
            case CPSS_EXMXPM_TTI_KEY_MIM_E:                                    \
                srcKeyType = PRV_TGF_TTI_KEY_MIM_E;                            \
                break;                                                         \
                                                                               \
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)

/* convert MAC mode into device specific format */
#define PRV_TGF_S2D_MAC_MODE_CONVERT_MAC(dstMacMode, srcMacMode)               \
    do                                                                         \
    {                                                                          \
        switch (srcMacMode)                                                    \
        {                                                                      \
            case PRV_TGF_TTI_MAC_MODE_DA_E:                                    \
                dstMacMode = CPSS_EXMXPM_TTI_MAC_MODE_DA_E;                    \
                break;                                                         \
                                                                               \
            case PRV_TGF_TTI_MAC_MODE_SA_E:                                    \
                dstMacMode = CPSS_EXMXPM_TTI_MAC_MODE_SA_E;                    \
                break;                                                         \
                                                                               \
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)

/* convert MAC mode from device specific format */
#define PRV_TGF_D2S_MAC_MODE_CONVERT_MAC(dstMacMode, srcMacMode)               \
    do                                                                         \
    {                                                                          \
        switch (dstMacMode)                                                    \
        {                                                                      \
            case CPSS_EXMXPM_TTI_MAC_MODE_DA_E:                                \
                srcMacMode = PRV_TGF_TTI_MAC_MODE_DA_E;                        \
                break;                                                         \
                                                                               \
            case CPSS_EXMXPM_TTI_MAC_MODE_SA_E:                                \
                srcMacMode = PRV_TGF_TTI_MAC_MODE_SA_E;                        \
                break;                                                         \
                                                                               \
            default:                                                           \
                return GT_BAD_PARAM;                                           \
        }                                                                      \
    } while (0)
#endif /* EXMXPM_FAMILY */


/******************************************************************************\
 *                     Private function's implementation                      *
\******************************************************************************/

#ifdef CHX_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToDxChTtiAction
*
* DESCRIPTION:
*       Convert generic TTI action type1 into device specific TTI action
*
* INPUTS:
*       actionPtr - (pointer to) TTI rule action
*
* OUTPUTS:
*       dxChTtiActionPtr - (pointer to) DxCh TTI rule action
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChTtiAction
(
    IN  PRV_TGF_TTI_ACTION_STC       *actionPtr,
    OUT CPSS_DXCH_TTI_ACTION_STC     *dxChTtiActionPtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) dxChTtiActionPtr, 0, sizeof(*dxChTtiActionPtr));

    /* convert passengerPacketType into device specific format */
    switch (actionPtr->passengerPacketType)
    {
        case PRV_TGF_TTI_PASSENGER_IPV4_E:
            dxChTtiActionPtr->passengerPacketType = CPSS_DXCH_TTI_PASSENGER_IPV4_E;
            break;

        case PRV_TGF_TTI_PASSENGER_IPV6_E:
            dxChTtiActionPtr->passengerPacketType = CPSS_DXCH_TTI_PASSENGER_IPV6_E;
            break;

        case PRV_TGF_TTI_PASSENGER_ETHERNET_CRC_E:
            dxChTtiActionPtr->passengerPacketType = CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E;
            break;

        case PRV_TGF_TTI_PASSENGER_ETHERNET_NO_CRC_E:
            dxChTtiActionPtr->passengerPacketType = CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E;
            break;

        case PRV_TGF_TTI_PASSENGER_IPV4V6_E:
            dxChTtiActionPtr->passengerPacketType = CPSS_DXCH_TTI_PASSENGER_IPV4V6_E;
            break;

        case PRV_TGF_TTI_PASSENGER_MPLS_E:
            dxChTtiActionPtr->passengerPacketType = CPSS_DXCH_TTI_PASSENGER_MPLS_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert redirectCommand into device specific format */
    switch (actionPtr->redirectCommand)
    {
        case PRV_TGF_TTI_NO_REDIRECT_E:
            dxChTtiActionPtr->redirectCommand = CPSS_DXCH_TTI_NO_REDIRECT_E;
            break;

        case PRV_TGF_TTI_REDIRECT_TO_EGRESS_E:
            dxChTtiActionPtr->redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E;
            break;

        case PRV_TGF_TTI_REDIRECT_TO_ROUTER_LOOKUP_E:
            dxChTtiActionPtr->redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E;
            break;

        case PRV_TGF_TTI_VRF_ID_ASSIGN_E:
            dxChTtiActionPtr->redirectCommand = CPSS_DXCH_TTI_VRF_ID_ASSIGN_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert vlanCmd into device specific format */
    switch (actionPtr->vlanCmd)
    {
        case PRV_TGF_TTI_VLAN_DO_NOT_MODIFY_E:
            dxChTtiActionPtr->vlanCmd = CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E;
            break;

        case PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E:
            dxChTtiActionPtr->vlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
            break;

        case PRV_TGF_TTI_VLAN_MODIFY_TAGGED_E:
            dxChTtiActionPtr->vlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E;
            break;

        case PRV_TGF_TTI_VLAN_MODIFY_ALL_E:
            dxChTtiActionPtr->vlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert qosTrustMode into device specific format */
    switch (actionPtr->qosTrustMode)
    {
        case PRV_TGF_TTI_QOS_KEEP_PREVIOUS_E:
            dxChTtiActionPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_KEEP_PREVIOUS_E;
            break;

        case PRV_TGF_TTI_QOS_TRUST_PASS_L2_E:
            dxChTtiActionPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_E;
            break;

        case PRV_TGF_TTI_QOS_TRUST_PASS_L3_E:
            dxChTtiActionPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_TRUST_PASS_L3_E;
            break;

        case PRV_TGF_TTI_QOS_TRUST_PASS_L2_L3_E:
            dxChTtiActionPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_L3_E;
            break;

        case PRV_TGF_TTI_QOS_UNTRUST_E:
            dxChTtiActionPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_UNTRUST_E;
            break;

        case PRV_TGF_TTI_QOS_TRUST_MPLS_EXP_E:
            dxChTtiActionPtr->qosTrustMode = CPSS_DXCH_TTI_QOS_TRUST_MPLS_EXP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert modifyUpEnable into device specific format */
    switch (actionPtr->modifyUpEnable)
    {
        case PRV_TGF_TTI_DO_NOT_MODIFY_PREV_UP_E:
            dxChTtiActionPtr->modifyUpEnable = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E;
            break;

        case PRV_TGF_TTI_MODIFY_UP_ENABLE_E:
            dxChTtiActionPtr->modifyUpEnable = CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E;
            break;

        case PRV_TGF_TTI_MODIFY_UP_DISABLE_E:
            dxChTtiActionPtr->modifyUpEnable = CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E;
            break;

        case PRV_TGF_TTI_MODIFY_UP_RESERVED_E:
            dxChTtiActionPtr->modifyUpEnable = CPSS_DXCH_TTI_MODIFY_UP_RESERVED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert modifyDscpEnable into device specific format */
    switch (actionPtr->modifyDscpEnable)
    {
        case PRV_TGF_TTI_DO_NOT_MODIFY_PREV_DSCP_E:
            dxChTtiActionPtr->modifyDscpEnable = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E;
            break;

        case PRV_TGF_TTI_MODIFY_DSCP_ENABLE_E:
            dxChTtiActionPtr->modifyDscpEnable = CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E;
            break;

        case PRV_TGF_TTI_MODIFY_DSCP_DISABLE_E:
            dxChTtiActionPtr->modifyDscpEnable = CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E;
            break;

        case PRV_TGF_TTI_MODIFY_DSCP_RESERVED_E:
            dxChTtiActionPtr->modifyDscpEnable = CPSS_DXCH_TTI_MODIFY_DSCP_RESERVED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tti action into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, command);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, userDefinedCpuCode);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tunnelTerminate);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, copyTtlFromTunnelHeader);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, mirrorToIngressAnalyzerEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, policerIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, vlanPrecedence);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, vlanId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, nestedVlanEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tunnelStart);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tunnelStartPtr);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, routerLookupPtr);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, vrfId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, useVidx);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, sourceIdSetEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, sourceId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, bindToPolicer);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, qosPrecedence);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, qosProfile);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, remapDSCP);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, vntl2Echo);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, bridgeBypass);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, actionStop);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, activateCounter);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, counterIndex);

    /* set interface info */
    cpssOsMemCpy(&(dxChTtiActionPtr->egressInterface), &(actionPtr->interfaceInfo), sizeof(actionPtr->interfaceInfo));

    if( PRV_TGF_TTI_REDIRECT_TO_EGRESS_E == actionPtr->redirectCommand &&
        CPSS_INTERFACE_PORT_E == actionPtr->interfaceInfo.type )
    {
        return prvUtfHwFromSwDeviceNumberGet(actionPtr->interfaceInfo.devPort.devNum,
                                             &(dxChTtiActionPtr->egressInterface.devPort.devNum));
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChTtiAction2
*
* DESCRIPTION:
*       Convert generic TTI action type2 into device specific TTI action
*
* INPUTS:
*       actionPtr - (pointer to) TTI rule action
*
* OUTPUTS:
*       dxChTtiAction2Ptr - (pointer to) DxCh TTI rule action
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChTtiAction2
(
    IN  PRV_TGF_TTI_ACTION_STC       *actionPtr,
    OUT CPSS_DXCH_TTI_ACTION_2_STC   *dxChTtiActionPtr
)
{
    GT_STATUS rc;

    /* clear tti action */
    cpssOsMemSet((GT_VOID*) dxChTtiActionPtr, 0, sizeof(*dxChTtiActionPtr));

    /* convert tti action into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tunnelTerminate);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, copyTtlFromTunnelHeader);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, command);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, arpPtr);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tunnelStart);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tunnelStartPtr);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, vrfId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, sourceIdSetEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, sourceId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tag0VlanId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tag1VlanId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, nestedVlanEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, bindToPolicer);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, policerIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, qosPrecedence);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, keepPreviousQoS);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, trustUp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, trustDscp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, trustExp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, qosProfile);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tag0Up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tag1Up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, remapDSCP);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, iPclConfigIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, mirrorToIngressAnalyzerEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, userDefinedCpuCode);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, bindToCentralCounter);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, centralCounterIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, vntl2Echo);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, bridgeBypass);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, ingressPipeBypass);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, actionStop);

    /* convert tti action into device specific format */
    dxChTtiActionPtr->mplsTtl            = actionPtr->ttl;
    dxChTtiActionPtr->enableDecrementTtl = actionPtr->enableDecTtl;
    dxChTtiActionPtr->routerLttPtr       = actionPtr->routerLookupPtr;
    dxChTtiActionPtr->tag0VlanPrecedence = actionPtr->vlanPrecedence;
    dxChTtiActionPtr->bindToPolicerMeter = actionPtr->meterEnable;

    /* set interface info */
    cpssOsMemCpy(&(dxChTtiActionPtr->egressInterface),
                 &(actionPtr->interfaceInfo),
                 sizeof(dxChTtiActionPtr->egressInterface));

    if( PRV_TGF_TTI_REDIRECT_TO_EGRESS_E == actionPtr->redirectCommand &&
        CPSS_INTERFACE_PORT_E == actionPtr->interfaceInfo.type )
    {
        rc = prvUtfHwFromSwDeviceNumberGet(actionPtr->interfaceInfo.devPort.devNum,
                                           &(dxChTtiActionPtr->egressInterface.devPort.devNum));
        if(GT_OK != rc)
            return rc;
    }

    /* convert ttPassengerPacketType into device specific format */
    switch (actionPtr->passengerPacketType)
    {
        case PRV_TGF_TTI_PASSENGER_IPV4_E:
        case PRV_TGF_TTI_PASSENGER_IPV6_E:
        case PRV_TGF_TTI_PASSENGER_IPV4V6_E:
            dxChTtiActionPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_IPV4V6_E;
            break;

        case PRV_TGF_TTI_PASSENGER_ETHERNET_CRC_E:
            dxChTtiActionPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E;
            break;

        case PRV_TGF_TTI_PASSENGER_ETHERNET_NO_CRC_E:
            dxChTtiActionPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E;
            break;

        case PRV_TGF_TTI_PASSENGER_MPLS_E:
            dxChTtiActionPtr->ttPassengerPacketType = CPSS_DXCH_TTI_PASSENGER_MPLS_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tsPassengerPacketType into device specific format */
    switch (actionPtr->tsPassengerPacketType)
    {
        case PRV_TGF_TUNNEL_PASSENGER_ETHERNET_E:
            dxChTtiActionPtr->tsPassengerPacketType = CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E;
            break;

        case PRV_TGF_TUNNEL_PASSENGER_OTHER_E:
            dxChTtiActionPtr->tsPassengerPacketType = CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert mplsCommand into device specific format */
    switch (actionPtr->mplsCmd)
    {
        case PRV_TGF_TTI_MPLS_NOP_COMMAND_E:
            dxChTtiActionPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_NOP_CMD_E;
            break;

        case PRV_TGF_TTI_MPLS_SWAP_COMMAND_E:
            dxChTtiActionPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_SWAP_CMD_E;
            break;

        case PRV_TGF_TTI_MPLS_PUSH1_LABEL_COMMAND_E:
            dxChTtiActionPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_PUSH1_CMD_E;
            break;

        case PRV_TGF_TTI_MPLS_POP1_LABEL_COMMAND_E:
            dxChTtiActionPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_POP1_CMD_E;
            break;

        case PRV_TGF_TTI_MPLS_POP2_LABELS_COMMAND_E:
            dxChTtiActionPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_POP2_CMD_E;
            break;

        case PRV_TGF_TTI_MPLS_POP_AND_SWAP_COMMAND_E:
            dxChTtiActionPtr->mplsCommand = CPSS_DXCH_TTI_MPLS_POP_AND_SWAP_CMD_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert redirectCommand into device specific format */
    switch (actionPtr->redirectCommand)
    {
        case PRV_TGF_TTI_NO_REDIRECT_E:
            dxChTtiActionPtr->redirectCommand = CPSS_DXCH_TTI_NO_REDIRECT_E;
            break;

        case PRV_TGF_TTI_REDIRECT_TO_EGRESS_E:
            dxChTtiActionPtr->redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E;
            break;

        case PRV_TGF_TTI_REDIRECT_TO_ROUTER_LOOKUP_E:
            dxChTtiActionPtr->redirectCommand = CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E;
            break;

        case PRV_TGF_TTI_VRF_ID_ASSIGN_E:
            dxChTtiActionPtr->redirectCommand = CPSS_DXCH_TTI_VRF_ID_ASSIGN_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tag0VlanCmd into device specific format */
    switch (actionPtr->tag0VlanCmd)
    {
        case PRV_TGF_TTI_VLAN_DO_NOT_MODIFY_E:
            dxChTtiActionPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E;
            break;

        case PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E:
            dxChTtiActionPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
            break;

        case PRV_TGF_TTI_VLAN_MODIFY_TAGGED_E:
            dxChTtiActionPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E;
            break;

        case PRV_TGF_TTI_VLAN_MODIFY_ALL_E:
            dxChTtiActionPtr->tag0VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tag1VlanCmd into device specific format */
    switch (actionPtr->tag1VlanCmd)
    {
        case PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E:
            dxChTtiActionPtr->tag1VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E;
            break;

        case PRV_TGF_TTI_VLAN_MODIFY_ALL_E:
            dxChTtiActionPtr->tag1VlanCmd = CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert modifyTag0Up into device specific format */
    switch (actionPtr->modifyUpEnable)
    {
        case PRV_TGF_TTI_DO_NOT_MODIFY_PREV_UP_E:
            dxChTtiActionPtr->modifyTag0Up = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E;
            break;

        case PRV_TGF_TTI_MODIFY_UP_ENABLE_E:
            dxChTtiActionPtr->modifyTag0Up = CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E;
            break;

        case PRV_TGF_TTI_MODIFY_UP_DISABLE_E:
            dxChTtiActionPtr->modifyTag0Up = CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E;
            break;

        case PRV_TGF_TTI_MODIFY_UP_RESERVED_E:
            dxChTtiActionPtr->modifyTag0Up = CPSS_DXCH_TTI_MODIFY_UP_RESERVED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tag1UpCommand into device specific format */
    switch (actionPtr->tag1UpCommand)
    {
        case PRV_TGF_TTI_TAG1_UP_ASSIGN_VLAN1_UNTAGGED_E:
            dxChTtiActionPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN1_UNTAGGED_E;
            break;

        case PRV_TGF_TTI_TAG1_UP_ASSIGN_VLAN0_UNTAGGED_E:
            dxChTtiActionPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN0_UNTAGGED_E;
            break;

        case PRV_TGF_TTI_TAG1_UP_ASSIGN_ALL_E:
            dxChTtiActionPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert modifyDscp into device specific format */
    switch (actionPtr->modifyDscpEnable)
    {
        case PRV_TGF_TTI_DO_NOT_MODIFY_PREV_DSCP_E:
            dxChTtiActionPtr->modifyDscp = CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E;
            break;

        case PRV_TGF_TTI_MODIFY_DSCP_ENABLE_E:
            dxChTtiActionPtr->modifyDscp = CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E;
            break;

        case PRV_TGF_TTI_MODIFY_DSCP_DISABLE_E:
            dxChTtiActionPtr->modifyDscp = CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E;
            break;

        case PRV_TGF_TTI_MODIFY_DSCP_RESERVED_E:
            dxChTtiActionPtr->modifyDscp = CPSS_DXCH_TTI_MODIFY_DSCP_RESERVED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert pcl0OverrideConfigIndex into device specific format */
    switch (actionPtr->pcl0OverrideConfigIndex)
    {
        case PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E:
            dxChTtiActionPtr->pcl0OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
            break;

        case PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E:
            dxChTtiActionPtr->pcl0OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert pcl0_1OverrideConfigIndex into device specific format */
    switch (actionPtr->pcl0_1OverrideConfigIndex)
    {
        case PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E:
            dxChTtiActionPtr->pcl0_1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
            break;

        case PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E:
            dxChTtiActionPtr->pcl0_1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert pcl1OverrideConfigIndex into device specific format */
    switch (actionPtr->pcl1OverrideConfigIndex)
    {
        case PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E:
            dxChTtiActionPtr->pcl1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
            break;

        case PRV_TGF_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E:
            dxChTtiActionPtr->pcl1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericTtiAction
*
* DESCRIPTION:
*       Convert device specific TTI action type1 into generic TTI action
*
* INPUTS:
*       dxChTtiActionPtr - (pointer to) DxCh TTI rule action
*
* OUTPUTS:
*       actionPtr - (pointer to) TTI rule action
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxChToGenericTtiAction
(
    IN  CPSS_DXCH_TTI_ACTION_STC     *dxChTtiActionPtr,
    OUT PRV_TGF_TTI_ACTION_STC       *actionPtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) actionPtr, 0, sizeof(*actionPtr));

    /* convert passengerPacketType from device specific format */
    switch (dxChTtiActionPtr->passengerPacketType)
    {
        case CPSS_DXCH_TTI_PASSENGER_IPV4_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_IPV4_E;
            break;

        case CPSS_DXCH_TTI_PASSENGER_IPV6_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_IPV6_E;
            break;

        case CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_ETHERNET_CRC_E;
            break;

        case CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_ETHERNET_NO_CRC_E;
            break;

        case CPSS_DXCH_TTI_PASSENGER_IPV4V6_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_IPV4V6_E;
            break;

        case CPSS_DXCH_TTI_PASSENGER_MPLS_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_MPLS_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert redirectCommand from device specific format */
    switch (dxChTtiActionPtr->redirectCommand)
    {
        case CPSS_DXCH_TTI_NO_REDIRECT_E:
            actionPtr->redirectCommand = PRV_TGF_TTI_NO_REDIRECT_E;
            break;

        case CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E:
            actionPtr->redirectCommand = PRV_TGF_TTI_REDIRECT_TO_EGRESS_E;
            break;

        case CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E:
            actionPtr->redirectCommand = PRV_TGF_TTI_REDIRECT_TO_ROUTER_LOOKUP_E;
            break;

        case CPSS_DXCH_TTI_VRF_ID_ASSIGN_E:
            actionPtr->redirectCommand = PRV_TGF_TTI_VRF_ID_ASSIGN_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert vlanCmd from device specific format */
    switch (dxChTtiActionPtr->vlanCmd)
    {
        case CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E:
            actionPtr->vlanCmd = PRV_TGF_TTI_VLAN_DO_NOT_MODIFY_E;
            break;

        case CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E:
            actionPtr->vlanCmd = PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E;
            break;

        case CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E:
            actionPtr->vlanCmd = PRV_TGF_TTI_VLAN_MODIFY_TAGGED_E;
            break;

        case CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E:
            actionPtr->vlanCmd = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert qosTrustMode from device specific format */
    switch (dxChTtiActionPtr->qosTrustMode)
    {
        case CPSS_DXCH_TTI_QOS_KEEP_PREVIOUS_E:
            actionPtr->qosTrustMode = PRV_TGF_TTI_QOS_KEEP_PREVIOUS_E;
            break;

        case CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_E:
            actionPtr->qosTrustMode = PRV_TGF_TTI_QOS_TRUST_PASS_L2_E;
            break;

        case CPSS_DXCH_TTI_QOS_TRUST_PASS_L3_E:
            actionPtr->qosTrustMode = PRV_TGF_TTI_QOS_TRUST_PASS_L3_E;
            break;

        case CPSS_DXCH_TTI_QOS_TRUST_PASS_L2_L3_E:
            actionPtr->qosTrustMode = PRV_TGF_TTI_QOS_TRUST_PASS_L2_L3_E;
            break;

        case CPSS_DXCH_TTI_QOS_UNTRUST_E:
            actionPtr->qosTrustMode = PRV_TGF_TTI_QOS_UNTRUST_E;
            break;

        case CPSS_DXCH_TTI_QOS_TRUST_MPLS_EXP_E:
            actionPtr->qosTrustMode = PRV_TGF_TTI_QOS_TRUST_MPLS_EXP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert modifyUpEnable from device specific format */
    switch (dxChTtiActionPtr->modifyUpEnable)
    {
        case CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E:
            actionPtr->modifyUpEnable = PRV_TGF_TTI_DO_NOT_MODIFY_PREV_UP_E;
            break;

        case CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E:
            actionPtr->modifyUpEnable = PRV_TGF_TTI_MODIFY_UP_ENABLE_E;
            break;

        case CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E:
            actionPtr->modifyUpEnable = PRV_TGF_TTI_MODIFY_UP_DISABLE_E;
            break;

        case CPSS_DXCH_TTI_MODIFY_UP_RESERVED_E:
            actionPtr->modifyUpEnable = PRV_TGF_TTI_MODIFY_UP_RESERVED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert modifyDscpEnable from device specific format */
    switch (dxChTtiActionPtr->modifyDscpEnable)
    {
        case CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E:
            actionPtr->modifyDscpEnable = PRV_TGF_TTI_DO_NOT_MODIFY_PREV_DSCP_E;
            break;

        case CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E:
            actionPtr->modifyDscpEnable = PRV_TGF_TTI_MODIFY_DSCP_ENABLE_E;
            break;

        case CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E:
            actionPtr->modifyDscpEnable = PRV_TGF_TTI_MODIFY_DSCP_DISABLE_E;
            break;

        case CPSS_DXCH_TTI_MODIFY_DSCP_RESERVED_E:
            actionPtr->modifyDscpEnable = PRV_TGF_TTI_MODIFY_DSCP_RESERVED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tti action from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, command);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, userDefinedCpuCode);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tunnelTerminate);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, copyTtlFromTunnelHeader);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, mirrorToIngressAnalyzerEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, policerIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, vlanPrecedence);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, vlanId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, nestedVlanEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tunnelStart);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tunnelStartPtr);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, routerLookupPtr);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, vrfId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, useVidx);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, sourceIdSetEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, sourceId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, bindToPolicer);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, qosPrecedence);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, qosProfile);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, up);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, remapDSCP);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, vntl2Echo);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, bridgeBypass);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, actionStop);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, activateCounter);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, counterIndex);

    /* set interface info */
    cpssOsMemCpy(&(actionPtr->interfaceInfo), &(dxChTtiActionPtr->egressInterface),
                 sizeof(dxChTtiActionPtr->egressInterface));

    if( CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E == dxChTtiActionPtr->redirectCommand &&
       CPSS_INTERFACE_PORT_E == dxChTtiActionPtr->egressInterface.type )
    {
        return prvUtfSwFromHwDeviceNumberGet(dxChTtiActionPtr->egressInterface.devPort.devNum,
                                             &(actionPtr->interfaceInfo.devPort.devNum));
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericTtiAction2
*
* DESCRIPTION:
*       Convert device specific TTI action type2 into generic TTI action
*
* INPUTS:
*       dxChTtiActionPtr - (pointer to) DxCh TTI rule action
*
* OUTPUTS:
*       actionPtr - (pointer to) TTI rule action
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxChToGenericTtiAction2
(
    IN  CPSS_DXCH_TTI_ACTION_2_STC   *dxChTtiActionPtr,
    OUT PRV_TGF_TTI_ACTION_STC       *actionPtr
)
{
    GT_STATUS rc;

    /* clear tti action */
    cpssOsMemSet((GT_VOID*) actionPtr, 0, sizeof(PRV_TGF_TTI_ACTION_STC));

    /* convert tti action from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tunnelTerminate);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, copyTtlFromTunnelHeader);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, command);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, arpPtr);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tunnelStart);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tunnelStartPtr);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, vrfId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, sourceIdSetEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, sourceId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tag0VlanId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tag1VlanId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, nestedVlanEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, bindToPolicer);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, policerIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, qosPrecedence);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, keepPreviousQoS);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, trustUp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, trustDscp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, trustExp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, qosProfile);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tag0Up);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, tag1Up);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, remapDSCP);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, iPclConfigIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, mirrorToIngressAnalyzerEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, userDefinedCpuCode);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, bindToCentralCounter);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, centralCounterIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, vntl2Echo);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, bridgeBypass);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, ingressPipeBypass);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChTtiActionPtr, actionPtr, actionStop);

    /* convert tti action from device specific format */
    actionPtr->ttl             = dxChTtiActionPtr->mplsTtl;
    actionPtr->enableDecTtl    = dxChTtiActionPtr->enableDecrementTtl;
    actionPtr->routerLookupPtr = dxChTtiActionPtr->routerLttPtr;
    actionPtr->vlanPrecedence  = dxChTtiActionPtr->tag0VlanPrecedence;
    actionPtr->meterEnable     = dxChTtiActionPtr->bindToPolicerMeter;

    /* set interface info */
    cpssOsMemCpy(&(actionPtr->interfaceInfo),
                 &(dxChTtiActionPtr->egressInterface),
                 sizeof(actionPtr->interfaceInfo));

    if( CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E == dxChTtiActionPtr->redirectCommand &&
        CPSS_INTERFACE_PORT_E == dxChTtiActionPtr->egressInterface.type )
    {
        rc = prvUtfSwFromHwDeviceNumberGet(dxChTtiActionPtr->egressInterface.devPort.devNum,
                                       &(actionPtr->interfaceInfo.devPort.devNum));
        if(GT_OK != rc)
            return rc;
    }

    /* convert ttPassengerPacketType from device specific format */
    switch (dxChTtiActionPtr->ttPassengerPacketType)
    {
        case CPSS_DXCH_TTI_PASSENGER_IPV4_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_IPV4_E;
            break;

        case CPSS_DXCH_TTI_PASSENGER_IPV6_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_IPV6_E;
            break;

        case CPSS_DXCH_TTI_PASSENGER_ETHERNET_CRC_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_ETHERNET_CRC_E;
            break;

        case CPSS_DXCH_TTI_PASSENGER_ETHERNET_NO_CRC_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_ETHERNET_NO_CRC_E;
            break;

        case CPSS_DXCH_TTI_PASSENGER_IPV4V6_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_IPV4V6_E;
            break;

        case CPSS_DXCH_TTI_PASSENGER_MPLS_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_MPLS_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tsPassengerPacketType from device specific format */
    switch (dxChTtiActionPtr->tsPassengerPacketType)
    {
        case CPSS_DXCH_TUNNEL_PASSENGER_ETHERNET_E:
            actionPtr->tsPassengerPacketType = PRV_TGF_TUNNEL_PASSENGER_ETHERNET_E;
            break;

        case CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E:
            actionPtr->tsPassengerPacketType = PRV_TGF_TUNNEL_PASSENGER_OTHER_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert mplsCommand from device specific format */
    switch (dxChTtiActionPtr->mplsCommand)
    {
        case CPSS_DXCH_TTI_MPLS_NOP_CMD_E:
            actionPtr->mplsCmd = PRV_TGF_TTI_MPLS_NOP_COMMAND_E;
            break;

        case CPSS_DXCH_TTI_MPLS_SWAP_CMD_E:
            actionPtr->mplsCmd = PRV_TGF_TTI_MPLS_SWAP_COMMAND_E;
            break;

        case CPSS_DXCH_TTI_MPLS_PUSH1_CMD_E:
            actionPtr->mplsCmd = PRV_TGF_TTI_MPLS_PUSH1_LABEL_COMMAND_E;
            break;

        case CPSS_DXCH_TTI_MPLS_POP1_CMD_E:
            actionPtr->mplsCmd = PRV_TGF_TTI_MPLS_POP1_LABEL_COMMAND_E;
            break;

        case CPSS_DXCH_TTI_MPLS_POP2_CMD_E:
            actionPtr->mplsCmd = PRV_TGF_TTI_MPLS_POP2_LABELS_COMMAND_E;
            break;

        case CPSS_DXCH_TTI_MPLS_POP_AND_SWAP_CMD_E:
            actionPtr->mplsCmd = PRV_TGF_TTI_MPLS_POP_AND_SWAP_COMMAND_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert redirectCommand from device specific format */
    switch (dxChTtiActionPtr->redirectCommand)
    {
        case CPSS_DXCH_TTI_NO_REDIRECT_E:
            actionPtr->redirectCommand = PRV_TGF_TTI_NO_REDIRECT_E;
            break;

        case CPSS_DXCH_TTI_REDIRECT_TO_EGRESS_E:
            actionPtr->redirectCommand = PRV_TGF_TTI_REDIRECT_TO_EGRESS_E;
            break;

        case CPSS_DXCH_TTI_REDIRECT_TO_ROUTER_LOOKUP_E:
            actionPtr->redirectCommand = PRV_TGF_TTI_REDIRECT_TO_ROUTER_LOOKUP_E;
            break;

        case CPSS_DXCH_TTI_VRF_ID_ASSIGN_E:
            actionPtr->redirectCommand = PRV_TGF_TTI_VRF_ID_ASSIGN_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tag0VlanCmd from device specific format */
    switch (dxChTtiActionPtr->tag0VlanCmd)
    {
        case CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E:
            actionPtr->tag0VlanCmd = PRV_TGF_TTI_VLAN_DO_NOT_MODIFY_E;
            break;

        case CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E:
            actionPtr->tag0VlanCmd = PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E;
            break;

        case CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E:
            actionPtr->tag0VlanCmd = PRV_TGF_TTI_VLAN_MODIFY_TAGGED_E;
            break;

        case CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E:
            actionPtr->tag0VlanCmd = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tag1VlanCmd from device specific format */
    switch (dxChTtiActionPtr->tag1VlanCmd)
    {
        case CPSS_DXCH_TTI_VLAN_DO_NOT_MODIFY_E:
            actionPtr->tag1VlanCmd = PRV_TGF_TTI_VLAN_DO_NOT_MODIFY_E;
            break;

        case CPSS_DXCH_TTI_VLAN_MODIFY_UNTAGGED_E:
            actionPtr->tag1VlanCmd = PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E;
            break;

        case CPSS_DXCH_TTI_VLAN_MODIFY_TAGGED_E:
            actionPtr->tag1VlanCmd = PRV_TGF_TTI_VLAN_MODIFY_TAGGED_E;
            break;

        case CPSS_DXCH_TTI_VLAN_MODIFY_ALL_E:
            actionPtr->tag1VlanCmd = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert modifyTag0Up from device specific format */
    switch (dxChTtiActionPtr->modifyTag0Up)
    {
        case CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_UP_E:
            actionPtr->modifyUpEnable = PRV_TGF_TTI_DO_NOT_MODIFY_PREV_UP_E;
            break;

        case CPSS_DXCH_TTI_MODIFY_UP_ENABLE_E:
            actionPtr->modifyUpEnable = PRV_TGF_TTI_MODIFY_UP_ENABLE_E;
            break;

        case CPSS_DXCH_TTI_MODIFY_UP_DISABLE_E:
            actionPtr->modifyUpEnable = PRV_TGF_TTI_MODIFY_UP_DISABLE_E;
            break;

        case CPSS_DXCH_TTI_MODIFY_UP_RESERVED_E:
            actionPtr->modifyUpEnable = PRV_TGF_TTI_MODIFY_UP_RESERVED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tag1UpCommand from device specific format */
    switch (dxChTtiActionPtr->tag1UpCommand)
    {
        case CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN1_UNTAGGED_E:
            actionPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN1_UNTAGGED_E;
            break;

        case CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN0_UNTAGGED_E:
            actionPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_VLAN0_UNTAGGED_E;
            break;

        case CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E:
            actionPtr->tag1UpCommand = CPSS_DXCH_TTI_TAG1_UP_ASSIGN_ALL_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert modifyDscp from device specific format */
    switch (dxChTtiActionPtr->modifyDscp)
    {
        case CPSS_DXCH_TTI_DO_NOT_MODIFY_PREV_DSCP_E:
            actionPtr->modifyDscpEnable = PRV_TGF_TTI_DO_NOT_MODIFY_PREV_DSCP_E;
            break;

        case CPSS_DXCH_TTI_MODIFY_DSCP_ENABLE_E:
            actionPtr->modifyDscpEnable = PRV_TGF_TTI_MODIFY_DSCP_ENABLE_E;
            break;

        case CPSS_DXCH_TTI_MODIFY_DSCP_DISABLE_E:
            actionPtr->modifyDscpEnable = PRV_TGF_TTI_MODIFY_DSCP_DISABLE_E;
            break;

        case CPSS_DXCH_TTI_MODIFY_DSCP_RESERVED_E:
            actionPtr->modifyDscpEnable = PRV_TGF_TTI_MODIFY_DSCP_RESERVED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert pcl0OverrideConfigIndex from device specific format */
    switch (dxChTtiActionPtr->pcl0OverrideConfigIndex)
    {
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E:
            actionPtr->pcl0OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
            break;

        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E:
            actionPtr->pcl0OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert pcl0_1OverrideConfigIndex from device specific format */
    switch (dxChTtiActionPtr->pcl0_1OverrideConfigIndex)
    {
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E:
            actionPtr->pcl0_1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
            break;

        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E:
            actionPtr->pcl0_1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert pcl1OverrideConfigIndex from device specific format */
    switch (dxChTtiActionPtr->pcl1OverrideConfigIndex)
    {
        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E:
            actionPtr->pcl1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;
            break;

        case CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E:
            actionPtr->pcl1OverrideConfigIndex = CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChIpv4TtiRule
*
* DESCRIPTION:
*       Convert generic IPv4 TTI rule into device specific IPv4 TTI rule
*
* INPUTS:
*       ipv4TtiRulePtr - (pointer to) IPv4 TTI rule
*
* OUTPUTS:
*       dxChIpv4TtiRulePtr - (pointer to) DxCh IPv4 TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertGenericToDxChIpv4TtiRule
(
    IN  PRV_TGF_TTI_IPV4_RULE_STC    *ipv4TtiRulePtr,
    OUT CPSS_DXCH_TTI_IPV4_RULE_STC  *dxChIpv4TtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) dxChIpv4TtiRulePtr, 0, sizeof(*dxChIpv4TtiRulePtr));

    /* convert IPv4 tti rule into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), isTagged);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpv4TtiRulePtr, ipv4TtiRulePtr, tunneltype);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpv4TtiRulePtr, ipv4TtiRulePtr, isArp);

    /* set MAC */
    cpssOsMemCpy(dxChIpv4TtiRulePtr->common.mac.arEther,
                 ipv4TtiRulePtr->common.mac.arEther,
                 sizeof(ipv4TtiRulePtr->common.mac.arEther));

    /* set source IP */
    cpssOsMemCpy(dxChIpv4TtiRulePtr->srcIp.arIP,
                 ipv4TtiRulePtr->srcIp.arIP,
                 sizeof(ipv4TtiRulePtr->srcIp.arIP));

    /* set destination IP */
    cpssOsMemCpy(dxChIpv4TtiRulePtr->destIp.arIP,
                 ipv4TtiRulePtr->destIp.arIP,
                 sizeof(ipv4TtiRulePtr->destIp.arIP));
}

/*******************************************************************************
* prvTgfConvertDxChToGenericIpv4TtiRule
*
* DESCRIPTION:
*       Convert device specific IPv4 TTI rule into generic IPv4 TTI rule
*
* INPUTS:
*       dxChIpv4TtiRulePtr - (pointer to) DxCh IPv4 TTI rule
*
* OUTPUTS:
*       ipv4TtiRulePtr - (pointer to) IPv4 TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertDxChToGenericIpv4TtiRule
(
    IN  CPSS_DXCH_TTI_IPV4_RULE_STC  *dxChIpv4TtiRulePtr,
    OUT PRV_TGF_TTI_IPV4_RULE_STC    *ipv4TtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) ipv4TtiRulePtr, 0, sizeof(*ipv4TtiRulePtr));

    /* convert IPv4 tti rule from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), pclId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), vid);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), isTagged);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpv4TtiRulePtr, ipv4TtiRulePtr, tunneltype);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpv4TtiRulePtr, ipv4TtiRulePtr, isArp);

    /* set MAC */
    cpssOsMemCpy(ipv4TtiRulePtr->common.mac.arEther,
                 dxChIpv4TtiRulePtr->common.mac.arEther,
                 sizeof(dxChIpv4TtiRulePtr->common.mac.arEther));

    /* set source IP */
    cpssOsMemCpy(ipv4TtiRulePtr->srcIp.arIP,
                 dxChIpv4TtiRulePtr->srcIp.arIP,
                 sizeof(dxChIpv4TtiRulePtr->srcIp.arIP));

    /* set destination IP */
    cpssOsMemCpy(ipv4TtiRulePtr->destIp.arIP,
                 dxChIpv4TtiRulePtr->destIp.arIP,
                 sizeof(dxChIpv4TtiRulePtr->destIp.arIP));
}

/*******************************************************************************
* prvTgfConvertGenericToDxChMplsTtiRule
*
* DESCRIPTION:
*       Convert generic Mpls TTI rule into device specific Mpls TTI rule
*
* INPUTS:
*       MplsTtiRulePtr - (pointer to) Mpls TTI rule
*
* OUTPUTS:
*       dxChMplsTtiRulePtr - (pointer to) DxCh Mpls TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertGenericToDxChMplsTtiRule
(
    IN  PRV_TGF_TTI_MPLS_RULE_STC    *mplsTtiRulePtr,
    OUT CPSS_DXCH_TTI_MPLS_RULE_STC  *dxChMplsTtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) dxChMplsTtiRulePtr, 0, sizeof(*dxChMplsTtiRulePtr));

    /* convert Mpls tti rule into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), isTagged);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, label0);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, exp0);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, label1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, exp1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, label2);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, exp2);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, numOfLabels);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, protocolAboveMPLS);

    /* set MAC */
    cpssOsMemCpy(dxChMplsTtiRulePtr->common.mac.arEther,
                 mplsTtiRulePtr->common.mac.arEther,
                 sizeof(mplsTtiRulePtr->common.mac.arEther));
}

/*******************************************************************************
* prvTgfConvertDxChToGenericMplsTtiRule
*
* DESCRIPTION:
*       Convert device specific Mpls TTI rule into generic Mpls TTI rule
*
* INPUTS:
*       dxChMplsTtiRulePtr - (pointer to) DxCh Mpls TTI rule
*
* OUTPUTS:
*       MplsTtiRulePtr - (pointer to) Mpls TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertDxChToGenericMplsTtiRule
(
    IN  CPSS_DXCH_TTI_MPLS_RULE_STC  *dxChMplsTtiRulePtr,
    OUT PRV_TGF_TTI_MPLS_RULE_STC    *mplsTtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) mplsTtiRulePtr, 0, sizeof(*mplsTtiRulePtr));

    /* convert Mpls tti rule from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), pclId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), vid);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), isTagged);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, label0);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, exp0);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, label1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, exp1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, label2);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, exp2);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, numOfLabels);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsTtiRulePtr, mplsTtiRulePtr, protocolAboveMPLS);

    /* set MAC */
    cpssOsMemCpy(mplsTtiRulePtr->common.mac.arEther,
                 dxChMplsTtiRulePtr->common.mac.arEther,
                 sizeof(dxChMplsTtiRulePtr->common.mac.arEther));
}

/*******************************************************************************
* prvTgfConvertGenericToDxChEthTtiRule
*
* DESCRIPTION:
*       Convert generic Ethernet TTI rule into device specific Ethernet TTI rule
*
* INPUTS:
*       ethTtiRulePtr - (pointer to) Ethernet TTI rule
*
* OUTPUTS:
*       dxChEthTtiRulePtr - (pointer to) DxCh Ethernet TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertGenericToDxChEthTtiRule
(
    IN  PRV_TGF_TTI_ETH_RULE_STC     *ethTtiRulePtr,
    OUT CPSS_DXCH_TTI_ETH_RULE_STC   *dxChEthTtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) dxChEthTtiRulePtr, 0, sizeof(*dxChEthTtiRulePtr));

    /* convert IPv4 tti rule into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), isTagged);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, up0);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, cfi0);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, isVlan1Exists);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, vid1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, up1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, cfi1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, etherType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, macToMe);

    /* set MAC */
    cpssOsMemCpy(dxChEthTtiRulePtr->common.mac.arEther,
                 ethTtiRulePtr->common.mac.arEther,
                 sizeof(ethTtiRulePtr->common.mac.arEther));
}

/*******************************************************************************
* prvTgfConvertDxChToGenericEthTtiRule
*
* DESCRIPTION:
*       Convert device specific Ethernet TTI rule into generic Ethernet TTI rule
*
* INPUTS:
*       dxChEthTtiRulePtr - (pointer to) DxCh Ethernet TTI rule
*
* OUTPUTS:
*       ethTtiRulePtr - (pointer to) Ethernet TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertDxChToGenericEthTtiRule
(
    IN  CPSS_DXCH_TTI_ETH_RULE_STC   *dxChEthTtiRulePtr,
    OUT PRV_TGF_TTI_ETH_RULE_STC     *ethTtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) ethTtiRulePtr, 0, sizeof(*ethTtiRulePtr));

    /* convert IPv4 tti rule from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), pclId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), vid);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), isTagged);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChEthTtiRulePtr->common), &(ethTtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, up0);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, cfi0);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, isVlan1Exists);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, vid1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, up1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, cfi1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, etherType);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChEthTtiRulePtr, ethTtiRulePtr, macToMe);

    /* set MAC */
    cpssOsMemCpy(ethTtiRulePtr->common.mac.arEther,
                 dxChEthTtiRulePtr->common.mac.arEther,
                 sizeof(dxChEthTtiRulePtr->common.mac.arEther));
}

/*******************************************************************************
* prvTgfConvertGenericToDxChMimTtiRule
*
* DESCRIPTION:
*       Convert generic MIM TTI rule into device specific MIM TTI rule
*
* INPUTS:
*       mimTtiRulePtr - (pointer to) MIM TTI rule
*
* OUTPUTS:
*       dxChMimTtiRulePtr - (pointer to) DxCh MIM TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertGenericToDxChMimTtiRule
(
    IN  PRV_TGF_TTI_MIM_RULE_STC       *mimTtiRulePtr,
    OUT CPSS_DXCH_TTI_MIM_RULE_STC     *dxChMimTtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) dxChMimTtiRulePtr, 0, sizeof(*dxChMimTtiRulePtr));

    /* convert IPv4 tti rule into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), isTagged);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimTtiRulePtr, mimTtiRulePtr, bUp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimTtiRulePtr, mimTtiRulePtr, bDp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimTtiRulePtr, mimTtiRulePtr, iSid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimTtiRulePtr, mimTtiRulePtr, iUp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimTtiRulePtr, mimTtiRulePtr, iDp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimTtiRulePtr, mimTtiRulePtr, iRes1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimTtiRulePtr, mimTtiRulePtr, iRes2);

    /* set MAC */
    cpssOsMemCpy(dxChMimTtiRulePtr->common.mac.arEther,
                 mimTtiRulePtr->common.mac.arEther,
                 sizeof(dxChMimTtiRulePtr->common.mac.arEther));
}

/*******************************************************************************
* prvTgfConvertDxChToGenericMimTtiRule
*
* DESCRIPTION:
*       Convert device specific MIM TTI rule into generic MIM TTI rule
*
* INPUTS:
*       dxChMimTtiRulePtr - (pointer to) DxCh MIM TTI rule
*
* OUTPUTS:
*       mimTtiRulePtr - (pointer to) MIM TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertDxChToGenericMimTtiRule
(
    IN  CPSS_DXCH_TTI_MIM_RULE_STC     *dxChMimTtiRulePtr,
    OUT PRV_TGF_TTI_MIM_RULE_STC       *mimTtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) mimTtiRulePtr, 0, sizeof(*mimTtiRulePtr));

    /* convert IPv4 tti rule from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), pclId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), vid);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), isTagged);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(dxChMimTtiRulePtr->common), &(mimTtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimTtiRulePtr, mimTtiRulePtr, bUp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimTtiRulePtr, mimTtiRulePtr, bDp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimTtiRulePtr, mimTtiRulePtr, iSid);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimTtiRulePtr, mimTtiRulePtr, iUp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimTtiRulePtr, mimTtiRulePtr, iDp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimTtiRulePtr, mimTtiRulePtr, iRes1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimTtiRulePtr, mimTtiRulePtr, iRes2);

    /* set MAC */
    cpssOsMemCpy(mimTtiRulePtr->common.mac.arEther,
                 dxChMimTtiRulePtr->common.mac.arEther,
                 sizeof(mimTtiRulePtr->common.mac.arEther));
}

/*******************************************************************************
* prvTgfConvertGenericToDxChTunnelStartIpv4Entry
*
* DESCRIPTION:
*       Convert generic tunnel start IPv4 entry into device specific
*
* INPUTS:
*       ipv4EntryPtr - (pointer to) tunnel start IPv4 entry
*
* OUTPUTS:
*       dxChIpv4EntryPtr - (pointer to) DxCh tunnel start IPv4 entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChTunnelStartIpv4Entry
(
    IN  PRV_TGF_TUNNEL_START_IPV4_ENTRY_STC       *ipv4EntryPtr,
    OUT CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC    *dxChIpv4EntryPtr
)
{
    /* clear tunnel start IPv4 entry */
    cpssOsMemSet((GT_VOID*) dxChIpv4EntryPtr, 0, sizeof(*dxChIpv4EntryPtr));

    /* convert upMarkMode into device specific format */
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(dxChIpv4EntryPtr->upMarkMode, ipv4EntryPtr->upMarkMode);

    /* convert dscpMarkMode into device specific format */
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(dxChIpv4EntryPtr->dscpMarkMode, ipv4EntryPtr->dscpMarkMode);

    /* convert tunnel start IPv4 entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, tagEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, vlanId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, dscp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, macDa);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, dontFragmentFlag);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, ttl);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, autoTunnel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, autoTunnelOffset);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, destIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, srcIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, cfi);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericTunnelStartIpv4Entry
*
* DESCRIPTION:
*       Convert device specific tunnel start IPv4 entry into generic
*
* INPUTS:
*       dxChIpv4EntryPtr - (pointer to) DxCh tunnel start IPv4 entry
*
* OUTPUTS:
*       ipv4EntryPtr - (pointer to) tunnel start IPv4 entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxChToGenericTunnelStartIpv4Entry
(
    IN  CPSS_DXCH_TUNNEL_START_IPV4_CONFIG_STC    *dxChIpv4EntryPtr,
    OUT PRV_TGF_TUNNEL_START_IPV4_ENTRY_STC       *ipv4EntryPtr
)
{
    /* clear tunnel start IPv4 entry */
    cpssOsMemSet((GT_VOID*) ipv4EntryPtr, 0, sizeof(*ipv4EntryPtr));

    /* convert upMarkMode from device specific format */
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(dxChIpv4EntryPtr->upMarkMode, ipv4EntryPtr->upMarkMode);

    /* convert dscpMarkMode from device specific format */
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(dxChIpv4EntryPtr->dscpMarkMode, ipv4EntryPtr->dscpMarkMode);

    /* convert tunnel start IPv4 entry from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, tagEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, vlanId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, up);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, dscp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, macDa);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, dontFragmentFlag);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, ttl);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, autoTunnel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, autoTunnelOffset);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, destIp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, srcIp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChIpv4EntryPtr, ipv4EntryPtr, cfi);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChTunnelStartMplsEntry
*
* DESCRIPTION:
*       Convert generic tunnel start Mpls entry into device specific
*
* INPUTS:
*       mplsEntryPtr - (pointer to) tunnel start Mpls entry
*
* OUTPUTS:
*       dxChMplsEntryPtr - (pointer to) DxCh tunnel start Mpls entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChTunnelStartMplsEntry
(
    IN  PRV_TGF_TUNNEL_START_MPLS_ENTRY_STC       *mplsEntryPtr,
    OUT CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC    *dxChMplsEntryPtr
)
{
    /* clear tunnel start Mpls entry */
    cpssOsMemSet((GT_VOID*) dxChMplsEntryPtr, 0, sizeof(*mplsEntryPtr));

    /* convert MarkMode into device specific format */
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(dxChMplsEntryPtr->upMarkMode,   mplsEntryPtr->upMarkMode);
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(dxChMplsEntryPtr->exp1MarkMode, mplsEntryPtr->exp1MarkMode);
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(dxChMplsEntryPtr->exp2MarkMode, mplsEntryPtr->exp2MarkMode);
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(dxChMplsEntryPtr->exp3MarkMode, mplsEntryPtr->exp3MarkMode);

    /* convert ttlMode into device specific format */
    switch (mplsEntryPtr->ttlMode)
    {
        case PRV_TGF_TUNNEL_START_TTL_TO_INCOMING_TTL_E:
            dxChMplsEntryPtr->ttlMode = CPSS_DXCH_TUNNEL_START_TTL_TO_INCOMING_TTL_E;
            break;

        case PRV_TGF_TUNNEL_START_TTL_TO_SWAP_LABEL_E:
            dxChMplsEntryPtr->ttlMode = CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_E;
            break;

        case PRV_TGF_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E:
            dxChMplsEntryPtr->ttlMode = CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tunnel start Mpls entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, tagEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, vlanId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, macDa);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, numLabels);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, ttl);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, label1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, exp1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, label2);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, exp2);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, label3);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, exp3);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, retainCRC);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, setSBit);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, cfi);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericTunnelStartMplsEntry
*
* DESCRIPTION:
*       Convert device specific tunnel start Mpls entry into generic
*
* INPUTS:
*       dxChMplsEntryPtr - (pointer to) DxCh tunnel start Mpls entry
*
* OUTPUTS:
*       mplsEntryPtr - (pointer to) tunnel start Mpls entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxChToGenericTunnelStartMplsEntry
(
    IN  CPSS_DXCH_TUNNEL_START_MPLS_CONFIG_STC    *dxChMplsEntryPtr,
    OUT PRV_TGF_TUNNEL_START_MPLS_ENTRY_STC       *mplsEntryPtr
)
{
    /* clear tunnel start Mpls entry */
    cpssOsMemSet((GT_VOID*) mplsEntryPtr, 0, sizeof(*mplsEntryPtr));

    /* convert MarkMode from device specific format */
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(dxChMplsEntryPtr->upMarkMode,   mplsEntryPtr->upMarkMode);
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(dxChMplsEntryPtr->exp1MarkMode, mplsEntryPtr->exp1MarkMode);
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(dxChMplsEntryPtr->exp2MarkMode, mplsEntryPtr->exp2MarkMode);
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(dxChMplsEntryPtr->exp3MarkMode, mplsEntryPtr->exp3MarkMode);

    /* convert ttlMode from device specific format */
    switch (dxChMplsEntryPtr->ttlMode)
    {
        case CPSS_DXCH_TUNNEL_START_TTL_TO_INCOMING_TTL_E:
            mplsEntryPtr->ttlMode = PRV_TGF_TUNNEL_START_TTL_TO_INCOMING_TTL_E;
            break;

        case CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_E:
            mplsEntryPtr->ttlMode = PRV_TGF_TUNNEL_START_TTL_TO_SWAP_LABEL_E;
            break;

        case CPSS_DXCH_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E:
            mplsEntryPtr->ttlMode = PRV_TGF_TUNNEL_START_TTL_TO_SWAP_LABEL_MINUS_ONE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tunnel start Mpls entry from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, tagEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, vlanId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, up);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, macDa);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, numLabels);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, ttl);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, label1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, exp1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, label2);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, exp2);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, label3);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, exp3);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, retainCRC);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, setSBit);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMplsEntryPtr, mplsEntryPtr, cfi);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChTunnelStartMimEntry
*
* DESCRIPTION:
*       Convert generic tunnel start Mim entry into device specific
*
* INPUTS:
*       mimEntryPtr - (pointer to) tunnel start Mim entry
*
* OUTPUTS:
*       dxChMimEntryPtr - (pointer to) DxCh tunnel start Mim entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChTunnelStartMimEntry
(
    IN  PRV_TGF_TUNNEL_START_MIM_ENTRY_STC      *mimEntryPtr,
    OUT CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC   *dxChMimEntryPtr
)
{
    /* clear tunnel start Mim entry */
    cpssOsMemSet((GT_VOID*) dxChMimEntryPtr, 0, sizeof(*mimEntryPtr));

    /* convert upMarkMode into device specific format */
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(dxChMimEntryPtr->upMarkMode, mimEntryPtr->upMarkMode);

    /* convert iUpMarkMode into device specific format */
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(dxChMimEntryPtr->iUpMarkMode, mimEntryPtr->iUpMarkMode);

    /* convert iDpMarkMode into device specific format */
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(dxChMimEntryPtr->iDpMarkMode, mimEntryPtr->iDpMarkMode);

    /* convert tunnel start Mim entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, tagEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, vlanId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, macDa);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, retainCrc);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, iSid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, iUp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, iDp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, iTagReserved);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericTunnelStartMimEntry
*
* DESCRIPTION:
*       Convert device specific tunnel start Mim entry into generic
*
* INPUTS:
*       dxChMimEntryPtr - (pointer to) DxCh tunnel start Mim entry
*
* OUTPUTS:
*       mimEntryPtr - (pointer to) tunnel start Mim entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxChToGenericTunnelStartMimEntry
(
    IN  CPSS_DXCH_TUNNEL_START_MIM_CONFIG_STC   *dxChMimEntryPtr,
    OUT PRV_TGF_TUNNEL_START_MIM_ENTRY_STC      *mimEntryPtr
)
{
    /* clear tunnel start Mim entry */
    cpssOsMemSet((GT_VOID*) mimEntryPtr, 0, sizeof(*mimEntryPtr));

    /* convert upMarkMode from device specific format */
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(dxChMimEntryPtr->upMarkMode, mimEntryPtr->upMarkMode);

    /* convert iUpMarkMode from device specific format */
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(dxChMimEntryPtr->iUpMarkMode, mimEntryPtr->iUpMarkMode);

    /* convert iDpMarkMode from device specific format */
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(dxChMimEntryPtr->iDpMarkMode, mimEntryPtr->iDpMarkMode);

    /* convert tunnel start Mim entry from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, tagEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, vlanId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, up);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, macDa);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, retainCrc);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, iSid);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, iUp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, iDp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(dxChMimEntryPtr, mimEntryPtr, iTagReserved);

    return GT_OK;
}
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToExMxPmTtiAction
*
* DESCRIPTION:
*       Convert generic TTI action into device specific TTI action
*
* INPUTS:
*       actionPtr - (pointer to) TTI rule action
*
* OUTPUTS:
*       exMxPmTtiActionPtr - (pointer to) ExMxPm standert TTI rule action
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmTtiAction
(
    IN  PRV_TGF_TTI_ACTION_STC                  *actionPtr,
    OUT CPSS_EXMXPM_TTI_STANDARD_ACTION_STC     *exMxPmTtiActionPtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) exMxPmTtiActionPtr, 0, sizeof(*exMxPmTtiActionPtr));

    /* convert tti action into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, command);

    /* set redirect command */
    switch (actionPtr->redirectCommand)
    {
        case PRV_TGF_TTI_NO_REDIRECT_E:
            exMxPmTtiActionPtr->redirectCommand = CPSS_EXMXPM_TTI_NO_REDIRECT_E;
            break;
        case PRV_TGF_TTI_REDIRECT_TO_OUTLIF_E:
            exMxPmTtiActionPtr->redirectCommand = CPSS_EXMXPM_TTI_REDIRECT_TO_OUTLIF_E;
            break;
        case PRV_TGF_TTI_REDIRECT_TO_NEXT_HOP_E:
            exMxPmTtiActionPtr->redirectCommand = CPSS_EXMXPM_TTI_REDIRECT_TO_NEXT_HOP_E;
            break;
        case PRV_TGF_TTI_TRIGGER_VPLS_LOOKUP_E:
            exMxPmTtiActionPtr->redirectCommand = CPSS_EXMXPM_TTI_TRIGGER_VPLS_LOOKUP_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* set outlif info */
    cpssOsMemCpy((GT_VOID*) &(exMxPmTtiActionPtr->outlif.interfaceInfo),
                 (GT_VOID*) &(actionPtr-> interfaceInfo),
                 sizeof(actionPtr->interfaceInfo));

    exMxPmTtiActionPtr->outlif.outlifType           = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    exMxPmTtiActionPtr->outlif.outlifPointer.arpPtr = actionPtr->arpPtr;

    /* set vlanCmd */
    switch (actionPtr->vlanCmd)
    {
        case PRV_TGF_TTI_VLAN_DO_NOT_MODIFY_E:
            exMxPmTtiActionPtr->inLifVidCmd = CPSS_EXMXPM_TTI_VLAN_INLIF_DO_NOT_MODIFY_E;
            break;

        case PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E:
            exMxPmTtiActionPtr->inLifVidCmd = CPSS_EXMXPM_TTI_VLAN_INLIF_MODIFY_UNTAGGED_E;
            break;

        case PRV_TGF_TTI_VLAN_MODIFY_TAGGED_E:
            exMxPmTtiActionPtr->inLifVidCmd = CPSS_EXMXPM_TTI_VLAN_INLIF_MODIFY_TAGGED_E;
            break;

        case PRV_TGF_TTI_VLAN_MODIFY_ALL_E:
            exMxPmTtiActionPtr->inLifVidCmd = CPSS_EXMXPM_TTI_VLAN_INLIF_MODIFY_ALL_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    exMxPmTtiActionPtr->inLifVidId = actionPtr->vlanId;

    /* convert mplsCmd into device specific format */
    switch (actionPtr->mplsCmd)
    {
        case PRV_TGF_TTI_MPLS_NOP_COMMAND_E:
            exMxPmTtiActionPtr->mplsCmd = CPSS_EXMXPM_TTI_MPLS_NOP_COMMAND_E;
            break;

        case PRV_TGF_TTI_MPLS_SWAP_COMMAND_E:
            exMxPmTtiActionPtr->mplsCmd = CPSS_EXMXPM_TTI_MPLS_SWAP_COMMAND_E;
            break;

        case PRV_TGF_TTI_MPLS_PUSH1_LABEL_COMMAND_E:
            exMxPmTtiActionPtr->mplsCmd = CPSS_EXMXPM_TTI_MPLS_PUSH1_LABEL_COMMAND_E;
            break;

        case PRV_TGF_TTI_MPLS_POP1_LABEL_COMMAND_E:
            exMxPmTtiActionPtr->mplsCmd = CPSS_EXMXPM_TTI_MPLS_POP1_LABEL_COMMAND_E;
            break;

        case PRV_TGF_TTI_MPLS_POP2_LABELS_COMMAND_E:
            exMxPmTtiActionPtr->mplsCmd = CPSS_EXMXPM_TTI_MPLS_POP2_LABELS_COMMAND_E;
            break;

        case PRV_TGF_TTI_MPLS_POP3_LABELS_COMMAND_E:
            exMxPmTtiActionPtr->mplsCmd = CPSS_EXMXPM_TTI_MPLS_POP3_LABELS_COMMAND_E;
            break;

        case PRV_TGF_TTI_MPLS_POP_AND_SWAP_COMMAND_E:
            exMxPmTtiActionPtr->mplsCmd = CPSS_EXMXPM_TTI_MPLS_POP_AND_SWAP_COMMAND_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert passengerPacketType into device specific format */
    switch (actionPtr->passengerPacketType)
    {
        case PRV_TGF_TTI_PASSENGER_IPV4_E:
            exMxPmTtiActionPtr->passengerPacketType = CPSS_EXMXPM_TTI_PASSENGER_IPV4_E;
            break;

        case PRV_TGF_TTI_PASSENGER_IPV6_E:
            exMxPmTtiActionPtr->passengerPacketType = CPSS_EXMXPM_TTI_PASSENGER_IPV6_E;
            break;

        case PRV_TGF_TTI_PASSENGER_ETHERNET_CRC_E:
            exMxPmTtiActionPtr->passengerPacketType = CPSS_EXMXPM_TTI_PASSENGER_ETHERNET_CRC_E;
            break;

        case PRV_TGF_TTI_PASSENGER_ETHERNET_NO_CRC_E:
            exMxPmTtiActionPtr->passengerPacketType = CPSS_EXMXPM_TTI_PASSENGER_ETHERNET_NO_CRC_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tti action into device specific format */
    exMxPmTtiActionPtr->tunnelTerminateEnable = actionPtr->tunnelTerminate;

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, userDefinedCpuCode);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, ipNextHopIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, copyTtlFromTunnelHeader);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, mirrorToIngressAnalyzerEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, policerIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, vlanPrecedence);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, nestedVlanEnable);

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, ipNextHopIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, exp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, setExp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, enableDecTtl);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, mplsLabel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, ttl);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, pwId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, sourceIsPE);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, enableSourceLocalFiltering);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, floodDitPointer);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, counterEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, meterEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, flowId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, sstIdEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, sstId);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertExMxPmToGenericTtiAction
*
* DESCRIPTION:
*       Convert device specific TTI action into generic TTI action
*
* INPUTS:
*       exMxPmTtiActionPtr - (pointer to) ExMxPm standert TTI rule action
*
* OUTPUTS:
*       actionPtr - (pointer to) TTI rule action
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertExMxPmToGenericTtiAction
(
    IN  CPSS_EXMXPM_TTI_STANDARD_ACTION_STC     *exMxPmTtiActionPtr,
    OUT PRV_TGF_TTI_ACTION_STC                  *actionPtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) actionPtr, 0, sizeof(*actionPtr));

    /* convert tti action from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, command);

    /* set redirect command */
    switch (exMxPmTtiActionPtr->redirectCommand)
    {
        case CPSS_EXMXPM_TTI_NO_REDIRECT_E:
            actionPtr->redirectCommand = PRV_TGF_TTI_NO_REDIRECT_E;
            break;
        case CPSS_EXMXPM_TTI_REDIRECT_TO_OUTLIF_E:
            actionPtr->redirectCommand = PRV_TGF_TTI_REDIRECT_TO_OUTLIF_E;
            break;
        case CPSS_EXMXPM_TTI_REDIRECT_TO_NEXT_HOP_E:
            actionPtr->redirectCommand = PRV_TGF_TTI_REDIRECT_TO_NEXT_HOP_E;
            break;
        case CPSS_EXMXPM_TTI_TRIGGER_VPLS_LOOKUP_E:
            actionPtr->redirectCommand = PRV_TGF_TTI_TRIGGER_VPLS_LOOKUP_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* set outlif info */
    cpssOsMemCpy((GT_VOID*) &(actionPtr->interfaceInfo),
                 (GT_VOID*) &(exMxPmTtiActionPtr->outlif.interfaceInfo),
                 sizeof(exMxPmTtiActionPtr->outlif.interfaceInfo));

    actionPtr->arpPtr = exMxPmTtiActionPtr->outlif.outlifPointer.arpPtr;

    /* set vlanCmd */
    switch (exMxPmTtiActionPtr->inLifVidCmd)
    {
        case CPSS_EXMXPM_TTI_VLAN_INLIF_DO_NOT_MODIFY_E:
            actionPtr->vlanCmd = PRV_TGF_TTI_VLAN_DO_NOT_MODIFY_E;
            break;

        case CPSS_EXMXPM_TTI_VLAN_INLIF_MODIFY_UNTAGGED_E:
            actionPtr->vlanCmd = PRV_TGF_TTI_VLAN_MODIFY_UNTAGGED_E;
            break;

        case CPSS_EXMXPM_TTI_VLAN_INLIF_MODIFY_TAGGED_E:
            actionPtr->vlanCmd = PRV_TGF_TTI_VLAN_MODIFY_TAGGED_E;
            break;

        case CPSS_EXMXPM_TTI_VLAN_INLIF_MODIFY_ALL_E:
            actionPtr->vlanCmd = PRV_TGF_TTI_VLAN_MODIFY_ALL_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    actionPtr->vlanId = exMxPmTtiActionPtr->inLifVidId;

    /* convert mplsCmd from device specific format */
    switch (exMxPmTtiActionPtr->mplsCmd)
    {
        case CPSS_EXMXPM_TTI_MPLS_NOP_COMMAND_E:
            actionPtr->mplsCmd = PRV_TGF_TTI_MPLS_NOP_COMMAND_E;
            break;

        case CPSS_EXMXPM_TTI_MPLS_SWAP_COMMAND_E:
            actionPtr->mplsCmd = PRV_TGF_TTI_MPLS_SWAP_COMMAND_E;
            break;

        case CPSS_EXMXPM_TTI_MPLS_PUSH1_LABEL_COMMAND_E:
            actionPtr->mplsCmd = PRV_TGF_TTI_MPLS_PUSH1_LABEL_COMMAND_E;
            break;

        case CPSS_EXMXPM_TTI_MPLS_POP1_LABEL_COMMAND_E:
            actionPtr->mplsCmd = PRV_TGF_TTI_MPLS_POP1_LABEL_COMMAND_E;
            break;

        case CPSS_EXMXPM_TTI_MPLS_POP2_LABELS_COMMAND_E:
            actionPtr->mplsCmd = PRV_TGF_TTI_MPLS_POP2_LABELS_COMMAND_E;
            break;

        case CPSS_EXMXPM_TTI_MPLS_POP3_LABELS_COMMAND_E:
            actionPtr->mplsCmd = PRV_TGF_TTI_MPLS_POP3_LABELS_COMMAND_E;
            break;

        case CPSS_EXMXPM_TTI_MPLS_POP_AND_SWAP_COMMAND_E:
            actionPtr->mplsCmd = PRV_TGF_TTI_MPLS_POP_AND_SWAP_COMMAND_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert passengerPacketType from device specific format */
    switch (exMxPmTtiActionPtr->passengerPacketType)
    {
        case CPSS_EXMXPM_TTI_PASSENGER_IPV4_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_IPV4_E;
            break;

        case CPSS_EXMXPM_TTI_PASSENGER_IPV6_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_IPV6_E;
            break;

        case CPSS_EXMXPM_TTI_PASSENGER_ETHERNET_CRC_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_ETHERNET_CRC_E;
            break;

        case CPSS_EXMXPM_TTI_PASSENGER_ETHERNET_NO_CRC_E:
            actionPtr->passengerPacketType = PRV_TGF_TTI_PASSENGER_ETHERNET_NO_CRC_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tti action from device specific format */
    actionPtr->tunnelTerminate = exMxPmTtiActionPtr->tunnelTerminateEnable;

    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, userDefinedCpuCode);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, ipNextHopIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, copyTtlFromTunnelHeader);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, mirrorToIngressAnalyzerEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, policerIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, vlanPrecedence);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, nestedVlanEnable);

    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, ipNextHopIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, exp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, setExp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, enableDecTtl);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, mplsLabel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, ttl);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, pwId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, sourceIsPE);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, enableSourceLocalFiltering);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, floodDitPointer);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, counterEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, meterEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, flowId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, sstIdEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmTtiActionPtr, actionPtr, sstId);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmIpv4TtiRule
*
* DESCRIPTION:
*       Convert generic IPv4 TTI rule into device specific IPv4 TTI rule
*
* INPUTS:
*       ipv4TtiRulePtr - (pointer to) IPv4 TTI rule
*
* OUTPUTS:
*       exMxPmIpv4TtiRulePtr - (pointer to) ExMxPm IPv4 TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertGenericToExMxPmIpv4TtiRule
(
    IN  PRV_TGF_TTI_IPV4_RULE_STC      *ipv4TtiRulePtr,
    OUT CPSS_EXMXPM_TTI_IPV4_RULE_STC  *exMxPmIpv4TtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) exMxPmIpv4TtiRulePtr, 0, sizeof(*exMxPmIpv4TtiRulePtr));

    /* convert IPv4 tti rule into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), isTagged);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4TtiRulePtr, ipv4TtiRulePtr, tunneltype);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4TtiRulePtr, ipv4TtiRulePtr, isArp);

    /* set MAC */
    cpssOsMemCpy(exMxPmIpv4TtiRulePtr->common.mac.arEther,
                 ipv4TtiRulePtr->common.mac.arEther,
                 sizeof(ipv4TtiRulePtr->common.mac.arEther));

    /* set source IP */
    cpssOsMemCpy(exMxPmIpv4TtiRulePtr->srcIp.arIP,
                 ipv4TtiRulePtr->srcIp.arIP,
                 sizeof(ipv4TtiRulePtr->srcIp.arIP));

    /* set destination IP */
    cpssOsMemCpy(exMxPmIpv4TtiRulePtr->destIp.arIP,
                 ipv4TtiRulePtr->destIp.arIP,
                 sizeof(ipv4TtiRulePtr->destIp.arIP));
}

/*******************************************************************************
* prvTgfConvertExMxPmToGenericIpv4TtiRule
*
* DESCRIPTION:
*       Convert device specific IPv4 TTI rule into generic IPv4 TTI rule
*
* INPUTS:
*       exMxPmIpv4TtiRulePtr - (pointer to) ExMxPm IPv4 TTI rule
*
* OUTPUTS:
*       ipv4TtiRulePtr - (pointer to) IPv4 TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertExMxPmToGenericIpv4TtiRule
(
    IN  CPSS_EXMXPM_TTI_IPV4_RULE_STC  *exMxPmIpv4TtiRulePtr,
    OUT PRV_TGF_TTI_IPV4_RULE_STC      *ipv4TtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) ipv4TtiRulePtr, 0, sizeof(*ipv4TtiRulePtr));

    /* convert IPv4 tti rule from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), pclId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), vid);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), isTagged);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmIpv4TtiRulePtr->common), &(ipv4TtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4TtiRulePtr, ipv4TtiRulePtr, tunneltype);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4TtiRulePtr, ipv4TtiRulePtr, isArp);

    /* set MAC */
    cpssOsMemCpy(ipv4TtiRulePtr->common.mac.arEther,
                 exMxPmIpv4TtiRulePtr->common.mac.arEther,
                 sizeof(exMxPmIpv4TtiRulePtr->common.mac.arEther));

    /* set source IP */
    cpssOsMemCpy(ipv4TtiRulePtr->srcIp.arIP,
                 exMxPmIpv4TtiRulePtr->srcIp.arIP,
                 sizeof(exMxPmIpv4TtiRulePtr->srcIp.arIP));

    /* set destination IP */
    cpssOsMemCpy(ipv4TtiRulePtr->destIp.arIP,
                 exMxPmIpv4TtiRulePtr->destIp.arIP,
                 sizeof(exMxPmIpv4TtiRulePtr->destIp.arIP));
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmMplsTtiRule
*
* DESCRIPTION:
*       Convert generic Mpls TTI rule into device specific Mpls TTI rule
*
* INPUTS:
*       mplsTtiRulePtr - (pointer to) Mpls TTI rule
*
* OUTPUTS:
*       exMxPmMplsTtiRulePtr - (pointer to) ExMxPm Mpls TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertGenericToExMxPmMplsTtiRule
(
    IN  PRV_TGF_TTI_MPLS_RULE_STC      *mplsTtiRulePtr,
    OUT CPSS_EXMXPM_TTI_MPLS_RULE_STC  *exMxPmMplsTtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) exMxPmMplsTtiRulePtr, 0, sizeof(*exMxPmMplsTtiRulePtr));

    /* convert Mpls tti rule into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), isTagged);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsTtiRulePtr, mplsTtiRulePtr, label0);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsTtiRulePtr, mplsTtiRulePtr, exp0);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsTtiRulePtr, mplsTtiRulePtr, label1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsTtiRulePtr, mplsTtiRulePtr, exp1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsTtiRulePtr, mplsTtiRulePtr, label2);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsTtiRulePtr, mplsTtiRulePtr, exp2);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsTtiRulePtr, mplsTtiRulePtr, numOfLabels);

    exMxPmMplsTtiRulePtr->nlpAboveMPLS = mplsTtiRulePtr->protocolAboveMPLS;

    /* set MAC */
    cpssOsMemCpy(exMxPmMplsTtiRulePtr->common.mac.arEther,
                 mplsTtiRulePtr->common.mac.arEther,
                 sizeof(mplsTtiRulePtr->common.mac.arEther));
}

/*******************************************************************************
* prvTgfConvertExMxPmToGenericMplsTtiRule
*
* DESCRIPTION:
*       Convert device specific Mpls TTI rule into generic Mpls TTI rule
*
* INPUTS:
*       exMxPmMplsTtiRulePtr - (pointer to) ExMxPm Mpls TTI rule
*
* OUTPUTS:
*       mplsTtiRulePtr - (pointer to) Mpls TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertExMxPmToGenericMplsTtiRule
(
    IN  CPSS_EXMXPM_TTI_MPLS_RULE_STC  *exMxPmMplsTtiRulePtr,
    OUT PRV_TGF_TTI_MPLS_RULE_STC      *mplsTtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) mplsTtiRulePtr, 0, sizeof(*mplsTtiRulePtr));

    /* convert Mpls tti rule from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), pclId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), vid);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), isTagged);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMplsTtiRulePtr->common), &(mplsTtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsTtiRulePtr, mplsTtiRulePtr, label0);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsTtiRulePtr, mplsTtiRulePtr, exp0);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsTtiRulePtr, mplsTtiRulePtr, label1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsTtiRulePtr, mplsTtiRulePtr, exp1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsTtiRulePtr, mplsTtiRulePtr, label2);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsTtiRulePtr, mplsTtiRulePtr, exp2);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsTtiRulePtr, mplsTtiRulePtr, numOfLabels);

    mplsTtiRulePtr->protocolAboveMPLS = exMxPmMplsTtiRulePtr->nlpAboveMPLS;

    /* set MAC */
    cpssOsMemCpy(mplsTtiRulePtr->common.mac.arEther,
                 exMxPmMplsTtiRulePtr->common.mac.arEther,
                 sizeof(exMxPmMplsTtiRulePtr->common.mac.arEther));
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmEthTtiRule
*
* DESCRIPTION:
*       Convert generic Ethernet TTI rule into device specific Ethernet TTI rule
*
* INPUTS:
*       ethTtiRulePtr - (pointer to) Ethernet TTI rule
*
* OUTPUTS:
*       exMxPmEthTtiRulePtr - (pointer to) ExMxPm Ethernet TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertGenericToExMxPmEthTtiRule
(
    IN  PRV_TGF_TTI_ETH_RULE_STC       *ethTtiRulePtr,
    OUT CPSS_EXMXPM_TTI_ETH_RULE_STC   *exMxPmEthTtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) exMxPmEthTtiRulePtr, 0, sizeof(*exMxPmEthTtiRulePtr));

    /* convert IPv4 tti rule into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), isTagged);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, up0);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, cfi0);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, isVlan1Exists);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, vid1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, up1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, cfi1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, etherType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, macToMe);

    /* set MAC */
    cpssOsMemCpy(exMxPmEthTtiRulePtr->common.mac.arEther,
                 ethTtiRulePtr->common.mac.arEther,
                 sizeof(ethTtiRulePtr->common.mac.arEther));
}

/*******************************************************************************
* prvTgfConvertExMxPmToGenericEthTtiRule
*
* DESCRIPTION:
*       Convert device specific Ethernet TTI rule into generic Ethernet TTI rule
*
* INPUTS:
*       exMxPmEthTtiRulePtr - (pointer to) ExMxPm Ethernet TTI rule
*
* OUTPUTS:
*       ethTtiRulePtr - (pointer to) Ethernet TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertExMxPmToGenericEthTtiRule
(
    IN  CPSS_EXMXPM_TTI_ETH_RULE_STC   *exMxPmEthTtiRulePtr,
    OUT PRV_TGF_TTI_ETH_RULE_STC       *ethTtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) ethTtiRulePtr, 0, sizeof(*ethTtiRulePtr));

    /* convert IPv4 tti rule from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), pclId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), vid);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), isTagged);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmEthTtiRulePtr->common), &(ethTtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, up0);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, cfi0);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, isVlan1Exists);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, vid1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, up1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, cfi1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, etherType);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmEthTtiRulePtr, ethTtiRulePtr, macToMe);

    /* set MAC */
    cpssOsMemCpy(ethTtiRulePtr->common.mac.arEther,
                 exMxPmEthTtiRulePtr->common.mac.arEther,
                 sizeof(exMxPmEthTtiRulePtr->common.mac.arEther));
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmMimTtiRule
*
* DESCRIPTION:
*       Convert generic MIM TTI rule into device specific MIM TTI rule
*
* INPUTS:
*       ethTtiRulePtr - (pointer to) MIM TTI rule
*
* OUTPUTS:
*       exMxPmMimTtiRulePtr - (pointer to) ExMxPm MIM TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertGenericToExMxPmMimTtiRule
(
    IN  PRV_TGF_TTI_MIM_RULE_STC       *mimTtiRulePtr,
    OUT CPSS_EXMXPM_TTI_MIM_RULE_STC   *exMxPmMimTtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) exMxPmMimTtiRulePtr, 0, sizeof(*exMxPmMimTtiRulePtr));

    /* convert IPv4 tti rule into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), pclId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), vid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), isTagged);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimTtiRulePtr, mimTtiRulePtr, bUp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimTtiRulePtr, mimTtiRulePtr, bDp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimTtiRulePtr, mimTtiRulePtr, iSid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimTtiRulePtr, mimTtiRulePtr, iUp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimTtiRulePtr, mimTtiRulePtr, iDp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimTtiRulePtr, mimTtiRulePtr, iRes1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimTtiRulePtr, mimTtiRulePtr, iRes2);

    /* set MAC */
    cpssOsMemCpy(exMxPmMimTtiRulePtr->common.mac.arEther,
                 mimTtiRulePtr->common.mac.arEther,
                 sizeof(mimTtiRulePtr->common.mac.arEther));
}

/*******************************************************************************
* prvTgfConvertExMxPmToGenericMimTtiRule
*
* DESCRIPTION:
*       Convert device specific MIM TTI rule into generic MIM TTI rule
*
* INPUTS:
*       exMxPmMimTtiRulePtr - (pointer to) ExMxPm MIM TTI rule
*
* OUTPUTS:
*       ethTtiRulePtr - (pointer to) MIM TTI rule
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID prvTgfConvertExMxPmToGenericMimTtiRule
(
    IN  CPSS_EXMXPM_TTI_MIM_RULE_STC   *exMxPmMimTtiRulePtr,
    OUT PRV_TGF_TTI_MIM_RULE_STC       *mimTtiRulePtr
)
{
    /* clear tti action */
    cpssOsMemSet((GT_VOID*) mimTtiRulePtr, 0, sizeof(*mimTtiRulePtr));

    /* convert IPv4 tti rule from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), pclId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), srcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), srcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), vid);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), isTagged);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), dsaSrcIsTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), dsaSrcPortTrunk);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(&(exMxPmMimTtiRulePtr->common), &(mimTtiRulePtr->common), dsaSrcDevice);

    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimTtiRulePtr, mimTtiRulePtr, bUp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimTtiRulePtr, mimTtiRulePtr, bDp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimTtiRulePtr, mimTtiRulePtr, iSid);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimTtiRulePtr, mimTtiRulePtr, iUp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimTtiRulePtr, mimTtiRulePtr, iDp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimTtiRulePtr, mimTtiRulePtr, iRes1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimTtiRulePtr, mimTtiRulePtr, iRes2);

    /* set MAC */
    cpssOsMemCpy(mimTtiRulePtr->common.mac.arEther,
                 exMxPmMimTtiRulePtr->common.mac.arEther,
                 sizeof(exMxPmMimTtiRulePtr->common.mac.arEther));
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmTunnelStartIpv4Entry
*
* DESCRIPTION:
*       Convert generic tunnel start IPv4 entry into device specific
*
* INPUTS:
*       ipv4EntryPtr - (pointer to) tunnel start IPv4 entry
*
* OUTPUTS:
*       exMxPmIpv4EntryPtr - (pointer to) ExMxPm tunnel start IPv4 entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmTunnelStartIpv4Entry
(
    IN  PRV_TGF_TUNNEL_START_IPV4_ENTRY_STC       *ipv4EntryPtr,
    OUT CPSS_EXMXPM_TUNNEL_START_IPV4_ENTRY_STC   *exMxPmIpv4EntryPtr
)
{
    /* clear tunnel start IPv4 entry */
    cpssOsMemSet((GT_VOID*) exMxPmIpv4EntryPtr, 0, sizeof(*exMxPmIpv4EntryPtr));

    /* convert upMarkMode into device specific format */
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(exMxPmIpv4EntryPtr->upMarkMode, ipv4EntryPtr->upMarkMode);

    /* convert dscpMarkMode into device specific format */
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(exMxPmIpv4EntryPtr->dscpMarkMode, ipv4EntryPtr->dscpMarkMode);

    /* convert tunnel start IPv4 entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, tagEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, vlanId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, dscp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, macDa);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, dontFragmentFlag);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, ttl);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, autoTunnel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, autoTunnelOffset);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, ethType);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, destIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, srcIp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, retainCrc);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertExMxPmToGenericTunnelStartIpv4Entry
*
* DESCRIPTION:
*       Convert device specific tunnel start IPv4 entry into generic
*
* INPUTS:
*       exMxPmIpv4EntryPtr - (pointer to) ExMxPm tunnel start IPv4 entry
*
* OUTPUTS:
*       ipv4EntryPtr - (pointer to) tunnel start IPv4 entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertExMxPmToGenericTunnelStartIpv4Entry
(
    IN  CPSS_EXMXPM_TUNNEL_START_IPV4_ENTRY_STC   *exMxPmIpv4EntryPtr,
    OUT PRV_TGF_TUNNEL_START_IPV4_ENTRY_STC       *ipv4EntryPtr
)
{
    /* clear tunnel start IPv4 entry */
    cpssOsMemSet((GT_VOID*) ipv4EntryPtr, 0, sizeof(*ipv4EntryPtr));

    /* convert upMarkMode from device specific format */
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(exMxPmIpv4EntryPtr->upMarkMode, ipv4EntryPtr->upMarkMode);

    /* convert dscpMarkMode from device specific format */
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(exMxPmIpv4EntryPtr->dscpMarkMode, ipv4EntryPtr->dscpMarkMode);

    /* convert tunnel start IPv4 entry from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, tagEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, vlanId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, up);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, dscp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, macDa);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, dontFragmentFlag);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, ttl);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, autoTunnel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, autoTunnelOffset);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, ethType);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, destIp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, srcIp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmIpv4EntryPtr, ipv4EntryPtr, retainCrc);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmTunnelStartMplsEntry
*
* DESCRIPTION:
*       Convert generic tunnel start Mpls entry into device specific
*
* INPUTS:
*       mplsEntryPtr - (pointer to) tunnel start Mpls entry
*
* OUTPUTS:
*       exMxPmMplsEntryPtr - (pointer to) ExMxPm tunnel start Mpls entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmTunnelStartMplsEntry
(
    IN  PRV_TGF_TUNNEL_START_MPLS_ENTRY_STC       *mplsEntryPtr,
    OUT CPSS_EXMXPM_TUNNEL_START_MPLS_ENTRY_STC   *exMxPmMplsEntryPtr
)
{
    /* clear tunnel start Mpls entry */
    cpssOsMemSet((GT_VOID*) exMxPmMplsEntryPtr, 0, sizeof(*mplsEntryPtr));

    /* convert upMarkMode into device specific format */
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(exMxPmMplsEntryPtr->upMarkMode, mplsEntryPtr->upMarkMode);

    /* convert ethType into device specific format */
    switch (mplsEntryPtr->ethType)
    {
        case PRV_TGF_TUNNEL_START_MPLS_ETHER_TYPE_UC_E:
            exMxPmMplsEntryPtr->ethType = CPSS_EXMXPM_TUNNEL_START_MPLS_ETHER_TYPE_UC_E;
            break;

        case PRV_TGF_TUNNEL_START_MPLS_ETHER_TYPE_MC_E:
            exMxPmMplsEntryPtr->ethType = CPSS_EXMXPM_TUNNEL_START_MPLS_ETHER_TYPE_MC_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert exp1MarkMode into device specific format */
    switch (mplsEntryPtr->exp1MarkMode)
    {
        case PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E:
            exMxPmMplsEntryPtr->exp1MarkMode = CPSS_EXMXPM_TUNNEL_START_EXP_MARK_FROM_INGRESS_PIPE_E;
            break;

        case PRV_TGF_TUNNEL_START_MARK_FROM_PACKET_QOS_PROFILE_E:
            exMxPmMplsEntryPtr->exp1MarkMode = CPSS_EXMXPM_TUNNEL_START_EXP_MARK_FROM_TS_ENTRY_E;
            break;

        case PRV_TGF_TUNNEL_START_MARK_FROM_INGRESS_PIPE_E:
            exMxPmMplsEntryPtr->exp1MarkMode = CPSS_EXMXPM_TUNNEL_START_EXP_MARK_FROM_TS_ENTRY_DP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert exp2MarkMode into device specific format */
    switch (mplsEntryPtr->exp2MarkMode)
    {
        case PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E:
            exMxPmMplsEntryPtr->exp2MarkMode = CPSS_EXMXPM_TUNNEL_START_EXP_MARK_FROM_INGRESS_PIPE_E;
            break;

        case PRV_TGF_TUNNEL_START_MARK_FROM_PACKET_QOS_PROFILE_E:
            exMxPmMplsEntryPtr->exp2MarkMode = CPSS_EXMXPM_TUNNEL_START_EXP_MARK_FROM_TS_ENTRY_E;
            break;

        case PRV_TGF_TUNNEL_START_MARK_FROM_INGRESS_PIPE_E:
            exMxPmMplsEntryPtr->exp2MarkMode = CPSS_EXMXPM_TUNNEL_START_EXP_MARK_FROM_TS_ENTRY_DP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tunnel start Mpls entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, tagEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, vlanId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, macDa);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, numLabels);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, ttl);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, label1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, exp1);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, label2);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, exp2);

    exMxPmMplsEntryPtr->retainCrc = mplsEntryPtr->retainCRC;

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertExMxPmToGenericTunnelStartMplsEntry
*
* DESCRIPTION:
*       Convert device specific tunnel start Mpls entry into generic
*
* INPUTS:
*       exMxPmMplsEntryPtr - (pointer to) ExMxPm tunnel start Mpls entry
*
* OUTPUTS:
*       mplsEntryPtr - (pointer to) tunnel start Mpls entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertExMxPmToGenericTunnelStartMplsEntry
(
    IN  CPSS_EXMXPM_TUNNEL_START_MPLS_ENTRY_STC   *exMxPmMplsEntryPtr,
    OUT PRV_TGF_TUNNEL_START_MPLS_ENTRY_STC       *mplsEntryPtr
)
{
    /* clear tunnel start Mpls entry */
    cpssOsMemSet((GT_VOID*) mplsEntryPtr, 0, sizeof(*mplsEntryPtr));

    /* convert upMarkMode from device specific format */
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(exMxPmMplsEntryPtr->upMarkMode, mplsEntryPtr->upMarkMode);

    /* convert ethType from device specific format */
    switch (exMxPmMplsEntryPtr->ethType)
    {
        case CPSS_EXMXPM_TUNNEL_START_MPLS_ETHER_TYPE_UC_E:
            mplsEntryPtr->ethType = PRV_TGF_TUNNEL_START_MPLS_ETHER_TYPE_UC_E;
            break;

        case CPSS_EXMXPM_TUNNEL_START_MPLS_ETHER_TYPE_MC_E:
            mplsEntryPtr->ethType = PRV_TGF_TUNNEL_START_MPLS_ETHER_TYPE_MC_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert exp1MarkMode from device specific format */
    switch (exMxPmMplsEntryPtr->exp1MarkMode)
    {
        case CPSS_EXMXPM_TUNNEL_START_EXP_MARK_FROM_INGRESS_PIPE_E:
            mplsEntryPtr->exp1MarkMode = PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E;
            break;

        case CPSS_EXMXPM_TUNNEL_START_EXP_MARK_FROM_TS_ENTRY_E:
            mplsEntryPtr->exp1MarkMode = PRV_TGF_TUNNEL_START_MARK_FROM_PACKET_QOS_PROFILE_E;
            break;

        case CPSS_EXMXPM_TUNNEL_START_EXP_MARK_FROM_TS_ENTRY_DP_E:
            mplsEntryPtr->exp1MarkMode = PRV_TGF_TUNNEL_START_MARK_FROM_INGRESS_PIPE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert exp2MarkMode from device specific format */
    switch (exMxPmMplsEntryPtr->exp2MarkMode)
    {
        case CPSS_EXMXPM_TUNNEL_START_EXP_MARK_FROM_INGRESS_PIPE_E:
            mplsEntryPtr->exp2MarkMode = PRV_TGF_TUNNEL_START_MARK_FROM_ENTRY_E;
            break;

        case CPSS_EXMXPM_TUNNEL_START_EXP_MARK_FROM_TS_ENTRY_E:
            mplsEntryPtr->exp2MarkMode = PRV_TGF_TUNNEL_START_MARK_FROM_PACKET_QOS_PROFILE_E;
            break;

        case CPSS_EXMXPM_TUNNEL_START_EXP_MARK_FROM_TS_ENTRY_DP_E:
            mplsEntryPtr->exp2MarkMode = PRV_TGF_TUNNEL_START_MARK_FROM_INGRESS_PIPE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tunnel start Mpls entry from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, tagEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, vlanId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, up);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, macDa);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, numLabels);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, ttl);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, label1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, exp1);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, label2);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMplsEntryPtr, mplsEntryPtr, exp2);

    mplsEntryPtr->retainCRC = exMxPmMplsEntryPtr->retainCrc;

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmTunnelStartMimEntry
*
* DESCRIPTION:
*       Convert generic tunnel start Mim entry into device specific
*
* INPUTS:
*       mimEntryPtr - (pointer to) tunnel start Mim entry
*
* OUTPUTS:
*       exMxPmMimEntryPtr - (pointer to) ExMxPm tunnel start Mim entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmTunnelStartMimEntry
(
    IN  PRV_TGF_TUNNEL_START_MIM_ENTRY_STC        *mimEntryPtr,
    OUT CPSS_EXMXPM_TUNNEL_START_MIM_ENTRY_STC    *exMxPmMimEntryPtr
)
{
    /* clear tunnel start Mim entry */
    cpssOsMemSet((GT_VOID*) exMxPmMimEntryPtr, 0, sizeof(*mimEntryPtr));

    /* convert upMarkMode into device specific format */
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(exMxPmMimEntryPtr->upMarkMode, mimEntryPtr->upMarkMode);

    /* convert iSidAssignMode into device specific format */
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(exMxPmMimEntryPtr->iSidAssignMode, mimEntryPtr->iSidAssignMode);

    /* convert iUpMarkMode into device specific format */
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(exMxPmMimEntryPtr->iUpMarkMode, mimEntryPtr->iUpMarkMode);

    /* convert iDpMarkMode into device specific format */
    PRV_TGF_S2D_MARK_MODE_CONVERT_MAC(exMxPmMimEntryPtr->iDpMarkMode, mimEntryPtr->iDpMarkMode);

    /* convert tunnel start Mim entry into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, tagEnable);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, vlanId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, up);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, ttl);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, macDa);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, retainCrc);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, iSid);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, iUp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, iDp);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, iTagReserved);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertExMxPmToGenericTunnelStartMimEntry
*
* DESCRIPTION:
*       Convert device specific tunnel start Mim entry into generic
*
* INPUTS:
*       exMxPmMimEntryPtr - (pointer to) ExMxPm tunnel start Mim entry
*
* OUTPUTS:
*       mimEntryPtr - (pointer to) tunnel start Mim entry
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertExMxPmToGenericTunnelStartMimEntry
(
    IN  CPSS_EXMXPM_TUNNEL_START_MIM_ENTRY_STC   *exMxPmMimEntryPtr,
    OUT PRV_TGF_TUNNEL_START_MIM_ENTRY_STC       *mimEntryPtr
)
{
    /* clear tunnel start Mim entry */
    cpssOsMemSet((GT_VOID*) mimEntryPtr, 0, sizeof(*mimEntryPtr));

    /* convert upMarkMode from device specific format */
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(exMxPmMimEntryPtr->upMarkMode, mimEntryPtr->upMarkMode);

    /* convert iSidAssignMode from device specific format */
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(exMxPmMimEntryPtr->iSidAssignMode, mimEntryPtr->iSidAssignMode);

    /* convert iUpMarkMode from device specific format */
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(exMxPmMimEntryPtr->iUpMarkMode, mimEntryPtr->iUpMarkMode);

    /* convert iDpMarkMode from device specific format */
    PRV_TGF_D2S_MARK_MODE_CONVERT_MAC(exMxPmMimEntryPtr->iDpMarkMode, mimEntryPtr->iDpMarkMode);

    /* convert tunnel start Mim entry from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, tagEnable);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, vlanId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, up);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, ttl);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, macDa);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, retainCrc);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, iSid);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, iUp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, iDp);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMimEntryPtr, mimEntryPtr, iTagReserved);

    return GT_OK;
}
#endif /* EXMXPM_FAMILY */


/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfTtiRuleActionSet
*
* DESCRIPTION:
*       This function sets rule action
*
* INPUTS:
*       devNum   - device number
*       ruleIndex - index of the rule in the TCAM
*       actionPtr - (pointer to) the TTI rule action
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiRuleActionSet
(
    IN  GT_U32                        ruleIndex,
    IN  PRV_TGF_TTI_ACTION_STC       *actionPtr
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
#ifdef CHX_FAMILY
    CPSS_PP_FAMILY_TYPE_ENT             devFamily  = CPSS_PP_FAMILY_START_DXCH_E;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;
    CPSS_DXCH_TTI_ACTION_UNT            dxChTtiAction;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_TCAM_TYPE_ENT           tcamType   = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    CPSS_EXMXPM_TTI_ACTION_TYPE_ENT     actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
    CPSS_EXMXPM_TCAM_ACTION_INFO_UNT    actionInfo;
    CPSS_EXMXPM_TTI_ACTION_UNT          exMxPmTtiAction;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* get device family */
    rc = prvUtfDeviceFamilyGet(prvTgfDevNum, &devFamily);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfDeviceFamilyGet FAILED, rc = [%d]", rc);

        return rc;
    }

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(prvTgfDevNum))
    {
        /* set rule action type */
        actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

        /* convert tti action type1 into device specific format */
        rc = prvTgfConvertGenericToDxChTtiAction(actionPtr, &dxChTtiAction.type1);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChTtiAction FAILED, rc = [%d]", rc);

            return rc;
        }
    }
    else
    {
        /* set rule action type */
        actionType = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        /* convert tti action type2 into device specific format */
        rc = prvTgfConvertGenericToDxChTtiAction2(actionPtr, &dxChTtiAction.type2);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChTtiAction2 FAILED, rc = [%d]", rc);

            return rc;
        }
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChTtiRuleActionUpdate(devNum, ruleIndex, actionType, &dxChTtiAction);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTtiRuleActionUpdate FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* set tcam type and tcam action type */
    tcamType   = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;

    /* clear tcam action info and tti action */
    cpssOsMemSet((GT_VOID*) &actionInfo, 0, sizeof(actionInfo));
    cpssOsMemSet((GT_VOID*) &exMxPmTtiAction, 0, sizeof(exMxPmTtiAction));

    /* set rule index */
    actionInfo.internalTcamRuleStartIndex = ruleIndex;

    /* convert tti action into device specific format */
    rc = prvTgfConvertGenericToExMxPmTtiAction(actionPtr, &(exMxPmTtiAction.standard));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmTtiAction FAILED, rc = [%d]", rc);

        return rc;
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
        rc = cpssExMxPmTtiRuleActionUpdate(devNum, tcamType, &actionInfo, actionType, &exMxPmTtiAction);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmTtiRuleActionUpdate FAILED, rc = [%d]", rc);

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
* prvTgfTtiRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action
*
* INPUTS:
*       devNum     - device number
*       keyType    - TTI key type
*       ruleIndex  - index of the tunnel termination entry
*       patternPtr - (pointer to) the rule's pattern
*       maskPtr    - (pointer to) the rule's mask
*       actionPtr  - (pointer to) the TTI rule action
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiRuleSet
(
    IN  GT_U32                        ruleIndex,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    IN  PRV_TGF_TTI_RULE_UNT         *patternPtr,
    IN  PRV_TGF_TTI_RULE_UNT         *maskPtr,
    IN  PRV_TGF_TTI_ACTION_STC       *actionPtr
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
#ifdef CHX_FAMILY
    CPSS_PP_FAMILY_TYPE_ENT         devFamily  = CPSS_PP_FAMILY_START_DXCH_E;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;
    CPSS_DXCH_TTI_RULE_UNT          dxChPattern;
    CPSS_DXCH_TTI_RULE_UNT          dxChMask;
    CPSS_DXCH_TTI_ACTION_UNT        dxChTtiAction;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_TCAM_TYPE_ENT               tcamType   = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    CPSS_EXMXPM_TTI_ACTION_TYPE_ENT         actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
    GT_BOOL                                 valid      = GT_FALSE;
    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT   ruleInfo;
    CPSS_EXMXPM_TTI_RULE_UNT                exMxPmPattern;
    CPSS_EXMXPM_TTI_RULE_UNT                exMxPmMask;
    CPSS_EXMXPM_TTI_ACTION_UNT              exMxPmTtiAction;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* reset variables */
    cpssOsMemSet((GT_VOID*) &dxChPattern,   0, sizeof(dxChPattern));
    cpssOsMemSet((GT_VOID*) &dxChMask,      0, sizeof(dxChMask));
    cpssOsMemSet((GT_VOID*) &dxChTtiAction, 0, sizeof(dxChTtiAction));

    /* get device family */
    rc = prvUtfDeviceFamilyGet(prvTgfDevNum, &devFamily);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfDeviceFamilyGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert tti rule into device specific format */
    switch (keyType)
    {
        case PRV_TGF_TTI_KEY_IPV4_E:
            /* convert IPv4 tti rule pattern into device specific format */
            prvTgfConvertGenericToDxChIpv4TtiRule(&(patternPtr->ipv4),
                                                  &(dxChPattern.ipv4));
            /* convert IPv4 tti rule mask into device specific format */
            prvTgfConvertGenericToDxChIpv4TtiRule(&(maskPtr->ipv4),
                                                  &(dxChMask.ipv4));
            break;

        case PRV_TGF_TTI_KEY_MPLS_E:
            /* convert Mpls tti rule pattern into device specific format */
            prvTgfConvertGenericToDxChMplsTtiRule(&(patternPtr->mpls),
                                                  &(dxChPattern.mpls));
            /* convert Mpls tti rule mask into device specific format */
            prvTgfConvertGenericToDxChMplsTtiRule(&(maskPtr->mpls),
                                                  &(dxChMask.mpls));
            break;

        case PRV_TGF_TTI_KEY_ETH_E:
            /* convert Ethernet tti rule pattern into device specific format */
            prvTgfConvertGenericToDxChEthTtiRule(&(patternPtr->eth),
                                                  &(dxChPattern.eth));
            /* convert Ethernet tti rule mask into device specific format */
            prvTgfConvertGenericToDxChEthTtiRule(&(maskPtr->eth),
                                                  &(dxChMask.eth));
            break;

        case PRV_TGF_TTI_KEY_MIM_E:
            /* convert Mac-in-Mac tti rule pattern into device specific format */
            prvTgfConvertGenericToDxChMimTtiRule(&(patternPtr->mim),
                                                 &(dxChPattern.mim));
            /* convert Mac-in-Mac tti rule mask into device specific format */
            prvTgfConvertGenericToDxChMimTtiRule(&(maskPtr->mim),
                                                 &(dxChMask.mim));
            break;

        default:
            return GT_BAD_PARAM;
    }

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(prvTgfDevNum))
    {
        /* set rule action type */
        actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

        /* convert tti action type1 into device specific format */
        rc = prvTgfConvertGenericToDxChTtiAction(actionPtr, &dxChTtiAction.type1);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChTtiAction FAILED, rc = [%d]", rc);

            return rc;
        }
    }
    else
    {
        /* set rule action type */
        actionType = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        /* convert tti action type2 into device specific format */
        rc = prvTgfConvertGenericToDxChTtiAction2(actionPtr, &dxChTtiAction.type2);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChTtiAction2 FAILED, rc = [%d]", rc);

            return rc;
        }
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChTtiRuleSet(devNum, ruleIndex, (CPSS_DXCH_TTI_KEY_TYPE_ENT) keyType,
                                &dxChPattern, &dxChMask, actionType, &dxChTtiAction);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTtiRuleSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* reset variables */
    cpssOsMemSet((GT_VOID*) &ruleInfo, 0, sizeof(ruleInfo));
    cpssOsMemSet((GT_VOID*) &exMxPmPattern, 0, sizeof(exMxPmPattern));
    cpssOsMemSet((GT_VOID*) &exMxPmMask, 0, sizeof(exMxPmMask));
    cpssOsMemSet((GT_VOID*) &exMxPmTtiAction, 0, sizeof(exMxPmTtiAction));

    /* set tcam type and tcam action type */
    tcamType   = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
    valid      = GT_TRUE;

    /* set rule index */
    ruleInfo.internalTcamRuleStartIndex = ruleIndex;

    /* convert tti rule into device specific format */
    switch (keyType)
    {
        case PRV_TGF_TTI_KEY_IPV4_E:
            /* convert IPv4 tti rule pattern into device specific format */
            prvTgfConvertGenericToExMxPmIpv4TtiRule(&(patternPtr->ipv4),
                                                    &(exMxPmPattern.ipv4));
            /* convert IPv4 tti rule mask into device specific format */
            prvTgfConvertGenericToExMxPmIpv4TtiRule(&(maskPtr->ipv4),
                                                    &(exMxPmMask.ipv4));
            break;

        case PRV_TGF_TTI_KEY_MPLS_E:
            /* convert Mpls tti rule pattern into device specific format */
            prvTgfConvertGenericToExMxPmMplsTtiRule(&(patternPtr->mpls),
                                                    &(exMxPmPattern.mpls));
            /* convert Mpls tti rule mask into device specific format */
            prvTgfConvertGenericToExMxPmMplsTtiRule(&(maskPtr->mpls),
                                                    &(exMxPmMask.mpls));
            break;

        case PRV_TGF_TTI_KEY_ETH_E:
            /* convert Ethernet tti rule pattern into device specific format */
            prvTgfConvertGenericToExMxPmEthTtiRule(&(patternPtr->eth),
                                                   &(exMxPmPattern.eth));
            /* convert Ethernet tti rule mask into device specific format */
            prvTgfConvertGenericToExMxPmEthTtiRule(&(maskPtr->eth),
                                                   &(exMxPmMask.eth));
            break;

        case PRV_TGF_TTI_KEY_MIM_E:
            /* convert MIM tti rule pattern into device specific format */
            prvTgfConvertGenericToExMxPmMimTtiRule(&(patternPtr->mim),
                                                   &(exMxPmPattern.mim));
            /* convert MIM tti rule mask into device specific format */
            prvTgfConvertGenericToExMxPmMimTtiRule(&(maskPtr->mim),
                                                   &(exMxPmMask.mim));
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tti action into device specific format */
    rc = prvTgfConvertGenericToExMxPmTtiAction(actionPtr, &(exMxPmTtiAction.standard));
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmTtiAction FAILED, rc = [%d]", rc);

        return rc;
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
        rc = cpssExMxPmTtiRuleSet(devNum, tcamType, &ruleInfo, (CPSS_EXMXPM_TTI_KEY_TYPE_ENT) keyType,
                                  actionType, valid, &exMxPmMask, &exMxPmPattern, &exMxPmTtiAction);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmTtiRuleSet FAILED, rc = [%d]", rc);

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
* prvTgfTtiRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action
*
* INPUTS:
*       devNum    - device number
*       keyType   - TTI key type
*       ruleIndex - index of the tunnel termination entry
*
* OUTPUTS:
*       patternPtr - (pointer to) the rule's pattern
*       maskPtr    - (pointer to) the rule's mask
*       actionPtr  - (pointer to) the TTI rule action
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiRuleGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    OUT PRV_TGF_TTI_RULE_UNT         *patternPtr,
    OUT PRV_TGF_TTI_RULE_UNT         *maskPtr,
    OUT PRV_TGF_TTI_ACTION_STC       *actionPtr
)
{
    GT_STATUS   rc = GT_OK;
#ifdef CHX_FAMILY
    CPSS_PP_FAMILY_TYPE_ENT         devFamily  = CPSS_PP_FAMILY_START_DXCH_E;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;
    CPSS_DXCH_TTI_RULE_UNT          dxChPattern;
    CPSS_DXCH_TTI_RULE_UNT          dxChMask;
    CPSS_DXCH_TTI_ACTION_UNT        dxChTtiAction;
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_TCAM_TYPE_ENT               tcamType   = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    CPSS_EXMXPM_TTI_ACTION_TYPE_ENT         actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
    GT_BOOL                                 valid      = GT_FALSE;
    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT   ruleInfo;
    CPSS_EXMXPM_TTI_RULE_UNT                exMxPmPattern;
    CPSS_EXMXPM_TTI_RULE_UNT                exMxPmMask;
    CPSS_EXMXPM_TTI_ACTION_UNT              exMxPmTtiAction;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* get device family */
    rc = prvUtfDeviceFamilyGet(prvTgfDevNum, &devFamily);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfDeviceFamilyGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* set action type */
    actionType = PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(prvTgfDevNum) ?
                  CPSS_DXCH_TTI_ACTION_TYPE2_ENT : CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

    /* initialize action, pattern and mask */
    cpssOsMemSet(&dxChTtiAction, 0, sizeof(dxChTtiAction));
    cpssOsMemSet(&dxChPattern, 0, sizeof(dxChPattern));
    cpssOsMemSet(&dxChMask, 0, sizeof(dxChMask));


    /* call device specific API */
    rc = cpssDxChTtiRuleGet(devNum, ruleIndex, (CPSS_DXCH_TTI_KEY_TYPE_ENT) keyType,
                            &dxChPattern, &dxChMask, actionType, &dxChTtiAction);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTtiRuleGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert tti rule from device specific format */
    switch (keyType)
    {
        case PRV_TGF_TTI_KEY_IPV4_E:
            /* convert IPv4 tti rule pattern from device specific format */
            prvTgfConvertDxChToGenericIpv4TtiRule(&(dxChPattern.ipv4),
                                                  &(patternPtr->ipv4));
            /* convert IPv4 tti rule mask from device specific format */
            prvTgfConvertDxChToGenericIpv4TtiRule(&(dxChMask.ipv4),
                                                  &(maskPtr->ipv4));
            break;

        case PRV_TGF_TTI_KEY_MPLS_E:
            /* convert Mpls tti rule pattern from device specific format */
            prvTgfConvertDxChToGenericMplsTtiRule(&(dxChPattern.mpls),
                                                  &(patternPtr->mpls));
            /* convert Mpls tti rule mask from device specific format */
            prvTgfConvertDxChToGenericMplsTtiRule(&(dxChMask.mpls),
                                                  &(maskPtr->mpls));
            break;

        case PRV_TGF_TTI_KEY_ETH_E:
            /* convert Ethernet tti rule pattern from device specific format */
            prvTgfConvertDxChToGenericEthTtiRule(&(dxChPattern.eth),
                                                 &(patternPtr->eth));
            /* convert Ethernet tti rule mask from device specific format */
            prvTgfConvertDxChToGenericEthTtiRule(&(dxChMask.eth),
                                                 &(maskPtr->eth));
            break;

        case PRV_TGF_TTI_KEY_MIM_E:
            /* convert Mac-in-Mac tti rule pattern from device specific format */
            prvTgfConvertDxChToGenericMimTtiRule(&(dxChPattern.mim),
                                                 &(patternPtr->mim));
            /* convert Mac-in-Mac tti rule mask from device specific format */
            prvTgfConvertDxChToGenericMimTtiRule(&(dxChMask.mim),
                                                 &(maskPtr->mim));
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tti action */
    if (CPSS_DXCH_TTI_ACTION_TYPE1_ENT == actionType)
    {
        /* convert tti type1 action from device specific format */
        return prvTgfConvertDxChToGenericTtiAction(&dxChTtiAction.type1, actionPtr);
    }
    else
    {
        /* convert tti type2 action from device specific format */
        return prvTgfConvertDxChToGenericTtiAction2(&dxChTtiAction.type2, actionPtr);
    }
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* reset variables */
    cpssOsMemSet((GT_VOID*) &ruleInfo, 0, sizeof(ruleInfo));

    /* set tcam type and tcam action type */
    tcamType   = CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E;
    actionType = CPSS_EXMXPM_TTI_ACTION_STANDARD_E;
    valid      = GT_TRUE;

    /* set rule index */
    ruleInfo.internalTcamRuleStartIndex = ruleIndex;

    /* call device specific API */
    rc = cpssExMxPmTtiRuleGet(devNum, tcamType, &ruleInfo, (CPSS_EXMXPM_TTI_KEY_TYPE_ENT) keyType,
                              actionType, &valid, &exMxPmMask, &exMxPmPattern, &exMxPmTtiAction);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmTtiRuleGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert tti rule from device specific format */
    switch (keyType)
    {
        case PRV_TGF_TTI_KEY_IPV4_E:
            /* convert IPv4 tti rule pattern from device specific format */
            prvTgfConvertExMxPmToGenericIpv4TtiRule(&(exMxPmPattern.ipv4),
                                                    &(patternPtr->ipv4));
            /* convert IPv4 tti rule mask from device specific format */
            prvTgfConvertExMxPmToGenericIpv4TtiRule(&(exMxPmMask.ipv4),
                                                    &(maskPtr->ipv4));
            break;

        case PRV_TGF_TTI_KEY_MPLS_E:
            /* convert Mpls tti rule pattern from device specific format */
            prvTgfConvertExMxPmToGenericMplsTtiRule(&(exMxPmPattern.mpls),
                                                    &(patternPtr->mpls));
            /* convert Mpls tti rule mask from device specific format */
            prvTgfConvertExMxPmToGenericMplsTtiRule(&(exMxPmMask.mpls),
                                                    &(maskPtr->mpls));
            break;

        case PRV_TGF_TTI_KEY_ETH_E:
            /* convert Ethernet tti rule pattern from device specific format */
            prvTgfConvertExMxPmToGenericEthTtiRule(&(exMxPmPattern.eth),
                                                   &(patternPtr->eth));
            /* convert Ethernet tti rule mask from device specific format */
            prvTgfConvertExMxPmToGenericEthTtiRule(&(exMxPmMask.eth),
                                                   &(maskPtr->eth));
            break;

        case PRV_TGF_TTI_KEY_MIM_E:
            /* convert MIM tti rule pattern from device specific format */
            prvTgfConvertExMxPmToGenericMimTtiRule(&(exMxPmPattern.mim),
                                                   &(patternPtr->mim));
            /* convert MIM tti rule mask from device specific format */
            prvTgfConvertExMxPmToGenericMimTtiRule(&(exMxPmMask.mim),
                                                   &(maskPtr->mim));
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tti action from device specific format */
    return prvTgfConvertExMxPmToGenericTtiAction(&(exMxPmTtiAction.standard), actionPtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfTtiRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       If valid == GT_TRUE it is assumed that the TCAM entry already contains
*       the TTI entry information.
*
*******************************************************************************/
GT_STATUS prvTgfTtiRuleValidStatusSet
(
    IN  GT_U32                        routerTtiTcamRow,
    IN  GT_BOOL                       valid
)
{
    GT_U8       devNum = 0;
    GT_STATUS   rc, rc1 = GT_OK;
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_TCAM_RULE_ACTION_INFO_UNT    ruleActionInfo = {0};
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChTtiRuleValidStatusSet(devNum, routerTtiTcamRow, valid);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTtiMimEthTypeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* configure rule action info */
    ruleActionInfo.internalTcamRuleStartIndex = routerTtiTcamRow;

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
        rc = cpssExMxPmTtiRuleValidStatusSet(devNum, CPSS_EXMXPM_TCAM_TYPE_INTERNAL_E,
                                             &ruleActionInfo, valid);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmTtiRuleValidStatusSet FAILED, rc = [%d]", rc);

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
* prvTgfTunnelStartEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry
*
* INPUTS:
*       devNum     - device number
*       index      - line index for the tunnel start entry
*       tunnelType - type of the tunnel
*       configPtr  - (pointer to) tunnel start configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad parameter
*       GT_BAD_PTR      - on NULL pointer
*       GT_FAIL         - on failure
*       GT_OUT_OF_RANGE - parameter not in valid range
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTunnelStartEntrySet
(
    IN  GT_U32                          index,
    IN  CPSS_TUNNEL_TYPE_ENT            tunnelType,
    IN  PRV_TGF_TUNNEL_START_ENTRY_UNT *configPtr
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_TUNNEL_START_CONFIG_UNT dxChTunnelStartEntry = {{0}};
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_TUNNEL_START_ENTRY_UNT exMxPmTunnelStartEntry = {{0}};
#endif /* EXMXPM_FAMILY */


    switch (tunnelType)
    {
        case CPSS_TUNNEL_IPV4_OVER_IPV4_E:
        case CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E:
        case CPSS_TUNNEL_IPV6_OVER_IPV4_E:
        case CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E:
        case CPSS_TUNNEL_X_OVER_IPV4_E:
        case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
        case CPSS_TUNNEL_IP_OVER_X_E:
#ifdef CHX_FAMILY
            /* convert tunnel start Ipv4 entry into device specific format */
            rc = prvTgfConvertGenericToDxChTunnelStartIpv4Entry(&(configPtr->ipv4Cfg), &(dxChTunnelStartEntry.ipv4Cfg));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChTunnelStartIpv4Entry FAILED, rc = [%d]", rc);

                return rc;
            }
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
            /* convert tunnel start Ipv4 entry into device specific format */
            rc = prvTgfConvertGenericToExMxPmTunnelStartIpv4Entry(&(configPtr->ipv4Cfg), &(exMxPmTunnelStartEntry.ipv4Cfg));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmTunnelStartIpv4Entry FAILED, rc = [%d]", rc);

                return rc;
            }
#endif /* EXMXPM_FAMILY */
            break;

        case CPSS_TUNNEL_IP_OVER_MPLS_E:
        case CPSS_TUNNEL_ETHERNET_OVER_MPLS_E:
        case CPSS_TUNNEL_X_OVER_MPLS_E:
#ifdef CHX_FAMILY
            /* convert tunnel start Mpls entry into device specific format */
            rc = prvTgfConvertGenericToDxChTunnelStartMplsEntry(&(configPtr->mplsCfg), &(dxChTunnelStartEntry.mplsCfg));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChTunnelStartMplsEntry FAILED, rc = [%d]", rc);

                return rc;
            }
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
            /* convert tunnel start Mpls entry into device specific format */
            rc = prvTgfConvertGenericToExMxPmTunnelStartMplsEntry(&(configPtr->mplsCfg), &(exMxPmTunnelStartEntry.mplsCfg));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmTunnelStartMplsEntry FAILED, rc = [%d]", rc);

                return rc;
            }
#endif /* EXMXPM_FAMILY */
            break;

        case CPSS_TUNNEL_MAC_IN_MAC_E:
#ifdef CHX_FAMILY
            /* convert tunnel start Mim entry into device specific format */
            rc = prvTgfConvertGenericToDxChTunnelStartMimEntry(&(configPtr->mimCfg), &(dxChTunnelStartEntry.mimCfg));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChTunnelStartMimEntry FAILED, rc = [%d]", rc);

                return rc;
            }
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
            /* convert tunnel start Mim entry into device specific format */
            rc = prvTgfConvertGenericToExMxPmTunnelStartMimEntry(&(configPtr->mimCfg), &(exMxPmTunnelStartEntry.mimCfg));
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmTunnelStartMimEntry FAILED, rc = [%d]", rc);

                return rc;
            }
#endif /* EXMXPM_FAMILY */
            break;

        default:
            return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChTunnelStartEntrySet(devNum, index, tunnelType, &dxChTunnelStartEntry);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTunnelStartEntrySet FAILED, rc = [%d]", rc);

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
        rc = cpssExMxPmTunnelStartEntrySet(devNum, index, tunnelType, &exMxPmTunnelStartEntry);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmTunnelStartEntrySet FAILED, rc = [%d]", rc);

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
* prvTgfTunnelStartEntryGet
*
* DESCRIPTION:
*       Get a tunnel start entry
*
* INPUTS:
*       devNum - device number
*       index  - line index for the tunnel start entry
*
* OUTPUTS:
*       tunnelTypePtr - (pointer to) the type of the tunnel
*       configPtr     - (pointer to) tunnel start configuration
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on bad parameter.
*       GT_BAD_PTR      - on NULL pointer
*       GT_FAIL         - on failure.
*       GT_OUT_OF_RANGE - parameter not in valid range.
*       GT_BAD_STATE    - on invalid tunnel type
*       GT_TIMEOUT      - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTunnelStartEntryGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          index,
    OUT CPSS_TUNNEL_TYPE_ENT           *tunnelTypePtr,
    OUT PRV_TGF_TUNNEL_START_ENTRY_UNT *configPtr
)
{
    GT_STATUS   rc = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_TUNNEL_START_CONFIG_UNT dxChTunnelStartEntry = {{0}};
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_TUNNEL_START_ENTRY_UNT exMxPmTunnelStartEntry = {{0}};
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    /* call device specific API */
    rc = cpssDxChTunnelStartEntryGet(devNum, index, tunnelTypePtr, &dxChTunnelStartEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTunnelStartEntryGet FAILED, rc = [%d]", rc);

        return rc;
    }
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    rc = cpssExMxPmTunnelStartEntryGet(devNum, index, *tunnelTypePtr, &exMxPmTunnelStartEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmTunnelStartEntryGet FAILED, rc = [%d]", rc);

        return rc;
    }
#endif /* EXMXPM_FAMILY */

    switch (*tunnelTypePtr)
    {
        case CPSS_TUNNEL_IPV4_OVER_IPV4_E:
        case CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E:
        case CPSS_TUNNEL_IPV6_OVER_IPV4_E:
        case CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E:
        case CPSS_TUNNEL_X_OVER_IPV4_E:
        case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
        case CPSS_TUNNEL_IP_OVER_X_E:
#ifdef CHX_FAMILY
            /* convert tunnel start Ipv4 entry from device specific format */
            return prvTgfConvertDxChToGenericTunnelStartIpv4Entry(&(dxChTunnelStartEntry.ipv4Cfg), &(configPtr->ipv4Cfg));

#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
            /* convert tunnel start Ipv4 entry from device specific format */
            return prvTgfConvertExMxPmToGenericTunnelStartIpv4Entry(&(exMxPmTunnelStartEntry.ipv4Cfg), &(configPtr->ipv4Cfg));
#endif /* EXMXPM_FAMILY */

        case CPSS_TUNNEL_IP_OVER_MPLS_E:
        case CPSS_TUNNEL_ETHERNET_OVER_MPLS_E:
        case CPSS_TUNNEL_X_OVER_MPLS_E:
#ifdef CHX_FAMILY
            /* convert tunnel start Mpls entry from device specific format */
            return prvTgfConvertDxChToGenericTunnelStartMplsEntry(&(dxChTunnelStartEntry.mplsCfg), &(configPtr->mplsCfg));
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
            /* convert tunnel start Mpls entry from device specific format */
            return prvTgfConvertExMxPmToGenericTunnelStartMplsEntry(&(exMxPmTunnelStartEntry.mplsCfg), &(configPtr->mplsCfg));
#endif /* EXMXPM_FAMILY */
        case CPSS_TUNNEL_MAC_IN_MAC_E:
#ifdef CHX_FAMILY
            /* convert tunnel start Mim entry from device specific format */
            return prvTgfConvertDxChToGenericTunnelStartMimEntry(&(dxChTunnelStartEntry.mimCfg), &(configPtr->mimCfg));
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
            /* convert tunnel start Mim entry from device specific format */
            return prvTgfConvertExMxPmToGenericTunnelStartMimEntry(&(exMxPmTunnelStartEntry.mimCfg), &(configPtr->mimCfg));
#endif /* EXMXPM_FAMILY */

        default:
            return GT_BAD_PARAM;
    }

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfTtiPortLookupEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for the specified key
*       type at the port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       keyType - TTI key type
*       enable  - enable\disable TTI lookup
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiPortLookupEnableSet
(
    IN  GT_U8                         portNum,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    IN  GT_BOOL                       enable
)
{
    GT_U32  portIter = 0;
#ifdef CHX_FAMILY
    CPSS_DXCH_TTI_KEY_TYPE_ENT      dxChKeyType = CPSS_DXCH_TTI_KEY_IPV4_E;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT    exMxPmKeyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
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
    /* convert keyType into device specific format */
    PRV_TGF_S2D_KEY_TYPE_CONVERT_MAC(dxChKeyType, keyType);

    /* call device specific API */
    return cpssDxChTtiPortLookupEnableSet(prvTgfDevsArray[portIter], portNum, dxChKeyType, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* convert keyType into device specific format */
    PRV_TGF_S2D_KEY_TYPE_CONVERT_MAC(exMxPmKeyType, keyType);

    /* call device specific API */
    return cpssExMxPmTtiPortLookupEnableSet(prvTgfDevsArray[portIter], portNum, exMxPmKeyType, enable);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfTtiMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type
*
* INPUTS:
*       devNum  - device number
*       keyType - TTI key type
*       macMode - MAC mode to use
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiMacModeSet
(
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    IN  PRV_TGF_TTI_MAC_MODE_ENT      macMode
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_TTI_KEY_TYPE_ENT      dxChKeyType = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_MAC_MODE_ENT      dxChMacMode = CPSS_DXCH_TTI_MAC_MODE_DA_E;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT    exMxPmKeyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    CPSS_EXMXPM_TTI_MAC_MODE_ENT    exMxPmMacMode = CPSS_EXMXPM_TTI_MAC_MODE_DA_E;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* convert keyType into device specific format */
    PRV_TGF_S2D_KEY_TYPE_CONVERT_MAC(dxChKeyType, keyType);

    /* convert macMode into device specific format */
    PRV_TGF_S2D_MAC_MODE_CONVERT_MAC(dxChMacMode, macMode);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChTtiMacModeSet(devNum, dxChKeyType, dxChMacMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTtiMacModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* convert keyType into device specific format */
    PRV_TGF_S2D_KEY_TYPE_CONVERT_MAC(exMxPmKeyType, keyType);

    /* convert macMode into device specific format */
    PRV_TGF_S2D_MAC_MODE_CONVERT_MAC(exMxPmMacMode, macMode);

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
        rc = cpssExMxPmTtiMacModeSet(devNum, exMxPmKeyType, exMxPmMacMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmTtiMacModeSet FAILED, rc = [%d]", rc);

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
* prvTgfTtiMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type
*
* INPUTS:
*       devNum  - device number
*       keyType - TTI key type
*
* OUTPUTS:
*       macModePtr - (pointer to) MAC mode to use
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiMacModeGet
(
    IN  GT_U8                         devNum,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    OUT PRV_TGF_TTI_MAC_MODE_ENT     *macModePtr
)
{
    GT_STATUS   rc = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_TTI_KEY_TYPE_ENT      dxChKeyType = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_MAC_MODE_ENT      dxChMacMode = CPSS_DXCH_TTI_MAC_MODE_DA_E;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_TTI_KEY_TYPE_ENT    exMxPmKeyType = CPSS_EXMXPM_TTI_KEY_IPV4_E;
    CPSS_EXMXPM_TTI_MAC_MODE_ENT    exMxPmMacMode = CPSS_EXMXPM_TTI_MAC_MODE_DA_E;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* convert keyType into device specific format */
    PRV_TGF_S2D_KEY_TYPE_CONVERT_MAC(dxChKeyType, keyType);

    /* call device specific API */
    rc = cpssDxChTtiMacModeGet(devNum, dxChKeyType, &dxChMacMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTtiMacModeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert macMode from device specific format */
    PRV_TGF_D2S_MAC_MODE_CONVERT_MAC(dxChMacMode, *macModePtr);

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* convert keyType into device specific format */
    PRV_TGF_S2D_KEY_TYPE_CONVERT_MAC(exMxPmKeyType, keyType);

    /* call device specific API */
    rc = cpssExMxPmTtiMacModeGet(devNum, exMxPmKeyType, &exMxPmMacMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmTtiMacModeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert macMode from device specific format */
    PRV_TGF_D2S_MAC_MODE_CONVERT_MAC(exMxPmMacMode, *macModePtr);

    return rc;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfTtiPortIpv4OnlyTunneledEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only tunneled
*       packets received on port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - enable\disable IPv4 TTI lookup only for tunneled packets
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiPortIpv4OnlyTunneledEnableSet
(
    IN  GT_U8                         portNum,
    IN  GT_BOOL                       enable
)
{
    GT_U32  portIter = 0;


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
    return cpssDxChTtiPortIpv4OnlyTunneledEnableSet(prvTgfDevsArray[portIter], portNum, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmTtiPortIpv4OnlyTunneledEnableSet(prvTgfDevsArray[portIter], portNum, enable);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfTtiMimEthTypeSet
*
* DESCRIPTION:
*       This function sets the MIM Ethernet type
*
* INPUTS:
*       devNum  - device number
*       ethType - Ethernet type value (range 16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiMimEthTypeSet
(
    IN  GT_U32                        ethType
)
{
    GT_U8       devNum = 0;
    GT_STATUS   rc, rc1 = GT_OK;


#ifdef CHX_FAMILY
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChTtiMimEthTypeSet(devNum, ethType);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTtiMimEthTypeSet FAILED, rc = [%d]", rc);

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
        rc = cpssExMxPmTtiMimEtherTypeSet(devNum, ethType);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmTtiMimEtherTypeSet FAILED, rc = [%d]", rc);

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
* prvTgfTtiMacToMeSet
*
* DESCRIPTION:
*       This function sets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MACTOME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MACTOME
*       lookup is performed and the internal flag is set accordingly.
*
*
* INPUTS:
*       entryIndex        - Index of mac and vlan in MacToMe table
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfTtiMacToMeSet
(
    IN  GT_U32                            entryIndex,
    IN  PRV_TGF_TTI_MAC_VLAN_STC         *valuePtr,
    IN  PRV_TGF_TTI_MAC_VLAN_STC         *maskPtr
)
{
    GT_U8       devNum = 0;
    GT_STATUS   rc, rc1 = GT_OK;

#ifdef CHX_FAMILY
{
    CPSS_DXCH_TTI_MAC_VLAN_STC dxchValue;
    CPSS_DXCH_TTI_MAC_VLAN_STC dxchMask;

    dxchValue.mac = valuePtr->mac;
    dxchValue.vlanId = valuePtr->vlanId;
    dxchMask.mac = maskPtr->mac;
    dxchMask.vlanId = maskPtr->vlanId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChTtiMacToMeSet(devNum, entryIndex, &dxchValue, &dxchMask);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTtiMacToMeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
}
    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
{

    CPSS_EXMXPM_TTI_MAC_VLAN_STC exmxpmValue;
    CPSS_EXMXPM_TTI_MAC_VLAN_STC exmxpmMask;

    exmxpmValue.mac = valuePtr->mac;
    exmxpmValue.vlanId = valuePtr->vlanId;
    exmxpmMask.mac = maskPtr->mac;
    exmxpmMask.vlanId = maskPtr->vlanId;

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
        rc = cpssExMxPmTtiMacToMeSet(devNum, entryIndex, GT_TRUE, &exmxpmValue, &exmxpmMask);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmTtiMacToMeSet FAILED, rc = [%d]", rc);
            rc1 = rc;
        }
    }
}
    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfTtiPortGroupRuleSet
*
* DESCRIPTION:
*       This function sets the TTI Rule Pattern, Mask and Action
*
* INPUTS:
*       devNum     - device number
*       portGroupsBmp - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType    - TTI key type
*       ruleIndex  - index of the tunnel termination entry
*       patternPtr - (pointer to) the rule's pattern
*       maskPtr    - (pointer to) the rule's mask
*       actionPtr  - (pointer to) the TTI rule action
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiPortGroupRuleSet
(
    IN  GT_U32                        ruleIndex,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    IN  PRV_TGF_TTI_RULE_UNT         *patternPtr,
    IN  PRV_TGF_TTI_RULE_UNT         *maskPtr,
    IN  PRV_TGF_TTI_ACTION_STC       *actionPtr
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    CPSS_PP_FAMILY_TYPE_ENT         devFamily  = CPSS_PP_FAMILY_START_DXCH_E;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;
    CPSS_DXCH_TTI_RULE_UNT          dxChPattern;
    CPSS_DXCH_TTI_RULE_UNT          dxChMask;
    CPSS_DXCH_TTI_ACTION_UNT        dxChTtiAction;

    /* reset variables */
    cpssOsMemSet((GT_VOID*) &dxChPattern,   0, sizeof(dxChPattern));
    cpssOsMemSet((GT_VOID*) &dxChMask,      0, sizeof(dxChMask));
    cpssOsMemSet((GT_VOID*) &dxChTtiAction, 0, sizeof(dxChTtiAction));

    /* get device family */
    rc = prvUtfDeviceFamilyGet(prvTgfDevNum, &devFamily);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfDeviceFamilyGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert tti rule into device specific format */
    switch (keyType)
    {
        case PRV_TGF_TTI_KEY_IPV4_E:
            /* convert IPv4 tti rule pattern into device specific format */
            prvTgfConvertGenericToDxChIpv4TtiRule(&(patternPtr->ipv4),
                                                  &(dxChPattern.ipv4));
            /* convert IPv4 tti rule mask into device specific format */
            prvTgfConvertGenericToDxChIpv4TtiRule(&(maskPtr->ipv4),
                                                  &(dxChMask.ipv4));
            break;

        case PRV_TGF_TTI_KEY_MPLS_E:
            /* convert Mpls tti rule pattern into device specific format */
            prvTgfConvertGenericToDxChMplsTtiRule(&(patternPtr->mpls),
                                                  &(dxChPattern.mpls));
            /* convert Mpls tti rule mask into device specific format */
            prvTgfConvertGenericToDxChMplsTtiRule(&(maskPtr->mpls),
                                                  &(dxChMask.mpls));
            break;

        case PRV_TGF_TTI_KEY_ETH_E:
            /* convert Ethernet tti rule pattern into device specific format */
            prvTgfConvertGenericToDxChEthTtiRule(&(patternPtr->eth),
                                                  &(dxChPattern.eth));
            /* convert Ethernet tti rule mask into device specific format */
            prvTgfConvertGenericToDxChEthTtiRule(&(maskPtr->eth),
                                                  &(dxChMask.eth));
            break;

        case PRV_TGF_TTI_KEY_MIM_E:
            /* convert Mac-in-Mac tti rule pattern into device specific format */
            prvTgfConvertGenericToDxChMimTtiRule(&(patternPtr->mim),
                                                 &(dxChPattern.mim));
            /* convert Mac-in-Mac tti rule mask into device specific format */
            prvTgfConvertGenericToDxChMimTtiRule(&(maskPtr->mim),
                                                 &(dxChMask.mim));
            break;

        default:
            return GT_BAD_PARAM;
    }

    if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(prvTgfDevNum))
    {
        /* set rule action type */
        actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

        /* convert tti action type1 into device specific format */
        rc = prvTgfConvertGenericToDxChTtiAction(actionPtr, &dxChTtiAction.type1);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChTtiAction FAILED, rc = [%d]", rc);

            return rc;
        }
    }
    else
    {
        /* set rule action type */
        actionType = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        /* convert tti action type2 into device specific format */
        rc = prvTgfConvertGenericToDxChTtiAction2(actionPtr, &dxChTtiAction.type2);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChTtiAction2 FAILED, rc = [%d]", rc);

            return rc;
        }
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChTtiPortGroupRuleSet(devNum, portGroupsBmp, ruleIndex, (CPSS_DXCH_TTI_KEY_TYPE_ENT) keyType,
                                &dxChPattern, &dxChMask, actionType, &dxChTtiAction);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTtiRuleSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    /* avoid warnings */
    TGF_PARAM_NOT_USED(ruleIndex);
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(keyType);
    TGF_PARAM_NOT_USED(patternPtr);
    TGF_PARAM_NOT_USED(maskPtr);
    TGF_PARAM_NOT_USED(actionPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfTtiPortGroupRuleGet
*
* DESCRIPTION:
*       This function gets the TTI Rule Pattern, Mask and Action
*
* INPUTS:
*       devNum    - device number
*       portGroupsBmp - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType   - TTI key type
*       ruleIndex - index of the tunnel termination entry
*
* OUTPUTS:
*       patternPtr - (pointer to) the rule's pattern
*       maskPtr    - (pointer to) the rule's mask
*       actionPtr  - (pointer to) the TTI rule action
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiPortGroupRuleGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  GT_U32                        ruleIndex,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    OUT PRV_TGF_TTI_RULE_UNT         *patternPtr,
    OUT PRV_TGF_TTI_RULE_UNT         *maskPtr,
    OUT PRV_TGF_TTI_ACTION_STC       *actionPtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    CPSS_PP_FAMILY_TYPE_ENT         devFamily  = CPSS_PP_FAMILY_START_DXCH_E;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;
    CPSS_DXCH_TTI_RULE_UNT          dxChPattern;
    CPSS_DXCH_TTI_RULE_UNT          dxChMask;
    CPSS_DXCH_TTI_ACTION_UNT        dxChTtiAction;

    /* get device family */
    rc = prvUtfDeviceFamilyGet(prvTgfDevNum, &devFamily);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfDeviceFamilyGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* set action type */
    actionType = PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(prvTgfDevNum) ?
                  CPSS_DXCH_TTI_ACTION_TYPE2_ENT : CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

    /* initialize action, pattern and mask */
    cpssOsMemSet(&dxChTtiAction, 0, sizeof(dxChTtiAction));
    cpssOsMemSet(&dxChPattern, 0, sizeof(dxChPattern));
    cpssOsMemSet(&dxChMask, 0, sizeof(dxChMask));


    /* call device specific API */
    rc = cpssDxChTtiPortGroupRuleGet(devNum, portGroupsBmp, ruleIndex, (CPSS_DXCH_TTI_KEY_TYPE_ENT) keyType,
                            &dxChPattern, &dxChMask, actionType, &dxChTtiAction);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTtiRuleGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert tti rule from device specific format */
    switch (keyType)
    {
        case PRV_TGF_TTI_KEY_IPV4_E:
            /* convert IPv4 tti rule pattern from device specific format */
            prvTgfConvertDxChToGenericIpv4TtiRule(&(dxChPattern.ipv4),
                                                  &(patternPtr->ipv4));
            /* convert IPv4 tti rule mask from device specific format */
            prvTgfConvertDxChToGenericIpv4TtiRule(&(dxChMask.ipv4),
                                                  &(maskPtr->ipv4));
            break;

        case PRV_TGF_TTI_KEY_MPLS_E:
            /* convert Mpls tti rule pattern from device specific format */
            prvTgfConvertDxChToGenericMplsTtiRule(&(dxChPattern.mpls),
                                                  &(patternPtr->mpls));
            /* convert Mpls tti rule mask from device specific format */
            prvTgfConvertDxChToGenericMplsTtiRule(&(dxChMask.mpls),
                                                  &(maskPtr->mpls));
            break;

        case PRV_TGF_TTI_KEY_ETH_E:
            /* convert Ethernet tti rule pattern from device specific format */
            prvTgfConvertDxChToGenericEthTtiRule(&(dxChPattern.eth),
                                                 &(patternPtr->eth));
            /* convert Ethernet tti rule mask from device specific format */
            prvTgfConvertDxChToGenericEthTtiRule(&(dxChMask.eth),
                                                 &(maskPtr->eth));
            break;

        case PRV_TGF_TTI_KEY_MIM_E:
            /* convert Mac-in-Mac tti rule pattern from device specific format */
            prvTgfConvertDxChToGenericMimTtiRule(&(dxChPattern.mim),
                                                 &(patternPtr->mim));
            /* convert Mac-in-Mac tti rule mask from device specific format */
            prvTgfConvertDxChToGenericMimTtiRule(&(dxChMask.mim),
                                                 &(maskPtr->mim));
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert tti action */
    if (CPSS_DXCH_TTI_ACTION_TYPE1_ENT == actionType)
    {
        /* convert tti type1 action from device specific format */
        return prvTgfConvertDxChToGenericTtiAction(&dxChTtiAction.type1, actionPtr);
    }
    else
    {
        /* convert tti type2 action from device specific format */
        return prvTgfConvertDxChToGenericTtiAction2(&dxChTtiAction.type2, actionPtr);
    }
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(ruleIndex);
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(keyType);
    TGF_PARAM_NOT_USED(patternPtr);
    TGF_PARAM_NOT_USED(maskPtr);
    TGF_PARAM_NOT_USED(actionPtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfTtiPortGroupMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type
*
* INPUTS:
*       devNum  - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType - TTI key type
*       macMode - MAC mode to use
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiPortGroupMacModeSet
(
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  PRV_TGF_TTI_MAC_MODE_ENT      macMode
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    CPSS_DXCH_TTI_KEY_TYPE_ENT      dxChKeyType = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_MAC_MODE_ENT      dxChMacMode = CPSS_DXCH_TTI_MAC_MODE_DA_E;

    /* convert keyType into device specific format */
    PRV_TGF_S2D_KEY_TYPE_CONVERT_MAC(dxChKeyType, keyType);

    /* convert macMode into device specific format */
    PRV_TGF_S2D_MAC_MODE_CONVERT_MAC(dxChMacMode, macMode);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChTtiPortGroupMacModeSet(devNum, portGroupsBmp, dxChKeyType, dxChMacMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTtiMacModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    /* avoid warnings */
    TGF_PARAM_NOT_USED(keyType);
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(macMode);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfTtiPortGroupMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type
*
* INPUTS:
*       devNum  - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       keyType - TTI key type
*
* OUTPUTS:
*       macModePtr - (pointer to) MAC mode to use
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiPortGroupMacModeGet
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  PRV_TGF_TTI_KEY_TYPE_ENT      keyType,
    OUT PRV_TGF_TTI_MAC_MODE_ENT     *macModePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc = GT_OK;

    CPSS_DXCH_TTI_KEY_TYPE_ENT      dxChKeyType = CPSS_DXCH_TTI_KEY_IPV4_E;
    CPSS_DXCH_TTI_MAC_MODE_ENT      dxChMacMode = CPSS_DXCH_TTI_MAC_MODE_DA_E;

    /* convert keyType into device specific format */
    PRV_TGF_S2D_KEY_TYPE_CONVERT_MAC(dxChKeyType, keyType);

    /* call device specific API */
    rc = cpssDxChTtiPortGroupMacModeGet(devNum, portGroupsBmp, dxChKeyType, &dxChMacMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTtiMacModeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert macMode from device specific format */
    PRV_TGF_D2S_MAC_MODE_CONVERT_MAC(dxChMacMode, *macModePtr);

    return rc;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(keyType);
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(macModePtr);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfTtiPortGroupRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM
*
* INPUTS:
*       devNum            - device number
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       If valid == GT_TRUE it is assumed that the TCAM entry already contains
*       the TTI entry information.
*
*******************************************************************************/
GT_STATUS prvTgfTtiPortGroupRuleValidStatusSet
(
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  GT_U32                        routerTtiTcamRow,
    IN  GT_BOOL                       valid
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChTtiPortGroupRuleValidStatusSet(devNum, portGroupsBmp, routerTtiTcamRow, valid);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChTtiMimEthTypeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)
    /* avoid warnings */
    TGF_PARAM_NOT_USED(portGroupsBmp);
    TGF_PARAM_NOT_USED(routerTtiTcamRow);
    TGF_PARAM_NOT_USED(valid);

    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgTtiCheck
*
* DESCRIPTION:
*    check if the device supports the tti
* INPUTS:
*       none
*
* RETURNS:
*       GT_TRUE - the device supports the tti
*       GT_FALSE - the device not supports the tti
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvTgfBrgTtiCheck(
    void
)
{
#ifdef CHX_FAMILY
    GT_STATUS rc;
    GT_U8     devNum;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        if(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerAndTunnelTermTcam == 0)
        {
            /* the device not support the tti */
            return GT_FALSE;
        }
    }

    return GT_TRUE;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)

    return GT_TRUE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfTtiIpv4GreEthTypeSet
*
* DESCRIPTION:
*       This function sets the IPv4 GRE protocol Ethernet type.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type
*       ethType       - Ethernet type value (range 16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiIpv4GreEthTypeSet
(
    IN  GT_U8                               devNum,
    IN  PRV_TGF_TUNNEL_GRE_ETHER_TYPE_ENT   greTunnelType,
    IN  GT_U32                              ethType
)
{
#ifdef CHX_FAMILY
    CPSS_DXCH_TTI_IPV4_GRE_TYPE_ENT dxChGreTunnelType;

    /* convert greTunnelType into device specific format */
    switch (greTunnelType)
    {
        case PRV_TGF_TUNNEL_GRE0_ETHER_TYPE_E:
            dxChGreTunnelType = CPSS_DXCH_TTI_IPV4_GRE0_E;
            break;

        case PRV_TGF_TUNNEL_GRE1_ETHER_TYPE_E:
            dxChGreTunnelType = CPSS_DXCH_TTI_IPV4_GRE1_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    return cpssDxChTtiIpv4GreEthTypeSet(devNum, dxChGreTunnelType, ethType);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_TUNNEL_GRE_ETHER_TYPE_ENT exMxPmGreTunnelType;

    /* convert greTunnelType into device specific format */
    switch (greTunnelType)
    {
        case PRV_TGF_TUNNEL_GRE0_ETHER_TYPE_E:
            exMxPmGreTunnelType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;
            break;

        case PRV_TGF_TUNNEL_GRE1_ETHER_TYPE_E:
            exMxPmGreTunnelType = CPSS_TUNNEL_GRE1_ETHER_TYPE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    return cpssExMxPmTtiIpv4GreEthTypeSet(devNum, exMxPmGreTunnelType, ethType);
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTtiIpv4GreEthTypeGet
*
* DESCRIPTION:
*       This function gets the IPv4 GRE protocol Ethernet type.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type
*
* OUTPUTS:
*       ethTypePtr - (pointer to) Ethernet type value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTtiIpv4GreEthTypeGet
(
    IN  GT_U8                               devNum,
    IN  PRV_TGF_TUNNEL_GRE_ETHER_TYPE_ENT   greTunnelType,
    OUT GT_U32                              *ethTypePtr
)
{
#ifdef CHX_FAMILY
    CPSS_DXCH_TTI_IPV4_GRE_TYPE_ENT dxChGreTunnelType;

    /* convert greTunnelType into device specific format */
    switch (greTunnelType)
    {
        case PRV_TGF_TUNNEL_GRE0_ETHER_TYPE_E:
            dxChGreTunnelType = CPSS_DXCH_TTI_IPV4_GRE0_E;
            break;

        case PRV_TGF_TUNNEL_GRE1_ETHER_TYPE_E:
            dxChGreTunnelType = CPSS_DXCH_TTI_IPV4_GRE1_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    return cpssDxChTtiIpv4GreEthTypeGet(devNum, dxChGreTunnelType, ethTypePtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_TUNNEL_GRE_ETHER_TYPE_ENT exMxPmGreTunnelType;

    /* convert greTunnelType into device specific format */
    switch (greTunnelType)
    {
        case PRV_TGF_TUNNEL_GRE0_ETHER_TYPE_E:
            exMxPmGreTunnelType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;
            break;

        case PRV_TGF_TUNNEL_GRE1_ETHER_TYPE_E:
            exMxPmGreTunnelType = CPSS_TUNNEL_GRE1_ETHER_TYPE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    return cpssExMxPmTtiIpv4GreEthTypeGet(devNum, exMxPmGreTunnelType, ethTypePtr);
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}



/*******************************************************************************
* prvTgfTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
*
* DESCRIPTION:
*       This feature allows overriding the <total length> in the IP header.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       This API enables this feature per port.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* INPUTS:
*       devNum - device number
*       port   - port number
*       enable - GT_TRUE: Add offset to tunnel total length
*                GT_FALSE: Don't add offset to tunnel total length
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong value in any of the parameters
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    IN  GT_BOOL enable
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet(devNum, port, enable);
#endif /* CHX_FAMILY */

#if (!defined CHX_FAMILY)

    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(port);
    TGF_PARAM_NOT_USED(enable);
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfTunnelStartIpTunnelTotalLengthOffsetSet
*
* DESCRIPTION:
*       This API sets the value for <IP Tunnel Total Length Offset>.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* INPUTS:
*       devNum           - device number
*       additionToLength - Ip tunnel total length offset (6 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong value in any of the parameters
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - on out of range values
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfTunnelStartIpTunnelTotalLengthOffsetSet
(
    IN  GT_U8  devNum,
    IN  GT_U32 additionToLength
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet(devNum, additionToLength);
#endif /* CHX_FAMILY */

#if (!defined CHX_FAMILY)

    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(additionToLength);
    return GT_BAD_STATE;
#endif
}


