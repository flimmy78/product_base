/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgVlanUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChBrgVlan, that provides
*       CPSS DxCh VLAN facility implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define BRG_VLAN_VALID_PHY_PORT_CNS    0

/* Default invalid value for port id */
#define BRG_VLAN_INVALID_PHY_PORT_CNS  0xFE

/* Tests use this vlan id for testing VLAN functions            */
#define BRG_VLAN_TESTED_VLAN_ID_CNS  100

/* Invalid vlan id for testing VLAN functions*/
#define BRG_VLAN_INVALID_VLAN_ID_CNS 0xAFFF

/* check if device supports TR101 */
#define IS_TR101_SUPPORTED_MAC(_devNum) \
    (PRV_CPSS_DXCH_PP_MAC(_devNum)->fineTuning.featureInfo.tr101Supported != GT_FALSE)

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    IN CPSS_PORTS_BMP_STC           *portsMembersPtr,
    IN CPSS_PORTS_BMP_STC           *portsTaggingPtr,
    IN CPSS_DXCH_BRG_VLAN_INFO_STC  *vlanInfoPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanEntryWrite)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100],
                   portsMemberPtr {[0, 16]},
                   portsTaggingPtr {[0, 16]},
                   vlanInfoPtr {unkSrcAddrSecBreach [GT_FALSE],
                                unregNonIpMcastCmd [CPSS_PACKET_CMD_FORWARD_E],
                                unregIpv4McastCmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                unregIpv6McastCmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                                unkUcastCmd [CPSS_PACKET_CMD_DROP_HARD_E],
                                unregIpv4BcastCmd [CPSS_PACKET_CMD_DROP_SOFT_E],
                                unregNonIpv4BcastCmd [CPSS_PACKET_CMD_FORWARD_E],
                                ipv4IgmpToCpuEn [GT_TRUE],
                                mirrToRxAnalyzerEn [GT_TRUE],
                                ipv6IcmpToCpuEn[GT_TRUE],
                                ipCtrlToCpuEn [CPSS_DXCH_BRG_IP_CTRL_NONE_E],
                                ipv4IpmBrgMode [CPSS_BRG_IPM_SGV_E],
                                ipv6IpmBrgMode [CPSS_BRG_IPM_GV_E],
                                ipv4IpmBrgEn[GT_TRUE],
                                ipv6IpmBrgEn [GT_TRUE],
                                ipv6SiteIdMode [CPSS_IP_SITE_ID_INTERNAL_E],
                                ipv4UcastRouteEn[GT_FALSE],
                                ipv4McastRouteEn [GT_FALSE],
                                ipv6UcastRouteEn[GT_FALSE],
                                ipv6McastRouteEn[GT_FALSE],
                                stgId[0],
                                autoLearnDisable [GT_FALSE],
                                naMsgToCpuEn [GT_FALSE],
                                mruIdx[0],
                                bcastUdpTrapMirrEn [GT_FALSE],
                                vrfId [0]}.
    Expected: GT_OK.
    1.2. Call cpssDxChBrgVlanEntryRead with the same vlanId.
    Expected: GT_OK and the same portsMembersPtr, portsTaggingPtr and vlanInfoPtr.
            Check vlanInfo by fields:
            1) skip ipv6SiteIdMode, ipv4McastRateEn,
            ipv6McastRateEn, autoLearnDisable, naMsgToCpuEn, mruIdx, bcastUdpTrapMirrEn
            fields for Cheetah, but they should be checked for DxCh2, DxCh3;
            2) ipv4IpmBrgMode and ipv6IpnBrgMode should be checked only if
            corresponding ipv4/6IpmBrgEn = GT_TRUE.
    1.3. Call with out of range ipv4IpmBrgMode,
                   ipv4IpmBrgEn [GT_FALSE] (in this case ipv4IpmBrgMode is not relevant)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.4. Call with out of range ipv4IpmBrgMode,
                   ipv4IpmBrgEn [GT_TRUE] (in this case ipv4IpmBrgMode is relevant)
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range ipv6IpmBrgMode,
                   ipv6IpmBrgEn [GT_FALSE] (in this case ipv6IpmBrgMode is not relevant)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.6. Call with out of range ipv6IpmBrgMode,
                   ipv6IpmBrgEn [GT_TRUE] (in this case ipv6IpmBrgMode is relevant)
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with ipCtrlToCpuEn [CPSS_DXCH_BRG_IP_CTRL_IPV4_E]
                   and other parameters from 1.1.
    Expected: GT_OK for DxCh2, DxCh3 and NOT GT_OK for others.
    1.8. Call with ipCtrlToCpuEn [CPSS_DXCH_BRG_IP_CTRL_IPV6_E]
                   and other parameters from 1.1.
    Expected: GT_OK for DxCh2, DxCh3 and NOT GT_OK for others.
    1.9. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.10. Call with out of range unregNonIpMcastCmd
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.11. Call with out of range unregIpv4McastCmd
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.12. Call with out of range unregIpv6McastCmd
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.13. Call with out of range unkUcastCmd
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.14. Call with out of range unregIpv4BcastCmd
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.15. Call with out of range unregNonIpv4BcastCmd
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.16. Call with out of range ipCtrlToCpuEn
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.17. Call with out of range ipv6SiteIdMode
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM for DxCh2, DxCh3 and GT_OK for others
                (supported only by Cheetah2,3).
    1.18. Call with vlanId [100]
                    and portsMembersPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.19. Call with vlanId [100]
                    and portsTaggingPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.20. Call with vlanId [100]
                    and vlanInfoPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.21. Call with out of range stgId [256]
                    and other parameters from 1.1.
    Expected: NON GT_OK.
    1.22. Call with mruIdx [PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS+1=8]
          (no constraints by contract) and other parameters from 1.1.
    Expected: GT_OK.
    1.23. Call with out of range vrfId [4096]
                    and other parameters from 1.1.
    Expected: NON GT_OK for DxCh3 and GT_OK for others (supported only by Cheetah3).
*/
    GT_STATUS   st     = GT_OK;
    GT_U32      index  = 0;

    GT_U8                       dev;
    GT_U16                      vlanId = 0;
    CPSS_PORTS_BMP_STC          portsMembers;
    CPSS_PORTS_BMP_STC          portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC vlanInfo;
    CPSS_PORTS_BMP_STC          portsMembersGet;
    CPSS_PORTS_BMP_STC          portsTaggingGet;
    CPSS_DXCH_BRG_VLAN_INFO_STC vlanInfoGet;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr = NULL;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmdGet;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdGetPtr;
    GT_BOOL                     isValid   = GT_FALSE;
    GT_BOOL                     isEqual   = GT_FALSE;
    CPSS_PP_FAMILY_TYPE_ENT     devFamily = CPSS_PP_FAMILY_CHEETAH_E;

    cpssOsBzero((GT_VOID*) &portsMembers, sizeof(portsMembers));
    cpssOsBzero((GT_VOID*) &portsTagging, sizeof(portsTagging));
    cpssOsBzero((GT_VOID*) &vlanInfo, sizeof(vlanInfo));
    cpssOsBzero((GT_VOID*) &portsTaggingCmd, sizeof(portsTaggingCmd));
    cpssOsBzero((GT_VOID*) &portsMembersGet, sizeof(portsMembersGet));
    cpssOsBzero((GT_VOID*) &portsTaggingGet, sizeof(portsTaggingGet));
    cpssOsBzero((GT_VOID*) &vlanInfoGet, sizeof(vlanInfoGet));
    cpssOsBzero((GT_VOID*) &portsTaggingCmdGet, sizeof(portsTaggingCmdGet));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /*
            1.1. Call with vlanId [100],
                          portsMemberPtr {[0, 16]},
                          portsTaggingPtr {[0, 16]},
                          vlanInfoPtr {unkSrcAddrSecBreach [GT_FALSE],
                                unregNonIpMcastCmd [CPSS_PACKET_CMD_FORWARD_E],
                                unregIpv4McastCmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                unregIpv6McastCmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                                unkUcastCmd [CPSS_PACKET_CMD_DROP_HARD_E],
                                unregIpv4BcastCmd [CPSS_PACKET_CMD_DROP_SOFT_E],
                                unregNonIpv4BcastCmd [CPSS_PACKET_CMD_FORWARD_E],
                                ipv4IgmpToCpuEn [GT_TRUE],
                                mirrToRxAnalyzerEn [GT_TRUE],
                                ipv6IcmpToCpuEn[GT_TRUE],
                                ipCtrlToCpuEn [CPSS_DXCH_BRG_IP_CTRL_NONE_E],
                                ipv4IpmBrgMode [CPSS_BRG_IPM_SGV_E],
                                ipv6IpmBrgMode [CPSS_BRG_IPM_GV_E],
                                ipv4IpmBrgEn[GT_TRUE],
                                ipv6IpmBrgEn [GT_TRUE],
                                ipv6SiteIdMode [CPSS_IP_SITE_ID_INTERNAL_E],
                                ipv4UcastRouteEn[GT_FALSE],
                                ipv4McastRouteEn [GT_FALSE],
                                ipv6UcastRouteEn[GT_FALSE],
                                ipv6McastRouteEn[GT_FALSE],
                                stgId[0],
                                autoLearnDisable [GT_FALSE],
                                naMsgToCpuEn [GT_FALSE],
                                mruIdx[0],
                                bcastUdpTrapMirrEn [GT_FALSE]}.
            Expected: GT_OK.
        */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        portsMembers.ports[0] = 17;
        portsMembers.ports[1] = 0;

        /* portsTagging relevant for TR101 disabled devices */
        portsTagging.ports[0] = 17;
        portsTagging.ports[1] = 0;

        if (IS_TR101_SUPPORTED_MAC(dev))
        {
            /* portsTaggingCmd relevant for TR101 enabled devices */
            portsTaggingCmd.portsCmd[0] = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;
            portsTaggingCmd.portsCmd[1] = CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E;
            portsTaggingCmd.portsCmd[2] = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E;
            portsTaggingCmd.portsCmd[3] = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E;
            portsTaggingCmd.portsCmd[8] = CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E;
            portsTaggingCmd.portsCmd[9] = CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E;
            portsTaggingCmd.portsCmd[17] = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;
            portsTaggingCmd.portsCmd[22] = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E;

            /* port tagging CMD is used */
            portsTaggingCmdPtr = &portsTaggingCmd;
            portsTaggingCmdGetPtr = &portsTaggingCmdGet;
        }
        else
        {
            /* port tagging CMD should be ignored */
            CPSS_COVERITY_NON_ISSUE_BOOKMARK
            /* coverity[assign_zero] */
            portsTaggingCmdPtr = portsTaggingCmdGetPtr = NULL;
        }

        vlanInfo.unkSrcAddrSecBreach  = GT_FALSE;
        vlanInfo.unregNonIpMcastCmd   = CPSS_PACKET_CMD_FORWARD_E;
        vlanInfo.unregIpv4McastCmd    = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        vlanInfo.unregIpv6McastCmd    = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
        vlanInfo.unkUcastCmd          = CPSS_PACKET_CMD_DROP_HARD_E;
        vlanInfo.unregIpv4BcastCmd    = CPSS_PACKET_CMD_DROP_SOFT_E;
        vlanInfo.unregNonIpv4BcastCmd = CPSS_PACKET_CMD_FORWARD_E;
        vlanInfo.ipv4IgmpToCpuEn      = GT_TRUE;
        vlanInfo.mirrToRxAnalyzerEn   = GT_TRUE;
        vlanInfo.ipv6IcmpToCpuEn      = GT_TRUE;
        vlanInfo.ipCtrlToCpuEn        = CPSS_DXCH_BRG_IP_CTRL_NONE_E;
        vlanInfo.ipv4IpmBrgMode       = CPSS_BRG_IPM_SGV_E;
        vlanInfo.ipv6IpmBrgMode       = CPSS_BRG_IPM_GV_E;
        vlanInfo.ipv4IpmBrgEn         = GT_TRUE;
        vlanInfo.ipv6IpmBrgEn         = GT_TRUE;
        vlanInfo.ipv6SiteIdMode       = CPSS_IP_SITE_ID_INTERNAL_E;
        vlanInfo.ipv4UcastRouteEn     = GT_FALSE;
        vlanInfo.ipv4McastRouteEn     = GT_FALSE;
        vlanInfo.ipv6UcastRouteEn     = GT_FALSE;
        vlanInfo.ipv6McastRouteEn     = GT_FALSE;
        vlanInfo.stgId                = 17;
        vlanInfo.autoLearnDisable     = GT_FALSE;
        vlanInfo.naMsgToCpuEn         = GT_FALSE;
        vlanInfo.mruIdx               = 0;
        vlanInfo.bcastUdpTrapMirrEn   = GT_FALSE;
        vlanInfo.vrfId                = 100;
        vlanInfo.mcastLocalSwitchingEn = GT_TRUE;
        vlanInfo.portIsolationMode    = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E;


        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[var_deref_model] */
        st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                       &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /*
            1.2. Call cpssDxChBrgVlanEntryRead with the same vlanId.
            Expected: GT_OK and the same portsMembersPtr, portsTaggingPtr and vlanInfoPtr.
                     Check vlanInfo by fields: 1) skip ipv6SiteIdMode, ipv4McastRateEn,
                                                 ipv6McastRateEn, autoLearnDisable, naMsgToCpuEn,
                                                 mruIdx, bcastUdpTrapMirrEn fields for Cheetah,
                                                 but they should be checked for DxCh2, DxCh3;
                                              2) ipv4IpmBrgMode and ipv6IpnBrgMode should be
                                                 checked only if  corresponding
                                                 ipv4/6IpmBrgEn = GT_TRUE.
        */
        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[var_deref_model] */ st = cpssDxChBrgVlanEntryRead(dev, vlanId, &portsMembersGet, &portsTaggingGet,
                                                                      &vlanInfoGet, &isValid, portsTaggingCmdGetPtr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgVlanEntryRead: %d, %d", dev, vlanId);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isValid,
                "Vlan entry is NOT valid: %d, %d", dev, vlanId);

        if (GT_TRUE == isValid)
        {
            /* Verifying portsMembersPtr */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsMembers,
                                         (GT_VOID*) &portsMembersGet,
                                         sizeof(portsMembers))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                       "get another portsMembersPtr than was set: %d, %d", dev, vlanId);

            if (IS_TR101_SUPPORTED_MAC(dev))
            {
                /* Verifying portsTaggingCmd */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsTaggingCmd,
                                             (GT_VOID*) &portsTaggingCmdGet,
                                             sizeof(portsTaggingCmd))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                           "get another portsTaggingCmd than was set: %d, %d", dev, vlanId);
            }
            else
            {
                /* Verifying portsTaggingPtr */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsTagging,
                                             (GT_VOID*) &portsTaggingGet,
                                             sizeof(portsTagging))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                           "get another portsTaggingPtr than was set: %d, %d", dev, vlanId);
            }

            /* Verifying vlanInfoPtr fields */
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unkSrcAddrSecBreach,
                                         vlanInfoGet.unkSrcAddrSecBreach,
             "get another vlanInfoPtr->unkSrcAddrSecBreach than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregNonIpMcastCmd,
                                         vlanInfoGet.unregNonIpMcastCmd,
             "get another vlanInfoPtr->unregNonIpMcastCmd than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregIpv4McastCmd,
                                         vlanInfoGet.unregIpv4McastCmd,
             "get another vlanInfoPtr->unregIpv4McastCmd than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregIpv6McastCmd,
                                         vlanInfoGet.unregIpv6McastCmd,
             "get another vlanInfoPtr->unregIpv6McastCmd than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unkUcastCmd,
                                         vlanInfoGet.unkUcastCmd,
             "get another vlanInfoPtr->unkUcastCmd than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregIpv4BcastCmd,
                                         vlanInfoGet.unregIpv4BcastCmd,
             "get another vlanInfoPtr->unregIpv4BcastCmd than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregNonIpv4BcastCmd,
                                         vlanInfoGet.unregNonIpv4BcastCmd,
             "get another vlanInfoPtr->unregNonIpv4BcastCmd than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IgmpToCpuEn,
                                         vlanInfoGet.ipv4IgmpToCpuEn,
             "get another vlanInfoPtr->ipv4IgmpToCpuEn than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.mirrToRxAnalyzerEn,
                                         vlanInfoGet.mirrToRxAnalyzerEn,
             "get another vlanInfoPtr->mirrToRxAnalyzerEn than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IcmpToCpuEn,
                                         vlanInfoGet.ipv6IcmpToCpuEn,
             "get another vlanInfoPtr->ipv6IcmpToCpuEn than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipCtrlToCpuEn,
                                         vlanInfoGet.ipCtrlToCpuEn,
             "get another vlanInfoPtr->ipCtrlToCpuEn than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IpmBrgEn,
                                         vlanInfoGet.ipv4IpmBrgEn,
             "get another vlanInfoPtr->ipv4IpmBrgEn than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IpmBrgEn,
                                         vlanInfoGet.ipv6IpmBrgEn,
             "get another vlanInfoPtr->ipv6IpmBrgEn than was set: %d, %d", dev, vlanId);

            if (GT_TRUE == vlanInfo.ipv4IpmBrgEn)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IpmBrgMode,
                                             vlanInfoGet.ipv4IpmBrgMode,
                 "get another vlanInfoPtr->ipv4IpmBrgMode than was set: %d, %d", dev, vlanId);
            }

            if (GT_TRUE == vlanInfo.ipv6IpmBrgEn)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IpmBrgMode,
                                             vlanInfoGet.ipv6IpmBrgMode,
                 "get another vlanInfoPtr->ipv6IpmBrgMode than was set: %d, %d", dev, vlanId);
            }

            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4UcastRouteEn,
                                         vlanInfoGet.ipv4UcastRouteEn,
             "get another vlanInfoPtr->ipv4UcastRouteEn than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6UcastRouteEn,
                                         vlanInfoGet.ipv6UcastRouteEn,
             "get another vlanInfoPtr->ipv6UcastRouteEn than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.stgId,
                                         vlanInfoGet.stgId,
             "get another vlanInfoPtr->stgId than was set: %d, %d", dev, vlanId);

            if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6SiteIdMode,
                                             vlanInfoGet.ipv6SiteIdMode,
                 "get another vlanInfoPtr->ipv6SiteIdMode than was set: %d, %d", dev, vlanId);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4McastRouteEn,
                                             vlanInfoGet.ipv4McastRouteEn,
                 "get another vlanInfoPtr->ipv4McastRouteEn than was set: %d, %d", dev, vlanId);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6McastRouteEn,
                                             vlanInfoGet.ipv6McastRouteEn,
                 "get another vlanInfoPtr->ipv6McastRouteEn than was set: %d, %d", dev, vlanId);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.autoLearnDisable,
                                             vlanInfoGet.autoLearnDisable,
                 "get another vlanInfoPtr->autoLearnDisable than was set: %d, %d", dev, vlanId);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.naMsgToCpuEn,
                                             vlanInfoGet.naMsgToCpuEn,
                 "get another vlanInfoPtr->naMsgToCpuEn than was set: %d, %d", dev, vlanId);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.mruIdx,
                                             vlanInfoGet.mruIdx,
                 "get another vlanInfoPtr->mruIdx than was set: %d, %d", dev, vlanId);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.bcastUdpTrapMirrEn,
                                             vlanInfoGet.bcastUdpTrapMirrEn,
                 "get another vlanInfoPtr->bcastUdpTrapMirrEn than was set: %d, %d", dev, vlanId);
            }

            if(devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.mcastLocalSwitchingEn,
                                             vlanInfoGet.mcastLocalSwitchingEn,
                 "get another vlanInfoPtr->mcastLocalSwitchingEn than was set: %d, %d", dev, vlanId);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.portIsolationMode,
                                             vlanInfoGet.portIsolationMode,
                 "get another vlanInfoPtr->portIsolationMode than was set: %d, %d", dev, vlanId);
            }
        }

        /* VRF ID supported starting from DxCh3 but xCat2 does not support it */
        if ((devFamily >= CPSS_PP_FAMILY_CHEETAH3_E) && (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.vrfId, vlanInfoGet.vrfId,
                 "get another vlanInfoPtr->vrfId than was set: %d, %d", dev, vlanId);
        }

        /*
            1.3. Call with wrong enum values ipv4IpmBrgMode,
                          ipv4IpmBrgEn [GT_FALSE] (in this case ipv4IpmBrgMode is not relevant)
                          and other parameters from 1.1.
            Expected: GT_OK.
        */
        vlanInfo.ipv4IpmBrgEn = GT_FALSE;

        for(index = 0; index < utfInvalidEnumArrSize; index++)
        {
            vlanInfo.ipv4IpmBrgMode = utfInvalidEnumArr[index];

            st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                           &portsTagging, &vlanInfo, portsTaggingCmdPtr);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, vlanInfoPtr->ipv4IpmBrgMode = %d, vlanInfoPtr->ipv4IpmBrgEn = %d",
                                         dev, vlanInfo.ipv4IpmBrgMode, vlanInfo.ipv4IpmBrgEn);
        }

        vlanInfo.ipv4IpmBrgMode = CPSS_BRG_IPM_SGV_E;

        /*
            1.4. Call with wrong enum values ipv4IpmBrgMode,
                           ipv4IpmBrgEn [GT_TRUE] (in this case ipv4IpmBrgMode is relevant)
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanInfo.ipv4IpmBrgEn = GT_TRUE;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntryWrite
                                                        (dev, vlanId, &portsMembers, &portsTagging,
                             &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.ipv4IpmBrgMode);

        /*
            1.5. Call with wrong enum values ipv6IpmBrgMode,
                          ipv6IpmBrgEn [GT_FALSE] (in this case ipv6IpmBrgMode is not relevant)
                          and other parameters from 1.1.
            Expected: GT_OK.
        */
        vlanInfo.ipv6IpmBrgEn = GT_FALSE;

        for(index = 0; index < utfInvalidEnumArrSize; index++)
        {
            vlanInfo.ipv6IpmBrgMode = utfInvalidEnumArr[index];

            st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                           &portsTagging, &vlanInfo, portsTaggingCmdPtr);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, vlanInfoPtr->ipv6IpmBrgMode = %d, vlanInfoPtr->ipv6IpmBrgEn = %d",
                                         dev, vlanInfo.ipv6IpmBrgMode, vlanInfo.ipv6IpmBrgEn);
        }

        vlanInfo.ipv6IpmBrgMode = CPSS_BRG_IPM_GV_E;

        /*
            1.6. Call with out of range ipv6IpmBrgMode,
                               ipv6IpmBrgEn [GT_TRUE] (in this case ipv6IpmBrgMode is relevant)
                               and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanInfo.ipv6IpmBrgEn = GT_TRUE;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntryWrite
                                                        (dev, vlanId, &portsMembers, &portsTagging,
                             &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.ipv6IpmBrgMode);

        vlanInfo.ipv6IpmBrgMode = CPSS_BRG_IPM_GV_E;

        /*
            1.7. Call with ipCtrlToCpuEn [CPSS_DXCH_BRG_IP_CTRL_IPV4_E]
                               and other parameters from 1.1.
            Expected: GT_OK for DxCh2, DxCh3 and NOT GT_OK for others.
        */
        vlanInfo.ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_IPV4_E;

        st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                       &portsTagging, &vlanInfo, portsTaggingCmdPtr);

        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->ipCtrlToCpuEn = %d",
                                         dev, vlanInfo.ipCtrlToCpuEn);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->ipCtrlToCpuEn = %d",
                                             dev, vlanInfo.ipCtrlToCpuEn);
        }

        /*
            1.8. Call with ipCtrlToCpuEn [CPSS_DXCH_BRG_IP_CTRL_IPV6_E]
                      and other parameters from 1.1.
            Expected: GT_OK for DxCh2, DxCh3 and NOT GT_OK for others.
        */
        vlanInfo.ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_IPV6_E;

        st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                       &portsTagging, &vlanInfo, portsTaggingCmdPtr);

        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->ipCtrlToCpuEn = %d",
                                         dev, vlanInfo.ipCtrlToCpuEn);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->ipCtrlToCpuEn = %d",
                                             dev, vlanInfo.ipCtrlToCpuEn);
        }

        vlanInfo.ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_NONE_E;

        /*
            1.9. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                       &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /*
            1.10. Call with wrong enum values unregNonIpMcastCmd and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntryWrite
                                                        (dev, vlanId, &portsMembers, &portsTagging,
                             &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.unregNonIpMcastCmd);

        vlanInfo.unregNonIpMcastCmd = CPSS_PACKET_CMD_FORWARD_E;

        /*
            1.11. Call with wrong enum values unregIpv4McastCmd and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntryWrite
                                                        (dev, vlanId, &portsMembers,
                                           &portsTagging, &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.unregIpv4McastCmd);

        vlanInfo.unregIpv4McastCmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        /*
            1.12. Call with wrong enum values unregIpv6McastCmd and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntryWrite
                                                        (dev, vlanId, &portsMembers, &portsTagging,
                             &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.unregIpv6McastCmd);

        vlanInfo.unregIpv6McastCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        /*
            1.13. Call with wrong enum values unkUcastCmd and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntryWrite
                                                        (dev, vlanId, &portsMembers, &portsTagging,
                             &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.unkUcastCmd);

        vlanInfo.unkUcastCmd = CPSS_PACKET_CMD_DROP_HARD_E;

        /*
            1.14. Call with wrong enum values unregIpv4BcastCmd and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntryWrite
                                                        (dev, vlanId, &portsMembers, &portsTagging,
                             &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.unregIpv4BcastCmd);

        vlanInfo.unregIpv4BcastCmd = CPSS_PACKET_CMD_DROP_SOFT_E;

        /*
            1.15. Call with wrong enum values unregNonIpv4BcastCmd and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntryWrite
                                                        (dev, vlanId, &portsMembers, &portsTagging,
                             &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.unregNonIpv4BcastCmd);

        vlanInfo.unregNonIpv4BcastCmd = CPSS_PACKET_CMD_FORWARD_E;

        /*
            1.16. Call with wrong enum values ipCtrlToCpuEn and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntryWrite
                                                        (dev, vlanId, &portsMembers, &portsTagging,
                             &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.ipCtrlToCpuEn);

        vlanInfo.ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_NONE_E;

        /*
            1.17. Call with wrong enum values ipv6SiteIdMode and other parameters from 1.1.
            Expected: GT_OK for Cheetah and xCat2
                      and GT_BAD_PARAM for others (supported only by Cheetah2).
        */
        if ((devFamily != CPSS_PP_FAMILY_CHEETAH_E) && (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntryWrite
                                (dev, vlanId, &portsMembers, &portsTagging,
                                 &vlanInfo, portsTaggingCmdPtr),
                                vlanInfo.ipv6SiteIdMode);
        }
        else
        {
            for(index = 0; index < utfInvalidEnumArrSize; index++)
            {
                vlanInfo.ipv6SiteIdMode = utfInvalidEnumArr[index];
                st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                               &portsTagging, &vlanInfo, portsTaggingCmdPtr);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->ipv6SiteIdMode = %d",
                                             dev, vlanInfo.ipv6SiteIdMode);
            }
        }

        vlanInfo.ipv6SiteIdMode = CPSS_IP_SITE_ID_INTERNAL_E;

        /*
            1.18. Call with vlanId [100] and portsMembersPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanEntryWrite(dev, vlanId, NULL,
                                       &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsMembersPtr = NULL", dev);

        /*
            1.19. Call with vlanId [100] and portsTaggingPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR for TR101 disabled devices,
                      GT_OK for TR101 enabled devices.
        */
        st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                       NULL, &vlanInfo, portsTaggingCmdPtr);
        if(IS_TR101_SUPPORTED_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, portsTaggingPtr = NULL", dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsTaggingPtr = NULL", dev);
        }

        /*
            1.19.1 Call with vlanId [100] and portsTaggingCmdPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR for TR101 enabled devices,
                      GT_OK for TR101 disabled devices.
        */
        st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                       &portsTagging, &vlanInfo, NULL);
        if(!IS_TR101_SUPPORTED_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, portsTaggingCmdPtr = NULL", dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsTaggingCmdPtr = NULL", dev);
        }

        /*
            1.20. Call with vlanId [100] and vlanInfoPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                       &portsTagging, NULL, portsTaggingCmdPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vlanInfoPtr = NULL", dev);

        /*
            1.21. Call with out of range stgId [256]
                                and other parameters from 1.1.
            Expected: NON GT_OK.
        */
        vlanInfo.stgId = 256;

        st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                       &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->stgId = %d",
                                         dev, vlanInfo.stgId);
        vlanInfo.stgId = 0;

        /*
            1.22. Call with mruIdx [PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS+1=8]
                      (no constraints by contract) and other parameters from 1.1.
            Expected: non GT_OK.
        */
        vlanInfo.mruIdx = 8;

        st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                       &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->mruIdx = %d",
                                     dev, vlanInfo.mruIdx);
        vlanInfo.mruIdx = 0;

        /*
            1.23. Call with out of range vrfId [4096]
                            and other parameters from 1.1.
            Expected: NON GT_OK for DxCh3 and above exclude xCat2
                      and GT_OK for others (supported only by Cheetah3 and above)
        */
        vlanInfo.vrfId = 4096;

        st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                       &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        if ((devFamily >= CPSS_PP_FAMILY_CHEETAH3_E) && (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->vrfId = %d",
                                             dev, vlanInfo.vrfId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->vrfId = %d",
                                         dev, vlanInfo.vrfId);
        }

        vlanInfo.vrfId = 5;

        if(devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            vlanInfo.mcastLocalSwitchingEn = GT_TRUE;

            for(vlanInfo.mcastLocalSwitchingEn = GT_FALSE ;
                vlanInfo.mcastLocalSwitchingEn <= GT_TRUE ;
                vlanInfo.mcastLocalSwitchingEn ++)
            {
                CPSS_COVERITY_NON_ISSUE_BOOKMARK
                /* coverity[var_deref_model] */
                st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                               &portsTagging, &vlanInfo, portsTaggingCmdPtr);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

                /*
                    1.2. Call cpssDxChBrgVlanEntryRead with the same vlanId.
                    Expected: GT_OK and the same portsMembersPtr, portsTaggingPtr and vlanInfoPtr.
                             Check vlanInfo by fields: 1) skip ipv6SiteIdMode, ipv4McastRateEn,
                                                         ipv6McastRateEn, autoLearnDisable, naMsgToCpuEn,
                                                         mruIdx, bcastUdpTrapMirrEn fields for Cheetah,
                                                         but they should be checked for DxCh2, DxCh3;
                                                      2) ipv4IpmBrgMode and ipv6IpnBrgMode should be
                                                         checked only if  corresponding
                                                         ipv4/6IpmBrgEn = GT_TRUE.
                */
                CPSS_COVERITY_NON_ISSUE_BOOKMARK
                /* coverity[var_deref_model] */ st = cpssDxChBrgVlanEntryRead(dev, vlanId, &portsMembersGet, &portsTaggingGet,
                                                                              &vlanInfoGet, &isValid, portsTaggingCmdGetPtr);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssDxChBrgVlanEntryRead: %d, %d", dev, vlanId);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isValid,
                        "Vlan entry is NOT valid: %d, %d", dev, vlanId);

                if (GT_TRUE == isValid)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.mcastLocalSwitchingEn,
                                                 vlanInfoGet.mcastLocalSwitchingEn,
                     "get another vlanInfoPtr->mcastLocalSwitchingEn than was set: %d, %d", dev, vlanId);
                }
            }

            vlanInfo.mcastLocalSwitchingEn = GT_FALSE;


            for(vlanInfo.portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E ;
                vlanInfo.portIsolationMode <= CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_L3_CMD_E ;
                vlanInfo.portIsolationMode ++)
            {
                CPSS_COVERITY_NON_ISSUE_BOOKMARK
                /* coverity[var_deref_model] */
                st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                               &portsTagging, &vlanInfo, portsTaggingCmdPtr);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

                /*
                    1.2. Call cpssDxChBrgVlanEntryRead with the same vlanId.
                    Expected: GT_OK and the same portsMembersPtr, portsTaggingPtr and vlanInfoPtr.
                             Check vlanInfo by fields: 1) skip ipv6SiteIdMode, ipv4McastRateEn,
                                                         ipv6McastRateEn, autoLearnDisable, naMsgToCpuEn,
                                                         mruIdx, bcastUdpTrapMirrEn fields for Cheetah,
                                                         but they should be checked for DxCh2, DxCh3;
                                                      2) ipv4IpmBrgMode and ipv6IpnBrgMode should be
                                                         checked only if  corresponding
                                                         ipv4/6IpmBrgEn = GT_TRUE.
                */
                CPSS_COVERITY_NON_ISSUE_BOOKMARK
                /* coverity[var_deref_model] */ st = cpssDxChBrgVlanEntryRead(dev, vlanId, &portsMembersGet, &portsTaggingGet,
                                                                              &vlanInfoGet, &isValid, portsTaggingCmdGetPtr);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssDxChBrgVlanEntryRead: %d, %d", dev, vlanId);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isValid,
                        "Vlan entry is NOT valid: %d, %d", dev, vlanId);

                if (GT_TRUE == isValid)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.portIsolationMode,
                                                 vlanInfoGet.portIsolationMode,
                     "get another vlanInfoPtr->portIsolationMode than was set: %d, %d", dev, vlanId);
                }
            }
            vlanInfo.portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E;

            UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntryWrite
                                (dev, vlanId, &portsMembers, &portsTagging,
                                 &vlanInfo, portsTaggingCmdPtr),
                                vlanInfo.portIsolationMode);
        }
        /*
           1.24. Call cpssDxChBrgVlanEntryInvalidate with the same
                 vlanId to invalidate all changes.
           Expected: GT_OK.
        */
        st = cpssDxChBrgVlanEntryInvalidate(dev, vlanId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgVlanEntryInvalidate: %d, %d", dev, vlanId);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

    portsMembers.ports[0] = 1000;
    portsMembers.ports[1] = 0;
    portsTagging.ports[0] = 1500;
    portsTagging.ports[1] = 0;

    vlanInfo.unkSrcAddrSecBreach  = GT_FALSE;
    vlanInfo.unregNonIpMcastCmd   = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregIpv4McastCmd    = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
    vlanInfo.unregIpv6McastCmd    = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    vlanInfo.unkUcastCmd          = CPSS_PACKET_CMD_DROP_HARD_E;
    vlanInfo.unregIpv4BcastCmd    = CPSS_PACKET_CMD_DROP_SOFT_E;
    vlanInfo.unregNonIpv4BcastCmd = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.ipv4IgmpToCpuEn      = GT_TRUE;
    vlanInfo.mirrToRxAnalyzerEn   = GT_TRUE;
    vlanInfo.ipv6IcmpToCpuEn      = GT_TRUE;
    vlanInfo.ipCtrlToCpuEn        = CPSS_DXCH_BRG_IP_CTRL_NONE_E;
    vlanInfo.ipv4IpmBrgMode       = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv6IpmBrgMode       = CPSS_BRG_IPM_GV_E;
    vlanInfo.ipv4IpmBrgEn         = GT_TRUE;
    vlanInfo.ipv6IpmBrgEn         = GT_TRUE;
    vlanInfo.ipv6SiteIdMode       = CPSS_IP_SITE_ID_INTERNAL_E;
    vlanInfo.ipv4UcastRouteEn     = GT_FALSE;
    vlanInfo.ipv4McastRouteEn     = GT_FALSE;
    vlanInfo.ipv6UcastRouteEn     = GT_FALSE;
    vlanInfo.ipv6McastRouteEn     = GT_FALSE;
    vlanInfo.stgId                = 0;
    vlanInfo.autoLearnDisable     = GT_FALSE;
    vlanInfo.naMsgToCpuEn         = GT_FALSE;
    vlanInfo.mruIdx               = 0;
    vlanInfo.bcastUdpTrapMirrEn   = GT_FALSE;
    vlanInfo.vrfId                = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                       &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                       &portsTagging, &vlanInfo, portsTaggingCmdPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanEntriesRangeWrite(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    IN GT_U32                       numOfEntries,
    IN CPSS_PORTS_BMP_STC           *portsMembersPtr,
    IN CPSS_PORTS_BMP_STC           *portsTaggingPtr,
    IN CPSS_DXCH_BRG_VLAN_INFO_STC  *vlanInfoPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanEntriesRangeWrite)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100],
                   numOfEntries[10],
                   portsMemberPtr {[0, 16]},
                   portsTaggingPtr {[0, 16]},
                   vlanInfoPtr {unkSrcAddrSecBreach [GT_FALSE],
                                unregNonIpMcastCmd [CPSS_PACKET_CMD_FORWARD_E],
                                unregIpv4McastCmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                unregIpv6McastCmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                                unkUcastCmd [CPSS_PACKET_CMD_DROP_HARD_E],
                                unregIpv4BcastCmd [CPSS_PACKET_CMD_DROP_SOFT_E],
                                unregNonIpv4BcastCmd [CPSS_PACKET_CMD_FORWARD_E],
                                ipv4IgmpToCpuEn [GT_TRUE],
                                mirrToRxAnalyzerEn [GT_TRUE],
                                ipv6IcmpToCpuEn[GT_TRUE],
                                ipCtrlToCpuEn [CPSS_DXCH_BRG_IP_CTRL_NONE_E],
                                ipv4IpmBrgMode [CPSS_BRG_IPM_SGV_E],
                                ipv6IpmBrgMode [CPSS_BRG_IPM_GV_E],
                                ipv4IpmBrgEn[GT_TRUE],
                                ipv6IpmBrgEn [GT_TRUE],
                                ipv6SiteIdMode [CPSS_IP_SITE_ID_INTERNAL_E],
                                ipv4UcastRouteEn[GT_FALSE],
                                ipv4McastRouteEn [GT_FALSE],
                                ipv6UcastRouteEn[GT_FALSE],
                                ipv6McastRouteEn[GT_FALSE],
                                stgId[0],
                                utoLearnDisable
                                [GT_FALSE],
                                naMsgToCpuEn
                                [GT_FALSE],
                                mruIdx[0],
                                bcastUdpTrapMirrEn [GT_FALSE],
                                vrfId [0]}.
    Expected: GT_OK.
    1.2. Call cpssDxChBrgVlanEntryRead with the same vlanId for each entry.
    Expected: GT_OK and the same portsMembersPtr, portsTaggingPtr and vlanInfoPtr.
    Check vlanInfo by fields: 1) skip ipv6SiteIdMode,
                                      ipv4McastRateEn,
                                      ipv6McastRateEn,
                                      autoLearnDisable,
                                      naMsgToCpuEn,
                                      mruIdx,
                                      bcastUdpTrapMirrEn fields for Cheetah,
                                      but they should be checked for Cheetah2;
                              2) ipv4IpmBrgMode and ipv6IpnBrgMode
                                 should be checked only if corresponding ipv4/6IpmBrgEn = GT_TRUE.
    1.3. Call with out of range ipv4IpmBrgMode,
                   ipv4IpmBrgEn [GT_FALSE] (in this case ipv4IpmBrgMode is not relevant)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.4. Call with out of range ipv4IpmBrgMode,
                   ipv4IpmBrgEn [GT_TRUE] (in this case ipv4IpmBrgMode is relevant)
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range ipv6IpmBrgMode,
                   ipv6IpmBrgEn [GT_FALSE] (in this case ipv6IpmBrgMode is not relevant)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.6. Call with out of range ipv6IpmBrgMode,
                   ipv6IpmBrgEn [GT_TRUE] (in this case ipv6IpmBrgMode is relevant)
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with ipCtrlToCpuEn [CPSS_DXCH_BRG_IP_CTRL_IPV4_E]
                   and other parameters from 1.1.
    Expected: GT_OK for DxCh2, DxCh3 and NOT GT_OK for others.
    1.8. Call with ipCtrlToCpuEn [CPSS_DXCH_BRG_IP_CTRL_IPV6_E]
                   and other parameters from 1.1.
    Expected: GT_OK for DxCh2, DxCh3 and NOT GT_OK for others.
    1.9. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.10. Call with vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS - 1],
                    numOfEntries [1] and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.11. Call with out of range unregNonIpMcastCmd
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.12. Call with out of range unregIpv4McastCmd
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.13. Call with out of range unregIpv6McastCmd
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.14. Call with out of range unkUcastCmd
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.15. Call with out of range unregIpv4BcastCmd
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.16. Call with out of range unregNonIpv4BcastCmd
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.17. Call with out of range ipCtrlToCpuEn
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.18. Call with out of range ipv6SiteIdMode
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM for DxCh2, DxCh3 and GT_OK for others (supported only by Cheetah2,3).
    1.19. Call with vlanId [100] and portsMembersPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.20. Call with vlanId [100] and portsTaggingPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.21. Call with vlanId [100] and vlanInfoPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.22. Call with out of range stgId [256]
                    and other parameters from 1.1.
    Expected: NON GT_OK.
    1.23. Call with mruIdx [PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS+1=8]
                    and other parameters from 1.1.
    Expected: non GT_OK.
    1.24. Call with out of range vrfId [4096]
                    and other parameters from 1.1.
    Expected: NON GT_OK for DxCh3 and GT_OK for others (supported only by Cheetah3)
*/
    GT_STATUS st     = GT_OK;
    GT_U32    index  = 0;

    GT_U8                       dev;
    GT_U16                      vlanId       = 0;
    GT_U32                      numOfEntries = 0;
    CPSS_PORTS_BMP_STC          portsMembers;
    CPSS_PORTS_BMP_STC          portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC vlanInfo;
    CPSS_PORTS_BMP_STC          portsMembersGet;
    CPSS_PORTS_BMP_STC          portsTaggingGet;
    CPSS_DXCH_BRG_VLAN_INFO_STC vlanInfoGet;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr = NULL;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmdGet;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdGetPtr;

    GT_BOOL                     isValid   = GT_FALSE;
    GT_BOOL                     isEqual   = GT_FALSE;
    CPSS_PP_FAMILY_TYPE_ENT     devFamily = CPSS_PP_FAMILY_CHEETAH_E;
    GT_U16                      vlanIdIterator = 0;


    cpssOsBzero((GT_VOID*) &portsMembers, sizeof(portsMembers));
    cpssOsBzero((GT_VOID*) &portsTagging, sizeof(portsTagging));
    cpssOsBzero((GT_VOID*) &vlanInfo, sizeof(vlanInfo));
    cpssOsBzero((GT_VOID*) &portsMembersGet, sizeof(portsMembersGet));
    cpssOsBzero((GT_VOID*) &portsTaggingGet, sizeof(portsTaggingGet));
    cpssOsBzero((GT_VOID*) &vlanInfoGet, sizeof(vlanInfoGet));
    cpssOsBzero((GT_VOID*) &portsTaggingCmd, sizeof(portsTaggingCmd));
    cpssOsBzero((GT_VOID*) &portsTaggingCmdGet, sizeof(portsTaggingCmdGet));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /*
            1.1. Call with vlanId [100],
                   numOfEntries[10],
                   portsMemberPtr {[0, 16]},
                   portsTaggingPtr {[0, 16]},
                   vlanInfoPtr {unkSrcAddrSecBreach [GT_FALSE],
                                unregNonIpMcastCmd [CPSS_PACKET_CMD_FORWARD_E],
                                unregIpv4McastCmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                unregIpv6McastCmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                                unkUcastCmd [CPSS_PACKET_CMD_DROP_HARD_E],
                                unregIpv4BcastCmd [CPSS_PACKET_CMD_DROP_SOFT_E],
                                unregNonIpv4BcastCmd [CPSS_PACKET_CMD_FORWARD_E],
                                ipv4IgmpToCpuEn [GT_TRUE],
                                mirrToRxAnalyzerEn [GT_TRUE],
                                ipv6IcmpToCpuEn[GT_TRUE],
                                ipCtrlToCpuEn [CPSS_DXCH_BRG_IP_CTRL_NONE_E],
                                ipv4IpmBrgMode [CPSS_BRG_IPM_SGV_E],
                                ipv6IpmBrgMode [CPSS_BRG_IPM_GV_E],
                                ipv4IpmBrgEn[GT_TRUE],
                                ipv6IpmBrgEn [GT_TRUE],
                                ipv6SiteIdMode [CPSS_IP_SITE_ID_INTERNAL_E],
                                ipv4UcastRouteEn[GT_FALSE],
                                ipv4McastRouteEn [GT_FALSE],
                                ipv6UcastRouteEn[GT_FALSE],
                                ipv6McastRouteEn[GT_FALSE],
                                stgId[0],
                                utoLearnDisable
                                [GT_FALSE],
                                naMsgToCpuEn
                                [GT_FALSE],
                                mruIdx[0],
                                bcastUdpTrapMirrEn [GT_FALSE],
                                vrfId [0]}.
            Expected: GT_OK.
        */
        vlanId       = BRG_VLAN_TESTED_VLAN_ID_CNS;
        numOfEntries = 10;

        portsMembers.ports[0] = 17;
        portsMembers.ports[1] = 0;
        portsTagging.ports[0] = 17;
        portsTagging.ports[1] = 0;

        if (IS_TR101_SUPPORTED_MAC(dev))
        {
            /* portsTaggingCmd relevant for TR101 enabled devices */
            portsTaggingCmd.portsCmd[0] = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;
            portsTaggingCmd.portsCmd[1] = CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E;
            portsTaggingCmd.portsCmd[2] = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E;
            portsTaggingCmd.portsCmd[3] = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E;
            portsTaggingCmd.portsCmd[8] = CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E;
            portsTaggingCmd.portsCmd[9] = CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E;
            portsTaggingCmd.portsCmd[17] = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;
            portsTaggingCmd.portsCmd[22] = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E;

            /* port tagging CMD is used */
            portsTaggingCmdPtr = &portsTaggingCmd;
            portsTaggingCmdGetPtr = &portsTaggingCmdGet;
        }
        else
        {
            /* port tagging CMD should be ignored */
            /* coverity[assign_zero] */ portsTaggingCmdPtr = portsTaggingCmdGetPtr = NULL;
        }

        vlanInfo.unkSrcAddrSecBreach  = GT_FALSE;
        vlanInfo.unregNonIpMcastCmd   = CPSS_PACKET_CMD_FORWARD_E;
        vlanInfo.unregIpv4McastCmd    = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        vlanInfo.unregIpv6McastCmd    = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
        vlanInfo.unkUcastCmd          = CPSS_PACKET_CMD_DROP_HARD_E;
        vlanInfo.unregIpv4BcastCmd    = CPSS_PACKET_CMD_DROP_SOFT_E;
        vlanInfo.unregNonIpv4BcastCmd = CPSS_PACKET_CMD_FORWARD_E;
        vlanInfo.ipv4IgmpToCpuEn      = GT_TRUE;
        vlanInfo.mirrToRxAnalyzerEn   = GT_TRUE;
        vlanInfo.ipv6IcmpToCpuEn      = GT_TRUE;
        vlanInfo.ipCtrlToCpuEn        = CPSS_DXCH_BRG_IP_CTRL_NONE_E;
        vlanInfo.ipv4IpmBrgMode       = CPSS_BRG_IPM_SGV_E;
        vlanInfo.ipv6IpmBrgMode       = CPSS_BRG_IPM_GV_E;
        vlanInfo.ipv4IpmBrgEn         = GT_TRUE;
        vlanInfo.ipv6IpmBrgEn         = GT_TRUE;
        vlanInfo.ipv6SiteIdMode       = CPSS_IP_SITE_ID_INTERNAL_E;
        vlanInfo.ipv4UcastRouteEn     = GT_FALSE;
        vlanInfo.ipv4McastRouteEn     = GT_FALSE;
        vlanInfo.ipv6UcastRouteEn     = GT_FALSE;
        vlanInfo.ipv6McastRouteEn     = GT_FALSE;
        vlanInfo.stgId                = 0;
        vlanInfo.autoLearnDisable     = GT_FALSE;
        vlanInfo.naMsgToCpuEn         = GT_FALSE;
        vlanInfo.mruIdx               = 0;
        vlanInfo.bcastUdpTrapMirrEn   = GT_FALSE;
        vlanInfo.vrfId                = 100;

        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[var_deref_model] */
        st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                              &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /*
            1.2. Call cpssDxChBrgVlanEntryRead with the same vlanId for each entry.
            Expected: GT_OK and the same portsMembersPtr, portsTaggingPtr and vlanInfoPtr.
            Check vlanInfo by fields: 1) skip ipv6SiteIdMode,
                                              ipv4McastRateEn,
                                              ipv6McastRateEn,
                                              autoLearnDisable,
                                              naMsgToCpuEn,
                                              mruIdx,
                                              bcastUdpTrapMirrEn fields for Cheetah,
                                              but they should be checked for Cheetah2;
                                      2) ipv4IpmBrgMode and ipv6IpnBrgMode
                                         should be checked only if corresponding ipv4/6IpmBrgEn = GT_TRUE.
        */
        for (vlanIdIterator = vlanId; vlanIdIterator < (vlanId + numOfEntries); vlanIdIterator++)
        {
            CPSS_COVERITY_NON_ISSUE_BOOKMARK
            /* coverity[var_deref_model] */
            st = cpssDxChBrgVlanEntryRead(dev, vlanIdIterator, &portsMembersGet, &portsTaggingGet,
                                          &vlanInfoGet, &isValid, portsTaggingCmdGetPtr);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgVlanEntryRead: %d, %d", dev, vlanIdIterator);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isValid,
                    "Vlan entry is NOT valid: %d, %d", dev, vlanIdIterator);

            if (GT_TRUE == isValid)
            {
                /* Verifying portsMembersPtr */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsMembers,
                                             (GT_VOID*) &portsMembersGet,
                                             sizeof(portsMembers))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                   "get another portsMembersPtr than was set: %d, %d", dev, vlanIdIterator);

                if (IS_TR101_SUPPORTED_MAC(dev))
                {
                    /* Verifying portsTaggingCmd */
                    isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsTaggingCmd,
                                                 (GT_VOID*) &portsTaggingCmdGet,
                                                 sizeof(portsTaggingCmd))) ? GT_TRUE : GT_FALSE;
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                               "get another portsTaggingCmd than was set: %d, %d", dev, vlanId);
                }
                else
                {
                    /* Verifying portsTaggingPtr */
                    isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsTagging,
                                                 (GT_VOID*) &portsTaggingGet,
                                                 sizeof(portsTagging))) ? GT_TRUE : GT_FALSE;
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                               "get another portsTaggingPtr than was set: %d, %d", dev, vlanId);
                }

                /* Verifying vlanInfoPtr fields */
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unkSrcAddrSecBreach,
                                             vlanInfoGet.unkSrcAddrSecBreach,
                 "get another vlanInfoPtr->unkSrcAddrSecBreach than was set: %d, %d", dev, vlanIdIterator);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregNonIpMcastCmd,
                                             vlanInfoGet.unregNonIpMcastCmd,
                 "get another vlanInfoPtr->unregNonIpMcastCmd than was set: %d, %d", dev, vlanIdIterator);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregIpv4McastCmd,
                                             vlanInfoGet.unregIpv4McastCmd,
                 "get another vlanInfoPtr->unregIpv4McastCmd than was set: %d, %d", dev, vlanIdIterator);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregIpv6McastCmd,
                                             vlanInfoGet.unregIpv6McastCmd,
                 "get another vlanInfoPtr->unregIpv6McastCmd than was set: %d, %d", dev, vlanIdIterator);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unkUcastCmd,
                                             vlanInfoGet.unkUcastCmd,
                 "get another vlanInfoPtr->unkUcastCmd than was set: %d, %d", dev, vlanIdIterator);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregIpv4BcastCmd,
                                             vlanInfoGet.unregIpv4BcastCmd,
                 "get another vlanInfoPtr->unregIpv4BcastCmd than was set: %d, %d", dev, vlanIdIterator);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregNonIpv4BcastCmd,
                                             vlanInfoGet.unregNonIpv4BcastCmd,
                 "get another vlanInfoPtr->unregNonIpv4BcastCmd than was set: %d, %d", dev, vlanIdIterator);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IgmpToCpuEn,
                                             vlanInfoGet.ipv4IgmpToCpuEn,
                 "get another vlanInfoPtr->ipv4IgmpToCpuEn than was set: %d, %d", dev, vlanIdIterator);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.mirrToRxAnalyzerEn,
                                             vlanInfoGet.mirrToRxAnalyzerEn,
                 "get another vlanInfoPtr->mirrToRxAnalyzerEn than was set: %d, %d", dev, vlanIdIterator);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IcmpToCpuEn,
                                             vlanInfoGet.ipv6IcmpToCpuEn,
                 "get another vlanInfoPtr->ipv6IcmpToCpuEn than was set: %d, %d", dev, vlanIdIterator);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipCtrlToCpuEn,
                                             vlanInfoGet.ipCtrlToCpuEn,
                 "get another vlanInfoPtr->ipCtrlToCpuEn than was set: %d, %d", dev, vlanIdIterator);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IpmBrgEn,
                                             vlanInfoGet.ipv4IpmBrgEn,
                 "get another vlanInfoPtr->ipv4IpmBrgEn than was set: %d, %d", dev, vlanIdIterator);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IpmBrgEn,
                                             vlanInfoGet.ipv6IpmBrgEn,
                 "get another vlanInfoPtr->ipv6IpmBrgEn than was set: %d, %d", dev, vlanIdIterator);

                if (GT_TRUE == vlanInfo.ipv4IpmBrgEn)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IpmBrgMode,
                                                 vlanInfoGet.ipv4IpmBrgMode,
                     "get another vlanInfoPtr->ipv4IpmBrgMode than was set: %d, %d", dev, vlanIdIterator);
                }

                if (GT_TRUE == vlanInfo.ipv6IpmBrgEn)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IpmBrgMode,
                                                 vlanInfoGet.ipv6IpmBrgMode,
                     "get another vlanInfoPtr->ipv6IpmBrgMode than was set: %d, %d", dev, vlanIdIterator);
                }

                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4UcastRouteEn,
                                             vlanInfoGet.ipv4UcastRouteEn,
                 "get another vlanInfoPtr->ipv4UcastRouteEn than was set: %d, %d", dev, vlanIdIterator);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6UcastRouteEn,
                                             vlanInfoGet.ipv6UcastRouteEn,
                 "get another vlanInfoPtr->ipv6UcastRouteEn than was set: %d, %d", dev, vlanIdIterator);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.stgId,
                                             vlanInfoGet.stgId,
                 "get another vlanInfoPtr->stgId than was set: %d, %d", dev, vlanIdIterator);

                if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
                {
                    if (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
                    {
                        UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6SiteIdMode,
                                                     vlanInfoGet.ipv6SiteIdMode,
                         "get another vlanInfoPtr->ipv6SiteIdMode than was set: %d, %d", dev, vlanIdIterator);
                        UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4McastRouteEn,
                                                     vlanInfoGet.ipv4McastRouteEn,
                         "get another vlanInfoPtr->ipv4McastRouteEn than was set: %d, %d", dev, vlanIdIterator);
                        UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6McastRouteEn,
                                                     vlanInfoGet.ipv6McastRouteEn,
                         "get another vlanInfoPtr->ipv6McastRouteEn than was set: %d, %d", dev, vlanIdIterator);
                    }
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.autoLearnDisable,
                                                 vlanInfoGet.autoLearnDisable,
                     "get another vlanInfoPtr->autoLearnDisable than was set: %d, %d", dev, vlanIdIterator);
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.naMsgToCpuEn,
                                                 vlanInfoGet.naMsgToCpuEn,
                     "get another vlanInfoPtr->naMsgToCpuEn than was set: %d, %d", dev, vlanIdIterator);
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.mruIdx,
                                                 vlanInfoGet.mruIdx,
                     "get another vlanInfoPtr->mruIdx than was set: %d, %d", dev, vlanIdIterator);
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.bcastUdpTrapMirrEn,
                                                 vlanInfoGet.bcastUdpTrapMirrEn,
                     "get another vlanInfoPtr->bcastUdpTrapMirrEn than was set: %d, %d", dev, vlanIdIterator);
                }

                if ((devFamily >= CPSS_PP_FAMILY_CHEETAH3_E) && (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.vrfId,
                                                 vlanInfoGet.vrfId,
                     "get another vlanInfoPtr->vrfId than was set: %d, %d", dev, vlanIdIterator);
                }
            }
        }

        /*
            1.3. Call with wrong enum values ipv4IpmBrgMode,
                          ipv4IpmBrgEn [GT_FALSE] (in this case ipv4IpmBrgMode is not relevant)
                          and other parameters from 1.1.
            Expected: GT_OK.
        */
        vlanInfo.ipv4IpmBrgEn = GT_FALSE;

        for(index = 0; index < utfInvalidEnumArrSize; index++)
        {
            vlanInfo.ipv4IpmBrgMode = utfInvalidEnumArr[index];

            st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                                  &portsTagging, &vlanInfo, portsTaggingCmdPtr);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, vlanInfoPtr->ipv4IpmBrgMode = %d, vlanInfoPtr->ipv4IpmBrgEn = %d",
                                         dev, vlanInfo.ipv4IpmBrgMode, vlanInfo.ipv4IpmBrgEn);
        }

        vlanInfo.ipv4IpmBrgMode = CPSS_BRG_IPM_SGV_E;

        /*
            1.4. Call with wrong enum values ipv4IpmBrgMode,
                           ipv4IpmBrgEn [GT_TRUE] (in this case ipv4IpmBrgMode is relevant)
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanInfo.ipv4IpmBrgEn = GT_TRUE;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntriesRangeWrite
                                                        (dev, vlanId, numOfEntries, &portsMembers,
                             &portsTagging, &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.ipv4IpmBrgMode);

        vlanInfo.ipv4IpmBrgMode = CPSS_BRG_IPM_SGV_E;

        /*
            1.5. Call with wrong enum values ipv6IpmBrgMode,
                          ipv6IpmBrgEn [GT_FALSE] (in this case ipv6IpmBrgMode is not relevant)
                          and other parameters from 1.1.
            Expected: GT_OK.
        */
        vlanInfo.ipv6IpmBrgEn = GT_FALSE;

        for(index = 0; index < utfInvalidEnumArrSize; index++)
        {
            vlanInfo.ipv6IpmBrgMode = utfInvalidEnumArr[index];

            st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                                  &portsTagging, &vlanInfo, portsTaggingCmdPtr);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "%d, vlanInfoPtr->ipv6IpmBrgMode = %d, vlanInfoPtr->ipv6IpmBrgEn = %d",
                                         dev, vlanInfo.ipv6IpmBrgMode, vlanInfo.ipv6IpmBrgEn);
        }

        vlanInfo.ipv6IpmBrgMode = CPSS_BRG_IPM_GV_E;

        /*
            1.6. Call with wrong enum values ipv6IpmBrgMode,
                               ipv6IpmBrgEn [GT_TRUE] (in this case ipv6IpmBrgMode is relevant)
                               and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanInfo.ipv6IpmBrgEn = GT_TRUE;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntriesRangeWrite
                                                        (dev, vlanId, numOfEntries, &portsMembers,
                             &portsTagging, &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.ipv6IpmBrgMode);

        vlanInfo.ipv6IpmBrgMode = CPSS_BRG_IPM_GV_E;

        /*
            1.7. Call with ipCtrlToCpuEn [CPSS_DXCH_BRG_IP_CTRL_IPV4_E]
                               and other parameters from 1.1.
            Expected: GT_OK for DxCh2, DxCh3 and NOT GT_OK for others.
        */
        vlanInfo.ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_IPV4_E;

        st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                              &portsTagging, &vlanInfo, portsTaggingCmdPtr);

        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->ipCtrlToCpuEn = %d",
                                         dev, vlanInfo.ipCtrlToCpuEn);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->ipCtrlToCpuEn = %d",
                                             dev, vlanInfo.ipCtrlToCpuEn);
        }

        /*
            1.8. Call with ipCtrlToCpuEn [CPSS_DXCH_BRG_IP_CTRL_IPV6_E]
                      and other parameters from 1.1.
            Expected: GT_OK for DxCh2, DxCh3 and NOT GT_OK for others.
        */
        vlanInfo.ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_IPV6_E;

        st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                              &portsTagging, &vlanInfo, portsTaggingCmdPtr);

        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->ipCtrlToCpuEn = %d",
                                         dev, vlanInfo.ipCtrlToCpuEn);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->ipCtrlToCpuEn = %d",
                                             dev, vlanInfo.ipCtrlToCpuEn);
        }

        vlanInfo.ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_NONE_E;

        /*
            1.9. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                              &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /*
            1.10. Call with vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS - 1],
                            numOfEntries [1] and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanId       = PRV_CPSS_MAX_NUM_VLANS_CNS - 1;
        numOfEntries = 2;

        st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                              &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, numOfEntries);

        vlanId       = BRG_VLAN_TESTED_VLAN_ID_CNS;
        numOfEntries = 10;

        /*
            1.11. Call with wrong enum values unregNonIpMcastCmd and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntriesRangeWrite
                                                        (dev, vlanId, numOfEntries, &portsMembers,
                             &portsTagging, &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.unregNonIpMcastCmd);

        vlanInfo.unregNonIpMcastCmd = CPSS_PACKET_CMD_FORWARD_E;

        /*
            1.12. Call with wrong enum values unregIpv4McastCmd and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntriesRangeWrite
                                                        (dev, vlanId, numOfEntries, &portsMembers,
                             &portsTagging, &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.unregIpv4McastCmd);

        vlanInfo.unregIpv4McastCmd = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        /*
            1.13. Call with wrong enum values unregIpv6McastCmd and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntriesRangeWrite
                                                        (dev, vlanId, numOfEntries, &portsMembers,
                             &portsTagging, &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.unregIpv6McastCmd);

        vlanInfo.unregIpv6McastCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        /*
            1.14. Call with wrong enum values unkUcastCmd and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntriesRangeWrite
                                                        (dev, vlanId, numOfEntries, &portsMembers,
                             &portsTagging, &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.unkUcastCmd);

        vlanInfo.unkUcastCmd = CPSS_PACKET_CMD_DROP_HARD_E;

        /*
            1.15. Call with wrong enum values unregIpv4BcastCmd and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntriesRangeWrite
                                                        (dev, vlanId, numOfEntries, &portsMembers,
                             &portsTagging, &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.unregIpv4BcastCmd);

        vlanInfo.unregIpv4BcastCmd = CPSS_PACKET_CMD_DROP_SOFT_E;

        /*
            1.16. Call with wrong enum values unregNonIpv4BcastCmd and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntriesRangeWrite
                                                        (dev, vlanId, numOfEntries, &portsMembers,
                             &portsTagging, &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.unregNonIpv4BcastCmd);

        vlanInfo.unregNonIpv4BcastCmd = CPSS_PACKET_CMD_FORWARD_E;

        /*
            1.17. Call with wrong enum values ipCtrlToCpuEn and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntriesRangeWrite
                                                        (dev, vlanId, numOfEntries, &portsMembers,
                             &portsTagging, &vlanInfo, portsTaggingCmdPtr),
                                                        vlanInfo.ipCtrlToCpuEn);

        vlanInfo.ipCtrlToCpuEn = CPSS_DXCH_BRG_IP_CTRL_NONE_E;

        /*
            1.18. Call with wrong enum values ipv6SiteIdMode and other parameters from 1.1.
            Expected: GT_BAD_PARAM for DxCh2, DxCh3 and GT_OK for others
                      (supported only by Cheetah2,3).
        */
        if ((devFamily != CPSS_PP_FAMILY_CHEETAH_E) && (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEntriesRangeWrite
                                (dev, vlanId, numOfEntries, &portsMembers,
                                 &portsTagging, &vlanInfo, portsTaggingCmdPtr),
                                vlanInfo.ipv6SiteIdMode);
        }
        else
        {
            for(index = 0; index < utfInvalidEnumArrSize; index++)
            {
                vlanInfo.ipv6SiteIdMode = utfInvalidEnumArr[index];

                st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                                      &portsTagging, &vlanInfo, portsTaggingCmdPtr);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->ipv6SiteIdMode = %d",
                                             dev, vlanInfo.ipv6SiteIdMode);
            }
        }

        vlanInfo.ipv6SiteIdMode = CPSS_IP_SITE_ID_INTERNAL_E;

        /*
            1.19. Call with vlanId [100]
                            and portsMembersPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, NULL,
                                              &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsMembersPtr = NULL", dev);

        /*
            1.20. Call with vlanId [100]
                           and portsTaggingPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR for TR101 disabled devices,
                      GT_OK for TR101 enabled devices.
        */
        st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                              NULL, &vlanInfo, portsTaggingCmdPtr);
        if(IS_TR101_SUPPORTED_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, portsTaggingPtr = NULL", dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsTaggingPtr = NULL", dev);
        }

        /*
            1.20.1 Call with vlanId [100]
                           and portsTaggingCmdPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR for TR101 enabled devices,
                      GT_OK for TR101 disabled devices.
        */
        st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                       &portsTagging, &vlanInfo, NULL);
        if(!IS_TR101_SUPPORTED_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, portsTaggingCmdPtr = NULL", dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsTaggingCmdPtr = NULL", dev);
        }

        /*
            1.21. Call with vlanId [100] and vlanInfoPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                              &portsTagging, NULL, portsTaggingCmdPtr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vlanInfoPtr = NULL", dev);

        /*
            1.21. Call with out of range stgId [256] and other parameters from 1.1.
            Expected: NON GT_OK.
        */
        vlanInfo.stgId = 256;

        st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                              &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->stgId = %d",
                                         dev, vlanInfo.stgId);
        vlanInfo.stgId = 0;

        /*
            1.22. Call with mruIdx [PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS+1=8]
                       and other parameters from 1.1.
            Expected: non GT_OK.
        */
        vlanInfo.mruIdx = 8;

        st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                              &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->mruIdx = %d",
                                     dev, vlanInfo.mruIdx);
        vlanInfo.mruIdx = 0;

        /*
            1.23. Call with out of range vrfId [4096]
                            and other parameters from 1.1.
            Expected: NON GT_OK for DxCh3 and GT_OK for others (supported only by Cheetah3)
        */
        vlanInfo.vrfId = 4096;

        st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                       &portsTagging, &vlanInfo, portsTaggingCmdPtr);

        if ((devFamily >= CPSS_PP_FAMILY_CHEETAH3_E) && (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->vrfId = %d",
                                             dev, vlanInfo.vrfId);
        }
        else
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfoPtr->vrfId = %d",
                                         dev, vlanInfo.vrfId);
        }

        /*
            1.24. Call cpssDxChBrgVlanEntryInvalidate with the same vlanId
                  to invalidate all changes.
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanEntryInvalidate(dev, vlanId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgVlanEntryInvalidate: %d, %d", dev, vlanId);
    }

    vlanId       = BRG_VLAN_TESTED_VLAN_ID_CNS;
    numOfEntries = 10;

    portsMembers.ports[0] = 1000;
    portsMembers.ports[1] = 0;
    portsTagging.ports[0] = 1500;
    portsTagging.ports[1] = 0;

    vlanInfo.unkSrcAddrSecBreach  = GT_FALSE;
    vlanInfo.unregNonIpMcastCmd   = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregIpv4McastCmd    = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
    vlanInfo.unregIpv6McastCmd    = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    vlanInfo.unkUcastCmd          = CPSS_PACKET_CMD_DROP_HARD_E;
    vlanInfo.unregIpv4BcastCmd    = CPSS_PACKET_CMD_DROP_SOFT_E;
    vlanInfo.unregNonIpv4BcastCmd = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.ipv4IgmpToCpuEn      = GT_TRUE;
    vlanInfo.mirrToRxAnalyzerEn   = GT_TRUE;
    vlanInfo.ipv6IcmpToCpuEn      = GT_TRUE;
    vlanInfo.ipCtrlToCpuEn        = CPSS_DXCH_BRG_IP_CTRL_NONE_E;
    vlanInfo.ipv4IpmBrgMode       = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv6IpmBrgMode       = CPSS_BRG_IPM_GV_E;
    vlanInfo.ipv4IpmBrgEn         = GT_TRUE;
    vlanInfo.ipv6IpmBrgEn         = GT_TRUE;
    vlanInfo.ipv6SiteIdMode       = CPSS_IP_SITE_ID_INTERNAL_E;
    vlanInfo.ipv4UcastRouteEn     = GT_FALSE;
    vlanInfo.ipv4McastRouteEn     = GT_FALSE;
    vlanInfo.ipv6UcastRouteEn     = GT_FALSE;
    vlanInfo.ipv6McastRouteEn     = GT_FALSE;
    vlanInfo.stgId                = 0;
    vlanInfo.autoLearnDisable     = GT_FALSE;
    vlanInfo.naMsgToCpuEn         = GT_FALSE;
    vlanInfo.mruIdx               = 0;
    vlanInfo.bcastUdpTrapMirrEn   = GT_FALSE;
    vlanInfo.vrfId                = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                              &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanEntriesRangeWrite(dev, vlanId, numOfEntries, &portsMembers,
                                              &portsTagging, &vlanInfo, portsTaggingCmdPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanEntryRead
(
    IN  GT_U8                                devNum,
    IN  GT_U16                               vlanId,
    OUT CPSS_PORTS_BMP_STC                   *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC                   *portsTaggingPtr,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC          *vlanInfoPtr,
    OUT GT_BOOL                              *isValidPtr,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *portsTaggingCmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanEntryRead)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100],
                   non-null portsMemberPtr,
                   non-null portsTaggingPtr,
                   non-null vlanInfoPtr,
                   non-null isValidPtr.
    Expected: GT_OK.
    1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and non_null other parameters.
    Expected: GT_BAD_PARAM.
    1.3. Call with vlanId [100],
                   portsMembersPtr [NULL]
                   and non_null other parameters.
    Expected: GT_BAD_PTR.
    1.4. Call with vlanId [100],
                   portsTaggingPtr [NULL]
                   and non_null other parameters.
    Expected: GT_BAD_PTR.
    1.5. Call with vlanId [100],
                   vlanInfoPtr [NULL]
                   and non_null other parameters
    Expected: GT_BAD_PTR.
    1.6. Call with vlanId [100],
                   isValidPtr [NULL]
                   and non_null other parameters.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st     = GT_OK;

    GT_U8                       dev;
    GT_U16                      vlanId = 0;
    CPSS_PORTS_BMP_STC          portsMembers;
    CPSS_PORTS_BMP_STC          portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC vlanInfo;
    GT_BOOL                     isValid = GT_FALSE;

    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;

    cpssOsBzero((GT_VOID*) &portsMembers, sizeof(portsMembers));
    cpssOsBzero((GT_VOID*) &portsTagging, sizeof(portsTagging));
    cpssOsBzero((GT_VOID*) &vlanInfo, sizeof(vlanInfo));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100],
                           non-null portsMemberPtr,
                           non-null portsTaggingPtr,
                           non-null vlanInfoPtr,
                           non-null isValidPtr,
                           non-null portsTaggingCmdPtr.
            Expected: GT_OK.
        */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssDxChBrgVlanEntryRead(dev, vlanId, &portsMembers, &portsTagging,
                                      &vlanInfo, &isValid, &portsTaggingCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /*
            1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and non_null other parameters.
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanEntryRead(dev, vlanId, &portsMembers, &portsTagging,
                                      &vlanInfo, &isValid, &portsTaggingCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /*
            1.3. Call with vlanId [100], portsMembersPtr [NULL]
                 and non_null other parameters.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanEntryRead(dev, vlanId, NULL, &portsTagging,
                                      &vlanInfo, &isValid, &portsTaggingCmd);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsMembersPtr = NULL", dev);

        /*
            1.4. Call with vlanId [100], portsTaggingPtr [NULL]
                 and non_null other parameters.
            Expected: GT_BAD_PTR for TR101 disabled devices,
                      GT_OK for TR101 enabled devices.
        */
        st = cpssDxChBrgVlanEntryRead(dev, vlanId, &portsMembers, NULL,
                                      &vlanInfo, &isValid, &portsTaggingCmd);
        if(IS_TR101_SUPPORTED_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, portsTaggingPtr = NULL", dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsTaggingPtr = NULL", dev);
        }

        /*
            1.4.1 Call with vlanId [100], portsTaggingCmdPtr [NULL]
                 and non_null other parameters.
            Expected: GT_BAD_PTR for TR101 enabled devices,
                      GT_OK for TR101 disabled devices.
        */
        st = cpssDxChBrgVlanEntryRead(dev, vlanId, &portsMembers, &portsTagging,
                                      &vlanInfo, &isValid, NULL);
        if(!IS_TR101_SUPPORTED_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, portsTaggingPtr = NULL", dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsTaggingPtr = NULL", dev);
        }

        /*
            1.5. Call with vlanId [100], vlanInfoPtr [NULL]
                 and non_null other parameters
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanEntryRead(dev, vlanId, &portsMembers, &portsTagging,
                                      NULL, &isValid, &portsTaggingCmd);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vlanInfoPtr = NULL", dev);

        /*
            1.6. Call with vlanId [100], isValidPtr [NULL]
                 and non_null other parameters.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanEntryRead(dev, vlanId, &portsMembers, &portsTagging,
                                      &vlanInfo, NULL, &portsTaggingCmd);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, isValidPtr = NULL", dev);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanEntryRead(dev, vlanId, &portsMembers, &portsTagging,
                                      &vlanInfo, &isValid, &portsTaggingCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanEntryRead(dev, vlanId, &portsMembers, &portsTagging,
                                  &vlanInfo, &isValid, &portsTaggingCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanMemberAdd
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_U8   portNum,
    IN GT_BOOL isTagged
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanMemberAdd)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxChx)
    1.1.1. Call with vlanId [100],
                     isTagged [GT_FALSE and GT_ TRUE].
    Expected: GT_OK.
    1.1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                     and isTagged [GT_TRUE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port     = BRG_VLAN_VALID_PHY_PORT_CNS;
    GT_U16      vlanId   = 0;
    GT_BOOL     isTagged = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with vlanId [100],
                                 isTagged [GT_FALSE and GT_ TRUE].
                Expected: GT_OK.
            */

            /* Call function with isTagged [GT_FALSE] */
            vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
            isTagged = GT_FALSE;

            st = cpssDxChBrgVlanMemberAdd(dev, vlanId, port, isTagged, 0);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, port, isTagged);

            /* Call function with isTagged [GT_TRUE] */
            vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
            isTagged = GT_TRUE;

            st = cpssDxChBrgVlanMemberAdd(dev, vlanId, port, isTagged, 0);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, port, isTagged);

            /*
                1.1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                 and isTagged [GT_TRUE].
                Expected: GT_BAD_PARAM.
            */
            vlanId   = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssDxChBrgVlanMemberAdd(dev, vlanId, port, isTagged, 0);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, vlanId = %d, port = %d",
                                         dev, vlanId, port);
        }

        vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
        isTagged = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanMemberAdd(dev, vlanId, port, isTagged, 0);

            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev) ||
               port >= 64)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
            else
            {
                /* the device supports 64 ports regardless to physical ports */
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanMemberAdd(dev, vlanId, port, isTagged, 0);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK  */
        /* for CPU port number.                                      */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanMemberAdd(dev, vlanId, port, isTagged, 0);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
    isTagged = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanMemberAdd(dev, vlanId, port, isTagged, 0);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanMemberAdd(dev, vlanId, port, isTagged, 0);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortDelete
(
    IN GT_U8          devNum,
    IN GT_U16         vlanId,
    IN GT_U8          port
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortDelete)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxChx)
    1.1.1. Call with vlanId [100].
    Expected: GT_OK.
    1.1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_U8       port   = BRG_VLAN_VALID_PHY_PORT_CNS;
    GT_U16      vlanId = 0;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with vlanId [100].
                Expected: GT_OK.
            */
            vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;

            st = cpssDxChBrgVlanPortDelete(dev, vlanId, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, port);

            /*
                1.1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
                Expected: GT_BAD_PARAM.
            */
            vlanId   = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssDxChBrgVlanPortDelete(dev, vlanId, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, port);
        }

        vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanPortDelete(dev, vlanId, port);
            if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev) ||
               port >= 64)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
            else
            {
                /* the device supports 64 ports regardless to physical ports */
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanPortDelete(dev, vlanId, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanPortDelete(dev, vlanId, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortDelete(dev, vlanId, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortDelete(dev, vlanId, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortVidGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_U16  *vidPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortVidGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with non-null vidPtr.
    Expected: GT_OK.
    1.1.2. Call with vidPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;

    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_PHY_PORT_CNS;
    GT_U16      vid  = 0;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-null vidPtr.
                Expected: GT_OK.
            */
            st = cpssDxChBrgVlanPortVidGet(dev, port, &vid);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with vidPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChBrgVlanPortVidGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, vidPtr = NULL", dev, port);
        }

        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanPortVidGet(dev, port, &vid);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanPortVidGet(dev, port, &vid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanPortVidGet(dev, port, &vid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortVidGet(dev, port, &vid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortVidGet(dev, port, &vid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortVidSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    IN  GT_U16  vlanId
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortVidSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with vlanId [100].
    Expected: GT_OK.
    1.1.2. Call cpssDxChBrgVlanPortVidGet.
    Expected: GT_OK and the same vlanId.
    1.1.3. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st        = GT_OK;

    GT_U8       dev;
    GT_U8       port      = BRG_VLAN_VALID_PHY_PORT_CNS;
    GT_U16      vlanId    = 0;
    GT_U16      vlanIdGet = 0;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with vlanId [100].
                Expected: GT_OK.
            */
            vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

            st = cpssDxChBrgVlanPortVidSet(dev, port, vlanId);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vlanId);

            /*
                1.1.2. Call cpssDxChBrgVlanPortVidGet.
                Expected: GT_OK and the same vlanId.
            */
            st = cpssDxChBrgVlanPortVidGet(dev, port, &vlanIdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                     "cpssDxChBrgVlanPortVidGet: %d, %d", dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(vlanId, vlanIdGet,
                                     "get another vlanId than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
                Expected: GT_BAD_PARAM.
            */
            vlanId   = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssDxChBrgVlanPortVidSet(dev, port, vlanId);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, vlanId);
        }

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanPortVidSet(dev, port, vlanId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanPortVidSet(dev, port, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanPortVidSet(dev, port, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortVidSet(dev, port, vlanId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortVidSet(dev, port, vlanId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortIngFltEnable
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortIngFltEnable)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with enable [GT_TRUE and GT_FALSE].
    Expected: GT_OK.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = BRG_VLAN_VALID_PHY_PORT_CNS;
    GT_BOOL     state = GT_TRUE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_TRUE and GT_FALSE].
                Expected: GT_OK.
            */

            /* Call function with enable [GT_FALSE] */
            state = GT_FALSE;

            st = cpssDxChBrgVlanPortIngFltEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* Call function with enable [GT_TRUE] */
            state = GT_TRUE;

            st = cpssDxChBrgVlanPortIngFltEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
        }

        state = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanPortIngFltEnable(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanPortIngFltEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanPortIngFltEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    state = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortIngFltEnable(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortIngFltEnable(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortProtoClassVlanEnable
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortProtoClassVlanEnable)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with enable [GT_TRUE and GT_FALSE].
    Expected: GT_OK.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = BRG_VLAN_VALID_PHY_PORT_CNS;
    GT_BOOL     state = GT_TRUE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_TRUE and GT_FALSE].
                Expected: GT_OK.
            */

            /* Call function with enable [GT_FALSE] */
            state = GT_FALSE;

            st = cpssDxChBrgVlanPortProtoClassVlanEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* Call function with enable [GT_TRUE] */
            state = GT_TRUE;

            st = cpssDxChBrgVlanPortProtoClassVlanEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
        }

        state = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanPortProtoClassVlanEnable(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanPortProtoClassVlanEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanPortProtoClassVlanEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    state = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortProtoClassVlanEnable(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortProtoClassVlanEnable(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortProtoClassQosEnable
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortProtoClassQosEnable)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with enable [GT_TRUE and GT_FALSE].
    Expected: GT_OK.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = BRG_VLAN_VALID_PHY_PORT_CNS;
    GT_BOOL     state = GT_TRUE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_TRUE and GT_FALSE].
                Expected: GT_OK.
            */

            /* Call function with enable [GT_FALSE] */
            state = GT_FALSE;

            st = cpssDxChBrgVlanPortProtoClassQosEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* Call function with enable [GT_TRUE] */
            state = GT_TRUE;

            st = cpssDxChBrgVlanPortProtoClassQosEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
        }

        state = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanPortProtoClassQosEnable(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanPortProtoClassQosEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanPortProtoClassQosEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    state = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortProtoClassQosEnable(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortProtoClassQosEnable(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanProtoClassSet
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum,
    IN GT_U16                           etherType,
    IN CPSS_PROT_CLASS_ENCAP_STC        *encListPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanProtoClassSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with entryNum [0],
                   etherType [1],
                   and encListPtr {GT_TRUE, GT_FALSE, GT_FALSE}.
    Expected: GT_OK.
    1.2. Call cpssDxChBrgVlanProtoClassGet.
    Expected: GT_OK and the same etherType and encList.
    1.3. Call with out of range entryNum [8].
    Expected: NON GT_OK.
    1.4. Call with entryNum [1],
                   etherType [0xFFFF] (no any constraints),
                   and encListPtr {GT_FALSE, GT_TRUE, GT_FALSE}.
    Expected: GT_OK.
    1.5. Call with entryNum [2],
                   etherType [5]
                   and encListPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                 st           = GT_OK;

    GT_U8                     dev;
    GT_U32                    entryNum     = 0;
    GT_U16                    etherType    = 0;
    GT_U16                    etherTypeGet = 0;
    CPSS_PROT_CLASS_ENCAP_STC encList;
    CPSS_PROT_CLASS_ENCAP_STC encListGet;
    GT_BOOL                   isValidEntry = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryNum [0],
                           etherType [1],
                           and encListPtr {GT_TRUE, GT_FALSE, GT_FALSE}.
            Expected: GT_OK.
        */
        entryNum  = 0;
        etherType = 1;

        encList.ethV2      = GT_TRUE;
        encList.nonLlcSnap = GT_FALSE;
        encList.llcSnap    = GT_FALSE;

        st = cpssDxChBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, etherType);

        /*
            1.2. Call cpssDxChBrgVlanProtoClassGet.
            Expected: GT_OK and the same etherType and encList.
        */
        st = cpssDxChBrgVlanProtoClassGet(dev, entryNum, &etherTypeGet,
                                          &encListGet, &isValidEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                     "cpssDxChBrgVlanProtoClassGet: %d, %d", dev, entryNum);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValidEntry,
                                     "Entry is not valid: %d", dev);

        if (GT_TRUE == isValidEntry)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet,
                                         "get another etherType than was set: %d", dev);

            /* Verifying struct fields */
            UTF_VERIFY_EQUAL1_STRING_MAC(encList.ethV2, encListGet.ethV2,
                                         "get another encListPtr->ethV2 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(encList.nonLlcSnap, encListGet.nonLlcSnap,
                                         "get another encListPtr->nonLlcSnap than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(encList.llcSnap, encListGet.llcSnap,
                                         "get another encListPtr->llcSnap than was set: %d", dev);
        }

        /*
            1.3. Call with out of range entryNum [8 or 12].
            Expected: NON GT_OK.
        */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            entryNum = 12;
        }
        else
            entryNum = 8;

        st = cpssDxChBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryNum);

        /*
            1.4. Call with entryNum [1],
                           etherType [0xFFFF] (no any constraints),
                           and encListPtr {GT_FALSE, GT_TRUE, GT_FALSE}.
            Expected: GT_OK.
        */
        entryNum  = 1;
        etherType = 0xFFFF;

        encList.ethV2      = GT_FALSE;
        encList.nonLlcSnap = GT_TRUE;
        encList.llcSnap    = GT_FALSE;

        st = cpssDxChBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, etherType);

        /*
            1.5. Call with entryNum [2],
                           etherType [5]
                           and encListPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        entryNum  = 2;
        etherType = 5;

        st = cpssDxChBrgVlanProtoClassSet(dev, entryNum, etherType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, encListPtr = NULL", dev);
    }

    entryNum  = 0;
    etherType = 1;

    encList.ethV2      = GT_TRUE;
    encList.nonLlcSnap = GT_FALSE;
    encList.llcSnap    = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanProtoClassGet
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum,
    OUT GT_U16                          *etherTypePtr,
    OUT CPSS_PROT_CLASS_ENCAP_STC       *encListPtr,
    OUT GT_BOOL                         *validEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanProtoClassGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with entryNum [0],
                   non-null etherTypePtr,
                   non-null encListPtr
                   and non-null validEntryPtr.
    Expected: GT_OK.
    1.2. Call with entryNum [8] and other non-null parameters.
    Expected: non GT_OK.
    1.3. Call with entryNum [1],
                   null etherTypePtr [NULL],
                   non-null encListPtr
                   and non-null validEntryPtr.
    Expected: GT_BAD_PTR.
    1.4. Call with entryNum [2],
                   non-null etherTypePtr,
                   null encListPtr [NULL]
                   and non-null validEntryPtr.
    Expected: GT_BAD_PTR.
    1.5. Call with entryNum [3],
                   non-null etherTypePtr,
                   non-null encListPtr
                   and null validEntryPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                 st           = GT_OK;

    GT_U8                     dev;
    GT_U32                    entryNum     = 0;
    GT_U16                    etherTypePtr = 0;
    CPSS_PROT_CLASS_ENCAP_STC encListPtr;
    GT_BOOL                   validEntry   = GT_FALSE;


    cpssOsBzero((GT_VOID*) &encListPtr, sizeof(encListPtr));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryNum [0],
                           non-null etherTypePtr,
                           non-null encListPtr
                           and non-null validEntryPtr.
            Expected: GT_OK.
        */
        entryNum = 0;

        st = cpssDxChBrgVlanProtoClassGet(dev, entryNum, &etherTypePtr,
                                          &encListPtr, &validEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryNum);

        /*  1.2. Call with entryNum [8 or 12] and other non-null parameters.
            Expected: non GT_OK. */

        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            entryNum = 12;
        }
        else
            entryNum = 8;

        st = cpssDxChBrgVlanProtoClassGet(dev, entryNum, &etherTypePtr,
                                          &encListPtr, &validEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryNum);

        /*
            1.3. Call with entryNum [1],
                           null etherTypePtr [NULL],
                           non-null encListPtr
                           and non-null validEntryPtr.
            Expected: GT_BAD_PTR.
        */
        entryNum = 1;

        st = cpssDxChBrgVlanProtoClassGet(dev, entryNum, NULL,
                                          &encListPtr, &validEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, etherTypePtr = NULL", dev);

        /*
            1.4. Call with entryNum [2],
                           non-null etherTypePtr,
                           null encListPtr [NULL]
                           and non-null validEntryPtr.
            Expected: GT_BAD_PTR.
        */
        entryNum = 2;

        st = cpssDxChBrgVlanProtoClassGet(dev, entryNum, &etherTypePtr,
                                          NULL, &validEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, encListPtr = NULL", dev);

        /*
            1.5. Call with entryNum [3],
                           non-null etherTypePtr,
                           non-null encListPtr
                           and null validEntryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        entryNum = 3;

        st = cpssDxChBrgVlanProtoClassGet(dev, entryNum, &etherTypePtr,
                                          &encListPtr, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validEntryPtr = NULL", dev);
    }

    entryNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanProtoClassGet(dev, entryNum, &etherTypePtr,
                                          &encListPtr, &validEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanProtoClassGet(dev, entryNum, &etherTypePtr,
                                          &encListPtr, &validEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanProtoClassInvalidate
(
    IN GT_U8                            devNum,
    IN GT_U32                           entryNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanProtoClassInvalidate)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with entryNum [0 / 7].
    Expected: GT_OK.
    1.2. Call with [8]. Expected: non GT_OK.
*/
    GT_STATUS st       = GT_OK;

    GT_U8     dev;
    GT_U32    entryNum = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryNum [0 / 7].
            Expected: GT_OK.
        */
        entryNum = 0;

        st = cpssDxChBrgVlanProtoClassInvalidate(dev, entryNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryNum);

        entryNum = 7;

        st = cpssDxChBrgVlanProtoClassInvalidate(dev, entryNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryNum);

        /*
            1.2. Call with entryNum [8 or 12].
            Expected: non GT_OK.
        */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            entryNum = 12;
        }
        else
            entryNum = 8;

        st = cpssDxChBrgVlanProtoClassInvalidate(dev, entryNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryNum);
    }

    entryNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanProtoClassInvalidate(dev, entryNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanProtoClassInvalidate(dev, entryNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortProtoVlanQosSet
(
    IN GT_U8                                    devNum,
    IN GT_U8                                    port,
    IN GT_U32                                   entryNum,
    IN CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC    *vlanCfgPtr,
    IN CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC     *qosCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortProtoVlanQosSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with entryNum [0],
                     vlanCfgPtr {vlanId [100],
                                 vlanIdAssignCmd  [CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E /
                                                   CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E],
                                 vlanAssignPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E /
                                                       CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E]},
                     qosCfgPtr [{qosAssignCmd [CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E /
                                               CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E],
                                 qosProfileId [10],
                                 qosAssignPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E/
                                                      CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                 enableModifyUp [GT_TRUE/ GT_FALSE],
                                 enableModifyDscp [GT_TRUE/ GT_FALSE]}].
    Expected: GT_OK.
    1.1.2. Call cpssDxChBrgVlanPortProtoVlanQosGet with the same entryNum.
    Expected: GT_OK and the same vlanCfgPtr, qosCfgPtr and validEntryPtr [GT_TRUE].
    1.1.3. Call with entryNum [1],
                     vlanCfgPtr {out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                                 vlanIdAssignCmd [CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E],
                                 vlanAssignPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E]},
                     qosCfgPtr [NULL]
    Expected: GT_BAD_PARAM.
    1.1.4. Call with entryNum [2],
                     vlanCfgPtr {vlanId [100],
                                 out of range vlanIdAssignCmd,
                                 vlanAssignPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E}],
                     qosCfgPtr [NULL].
    Expected: GT_BAD_PARAM.
    1.1.5. Call with entryNum [3],
                     vlanCfgPtr {vlanId [100],
                                 vlanIdAssignCmd  [CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E]
                                 and out of range vlanAssignPrecedence}],
                     qosCfgPtr [NULL].
    Expected: GT_BAD_PARAM.
    1.1.6. Call with out of range entryNum [8 ] and other valid parameters from 1.1.1.
    Expected: non GT_OK.
    1.1.7. Call with entryNum [4],
                     vlanCfgPtr[NULL]
                     and other valid parameters from 1.1.1.
    Expected: GT_OK.
    1.1.8. Call with entryNum [5],
                     qosCfgPtr [NULL]
                     and other valid parameters from 1.1.1.
    Expected: GT_OK.
    1.1.9. Call with entryNum [6],
                     vlanCfgPtr[NULL],
                     qosCfgPtr [NULL].
    Expected: NON GT_ OK.
    1.1.10. Call with entryNum [1],
                      wrong enum values qosCfgPtr ->qosAssignCmd ,
                      valid other fields of qosCfg
                      and vlanCfgPtr [NULL]
    Expected: GT_BAD_PARAM.
    1.1.11. Call with entryNum [1],
                      wrong enum values qosCfgPtr ->qosAssignPrecedence ,
                      valid other fields of qosCfg
                      and vlanCfgPtr [NULL]
    Expected: GT_BAD_PARAM.
    1.1.12. Call with entryNum [1],
                      qosCfgPtr ->qosProfileId [PRV_CPSS_DXCH_QOS_PROFILE_NUM_MAX_CNS],
                      valid other fields of qosCfg
                      and vlanCfgPtr [NULL]
    Expected: NON GT_OK for Cheetah, GT_OK for Cheetah2.
    1.1.13. Call with entryNum [1],
                      qosCfgPtr ->qosProfileId [PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS],
                      valid other fields of qosCfg
                      and vlanCfgPtr [NULL]
    Expected: NON GT_OK.
*/
    GT_STATUS  st    = GT_OK;


    GT_U8                                 dev;
    GT_U8                                 port     = BRG_VLAN_VALID_PHY_PORT_CNS;
    GT_U32                                entryNum = 0;
    CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC vlanCfg;
    CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC  qosCfg;
    CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC vlanCfgGet;
    CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC  qosCfgGet;

    GT_BOOL                               isValid   = GT_FALSE;
    CPSS_PP_FAMILY_TYPE_ENT               devFamily = CPSS_PP_FAMILY_CHEETAH_E;


    cpssOsBzero((GT_VOID*) &vlanCfg, sizeof(vlanCfg));
    cpssOsBzero((GT_VOID*) &qosCfg, sizeof(qosCfg));
    cpssOsBzero((GT_VOID*) &vlanCfgGet, sizeof(vlanCfgGet));
    cpssOsBzero((GT_VOID*) &qosCfgGet, sizeof(qosCfgGet));

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with entryNum [0],
                     vlanCfgPtr {vlanId [100],
                                 vlanIdAssignCmd  [CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E /
                                                   CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E],
                                 vlanAssignPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E /
                                                       CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E]},
                     qosCfgPtr [{qosAssignCmd [CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E /
                                               CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E],
                                 qosProfileId [10],
                                 qosAssignPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E/
                                                      CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                 enableModifyUp [GT_TRUE/ GT_FALSE],
                                 enableModifyDscp [GT_TRUE/ GT_FALSE]}].
                Expected: GT_OK.
            */

            /* Call function with 1 entries */
            entryNum = 0;

            vlanCfg.vlanId                 = BRG_VLAN_TESTED_VLAN_ID_CNS;
            vlanCfg.vlanIdAssignCmd        = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
            vlanCfg.vlanIdAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

            qosCfg.qosAssignCmd        = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E;
            qosCfg.qosProfileId        = 10;
            qosCfg.qosAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
            qosCfg.enableModifyUp      = GT_TRUE;
            qosCfg.enableModifyDscp    = GT_TRUE;

            st = cpssDxChBrgVlanPortProtoVlanQosSet(dev, port, entryNum, &vlanCfg, &qosCfg);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, entryNum);

            /*
                1.1.2. Call cpssDxChBrgVlanPortProtoVlanQosGet with the same entryNum.
                Expected: GT_OK and the same vlanCfgPtr, qosCfgPtr and validEntryPtr [GT_TRUE].
            */
            st = cpssDxChBrgVlanPortProtoVlanQosGet(dev, port, entryNum,
                                                    &vlanCfgGet, &qosCfgGet, &isValid);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgVlanPortProtoVlanQosGet: %d, %d, %d", dev, port, entryNum);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_TRUE, isValid, "Entry is NOT valid: %d, %d, %d", dev, port, entryNum);

            if (GT_TRUE == isValid)
            {
                /* Verifying vlanCfgPtr */
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanId, vlanCfgGet.vlanId,
                           "get another vlanCfgPtr->vlanId than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanIdAssignCmd, vlanCfgGet.vlanIdAssignCmd,
                           "get another vlanCfgPtr->vlanIdAssignCmd than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanIdAssignPrecedence, vlanCfgGet.vlanIdAssignPrecedence,
                           "get another vlanCfgPtr->vlanIdAssignPrecedence than was set: %d, %d", dev, port);

                /* Verifying qosCfgPtr */
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosAssignCmd, qosCfgGet.qosAssignCmd,
                           "get another qosCfgPtr->qosAssignCmd than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosProfileId, qosCfgGet.qosProfileId,
                           "get another qosCfgPtr->qosProfileId than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosAssignPrecedence, qosCfgGet.qosAssignPrecedence,
                           "get another qosCfgPtr->qosAssignPrecedence than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.enableModifyUp, qosCfgGet.enableModifyUp,
                           "get another qosCfgPtr->enableModifyUp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.enableModifyDscp, qosCfgGet.enableModifyDscp,
                           "get another qosCfgPtr->enableModifyDscp than was set: %d, %d", dev, port);
            }

            /* Call function with 2 entries */
            vlanCfg.vlanIdAssignCmd        = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
            vlanCfg.vlanIdAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;

            qosCfg.qosAssignCmd        = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;
            qosCfg.qosAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
            qosCfg.enableModifyUp      = GT_FALSE;
            qosCfg.enableModifyDscp    = GT_FALSE;

            st = cpssDxChBrgVlanPortProtoVlanQosSet(dev, port, entryNum,
                                                    &vlanCfg, &qosCfg);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, entryNum);

            /*
                1.1.2. Call cpssDxChBrgVlanPortProtoVlanQosGet with the same entryNum.
                Expected: GT_OK and the same vlanCfgPtr, qosCfgPtr and validEntryPtr [GT_TRUE].
            */
            st = cpssDxChBrgVlanPortProtoVlanQosGet(dev, port, entryNum,
                                                    &vlanCfgGet, &qosCfgGet, &isValid);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgVlanPortProtoVlanQosGet: %d, %d, %d", dev, port, entryNum);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_TRUE, isValid, "Entry is NOT valid: %d, %d, %d", dev, port, entryNum);

            if (GT_TRUE == isValid)
            {
                /* Verifying vlanCfgPtr */
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanId, vlanCfgGet.vlanId,
                           "get another vlanCfgPtr->vlanId than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanIdAssignCmd, vlanCfgGet.vlanIdAssignCmd,
                           "get another vlanCfgPtr->vlanIdAssignCmd than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanIdAssignPrecedence, vlanCfgGet.vlanIdAssignPrecedence,
                           "get another vlanCfgPtr->vlanIdAssignPrecedence than was set: %d, %d", dev, port);

                /* Verifying qosCfgPtr */
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosAssignCmd, qosCfgGet.qosAssignCmd,
                           "get another qosCfgPtr->qosAssignCmd than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosProfileId, qosCfgGet.qosProfileId,
                           "get another qosCfgPtr->qosProfileId than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosAssignPrecedence, qosCfgGet.qosAssignPrecedence,
                           "get another qosCfgPtr->qosAssignPrecedence than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.enableModifyUp, qosCfgGet.enableModifyUp,
                           "get another qosCfgPtr->enableModifyUp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.enableModifyDscp, qosCfgGet.enableModifyDscp,
                           "get another qosCfgPtr->enableModifyDscp than was set: %d, %d", dev, port);
            }

            /*
                1.1.3. Call with entryNum [1],
                                 vlanCfgPtr {out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                                             vlanIdAssignCmd [CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E],
                                             vlanAssignPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E]},
                                 qosCfgPtr [NULL]
                Expected: GT_BAD_PARAM.
            */
            entryNum = 1;

            vlanCfg.vlanId                 = PRV_CPSS_MAX_NUM_VLANS_CNS;
            vlanCfg.vlanIdAssignCmd        = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
            vlanCfg.vlanIdAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

            st = cpssDxChBrgVlanPortProtoVlanQosSet(dev, port, entryNum,
                                                    &vlanCfg, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, vlanCfgPtr->vlanId = %d",
                                         dev, port, vlanCfg.vlanId);

            /*
                1.1.4. Call with entryNum [2], vlanCfgPtr {vlanId [100],
                        wrong enum values vlanIdAssignCmd,
                        vlanAssignPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E}],
                        qosCfgPtr [NULL].
                Expected: GT_BAD_PARAM.
            */
            entryNum = 2;
            vlanCfg.vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

            UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanPortProtoVlanQosSet
                                                        (dev, port, entryNum, &vlanCfg, NULL),
                                                        vlanCfg.vlanIdAssignCmd);

            vlanCfg.vlanIdAssignCmd = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;

            /*
                1.1.5. Call with entryNum [3],
                             vlanCfgPtr {vlanId [100],
                                         vlanIdAssignCmd  [CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E]
                                         and wrong enum values vlanAssignPrecedence}],
                             qosCfgPtr [NULL].
                Expected: GT_BAD_PARAM.
            */
            entryNum = 3;
            vlanCfg.vlanIdAssignCmd = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;

            UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanPortProtoVlanQosSet
                                                        (dev, port, entryNum, &vlanCfg, NULL),
                                                        vlanCfg.vlanIdAssignPrecedence);

            vlanCfg.vlanIdAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

            /*
                1.1.6. Call with out of range entryNum [8 or 12] and other valid
                       parameters from 1.1.1.
                Expected: non GT_OK.
            */
            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                entryNum = 12;
            }
            else
                entryNum = 8;

            st = cpssDxChBrgVlanPortProtoVlanQosSet(dev, port, entryNum, &vlanCfg, &qosCfg);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, entryNum);

            /*
                1.1.7. Call with entryNum [4], vlanCfgPtr[NULL]
                                 and other valid parameters from 1.1.1.
                Expected: GT_OK.
            */
            entryNum = 4;

            st = cpssDxChBrgVlanPortProtoVlanQosSet(dev, port, entryNum, NULL, &qosCfg);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, entryNum);

            /*
                1.1.8. Call with entryNum [5],
                                 qosCfgPtr [NULL]
                                 and other valid parameters from 1.1.1.
                Expected: GT_OK.
            */
            entryNum = 5;

            st = cpssDxChBrgVlanPortProtoVlanQosSet(dev, port, entryNum, &vlanCfg, NULL);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, entryNum);

            /*
                1.1.9. Call with entryNum [6],
                                 vlanCfgPtr[NULL],
                                 qosCfgPtr [NULL].
                Expected: NON GT_ OK.
            */
            entryNum = 6;

            st = cpssDxChBrgVlanPortProtoVlanQosSet(dev, port, entryNum, NULL, NULL);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                    "%d, %d, vlanCfgPtr = NULL, qosCfgPtr = NULL", dev, port);

            /*
                1.1.10. Call with entryNum [1],
                                  wrong enum values qosCfgPtr->qosAssignCmd,
                                  valid other fields of qosCfg
                                  and vlanCfgPtr [NULL]
                Expected: GT_BAD_PARAM.
            */
            entryNum = 1;

            UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanPortProtoVlanQosSet
                                                        (dev, port, entryNum, NULL, &qosCfg),
                                                        qosCfg.qosAssignCmd);

            qosCfg.qosAssignCmd  = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;

            /*
                1.1.11. Call with entryNum [1],
                                  wrong enum values qosCfgPtr->qosAssignPrecedence ,
                                  valid other fields of qosCfg
                                  and vlanCfgPtr [NULL]
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanPortProtoVlanQosSet
                                                        (dev, port, entryNum, NULL, &qosCfg),
                                                        qosCfg.qosAssignPrecedence);

            qosCfg.qosAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;

            /* Getting device family */
            st = prvUtfDeviceFamilyGet(dev, &devFamily);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

            /*
                1.1.12. Call with entryNum [1],
                                  qosCfgPtr->qosProfileId [PRV_CPSS_DXCH_QOS_PROFILE_NUM_MAX_CNS],
                                  valid other fields of qosCfg
                                  and vlanCfgPtr [NULL]
                Expected: NON GT_OK for Cheetah, GT_OK for Cheetah2.
            */
            qosCfg.qosProfileId        = PRV_CPSS_DXCH_QOS_PROFILE_NUM_MAX_CNS;

            st = cpssDxChBrgVlanPortProtoVlanQosSet(dev, port, entryNum,
                                                    NULL, &qosCfg);
            if (devFamily >= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, qosCfgPtr->qosProfileId = %d",
                                             dev, port, qosCfg.qosProfileId);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, qosCfgPtr->qosProfileId = %d",
                                                 dev, port, qosCfg.qosProfileId);
            }

            /*
                1.1.13. Call with entryNum [1],
                                  qosCfgPtr->qosProfileId [PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS],
                                  valid other fields of qosCfg
                                  and vlanCfgPtr [NULL]
                Expected: NON GT_OK.
            */
            qosCfg.qosProfileId = PRV_CPSS_DXCH2_QOS_PROFILE_NUM_MAX_CNS;

            st = cpssDxChBrgVlanPortProtoVlanQosSet(dev, port, entryNum,
                                                    NULL, &qosCfg);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, qosCfgPtr->qosProfileId = %d",
                                                 dev, port, qosCfg.qosProfileId);
        }

        entryNum = 0;

        vlanCfg.vlanId                 = BRG_VLAN_TESTED_VLAN_ID_CNS;
        vlanCfg.vlanIdAssignCmd        = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
        vlanCfg.vlanIdAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

        qosCfg.qosAssignCmd        = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E;
        qosCfg.qosProfileId        = 10;
        qosCfg.qosAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
        qosCfg.enableModifyUp      = GT_TRUE;
        qosCfg.enableModifyDscp    = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanPortProtoVlanQosSet(dev, port, entryNum,
                                                    &vlanCfg, &qosCfg);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanPortProtoVlanQosSet(dev, port, entryNum,
                                                &vlanCfg, &qosCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanPortProtoVlanQosSet(dev, port, entryNum,
                                                &vlanCfg, &qosCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    entryNum = 0;

    vlanCfg.vlanId                 = BRG_VLAN_TESTED_VLAN_ID_CNS;
    vlanCfg.vlanIdAssignCmd        = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
    vlanCfg.vlanIdAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    qosCfg.qosAssignCmd        = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E;
    qosCfg.qosProfileId        = 10;
    qosCfg.qosAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
    qosCfg.enableModifyUp      = GT_TRUE;
    qosCfg.enableModifyDscp    = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortProtoVlanQosSet(dev, port, entryNum,
                                                &vlanCfg, &qosCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortProtoVlanQosSet(dev, port, entryNum,
                                                &vlanCfg, &qosCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortProtoVlanQosGet
(
    IN GT_U8                                    devNum,
    IN GT_U8                                    port,
    IN GT_U32                                   entryNum,
    OUT CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC   *vlanCfgPtr,
    OUT CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC    *qosCfgPtr,
    OUT GT_BOOL                                 *validEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortProtoVlanQosGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with entryNum [0],
                     non-null vlanCfgPtr,
                     non-null qosCfgPtr
                     and non-null validEntryPtr.
    Expected: GT_OK.
    1.1.2. Call with out of range entryNum [8] and other non-null parameters.
    Expected: non GT_OK.
    1.1.3. Call with entryNum [1],
                     vlanCfgPtr [NULL],
                     non-null qosCfgPtr
                     and non-null validEntryPtr.
    Expected: GT_OK.
    1.1.4. Call with entryNum [2],
                     non-null vlanCfgPtr,
                     qosCfgPtr [NULL]
                     and non-null validEntryPtr.
    Expected: GT_OK.
    1.1.5. Call with entryNum [2],
                     vlanCfgPtr [NULL],
                     qosCfgPtr [NULL]
                     and non-null validEntryPtr.
    Expected: GT_OK.
    1.1.6. Call with entryNum [3],
                     non-null vlanCfgPtr,
                     non-null qosCfgPtr
                     and null validEntryPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                             st       = GT_OK;

    GT_U8                                 dev;
    GT_U8                                 port     = BRG_VLAN_VALID_PHY_PORT_CNS;
    GT_U32                                entryNum = 0;
    CPSS_DXCH_BRG_VLAN_PROT_CLASS_CFG_STC vlanCfg;
    CPSS_DXCH_BRG_QOS_PROT_CLASS_CFG_STC  qosCfg;
    GT_BOOL                               validEntry;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with entryNum [0],
                                 non-null vlanCfgPtr,
                                 non-null qosCfgPtr
                                 and non-null validEntryPtr.
                Expected: GT_OK.
            */
            entryNum = 0;

            st = cpssDxChBrgVlanPortProtoVlanQosGet(dev, port, entryNum,
                                                    &vlanCfg, &qosCfg, &validEntry);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, entryNum);

            /*
                1.1.2. Call with out of range entryNum [8 or 12] and other non-null
                parameters. Expected: non GT_OK.
            */
            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                entryNum = 12;
            }
            else
                entryNum = 8;

            st = cpssDxChBrgVlanPortProtoVlanQosGet(dev, port, entryNum,
                                                    &vlanCfg, &qosCfg, &validEntry);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, entryNum);

            /*
                1.1.3. Call with entryNum [1],
                                 vlanCfgPtr [NULL],
                                 non-null qosCfgPtr
                                 and non-null validEntryPtr.
                Expected: GT_OK.
            */
            entryNum = 1;

            st = cpssDxChBrgVlanPortProtoVlanQosGet(dev, port, entryNum,
                                                    NULL, &qosCfg, &validEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, vlanCfgPtr = NULL", dev, port);

            /*
                1.1.4. Call with entryNum [2],
                                 non-null vlanCfgPtr,
                                 qosCfgPtr [NULL]
                                 and non-null validEntryPtr.
                Expected: GT_OK.
            */
            entryNum = 2;

            st = cpssDxChBrgVlanPortProtoVlanQosGet(dev, port, entryNum,
                                                    &vlanCfg, NULL, &validEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, qosCfg = NULL", dev, port);

            /*
                1.1.5. Call with entryNum [2],
                                 vlanCfgPtr [NULL],
                                 qosCfgPtr [NULL]
                                 and non-null validEntryPtr.
                Expected: GT_OK.
            */
            st = cpssDxChBrgVlanPortProtoVlanQosGet(dev, port, entryNum,
                                                    NULL, NULL, &validEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, vlanCfgPtr = NULL, qosCfg = NULL",
                                             dev, port);

            /*
                1.1.6. Call with entryNum [3],
                                 non-null vlanCfgPtr,
                                 non-null qosCfgPtr
                                 and null validEntryPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            entryNum = 3;

            st = cpssDxChBrgVlanPortProtoVlanQosGet(dev, port, entryNum,
                                                    &vlanCfg, &qosCfg, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, validEntry = NULL", dev, port);
        }

        entryNum = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanPortProtoVlanQosGet(dev, port, entryNum,
                                                    &vlanCfg, &qosCfg, &validEntry);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanPortProtoVlanQosGet(dev, port, entryNum,
                                                &vlanCfg, &qosCfg, &validEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanPortProtoVlanQosGet(dev, port, entryNum,
                                                &vlanCfg, &qosCfg, &validEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    entryNum = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortProtoVlanQosGet(dev, port, entryNum,
                                                &vlanCfg, &qosCfg, &validEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortProtoVlanQosGet(dev, port, entryNum,
                                            &vlanCfg, &qosCfg, &validEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortProtoVlanQosInvalidate
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_U32   entryNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortProtoVlanQosInvalidate)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with entryNum [0].
    Expected: GT_OK.
    1.1.2. Call with out of range entryNum [8].
    Expected: non GT_OK.
*/
    GT_STATUS  st       = GT_OK;

    GT_U8      dev;
    GT_U8      port     = BRG_VLAN_VALID_PHY_PORT_CNS;
    GT_U32     entryNum = 0;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with entryNum [0].
                Expected: GT_OK.
            */
            entryNum = 0;

            st = cpssDxChBrgVlanPortProtoVlanQosInvalidate(dev, port, entryNum);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, entryNum);

            /*
                1.1.2. Call with out of range entryNum [8].
                Expected: non GT_OK.
            */
            if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
            {
                entryNum = 12;
            }
            else
                entryNum = 8;

            st = cpssDxChBrgVlanPortProtoVlanQosInvalidate(dev, port, entryNum);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, entryNum);
        }

        entryNum = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanPortProtoVlanQosInvalidate(dev, port, entryNum);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanPortProtoVlanQosInvalidate(dev, port, entryNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanPortProtoVlanQosInvalidate(dev, port, entryNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    entryNum = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortProtoVlanQosInvalidate(dev, port, entryNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortProtoVlanQosInvalidate(dev, port, entryNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanLearningStateSet
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_BOOL  status
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanLearningStateSet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with vlanId [100] and status [GT_FALSE and GT_ TRUE].
    Expected: GT_OK.
    1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
         and status [GT_FALSE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId = 0;
    GT_BOOL   status = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100] and status [GT_FALSE and GT_ TRUE].
            Expected: GT_OK.
        */

        /* Call with status [GT_FALSE] */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        status = GT_FALSE;

        st = cpssDxChBrgVlanLearningStateSet(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, status);

        /* Call with status [GT_TRUE] */
        status = GT_TRUE;

        st = cpssDxChBrgVlanLearningStateSet(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, status);

        /*
            1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and status [GT_FALSE].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        status = GT_FALSE;

        st = cpssDxChBrgVlanLearningStateSet(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, status);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    status = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanLearningStateSet(dev, vlanId, status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanLearningStateSet(dev, vlanId, status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_BOOL cpssDxChBrgVlanIsDevMember
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanIsDevMember)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100].
    Expected: GT_OK.
    1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100].
            Expected: GT_OK.
        */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssDxChBrgVlanIsDevMember(dev, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /*
            1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanIsDevMember(dev, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanIsDevMember(dev, vlanId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanIsDevMember(dev, vlanId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanToStpIdBind
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_U16   stpId
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanToStpIdBind)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100],
                   stpId [10].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgVlanStpIdGet with same vlandId.
    Expected: GT_OK and the same stpId.
    1.3. Call with vlanId [100],
                   stpId [PRV_CPSS_DXCH_STG_MAX_NUM_CNS].
    Expected: NON  GT_OK.
    1.4. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and stpId [5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st       = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId   = 0;
    GT_U16    stpId    = 0;
    GT_U16    stpIdGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100],
                           stpId [10].
            Expected: GT_OK.
        */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        stpId  = 10;

        st = cpssDxChBrgVlanToStpIdBind(dev, vlanId, stpId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, stpId);

        /*
            1.2. Call cpssDxChBrgVlanStpIdGet with same vlandId.
            Expected: GT_OK and the same stpId.
        */
        st = cpssDxChBrgVlanStpIdGet(dev, vlanId, &stpIdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgVlanStpIdGet: %d, %d", dev, vlanId);

        UTF_VERIFY_EQUAL1_STRING_MAC(stpId, stpIdGet,
                   "get another stpId than was set: %d", dev);

        /*
            1.3. Call with vlanId [100],
                           stpId [PRV_CPSS_DXCH_STG_MAX_NUM_CNS].
            Expected: NON  GT_OK.
        */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        stpId  = PRV_CPSS_DXCH_STG_MAX_NUM_CNS;

        st = cpssDxChBrgVlanToStpIdBind(dev, vlanId, stpId);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, stpId = %d", dev, stpId);

        /*
            1.4. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                               and stpId [5].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        stpId  = 5;

        st = cpssDxChBrgVlanToStpIdBind(dev, vlanId, stpId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    stpId  = 10;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanToStpIdBind(dev, vlanId, stpId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanToStpIdBind(dev, vlanId, stpId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanStpIdGet
(
    IN  GT_U8    devNum,
    IN  GT_U16   vlanId,
    OUT GT_U16   *stpIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanStpIdGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100],
                   non-null stpIdPtr.
    Expected: GT_OK.
    1.2. Call with vlanId [100]
                   and stpIdPtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and non-null stpIdPtr.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId = 0;
    GT_U16    stpId  = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100],
                           non-null stpIdPtr.
            Expected: GT_OK.
        */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssDxChBrgVlanStpIdGet(dev, vlanId, &stpId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /*
            1.2. Call with vlanId [100]
                           and stpIdPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanStpIdGet(dev, vlanId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, stpIdPtr = NULL", dev);

        /*
            1.3. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and non-null stpIdPtr.
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanStpIdGet(dev, vlanId, &stpId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanStpIdGet(dev, vlanId, &stpId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanStpIdGet(dev, vlanId, &stpId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanEtherTypeSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_ETHER_MODE_ENT     etherTypeMode,
    IN  GT_U16                  etherType,
    IN  GT_U16                  vidRange
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanEtherTypeSet)
{
/*
    ITERATE_DEVICES (DxCh1, DxCh2 and DxCh3)
    1.1. Call with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E /
                                  CPSS_VLAN_ETHERTYPE1_E],
                   etherType [0],
                   and vidRange [100].
    Expected: GT_OK.
    1.2. Call with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E],
                   etherType [FFFF] (no any constraints),
                   and vidRange [100].
    Expected: GT_OK.
    1.3. Call with out of range vidRange [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and other valid parameters.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range etherTypeMode
                   and other valid parameters.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st    = GT_OK;


    GT_U8               dev;
    CPSS_ETHER_MODE_ENT etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
    GT_U16              etherType     = 0;
    GT_U16              vidRange      = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E /
                                          CPSS_VLAN_ETHERTYPE1_E],
                           etherType [0],
                           and vidRange [100].
            Expected: GT_OK.
        */

        /* Call with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E] */
        etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
        etherType     = 0;
        vidRange      = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssDxChBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, etherTypeMode, etherType, vidRange);

        /* Call with etherTypeMode [CPSS_VLAN_ETHERTYPE1_E] */
        etherTypeMode = CPSS_VLAN_ETHERTYPE1_E;

        st = cpssDxChBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, etherTypeMode, etherType, vidRange);

        /*
            1.2. Call with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E],
                           etherType [FFFF] (no any constraints),
                           and vidRange [100].
            Expected: GT_OK.
        */
        etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
        etherType     = 0xFFFF;

        st = cpssDxChBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, etherTypeMode, etherType, vidRange);

        /*
            1.3. Call with out of range vidRange [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and other valid parameters.
            Expected: GT_BAD_PARAM.
        */
        etherType = 0;
        vidRange  = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, vidRange = %d", dev, vidRange);

        /*
            1.4. Call with wrong enum values etherTypeMode and other valid parameters.
            Expected: GT_BAD_PARAM.
        */
        vidRange = BRG_VLAN_TESTED_VLAN_ID_CNS;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanEtherTypeSet
                                                        (dev, etherTypeMode, etherType, vidRange),
                                                        etherTypeMode);
    }

    etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
    etherType     = 0;
    vidRange      = BRG_VLAN_TESTED_VLAN_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanTableInvalidate
(
    IN GT_U8 devNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanTableInvalidate)
{
/*
    ITERATE_DEVICES (DxChx)
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = cpssDxChBrgVlanTableInvalidate(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanTableInvalidate(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanTableInvalidate(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanEntryInvalidate
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanEntryInvalidate)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100].
    Expected: GT_OK.
    1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100].
            Expected: GT_OK.
        */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssDxChBrgVlanEntryInvalidate(dev, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /*
            1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanEntryInvalidate(dev, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanEntryInvalidate(dev, vlanId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanEntryInvalidate(dev, vlanId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanUnkUnregFilterSet
(
    IN GT_U8                                devNum,
    IN GT_U16                               vlanId,
    IN CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT   packetType,
    IN CPSS_PACKET_CMD_ENT                  cmd
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanUnkUnregFilterSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100],
                   packetType [CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E /
                               CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E /
                               CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_MCAST_E /
                               CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV6_MCAST_E /
                               CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IPV4_BCAST_E /
                               CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_BCAST_E]
                   and cmd [CPSS_PACKET_CMD_FORWARD_E /
                            CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                            CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                            CPSS_PACKET_CMD_DROP_HARD_E /
                            CPSS_PACKET_CMD_DROP_SOFT_E].
    Expected: GT_OK.
    1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                   packetType [CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E]
                   and cmd [CPSS_PACKET_CMD_FORWARD_E].
    Expected: GT_BAD_PARAM.
    1.3. Call with vlanId[100],
                   out of range packetType
                   and cmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E].
    Expected: GT_BAD_PARAM.
    1.4. Call with vlanId[100],
                   packetType [CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E]
                   and out of range cmd.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS  st    = GT_OK;


    GT_U8                              dev;
    GT_U16                             vlanId     = 0;
    CPSS_DXCH_BRG_VLAN_PACKET_TYPE_ENT packetType = CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E;
    CPSS_PACKET_CMD_ENT                cmd        = CPSS_PACKET_CMD_FORWARD_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100],
                           packetType [CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E /
                                       CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E /
                                       CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_MCAST_E /
                                       CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV6_MCAST_E /
                                       CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IPV4_BCAST_E /
                                       CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_BCAST_E]
                           and cmd [CPSS_PACKET_CMD_FORWARD_E /
                                    CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                                    CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                    CPSS_PACKET_CMD_DROP_HARD_E /
                                    CPSS_PACKET_CMD_DROP_SOFT_E].
            Expected: GT_OK.
        */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /* Call with packetType [CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E] */
        /*           and cmd [CPSS_PACKET_CMD_FORWARD_E]                */
        packetType = CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E;
        cmd        = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssDxChBrgVlanUnkUnregFilterSet(dev, vlanId, packetType, cmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, packetType, cmd);

        /* Call with packetType [CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E] */
        /*           and cmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E]                   */
        packetType = CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E;
        cmd        = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssDxChBrgVlanUnkUnregFilterSet(dev, vlanId, packetType, cmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, packetType, cmd);

        /* Call with packetType [CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV4_MCAST_E] */
        /*           and cmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E]                   */
        packetType = CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E;
        cmd        = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssDxChBrgVlanUnkUnregFilterSet(dev, vlanId, packetType, cmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, packetType, cmd);

        /* Call with packetType [CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV6_MCAST_E] */
        /*           and cmd [CPSS_PACKET_CMD_DROP_HARD_E]                     */
        packetType = CPSS_DXCH_BRG_VLAN_PACKET_UNREG_IPV6_MCAST_E;
        cmd        = CPSS_PACKET_CMD_DROP_HARD_E;

        st = cpssDxChBrgVlanUnkUnregFilterSet(dev, vlanId, packetType, cmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, packetType, cmd);

        /* Call with packetType [CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IPV4_BCAST_E] */
        /*           and cmd [CPSS_PACKET_CMD_DROP_SOFT_E]                         */
        packetType = CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IPV4_BCAST_E;
        cmd        = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssDxChBrgVlanUnkUnregFilterSet(dev, vlanId, packetType, cmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, packetType, cmd);

        /*
            1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                           packetType [CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E]
                           and cmd [CPSS_PACKET_CMD_FORWARD_E].
            Expected: GT_BAD_PARAM.
        */
        vlanId     = PRV_CPSS_MAX_NUM_VLANS_CNS;
        packetType = CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E;
        cmd        = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssDxChBrgVlanUnkUnregFilterSet(dev, vlanId, packetType, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        /*
            1.3. Call with vlanId[100], wrong enum values packetType
                           and cmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E].
            Expected: GT_BAD_PARAM.
        */
        vlanId     = BRG_VLAN_TESTED_VLAN_ID_CNS;
        cmd        = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanUnkUnregFilterSet
                                                        (dev, vlanId, packetType, cmd),
                                                        packetType);

        packetType = CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E;

        /*
            1.4. Call with vlanId[100],
                           packetType [CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E]
                           and wrong enum values cmd.
            Expected: GT_BAD_PARAM.
        */
        packetType = CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IP_MCAST_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanUnkUnregFilterSet
                                                        (dev, vlanId, packetType, cmd),
                                                        cmd);
    }

    vlanId     = BRG_VLAN_TESTED_VLAN_ID_CNS;
    packetType = CPSS_DXCH_BRG_VLAN_PACKET_UNK_UCAST_E;
    cmd        = CPSS_PACKET_CMD_FORWARD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanUnkUnregFilterSet(dev, vlanId, packetType, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanUnkUnregFilterSet(dev, vlanId, packetType, cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanForcePvidEnable
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanForcePvidEnable)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call get func.
    Expected: GT_OK and the same state.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = BRG_VLAN_VALID_PHY_PORT_CNS;
    GT_BOOL     state = GT_TRUE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_TRUE and GT_FALSE].
                Expected: GT_OK.
            */

            /* Call function with enable [GT_FALSE] */
            state = GT_FALSE;

            st = cpssDxChBrgVlanForcePvidEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call get func.
                Expected: GT_OK and the same state.
            */

            st = cpssDxChBrgVlanForcePvidEnableGet(dev, port, &stateGet);

            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                               "get another state than was set: %d", dev);

            /* Call function with enable [GT_TRUE] */
            state = GT_TRUE;

            st = cpssDxChBrgVlanForcePvidEnable(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call get func.
                Expected: GT_OK and the same state.
            */

            st = cpssDxChBrgVlanForcePvidEnableGet(dev, port, &stateGet);

            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
            UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                               "get another state than was set: %d", dev);
        }

        state = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanForcePvidEnable(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanForcePvidEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanForcePvidEnable(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    state = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanForcePvidEnable(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanForcePvidEnable(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanForcePvidEnableGet
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanForcePvidEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with not null enable.
    Expected: GT_OK.
    1.1.2. Call with wrong enable [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = BRG_VLAN_VALID_PHY_PORT_CNS;
    GT_BOOL     state = GT_TRUE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not null enable.
                Expected: GT_OK.
            */

            st = cpssDxChBrgVlanForcePvidEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call with wrong enable [NULL].
                Expected: GT_BAD_PTR.
            */

            st = cpssDxChBrgVlanForcePvidEnableGet(dev, port, &state);

            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanForcePvidEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanForcePvidEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanForcePvidEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanForcePvidEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanForcePvidEnableGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortVidPrecedenceSet
(
    IN GT_U8                                        devNum,
    IN GT_U8                                        port,
    IN CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  precedence
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortVidPrecedenceSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxChx)
    1.1.1. Call with precedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E /
                                 CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E].
    Expected: GT_OK.
    1.1.2. Call with out of range precedence.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS  st    = GT_OK;


    GT_U8                                       dev;
    GT_U8                                       port       = BRG_VLAN_VALID_PHY_PORT_CNS;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT precedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with precedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E /
                                             CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E].
                Expected: GT_OK.
            */

            /* Call function with precedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E] */
            precedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

            st = cpssDxChBrgVlanPortVidPrecedenceSet(dev, port, precedence);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, precedence);

            /* Call function with precedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E] */
            precedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;

            st = cpssDxChBrgVlanPortVidPrecedenceSet(dev, port, precedence);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, precedence);

            /*
                1.1.2. Call with wrong enum values precedence.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanPortVidPrecedenceSet
                                                        (dev, port, precedence),
                                                        precedence);
        }

        precedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanPortVidPrecedenceSet(dev, port, precedence);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanPortVidPrecedenceSet(dev, port, precedence);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanPortVidPrecedenceSet(dev, port, precedence);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    precedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortVidPrecedenceSet(dev, port, precedence);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortVidPrecedenceSet(dev, port, precedence);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanIpUcRouteEnable
(
    IN GT_U8                                    devNum,
    IN GT_U16                                   vlanId,
    IN CPSS_IP_PROTOCOL_STACK_ENT               protocol,
    IN GT_BOOL                                  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanIpUcRouteEnable)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100],
                   protocol [CPSS_IP_PROTOCOL_IPV4_E /
                             CPSS_IP_PROTOCOL_IPV6_E /
                             CPSS_IP_PROTOCOL_IPV4V6_E]
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                   protocol [CPSS_IP_PROTOCOL_IPV4V6_E]
                   and enable [GT_TRUE].
    Expected: GT_BAD_PARAM.
    1.3. Call with vlanId [100],
                   out of range protocol
                   and enable [GT_TRUE].
    Expected: GT_BAD_PARAM.

*/
    GT_STATUS  st    = GT_OK;


    GT_U8                      dev;
    GT_U16                     vlanId   = 0;
    CPSS_IP_PROTOCOL_STACK_ENT protocol = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                    enable   = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100],
                           protocol [CPSS_IP_PROTOCOL_IPV4_E /
                                     CPSS_IP_PROTOCOL_IPV6_E /
                                     CPSS_IP_PROTOCOL_IPV4V6_E]
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /* Call with protocol [CPSS_IP_PROTOCOL_IPV4_E] and enable [GT_FALSE] */
        protocol = CPSS_IP_PROTOCOL_IPV4_E;
        enable   = GT_FALSE;

        st = cpssDxChBrgVlanIpUcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, protocol, enable);

        /* Call with protocol [CPSS_IP_PROTOCOL_IPV6_E] and enable [GT_FALSE] */
        protocol = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssDxChBrgVlanIpUcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, protocol, enable);

        /* Call with protocol [CPSS_IP_PROTOCOL_IPV4V6_E] and enable [GT_FALSE] */
        protocol = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssDxChBrgVlanIpUcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, protocol, enable);

        /* Call with protocol [CPSS_IP_PROTOCOL_IPV4_E] and enable [GT_TRUE] */
        protocol = CPSS_IP_PROTOCOL_IPV4_E;
        enable   = GT_TRUE;

        st = cpssDxChBrgVlanIpUcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, protocol, enable);

        /* Call with protocol [CPSS_IP_PROTOCOL_IPV6_E] and enable [GT_TRUE] */
        protocol = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssDxChBrgVlanIpUcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, protocol, enable);

        /* Call with protocol [CPSS_IP_PROTOCOL_IPV4V6_E] and enable [GT_TRUE] */
        protocol = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssDxChBrgVlanIpUcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, protocol, enable);


        /*
            1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                           protocol [CPSS_IP_PROTOCOL_IPV4V6_E]
                           and enable [GT_TRUE].
            Expected: GT_BAD_PARAM.
        */
        vlanId   = PRV_CPSS_MAX_NUM_VLANS_CNS;
        protocol = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssDxChBrgVlanIpUcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        /*
            1.3. Call with vlanId [100], wrong enum values protocol and enable [GT_TRUE].
            Expected: GT_BAD_PARAM.
        */
        vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanIpUcRouteEnable
                                                        (dev, vlanId, protocol, enable),
                                                        protocol);
    }

    vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
    protocol = CPSS_IP_PROTOCOL_IPV4_E;
    enable   = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanIpUcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanIpUcRouteEnable(dev, vlanId, protocol, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanIpMcRouteEnable
(
    IN GT_U8                            devNum,
    IN GT_U16                           vlanId,
    IN CPSS_IP_PROTOCOL_STACK_ENT       protocol,
    IN GT_BOOL                          enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanIpMcRouteEnable)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with vlanId [100],
                   protocol [CPSS_IP_PROTOCOL_IPV4_E /
                             CPSS_IP_PROTOCOL_IPV6_E /
                             CPSS_IP_PROTOCOL_IPV4V6_E]
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                   protocol [CPSS_IP_PROTOCOL_IPV4V6_E]
                   and enable [GT_TRUE].
    Expected: GT_BAD_PARAM.
    1.3. Call with vlanId [100],
                   out of range protocol
                   and enable [GT_TRUE].
    Expected: GT_BAD_PARAM.

*/
    GT_STATUS  st    = GT_OK;


    GT_U8                      dev;
    GT_U16                     vlanId   = 0;
    CPSS_IP_PROTOCOL_STACK_ENT protocol = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                    enable   = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100],
                           protocol [CPSS_IP_PROTOCOL_IPV4_E /
                                     CPSS_IP_PROTOCOL_IPV6_E /
                                     CPSS_IP_PROTOCOL_IPV4V6_E]
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /* Call with protocol [CPSS_IP_PROTOCOL_IPV4_E] and enable [GT_FALSE] */
        protocol = CPSS_IP_PROTOCOL_IPV4_E;
        enable   = GT_FALSE;

        st = cpssDxChBrgVlanIpMcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, protocol, enable);

        /* Call with protocol [CPSS_IP_PROTOCOL_IPV6_E] and enable [GT_FALSE] */
        protocol = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssDxChBrgVlanIpMcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, protocol, enable);

        /* Call with protocol [CPSS_IP_PROTOCOL_IPV4V6_E] and enable [GT_FALSE] */
        protocol = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssDxChBrgVlanIpMcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, protocol, enable);

        /* Call with protocol [CPSS_IP_PROTOCOL_IPV4_E] and enable [GT_TRUE] */
        protocol = CPSS_IP_PROTOCOL_IPV4_E;
        enable   = GT_TRUE;

        st = cpssDxChBrgVlanIpMcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, protocol, enable);

        /* Call with protocol [CPSS_IP_PROTOCOL_IPV6_E] and enable [GT_TRUE] */
        protocol = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssDxChBrgVlanIpMcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, protocol, enable);

        /* Call with protocol [CPSS_IP_PROTOCOL_IPV4V6_E] and enable [GT_TRUE] */
        protocol = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssDxChBrgVlanIpMcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, protocol, enable);


        /*
            1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                           protocol [CPSS_IP_PROTOCOL_IPV4V6_E]
                           and enable [GT_TRUE].
            Expected: GT_BAD_PARAM.
        */
        vlanId   = PRV_CPSS_MAX_NUM_VLANS_CNS;
        protocol = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssDxChBrgVlanIpMcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        /*
            1.3. Call with vlanId [100], wrong enum values protocol and enable [GT_TRUE].
            Expected: GT_BAD_PARAM.
        */
        vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanIpMcRouteEnable
                                                        (dev, vlanId, protocol, enable),
                                                        protocol);
    }

    vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
    protocol = CPSS_IP_PROTOCOL_IPV4_E;
    enable   = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanIpMcRouteEnable(dev, vlanId, protocol, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanIpMcRouteEnable(dev, vlanId, protocol, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanNASecurEnable
(
    IN GT_U8           devNum,
    IN GT_U16          vlanId,
    IN GT_BOOL         enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanNASecurEnable)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100]
                   and enable [GT_FALSE and GT_ TRUE].
    Expected: GT_OK.
    1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and enable [GT_FALSE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId = 0;
    GT_BOOL   status = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100]
                           and enable [GT_FALSE and GT_ TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        status = GT_FALSE;

        st = cpssDxChBrgVlanNASecurEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, status);

        /* Call with enable [GT_TRUE] */
        status = GT_TRUE;

        st = cpssDxChBrgVlanNASecurEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, status);

        /*
            1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and enable [GT_FALSE].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        status = GT_FALSE;

        st = cpssDxChBrgVlanNASecurEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, status);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    status = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanNASecurEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanNASecurEnable(dev, vlanId, status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanIgmpSnoopingEnable
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanIgmpSnoopingEnable)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100] and enable [GT_FALSE and GT_ TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgVlanIgmpSnoopingEnableGet with the same parameters.
    Expected: GT_OK.
    1.3. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and enable [GT_FALSE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId = 0;
    GT_BOOL   status = GT_FALSE;
    GT_BOOL   statusGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100]
                           and enable [GT_FALSE and GT_ TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        status = GT_FALSE;

        st = cpssDxChBrgVlanIgmpSnoopingEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, status);

        /*
            1.2. Call cpssDxChBrgVlanIgmpSnoopingEnableGet
                 with the same parameters.
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanIgmpSnoopingEnableGet(dev, vlanId, &statusGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgVlanIgmpSnoopingEnableGet: %d", dev);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(status, statusGet,
                   "get another status than was set: %d", dev);


        /* Call with enable [GT_TRUE] */
        status = GT_TRUE;

        st = cpssDxChBrgVlanIgmpSnoopingEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, status);

        /*
            1.2. Call cpssDxChBrgVlanIgmpSnoopingEnableGet
                 with the same parameters.
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanIgmpSnoopingEnableGet(dev, vlanId, &statusGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgVlanIgmpSnoopingEnableGet: %d", dev);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(status, statusGet,
                   "get another status than was set: %d", dev);

        /*
            1.3. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and enable [GT_FALSE].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        status = GT_FALSE;

        st = cpssDxChBrgVlanIgmpSnoopingEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, status);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    status = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanIgmpSnoopingEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanIgmpSnoopingEnable(dev, vlanId, status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanIgmpSnoopingEnableGet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanIgmpSnoopingEnableGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100] and non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId = 0;
    GT_BOOL   statusGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100] and non-null enablePtr.
            Expected: GT_OK.
        */

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssDxChBrgVlanIgmpSnoopingEnableGet(dev, vlanId, &statusGet);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, statusGet);

        /*
            1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanIgmpSnoopingEnableGet(dev, vlanId, &statusGet);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, statusGet);

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /*
            1.3. Call with wrong enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanIgmpSnoopingEnableGet(dev, vlanId, NULL);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PTR, st, dev, vlanId, statusGet);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanIgmpSnoopingEnableGet(dev, vlanId, &statusGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanIgmpSnoopingEnableGet(dev, vlanId, &statusGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanIpCntlToCpuSet
(
    IN GT_U8                               devNum,
    IN GT_U16                              vlanId,
    IN CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT      ipCntrlType
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanIpCntlToCpuSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100],
                   ipCntrlType [CPSS_DXCH_BRG_IP_CTRL_NONE_E /
                                CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E].
    Expected: GT_OK.
    1.2. Call with vlanId [100],
                   ipCntrlType [CPSS_DXCH_BRG_IP_CTRL_IPV4_E /
                                CPSS_DXCH_BRG_IP_CTRL_IPV6_E].
    Expected: GT_OK for DxCh2, DxCh3 and NOT GT_OK for others.
    1.3. Call cpssDxChBrgVlanIpCntlToCpuGet with the same
                             parameters as in 1.1 and 1.2.
    Expected: GT_OK and the same value.
    1.4. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                   and ipCntrlType [CPSS_DXCH_BRG_IP_CTRL_NONE_E].
    Expected: GT_BAD_PARAM.
    1.5. Call with vlanId [100]
                   and out of range ipCntrlType.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS  st    = GT_OK;


    GT_U8                          dev;
    GT_U16                         vlanId      = 0;
    CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT ipCntrlType = CPSS_DXCH_BRG_IP_CTRL_NONE_E;
    CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT ipCntrlTypeGet;
    CPSS_PP_FAMILY_TYPE_ENT        devFamily;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100],
                           ipCntrlType [CPSS_DXCH_BRG_IP_CTRL_NONE_E /
                                        CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E].
            Expected: GT_OK.
        */

        /* Call with ipCntrlType [CPSS_DXCH_BRG_IP_CTRL_NONE_E] */
        vlanId      = BRG_VLAN_TESTED_VLAN_ID_CNS;
        ipCntrlType = CPSS_DXCH_BRG_IP_CTRL_NONE_E;

        st = cpssDxChBrgVlanIpCntlToCpuSet(dev, vlanId, ipCntrlType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, ipCntrlType);

        /*
            1.3. Call cpssDxChBrgVlanIpCntlToCpuGet with the same
                                     parameters as in 1.1 and 1.2.
            Expected: GT_OK and the same value.
        */

        st = cpssDxChBrgVlanIpCntlToCpuGet(dev, vlanId, &ipCntrlTypeGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgVlanIpCntlToCpuGet: %d", dev);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ipCntrlType, ipCntrlTypeGet,
                   "get another ipCntrlType than was set: %d", dev);


        /* Call with ipCntrlType [CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E] */
        ipCntrlType = CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E;

        st = cpssDxChBrgVlanIpCntlToCpuSet(dev, vlanId, ipCntrlType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, ipCntrlType);

        /*
            1.3. Call cpssDxChBrgVlanIpCntlToCpuGet with the same
                                     parameters as in 1.1 and 1.2.
            Expected: GT_OK and the same value.
        */

        st = cpssDxChBrgVlanIpCntlToCpuGet(dev, vlanId, &ipCntrlTypeGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgVlanIpCntlToCpuGet: %d", dev);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ipCntrlType, ipCntrlTypeGet,
                   "get another ipCntrlType than was set: %d", dev);


        /* Getting device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /*
            1.2. Call with vlanId [100],
                           ipCntrlType [CPSS_DXCH_BRG_IP_CTRL_IPV4_E /
                                        CPSS_DXCH_BRG_IP_CTRL_IPV6_E].
            Expected: GT_OK for DxCh2, DxCh3 and NOT GT_OK for others.
        */

        /* Call with ipCntrlType [CPSS_DXCH_BRG_IP_CTRL_IPV4_E] */
        ipCntrlType = CPSS_DXCH_BRG_IP_CTRL_IPV4_E;

        st = cpssDxChBrgVlanIpCntlToCpuSet(dev, vlanId, ipCntrlType);
        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipCntrlType = %d",
                                         dev, ipCntrlType);
            if(GT_OK == st)
            {
                /*
                    1.3. Call cpssDxChBrgVlanIpCntlToCpuGet with the same
                                             parameters as in 1.1 and 1.2.
                    Expected: GT_OK and the same value.
                */

                st = cpssDxChBrgVlanIpCntlToCpuGet(dev, vlanId, &ipCntrlTypeGet);

                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                           "cpssDxChBrgVlanIpCntlToCpuGet: %d", dev);
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(ipCntrlType, ipCntrlTypeGet,
                           "get another ipCntrlType than was set: %d", dev);
            }
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipCntrlType = %d",
                                             dev, ipCntrlType);
        }

        /* Call with ipCntrlType [CPSS_DXCH_BRG_IP_CTRL_IPV6_E] */
        ipCntrlType = CPSS_DXCH_BRG_IP_CTRL_IPV6_E;

        st = cpssDxChBrgVlanIpCntlToCpuSet(dev, vlanId, ipCntrlType);
        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipCntrlType = %d",
                                         dev, ipCntrlType);
            if(GT_OK == st)
            {
                /*
                    1.3. Call cpssDxChBrgVlanIpCntlToCpuGet with the same
                                             parameters as in 1.1 and 1.2.
                    Expected: GT_OK and the same value.
                */

                st = cpssDxChBrgVlanIpCntlToCpuGet(dev, vlanId, &ipCntrlTypeGet);

                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                           "cpssDxChBrgVlanIpCntlToCpuGet: %d", dev);
                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(ipCntrlType, ipCntrlTypeGet,
                           "get another ipCntrlType than was set: %d", dev);
            }
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipCntrlType = %d",
                                             dev, ipCntrlType);
        }

        /*
            1.4. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                           and ipCntrlType [CPSS_DXCH_BRG_IP_CTRL_NONE_E].
            Expected: GT_BAD_PARAM.
        */
        vlanId      = PRV_CPSS_MAX_NUM_VLANS_CNS;
        ipCntrlType = CPSS_DXCH_BRG_IP_CTRL_NONE_E;

        st = cpssDxChBrgVlanIpCntlToCpuSet(dev, vlanId, ipCntrlType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        /*
            1.5. Call with vlanId [100] and wrong enum values ipCntrlType.
            Expected: GT_BAD_PARAM.
        */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanIpCntlToCpuSet
                                                        (dev, vlanId, ipCntrlType),
                                                        ipCntrlType);
    }

    vlanId      = BRG_VLAN_TESTED_VLAN_ID_CNS;
    ipCntrlType = CPSS_DXCH_BRG_IP_CTRL_NONE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanIpCntlToCpuSet(dev, vlanId, ipCntrlType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanIpCntlToCpuSet(dev, vlanId, ipCntrlType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanIpCntlToCpuGet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    OUT CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT      *ipCntrlTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanIpCntlToCpuGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100] and non-null ipCntrlTypePtr.
    Expected: GT_OK.
    1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong ipCntrlTypePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId = 0;
    CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT ipCntrlTypeGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100] and non-null ipCntrlTypePtr.
            Expected: GT_OK.
        */

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssDxChBrgVlanIpCntlToCpuGet(dev, vlanId, &ipCntrlTypeGet);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, ipCntrlTypeGet);

        /*
            1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanIpCntlToCpuGet(dev, vlanId, &ipCntrlTypeGet);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, ipCntrlTypeGet);

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /*
            1.3. Call with wrong ipCntrlTypePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanIpCntlToCpuGet(dev, vlanId, NULL);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PTR, st, dev, vlanId, ipCntrlTypeGet);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanIpCntlToCpuGet(dev, vlanId, &ipCntrlTypeGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanIpCntlToCpuGet(dev, vlanId, &ipCntrlTypeGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanIpV6IcmpToCpuEnable
(
    IN GT_U8      devNum,
    IN GT_U16     vlanId,
    IN GT_BOOL    enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanIpV6IcmpToCpuEnable)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100] and enable [GT_FALSE and GT_ TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgVlanIpV6IcmpToCpuEnableGet with the same parameters.
    Expected: GT_OK.
    1.3. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and enable [GT_FALSE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId = 0;
    GT_BOOL   status = GT_FALSE;
    GT_BOOL   statusGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100] and enable [GT_FALSE and GT_ TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        status = GT_FALSE;

        st = cpssDxChBrgVlanIpV6IcmpToCpuEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, status);

        /*
            1.2. Call cpssDxChBrgVlanIpV6IcmpToCpuEnableGet
                 with the same parameters.
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanIpV6IcmpToCpuEnableGet(dev, vlanId, &statusGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgVlanIpV6IcmpToCpuEnableGet: %d", dev);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(status, statusGet,
                   "get another status than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        status = GT_TRUE;

        st = cpssDxChBrgVlanIpV6IcmpToCpuEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, status);

        /*
            1.2. Call cpssDxChBrgVlanIpV6IcmpToCpuEnableGet
                 with the same parameters.
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanIpV6IcmpToCpuEnableGet(dev, vlanId, &statusGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgVlanIpV6IcmpToCpuEnableGet: %d", dev);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(status, statusGet,
                   "get another status than was set: %d", dev);

        /*
            1.3. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and enable [GT_FALSE].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        status = GT_FALSE;

        st = cpssDxChBrgVlanIpV6IcmpToCpuEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, status);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    status = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanIpV6IcmpToCpuEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanIpV6IcmpToCpuEnable(dev, vlanId, status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanIpV6IcmpToCpuEnableGet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanIpV6IcmpToCpuEnableGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100] and non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId = 0;
    GT_BOOL   statusGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100] and non-null enablePtr.
            Expected: GT_OK.
        */

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssDxChBrgVlanIpV6IcmpToCpuEnableGet(dev, vlanId, &statusGet);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, statusGet);

        /*
            1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanIpV6IcmpToCpuEnableGet(dev, vlanId, &statusGet);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, statusGet);

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /*
            1.3. Call with wrong enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanIpV6IcmpToCpuEnableGet(dev, vlanId, NULL);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PTR, st, dev, vlanId, statusGet);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanIpV6IcmpToCpuEnableGet(dev, vlanId, &statusGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanIpV6IcmpToCpuEnableGet(dev, vlanId, &statusGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanUdpBcPktsToCpuEnable
(
    IN  GT_U8       devNum,
    IN  GT_U16      vlanId,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanUdpBcPktsToCpuEnable)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with vlanId [100] and enable [GT_FALSE and GT_ TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgVlanUdpBcPktsToCpuEnableGet with the same parameters.
    Expected: GT_OK and the same value.
    1.3. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and enable [GT_FALSE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId = 0;
    GT_BOOL   status = GT_FALSE;
    GT_BOOL   statusGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100] and enable [GT_FALSE and GT_ TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        status = GT_FALSE;

        st = cpssDxChBrgVlanUdpBcPktsToCpuEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, status);

        /*
            1.2. Call cpssDxChBrgVlanUdpBcPktsToCpuEnableGet
                 with the same parameters.
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanUdpBcPktsToCpuEnableGet(dev, vlanId, &statusGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgVlanUdpBcPktsToCpuEnableGet: %d", dev);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(status, statusGet,
                   "get another status than was set: %d", dev);


        /* Call with enable [GT_TRUE] */
        status = GT_TRUE;

        st = cpssDxChBrgVlanUdpBcPktsToCpuEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, status);

        /*
            1.2. Call cpssDxChBrgVlanUdpBcPktsToCpuEnableGet
                 with the same parameters.
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanUdpBcPktsToCpuEnableGet(dev, vlanId, &statusGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgVlanUdpBcPktsToCpuEnableGet: %d", dev);
        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(status, statusGet,
                   "get another status than was set: %d", dev);

        /*
            1.3. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and enable [GT_FALSE].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        status = GT_FALSE;

        st = cpssDxChBrgVlanUdpBcPktsToCpuEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, status);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    status = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanUdpBcPktsToCpuEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanUdpBcPktsToCpuEnable(dev, vlanId, status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanUdpBcPktsToCpuEnableGet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanUdpBcPktsToCpuEnableGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100] and non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId = 0;
    GT_BOOL   statusGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100] and non-null enablePtr.
            Expected: GT_OK.
        */

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssDxChBrgVlanUdpBcPktsToCpuEnableGet(dev, vlanId, &statusGet);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, statusGet);

        /*
            1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanUdpBcPktsToCpuEnableGet(dev, vlanId, &statusGet);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, statusGet);

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /*
            1.3. Call with wrong enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanUdpBcPktsToCpuEnableGet(dev, vlanId, NULL);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PTR, st, dev, vlanId, statusGet);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanUdpBcPktsToCpuEnableGet(dev, vlanId, &statusGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanUdpBcPktsToCpuEnableGet(dev, vlanId, &statusGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanIpv6SourceSiteIdSet
(
    IN GT_U8                                devNum,
    IN GT_U16                               vlanId,
    IN CPSS_IP_SITE_ID_ENT                  siteId
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanIpv6SourceSiteIdSet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with vlanId [100],
                   siteId [CPSS_IP_SITE_ID_INTERNAL_E /
                           CPSS_IP_SITE_ID_EXTERNAL_E].
    Expected: GT_OK.
    1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                   siteId [CPSS_IP_SITE_ID_INTERNAL_E].
    Expected: GT_BAD_PARAM.
    1.3. Call with vlanId [100]
                   and out of range siteId.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS  st    = GT_OK;


    GT_U8               dev;
    GT_U16              vlanId = 0;
    CPSS_IP_SITE_ID_ENT siteId = CPSS_IP_SITE_ID_INTERNAL_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100],
                           siteId [CPSS_IP_SITE_ID_INTERNAL_E /
                                   CPSS_IP_SITE_ID_EXTERNAL_E].
            Expected: GT_OK.
        */

        /* Call with siteId [CPSS_IP_SITE_ID_INTERNAL_E] */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        siteId = CPSS_IP_SITE_ID_INTERNAL_E;

        st = cpssDxChBrgVlanIpv6SourceSiteIdSet(dev, vlanId, siteId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, siteId);

        /* Call with siteId [CPSS_IP_SITE_ID_EXTERNAL_E] */
        siteId = CPSS_IP_SITE_ID_EXTERNAL_E;

        st = cpssDxChBrgVlanIpv6SourceSiteIdSet(dev, vlanId, siteId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, siteId);

        /*
            1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                           siteId [CPSS_IP_SITE_ID_INTERNAL_E].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        siteId = CPSS_IP_SITE_ID_INTERNAL_E;

        st = cpssDxChBrgVlanIpv6SourceSiteIdSet(dev, vlanId, siteId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        /*
            1.3. Call with vlanId [100] and wrong enum values siteId.
            Expected: GT_BAD_PARAM.
        */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanIpv6SourceSiteIdSet
                                                        (dev, vlanId, siteId),
                                                        siteId);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    siteId = CPSS_IP_SITE_ID_INTERNAL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanIpv6SourceSiteIdSet(dev, vlanId, siteId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanIpv6SourceSiteIdSet(dev, vlanId, siteId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanIpmBridgingEnable
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipVer,
    IN  GT_BOOL                     enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanIpmBridgingEnable)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100],
                   ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                          CPSS_IP_PROTOCOL_IPV6_E /
                          CPSS_IP_PROTOCOL_IPV4V6_E]
                   and enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                   ipVer [CPSS_IP_PROTOCOL_IPV4V6_E]
                   and enable [GT_TRUE].
    Expected: GT_BAD_PARAM.
    1.3. Call with vlanId [100],
                   out of range ipVer
                   and enable [GT_TRUE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st    = GT_OK;


    GT_U8                      dev;
    GT_U16                     vlanId = 0;
    CPSS_IP_PROTOCOL_STACK_ENT ipVer  = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                    enable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100],
                           ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E /
                                  CPSS_IP_PROTOCOL_IPV4V6_E]
                           and enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with ipVer [CPSS_IP_PROTOCOL_IPV4_E] and enable [GT_FALSE] */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        ipVer  = CPSS_IP_PROTOCOL_IPV4_E;
        enable = GT_FALSE;

        st = cpssDxChBrgVlanIpmBridgingEnable(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, enable);

        /* Call with ipVer [CPSS_IP_PROTOCOL_IPV6_E] and enable [GT_FALSE] */
        ipVer  = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssDxChBrgVlanIpmBridgingEnable(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, enable);

        /* Call with ipVer [CPSS_IP_PROTOCOL_IPV4V6_E] and enable [GT_FALSE] */
        ipVer  = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssDxChBrgVlanIpmBridgingEnable(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, enable);

        /* Call with ipVer [CPSS_IP_PROTOCOL_IPV4_E] and enable [GT_TRUE] */
        ipVer  = CPSS_IP_PROTOCOL_IPV4_E;
        enable = GT_TRUE;

        st = cpssDxChBrgVlanIpmBridgingEnable(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, enable);

        /* Call with ipVer [CPSS_IP_PROTOCOL_IPV6_E] and enable [GT_TRUE] */
        ipVer  = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssDxChBrgVlanIpmBridgingEnable(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, enable);

        /* Call with ipVer [CPSS_IP_PROTOCOL_IPV4V6_E] and enable [GT_TRUE] */
        ipVer  = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssDxChBrgVlanIpmBridgingEnable(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, enable);

        /*
            1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                           ipVer [CPSS_IP_PROTOCOL_IPV4V6_E]
                           and enable [GT_TRUE].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanIpmBridgingEnable(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        /*
            1.3. Call with vlanId [100], wrong enum values ipVer and enable [GT_TRUE].
            Expected: GT_BAD_PARAM.
        */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanIpmBridgingEnable
                                                        (dev, vlanId, ipVer, enable),
                                                        ipVer);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    ipVer  = CPSS_IP_PROTOCOL_IPV4_E;
    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanIpmBridgingEnable(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanIpmBridgingEnable(dev, vlanId, ipVer, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanIpmBridgingModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipVer,
    IN  CPSS_BRG_IPM_MODE_ENT       ipmMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanIpmBridgingModeSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100],
                   ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                          CPSS_IP_PROTOCOL_IPV6_E /
                          CPSS_IP_PROTOCOL_IPV4V6_E]
                   and ipmMode [CPSS_BRG_IPM_SGV_E /
                                CPSS_BRG_IPM_GV_E].
    Expected: GT_OK.
    1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                   ipVer [CPSS_IP_PROTOCOL_IPV4V6_E]
                   and ipmMode [CPSS_BRG_IPM_SGV_E].
    Expected: GT_BAD_PARAM.
    1.3. Call with vlanId [100],
                   out of range ipVer
                   and ipmMode [CPSS_BRG_IPM_GV_E].
    Expected: GT_BAD_PARAM.
    1.4. Call with vlanId [100],
                   ipVer [CPSS_IP_PROTOCOL_IPV6_E]
                   and out of range ipmMode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS  st    = GT_OK;


    GT_U8                      dev;
    GT_U16                     vlanId  = 0;
    CPSS_IP_PROTOCOL_STACK_ENT ipVer   = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_BRG_IPM_MODE_ENT      ipmMode = CPSS_BRG_IPM_SGV_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100],
                           ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E /
                                  CPSS_IP_PROTOCOL_IPV4V6_E]
                           and ipmMode [CPSS_BRG_IPM_SGV_E /
                                        CPSS_BRG_IPM_GV_E].
            Expected: GT_OK.
        */
        /* Call with ipVer [CPSS_IP_PROTOCOL_IPV4_E] and ipmMode [CPSS_BRG_IPM_SGV_E] */
        vlanId  = BRG_VLAN_TESTED_VLAN_ID_CNS;
        ipVer   = CPSS_IP_PROTOCOL_IPV4_E;
        ipmMode = CPSS_BRG_IPM_SGV_E;

        st = cpssDxChBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, ipmMode);

        /* Call with ipVer [CPSS_IP_PROTOCOL_IPV6_E] and ipmMode [CPSS_BRG_IPM_SGV_E] */
        ipVer = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssDxChBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, ipmMode);

        /* Call with ipVer [CPSS_IP_PROTOCOL_IPV4V6_E] and ipmMode [CPSS_BRG_IPM_SGV_E] */
        ipVer = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssDxChBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, ipmMode);

        /* Call with ipVer [CPSS_IP_PROTOCOL_IPV4_E] and ipmMode [CPSS_BRG_IPM_GV_E] */
        ipVer   = CPSS_IP_PROTOCOL_IPV4_E;
        ipmMode = CPSS_BRG_IPM_GV_E;

        st = cpssDxChBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, ipmMode);

        /* Call with ipVer [CPSS_IP_PROTOCOL_IPV6_E] and ipmMode [CPSS_BRG_IPM_GV_E] */
        ipVer = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssDxChBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, ipmMode);

        /* Call with ipVer [CPSS_IP_PROTOCOL_IPV4V6_E] and ipmMode [CPSS_BRG_IPM_GV_E] */
        ipVer = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssDxChBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, ipmMode);

        /*
            1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                           ipVer [CPSS_IP_PROTOCOL_IPV4V6_E]
                           and ipmMode [CPSS_BRG_IPM_SGV_E].
            Expected: GT_BAD_PARAM.
        */
        vlanId  = PRV_CPSS_MAX_NUM_VLANS_CNS;
        ipmMode = CPSS_BRG_IPM_SGV_E;

        st = cpssDxChBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        /*
            1.3. Call with vlanId [100], wrong enum values ipVer and ipmMode [CPSS_BRG_IPM_GV_E].
            Expected: GT_BAD_PARAM.
        */
        vlanId  = BRG_VLAN_TESTED_VLAN_ID_CNS;
        ipmMode = CPSS_BRG_IPM_GV_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanIpmBridgingModeSet
                                                        (dev, vlanId, ipVer, ipmMode),
                                                        ipVer);

        ipVer   = CPSS_IP_PROTOCOL_IPV6_E;

        /*
            1.4. Call with vlanId [100], ipVer [CPSS_IP_PROTOCOL_IPV6_E]
                 and wrong enum values ipmMode.
            Expected: GT_BAD_PARAM.
        */
        vlanId  = BRG_VLAN_TESTED_VLAN_ID_CNS;
        ipVer   = CPSS_IP_PROTOCOL_IPV6_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanIpmBridgingModeSet
                                                        (dev, vlanId, ipVer, ipmMode),
                                                        ipmMode);
    }

    vlanId  = BRG_VLAN_TESTED_VLAN_ID_CNS;
    ipVer   = CPSS_IP_PROTOCOL_IPV4_E;
    ipmMode = CPSS_BRG_IPM_SGV_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanIngressMirrorEnable
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanIngressMirrorEnable)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with vlanId [100] and enable [GT_FALSE and GT_ TRUE].
    Expected: GT_OK.
    1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and enable [GT_FALSE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId = 0;
    GT_BOOL   status = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100] and enable [GT_FALSE and GT_ TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        status = GT_FALSE;

        st = cpssDxChBrgVlanIngressMirrorEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, status);

        /* Call with enable [GT_TRUE] */
        status = GT_TRUE;

        st = cpssDxChBrgVlanIngressMirrorEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, status);

        /*
            1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and enable [GT_FALSE].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        status = GT_FALSE;

        st = cpssDxChBrgVlanIngressMirrorEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, status);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    status = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanIngressMirrorEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanIngressMirrorEnable(dev, vlanId, status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortAccFrameTypeSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    IN  CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     frameType
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortAccFrameTypeSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh2 and above)
    1.1.1. Call with frameType [CPSS_PORT_ACCEPT_FRAME_TAGGED_E /
                                CPSS_PORT_ACCEPT_FRAME_ALL_E /
                                CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E].
    Expected: GT_OK.
    1.1.2. Call cpssDxChBrgVlanPortAccFrameTypeGet with the same params.
    Expected: GT_OK and the same value.
    1.1.3. Call with frameType [CPSS_PORT_ACCEPT_FRAME_NONE_E].
    Expected: non GT_OK.
    1.1.4. Call with out of range frameType.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS  st    = GT_OK;


    GT_U8                           dev;
    GT_U8                           port      = BRG_VLAN_VALID_PHY_PORT_CNS;
    CPSS_PORT_ACCEPT_FRAME_TYPE_ENT frameType = CPSS_PORT_ACCEPT_FRAME_TAGGED_E;
    CPSS_PORT_ACCEPT_FRAME_TYPE_ENT frameTypeGet;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with frameType [CPSS_PORT_ACCEPT_FRAME_TAGGED_E /
                                            CPSS_PORT_ACCEPT_FRAME_ALL_E /
                                            CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E].
                Expected: GT_OK.
            */

            /* Call with frameType [CPSS_PORT_ACCEPT_FRAME_TAGGED_E] */
            frameType = CPSS_PORT_ACCEPT_FRAME_TAGGED_E;

            st = cpssDxChBrgVlanPortAccFrameTypeSet(dev, port, frameType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, frameType);

            /*
                1.1.2. Call cpssDxChBrgVlanPortAccFrameTypeGet with the same params.
                Expected: GT_OK and the same value.
            */

            st = cpssDxChBrgVlanPortAccFrameTypeGet(dev, port, &frameTypeGet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, frameType);

            UTF_VERIFY_EQUAL2_STRING_MAC(frameType, frameTypeGet,
                       "get another frameType than was set: %d, %d", dev, port);

            /* Call with frameType [CPSS_PORT_ACCEPT_FRAME_ALL_E] */
            frameType = CPSS_PORT_ACCEPT_FRAME_ALL_E;

            st = cpssDxChBrgVlanPortAccFrameTypeSet(dev, port, frameType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, frameType);

            /*
                1.1.2. Call cpssDxChBrgVlanPortAccFrameTypeGet with the same params.
                Expected: GT_OK and the same value.
            */

            st = cpssDxChBrgVlanPortAccFrameTypeGet(dev, port, &frameTypeGet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, frameType);

            UTF_VERIFY_EQUAL2_STRING_MAC(frameType, frameTypeGet,
                       "get another frameType than was set: %d, %d", dev, port);

            /* Call with frameType [CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E] */
            frameType = CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E;

            st = cpssDxChBrgVlanPortAccFrameTypeSet(dev, port, frameType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, frameType);

            /*
                1.1.2. Call cpssDxChBrgVlanPortAccFrameTypeGet with the same params.
                Expected: GT_OK and the same value.
            */

            st = cpssDxChBrgVlanPortAccFrameTypeGet(dev, port, &frameTypeGet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, frameType);

            UTF_VERIFY_EQUAL2_STRING_MAC(frameType, frameTypeGet,
                       "get another frameType than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with frameType [CPSS_PORT_ACCEPT_FRAME_NONE_E].
                Expected: non GT_OK.
            */

            frameType = CPSS_PORT_ACCEPT_FRAME_NONE_E;

            st = cpssDxChBrgVlanPortAccFrameTypeSet(dev, port, frameType);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, frameType);

            /*
                1.1.4. Call with wrong enum values frameType.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanPortAccFrameTypeSet
                                                        (dev, port, frameType),
                                                        frameType);
        }
        frameType = CPSS_PORT_ACCEPT_FRAME_TAGGED_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanPortAccFrameTypeSet(dev, port, frameType);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanPortAccFrameTypeSet(dev, port, frameType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanPortAccFrameTypeSet(dev, port, frameType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    frameType = CPSS_PORT_ACCEPT_FRAME_TAGGED_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortAccFrameTypeSet(dev, port, frameType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortAccFrameTypeSet(dev, port, frameType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortAccFrameTypeGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    OUT CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     *frameTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortAccFrameTypeGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh2 and above)
    1.1.1. Call with not NULL frameTypePtr.
    Expected: GT_OK.
    1.1.2. Call with null frameTypePtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS                       st        = GT_OK;

    GT_U8                           dev;
    GT_U8                           port      = BRG_VLAN_VALID_PHY_PORT_CNS;
    CPSS_PORT_ACCEPT_FRAME_TYPE_ENT frameTypeGet;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not NULL frameTypePtr.
                Expected: GT_OK.
            */
            st = cpssDxChBrgVlanPortAccFrameTypeGet(dev, port, &frameTypeGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with null frameTypePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChBrgVlanPortAccFrameTypeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanPortAccFrameTypeGet(dev, port, &frameTypeGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanPortAccFrameTypeGet(dev, port, &frameTypeGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanPortAccFrameTypeGet(dev, port, &frameTypeGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortAccFrameTypeGet(dev, port, &frameTypeGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortAccFrameTypeGet(dev, port, &frameTypeGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanMruProfileIdxSet
(
    IN GT_U8     devNum,
    IN GT_U16    vlanId,
    IN GT_U32    mruIndex
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanMruProfileIdxSet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with vlanId [100], mruIndex [0].
    Expected: GT_OK.
    1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS], mruIndex [1].
    Expected: GT_BAD_PARAM.
    1.3. Call with vlanId [100] and mruIndex [PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS + 1].
    Expected: non GT_OK.
*/
    GT_STATUS st       = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId   = 0;
    GT_U32    mruIndex = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100], mruIndex [0].
            Expected: GT_OK.
        */
        vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
        mruIndex = 0;

        st = cpssDxChBrgVlanMruProfileIdxSet(dev, vlanId, mruIndex);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, mruIndex);

        /*
            1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS],
                           mruIndex [1].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        mruIndex = 1;

        st = cpssDxChBrgVlanMruProfileIdxSet(dev, vlanId, mruIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        /*
            1.3. Call with vlanId [100]
                           and mruIndex [PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS + 1].
            Expected: non GT_OK.
        */
        vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
        mruIndex = 8;

        st = cpssDxChBrgVlanMruProfileIdxSet(dev, vlanId, mruIndex);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, mruIndex);
    }

    vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
    mruIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanMruProfileIdxSet(dev, vlanId, mruIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanMruProfileIdxSet(dev, vlanId, mruIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanMruProfileValueSet
(
    IN GT_U8     devNum,
    IN GT_U32    mruIndex,
    IN GT_U32    mruValue
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanMruProfileValueSet)
{
/*
ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with mruIndex [0] and mruValue[1 / 0xFFFF].
    Expected: GT_OK.
    1.2. Call with mruIndex [PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS + 1], mruValue [16].
    Expected: non GT_OK.
    1.3. Call with mruIndex [2] and mruValue [0x10000].
    Expected: non GT_OK.
*/
    GT_STATUS st       = GT_OK;

    GT_U8     dev;
    GT_U32    mruIndex = 0;
    GT_U32    mruValue = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mruIndex [0] and mruValue[1 / 0xFFFF].
            Expected: GT_OK.
        */
        mruIndex = 0;
        mruValue = 1;

        st = cpssDxChBrgVlanMruProfileValueSet(dev, mruIndex, mruValue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mruIndex, mruValue);

        mruValue = 0xFFFF;

        st = cpssDxChBrgVlanMruProfileValueSet(dev, mruIndex, mruValue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, mruIndex, mruValue);

        /*
            1.2. Call with mruIndex [PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS + 1],
                           mruValue [16].
            Expected: non GT_OK.
        */
        mruIndex = 8;
        mruValue = 16;

        st = cpssDxChBrgVlanMruProfileValueSet(dev, mruIndex, mruValue);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, mruIndex, mruValue);

        /*
            1.3. Call with mruIndex [2] and mruValue [0x10000].
            Expected: non GT_OK.
        */
        mruIndex = 2;
        mruValue = 0x10000;

        st = cpssDxChBrgVlanMruProfileValueSet(dev, mruIndex, mruValue);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, mruIndex, mruValue);
    }

    mruIndex = 0;
    mruValue = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanMruProfileValueSet(dev, mruIndex, mruValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanMruProfileValueSet(dev, mruIndex, mruValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanNaToCpuEnable
(
    IN GT_U8     devNum,
    IN GT_U16    vlanId,
    IN GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanNaToCpuEnable)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with vlanId [100] and enable [GT_FALSE and GT_ TRUE].
    Expected: GT_OK.
    1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                   and enable [GT_FALSE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st     = GT_OK;

    GT_U8     dev;
    GT_U16    vlanId = 0;
    GT_BOOL   status = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100] and enable [GT_FALSE and GT_ TRUE].
            Expected: GT_OK.
        */
        /* Call with enable [GT_FALSE] */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        status = GT_FALSE;

        st = cpssDxChBrgVlanNaToCpuEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, status);

        /* Call with enable [GT_TRUE] */
        status = GT_TRUE;

        st = cpssDxChBrgVlanNaToCpuEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, status);

        /*
            1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                           and enable [GT_FALSE].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        status = GT_FALSE;

        st = cpssDxChBrgVlanNaToCpuEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, status);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    status = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanNaToCpuEnable(dev, vlanId, status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanNaToCpuEnable(dev, vlanId, status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanBridgingModeSet
(
    IN GT_U8                devNum,
    IN CPSS_BRG_MODE_ENT    brgMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanBridgingModeSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with brgMode [CPSS_BRG_MODE_802_1Q_E /
                            CPSS_BRG_MODE_802_1D_E].
    Expected: GT_OK.
    1.2. Call with out of range brgMode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st    = GT_OK;


    GT_U8             dev;
    CPSS_BRG_MODE_ENT brgMode = CPSS_BRG_MODE_802_1Q_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with brgMode [CPSS_BRG_MODE_802_1Q_E /
                                    CPSS_BRG_MODE_802_1D_E].
            Expected: GT_OK.
        */
        /* Call with brgMode [CPSS_BRG_MODE_802_1Q_E] */
        brgMode = CPSS_BRG_MODE_802_1Q_E;

        st = cpssDxChBrgVlanBridgingModeSet(dev, brgMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, brgMode);

        /* Call with brgMode [CPSS_BRG_MODE_802_1D_E] */
        brgMode = CPSS_BRG_MODE_802_1D_E;

        st = cpssDxChBrgVlanBridgingModeSet(dev, brgMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, brgMode);

        /*
            1.2. Call with wrong enum values brgMode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanBridgingModeSet
                                                        (dev, brgMode),
                                                        brgMode);
    }

    brgMode = CPSS_BRG_MODE_802_1Q_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanBridgingModeSet(dev, brgMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanBridgingModeSet(dev, brgMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanVrfIdSet
(
    IN GT_U8                   devNum,
    IN GT_U16                  vlanId,
    IN GT_U32                  vrfId
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanVrfIdSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call with vlanId [100 / 4095], vrfId [0 / 4095].
    Expected: GT_OK.
    1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS], vrfId [0].
    Expected: GT_BAD_PARAM.
    1.3. Call with vlanId [100] and vrfId [4096]
    Expected: non GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16  vlanId = 0;
    GT_U32  vrfId  = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [100 / 4095], vrfId [0 / 4095].
            Expected: GT_OK.
        */

        /* Call with vlanId [100] */
        vlanId = 100;
        vrfId  = 0;

        st = cpssDxChBrgVlanVrfIdSet(dev, vlanId, vrfId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, vrfId);

        /* Call with vlanId [4095] */
        vlanId = 4095;
        vrfId  = 4095;

        st = cpssDxChBrgVlanVrfIdSet(dev, vlanId, vrfId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, vrfId);

        /*
            1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS], vrfId [0].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanVrfIdSet(dev, vlanId, vrfId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        vlanId = 100;

        /*
            1.3. Call with vlanId [100] and vrfId [4096]
            Expected: non GT_OK.
        */
        vrfId = 4096;

        st = cpssDxChBrgVlanVrfIdSet(dev, vlanId, vrfId);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vrfId = %d", dev, vrfId);
    }

    vlanId = 100;
    vrfId  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanVrfIdSet(dev, vlanId, vrfId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanVrfIdSet(dev, vlanId, vrfId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortTranslationEnableSet
(
    IN GT_U8                       devNum,
    IN GT_U8                       port,
    IN CPSS_DIRECTION_ENT          direction,
    IN GT_BOOL                     enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortTranslationEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh3 and above)
    1.1.1. Call with direction [CPSS_DIRECTION_INGRESS_E /
                                CPSS_DIRECTION_EGRESS_E]
                     and enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChBrgVlanPortTranslationEnableGet with same direction.
    Expected: GT_OK and the same enable.
    1.1.3. Call with out of range direction.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;

    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_PHY_PORT_CNS;

    CPSS_DIRECTION_ENT  direction = CPSS_DIRECTION_INGRESS_E;
    GT_BOOL             enable    = GT_FALSE;
    GT_BOOL             enableGet = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with direction [CPSS_DIRECTION_INGRESS_E /
                                            CPSS_DIRECTION_EGRESS_E]
                                 and enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */

            /* Call with direction [CPSS_DIRECTION_INGRESS_E] */
            direction = CPSS_DIRECTION_INGRESS_E;
            enable    = GT_FALSE;

            st = cpssDxChBrgVlanPortTranslationEnableSet(dev, port, direction, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, enable);

            /*
                1.1.2. Call cpssDxChBrgVlanPortTranslationEnableGet with same direction.
                Expected: GT_OK and the same enable.
            */
            st = cpssDxChBrgVlanPortTranslationEnableGet(dev, port, direction, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgVlanPortTranslationEnableGet: %d, %d, %d", dev, port, direction);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with direction [CPSS_DIRECTION_EGRESS_E] */
            direction = CPSS_DIRECTION_EGRESS_E;
            enable    = GT_TRUE;

            st = cpssDxChBrgVlanPortTranslationEnableSet(dev, port, direction, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, enable);

            /*
                1.1.2. Call cpssDxChBrgVlanPortTranslationEnableGet with same direction.
                Expected: GT_OK and the same enable.
            */
            st = cpssDxChBrgVlanPortTranslationEnableGet(dev, port, direction, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgVlanPortTranslationEnableGet: %d, %d, %d", dev, port, direction);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with wrong enum values direction.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanPortTranslationEnableSet
                                                        (dev, port, direction, enable),
                                                        direction);
        }

        direction = CPSS_DIRECTION_INGRESS_E;
        enable    = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanPortTranslationEnableSet(dev, port, direction, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanPortTranslationEnableSet(dev, port, direction, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanPortTranslationEnableSet(dev, port, direction, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    direction = CPSS_DIRECTION_INGRESS_E;
    enable    = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortTranslationEnableSet(dev, port, direction, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortTranslationEnableSet(dev, port, direction, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortTranslationEnableGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       port,
    IN  CPSS_DIRECTION_ENT          direction,
    OUT GT_BOOL                     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortTranslationEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh3 and above)
    1.1.1. Call with direction [CPSS_DIRECTION_INGRESS_E /
                                CPSS_DIRECTION_EGRESS_E]
                     and not-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with out of range direction.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;

    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_PHY_PORT_CNS;

    CPSS_DIRECTION_ENT  direction = CPSS_DIRECTION_INGRESS_E;
    GT_BOOL             enable    = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with direction [CPSS_DIRECTION_INGRESS_E /
                                            CPSS_DIRECTION_EGRESS_E]
                                 and not-NULL enablePtr.
                Expected: GT_OK.
            */

            /* Call with direction [CPSS_DIRECTION_INGRESS_E] */
            direction = CPSS_DIRECTION_INGRESS_E;

            st = cpssDxChBrgVlanPortTranslationEnableGet(dev, port, direction, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, direction);

            /* Call with direction [CPSS_DIRECTION_EGRESS_E] */
            direction = CPSS_DIRECTION_EGRESS_E;

            st = cpssDxChBrgVlanPortTranslationEnableGet(dev, port, direction, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, direction);

            /*
                1.1.2. Call with wrong enum values direction.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanPortTranslationEnableGet
                                                        (dev, port, direction, &enable),
                                                        direction);

            direction = CPSS_DIRECTION_INGRESS_E;

            /*
                1.1.3. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChBrgVlanPortTranslationEnableGet(dev, port, direction, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        direction = CPSS_DIRECTION_INGRESS_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChBrgVlanPortTranslationEnableGet(dev, port, direction, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChBrgVlanPortTranslationEnableGet(dev, port, direction, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChBrgVlanPortTranslationEnableGet(dev, port, direction, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    direction = CPSS_DIRECTION_INGRESS_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortTranslationEnableGet(dev, port, direction, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortTranslationEnableGet(dev, port, direction, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanTranslationEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U16                       vlanId,
    IN CPSS_DIRECTION_ENT           direction,
    IN GT_U16                       transVlanId
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanTranslationEntryWrite)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call with vlanId [0 / 4095],
                   direction [CPSS_DIRECTION_INGRESS_E /
                              CPSS_DIRECTION_EGRESS_E]
                   and transVlanId [100 / 4095].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgVlanTranslationEntryRead with same vlanId and direction.
    Expected: GT_OK and the same transVlanId.
    1.3. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range direction.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range transVlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;

    GT_U16              vlanId         = 0;
    CPSS_DIRECTION_ENT  direction      = CPSS_DIRECTION_INGRESS_E;
    GT_U16              transVlanId    = 0;
    GT_U16              transVlanIdGet = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [0 / 4095],
                           direction [CPSS_DIRECTION_INGRESS_E /
                                      CPSS_DIRECTION_EGRESS_E]
                           and transVlanId [100 / 4095].
            Expected: GT_OK.
        */

        /* Call with vlanId [100] */
        vlanId      = 100;
        direction   = CPSS_DIRECTION_INGRESS_E;
        transVlanId = 100;

        st = cpssDxChBrgVlanTranslationEntryWrite(dev, vlanId, direction, transVlanId);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, direction, transVlanId);

        /*
            1.2. Call cpssDxChBrgVlanTranslationEntryRead with same vlanId and direction.
            Expected: GT_OK and the same transVlanId.
        */
        st = cpssDxChBrgVlanTranslationEntryRead(dev, vlanId, direction, &transVlanIdGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgVlanTranslationEntryRead: %d, %d, %d", dev, vlanId, direction);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(transVlanId, transVlanIdGet,
                   "get another transVlanId than was set: %d", dev);

        /* Call with vlanId [4095] */
        vlanId      = 4095;
        direction   = CPSS_DIRECTION_EGRESS_E;
        transVlanId = 4095;

        st = cpssDxChBrgVlanTranslationEntryWrite(dev, vlanId, direction, transVlanId);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, direction, transVlanId);

        /*
            1.2. Call cpssDxChBrgVlanTranslationEntryRead with same vlanId and direction.
            Expected: GT_OK and the same transVlanId.
        */
        st = cpssDxChBrgVlanTranslationEntryRead(dev, vlanId, direction, &transVlanIdGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChBrgVlanTranslationEntryRead: %d, %d, %d", dev, vlanId, direction);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(transVlanId, transVlanIdGet,
                   "get another transVlanId than was set: %d", dev);

        /*
            1.3. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanTranslationEntryWrite(dev, vlanId, direction, transVlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        vlanId = 100;

        /*
            1.4. Call with wrong enum values direction.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanTranslationEntryWrite
                                                        (dev, vlanId, direction, transVlanId),
                                                        direction);

        direction = CPSS_DIRECTION_INGRESS_E;

        /*
            1.5. Call with out of range transVlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
            Expected: GT_BAD_PARAM.
        */
        transVlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanTranslationEntryWrite(dev, vlanId, direction, transVlanId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, transVlanId = %d", dev, transVlanId);
    }

    vlanId      = 100;
    direction   = CPSS_DIRECTION_INGRESS_E;
    transVlanId = 100;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanTranslationEntryWrite(dev, vlanId, direction, transVlanId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanTranslationEntryWrite(dev, vlanId, direction, transVlanId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanTranslationEntryRead
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN CPSS_DIRECTION_ENT           direction,
    OUT GT_U16                      *transVlanIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanTranslationEntryRead)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call with vlanId [0 / 4095],
                   direction [CPSS_DIRECTION_INGRESS_E /
                              CPSS_DIRECTION_EGRESS_E]
                   and non-NULL transVlanIdPtr.
    Expected: GT_OK.
    1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: GT_BAD_PARAM.
    1.3. Call with out of range direction.
    Expected: GT_BAD_PARAM.
    1.4. Call with transVlanIdPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;

    GT_U16              vlanId      = 0;
    CPSS_DIRECTION_ENT  direction   = CPSS_DIRECTION_INGRESS_E;
    GT_U16              transVlanId = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vlanId [0 / 4095],
                           direction [CPSS_DIRECTION_INGRESS_E /
                                      CPSS_DIRECTION_EGRESS_E]
                           and non-NULL transVlanIdPtr.
            Expected: GT_OK.
        */
        /* Call with vlanId [100] */
        vlanId      = 100;
        direction   = CPSS_DIRECTION_INGRESS_E;

        st = cpssDxChBrgVlanTranslationEntryRead(dev, vlanId, direction, &transVlanId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, direction);

        /* Call with vlanId [4095] */
        vlanId      = 4095;
        direction   = CPSS_DIRECTION_EGRESS_E;

        st = cpssDxChBrgVlanTranslationEntryRead(dev, vlanId, direction, &transVlanId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, direction);

        /*
            1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxChBrgVlanTranslationEntryRead(dev, vlanId, direction, &transVlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        vlanId = 100;

        /*
            1.3. Call with wrong enum values direction.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanTranslationEntryRead
                                                        (dev, vlanId, direction, &transVlanId),
                                                        direction);

        direction = CPSS_DIRECTION_INGRESS_E;

        /*
            1.4. Call with transVlanIdPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanTranslationEntryRead(dev, vlanId, direction, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, transVlanIdPtr = NULL", dev);
    }

    vlanId      = 100;
    direction   = CPSS_DIRECTION_INGRESS_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanTranslationEntryRead(dev, vlanId, direction, &transVlanId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanTranslationEntryRead(dev, vlanId, direction, &transVlanId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanValidCheckEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanValidCheckEnableSet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgVlanValidCheckEnableGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChBrgVlanValidCheckEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgVlanValidCheckEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgVlanValidCheckEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgVlanValidCheckEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enable than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChBrgVlanValidCheckEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgVlanValidCheckEnableGet
            Expected: GT_OK and the same enablePtr.
        */
        st = cpssDxChBrgVlanValidCheckEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChBrgVlanValidCheckEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enable than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanValidCheckEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanValidCheckEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanValidCheckEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanValidCheckEnableGet)
{
/*
    ITERATE_DEVICES (DxCh2 and above)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with null enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanValidCheckEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanValidCheckEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanValidCheckEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanValidCheckEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanFloodVidxModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U16                                  vlanId,
    IN  GT_U16                                  floodVidx,
    IN  CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT  floodVidxMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanFloodVidxModeSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with correct parameters  and floodVidxMode =
                  [ CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E /
                    CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E ].
    Expected: GT_OK.
    1.2. Call function with incorrect floodVidx > BIT_12.
    Expected: GT_OUT_OF_RANGE.
    1.3. Call function with incorrect vlanId.
    Expected: GT_BAD_PARAM.
    1.4. Call function with wrong enum values floodVidxMode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT floodVidxMode =
                                    CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with correct parameters  and floodVidxMode =
                  [ CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E /
                    CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E ].
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanFloodVidxModeSet(dev, 0, 0,
                                     CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        st = cpssDxChBrgVlanFloodVidxModeSet(dev, 0, 0,
                            CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with incorrect floodVidx > BIT_12.
            Expected: GT_OUT_OF_RANGE.
        */
        st = cpssDxChBrgVlanFloodVidxModeSet(dev, 0, BIT_12 + 1,
                            CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);

        /*
            1.3. Call function with incorrect vlanId = BRG_VLAN_INVALID_VLAN_ID_CNS.
            Expected: GT_BAD_PARAM.
        */
        st = cpssDxChBrgVlanFloodVidxModeSet(dev, BRG_VLAN_INVALID_VLAN_ID_CNS, 0,
                            CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /*
            1.4. Call function with wrong enum values floodVidxMode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanFloodVidxModeSet
                                                        (dev, 0, 0, floodVidxMode),
                                                        floodVidxMode);

        floodVidxMode = 0;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanFloodVidxModeSet(dev, 0, 0,
                                  CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanFloodVidxModeSet(dev, 0, 0,
                              CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanLocalSwitchingEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U16    vlanId,
    IN  CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT  trafficType,
    IN  GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanLocalSwitchingEnableSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with correct parameters  and trafficType =
               [ CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E /
                 CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E ].
    Expected: GT_OK.
    1.2. Call function with wrong enum values vlanId = BRG_VLAN_INVALID_VLAN_ID_CNS.
    Expected: GT_BAD_PARAM.
    1.3. Call function with wrong enum values trafficType.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_ENT trafficType =
                        CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with correct parameters and trafficType =
               [ CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E /
                 CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E ]
                                 and  enable status [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanLocalSwitchingEnableSet(dev, BRG_VLAN_TESTED_VLAN_ID_CNS,
                CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_KNOWN_UC_E, GT_TRUE);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        st = cpssDxChBrgVlanLocalSwitchingEnableSet(dev, BRG_VLAN_TESTED_VLAN_ID_CNS,
                CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E, GT_FALSE);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with incorrect vlanId = BRG_VLAN_INVALID_VLAN_ID_CNS.
            Expected: GT_BAD_PARAM.
        */
        st = cpssDxChBrgVlanLocalSwitchingEnableSet(dev, BRG_VLAN_INVALID_VLAN_ID_CNS,
               CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E, GT_FALSE);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /*
            1.3. Call function with wrong enum values trafficType.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanLocalSwitchingEnableSet
                                                        (dev, BRG_VLAN_TESTED_VLAN_ID_CNS, trafficType, GT_FALSE),
                                                        trafficType);
    }

    trafficType = CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanLocalSwitchingEnableSet(dev, BRG_VLAN_TESTED_VLAN_ID_CNS,
                CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E, GT_FALSE);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanLocalSwitchingEnableSet(dev, BRG_VLAN_TESTED_VLAN_ID_CNS,
            CPSS_DXCH_BRG_VLAN_LOCAL_SWITCHING_TRAFFIC_TYPE_FLOODED_E, GT_FALSE);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanMemberSet
(
    IN  GT_U8                               devNum,
    IN  GT_U16                              vlanId,
    IN  GT_U8                               portNum,
    IN  GT_BOOL                             isMember,
    IN  GT_BOOL                             isTagged,
    IN  CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT taggingCmd
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanMemberSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with correct parameters:  isMember = [GT_FALSE / GT_TRUE]
               isTagged = [GT_FALSE / GT_TRUE] and taggingCmd =
               [CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E /
                CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E].
    Expected: GT_OK.
    1.2. Call function with incorrect vlanId.
    Expected: GT_BAD_PARAM.
    1.3. Call function with wrong enum values taggingCmd.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U32      index = 0;

    GT_U8                   dev;
    CPSS_PP_FAMILY_TYPE_ENT devFamily;
    CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT taggingCmd = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /*
            1.1. Call function with correct parameters: isMember = [GT_FALSE / GT_TRUE]
                       isTagged = [GT_FALSE / GT_TRUE] and taggingCmd =
                       [CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E /
                        CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E].
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanMemberSet(dev, BRG_VLAN_TESTED_VLAN_ID_CNS,
                                      BRG_VLAN_VALID_PHY_PORT_CNS, GT_FALSE, GT_TRUE,
                                      CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        st = cpssDxChBrgVlanMemberSet(dev, BRG_VLAN_TESTED_VLAN_ID_CNS,
                                      BRG_VLAN_VALID_PHY_PORT_CNS, GT_TRUE, GT_FALSE,
                                      CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with incorrect vlanId.
            Expected: GT_BAD_PARAM.
        */
        st = cpssDxChBrgVlanMemberSet(dev, BRG_VLAN_INVALID_VLAN_ID_CNS,
                                      BRG_VLAN_VALID_PHY_PORT_CNS, GT_TRUE, GT_TRUE, 0);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /*
            1.3. Call function with wrong enum values taggingCmd.
            Expected: GT_BAD_PARAM.
        */
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanMemberSet
                                (dev, BRG_VLAN_TESTED_VLAN_ID_CNS,
                                 BRG_VLAN_VALID_PHY_PORT_CNS, GT_TRUE, GT_TRUE, taggingCmd),
                                taggingCmd);
        }
        else
        {
            for(index = 0; index < utfInvalidEnumArrSize; index++)
            {
                taggingCmd = utfInvalidEnumArr[index];

                st = cpssDxChBrgVlanMemberSet(dev, BRG_VLAN_TESTED_VLAN_ID_CNS,
                                    BRG_VLAN_VALID_PHY_PORT_CNS, GT_TRUE, GT_TRUE, taggingCmd);

                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
            }
        }
    }

    taggingCmd = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanMemberSet(dev, BRG_VLAN_TESTED_VLAN_ID_CNS,
                                   BRG_VLAN_VALID_PHY_PORT_CNS, GT_FALSE, GT_TRUE, 0);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanMemberSet(dev, BRG_VLAN_TESTED_VLAN_ID_CNS,
                               BRG_VLAN_VALID_PHY_PORT_CNS, GT_FALSE, GT_TRUE, 0);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortEgressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    OUT GT_U32               *tpidEntryIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortEgressTpidGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with correct parameters and ethMode =
              [ CPSS_VLAN_ETHERTYPE0_E / CPSS_VLAN_ETHERTYPE0_E ].
    Expected: GT_OK.
    1.2. Call function with incorrect portNum = BRG_VLAN_INVALID_PHY_PORT_CNS.
    Expected: GT_BAD_PARAM.
    1.3. Call function with wrong enum values ethMode.
    Expected: GT_BAD_PARAM.
    1.4. Call function with incorrect tpidEntryIndexPtr (NULL).
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    CPSS_ETHER_MODE_ENT  ethMode = CPSS_VLAN_ETHERTYPE0_E;
    GT_U32      tpidEntryIndexPtr;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with correct parameters and ethMode =
                      [ CPSS_VLAN_ETHERTYPE0_E / CPSS_VLAN_ETHERTYPE0_E ].
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanPortEgressTpidGet(dev, BRG_VLAN_VALID_PHY_PORT_CNS,
                                         CPSS_VLAN_ETHERTYPE0_E, &tpidEntryIndexPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        st = cpssDxChBrgVlanPortEgressTpidGet(dev, BRG_VLAN_VALID_PHY_PORT_CNS,
                                         CPSS_VLAN_ETHERTYPE1_E, &tpidEntryIndexPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with incorrect portNum = BRG_VLAN_INVALID_PHY_PORT_CNS.
            Expected: GT_BAD_PARAM.
        */
        st = cpssDxChBrgVlanPortEgressTpidGet(dev, BRG_VLAN_INVALID_PHY_PORT_CNS,
                                         CPSS_VLAN_ETHERTYPE0_E, &tpidEntryIndexPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /*
            1.3. Call function with wrong enum values ethMode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanPortEgressTpidGet
                                                        (dev, BRG_VLAN_VALID_PHY_PORT_CNS, ethMode, &tpidEntryIndexPtr),
                                                        ethMode);

        /*
            1.4. Call function with incorrect tpidEntryIndexPtr (NULL).
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanPortEgressTpidGet(dev, BRG_VLAN_VALID_PHY_PORT_CNS,
                                         CPSS_VLAN_ETHERTYPE0_E, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortEgressTpidGet(dev, BRG_VLAN_VALID_PHY_PORT_CNS,
                                         CPSS_VLAN_ETHERTYPE0_E, &tpidEntryIndexPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortEgressTpidGet(dev, BRG_VLAN_VALID_PHY_PORT_CNS,
                                     CPSS_VLAN_ETHERTYPE0_E, &tpidEntryIndexPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortEgressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidEntryIndex
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortEgressTpidSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with correct parameters and ethMode =
                  [ CPSS_VLAN_ETHERTYPE0_E / CPSS_VLAN_ETHERTYPE0_E ].
    Expected: GT_OK and the same tpidEntryIndex.
    1.2. Call function with incorrect portNum = BRG_VLAN_INVALID_PHY_PORT_CNS.
    Expected: GT_BAD_PARAM.
    1.3. Call function with wrong enum values ethMode.
    Expected: GT_BAD_PARAM.
    1.4. Call function with incorrect tpidEntryIndex = 9.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    CPSS_ETHER_MODE_ENT  ethMode = CPSS_VLAN_ETHERTYPE0_E;
    GT_U32      tpidEntryIndex    = 1;
    GT_U32      tpidEntryIndexGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with correct parameters and ethMode =
                          [ CPSS_VLAN_ETHERTYPE0_E / CPSS_VLAN_ETHERTYPE0_E ].
            Expected: GT_OK and the same tpidEntryIndex.
        */
        st = cpssDxChBrgVlanPortEgressTpidSet(dev, BRG_VLAN_VALID_PHY_PORT_CNS,
                                         CPSS_VLAN_ETHERTYPE0_E, tpidEntryIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        st = cpssDxChBrgVlanPortEgressTpidSet(dev, BRG_VLAN_VALID_PHY_PORT_CNS,
                                         CPSS_VLAN_ETHERTYPE1_E, tpidEntryIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Get value of tpidEntryIndex*/
        st = cpssDxChBrgVlanPortEgressTpidGet(dev, BRG_VLAN_VALID_PHY_PORT_CNS,
                                         CPSS_VLAN_ETHERTYPE0_E, &tpidEntryIndexGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Check if we get the same tpidEntryIndex*/
        UTF_VERIFY_EQUAL2_STRING_MAC(tpidEntryIndex, tpidEntryIndexGet,
         "get another tpidEntryIndex than was set: tpidBmp = %d,  tpidEntryIndexGet = %d",
                                     tpidEntryIndex, tpidEntryIndexGet);
        /*
            1.2. Call function with incorrect portNum = BRG_VLAN_INVALID_PHY_PORT_CNS.
            Expected: GT_BAD_PARAM.
        */
        st = cpssDxChBrgVlanPortEgressTpidSet(dev, BRG_VLAN_INVALID_PHY_PORT_CNS,
                                         CPSS_VLAN_ETHERTYPE0_E, tpidEntryIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /*
            1.3. Call function with wrong enum values ethMode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanPortEgressTpidSet
                                                        (dev, BRG_VLAN_VALID_PHY_PORT_CNS, ethMode, tpidEntryIndex),
                                                        ethMode);

        /*
            1.4. Call function with incorrect tpidEntryIndexPtr (9).
            Expected: GT_OUT_OF_RANGE.
        */
        st = cpssDxChBrgVlanPortEgressTpidSet(dev, BRG_VLAN_VALID_PHY_PORT_CNS,
                                              CPSS_VLAN_ETHERTYPE0_E, 9);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortEgressTpidSet(dev, BRG_VLAN_VALID_PHY_PORT_CNS,
                                         CPSS_VLAN_ETHERTYPE0_E, tpidEntryIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortEgressTpidSet(dev, BRG_VLAN_VALID_PHY_PORT_CNS,
                                     CPSS_VLAN_ETHERTYPE0_E, tpidEntryIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortIngressTpidGet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    OUT GT_U32               *tpidBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortIngressTpidGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with correct parameters and ethMode = CPSS_VLAN_ETHERTYPE0_E.
    Expected: GT_OK.
    1.2. Call function with incorrect portNum = BRG_VLAN_INVALID_PHY_PORT_CNS.
    Expected: GT_BAD_PARAM.
    1.3. Call function with wrong enum values ethMode.
    Expected: GT_BAD_PARAM.
    1.4. Call function with incorrect tpidBmpGet (NULL).
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_PHY_PORT_CNS;
    CPSS_ETHER_MODE_ENT  ethMode = CPSS_VLAN_ETHERTYPE0_E;
    GT_U32      tpidBmpGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1. Call function with correct parameters and
                     ethMode = CPSS_VLAN_ETHERTYPE0_E.
                Expected: GT_OK.
            */
            st = cpssDxChBrgVlanPortIngressTpidGet(dev, port,
                                 CPSS_VLAN_ETHERTYPE0_E, &tpidBmpGet);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.2. Call function with incorrect
                     portNum = BRG_VLAN_INVALID_PHY_PORT_CNS.
                Expected: GT_BAD_PARAM.
            */
            st = cpssDxChBrgVlanPortIngressTpidGet(dev,
                 BRG_VLAN_INVALID_PHY_PORT_CNS,CPSS_VLAN_ETHERTYPE0_E, &tpidBmpGet);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

            /*
                1.3. Call function with wrong enum values ethMode.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanPortIngressTpidGet
                                                        (dev, port, ethMode, &tpidBmpGet),
                                                        ethMode);

            /*
                1.4. Call function with incorrect tpidBmpGet (NULL).
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChBrgVlanPortIngressTpidGet(dev, port,
                                             CPSS_VLAN_ETHERTYPE0_E, NULL);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
        }
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortIngressTpidGet(dev, port,
                                    CPSS_VLAN_ETHERTYPE0_E, &tpidBmpGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortIngressTpidGet(dev, port,
                                     CPSS_VLAN_ETHERTYPE0_E, &tpidBmpGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortIngressTpidSet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_ETHER_MODE_ENT  ethMode,
    IN  GT_U32               tpidBmp
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortIngressTpidSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with correct parameters and ethMode = CPSS_VLAN_ETHERTYPE0_E.
    Expected: GT_OK and the same tpidBmp.
    1.2. Call function with incorrect portNum = BRG_VLAN_INVALID_PHY_PORT_CNS.
    Expected: GT_BAD_PARAM.
    1.3. Call function with wrong enum values ethMode.
    Expected: GT_BAD_PARAM.
    1.4. Call function with incorrect tpidBmp = BIT_8.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_PHY_PORT_CNS;
    CPSS_ETHER_MODE_ENT  ethMode = CPSS_VLAN_ETHERTYPE0_E;
    GT_U32      tpidBmp    = 1;
    GT_U32      tpidBmpGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1. Call function with correct parameters and
                     ethMode = CPSS_VLAN_ETHERTYPE0_E.
                Expected: GT_OK and the same tpidBmp.
            */
            st = cpssDxChBrgVlanPortIngressTpidSet(dev, port,
                                 CPSS_VLAN_ETHERTYPE0_E, tpidBmp);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* Get value of tpidBmp*/
            st = cpssDxChBrgVlanPortIngressTpidGet(dev, port,
                              CPSS_VLAN_ETHERTYPE0_E, &tpidBmpGet);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* Check if we get the same tpidBmp*/
            UTF_VERIFY_EQUAL2_STRING_MAC(tpidBmp, tpidBmpGet,
              "get another tpidBmp than was set: tpidBmp = %d, tpidBmpGet = %d",
                                         tpidBmp, tpidBmpGet);

            /*
                1.2. Call function with incorrect
                     portNum = BRG_VLAN_INVALID_PHY_PORT_CNS.
                Expected: GT_BAD_PARAM.
            */
            st = cpssDxChBrgVlanPortIngressTpidSet(dev,
                 BRG_VLAN_INVALID_PHY_PORT_CNS, CPSS_VLAN_ETHERTYPE0_E, tpidBmp);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

            /*
                1.3. Call function with wrong enum values ethMode.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanPortIngressTpidSet
                                (dev, port, ethMode, tpidBmp),
                                ethMode);

            /*
                1.4. Call function with incorrect tpidBmpPtr (BIT_8).
                Expected: GT_OUT_OF_RANGE.
            */
            st = cpssDxChBrgVlanPortIngressTpidSet(dev, port,
                                     CPSS_VLAN_ETHERTYPE0_E, BIT_8);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);
        }
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortIngressTpidSet(dev, port,
                                     CPSS_VLAN_ETHERTYPE0_E, tpidBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortIngressTpidSet(dev, port,
                             CPSS_VLAN_ETHERTYPE0_E, tpidBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortIsolationCmdSet
(
    IN  GT_U8                                      devNum,
    IN  GT_U16                                     vlanId,
    IN  CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT  cmd
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortIsolationCmdSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with correct parameters and cmd =
                  [ CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E /
                    CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E ]
    Expected: GT_OK.
    1.2. Call function with incorrect vlanId = BRG_VLAN_INVALID_VLAN_ID_CNS.
    Expected: GT_BAD_PARAM.
    1.3. Call function with wrong enum values cmd.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT cmd = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L2_CMD_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with correct parameters and cmd =
                          [ CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E /
                            CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E ]
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanPortIsolationCmdSet(dev, BRG_VLAN_TESTED_VLAN_ID_CNS,
                                 CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        st = cpssDxChBrgVlanPortIsolationCmdSet(dev, BRG_VLAN_TESTED_VLAN_ID_CNS,
                                      CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with incorrect vlanId = BRG_VLAN_INVALID_VLAN_ID_CNS.
            Expected: GT_BAD_PARAM.
        */
        st = cpssDxChBrgVlanPortIsolationCmdSet(dev, BRG_VLAN_INVALID_VLAN_ID_CNS,
                                       CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /*
            1.3. Call function with wrong enum values cmd.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanPortIsolationCmdSet
                                                        (dev, BRG_VLAN_TESTED_VLAN_ID_CNS, cmd),
                                                        cmd);
    }

    cmd = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_L3_CMD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortIsolationCmdSet(dev, BRG_VLAN_TESTED_VLAN_ID_CNS,
                                 CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortIsolationCmdSet(dev, BRG_VLAN_TESTED_VLAN_ID_CNS,
                             CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanPortVidPrecedenceGet
(
    IN  GT_U8                                         devNum,
    IN  GT_U8                                         port,
    OUT CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  *precedencePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanPortVidPrecedenceGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with correct parameters.
    Expected: GT_OK.
    1.2. Call function with incorrect portNum = BRG_VLAN_INVALID_PHY_PORT_CNS.
    Expected: GT_BAD_PARAM.
    1.3. Call function with incorrect precedencePtr (NULL).
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT      precedencePtr;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with correct parameters.
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanPortVidPrecedenceGet(dev, BRG_VLAN_VALID_PHY_PORT_CNS,
                                                 &precedencePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with incorrect portNum = BRG_VLAN_INVALID_PHY_PORT_CNS.
            Expected: GT_BAD_PARAM.
        */
        st = cpssDxChBrgVlanPortVidPrecedenceGet(dev, BRG_VLAN_INVALID_PHY_PORT_CNS,
                                                 &precedencePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /*
            1.3. Call function with incorrect precedencePtr (NULL).
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanPortVidPrecedenceGet(dev, BRG_VLAN_VALID_PHY_PORT_CNS, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanPortVidPrecedenceGet(dev, BRG_VLAN_VALID_PHY_PORT_CNS,
                                                 &precedencePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanPortVidPrecedenceGet(dev, BRG_VLAN_VALID_PHY_PORT_CNS,
                                             &precedencePtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanRangeGet
(
    IN  GT_U8   devNum,
    OUT GT_U16  *vidRangePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanRangeGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with correct parameters.
    Expected: GT_OK.
    1.2. Call function with incorrect vidRangePtr (NULL).
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vidRangePtr;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with correct parameters.
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanRangeGet(dev, &vidRangePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with incorrect vidRangePtr (NULL).
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanRangeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanRangeGet(dev, &vidRangePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanRangeGet(dev, &vidRangePtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanRangeSet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vidRange
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanRangeSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with correct parameters.
    Expected: GT_OK and the same vidRange.
    1.2. Call function with incorrect vidRange = PRV_CPSS_MAX_NUM_VLANS_CNS.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vidRange    = 1;
    GT_U16      vidRangeGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with correct parameters and
                 ethMode = CPSS_VLAN_ETHERTYPE0_E.
            Expected: GT_OK and the same vidRange.
        */
        st = cpssDxChBrgVlanRangeSet(dev, vidRange);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Get value of vidRange*/
        st = cpssDxChBrgVlanRangeGet(dev, &vidRangeGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Check if we get the same vidRange*/
        UTF_VERIFY_EQUAL2_STRING_MAC(vidRange, vidRangeGet,
          "get another vidRange than was set: vidRange = %d, vidRangeGet = %d",
                                     vidRange, vidRangeGet);
        /*
            1.2. Call function with incorrect
                 vidRangePtr (PRV_CPSS_MAX_NUM_VLANS_CNS).
            Expected: GT_OUT_OF_RANGE.
        */
        st = cpssDxChBrgVlanRangeSet(dev, PRV_CPSS_MAX_NUM_VLANS_CNS);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanRangeSet(dev, vidRange);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanRangeSet(dev, vidRange);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanTpidEntryGet
(
    IN  GT_U8               devNum,
    IN  CPSS_DIRECTION_ENT  direction,
    IN  GT_U32              entryIndex,
    OUT GT_U16              *etherTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanTpidEntryGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with correct parameters: direction =
                   [ CPSS_DIRECTION_INGRESS_E / CPSS_DIRECTION_EGRESS_E ].
    Expected: GT_OK.
    1.2. Call function with wrong enum values direction.
    Expected: GT_BAD_PARAM.
    1.3. Call function with incorrect entryIndex = 8.
    Expected: GT_BAD_PARAM.
    1.4. Call function with incorrect etherTypePtr (NULL).
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    CPSS_DIRECTION_ENT  direction = CPSS_DIRECTION_INGRESS_E;
    GT_U16      etherTypePtr;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with correct parameters: direction =
                            [ CPSS_DIRECTION_INGRESS_E / CPSS_DIRECTION_EGRESS_E ].
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanTpidEntryGet(dev, CPSS_DIRECTION_INGRESS_E,
                                         0, &etherTypePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        st = cpssDxChBrgVlanTpidEntryGet(dev, CPSS_DIRECTION_EGRESS_E,
                                         0, &etherTypePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with wrong enum values direction.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanTpidEntryGet
                                                        (dev, direction, 0, &etherTypePtr),
                                                        direction);

        /*
            1.3. Call function with incorrect entryIndex = 8.
            Expected: GT_BAD_PARAM.
        */
        st = cpssDxChBrgVlanTpidEntryGet(dev, CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                                         8, &etherTypePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /*
            1.4. Call function with incorrect etherTypePtr (NULL).
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanTpidEntryGet(dev, CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                                         0, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanTpidEntryGet(dev, CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                                         0, &etherTypePtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanTpidEntryGet(dev, CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E,
                                     0, &etherTypePtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanTpidEntrySet
(
    IN  GT_U8               devNum,
    IN  CPSS_DIRECTION_ENT  direction,
    IN  GT_U32              entryIndex,
    IN  GT_U16              etherType
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanTpidEntrySet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with correct parameters: direction =
                   [ CPSS_DIRECTION_INGRESS_E / CPSS_DIRECTION_EGRESS_E ].
    Expected: GT_OK and the same etherType.
    1.2. Call function with wrong enum values direction.
    Expected: GT_BAD_PARAM.
    1.3. Call function with incorrect entryIndex = 8.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st           = GT_OK;

    GT_U8       dev;
    CPSS_DIRECTION_ENT  direction = CPSS_DIRECTION_INGRESS_E;
    GT_U16      etherType    = 1;
    GT_U16      etherTypeGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with correct parameters: direction =
                   [ CPSS_DIRECTION_INGRESS_E / CPSS_DIRECTION_EGRESS_E ].
            Expected: GT_OK and the same etherType.
        */
        st = cpssDxChBrgVlanTpidEntrySet(dev, CPSS_DIRECTION_INGRESS_E, 0, etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Get value of etherType*/
        st = cpssDxChBrgVlanTpidEntryGet(dev, CPSS_DIRECTION_INGRESS_E, 0, &etherTypeGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Check if we get the same etherType*/
        UTF_VERIFY_EQUAL2_STRING_MAC(etherType, etherTypeGet,
               "get another etherType than was set: etherType = %d, etherTypeGet = %d",
                                     etherType, etherTypeGet);

        st = cpssDxChBrgVlanTpidEntrySet(dev, CPSS_DIRECTION_EGRESS_E, 0, etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Get value of etherType*/
        st = cpssDxChBrgVlanTpidEntryGet(dev, CPSS_DIRECTION_EGRESS_E, 0, &etherTypeGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Check if we get the same etherType*/
        UTF_VERIFY_EQUAL2_STRING_MAC(etherType, etherTypeGet,
               "get another etherType than was set: etherType = %d, etherTypeGet = %d",
                                     etherType, etherTypeGet);

        /*
            1.2. Call function with wrong enum values direction.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanTpidEntrySet
                                                        (dev, direction, 0, etherType),
                                                        direction);

        /*
            1.3. Call function with incorrect entryIndex = 8.
            Expected: GT_BAD_PARAM.
        */
        st = cpssDxChBrgVlanTpidEntrySet(dev, CPSS_DIRECTION_INGRESS_E, 8, etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanTpidEntrySet(dev, CPSS_DIRECTION_INGRESS_E, 0, etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanTpidEntrySet(dev, CPSS_DIRECTION_INGRESS_E, 0, etherType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanInit
(
    IN GT_U8 devNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanInit)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with correct dev.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with correct dev.
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanInit(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanInit(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT     mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with mode [CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E /
                         CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E /
                         CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E].
    Expected: GT_OK.
    1.2. Call with wrong enum values mode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_STATUS   expectedSt;
    GT_U8       dev;
    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT mode =
                            CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E /
                                 CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E /
                                 CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E].
            Expected: GT_OK.
        */

        if( (CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(dev)->devFamily) &&
            ( PRV_CPSS_PP_MAC(dev)->revision <= 2 ))
        {
            expectedSt = GT_NOT_APPLICABLE_DEVICE;
        }
        else
        {
            expectedSt = GT_OK;
        }

        /* Call with mode [CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E] */
        mode = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_DISABLE_E;

        st = cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(expectedSt, st, dev, mode);

        /* Call with mode [CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E] */
        mode = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_NOT_DOUBLE_TAG_E;

        st = cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(expectedSt, st, dev, mode);

        /* Call with mode [CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E] */
        mode = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E;

        st = cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(expectedSt, st, dev, mode);

        /*
            1.2. Call with wrong enum values mode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet
                                                        (dev, mode),
                                                        mode);
    }

    mode = CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet
(
    IN  GT_U8                                               devNum,
    OUT CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT     *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with not null mode.
    Expected: GT_OK.
    1.2. Call with wrong modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    CPSS_DXCH_BRG_VLAN_REMOVE_TAG1_IF_ZERO_MODE_ENT mode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null mode.
            Expected: GT_OK.
        */

        st = cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet(dev, &mode);
        if( (CPSS_PP_FAMILY_DXCH_LION_E == PRV_CPSS_PP_MAC(dev)->devFamily) &&
            ( PRV_CPSS_PP_MAC(dev)->revision <= 2 ))
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev, &mode);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, &mode);
        }

        /*
            1.2. Call with wrong modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet(dev, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, dev, &mode);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanForceNewDsaToCpuEnableSet
(
    IN  GT_U8   dev,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanForceNewDsaToCpuEnableSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1 Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgVlanForceNewDsaToCpuEnableGet.
    Expected: GT_OK and the same enable than was set.
*/
    GT_STATUS   st       = GT_OK;
    GT_U8       dev;
    GT_BOOL     state    = GT_TRUE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1 Call function with enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */
        /* call with state = GT_TRUE; */
        state = GT_TRUE;

        st = cpssDxChBrgVlanForceNewDsaToCpuEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgVlanForceNewDsaToCpuEnableGet.
            Expected: GT_OK and the same enable than was set.
        */
        st = cpssDxChBrgVlanForceNewDsaToCpuEnableGet(dev, &stateGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgVlanForceNewDsaToCpuEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                               "get another state than was set: %d", dev);

        /* call with state = GT_FALSE; */
        state = GT_FALSE;

        st = cpssDxChBrgVlanForceNewDsaToCpuEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChBrgVlanForceNewDsaToCpuEnableGet.
            Expected: GT_OK and the same enable than was set.
        */
        st = cpssDxChBrgVlanForceNewDsaToCpuEnableGet(dev, &stateGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgVlanForceNewDsaToCpuEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                               "get another state than was set: %d", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanForceNewDsaToCpuEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanForceNewDsaToCpuEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanForceNewDsaToCpuEnableGet
(
    IN  GT_U8   dev,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanForceNewDsaToCpuEnableGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with not null enablePtr.
    Expected: GT_OK.
    1.2. Call with incorrect enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;

    GT_U8       dev;
    GT_BOOL     state;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChBrgVlanForceNewDsaToCpuEnableGet(dev, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call with incorrect enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChBrgVlanForceNewDsaToCpuEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, dev, state);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanForceNewDsaToCpuEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanForceNewDsaToCpuEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}



/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanKeepVlan1EnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       up,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanKeepVlan1EnableSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1 Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssDxChBrgVlanKeepVlan1EnableSet.
    Expected: GT_OK and the same enable than was set.
    1.3 Call function with wrong portNum [63, 65].
    Expected: GT_BAD_PARAM.
    1.4 Call function with wrong up [8].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st       = GT_OK;
    GT_U8       dev;
    GT_U8       portNum;
    GT_U8       up;
    GT_BOOL     state    = GT_TRUE;
    GT_BOOL     stateGet = GT_FALSE;
    CPSS_DXCH_CFG_DEV_INFO_STC   devInfo;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.0 Get device info */
        st = cpssDxChCfgDevInfoGet(dev, &devInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.1 Call function with portNum = 0, up = 4, enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */
        /* call with state = GT_TRUE; */
        state = GT_TRUE;
        portNum = 0;
        up = 4;

        st = cpssDxChBrgVlanKeepVlan1EnableSet(dev, portNum, up, state);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, portNum, up, state);

        /*
            1.2. Call cpssDxChBrgVlanKeepVlan1EnableGet.
            Expected: GT_OK and the same enable than was set.
        */
        st = cpssDxChBrgVlanKeepVlan1EnableGet(dev, portNum, up, &stateGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgVlanKeepVlan1EnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                               "get another state than was set: %d", dev);

        /* call with state = GT_FALSE; */
        state = GT_FALSE;

        st = cpssDxChBrgVlanKeepVlan1EnableSet(dev, portNum, up, state);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, portNum, up, state);

        /*
            1.2. Call cpssDxChBrgVlanForceNewDsaToCpuEnableGet.
            Expected: GT_OK and the same enable than was set.
        */
        st = cpssDxChBrgVlanKeepVlan1EnableGet(dev, portNum, up, &stateGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgVlanKeepVlan1EnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                               "get another state than was set: %d", dev);

        /*
            1.1 Call function with portNum = 32, up = 7, enable = GT_TRUE.
            Expected: GT_OK.
        */
        state = GT_TRUE;
        if (devInfo.genDevInfo.maxPortNum > 32)
        {
            portNum = 32;
        }
        else
        {
            portNum = (GT_U8)(devInfo.genDevInfo.maxPortNum / 2);
        }

        up = 7;

        st = cpssDxChBrgVlanKeepVlan1EnableSet(dev, portNum, up, state);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, portNum, up, state);

        /*
            1.2. Call cpssDxChBrgVlanKeepVlan1EnableGet.
            Expected: GT_OK and the same enable than was set.
        */
        st = cpssDxChBrgVlanKeepVlan1EnableGet(dev, portNum, up, &stateGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgVlanKeepVlan1EnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                               "get another state than was set: %d", dev);
        /*
            1.1 Call function with portNum = 54, up = 0, enable = GT_FALSE.
            Expected: GT_OK.
        */
        state = GT_TRUE;
        if (devInfo.genDevInfo.maxPortNum > 54)
        {
            portNum = 54;
        }
        else
        {
            portNum = (GT_U8)((devInfo.genDevInfo.maxPortNum / 2) + (devInfo.genDevInfo.maxPortNum / 4));
        }

        up = 0;

        st = cpssDxChBrgVlanKeepVlan1EnableSet(dev, portNum, up, state);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, portNum, up, state);

        /*
            1.2. Call cpssDxChBrgVlanKeepVlan1EnableGet.
            Expected: GT_OK and the same enable than was set.
        */
        st = cpssDxChBrgVlanKeepVlan1EnableGet(dev, portNum, up, &stateGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgVlanKeepVlan1EnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                               "get another state than was set: %d", dev);


        /* 1.3 Call function with wrong portNum [63, 65].
           Expected: GT_BAD_PARAM.
        */
        state = GT_TRUE;
        portNum = 63;
        up = 0;

        st = cpssDxChBrgVlanKeepVlan1EnableSet(dev, portNum, up, state);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PARAM, st);

        portNum = 65;

        st = cpssDxChBrgVlanKeepVlan1EnableSet(dev, portNum, up, state);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PARAM, st);

        /* 1.4 Call function with wrong up [8].
           Expected: GT_BAD_PARAM.
        */
        portNum = 0;
        up = 8;

        st = cpssDxChBrgVlanKeepVlan1EnableSet(dev, portNum, up, state);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PARAM, st);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    portNum = 0;
    up = 0;

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanKeepVlan1EnableSet(dev, portNum, up, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanKeepVlan1EnableSet(dev, portNum, up, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChBrgVlanKeepVlan1EnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       up,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanKeepVlan1EnableGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1 Call function with valid parameters.
    Expected: GT_OK.
    1.2 Call function with wrong portNum [64].
    Expected: GT_BAD_PARAM.
    1.3 Call function with wrong up [8].
    Expected: GT_BAD_PARAM.
    1.4 Call function with Null pointer.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st       = GT_OK;
    GT_U8       dev;
    GT_U8       portNum = 0;
    GT_U8       up = 0;
    GT_BOOL     state;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with valid parameters.
            Expected: GT_OK.
        */
        portNum = 10;
        up = 2;

        st = cpssDxChBrgVlanKeepVlan1EnableGet(dev, portNum, up, &state);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, portNum, up, state);

        /*
            1.2. Call function with wrong portNum [63, 65].
            Expected: GT_BAD_PARAM.
        */
        portNum = 64;

        st = cpssDxChBrgVlanKeepVlan1EnableGet(dev, portNum, up, &state);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum, up, state);

        /*
            1.3. Call function with wrong up [8].
            Expected: GT_BAD_PARAM.
        */
        portNum = 20;
        up = 8;

        st = cpssDxChBrgVlanKeepVlan1EnableGet(dev, portNum, up, &state);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum, up, state);

        /*
            1.4. Call function with NULL pointer.
            Expected: GT_BAD_PTR.
        */
        portNum = 20;
        up = 7;

        st = cpssDxChBrgVlanKeepVlan1EnableGet(dev, portNum, up, NULL);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PTR, st, dev, portNum, up, state);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChBrgVlanKeepVlan1EnableGet(dev, portNum, up, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChBrgVlanKeepVlan1EnableGet(dev, portNum, up, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Vlan table.
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanFillVlanTable)
{
/*
    ITERATE_DEVICE (DxChx)
    1.1. Get table Size.
         Call cpssDxChCfgTableNumEntriesGet with table [CPSS_DXCH_CFG_TABLE_VLAN_E]
                                                 and non-NULL numEntriesPtr.
    Expected: GT_OK.
    1.2. Fill all entries in Vlan table.
            Call cpssDxChBrgVlanEntryWrite with vlanId [0..numEntries-1],
                               portsMemberPtr {[0, 16]},
                               portsTaggingPtr {[0, 16]},
                               vlanInfoPtr {unkSrcAddrSecBreach [GT_FALSE],
                                            unregNonIpMcastCmd [CPSS_PACKET_CMD_FORWARD_E],
                                            unregIpv4McastCmd [CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                            unregIpv6McastCmd [CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                                            unkUcastCmd [CPSS_PACKET_CMD_DROP_HARD_E],
                                            unregIpv4BcastCmd [CPSS_PACKET_CMD_DROP_SOFT_E],
                                            unregNonIpv4BcastCmd [CPSS_PACKET_CMD_FORWARD_E],
                                            ipv4IgmpToCpuEn [GT_TRUE],
                                            mirrToRxAnalyzerEn [GT_TRUE],
                                            ipv6IcmpToCpuEn[GT_TRUE],
                                            ipCtrlToCpuEn [CPSS_DXCH_BRG_IP_CTRL_NONE_E],
                                            ipv4IpmBrgMode [CPSS_BRG_IPM_SGV_E],
                                            ipv6IpmBrgMode [CPSS_BRG_IPM_GV_E],
                                            ipv4IpmBrgEn[GT_TRUE],
                                            ipv6IpmBrgEn [GT_TRUE],
                                            ipv6SiteIdMode [CPSS_IP_SITE_ID_INTERNAL_E],
                                            ipv4UcastRouteEn[GT_FALSE],
                                            ipv4McastRouteEn [GT_FALSE],
                                            ipv6UcastRouteEn[GT_FALSE],
                                            ipv6McastRouteEn[GT_FALSE],
                                            stgId[0],
                                            autoLearnDisable [GT_FALSE],
                                            naMsgToCpuEn [GT_FALSE],
                                            mruIdx[0],
                                            bcastUdpTrapMirrEn [GT_FALSE],
                                            vrfId [0]}.
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
            Call cpssDxChBrgVlanEntryWrite with vlanId [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in Vlan table and compare with original.
         Call cpssDxChBrgVlanEntryRead with the same vlanId.
    Expected: GT_OK and the same portsMembersPtr, portsTaggingPtr and vlanInfoPtr.
    Check vlanInfo by fields:
                1) skip ipv6SiteIdMode, ipv4McastRateEn, ipv6McastRateEn,
                   autoLearnDisable, naMsgToCpuEn, mruIdx, bcastUdpTrapMirrEn
                   fields for Cheetah, but they should be checked for DxCh2, DxCh3;
                2) ipv4IpmBrgMode and ipv6IpnBrgMode should be checked only if
                   corresponding ipv4/6IpmBrgEn = GT_TRUE.
    1.5. Try to read entry with index out of range.
         Call cpssDxChBrgVlanEntryRead with the same vlanId [numEntries].
    Expected: NOT GT_OK.
    1.6. Delete all entries in Vlan table.
         Call cpssDxChBrgVlanEntryInvalidate with the same vlanId to invalidate all changes.
    Expected: GT_OK.
    1.7. Try to delete entry with index out of range.
         Call cpssDxChBrgVlanEntryInvalidate with the same vlanId [numEntries].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U16      iTemp      = 0;

    CPSS_PORTS_BMP_STC          portsMembers;
    CPSS_PORTS_BMP_STC          portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC vlanInfo;

    CPSS_PORTS_BMP_STC          portsMembersGet;
    CPSS_PORTS_BMP_STC          portsTaggingGet;
    CPSS_DXCH_BRG_VLAN_INFO_STC vlanInfoGet;

    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmd;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  portsTaggingCmdGet;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdGetPtr;

    GT_BOOL                     isValid = GT_FALSE;
    GT_BOOL                     isEqual = GT_FALSE;

    CPSS_PP_FAMILY_TYPE_ENT     devFamily;

    cpssOsBzero((GT_VOID*) &vlanInfo, sizeof(vlanInfo));
    cpssOsBzero((GT_VOID*) &portsMembers, sizeof(portsMembers));
    cpssOsBzero((GT_VOID*) &portsTagging, sizeof(portsTagging));
    cpssOsBzero((GT_VOID*) &portsTaggingCmd, sizeof(portsTaggingCmd));
    cpssOsBzero((GT_VOID*) &vlanInfoGet, sizeof(vlanInfoGet));
    cpssOsBzero((GT_VOID*) &portsMembersGet, sizeof(portsMembersGet));
    cpssOsBzero((GT_VOID*) &portsTaggingGet, sizeof(portsTaggingGet));
    cpssOsBzero((GT_VOID*) &portsTaggingCmdGet, sizeof(portsTaggingCmdGet));

    /* Fill the entry for vlan table */
    portsMembers.ports[0] = 17;
    portsMembers.ports[1] = 0;
    portsTagging.ports[0] = 17;
    portsTagging.ports[1] = 0;

    vlanInfo.unkSrcAddrSecBreach  = GT_FALSE;
    vlanInfo.unregNonIpMcastCmd   = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregIpv4McastCmd    = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
    vlanInfo.unregIpv6McastCmd    = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    vlanInfo.unkUcastCmd          = CPSS_PACKET_CMD_DROP_HARD_E;
    vlanInfo.unregIpv4BcastCmd    = CPSS_PACKET_CMD_DROP_SOFT_E;
    vlanInfo.unregNonIpv4BcastCmd = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.ipv4IgmpToCpuEn      = GT_TRUE;
    vlanInfo.mirrToRxAnalyzerEn   = GT_TRUE;
    vlanInfo.ipv6IcmpToCpuEn      = GT_TRUE;
    vlanInfo.ipCtrlToCpuEn        = CPSS_DXCH_BRG_IP_CTRL_NONE_E;
    vlanInfo.ipv4IpmBrgMode       = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv6IpmBrgMode       = CPSS_BRG_IPM_GV_E;
    vlanInfo.ipv4IpmBrgEn         = GT_TRUE;
    vlanInfo.ipv6IpmBrgEn         = GT_TRUE;
    vlanInfo.ipv6SiteIdMode       = CPSS_IP_SITE_ID_INTERNAL_E;
    vlanInfo.ipv4UcastRouteEn     = GT_FALSE;
    vlanInfo.ipv4McastRouteEn     = GT_FALSE;
    vlanInfo.ipv6UcastRouteEn     = GT_FALSE;
    vlanInfo.ipv6McastRouteEn     = GT_FALSE;
    vlanInfo.stgId                = 0;
    vlanInfo.autoLearnDisable     = GT_FALSE;
    vlanInfo.naMsgToCpuEn         = GT_FALSE;
    vlanInfo.mruIdx               = 0;
    vlanInfo.bcastUdpTrapMirrEn   = GT_FALSE;
    vlanInfo.vrfId                = 100;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. Get table Size */
        st = cpssDxChCfgTableNumEntriesGet(dev, CPSS_DXCH_CFG_TABLE_VLAN_E, &numEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChCfgTableNumEntriesGet: %d", dev);

        if (IS_TR101_SUPPORTED_MAC(dev))
        {
            /* portsTaggingCmd relevant for TR101 enabled devices */
            portsTaggingCmd.portsCmd[0] = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;
            portsTaggingCmd.portsCmd[1] = CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E;
            portsTaggingCmd.portsCmd[2] = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E;
            portsTaggingCmd.portsCmd[3] = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E;
            portsTaggingCmd.portsCmd[8] = CPSS_DXCH_BRG_VLAN_PORT_PUSH_TAG0_CMD_E;
            portsTaggingCmd.portsCmd[9] = CPSS_DXCH_BRG_VLAN_PORT_POP_OUTER_TAG_CMD_E;
            portsTaggingCmd.portsCmd[17] = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;
            portsTaggingCmd.portsCmd[22] = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E;

            /* port tagging CMD is used */
            portsTaggingCmdPtr = &portsTaggingCmd;
            portsTaggingCmdGetPtr = &portsTaggingCmdGet;
        }
        else
        {
            /* port tagging CMD should be ignored */
            /* coverity[assign_zero] */ portsTaggingCmdPtr = portsTaggingCmdGetPtr = NULL;
        }

        /* 1.2. Fill all entries in vlan table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            CPSS_COVERITY_NON_ISSUE_BOOKMARK
            /* coverity[var_deref_model] */
            st = cpssDxChBrgVlanEntryWrite(dev, iTemp, &portsMembers, &portsTagging,
                                           &vlanInfo, portsTaggingCmdPtr);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgVlanEntryWrite: %d, %d", dev, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[var_deref_model] */
        st = cpssDxChBrgVlanEntryWrite(dev, (GT_U16)numEntries, &portsMembers,
                                       &portsTagging, &vlanInfo, portsTaggingCmdPtr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChBrgVlanEntryWrite: %d, %d", dev, numEntries);

        /* 1.4. Read all entries in vlan table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssDxChBrgVlanEntryRead(dev, iTemp, &portsMembersGet, &portsTaggingGet,
                                            &vlanInfoGet, &isValid, portsTaggingCmdGetPtr);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgVlanEntryRead: %d, %d", dev, iTemp);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isValid,
                    "Vlan entry is NOT valid: %d, %d", dev, iTemp);

            if (GT_TRUE == isValid)
            {
                /* Verifying portsMembersPtr */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsMembers,
                                             (GT_VOID*) &portsMembersGet,
                                             sizeof(portsMembers))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                           "get another portsMembersPtr than was set: %d, %d", dev, iTemp);

                if (IS_TR101_SUPPORTED_MAC(dev))
                {
                    /* Verifying portsTaggingCmd */
                    isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsTaggingCmd,
                                                 (GT_VOID*) &portsTaggingCmdGet,
                                                 sizeof(portsTaggingCmd))) ? GT_TRUE : GT_FALSE;
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                               "get another portsTaggingCmd than was set: %d, %d", dev, iTemp);
                }
                else
                {
                    /* Verifying portsTaggingPtr */
                    isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsTagging,
                                                 (GT_VOID*) &portsTaggingGet,
                                                 sizeof(portsTagging))) ? GT_TRUE : GT_FALSE;
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                               "get another portsTaggingPtr than was set: %d, %d", dev, iTemp);
                }

                /* Verifying vlanInfoPtr fields */
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unkSrcAddrSecBreach,
                                             vlanInfoGet.unkSrcAddrSecBreach,
                 "get another vlanInfoPtr->unkSrcAddrSecBreach than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregNonIpMcastCmd,
                                             vlanInfoGet.unregNonIpMcastCmd,
                 "get another vlanInfoPtr->unregNonIpMcastCmd than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregIpv4McastCmd,
                                             vlanInfoGet.unregIpv4McastCmd,
                 "get another vlanInfoPtr->unregIpv4McastCmd than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregIpv6McastCmd,
                                             vlanInfoGet.unregIpv6McastCmd,
                 "get another vlanInfoPtr->unregIpv6McastCmd than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unkUcastCmd,
                                             vlanInfoGet.unkUcastCmd,
                 "get another vlanInfoPtr->unkUcastCmd than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregIpv4BcastCmd,
                                             vlanInfoGet.unregIpv4BcastCmd,
                 "get another vlanInfoPtr->unregIpv4BcastCmd than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.unregNonIpv4BcastCmd,
                                             vlanInfoGet.unregNonIpv4BcastCmd,
                 "get another vlanInfoPtr->unregNonIpv4BcastCmd than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IgmpToCpuEn,
                                             vlanInfoGet.ipv4IgmpToCpuEn,
                 "get another vlanInfoPtr->ipv4IgmpToCpuEn than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.mirrToRxAnalyzerEn,
                                             vlanInfoGet.mirrToRxAnalyzerEn,
                 "get another vlanInfoPtr->mirrToRxAnalyzerEn than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IcmpToCpuEn,
                                             vlanInfoGet.ipv6IcmpToCpuEn,
                 "get another vlanInfoPtr->ipv6IcmpToCpuEn than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipCtrlToCpuEn,
                                             vlanInfoGet.ipCtrlToCpuEn,
                 "get another vlanInfoPtr->ipCtrlToCpuEn than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IpmBrgEn,
                                             vlanInfoGet.ipv4IpmBrgEn,
                 "get another vlanInfoPtr->ipv4IpmBrgEn than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IpmBrgEn,
                                             vlanInfoGet.ipv6IpmBrgEn,
                 "get another vlanInfoPtr->ipv6IpmBrgEn than was set: %d, %d", dev, iTemp);

                if (GT_TRUE == vlanInfo.ipv4IpmBrgEn)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IpmBrgMode,
                                                 vlanInfoGet.ipv4IpmBrgMode,
                     "get another vlanInfoPtr->ipv4IpmBrgMode than was set: %d, %d", dev, iTemp);
                }

                if (GT_TRUE == vlanInfo.ipv6IpmBrgEn)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IpmBrgMode,
                                                 vlanInfoGet.ipv6IpmBrgMode,
                     "get another vlanInfoPtr->ipv6IpmBrgMode than was set: %d, %d", dev, iTemp);
                }

                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4UcastRouteEn,
                                             vlanInfoGet.ipv4UcastRouteEn,
                 "get another vlanInfoPtr->ipv4UcastRouteEn than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6UcastRouteEn,
                                             vlanInfoGet.ipv6UcastRouteEn,
                 "get another vlanInfoPtr->ipv6UcastRouteEn than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.stgId,
                                             vlanInfoGet.stgId,
                 "get another vlanInfoPtr->stgId than was set: %d, %d", dev, iTemp);

                if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
                {
                    if (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
                    {
                        UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6SiteIdMode,
                                                     vlanInfoGet.ipv6SiteIdMode,
                         "get another vlanInfoPtr->ipv6SiteIdMode than was set: %d, %d", dev, iTemp);
                        UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4McastRouteEn,
                                                     vlanInfoGet.ipv4McastRouteEn,
                         "get another vlanInfoPtr->ipv4McastRouteEn than was set: %d, %d", dev, iTemp);
                        UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6McastRouteEn,
                                                     vlanInfoGet.ipv6McastRouteEn,
                         "get another vlanInfoPtr->ipv6McastRouteEn than was set: %d, %d", dev, iTemp);
                    }
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.autoLearnDisable,
                                                 vlanInfoGet.autoLearnDisable,
                     "get another vlanInfoPtr->autoLearnDisable than was set: %d, %d", dev, iTemp);
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.naMsgToCpuEn,
                                                 vlanInfoGet.naMsgToCpuEn,
                     "get another vlanInfoPtr->naMsgToCpuEn than was set: %d, %d", dev, iTemp);
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.mruIdx,
                                                 vlanInfoGet.mruIdx,
                     "get another vlanInfoPtr->mruIdx than was set: %d, %d", dev, iTemp);
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.bcastUdpTrapMirrEn,
                                                 vlanInfoGet.bcastUdpTrapMirrEn,
                     "get another vlanInfoPtr->bcastUdpTrapMirrEn than was set: %d, %d", dev, iTemp);
                }
            }

            if ((devFamily >= CPSS_PP_FAMILY_CHEETAH3_E) && (devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.vrfId, vlanInfoGet.vrfId,
                 "get another vlanInfoPtr->vrfId than was set: %d, %d", dev, iTemp);
            }
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssDxChBrgVlanEntryRead(dev, (GT_U16)numEntries, &portsMembersGet,
                        &portsTaggingGet,&vlanInfoGet, &isValid, portsTaggingCmdGetPtr);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChBrgVlanEntryRead: %d, %d", dev, numEntries);

        /* 1.6. Delete all entries in vlan table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssDxChBrgVlanEntryInvalidate(dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgVlanEntryInvalidate: %d, %d", dev, iTemp);
        }

        /* 1.7. Try to delete entry with index out of range. */
        st = cpssDxChBrgVlanEntryInvalidate(dev, (GT_U16)numEntries);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                "cpssDxChBrgVlanEntryInvalidate: %d, %d", dev, numEntries);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Vlan Translation Table Ingress.
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanFillVlanTranslationTableIngress)
{
/*
    ITERATE_DEVICE (DxCh3)
    1.1. Get table Size.
         Call cpssDxChCfgTableNumEntriesGet with table [CPSS_DXCH_CFG_TABLE_VLAN_E]
                                                 and non-NULL numEntriesPtr.
    Expected: GT_OK.
    1.2. Fill all entries in Vlan Translation Table Ingress.
         Call cpssDxChBrgVlanTranslationEntryWrite with vlanId [0..numEntries-1],
                                                        direction [CPSS_DIRECTION_INGRESS_E]
                                                        and transVlanId [100].
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssDxChBrgVlanTranslationEntryWrite with vlanId [numEntries],
                                                        direction [CPSS_DIRECTION_INGRESS_E]
                                                        and transVlanId [100].
    Expected: NOT GT_OK.
    1.4. Read all entries in Vlan Translation Table Ingress and compare with original.
         Call cpssDxChBrgVlanTranslationEntryRead with same vlanId and direction.
    Expected: GT_OK and the same transVlanId.
    1.5. Try to read entry with index out of range.
         Call cpssDxChBrgVlanTranslationEntryRead with same vlanId [numEntries] and direction.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries  = 0;
    GT_U16      iTemp       = 0;

    GT_U16              vlanId         = 0;
    CPSS_DIRECTION_ENT  direction      = CPSS_DIRECTION_INGRESS_E;
    GT_U16              transVlanId    = 0;
    GT_U16              transVlanIdGet = 0;

    /* Fill the entry for Vlan Translation Table Ingress */
    vlanId      = 100;
    direction   = CPSS_DIRECTION_INGRESS_E;
    transVlanId = 100;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        st = cpssDxChCfgTableNumEntriesGet(dev, CPSS_DXCH_CFG_TABLE_VLAN_E, &numEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChCfgTableNumEntriesGet: %d", dev);

        /* 1.2. Fill all entries in Vlan Translation Table Ingress */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssDxChBrgVlanTranslationEntryWrite(dev, iTemp, direction, transVlanId);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgVlanTranslationEntryWrite: %d, %d, %d, %d",
                                         dev, iTemp, direction, transVlanId);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssDxChBrgVlanTranslationEntryWrite(dev, (GT_U16)numEntries, direction, transVlanId);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                "cpssDxChBrgVlanTranslationEntryWrite: %d, %d, %d, %d",
                                         dev, numEntries, direction, transVlanId);

        /* 1.4. Read all entries in Vlan Translation Table Ingress and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssDxChBrgVlanTranslationEntryRead(dev, iTemp, direction, &transVlanIdGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgVlanTranslationEntryRead: %d, %d, %d", dev, iTemp, direction);
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(transVlanId, transVlanIdGet,
                       "get another transVlanId than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssDxChBrgVlanTranslationEntryRead(dev, (GT_U16)numEntries,
                                                 direction, &transVlanIdGet);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "cpssDxChBrgVlanTranslationEntryRead: %d, %d, %d", dev, numEntries, direction);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Vlan Translation Table Egress.
*/
UTF_TEST_CASE_MAC(cpssDxChBrgVlanFillVlanTranslationTableEgress)
{
/*
    ITERATE_DEVICE (DxCh3)
    1.1. Get table Size.
         Call cpssDxChCfgTableNumEntriesGet with table [CPSS_DXCH_CFG_TABLE_VLAN_E]
                                                 and non-NULL numEntriesPtr.
    Expected: GT_OK.
    1.2. Fill all entries in Vlan Translation Table Ingress.
         Call cpssDxChBrgVlanTranslationEntryWrite with vlanId [0..numEntries-1],
                                                        direction [CPSS_DIRECTION_INGRESS_E]
                                                        and transVlanId [100].
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssDxChBrgVlanTranslationEntryWrite with vlanId [numEntries],
                                                        direction [CPSS_DIRECTION_INGRESS_E]
                                                        and transVlanId [100].
    Expected: NOT GT_OK.
    1.4. Read all entries in Vlan Translation Table Ingress and compare with original.
         Call cpssDxChBrgVlanTranslationEntryRead with same vlanId and direction.
    Expected: GT_OK and the same transVlanId.
    1.5. Try to read entry with index out of range.
         Call cpssDxChBrgVlanTranslationEntryRead with same vlanId [numEntries] and direction.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U16      iTemp      = 0;

    CPSS_DIRECTION_ENT  direction      = CPSS_DIRECTION_INGRESS_E;
    GT_U16              transVlanId    = 0;
    GT_U16              transVlanIdGet = 0;


    /* Fill the entry for Vlan Translation Table Egress */
    direction   = CPSS_DIRECTION_EGRESS_E;
    transVlanId = 100;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        st = cpssDxChCfgTableNumEntriesGet(dev, CPSS_DXCH_CFG_TABLE_VLAN_E, &numEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChCfgTableNumEntriesGet: %d", dev);

        /* 1.2. Fill all entries in Vlan Translation Table Egress */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssDxChBrgVlanTranslationEntryWrite(dev, iTemp, direction, transVlanId);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgVlanTranslationEntryWrite: %d, %d, %d, %d",
                                         dev, iTemp, direction, transVlanId);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssDxChBrgVlanTranslationEntryWrite(dev, (GT_U16)numEntries, direction, transVlanId);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                "cpssDxChBrgVlanTranslationEntryWrite: %d, %d, %d, %d",
                                         dev, numEntries, direction, transVlanId);

        /* 1.4. Read all entries in Vlan Translation Table Egress and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssDxChBrgVlanTranslationEntryRead(dev, iTemp, direction, &transVlanIdGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "cpssDxChBrgVlanTranslationEntryRead: %d, %d, %d", dev, iTemp, direction);
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(transVlanId, transVlanIdGet,
                       "get another transVlanId than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssDxChBrgVlanTranslationEntryRead(dev, (GT_U16)numEntries, direction, &transVlanIdGet);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "cpssDxChBrgVlanTranslationEntryRead: %d, %d, %d", dev, numEntries, direction);
    }
}
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChBrgVlan suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChBrgVlan)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanEntriesRangeWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanMemberAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortVidGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortVidSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortIngFltEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortProtoClassVlanEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortProtoClassQosEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanProtoClassSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanProtoClassGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanProtoClassInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortProtoVlanQosSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortProtoVlanQosGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortProtoVlanQosInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanLearningStateSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanIsDevMember)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanToStpIdBind)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanStpIdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanEtherTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanTableInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanEntryInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanUnkUnregFilterSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanForcePvidEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanForcePvidEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortVidPrecedenceSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanIpUcRouteEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanIpMcRouteEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanNASecurEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanIgmpSnoopingEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanIgmpSnoopingEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanIpCntlToCpuSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanIpCntlToCpuGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanIpV6IcmpToCpuEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanIpV6IcmpToCpuEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanUdpBcPktsToCpuEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanUdpBcPktsToCpuEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanIpv6SourceSiteIdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanIpmBridgingEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanIpmBridgingModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanIngressMirrorEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortAccFrameTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortAccFrameTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanMruProfileIdxSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanMruProfileValueSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanNaToCpuEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanBridgingModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanVrfIdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortTranslationEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortTranslationEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanTranslationEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanTranslationEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanValidCheckEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanValidCheckEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanFloodVidxModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanLocalSwitchingEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanMemberSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortEgressTpidGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortEgressTpidSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortIngressTpidGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortIngressTpidSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortIsolationCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanPortVidPrecedenceGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanRangeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanRangeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanTpidEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanTpidEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanInit)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanRemoveVlanTag1IfZeroModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanRemoveVlanTag1IfZeroModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanForceNewDsaToCpuEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanForceNewDsaToCpuEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanKeepVlan1EnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanKeepVlan1EnableGet)


    /* Test filling Table */
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanFillVlanTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanFillVlanTranslationTableIngress)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChBrgVlanFillVlanTranslationTableEgress)
UTF_SUIT_END_TESTS_MAC(cpssDxChBrgVlan)


