/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmIpTablesUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmIpTables, that provides
*       The CPSS EXMXPM Ip APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
#include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpTypes.h>
#include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpTables.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* number of Arp Entry entries */
#define PRV_CPSS_EXMXPM_IP_ARP_ENTRY_INDEX_NUM_MAX_CNS      16383

/* IP module pointer */
#define PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)           \
                            (&(PRV_CPSS_EXMXPM_DEV_MODULE_CFG_MAC(dev)->ipCfg))

/* Invalid enum */
#define IP_TABLES_INVALID_ENUM_CNS    0x5AAAAAA5

/* Tests use this vlan id for testing VLAN functions */
#define IP_TABLES_TESTED_VLAN_ID_CNS  100

/* Internal functions forward declaration       */
/* Get Get Max Size of ARP / Tunnel start Table */
static GT_U32 prvGetMaxARPTableSize (IN GT_U8 dev);

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpUcRouteEntriesWrite
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              ucMcRouteEntryBaseIndex,
    IN  GT_U32                              numOfRouteEntries,
    IN  CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC   *routeEntriesArrayPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpUcRouteEntriesWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ucMcRouteEntryBaseIndex [0],
                   numOfRouteEntries [1],
                   routeEntriesArrayPtr {type  {CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E},
                                         entry { regularEntry{
                                                              command [CPSS_PACKET_CMD_ROUTE_E],                    --> [0x5AAAAAA5] GT_BAD_PARAM, [CPSS_PACKET_CMD_FORWARD_E] not supported
                                                              bypassTtlExceptionCheckEnable [GT_TRUE],
                                                              cpuCodeIndex [3],                                     --> [4] NOT_GT_OK
                                                              ageRefresh [GT_TRUE],
                                                              ipv6ScopeCheckEnable [GT_FALSE],
                                                              ipv6DestinationSiteId [CPSS_IP_SITE_ID_INTERNAL_E],   --> [0x5AAAAAA5] GT_BAD_PARAM
                                                              sipAccessLevel [7],                                   --> [8] NOT_GT_OK
                                                              dipAccessLevel [7],                                   --> [8] NOT_GT_OK
                                                              ucRpfOrIcmpRedirectIfMode [CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E], --> [0x5AAAAAA5] GT_BAD_PARAM, [CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E] NOT_GT_OK,
                                                              sipSaCheckEnable [GT_TRUE],
                                                              trapMirrorArpBcToCpu [GT_TRUE],
                                                              ucPacketSipFilterEnable [GT_TRUE],
                                                              nextHopIf {
                                                                          outlifType [CPSS_EXMXPM_OUTLIF_TYPE_LL_E],    --> [0x5AAAAAA5] GT_BAD_PARAM
                                                                          interfaceInfo {
                                                                                          type [CPSS_INTERFACE_TRUNK_E],
                                                                                          trunkId [100] }
                                                                          outlifPointer { arpPtr [100] } },
                                                              nextHopVlanId [10],                                   --> [4096] NOT GT_OK
                                                              mirrorToIngressAnalyzer [GT_TRUE],
                                                              cntrSetIndex [CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E], --> [0x5AAAAAA5] GT_BAD_PARAM
                                                              mtuProfileIndex [15],                                --> [16] NOT_GT_OK
                                                              icmpRedirectEnable [GT_TRUE],
                                                              appSpecificCpuCodeEnable [GT_FALSE],
                                                              hopLimitMode [CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E], --> [0x5AAAAAA5] GT_BAD_PARAM
                                                              ttl [0],                                             --> [256] NOT GT_OK
                                                              ttlMode [CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E],      --> [0x5AAAAAA5] GT_BAD_PARAM
                                                              mplsLabel [0],                                       --> [BIT_20] NOT GT_OK
                                                              mplsCommand [CPSS_EXMXPM_MPLS_PUSH_CMD_E],
                                                              qosParamsModify { modifyTc [GT_FALSE],
                                                                                modifyUp [GT_FALSE],
                                                                                modifyDp [GT_FALSE],
                                                                                modifyDscp [GT_FALSE],
                                                                                modifyExp [GT_FALSE] },
                                                              qosParams {   tc [0],                                 --> [8] NOT GT_OK
                                                                            dp [0],                                 --> [4] NOT GT_OK
                                                                            up [0],                                 --> [8] NOT GT_OK
                                                                            dscp [0],                               --> [64] NOT GT_OK
                                                                            exp [0] } } }                           --> [8] NOT GT_OK
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpUcRouteEntriesRead with not NULL routeEntriesArrayPtr and other params from 1.1.
    Expected: GT_OK and the same params as was set.
    1.3. Call with routeEntriesArrayPtr->type [0x5AAAAAA5] out of range
                   and other params from 1.1.
    Expected: GT_BAD_PARAM
    1.4. Call with routeEntriesArrayPtr->entry.regularEntry.command [CPSS_PACKET_CMD_FORWARD_E] not supported
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with routeEntriesArrayPtr->entry.regularEntry.command [0x5AAAAAA5] out of range
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with routeEntriesArrayPtr->entry.regularEntry.command [CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E / CPSS_PACKET_CMD_TRAP_TO_CPU_E]
                   routeEntriesArrayPtr->entry.regularEntry.cpuCodeIndex [4] (out of range)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with routeEntriesArrayPtr->entry.regularEntry.ipv6DestinationSiteId [0x5AAAAAA5] (out of range)
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with routeEntriesArrayPtr->entry.regularEntry.sipAccessLevel [8] (out of range)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.10. Call with routeEntriesArrayPtr->entry.regularEntry.dipAccessLevel [8] (out of range)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.11. Call with routeEntriesArrayPtr->entry.regularEntry.ucRpfOrIcmpRedirectIfMode [0x5AAAAAA5] (out of range)
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.12. Call with routeEntriesArrayPtr->entry.regularEntry.ucRpfOrIcmpRedirectIfMode [CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.13. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopVlanId [0x4000] (out of range)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.14. Call with routeEntriesArrayPtr->entry.regularEntry.hopLimitMode [0x5AAAAAA5] (out of range)
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.15. Call with routeEntriesArrayPtr->entry.regularEntry.ttl [256] (out of range)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.16. Call with routeEntriesArrayPtr->entry.regularEntry.mplsLabel [BIT_20] (out of range)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.17. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyTc [GT_TRUE],
                   routeEntriesArrayPtr->entry.regularEntry.qosParams->tc [8] (out of range)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.18. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyTc [GT_FALSE],
                   routeEntriesArrayPtr->entry.regularEntry.qosParams->tc [8] (not relevant)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.19. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyUp [GT_TRUE],
                   routeEntriesArrayPtr->entry.regularEntry.qosParams->up [8] (out of range)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.20. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyUp [GT_FALSE],
                   routeEntriesArrayPtr->entry.regularEntry.qosParams->up [8] (not relevant)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.21. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyDp [GT_TRUE],
                   routeEntriesArrayPtr->entry.regularEntry.qosParams->dp [4] (out of range)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.22. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyDp [GT_FALSE],
                   routeEntriesArrayPtr->entry.regularEntry.qosParams->dp [4] (not relevant)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.23. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyDscp [GT_TRUE],
                   routeEntriesArrayPtr->entry.regularEntry.qosParams->dscp [64] (out of range)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.24. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyDscp [GT_FALSE],
                   routeEntriesArrayPtr->entry.regularEntry.qosParams->dscp [64] (not relevant)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.25. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyExp [GT_TRUE],
                   routeEntriesArrayPtr->entry.regularEntry.qosParams->exp [8] (out of range)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.26. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyExp [GT_FALSE],
                   routeEntriesArrayPtr->entry.regularEntry.qosParams->exp [8] (not relevant)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.27. Call with routeEntriesArrayPtr->entry.regularEntry.ucRpfOrIcmpRedirectIfMode [0x5AAAAAA5] out of range
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.28. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.outlifType [0x5AAAAAA5] out of range
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.29. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_DEVICE_E] not supported
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.30. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [0x5AAAAAA5] out of range
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.31. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                    routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] out of range
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.32. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                    routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] out of range
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.33. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                    routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] not relevant
                   and other params from 1.1.
    Expected: GT_OK.
    1.34. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                    routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant
                   and other params from 1.1.
    Expected: GT_OK.
    1.35. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                    routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.trunkId [4096] (out of range)
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.36. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                    routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.trunkId [4096] (not relevant)
                   and other params from 1.1.
    Expected: GT_OK.

    1.37. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                    routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.vidx [4096] out of range
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.38. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_FABRIC_VIDX_E]
                    routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.vidx [4096] not relevant
                   and other params from 1.1.
    Expected: GT_OK.
    1.39. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_FABRIC_VIDX_E]
                    routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx [4096] out of range
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.40. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                    routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx [4096] not relevant
                   and other params from 1.1.
    Expected: GT_OK.
    1.41. Call with routeEntriesArrayPtr->entry.regularEntry.mirrorToIngressAnalyzer [2] out of range
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.42. Call with routeEntriesArrayPtr->entry.regularEntry.cntrSetIndex [0x5AAAAAA5] out of range
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.43. Call with routeEntriesArrayPtr->entry.regularEntry.mtuProfileIndex [16] out of range
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.44. Call with routeEntriesArrayPtr->entry.regularEntry.ttlMode [0x5AAAAAA5] out of range
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.45. Call with routeEntriesArrayPtr->entry.regularEntry.mplsCommand [0x5AAAAAA5] out of range
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.

[ CPSS_EXMXPM_IP_UC_ECMP_RPF_E ]
    1.46. Call with ucMcRouteEntryBaseIndex [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1],
                    numOfRouteEntries [1],
                    routeEntriesArrayPtr {type  {CPSS_EXMXPM_IP_UC_ECMP_RPF_E},
                                          entry { rpfIcmpMode [CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E],
                                                  sipFilterEnable [GT_TRUE],
                                                  sipAccessLevel [1],
                                                  rpfIf[8]{valid[GT_TRUE], interface{set to trunk} } }
    Expected: GT_OK.
    1.47. Call cpssExMxPmIpUcRouteEntriesRead with not NULL routeEntriesArrayPtr and other params from 1.25.
    Expected: GT_OK and the same params as was set.
    1.48. Call with  routeEntriesArrayPtr->entry.rpfIcmpMode [0x5AAAAAA5] not supported
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.49. Call with  routeEntriesArrayPtr->entry.rpfIcmpMode [CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E] not supported
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.50. Call with  routeEntriesArrayPtr->entry.sipAccessLevel [8] not supported
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.51. Call with routeEntriesArrayPtr->entry.rpfIf[0].interface.type [0x5AAAAAA5] out of range
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.52. Call with routeEntriesArrayPtr->entry.rpfIcmpMode [CPSS_EXMXPM_RPF_IF_MODE_PORT_E],
                    routeEntriesArrayPtr->entry.rpfIf[0].interface.type [CPSS_INTERFACE_PORT_E],
                    routeEntriesArrayPtr->entry.rpfIf[0].interface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.53. Call with routeEntriesArrayPtr->entry.rpfIcmpMode [CPSS_EXMXPM_RPF_IF_MODE_PORT_E],
                    routeEntriesArrayPtr->entry.rpfIf[0].interface.type [CPSS_INTERFACE_TRUNK_E],
                    routeEntriesArrayPtr->entry.rpfIf[0].interface.trunkId [4096] (out of range)
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.54. Call with routeEntriesArrayPtr->entry.rpfIcmpMode [CPSS_EXMXPM_RPF_IF_MODE_VLAN_INLIF_E],
                    routeEntriesArrayPtr->entry.rpfIf[0].interface.type [CPSS_INTERFACE_VID_E],
                    routeEntriesArrayPtr->entry.rpfIf[0].interface.devPort.vlanId [4096] (out of range)
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.55. Call with routeEntriesArrayPtr->entry.rpfIf[0].interface.type [CPSS_INTERFACE_VIDX_E /
                                                                         CPSS_INTERFACE_DEVICE_E /
                                                                         CPSS_INTERFACE_FABRIC_VIDX_E] (out of range)
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.56. Call with routeEntriesArrayPtr->entry.rpfIcmpMode [CPSS_EXMXPM_RPF_IF_MODE_PORT_E],
                    routeEntriesArrayPtr->entry.rpfIf[0].interface.type [CPSS_INTERFACE_VID_E] (not supported to rpfIcmpMode),
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.57. Call with routeEntriesArrayPtr->entry.rpfIcmpMode [CPSS_EXMXPM_RPF_IF_MODE_VLAN_INLIF_E],
                    routeEntriesArrayPtr->entry.rpfIf[0].interface.type [CPSS_INTERFACE_TRUNK_E] (not supported to rpfIcmpMode),
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.58. Call with routeEntriesArrayPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     isEqual = GT_FALSE;

    GT_U32                              ucMcRouteEntryBaseIndex = 0;
    GT_U32                              numOfRouteEntries       = 0;
    CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC   routeEntry;
    CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC   routeEntryGet;


    cpssOsBzero((GT_VOID*) &routeEntry, sizeof(routeEntry));
    cpssOsBzero((GT_VOID*) &routeEntryGet, sizeof(routeEntryGet));


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ucMcRouteEntryBaseIndex [0],
                           numOfRouteEntries [1],
                           routeEntriesArrayPtr {type  {CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E},
                                                 entry { regularEntry{
                                                                      command [CPSS_PACKET_CMD_ROUTE_E],
                                                                      bypassTtlExceptionCheckEnable [GT_TRUE],
                                                                      cpuCodeIndex [3],
                                                                      ageRefresh [GT_TRUE],
                                                                      ipv6ScopeCheckEnable [GT_FALSE],
                                                                      ipv6DestinationSiteId [CPSS_IP_SITE_ID_INTERNAL_E],
                                                                      sipAccessLevel [7],
                                                                      dipAccessLevel [7],
                                                                      ucRpfOrIcmpRedirectIfMode [CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E],
                                                                      sipSaCheckEnable [GT_TRUE],
                                                                      trapMirrorArpBcToCpu [GT_TRUE],
                                                                      ucPacketSipFilterEnable [GT_TRUE],
                                                                      nextHopIf {
                                                                                  outlifType [CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                                  interfaceInfo {
                                                                                                  type [CPSS_INTERFACE_TRUNK_E],
                                                                                                  trunkId [100] }
                                                                                  outlifPointer { arpPtr [100] } },
                                                                      nextHopVlanId [10],
                                                                      mirrorToIngressAnalyzer [GT_TRUE],
                                                                      cntrSetIndex [CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E],
                                                                      mtuProfileIndex [15],
                                                                      icmpRedirectEnable [GT_TRUE],
                                                                      appSpecificCpuCodeEnable [GT_FALSE], nr
                                                                      hopLimitMode [CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E],
                                                                      ttlIndex [0],
                                                                      ttlMode [CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E],
                                                                      mplsLabel [0],
                                                                      mplsCommand [CPSS_EXMXPM_MPLS_PUSH_CMD_E],
                                                                      qosParamsModify { modifyTc [GT_FALSE],
                                                                                        modifyUp [GT_FALSE],
                                                                                        modifyDp [GT_FALSE],
                                                                                        modifyDscp [GT_FALSE],
                                                                                        modifyExp [GT_FALSE] },
                                                                      qosParams {   tc [0],
                                                                                    dp [0],
                                                                                    up [0],
                                                                                    dscp [0],
                                                                                    exp [0] } } }
            Expected: GT_OK.
        */
        ucMcRouteEntryBaseIndex = 0;
        numOfRouteEntries = 1;

        routeEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        routeEntry.entry.regularEntry.command = CPSS_PACKET_CMD_ROUTE_E;
        routeEntry.entry.regularEntry.bypassTtlExceptionCheckEnable = GT_TRUE;
        routeEntry.entry.regularEntry.cpuCodeIndex = 3;
        routeEntry.entry.regularEntry.ageRefresh = GT_TRUE;
        routeEntry.entry.regularEntry.ipv6ScopeCheckEnable = GT_FALSE;
        routeEntry.entry.regularEntry.ipv6DestinationSiteId = CPSS_IP_SITE_ID_INTERNAL_E;
        routeEntry.entry.regularEntry.sipAccessLevel = 7;
        routeEntry.entry.regularEntry.dipAccessLevel = 7;
        routeEntry.entry.regularEntry.ucRpfOrIcmpRedirectIfMode = CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E;
        routeEntry.entry.regularEntry.sipSaCheckEnable = GT_TRUE;
        routeEntry.entry.regularEntry.trapMirrorArpBcToCpu = GT_TRUE;
        routeEntry.entry.regularEntry.ucPacketSipFilterEnable = GT_TRUE;

        routeEntry.entry.regularEntry.nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.trunkId = 100;
        routeEntry.entry.regularEntry.nextHopIf.outlifPointer.arpPtr = 100;

        routeEntry.entry.regularEntry.nextHopVlanId = 10;
        routeEntry.entry.regularEntry.mirrorToIngressAnalyzer = GT_TRUE;
        routeEntry.entry.regularEntry.cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E;
        routeEntry.entry.regularEntry.mtuProfileIndex = 15;
        routeEntry.entry.regularEntry.icmpRedirectEnable = GT_TRUE;
        routeEntry.entry.regularEntry.appSpecificCpuCodeEnable = GT_FALSE;
        routeEntry.entry.regularEntry.hopLimitMode = CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
        routeEntry.entry.regularEntry.ttl = 0;
        routeEntry.entry.regularEntry.ttlMode = CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
        routeEntry.entry.regularEntry.mplsLabel = 0;
        routeEntry.entry.regularEntry.mplsCommand = CPSS_EXMXPM_MPLS_PUSH_CMD_E;

        routeEntry.entry.regularEntry.qosParamsModify.modifyTc = GT_FALSE;
        routeEntry.entry.regularEntry.qosParamsModify.modifyUp = GT_FALSE;
        routeEntry.entry.regularEntry.qosParamsModify.modifyDp = GT_FALSE;
        routeEntry.entry.regularEntry.qosParamsModify.modifyDscp = GT_FALSE;
        routeEntry.entry.regularEntry.qosParamsModify.modifyExp = GT_FALSE;

        routeEntry.entry.regularEntry.qosParams.tc = 0;
        routeEntry.entry.regularEntry.qosParams.dp = CPSS_DP_GREEN_E;
        routeEntry.entry.regularEntry.qosParams.up = 0;
        routeEntry.entry.regularEntry.qosParams.dscp = 0;
        routeEntry.entry.regularEntry.qosParams.exp = 0;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);

        /*
            1.2. Call cpssExMxPmIpUcRouteEntriesRead with not NULL routeEntriesArrayPtr and other params from 1.1.
            Expected: GT_OK and the same params as was set.
        */
        routeEntryGet.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        st = cpssExMxPmIpUcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntryGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpUcRouteEntriesRead: %d, %d, %d",
                                    dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);

        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.type, routeEntryGet.type, "got another routeEntry.type than was set", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &routeEntry.entry.regularEntry,
                                     (GT_VOID*) &routeEntryGet.entry.regularEntry,
                                     sizeof(routeEntry.entry.regularEntry) )) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another routeEntry.entry.regularEntry than was set", dev);

        /*
            1.3. Call with routeEntriesArrayPtr->type [0x5AAAAAA5] out of range
                           and other params from 1.1.
            Expected: GT_BAD_PARAM
        */
        routeEntry.type = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntryPtr->type = %d", dev, routeEntry.type);

        routeEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        /*
            1.4. Call with routeEntriesArrayPtr->entry.regularEntry.command [CPSS_PACKET_CMD_FORWARD_E] not supported
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.command = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.command = %d",
                                     dev, routeEntry.entry.regularEntry.command);

        routeEntry.entry.regularEntry.command = CPSS_PACKET_CMD_ROUTE_E;

        /*
            1.5. Call with routeEntriesArrayPtr->entry.regularEntry.command [0x5AAAAAA5] out of range
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.entry.regularEntry.command = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntryPtr->entry.regularEntry.command = %d",
                                     dev, routeEntry.entry.regularEntry.command);

        routeEntry.entry.regularEntry.command = CPSS_PACKET_CMD_ROUTE_E;
        routeEntry.entry.regularEntry.ucRpfOrIcmpRedirectIfMode = CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E;

/* ---- ! */
        /*
            1.6. Call with routeEntriesArrayPtr->entry.regularEntry.command [CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E / CPSS_PACKET_CMD_TRAP_TO_CPU_E]
                           routeEntriesArrayPtr->entry.regularEntry.cpuCodeIndex [4] (out of range)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.command = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
        routeEntry.entry.regularEntry.cpuCodeIndex = 4;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.command = %d, routeEntry.entry.regularEntry.cpuCodeIndex = %d",
                                     dev, routeEntry.entry.regularEntry.command, routeEntry.entry.regularEntry.cpuCodeIndex);

        routeEntry.entry.regularEntry.command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
        routeEntry.entry.regularEntry.cpuCodeIndex = 4;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.command = %d, routeEntry.entry.regularEntry.cpuCodeIndex = %d",
                                     dev, routeEntry.entry.regularEntry.command, routeEntry.entry.regularEntry.cpuCodeIndex);

        routeEntry.entry.regularEntry.cpuCodeIndex = 0;

        /*
            1.8. Call with routeEntriesArrayPtr->entry.regularEntry.ipv6DestinationSiteId [0x5AAAAAA5] (out of range)
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.entry.regularEntry.command = CPSS_PACKET_CMD_ROUTE_E;
        routeEntry.entry.regularEntry.ipv6DestinationSiteId = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntryPtr->entry.regularEntry.ipv6DestinationSiteId = %d",
                                     dev, routeEntry.entry.regularEntry.ipv6DestinationSiteId);

        routeEntry.entry.regularEntry.ipv6DestinationSiteId = CPSS_IP_SITE_ID_INTERNAL_E;

        /*
            1.9. Call with routeEntriesArrayPtr->entry.regularEntry.sipAccessLevel [8] (out of range)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.sipAccessLevel = 8;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.sipAccessLevel = %d",
                                         dev, routeEntry.entry.regularEntry.sipAccessLevel);

        routeEntry.entry.regularEntry.sipAccessLevel = 0;

        /*
            1.10. Call with routeEntriesArrayPtr->entry.regularEntry.dipAccessLevel [8] (out of range)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.dipAccessLevel = 8;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.dipAccessLevel = %d",
                                         dev, routeEntry.entry.regularEntry.dipAccessLevel);

        routeEntry.entry.regularEntry.dipAccessLevel = 0;

        /*
            1.11. Call with routeEntriesArrayPtr->entry.regularEntry.ucRpfOrIcmpRedirectIfMode [0x5AAAAAA5] (out of range)
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.entry.regularEntry.ucRpfOrIcmpRedirectIfMode = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntryPtr->entry.regularEntry.ucRpfOrIcmpRedirectIfMode = %d",
                                     dev, routeEntry.entry.regularEntry.ucRpfOrIcmpRedirectIfMode);

        routeEntry.entry.regularEntry.ucRpfOrIcmpRedirectIfMode = CPSS_EXMXPM_RPF_IF_MODE_PORT_E;

        /*
            1.12. Call with routeEntriesArrayPtr->entry.regularEntry.ucRpfOrIcmpRedirectIfMode [CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.ucRpfOrIcmpRedirectIfMode = CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.ucRpfOrIcmpRedirectIfMode = %d",
                                         dev, routeEntry.entry.regularEntry.ucRpfOrIcmpRedirectIfMode);

        routeEntry.entry.regularEntry.ucRpfOrIcmpRedirectIfMode = CPSS_EXMXPM_RPF_IF_MODE_PORT_E;

        /*
            1.13. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopVlanId [0x4000] (out of range)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.nextHopVlanId = 0x4000;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.nextHopVlanId = %d",
                                         dev, routeEntry.entry.regularEntry.nextHopVlanId);

        routeEntry.entry.regularEntry.nextHopVlanId = 100;

        /*
            1.14. Call with routeEntriesArrayPtr->entry.regularEntry.hopLimitMode [0x5AAAAAA5] (out of range)
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.entry.regularEntry.hopLimitMode = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntryPtr->entry.regularEntry.hopLimitMode = %d",
                                     dev, routeEntry.entry.regularEntry.hopLimitMode);

        routeEntry.entry.regularEntry.hopLimitMode = CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;

        /*
            1.15. Call with routeEntriesArrayPtr->entry.regularEntry.ttl [256] (out of range)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.ttl = 256;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.ttl = %d",
                                         dev, routeEntry.entry.regularEntry.ttl);

        routeEntry.entry.regularEntry.ttl = 0;

        /*
            1.16. Call with routeEntriesArrayPtr->entry.regularEntry.mplsLabel [BIT_20] (out of range)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.mplsLabel = BIT_20;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.mplsLabel = %d",
                                         dev, routeEntry.entry.regularEntry.mplsLabel);

        routeEntry.entry.regularEntry.mplsLabel = 0;

        /*
            1.17. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyTc [GT_TRUE],
                           routeEntriesArrayPtr->entry.regularEntry.qosParams->tc [8] (out of range)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.qosParamsModify.modifyTc = GT_TRUE;
        routeEntry.entry.regularEntry.qosParams.tc = 8;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.regularEntry.qosParamsModify.modifyTc = %d, routeEntry.entry.regularEntry.qosParams.tc = %d",
                                         dev, routeEntry.entry.regularEntry.qosParamsModify.modifyTc,
                                         routeEntry.entry.regularEntry.qosParams.tc);

        routeEntry.entry.regularEntry.qosParams.tc = 0;

        /*
            1.18. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyTc [GT_FALSE],
                           routeEntriesArrayPtr->entry.regularEntry.qosParams->tc [8] (not relevant)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        routeEntry.entry.regularEntry.qosParamsModify.modifyTc = GT_FALSE;
        routeEntry.entry.regularEntry.qosParams.tc = 8;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.regularEntry.qosParamsModify.modifyTc = %d, routeEntry.entry.regularEntry.qosParams.tc = %d",
                                     dev, routeEntry.entry.regularEntry.qosParamsModify.modifyTc,
                                     routeEntry.entry.regularEntry.qosParams.tc);

        routeEntry.entry.regularEntry.qosParams.tc = 0;

        /*
            1.19. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyUp [GT_TRUE],
                           routeEntriesArrayPtr->entry.regularEntry.qosParams->up [8] (out of range)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.qosParamsModify.modifyUp = GT_TRUE;
        routeEntry.entry.regularEntry.qosParams.up = 8;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.regularEntry.qosParamsModify.modifyUp = %d, routeEntry.entry.regularEntry.qosParams.up = %d",
                                         dev, routeEntry.entry.regularEntry.qosParamsModify.modifyUp,
                                         routeEntry.entry.regularEntry.qosParams.up);

        routeEntry.entry.regularEntry.qosParams.up = 0;

        /*
            1.20. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyUp [GT_FALSE],
                           routeEntriesArrayPtr->entry.regularEntry.qosParams->up [8] (not relevant)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        routeEntry.entry.regularEntry.qosParamsModify.modifyUp = GT_FALSE;
        routeEntry.entry.regularEntry.qosParams.up = 8;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.regularEntry.qosParamsModify.modifyUp = %d, routeEntry.entry.regularEntry.qosParams.up = %d",
                                     dev, routeEntry.entry.regularEntry.qosParamsModify.modifyUp,
                                     routeEntry.entry.regularEntry.qosParams.up);

        routeEntry.entry.regularEntry.qosParams.up = 0;

        /*
            1.21. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyDp [GT_TRUE],
                           routeEntriesArrayPtr->entry.regularEntry.qosParams->dp [0x5AAAAAA5] (out of range)
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.entry.regularEntry.qosParamsModify.modifyDp = GT_TRUE;
        routeEntry.entry.regularEntry.qosParams.dp = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntry.entry.regularEntry.qosParamsModify.modifyDp = %d, routeEntry.entry.regularEntry.qosParams.dp = %d",
                                     dev, routeEntry.entry.regularEntry.qosParamsModify.modifyDp,
                                     routeEntry.entry.regularEntry.qosParams.dp);

        routeEntry.entry.regularEntry.qosParams.dp = CPSS_DP_GREEN_E;

        /*
            1.22. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyDp [GT_FALSE],
                           routeEntriesArrayPtr->entry.regularEntry.qosParams->dp [4] (not relevant)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        routeEntry.entry.regularEntry.qosParamsModify.modifyDp = GT_FALSE;
        routeEntry.entry.regularEntry.qosParams.dp = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.regularEntry.qosParamsModify.modifyDp = %d, routeEntry.entry.regularEntry.qosParams.dp = %d",
                                     dev, routeEntry.entry.regularEntry.qosParamsModify.modifyDp,
                                     routeEntry.entry.regularEntry.qosParams.dp);

        routeEntry.entry.regularEntry.qosParams.dp = CPSS_DP_GREEN_E;

        /*
            1.23. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyDscp [GT_TRUE],
                           routeEntriesArrayPtr->entry.regularEntry.qosParams->dscp [64] (out of range)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.qosParamsModify.modifyDscp = GT_TRUE;
        routeEntry.entry.regularEntry.qosParams.dscp = 64;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.regularEntry.qosParamsModify.modifyDscp = %d, routeEntry.entry.regularEntry.qosParams.dscp = %d",
                                         dev, routeEntry.entry.regularEntry.qosParamsModify.modifyDscp,
                                         routeEntry.entry.regularEntry.qosParams.dscp);

        routeEntry.entry.regularEntry.qosParams.dscp = 0;

        /*
            1.24. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyDscp [GT_FALSE],
                           routeEntriesArrayPtr->entry.regularEntry.qosParams->dscp [64] (not relevant)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        routeEntry.entry.regularEntry.qosParamsModify.modifyDscp = GT_FALSE;
        routeEntry.entry.regularEntry.qosParams.dscp = 64;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.regularEntry.qosParamsModify.modifyDscp = %d, routeEntry.entry.regularEntry.qosParams.dscp = %d",
                                     dev, routeEntry.entry.regularEntry.qosParamsModify.modifyDscp,
                                     routeEntry.entry.regularEntry.qosParams.dscp);

        routeEntry.entry.regularEntry.qosParams.dscp = 0;

        /*
            1.25. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyExp [GT_TRUE],
                           routeEntriesArrayPtr->entry.regularEntry.qosParams->exp [8] (out of range)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.qosParamsModify.modifyExp = GT_TRUE;
        routeEntry.entry.regularEntry.qosParams.exp = 8;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.regularEntry.qosParamsModify.modifyExp = %d, routeEntry.entry.regularEntry.qosParams.exp = %d",
                                         dev, routeEntry.entry.regularEntry.qosParamsModify.modifyExp,
                                         routeEntry.entry.regularEntry.qosParams.exp);

        routeEntry.entry.regularEntry.qosParams.exp = 0;

        /*
            1.26. Call with routeEntriesArrayPtr->entry.regularEntry.qosParamsModify->modifyExp [GT_FALSE],
                           routeEntriesArrayPtr->entry.regularEntry.qosParams->exp [8] (not relevant)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        routeEntry.entry.regularEntry.qosParamsModify.modifyExp = GT_FALSE;
        routeEntry.entry.regularEntry.qosParams.exp = 8;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.regularEntry.qosParamsModify.modifyExp = %d, routeEntry.entry.regularEntry.qosParams.exp = %d",
                                     dev, routeEntry.entry.regularEntry.qosParamsModify.modifyExp,
                                     routeEntry.entry.regularEntry.qosParams.exp);

        routeEntry.entry.regularEntry.qosParams.exp = 0;

        /*
            1.27. Call with routeEntriesArrayPtr->entry.regularEntry.ucRpfOrIcmpRedirectIfMode [0x5AAAAAA5] out of range
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.entry.regularEntry.ucRpfOrIcmpRedirectIfMode = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntryPtr->entry.regularEntry.ucRpfOrIcmpRedirectIfMode = %d",
                                     dev, routeEntry.entry.regularEntry.ucRpfOrIcmpRedirectIfMode);

        routeEntry.entry.regularEntry.ucRpfOrIcmpRedirectIfMode = CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E;

        /*
            1.28. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.outlifType [0x5AAAAAA5] out of range
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.entry.regularEntry.nextHopIf.outlifType = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntryPtr->entry.regularEntry.nextHopIf.outlifType = %d",
                                     dev, routeEntry.entry.regularEntry.nextHopIf.outlifType);

        routeEntry.entry.regularEntry.nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;

        /*
            1.29. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_DEVICE_E] not supported
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_DEVICE_E;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.nextHopIf.interfaceInfo.type = %d",
                                     dev, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type);

        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;

        /*
            1.30. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [0x5AAAAAA5] out of range
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntryPtr->entry.regularEntry.nextHopIf.interfaceInfo.type = %d",
                                     dev, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type);

        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;

        /*
            1.31. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                            routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] out of range
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.nextHopIf.interfaceInfo.type = %d, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum = %d",
                                     dev, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum);

        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum = 0;

        /*
            1.32. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                            routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] out of range
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.nextHopIf.interfaceInfo.type = %d, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum = %d",
                                     dev, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum);

        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum = 0;

        /*
            1.33. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                            routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] not relevant
                           and other params from 1.1.
            Expected: GT_OK.
        */
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.nextHopIf.interfaceInfo.type = %d, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum = %d",
                                     dev, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum);

        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.devNum = 0;

        /*
            1.34. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                            routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] not relevant
                           and other params from 1.1.
            Expected: GT_OK.
        */
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.nextHopIf.interfaceInfo.type = %d, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum = %d",
                                     dev, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum);

        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.devPort.portNum = 0;

        /*
            1.35. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                            routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.trunkId [4096] (out of range)
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.trunkId = 4096;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.nextHopIf.interfaceInfo.type = %d, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.trunkId = %d",
                                         dev, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.trunkId);

        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.trunkId = 100;

        /*
            1.36. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                            routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.trunkId [4096] (not relevant)
                           and other params from 1.1.
            Expected: GT_OK.
        */
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.trunkId = 4096;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.nextHopIf.interfaceInfo.type = %d, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.trunkId = %d",
                                     dev, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.trunkId);

        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.trunkId = 100;

        /*
            1.37. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                            routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.vidx [4096] out of range
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.vidx = 4096;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.nextHopIf.interfaceInfo.type = %d, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.vidx = %d",
                                     dev, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.vidx);

        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.vidx = 0;

        /*
            1.38. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_FABRIC_VIDX_E]
                            routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.vidx [4096] not relevant
                           and other params from 1.1.
            Expected: GT_OK.
        */
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_FABRIC_VIDX_E;
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.vidx = 4096;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.nextHopIf.interfaceInfo.type = %d, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.vidx = %d",
                                     dev, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.vidx);

        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.vidx = 0;

        /*
            1.39. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_FABRIC_VIDX_E]
                            routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx [4096] out of range
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_FABRIC_VIDX_E;
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx = 4096;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.nextHopIf.interfaceInfo.type = %d, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx = %d",
                                     dev, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx);

        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx = 0;

        /*
            1.40. Call with routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                            routeEntriesArrayPtr->entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx [4096] not relevant
                           and other params from 1.1.
            Expected: GT_OK.
        */
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx = 4096;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.nextHopIf.interfaceInfo.type = %d, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx = %d",
                                     dev, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type, routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx);

        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.fabricVidx = 0;

        /*
            1.41. Call with routeEntriesArrayPtr->entry.regularEntry.mirrorToIngressAnalyzer [2] out of range
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.mirrorToIngressAnalyzer = 2;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.mirrorToIngressAnalyzer = %d",
                                     dev, routeEntry.entry.regularEntry.mirrorToIngressAnalyzer);

        routeEntry.entry.regularEntry.mirrorToIngressAnalyzer = GT_TRUE;

        /*
            1.42. Call with routeEntriesArrayPtr->entry.regularEntry.cntrSetIndex [0x5AAAAAA5] out of range
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.entry.regularEntry.cntrSetIndex = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntryPtr->entry.regularEntry.cntrSetIndex = %d",
                                     dev, routeEntry.entry.regularEntry.cntrSetIndex);

        routeEntry.entry.regularEntry.cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E;

        /*
            1.43. Call with routeEntriesArrayPtr->entry.regularEntry.mtuProfileIndex [16] out of range
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.regularEntry.mtuProfileIndex = 16;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.regularEntry.mtuProfileIndex = %d",
                                     dev, routeEntry.entry.regularEntry.mtuProfileIndex);

        routeEntry.entry.regularEntry.mtuProfileIndex = 15;
        routeEntry.entry.regularEntry.ttlMode = CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
        routeEntry.entry.regularEntry.mplsCommand = CPSS_EXMXPM_MPLS_PUSH_CMD_E;

        /*
            1.44. Call with routeEntriesArrayPtr->entry.regularEntry.ttlMode [0x5AAAAAA5] out of range
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.entry.regularEntry.ttlMode = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntryPtr->entry.regularEntry.ttlMode = %d",
                                     dev, routeEntry.entry.regularEntry.ttlMode);

        routeEntry.entry.regularEntry.ttlMode = CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;

        /*
            1.45. Call with routeEntriesArrayPtr->entry.regularEntry.mplsCommand [0x5AAAAAA5] out of range
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.entry.regularEntry.mplsCommand = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntryPtr->entry.regularEntry.ttlMode = %d",
                                     dev, routeEntry.entry.regularEntry.ttlMode);

        routeEntry.entry.regularEntry.mplsCommand = CPSS_EXMXPM_MPLS_PHP_CMD_E;

        /*
        [ CPSS_EXMXPM_IP_UC_ECMP_RPF_E ]
            1.46. Call with ucMcRouteEntryBaseIndex [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1],
                            numOfRouteEntries [1],
                            routeEntriesArrayPtr {type  {CPSS_EXMXPM_IP_UC_ECMP_RPF_E},
                                                  entry { rpfIcmpMode [CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E],
                                                          sipFilterEnable [GT_TRUE],
                                                          sipAccessLevel [1],
                                                          rpfIf[8]{valid[GT_TRUE], interface{set to trunk} } }
            Expected: GT_OK.
        */
        ucMcRouteEntryBaseIndex = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1;
        numOfRouteEntries = 1;

        cpssOsBzero((GT_VOID*) &routeEntry, sizeof(routeEntry));

        routeEntry.type = CPSS_EXMXPM_IP_UC_ECMP_RPF_E;

        routeEntry.entry.ecmpRpfCheck.rpfIcmpMode = CPSS_EXMXPM_RPF_IF_MODE_PORT_E;
        routeEntry.entry.ecmpRpfCheck.sipFilterEnable = GT_TRUE;
        routeEntry.entry.ecmpRpfCheck.sipAccessLevel = 1;

        routeEntry.entry.ecmpRpfCheck.rpfIf[0].valid = GT_TRUE;
        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.devPort.devNum = dev;
        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.devPort.portNum = dev;

        routeEntry.entry.ecmpRpfCheck.rpfIf[1].valid = GT_TRUE;
        routeEntry.entry.ecmpRpfCheck.rpfIf[1].interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        routeEntry.entry.ecmpRpfCheck.rpfIf[1].interfaceInfo.trunkId = 100;

        routeEntry.entry.ecmpRpfCheck.rpfIf[2].valid = GT_FALSE;
        routeEntry.entry.ecmpRpfCheck.rpfIf[3].valid = GT_FALSE;
        routeEntry.entry.ecmpRpfCheck.rpfIf[4].valid = GT_FALSE;
        routeEntry.entry.ecmpRpfCheck.rpfIf[5].valid = GT_FALSE;
        routeEntry.entry.ecmpRpfCheck.rpfIf[6].valid = GT_FALSE;
        routeEntry.entry.ecmpRpfCheck.rpfIf[7].valid = GT_FALSE;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);

        /*
            1.47. Call cpssExMxPmIpUcRouteEntriesRead with not NULL routeEntriesArrayPtr and other params from 1.25.
            Expected: GT_OK and the same params as was set.
        */
        routeEntryGet.type = CPSS_EXMXPM_IP_UC_ECMP_RPF_E;

        st = cpssExMxPmIpUcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntryGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpUcRouteEntriesRead: %d, %d, %d",
                                    dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);

        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.type, routeEntryGet.type, "got another routeEntry.type than was set", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.rpfIcmpMode,
                                     routeEntryGet.entry.ecmpRpfCheck.rpfIcmpMode,
                                     "got another routeEntry.entry.ecmpRpfCheck.rpfIcmpMode than was set", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.sipFilterEnable,
                                     routeEntryGet.entry.ecmpRpfCheck.sipFilterEnable,
                                     "got another routeEntry.entry.ecmpRpfCheck.sipFilterEnable than was set", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.sipAccessLevel,
                                     routeEntryGet.entry.ecmpRpfCheck.sipAccessLevel,
                                     "got another routeEntry.entry.ecmpRpfCheck.sipAccessLevel than was set", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.rpfIf[0].valid,
                                     routeEntryGet.entry.ecmpRpfCheck.rpfIf[0].valid,
                                     "got another routeEntry.entry.ecmpRpfCheck.rpfIf[0].valid than was set", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type,
                                     routeEntryGet.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type,
                                     "got another routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type than was set", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.devPort.devNum,
                                     routeEntryGet.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.devPort.devNum,
                                     "got another routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.devPort.devNum than was set", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.devPort.portNum,
                                     routeEntryGet.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.devPort.portNum,
                                     "got another routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.devPort.portNum than was set", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.rpfIf[1].valid,
                                     routeEntryGet.entry.ecmpRpfCheck.rpfIf[1].valid,
                                     "got another routeEntry.entry.ecmpRpfCheck.rpfIf[1].valid than was set", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.rpfIf[1].interfaceInfo.type,
                                     routeEntryGet.entry.ecmpRpfCheck.rpfIf[1].interfaceInfo.type,
                                     "got another routeEntry.entry.ecmpRpfCheck.rpfIf[1].interfaceInfo.type than was set", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.rpfIf[1].interfaceInfo.trunkId,
                                     routeEntryGet.entry.ecmpRpfCheck.rpfIf[1].interfaceInfo.trunkId,
                                     "got another routeEntry.entry.ecmpRpfCheck.rpfIf[1].interfaceInfo.trunkId than was set", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.rpfIf[2].valid,
                                     routeEntryGet.entry.ecmpRpfCheck.rpfIf[2].valid,
                                     "got another routeEntry.entry.ecmpRpfCheck.rpfIf[2].valid than was set", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.rpfIf[3].valid,
                                     routeEntryGet.entry.ecmpRpfCheck.rpfIf[3].valid,
                                     "got another routeEntry.entry.ecmpRpfCheck.rpfIf[3].valid than was set", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.rpfIf[4].valid,
                                     routeEntryGet.entry.ecmpRpfCheck.rpfIf[4].valid,
                                     "got another routeEntry.entry.ecmpRpfCheck.rpfIf[4].valid than was set", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.rpfIf[5].valid,
                                     routeEntryGet.entry.ecmpRpfCheck.rpfIf[5].valid,
                                     "got another routeEntry.entry.ecmpRpfCheck.rpfIf[5].valid than was set", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.rpfIf[6].valid,
                                     routeEntryGet.entry.ecmpRpfCheck.rpfIf[6].valid,
                                     "got another routeEntry.entry.ecmpRpfCheck.rpfIf[6].valid than was set", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.entry.ecmpRpfCheck.rpfIf[7].valid,
                                     routeEntryGet.entry.ecmpRpfCheck.rpfIf[7].valid,
                                     "got another routeEntry.entry.ecmpRpfCheck.rpfIf[7].valid than was set", dev);

        /*
            1.48. Call with  routeEntriesArrayPtr->entry.rpfIcmpMode [0x5AAAAAA5] not supported
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.entry.ecmpRpfCheck.rpfIcmpMode = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntryPtr->entry.ecmpRpfCheck.rpfIcmpMode = %d",
                                    dev, routeEntry.entry.ecmpRpfCheck.rpfIcmpMode);

        routeEntry.entry.ecmpRpfCheck.rpfIcmpMode = CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E;

        /*
            1.49. Call with  routeEntriesArrayPtr->entry.rpfIcmpMode [CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E] not supported
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.ecmpRpfCheck.rpfIcmpMode = CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.ecmpRpfCheck.rpfIcmpMode = %d",
                                         dev, routeEntry.entry.ecmpRpfCheck.rpfIcmpMode);

        routeEntry.entry.ecmpRpfCheck.rpfIcmpMode = CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E;

        /*
            1.50. Call with  routeEntriesArrayPtr->entry.sipAccessLevel [8] not supported
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.ecmpRpfCheck.sipAccessLevel = 8;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntryPtr->entry.ecmpRpfCheck.sipAccessLevel = %d",
                                         dev, routeEntry.entry.ecmpRpfCheck.sipAccessLevel);

        routeEntry.entry.ecmpRpfCheck.sipAccessLevel = 0;

        /*
            1.51. Call with routeEntriesArrayPtr->entry.rpfIf[0].interface.type [0x5AAAAAA5] out of range
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntryPtr->entry.ecmpRpfCheck.rpfIf[0].interface.type = %d",
                                    dev, routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type);

        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        /*
            1.52. Call with routeEntriesArrayPtr->entry.rpfIcmpMode [CPSS_EXMXPM_RPF_IF_MODE_PORT_E],
                            routeEntriesArrayPtr->entry.rpfIf[0].interface.type [CPSS_INTERFACE_PORT_E],
                            routeEntriesArrayPtr->entry.rpfIf[0].interface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (out of range)
                            and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.ecmpRpfCheck.rpfIcmpMode = CPSS_EXMXPM_RPF_IF_MODE_PORT_E;
        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.rpfIcmpMode = %d, routeEntry.entry.rpfIf[0].interface.type = %d, routeEntry.entry.rpfIf[0].interface.devPort.portNum = %d",
                                         dev, routeEntry.entry.ecmpRpfCheck.rpfIcmpMode,
                                         routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type,
                                         routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.devPort.portNum);

        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.devPort.portNum = 0;

        /*
            1.53. Call with routeEntriesArrayPtr->entry.rpfIcmpMode [CPSS_EXMXPM_RPF_IF_MODE_PORT_E],
                            routeEntriesArrayPtr->entry.rpfIf[0].interface.type [CPSS_INTERFACE_TRUNK_E],
                            routeEntriesArrayPtr->entry.rpfIf[0].interface.trunkId [4096] (out of range)
                            and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.ecmpRpfCheck.rpfIcmpMode = CPSS_EXMXPM_RPF_IF_MODE_PORT_E;
        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.trunkId = 4096;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.rpfIcmpMode = %d, routeEntry.entry.rpfIf[0].interface.type = %d, routeEntry.entry.rpfIf[0].interface.trunkId = %d",
                                         dev, routeEntry.entry.ecmpRpfCheck.rpfIcmpMode,
                                         routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type,
                                         routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.trunkId);

        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.trunkId = 0;

        /*
            1.54. Call with routeEntriesArrayPtr->entry.rpfIcmpMode [CPSS_EXMXPM_RPF_IF_MODE_VLAN_INLIF_E],
                            routeEntriesArrayPtr->entry.rpfIf[0].interface.type [CPSS_INTERFACE_VID_E],
                            routeEntriesArrayPtr->entry.rpfIf[0].interface.devPort.vlanId [4096] (out of range)
                            and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.ecmpRpfCheck.rpfIcmpMode = CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E;
        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type = CPSS_INTERFACE_VID_E;
        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.vlanId = 4096;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.rpfIcmpMode = %d, routeEntry.entry.rpfIf[0].interface.type = %d, routeEntry.entry.rpfIf[0].interface.vlanId = %d",
                                         dev, routeEntry.entry.ecmpRpfCheck.rpfIcmpMode,
                                         routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type,
                                         routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.vlanId);

        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.vlanId = 0;

        /*
            1.55. Call with routeEntriesArrayPtr->entry.rpfIf[0].interface.type [CPSS_INTERFACE_VIDX_E /
                                                                                 CPSS_INTERFACE_DEVICE_E /
                                                                                 CPSS_INTERFACE_FABRIC_VIDX_E] (out of range)
                            and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type = CPSS_INTERFACE_VIDX_E;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.rpfIf[0].interface.type = %d",
                                         dev, routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type);

        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type = CPSS_INTERFACE_DEVICE_E;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.rpfIf[0].interface.type = %d",
                                         dev, routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type);

        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type = CPSS_INTERFACE_FABRIC_VIDX_E;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.rpfIf[0].interface.type = %d",
                                         dev, routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type);

        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type = CPSS_INTERFACE_VID_E;

        /*
            1.56. Call with routeEntriesArrayPtr->entry.rpfIcmpMode [CPSS_EXMXPM_RPF_IF_MODE_PORT_E],
                            routeEntriesArrayPtr->entry.rpfIf[0].interface.type [CPSS_INTERFACE_VID_E] (not supported to rpfIcmpMode),
                            and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.ecmpRpfCheck.rpfIcmpMode = CPSS_EXMXPM_RPF_IF_MODE_PORT_E;
        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type = CPSS_INTERFACE_VID_E;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.rpfIcmpMode = %d, routeEntry.entry.rpfIf[0].interface.type = %d",
                                         dev, routeEntry.entry.ecmpRpfCheck.rpfIcmpMode,
                                         routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type);

        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type = CPSS_INTERFACE_VID_E;

        /*
            1.57. Call with routeEntriesArrayPtr->entry.rpfIcmpMode [CPSS_EXMXPM_RPF_IF_MODE_VLAN_INLIF_E],
                            routeEntriesArrayPtr->entry.rpfIf[0].interface.type [CPSS_INTERFACE_TRUNK_E] (not supported to rpfIcmpMode),
                            and other params from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.entry.ecmpRpfCheck.rpfIcmpMode = CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E;
        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntry.entry.rpfIcmpMode = %d, routeEntry.entry.rpfIf[0].interface.type = %d",
                                         dev, routeEntry.entry.ecmpRpfCheck.rpfIcmpMode,
                                         routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type);

        routeEntry.entry.ecmpRpfCheck.rpfIf[0].interfaceInfo.type = CPSS_INTERFACE_VID_E;

        /*
            1.58. Call with routeEntriesArrayPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, routeEntryPtr = NULL", dev);
    }

    ucMcRouteEntryBaseIndex = 0;
    numOfRouteEntries = 1;

    routeEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

    routeEntry.entry.regularEntry.command = CPSS_PACKET_CMD_ROUTE_E;
    routeEntry.entry.regularEntry.bypassTtlExceptionCheckEnable = GT_TRUE;
    routeEntry.entry.regularEntry.cpuCodeIndex = 0;
    routeEntry.entry.regularEntry.ageRefresh = GT_TRUE;
    routeEntry.entry.regularEntry.ipv6ScopeCheckEnable = GT_FALSE;
    routeEntry.entry.regularEntry.ipv6DestinationSiteId = CPSS_IP_SITE_ID_INTERNAL_E;
    routeEntry.entry.regularEntry.sipAccessLevel = 0;
    routeEntry.entry.regularEntry.dipAccessLevel = 0;
    routeEntry.entry.regularEntry.ucRpfOrIcmpRedirectIfMode = CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E;
    routeEntry.entry.regularEntry.sipSaCheckEnable = GT_TRUE;
    routeEntry.entry.regularEntry.trapMirrorArpBcToCpu = GT_TRUE;
    routeEntry.entry.regularEntry.ucPacketSipFilterEnable = GT_TRUE;

    routeEntry.entry.regularEntry.nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
    routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.trunkId = 100;
    routeEntry.entry.regularEntry.nextHopIf.outlifPointer.arpPtr = 100;

    routeEntry.entry.regularEntry.nextHopVlanId = 10;
    routeEntry.entry.regularEntry.mirrorToIngressAnalyzer = GT_TRUE;
    routeEntry.entry.regularEntry.cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E;
    routeEntry.entry.regularEntry.mtuProfileIndex = 15;
    routeEntry.entry.regularEntry.icmpRedirectEnable = GT_TRUE;
    routeEntry.entry.regularEntry.appSpecificCpuCodeEnable = GT_FALSE;
    routeEntry.entry.regularEntry.hopLimitMode = CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
    routeEntry.entry.regularEntry.ttl = 0;
    routeEntry.entry.regularEntry.ttlMode = CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
    routeEntry.entry.regularEntry.mplsLabel = 0;
    routeEntry.entry.regularEntry.mplsCommand = CPSS_EXMXPM_MPLS_PUSH_CMD_E;

    routeEntry.entry.regularEntry.qosParamsModify.modifyTc = GT_FALSE;
    routeEntry.entry.regularEntry.qosParamsModify.modifyUp = GT_FALSE;
    routeEntry.entry.regularEntry.qosParamsModify.modifyDp = GT_FALSE;
    routeEntry.entry.regularEntry.qosParamsModify.modifyDscp = GT_FALSE;
    routeEntry.entry.regularEntry.qosParamsModify.modifyExp = GT_FALSE;

    routeEntry.entry.regularEntry.qosParams.tc = 0;
    routeEntry.entry.regularEntry.qosParams.dp = CPSS_DP_GREEN_E;
    routeEntry.entry.regularEntry.qosParams.up = 0;
    routeEntry.entry.regularEntry.qosParams.dscp = 0;
    routeEntry.entry.regularEntry.qosParams.exp = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpUcRouteEntriesRead
(
    IN      GT_U8                                devNum,
    IN      GT_U32                               ucMcRouteEntryBaseIndex,
    IN      GT_U32                               numOfRouteEntries,
    INOUT   CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC    *routeEntriesArrayPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpUcRouteEntriesRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with cpssExMxPmIpUcRouteEntriesWrite ucMcRouteEntryBaseIndex [0 / PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1],
                   numOfRouteEntries [1],
                   routeEntriesArrayPtr {type  {CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E},
                                         entry { regularEntry{
                                                              command [CPSS_PACKET_CMD_ROUTE_E],
                                                              bypassTtlExceptionCheckEnable [GT_TRUE],
                                                              cpuCodeIndex [3],
                                                              ageRefresh [GT_TRUE],
                                                              ipv6ScopeCheckEnable [GT_FALSE],
                                                              ipv6DestinationSiteId [CPSS_IP_SITE_ID_INTERNAL_E],
                                                              sipAccessLevel [7],
                                                              dipAccessLevel [7],
                                                              ucRpfOrIcmpRedirectIfMode [CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E],
                                                              sipSaCheckEnable [GT_TRUE],
                                                              trapMirrorArpBcToCpu [GT_TRUE],
                                                              ucPacketSipFilterEnable [GT_TRUE],
                                                              nextHopIf {
                                                                          outlifType [CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                          interfaceInfo {
                                                                                          type [CPSS_INTERFACE_TRUNK_E],
                                                                                          trunkId [100] }
                                                                          outlifPointer { arpPtr [100] } },
                                                              nextHopVlanId [10],
                                                              mirrorToIngressAnalyzer [GT_TRUE],
                                                              cntrSetIndex [CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E],
                                                              mtuProfileIndex [15],
                                                              icmpRedirectEnable [GT_TRUE],
                                                              appSpecificCpuCodeEnable [GT_FALSE], nr
                                                              hopLimitMode [CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E],
                                                              ttlIndex [0],
                                                              ttlMode [CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E],
                                                              mplsLabel [0],
                                                              mplsCommand [CPSS_EXMXPM_MPLS_PUSH_CMD_E],
                                                              qosParamsModify { modifyTc [GT_FALSE],
                                                                                modifyUp [GT_FALSE],
                                                                                modifyDp [GT_FALSE],
                                                                                modifyDscp [GT_FALSE],
                                                                                modifyExp [GT_FALSE] },
                                                              qosParams {   tc [0],
                                                                            dp [0],
                                                                            up [0],
                                                                            dscp [0],
                                                                            exp [0] } } }
    Expected: GT_OK.
    1.2. Call with ucMcRouteEntryBaseIndex [0 / PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1],
                   numOfRouteEntries [1 / 1]
                   and non-NULL routeEntriesArrayPtr.
    Expected: GT_OK.
    1.3. Call with out of range ucMcRouteEntryBaseIndex [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range numOfRouteEntries [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range numOfRouteEntries [0].
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with routeEntriesArrayPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                              ucMcRouteEntryBaseIndex = 0;
    GT_U32                              numOfRouteEntries       = 0;
    CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC   routeEntry;


    cpssOsBzero((GT_VOID*) &routeEntry, sizeof(routeEntry));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cpssExMxPmIpUcRouteEntriesWrite ucMcRouteEntryBaseIndex [0 / PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1],
                           numOfRouteEntries [1],
                           routeEntriesArrayPtr {type  {CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E},
                                                 entry { regularEntry{
                                                                      command [CPSS_PACKET_CMD_ROUTE_E],
                                                                      bypassTtlExceptionCheckEnable [GT_TRUE],
                                                                      cpuCodeIndex [3],
                                                                      ageRefresh [GT_TRUE],
                                                                      ipv6ScopeCheckEnable [GT_FALSE],
                                                                      ipv6DestinationSiteId [CPSS_IP_SITE_ID_INTERNAL_E],
                                                                      sipAccessLevel [7],
                                                                      dipAccessLevel [7],
                                                                      ucRpfOrIcmpRedirectIfMode [CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E],
                                                                      sipSaCheckEnable [GT_TRUE],
                                                                      trapMirrorArpBcToCpu [GT_TRUE],
                                                                      ucPacketSipFilterEnable [GT_TRUE],
                                                                      nextHopIf {
                                                                                  outlifType [CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                                  interfaceInfo {
                                                                                                  type [CPSS_INTERFACE_TRUNK_E],
                                                                                                  trunkId [100] }
                                                                                  outlifPointer { arpPtr [100] } },
                                                                      nextHopVlanId [10],
                                                                      mirrorToIngressAnalyzer [GT_TRUE],
                                                                      cntrSetIndex [CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E],
                                                                      mtuProfileIndex [15],
                                                                      icmpRedirectEnable [GT_TRUE],
                                                                      appSpecificCpuCodeEnable [GT_FALSE], nr
                                                                      hopLimitMode [CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E],
                                                                      ttlIndex [0],
                                                                      ttlMode [CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E],
                                                                      mplsLabel [0],
                                                                      mplsCommand [CPSS_EXMXPM_MPLS_PUSH_CMD_E],
                                                                      qosParamsModify { modifyTc [GT_FALSE],
                                                                                        modifyUp [GT_FALSE],
                                                                                        modifyDp [GT_FALSE],
                                                                                        modifyDscp [GT_FALSE],
                                                                                        modifyExp [GT_FALSE] },
                                                                      qosParams {   tc [0],
                                                                                    dp [0],
                                                                                    up [0],
                                                                                    dscp [0],
                                                                                    exp [0] } } }
            Expected: GT_OK.
        */
        ucMcRouteEntryBaseIndex = 0;
        numOfRouteEntries = 1;

        routeEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        routeEntry.entry.regularEntry.command = CPSS_PACKET_CMD_ROUTE_E;
        routeEntry.entry.regularEntry.bypassTtlExceptionCheckEnable = GT_TRUE;
        routeEntry.entry.regularEntry.cpuCodeIndex = 3;
        routeEntry.entry.regularEntry.ageRefresh = GT_TRUE;
        routeEntry.entry.regularEntry.ipv6ScopeCheckEnable = GT_FALSE;
        routeEntry.entry.regularEntry.ipv6DestinationSiteId = CPSS_IP_SITE_ID_INTERNAL_E;
        routeEntry.entry.regularEntry.sipAccessLevel = 7;
        routeEntry.entry.regularEntry.dipAccessLevel = 7;
        routeEntry.entry.regularEntry.ucRpfOrIcmpRedirectIfMode = CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E;
        routeEntry.entry.regularEntry.sipSaCheckEnable = GT_TRUE;
        routeEntry.entry.regularEntry.trapMirrorArpBcToCpu = GT_TRUE;
        routeEntry.entry.regularEntry.ucPacketSipFilterEnable = GT_TRUE;

        routeEntry.entry.regularEntry.nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.trunkId = 100;
        routeEntry.entry.regularEntry.nextHopIf.outlifPointer.arpPtr = 100;

        routeEntry.entry.regularEntry.nextHopVlanId = 10;
        routeEntry.entry.regularEntry.mirrorToIngressAnalyzer = GT_TRUE;
        routeEntry.entry.regularEntry.cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E;
        routeEntry.entry.regularEntry.mtuProfileIndex = 15;
        routeEntry.entry.regularEntry.icmpRedirectEnable = GT_TRUE;
        routeEntry.entry.regularEntry.appSpecificCpuCodeEnable = GT_FALSE;
        routeEntry.entry.regularEntry.hopLimitMode = CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
        routeEntry.entry.regularEntry.ttl = 0;
        routeEntry.entry.regularEntry.ttlMode = CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
        routeEntry.entry.regularEntry.mplsLabel = 0;
        routeEntry.entry.regularEntry.mplsCommand = CPSS_EXMXPM_MPLS_PUSH_CMD_E;

        routeEntry.entry.regularEntry.qosParamsModify.modifyTc = GT_FALSE;
        routeEntry.entry.regularEntry.qosParamsModify.modifyUp = GT_FALSE;
        routeEntry.entry.regularEntry.qosParamsModify.modifyDp = GT_FALSE;
        routeEntry.entry.regularEntry.qosParamsModify.modifyDscp = GT_FALSE;
        routeEntry.entry.regularEntry.qosParamsModify.modifyExp = GT_FALSE;

        routeEntry.entry.regularEntry.qosParams.tc = 0;
        routeEntry.entry.regularEntry.qosParams.dp = CPSS_DP_GREEN_E;
        routeEntry.entry.regularEntry.qosParams.up = 0;
        routeEntry.entry.regularEntry.qosParams.dscp = 0;
        routeEntry.entry.regularEntry.qosParams.exp = 0;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpUcRouteEntriesWrite: %d, %d, %d",
                                     dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);

        ucMcRouteEntryBaseIndex = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum - 1;

        st = cpssExMxPmIpUcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpUcRouteEntriesWrite: %d, %d, %d",
                                     dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);


        /*
            1.2. Call with ucMcRouteEntryBaseIndex [0 / PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1],
                           numOfRouteEntries [1 / 1]
                           and non-NULL routeEntriesArrayPtr.
            Expected: GT_OK.
        */

        /* Call with ucMcRouteEntryBaseIndex [0] */
        ucMcRouteEntryBaseIndex = 0;
        numOfRouteEntries       = 1;

        routeEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        st = cpssExMxPmIpUcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);

        /* Call with ucMcRouteEntryBaseIndex [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1] */
        ucMcRouteEntryBaseIndex = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1;
        numOfRouteEntries       = 1;

        routeEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        st = cpssExMxPmIpUcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);


        /*
            1.3. Call with out of range ucMcRouteEntryBaseIndex [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ucMcRouteEntryBaseIndex = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum;

        routeEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        st = cpssExMxPmIpUcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryBaseIndex);

        ucMcRouteEntryBaseIndex = 1;

        /*
            1.4. Call with out of range numOfRouteEntries [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        numOfRouteEntries = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum;

        routeEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        st = cpssExMxPmIpUcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfRouteEntries = %d", dev, numOfRouteEntries);

        numOfRouteEntries = 1;

        /*
            1.5. Call with out of range numOfRouteEntries [0]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ucMcRouteEntryBaseIndex = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum - 1;
        numOfRouteEntries       = 0;

        routeEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        st = cpssExMxPmIpUcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ucMcRouteEntryBaseIndex + numOfRouteEntries = %d",
                                         dev, ucMcRouteEntryBaseIndex + numOfRouteEntries);

        ucMcRouteEntryBaseIndex = 0;
        numOfRouteEntries       = 1;

        /*
            1.6. Call with routeEntriesArrayPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        routeEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        st = cpssExMxPmIpUcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, routeEntriesArrayPtr = NULL", dev);
    }

    ucMcRouteEntryBaseIndex = 0;
    numOfRouteEntries       = 1;
    routeEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        routeEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        st = cpssExMxPmIpUcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    routeEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

    st = cpssExMxPmIpUcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpMcRouteEntriesWrite
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              ucMcRouteEntryBaseIndex,
    IN  GT_U32                              numOfRouteEntries,
    IN  CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC   *routeEntriesArrayPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpMcRouteEntriesWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ucMcRouteEntryBaseIndex [0 / PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1],
                   numOfRouteEntries [1 / 1]
                   and routeEntriesArrayPtr {command [CPSS_PACKET_CMD_ROUTE_E],
                                             bypassTtlExceptionCheckEnable [GT_TRUE],
                                             cpuCodeIndex [0],
                                             ageRefresh [GT_TRUE],
                                             ipv6ScopeCheckEnable [GT_FALSE],
                                             ipv6ScopeLevel [0],
                                             ipv6DestinationSiteId [CPSS_IP_SITE_ID_INTERNAL_E],
                                             mcRpfIfMode [CPSS_EXMXPM_RPF_IF_MODE_PORT_E],
                                             mcRpfIf {type [CPSS_INTERFACE_TRUNK_E],
                                                                 trunkId [2]},
                                             mcRpfFailCommand [CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E],
                                             nextHopIf {type [CPSS_INTERFACE_TRUNK_E],
                                                         trunkId [2]},
                                             mirrorToIngressAnalyzer [GT_TRUE],
                                             cntrSetIndex  [CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E],
                                             mtuProfileIndex [0],
                                             appSpecificCpuCodeEnable [GT_TRUE],
                                             hopLimitMode [CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E],
                                             qosParamsModify {modifyTc [GT_TRUE],
                                                              modifyUp [GT_TRUE],
                                                              modifyDp [GT_TRUE],
                                                              modifyDscp [GT_TRUE],
                                                              modifyExp [GT_TRUE]},
                                             qosParams {tc [0],
                                                        dp [CPSS_DP_GREEN_E],
                                                        up [0],
                                                        dscp [0],
                                                        exp [0]},
                                             ttl [0],
                                             ttlMode [CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E],
                                             mplsLabel [0],
                                             mplsCommand [CPSS_EXMXPM_MPLS_PUSH_CMD_E] }
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpMcRouteEntriesRead with the same ucMcRouteEntryBaseIndex,
                                                  numOfRouteEntries
                                                  and non-NULL routeEntriesArrayPtr.
    Expected: GT_OK and the same routeEntriesArrayPtr.
    1.3 Call cpssExMxPmIpMcRouteEntriesWrite with the same ucMcRouteEntryBaseIndex,
                                                  numOfRouteEntries,
                                                  routeEntriesArrayPtr,
                                                  mcRpfIfMode  = CPSS_EXMXPM_RPF_IF_MODE_VLAN_INLIF_E;
    Expected: GT_OK and the same routeEntriesArrayPtr.
    Call cpssExMxPmIpMcRouteEntriesRead with the same ucMcRouteEntryBaseIndex,
                                                  numOfRouteEntries,
                                                  routeEntriesArrayPtr,
                                                  mcRpfIfMode  = CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E;
    Expected: GT_OK and the same routeEntriesArrayPtr.
    Call cpssExMxPmIpMcRouteEntriesWrite with the same ucMcRouteEntryBaseIndex,
                                                  numOfRouteEntries,
                                                  routeEntriesArrayPtr,
                                                  mcRpfIfMode  = CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E;
    Expected: GT_OK and the same routeEntriesArrayPtr.
    Call cpssExMxPmIpMcRouteEntriesRead with the same ucMcRouteEntryBaseIndex,
                                                  numOfRouteEntries,
                                                  routeEntriesArrayPtr,
                                                  mcRpfIfMode  = CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E;
    Expected: GT_OK and the same routeEntriesArrayPtr.


    1.4. Call with out of range numOfRouteEntries [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with out of range numOfRouteEntries [0]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range routeEntriesArrayPtr->command [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with routeEntriesArrayPtr->command [CPSS_PACKET_CMD_FORWARD_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with routeEntriesArrayPtr->command [CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E]
                   out of range routeEntriesArrayPtr->cpuCodeIndex [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with routeEntriesArrayPtr->qosParamsModify.modifyTc [GT_TRUE]
                    out of range routeEntriesArrayPtr->qosParams.tc [CPSS_TC_RANGE_CNS]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.

    1.11. Call with routeEntriesArrayPtr->qosParamsModify.modifyUp [GT_TRUE]
                    out of range routeEntriesArrayPtr->qosParams.up [CPSS_USER_PRIORITY_RANGE_CNS]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.

    1.13. Call with routeEntriesArrayPtr->qosParamsModify.modifyDp [GT_TRUE]
                    out of range routeEntriesArrayPtr->qosParams.dp [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.

    1.15. Call with routeEntriesArrayPtr->qosParamsModify.modifyDscp [GT_TRUE]
                    out of range routeEntriesArrayPtr->qosParams.dscp [CPSS_DSCP_RANGE_CNS]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.

    1.17. Call with out of range routeEntriesArrayPtr->cntrSetIndex [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.18. Call with out of range routeEntriesArrayPtr->mcRpfIfMode [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
--*
    1.19. Call with routeEntriesArrayPtr->mcRpfIf.type [0x5AAAAAA5] out of range
                  and other valid params.
    Expected: GT_BAD_PARAM.
    1.20. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_VIDX_E] not supported
                  and other valid params.
    Expected: NOT GT_OK.
    1.21. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_DEVICE_E] not supported
                  and other valid params.
    Expected: NOT GT_OK.
    1.22. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_FABRIC_VIDX_E] out of range
                  and other valid params.
    Expected: GT_BAD_PARAM.
[ CPSS_INTERFACE_TRUNK_E ]
    -> out of range
    1.23. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_TRUNK_E]
                    routeEntriesArrayPtr->mcRpfIf.trunkId [4096]
                    and other valid params.
    Expected: NOT GT_OK.
   -> not relevant
    1.24. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_PORT_E]
                    routeEntriesArrayPtr->mcRpfIf.trunkId [4096]
                    and other valid params.
    Expected: GT_OK.
[ CPSS_INTERFACE_PORT_E ]
    -> out of range
    1.25. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_PORT_E]
                    routeEntriesArrayPtr->mcRpfIf.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other valid params.
    Expected: NOT GT_OK.
    1.26. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_PORT_E]
                    routeEntriesArrayPtr->mcRpfIf.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                    and other valid params.
    Expected: NOT GT_OK.
   -> not relevant
    1.27. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_VID_E]
                    routeEntriesArrayPtr->mcRpfIf.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other valid params.
    Expected: GT_OK.
    1.28. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_VID_E]
                    routeEntriesArrayPtr->mcRpfIf.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                    and other valid params.
    Expected: GT_OK.
[ CPSS_INTERFACE_VID_E ]
    -> out of range
    1.29. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_VID_E]
                    routeEntriesArrayPtr->mcRpfIf.vlanId [4096]
                    and other valid params.
    Expected: NOT GT_OK.
    -> not relevant
    1.30. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_PORT_E]
                    routeEntriesArrayPtr->mcRpfIf.vlanId [4096]
                    and other valid params.
    Expected: GT_OK.
--*
    1.31. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [0x5AAAAAA5] out of range
                  and other valid params.
    Expected: GT_BAD_PARAM.
    1.32. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_VID_E] out of range
                  and other valid params.
    Expected: GT_BAD_PARAM.
    1.33. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_DEVICE_E] out of range
                  and other valid params.
    Expected: GT_BAD_PARAM.

[ CPSS_INTERFACE_TRUNK_E ]
    -> out of range
    1.34. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                    routeEntriesArrayPtr->nextHopIf.interfaceInfo.trunkId [4096]
                    and other valid params.
    Expected: NOT GT_OK.

   -> not relevant
    1.35. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                    routeEntriesArrayPtr->nextHopIf.interfaceInfo.trunkId [4096]
                    and other valid params.
    Expected: GT_OK.
[ CPSS_INTERFACE_PORT_E ]
    -> out of range
    1.36. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                    routeEntriesArrayPtr->nextHopIf.interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other valid params.
    Expected: NOT GT_OK.
    1.37. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                    routeEntriesArrayPtr->nextHopIf.interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                    and other valid params.
    Expected: NOT GT_OK.
   -> not relevant
    1.38. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                    routeEntriesArrayPtr->nextHopIf.interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other valid params.
    Expected: NOT GT_OK.
    1.39. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                    routeEntriesArrayPtr->nextHopIf.interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                    and other valid params.
    Expected: NOT GT_OK.
[ CPSS_INTERFACE_VIDX_E ]
    -> out of range
    1.40. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                    routeEntriesArrayPtr->nextHopIf.interfaceInfo.vidx [4096]
                    and other valid params.
    Expected: NOT GT_OK.
   -> not relevant
    1.41. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_VID_E]
                    routeEntriesArrayPtr->nextHopIf.interfaceInfo.vidx [4096]
                    and other valid params.
    Expected: GT_OK.
[ CPSS_INTERFACE_FABRIC_VIDX_E ]
    -> out of range
    1.42. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_FABRIC_VIDX_E]
                    routeEntriesArrayPtr->nextHopIf.interfaceInfo.fabricVidx [4096]
                   and other valid params.
    Expected: NOT GT_OK.
    -> not relevant
    1.43. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                    routeEntriesArrayPtr->nextHopIf.interfaceInfo.fabricVidx [4096]
                   and other valid params.
    Expected: GT_OK.
    1.44. Call with out of range routeEntriesArrayPtr->mcRpfFailCommand [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.45. Call with out of range routeEntriesArrayPtr->ipv6ScopeLevel [4] (out of range)
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.46. Call with out of range routeEntriesArrayPtr->hopLimitMode [0x5AAAAAA5] (out of range)
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.47. Call with out of range routeEntriesArrayPtr->ttl [256] (out of range)
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.48. Call with out of range routeEntriesArrayPtr->ttlMode [0x5AAAAAA5] (out of range)
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.49. Call with out of range routeEntriesArrayPtr->mplsLabel [BIT_20] (out of range)
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.50. Call with out of range routeEntriesArrayPtr->mplsCommand [0x5AAAAAA5] (out of range)
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.51. Call with out of range routeEntriesArrayPtr->ipv6DestinationSiteId [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.52. Call with out of range routeEntriesArrayPtr->mtuProfileIndex [16]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.53. Call with out of range routeEntriesArrayPtr->nextHopIf.type [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.54. Call with routeEntriesArrayPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                              ucMcRouteEntryBaseIndex = 0;
    GT_U32                              numOfRouteEntries       = 0;
    CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC   routeEntry;
    CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC   routeEntriesArrayGet;


    cpssOsBzero((GT_VOID*) &routeEntry, sizeof(routeEntry));
    cpssOsBzero((GT_VOID*) &routeEntriesArrayGet, sizeof(routeEntriesArrayGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ucMcRouteEntryBaseIndex [0 / PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1],
                           numOfRouteEntries [1 / 1]
                           and routeEntriesArrayPtr {command [CPSS_PACKET_CMD_ROUTE_E],
                                                     bypassTtlExceptionCheckEnable [GT_TRUE],
                                                     cpuCodeIndex [0],
                                                     ageRefresh [GT_TRUE],
                                                     ipv6ScopeCheckEnable [GT_FALSE],
                                                     ipv6ScopeLevel [0],
                                                     ipv6DestinationSiteId [CPSS_IP_SITE_ID_INTERNAL_E],
                                                     mcRpfIfMode [CPSS_EXMXPM_RPF_IF_MODE_PORT_E],
                                                     mcRpfIf {type [CPSS_INTERFACE_TRUNK_E],
                                                                 trunkId [2]},
                                                     mcRpfFailCommand [CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E],
                                                     nextHopIf {type [CPSS_INTERFACE_TRUNK_E],
                                                                 trunkId [2]},
                                                     mirrorToIngressAnalyzer [GT_TRUE],
                                                     cntrSetIndex  [CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E],
                                                     mtuProfileIndex [0],
                                                     appSpecificCpuCodeEnable [GT_TRUE],
                                                     hopLimitMode [CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E],
                                                     qosParamsModify {modifyTc [GT_TRUE],
                                                                      modifyUp [GT_TRUE],
                                                                      modifyDp [GT_TRUE],
                                                                      modifyDscp [GT_TRUE],
                                                                      modifyExp [GT_TRUE]},
                                                     qosParams {tc [0],
                                                                dp [CPSS_DP_GREEN_E],
                                                                up [0],
                                                                dscp [0],
                                                                exp [0]},
                                                     ttl [0],
                                                     ttlMode [CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E],
                                                     mplsLabel [0],
                                                     mplsCommand [CPSS_EXMXPM_MPLS_PUSH_CMD_E] }
            Expected: GT_OK.
        */

        /* Call with ucMcRouteEntryBaseIndex [0] */
        ucMcRouteEntryBaseIndex = 0;
        numOfRouteEntries       = 1;

        routeEntry.command                   = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
        routeEntry.cpuCodeIndex              = 0;
        routeEntry.appSpecificCpuCodeEnable  = GT_TRUE;
        routeEntry.mirrorToIngressAnalyzer   = GT_TRUE;
        routeEntry.bypassTtlExceptionCheckEnable = GT_TRUE;
        routeEntry.hopLimitMode               = CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
        routeEntry.qosParamsModify.modifyTc   = GT_TRUE;
        routeEntry.qosParamsModify.modifyUp   = GT_TRUE;
        routeEntry.qosParamsModify.modifyDp   = GT_TRUE;
        routeEntry.qosParamsModify.modifyDscp = GT_TRUE;
        routeEntry.qosParamsModify.modifyExp  = GT_TRUE;

        routeEntry.qosParams.tc   = 0;
        routeEntry.qosParams.dp   = CPSS_DP_GREEN_E;
        routeEntry.qosParams.up   = 0;
        routeEntry.qosParams.dscp = 0;
        routeEntry.qosParams.exp  = 0;

        routeEntry.cntrSetIndex          = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E;
        routeEntry.mcRpfIfMode           = CPSS_EXMXPM_RPF_IF_MODE_PORT_E;
        routeEntry.mcRpfIf.type          = CPSS_INTERFACE_TRUNK_E;
        routeEntry.mcRpfIf.trunkId       = 100;
        routeEntry.mcRpfFailCommand      = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E;
        routeEntry.ipv6ScopeLevel        = 0;
        routeEntry.ipv6DestinationSiteId = CPSS_IP_SITE_ID_INTERNAL_E;
        routeEntry.ipv6ScopeCheckEnable  = GT_FALSE;
        routeEntry.mtuProfileIndex       = 0;
        routeEntry.ageRefresh            = GT_TRUE;

        routeEntry.nextHopIf.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
        routeEntry.nextHopIf.interfaceInfo.trunkId = 2;
        routeEntry.nextHopIf.outlifType            = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        routeEntry.nextHopIf.outlifPointer.arpPtr  = 100;

        routeEntry.ttl = 0,
        routeEntry.ttlMode = CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E,
        routeEntry.mplsLabel = 0,
        routeEntry.mplsCommand = CPSS_EXMXPM_MPLS_PUSH_CMD_E;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);

        /*
            1.2. Call cpssExMxPmIpMcRouteEntriesRead with the same ucMcRouteEntryBaseIndex,
                                                          numOfRouteEntries
                                                          and non-NULL routeEntriesArrayPtr.
            Expected: GT_OK and the same routeEntriesArrayPtr.
        */
        st = cpssExMxPmIpMcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntriesArrayGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpMcRouteEntriesRead: %d, %d, %d", dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.command, routeEntriesArrayGet.command,
                   "get another routeEntriesArrayPtr->command than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.cpuCodeIndex, routeEntriesArrayGet.cpuCodeIndex,
                   "get another routeEntriesArrayPtr->cpuCodeIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.appSpecificCpuCodeEnable, routeEntriesArrayGet.appSpecificCpuCodeEnable,
                   "get another routeEntriesArrayPtr->appSpecificCpuCodeEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.appSpecificCpuCodeEnable, routeEntriesArrayGet.appSpecificCpuCodeEnable,
                   "get another routeEntriesArrayPtr->appSpecificCpuCodeEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mirrorToIngressAnalyzer, routeEntriesArrayGet.mirrorToIngressAnalyzer,
                   "get another routeEntriesArrayPtr->mirrorToIngressAnalyzer than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.bypassTtlExceptionCheckEnable, routeEntriesArrayGet.bypassTtlExceptionCheckEnable,
                   "get another routeEntriesArrayPtr->bypassTtlExceptionCheckEnable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParamsModify.modifyTc, routeEntriesArrayGet.qosParamsModify.modifyTc,
                   "get another routeEntriesArrayPtr->qosParamsModify.modifyTc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParamsModify.modifyUp, routeEntriesArrayGet.qosParamsModify.modifyUp,
                   "get another routeEntriesArrayPtr->qosParamsModify.modifyUp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParamsModify.modifyDp, routeEntriesArrayGet.qosParamsModify.modifyDp,
                   "get another routeEntriesArrayPtr->qosParamsModify.modifyDp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParamsModify.modifyDscp, routeEntriesArrayGet.qosParamsModify.modifyDscp,
                   "get another routeEntriesArrayPtr->qosParamsModify.modifyDscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParams.tc, routeEntriesArrayGet.qosParams.tc,
                   "get another routeEntriesArrayPtr->qosParams.tc than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParams.dp, routeEntriesArrayGet.qosParams.dp,
                   "get another routeEntriesArrayPtr->qosParams.dp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParams.up, routeEntriesArrayGet.qosParams.up,
                   "get another routeEntriesArrayPtr->qosParams.up than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParams.dscp, routeEntriesArrayGet.qosParams.dscp,
                   "get another routeEntriesArrayPtr->qosParams.dscp than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.cntrSetIndex, routeEntriesArrayGet.cntrSetIndex,
                   "get another routeEntriesArrayPtr->cntrSetIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mcRpfIfMode, routeEntriesArrayGet.mcRpfIfMode,
                   "get another routeEntriesArrayPtr->mcRpfIfMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mcRpfIf.type, routeEntriesArrayGet.mcRpfIf.type,
                   "get another routeEntriesArrayPtr->mcRpfIf.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mcRpfIf.trunkId, routeEntriesArrayGet.mcRpfIf.trunkId,
                   "get another routeEntriesArrayPtr->mcRpfIf.trunkId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mcRpfFailCommand, routeEntriesArrayGet.mcRpfFailCommand,
                   "get another routeEntriesArrayPtr->mcRpfFailCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.ipv6ScopeLevel, routeEntriesArrayGet.ipv6ScopeLevel,
                   "get another routeEntriesArrayPtr->ipv6ScopeLevel than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.ipv6DestinationSiteId, routeEntriesArrayGet.ipv6DestinationSiteId,
                   "get another routeEntriesArrayPtr->ipv6DestinationSiteId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mtuProfileIndex, routeEntriesArrayGet.mtuProfileIndex,
                   "get another routeEntriesArrayPtr->mtuProfileIndex than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.ageRefresh, routeEntriesArrayGet.ageRefresh,
                   "get another routeEntriesArrayPtr->ageRefresh than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.nextHopIf.interfaceInfo.type, routeEntriesArrayGet.nextHopIf.interfaceInfo.type,
                   "get another routeEntriesArrayPtr->nextHopIf.interfaceInfo.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.ttl, routeEntriesArrayGet.ttl,
                   "get another routeEntriesArrayPtr->ttl than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.ttlMode, routeEntriesArrayGet.ttlMode,
                   "get another routeEntriesArrayPtr->ttlMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mplsLabel, routeEntriesArrayGet.mplsLabel,
                   "get another routeEntriesArrayPtr->mplsLabel than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mplsCommand, routeEntriesArrayGet.mplsCommand,
                   "get another routeEntriesArrayPtr->mplsCommand than was set: %d", dev);

        /* 1.3 Call cpssExMxPmIpMcRouteEntriesWrite with mcRpfIfMode = CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E,
                     mcRpfIf.type = CPSS_INTERFACE_VID_E,
                     mcRpfIf.vlanId = 1 */
        /*  Expected: GT_OK and the same routeEntriesArrayPtr. */
        routeEntry.mcRpfIfMode           = CPSS_EXMXPM_RPF_IF_MODE_VLAN_INLIF_E;
        routeEntry.mcRpfIf.type          = CPSS_INTERFACE_VID_E;
        routeEntry.mcRpfIf.vlanId        = 1;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);

        /*Call cpssExMxPmIpMcRouteEntriesRead with the same ucMcRouteEntryBaseIndex,
                                                          numOfRouteEntries
                                                          and non-NULL routeEntriesArrayPtr.
            Expected: GT_OK and the same routeEntriesArrayPtr.
        */
        st = cpssExMxPmIpMcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntriesArrayGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpMcRouteEntriesRead: %d, %d, %d", dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mcRpfIfMode, routeEntriesArrayGet.mcRpfIfMode,
                   "get another routeEntriesArrayPtr->mcRpfIfMode than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mcRpfIf.type, routeEntriesArrayGet.mcRpfIf.type,
                   "get another routeEntriesArrayPtr->mcRpfIf.type than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mcRpfIf.vlanId, routeEntriesArrayGet.mcRpfIf.vlanId,
                   "get another routeEntriesArrayPtr->mcRpfIf.type than was set: %d", dev);

        routeEntry.mcRpfIfMode           = CPSS_EXMXPM_RPF_IF_MODE_L2_VLAN_E;
        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);

        /*Call cpssExMxPmIpMcRouteEntriesRead with the same ucMcRouteEntryBaseIndex,
                                                          numOfRouteEntries
                                                          and non-NULL routeEntriesArrayPtr.
            Expected: GT_OK and the same routeEntriesArrayPtr.
        */
        st = cpssExMxPmIpMcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntriesArrayGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpMcRouteEntriesRead: %d, %d, %d", dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mcRpfIfMode, routeEntriesArrayGet.mcRpfIfMode,
                   "get another routeEntriesArrayPtr->mcRpfIfMode than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mcRpfIf.type, routeEntriesArrayGet.mcRpfIf.type,
                   "get another routeEntriesArrayPtr->mcRpfIf.type than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mcRpfIf.vlanId, routeEntriesArrayGet.mcRpfIf.vlanId,
                   "get another routeEntriesArrayPtr->mcRpfIf.type than was set: %d", dev);

        /* Call with ucMcRouteEntryBaseIndex [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1] */
        ucMcRouteEntryBaseIndex = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum - 1;
        numOfRouteEntries       = 1;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);

        /*
            1.4. Call cpssExMxPmIpMcRouteEntriesRead with the same ucMcRouteEntryBaseIndex,
                                                          numOfRouteEntries
                                                          and non-NULL routeEntriesArrayPtr.
            Expected: GT_OK and the same routeEntriesArrayPtr.
        */
        st = cpssExMxPmIpMcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntriesArrayGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpMcRouteEntriesRead: %d, %d, %d", dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);

        /*
            1.5. Call with out of range numOfRouteEntries [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        numOfRouteEntries = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfRouteEntries = %d", dev, numOfRouteEntries);

        numOfRouteEntries = 1;

        /*
            1.6. Call with out of range numOfRouteEntries [0]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ucMcRouteEntryBaseIndex = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum - 1;
        numOfRouteEntries       = 0;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ucMcRouteEntryBaseIndex + numOfRouteEntries = %d",
                                         dev, ucMcRouteEntryBaseIndex + numOfRouteEntries);

        ucMcRouteEntryBaseIndex = 0;
        numOfRouteEntries       = 1;

        /*
            1.7. Call with out of range routeEntriesArrayPtr->command [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.command = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntriesArrayPtr->command = %d",
                                     dev, routeEntry.command);

        /*
            1.8. Call with routeEntriesArrayPtr->command [CPSS_PACKET_CMD_FORWARD_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.command = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->command = %d",
                                         dev, routeEntry.command);

        routeEntry.command = CPSS_PACKET_CMD_ROUTE_E;

        /*
            1.9. Call with routeEntriesArrayPtr->command [CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E]
                           out of range routeEntriesArrayPtr->cpuCodeIndex [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.cpuCodeIndex = 4;
        routeEntry.command      = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->command = %d, ->cpuCodeIndex = %d",
                                         dev, routeEntry.command, routeEntry.cpuCodeIndex);

        routeEntry.cpuCodeIndex = 0;

        /*
            1.10. Call with routeEntriesArrayPtr->qosParamsModify.modifyTc [GT_TRUE]
                            out of range routeEntriesArrayPtr->qosParams.tc [CPSS_TC_RANGE_CNS]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.qosParamsModify.modifyTc = GT_TRUE;
        routeEntry.qosParams.tc             = CPSS_TC_RANGE_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->qosParamsModify.modifyTc = %d, ->qosParams.tc = %d",
                                     dev, routeEntry.qosParamsModify.modifyTc, routeEntry.qosParams.tc);

        routeEntry.qosParams.tc = 7;

        /*
            1.11. Call with routeEntriesArrayPtr->qosParamsModify.modifyUp [GT_TRUE]
                            out of range routeEntriesArrayPtr->qosParams.up [CPSS_USER_PRIORITY_RANGE_CNS]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.qosParamsModify.modifyUp = GT_TRUE;
        routeEntry.qosParams.up             = CPSS_USER_PRIORITY_RANGE_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->qosParamsModify.modifyUp = %d, ->qosParams.up = %d",
                                         dev, routeEntry.qosParamsModify.modifyUp, routeEntry.qosParams.up);

        routeEntry.qosParams.up = 7;

        /*
            1.13. Call with routeEntriesArrayPtr->qosParamsModify.modifyDp [GT_TRUE]
                            out of range routeEntriesArrayPtr->qosParams.dp [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.qosParamsModify.modifyDp = GT_TRUE;
        routeEntry.qosParams.dp             = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->qosParamsModify.modifyDp = %d, ->qosParams.dp = %d",
                                         dev, routeEntry.qosParamsModify.modifyDp, routeEntry.qosParams.dp);

        routeEntry.qosParams.dp = CPSS_DP_GREEN_E;

        /*
            1.15. Call with routeEntriesArrayPtr->qosParamsModify.modifyDscp [GT_TRUE]
                            out of range routeEntriesArrayPtr->qosParams.dscp [CPSS_DSCP_RANGE_CNS]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.qosParamsModify.modifyDscp = GT_TRUE;
        routeEntry.qosParams.dscp             = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->qosParamsModify.modifyDscp = %d, ->qosParams.dscp = %d",
                                         dev, routeEntry.qosParamsModify.modifyDscp, routeEntry.qosParams.dscp);

        routeEntry.qosParams.dscp = 63;

        /*
            1.17. Call with out of range routeEntriesArrayPtr->cntrSetIndex [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.cntrSetIndex = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntriesArrayPtr->cntrSetIndex = %d",
                                     dev, routeEntry.cntrSetIndex);

        routeEntry.cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E;

        /*
            1.18. Call with out of range routeEntriesArrayPtr->mcRpfIfMode [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.mcRpfIfMode = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntriesArrayPtr->mcRpfIfMode = %d",
                                     dev, routeEntry.mcRpfIfMode);

        routeEntry.mcRpfIfMode = CPSS_EXMXPM_RPF_IF_MODE_PORT_E;

        /*
            1.19. Call with routeEntriesArrayPtr->mcRpfIf.type [0x5AAAAAA5] out of range
                          and other valid params.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.mcRpfIf.type = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntriesArrayPtr->mcRpfIf.type = %d",
                                     dev, routeEntry.mcRpfIf.type);

        routeEntry.mcRpfIf.type = CPSS_INTERFACE_PORT_E;

        /*
            1.20. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_VIDX_E] not supported
                          and other valid params.
            Expected: NOT GT_OK.
        */
        routeEntry.mcRpfIf.type = CPSS_INTERFACE_VIDX_E;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->mcRpfIf.type = %d",
                                         dev, routeEntry.mcRpfIf.type);

        routeEntry.mcRpfIf.type = CPSS_INTERFACE_PORT_E;

        /*
            1.21. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_DEVICE_E] not supported
                          and other valid params.
            Expected: NOT GT_OK.
        */
        routeEntry.mcRpfIf.type = CPSS_INTERFACE_DEVICE_E;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->mcRpfIf.type = %d",
                                         dev, routeEntry.mcRpfIf.type);

        routeEntry.mcRpfIf.type = CPSS_INTERFACE_PORT_E;

        /*
            1.22. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_FABRIC_VIDX_E] not supported
                          and other valid params.
            Expected: NOT GT_OK.
        */
        routeEntry.mcRpfIf.type = CPSS_INTERFACE_FABRIC_VIDX_E;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->mcRpfIf.type = %d",
                                         dev, routeEntry.mcRpfIf.type);

        routeEntry.mcRpfIf.type = CPSS_INTERFACE_PORT_E;

        /*
        [ CPSS_INTERFACE_TRUNK_E ]
            -> out of range
            1.23. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_TRUNK_E]
                            routeEntriesArrayPtr->mcRpfIf.trunkId [4096]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        routeEntry.mcRpfIf.type    = CPSS_INTERFACE_TRUNK_E;
        routeEntry.mcRpfIf.trunkId = 4096;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->mcRpfIf.type = %d, routeEntry.mcRpfIf.trunkId = %d",
                                         dev, routeEntry.mcRpfIf.type, routeEntry.mcRpfIf.trunkId);

        routeEntry.mcRpfIf.trunkId = 100;

        /*
           -> not relevant
            1.24. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_PORT_E]
                            routeEntriesArrayPtr->mcRpfIf.trunkId [4096]
                            and other valid params.
            Expected: GT_OK.
        */
        routeEntry.mcRpfIf.type    = CPSS_INTERFACE_PORT_E;
        routeEntry.mcRpfIf.trunkId = 4096;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->mcRpfIf.type = %d, routeEntry.mcRpfIf.trunkId = %d",
                                     dev, routeEntry.mcRpfIf.type, routeEntry.mcRpfIf.trunkId);

        routeEntry.mcRpfIf.trunkId = 100;

        /*
        [ CPSS_INTERFACE_PORT_E ]
            1.26. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_PORT_E]
                            routeEntriesArrayPtr->mcRpfIf.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        routeEntry.mcRpfIf.type            = CPSS_INTERFACE_PORT_E;
        routeEntry.mcRpfIf.devPort.devNum  = dev;
        routeEntry.mcRpfIf.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->mcRpfIf.type = %d, routeEntry.mcRpfIf.devPort.portNum = %d",
                                         dev, routeEntry.mcRpfIf.type, routeEntry.mcRpfIf.devPort.portNum);

        routeEntry.mcRpfIf.devPort.devNum  = dev;
        routeEntry.mcRpfIf.devPort.portNum = 0;

        /*
            1.28. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_TRUNK_E]
                            routeEntriesArrayPtr->mcRpfIf.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                            and other valid params.
            Expected: GT_OK.
        */
        routeEntry.mcRpfIf.type            = CPSS_INTERFACE_TRUNK_E;
        routeEntry.mcRpfIf.devPort.devNum  = dev;
        routeEntry.mcRpfIf.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->mcRpfIf.type = %d, routeEntry.mcRpfIf.devPort.portNum = %d",
                                     dev, routeEntry.mcRpfIf.type, routeEntry.mcRpfIf.devPort.portNum);

        routeEntry.mcRpfIf.devPort.devNum  = dev;
        routeEntry.mcRpfIf.devPort.portNum = 0;

        /*
        [ CPSS_INTERFACE_VID_E ]
            -> out of range
            1.29. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_VID_E]
                            routeEntriesArrayPtr->mcRpfIf.vlanId [4096]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        routeEntry.mcRpfIf.type   = CPSS_INTERFACE_VID_E;
        routeEntry.mcRpfIf.vlanId = 4096;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->mcRpfIf.type = %d, routeEntry.mcRpfIf.vlanId = %d",
                                         dev, routeEntry.mcRpfIf.type, routeEntry.mcRpfIf.vlanId);

        routeEntry.mcRpfIf.vlanId = 100;

        /*
            -> not relevant
            1.30. Call with routeEntriesArrayPtr->mcRpfIf.type [CPSS_INTERFACE_PORT_E]
                            routeEntriesArrayPtr->mcRpfIf.vlanId [4096]
                            and other valid params.
            Expected: GT_OK.
        */
        routeEntry.mcRpfIf.type   = CPSS_INTERFACE_PORT_E;
        routeEntry.mcRpfIf.vlanId = 4096;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->mcRpfIf.type = %d, routeEntry.mcRpfIf.vlanId = %d",
                                     dev, routeEntry.mcRpfIf.type, routeEntry.mcRpfIf.vlanId);

        routeEntry.mcRpfIf.vlanId = 100;

        /*
            1.31. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [0x5AAAAAA5] out of range
                          and other valid params.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.nextHopIf.interfaceInfo.type = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->nextHopIf.interfaceInfo.type = %d",
                                     dev, routeEntry.nextHopIf.interfaceInfo.type);

        routeEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        /*
            1.32. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_VID_E] out of range
                          and other valid params.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_VID_E;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->nextHopIf.interfaceInfo.type = %d",
                                     dev, routeEntry.nextHopIf.interfaceInfo.type);

        routeEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        /*
            1.33. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_DEVICE_E] out of range
                          and other valid params.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_DEVICE_E;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->nextHopIf.interfaceInfo.type = %d",
                                     dev, routeEntry.nextHopIf.interfaceInfo.type);

        routeEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_PORT_E;

        /*
        [ CPSS_INTERFACE_TRUNK_E ]
            -> out of range
            1.34. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                            routeEntriesArrayPtr->nextHopIf.interfaceInfo.trunkId [4096]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        routeEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        routeEntry.nextHopIf.interfaceInfo.trunkId = 4096;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->nextHopIf.interfaceInfo.type = %d, ->nextHopIf.interfaceInfo.trunkId = %d",
                                         dev, routeEntry.nextHopIf.interfaceInfo.type,
                                         routeEntry.nextHopIf.interfaceInfo.trunkId);

        routeEntry.nextHopIf.interfaceInfo.trunkId = 100;

        /*
           -> not relevant
            1.35. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                            routeEntriesArrayPtr->nextHopIf.interfaceInfo.trunkId [4096]
                            and other valid params.
            Expected: GT_OK.
        */
        routeEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        routeEntry.nextHopIf.interfaceInfo.trunkId = 4096;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->nextHopIf.interfaceInfo.type = %d, ->nextHopIf.interfaceInfo.trunkId = %d",
                                     dev, routeEntry.nextHopIf.interfaceInfo.type,
                                     routeEntry.nextHopIf.interfaceInfo.trunkId);

        routeEntry.nextHopIf.interfaceInfo.trunkId = 100;

        /*
        [ CPSS_INTERFACE_PORT_E ]
            -> out of range
            1.36. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                            routeEntriesArrayPtr->nextHopIf.interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        routeEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        routeEntry.nextHopIf.interfaceInfo.devPort.devNum  = PRV_CPSS_MAX_PP_DEVICES_CNS;
        routeEntry.nextHopIf.interfaceInfo.devPort.portNum = 0;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->nextHopIf.interfaceInfo.type = %d, ->nextHopIf.interfaceInfo.devPort.devNum = %d",
                                         dev, routeEntry.nextHopIf.interfaceInfo.type,
                                         routeEntry.nextHopIf.interfaceInfo.devPort.devNum);

        routeEntry.nextHopIf.interfaceInfo.devPort.devNum  = dev;
        routeEntry.nextHopIf.interfaceInfo.devPort.portNum = 0;

        /*
            1.37. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_PORT_E]
                            routeEntriesArrayPtr->nextHopIf.interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        routeEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_PORT_E;
        routeEntry.nextHopIf.interfaceInfo.devPort.devNum  = dev;
        routeEntry.nextHopIf.interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->nextHopIf.interfaceInfo.type = %d, ->nextHopIf.interfaceInfo.devPort.portNum = %d",
                                         dev, routeEntry.nextHopIf.interfaceInfo.type,
                                         routeEntry.nextHopIf.interfaceInfo.devPort.portNum);

        routeEntry.nextHopIf.interfaceInfo.devPort.devNum  = dev;
        routeEntry.nextHopIf.interfaceInfo.devPort.portNum = 0;

        /*
           -> not relevant
            1.38. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                            routeEntriesArrayPtr->nextHopIf.interfaceInfo.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        routeEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        routeEntry.nextHopIf.interfaceInfo.devPort.devNum  = PRV_CPSS_MAX_PP_DEVICES_CNS;
        routeEntry.nextHopIf.interfaceInfo.devPort.portNum = 0;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->nextHopIf.interfaceInfo.type = %d, ->nextHopIf.interfaceInfo.devPort.devNum = %d",
                                     dev, routeEntry.nextHopIf.interfaceInfo.type,
                                     routeEntry.nextHopIf.interfaceInfo.devPort.devNum);

        routeEntry.nextHopIf.interfaceInfo.devPort.devNum  = dev;
        routeEntry.nextHopIf.interfaceInfo.devPort.portNum = 0;

        /*
            1.39. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                            routeEntriesArrayPtr->nextHopIf.interfaceInfo.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        routeEntry.nextHopIf.interfaceInfo.type    = CPSS_INTERFACE_VIDX_E;
        routeEntry.nextHopIf.interfaceInfo.devPort.devNum  = dev;
        routeEntry.nextHopIf.interfaceInfo.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->nextHopIf.interfaceInfo.type = %d, ->nextHopIf.interfaceInfo.devPort.portNum = %d",
                                     dev, routeEntry.nextHopIf.interfaceInfo.type,
                                     routeEntry.nextHopIf.interfaceInfo.devPort.portNum);

        routeEntry.nextHopIf.interfaceInfo.devPort.devNum  = dev;
        routeEntry.nextHopIf.interfaceInfo.devPort.portNum = 0;

        /*
        [ CPSS_INTERFACE_VIDX_E ]
            -> out of range
            1.40. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_VIDX_E]
                            routeEntriesArrayPtr->nextHopIf.interfaceInfo.vidx [4096]
                            and other valid params.
            Expected: NOT GT_OK.
        */
        routeEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_VIDX_E;
        routeEntry.nextHopIf.interfaceInfo.vidx = 4096;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->nextHopIf.interfaceInfo.type = %d, ->nextHopIf.interfaceInfo.vidx = %d",
                                         dev, routeEntry.nextHopIf.interfaceInfo.type,
                                         routeEntry.nextHopIf.interfaceInfo.vidx);

        routeEntry.nextHopIf.interfaceInfo.vidx = 100;

        /*
           -> not relevant
            1.41. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_VID_E] not supported
                            and other valid params.
            Expected: NOT GT_OK.
        */
        routeEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_VID_E;
        routeEntry.nextHopIf.interfaceInfo.vidx = 100;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ->nextHopIf.interfaceInfo.type = %d",
                                         dev, routeEntry.nextHopIf.interfaceInfo.type);

        /*
        [ CPSS_INTERFACE_FABRIC_VIDX_E ]
            -> out of range
            1.42. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_FABRIC_VIDX_E]
                            routeEntriesArrayPtr->nextHopIf.interfaceInfo.fabricVidx [4096]
                           and other valid params.
            Expected: NOT GT_OK.
        */
        routeEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_FABRIC_VIDX_E;
        routeEntry.nextHopIf.interfaceInfo.fabricVidx = 4096;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->nextHopIf.interfaceInfo.type = %d, ->nextHopIf.interfaceInfo.fabricVidx = %d",
                                         dev, routeEntry.nextHopIf.interfaceInfo.type,
                                         routeEntry.nextHopIf.interfaceInfo.fabricVidx);

        routeEntry.nextHopIf.interfaceInfo.fabricVidx = 100;

        /*
            -> not relevant
            1.43. Call with routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [CPSS_INTERFACE_TRUNK_E]
                            routeEntriesArrayPtr->nextHopIf.interfaceInfo.fabricVidx [4096]
                           and other valid params.
            Expected: GT_OK.
        */
        routeEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        routeEntry.nextHopIf.interfaceInfo.fabricVidx = 4096;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->nextHopIf.interfaceInfo.type = %d, ->nextHopIf.interfaceInfo.fabricVidx = %d",
                                     dev, routeEntry.nextHopIf.interfaceInfo.type,
                                     routeEntry.nextHopIf.interfaceInfo.fabricVidx);

        routeEntry.nextHopIf.interfaceInfo.fabricVidx = 100;

        /*
            1.44. Call with out of range routeEntriesArrayPtr->mcRpfFailCommand [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.mcRpfFailCommand = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntriesArrayPtr->mcRpfFailCommand = %d",
                                     dev, routeEntry.mcRpfFailCommand);

        routeEntry.mcRpfFailCommand = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E;

        /*
            1.45. Call with out of range routeEntriesArrayPtr->ipv6ScopeLevel [4] (out of range)
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.ipv6ScopeLevel = 4;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->ipv6ScopeLevel = %d",
                                         dev, routeEntry.ipv6ScopeLevel);

        routeEntry.ipv6ScopeLevel = 0;

        /*
            1.46. Call with out of range routeEntriesArrayPtr->hopLimitMode [0x5AAAAAA5] (out of range)
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.hopLimitMode = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntriesArrayPtr->hopLimitMode = %d",
                                     dev, routeEntry.hopLimitMode);

        routeEntry.hopLimitMode = CPSS_EXMXPM_IP_HOP_LIMIT_DECREMENTED_BY_ONE_E;

        /*
            1.47. Call with out of range routeEntriesArrayPtr->ttl [256] (out of range)
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.ttl = 256;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->ttl = %d",
                                         dev, routeEntry.ttl);

        routeEntry.ttl = 0;

        /*
            1.48. Call with out of range routeEntriesArrayPtr->ttlMode [0x5AAAAAA5] (out of range)
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.ttlMode = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntriesArrayPtr->ttlMode = %d",
                                     dev, routeEntry.ttlMode);

        routeEntry.ttlMode = CPSS_EXMXPM_IP_TTL_DECREMENTED_BY_ENTRY_E;

        /*
            1.49. Call with out of range routeEntriesArrayPtr->mplsLabel [BIT_20] (out of range)
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.mplsLabel = BIT_20;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->mplsLabel = %d",
                                         dev, routeEntry.mplsLabel);

        routeEntry.mplsLabel = 0;

        /*
            1.50. Call with out of range routeEntriesArrayPtr->mplsCommand [0x5AAAAAA5] (out of range)
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.mplsCommand = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntriesArrayPtr->mplsCommand = %d",
                                     dev, routeEntry.mplsCommand);

        routeEntry.mplsCommand = CPSS_EXMXPM_MPLS_PHP_CMD_E;

        /*
            1.51. Call with out of range routeEntriesArrayPtr->ipv6DestinationSiteId [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        routeEntry.ipv6DestinationSiteId = IP_TABLES_INVALID_ENUM_CNS;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, routeEntriesArrayPtr->ipv6DestinationSiteId = %d",
                                     dev, routeEntry.ipv6DestinationSiteId);

        routeEntry.ipv6DestinationSiteId = CPSS_IP_SITE_ID_INTERNAL_E;

        /*
            1.52. Call with out of range routeEntriesArrayPtr->mtuProfileIndex [16]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.mtuProfileIndex = 16;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->mtuProfileIndex = %d",
                                         dev, routeEntry.mtuProfileIndex);

        routeEntry.mtuProfileIndex = 0;

        /*
            1.53. Call with out of range routeEntriesArrayPtr->nextHopIf.interfaceInfo.type [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routeEntry.nextHopIf.interfaceInfo.type = 65536;

        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routeEntriesArrayPtr->nextHopIf.interfaceInfo.type = %d",
                                         dev, routeEntry.nextHopIf.interfaceInfo.type);

        routeEntry.nextHopIf.interfaceInfo.type = 0;

        /*
            1.54. Call with routeEntriesArrayPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, routeEntriesArrayPtr = NULL", dev);
    }

    ucMcRouteEntryBaseIndex = 0;
    numOfRouteEntries       = 1;

    routeEntry.command                   = CPSS_PACKET_CMD_ROUTE_E;
    routeEntry.cpuCodeIndex              = 0;
    routeEntry.appSpecificCpuCodeEnable  = GT_TRUE;
    routeEntry.mirrorToIngressAnalyzer   = GT_TRUE;
    routeEntry.bypassTtlExceptionCheckEnable = GT_TRUE;
    routeEntry.hopLimitMode               = CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
    routeEntry.qosParamsModify.modifyTc   = GT_TRUE;
    routeEntry.qosParamsModify.modifyUp   = GT_TRUE;
    routeEntry.qosParamsModify.modifyDp   = GT_TRUE;
    routeEntry.qosParamsModify.modifyDscp = GT_TRUE;
    routeEntry.qosParamsModify.modifyExp  = GT_TRUE;

    routeEntry.qosParams.tc   = 0;
    routeEntry.qosParams.dp   = CPSS_DP_GREEN_E;
    routeEntry.qosParams.up   = 0;
    routeEntry.qosParams.dscp = 0;
    routeEntry.qosParams.exp  = 0;

    routeEntry.cntrSetIndex          = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E;
    routeEntry.mcRpfIfMode           = CPSS_EXMXPM_RPF_IF_MODE_PORT_E;
    routeEntry.mcRpfIf.type          = CPSS_INTERFACE_TRUNK_E;
    routeEntry.mcRpfIf.trunkId       = 100;
    routeEntry.mcRpfFailCommand      = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E;
    routeEntry.ipv6ScopeLevel        = 0;
    routeEntry.ipv6DestinationSiteId = CPSS_IP_SITE_ID_INTERNAL_E;
    routeEntry.ipv6ScopeCheckEnable  = GT_FALSE;
    routeEntry.mtuProfileIndex       = 0;
    routeEntry.ageRefresh            = GT_TRUE;

    routeEntry.nextHopIf.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
    routeEntry.nextHopIf.interfaceInfo.trunkId = 2;
    routeEntry.nextHopIf.outlifType            = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    routeEntry.nextHopIf.outlifPointer.arpPtr  = 100;

    routeEntry.ttl = 0,
    routeEntry.ttlMode = CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E,
    routeEntry.mplsLabel = 0,
    routeEntry.mplsCommand = CPSS_EXMXPM_MPLS_PUSH_CMD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpMcRouteEntriesWrite(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpMcRouteEntriesRead
(
    IN  GT_U8                                devNum,
    IN  GT_U32                               ucMcRouteEntryBaseIndex,
    IN  GT_U32                               numOfRouteEntries,
    OUT CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC    *routeEntriesArrayPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpMcRouteEntriesRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ucMcRouteEntryBaseIndex [0 / PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1],
                   numOfRouteEntries [1 / 1]
                   and non-NULL routeEntriesArrayPtr.
    Expected: GT_OK.
    1.2. Call with out of range ucMcRouteEntryBaseIndex [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range numOfRouteEntries [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range numOfRouteEntries [0]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with routeEntriesArrayPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                              ucMcRouteEntryBaseIndex = 0;
    GT_U32                              numOfRouteEntries       = 0;
    CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC   routeEntry;


    cpssOsBzero((GT_VOID*) &routeEntry, sizeof(routeEntry));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ucMcRouteEntryBaseIndex [0 / PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1],
                           numOfRouteEntries [1 / 1]
                           and non-NULL routeEntriesArrayPtr.
            Expected: GT_OK.
        */

        /* Call with ucMcRouteEntryBaseIndex [0] */
        ucMcRouteEntryBaseIndex = 0;
        numOfRouteEntries       = 1;

        st = cpssExMxPmIpMcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);

        /* Call with ucMcRouteEntryBaseIndex [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum-1] */
        ucMcRouteEntryBaseIndex = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum - 1;
        numOfRouteEntries       = 1;

        st = cpssExMxPmIpMcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryBaseIndex, numOfRouteEntries);


        /*
            1.2. Call with out of range ucMcRouteEntryBaseIndex [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ucMcRouteEntryBaseIndex = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum;

        st = cpssExMxPmIpMcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryBaseIndex);

        ucMcRouteEntryBaseIndex = 1;

        /*
            1.3. Call with out of range numOfRouteEntries [PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        numOfRouteEntries = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum;

        st = cpssExMxPmIpMcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfRouteEntries = %d", dev, numOfRouteEntries);

        numOfRouteEntries = 1;

        /*
            1.4. Call with out of range numOfRouteEntries [0]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ucMcRouteEntryBaseIndex = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum - 1;
        numOfRouteEntries       = 0;

        st = cpssExMxPmIpMcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ucMcRouteEntryBaseIndex + numOfRouteEntries = %d",
                                         dev, ucMcRouteEntryBaseIndex + numOfRouteEntries);

        ucMcRouteEntryBaseIndex = 0;
        numOfRouteEntries       = 1;

        /*
            1.5. Call with routeEntriesArrayPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpMcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, routeEntriesArrayPtr = NULL", dev);
    }

    ucMcRouteEntryBaseIndex = 0;
    numOfRouteEntries       = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpMcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpMcRouteEntriesRead(dev, ucMcRouteEntryBaseIndex, numOfRouteEntries, &routeEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpRouteEntryAgeRefreshSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  ucMcRouteEntryIndex,
    IN  GT_U32  ageRefresh
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpRouteEntryAgeRefreshSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ucMcRouteEntryIndex [0 / 4095]
                   and ageRefresh [0 / 1].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpRouteEntryAgeRefreshGet with the same ucMcRouteEntryIndex
                                                       and non-NULL ageRefreshPtr.
    Expected: GT_OK and the same ageRefresh.
    1.3. Call with out of range ucMcRouteEntryIndex [16384]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range ageRefresh [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      ucMcRouteEntryIndex = 0;
    GT_U32      ageRefresh          = 0;
    GT_U32      ageRefreshGet       = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ucMcRouteEntryIndex [0 / 10]
                           and ageRefresh [0 / 1].
            Expected: GT_OK.
        */

        /* Call with ucMcRouteEntryIndex [0] */
        ucMcRouteEntryIndex = 0;
        ageRefresh          = 0;

        st = cpssExMxPmIpRouteEntryAgeRefreshSet(dev, ucMcRouteEntryIndex, ageRefresh);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryIndex, ageRefresh);

        /*
            1.2. Call cpssExMxPmIpRouteEntryAgeRefreshGet with the same ucMcRouteEntryIndex
                                                               and non-NULL ageRefreshPtr.
            Expected: GT_OK and the same ageRefresh.
        */
        st = cpssExMxPmIpRouteEntryAgeRefreshGet(dev, ucMcRouteEntryIndex, &ageRefreshGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpRouteEntryAgeRefreshGet: %d, %d", dev, ucMcRouteEntryIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ageRefresh, ageRefreshGet,
                       "get another ageRefresh than was set: %d", dev);

        /* Call with ucMcRouteEntryIndex [10] */
        ucMcRouteEntryIndex = 10;
        ageRefresh          = 1;

        st = cpssExMxPmIpRouteEntryAgeRefreshSet(dev, ucMcRouteEntryIndex, ageRefresh);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryIndex, ageRefresh);

        /*
            1.2. Call cpssExMxPmIpRouteEntryAgeRefreshGet with the same ucMcRouteEntryIndex
                                                               and non-NULL ageRefreshPtr.
            Expected: GT_OK and the same ageRefresh.
        */
        st = cpssExMxPmIpRouteEntryAgeRefreshGet(dev, ucMcRouteEntryIndex, &ageRefreshGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpRouteEntryAgeRefreshGet: %d, %d", dev, ucMcRouteEntryIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ageRefresh, ageRefreshGet,
                       "get another ageRefresh than was set: %d", dev);

        /*
            1.3. Call with out of range ucMcRouteEntryIndex [16384]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ucMcRouteEntryIndex = 16384;

        st = cpssExMxPmIpRouteEntryAgeRefreshSet(dev, ucMcRouteEntryIndex, ageRefresh);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryIndex);

        ucMcRouteEntryIndex = 0;

        /*
            1.4. Call with out of range ageRefresh [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ageRefresh = 2;

        st = cpssExMxPmIpRouteEntryAgeRefreshSet(dev, ucMcRouteEntryIndex, ageRefresh);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ageRefresh = %d", dev, ageRefresh);
    }

    ucMcRouteEntryIndex = 0;
    ageRefresh          = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpRouteEntryAgeRefreshSet(dev, ucMcRouteEntryIndex, ageRefresh);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpRouteEntryAgeRefreshSet(dev, ucMcRouteEntryIndex, ageRefresh);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpRouteEntryAgeRefreshGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  ucMcRouteEntryIndex,
    OUT GT_U32  *ageRefreshPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpRouteEntryAgeRefreshGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ucMcRouteEntryIndex [0 / 10]
                   and non-NULL ageRefreshPtr.
    Expected: GT_OK.
    1.2. Call with out of range ucMcRouteEntryIndex [16384]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with ageRefreshPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      ucMcRouteEntryIndex = 0;
    GT_U32      ageRefresh          = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ucMcRouteEntryIndex [0 / 10]
                           and ageRefresh [0 / 1].
            Expected: GT_OK.
        */

        /* Call with ucMcRouteEntryIndex [0] */
        ucMcRouteEntryIndex = 0;

        st = cpssExMxPmIpRouteEntryAgeRefreshGet(dev, ucMcRouteEntryIndex, &ageRefresh);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryIndex);

        /* Call with ucMcRouteEntryIndex [10] */
        ucMcRouteEntryIndex = 10;

        st = cpssExMxPmIpRouteEntryAgeRefreshGet(dev, ucMcRouteEntryIndex, &ageRefresh);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryIndex);

        /*
            1.2. Call with out of range ucMcRouteEntryIndex [16384]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ucMcRouteEntryIndex = 16384;

        st = cpssExMxPmIpRouteEntryAgeRefreshGet(dev, ucMcRouteEntryIndex, &ageRefresh);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ucMcRouteEntryIndex);

        ucMcRouteEntryIndex = 0;

        /*
            1.3. Call with ageRefreshPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpRouteEntryAgeRefreshGet(dev, ucMcRouteEntryIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ageRefreshPtr = NULL", dev);
    }

    ucMcRouteEntryIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpRouteEntryAgeRefreshGet(dev, ucMcRouteEntryIndex, &ageRefresh);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpRouteEntryAgeRefreshGet(dev, ucMcRouteEntryIndex, &ageRefresh);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpRouterArpAddrWrite
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerArpIndex,
    IN  GT_ETHERADDR                *arpMacAddrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpRouterArpAddrWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with routerArpIndex [0 / 16383]
                   and arpMacAddrPtr->arEther [0x0, 0x0, 0x0, 0x0, 0x0, 0x0 /
                                               0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpRouterArpAddrRead with the same routerArpIndex
                                               and non-NULL arpMacAddrPtr.
    Expected: GT_OK and the same arpMacAddrPtr.
    1.3. Call with out of range routerArpIndex [16384]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with arpMacAddrPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32          routerArpIndex = 0;
    GT_ETHERADDR    arpMacAddr;
    GT_ETHERADDR    arpMacAddrGet;
    GT_BOOL         isEqual        = GT_FALSE;
    GT_U32          numEntries     = 0;

    cpssOsBzero((GT_VOID*) &arpMacAddr, sizeof(arpMacAddr));
    cpssOsBzero((GT_VOID*) &arpMacAddrGet, sizeof(arpMacAddrGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        numEntries = prvGetMaxARPTableSize(dev);

        /*
            1.1. Call with routerArpIndex [0 / MAX]
                           and arpMacAddrPtr->arEther [0x0, 0x0, 0x0, 0x0, 0x0, 0x0 /
                                                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF].
            Expected: GT_OK.
        */

        /* Call with routerArpIndex [0] */
        routerArpIndex = 0;

        arpMacAddr.arEther[0] = 0;
        arpMacAddr.arEther[1] = 0;
        arpMacAddr.arEther[2] = 0;
        arpMacAddr.arEther[3] = 0;
        arpMacAddr.arEther[4] = 0;
        arpMacAddr.arEther[5] = 0;

        st = cpssExMxPmIpRouterArpAddrWrite(dev, routerArpIndex, &arpMacAddr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, routerArpIndex);

        /*
            1.2. Call cpssExMxPmIpRouterArpAddrRead with the same routerArpIndex
                                                       and non-NULL arpMacAddrPtr.
            Expected: GT_OK and the same arpMacAddrPtr.
        */
        st = cpssExMxPmIpRouterArpAddrRead(dev, routerArpIndex, &arpMacAddrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpRouterArpAddrRead: %d, %d", dev, routerArpIndex);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &arpMacAddr,
                                     (GT_VOID*) &arpMacAddrGet,
                                     sizeof(arpMacAddr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another arpMacAddr than was set: %d", dev);

        /* Call with routerArpIndex [MAX] */
        routerArpIndex = numEntries-1;

        arpMacAddr.arEther[0] = 0xFF;
        arpMacAddr.arEther[1] = 0xFF;
        arpMacAddr.arEther[2] = 0xFF;
        arpMacAddr.arEther[3] = 0xFF;
        arpMacAddr.arEther[4] = 0xFF;
        arpMacAddr.arEther[5] = 0xFF;

        st = cpssExMxPmIpRouterArpAddrWrite(dev, routerArpIndex, &arpMacAddr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, routerArpIndex);

        /*
            1.2. Call cpssExMxPmIpRouterArpAddrRead with the same routerArpIndex
                                                       and non-NULL arpMacAddrPtr.
            Expected: GT_OK and the same arpMacAddrPtr.
        */
        st = cpssExMxPmIpRouterArpAddrRead(dev, routerArpIndex, &arpMacAddrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpRouterArpAddrRead: %d, %d", dev, routerArpIndex);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &arpMacAddr,
                                     (GT_VOID*) &arpMacAddrGet,
                                     sizeof(arpMacAddr))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another arpMacAddr than was set: %d", dev);

        /*
            1.3. Call with out of range routerArpIndex [MAX+1]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routerArpIndex = numEntries;

        st = cpssExMxPmIpRouterArpAddrWrite(dev, routerArpIndex, &arpMacAddr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, routerArpIndex);

        routerArpIndex = 0;

        /*
            1.4. Call with arpMacAddrPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpRouterArpAddrWrite(dev, routerArpIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, arpMacAddrPtr = NULL", dev);
    }

    routerArpIndex = 0;

    arpMacAddr.arEther[0] = 0;
    arpMacAddr.arEther[1] = 0;
    arpMacAddr.arEther[2] = 0;
    arpMacAddr.arEther[3] = 0;
    arpMacAddr.arEther[4] = 0;
    arpMacAddr.arEther[5] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpRouterArpAddrWrite(dev, routerArpIndex, &arpMacAddr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpRouterArpAddrWrite(dev, routerArpIndex, &arpMacAddr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpRouterArpAddrRead
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      routerArpIndex,
    OUT GT_ETHERADDR                *arpMacAddrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpRouterArpAddrRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with routerArpIndex [0 / MAX]
                   and non-NULL arpMacAddrPtr.
    Expected: GT_OK.
    1.2. Call with out of range routerArpIndex [MAX+1]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with arpMacAddrPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32          routerArpIndex = 0;
    GT_ETHERADDR    arpMacAddr;
    GT_U32          numEntries = 0;

    cpssOsBzero((GT_VOID*) &arpMacAddr, sizeof(arpMacAddr));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        numEntries = prvGetMaxARPTableSize(dev);

        /*
            1.1. Call with routerArpIndex [0 / MAX]
                           and non-NULL arpMacAddrPtr.
            Expected: GT_OK.
        */

        /* Call with routerArpIndex [0] */
        routerArpIndex = 0;

        st = cpssExMxPmIpRouterArpAddrRead(dev, routerArpIndex, &arpMacAddr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, routerArpIndex);

        /* Call with routerArpIndex [MAX] */
        routerArpIndex = numEntries-1;

        st = cpssExMxPmIpRouterArpAddrRead(dev, routerArpIndex, &arpMacAddr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, routerArpIndex);

        /*
            1.2. Call with out of range routerArpIndex [MAX+1]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        routerArpIndex = numEntries+1;

        st = cpssExMxPmIpRouterArpAddrRead(dev, routerArpIndex, &arpMacAddr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, routerArpIndex);

        routerArpIndex = 0;

        /*
            1.3. Call with arpMacAddrPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpRouterArpAddrRead(dev, routerArpIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, arpMacAddrPtr = NULL", dev);
    }

    routerArpIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpRouterArpAddrRead(dev, routerArpIndex, &arpMacAddr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpRouterArpAddrRead(dev, routerArpIndex, &arpMacAddr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill UcRoute table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpFillUcRouteTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in UcRoute table.
         Call cpssExMxPmIpUcRouteEntriesWrite with ucMcRouteEntryBaseIndex [0..numEntries - 1],
                                                       numOfRouteEntries [1],
                                                       routeEntriesArrayPtr {type  {CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E},
                                                                             entry { regularEntry{
                                                                                                  command [CPSS_PACKET_CMD_ROUTE_E],
                                                                                                  bypassTtlExceptionCheckEnable [GT_TRUE],
                                                                                                  cpuCodeIndex [0],
                                                                                                  ageRefresh [GT_TRUE],
                                                                                                  ipv6ScopeCheckEnable [GT_FALSE],
                                                                                                  ipv6DestinationSiteId [CPSS_IP_SITE_ID_INTERNAL_E],
                                                                                                  sipAccessLevel [0],
                                                                                                  dipAccessLevel [0],
                                                                                                  ucRpfOrIcmpRedirectIfMode [CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E],
                                                                                                  sipSaCheckEnable [GT_TRUE],
                                                                                                  trapMirrorArpBcToCpu [GT_TRUE],
                                                                                                  ucPacketSipFilterEnable [GT_TRUE],
                                                                                                  nextHopIf {
                                                                                                              outlifType [CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                                                              interfaceInfo {
                                                                                                                              type [CPSS_INTERFACE_TRUNK_E],
                                                                                                                              trunkId [100] }
                                                                                                              outlifPointer { arpPtr [100] } },
                                                                                                  nextHopVlanId [10],
                                                                                                  mirrorToIngressAnalyzer [GT_TRUE],
                                                                                                  cntrSetIndex [CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E],
                                                                                                  mtuProfileIndex [15],
                                                                                                  icmpRedirectEnable [GT_TRUE],
                                                                                                  appSpecificCpuCodeEnable [GT_FALSE],
                                                                                                  hopLimitMode [CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E],
                                                                                                  ttl [0],
                                                                                                  ttlMode [CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E],
                                                                                                  mplsLabel [0],
                                                                                                  mplsCommand [CPSS_EXMXPM_MPLS_PUSH_CMD_E],
                                                                                                  qosParamsModify { modifyTc [GT_FALSE],
                                                                                                                    modifyUp [GT_FALSE],
                                                                                                                    modifyDp [GT_FALSE],
                                                                                                                    modifyDscp [GT_FALSE],
                                                                                                                    modifyExp [GT_FALSE] },
                                                                                                  qosParams {   tc [0],
                                                                                                                dp [0],
                                                                                                                up [0],
                                                                                                                dscp [0],
                                                                                                                exp [0] } } }
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmIpUcRouteEntriesWrite with ucMcRouteEntryBaseIndex [numEntries],
                                                 and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in UcRoute table and compare with original.
         Call cpssExMxPmIpUcRouteEntriesRead with not NULL routeEntriesArrayPtr and other params from 1.2.
    Expected: GT_OK and the same params as was set.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmIpUcRouteEntriesRead with ucMcRouteEntryBaseIndex [numEntries],
                                                 and other params from 1.4.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual           = GT_FALSE;
    GT_U32      numEntries        = 0;
    GT_U32      iTemp             = 0;
    GT_U32      numOfRouteEntries = 0;

    CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC   routeEntry;
    CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC   routeEntryGet;


    cpssOsBzero((GT_VOID*) &routeEntry, sizeof(routeEntry));
    cpssOsBzero((GT_VOID*) &routeEntryGet, sizeof(routeEntryGet));

    /* Fill the entry for UcRoute table */
    numOfRouteEntries = 1;

    routeEntry.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;
    routeEntry.entry.regularEntry.command = CPSS_PACKET_CMD_ROUTE_E;
    routeEntry.entry.regularEntry.bypassTtlExceptionCheckEnable = GT_TRUE;
    routeEntry.entry.regularEntry.cpuCodeIndex = 0;
    routeEntry.entry.regularEntry.ageRefresh = GT_TRUE;
    routeEntry.entry.regularEntry.ipv6ScopeCheckEnable = GT_FALSE;
    routeEntry.entry.regularEntry.ipv6DestinationSiteId = CPSS_IP_SITE_ID_INTERNAL_E;
    routeEntry.entry.regularEntry.sipAccessLevel = 0;
    routeEntry.entry.regularEntry.dipAccessLevel = 0;
    routeEntry.entry.regularEntry.ucRpfOrIcmpRedirectIfMode = CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E;
    routeEntry.entry.regularEntry.sipSaCheckEnable = GT_TRUE;
    routeEntry.entry.regularEntry.trapMirrorArpBcToCpu = GT_TRUE;
    routeEntry.entry.regularEntry.ucPacketSipFilterEnable = GT_TRUE;

    routeEntry.entry.regularEntry.nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
    routeEntry.entry.regularEntry.nextHopIf.interfaceInfo.trunkId = 100;
    routeEntry.entry.regularEntry.nextHopIf.outlifPointer.arpPtr = 100;

    routeEntry.entry.regularEntry.nextHopVlanId = 10;
    routeEntry.entry.regularEntry.mirrorToIngressAnalyzer = GT_TRUE;
    routeEntry.entry.regularEntry.cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E;
    routeEntry.entry.regularEntry.mtuProfileIndex = 15;
    routeEntry.entry.regularEntry.icmpRedirectEnable = GT_TRUE;
    routeEntry.entry.regularEntry.appSpecificCpuCodeEnable = GT_FALSE;
    routeEntry.entry.regularEntry.hopLimitMode = CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
    routeEntry.entry.regularEntry.ttl = 0;
    routeEntry.entry.regularEntry.ttlMode = CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
    routeEntry.entry.regularEntry.mplsLabel = 0;
    routeEntry.entry.regularEntry.mplsCommand = CPSS_EXMXPM_MPLS_PUSH_CMD_E;

    routeEntry.entry.regularEntry.qosParamsModify.modifyTc = GT_FALSE;
    routeEntry.entry.regularEntry.qosParamsModify.modifyUp = GT_FALSE;
    routeEntry.entry.regularEntry.qosParamsModify.modifyDp = GT_FALSE;
    routeEntry.entry.regularEntry.qosParamsModify.modifyDscp = GT_FALSE;
    routeEntry.entry.regularEntry.qosParamsModify.modifyExp = GT_FALSE;

    routeEntry.entry.regularEntry.qosParams.tc = 0;
    routeEntry.entry.regularEntry.qosParams.dp = CPSS_DP_GREEN_E;
    routeEntry.entry.regularEntry.qosParams.up = 0;
    routeEntry.entry.regularEntry.qosParams.dscp = 0;
    routeEntry.entry.regularEntry.qosParams.exp = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum;

        /* 1.2. Fill all entries in UcRoute table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            routeEntry.entry.regularEntry.ttl = iTemp % 7;
            routeEntry.entry.regularEntry.nextHopVlanId   = (GT_U16)(iTemp % 4095);
            routeEntry.entry.regularEntry.mtuProfileIndex = iTemp % 15;

            st = cpssExMxPmIpUcRouteEntriesWrite(dev, iTemp, numOfRouteEntries, &routeEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpUcRouteEntriesWrite: %d, %d, %d", dev, iTemp, numOfRouteEntries);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmIpUcRouteEntriesWrite(dev, numEntries, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpUcRouteEntriesWrite: %d, %d, %d", dev, numEntries, numOfRouteEntries);

        /* 1.4. Read all entries in UcRoute table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            routeEntry.entry.regularEntry.ttl = iTemp % 7;
            routeEntry.entry.regularEntry.nextHopVlanId   = (GT_U16)(iTemp % 4095);
            routeEntry.entry.regularEntry.mtuProfileIndex = iTemp % 15;

            routeEntryGet.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

            st = cpssExMxPmIpUcRouteEntriesRead(dev, iTemp, numOfRouteEntries, &routeEntryGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpUcRouteEntriesRead: %d, %d, %d",
                                        dev, iTemp, numOfRouteEntries);

            UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry.type, routeEntryGet.type, "got another routeEntry.type than was set: %d, %d", dev, iTemp);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &routeEntry.entry.regularEntry,
                                         (GT_VOID*) &routeEntryGet.entry.regularEntry,
                                         sizeof(routeEntry.entry.regularEntry) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "got another routeEntry.entry.regularEntry than was set: %d, %d", dev, iTemp);
        }

        /* 1.5. Try to read entry with index out of range. */
        routeEntryGet.type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        st = cpssExMxPmIpUcRouteEntriesRead(dev, numEntries, numOfRouteEntries, &routeEntryGet);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpUcRouteEntriesRead: %d, %d, %d",
                                    dev, iTemp, numOfRouteEntries);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill UcRoute table (group write - with numEntries > 1)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpFillUcRouteTable_GroupWrite)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in UcRoute table.
         Call cpssExMxPmIpUcRouteEntriesWrite with ucMcRouteEntryBaseIndex [0..numEntries - 1],
                                                       numOfRouteEntries [10],
                                                       routeEntriesArrayPtr {type  {CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E},
                                                                             entry { regularEntry{
                                                                                                  command [CPSS_PACKET_CMD_ROUTE_E],
                                                                                                  bypassTtlExceptionCheckEnable [GT_TRUE],
                                                                                                  cpuCodeIndex [0],
                                                                                                  ageRefresh [GT_TRUE],
                                                                                                  ipv6ScopeCheckEnable [GT_FALSE],
                                                                                                  ipv6DestinationSiteId [CPSS_IP_SITE_ID_INTERNAL_E],
                                                                                                  sipAccessLevel [0],
                                                                                                  dipAccessLevel [0],
                                                                                                  ucRpfOrIcmpRedirectIfMode [CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E],
                                                                                                  sipSaCheckEnable [GT_TRUE],
                                                                                                  trapMirrorArpBcToCpu [GT_TRUE],
                                                                                                  ucPacketSipFilterEnable [GT_TRUE],
                                                                                                  nextHopIf {
                                                                                                              outlifType [CPSS_EXMXPM_OUTLIF_TYPE_LL_E],
                                                                                                              interfaceInfo {
                                                                                                                              type [CPSS_INTERFACE_TRUNK_E],
                                                                                                                              trunkId [100] }
                                                                                                              outlifPointer { arpPtr [100] } },
                                                                                                  nextHopVlanId [10],
                                                                                                  mirrorToIngressAnalyzer [GT_TRUE],
                                                                                                  cntrSetIndex [CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E],
                                                                                                  mtuProfileIndex [15],
                                                                                                  icmpRedirectEnable [GT_TRUE],
                                                                                                  appSpecificCpuCodeEnable [GT_FALSE],
                                                                                                  hopLimitMode [CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E],
                                                                                                  ttl [0],
                                                                                                  ttlMode [CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E],
                                                                                                  mplsLabel [0],
                                                                                                  mplsCommand [CPSS_EXMXPM_MPLS_PUSH_CMD_E],
                                                                                                  qosParamsModify { modifyTc [GT_FALSE],
                                                                                                                    modifyUp [GT_FALSE],
                                                                                                                    modifyDp [GT_FALSE],
                                                                                                                    modifyDscp [GT_FALSE],
                                                                                                                    modifyExp [GT_FALSE] },
                                                                                                  qosParams {   tc [0],
                                                                                                                dp [0],
                                                                                                                up [0],
                                                                                                                dscp [0],
                                                                                                                exp [0] } } }
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmIpUcRouteEntriesWrite with (ucMcRouteEntryBaseIndex+numOfRouteEntries) > numEntries,
                                                 and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in UcRoute table and compare with original.
         Call cpssExMxPmIpUcRouteEntriesRead with not NULL routeEntriesArrayPtr and other params from 1.2.
    Expected: GT_OK and the same params as was set.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmIpUcRouteEntriesRead with (ucMcRouteEntryBaseIndex+numOfRouteEntries) > numEntries,
                                                 and other params from 1.4.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual           = GT_FALSE;
    GT_U32      numEntries        = 0;
    GT_U32      iTemp             = 0;
    GT_U32      numOfRouteEntries = 0;
    GT_U32      i                 = 0;

    CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC   routeEntry [10];
    CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_STC   routeEntryGet [10];


    cpssOsBzero((GT_VOID*) &routeEntry, sizeof(routeEntry));
    cpssOsBzero((GT_VOID*) &routeEntryGet, sizeof(routeEntryGet));

    /* Fill the entry for UcRoute table */
    numOfRouteEntries = 10;

    for(iTemp=0; iTemp<numOfRouteEntries; ++iTemp)
    {
        routeEntry[iTemp].type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;
        routeEntry[iTemp].entry.regularEntry.command = CPSS_PACKET_CMD_ROUTE_E;
        routeEntry[iTemp].entry.regularEntry.bypassTtlExceptionCheckEnable = GT_TRUE;
        routeEntry[iTemp].entry.regularEntry.cpuCodeIndex = 0;
        routeEntry[iTemp].entry.regularEntry.ageRefresh = GT_TRUE;
        routeEntry[iTemp].entry.regularEntry.ipv6ScopeCheckEnable = GT_FALSE;
        routeEntry[iTemp].entry.regularEntry.ipv6DestinationSiteId = CPSS_IP_SITE_ID_INTERNAL_E;
        routeEntry[iTemp].entry.regularEntry.sipAccessLevel = 0;
        routeEntry[iTemp].entry.regularEntry.dipAccessLevel = 0;
        routeEntry[iTemp].entry.regularEntry.ucRpfOrIcmpRedirectIfMode = CPSS_EXMXPM_RPF_IF_MODE_DISABLED_E;
        routeEntry[iTemp].entry.regularEntry.sipSaCheckEnable = GT_TRUE;
        routeEntry[iTemp].entry.regularEntry.trapMirrorArpBcToCpu = GT_TRUE;
        routeEntry[iTemp].entry.regularEntry.ucPacketSipFilterEnable = GT_TRUE;

        routeEntry[iTemp].entry.regularEntry.nextHopIf.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        routeEntry[iTemp].entry.regularEntry.nextHopIf.interfaceInfo.type = CPSS_INTERFACE_TRUNK_E;
        routeEntry[iTemp].entry.regularEntry.nextHopIf.interfaceInfo.trunkId = 100;
        routeEntry[iTemp].entry.regularEntry.nextHopIf.outlifPointer.arpPtr = 100;

        routeEntry[iTemp].entry.regularEntry.nextHopVlanId = (GT_U16)(iTemp % 4095);  /* unique */
        routeEntry[iTemp].entry.regularEntry.mirrorToIngressAnalyzer = GT_TRUE;
        routeEntry[iTemp].entry.regularEntry.cntrSetIndex = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E;
        routeEntry[iTemp].entry.regularEntry.mtuProfileIndex = iTemp % 15; /* unique */
        routeEntry[iTemp].entry.regularEntry.icmpRedirectEnable = GT_TRUE;
        routeEntry[iTemp].entry.regularEntry.appSpecificCpuCodeEnable = GT_FALSE;
        routeEntry[iTemp].entry.regularEntry.hopLimitMode = CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
        routeEntry[iTemp].entry.regularEntry.ttl = iTemp % 7; /* unique */
        routeEntry[iTemp].entry.regularEntry.ttlMode = CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E;
        routeEntry[iTemp].entry.regularEntry.mplsLabel = 0;
        routeEntry[iTemp].entry.regularEntry.mplsCommand = CPSS_EXMXPM_MPLS_PUSH_CMD_E;

        routeEntry[iTemp].entry.regularEntry.qosParamsModify.modifyTc = GT_FALSE;
        routeEntry[iTemp].entry.regularEntry.qosParamsModify.modifyUp = GT_FALSE;
        routeEntry[iTemp].entry.regularEntry.qosParamsModify.modifyDp = GT_FALSE;
        routeEntry[iTemp].entry.regularEntry.qosParamsModify.modifyDscp = GT_FALSE;
        routeEntry[iTemp].entry.regularEntry.qosParamsModify.modifyExp = GT_FALSE;

        routeEntry[iTemp].entry.regularEntry.qosParams.tc = 0;
        routeEntry[iTemp].entry.regularEntry.qosParams.dp = CPSS_DP_GREEN_E;
        routeEntry[iTemp].entry.regularEntry.qosParams.up = 0;
        routeEntry[iTemp].entry.regularEntry.qosParams.dscp = 0;
        routeEntry[iTemp].entry.regularEntry.qosParams.exp = 0;
    }

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum - 1;

        /* 1.2. Fill all table in UcRoute by 10 entry */
        iTemp = 0;
        while( (iTemp + numOfRouteEntries) >= numEntries )
        {
            st = cpssExMxPmIpUcRouteEntriesWrite(dev, iTemp, numOfRouteEntries, routeEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpUcRouteEntriesWrite: %d, %d, %d", dev, iTemp, numOfRouteEntries);

            iTemp += numOfRouteEntries;
        }

        /* 1.3. Try to write entry with (index+numOfRouteEntries) out of range. */
        st = cpssExMxPmIpUcRouteEntriesWrite(dev, numEntries, numOfRouteEntries, routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpUcRouteEntriesWrite: %d, %d, %d", dev, iTemp, numOfRouteEntries);

        /* 1.4. Read all table in UcRoute and compare with original by 10 entry */
        iTemp = 0;
        while( (iTemp + numOfRouteEntries) >= numEntries )
        {
            for (i = 0 ; i < numOfRouteEntries ; i++)
                routeEntryGet[i].type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

            st = cpssExMxPmIpUcRouteEntriesRead(dev, iTemp, numOfRouteEntries, routeEntryGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpUcRouteEntriesRead: %d, %d, %d", dev, iTemp, numOfRouteEntries);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) routeEntry, (GT_VOID*) routeEntryGet, (sizeof(routeEntry)*10) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_TRUE, isEqual, "got another routeEntry than was set: %d, %d, %d", dev, iTemp, numOfRouteEntries);
        }

        /* 1.5. Try to read entry with index out of range. */
        for (i = 0 ; i < numOfRouteEntries ; i++)
            routeEntryGet[i].type = CPSS_EXMXPM_IP_UC_ROUTE_ENTRY_E;

        st = cpssExMxPmIpUcRouteEntriesRead(dev, numEntries, numOfRouteEntries, routeEntryGet);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpUcRouteEntriesRead: %d, %d, %d", dev, iTemp, numOfRouteEntries);
    }
}


/*----------------------------------------------------------------------------*/
/*
    Test function to Fill McRoute table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpFillMcRouteTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in McRoute table.
         Call cpssExMxPmIpMcRouteEntriesWrite with ucMcRouteEntryBaseIndex [0..numEntries - 1],
                                                       numOfRouteEntries [1]
                                                       and routeEntriesArrayPtr {command [CPSS_PACKET_CMD_ROUTE_E],
                                                                                 bypassTtlExceptionCheckEnable [GT_TRUE],
                                                                                 cpuCodeIndex [0],
                                                                                 ageRefresh [GT_TRUE],
                                                                                 ipv6ScopeCheckEnable [GT_FALSE],
                                                                                 ipv6ScopeLevel [0],
                                                                                 ipv6DestinationSiteId [CPSS_IP_SITE_ID_INTERNAL_E],
                                                                                 mcRpfIfMode [CPSS_EXMXPM_RPF_IF_MODE_PORT_E],
                                                                                 mcRpfIf {type [CPSS_INTERFACE_TRUNK_E],
                                                                                                     trunkId [2]},
                                                                                 mcRpfFailCommand [CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E],
                                                                                 nextHopIf {type [CPSS_INTERFACE_TRUNK_E],
                                                                                             trunkId [2]},
                                                                                 mirrorToIngressAnalyzer [GT_TRUE],
                                                                                 cntrSetIndex  [CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E],
                                                                                 mtuProfileIndex [0],
                                                                                 appSpecificCpuCodeEnable [GT_TRUE],
                                                                                 hopLimitMode [CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E],
                                                                                 qosParamsModify {modifyTc [GT_TRUE],
                                                                                                  modifyUp [GT_TRUE],
                                                                                                  modifyDp [GT_TRUE],
                                                                                                  modifyDscp [GT_TRUE],
                                                                                                  modifyExp [GT_TRUE]},
                                                                                 qosParams {tc [0],
                                                                                            dp [CPSS_DP_GREEN_E],
                                                                                            up [0],
                                                                                            dscp [0],
                                                                                            exp [0]},
                                                                                 ttl [0],
                                                                                 ttlMode [CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E],
                                                                                 mplsLabel [0],
                                                                                 mplsCommand [CPSS_EXMXPM_MPLS_PUSH_CMD_E] }
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmIpMcRouteEntriesWrite with ucMcRouteEntryBaseIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in McRoute table and compare with original.
         Call cpssExMxPmIpMcRouteEntriesRead with the same ucMcRouteEntryBaseIndex,
                                                          numOfRouteEntries
                                                          and non-NULL routeEntriesArrayPtr.
    Expected: GT_OK and the same routeEntriesArrayPtr.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmIpMcRouteEntriesRead with ucMcRouteEntryBaseIndex [numEntries],
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries        = 0;
    GT_U32      iTemp             = 0;
    GT_U32      numOfRouteEntries = 0;

    CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC   routeEntry;
    CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC   routeEntriesArrayGet;


    cpssOsBzero((GT_VOID*) &routeEntry, sizeof(routeEntry));
    cpssOsBzero((GT_VOID*) &routeEntriesArrayGet, sizeof(routeEntriesArrayGet));

    /* Fill the entry for McRoute table */
    numOfRouteEntries       = 1;
    routeEntry.command                   = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
    routeEntry.cpuCodeIndex              = 0;
    routeEntry.appSpecificCpuCodeEnable  = GT_TRUE;
    routeEntry.mirrorToIngressAnalyzer   = GT_TRUE;
    routeEntry.bypassTtlExceptionCheckEnable = GT_TRUE;
    routeEntry.hopLimitMode               = CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
    routeEntry.qosParamsModify.modifyTc   = GT_TRUE;
    routeEntry.qosParamsModify.modifyUp   = GT_TRUE;
    routeEntry.qosParamsModify.modifyDp   = GT_TRUE;
    routeEntry.qosParamsModify.modifyDscp = GT_TRUE;
    routeEntry.qosParamsModify.modifyExp  = GT_TRUE;

    routeEntry.qosParams.tc   = 0;
    routeEntry.qosParams.dp   = CPSS_DP_GREEN_E;
    routeEntry.qosParams.up   = 0;
    routeEntry.qosParams.dscp = 0;
    routeEntry.qosParams.exp  = 0;

    routeEntry.cntrSetIndex          = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E;
    routeEntry.mcRpfIfMode           = CPSS_EXMXPM_RPF_IF_MODE_PORT_E;
    routeEntry.mcRpfIf.type          = CPSS_INTERFACE_TRUNK_E;
    routeEntry.mcRpfIf.trunkId       = 100;
    routeEntry.mcRpfFailCommand      = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E;
    routeEntry.ipv6ScopeLevel        = 0;
    routeEntry.ipv6DestinationSiteId = CPSS_IP_SITE_ID_INTERNAL_E;
    routeEntry.ipv6ScopeCheckEnable  = GT_FALSE;
    routeEntry.mtuProfileIndex       = 0;
    routeEntry.ageRefresh            = GT_TRUE;

    routeEntry.nextHopIf.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
    routeEntry.nextHopIf.interfaceInfo.trunkId = 2;
    routeEntry.nextHopIf.outlifType            = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    routeEntry.nextHopIf.outlifPointer.arpPtr  = 100;

    routeEntry.ttl = 0,
    routeEntry.ttlMode = CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E,
    routeEntry.mplsLabel = 0,
    routeEntry.mplsCommand = CPSS_EXMXPM_MPLS_PUSH_CMD_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum;

        /* 1.2. Fill all entries in McRoute table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            routeEntry.mcRpfIf.trunkId = (GT_U16)(iTemp % 255);
            routeEntry.mtuProfileIndex = iTemp % 15;

            st = cpssExMxPmIpMcRouteEntriesWrite(dev, iTemp, numOfRouteEntries, &routeEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpMcRouteEntriesWrite: %d, %d, %d", dev, iTemp, numOfRouteEntries);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmIpMcRouteEntriesWrite(dev, numEntries, numOfRouteEntries, &routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpMcRouteEntriesWrite: %d, %d, %d", dev, iTemp, numOfRouteEntries);

        /* 1.4. Read all entries in McRoute table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            routeEntry.mcRpfIf.trunkId = (GT_U16)(iTemp % 255);
            routeEntry.mtuProfileIndex = iTemp % 15;

            st = cpssExMxPmIpMcRouteEntriesRead(dev, iTemp, numOfRouteEntries, &routeEntriesArrayGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpMcRouteEntriesRead: %d, %d, %d", dev, iTemp, numOfRouteEntries);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.command, routeEntriesArrayGet.command,
                       "get another routeEntriesArrayPtr->command than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.cpuCodeIndex, routeEntriesArrayGet.cpuCodeIndex,
                       "get another routeEntriesArrayPtr->cpuCodeIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.appSpecificCpuCodeEnable, routeEntriesArrayGet.appSpecificCpuCodeEnable,
                       "get another routeEntriesArrayPtr->appSpecificCpuCodeEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.appSpecificCpuCodeEnable, routeEntriesArrayGet.appSpecificCpuCodeEnable,
                       "get another routeEntriesArrayPtr->appSpecificCpuCodeEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mirrorToIngressAnalyzer, routeEntriesArrayGet.mirrorToIngressAnalyzer,
                       "get another routeEntriesArrayPtr->mirrorToIngressAnalyzer than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.bypassTtlExceptionCheckEnable, routeEntriesArrayGet.bypassTtlExceptionCheckEnable,
                       "get another routeEntriesArrayPtr->bypassTtlExceptionCheckEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParamsModify.modifyTc, routeEntriesArrayGet.qosParamsModify.modifyTc,
                       "get another routeEntriesArrayPtr->qosParamsModify.modifyTc than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParamsModify.modifyUp, routeEntriesArrayGet.qosParamsModify.modifyUp,
                       "get another routeEntriesArrayPtr->qosParamsModify.modifyUp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParamsModify.modifyDp, routeEntriesArrayGet.qosParamsModify.modifyDp,
                       "get another routeEntriesArrayPtr->qosParamsModify.modifyDp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParamsModify.modifyDscp, routeEntriesArrayGet.qosParamsModify.modifyDscp,
                       "get another routeEntriesArrayPtr->qosParamsModify.modifyDscp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParams.tc, routeEntriesArrayGet.qosParams.tc,
                       "get another routeEntriesArrayPtr->qosParams.tc than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParams.dp, routeEntriesArrayGet.qosParams.dp,
                       "get another routeEntriesArrayPtr->qosParams.dp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParams.up, routeEntriesArrayGet.qosParams.up,
                       "get another routeEntriesArrayPtr->qosParams.up than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.qosParams.dscp, routeEntriesArrayGet.qosParams.dscp,
                       "get another routeEntriesArrayPtr->qosParams.dscp than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.cntrSetIndex, routeEntriesArrayGet.cntrSetIndex,
                       "get another routeEntriesArrayPtr->cntrSetIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mcRpfIfMode, routeEntriesArrayGet.mcRpfIfMode,
                       "get another routeEntriesArrayPtr->mcRpfIfMode than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mcRpfIf.type, routeEntriesArrayGet.mcRpfIf.type,
                       "get another routeEntriesArrayPtr->mcRpfIf.type than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mcRpfIf.trunkId, routeEntriesArrayGet.mcRpfIf.trunkId,
                       "get another routeEntriesArrayPtr->mcRpfIf.trunkId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mcRpfFailCommand, routeEntriesArrayGet.mcRpfFailCommand,
                       "get another routeEntriesArrayPtr->mcRpfFailCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.ipv6ScopeLevel, routeEntriesArrayGet.ipv6ScopeLevel,
                       "get another routeEntriesArrayPtr->ipv6ScopeLevel than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.ipv6DestinationSiteId, routeEntriesArrayGet.ipv6DestinationSiteId,
                       "get another routeEntriesArrayPtr->ipv6DestinationSiteId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mtuProfileIndex, routeEntriesArrayGet.mtuProfileIndex,
                       "get another routeEntriesArrayPtr->mtuProfileIndex than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.ageRefresh, routeEntriesArrayGet.ageRefresh,
                       "get another routeEntriesArrayPtr->ageRefresh than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.nextHopIf.interfaceInfo.type, routeEntriesArrayGet.nextHopIf.interfaceInfo.type,
                       "get another routeEntriesArrayPtr->nextHopIf.interfaceInfo.type than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.ttl, routeEntriesArrayGet.ttl,
                       "get another routeEntriesArrayPtr->ttl than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.ttlMode, routeEntriesArrayGet.ttlMode,
                       "get another routeEntriesArrayPtr->ttlMode than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mplsLabel, routeEntriesArrayGet.mplsLabel,
                       "get another routeEntriesArrayPtr->mplsLabel than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(routeEntry.mplsCommand, routeEntriesArrayGet.mplsCommand,
                       "get another routeEntriesArrayPtr->mplsCommand than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmIpMcRouteEntriesRead(dev, numEntries, numOfRouteEntries, &routeEntriesArrayGet);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpMcRouteEntriesRead: %d, %d, %d", dev, iTemp, numOfRouteEntries);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill McRoute table (group write - with numEntries > 1).
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpFillMcRouteTable_GroupWrite)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in McRoute table.
         Call cpssExMxPmIpMcRouteEntriesWrite with ucMcRouteEntryBaseIndex [0..numEntries - 1],
                                                       numOfRouteEntries [10]
                                                       and routeEntriesArrayPtr {command [CPSS_PACKET_CMD_ROUTE_E],
                                                                                 bypassTtlExceptionCheckEnable [GT_TRUE],
                                                                                 cpuCodeIndex [0],
                                                                                 ageRefresh [GT_TRUE],
                                                                                 ipv6ScopeCheckEnable [GT_FALSE],
                                                                                 ipv6ScopeLevel [0],
                                                                                 ipv6DestinationSiteId [CPSS_IP_SITE_ID_INTERNAL_E],
                                                                                 mcRpfIfMode [CPSS_EXMXPM_RPF_IF_MODE_PORT_E],
                                                                                 mcRpfIf {type [CPSS_INTERFACE_TRUNK_E],
                                                                                                     trunkId [2]},
                                                                                 mcRpfFailCommand [CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E],
                                                                                 nextHopIf {type [CPSS_INTERFACE_TRUNK_E],
                                                                                             trunkId [2]},
                                                                                 mirrorToIngressAnalyzer [GT_TRUE],
                                                                                 cntrSetIndex  [CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E],
                                                                                 mtuProfileIndex [0],
                                                                                 appSpecificCpuCodeEnable [GT_TRUE],
                                                                                 hopLimitMode [CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E],
                                                                                 qosParamsModify {modifyTc [GT_TRUE],
                                                                                                  modifyUp [GT_TRUE],
                                                                                                  modifyDp [GT_TRUE],
                                                                                                  modifyDscp [GT_TRUE],
                                                                                                  modifyExp [GT_TRUE]},
                                                                                 qosParams {tc [0],
                                                                                            dp [CPSS_DP_GREEN_E],
                                                                                            up [0],
                                                                                            dscp [0],
                                                                                            exp [0]},
                                                                                 ttl [0],
                                                                                 ttlMode [CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E],
                                                                                 mplsLabel [0],
                                                                                 mplsCommand [CPSS_EXMXPM_MPLS_PUSH_CMD_E] }
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmIpMcRouteEntriesWrite with (ucMcRouteEntryBaseIndex + numEntries) > numEntries and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in McRoute table and compare with original.
         Call cpssExMxPmIpMcRouteEntriesRead with the same ucMcRouteEntryBaseIndex,
                                                          numOfRouteEntries
                                                          and non-NULL routeEntriesArrayPtr.
    Expected: GT_OK and the same routeEntriesArrayPtr.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmIpMcRouteEntriesRead with (ucMcRouteEntryBaseIndex + numEntries) > numEntries,
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries        = 0;
    GT_U32      iTemp             = 0;
    GT_U32      jTemp             = 0;
    GT_U32      numOfRouteEntries = 0;

    CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC   routeEntry[10];
    CPSS_EXMXPM_IP_MC_ROUTE_ENTRY_STC   routeEntriesArrayGet[10];


    cpssOsBzero((GT_VOID*) &routeEntry, sizeof(routeEntry));
    cpssOsBzero((GT_VOID*) &routeEntriesArrayGet, sizeof(routeEntriesArrayGet));

    /* Fill the entry for McRoute table */
    numOfRouteEntries = 10;

    for(iTemp=0; iTemp<numOfRouteEntries; ++iTemp)
    {
        routeEntry[iTemp].command                   = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
        routeEntry[iTemp].cpuCodeIndex              = 0;
        routeEntry[iTemp].appSpecificCpuCodeEnable  = GT_TRUE;
        routeEntry[iTemp].mirrorToIngressAnalyzer   = GT_TRUE;
        routeEntry[iTemp].bypassTtlExceptionCheckEnable = GT_TRUE;
        routeEntry[iTemp].hopLimitMode               = CPSS_EXMXPM_IP_HOP_LIMIT_NOT_DECREMENTED_E;
        routeEntry[iTemp].qosParamsModify.modifyTc   = GT_TRUE;
        routeEntry[iTemp].qosParamsModify.modifyUp   = GT_TRUE;
        routeEntry[iTemp].qosParamsModify.modifyDp   = GT_TRUE;
        routeEntry[iTemp].qosParamsModify.modifyDscp = GT_TRUE;
        routeEntry[iTemp].qosParamsModify.modifyExp  = GT_TRUE;

        routeEntry[iTemp].qosParams.tc   = 0;
        routeEntry[iTemp].qosParams.dp   = CPSS_DP_GREEN_E;
        routeEntry[iTemp].qosParams.up   = 0;
        routeEntry[iTemp].qosParams.dscp = 0;
        routeEntry[iTemp].qosParams.exp  = 0;

        routeEntry[iTemp].cntrSetIndex          = CPSS_EXMXPM_IP_COUNTER_SET_INDEX_0_E;
        routeEntry[iTemp].mcRpfIfMode           = CPSS_EXMXPM_RPF_IF_MODE_PORT_E;
        routeEntry[iTemp].mcRpfIf.type          = CPSS_INTERFACE_TRUNK_E;
        routeEntry[iTemp].mcRpfIf.trunkId       = (GT_U16)(iTemp % 255);
        routeEntry[iTemp].mcRpfFailCommand      = CPSS_EXMXPM_MC_RPF_FAIL_COMMAND_DROP_HARD_E;
        routeEntry[iTemp].ipv6ScopeLevel        = 0;
        routeEntry[iTemp].ipv6DestinationSiteId = CPSS_IP_SITE_ID_INTERNAL_E;
        routeEntry[iTemp].ipv6ScopeCheckEnable  = GT_FALSE;
        routeEntry[iTemp].mtuProfileIndex       = iTemp % 15;
        routeEntry[iTemp].ageRefresh            = GT_TRUE;

        routeEntry[iTemp].nextHopIf.interfaceInfo.type    = CPSS_INTERFACE_TRUNK_E;
        routeEntry[iTemp].nextHopIf.interfaceInfo.trunkId = 2;
        routeEntry[iTemp].nextHopIf.outlifType            = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
        routeEntry[iTemp].nextHopIf.outlifPointer.arpPtr  = 100;

        routeEntry[iTemp].ttl = 0,
        routeEntry[iTemp].ttlMode = CPSS_EXMXPM_IP_TTL_NOT_DECREMENTED_E,
        routeEntry[iTemp].mplsLabel = 0,
        routeEntry[iTemp].mplsCommand = CPSS_EXMXPM_MPLS_PUSH_CMD_E;
    }

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = PRV_CPSS_EXMXPM_IP_MODULE_MAC(dev)->routeEntriesNum - 1;

        /* 1.2. Fill all table in McRoute by 10 entry */
        iTemp = 0;
        while( (iTemp + numOfRouteEntries) >= numEntries )
        {
            st = cpssExMxPmIpMcRouteEntriesWrite(dev, iTemp, numOfRouteEntries, routeEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpMcRouteEntriesWrite: %d, %d, %d", dev, iTemp, numOfRouteEntries);

            iTemp += numOfRouteEntries;
        }

        /* 1.3. Try to write entry with (index + numOfRouteEntries > numEntries) out of range. */
        st = cpssExMxPmIpMcRouteEntriesWrite(dev, numEntries, numOfRouteEntries, routeEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpMcRouteEntriesWrite: %d, %d, %d", dev, iTemp, numOfRouteEntries);

        /* 1.4. Read all entries in McRoute table and compare with original */
        iTemp = 0;
        while( (iTemp + numOfRouteEntries) >= numEntries )
        {
            st = cpssExMxPmIpMcRouteEntriesRead(dev, iTemp, numOfRouteEntries, routeEntriesArrayGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpMcRouteEntriesRead: %d, %d, %d", dev, iTemp, numOfRouteEntries);

            /* Verifying values */
            for(jTemp=0; jTemp<numOfRouteEntries; ++jTemp)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].command, routeEntriesArrayGet[jTemp].command,
                           "get another routeEntriesArrayPtr[%d]->command than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].cpuCodeIndex, routeEntriesArrayGet[jTemp].cpuCodeIndex,
                           "get another routeEntriesArrayPtr[%d]->cpuCodeIndex than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].appSpecificCpuCodeEnable, routeEntriesArrayGet[jTemp].appSpecificCpuCodeEnable,
                           "get another routeEntriesArrayPtr[%d]->appSpecificCpuCodeEnable than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].appSpecificCpuCodeEnable, routeEntriesArrayGet[jTemp].appSpecificCpuCodeEnable,
                           "get another routeEntriesArrayPtr[%d]->appSpecificCpuCodeEnable than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].mirrorToIngressAnalyzer, routeEntriesArrayGet[jTemp].mirrorToIngressAnalyzer,
                           "get another routeEntriesArrayPtr[%d]->mirrorToIngressAnalyzer than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].bypassTtlExceptionCheckEnable, routeEntriesArrayGet[jTemp].bypassTtlExceptionCheckEnable,
                           "get another routeEntriesArrayPtr[%d]->bypassTtlExceptionCheckEnable than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].qosParamsModify.modifyTc, routeEntriesArrayGet[jTemp].qosParamsModify.modifyTc,
                           "get another routeEntriesArrayPtr[%d]->qosParamsModify.modifyTc than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].qosParamsModify.modifyUp, routeEntriesArrayGet[jTemp].qosParamsModify.modifyUp,
                           "get another routeEntriesArrayPtr[%d]->qosParamsModify.modifyUp than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].qosParamsModify.modifyDp, routeEntriesArrayGet[jTemp].qosParamsModify.modifyDp,
                           "get another routeEntriesArrayPtr[%d]->qosParamsModify.modifyDp than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].qosParamsModify.modifyDscp, routeEntriesArrayGet[jTemp].qosParamsModify.modifyDscp,
                           "get another routeEntriesArrayPtr[%d]->qosParamsModify.modifyDscp than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].qosParams.tc, routeEntriesArrayGet[jTemp].qosParams.tc,
                           "get another routeEntriesArrayPtr[%d]->qosParams.tc than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].qosParams.dp, routeEntriesArrayGet[jTemp].qosParams.dp,
                           "get another routeEntriesArrayPtr[%d]->qosParams.dp than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].qosParams.up, routeEntriesArrayGet[jTemp].qosParams.up,
                           "get another routeEntriesArrayPtr[%d]->qosParams.up than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].qosParams.dscp, routeEntriesArrayGet[jTemp].qosParams.dscp,
                           "get another routeEntriesArrayPtr[%d]->qosParams.dscp than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].cntrSetIndex, routeEntriesArrayGet[jTemp].cntrSetIndex,
                           "get another routeEntriesArrayPtr[%d]->cntrSetIndex than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].mcRpfIfMode, routeEntriesArrayGet[jTemp].mcRpfIfMode,
                           "get another routeEntriesArrayPtr[%d]->mcRpfIfMode than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].mcRpfIf.type, routeEntriesArrayGet[jTemp].mcRpfIf.type,
                           "get another routeEntriesArrayPtr[%d]->mcRpfIf.type than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].mcRpfIf.trunkId, routeEntriesArrayGet[jTemp].mcRpfIf.trunkId,
                           "get another routeEntriesArrayPtr[%d]->mcRpfIf.trunkId than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].mcRpfFailCommand, routeEntriesArrayGet[jTemp].mcRpfFailCommand,
                           "get another routeEntriesArrayPtr[%d]->mcRpfFailCommand than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].ipv6ScopeLevel, routeEntriesArrayGet[jTemp].ipv6ScopeLevel,
                           "get another routeEntriesArrayPtr[%d][jTemp]->ipv6ScopeLevel than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].ipv6DestinationSiteId, routeEntriesArrayGet[jTemp].ipv6DestinationSiteId,
                           "get another routeEntriesArrayPtr[%d]->ipv6DestinationSiteId than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].mtuProfileIndex, routeEntriesArrayGet[jTemp].mtuProfileIndex,
                           "get another routeEntriesArrayPtr[%d]->mtuProfileIndex than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].ageRefresh, routeEntriesArrayGet[jTemp].ageRefresh,
                           "get another routeEntriesArrayPtr[%d]->ageRefresh than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].nextHopIf.interfaceInfo.type, routeEntriesArrayGet[jTemp].nextHopIf.interfaceInfo.type,
                           "get another routeEntriesArrayPtr[%d]->nextHopIf.interfaceInfo.type than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].ttl, routeEntriesArrayGet[jTemp].ttl,
                           "get another routeEntriesArrayPtr[%d]->ttl than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].ttlMode, routeEntriesArrayGet[jTemp].ttlMode,
                           "get another routeEntriesArrayPtr[%d]->ttlMode than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].mplsLabel, routeEntriesArrayGet[jTemp].mplsLabel,
                           "get another routeEntriesArrayPtr[%d]->mplsLabel than was set: %d", jTemp, dev);
                UTF_VERIFY_EQUAL2_STRING_MAC(routeEntry[jTemp].mplsCommand, routeEntriesArrayGet[jTemp].mplsCommand,
                           "get another routeEntriesArrayPtr[%d]->mplsCommand than was set: %d", jTemp, dev);
            }
        }

        /* 1.5. Try to read entry with (index+numOfRouteEntries > numEntries) out of range. */
        st = cpssExMxPmIpMcRouteEntriesRead(dev, numEntries, numOfRouteEntries, routeEntriesArrayGet);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpMcRouteEntriesRead: %d, %d, %d", dev, iTemp, numOfRouteEntries);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill ArpAddr table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpFillArpAddrTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in ArpAddr table.
         Call cpssExMxPmIpRouterArpAddrWrite with routerArpIndex [0..numEntries - 1]
                                               and arpMacAddrPtr->arEther [0x0, 0x0, 0x0, 0x0, 0x0, 0x0].
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmIpRouterArpAddrWrite with routerArpIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in ArpAddr table and compare with original.
         Call cpssExMxPmIpRouterArpAddrRead with the same routerArpIndex
                                                       and non-NULL arpMacAddrPtr.
    Expected: GT_OK and the same arpMacAddrPtr.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmIpRouterArpAddrRead with routerArpIndex [numEntries] and non-NULL arpMacAddrPtr.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    GT_ETHERADDR    arpMacAddr;
    GT_ETHERADDR    arpMacAddrGet;


    cpssOsBzero((GT_VOID*) &arpMacAddr, sizeof(arpMacAddr));
    cpssOsBzero((GT_VOID*) &arpMacAddrGet, sizeof(arpMacAddrGet));

    /* Fill the entry for ArpAddr table */
    arpMacAddr.arEther[0] = 0;
    arpMacAddr.arEther[1] = 0;
    arpMacAddr.arEther[2] = 0;
    arpMacAddr.arEther[3] = 0;
    arpMacAddr.arEther[4] = 0;
    arpMacAddr.arEther[5] = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = prvGetMaxARPTableSize(dev);

        /* 1.2. Fill all entries in ArpAddr table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            arpMacAddr.arEther[0] = (GT_U8)(iTemp % 255);
            arpMacAddr.arEther[1] = (GT_U8)(iTemp % 255);
            arpMacAddr.arEther[2] = (GT_U8)(iTemp % 255);

            st = cpssExMxPmIpRouterArpAddrWrite(dev, iTemp, &arpMacAddr);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpRouterArpAddrWrite: %d, %d", dev, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmIpRouterArpAddrWrite(dev, numEntries, &arpMacAddr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpRouterArpAddrWrite: %d, %d", dev, iTemp);

        /* 1.4. Read all entries in ArpAddr table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            arpMacAddr.arEther[0] = (GT_U8)(iTemp % 255);
            arpMacAddr.arEther[1] = (GT_U8)(iTemp % 255);
            arpMacAddr.arEther[2] = (GT_U8)(iTemp % 255);

            st = cpssExMxPmIpRouterArpAddrRead(dev, iTemp, &arpMacAddrGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmIpRouterArpAddrRead: %d, %d", dev, iTemp);

            /* Verifying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &arpMacAddr,
                                         (GT_VOID*) &arpMacAddrGet,
                                         sizeof(arpMacAddr))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another arpMacAddr than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmIpRouterArpAddrRead(dev, numEntries, &arpMacAddrGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpRouterArpAddrRead: %d, %d", dev, iTemp);
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmIpTables suit
 */

UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmIpTables)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpUcRouteEntriesWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpUcRouteEntriesRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpMcRouteEntriesWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpMcRouteEntriesRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpRouteEntryAgeRefreshSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpRouteEntryAgeRefreshGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpRouterArpAddrWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpRouterArpAddrRead)
    /* Tests for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpFillUcRouteTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpFillUcRouteTable_GroupWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpFillMcRouteTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpFillMcRouteTable_GroupWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpFillArpAddrTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmIpTables)



/*----------------------------------------------------------------------------*/
/*
    Get Max Size of ARP / Tunnel start Table
*/
static GT_U32 prvGetMaxARPTableSize
(
    IN GT_U8 dev
)
{
    switch (PRV_CPSS_EXMXPM_PP_MAC(dev)->moduleCfg.bridgeCfg.fdbMemoryMode)
    {
        case CPSS_EXMXPM_PP_CONFIG_FDB_AND_ARP_TABLE_MODE_INTERNAL_SPLIT_E:
                return 16384;
            break;
        case CPSS_EXMXPM_PP_CONFIG_FDB_AND_ARP_TABLE_MODE_INTERNAL_SHARED_E:
        case CPSS_EXMXPM_PP_CONFIG_FDB_TABLE_MODE_EXTERNAL_E:
                return 65536;
            break;
        default:
             return 0;
    }
}

