/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfBridgeGen.c
*
* DESCRIPTION:
*       Generic API implementation for Bridge
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
#include <common/tgfBridgeGen.h>
#include <appDemo/utils/appDemoFdbUpdateLock.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

#ifdef CHX_FAMILY
    #include <bridge/prvTgfBasicDynamicLearning.h>
#endif /*CHX_FAMILY*/

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
    #include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
    #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#endif /*CHX_FAMILY*/

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/config/cpssExMxPmCfg.h>
    #include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgGen.h>
    #include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdb.h>
#endif /*EXMXPM_FAMILY*/

/******************************************************************************\
 *                           Private declarations                             *
\******************************************************************************/

/* did we initialized the local variables */
static GT_BOOL isInitialized = GT_FALSE;

/* default mac entry */
static PRV_TGF_BRG_MAC_ENTRY_STC   prvTgfDefMacEntry;

/* default vlan entry */
static PRV_TGF_BRG_VLAN_INFO_STC   prvTgfDefVlanInfo;

static PRV_TGF_BRG_FDB_ACCESS_MODE_ENT prvTgfBrgFdbAccessMode = PRV_TGF_BRG_FDB_ACCESS_MODE_BY_MESSAGE_E;

static GT_U32 prvTgfBrgFdbHashLengthLookup = 4;/* 4 index in bucket -- assume that 4 is the length of lookup */

/* FDB entry 'by message' must succeed */
static GT_BOOL  fdbEntryByMessageMustSucceed = GT_TRUE;

/******************************************************************************\
 *                     Private function's implementation                      *
\******************************************************************************/
static GT_STATUS prvTgfConvertCpssToGenericMacEntry
(
    IN  CPSS_MAC_ENTRY_EXT_STC       *cpssMacEntryPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
);

/* no initialized parameter value */
#define NOT_INIT_CNS    0xFFFFFFFF
/*******************************************************************************
* prvTgfBrgInit
*
* DESCRIPTION:
*       Initialize local default settings
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfBrgInit
(
    GT_VOID
)
{
    if(GT_TRUE == isInitialized)
    {
        return;
    }

    isInitialized = GT_TRUE;

    /* clear entry */
    cpssOsMemSet(&prvTgfDefMacEntry, 0, sizeof(prvTgfDefMacEntry));

    /* set default mac entry */
    prvTgfDefMacEntry.key.entryType            = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
    prvTgfDefMacEntry.key.key.macVlan.vlanId   = 0;
    prvTgfDefMacEntry.dstInterface.type        = CPSS_INTERFACE_VIDX_E;
    prvTgfDefMacEntry.dstInterface.vidx        = 0;
    prvTgfDefMacEntry.isStatic                 = GT_FALSE;
    prvTgfDefMacEntry.daCommand                = PRV_TGF_PACKET_CMD_FORWARD_E;
    prvTgfDefMacEntry.saCommand                = PRV_TGF_PACKET_CMD_FORWARD_E;
    prvTgfDefMacEntry.daRoute                  = GT_FALSE;
    prvTgfDefMacEntry.mirrorToRxAnalyzerPortEn = GT_FALSE;
    prvTgfDefMacEntry.userDefined              = 0;
    prvTgfDefMacEntry.daQosIndex               = 0;
    prvTgfDefMacEntry.saQosIndex               = 0;
    prvTgfDefMacEntry.daSecurityLevel          = 0;
    prvTgfDefMacEntry.saSecurityLevel          = 0;
    prvTgfDefMacEntry.appSpecificCpuCode       = GT_FALSE;
    prvTgfDefMacEntry.pwId                     = 0;
    prvTgfDefMacEntry.spUnknown                = GT_FALSE;
    prvTgfDefMacEntry.sourceId                 = 0;

    /* clear entry */
    cpssOsMemSet(&prvTgfDefVlanInfo, 0, sizeof(prvTgfDefVlanInfo));

    /* set default Vlan info entry */
    prvTgfDefVlanInfo.cpuMember            = GT_FALSE;
    prvTgfDefVlanInfo.unkSrcAddrSecBreach  = GT_FALSE;
    prvTgfDefVlanInfo.unregNonIpMcastCmd   = CPSS_PACKET_CMD_FORWARD_E;
    prvTgfDefVlanInfo.unregIpv4McastCmd    = CPSS_PACKET_CMD_FORWARD_E;
    prvTgfDefVlanInfo.unregIpv6McastCmd    = CPSS_PACKET_CMD_FORWARD_E;
    prvTgfDefVlanInfo.unkUcastCmd          = CPSS_PACKET_CMD_FORWARD_E;
    prvTgfDefVlanInfo.unregIpv4BcastCmd    = CPSS_PACKET_CMD_FORWARD_E;
    prvTgfDefVlanInfo.unregNonIpv4BcastCmd = CPSS_PACKET_CMD_FORWARD_E;
    prvTgfDefVlanInfo.ipv4IgmpToCpuEn      = GT_FALSE;
    prvTgfDefVlanInfo.mirrToRxAnalyzerEn   = GT_FALSE;
    prvTgfDefVlanInfo.mirrorToTxAnalyzerEn = GT_FALSE;
    prvTgfDefVlanInfo.ipv6IcmpToCpuEn      = GT_FALSE;
    prvTgfDefVlanInfo.ipCtrlToCpuEn        = PRV_TGF_BRG_IP_CTRL_NONE_E;
    prvTgfDefVlanInfo.ipv4IpmBrgMode       = CPSS_BRG_IPM_SGV_E;
    prvTgfDefVlanInfo.ipv6IpmBrgMode       = CPSS_BRG_IPM_SGV_E;
    prvTgfDefVlanInfo.ipv4IpmBrgEn         = GT_FALSE;
    prvTgfDefVlanInfo.ipv6IpmBrgEn         = GT_FALSE;
    prvTgfDefVlanInfo.ipv6SiteIdMode       = CPSS_IP_SITE_ID_INTERNAL_E;
    prvTgfDefVlanInfo.ipv4UcastRouteEn     = GT_FALSE;
    prvTgfDefVlanInfo.ipv4McastRouteEn     = GT_FALSE;
    prvTgfDefVlanInfo.ipv6UcastRouteEn     = GT_FALSE;
    prvTgfDefVlanInfo.ipv6McastRouteEn     = GT_FALSE;
    prvTgfDefVlanInfo.stgId                = 0;
    prvTgfDefVlanInfo.autoLearnDisable     = GT_TRUE;
    prvTgfDefVlanInfo.naMsgToCpuEn         = GT_TRUE;
    prvTgfDefVlanInfo.mruIdx               = 0;
    prvTgfDefVlanInfo.bcastUdpTrapMirrEn   = GT_FALSE;
    prvTgfDefVlanInfo.vrfId                = 0;
    prvTgfDefVlanInfo.egressUnregMcFilterCmd = CPSS_UNREG_MC_VLAN_FRWD_E;
    prvTgfDefVlanInfo.floodVidx            = 0xFFF;/* flood into vlan */
}

#ifdef CHX_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToDxChMacEntryKey
*
* DESCRIPTION:
*       Convert generic MAC entry key into device specific MAC entry key
*
* INPUTS:
*       macEntryKeyPtr - (pointer to) MAC entry key parameters
*
* OUTPUTS:
*       dxChMacEntryKeyPtr - (pointer to) DxCh MAC entry key parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChMacEntryKey
(
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *macEntryKeyPtr,
    OUT CPSS_MAC_ENTRY_EXT_KEY_STC   *dxChMacEntryKeyPtr
)
{
    cpssOsMemSet(dxChMacEntryKeyPtr, 0, sizeof(*dxChMacEntryKeyPtr));

    /* convert entry type into device specific format */
    switch (macEntryKeyPtr->entryType)
    {
        case PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E:
            dxChMacEntryKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E:
            dxChMacEntryKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E:
            dxChMacEntryKeyPtr->entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert key data into device specific format */
    if (PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E == macEntryKeyPtr->entryType)
    {
        cpssOsMemCpy(dxChMacEntryKeyPtr->key.macVlan.macAddr.arEther,
                     macEntryKeyPtr->key.macVlan.macAddr.arEther,
                     sizeof(dxChMacEntryKeyPtr->key.macVlan.macAddr));
        dxChMacEntryKeyPtr->key.macVlan.vlanId = macEntryKeyPtr->key.macVlan.vlanId;
    }
    else
    {
        cpssOsMemCpy(dxChMacEntryKeyPtr->key.ipMcast.sip,
                     macEntryKeyPtr->key.ipMcast.sip,
                     sizeof(dxChMacEntryKeyPtr->key.ipMcast.sip));
        cpssOsMemCpy(dxChMacEntryKeyPtr->key.ipMcast.dip,
                     macEntryKeyPtr->key.ipMcast.dip,
                     sizeof(dxChMacEntryKeyPtr->key.ipMcast.dip));
        dxChMacEntryKeyPtr->key.ipMcast.vlanId = macEntryKeyPtr->key.ipMcast.vlanId;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToDxChMacEntry
*
* DESCRIPTION:
*       Convert generic MAC entry into device specific MAC entry
*
* INPUTS:
*       macEntryPtr - (pointer to) MAC entry parameters
*
* OUTPUTS:
*       dxChMacEntryPtr - (pointer to) DxCh MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChMacEntry
(
    IN  PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC       *dxChMacEntryPtr
)
{
    GT_STATUS rc; /* return code */

    /* convert entry type into device specific format */
    switch (macEntryPtr->key.entryType)
    {
        case PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E:
            dxChMacEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E:
            dxChMacEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E:
            dxChMacEntryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert key data into device specific format */
    if (PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E == macEntryPtr->key.entryType)
    {
        cpssOsMemCpy(dxChMacEntryPtr->key.key.macVlan.macAddr.arEther,
                     macEntryPtr->key.key.macVlan.macAddr.arEther,
                     sizeof(dxChMacEntryPtr->key.key.macVlan.macAddr));
        dxChMacEntryPtr->key.key.macVlan.vlanId = macEntryPtr->key.key.macVlan.vlanId;
    }
    else
    {
        cpssOsMemCpy(dxChMacEntryPtr->key.key.ipMcast.sip,
                     macEntryPtr->key.key.ipMcast.sip,
                     sizeof(dxChMacEntryPtr->key.key.ipMcast.sip));
        cpssOsMemCpy(dxChMacEntryPtr->key.key.ipMcast.dip,
                     macEntryPtr->key.key.ipMcast.dip,
                     sizeof(dxChMacEntryPtr->key.key.ipMcast.dip));
        dxChMacEntryPtr->key.key.ipMcast.vlanId = macEntryPtr->key.key.ipMcast.vlanId;
    }

    /* convert interface info into device specific format */
    dxChMacEntryPtr->dstInterface.type = macEntryPtr->dstInterface.type;
    switch (dxChMacEntryPtr->dstInterface.type)
    {
        case CPSS_INTERFACE_PORT_E:
            rc = prvUtfHwFromSwDeviceNumberGet(macEntryPtr->dstInterface.devPort.devNum,
                                               &(dxChMacEntryPtr->dstInterface.devPort.devNum));
            if(GT_OK != rc)
                return rc;
            dxChMacEntryPtr->dstInterface.devPort.portNum = macEntryPtr->dstInterface.devPort.portNum;
            break;

        case CPSS_INTERFACE_TRUNK_E:
            dxChMacEntryPtr->dstInterface.trunkId = macEntryPtr->dstInterface.trunkId;
            break;

        case CPSS_INTERFACE_VIDX_E:
            dxChMacEntryPtr->dstInterface.vidx = macEntryPtr->dstInterface.vidx;
            break;

        case CPSS_INTERFACE_VID_E:
            dxChMacEntryPtr->dstInterface.vlanId = macEntryPtr->dstInterface.vlanId;
            break;

        case CPSS_INTERFACE_DEVICE_E:
            dxChMacEntryPtr->dstInterface.devNum = macEntryPtr->dstInterface.devNum;
            break;

        case CPSS_INTERFACE_FABRIC_VIDX_E:
            dxChMacEntryPtr->dstInterface.fabricVidx = macEntryPtr->dstInterface.fabricVidx;
            break;

        default:
            return GT_BAD_PARAM;
    }

    dxChMacEntryPtr->sourceID = macEntryPtr->sourceId;

    /* convert daCommand info into device specific format */
    switch (macEntryPtr->daCommand)
    {
        case PRV_TGF_PACKET_CMD_FORWARD_E:
            dxChMacEntryPtr->daCommand = CPSS_MAC_TABLE_FRWRD_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_HARD_E:
            dxChMacEntryPtr->daCommand = CPSS_MAC_TABLE_DROP_E;
            break;

        case PRV_TGF_PACKET_CMD_INTERV_E:
            dxChMacEntryPtr->daCommand = CPSS_MAC_TABLE_INTERV_E;
            break;

        case PRV_TGF_PACKET_CMD_CNTL_E:
            dxChMacEntryPtr->daCommand = CPSS_MAC_TABLE_CNTL_E;
            break;

        case PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E:
            dxChMacEntryPtr->daCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_SOFT_E:
            dxChMacEntryPtr->daCommand = CPSS_MAC_TABLE_SOFT_DROP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert saCommand info into device specific format */
    switch (macEntryPtr->saCommand)
    {
        case PRV_TGF_PACKET_CMD_FORWARD_E:
            dxChMacEntryPtr->saCommand = CPSS_MAC_TABLE_FRWRD_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_HARD_E:
            dxChMacEntryPtr->saCommand = CPSS_MAC_TABLE_DROP_E;
            break;

        case PRV_TGF_PACKET_CMD_INTERV_E:
            dxChMacEntryPtr->saCommand = CPSS_MAC_TABLE_INTERV_E;
            break;

        case PRV_TGF_PACKET_CMD_CNTL_E:
            dxChMacEntryPtr->saCommand = CPSS_MAC_TABLE_CNTL_E;
            break;

        case PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E:
            dxChMacEntryPtr->saCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_SOFT_E:
            dxChMacEntryPtr->saCommand = CPSS_MAC_TABLE_SOFT_DROP_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    dxChMacEntryPtr->age = GT_TRUE;

    /* convert MAC entry info into device specific format */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, isStatic);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, daRoute);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, mirrorToRxAnalyzerPortEn);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, userDefined);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, daQosIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, saQosIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, daSecurityLevel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, saSecurityLevel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, appSpecificCpuCode);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(dxChMacEntryPtr, macEntryPtr, spUnknown);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericMacEntry
*
* DESCRIPTION:
*       Convert device specific MAC entry into generic MAC entry
*
* INPUTS:
*       dxChMacEntryPtr - (pointer to) DxCh MAC entry parameters
*
* OUTPUTS:
*       macEntryPtr - (pointer to) MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxChToGenericMacEntry
(
    IN  CPSS_MAC_ENTRY_EXT_STC       *dxChMacEntryPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
)
{

    return prvTgfConvertCpssToGenericMacEntry(dxChMacEntryPtr,macEntryPtr);
}

/*******************************************************************************
* prvTgfConvertGenericToDxChDropCntMode
*
* DESCRIPTION:
*       Convert generic drop count mode into device specific drop count mode
*
* INPUTS:
*       dropMode - drop counter mode
*
* OUTPUTS:
*       dxChDropModePtr - (pointer to) DxCh drop counter mode
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToDxChDropCntMode
(
    IN  PRV_TGF_BRG_DROP_CNTR_MODE_ENT       dropMode,
    OUT CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT *dxChDropModePtr
)
{
    /* convert drop counter mode into device specific format */
    switch (dropMode)
    {
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_COUNT_ALL_E,
                                CPSS_DXCH_BRG_DROP_CNTR_COUNT_ALL_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_FDB_ENTRY_CMD_E,
                                CPSS_DXCH_BRG_DROP_CNTR_FDB_ENTRY_CMD_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E,
                                CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_INVALID_SA_E,
                                CPSS_DXCH_BRG_DROP_CNTR_INVALID_SA_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_INVALID_VLAN_E,
                                CPSS_DXCH_BRG_DROP_CNTR_INVALID_VLAN_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E,
                                CPSS_DXCH_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_VLAN_RANGE_E,
                                CPSS_DXCH_BRG_DROP_CNTR_VLAN_RANGE_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E,
                                CPSS_DXCH_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_ARP_SA_MISMATCH_E,
                                CPSS_DXCH_BRG_DROP_CNTR_ARP_SA_MISMATCH_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_SYN_WITH_DATA_E,
                                CPSS_DXCH_BRG_DROP_CNTR_SYN_WITH_DATA_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_TCP_OVER_MC_OR_BC_E,
                                CPSS_DXCH_BRG_DROP_CNTR_TCP_OVER_MC_OR_BC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_ACCESS_MATRIX_E,
                                CPSS_DXCH_BRG_DROP_CNTR_ACCESS_MATRIX_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_SEC_LEARNING_E,
                                CPSS_DXCH_BRG_DROP_CNTR_SEC_LEARNING_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_ACCEPT_FRAME_TYPE_E,
                                CPSS_DXCH_BRG_DROP_CNTR_ACCEPT_FRAME_TYPE_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_FRAG_ICMP_E,
                                CPSS_DXCH_BRG_DROP_CNTR_FRAG_ICMP_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E,
                                CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E,
                                CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E,
                                CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E,
                                CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E,
                                CPSS_DXCH_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_VLAN_MRU_E,
                                CPSS_DXCH_BRG_DROP_CNTR_VLAN_MRU_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_RATE_LIMIT_E,
                                CPSS_DXCH_BRG_DROP_CNTR_RATE_LIMIT_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_LOCAL_PORT_E,
                                CPSS_DXCH_BRG_DROP_CNTR_LOCAL_PORT_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_SPAN_TREE_PORT_ST_E,
                                CPSS_DXCH_BRG_DROP_CNTR_SPAN_TREE_PORT_ST_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_IP_MC_E,
                                CPSS_DXCH_BRG_DROP_CNTR_IP_MC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_NON_IP_MC_E,
                                CPSS_DXCH_BRG_DROP_CNTR_NON_IP_MC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_DSATAG_LOCAL_DEV_E,
                                CPSS_DXCH_BRG_DROP_CNTR_DSATAG_LOCAL_DEV_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_IEEE_RESERVED_E,
                                CPSS_DXCH_BRG_DROP_CNTR_IEEE_RESERVED_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_UREG_L2_NON_IPM_MC_E,
                                CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPM_MC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV6_MC_E,
                                CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV6_MC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV4_MC_E,
                                CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_MC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_UNKNOWN_L2_UC_E,
                                CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_L2_UC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV4_BC_E,
                                CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_BC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dxChDropModePtr, PRV_TGF_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E,
                                CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E);

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertDxChToGenericDropCntMode
*
* DESCRIPTION:
*       Convert generic drop count mode from device specific drop count mode
*
* INPUTS:
*       dxChDropMode - DxCh drop counter mode
*
* OUTPUTS:
*       dropModePtr - (pointer to) drop counter mode
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertDxChToGenericDropCntMode
(
    IN  CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT  dxChDropMode,
    OUT PRV_TGF_BRG_DROP_CNTR_MODE_ENT      *dropModePtr
)
{
    /* convert drop counter mode from device specific format */
    switch (dxChDropMode)
    {
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_COUNT_ALL_E,
                                PRV_TGF_BRG_DROP_CNTR_COUNT_ALL_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_FDB_ENTRY_CMD_E,
                                PRV_TGF_BRG_DROP_CNTR_FDB_ENTRY_CMD_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E,
                                PRV_TGF_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_INVALID_SA_E,
                                PRV_TGF_BRG_DROP_CNTR_INVALID_SA_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_INVALID_VLAN_E,
                                PRV_TGF_BRG_DROP_CNTR_INVALID_VLAN_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E,
                                PRV_TGF_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_VLAN_RANGE_E,
                                PRV_TGF_BRG_DROP_CNTR_VLAN_RANGE_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E,
                                PRV_TGF_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_ARP_SA_MISMATCH_E,
                                PRV_TGF_BRG_DROP_CNTR_ARP_SA_MISMATCH_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_SYN_WITH_DATA_E,
                                PRV_TGF_BRG_DROP_CNTR_SYN_WITH_DATA_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_TCP_OVER_MC_OR_BC_E,
                                PRV_TGF_BRG_DROP_CNTR_TCP_OVER_MC_OR_BC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_ACCESS_MATRIX_E,
                                PRV_TGF_BRG_DROP_CNTR_ACCESS_MATRIX_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_SEC_LEARNING_E,
                                PRV_TGF_BRG_DROP_CNTR_SEC_LEARNING_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_ACCEPT_FRAME_TYPE_E,
                                PRV_TGF_BRG_DROP_CNTR_ACCEPT_FRAME_TYPE_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_FRAG_ICMP_E,
                                PRV_TGF_BRG_DROP_CNTR_FRAG_ICMP_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E,
                                PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E,
                                PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E,
                                PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E,
                                PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E,
                                PRV_TGF_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_VLAN_MRU_E,
                                PRV_TGF_BRG_DROP_CNTR_VLAN_MRU_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_RATE_LIMIT_E,
                                PRV_TGF_BRG_DROP_CNTR_RATE_LIMIT_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_LOCAL_PORT_E,
                                PRV_TGF_BRG_DROP_CNTR_LOCAL_PORT_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_SPAN_TREE_PORT_ST_E,
                                PRV_TGF_BRG_DROP_CNTR_SPAN_TREE_PORT_ST_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_IP_MC_E,
                                PRV_TGF_BRG_DROP_CNTR_IP_MC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_NON_IP_MC_E,
                                PRV_TGF_BRG_DROP_CNTR_NON_IP_MC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_DSATAG_LOCAL_DEV_E,
                                PRV_TGF_BRG_DROP_CNTR_DSATAG_LOCAL_DEV_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_IEEE_RESERVED_E,
                                PRV_TGF_BRG_DROP_CNTR_IEEE_RESERVED_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPM_MC_E,
                                PRV_TGF_BRG_DROP_CNTR_UREG_L2_NON_IPM_MC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV6_MC_E,
                                PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV6_MC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_MC_E,
                                PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV4_MC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_UNKNOWN_L2_UC_E,
                                PRV_TGF_BRG_DROP_CNTR_UNKNOWN_L2_UC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_IPV4_BC_E,
                                PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV4_BC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_DXCH_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E,
                                PRV_TGF_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E);

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
/*******************************************************************************
* prvTgfConvertGenericToExMxPmMacEntryKey
*
* DESCRIPTION:
*       Convert generic MAC entry key into device specific MAC entry key
*
* INPUTS:
*       macEntryKeyPtr - (pointer to) MAC entry key parameters
*
* OUTPUTS:
*       exMxPmMacEntryKeyPtr - (pointer to) ExMxPm MAC entry key parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmMacEntryKey
(
    IN  PRV_TGF_MAC_ENTRY_KEY_STC       *macEntryKeyPtr,
    OUT CPSS_EXMXPM_FDB_ENTRY_KEY_STC   *exMxPmMacEntryKeyPtr
)
{
    cpssOsMemSet(exMxPmMacEntryKeyPtr, 0, sizeof(*exMxPmMacEntryKeyPtr));

    /* convert entry type into device specific format */
    switch (macEntryKeyPtr->entryType)
    {
        case PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E:
            exMxPmMacEntryKeyPtr->entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E:
            exMxPmMacEntryKeyPtr->entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E:
            exMxPmMacEntryKeyPtr->entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_VPLS_E:
            exMxPmMacEntryKeyPtr->entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert key data into device specific format */
    if ((PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E == macEntryKeyPtr->entryType) ||
        (PRV_TGF_FDB_ENTRY_TYPE_VPLS_E == macEntryKeyPtr->entryType))
    {
        cpssOsMemCpy(exMxPmMacEntryKeyPtr->key.macFid.macAddr.arEther,
                     macEntryKeyPtr->key.macVlan.macAddr.arEther,
                     sizeof(exMxPmMacEntryKeyPtr->key.macFid.macAddr));
        exMxPmMacEntryKeyPtr->key.macFid.fId = macEntryKeyPtr->key.macVlan.fId;
    }
    else
    {
        cpssOsMemCpy(exMxPmMacEntryKeyPtr->key.ipMcast.sip,
                     macEntryKeyPtr->key.ipMcast.sip,
                     sizeof(exMxPmMacEntryKeyPtr->key.ipMcast.sip));
        cpssOsMemCpy(exMxPmMacEntryKeyPtr->key.ipMcast.dip,
                     macEntryKeyPtr->key.ipMcast.dip,
                     sizeof(exMxPmMacEntryKeyPtr->key.ipMcast.dip));
        exMxPmMacEntryKeyPtr->key.ipMcast.fId = macEntryKeyPtr->key.ipMcast.fId;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmMacEntry
*
* DESCRIPTION:
*       Convert generic MAC entry into device specific MAC entry
*
* INPUTS:
*       macEntryPtr - (pointer to) MAC entry parameters
*
* OUTPUTS:
*       exMxPmMacEntryPtr - (pointer to) ExMxPm MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmMacEntry
(
    IN  PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr,
    OUT CPSS_EXMXPM_FDB_ENTRY_STC    *exMxPmMacEntryPtr
)
{
    /* convert entry type into device specific format */
    switch (macEntryPtr->key.entryType)
    {
        case PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E:
            exMxPmMacEntryPtr->key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E:
            exMxPmMacEntryPtr->key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E:
            exMxPmMacEntryPtr->key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E;
            break;

        case PRV_TGF_FDB_ENTRY_TYPE_VPLS_E:
            exMxPmMacEntryPtr->key.entryType = CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert key data into device specific format */
    if ((PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E == macEntryPtr->key.entryType) ||
        (PRV_TGF_FDB_ENTRY_TYPE_VPLS_E == macEntryPtr->key.entryType))
    {
        cpssOsMemCpy(exMxPmMacEntryPtr->key.key.macFid.macAddr.arEther,
                     macEntryPtr->key.key.macVlan.macAddr.arEther,
                     sizeof(exMxPmMacEntryPtr->key.key.macFid.macAddr));
        exMxPmMacEntryPtr->key.key.macFid.fId = macEntryPtr->key.key.macVlan.fId;
    }
    else
    {
        cpssOsMemCpy(exMxPmMacEntryPtr->key.key.ipMcast.sip,
                     macEntryPtr->key.key.ipMcast.sip,
                     sizeof(exMxPmMacEntryPtr->key.key.ipMcast.sip));
        cpssOsMemCpy(exMxPmMacEntryPtr->key.key.ipMcast.dip,
                     macEntryPtr->key.key.ipMcast.dip,
                     sizeof(exMxPmMacEntryPtr->key.key.ipMcast.dip));
        exMxPmMacEntryPtr->key.key.ipMcast.fId = macEntryPtr->key.key.ipMcast.fId;
    }

    /* convert interface info into device specific format */
    exMxPmMacEntryPtr->dstOutlif.interfaceInfo.type = macEntryPtr->dstInterface.type;
    switch (exMxPmMacEntryPtr->dstOutlif.interfaceInfo.type)
    {
        case CPSS_INTERFACE_PORT_E:
            exMxPmMacEntryPtr->dstOutlif.interfaceInfo.devPort.devNum  = macEntryPtr->dstInterface.devPort.devNum;
            exMxPmMacEntryPtr->dstOutlif.interfaceInfo.devPort.portNum = macEntryPtr->dstInterface.devPort.portNum;
            break;

        case CPSS_INTERFACE_TRUNK_E:
            exMxPmMacEntryPtr->dstOutlif.interfaceInfo.trunkId = macEntryPtr->dstInterface.trunkId;
            break;

        case CPSS_INTERFACE_VIDX_E:
            exMxPmMacEntryPtr->dstOutlif.interfaceInfo.vidx = macEntryPtr->dstInterface.vidx;
            break;

        case CPSS_INTERFACE_VID_E:
            exMxPmMacEntryPtr->dstOutlif.interfaceInfo.vlanId = macEntryPtr->dstInterface.vlanId;
            break;

        case CPSS_INTERFACE_DEVICE_E:
            exMxPmMacEntryPtr->dstOutlif.interfaceInfo.devNum = macEntryPtr->dstInterface.devNum;
            break;

        case CPSS_INTERFACE_FABRIC_VIDX_E:
            exMxPmMacEntryPtr->dstOutlif.interfaceInfo.fabricVidx = macEntryPtr->dstInterface.fabricVidx;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert interface info into device specific format */
    exMxPmMacEntryPtr->dstOutlif.outlifType = macEntryPtr->dstOutlif.outlifType;
    switch (exMxPmMacEntryPtr->dstOutlif.outlifType)
    {
        case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
            exMxPmMacEntryPtr->dstOutlif.outlifPointer.arpPtr = macEntryPtr->dstOutlif.outlifPointer.arpPtr;
            break;

        case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
            exMxPmMacEntryPtr->dstOutlif.outlifPointer.ditPtr = macEntryPtr->dstOutlif.outlifPointer.ditPtr;
            break;

        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
            /* convert passenger type into device specific format */
            switch (macEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType)
            {
                case PRV_TGF_TS_PASSENGER_PACKET_TYPE_ETHERNET_E:
                    exMxPmMacEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType =
                        CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ETHERNET_E;
                    break;

                case PRV_TGF_TS_PASSENGER_PACKET_TYPE_OTHER_E:
                    exMxPmMacEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType =
                        CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_OTHER_E;
                    break;

                default:
                    return GT_BAD_PARAM;
            }

            exMxPmMacEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.ptr =
                macEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.ptr;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert daCommand info into device specific format */
    switch (macEntryPtr->daCommand)
    {
        case PRV_TGF_PACKET_CMD_FORWARD_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_FORWARD_E;
            break;

        case PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
            break;

        case PRV_TGF_PACKET_CMD_TRAP_TO_CPU_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_HARD_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_DROP_HARD_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_SOFT_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;

        case PRV_TGF_PACKET_CMD_ROUTE_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_ROUTE_E;
            break;

        case PRV_TGF_PACKET_CMD_ROUTE_AND_MIRROR_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
            break;

        case PRV_TGF_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;

        case PRV_TGF_PACKET_CMD_BRIDGE_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_BRIDGE_E;
            break;

        case PRV_TGF_PACKET_CMD_NONE_E:
            exMxPmMacEntryPtr->daCommand = CPSS_PACKET_CMD_NONE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert saCommand info into device specific format */
    switch (macEntryPtr->saCommand)
    {
        case PRV_TGF_PACKET_CMD_FORWARD_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_FORWARD_E;
            break;

        case PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
            break;

        case PRV_TGF_PACKET_CMD_TRAP_TO_CPU_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_HARD_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_DROP_HARD_E;
            break;

        case PRV_TGF_PACKET_CMD_DROP_SOFT_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;

        case PRV_TGF_PACKET_CMD_ROUTE_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_ROUTE_E;
            break;

        case PRV_TGF_PACKET_CMD_ROUTE_AND_MIRROR_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
            break;

        case PRV_TGF_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;

        case PRV_TGF_PACKET_CMD_BRIDGE_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_BRIDGE_E;
            break;

        case PRV_TGF_PACKET_CMD_NONE_E:
            exMxPmMacEntryPtr->saCommand = CPSS_PACKET_CMD_NONE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert generic vlanInfo into device specific vlanInfo */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, isStatic);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, daRoute);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, mirrorToRxAnalyzerPortEn);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, sourceId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, userDefined);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, daQosIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, saQosIndex);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, daSecurityLevel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, saSecurityLevel);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, appSpecificCpuCode);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, pwId);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, spUnknown);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertExMxPmToGenericMacEntry
*
* DESCRIPTION:
*       Convert device specific MAC entry into generic MAC entry
*
* INPUTS:
*       exMxPmMacEntryPtr - (pointer to) ExMxPm MAC entry parameters
*
* OUTPUTS:
*       macEntryPtr - (pointer to) MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertExMxPmToGenericMacEntry
(
    IN  CPSS_EXMXPM_FDB_ENTRY_STC    *exMxPmMacEntryPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
)
{
    /* convert entry type from device specific format */
    switch (exMxPmMacEntryPtr->key.entryType)
    {
        case CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
            break;

        case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E;
            break;

        case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E;
            break;

        case CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_VPLS_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert key data from device specific format */
    if ((CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E == exMxPmMacEntryPtr->key.entryType) ||
        (CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E == exMxPmMacEntryPtr->key.entryType))
    {
        cpssOsMemCpy(macEntryPtr->key.key.macVlan.macAddr.arEther,
                     exMxPmMacEntryPtr->key.key.macFid.macAddr.arEther,
                     sizeof(exMxPmMacEntryPtr->key.key.macFid.macAddr));
        macEntryPtr->key.key.macVlan.fId = exMxPmMacEntryPtr->key.key.macFid.fId;
    }
    else
    {
        cpssOsMemCpy(macEntryPtr->key.key.ipMcast.sip,
                     exMxPmMacEntryPtr->key.key.ipMcast.sip,
                     sizeof(exMxPmMacEntryPtr->key.key.ipMcast.sip));
        cpssOsMemCpy(macEntryPtr->key.key.ipMcast.dip,
                     exMxPmMacEntryPtr->key.key.ipMcast.dip,
                     sizeof(exMxPmMacEntryPtr->key.key.ipMcast.dip));
        macEntryPtr->key.key.ipMcast.fId = exMxPmMacEntryPtr->key.key.ipMcast.fId;
    }

    /* convert interface info from device specific format */
    macEntryPtr->dstInterface.type = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.type;
    switch (exMxPmMacEntryPtr->dstOutlif.interfaceInfo.type)
    {
        case CPSS_INTERFACE_PORT_E:
            macEntryPtr->dstInterface.devPort.devNum  = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.devPort.devNum;
            macEntryPtr->dstInterface.devPort.portNum = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.devPort.portNum;
            break;

        case CPSS_INTERFACE_TRUNK_E:
            macEntryPtr->dstInterface.trunkId = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.trunkId;
            break;

        case CPSS_INTERFACE_VIDX_E:
            macEntryPtr->dstInterface.vidx = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.vidx;
            break;

        case CPSS_INTERFACE_VID_E:
            macEntryPtr->dstInterface.vlanId = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.vlanId;
            break;

        case CPSS_INTERFACE_DEVICE_E:
            macEntryPtr->dstInterface.devNum = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.devNum;
            break;

        case CPSS_INTERFACE_FABRIC_VIDX_E:
            macEntryPtr->dstInterface.fabricVidx = exMxPmMacEntryPtr->dstOutlif.interfaceInfo.fabricVidx;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert interface info from device specific format */
    macEntryPtr->dstOutlif.outlifType = exMxPmMacEntryPtr->dstOutlif.outlifType;
    switch (exMxPmMacEntryPtr->dstOutlif.outlifType)
    {
        case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
            macEntryPtr->dstOutlif.outlifPointer.arpPtr = exMxPmMacEntryPtr->dstOutlif.outlifPointer.arpPtr;
            break;

        case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
            macEntryPtr->dstOutlif.outlifPointer.ditPtr = exMxPmMacEntryPtr->dstOutlif.outlifPointer.ditPtr;
            break;

        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
            /* convert passenger type from device specific format */
            switch (exMxPmMacEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType)
            {
                case CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ETHERNET_E:
                    macEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType =
                        PRV_TGF_TS_PASSENGER_PACKET_TYPE_ETHERNET_E;
                    break;

                case CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_OTHER_E:
                    macEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.passengerPacketType =
                        PRV_TGF_TS_PASSENGER_PACKET_TYPE_OTHER_E;
                    break;

                default:
                    return GT_BAD_PARAM;
            }

            macEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.ptr =
                exMxPmMacEntryPtr->dstOutlif.outlifPointer.tunnelStartPtr.ptr;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* convert daCommand info from device specific format */
    switch (exMxPmMacEntryPtr->daCommand)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_FORWARD_E;
            break;

        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E;
            break;

        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_TRAP_TO_CPU_E;
            break;

        case CPSS_PACKET_CMD_DROP_HARD_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_DROP_HARD_E;
            break;

        case CPSS_PACKET_CMD_DROP_SOFT_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_DROP_SOFT_E;
            break;

        case CPSS_PACKET_CMD_ROUTE_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_ROUTE_E;
            break;

        case CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_ROUTE_AND_MIRROR_E;
            break;

        case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;

        case CPSS_PACKET_CMD_BRIDGE_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_BRIDGE_E;
            break;

        case CPSS_PACKET_CMD_NONE_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_NONE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert saCommand info from device specific format */
    switch (exMxPmMacEntryPtr->saCommand)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_FORWARD_E;
            break;

        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E;
            break;

        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_TRAP_TO_CPU_E;
            break;

        case CPSS_PACKET_CMD_DROP_HARD_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_DROP_HARD_E;
            break;

        case CPSS_PACKET_CMD_DROP_SOFT_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_DROP_SOFT_E;
            break;

        case CPSS_PACKET_CMD_ROUTE_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_ROUTE_E;
            break;

        case CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_ROUTE_AND_MIRROR_E;
            break;

        case CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;

        case CPSS_PACKET_CMD_BRIDGE_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_BRIDGE_E;
            break;

        case CPSS_PACKET_CMD_NONE_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_NONE_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert generic vlanInfo from device specific vlanInfo */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, isStatic);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, daRoute);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, mirrorToRxAnalyzerPortEn);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, sourceId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, userDefined);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, daQosIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, saQosIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, daSecurityLevel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, saSecurityLevel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, appSpecificCpuCode);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, pwId);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(exMxPmMacEntryPtr, macEntryPtr, spUnknown);

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertGenericToExMxPmDropCntMode
*
* DESCRIPTION:
*       Convert generic drop count mode into device specific drop count mode
*
* INPUTS:
*       dropMode - drop counter mode
*
* OUTPUTS:
*       dxChDropModePtr - (pointer to) ExMxPm drop counter mode
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertGenericToExMxPmDropCntMode
(
    IN  PRV_TGF_BRG_DROP_CNTR_MODE_ENT       dropMode,
    OUT CPSS_EXMXPM_BRG_DROP_CNTR_MODE_ENT  *exMxPmDropModePtr
)
{
    /* convert drop counter mode into device specific format */
    switch (dropMode)
    {
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_COUNT_ALL_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_COUNT_ALL_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_FDB_ENTRY_CMD_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_FDB_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_INVALID_SA_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_INVALID_MAC_SA_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_INVALID_VLAN_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_INVALID_VLAN_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_VLAN_RANGE_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_VLAN_RANGE_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_MOVED_STATIC_SA_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_ARP_SA_MISMATCH_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_ARP_SA_MISMATCH_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_SYN_WITH_DATA_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_SYN_WITH_DATA_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_TCP_OVER_MC_OR_BC_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_TCP_OVER_MC_BC_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_ACCESS_MATRIX_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_ACCESS_MATRIX_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_SEC_LEARNING_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_SECURE_LEARNING_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_ACCEPT_FRAME_TYPE_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_ACCEPT_FRAME_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_FRAG_ICMP_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_FRAG_ICMP_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_VLAN_MRU_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_VLAN_MRU_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_RATE_LIMIT_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_RATE_LIMIT_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_LOCAL_PORT_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_LOCAL_PACKET_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_IEEE_RESERVED_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_IEEE_RESERVED_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_UREG_L2_NON_IPM_MC_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_NON_IPM_MC_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV6_MC_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_IPV6_IPM_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV4_MC_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_IPV4_IPM_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_UNKNOWN_L2_UC_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_UC_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV4_BC_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_IPV4_BC_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_NON_IPV4_BC_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_DATA_ERROR_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_DATA_ERROR_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_IPMC_IANA_RANGE_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_IPMC_IANA_RANGE_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_IPMC_NOT_IANA_RANGE_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_IPMC_NOT_IANA_RANGE_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_VALUE_CHASIS_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_VALUE_CHASIS_E);
        PRV_TGF_SWITCH_CASE_MAC(*exMxPmDropModePtr, PRV_TGF_BRG_DROP_CNTR_BLOCKING_LEARNING_E,
                                CPSS_EXMXPM_BRG_DROP_CNTR_BLOCKING_LEARNING_E);

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertExMxPmToGenericDropCntMode
*
* DESCRIPTION:
*       Convert generic drop count mode from device specific drop count mode
*
* INPUTS:
*       exMxPmDropMode - ExMxPm drop counter mode
*
* OUTPUTS:
*       dropModePtr - (pointer to) drop counter mode
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertExMxPmToGenericDropCntMode
(
    IN  CPSS_EXMXPM_BRG_DROP_CNTR_MODE_ENT   exMxPmDropMode,
    OUT PRV_TGF_BRG_DROP_CNTR_MODE_ENT      *dropModePtr
)
{
    /* convert drop counter mode from device specific format */
    switch (exMxPmDropMode)
    {
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_COUNT_ALL_E,
                                PRV_TGF_BRG_DROP_CNTR_COUNT_ALL_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_FDB_E,
                                PRV_TGF_BRG_DROP_CNTR_FDB_ENTRY_CMD_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E,
                                PRV_TGF_BRG_DROP_CNTR_UNKNOWN_MAC_SA_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_INVALID_MAC_SA_E,
                                PRV_TGF_BRG_DROP_CNTR_INVALID_SA_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_INVALID_VLAN_E,
                                PRV_TGF_BRG_DROP_CNTR_INVALID_VLAN_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E,
                                PRV_TGF_BRG_DROP_CNTR_PORT_NOT_IN_VLAN_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_VLAN_RANGE_E,
                                PRV_TGF_BRG_DROP_CNTR_VLAN_RANGE_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_MOVED_STATIC_SA_E,
                                PRV_TGF_BRG_DROP_CNTR_MOVED_STATIC_ADDR_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_ARP_SA_MISMATCH_E,
                                PRV_TGF_BRG_DROP_CNTR_ARP_SA_MISMATCH_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_SYN_WITH_DATA_E,
                                PRV_TGF_BRG_DROP_CNTR_SYN_WITH_DATA_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_TCP_OVER_MC_BC_E,
                                PRV_TGF_BRG_DROP_CNTR_TCP_OVER_MC_OR_BC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_ACCESS_MATRIX_E,
                                PRV_TGF_BRG_DROP_CNTR_ACCESS_MATRIX_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_SECURE_LEARNING_E,
                                PRV_TGF_BRG_DROP_CNTR_SEC_LEARNING_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_ACCEPT_FRAME_E,
                                PRV_TGF_BRG_DROP_CNTR_ACCEPT_FRAME_TYPE_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_FRAG_ICMP_E,
                                PRV_TGF_BRG_DROP_CNTR_FRAG_ICMP_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E,
                                PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_ZERO_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E,
                                PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_FUP_SET_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E,
                                PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_SF_SET_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E,
                                PRV_TGF_BRG_DROP_CNTR_TCP_FLAGS_SR_SET_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E,
                                PRV_TGF_BRG_DROP_CNTR_TCP_UDP_PORT_ZERO_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_VLAN_MRU_E,
                                PRV_TGF_BRG_DROP_CNTR_VLAN_MRU_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_RATE_LIMIT_E,
                                PRV_TGF_BRG_DROP_CNTR_RATE_LIMIT_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_LOCAL_PACKET_E,
                                PRV_TGF_BRG_DROP_CNTR_LOCAL_PORT_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_IEEE_RESERVED_E,
                                PRV_TGF_BRG_DROP_CNTR_IEEE_RESERVED_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_NON_IPM_MC_E,
                                PRV_TGF_BRG_DROP_CNTR_UREG_L2_NON_IPM_MC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_IPV6_IPM_E,
                                PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV6_MC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_IPV4_IPM_E,
                                PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV4_MC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_UC_E,
                                PRV_TGF_BRG_DROP_CNTR_UNKNOWN_L2_UC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_IPV4_BC_E,
                                PRV_TGF_BRG_DROP_CNTR_UREG_L2_IPV4_BC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_UNREG_NON_IPV4_BC_E,
                                PRV_TGF_BRG_DROP_CNTR_UREG_L2_NON_IPV4_BC_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_DATA_ERROR_E,
                                PRV_TGF_BRG_DROP_CNTR_DATA_ERROR_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_IPMC_IANA_RANGE_E,
                                PRV_TGF_BRG_DROP_CNTR_IPMC_IANA_RANGE_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_IPMC_NOT_IANA_RANGE_E,
                                PRV_TGF_BRG_DROP_CNTR_IPMC_NOT_IANA_RANGE_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_VALUE_CHASIS_E,
                                PRV_TGF_BRG_DROP_CNTR_VALUE_CHASIS_E);
        PRV_TGF_SWITCH_CASE_MAC(*dropModePtr, CPSS_EXMXPM_BRG_DROP_CNTR_BLOCKING_LEARNING_E,
                                PRV_TGF_BRG_DROP_CNTR_BLOCKING_LEARNING_E);

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
* prvTgfBrgVlanVrfIdSet
*
* DESCRIPTION:
*       Updates VRF Id in HW vlan entry
*
* INPUTS:
*       vlanId             - VLAN Id
*       vrfId              - VRF Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanVrfIdSet
(
    IN GT_U16                               vlanId,
    IN GT_U32                               vrfId
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
    CPSS_DXCH_BRG_VLAN_INFO_STC             dxChVlanInfo;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC    dxChPortsTaggingCmd;
    CPSS_PORTS_BMP_STC                      portsMembers;
    CPSS_PORTS_BMP_STC                      portsTagging;
    GT_BOOL                                 isValid;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while (GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific read API */
        rc = cpssDxChBrgVlanEntryRead(
            devNum,
            vlanId,
            &portsMembers,
            &portsTagging,
            &dxChVlanInfo,
            &isValid,
            &dxChPortsTaggingCmd);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanEntryRead FAILED, rc = [%d]", rc);

            rc1 = rc;
        }

        if (isValid == GT_FALSE)
        {
            /* bypass invalid VLAN entry */
            rc1 = GT_BAD_STATE;
            continue;
        }

        /* update VRF Id*/
        dxChVlanInfo.vrfId = vrfId;

        /* call device specific write API */
        rc = cpssDxChBrgVlanEntryWrite(
            devNum,
            vlanId,
            &portsMembers,
            &portsTagging,
            &dxChVlanInfo,
            &dxChPortsTaggingCmd);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanEntryWrite FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#else
    /* avoid warnings */
    TGF_PARAM_NOT_USED(vlanId);
    TGF_PARAM_NOT_USED(vrfId);

    /* not applicable for not DXCH devices */
    return GT_NOT_APPLICABLE_DEVICE;
#endif /* CHX_FAMILY */
}

/*******************************************************************************
* prvTgfBrgVlanEntryWrite
*
* DESCRIPTION:
*       Builds and writes vlan entry to HW
*
* INPUTS:
*       devNum             - device number
*       vlanId             - VLAN Id
*       portsMembersPtr    - (pointer to) bmp of ports members in vlan
*       portsTaggingPtr    - (pointer to) bmp of ports tagged in the vlan
*       vlanInfoPtr        - (pointer to) VLAN specific information
*       portsTaggingCmdPtr - (pointer to) ports tagging commands in the vlan
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vid
*       GT_OUT_OF_RANGE - length of portsMembersPtr or portsTaggingPtr is
*                         out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanEntryWrite
(
    IN GT_U8                                devNum,
    IN GT_U16                               vlanId,
    IN CPSS_PORTS_BMP_STC                  *portsMembersPtr,
    IN CPSS_PORTS_BMP_STC                  *portsTaggingPtr,
    IN PRV_TGF_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    IN PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr
)
{
    GT_U32  ii;
    PRV_TGF_MEMBER_FORCE_INFO_STC *currMemPtr;
    CPSS_PORTS_BMP_STC            tmpPortsMembers;
    CPSS_PORTS_BMP_STC            tmpPortsTagging;
    PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC  tmpPortsTaggingCmd;

#ifdef CHX_FAMILY
    GT_U32      portIter = 0;
    CPSS_DXCH_BRG_VLAN_INFO_STC             dxChVlanInfo;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC    dxChPortsTaggingCmd;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_BRG_VLAN_INFO_STC   exMxPmVlanInfo;
#endif /* EXMXPM_FAMILY */

    if(portsMembersPtr)
    {
        tmpPortsMembers = *portsMembersPtr;
    }
    else
    {
        cpssOsMemSet(&tmpPortsMembers,0,sizeof(tmpPortsMembers));
    }

    if(portsTaggingPtr)
    {
        tmpPortsTagging = *portsTaggingPtr;
    }
    else
    {
        cpssOsMemSet(&tmpPortsTagging,0,sizeof(tmpPortsTagging));
    }

    if(portsTaggingCmdPtr)
    {
        tmpPortsTaggingCmd = *portsTaggingCmdPtr;
    }
    else
    {
        cpssOsMemSet(&tmpPortsTaggingCmd,0,sizeof(tmpPortsTaggingCmd));
    }

    /* look for this member in the DB */
    currMemPtr = &prvTgfDevPortForceArray[0];
    for(ii = 0 ; ii < prvTgfDevPortForceNum; ii++,currMemPtr++)
    {
        if(devNum != currMemPtr->member.devNum)
        {
            continue;
        }

        if(currMemPtr->forceToVlan == GT_FALSE)
        {
            /* member not need to be forced to any vlan */
            continue;
        }

        if(currMemPtr->member.portNum >= CPSS_MAX_PORTS_NUM_CNS)
        {
            return GT_BAD_PARAM;
        }

        /* set the member */
        CPSS_PORTS_BMP_PORT_SET_MAC(&tmpPortsMembers,currMemPtr->member.portNum);

        if(currMemPtr->vlanInfo.tagged == GT_TRUE)
        {
            CPSS_PORTS_BMP_PORT_SET_MAC(&tmpPortsTagging,currMemPtr->member.portNum);
        }

        tmpPortsTaggingCmd.portsCmd[currMemPtr->member.portNum] = currMemPtr->vlanInfo.tagCmd;
    }


#ifdef CHX_FAMILY
    /* clear entry */
    cpssOsMemSet((GT_VOID*) &dxChVlanInfo, 0, sizeof(dxChVlanInfo));
    cpssOsMemSet((GT_VOID*) &dxChPortsTaggingCmd, 0, sizeof(dxChPortsTaggingCmd));

    /* convert generic vlanInfo into device specific vlanInfo */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, unkSrcAddrSecBreach );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, unregNonIpMcastCmd  );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, unregIpv4McastCmd   );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, unregIpv6McastCmd   );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, unkUcastCmd         );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, unregIpv4BcastCmd   );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, unregNonIpv4BcastCmd);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, ipv4IgmpToCpuEn     );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, mirrToRxAnalyzerEn  );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, ipv6IcmpToCpuEn     );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, ipv4IpmBrgMode      );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, ipv6IpmBrgMode      );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, ipv4IpmBrgEn        );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, ipv6IpmBrgEn        );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, ipv6SiteIdMode      );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, ipv4UcastRouteEn    );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, ipv4McastRouteEn    );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, ipv6UcastRouteEn    );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, ipv6McastRouteEn    );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, stgId               );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, autoLearnDisable    );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, naMsgToCpuEn        );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, mruIdx              );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, bcastUdpTrapMirrEn  );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, vrfId               );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, floodVidx           );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, ucastLocalSwitchingEn);
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&dxChVlanInfo, vlanInfoPtr, mcastLocalSwitchingEn);

    /* convert generic ipCtrlToCpuEn into device specific ipCtrlToCpuEn */
    switch (vlanInfoPtr->ipCtrlToCpuEn)
    {
        case PRV_TGF_BRG_IP_CTRL_NONE_E:
            dxChVlanInfo.ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_NONE_E;
            break;

        case PRV_TGF_BRG_IP_CTRL_IPV4_E:
            dxChVlanInfo.ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_IPV4_E;
            break;

        case PRV_TGF_BRG_IP_CTRL_IPV6_E:
            dxChVlanInfo.ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_IPV6_E;
            break;

        case PRV_TGF_BRG_IP_CTRL_IPV4_IPV6_E:
            dxChVlanInfo.ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert generic floodVidxMode into device specific floodVidxMode */
    switch (vlanInfoPtr->floodVidxMode)
    {
        case PRV_TGF_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E:
            dxChVlanInfo.floodVidxMode = CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E;
            break;

        case PRV_TGF_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E:
            dxChVlanInfo.floodVidxMode = CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert generic portIsolationMode into device specific portIsolationMode */
    switch (vlanInfoPtr->portIsolationMode)
    {
        case PRV_TGF_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E:
            dxChVlanInfo.portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E;
            break;

        case PRV_TGF_BRG_VLAN_PORT_ISOLATION_L2_CMD_E:
            dxChVlanInfo.portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_CMD_E;
            break;

        case PRV_TGF_BRG_VLAN_PORT_ISOLATION_L3_CMD_E:
            dxChVlanInfo.portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E;
            break;

        case PRV_TGF_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E:
            dxChVlanInfo.portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert generic portsTaggingCmd into device specific portsTaggingCmd */
    for (portIter = 0; portIter < CPSS_MAX_PORTS_NUM_CNS; portIter++)
    {
        switch (tmpPortsTaggingCmd.portsCmd[portIter])
        {
            case PRV_TGF_BRG_VLAN_PORT_UNTAGGED_CMD_E:
                dxChPortsTaggingCmd.portsCmd[portIter] = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
                break;

            case PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E:
                dxChPortsTaggingCmd.portsCmd[portIter] = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;
                break;

            case PRV_TGF_BRG_VLAN_PORT_TAG1_CMD_E:
                dxChPortsTaggingCmd.portsCmd[portIter] = CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E;
                break;

            case PRV_TGF_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E:
                dxChPortsTaggingCmd.portsCmd[portIter] = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E;
                break;

            case PRV_TGF_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E:
                dxChPortsTaggingCmd.portsCmd[portIter] = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E;
                break;

            case PRV_TGF_BRG_VLAN_PORT_PUSH_TAG0_CMD_E:
                dxChPortsTaggingCmd.portsCmd[portIter] = CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E;
                break;

            case PRV_TGF_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E:
                dxChPortsTaggingCmd.portsCmd[portIter] = CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E;
                break;

            default:
                return GT_BAD_PARAM;
        }
    }

    /* call device specific API */
    return cpssDxChBrgVlanEntryWrite(devNum,
                                     vlanId,
                                     &tmpPortsMembers,
                                     &tmpPortsTagging,
                                     &dxChVlanInfo,
                                     &dxChPortsTaggingCmd);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* not used param */
    TGF_PARAM_NOT_USED(portsTaggingCmdPtr);

    /* clear entry */
    cpssOsMemSet((GT_VOID*) &exMxPmVlanInfo, 0, sizeof(exMxPmVlanInfo));

    /* convert generic vlanInfo into device specific vlanInfo */
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmVlanInfo, vlanInfoPtr, cpuMember             );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmVlanInfo, vlanInfoPtr, mirrorToTxAnalyzerEn  );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmVlanInfo, vlanInfoPtr, ipv4IpmBrgMode        );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmVlanInfo, vlanInfoPtr, ipv6IpmBrgMode        );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmVlanInfo, vlanInfoPtr, ipv4IpmBrgEn          );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmVlanInfo, vlanInfoPtr, ipv6IpmBrgEn          );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmVlanInfo, vlanInfoPtr, stgId                 );
    PRV_TGF_STC_S2D_FIELD_COPY_MAC(&exMxPmVlanInfo, vlanInfoPtr, egressUnregMcFilterCmd);

    /* call device specific API */
    return cpssExMxPmBrgVlanEntryWrite(devNum,
                                         vlanId,
                                         &tmpPortsMembers,
                                         &tmpPortsTagging,
                                         &exMxPmVlanInfo);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgVlanEntryInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN entry
*
* INPUTS:
*       vlanId - VLAN id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - wrong vid
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanEntryInvalidate
(
    IN GT_U16               vlanId
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
        rc = cpssDxChBrgVlanEntryInvalidate(devNum, vlanId);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanEntryInvalidate FAILED, rc = [%d]", rc);

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
        rc = cpssExMxPmBrgVlanEntryInvalidate(devNum, vlanId);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgVlanEntryInvalidate FAILED, rc = [%d]", rc);

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
* prvTgfBrgVlanMemberAdd
*
* DESCRIPTION:
*       Set specific member at VLAN entry
*
* INPUTS:
*       devNum   - device number
*       vlanId   - VLAN Id
*       portNum  - port number
*       isTagged - GT_TRUE/GT_FALSE, to set the port as tagged/untagged
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong input parameters
*       GT_FAIL      - otherwise
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanMemberAdd
(
    IN GT_U8                devNum,
    IN GT_U16               vlanId,
    IN GT_U8                portNum,
    IN GT_BOOL              isTagged
)
{
#ifdef CHX_FAMILY
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT dxChPortTaggingCmd = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;


    /* set port tagging command */
    dxChPortTaggingCmd = (GT_FALSE == isTagged) ? CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E :
                                                  CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;
    /* call device specific API */
    return cpssDxChBrgVlanMemberAdd(devNum, vlanId, portNum, isTagged, dxChPortTaggingCmd);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmBrgVlanMemberSet(devNum, vlanId, portNum, GT_TRUE, isTagged);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgVlanTagMemberAdd
*
* DESCRIPTION:
*       Set specific tagged member at VLAN entry
*
* INPUTS:
*       vlanId          - VLAN Id
*       portNum         - port number
*       portTaggingCmd  - port tagging command
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong input parameters
*       GT_FAIL      - otherwise
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanTagMemberAdd
(
    IN GT_U16                            vlanId,
    IN GT_U8                             portNum,
    IN PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT portTaggingCmd
)
{
    GT_U32  portIter = 0;
#ifdef CHX_FAMILY
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT dxChPortTaggingCmd = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    GT_BOOL isTagged = GT_FALSE;
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
    /* set port tagging command */
    switch (portTaggingCmd)
    {
        case PRV_TGF_BRG_VLAN_PORT_UNTAGGED_CMD_E:
            dxChPortTaggingCmd = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
            break;

        case PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E:
            dxChPortTaggingCmd = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;
            break;

        case PRV_TGF_BRG_VLAN_PORT_TAG1_CMD_E:
            dxChPortTaggingCmd = CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E;
            break;

        case PRV_TGF_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E:
            dxChPortTaggingCmd = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E;
            break;

        case PRV_TGF_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E:
            dxChPortTaggingCmd = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E;
            break;

        case PRV_TGF_BRG_VLAN_PORT_PUSH_TAG0_CMD_E:
            dxChPortTaggingCmd = CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E;
            break;

        case PRV_TGF_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E:
            dxChPortTaggingCmd = CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    return cpssDxChBrgVlanMemberAdd(prvTgfDevsArray[portIter], vlanId, portNum, GT_TRUE, dxChPortTaggingCmd);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* check if port is tagged */
    isTagged = (PRV_TGF_BRG_VLAN_PORT_UNTAGGED_CMD_E == portTaggingCmd) ? GT_FALSE : GT_TRUE;

    /* call device specific API */
    return cpssExMxPmBrgVlanMemberSet(prvTgfDevsArray[portIter], vlanId, portNum, GT_TRUE, isTagged);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgVlanMemberRemove
*
* DESCRIPTION:
*       Delete port member from vlan entry.
*
* INPUTS:
*       devNum  - device number
*       vlanId  - VLAN Id
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error.
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum or vlanId
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanMemberRemove
(
    IN GT_U8                devNum,
    IN GT_U16               vlanId,
    IN GT_U8                portNum
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgVlanPortDelete(devNum, vlanId, portNum);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmBrgVlanMemberSet(devNum, vlanId, portNum, GT_FALSE, GT_FALSE);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgVlanPortVidSet
*
* DESCRIPTION:
*       Set port's default VLAN Id.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       vlanId  - VLAN Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong devNum port, or vlanId
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortVidSet
(
    IN  GT_U8               devNum,
    IN  GT_U8               portNum,
    IN  GT_U16              vlanId
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgVlanPortVidSet(devNum, portNum, vlanId);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmBrgVlanPortPvidSet(devNum, portNum, vlanId);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgVlanPortVidGet
*
* DESCRIPTION:
*       Get port's default VLAN Id
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       vlanIdPtr - VLAN Id
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong devNum port, or vlanId
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortVidGet
(
    IN  GT_U8               devNum,
    IN  GT_U8               portNum,
    OUT GT_U16             *vlanIdPtr
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgVlanPortVidGet(devNum, portNum, vlanIdPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmBrgVlanPortPvidGet(devNum, portNum, vlanIdPtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgVlanPortVidPrecedenceSet
*
* DESCRIPTION:
*       Set Port VID Precedence
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       precedence - precedence type
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
GT_STATUS prvTgfBrgVlanPortVidPrecedenceSet
(
    IN GT_U8                                        devNum,
    IN GT_U8                                        portNum,
    IN CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  precedence
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgVlanPortVidPrecedenceSet(devNum, portNum, precedence);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmBrgVlanPortPvidPrecedenceSet(devNum, portNum, precedence);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgFdbMacEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware MAC address table in specified index.
*
* INPUTS:
*       index       - hw mac entry index
*       skip        - entry skip control
*       macEntryPtr - (pointer to) MAC entry parameters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum,saCommand,daCommand
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryWrite
(
    IN GT_U32                        index,
    IN GT_BOOL                       skip,
    IN PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
#ifdef CHX_FAMILY
    CPSS_MAC_ENTRY_EXT_STC      dxChMacEntry;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_FDB_ENTRY_STC   exMxPmMacEntry;
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToDxChMacEntry(macEntryPtr, &dxChMacEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMacEntry FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        APPDEMO_FDB_UPDATE_LOCK();
        rc = utfCpssDxChBrgFdbMacEntryWrite(devNum, index, skip, &dxChMacEntry);
        APPDEMO_FDB_UPDATE_UNLOCK();
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbMacEntryWrite FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToExMxPmMacEntry(macEntryPtr, &exMxPmMacEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmMacEntry FAILED, rc = [%d]", rc);

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
        rc = cpssExMxPmBrgFdbEntryWrite(devNum, index, skip, &exMxPmMacEntry);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbEntryWrite FAILED, rc = [%d]", rc);

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
* prvTgfBrgFdbMacEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware MAC address table from specified index.
*
* INPUTS:
*       index  - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       devNumPtr   - (pointer to) is device number associated with the entry
*       macEntryPtr - (pointer to) extended Mac table entry
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryRead
(
    IN  GT_U32                        index,
    OUT GT_BOOL                      *validPtr,
    OUT GT_BOOL                      *skipPtr,
    OUT GT_BOOL                      *agedPtr,
    OUT GT_U8                        *devNumPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
)
{
    GT_STATUS   rc = GT_OK;
#ifdef CHX_FAMILY
    CPSS_MAC_ENTRY_EXT_STC      dxChMacEntry;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_FDB_ENTRY_STC   exMxPmMacEntry;
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    /* call device specific API */
    APPDEMO_FDB_UPDATE_LOCK();
    rc = utfCpssDxChBrgFdbMacEntryRead(prvTgfDevNum, index, validPtr, skipPtr,
                                    agedPtr, devNumPtr, &dxChMacEntry);
    APPDEMO_FDB_UPDATE_UNLOCK();
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbMacEntryRead FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert devNumPtr to SW devNum.*/
    rc = prvUtfSwFromHwDeviceNumberGet(*devNumPtr, devNumPtr);
    if(GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfSwFromHwDeviceNumberGet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* convert key data from device specific format */
    return prvTgfConvertDxChToGenericMacEntry(&dxChMacEntry, macEntryPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    rc = cpssExMxPmBrgFdbEntryRead(prvTgfDevNum, index,validPtr, skipPtr,
                                   agedPtr, devNumPtr, &exMxPmMacEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbEntryRead FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert key data from device specific format */
    return prvTgfConvertExMxPmToGenericMacEntry(&exMxPmMacEntry, macEntryPtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgFdbMacEntryInvalidate
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified index.
*       the invalidation done by resetting to first word of the entry
*
* INPUTS:
*       index  - hw mac entry index.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong parameters
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryInvalidate
(
    IN GT_U32               index
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
        APPDEMO_FDB_UPDATE_LOCK();
        rc = utfCpssDxChBrgFdbMacEntryInvalidate(devNum, index);
        APPDEMO_FDB_UPDATE_UNLOCK();
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbMacEntryInvalidate FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmBrgFdbEntryInvalidate(devNum, index);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbEntryInvalidate FAILED, rc = [%d]", rc);

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
* prvTgfBrgFdbEntryByMessageMustSucceedSet
*
* DESCRIPTION:
*       when add/delete FDB entry 'by message' operation can not succeed.
*       the operation MUST finish but is may not succeed.
*       An AU message sent by CPU can fail in the following cases:
*       1. The message type = CPSS_NA_E and the hash chain has reached it's
*          maximum length.
*       2. The message type = CPSS_QA_E or CPSS_AA_E and the FDB entry doesn't
*          exist.
*
* INPUTS:
*       mustSucceed - GT_TRUE - operation must succeed
*                     GT_FALSE - operation may fail (must finish but not succeed)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       the previous state of the flag
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvTgfBrgFdbEntryByMessageMustSucceedSet
(
    IN GT_BOOL     mustSucceed
)
{
    GT_BOOL previousState = fdbEntryByMessageMustSucceed;

    fdbEntryByMessageMustSucceed = mustSucceed ;

    return previousState;
}

/*******************************************************************************
* prvTgfBrgFdbMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table
*
* INPUTS:
*       macEntryPtr - (pointer to) mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntrySet
(
    IN PRV_TGF_BRG_MAC_ENTRY_STC     *macEntryPtr
)
{
    GT_U8       devNum    = 0;
    GT_STATUS   rc, rc1   = GT_OK;
    GT_U32  index;/* fdb index */
#ifdef CHX_FAMILY
    CPSS_MAC_ENTRY_EXT_STC      dxChMacEntry;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    GT_BOOL     completed = GT_FALSE;
    GT_BOOL     succeeded = GT_FALSE;
    CPSS_EXMXPM_FDB_ENTRY_STC   exMxPmMacEntry;
#endif /* EXMXPM_FAMILY */


    if(prvTgfBrgFdbAccessMode == PRV_TGF_BRG_FDB_ACCESS_MODE_BY_INDEX_E)
    {
        /* allow to set the entry by index */

        /* calculate and find the index */
        rc = prvTgfBrgFdbMacEntryIndexFind(&macEntryPtr->key,&index);
        if(rc != GT_FULL)
        {
            if (GT_OK != rc && GT_NOT_FOUND != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfBrgFdbMacEntryIndexFind FAILED, rc = [%d]", rc);

                return rc;
            }

            return prvTgfBrgFdbMacEntryWrite(index,GT_FALSE,macEntryPtr);
        }
        else
        {
            /*
                NOTE: GT_FULL means that BUCKET is FULL --> the operation would
                    have failed on 'by messages' !!!


                fall through to do the 'by message' action
            */

            rc = GT_OK;
        }
    }


#ifdef CHX_FAMILY
    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToDxChMacEntry(macEntryPtr, &dxChMacEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMacEntry FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToExMxPmMacEntry(macEntryPtr, &exMxPmMacEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmMacEntry FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }
#endif /* EXMXPM_FAMILY */

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
#ifdef CHX_FAMILY
        /* call device specific API */
        APPDEMO_FDB_UPDATE_LOCK();
        rc = utfCpssDxChBrgFdbMacEntrySet(devNum, &dxChMacEntry);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbMacEntryInvalidate FAILED, rc = [%d]", rc);

            rc1 = rc;
            APPDEMO_FDB_UPDATE_UNLOCK();
            continue;
        }

        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(devNum,fdbEntryByMessageMustSucceed);
        APPDEMO_FDB_UPDATE_UNLOCK();
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
        /* call device specific API */
        rc = cpssExMxPmBrgFdbEntrySet(devNum, &exMxPmMacEntry);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbEntrySet FAILED, rc = [%d]", rc);

            rc1 = rc;

            continue;
        }
        /* synchronize call */
        completed = GT_FALSE;
        succeeded = GT_FALSE;
        do {
            cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(devNum, CPSS_QA_E, &completed, &succeeded);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbFromCpuAuMsgStatusGet FAILED, rc = [%d]", rc);

                rc1 = rc;
            }
        } while ((GT_FALSE == completed) && (GT_FALSE == succeeded));
#endif /* EXMXPM_FAMILY */
    }

    return rc1;
}

/*******************************************************************************
* prvTgfBrgFdbMacEntryGet
*
* DESCRIPTION:
*       Get existing entry in Hardware MAC address table
*
* INPUTS:
*       macEntryKeyPtr - (pointer to) mac entry key
*
* OUTPUTS:
*       macEntryPtr - (pointer to) mac table entry
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - entry not found
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryGet
(
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *macEntryKeyPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
)
{
    GT_STATUS   rc    = GT_OK;
    GT_U32      index = 0;
    GT_BOOL     valid = GT_FALSE;
    GT_BOOL     skip  = GT_FALSE;
    GT_BOOL     aged  = GT_FALSE;
    GT_U8       dev   = 0;

    if(prvTgfNumOfPortGroups && usePortGroupsBmp == GT_FALSE)
    {
        GT_U32      jj;
        /* save original value*/
        GT_U32 origCurrPortGroupsBmp = currPortGroupsBmp;

        rc = GT_FAIL;

        /***************************************************/
        /* lookup in all the port groups for the FDB entry */
        /***************************************************/

        usePortGroupsBmp = GT_TRUE;
        for(jj = 0 ; jj < prvTgfNumOfPortGroups ; jj++)
        {
            currPortGroupsBmp = 1 << jj;
            rc = prvTgfBrgFdbMacEntryIndexFind(macEntryKeyPtr,&index);
            if(rc == GT_NOT_FOUND || rc == GT_FULL)
            {
                /* try next port group */
                continue;
            }
            else /* (rc != GT_OK) || (rc == GT_OK) */
            {
                break;
            }
        }

        if(rc == GT_NOT_FOUND || rc == GT_FULL || (jj == prvTgfNumOfPortGroups))
        {
            rc = GT_NOT_FOUND;
        }
        else if (rc != GT_OK)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfBrgFdbMacEntryIndexFind 1 FAILED, rc = [%d]", rc);
        }

        if(rc == GT_OK)
        {
            /* read MAC entry */
            rc = prvTgfBrgFdbMacEntryRead(index, &valid, &skip, &aged, &dev, macEntryPtr);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvTgfBrgFdbMacEntryRead 1 FAILED, rc = [%d]", rc);
            }
        }

        /* restore original values */
        currPortGroupsBmp = origCurrPortGroupsBmp;
        usePortGroupsBmp = GT_FALSE;

        return rc;
    }

    /* find the proper index */
    rc = prvTgfBrgFdbMacEntryIndexFind(macEntryKeyPtr,&index);
    if(rc == GT_NOT_FOUND || rc == GT_FULL)
    {
        return GT_NOT_FOUND;
    }
    else if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfBrgFdbMacEntryIndexFind FAILED, rc = [%d]", rc);
        return rc;
    }

    /* read MAC entry */
    rc = prvTgfBrgFdbMacEntryRead(index, &valid, &skip, &aged, &dev, macEntryPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfBrgFdbMacEntryRead FAILED, rc = [%d]", rc);

        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfBrgFdbMacEntryIndexFind
*
* DESCRIPTION:
*   function calculate hash index for this mac address , and then start to read
*       from HW the entries to find an existing entry that match the key.
*
* INPUTS:
*       macEntryKeyPtr - (pointer to) mac entry key
*
* OUTPUTS:
*       indexPtr - (pointer to) :
*                   when GT_OK (found) --> index of the entry
*                   when GT_NOT_FOUND (not found) --> first index that can be used
*                   when GT_FULL (not found) --> index od start of bucket
*                   on other return value --> not relevant
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - entry not found, but bucket is not FULL
*       GT_FULL         - entry not found, but bucket is FULL
*       GT_BAD_PARAM    - wrong devNum.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryIndexFind
(
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *macEntryKeyPtr,
    OUT GT_U32                       *indexPtr
)
{
    GT_STATUS   rc    ;
    GT_U32      index ;
    GT_BOOL     valid ;
    GT_BOOL     skip  ;
    GT_BOOL     aged  ;
    GT_U8       dev   ;
    GT_U32      ii;
    PRV_TGF_BRG_MAC_ENTRY_STC    macEntry;
    GT_U32      firstEmptyIndex = NOT_INIT_CNS;

    /* calculate the hash index */
    rc = prvTgfBrgFdbMacEntryHashCalc(macEntryKeyPtr,&index);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfBrgFdbMacEntryHashCalc FAILED, rc = [%d]", rc);

        return rc;
    }

    for(ii = 0 ; ii < prvTgfBrgFdbHashLengthLookup ; ii++)
    {
        /* read MAC entry */
        rc = prvTgfBrgFdbMacEntryRead((index + ii), &valid, &skip, &aged, &dev, &macEntry);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfBrgFdbMacEntryIndexFind FAILED, rc = [%d]", rc);

            return rc;
        }

        if(valid && skip == GT_FALSE)
        {
            /* used entry */
            if((0 == prvTgfBrgFdbMacKeyCompare(macEntryKeyPtr,&macEntry.key)))
            {
                *indexPtr = index + ii;
                return GT_OK;
            }
        }
        else
        {
            /* UN-used entry */
            if(firstEmptyIndex == NOT_INIT_CNS)
            {
                firstEmptyIndex = index + ii;
            }
        }
    }

    if(firstEmptyIndex == NOT_INIT_CNS)
    {
        /* all entries are used */
        /* the returned index is of start of bucket */
        *indexPtr = index;
        return GT_FULL;
    }
    else
    {
        /* the returned index is of first empty entry */
        *indexPtr = firstEmptyIndex;
        return GT_NOT_FOUND;
    }
}


/*******************************************************************************
* prvTgfBrgFdbMacEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware MAC address table
*
* INPUTS:
*       macEntryKeyPtr - (pointer to) key parameters of the mac entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or vlanId
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryDelete
(
    IN PRV_TGF_MAC_ENTRY_KEY_STC     *macEntryKeyPtr
)
{
    GT_U8       devNum    = 0;
    GT_STATUS   rc, rc1   = GT_OK;
#ifdef CHX_FAMILY
    CPSS_MAC_ENTRY_EXT_KEY_STC  dxChMacEntryKey;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    GT_BOOL     completed = GT_FALSE;
    GT_BOOL     succeeded = GT_FALSE;
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC exMxPmMacEntryKey;
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToDxChMacEntryKey(macEntryKeyPtr, &dxChMacEntryKey);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMacEntryKey FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToExMxPmMacEntryKey(macEntryKeyPtr, &exMxPmMacEntryKey);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmMacEntryKey FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }
#endif /* EXMXPM_FAMILY */

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
#ifdef CHX_FAMILY
        /* call device specific API */
        APPDEMO_FDB_UPDATE_LOCK();
        rc = utfCpssDxChBrgFdbMacEntryDelete(devNum, &dxChMacEntryKey);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbMacEntryDelete FAILED, rc = [%d]", rc);

            rc1 = rc;

            APPDEMO_FDB_UPDATE_UNLOCK();

            continue;
        }

        utfCpssDxChBrgFdbFromCpuAuMsgStatusGet(devNum,fdbEntryByMessageMustSucceed);
        APPDEMO_FDB_UPDATE_UNLOCK();
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
        /* call device specific API */
        rc = cpssExMxPmBrgFdbEntryDelete(devNum, &exMxPmMacEntryKey);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbEntryDelete FAILED, rc = [%d]", rc);

            rc1 = rc;

            continue;
        }
        /* synchronize call */
        completed = GT_FALSE;
        succeeded = GT_FALSE;
        do {
            cpssExMxPmBrgFdbFromCpuAuMsgStatusGet(devNum, CPSS_QA_E, &completed, &succeeded);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbFromCpuAuMsgStatusGet FAILED, rc = [%d]", rc);

                rc1 = rc;
            }
        } while ((GT_FALSE == completed) && (GT_FALSE == succeeded));
#endif /* EXMXPM_FAMILY */
    }

    return rc1;
}

/*******************************************************************************
* prvTgfBrgFdbFlush
*
* DESCRIPTION:
*       Flush FDB table (with/without static entries).
*       function sets actDev and it's mask to 'dont care' so all entries can be
*       flushed (function restore default values at end of operation).
*
* INPUTS:
*       includeStatic - include static entries
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbFlush
(
    IN GT_BOOL                        includeStatic
)
{
    GT_U8       devNum        = 0;
    GT_STATUS   rc, rc1       = GT_OK;
    GT_BOOL     isCompleted   = GT_FALSE;
    GT_BOOL     staticEnabled = GT_FALSE;
#ifdef CHX_FAMILY
    GT_BOOL     messagesToCpuEnabled = GT_FALSE;
    GT_U32   origActDev;/*Action active device */
    GT_U32   origActDevMask;/*Action active mask*/
#endif /* CHX_FAMILY */

#ifdef CHX_FAMILY
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* get original act device info */
        rc = cpssDxChBrgFdbActionActiveDevGet(devNum,&origActDev,&origActDevMask);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbActionActiveDevGet FAILED, rc = [%d]", rc);

            rc1 = rc;
            continue;
        }

        /* get FDB flush status */
        rc = cpssDxChBrgFdbStaticDelEnableGet(devNum, &staticEnabled);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbStaticDelEnableGet FAILED, rc = [%d]", rc);

            rc1 = rc;
            continue;
        }

        if(includeStatic != staticEnabled)
        {
            /* set FDB flush status */
            rc = cpssDxChBrgFdbStaticDelEnable(devNum, includeStatic);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbStaticDelEnable FAILED, rc = [%d]", rc);

                rc1 = rc;
                continue;
            }
        }

        /* disable AA and TA messages to CPU */
        rc = cpssDxChBrgFdbAAandTAToCpuGet(devNum, &messagesToCpuEnabled);
        if ((rc == GT_OK) && messagesToCpuEnabled)
        {
            rc = cpssDxChBrgFdbAAandTAToCpuSet(devNum, GT_FALSE);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbAAandTAToCpuSet FAILED, rc = [%d]", rc);
            }
        }
        else
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbAAandTAToCpuGet FAILED, rc = [%d]", rc);
            messagesToCpuEnabled = GT_FALSE;
        }

        /* set new act device info */
        rc = cpssDxChBrgFdbActionActiveDevSet(devNum,0,0);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbActionActiveDevSet FAILED, rc = [%d]", rc);

            rc1 = rc;
            continue;
        }

        /* trigger address deleting */
        APPDEMO_FDB_UPDATE_LOCK();
        rc = cpssDxChBrgFdbTrigActionStart(devNum, CPSS_FDB_ACTION_DELETING_E);
        APPDEMO_FDB_UPDATE_UNLOCK();
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbTrigActionStart FAILED, rc = [%d]", rc);

            rc1 = rc;

            /* restore AA and TA messages to CPU */
            if (messagesToCpuEnabled)
            {
                rc = cpssDxChBrgFdbAAandTAToCpuSet(devNum, GT_TRUE);
                if (GT_OK != rc)
                {
                    PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbAAandTAToCpuSet FAILED, rc = [%d]", rc);
                }
            }
            continue;
        }

        /* wait that triggered action is completed */
        while (GT_TRUE != isCompleted)
        {
#ifdef ASIC_SIMULATION
            /* allow simulation to complete the operation*/
            cpssOsTimerWkAfter(50);
#endif /* ASIC_SIMULATION */

            /* get triggered action status */
            rc = cpssDxChBrgFdbTrigActionStatusGet(devNum, &isCompleted);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbTrigActionStatusGet FAILED, rc = [%d]", rc);

                rc1 = rc;

                break;
            }
        }

        /* restore AA and TA messages to CPU */
        if (messagesToCpuEnabled)
        {
            rc = cpssDxChBrgFdbAAandTAToCpuSet(devNum, GT_TRUE);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbAAandTAToCpuSet FAILED, rc = [%d]", rc);
            }
        }

        if(includeStatic != staticEnabled)
        {
            /* restore FDB flush status */
            rc = cpssDxChBrgFdbStaticDelEnable(devNum, staticEnabled);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbStaticDelEnable FAILED, rc = [%d]", rc);

                rc1 = rc;

                continue;
            }
        }

        /* restore original act device info */
        rc = cpssDxChBrgFdbActionActiveDevSet(devNum,origActDev,origActDevMask);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbActionActiveDevSet FAILED, rc = [%d]", rc);

            rc1 = rc;
            continue;
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
        /* get FDB flush status */
        rc = cpssExMxPmBrgFdbStaticDelEnableGet(devNum, &staticEnabled);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbStaticDelEnableGet FAILED, rc = [%d]", rc);

            rc1 = rc;

            continue;
        }

        if(includeStatic != staticEnabled)
        {
            /* set FDB flush status */
            rc = cpssExMxPmBrgFdbStaticDelEnableSet(devNum,includeStatic);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbStaticDelEnableSet FAILED, rc = [%d]", rc);

                rc1 = rc;

                continue;
            }
        }

        /* trigger address deleting */
        rc = cpssExMxPmBrgFdbTriggeredActionStart(devNum,CPSS_EXMXPM_FDB_ACTION_DELETING_E);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbTriggeredActionStart FAILED, rc = [%d]", rc);

            rc1 = rc;

            continue;
        }

        /* wait that triggered action is completed */
        while (GT_TRUE != isCompleted)
        {
#ifdef ASIC_SIMULATION
            /* allow simulation to complete the operation*/
            cpssOsTimerWkAfter(50);
#endif /* ASIC_SIMULATION */

            /* get triggered action status */
            rc = cpssExMxPmBrgFdbAgingTriggerGet(devNum, &isCompleted);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbAgingTriggerGet FAILED, rc = [%d]", rc);

                rc1 = rc;

                break;
            }
        }

        if(includeStatic != staticEnabled)
        {
            /* restore FDB flush status */
            rc = cpssExMxPmBrgFdbStaticDelEnableSet(devNum,staticEnabled);
            if (GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbStaticDelEnableSet FAILED, rc = [%d]", rc);

                rc1 = rc;

                continue;
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
* prvTgfBrgFdbActionStart
*
* DESCRIPTION:
*       Enable actions, sets action type, action mode to CPSS_ACT_TRIG_E and
*       starts triggered action by setting Aging Trigger.
*       This API may be used to start one of triggered actions: Aging, Deleting,
*       Transplanting and FDB Upload.
*
*       NOTE: caller should call prvTgfBrgFdbActionDoneWait(...) to check that
*       operation done before checking for the action outcomes...
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       mode    - action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device or mode
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE    - when previous FDB triggered action is not completed yet
*                         or CNC block upload not finished (or not all of it's
*                         results retrieved from the common used FU and CNC
*                         upload queue)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Deleting and Transplanting actions can be done only by CPU triggered
*       FDB action that activated by the function.
*       See below sequence of CPSS API calls for Deleting and
*       Transplanting actions.
*
*       Before making Delete or Transplant sequence the application need to
*       disable Actions to avoid wrong automatic aging.
*
*       It is Application responsibility to get and store all parameters
*       that are changed during those actions.
*       1. AA message to CPU status by cpssDxChBrgFdbAAandTAToCpuGet.
*       2. Action Active configuration by
*          cpssDxChBrgFdbActionActiveInterfaceGet,
*          cpssDxChBrgFdbActionActiveDevGet,
*          cpssDxChBrgFdbActionActiveVlanGet.
*       3. Action mode and trigger mode by cpssDxChBrgFdbActionModeGet
*          cpssDxChBrgFdbActionTriggerModeGet.
*
*       The AA and TA messages may be disabled before the FDB action
*       4. Disable AA and TA messages to CPU by cpssDxChBrgFdbAAandTAToCpuSet.
*
*       5. Set Active configuration by: cpssDxChBrgFdbActionActiveInterfaceSet,
*       cpssDxChBrgFdbActionActiveVlanSet and cpssDxChBrgFdbActionActiveDevSet.
*
*       6. Start triggered action by cpssDxChBrgFdbTrigActionStart
*
*       7. Wait that triggered action is completed by:
*           -  Busy-wait poling of status - cpssDxChBrgFdbTrigActionStatusGet
*           -  Wait of event CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E.
*              This wait may be done in context of dedicated task to restore
*              Active configuration and AA messages configuration.
*
*   for multi-port groups device :
*           the trigger is done on ALL port groups regardless to operation mode
*           (Trigger aging/transplant/delete/upload).
*           see also description of function cpssDxChBrgFdbActionModeSet about
*           'multi-port groups device':
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbActionStart
(
    IN PRV_TGF_FDB_ACTION_MODE_ENT  mode
)
{
    GT_STATUS rc;
#ifdef CHX_FAMILY
    CPSS_FDB_ACTION_MODE_ENT dxChMode;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_FDB_ACTION_MODE_ENT exMxPmMode;
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    switch(mode)
    {
        case PRV_TGF_FDB_ACTION_AGE_WITH_REMOVAL_E:
            dxChMode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;
            break;
        case PRV_TGF_FDB_ACTION_AGE_WITHOUT_REMOVAL_E:
            dxChMode = CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;
            break;
        case PRV_TGF_FDB_ACTION_DELETING_E:
            dxChMode = CPSS_FDB_ACTION_DELETING_E;
            break;
        case PRV_TGF_FDB_ACTION_TRANSPLANTING_E:
            dxChMode = CPSS_FDB_ACTION_TRANSPLANTING_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    APPDEMO_FDB_UPDATE_LOCK();
    /* trigger address deleting */
    rc = cpssDxChBrgFdbTrigActionStart(prvTgfDevNum,dxChMode);
    APPDEMO_FDB_UPDATE_UNLOCK();
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbTriggeredActionStart FAILED, rc = [%d]", rc);
    }

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    switch(mode)
    {
        case PRV_TGF_FDB_ACTION_AGE_WITH_REMOVAL_E:
            exMxPmMode = CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E;
            break;
        case PRV_TGF_FDB_ACTION_AGE_WITHOUT_REMOVAL_E:
            exMxPmMode = CPSS_EXMXPM_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;
            break;
        case PRV_TGF_FDB_ACTION_DELETING_E:
            exMxPmMode = CPSS_EXMXPM_FDB_ACTION_DELETING_E;
            break;
        case PRV_TGF_FDB_ACTION_TRANSPLANTING_E:
            exMxPmMode = CPSS_EXMXPM_FDB_ACTION_TRANSPLANTING_E;
            break;
        case PRV_TGF_FDB_ACTION_FDB_UPLOAD_E:
            exMxPmMode = CPSS_EXMXPM_FDB_ACTION_FDB_UPLOAD_E;
            break;
        default:
            return GT_BAD_PARAM;
    }
    /* trigger address deleting */
    rc = cpssExMxPmBrgFdbTriggeredActionStart(prvTgfDevNum,exMxPmMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbTriggeredActionStart FAILED, rc = [%d]", rc);
    }

    return rc;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgFdbActionDoneWait
*
* DESCRIPTION:
*       wait for the FDB triggered action to end
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       aaTaClosed - indication that the caller closed the AA,TA before the
*                   action -->
*                   GT_TRUE - the caller closed AA,TA before calling
*                               prvTgfBrgFdbActionStart(...)
*                             so action should be ended quickly.
*                             timeout is set to 50 milliseconds
*                   GT_FALSE - the caller NOT closed AA,TA before calling
*                               prvTgfBrgFdbActionStart(...)
*                             so action may take unpredictable time.
*                             timeout is set to 50 seconds !!!
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_TIMEOUT               - after timed out. see timeout definition
*                                  according to aaTaClosed parameter.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbActionDoneWait
(
    IN  GT_BOOL  aaTaClosed
)
{
    GT_STATUS   rc;
    GT_BOOL     isCompleted   = GT_FALSE;
    GT_U32      timeOut = (aaTaClosed == GT_TRUE) ? 2 : 2000;

#ifdef ASIC_SIMULATION
    if(timeOut < 10)
    {
        /* allow simulation minimal extra time */
        timeOut = 10;
    }
#endif /* ASIC_SIMULATION */

    /* wait that triggered action is completed */
    while (GT_TRUE != isCompleted)
    {
        /* get triggered action status */
#ifdef CHX_FAMILY
        rc = cpssDxChBrgFdbTrigActionStatusGet(prvTgfDevNum, &isCompleted);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbTrigActionStatusGet FAILED, rc = [%d]", rc);
            return rc;
        }
#endif /* CHX_FAMILY */
#ifdef EXMXPM_FAMILY
        rc = cpssExMxPmBrgFdbAgingTriggerGet(prvTgfDevNum, &isCompleted);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbAgingTriggerGet FAILED, rc = [%d]", rc);
            return rc;
        }
#endif /* EXMXPM_FAMILY */

        /* allow device to complete the operation*/
        cpssOsTimerWkAfter(25);
        if(0 == (--timeOut))
        {
            return GT_TIMEOUT;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfBrgFdbActionModeSet
*
* DESCRIPTION:
*       Sets FDB action mode without setting Action Trigger
*
* INPUTS:
*       mode - FDB action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbActionModeSet
(
    IN PRV_TGF_FDB_ACTION_MODE_ENT  mode
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
#ifdef CHX_FAMILY
    CPSS_FDB_ACTION_MODE_ENT dxChMode;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_FDB_ACTION_MODE_ENT exMxPmMode;
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    /* convert into device specific format */
    switch(mode)
    {
        case PRV_TGF_FDB_ACTION_AGE_WITH_REMOVAL_E:
            dxChMode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;
            break;
        case PRV_TGF_FDB_ACTION_AGE_WITHOUT_REMOVAL_E:
            dxChMode = CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;
            break;
        case PRV_TGF_FDB_ACTION_DELETING_E:
            dxChMode = CPSS_FDB_ACTION_DELETING_E;
            break;
        case PRV_TGF_FDB_ACTION_TRANSPLANTING_E:
            dxChMode = CPSS_FDB_ACTION_TRANSPLANTING_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChBrgFdbActionModeSet(devNum, dxChMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbActionModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* convert into device specific format */
    switch(mode)
    {
        case PRV_TGF_FDB_ACTION_AGE_WITH_REMOVAL_E:
            exMxPmMode = CPSS_EXMXPM_FDB_ACTION_AGE_WITH_REMOVAL_E;
            break;
        case PRV_TGF_FDB_ACTION_AGE_WITHOUT_REMOVAL_E:
            exMxPmMode = CPSS_EXMXPM_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;
            break;
        case PRV_TGF_FDB_ACTION_DELETING_E:
            exMxPmMode = CPSS_EXMXPM_FDB_ACTION_DELETING_E;
            break;
        case PRV_TGF_FDB_ACTION_TRANSPLANTING_E:
            exMxPmMode = CPSS_EXMXPM_FDB_ACTION_TRANSPLANTING_E;
            break;
        case PRV_TGF_FDB_ACTION_FDB_UPLOAD_E:
            exMxPmMode = CPSS_EXMXPM_FDB_ACTION_FDB_UPLOAD_E;
            break;
        default:
            return GT_BAD_PARAM;
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
        rc = cpssExMxPmBrgFdbActionModeSet(devNum, exMxPmMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbActionModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */

    return rc1;
}

/******************************************************************************
* prvTgfBrgFdbAgingTimeoutSet
*
* DESCRIPTION:
*       Sets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
* INPUTS:
*       timeout - aging time in seconds.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbAgingTimeoutSet
(
    IN GT_U32 timeout
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
        rc = cpssDxChBrgFdbAgingTimeoutSet(devNum, timeout);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbAgingTimeoutSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
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
        rc = cpssExMxPmBrgFdbAgingTimeoutSet(devNum, timeout);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbAgingTimeoutSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */

    return rc1;
}

/*******************************************************************************
* prvTgfBrgFdbAgeBitDaRefreshEnableSet
*
* DESCRIPTION:
*       Enables/disables destination address-based aging. When this bit is
*       set, the aging process is done both on the source and the destination
*       address (i.e. the age bit will be refresh when MAC DA hit occurs, as
*       well as MAC SA hit).
*
* INPUTS:
*       enable - GT_TRUE - enable refreshing
*                GT_FALSE - disable refreshing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbAgeBitDaRefreshEnableSet
(
    IN GT_BOOL enable
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
        rc = cpssDxChBrgFdbAgeBitDaRefreshEnableSet(devNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbAgeBitDaRefreshEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#else /* !CHX_FAMILY */
    TGF_PARAM_NOT_USED(enable);

    return GT_NOT_SUPPORTED;
#endif /* CHX_FAMILY */
}

/*******************************************************************************
* prvTgfBrgFdbMacTriggerModeSet
*
* DESCRIPTION:
*       Sets Mac address table Triggered\Automatic action mode.
*
* INPUTS:
*       mode - action mode:
*                 PRV_TGF_ACT_AUTO_E - Action is done Automatically.
*                 PRV_TGF_ACT_TRIG_E - Action is done via a trigger from CPU.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacTriggerModeSet
(
    IN PRV_TGF_MAC_ACTION_MODE_ENT  mode
)
{
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

#ifdef CHX_FAMILY
    CPSS_MAC_ACTION_MODE_ENT dxChMode;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_FDB_TRIGGER_MODE_ENT exMxPmMode;
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    /* convert into device specific format */
    switch(mode)
    {
        case PRV_TGF_ACT_AUTO_E:
            dxChMode = CPSS_ACT_AUTO_E;
            break;
        case PRV_TGF_ACT_TRIG_E:
            dxChMode = CPSS_ACT_TRIG_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        APPDEMO_FDB_UPDATE_LOCK();
        rc = cpssDxChBrgFdbMacTriggerModeSet(devNum, dxChMode);
        APPDEMO_FDB_UPDATE_UNLOCK();
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbMacTriggerModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* convert into device specific format */
    switch(mode)
    {
        case PRV_TGF_ACT_AUTO_E:
            exMxPmMode = CPSS_EXMXPM_FDB_TRIGGER_AUTO_E;
            break;
        case PRV_TGF_ACT_TRIG_E:
            exMxPmMode = CPSS_EXMXPM_FDB_TRIGGER_TRIG_E;
            break;
        default:
            return GT_BAD_PARAM;
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
        rc = cpssExMxPmBrgFdbTriggerModeSet(devNum, exMxPmMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbTriggerModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */

    return rc1;
}

/*******************************************************************************
* prvTgfBrgFdbActionsEnableSet
*
* DESCRIPTION:
*       Enables/Disables FDB actions.
*
* INPUTS:
*       enable - GT_TRUE -  Actions are enabled
*                GT_FALSE -  Actions are disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbActionsEnableSet
(
    IN GT_BOOL  enable
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
        APPDEMO_FDB_UPDATE_LOCK();
        rc = cpssDxChBrgFdbActionsEnableSet(devNum, enable);
        APPDEMO_FDB_UPDATE_UNLOCK();
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbActionsEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
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
        rc = cpssExMxPmBrgFdbActionsEnableSet(devNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbActionsEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */

    return rc1;
}

/*******************************************************************************
* prvTgfBrgFdbMacEntryStatusGet
*
* DESCRIPTION:
*       Get the Valid and Skip Values of a FDB entry.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryStatusGet
(
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
)
{
    GT_STATUS   rc;
#ifdef CHX_FAMILY
    /* call device specific API */
    APPDEMO_FDB_UPDATE_LOCK();
    rc = utfCpssDxChBrgFdbMacEntryStatusGet(prvTgfDevNum,index,validPtr,skipPtr);
    APPDEMO_FDB_UPDATE_UNLOCK();
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbMacEntryStatusGet FAILED, rc = [%d]", rc);
        return rc;
    }

    return GT_OK;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    rc = cpssExMxPmBrgFdbMacEntryStatusGet(prvTgfDevNum,index,validPtr,skipPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbMacEntryStatusGet FAILED, rc = [%d]", rc);
        return rc;
    }

    return GT_OK;
#endif /*EXMXPM_FAMILY*/

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgFdbMacEntryAgeBitSet
*
* DESCRIPTION:
*       Set age bit in specific FDB entry.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       index - hw mac entry index
*       age   - Age flag that is used for the two-step Aging process.
*               GT_FALSE - The entry will be aged out in the next pass.
*               GT_TRUE - The entry will be aged-out in two age-passes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - index out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryAgeBitSet
(
    IN GT_U32       index,
    IN GT_BOOL      age
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
        APPDEMO_FDB_UPDATE_LOCK();
        rc = utfCpssDxChBrgFdbMacEntryAgeBitSet(devNum, index, age);
        APPDEMO_FDB_UPDATE_UNLOCK();
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbMacEntryAgeBitSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#else /* !CHX_FAMILY */

    TGF_PARAM_NOT_USED(index);
    TGF_PARAM_NOT_USED(age);

    return GT_NOT_SUPPORTED;
#endif /* !CHX_FAMILY */
}



static GT_U32  numOfValid;
static GT_U32  numOfSkip;
static GT_U32  numOfAged;
static GT_U32  numOfStormPrevention;
static GT_BOOL numOfErrors;

/*******************************************************************************
* prvTgfBrgFdbPortGroupCount
*
* DESCRIPTION:
*       This function count number of valid ,skip entries - for specific port group
*
* INPUTS:
*       devNum  - physical device number.
*       portGroupId - port group Id
*           when CPSS_PORT_GROUP_UNAWARE_MODE_CNS meaning read from first port group
* OUTPUTS:
*       numOfValidPtr - (pointer to) number of entries with valid bit set
*                                   (number of valid entries)
*       numOfSkipPtr  - (pointer to) number of entries with skip bit set
*                                   (number of skipped entries)
*                                    entry not counted when valid = 0
*       numOfAgedPtr  - (pointer to) number of entries with age bit = 0 !!!
*                                   (number of aged out entries)
*                                    entry not counted when valid = 0 or skip = 1
*       numOfStormPreventionPtr - (pointer to) number of entries with SP bit set
*                                   (number of SP entries)
*                                    entry not counted when valid = 0 or skip = 1
*       numOfErrorsPtr - (pointer to) number of entries with read error
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbPortGroupCount
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupId,

    OUT GT_U32  *numOfValidPtr,
    OUT GT_U32  *numOfSkipPtr,
    OUT GT_U32  *numOfAgedPtr,
    OUT GT_U32  *numOfStormPreventionPtr,
    OUT GT_U32  *numOfErrorsPtr
)
{
    GT_STATUS  rc;
    GT_U32     hwData[8];
    GT_U16     entryIndex;
    GT_U32     valid;
    GT_U32     skip;
    GT_U32     age;
    GT_U32     spUnknown;
    GT_U32     fdbSize;

    numOfSkip = 0;
    numOfValid = 0;
    numOfAged = 0;
    numOfStormPrevention = 0;
    numOfErrors = 0;

#ifdef CHX_FAMILY
    rc = cpssDxChCfgTableNumEntriesGet(devNum,CPSS_DXCH_CFG_TABLE_FDB_E,&fdbSize);
#endif /*CHX_FAMILY*/
#ifdef EXMXPM_FAMILY
    portGroupId = portGroupId;
    rc = cpssExMxPmCfgTableNumEntriesGet(devNum,CPSS_EXMXPM_CFG_TABLE_FDB_E,&fdbSize);
#endif/*EXMXPM_FAMILY*/

    if(rc != GT_OK)
    {
        return rc;
    }

    for( entryIndex = 0 ; entryIndex < fdbSize; entryIndex++)
    {
#ifdef CHX_FAMILY
        rc = prvCpssDxChPortGroupReadTableEntry(devNum,portGroupId,
                                       PRV_CPSS_DXCH_TABLE_FDB_E,
                                       entryIndex,
                                       &hwData[0]);
#endif /*CHX_FAMILY*/
#ifdef EXMXPM_FAMILY
        rc = prvCpssExMxPmReadTableEntry(devNum,
            PRV_CPSS_EXMXPM_PP_MAC(devNum)->tblsAddr.genericPointers.fdbTablePtr,
            entryIndex,
            &hwData[0]);
#endif/*EXMXPM_FAMILY*/
        if(rc != GT_OK)
        {
            numOfErrors++;
            continue;
        }

        valid  = U32_GET_FIELD_MAC(hwData[0],0,1);
        skip   = U32_GET_FIELD_MAC(hwData[0],1,1);
        age    = U32_GET_FIELD_MAC(hwData[0],2,1);
        spUnknown = U32_GET_FIELD_MAC(hwData[3],2,1);

        if (valid == 0)
        {
            continue;
        }

        numOfValid++;

        if(skip)
        {
            numOfSkip++;
            continue;
        }

        if (age == 0x0)
        {
            numOfAged++;
        }

        if(spUnknown)
        {
            numOfStormPrevention++;
        }
    }

    if(numOfValidPtr)
        *numOfValidPtr = numOfValid;

    if(numOfSkipPtr)
        *numOfSkipPtr = numOfSkip;

    if(numOfAgedPtr)
        *numOfAgedPtr = numOfAged;

    if(numOfStormPreventionPtr)
        *numOfStormPreventionPtr = numOfStormPrevention;

    if(numOfErrorsPtr)
        *numOfErrorsPtr = numOfErrors;

    return GT_OK;
}

/*******************************************************************************
* prvTgfBrgFdbCount
*
* DESCRIPTION:
*       This function count number of valid ,skip entries.
*
*       in multi ports port groups device the function summarize the entries
*       from all the port groups (don't care about duplications)
*
* INPUTS:
*       None
*
* OUTPUTS:
*       numOfValidPtr - (pointer to) number of entries with valid bit set
*                                   (number of valid entries)
*       numOfSkipPtr  - (pointer to) number of entries with skip bit set
*                                   (number of skipped entries)
*                                    entry not counted when valid = 0
*       numOfAgedPtr  - (pointer to) number of entries with age bit = 0 !!!
*                                   (number of aged out entries)
*                                    entry not counted when valid = 0 or skip = 1
*       numOfStormPreventionPtr - (pointer to) number of entries with SP bit set
*                                   (number of SP entries)
*                                    entry not counted when valid = 0 or skip = 1
*       numOfErrorsPtr - (pointer to) number of entries with read error
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbCount
(
    OUT GT_U32  *numOfValidPtr,
    OUT GT_U32  *numOfSkipPtr,
    OUT GT_U32  *numOfAgedPtr,
    OUT GT_U32  *numOfStormPreventionPtr,
    OUT GT_U32  *numOfErrorsPtr
)
{
    GT_STATUS rc;
    GT_U32     portGroupId;
    GT_U32  tmpNumOfSkip = 0;
    GT_U32  tmpNumOfValid = 0;
    GT_U32  tmpNumOfAged = 0;
    GT_U32  tmpNumOfStormPrevention = 0;
    GT_BOOL tmpNumOfErrors = 0;


    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(prvTgfDevNum,portGroupId)
    {
        rc  = prvTgfBrgFdbPortGroupCount(prvTgfDevNum,portGroupId,
        NULL,NULL,NULL,NULL,NULL);
        if(rc != GT_OK)
        {
            return rc;
        }

        tmpNumOfSkip             += numOfSkip;
        tmpNumOfValid            += numOfValid;
        tmpNumOfAged             += numOfAged;
        tmpNumOfStormPrevention  += numOfStormPrevention;
        tmpNumOfErrors           += numOfErrors;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(prvTgfDevNum,portGroupId)

    if(numOfValidPtr)
        *numOfValidPtr = tmpNumOfValid;

    if(numOfSkipPtr)
        *numOfSkipPtr = tmpNumOfSkip;

    if(numOfAgedPtr)
        *numOfAgedPtr = tmpNumOfAged;

    if(numOfStormPreventionPtr)
        *numOfStormPreventionPtr = tmpNumOfStormPrevention;

    if(numOfErrorsPtr)
        *numOfErrorsPtr = tmpNumOfErrors;

    return GT_OK;
}

/*******************************************************************************
* prvTgfConvertCpssToGenericMacEntry
*
* DESCRIPTION:
*       Convert device specific MAC entry into generic MAC entry
*
* INPUTS:
*       cpssMacEntryPtr - (pointer to) cpss MAC entry parameters
*
* OUTPUTS:
*       macEntryPtr - (pointer to) MAC entry parameters
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvTgfConvertCpssToGenericMacEntry
(
    IN  CPSS_MAC_ENTRY_EXT_STC       *cpssMacEntryPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
)
{
    GT_STATUS   rc;
    /* convert entry type from device specific format */
    switch (cpssMacEntryPtr->key.entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV4_MCAST_E;
            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
            macEntryPtr->key.entryType = PRV_TGF_FDB_ENTRY_TYPE_IPV6_MCAST_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert key data from device specific format */
    if (CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E == cpssMacEntryPtr->key.entryType)
    {
        cpssOsMemCpy(macEntryPtr->key.key.macVlan.macAddr.arEther,
                     cpssMacEntryPtr->key.key.macVlan.macAddr.arEther,
                     sizeof(cpssMacEntryPtr->key.key.macVlan.macAddr));
        macEntryPtr->key.key.macVlan.vlanId = cpssMacEntryPtr->key.key.macVlan.vlanId;
        macEntryPtr->key.key.macVlan.fId = macEntryPtr->key.key.macVlan.vlanId;
    }
    else
    {
        cpssOsMemCpy(macEntryPtr->key.key.ipMcast.sip,
                     cpssMacEntryPtr->key.key.ipMcast.sip,
                     sizeof(cpssMacEntryPtr->key.key.ipMcast.sip));
        cpssOsMemCpy(macEntryPtr->key.key.ipMcast.dip,
                     cpssMacEntryPtr->key.key.ipMcast.dip,
                     sizeof(cpssMacEntryPtr->key.key.ipMcast.dip));
        macEntryPtr->key.key.ipMcast.vlanId = cpssMacEntryPtr->key.key.ipMcast.vlanId;
        macEntryPtr->key.key.ipMcast.fId = macEntryPtr->key.key.ipMcast.vlanId;
    }

    /* convert interface info from device specific format */
    macEntryPtr->dstInterface.type = cpssMacEntryPtr->dstInterface.type;
    switch (cpssMacEntryPtr->dstInterface.type)
    {
        case CPSS_INTERFACE_PORT_E:
            rc = prvUtfSwFromHwDeviceNumberGet(cpssMacEntryPtr->dstInterface.devPort.devNum,
                                               &(macEntryPtr->dstInterface.devPort.devNum));
            if(GT_OK != rc)
                return rc;
            macEntryPtr->dstInterface.devPort.portNum = cpssMacEntryPtr->dstInterface.devPort.portNum;
            break;

        case CPSS_INTERFACE_TRUNK_E:
            macEntryPtr->dstInterface.trunkId = cpssMacEntryPtr->dstInterface.trunkId;
            break;

        case CPSS_INTERFACE_VIDX_E:
            macEntryPtr->dstInterface.vidx = cpssMacEntryPtr->dstInterface.vidx;
            break;

        case CPSS_INTERFACE_VID_E:
            macEntryPtr->dstInterface.vlanId = cpssMacEntryPtr->dstInterface.vlanId;
            break;

        case CPSS_INTERFACE_DEVICE_E:
            macEntryPtr->dstInterface.devNum = cpssMacEntryPtr->dstInterface.devNum;
            break;

        case CPSS_INTERFACE_FABRIC_VIDX_E:
            macEntryPtr->dstInterface.fabricVidx = cpssMacEntryPtr->dstInterface.fabricVidx;
            break;

        default:
            return GT_BAD_PARAM;
    }

    macEntryPtr->sourceId = cpssMacEntryPtr->sourceID;

    /* convert daCommand info from device specific format */
    switch (cpssMacEntryPtr->daCommand)
    {
        case CPSS_MAC_TABLE_FRWRD_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_FORWARD_E;
            break;

        case CPSS_MAC_TABLE_DROP_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_DROP_HARD_E;
            break;

        case CPSS_MAC_TABLE_INTERV_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_INTERV_E;
            break;

        case CPSS_MAC_TABLE_CNTL_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_CNTL_E;
            break;

        case CPSS_MAC_TABLE_MIRROR_TO_CPU_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E;
            break;

        case CPSS_MAC_TABLE_SOFT_DROP_E:
            macEntryPtr->daCommand = PRV_TGF_PACKET_CMD_DROP_SOFT_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert saCommand info from device specific format */
    switch (cpssMacEntryPtr->saCommand)
    {
        case CPSS_MAC_TABLE_FRWRD_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_FORWARD_E;
            break;

        case CPSS_MAC_TABLE_DROP_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_DROP_HARD_E;
            break;

        case CPSS_MAC_TABLE_INTERV_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_INTERV_E;
            break;

        case CPSS_MAC_TABLE_CNTL_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_CNTL_E;
            break;

        case CPSS_MAC_TABLE_MIRROR_TO_CPU_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_MIRROR_TO_CPU_E;
            break;

        case CPSS_MAC_TABLE_SOFT_DROP_E:
            macEntryPtr->saCommand = PRV_TGF_PACKET_CMD_DROP_SOFT_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* convert MAC entry info from device specific format */
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, isStatic);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, daRoute);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, mirrorToRxAnalyzerPortEn);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, userDefined);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, daQosIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, saQosIndex);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, daSecurityLevel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, saSecurityLevel);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, appSpecificCpuCode);
    PRV_TGF_STC_D2S_FIELD_COPY_MAC(cpssMacEntryPtr, macEntryPtr, spUnknown);

    return GT_OK;
}


/* max number of messages to store */
#define MAX_MESSAGE_NUM_CNS     50

typedef struct{
    GT_BOOL valid;
    GT_U8   device;
    CPSS_MAC_UPDATE_MSG_EXT_STC message;
}BRG_FDB_AUQ_MESSAGE_INFO;
/* array of validation flag for the messages */
static BRG_FDB_AUQ_MESSAGE_INFO  brgFdbAuqMessagesArr[MAX_MESSAGE_NUM_CNS]={{0,0,{0}}};

/*******************************************************************************
* brgFdbAuqMessageHandler
*
* DESCRIPTION:
*       the function handle AUQ messages , currently only QR needed.
*       NOTE: the function that is called from the appDemo when appdemo receive
*       FDB AUQ message
*
* INPUTS:
*      dev              -   device number
*      auMessagesPtr    -   pointer to the AUQ message
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FULL         - array full
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS brgFdbAuqMessageHandler
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagesPtr
)
{

    GT_U32  ii;
    BRG_FDB_AUQ_MESSAGE_INFO *currInfoPtr;/* pointer to current info */

    currInfoPtr = &brgFdbAuqMessagesArr[0];

    for(ii = 0 ; ii < MAX_MESSAGE_NUM_CNS; ii++,currInfoPtr++)
    {
        if(currInfoPtr->valid == GT_TRUE)
        {
            continue;
        }

        currInfoPtr->device = devNum;
        currInfoPtr->message = *auMessagesPtr;

        currInfoPtr->valid = GT_TRUE;

        return GT_OK;
    }

    /* not found empty place in array */
    return GT_FULL;
}

/*******************************************************************************
* brgFdbAuqMessageQrWait
*
* DESCRIPTION:
*       the function wait for QR (query response)
*
* INPUTS:
*       devNum - device number
*       searchKeyPtr - (pointer to) the search key (mac+vlan)
* OUTPUTS:
*       indexPtr    - (pointer to) index
*                       when NULL --> ignored
*       agedPtr     - (pointer to) is entry aged
*                       when NULL --> ignored
*       devNumPtr   - (pointer to) is device number associated with the entry
*                       when NULL --> ignored
*       macEntryPtr - (pointer to) extended Mac table entry
*                       when NULL --> ignored
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - entry not found
*       GT_TIMEOUT      - on timeout waiting for the QR
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS brgFdbAuqMessageQrWait
(
    IN  GT_U8                         devNum,
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *searchKeyPtr,
    OUT GT_U32                       *indexPtr,
    OUT GT_BOOL                      *agedPtr,
    OUT GT_U8                        *devNumPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
)
{
    GT_STATUS   rc;
    GT_U32  ii;
    BRG_FDB_AUQ_MESSAGE_INFO *currInfoPtr;/* pointer to current info */
    GT_U32  maxRetry = 20;/* 1 seconds */
    GT_U32  index;/* hash index */
    PRV_TGF_BRG_MAC_ENTRY_STC macEntry;

    while(maxRetry--)
    {
        currInfoPtr = &brgFdbAuqMessagesArr[0];

        for(ii = 0 ; ii < MAX_MESSAGE_NUM_CNS; ii++,currInfoPtr++)
        {
            if(currInfoPtr->valid == GT_FALSE)
            {
                continue;
            }
            /* check that this is QR message */
            if(currInfoPtr->message.updType != CPSS_QR_E ||
               devNum !=  currInfoPtr->device)
            {
                continue;
            }

            /* convert cpss to TGF format */
            rc = prvTgfConvertCpssToGenericMacEntry(&currInfoPtr->message.macEntry,&macEntry);
            if(rc != GT_OK)
            {
                return rc;
            }

            if(0 != prvTgfBrgFdbMacKeyCompare(&macEntry.key,searchKeyPtr))
            {
                /* response to other query */
                continue;
            }

            /* state that we are done with this message */
            currInfoPtr->valid = GT_FALSE;

            if(currInfoPtr->message.entryWasFound == GT_FALSE)
            {
                return GT_NOT_FOUND;
            }

            if(indexPtr || macEntryPtr)
            {
                /* we have found the QR */
                if(indexPtr)
                {
                    /* get the hash index */
                    rc = prvTgfBrgFdbMacEntryHashCalc(&macEntry.key,&index);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }

                    *indexPtr = index + currInfoPtr->message.macEntryIndex;
                }

                if(macEntryPtr)
                {
                    *macEntryPtr = macEntry;
                }
            }


            if(agedPtr)
            {
                *agedPtr = currInfoPtr->message.skip;
            }

            if(devNumPtr)
            {
                *devNumPtr = currInfoPtr->message.associatedDevNum;
            }

            return GT_OK;
        }

        /* wait for message */
        cpssOsTimerWkAfter(50);
    }

    return GT_TIMEOUT;
}

/*******************************************************************************
* prvTgfBrgFdbMacCompare
*
* DESCRIPTION:
*       compare 2 mac entries.
*
* INPUTS:
*       mac1Ptr - (pointer to) mac 1
*       mac2Ptr - (pointer to) mac 2
*
* OUTPUTS:
*       > 0  - if mac1Ptr is  bigger than mac2Ptr
*       == 0 - if mac1Ptr is equal to mac2Ptr
*       < 0  - if mac1Ptr is smaller than mac2Ptr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - entry not found
*       GT_TIMEOUT      - on timeout waiting for the QR
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
int prvTgfBrgFdbMacCompare
(
    IN  PRV_TGF_BRG_MAC_ENTRY_STC    *mac1Ptr,
    IN  PRV_TGF_BRG_MAC_ENTRY_STC    *mac2Ptr
)
{
    GT_STATUS   rc;

#ifdef CHX_FAMILY
    CPSS_MAC_ENTRY_EXT_STC  dxChMacEntry1,dxChMacEntry2;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_FDB_ENTRY_STC exMxPmMacEntry1,exMxPmMacEntry2;
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToDxChMacEntry(mac1Ptr, &dxChMacEntry1);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMacEntry FAILED, rc = [%d]", rc);
        return rc;  /* value is not 0 (GT_OK) */
    }
    rc = prvTgfConvertGenericToDxChMacEntry(mac2Ptr, &dxChMacEntry2);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMacEntry FAILED, rc = [%d]", rc);
        return rc;  /* value is not 0 (GT_OK) */
    }

    return cpssOsMemCmp(&dxChMacEntry1,&dxChMacEntry2,sizeof(dxChMacEntry2));
#endif /*CHX_FAMILY*/

#ifdef EXMXPM_FAMILY
    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToExMxPmMacEntry(mac1Ptr, &exMxPmMacEntry1);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmMacEntry FAILED, rc = [%d]", rc);

        return rc;  /* value is not 0 (GT_OK) */
    }

    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToExMxPmMacEntry(mac2Ptr, &exMxPmMacEntry2);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmMacEntryKey FAILED, rc = [%d]", rc);

        return rc;  /* value is not 0 (GT_OK) */
    }

    return cpssOsMemCmp(&exMxPmMacEntry1,&exMxPmMacEntry2,sizeof(exMxPmMacEntry2));

#endif /* EXMXPM_FAMILY */

}

/*******************************************************************************
* prvTgfBrgFdbMacKeyCompare
*
* DESCRIPTION:
*       compare 2 mac keys.
*
* INPUTS:
*       key1Ptr - (pointer to) key 1
*       key2Ptr - (pointer to) key 2
*
* OUTPUTS:
*       > 0  - if key1Ptr is  bigger than key2Ptr
*       == 0 - if key1Ptr is equal to str2
*       < 0  - if key1Ptr is smaller than key2Ptr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - entry not found
*       GT_TIMEOUT      - on timeout waiting for the QR
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
int prvTgfBrgFdbMacKeyCompare
(
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *key1Ptr,
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *key2Ptr
)
{
    GT_STATUS   rc;

#ifdef CHX_FAMILY
    CPSS_MAC_ENTRY_EXT_KEY_STC  dxChMacEntryKey1,dxChMacEntryKey2;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC exMxPmMacEntryKey1,exMxPmMacEntryKey2;
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToDxChMacEntryKey(key1Ptr, &dxChMacEntryKey1);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMacEntryKey FAILED, rc = [%d]", rc);
        return rc;  /* value is not 0 (GT_OK) */
    }
    rc = prvTgfConvertGenericToDxChMacEntryKey(key2Ptr, &dxChMacEntryKey2);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMacEntryKey FAILED, rc = [%d]", rc);
        return rc;  /* value is not 0 (GT_OK) */
    }

    return cpssOsMemCmp(&dxChMacEntryKey1,&dxChMacEntryKey2,sizeof(dxChMacEntryKey2));
#endif /*CHX_FAMILY*/

#ifdef EXMXPM_FAMILY
    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToExMxPmMacEntryKey(key1Ptr, &exMxPmMacEntryKey1);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmMacEntryKey FAILED, rc = [%d]", rc);

        return rc;  /* value is not 0 (GT_OK) */
    }

    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToExMxPmMacEntryKey(key2Ptr, &exMxPmMacEntryKey2);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmMacEntryKey FAILED, rc = [%d]", rc);

        return rc;  /* value is not 0 (GT_OK) */
    }

    return cpssOsMemCmp(&exMxPmMacEntryKey1,&exMxPmMacEntryKey2,sizeof(exMxPmMacEntryKey2));

#endif /* EXMXPM_FAMILY */

}

/*******************************************************************************
* prvTgfBrgFdbMacEntryHashCalc
*
* DESCRIPTION:
*       calculate the hash index for the key
*
*       NOTE:
*       see also function prvTgfBrgFdbMacEntryIndexFind(...) that find actual
*       place according to actual HW capacity
*
* INPUTS:
*       macEntryKeyPtr - (pointer to) mac entry key
*
* OUTPUTS:
*       indexPtr - (pointer to) index
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryHashCalc
(
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *macEntryKeyPtr,
    OUT GT_U32                       *indexPtr
)
{
    GT_STATUS   rc = GT_OK;
#ifdef CHX_FAMILY
    CPSS_MAC_ENTRY_EXT_KEY_STC  dxChMacEntryKey;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC exMxPmMacEntryKey;
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToDxChMacEntryKey(macEntryKeyPtr, &dxChMacEntryKey);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChMacEntryKey FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssDxChBrgFdbHashCalc(prvTgfDevNum, &dxChMacEntryKey, indexPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbHashCalc FAILED, rc = [%d]", rc);

        return rc;
    }
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* convert key data into device specific format */
    rc = prvTgfConvertGenericToExMxPmMacEntryKey(macEntryKeyPtr, &exMxPmMacEntryKey);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmMacEntryKey FAILED, rc = [%d]", rc);

        return rc;
    }

    /* call device specific API */
    rc = cpssExMxPmBrgFdbInternalHashCalc(prvTgfDevNum, &exMxPmMacEntryKey, indexPtr);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbInternalHashCalc FAILED, rc = [%d]", rc);

        return rc;
    }
#endif /* EXMXPM_FAMILY */

    return rc;
}

/*******************************************************************************
* prvTgfBrgFdbMacEntryFind
*
* DESCRIPTION:
*       scan the FDB table and find entry with same mac+vlan
*       entry MUST be valid and non-skipped
*
* INPUTS:
*       searchKeyPtr - (pointer to) the search key (mac+vlan)
* OUTPUTS:
*       indexPtr    - (pointer to) index
*                       when NULL --> ignored
*       agedPtr     - (pointer to) is entry aged
*                       when NULL --> ignored
*       devNumPtr   - (pointer to) is device number associated with the entry
*                       when NULL --> ignored
*       macEntryPtr - (pointer to) extended Mac table entry
*                       when NULL --> ignored
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - entry not found
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbMacEntryFind
(
    IN  PRV_TGF_MAC_ENTRY_KEY_STC    *searchKeyPtr,
    OUT GT_U32                       *indexPtr,
    OUT GT_BOOL                      *agePtr,
    OUT GT_U8                        *devNumPtr,
    OUT PRV_TGF_BRG_MAC_ENTRY_STC    *macEntryPtr
)
{
    GT_STATUS  rc;
    GT_U32     entryIndex;
    GT_BOOL    valid;
    GT_BOOL    skip;
    GT_BOOL    age;
    PRV_TGF_BRG_MAC_ENTRY_STC  macEntry;
    GT_U8   associatedDevNum;
#ifdef CHX_FAMILY
    CPSS_MAC_ENTRY_EXT_KEY_STC dxCpssKey;
#endif /*CHX_FAMILY*/
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC pmCpssKey;
#endif/*EXMXPM_FAMILY*/

    if(prvTgfBrgFdbAccessMode == PRV_TGF_BRG_FDB_ACCESS_MODE_BY_MESSAGE_E)
    {
        /* register with the AppDemo to receive QR messages */
        rc = appDemoBrgFdbAuqCbRegister(CPSS_QR_E,brgFdbAuqMessageHandler);
        if(rc != GT_OK)
        {
            return rc;
        }

#ifdef CHX_FAMILY
        rc = prvTgfConvertGenericToDxChMacEntryKey(searchKeyPtr,&dxCpssKey);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = utfCpssDxChBrgFdbQaSend(prvTgfDevNum,&dxCpssKey);
        if(rc != GT_OK)
        {
            return rc;
        }
#endif /*CHX_FAMILY*/
#ifdef EXMXPM_FAMILY
        rc = prvTgfConvertGenericToExMxPmMacEntryKey(searchKeyPtr,&pmCpssKey);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssExMxPmBrgFdbQaSend(prvTgfDevNum,&pmCpssKey);
        if(rc != GT_OK)
        {
            return rc;
        }
#endif/*EXMXPM_FAMILY*/

        /* wait for the info from the AppDemo */
        rc = brgFdbAuqMessageQrWait(prvTgfDevNum,searchKeyPtr,indexPtr,agePtr,devNumPtr,macEntryPtr);

        /* unbind the CB function */
        (void)appDemoBrgFdbAuqCbRegister(CPSS_QR_E,NULL);

        return rc;
    }

    rc = prvTgfBrgFdbMacEntryIndexFind(searchKeyPtr,&entryIndex);
    if(rc != GT_OK)
    {
        return rc;
    }
    /***************/
    /* entry found */
    /***************/

    /* read MAC entry */
    rc = prvTgfBrgFdbMacEntryRead(entryIndex, &valid, &skip, &age, &associatedDevNum, &macEntry);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfBrgFdbMacEntryRead FAILED, rc = [%d]", rc);

        return rc;
    }

    if(indexPtr)
    {
        *indexPtr = entryIndex;
    }

    if(agePtr)
    {
        *agePtr = age;
    }

    if(indexPtr)
    {
        *indexPtr = entryIndex;
    }

    if(devNumPtr)
    {
        *devNumPtr = associatedDevNum;
    }

    if(macEntryPtr)
    {
        *macEntryPtr = macEntry;
    }

    return GT_OK;

}


/*******************************************************************************
* prvTgfBrgFdbAccessModeSet
*
* DESCRIPTION:
*       Set access mode to the FDB : by message or by index
*
* INPUTS:
*       mode - access mode : by message or by index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       previous state
*
* COMMENTS:
*
*******************************************************************************/
PRV_TGF_BRG_FDB_ACCESS_MODE_ENT prvTgfBrgFdbAccessModeSet
(
    IN PRV_TGF_BRG_FDB_ACCESS_MODE_ENT  mode
)
{
    PRV_TGF_BRG_FDB_ACCESS_MODE_ENT oldMode = prvTgfBrgFdbAccessMode;
    switch(mode)
    {
        case  PRV_TGF_BRG_FDB_ACCESS_MODE_BY_MESSAGE_E:
        case  PRV_TGF_BRG_FDB_ACCESS_MODE_BY_INDEX_E:
            break;
        default:
            /*error*/
            return (PRV_TGF_BRG_FDB_ACCESS_MODE_ENT)(0x7FFFFFFF);
    }

    prvTgfBrgFdbAccessMode = mode;

    return oldMode;
}

/*******************************************************************************
* prvTgfBrgVlanLearningStateSet
*
* DESCRIPTION:
*       Sets state of VLAN based learning to specified VLAN on specified device
*
* INPUTS:
*       vlanId - vlan Id
*       status - GT_TRUE for enable  or GT_FALSE otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum or vid
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanLearningStateSet
(
    IN GT_U16                         vlanId,
    IN GT_BOOL                        status
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
        rc = cpssDxChBrgVlanLearningStateSet(devNum, vlanId, status);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanLearningStateSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    TGF_PARAM_NOT_USED(vlanId);
    TGF_PARAM_NOT_USED(status);

    return GT_OK;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgMcMemberAdd
*
* DESCRIPTION:
*       Add new port member to the Multicast Group entry
*
* INPUTS:
*       devNum  - PP's device number
*       vidx    - multicast group index
*       portNum - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong devNum
*       GT_OUT_OF_RANGE - if vidx is larger than the allowed value
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgMcMemberAdd
(
    IN GT_U8                          devNum,
    IN GT_U16                         vidx,
    IN GT_U8                          portNum
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgMcMemberAdd(devNum, vidx, portNum);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmBrgMcMemberAdd(devNum, vidx, portNum);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgMcEntryWrite
*
* DESCRIPTION:
*       Writes Multicast Group entry to the HW
*
* INPUTS:
*       devNum        - PP's device number
*       vidx          - multicast group index
*       portBitmapPtr - pointer to the bitmap of ports
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or ports bitmap value.
*       GT_OUT_OF_RANGE - if vidx is larger than the allowed value.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgMcEntryWrite
(
    IN GT_U8                          devNum,
    IN GT_U16                         vidx,
    IN CPSS_PORTS_BMP_STC            *portBitmapPtr
)
{
    GT_U32  ii;
    PRV_TGF_MEMBER_FORCE_INFO_STC *currMemPtr;
    CPSS_PORTS_BMP_STC            tmpPortsMembers;

    if(portBitmapPtr)
    {
        tmpPortsMembers = *portBitmapPtr;
    }
    else
    {
        cpssOsMemSet(&tmpPortsMembers,0,sizeof(tmpPortsMembers));
    }

    /* look for this member in the DB */
    currMemPtr = &prvTgfDevPortForceArray[0];
    for(ii = 0 ; ii < prvTgfDevPortForceNum; ii++,currMemPtr++)
    {
        if(devNum != currMemPtr->member.devNum)
        {
            continue;
        }

        if(currMemPtr->forceToVidx == GT_FALSE)
        {
            /* member not need to be forced to any vlan */
            continue;
        }

        if(currMemPtr->member.portNum >= CPSS_MAX_PORTS_NUM_CNS)
        {
            return GT_BAD_PARAM;
        }

        /* set the member */
        CPSS_PORTS_BMP_PORT_SET_MAC(&tmpPortsMembers,currMemPtr->member.portNum);
    }


#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgMcEntryWrite(devNum, vidx, &tmpPortsMembers);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmBrgMcEntryWrite(devNum, vidx, GT_FALSE, &tmpPortsMembers);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgMcEntryRead
*
* DESCRIPTION:
*       Reads the Multicast Group entry from the HW
*
* INPUTS:
*       devNum - PP's device number.
*       vidx   - multicast group index
*
* OUTPUTS:
*       portBitmapPtr   - pointer to the bitmap of ports
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_OUT_OF_RANGE - if vidx is larger than the allowed value.
*       GT_TIMEOUT      - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgMcEntryRead
(
    IN  GT_U8                         devNum,
    IN  GT_U16                        vidx,
    OUT CPSS_PORTS_BMP_STC           *portBitmapPtr
)
{
#ifdef EXMXPM_FAMILY
    GT_BOOL     isCpuMember = GT_FALSE;
#endif /* EXMXPM_FAMILY */

#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgMcEntryRead(devNum, vidx, portBitmapPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmBrgMcEntryRead(devNum, vidx, &isCpuMember, portBitmapPtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgVlanEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable VLAN Egress Filtering on specified device for Bridged
*       Known Unicast packets
*
* INPUTS:
*       enable - enable\disable VLAN egress filtering
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong dev
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanEgressFilteringEnable
(
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
        /* call device specific API */
        rc = cpssDxChBrgVlanEgressFilteringEnable(devNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanEgressFilteringEnable FAILED, rc = [%d]", rc);

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
        rc = cpssExMxPmBrgEgressFilteringVlanEnableSet(devNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgEgressFilteringVlanEnableSet FAILED, rc = [%d]", rc);

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
* prvTgfBrgVlanEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the VLAN Egress Filtering current state (enable/disable)
*      on specified device for Bridged Known Unicast packets.
*      If enabled the VLAN egress filter verifies that the egress port is a
*      member of the VID assigned to the packet.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - points to (enable/disable) bridged known unicast packets filtering
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong dev or port
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanEgressFilteringEnableGet
(
    IN  GT_U8                         devNum,
    OUT GT_BOOL                      *enablePtr
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgVlanEgressFilteringEnableGet(devNum, enablePtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmBrgEgressFilteringVlanEnableGet(devNum, enablePtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgCntDropCntrModeSet
*
* DESCRIPTION:
*       Sets Drop Counter Mode
*
* INPUTS:
*       dropMode - Drop Counter mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on HW error
*       GT_BAD_PARAM     - on wrong devNum or dropMode
*       GT_NOT_SUPPORTED - on non-supported dropMode
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfBrgCntDropCntrModeSet
(
    IN  PRV_TGF_BRG_DROP_CNTR_MODE_ENT dropMode
)
{
    GT_U8       devNum = 0;
    GT_STATUS   rc, rc1 = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT dxChDropMode;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_BRG_DROP_CNTR_MODE_ENT exMxPmDropMode;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* convert drop count mode into device specific format */
    rc = prvTgfConvertGenericToDxChDropCntMode(dropMode, &dxChDropMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToDxChDropCntMode FAILED, rc = [%d]", rc);

        return rc;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChBrgCntDropCntrModeSet(devNum, dxChDropMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgCntDropCntrModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* convert drop count mode into device specific format */
    rc = prvTgfConvertGenericToExMxPmDropCntMode(dropMode, &exMxPmDropMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfConvertGenericToExMxPmDropCntMode FAILED, rc = [%d]", rc);

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
        rc = cpssExMxPmBrgCntDropCntrModeSet(devNum, (CPSS_EXMXPM_BRG_DROP_CNTR_MODE_ENT) dropMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgCntDropCntrModeSet FAILED, rc = [%d]", rc);

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
* prvTgfBrgCntDropCntrModeGet
*
* DESCRIPTION:
*       Gets the Drop Counter Mode
*
* INPUTS:
*       None
*
* OUTPUTS:
*       dropModePtr - (pointer to) the Drop Counter mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on HW error
*       GT_BAD_PARAM     - on wrong devNum
*       GT_BAD_PTR       - on NULL pointer
*       GT_NOT_SUPPORTED - on non-supported dropMode
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfBrgCntDropCntrModeGet
(
    OUT  PRV_TGF_BRG_DROP_CNTR_MODE_ENT *dropModePtr
)
{
    GT_STATUS   rc = GT_OK;
#ifdef CHX_FAMILY
    CPSS_DXCH_BRIDGE_DROP_CNTR_MODE_ENT dxChDropMode;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_BRG_DROP_CNTR_MODE_ENT exMxPmDropMode;
#endif /* EXMXPM_FAMILY */


#ifdef CHX_FAMILY
    /* call device specific API */
    rc = cpssDxChBrgCntDropCntrModeGet(prvTgfDevNum, &dxChDropMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgCntDropCntrModeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert drop count mode from device specific format */
    return prvTgfConvertDxChToGenericDropCntMode(dxChDropMode, dropModePtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    rc = cpssExMxPmBrgCntDropCntrModeGet(prvTgfDevNum, &exMxPmDropMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgCntDropCntrModeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* convert drop count mode from device specific format */
    return prvTgfConvertExMxPmToGenericDropCntMode(exMxPmDropMode, dropModePtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgVlanPortIngFltEnable
*
* DESCRIPTION:
*       Enable/disable Ingress Filtering for specific port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - enable\disable ingress filtering
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum or port
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortIngFltEnable
(
    IN GT_U8                          devNum,
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgVlanPortIngFltEnable(devNum, portNum, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmBrgVlanPortIngressFilterEnableSet(devNum, portNum, enable);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgCntDropCntrSet
*
* DESCRIPTION:
*       Sets the Bridge Ingress Drop Counter of specified device
*
* INPUTS:
*       dropCnt - the number of packets that were dropped
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on HW error
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgCntDropCntrSet
(
    IN  GT_U32                        dropCnt
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
        rc = cpssDxChBrgCntDropCntrSet(devNum, dropCnt);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgCntDropCntrSet FAILED, rc = [%d]", rc);

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
        rc = cpssExMxPmBrgCntDropCntrSet(devNum, dropCnt);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgCntDropCntrSet FAILED, rc = [%d]", rc);

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
* prvTgfBrgCntDropCntrGet
*
* DESCRIPTION:
*       Gets the Bridge Ingress Drop Counter of specified device
*
* INPUTS:
*       None
*
* OUTPUTS:
*       dropCntPtr - (pointer to) the number of packets that were dropped
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on HW error
*       GT_BAD_PARAM - on wrong devNum
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgCntDropCntrGet
(
    OUT GT_U32                       *dropCntPtr
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgCntDropCntrGet(prvTgfDevNum, dropCntPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmBrgCntDropCntrGet(prvTgfDevNum, dropCntPtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgVlanLocalSwitchingEnableSet
*
* DESCRIPTION:
*       Function sets local switching of Multicast, known and unknown Unicast,
*       and Broadcast traffic per VLAN.
*
* INPUTS:
*       vlanId      - VLAN id
*       trafficType - local switching traffic type
*       enable      - enable/disable of local switching
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong vlanId or trafficType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanLocalSwitchingEnableSet
(
    IN  GT_U16                                              vlanId,
    IN  PRV_TGF_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT   trafficType,
    IN  GT_BOOL                                             enable
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
    CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT dxChTrafficType;


    /* convert trafficType into device specific format */
    switch (trafficType)
    {
        case PRV_TGF_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E:
            dxChTrafficType = CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E;
            break;

        case PRV_TGF_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E:
            dxChTrafficType = CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChBrgVlanLocalSwitchingEnableSet(devNum, vlanId, dxChTrafficType, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanLocalSwitchingEnableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(vlanId);
    TGF_PARAM_NOT_USED(trafficType);
    TGF_PARAM_NOT_USED(enable);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgGenUcLocalSwitchingEnable
*
* DESCRIPTION:
*       Enable/disable local switching back through the ingress interface
*       for for known Unicast packets
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_FALSE = Unicast packets whose bridging decision is to be
*                            forwarded back to its Ingress port or trunk
*                            are assigned with a soft drop command.
*                 GT_TRUE  = Unicast packets whose bridging decision is to be
*                            forwarded back to its Ingress port or trunk may be
                             forwarded back to their source.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on on bad device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenUcLocalSwitchingEnable
(
    IN GT_U8                          devNum,
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgGenUcLocalSwitchingEnable(devNum, portNum, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(enable);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgPortEgressMcLocalEnable
*
* DESCRIPTION:
*       Enable/Disable sending Multicast packets back to its source
*       port on the local device.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - Boolean value:
*                 GT_TRUE  - Multicast packets may be sent back to
*                            their source port on the local device.
*                 GT_FALSE - Multicast packets are not sent back to
*                            their source port on the local device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgPortEgressMcLocalEnable
(
    IN GT_U8                          devNum,
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgPortEgressMcastLocalEnable(devNum, portNum, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(enable);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgVlanRangeSet
*
* DESCRIPTION:
*       Function configures the valid VLAN Range
*
* INPUTS:
*       vidRange - VID range for VLAN filtering (0 - 4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameters
*       GT_OUT_OF_RANGE          - illegal vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanRangeSet
(
    IN  GT_U16                        vidRange
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
        rc = cpssDxChBrgVlanRangeSet(devNum, vidRange);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanRangeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(vidRange);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgVlanRangeGet
*
* DESCRIPTION:
*       Function gets the valid VLAN Range.
*
* INPUTS:
*       None
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
GT_STATUS prvTgfBrgVlanRangeGet
(
    OUT GT_U16                       *vidRangePtr
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgVlanRangeGet(prvTgfDevNum, vidRangePtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(vidRangePtr);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/******************************************************************************\
 *                       API for default settings                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfBrgDefVlanEntryWriteWithTaggingCmd
*
* DESCRIPTION:
*       Set vlan entry, with the all ports of the test as
*       in the vlan with specified tagging command
*
* INPUTS:
*       vlanId     - the vlan Id
*       taggingCmd - GT_TRUE - tagging command for all members
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on HW error
*       GT_BAD_PARAM - on wrong devNum
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgDefVlanEntryWriteWithTaggingCmd
(
    IN GT_U16                             vlanId,
    IN PRV_TGF_BRG_VLAN_PORT_TAG_CMD_ENT  taggingCmd
)
{
    GT_U32                              portIter;
    CPSS_PORTS_BMP_STC                  portsMembers;
    CPSS_PORTS_BMP_STC                  portsTagging;
    PRV_TGF_BRG_VLAN_INFO_STC           vlanInfo;
    PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;

    /* initialize default settings */
    prvTgfBrgInit();

    /* set default vlan entry */
    cpssOsMemCpy(&vlanInfo, &prvTgfDefVlanInfo, sizeof(vlanInfo));

    /* start with no ports */
    portsMembers.ports[0] = 0;
    portsMembers.ports[1] = 0;

    /* all ports are 'untagged' */
    portsTagging.ports[0] = 0;
    portsTagging.ports[1] = 0;

    /* clear portsTaggingCmd */
    cpssOsMemSet(
        &portsTaggingCmd, 0, sizeof(PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC));

    /* add the needed ports to the vlan */
    for(portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfDevsArray[portIter] != prvTgfDevNum)
        {
            /* this port not belong to current device */
            continue;
        }

        portsMembers.ports[prvTgfPortsArray[portIter] >> 5] |=
            1 << (prvTgfPortsArray[portIter] & 0x1f);

        if(taggingCmd != PRV_TGF_BRG_VLAN_PORT_UNTAGGED_CMD_E)
        {
            CPSS_PORTS_BMP_PORT_SET_MAC(&portsTagging,prvTgfPortsArray[portIter]);
        }

        portsTaggingCmd.portsCmd[prvTgfPortsArray[portIter]] =
            taggingCmd;
    }

    return prvTgfBrgVlanEntryWrite(
        prvTgfDevNum, vlanId, &portsMembers, &portsTagging,
        &vlanInfo, &portsTaggingCmd);
}

/*******************************************************************************
* prvTgfBrgDefVlanEntryWrite
*
* DESCRIPTION:
*       Set vlan entry, with the all ports of the test as untagged in the vlan
*
* INPUTS:
*       vlanId - the vlan Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on HW error
*       GT_BAD_PARAM - on wrong devNum
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgDefVlanEntryWrite
(
    IN GT_U16                         vlanId
)
{
    return prvTgfBrgDefVlanEntryWriteWithTaggingCmd(
        vlanId, PRV_TGF_BRG_VLAN_PORT_UNTAGGED_CMD_E);
}

/*******************************************************************************
* prvTgfBrgDefVlanEntryWithPortsSet
*
* DESCRIPTION:
*       Set vlan entry, with the requested ports, tagging
*
* INPUTS:
*       vlanId     - the vlan id
*       portsArray - array of ports
*       devArray   - array of devices (when NULL -> assume all ports belong to devNum)
*       tagArray   - array of tagging for the ports (when NULL -> assume all ports untagged)
*       numPorts   - number of ports in the arrays
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on HW error
*       GT_BAD_PARAM - on wrong parameters
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgDefVlanEntryWithPortsSet
(
    IN GT_U16                         vlanId,
    IN GT_U8                          portsArray[],
    IN GT_U8                          devArray[],
    IN GT_U8                          tagArray[],
    IN GT_U32                         numPorts
)
{
    GT_U8                               devNum  = 0;
    GT_STATUS                           rc, rc1 = GT_OK;
    GT_U32                              portIter;
    CPSS_PORTS_BMP_STC                  portsMembers;
    CPSS_PORTS_BMP_STC                  portsTagging;
    PRV_TGF_BRG_VLAN_INFO_STC           vlanInfo;
    PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;


    /* initialize default settings */
    prvTgfBrgInit();

    /* start with no ports */
    portsMembers.ports[0] = 0;
    portsMembers.ports[1] = 0;

    /* all ports are 'untagged' */
    portsTagging.ports[0] = 0;
    portsTagging.ports[1] = 0;

    /* clear portsTaggingCmd */
    cpssOsMemSet(&portsTaggingCmd, 0, sizeof(PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);
    
    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* add the needed ports to the vlan */
        for(portIter = 0; portIter < numPorts; portIter++)
        {
            if(devArray)
            {
                if(devArray[portIter] != devNum)
                {
                    /* this port not belong to current device */
                    continue;
                }
            }

            portsMembers.ports[portsArray[portIter] >> 5] |=
                1 << (portsArray[portIter] & 0x1F);

            if(tagArray)
            {
                portsTagging.ports[portsArray[portIter] >> 5] |=
                    (tagArray[portIter] & 1) << (portsArray[portIter] & 0x1F) ;

                portsTaggingCmd.portsCmd[portsArray[portIter]] = (tagArray[portIter] & 1) ?
                    PRV_TGF_BRG_VLAN_PORT_TAG0_CMD_E :
                    PRV_TGF_BRG_VLAN_PORT_UNTAGGED_CMD_E;
            }
        }

        /* set default vlan entry */
        cpssOsMemCpy(&vlanInfo, &prvTgfDefVlanInfo, sizeof(vlanInfo));

        /* call device specific API */
        rc = prvTgfBrgVlanEntryWrite(devNum, vlanId,
                                     &portsMembers, &portsTagging,
                                     &vlanInfo, &portsTaggingCmd);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfBrgVlanEntryWrite FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
}

/*******************************************************************************
* prvTgfBrgDefVlanEntryInvalidate
*
* DESCRIPTION:
*       This function invalidates VLAN entry and reset all it's content
*
* INPUTS:
*       vlanId - VLAN id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - wrong devNum or vid
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgDefVlanEntryInvalidate
(
    IN GT_U16               vlanId
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_PORTS_BMP_STC                  portsMembers;
    CPSS_PORTS_BMP_STC                  portsTagging;
    PRV_TGF_BRG_VLAN_INFO_STC           vlanInfo;
    PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;


    /* initialize default settings */
    prvTgfBrgInit();

    /* start with no ports */
    portsMembers.ports[0] = 0;
    portsMembers.ports[1] = 0;

    /* all ports are 'untagged' */
    portsTagging.ports[0] = 0;
    portsTagging.ports[1] = 0;

    /* reset vlan entry */
    cpssOsMemSet(&vlanInfo, 0, sizeof(vlanInfo));

    /* reset portsTaggingCmd */
    cpssOsMemSet(&portsTaggingCmd, 0, sizeof(PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC));

    /* write Vlan entry */
    rc = prvTgfBrgVlanEntryWrite(prvTgfDevNum, vlanId,
                                 &portsMembers, &portsTagging,
                                 &vlanInfo, &portsTaggingCmd);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfBrgVlanEntryWrite FAILED, rc = [%d]", rc);

        return rc;
    }

    /* invalidate vlan entry */
    rc = prvTgfBrgVlanEntryInvalidate(vlanId);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvTgfBrgVlanEntryInvalidate FAILED, rc = [%d]", rc);
    }

    return rc;
}

/*******************************************************************************
* prvTgfBrgVlanMruProfileIdxSet
*
* DESCRIPTION:
*     Set Maximum Receive Unit MRU profile index for a VLAN.
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*     Value of MRU for profile is set by cpssDxChBrgVlanMruProfileValueSet.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       vlanId   - vlan id
*       mruIndex - MRU profile index [0..7]
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanMruProfileIdxSet
(
    IN GT_U16    vlanId,
    IN GT_U32    mruIndex
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
        rc = cpssDxChBrgVlanMruProfileIdxSet(devNum, vlanId, mruIndex);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCutThroughVlanEthertypeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(vlanId);
    TGF_PARAM_NOT_USED(mruIndex);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */

}

/*******************************************************************************
* prvTgfBrgVlanMruProfileValueSet
*
* DESCRIPTION:
*     Set MRU value for a VLAN MRU profile.
*     MRU VLAN profile sets maximum packet size that can be received
*     for the given VLAN.
*     cpssDxChBrgVlanMruProfileIdxSet set index of profile for a specific VLAN.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       mruIndex - MRU profile index [0..7]
*       mruValue - MRU value in bytes [0..0xFFFF]
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanMruProfileValueSet
(
    IN GT_U32    mruIndex,
    IN GT_U32    mruValue
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
        rc = cpssDxChBrgVlanMruProfileValueSet(devNum, mruIndex, mruValue);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChCutThroughVlanEthertypeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(mruIndex);
    TGF_PARAM_NOT_USED(mruValue);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */


}

/*******************************************************************************
* prvTgfBrgDefFdbMacEntryOnPortSet
*
* DESCRIPTION:
*       Set FDB entry, as 'mac address + vlan' on port
*
* INPUTS:
*       macAddr    - mac address
*       vlanId     - vlan Id
*       dstDevNum  - destination device
*       dstPortNum - destination port
*       isStatic   - is the entry static/dynamic
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - wrong devNum or vid
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgDefFdbMacEntryOnPortSet
(
    IN TGF_MAC_ADDR                   macAddr,
    IN GT_U16                         vlanId,
    IN GT_U8                          dstDevNum,
    IN GT_U8                          dstPortNum,
    IN GT_BOOL                        isStatic
)
{
    PRV_TGF_BRG_MAC_ENTRY_STC   macEntry;


    /* initialize default settings */
    prvTgfBrgInit();

    /* create a mac entry with PORT interface */
    cpssOsMemCpy(&macEntry, &prvTgfDefMacEntry, sizeof(macEntry));
    cpssOsMemCpy(macEntry.key.key.macVlan.macAddr.arEther, macAddr, sizeof(TGF_MAC_ADDR));

    macEntry.key.key.macVlan.vlanId       = vlanId;
    macEntry.dstInterface.type            = CPSS_INTERFACE_PORT_E;
    macEntry.dstInterface.devPort.devNum  = dstDevNum;
    macEntry.dstInterface.devPort.portNum = dstPortNum;
    macEntry.isStatic                     = isStatic;

    return prvTgfBrgFdbMacEntrySet(&macEntry);
}

/*******************************************************************************
* prvTgfBrgDefFdbMacEntryOnVidxSet
*
* DESCRIPTION:
*       Set FDB entry, as 'mac address + vlan' on Vidx
*
* INPUTS:
*       macAddr    - mac address
*       vlanId     - vlan Id
*       vidx       - vidx number
*       isStatic   - is the entry static/dynamic
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - wrong devNum or vid
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgDefFdbMacEntryOnVidxSet
(
    IN TGF_MAC_ADDR                   macAddr,
    IN GT_U16                         vlanId,
    IN GT_U16                         vidx,
    IN GT_BOOL                        isStatic
)
{
    PRV_TGF_BRG_MAC_ENTRY_STC   macEntry;


    /* initialize default settings */
    prvTgfBrgInit();

    /* create a mac entry with VIDX interface */
    cpssOsMemCpy(&macEntry, &prvTgfDefMacEntry, sizeof(macEntry));
    cpssOsMemCpy(macEntry.key.key.macVlan.macAddr.arEther, macAddr, sizeof(TGF_MAC_ADDR));

    macEntry.key.key.macVlan.vlanId = vlanId;
    macEntry.dstInterface.type      = CPSS_INTERFACE_VIDX_E;
    macEntry.dstInterface.vidx      = vidx;
    macEntry.isStatic               = isStatic;

    return prvTgfBrgFdbMacEntrySet(&macEntry);
}

/*******************************************************************************
* prvTgfBrgDefFdbMacEntryOnVidSet
*
* DESCRIPTION:
*       Set FDB entry, as 'mac address + vlan' on Vlan ID
*
* INPUTS:
*       macAddr  - mac address
*       vlanId   - vlan Id
*       vid      - vlan Id
*       isStatic - is the entry static/dynamic
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - wrong devNum or vid
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgDefFdbMacEntryOnVidSet
(
    IN TGF_MAC_ADDR                   macAddr,
    IN GT_U16                         vlanId,
    IN GT_U16                         vid,
    IN GT_BOOL                        isStatic
)
{
    PRV_TGF_BRG_MAC_ENTRY_STC   macEntry;


    /* initialize default settings */
    prvTgfBrgInit();

    /* create a mac entry with VLAN interface */
    cpssOsMemCpy(&macEntry, &prvTgfDefMacEntry, sizeof(macEntry));
    cpssOsMemCpy(macEntry.key.key.macVlan.macAddr.arEther, macAddr, sizeof(TGF_MAC_ADDR));

    macEntry.key.key.macVlan.vlanId = vlanId;
    macEntry.dstInterface.type      = CPSS_INTERFACE_VID_E;
    macEntry.dstInterface.vlanId    = vid;
    macEntry.daRoute                = GT_TRUE;
    macEntry.isStatic               = isStatic;

    return prvTgfBrgFdbMacEntrySet(&macEntry);
}

/*******************************************************************************
* prvTgfBrgVidxEntrySet
*
* DESCRIPTION:
*       Set vidx entry with the requested ports
*
* INPUTS:
*       vidx         - vidx number
*       portsArray   - array of ports
*       devicesArray - array of devices
*       numPorts     - number of ports in the arrays
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - wrong devNum or vid
*       GT_BAD_PTR   - on NULL pointer
*
* COMMENTS:
*       When devicesArray is NULL assume all ports belong to devNum
*
*******************************************************************************/
GT_STATUS prvTgfBrgVidxEntrySet
(
    IN GT_U16                         vidx,
    IN GT_U8                          portsArray[],
    IN GT_U8                          devicesArray[],
    IN GT_U8                          numPorts
)
{
    GT_U32              portIter;
    CPSS_PORTS_BMP_STC  portsMembers;


    /* initialize default settings */
    prvTgfBrgInit();

    /* start with no ports */
    portsMembers.ports[0] = 0;
    portsMembers.ports[1] = 0;

    /* add the needed ports to the vidx */
    for(portIter = 0; portIter < numPorts; portIter++)
    {
        if(devicesArray)
        {
            if(devicesArray[portIter] != prvTgfDevNum)
            {
                /* this port not belong to current device */
                continue;
            }
        }

        portsMembers.ports[portsArray[portIter] >> 5] |= 1 << (portsArray[portIter] & 0x1f);
    }

    /* create a new record in MC Group Table with new VDIX entry */
    return prvTgfBrgMcEntryWrite(prvTgfDevNum, vidx, &portsMembers);
}

/*******************************************************************************
* prvTgfBrgVlanPvidSet
*
* DESCRIPTION:
*       Set port's Vlan Id to all tested ports
*
* INPUTS:
*       devNum - device number
*       vlanId - vlan Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - otherwise
*       GT_BAD_PARAM - wrong devNum or vid
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPvidSet
(
    IN GT_U16                         pvid
)
{
    GT_STATUS   rc       = GT_OK;
    GT_U32      portIter = 0;


    /* add the needed ports to the vlan */
    for(portIter = 0 ; portIter < prvTgfPortsNum; portIter++)
    {
        if(prvTgfDevsArray[portIter] != prvTgfDevNum)
        {
            /* this port not belong to current device */
            continue;
        }

        rc = prvTgfBrgVlanPortVidSet(prvTgfDevNum, prvTgfPortsArray[portIter], pvid);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfBrgVlanPortVidSet FAILED, rc = [%d]", rc);

            return rc;
        }
    }

    return rc;
}

/*******************************************************************************
* prvTgfBrgVlanTpidEntrySet
*
* DESCRIPTION:
*       Function sets TPID (Tag Protocol ID) table entry.
*
* INPUTS:
*       direction   - ingress/egress direction
*       entryIndex  - entry index for TPID table
*       etherType   - Tag Protocol ID value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong entryIndex, direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanTpidEntrySet
(
    IN  CPSS_DIRECTION_ENT  direction,
    IN  GT_U32              entryIndex,
    IN  GT_U16              etherType
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;
    GT_U32  tmpIndex;
    GT_U16  tmpEtherType;
    GT_U32  numOf0x8100Entries = 0;
    GT_U32  largestIndex;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        if( GT_FALSE == prvTgfCommonIsDeviceForce(devNum))
        {
            /* this device not need it */
            continue;
        }


        /* traffic that egress from cascade/ring port , must be recognized as with
           2 tags (first tag in dsa tag) on the ingress of the cascade port/ring port */
        if(direction == CPSS_DIRECTION_EGRESS_E)
        {
            /* check that there is free index left in the ingress table (look for 0x8100) */
            largestIndex = 0;
            tmpEtherType = 0;
            for(tmpIndex = 8 ; tmpIndex != 0 ; tmpIndex--)
            {
                /* set the member with full bmp for the ethertype */
                rc = cpssDxChBrgVlanTpidEntryGet(devNum,
                                CPSS_DIRECTION_INGRESS_E,
                                tmpIndex - 1,&tmpEtherType);
                if(rc != GT_OK)
                {
                    PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanTpidEntryGet FAILED, rc = [%d]", rc);
                    return rc;
                }

                if(tmpEtherType == etherType)
                {
                    /* the ethretype already exists in the ingress table too */
                    break;
                }

                if(tmpEtherType == 0x8100)
                {
                    if(numOf0x8100Entries == 0)
                    {
                        largestIndex = tmpIndex - 1;
                    }

                    numOf0x8100Entries++;

                    if(numOf0x8100Entries == 2)
                    {
                        /* we can use this index */
                        break;
                    }
                }
            }

            if(tmpEtherType == etherType)
            {
                /* do nothing - the ethretype already exists in the ingress table too */
                continue;
            }
            else if(tmpIndex == 0)
            {
                rc = GT_FULL;
                /* not found free place --> maybe use better algorithm for
                    duplications and not only 0x8100 */
                PRV_UTF_LOG0_MAC("[TGF]: prvTgfBrgVlanTpidEntrySet : table full \n");
                /*return rc;*/
            }
            else
            {
                /* found 2 places , use the largest index */
                rc = cpssDxChBrgVlanTpidEntrySet(devNum, CPSS_DIRECTION_INGRESS_E, largestIndex, etherType);
                if (GT_OK != rc)
                {
                    PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanTpidEntrySet FAILED, rc = [%d]", rc);

                    return rc;
                }
            }
        }
        else /*CPSS_DIRECTION_INGRESS_E*/
        {
            if(entryIndex > 5)
            {
                if(GT_FALSE == prvTgfCommonIsUseLastTpid())
                {
                    /* don't allow test to override last 2 indexes in the ingress --
                        we need it for the cascade/ring ports */
                    return GT_OK;
                }
            }
        }
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);
    
    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChBrgVlanTpidEntrySet(devNum, direction, entryIndex, etherType);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanTpidEntrySet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(direction);
    TGF_PARAM_NOT_USED(entryIndex);
    TGF_PARAM_NOT_USED(etherType);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgVlanTpidEntryGet
*
* DESCRIPTION:
*       Function gets TPID (Tag Protocol ID) table entry.
*
* INPUTS:
*       direction  - ingress/egress direction
*       entryIndex - entry index for TPID table
*
* OUTPUTS:
*       etherTypePtr - (pointer to) Tag Protocol ID value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, entryIndex, direction
*       GT_BAD_PTR               - etherTypePtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanTpidEntryGet
(
    IN  CPSS_DIRECTION_ENT  direction,
    IN  GT_U32              entryIndex,
    OUT GT_U16              *etherTypePtr
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgVlanTpidEntryGet(prvTgfDevNum, direction, entryIndex, etherTypePtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(direction);
    TGF_PARAM_NOT_USED(entryIndex);
    TGF_PARAM_NOT_USED(etherTypePtr);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgVlanPortIngressTpidSet
*
* DESCRIPTION:
*       Function sets bitmap of TPID (Tag Protocol ID) table indexes per
*       ingress port.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       ethMode    - TAG0/TAG1 selector
*       tpidBmp    - bitmap represent entries in the TPID Table
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
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortIngressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidBmp
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc;
    GT_U32  ii;
    PRV_TGF_MEMBER_FORCE_INFO_STC *currMemPtr;

    /* add the classification for all cascade/ring ports , with full bmp of ethertype 0,1 */

    /* look for this member in the DB */
    currMemPtr = &prvTgfDevPortForceArray[0];
    for(ii = 0 ; ii < prvTgfDevPortForceNum; ii++,currMemPtr++)
    {
        if(devNum != currMemPtr->member.devNum)
        {
            continue;
        }

        if(currMemPtr->forceToVlan == GT_FALSE)
        {
            /* member not need to be forced to any vlan */
            continue;
        }

        if(currMemPtr->member.portNum >= CPSS_MAX_PORTS_NUM_CNS)
        {
            return GT_BAD_PARAM;
        }

        /* set the member with full bmp for the ethertype */
        rc = cpssDxChBrgVlanPortIngressTpidSet(currMemPtr->member.devNum,
                        currMemPtr->member.portNum,
                        ethMode,0x000000FF);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* call device specific API */
    return cpssDxChBrgVlanPortIngressTpidSet(devNum, portNum, ethMode, tpidBmp);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(ethMode);
    TGF_PARAM_NOT_USED(tpidBmp);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgVlanPortIngressTpidGet
*
* DESCRIPTION:
*       Function gets bitmap of TPID (Tag Protocol ID) table indexes per
*       ingress port.
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
*       GT_OUT_OF_RANGE          - tpidBmp initialized with more then 8 bits
*       GT_BAD_PTR               - tpidBmpPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortIngressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    OUT GT_U32              *tpidBmpPtr
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgVlanPortIngressTpidGet(devNum, portNum, ethMode, tpidBmpPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(ethMode);
    TGF_PARAM_NOT_USED(tpidBmpPtr);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgVlanPortEgressTpidSet
*
* DESCRIPTION:
*       Function sets index of TPID (Tag protocol ID) table per egress port.
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number (CPU port is supported)
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
GT_STATUS prvTgfBrgVlanPortEgressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidEntryIndex
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgVlanPortEgressTpidSet(devNum, portNum, ethMode, tpidEntryIndex);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(ethMode);
    TGF_PARAM_NOT_USED(tpidEntryIndex);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgVlanPortEgressTpidGet
*
* DESCRIPTION:
*       Function gets index of TPID (Tag protocol ID) table per egress port.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number (CPU port is supported)
*       ethMode - TAG0/TAG1 selector
*
* OUTPUTS:
*       tpidEntryIndexPtr - (pointer to) TPID table entry index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - illegal tpidEntryIndex (not 0-7)
*       GT_BAD_PTR               - tpidEntryIndexPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanPortEgressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    OUT GT_U32               *tpidEntryIndexPtr
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgVlanPortEgressTpidGet(devNum, portNum, ethMode, tpidEntryIndexPtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(ethMode);
    TGF_PARAM_NOT_USED(tpidEntryIndexPtr);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgVlanBridgingModeSet
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
* INPUTS:
*       brgMode - bridge mode: IEEE 802.1Q bridge or IEEE 802.1D bridge
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanBridgingModeSet
(
    IN CPSS_BRG_MODE_ENT    brgMode
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
        rc = cpssDxChBrgVlanBridgingModeSet(devNum, brgMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanBridgingModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(brgMode);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgGenIeeeReservedMcastTrapEnable
*
* DESCRIPTION:
*       Enables trapping or mirroring to CPU Multicast packets, with MAC_DA in
*       the IEEE reserved Multicast range (01-80-C2-00-00-xx).
*
* INPUTS:
*       dev    - physical device number
*       enable - GT_FALSE -
*                   Trapping or mirroring to CPU of packet with
*                   MAC_DA = 01-80-C2-00-00-xx disabled.
*                GT_TRUE -
*                   Trapping or mirroring to CPU of packet, with
*                   MAC_DA = 01-80-C2-00-00-xx enabled according to the setting
*                   of IEEE Reserved Multicast Configuration Register<n> (0<=n<16)
*                   by cpssDxChBrgGenIeeeReservedMcastProtCmdSet function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenIeeeReservedMcastTrapEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgGenIeeeReservedMcastTrapEnable(dev, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    return cpssExMxPmBrgGenBpduTrapEnableSet(dev, enable);
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgGenPortIeeeReservedMcastProfileIndexSet
*
* DESCRIPTION:
*       Select the IEEE Reserved Multicast profile (table) associated with port
*
* INPUTS:
*       dev          - device number
*       port         - port number (including CPU port)
*       profileIndex - profile index (0..3). The parameter defines profile (table
*                      number) for the 256 protocols.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev, port
*       GT_OUT_OF_RANGE          - for profileIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenPortIeeeReservedMcastProfileIndexSet
(
    IN GT_U8                dev,
    IN GT_U8                port,
    IN GT_U32               profileIndex
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgGenPortIeeeReservedMcastProfileIndexSet(dev, port, profileIndex);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(dev);
    TGF_PARAM_NOT_USED(port);
    TGF_PARAM_NOT_USED(profileIndex);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgGenIeeeReservedMcastProtCmdSet
*
* DESCRIPTION:
*       Enables forwarding, trapping, or mirroring to the CPU any of the
*       packets with MAC DA in the IEEE reserved, Multicast addresses
*       for bridge control traffic, in the range of 01-80-C2-00-00-xx,
*       where 0<=xx<256
*
* INPUTS:
*       dev          - device number
*       profileIndex - profile index (0..3). Parameter is relevant only for
*                      DxChXcat and above. The parameter defines profile (table
*                      number) for the 256 protocols. Profile bound per port by
*                      <prvTgfBrgGenPortIeeeReservedMcastProfileIndexSet>.
*       protocol     - specifies the Multicast protocol
*       cmd          - supported commands:
*                      CPSS_PACKET_CMD_FORWARD_E , CPSS_PACKET_CMD_MIRROR_TO_CPU_E,
*                      CPSS_PACKET_CMD_TRAP_TO_CPU_E , CPSS_PACKET_CMD_DROP_SOFT_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on bad device number, protocol or control packet command
*                      of profileIndex.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       BPDU packets with MAC DA = 01-80-C2-00-00-00 are not affect
*       by this mechanism. BPDUs are implicitly trapped to the CPU
*       if the ingress port span tree is not disabled.
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenIeeeReservedMcastProtCmdSet
(
    IN GT_U8                dev,
    IN GT_U32               profileIndex,
    IN GT_U8                protocol,
    IN CPSS_PACKET_CMD_ENT  cmd
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgGenIeeeReservedMcastProtCmdSet(dev,
                                                     profileIndex,
                                                     protocol,
                                                     cmd);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(dev);
    TGF_PARAM_NOT_USED(profileIndex);
    TGF_PARAM_NOT_USED(protocol);
    TGF_PARAM_NOT_USED(cmd);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgSrcIdPortSrcIdForceEnableSet
*
* DESCRIPTION:
*         Set Source ID Assignment force mode per Ingress Port.
*
* INPUTS:
*       devNum    - device number
*       portNum   - Physical port number, CPU port
*       enable    - enable/disable SourceID force mode
*                     GT_TRUE - enable Source ID force mode
*                     GT_FALSE - disable Source ID force mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgSrcIdPortSrcIdForceEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc;

    /* call device specific API */
    rc = cpssDxChBrgSrcIdPortSrcIdForceEnableSet(devNum, portNum, enable);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgSrcIdPortSrcIdForceEnableSet FAILED, rc = [%d]", rc);
    }
    return rc;
#else /* !CHX_FAMILY */

    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(enable);

    return GT_NOT_SUPPORTED;
#endif /* CHX_FAMILY */

}

/*******************************************************************************
* prvTgfBrgSrcIdGlobalSrcIdAssignModeSet
*
* DESCRIPTION:
*         Set Source ID Assignment mode.
*
* INPUTS:
*       mode - the assignment mode of the packet Source ID.
*              CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E mode is
*              supported only for DxChXcat and above.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgSrcIdGlobalSrcIdAssignModeSet
(
    IN CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  mode
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
        rc = cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet(devNum, mode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#else /* !CHX_FAMILY */
    TGF_PARAM_NOT_USED(mode);

    return GT_NOT_SUPPORTED;
#endif /* CHX_FAMILY */
}

/*******************************************************************************
* prvTgfBrgSrcIdPortDefaultSrcIdSet
*
* DESCRIPTION:
*       Configure Port's Default Source ID.
*       The Source ID is used for source based egress filtering.
*
* INPUTS:
*       devNum          - device number
*       portNum         - Physical port number, CPU port
*       defaultSrcId    - Port's Default Source ID (0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, defaultSrcId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgSrcIdPortDefaultSrcIdSet
(
    IN GT_U8   devNum,
    IN GT_U8   portNum,
    IN GT_U32  defaultSrcId
)
{
    GT_STATUS   rc    = GT_OK;

#ifdef CHX_FAMILY

    /* call device specific API */
    rc = cpssDxChBrgSrcIdPortDefaultSrcIdSet(devNum, portNum, defaultSrcId);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgSrcIdPortDefaultSrcIdSet FAILED, rc = [%d]", rc);

        return rc;
    }
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY

        /* call device specific API */
        rc = cpssExMxPmBrgSrcIdPortDefaultSrcIdSet(devNum, portNum, defaultSrcId);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgSrcIdPortDefaultSrcIdSet FAILED, rc = [%d]", rc);

            return rc;
        }

#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */

    return rc;
}

/*******************************************************************************
* prvTgfBrgSrcIdGroupPortDelete
*
* DESCRIPTION:
*         Delete a port from Source ID group. Packets assigned with this
*         Source ID will not be forwarded to this port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID  number that the port is added to (0..31).
*       portNum  - Physical port number, CPU port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgSrcIdGroupPortDelete
(
    IN GT_U8   devNum,
    IN GT_U32  sourceId,
    IN GT_U8   portNum
)
{
    GT_STATUS   rc    = GT_OK;

#ifdef CHX_FAMILY

    /* call device specific API */
    rc = cpssDxChBrgSrcIdGroupPortDelete(devNum, sourceId, portNum);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgSrcIdGroupPortDelete FAILED, rc = [%d]", rc);

        return rc;
    }
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY

        /* call device specific API */
        rc = cpssExMxPmBrgSrcIdGroupPortDelete(devNum, sourceId, portNum);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgSrcIdGroupPortDelete FAILED, rc = [%d]", rc);

            return rc;
        }

#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */

    return rc;
}

/*******************************************************************************
* prvTgfBrgSrcIdPortUcastEgressFilterSet
*
* DESCRIPTION:
*     Per Egress Port enable or disable Source ID egress filter for unicast
*     packets. The Source ID filter is configured by
*     cpssDxChBrgSrcIdGroupPortAdd and cpssDxChBrgSrcIdGroupPortDelete.
*
* INPUTS:
*       devNum   - device number
*       portNum  - Physical port number/all ports wild card, CPU port
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
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgSrcIdPortUcastEgressFilterSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
    GT_STATUS   rc    = GT_OK;

#ifdef CHX_FAMILY

    /* call device specific API */
    rc = cpssDxChBrgSrcIdPortUcastEgressFilterSet(devNum, portNum, enable);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgSrcIdPortUcastEgressFilterSet FAILED, rc = [%d]", rc);

        return rc;
    }
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY

        /* call device specific API */
        rc = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet(devNum, portNum, enable);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet FAILED, rc = [%d]", rc);

            return rc;
        }

#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */

    return rc;
}

/*******************************************************************************
* prvTgfBrgSrcIdGroupPortAdd
*
* DESCRIPTION:
*         Add a port to Source ID group. Packets assigned with this Source ID
*         may be forwarded to this port.
*
* INPUTS:
*       devNum   - device number
*       sourceId - Source ID  number that the port is added to (0..31).
*       portNum  - Physical port number, CPU port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum, sourceId
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgSrcIdGroupPortAdd
(
    IN GT_U8   devNum,
    IN GT_U32  sourceId,
    IN GT_U8   portNum
)
{
    GT_STATUS   rc    = GT_OK;

#ifdef CHX_FAMILY

    /* call device specific API */
    rc = cpssDxChBrgSrcIdGroupPortAdd(devNum, sourceId, portNum);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgSrcIdGroupPortAdd FAILED, rc = [%d]", rc);

        return rc;
    }
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY

        /* call device specific API */
        rc = cpssExMxPmBrgSrcIdGroupPortAdd(devNum, sourceId, portNum);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgSrcIdGroupPortAdd FAILED, rc = [%d]", rc);

            return rc;
        }

#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */

    return rc;
}


/*******************************************************************************
* prvTgfBrgVlanRemoveVlanTag1IfZeroModeSet
*
* DESCRIPTION:
*       Set Tag1 removal mode from the egress port Tag State if Tag1 VID
*       is assigned a value of 0.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       mode - Vlan Tag1 Removal mode when Tag1 VID=0
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanRemoveVlanTag1IfZeroModeSet
(
    IN  PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT   mode
)
{
#ifdef CHX_FAMILY
    GT_U8                                             devNum  = 0;
    GT_STATUS                                         rc, rc1 = GT_OK;
    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT   cpssMode;

    /* Translate to CPSS mode */
    switch (mode)
    {
        case PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E:
            cpssMode = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E;
            break;
        case PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E:
            cpssMode = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E;
            break;
        case PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E:
            cpssMode = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet(devNum, cpssMode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }
    }

    return rc1;
#else
    /* avoid warnings */
    TGF_PARAM_NOT_USED(mode);

    return GT_BAD_STATE;
#endif /* CHX_FAMILY */

}

/*******************************************************************************
* prvTgfBrgVlanRemoveVlanTag1IfZeroModeGet
*
* DESCRIPTION:
*       Get Tag1 removal mode from the egress port Tag State when Tag1 VID
*       is assigned a value of 0.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       None
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
GT_STATUS prvTgfBrgVlanRemoveVlanTag1IfZeroModeGet
(
    OUT PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT   *modePtr
)
{
#ifdef CHX_FAMILY
    GT_STATUS   rc    = GT_OK;

    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT   cpssMode;

    /* call device specific API */
    rc = cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet(prvTgfDevNum, &cpssMode);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet FAILED, rc = [%d]", rc);

        return rc;
    }

    /* Translate to CPSS mode */
    switch (cpssMode)
    {
        case CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E:
            *modePtr = PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E;
            break;
        case CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E:
            *modePtr = PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E;
            break;
        case CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E:
            *modePtr = PRV_TGF_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E;
            break;
        default:
            return GT_FAIL;
    }
    return rc;
#else
    /* avoid warnings */
    TGF_PARAM_NOT_USED(modePtr);

    return GT_BAD_STATE;
#endif /* CHX_FAMILY */
}

/*******************************************************************************
* prvTgfBrgFdbDeviceTableSet
*
* DESCRIPTION:
*       This function sets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*       So for proper work of PP the application must set the relevant bits of
*       all devices in the system prior to inserting FDB entries associated with
*       them
*
* APPLICABLE DEVICES:  All Devices
*
* INPUTS:
*       devTableArr[4] - (array of)bmp of devices to set.
*                       DxCh devices use only devTableBmp[0]
*                       ExMxPm devices use devTableBmp[0..3]
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbDeviceTableSet
(
    IN  GT_U32  devTableArr[PRV_TGF_FDB_DEV_TABLE_SIZE_CNS]
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
        rc = cpssDxChBrgFdbDeviceTableSet(devNum, devTableArr[0]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbDeviceTableSet FAILED, rc = [%d]", rc);

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
        rc = cpssExMxPmBrgFdbDeviceTableSet(devNum, devTableArr);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbDeviceTableSet FAILED, rc = [%d]", rc);

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
* prvTgfBrgFdbDeviceTableGet
*
* DESCRIPTION:
*       This function gets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon.
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it, the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       none
*
* OUTPUTS:
*       devTableArr - pointer to (Array of) bmp (bitmap) of devices.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbDeviceTableGet
(
    OUT GT_U32  devTableArr[PRV_TGF_FDB_DEV_TABLE_SIZE_CNS]
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
        rc = cpssDxChBrgFdbDeviceTableGet(devNum, &devTableArr[0]);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbDeviceTableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }

        break;/* take first device */
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
        rc = cpssExMxPmBrgFdbDeviceTableGet(devNum, devTableArr);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgFdbDeviceTableSet FAILED, rc = [%d]", rc);

            rc1 = rc;
        }

        break;/* take first device */
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}


/*******************************************************************************
* prvTgfBrgVlanKeepVlan1EnableSet
*
* DESCRIPTION:
*       Enable/Disable keeping VLAN1 in the packet, for packets received with VLAN1,
*       even-though the tag-state of this egress-port is configured in the
*       VLAN-table to "untagged" or "VLAN0".
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
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - illegal vidRange
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgVlanKeepVlan1EnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       up,
    IN  GT_BOOL     enable
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgVlanKeepVlan1EnableSet(devNum, portNum, up, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(up);
    TGF_PARAM_NOT_USED(enable);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgVlanKeepVlan1EnableGet
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
GT_STATUS prvTgfBrgVlanKeepVlan1EnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       up,
    OUT GT_BOOL     *enablePtr
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgVlanKeepVlan1EnableGet(devNum, portNum, up, enablePtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(up);
    TGF_PARAM_NOT_USED(enablePtr);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgVlanTr101Check
*
* DESCRIPTION:
*    check if the device supports the tr101
* INPUTS:
*       none
*
* RETURNS:
*       GT_TRUE - the device supports the tr101
*       GT_FALSE - the device not supports the tr101
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvTgfBrgVlanTr101Check(
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
        if(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported == GT_FALSE)
        {
            /* the device not support the tr101 */
            return GT_FALSE;
        }
    }

    return GT_TRUE;
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY)

    return GT_FALSE;
#endif /* !(defined CHX_FAMILY) */
}

/*******************************************************************************
* prvTgfBrgGenPortIeeeReservedMcastLearningEnableSet
*
* DESCRIPTION:
*     Enable/Disable MAC SA learning on reserved IEEE MC packets, that are
*     trapped to CPU.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*        devNum         - device number
*        portNum        - port number 
*        enable         - GT_TRUE - learning is performed also for IEEE MC
*                               trapped packets.
*                       - GT_FALSE - No learning is performed for IEEE MC
*                               trapped packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenPortIeeeReservedMcastLearningEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgGenPortIeeeReservedMcastLearningEnableSet(devNum, 
                                                                portNum,
                                                                enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(portNum);
    TGF_PARAM_NOT_USED(enable);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgGenRateLimitGlobalCfgSet
*
* DESCRIPTION:
*       Configures global ingress rate limit parameters - rate limiting mode,
*       Four configurable global time window periods are assigned to ports as
*       a function of their operating speed:
*       10 Gbps, 1 Gbps, 100 Mbps, and 10 Mbps.
*
* INPUTS:
*       devNum         - device number
*       brgRateLimitPtr - pointer to global rate limit parameters structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   Supported windows for DxCh devices:
*       1000 Mbps: range - 256-16384 uSec    granularity - 256 uSec
*       100  Mbps: range - 256-131072 uSec   granularity - 256 uSec
*       10   Mbps: range - 256-1048570 uSec  granularity - 256 uSec
*       10   Gbps: range - 25.6-104857 uSec  granularity - 25.6 uSec
*       For DxCh3 and above: The granularity doesn't depend from PP core clock.
*       For DxCh1, DxCh2:
*       Time ranges and granularity specified for clock 200MHz(DxCh2 - 220 MHz)
*       for they changed linearly with the clock value.
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenRateLimitGlobalCfgSet
(
    IN GT_U8                            devNum,
    IN PRV_TGF_BRG_GEN_RATE_LIMIT_STC   *brgRateLimitPtr
)
{
    GT_STATUS   rc;
#ifdef CHX_FAMILY
    CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC    dxChBrgRateLimitCfg;
    
    dxChBrgRateLimitCfg.win1000Mbps = brgRateLimitPtr->win1000Mbps;
    dxChBrgRateLimitCfg.win100Mbps = brgRateLimitPtr->win100Mbps;
    dxChBrgRateLimitCfg.win10Gbps = brgRateLimitPtr->win10Gbps;
    dxChBrgRateLimitCfg.win10Mbps = brgRateLimitPtr->win10Mbps;
    dxChBrgRateLimitCfg.dropMode = brgRateLimitPtr->dropMode;
    dxChBrgRateLimitCfg.rMode = brgRateLimitPtr->rMode;


    /* call device specific API */
    rc = cpssDxChBrgGenRateLimitGlobalCfgSet(devNum, &dxChBrgRateLimitCfg);
    if (rc != GT_OK) 
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgGenRateLimitGlobalCfgSet FAILED, rc = [%d]", rc);
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_BRG_GEN_RATE_LIMIT_STC exMxPmBrgRateLimitCfg;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    exMxPmBrgRateLimitCfg.win1000Mbps = brgRateLimitPtr->win1000Mbps;
    exMxPmBrgRateLimitCfg.win100Mbps = brgRateLimitPtr->win100Mbps;
    exMxPmBrgRateLimitCfg.win10Gbps = brgRateLimitPtr->win10Gbps;
    exMxPmBrgRateLimitCfg.win10Mbps = brgRateLimitPtr->win10Mbps;
    exMxPmBrgRateLimitCfg.dropMode = brgRateLimitPtr->dropMode;
    exMxPmBrgRateLimitCfg.rMode = brgRateLimitPtr->rMode;

    switch (brgRateLimitPtr->rateLimitModeL1) 
    {
        case PRV_TGF_BRG_GEN_L1_RATE_LIMIT_EXCL_E:
            exMxPmBrgRateLimitCfg.rateLimitModeL1 = 
                CPSS_EXMXPM_BRG_GEN_L1_RATE_LIMIT_EXCL_E;
            break;
        case PRV_TGF_BRG_GEN_L1_RATE_LIMIT_INCL_E:
            exMxPmBrgRateLimitCfg.rateLimitModeL1 = 
                CPSS_EXMXPM_BRG_GEN_L1_RATE_LIMIT_INCL_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* call device specific API */
    rc = cpssExMxPmBrgGenRateLimitGlobalCfgSet(devNum,&exMxPmBrgRateLimitCfg);

    if (rc != GT_OK) 
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgGenRateLimitGlobalCfgSet FAILED, rc = [%d]", rc);
        return rc;
    }

    return rc;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}


/*******************************************************************************
* prvTgfBrgGenRateLimitGlobalCfgGet
*
* DESCRIPTION:
*     Get global ingress rate limit parameters
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       brgRateLimitPtr - pointer to global rate limit parameters structure
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DxCh3 and above: The granularity doesn't depend from PP core clock.
*       For DxCh1, DxCh2:
*       Time ranges and granularity specified for clock 200MHz(DxCh2 - 220 MHz)
*       for they changed linearly with the clock value.
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenRateLimitGlobalCfgGet
(
    IN  GT_U8                            devNum,
    OUT PRV_TGF_BRG_GEN_RATE_LIMIT_STC   *brgRateLimitPtr
)
{
    GT_STATUS   rc;

#ifdef CHX_FAMILY
    CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC    dxChBrgRateLimitCfg;
    

    /* call device specific API */
    rc = cpssDxChBrgGenRateLimitGlobalCfgGet(devNum, &dxChBrgRateLimitCfg);
    if (rc != GT_OK) 
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgGenRateLimitGlobalCfgGet FAILED, rc = [%d]", rc);
        return rc;
    }
    
    brgRateLimitPtr->win1000Mbps = dxChBrgRateLimitCfg.win1000Mbps;
    brgRateLimitPtr->win100Mbps = dxChBrgRateLimitCfg.win100Mbps;
    brgRateLimitPtr->win10Gbps = dxChBrgRateLimitCfg.win10Gbps;
    brgRateLimitPtr->win10Mbps = dxChBrgRateLimitCfg.win10Mbps;
    brgRateLimitPtr->dropMode = dxChBrgRateLimitCfg.dropMode;
    brgRateLimitPtr->rMode = dxChBrgRateLimitCfg.rMode;
    return rc;

#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_BRG_GEN_RATE_LIMIT_STC exMxPmBrgRateLimitCfg;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);

    /* use first active devices */
    rc = prvUtfNextDeviceGet(&devNum, GT_TRUE);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceGet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* call device specific API */
    rc = cpssExMxPmBrgGenRateLimitGlobalCfgGet(devNum, &exMxPmBrgRateLimitCfg);
    if (rc != GT_OK) 
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgGenRateLimitGlobalCfgGet FAILED, rc = [%d]", rc);
        return rc;
    }

    brgRateLimitPtr->win1000Mbps = exMxPmBrgRateLimitCfg.win1000Mbps;
    brgRateLimitPtr->win100Mbps = exMxPmBrgRateLimitCfg.win100Mbps;
    brgRateLimitPtr->win10Gbps = exMxPmBrgRateLimitCfg.win10Gbps;
    brgRateLimitPtr->win10Mbps = exMxPmBrgRateLimitCfg.win10Mbps;
    brgRateLimitPtr->dropMode = exMxPmBrgRateLimitCfg.dropMode;
    brgRateLimitPtr->rMode = exMxPmBrgRateLimitCfg.rMode;

    brgRateLimitPtr->rateLimitModeL1 = (exMxPmBrgRateLimitCfg.rateLimitModeL1 == 
                                         CPSS_EXMXPM_BRG_GEN_L1_RATE_LIMIT_EXCL_E) ? 
        PRV_TGF_BRG_GEN_L1_RATE_LIMIT_EXCL_E : PRV_TGF_BRG_GEN_L1_RATE_LIMIT_INCL_E;

    return rc;

#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgGenPortRateLimitSet
*
* DESCRIPTION:
*       Configures port ingress rate limit parameters
*       Each port maintains rate limits for unknown unicast packets,
*       known unicast packets, multicast packets and broadcast packets,
*       single configurable limit threshold value, and a single internal counter.
*
* INPUTS:
*       devNum      - device number
*       port        - port number
*       portGfgPtr  - pointer to rate limit configuration for a port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_BAD_PARAM             - on wrong device number or port
*       GT_OUT_OF_RANGE          - on out of range rate limit values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvTgfBrgGenPortRateLimitSet
(
    IN  GT_U8                                   devNum,
    IN GT_U8                                    port,
    IN PRV_TGF_BRG_GEN_RATE_LIMIT_PORT_STC      *portGfgPtr
)
{
    GT_STATUS   rc;
#ifdef CHX_FAMILY
    CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC  dxChPortCfg;

    dxChPortCfg.enableBc = portGfgPtr->enableBc;
    dxChPortCfg.enableMc = portGfgPtr->enableMc;
    dxChPortCfg.enableMcReg = portGfgPtr->enableMcReg;
    dxChPortCfg.enableUcKnown = portGfgPtr->enableUcKnown;
    dxChPortCfg.enableUcUnk = portGfgPtr->enableUcUnk;
    dxChPortCfg.rateLimit = portGfgPtr->rateLimit;

    /* call device specific API */
    rc = cpssDxChBrgGenPortRateLimitSet(devNum, port, &dxChPortCfg);
    if (rc != GT_OK) 
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgGenPortRateLimitSet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* call device specific API for TcpSyn */
    rc = cpssDxChBrgGenPortRateLimitTcpSynSet(devNum, port, 
                                              portGfgPtr->enableTcpSyn);
    if (rc != GT_OK)
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgGenPortRateLimitTcpSynSet FAILED, rc = [%d]", rc);
        return rc;
    }

    return rc;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_BRG_GEN_PORT_RATE_LIMIT_STC  exMxPmPortCfg;

    exMxPmPortCfg.enableBc = portGfgPtr->enableBc;
    exMxPmPortCfg.enableMc = portGfgPtr->enableMcReg;
    exMxPmPortCfg.enableMcUnreg = portGfgPtr->enableMc;
    exMxPmPortCfg.enableTcpSyn = portGfgPtr->enableTcpSyn;
    exMxPmPortCfg.enableUcKnown = portGfgPtr->enableUcKnown;
    exMxPmPortCfg.enableUcUnk = portGfgPtr->enableUcUnk;
    exMxPmPortCfg.rateLimit = portGfgPtr->rateLimit;

    /* call device specific API */
    rc = cpssExMxPmBrgGenPortRateLimitSet(devNum, port, &exMxPmPortCfg);
    if (rc != GT_OK) 
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgGenPortRateLimitSet FAILED, rc = [%d]", rc);
        return rc;
    }

    return rc;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}


#ifdef EXMXPM_FAMILY
/*******************************************************************************
* prvTgfBrgFdbAuMsgConvertFromExMxPmToGeneric
*
* DESCRIPTION:
*       Convert ExMxPm AU message to generic.
*
* INPUTS:
*       exMxPmAuMessagesPtr - array that holds received ExMxPm AU message 
*
* OUTPUTS:
*       auMessagesPtr - array that holds received AU messages
*
* RETURNS:
*       GT_OK       - on success
*       GT_NO_MORE  - the action succeeded and there are no more waiting
*                     AU messages
*
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - wrong devNum
*
*******************************************************************************/
static GT_STATUS prvTgfBrgFdbAuMsgConvertFromExMxPmToGeneric
(
    IN  CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC    *exMxPmAuMessagesPtr,
    OUT CPSS_MAC_UPDATE_MSG_EXT_STC             *auMessagesPtr
)
{

    auMessagesPtr->aging = exMxPmAuMessagesPtr->aging;
    auMessagesPtr->associatedDevNum = exMxPmAuMessagesPtr->associatedDevNum;
    auMessagesPtr->entryOffset = exMxPmAuMessagesPtr->entryOffset;
    auMessagesPtr->entryWasFound = exMxPmAuMessagesPtr->entryWasFound;
    auMessagesPtr->macEntryIndex = exMxPmAuMessagesPtr->fdbEntryIndex;
    auMessagesPtr->skip = exMxPmAuMessagesPtr->skip;
    auMessagesPtr->updType = exMxPmAuMessagesPtr->updType;

    auMessagesPtr->macEntry.appSpecificCpuCode = exMxPmAuMessagesPtr->fdbEntry.appSpecificCpuCode;
    auMessagesPtr->macEntry.daCommand = exMxPmAuMessagesPtr->fdbEntry.daCommand;
    auMessagesPtr->macEntry.daQosIndex = exMxPmAuMessagesPtr->fdbEntry.daQosIndex;
    auMessagesPtr->macEntry.daRoute = exMxPmAuMessagesPtr->fdbEntry.daRoute;
    auMessagesPtr->macEntry.daSecurityLevel = exMxPmAuMessagesPtr->fdbEntry.daSecurityLevel;
    auMessagesPtr->macEntry.isStatic = exMxPmAuMessagesPtr->fdbEntry.isStatic;
    auMessagesPtr->macEntry.mirrorToRxAnalyzerPortEn = exMxPmAuMessagesPtr->fdbEntry.mirrorToRxAnalyzerPortEn;
    auMessagesPtr->macEntry.saCommand = exMxPmAuMessagesPtr->fdbEntry.saCommand;
    auMessagesPtr->macEntry.saQosIndex = exMxPmAuMessagesPtr->fdbEntry.saQosIndex;
    auMessagesPtr->macEntry.saSecurityLevel = exMxPmAuMessagesPtr->fdbEntry.saSecurityLevel;
    auMessagesPtr->macEntry.sourceID = exMxPmAuMessagesPtr->fdbEntry.sourceId;
    auMessagesPtr->macEntry.spUnknown = exMxPmAuMessagesPtr->fdbEntry.spUnknown;
    auMessagesPtr->macEntry.userDefined = exMxPmAuMessagesPtr->fdbEntry.userDefined;

    /* Parse destination interface */
    exMxPmAuMessagesPtr->fdbEntry.dstOutlif.outlifType = CPSS_EXMXPM_OUTLIF_TYPE_LL_E;
    auMessagesPtr->macEntry.dstInterface.type = exMxPmAuMessagesPtr->fdbEntry.dstOutlif.interfaceInfo.type;

    switch (auMessagesPtr->macEntry.dstInterface.type) 
    {
        case CPSS_INTERFACE_PORT_E:
            auMessagesPtr->macEntry.dstInterface.devPort.devNum = 
                exMxPmAuMessagesPtr->fdbEntry.dstOutlif.interfaceInfo.devPort.devNum;
            auMessagesPtr->macEntry.dstInterface.devPort.portNum = 
                exMxPmAuMessagesPtr->fdbEntry.dstOutlif.interfaceInfo.devPort.portNum;
            break;
        case CPSS_INTERFACE_TRUNK_E:
            auMessagesPtr->macEntry.dstInterface.trunkId = 
                exMxPmAuMessagesPtr->fdbEntry.dstOutlif.interfaceInfo.trunkId;
            break;
        case CPSS_INTERFACE_VIDX_E:
            auMessagesPtr->macEntry.dstInterface.vidx = 
                exMxPmAuMessagesPtr->fdbEntry.dstOutlif.interfaceInfo.vidx;
            break;
        case CPSS_INTERFACE_VID_E:
            auMessagesPtr->macEntry.dstInterface.vlanId = 
                exMxPmAuMessagesPtr->fdbEntry.dstOutlif.interfaceInfo.vlanId;
            break;
        case CPSS_INTERFACE_DEVICE_E:
            auMessagesPtr->macEntry.dstInterface.devNum = 
                exMxPmAuMessagesPtr->fdbEntry.dstOutlif.interfaceInfo.devNum;
            break;
        case CPSS_INTERFACE_FABRIC_VIDX_E:
            auMessagesPtr->macEntry.dstInterface.fabricVidx = 
                exMxPmAuMessagesPtr->fdbEntry.dstOutlif.interfaceInfo.fabricVidx;
            break;
        case CPSS_INTERFACE_INDEX_E:
            auMessagesPtr->macEntry.dstInterface.index = 
                exMxPmAuMessagesPtr->fdbEntry.dstOutlif.interfaceInfo.index;
            break;
        default:
            break;
    }

    auMessagesPtr->macEntry.key.entryType = exMxPmAuMessagesPtr->fdbEntry.key.entryType;

    /* Parse key */
    if (auMessagesPtr->macEntry.key.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E) 
    {
        auMessagesPtr->macEntry.key.key.macVlan.vlanId = 
            (GT_U16)exMxPmAuMessagesPtr->fdbEntry.key.key.macFid.fId;
        auMessagesPtr->macEntry.key.key.macVlan.macAddr.arEther[5] = 
            exMxPmAuMessagesPtr->fdbEntry.key.key.macFid.macAddr.arEther[5];
        auMessagesPtr->macEntry.key.key.macVlan.macAddr.arEther[4] = 
            exMxPmAuMessagesPtr->fdbEntry.key.key.macFid.macAddr.arEther[4];
        auMessagesPtr->macEntry.key.key.macVlan.macAddr.arEther[3] = 
            exMxPmAuMessagesPtr->fdbEntry.key.key.macFid.macAddr.arEther[3];
        auMessagesPtr->macEntry.key.key.macVlan.macAddr.arEther[2] = 
            exMxPmAuMessagesPtr->fdbEntry.key.key.macFid.macAddr.arEther[2];
        auMessagesPtr->macEntry.key.key.macVlan.macAddr.arEther[1] = 
            exMxPmAuMessagesPtr->fdbEntry.key.key.macFid.macAddr.arEther[1];
        auMessagesPtr->macEntry.key.key.macVlan.macAddr.arEther[0] = 
            exMxPmAuMessagesPtr->fdbEntry.key.key.macFid.macAddr.arEther[0];
    }
    else
    {
        auMessagesPtr->macEntry.key.key.ipMcast.vlanId = 
            (GT_U16)exMxPmAuMessagesPtr->fdbEntry.key.key.ipMcast.fId;

        cpssOsMemCpy(auMessagesPtr->macEntry.key.key.ipMcast.dip, 
                     exMxPmAuMessagesPtr->fdbEntry.key.key.ipMcast.dip, 4);
        cpssOsMemCpy(auMessagesPtr->macEntry.key.key.ipMcast.sip, 
                     exMxPmAuMessagesPtr->fdbEntry.key.key.ipMcast.sip, 4);
    }

    return GT_OK;
}
#endif /* EXMXPM_FAMILY */

/*******************************************************************************
* prvTgfBrgFdbAuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE  - the action succeeded and there are no more waiting
*                     AU messages
*
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       for multi-port groups device :
*           1. Unified FDB mode:
*              portGroup indication is required , for:
*                 a. Aging calculations of Trunks
*                 b. Aging calculations of regular entries when DA refresh is enabled.
*              In these 2 cases entries are considered as aged-out only if AA is
*              receives from all 4 portGroups.
*           2. In Unified-Linked FDB mode and Linked FDB mode:
*              portGroup indication is required for these reasons:
*                 a. Aging calculations of Trunk and regular entries which have
*                    been written to multiple portGroups.
*                 b. New addresses (NA) source portGroup indication is required
*                    so application can add new entry to the exact portGroup
*                    which saw this station (especially on trunk entries).
*                 c. Indication on which portGroup has removed this address when
*                    AA of delete is fetched (relevant when same entry was written
*                    to multiple portGroups).
*                 d. Indication on which portGroup currently has this address
*                    when QR (query reply) is being fetched (relevant when same
*                    entry was written to multiple portGroups).
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbAuMsgBlockGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfAuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagesPtr
)
{
    GT_STATUS   rc;

#ifdef CHX_FAMILY
    GT_U32      ii;

    /* call device specific API */
    rc = cpssDxChBrgFdbAuMsgBlockGet(devNum, numOfAuPtr, auMessagesPtr);
    if ((rc != GT_OK) && (rc != GT_NO_MORE))
    {
        PRV_UTF_LOG1_MAC("[TGF]: cpssDxChBrgFdbAuMsgBlockGet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* convert device numbers */
    for (ii = 0; ii < *numOfAuPtr; ii++)
    {
        /* convert device number from HW to SW devNum.*/
        rc = prvUtfSwFromHwDeviceNumberGet(auMessagesPtr[ii].associatedDevNum, &(auMessagesPtr[ii].associatedDevNum));
        if(GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: prvUtfSwFromHwDeviceNumberGet FAILED, rc = [%d]", rc);
            return rc;
        }

        if (auMessagesPtr[ii].macEntry.dstInterface.type == CPSS_INTERFACE_PORT_E)
        {
            rc = prvUtfSwFromHwDeviceNumberGet(auMessagesPtr[ii].macEntry.dstInterface.devPort.devNum, &(auMessagesPtr[ii].macEntry.dstInterface.devPort.devNum));
            if(GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvUtfSwFromHwDeviceNumberGet FAILED, rc = [%d]", rc);
                return rc;
            }
        }

        if (auMessagesPtr[ii].isMoved == GT_TRUE)
        {
            rc = prvUtfSwFromHwDeviceNumberGet(auMessagesPtr[ii].oldAssociatedDevNum, &(auMessagesPtr[ii].oldAssociatedDevNum));
            if(GT_OK != rc)
            {
                PRV_UTF_LOG1_MAC("[TGF]: prvUtfSwFromHwDeviceNumberGet FAILED, rc = [%d]", rc);
                return rc;
            }

            if (auMessagesPtr[ii].oldDstInterface.type == CPSS_INTERFACE_PORT_E)
            {
                rc = prvUtfSwFromHwDeviceNumberGet(auMessagesPtr[ii].oldDstInterface.devPort.devNum, &(auMessagesPtr[ii].oldDstInterface.devPort.devNum));
                if(GT_OK != rc)
                {
                    PRV_UTF_LOG1_MAC("[TGF]: prvUtfSwFromHwDeviceNumberGet FAILED, rc = [%d]", rc);
                    return rc;
                }
            }
        }
    }
    
    return GT_OK;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC     *exMxPmAuMessagesPtr;
    GT_U32                                   i;

   /* allocate memory */
    exMxPmAuMessagesPtr = cpssOsMalloc((*numOfAuPtr) * sizeof(CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC));

    /* reset variables */
    cpssOsMemSet(exMxPmAuMessagesPtr, 0,
                 (*numOfAuPtr) * sizeof(CPSS_EXMXPM_FDB_UPDATE_MSG_ENTRY_STC));

    /* call device specific API */
    rc = cpssExMxPmBrgFdbAuMsgBlockGet(devNum, numOfAuPtr, exMxPmAuMessagesPtr);
    if (rc != GT_OK) 
    {
        cpssOsFree(exMxPmAuMessagesPtr);
        PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmBrgGenPortRateLimitSet FAILED, rc = [%d]", rc);
        return rc;
    }

    for (i = 0; i < (*numOfAuPtr); i++) 
    {
        /* Parse AU message */
        rc = prvTgfBrgFdbAuMsgConvertFromExMxPmToGeneric(&(exMxPmAuMessagesPtr[i]), 
                                                         &(auMessagesPtr[i]));
        if (rc != GT_OK) 
        {
            cpssOsFree(exMxPmAuMessagesPtr);
            PRV_UTF_LOG1_MAC("[TGF]: prvTgfBrgFdbParseAuMsgFromExMxPmToGeneric FAILED, rc = [%d]", rc);
            return rc;
        }
    }
    cpssOsFree(exMxPmAuMessagesPtr);

    return rc;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvTgfBrgFdbNaMsgVid1EnableSet
*
* DESCRIPTION:
*       Enable/Disable Tag1 VLAN Id assignment in vid1 field of the NA AU 
*       message CPSS_MAC_UPDATE_MSG_EXT_STC.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE: vid1 field of the NA AU message is taken from 
*                          Tag1 VLAN. 
*                 GT_FALSE: vid1 field of the NA AU message is taken from 
*                          Tag0 VLAN. 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfBrgFdbNaMsgVid1EnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChBrgFdbNaMsgVid1EnableSet(devNum, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(enable);

    /* call device specific API */
    return GT_NOT_SUPPORTED;
#endif /* EXMXPM_FAMILY */

#if (!defined CHX_FAMILY) && (!defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif
}
