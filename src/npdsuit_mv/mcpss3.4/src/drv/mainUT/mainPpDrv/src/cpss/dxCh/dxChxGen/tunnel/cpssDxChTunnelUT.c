/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTunnelUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChTunnel, that provides
*       CPSS tunnel implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnel.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define TUNNEL_VALID_PHY_PORT_CNS  0


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelStartEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *configPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelStartEntrySet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: DxCh2; DxCh3; xCat; Lion)
    1.1. Call with routerArpTunnelStartLineIndex [max - 1],
              tunnelType [CPSS_TUNNEL_X_OVER_IPV4_E /
                          CPSS_TUNNEL_X_OVER_GRE_IPV4_E]
              and configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                                     vlanId [100],
                                     upMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                     up [0],
                                     dscpMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                     dscp [0],
                                     macDa [00:1A:FF:FF:FF:FF],
                                     dontFragmentFlag [GT_TRUE],
                                     ttl [0],
                                     autoTunnel [GT_TRUE],
                                     autoTunnelOffset [1],
                                     destIp->arIP [10.15.1.250],
                                     srcIp->arIP [10.15.1.255]}.
    Expected: GT_OK.
    1.2. Call with configPtr->ipv4Cfg{destIp->arIP [255.255.255.255],
                                      srcIp->arIP [255.255.255.251]}
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.3. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                                      out of range vlanId [4096]}
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                                      wrong enum values upMarkMode }
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                                      upMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                      out of range up [8]}
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                                       dscpMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                       out of range dscp [64]}
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range configPtr->ipv4Cfg->ttl [256]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with routerArpTunnelStartLineIndex [10],
        tunnelType [CPSS_TUNNEL_X_OVER_MPLS_E]
        and configPtr->mplsCfg {tagEnable [GT_TRUE],
                                vlanId[100],
                                upMarkMode[CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                up [7],
                                macDa [00:1A:FF:FF:FF:FF],
                                numLabels[2],
                                ttl [10],
                                label1 [100],
                                exp1MarkMode[CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                exp1[7],
                                label2 [200],
                                exp2MarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                exp2[7],
                                retainCRC [GT_TRUE]}
    Expected: GT_OK.
    1.9. Call with configPtr->mplsCfg {tagEnable [GT_TRUE],
                                        out of range vlanId [4096]}
                    and other parameters from 1.19.
    Expected: GT_BAD_PARAM.
    1.10. Call with configPtr->mplsCfg {tagEnable [GT_TRUE],
                                        wrong enum values upMarkMode }
                    and other parameters from 1.19.
    Expected: GT_BAD_PARAM.
    1.11. Call with configPtr->mplsCfg {tagEnable [GT_TRUE],
                                        upMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                        out of range up [8]}
                    and other parameters from 1.19.
    Expected: NOT GT_OK.
    1.12. Call with out of range configPtr->mplsCfg {numLabels[0, 5]}
                    and other parameters from 1.19.
    Expected: NOT GT_OK.
    1.13. Call with out of range configPtr->mplsCfg->ttl [256]
                    and other parameters from 1.19.
    Expected: NOT GT_OK.
    1.14. Call with configPtr->mplsCfg->{numLabels [2],
                                         out of range label1 [1048576]
                                    (relevant only when number of labels is 2)}
                    and other parameters from 1.19.
    Expected: NOT GT_OK.
    1.15. Call with configPtr->mplsCfg->{numLabels [2],
                                         wrong enum values exp1MarkMode
                                    (relevant only when number of labels is 2)}
                    and other parameters from 1.19.
    Expected: GT_BAD_PARAM.
    1.16. Call with configPtr->mplsCfg->{numLabels [2],
          exp1MarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
          out of range exp1[8]
          (relevant only when number of labels is 2 and exp1MarkMode is set according to entry <EXP1>)}
          and other parameters from 1.19.
    Expected: GT_BAD_PARAM.
    1.17. Call with configPtr->mplsCfg->{out of range label2 [1048576]}
                    and other parameters from 1.19.
    Expected: NOT GT_OK.
    1.18. Call with configPtr->mplsCfg->{wrong enum values exp2MarkMode }
                    and other parameters from 1.19.
    Expected: GT_BAD_PARAM.
    1.19. Call with configPtr->mplsCfg->{exp2MarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                         out of range exp2[8]}
                    and other parameters from 1.19.
    Expected: GT_BAD_PARAM.
    1.20. Call with out of range routerArpTunnelStartLineIndex [max]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.21. Call with wrong enum values tunnelType
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.22. Call with out of range tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E /
                                             CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E /
                                             CPSS_TUNNEL_IPV6_OVER_IPV4_E /
                                             CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E /
                                             CPSS_TUNNEL_X_OVER_MPLS_E /
                                             CPSS_TUNNEL_ETHERNET_OVER_MPLS_E /
                                             CPSS_TUNNEL_IP_OVER_X_E]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.23. Call with configPtr [NULL] and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                         st         = GT_OK;

    GT_U8                             dev;
    GT_U32                            lineIndex  = 0;
    CPSS_TUNNEL_TYPE_ENT              tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;
    CPSS_DXCH_TUNNEL_START_CONFIG_UNT config;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with routerArpTunnelStartLineIndex [max - 1],
                           tunnelType [CPSS_TUNNEL_X_OVER_IPV4_E /
                                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E]
                           and configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                                                  vlanId [100],
                                                  upMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                                  up [0],
                                                  dscpMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                                  dscp [0],
                                                  cfi [0],
                                                  macDa [00:1A:FF:FF:FF:FF],
                                                  dontFragmentFlag [GT_TRUE],
                                                  ttl [0],
                                                  autoTunnel [GT_TRUE],
                                                  autoTunnelOffset [1],
                                                  destIp->arIP [10.15.1.250],
                                                  srcIp->arIP [10.15.1.255]}.
            Expected: GT_OK.
        */

        /* Call with tunnelType [CPSS_TUNNEL_X_OVER_IPV4_E] */
        lineIndex  = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.tunnelStart - 1;

        tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;

        config.ipv4Cfg.tagEnable    = GT_TRUE;
        config.ipv4Cfg.vlanId       = 100;
        config.ipv4Cfg.upMarkMode   = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        config.ipv4Cfg.up           = 0;
        config.ipv4Cfg.dscpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        config.ipv4Cfg.dscp         = 0;
        config.ipv4Cfg.cfi          = 0;

        config.ipv4Cfg.macDa.arEther[0] = 0x0;
        config.ipv4Cfg.macDa.arEther[1] = 0x1A;
        config.ipv4Cfg.macDa.arEther[2] = 0xFF;
        config.ipv4Cfg.macDa.arEther[3] = 0xFF;
        config.ipv4Cfg.macDa.arEther[4] = 0xFF;
        config.ipv4Cfg.macDa.arEther[5] = 0xFF;

        config.ipv4Cfg.dontFragmentFlag = GT_TRUE;
        config.ipv4Cfg.ttl              = 0;
        config.ipv4Cfg.autoTunnel       = GT_TRUE;
        config.ipv4Cfg.autoTunnelOffset = 1;

        config.ipv4Cfg.destIp.arIP[0] = 10;
        config.ipv4Cfg.destIp.arIP[1] = 15;
        config.ipv4Cfg.destIp.arIP[2] = 1;
        config.ipv4Cfg.destIp.arIP[3] = 250;

        config.ipv4Cfg.srcIp.arIP[0] = 10;
        config.ipv4Cfg.srcIp.arIP[1] = 15;
        config.ipv4Cfg.srcIp.arIP[2] = 1;
        config.ipv4Cfg.srcIp.arIP[3] = 255;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
        }
        else
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
        }

        /* Call with tunnelType [CPSS_TUNNEL_X_OVER_GRE_IPV4_E] */
        tunnelType = CPSS_TUNNEL_X_OVER_GRE_IPV4_E;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
        }
        else
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
        }

        /*
            1.2. Call with configPtr->ipv4Cfg{destIp->arIP [255.255.255.255],
                                              srcIp->arIP [255.255.255.251]}
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;

        config.ipv4Cfg.destIp.arIP[0] = 255;
        config.ipv4Cfg.destIp.arIP[1] = 255;
        config.ipv4Cfg.destIp.arIP[2] = 255;
        config.ipv4Cfg.destIp.arIP[3] = 255;

        config.ipv4Cfg.srcIp.arIP[0] = 255;
        config.ipv4Cfg.srcIp.arIP[1] = 255;
        config.ipv4Cfg.srcIp.arIP[2] = 255;
        config.ipv4Cfg.srcIp.arIP[3] = 251;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
        }
        else
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
        }

        /*
            1.3. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                                              out of range vlanId [4096]}
                      and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        config.ipv4Cfg.destIp.arIP[0] = 10;
        config.ipv4Cfg.destIp.arIP[1] = 15;
        config.ipv4Cfg.destIp.arIP[2] = 1;
        config.ipv4Cfg.destIp.arIP[3] = 250;

        config.ipv4Cfg.srcIp.arIP[0] = 10;
        config.ipv4Cfg.srcIp.arIP[1] = 15;
        config.ipv4Cfg.srcIp.arIP[2] = 1;
        config.ipv4Cfg.srcIp.arIP[3] = 255;

        config.ipv4Cfg.tagEnable = GT_TRUE;
        config.ipv4Cfg.vlanId    = 4096;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
            "%d, configPtr->ipv4Cfg.tagEnable = %d, configPtr->ipv4Cfg.vlanId = %d",
                        dev, config.ipv4Cfg.tagEnable, config.ipv4Cfg.vlanId);

        config.ipv4Cfg.vlanId = 100;

        /*
            1.4. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                                              wrong enum values upMarkMode }
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        config.ipv4Cfg.vlanId     = 100;
        config.ipv4Cfg.tagEnable  = GT_TRUE;

        if (!prvUtfIsPbrModeUsed() || (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChTunnelStartEntrySet
                                (dev, lineIndex, tunnelType, &config),
                                config.ipv4Cfg.upMarkMode);
        }

        /*
            1.5. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                      upMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                      out of range up [8]}
                      and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        config.ipv4Cfg.tagEnable  = GT_TRUE;
        config.ipv4Cfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        config.ipv4Cfg.up         = 8;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
            "%d, configPtr->ipv4Cfg.tagEnable = %d, ->upMarkMode = %d, ->up = %d",
            dev, config.ipv4Cfg.tagEnable, config.ipv4Cfg.upMarkMode, config.ipv4Cfg.up);

        config.ipv4Cfg.up = 0;

        /*
            1.6. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                      dscpMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                      out of range dscp [64]}
                      and other parameters from 1.1.
            Expected: NOT GT_OK.
        */

        config.ipv4Cfg.tagEnable    = GT_TRUE;
        config.ipv4Cfg.dscpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        config.ipv4Cfg.dscp         = 64;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
            "%d, configPtr->ipv4Cfg.tagEnable = %d, ->dscpMarkMode = %d, ->dscp = %d",
            dev, config.ipv4Cfg.tagEnable, config.ipv4Cfg.dscpMarkMode, config.ipv4Cfg.dscp);

        config.ipv4Cfg.dscp = 0;

        /*
            1.7. Call with out of range configPtr->ipv4Cfg->ttl [256]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        config.ipv4Cfg.ttl = 256;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->ipv4Cfg.ttl = %d",
                                         dev, config.ipv4Cfg.ttl);

        config.ipv4Cfg.ttl = 0;

        /*
            1.8. Call with configPtr->ipv4Cfg->autoTunnelOffset [16] and autoTunnel [GT_TRUE]
                 (relevant for IPv6-over-IPv4 or IPv6-over-GRE-IPv4)
                 and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        config.ipv4Cfg.autoTunnel       = GT_TRUE;
        config.ipv4Cfg.autoTunnelOffset = 16;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
            "%d, tunnelType = %d, configPtr->ipv4Cfg.autoTunnelOffset = %d, ->autoTunnel = %d",
            dev, tunnelType, config.ipv4Cfg.autoTunnelOffset, config.ipv4Cfg.autoTunnel);

        /*
            1.9. Call with routerArpTunnelStartLineIndex [10],
                tunnelType [CPSS_TUNNEL_X_OVER_MPLS_E]
                and configPtr->mplsCfg {tagEnable [GT_TRUE],
                                        vlanId[100],
                                        upMarkMode[CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                        up [7],
                                        cfi [0],
                                        macDa [00:1A:FF:FF:FF:FF],
                                        numLabels[2],
                                        ttl [10],
                                        label1 [100],
                                        exp1MarkMode[CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                        exp1[7],
                                        label2 [200],
                                        exp2MarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                        exp2[7],
                                        retainCRC [GT_TRUE]}
            Expected: GT_OK.
        */
        lineIndex  = 10;
        tunnelType = CPSS_TUNNEL_X_OVER_MPLS_E;

        config.mplsCfg.tagEnable  = GT_TRUE;
        config.mplsCfg.vlanId     = 100;
        config.mplsCfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        config.mplsCfg.up         = 7;
        config.mplsCfg.cfi        = 0;

        config.mplsCfg.macDa.arEther[0] = 0x0;
        config.mplsCfg.macDa.arEther[1] = 0x1A;
        config.mplsCfg.macDa.arEther[2] = 0xFF;
        config.mplsCfg.macDa.arEther[3] = 0xFF;
        config.mplsCfg.macDa.arEther[4] = 0xFF;
        config.mplsCfg.macDa.arEther[5] = 0xFF;

        config.mplsCfg.numLabels    = 2;
        config.mplsCfg.ttl          = 10;
        config.mplsCfg.label1       = 100;
        config.mplsCfg.exp1MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        config.mplsCfg.exp1         = 7;
        config.mplsCfg.label2       = 200;
        config.mplsCfg.exp2MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        config.mplsCfg.exp2         = 7;
        config.mplsCfg.retainCRC    = GT_FALSE;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
        }
        else
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
        }

        /*
            1.10. Call with configPtr->mplsCfg {tagEnable [GT_TRUE],
                                                out of range vlanId [4096]}
                            and other parameters from 1.19.
            Expected: GT_BAD_PARAM.
        */
        config.mplsCfg.tagEnable = GT_TRUE;
        config.mplsCfg.vlanId    = 4096;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
                "%d, configPtr->mplsCfg.tagEnable = %d, configPtr->mplsCfg.vlanId = %d",
                                     dev, config.mplsCfg.tagEnable, config.mplsCfg.vlanId);

        config.mplsCfg.vlanId = 100;

        /*
            1.11. Call with configPtr->mplsCfg {tagEnable [GT_TRUE], wrong enum values upMarkMode}
                            and other parameters from 1.19.
            Expected: GT_BAD_PARAM.
        */
        config.mplsCfg.tagEnable  = GT_TRUE;

        if (!prvUtfIsPbrModeUsed() || (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChTunnelStartEntrySet
                                (dev, lineIndex, tunnelType, &config),
                                config.mplsCfg.upMarkMode);
        }

        /*
            1.12. Call with configPtr->mplsCfg {tagEnable [GT_TRUE],
                                                upMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                                out of range up [8]}
                            and other parameters from 1.19.
            Expected: NOT GT_OK.
        */
        config.mplsCfg.tagEnable  = GT_TRUE;
        config.mplsCfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        config.mplsCfg.up         = 8;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                "%d, configPtr->mplsCfg.tagEnable = %d, ->upMarkMode = %d, ->up = %d",
                dev, config.mplsCfg.tagEnable, config.mplsCfg.upMarkMode, config.mplsCfg.up);

        config.mplsCfg.up = 0;

        /*
            1.13. Call with out of range configPtr->mplsCfg {numLabels[0, 5]}
                            and other parameters from 1.19.
            Expected: NOT GT_OK.
        */
        /* Call with configPtr->mplsCfg.numLabels [0] */
        config.mplsCfg.numLabels = 0;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.numLabels = %d",
                                         dev, config.mplsCfg.numLabels);

        /* Call with configPtr->mplsCfg.numLabels [5] */
        config.mplsCfg.numLabels = 5;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.numLabels = %d",
                                         dev, config.mplsCfg.numLabels);

        /*
            1.14. Call with out of range configPtr->mplsCfg->ttl [256]
                            and other parameters from 1.19.
            Expected: NOT GT_OK.
        */
        config.mplsCfg.numLabels = 2;
        config.mplsCfg.ttl       = 256;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.ttl = %d",
                                         dev, config.mplsCfg.ttl);

        config.mplsCfg.ttl = 10;

        /*
            1.15. Call with configPtr->mplsCfg->{numLabels [2],
                  out of range label1 [1048576] (relevant only when number of labels is 2)}
                            and other parameters from 1.19.
            Expected: NOT GT_OK.
        */
        config.mplsCfg.label1 = 1048576;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, configPtr->mplsCfg.numLabels = %d, configPtr->mplsCfg.label1 = %d",
                                     dev, config.mplsCfg.numLabels, config.mplsCfg.label1);

        /*
            1.16. Call with configPtr->mplsCfg->{numLabels [1],
                                                 out of range label1 [1048576] }
                            and other parameters from 1.19.
            Expected: GT_BAD_PARAM.
        */
        config.mplsCfg.numLabels = 1;
        config.mplsCfg.label1    = 1048576;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
                "%d, configPtr->mplsCfg.numLabels = %d, configPtr->mplsCfg.label1 = %d",
                                     dev, config.mplsCfg.numLabels, config.mplsCfg.label1);

        /*
            1.17. Call with configPtr->mplsCfg->{numLabels [2],
                                                 wrong enum values exp1MarkMode
                                                 (relevant only when number of labels is 2)}
                            and other parameters from 1.19.
            Expected: GT_BAD_PARAM.
        */
        config.mplsCfg.numLabels    = 2;
        config.mplsCfg.label1       = 100;

        if (!prvUtfIsPbrModeUsed() || (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChTunnelStartEntrySet
                                (dev, lineIndex, tunnelType, &config),
                                config.mplsCfg.exp1MarkMode);
        }

        /*
            1.18. Call with configPtr->mplsCfg->{numLabels [2],
                  exp1MarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                  out of range exp1[8]
                  (relevant only when exp1MarkMode is set according to entry <EXP1>)}
                  and other parameters from 1.19.
            Expected: NOT GT_OK.
        */
        config.mplsCfg.numLabels    = 2;
        config.mplsCfg.exp1MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        config.mplsCfg.exp1         = 8;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
        "%d, configPtr->mplsCfg.numLabels = %d, configPtr->mplsCfg.exp1MarkMode = %d, configPtr->mplsCfg.exp1 = %d",
             dev, config.mplsCfg.numLabels, config.mplsCfg.exp1MarkMode, config.mplsCfg.exp1);

        /*
            1.19. Call with configPtr->mplsCfg->{numLabels [1],
                  exp1MarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                  out of range exp1[8]
                  (relevant only when exp1MarkMode is set according to entry <EXP1>)}
                  and other parameters from 1.19.
            Expected: GT_BAD_PARAM.
        */
        config.mplsCfg.numLabels = 1;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
                "%d, configPtr->mplsCfg.numLabels = %d, configPtr->mplsCfg.exp1 = %d",
                          dev, config.mplsCfg.numLabels, config.mplsCfg.exp1);

        /*
            1.20. Call with configPtr->mplsCfg->{out of range label2 [1048576]}
                            and other parameters from 1.19.
            Expected: NOT GT_OK.
        */
        config.mplsCfg.numLabels = 2;
        config.mplsCfg.label2    = 1048576;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.label2 = %d",
                                         dev, config.mplsCfg.label2);

        config.mplsCfg.label2 = 200;

        /*
            1.21. Call with configPtr->mplsCfg->{wrong enum values exp2MarkMode}
                            and other parameters from 1.19.
            Expected: GT_BAD_PARAM.
        */
        if (!prvUtfIsPbrModeUsed() || (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChTunnelStartEntrySet
                                (dev, lineIndex, tunnelType, &config),
                                config.mplsCfg.exp2MarkMode);
        }

        /*
            1.22. Call with configPtr->mplsCfg->{exp2MarkMode
                    [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                    out of range exp2[8]} and other parameters from 1.19.
            Expected: NOT GT_OK.
        */
        config.mplsCfg.exp2MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        config.mplsCfg.exp2         = 8;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.exp2 = %d",
                                         dev, config.mplsCfg.exp2);

        /*
            1.23. Call with out of range routerArpTunnelStartLineIndex [max]
                            and other parameters from 1.1.
            Expected: GT_OK for DxCh3 and NOT GT_OK for DxCh, DxCh2.
        */
        lineIndex  = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.tunnelStart;
        tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;

        config.ipv4Cfg.tagEnable    = GT_TRUE;
        config.ipv4Cfg.vlanId       = 100;
        config.ipv4Cfg.upMarkMode   = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        config.ipv4Cfg.up           = 0;
        config.ipv4Cfg.dscpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
        config.ipv4Cfg.dscp         = 0;
        config.ipv4Cfg.cfi          = 0;

        config.ipv4Cfg.macDa.arEther[0] = 0x0;
        config.ipv4Cfg.macDa.arEther[1] = 0x1A;
        config.ipv4Cfg.macDa.arEther[2] = 0xFF;
        config.ipv4Cfg.macDa.arEther[3] = 0xFF;
        config.ipv4Cfg.macDa.arEther[4] = 0xFF;
        config.ipv4Cfg.macDa.arEther[5] = 0xFF;

        config.ipv4Cfg.dontFragmentFlag = GT_TRUE;
        config.ipv4Cfg.ttl              = 0;
        config.ipv4Cfg.autoTunnel       = GT_TRUE;
        config.ipv4Cfg.autoTunnelOffset = 1;

        config.ipv4Cfg.destIp.arIP[0] = 10;
        config.ipv4Cfg.destIp.arIP[1] = 15;
        config.ipv4Cfg.destIp.arIP[2] = 1;
        config.ipv4Cfg.destIp.arIP[3] = 250;

        config.ipv4Cfg.srcIp.arIP[0] = 10;
        config.ipv4Cfg.srcIp.arIP[1] = 15;
        config.ipv4Cfg.srcIp.arIP[2] = 1;
        config.ipv4Cfg.srcIp.arIP[3] = 255;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lineIndex);

        /*
            1.24. Call with wrong enum values tunnelType and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        lineIndex  = 10;

        if (!prvUtfIsPbrModeUsed() || (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChTunnelStartEntrySet
                                (dev, lineIndex, tunnelType, &config),
                                tunnelType);
        }

        /*
            1.25. Call with out of range tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E /
                                                     CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E /
                                                     CPSS_TUNNEL_IPV6_OVER_IPV4_E /
                                                     CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E /
                                                     CPSS_TUNNEL_X_OVER_MPLS_E /
                                                     CPSS_TUNNEL_ETHERNET_OVER_MPLS_E /
                                                     CPSS_TUNNEL_IP_OVER_X_E]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */

        /* Call with tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E] */
        tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d",
                                         dev, tunnelType);

        /* Call with tunnelType [CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E] */
        tunnelType = CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d",
                                         dev, tunnelType);

        /* Call with tunnelType [CPSS_TUNNEL_IPV6_OVER_IPV4_E] */
        tunnelType = CPSS_TUNNEL_IPV6_OVER_IPV4_E;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d",
                                         dev, tunnelType);

        /* Call with tunnelType [CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E] */
        tunnelType = CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d",
                                         dev, tunnelType);

        /* Call with tunnelType [CPSS_TUNNEL_X_OVER_MPLS_E] */
        tunnelType = CPSS_TUNNEL_X_OVER_MPLS_E;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d",
                                         dev, tunnelType);

        /* Call with tunnelType [CPSS_TUNNEL_ETHERNET_OVER_MPLS_E] */
        tunnelType = CPSS_TUNNEL_ETHERNET_OVER_MPLS_E;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d",
                                         dev, tunnelType);

        /* Call with tunnelType [CPSS_TUNNEL_IP_OVER_X_E] */
        tunnelType = CPSS_TUNNEL_IP_OVER_X_E;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d",
                                         dev, tunnelType);

        /*
            1.25. Call with configPtr [NULL] and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;

        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, NULL);
        if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, configPtr = NULL", dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configPtr = NULL", dev);
        }
    }

    lineIndex  = 10;
    tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;

    config.ipv4Cfg.tagEnable    = GT_TRUE;
    config.ipv4Cfg.vlanId       = 100;
    config.ipv4Cfg.upMarkMode   = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
    config.ipv4Cfg.up           = 0;
    config.ipv4Cfg.dscpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
    config.ipv4Cfg.dscp         = 0;
    config.ipv4Cfg.cfi          = 0;

    config.ipv4Cfg.macDa.arEther[0] = 0x0;
    config.ipv4Cfg.macDa.arEther[1] = 0x1A;
    config.ipv4Cfg.macDa.arEther[2] = 0xFF;
    config.ipv4Cfg.macDa.arEther[3] = 0xFF;
    config.ipv4Cfg.macDa.arEther[4] = 0xFF;
    config.ipv4Cfg.macDa.arEther[5] = 0xFF;

    config.ipv4Cfg.dontFragmentFlag = GT_TRUE;
    config.ipv4Cfg.ttl              = 0;
    config.ipv4Cfg.autoTunnel       = GT_TRUE;
    config.ipv4Cfg.autoTunnelOffset = 1;

    config.ipv4Cfg.destIp.arIP[0] = 10;
    config.ipv4Cfg.destIp.arIP[1] = 15;
    config.ipv4Cfg.destIp.arIP[2] = 1;
    config.ipv4Cfg.destIp.arIP[3] = 250;

    config.ipv4Cfg.srcIp.arIP[0] = 10;
    config.ipv4Cfg.srcIp.arIP[1] = 15;
    config.ipv4Cfg.srcIp.arIP[2] = 1;
    config.ipv4Cfg.srcIp.arIP[3] = 255;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelStartEntrySet(dev, lineIndex, tunnelType, &config);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelStartEntryGet
(
    IN   GT_U8                              devNum,
    IN   GT_U32                             routerArpTunnelStartLineIndex,
    OUT  CPSS_TUNNEL_TYPE_ENT               *tunnelTypePtr,
    OUT  CPSS_DXCH_TUNNEL_START_CONFIG_UNT  *configPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelStartEntryGet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: DxCh2; DxCh3; xCat; Lion)
    1.1. Call with routerArpTunnelStartLineIndex[max - 1]
                   non-null tunnelTypePtr
                   and non-null configPtr.
    Expected: GT_OK.
    1.2. Call with out of range routerArpTunnelStartLineIndex [max],
                   non-null tunnelTypePtr
                   and non-null configPtr.
    Expected: NOT GT_OK.
    1.3. Call with routerArpTunnelStartLineIndex[max - 1],
                   tunnelTypePtr[NULL]
                   and non-null configPtr.
    Expected: GT_BAD_PTR.
    1.4. Call with routerArpTunnelStartLineIndex[max - 1],
                   non-null tunnelTypePtr and configPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                         st        = GT_OK;

    GT_U8                             dev;
    GT_U32                            lineIndex = 0;
    CPSS_TUNNEL_TYPE_ENT              tunnelType;
    CPSS_DXCH_TUNNEL_START_CONFIG_UNT config;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with routerArpTunnelStartLineIndex[max - 1]
                           non-null tunnelTypePtr
                           and non-null configPtr.
            Expected: GT_OK.
        */
        lineIndex  = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.tunnelStart - 1;

        st = cpssDxChTunnelStartEntryGet(dev, lineIndex, &tunnelType, &config);
        if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lineIndex);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lineIndex);
        }

        /*
            1.2. Call with out of range routerArpTunnelStartLineIndex [max],
                           non-null tunnelTypePtr
                           and non-null configPtr.
            Expected: NOT GT_OK.
        */
        lineIndex  = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.tunnelStart;

        st = cpssDxChTunnelStartEntryGet(dev, lineIndex, &tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lineIndex);

        lineIndex  = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.tunnelStart - 1;

        /*
            1.3. Call with routerArpTunnelStartLineIndex[max - 1],
                           tunnelTypePtr[NULL]
                           and non-null configPtr.
            Expected: GT_BAD_PTR.
        */
        lineIndex  = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.tunnelStart - 1;

        st = cpssDxChTunnelStartEntryGet(dev, lineIndex, NULL, &config);
        if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lineIndex);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tunnelTypePtr = NULL", dev);
        }

        /*
            1.4. Call with routerArpTunnelStartLineIndex[max - 1],
                           non-null tunnelTypePtr and configPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTunnelStartEntryGet(dev, lineIndex, &tunnelType, NULL);
        if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lineIndex);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configPtr = NULL", dev);
        }
    }

    lineIndex  = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelStartEntryGet(dev, lineIndex, &tunnelType, &config);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelStartEntryGet(dev, lineIndex, &tunnelType, &config);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpv4TunnelTermPortSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpv4TunnelTermPortSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh2, DxCh3)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChIpv4TunnelTermPortGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port     = TUNNEL_VALID_PHY_PORT_CNS;
    GT_BOOL     state    = GT_TRUE;
    GT_BOOL     stateGet = GT_TRUE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_TRUE and GT_FALSE].
                Expected: GT_OK.
            */

            /* Call function with enable [GT_FALSE] */
            state = GT_FALSE;

            st = cpssDxChIpv4TunnelTermPortSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* Call function with enable [GT_TRUE] */
            state = GT_TRUE;

            st = cpssDxChIpv4TunnelTermPortSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssDxChIpv4TunnelTermPortGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssDxChIpv4TunnelTermPortGet(dev, port, &stateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "cpssDxChIpv4TunnelTermPortGet: %d, %d", dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                                         "get another enable than was set: %d, %d", dev, port);
        }

        state = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChIpv4TunnelTermPortSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChIpv4TunnelTermPortSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChIpv4TunnelTermPortSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    state = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TUNNEL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpv4TunnelTermPortSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpv4TunnelTermPortSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChIpv4TunnelTermPortGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChIpv4TunnelTermPortGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh1, DxCh2)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = TUNNEL_VALID_PHY_PORT_CNS;
    GT_BOOL     state = GT_TRUE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-null enablePtr.
                Expected: GT_OK.
            */
            st = cpssDxChIpv4TunnelTermPortGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChIpv4TunnelTermPortGet(dev, port, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChIpv4TunnelTermPortGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChIpv4TunnelTermPortGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChIpv4TunnelTermPortGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TUNNEL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChIpv4TunnelTermPortGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChIpv4TunnelTermPortGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChMplsTunnelTermPortSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChMplsTunnelTermPortSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh1, DxCh2)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE ].
    Expected: GT_OK.
    1.1.2. Call cpssDxChMplsTunnelTermPortGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port     = TUNNEL_VALID_PHY_PORT_CNS;
    GT_BOOL     state    = GT_TRUE;
    GT_BOOL     stateGet = GT_TRUE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_TRUE and GT_FALSE].
                Expected: GT_OK.
            */

            /* Call function with enable [GT_FALSE] */
            state = GT_FALSE;

            st = cpssDxChMplsTunnelTermPortSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /* Call function with enable [GT_TRUE] */
            state = GT_TRUE;

            st = cpssDxChMplsTunnelTermPortSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssDxChMplsTunnelTermPortGet.
                Expected: GT_OK and the same enable.
            */
            st = cpssDxChMplsTunnelTermPortGet(dev, port, &stateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "cpssDxChMplsTunnelTermPortGet: %d, %d", dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                                         "get another enable than was set: %d, %d", dev, port);
        }

        state = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChMplsTunnelTermPortSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChMplsTunnelTermPortSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChMplsTunnelTermPortSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    state = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TUNNEL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChMplsTunnelTermPortSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChMplsTunnelTermPortSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChMplsTunnelTermPortGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChMplsTunnelTermPortGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh1, DxCh2)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;

    GT_U8       dev;
    GT_U8       port  = TUNNEL_VALID_PHY_PORT_CNS;
    GT_BOOL     state = GT_TRUE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-null enablePtr.
                Expected: GT_OK.
            */
            st = cpssDxChMplsTunnelTermPortGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChMplsTunnelTermPortGet(dev, port, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChMplsTunnelTermPortGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChMplsTunnelTermPortGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChMplsTunnelTermPortGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TUNNEL_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChMplsTunnelTermPortGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChMplsTunnelTermPortGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelTermEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTunnelTermTcamIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configPtr,
    IN  CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configMaskPtr,
    IN  CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelTermEntrySet)
{
/*

    ITERATE_DEVICES (DxCh1, DxCh2)
    1.1. Call with routerTunnelTermTcamIndex [10],
                   tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E],
                   configPtr->ipv4Cfg{srcPortTrunk [0],
                                      srcIsTrunk [0],
                                      srcDev [0],
                                      vid [100],
                                      macDa [00:1A:FF:FF:FF:FF],
                                      srcIp->arIP[10.15.1.250],
                                      destIp->arIP[10.15.1.255]},
                   configMaskPtr-> ipv4Cfg{0xFF,0xFF,...},
                   actionPtr{command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                             userDefinedCpuCode [CPSS_NET_FIRST_USER_DEFINED_E],
                             passengerPacketType [CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E],
                             egressInterface { type [CPSS_INTERFACE_TRUNK_E],
                                               trunkId [0]},
                             isTunnelStart [GT_FALSE],
                             tunnelStartIdx [0],
                             vlanId [100],
                             vlanPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                             nestedVlanEnable [GT_FALSE],
                             copyTtlFromTunnelHeader [GT_FALSE],
                             qosMode [CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E],
                             qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                             qosProfile [127],
                             defaultUP [0],
                             modifyUP [CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E],
                             remapDSCP [GT_FALSE],
                             modifyDSCP [CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E],
                             mirrorToIngressAnalyzerEnable [GT_FALSE],
                             policerEnable [GT_FALSE],
                             policerIndex [0],
                             matchCounterEnable [GT_FALSE],
                             matchCounterIndex [0]}
    Expected: GT_OK.

    1.2. Call cpssDxChTunnelTermEntryGet with non-NULL pointers and tcamIndex [10].
        Expected: GT_OK and  valid [GT_TRUE], tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E],
        config, configMask and action the same as written - for important fields only

    1.3. Call with configPtr->ipv4Cfg{out of range srcPortTrunk [64],
                                      srcIsTrunk [1]}
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.4. Call with configPtr->ipv4Cfg{out of range srcPortTrunk [128],
                                      srcIsTrunk [1]}
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with configPtr->ipv4Cfg{out of range srcPortTrunk [64],
                                      srcIsTrunk [0]}
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with configPtr->mplsCfg {out of range srcIsTrunk [10]}
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with configPtr->ipv4Cfg{out of range srcDev [32]}
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with configPtr->ipv4Cfg{out of range vid [4096]}
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with wrong enum values actionPtr->command
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.10. Call with actionPtr{command[CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                             wrong enum values userDefinedCpuCode}
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.11. Call with wrong enum values actionPtr->passengerPacketType
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.12. Call with actionPtr->egressInterface{type [CPSS_INTERFACE_PORT_E],
                                               devPort {devNum [dev],
                                                        out of range portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]}
                                               and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.13. Call with actionPtr->egressInterface{type [CPSS_INTERFACE_PORT_E],
                                               devPort {out of range devNum [PRV_CPSS_MAX_PP_DEVICES_CNS],
                                                        portNum [0]}
                                               and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.14. Call with actionPtr->egressInterface{type [CPSS_INTERFACE_TRUNK_E],
                                               out of range trunkId[0xFFFF]}
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.15. Call with actionPtr->egressInterface{type [CPSS_INTERFACE_VID_E],
                                               out of range vlanId[4096]}
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.16. Call with actionPtr->egressInterface{type [CPSS_INTERFACE_VIDX_E],
                                               out of range vidx [4096]}
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.17. Call with out of range actionPtr->vlanId[4096]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.18. Call with wrong enum values actionPtr->vlanPrecedence
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.20. Call with wrong enum values actionPtr->qosMode
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.21. Call with wrong enum values actionPtr->qosPrecedence
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.22. Call with actionPtr{qosMode [CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E],
                              out of range qosProfile [128]}
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.23. Call with out of range actionPtr->defaultUP [8]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.24. Call with actionPtr{policerEnable [GT_TRUE],
                              out of range policerIndex [256]}
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.25. Call with actionPtr{matchCounterEnable [GT_TRUE],
                              matchCounterIndex [32]}
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.26. Call with routerTunnelTermTcamIndex [10],
                    tunnelType [CPSS_TUNNEL_X_OVER_MPLS_E],
                    configPtr->mplsCfg{srcPortTrunk [0],
                                       srcIsTrunk [0],
                                       srcDev [0],
                                       vid [100],
                                       macDa [00:1A:FF:FF:FF:FF],
                                       label1[0],
                                       sBit1[0],
                                       exp1[0],
                                       label2[0],
                                       sBit2[0],
                                       exp2[0]},
                    configMaskPtr-> ipv4Cfg{0xFF,0xFF,...},
                    actionPtr{command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                              userDefinedCpuCode [CPSS_NET_FIRST_USER_DEFINED_E],
                              passengerPacketType [CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E],
                              egressInterface {type [CPSS_INTERFACE_TRUNK_E],
                                               trunkId [0]},
                              isTunnelStart [GT_TRUE],
                              tunnelStartIdx [10],
                              vlanId [100],
                              vlanPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                              nestedVlanEnable [GT_FALSE],
                              copyTtlFromTunnelHeader [GT_FALSE],
                              qosMode [CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E],
                              qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                              qosProfile [127],
                              defaultUP [0],
                              modifyUP [CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E],
                              remapDSCP [GT_FALSE],
                              modifyDSCP [CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E],
                              mirrorToIngressAnalyzerEnable [GT_FALSE],
                              policerEnable [GT_FALSE],
                              policerIndex [0],
                              matchCounterEnable [GT_FALSE],
                              matchCounterIndex [0]}
    Expected: GT_OK.

    1.27. Call cpssDxChTunnelTermEntryGet with non-NULL pointers and tcamIndex [10].
        Expected: GT_OK and  valid [GT_TRUE], tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E],
        config, configMask and action the same as written - for important fields only
    1.28. Call with configPtr->mplsCfg {out of range srcPortTrunk [64],
                                        srcIsTrunk [1]}
                    and other parameters from 1.27.
    Expected: GT_OK.
    1.29. Call with configPtr->mplsCfg {out of range srcPortTrunk [128],
                                        srcIsTrunk [1]}
                    and other parameters from 1.27.
    Expected: NOT GT_OK.
    1.30. Call with configPtr->mplsCfg {out of range srcPortTrunk [64],
                                        srcIsTrunk [0]}
                    and other parameters from 1.26.
    Expected: NOT GT_OK.
    1.31. Call with configPtr->mplsCfg {out of range srcIsTrunk [10]}
                    and other parameters from 1.27.
    Expected: NOT GT_OK.
    1.32. Call with configPtr->mplsCfg {out of range srcDev [32]}
                    and other parameters from 1.27.
    Expected: NOT GT_OK.
    1.33. Call with configPtr->mplsCfg {out of range vid [4096]}
                    and other parameters from 1.27.
    Expected: GT_BAD_PARAM.
    1.34. Call with configPtr->mplsCfg {out of range label1 [1048576]}
                    and other parameters from 1.27.
    Expected: NOT GT_OK.
    1.35. Call with configPtr->mplsCfg {out of range exp1 [8]}
                    and other parameters from 1.27.
    Expected: NOT GT_OK.
    1.36. Call with configPtr->mplsCfg {out of range label2 [1048576]}
                    and other parameters from 1.27.
    Expected: NOT GT_OK.
    1.37. Call with configPtr->mplsCfg {out of range exp2 [8]}
                    and other parameters from 1.27.
    Expected: NOT GT_OK.
    1.38. Call with actionPtr {isTunnelStart[GT_TRUE],
                    out of range tunnelStartIdx[1024]}
                    and other parameters from 1.27.
    Expected: NOT GT_OK.
    1.39. Call with wrong enum values actionPtr->modifyUP
                    and other parameters from 1.27.
    Expected: GT_BAD_PARAM.
    1.40. Call with tunnelType [CPSS_TUNNEL_ETHERNET_OVER_MPLS_E]
                    wrong enum values actionPtr->modifyUP
                    and other parameters from 1.27.
    Expected: GT_BAD_PARAM.
    1.41. Call with configPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.42. Call with configMaskPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.43. Call with actionPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.44. Call with wrong enum values tunnelType
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.45. Call with out of range tunnelType [CPSS_TUNNEL_IP_OVER_X_E]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.46. Call with out of range routerTunnelTermTcamIndex [1024]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.47. Call cpssDxChTunnelTermEntryInvalidate with routerTunnelTermTcamIndex [10].
    Expected: GT_OK.
*/
    GT_STATUS                        st         = GT_OK;

    GT_U8                            dev;
    GT_U32                           tcamIndex  = 0;
    CPSS_TUNNEL_TYPE_ENT             tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;
    CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT config;
    CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT configMask;
    CPSS_DXCH_TUNNEL_TERM_ACTION_STC action;

    CPSS_TUNNEL_TYPE_ENT             tunnelTypeGet;
    CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT configGet;
    CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT configMaskGet;
    CPSS_DXCH_TUNNEL_TERM_ACTION_STC actionGet;
    GT_BOOL                          validGet = GT_FALSE;
    GT_BOOL                          isEqual  = GT_FALSE;


    cpssOsBzero((GT_VOID*) &config, sizeof(config));
    cpssOsBzero((GT_VOID*) &configMask, sizeof(configMask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));
    cpssOsBzero((GT_VOID*) &configGet, sizeof(configGet));
    cpssOsBzero((GT_VOID*) &configMaskGet, sizeof(configMaskGet));
    cpssOsBzero((GT_VOID*) &actionGet, sizeof(actionGet));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with routerTunnelTermTcamIndex [10],
                           tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E],
                           configPtr->ipv4Cfg{srcPortTrunk [0],
                                              srcIsTrunk [0],
                                              srcDev [0],
                                              vid [100],
                                              macDa [00:1A:FF:FF:FF:FF],
                                              srcIp->arIP[10.15.1.250],
                                              destIp->arIP[10.15.1.255]},
                            configMaskPtr-> ipv4Cfg{0xFF,0xFF,...},
                            actionPtr{command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                     userDefinedCpuCode [CPSS_NET_FIRST_USER_DEFINED_E],
                                     passengerPacketType [CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E],
                                     egressInterface { type [CPSS_INTERFACE_TRUNK_E],
                                                       trunkId [0]},
                                     isTunnelStart [GT_FALSE],
                                     tunnelStartIdx [0],
                                     vlanId [100],
                                     vlanPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                     nestedVlanEnable [GT_FALSE],
                                     copyTtlFromTunnelHeader [GT_FALSE],
                                     qosMode [CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E],
                                     qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                     qosProfile [127],
                                     defaultUP [0],
                                     modifyUP [CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E],
                                     remapDSCP [GT_FALSE],
                                     modifyDSCP [CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E],
                                     mirrorToIngressAnalyzerEnable [GT_FALSE],
                                     policerEnable [GT_FALSE],
                                     policerIndex [0],
                                     matchCounterEnable [GT_FALSE],
                                     matchCounterIndex [0]}
            Expected: GT_OK.
        */
        tcamIndex  = 10;
        tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;

        config.ipv4Cfg.srcPortTrunk = 0;
        config.ipv4Cfg.srcIsTrunk = 0;
        config.ipv4Cfg.srcDev = 0;
        config.ipv4Cfg.vid = 100;

        config.ipv4Cfg.macDa.arEther[0] = 0x0;
        config.ipv4Cfg.macDa.arEther[1] = 0x1A;
        config.ipv4Cfg.macDa.arEther[2] = 0xFF;
        config.ipv4Cfg.macDa.arEther[3] = 0xFF;
        config.ipv4Cfg.macDa.arEther[4] = 0xFF;
        config.ipv4Cfg.macDa.arEther[5] = 0xFF;

        config.ipv4Cfg.srcIp.arIP[0] = 10;
        config.ipv4Cfg.srcIp.arIP[1] = 15;
        config.ipv4Cfg.srcIp.arIP[2] = 1;
        config.ipv4Cfg.srcIp.arIP[3] = 250;

        config.ipv4Cfg.destIp.arIP[0] = 10;
        config.ipv4Cfg.destIp.arIP[1] = 15;
        config.ipv4Cfg.destIp.arIP[2] = 1;
        config.ipv4Cfg.destIp.arIP[3] = 255;

        cpssOsBzero((GT_VOID*) &configMask, sizeof(configMask));
        cpssOsMemSet((GT_VOID*) &(configMask), 0xFF, sizeof(configMask));

        action.command             = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        action.userDefinedCpuCode  = CPSS_NET_FIRST_USER_DEFINED_E;
        action.passengerPacketType = CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E;
        action.isTunnelStart       = GT_FALSE;
        action.tunnelStartIdx      = 0;
        action.vlanId              = 100;
        action.vlanPrecedence      = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        action.nestedVlanEnable    = GT_FALSE;
        action.qosMode             = CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E;
        action.qosPrecedence       = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        action.qosProfile          = 127;
        action.defaultUP           = 0;
        action.modifyUP            = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
        action.remapDSCP           = GT_FALSE;
        action.modifyDSCP          = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        action.policerEnable       = GT_FALSE;
        action.policerIndex        = 0;
        action.matchCounterEnable  = GT_FALSE;
        action.matchCounterIndex   = 0;
        action.mirrorToIngressAnalyzerEnable = GT_FALSE;
        action.copyTtlFromTunnelHeader = GT_FALSE;

        action.egressInterface.type    = CPSS_INTERFACE_TRUNK_E;
        action.egressInterface.trunkId = 0;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(action.egressInterface.trunkId);

        configMask.ipv4Cfg.srcPortTrunk = 0x7F;
        configMask.ipv4Cfg.srcIsTrunk = 1;
        configMask.ipv4Cfg.srcDev = 0x1F;
        configMask.ipv4Cfg.vid = 0xFFF;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamIndex, tunnelType);

        /* 1.2. Call cpssDxChTunnelTermEntryGet with non-NULL pointers and tcamIndex [10].
        Expected: GT_OK and  valid [GT_TRUE], tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E],
        config, configMask and action the same as written - for important fields only for action:
        command, userDefinedCpuCode, passengerPacketType, vlanId, vlanPrecedence,
        qosMode, qosPrecedence, qosProfile, defaultUP, mirrorToIngressAnalyzerEnable,
        policerEnable,  matchCounterEnable.*/

        st = cpssDxChTunnelTermEntryGet(dev, tcamIndex, &validGet, &tunnelTypeGet,
                                        &configGet, &configMaskGet, &actionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTunnelTermEntryGet: %d, %d", dev, tcamIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, validGet,
                                     "entry is treated as invalid: %d, %d", dev, tcamIndex);
        if((GT_OK == st) && (GT_TRUE == validGet))
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(tunnelType, tunnelTypeGet,
                                         "get another tunnelType than was set: %d, %d", dev, tcamIndex);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&config.ipv4Cfg, (GT_VOID*)&configGet.ipv4Cfg, sizeof (config.ipv4Cfg)))
                      ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                                         "get another config than was set: %d, %d", dev, tcamIndex);

            /* check configMask fields */
            UTF_VERIFY_EQUAL2_STRING_MAC(configMask.ipv4Cfg.vid, configMaskGet.ipv4Cfg.vid,
                                         "get another configMask.ipv4Cfg.vid than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(configMask.ipv4Cfg.srcPortTrunk, configMaskGet.ipv4Cfg.srcPortTrunk,
                                         "get another configMask.ipv4Cfg.srcPortTrunk than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(configMask.ipv4Cfg.srcIsTrunk, configMaskGet.ipv4Cfg.srcIsTrunk,
                                         "get another configMask.ipv4Cfg.srcIsTrunk than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(configMask.ipv4Cfg.srcDev, configMaskGet.ipv4Cfg.srcDev,
                                         "get another configMask.ipv4Cfg.srcDev than was set: %d, %d", dev, tcamIndex);
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)(&(configMask.ipv4Cfg.macDa)), (GT_VOID*)(&(configMaskGet.ipv4Cfg.macDa)), sizeof (configMask.ipv4Cfg.macDa)))
                      ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                                         "get another macDa than was set: %d, %d", dev, tcamIndex);
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)(&(configMask.ipv4Cfg.srcIp)), (GT_VOID*)(&(configMaskGet.ipv4Cfg.srcIp)), sizeof (configMask.ipv4Cfg.srcIp)))
                      ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                                         "get another srcIp than was set: %d, %d", dev, tcamIndex);
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)(&(configMask.ipv4Cfg.destIp)), (GT_VOID*)(&(configMaskGet.ipv4Cfg.destIp)), sizeof (configMask.ipv4Cfg.destIp)))
                      ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                                         "get another destIp than was set: %d, %d", dev, tcamIndex);

            /* check action fields */
            UTF_VERIFY_EQUAL2_STRING_MAC(action.command, actionGet.command,
                                         "get another actionPtr->command than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.userDefinedCpuCode, actionGet.userDefinedCpuCode,
                                         "get another actionPtr-> than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.passengerPacketType, actionGet.passengerPacketType,
                                         "get another actionPtr->passengerPacketType than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.vlanId, actionGet.vlanId,
                                         "get another actionPtr->vlanId than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.vlanPrecedence, actionGet.vlanPrecedence,
                                         "get another actionPtr->vlanPrecedence than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.qosMode, actionGet.qosMode,
                                         "get another actionPtr->qosMode than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.qosPrecedence, actionGet.qosPrecedence,
                                         "get another actionPtr->qosPrecedence than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.qosProfile, actionGet.qosProfile,
                                         "get another actionPtr->qosProfile than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.defaultUP, actionGet.defaultUP,
                                         "get another actionPtr->defaultUP than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.mirrorToIngressAnalyzerEnable, actionGet.mirrorToIngressAnalyzerEnable,
                                         "get another actionPtr->mirrorToIngressAnalyzerEnable than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.policerEnable, actionGet.policerEnable,
                                         "get another actionPtr->policerEnable than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.matchCounterEnable, actionGet.matchCounterEnable,
                                         "get another actionPtr->matchCounterEnable than was set: %d, %d", dev, tcamIndex);
        }

        /*
            1.3. Call with configPtr->ipv4Cfg{out of range srcPortTrunk [64],
                                              srcIsTrunk [1]}
                           and other parameters from 1.1.
            Expected: GT_OK.
        */

        config.ipv4Cfg.srcIsTrunk   = 1;
        config.ipv4Cfg.srcPortTrunk = 64;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, configPtr->ipv4Cfg.srcIsTrunk = %d, configPtr->ipv4Cfg.srcPortTrunk = %d",
                                     dev, config.ipv4Cfg.srcIsTrunk, config.ipv4Cfg.srcPortTrunk);

        /*
            1.4. Call with configPtr->ipv4Cfg{out of range srcPortTrunk [128],
                                              srcIsTrunk [1]}
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        config.ipv4Cfg.srcPortTrunk = 128;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, configPtr->ipv4Cfg.srcIsTrunk = %d, configPtr->ipv4Cfg.srcPortTrunk = %d",
                                         dev, config.ipv4Cfg.srcIsTrunk, config.ipv4Cfg.srcPortTrunk);

        /*
            1.45. Call with configPtr->ipv4Cfg{out of range srcPortTrunk [64],
                                              srcIsTrunk [0]}
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        config.ipv4Cfg.srcIsTrunk   = 0;
        config.ipv4Cfg.srcPortTrunk = 64;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, configPtr->ipv4Cfg.srcIsTrunk = %d, configPtr->ipv4Cfg.srcPortTrunk = %d",
                                         dev, config.ipv4Cfg.srcIsTrunk, config.ipv4Cfg.srcPortTrunk);

        /*
            1.6. Call with configPtr->mplsCfg {out of range srcIsTrunk [10]}
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        config.ipv4Cfg.srcPortTrunk = 0;
        config.ipv4Cfg.srcIsTrunk   = 10;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->ipv4Cfg.srcIsTrunk = %d",
                                         dev, config.ipv4Cfg.srcIsTrunk);

        /*
            1.7. Call with configPtr->ipv4Cfg{out of range srcDev [32]}
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        config.ipv4Cfg.srcIsTrunk = 0;
        config.ipv4Cfg.srcDev     = 32;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->ipv4Cfg.srcDev = %d",
                                         dev, config.ipv4Cfg.srcDev);

        /*
            1.8. Call with configPtr->ipv4Cfg{out of range vid [4096]}
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        config.ipv4Cfg.srcDev = 0;
        config.ipv4Cfg.vid    = 4096;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, configPtr->ipv4Cfg.vid = %d",
                                     dev, config.ipv4Cfg.vid);

        /*
            1.9. Call with wrong enum values actionPtr->command and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        config.ipv4Cfg.vid = 100;

        UTF_ENUMS_CHECK_MAC(cpssDxChTunnelTermEntrySet
                            (dev, tcamIndex, tunnelType, &config, &configMask, &action),
                            action.command);

        /*
            1.10. Call with actionPtr{command[CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                                     wrong enum values userDefinedCpuCode}
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        action.command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChTunnelTermEntrySet
                            (dev, tcamIndex, tunnelType, &config, &configMask, &action),
                            action.userDefinedCpuCode);

        /*
            1.11. Call with wrong enum values actionPtr->passengerPacketType
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        action.command             = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        action.userDefinedCpuCode  = CPSS_NET_FIRST_USER_DEFINED_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChTunnelTermEntrySet
                            (dev, tcamIndex, tunnelType, &config, &configMask, &action),
                            action.passengerPacketType);

        /*
            1.12. Call with actionPtr->egressInterface{type [CPSS_INTERFACE_PORT_E],
                                                       devPort {devNum [dev],
                                                                out of range portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]}
                                                       and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        action.passengerPacketType = CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E;

        action.egressInterface.type            = CPSS_INTERFACE_PORT_E;
        action.egressInterface.devPort.devNum  = dev;
        action.egressInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, actionPtr->egressInterface.type= %d, actionPtr->egressInterface.devPort.portNum = %d",
                                         dev, action.egressInterface.type, action.egressInterface.devPort.portNum);

        /*
            1.13. Call with actionPtr->egressInterface{type [CPSS_INTERFACE_PORT_E],
                                                       devPort {out of range devNum [PRV_CPSS_MAX_PP_DEVICES_CNS],
                                                                portNum [0]}
                                                       and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        action.egressInterface.devPort.devNum  = PRV_CPSS_MAX_PP_DEVICES_CNS;
        action.egressInterface.devPort.portNum = 0;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, actionPtr->egressInterface.type= %d, actionPtr->egressInterface.devPort.devNum = %d",
                                         dev, action.egressInterface.type, action.egressInterface.devPort.devNum);

        /*
            1.14. Call with actionPtr->egressInterface{type [CPSS_INTERFACE_TRUNK_E],
                                                       out of range trunkId[0xFFFF]}
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        action.egressInterface.devPort.devNum  = dev;

        action.egressInterface.type    = CPSS_INTERFACE_TRUNK_E;
        action.egressInterface.trunkId = 0xFFFF;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(action.egressInterface.trunkId);

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, actionPtr->egressInterface.type= %d, actionPtr->egressInterface.trunkId = %d",
                                         dev, action.egressInterface.type, action.egressInterface.trunkId);

        /*
            1.15. Call with actionPtr->egressInterface{type [CPSS_INTERFACE_VID_E],
                                                       out of range vlanId[4096]}
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        action.egressInterface.trunkId = 0;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(action.egressInterface.trunkId);
        action.egressInterface.type    = CPSS_INTERFACE_VID_E;
        action.egressInterface.vlanId  = 4096;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, actionPtr->egressInterface.type= %d, actionPtr->egressInterface.vlanId = %d",
                                     dev, action.egressInterface.type, action.egressInterface.vlanId);

        /*
            1.16. Call with actionPtr->egressInterface{type [CPSS_INTERFACE_VIDX_E],
                                                       out of range vidx [4096]}
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        action.egressInterface.vlanId = 100;
        action.egressInterface.type   = CPSS_INTERFACE_VIDX_E;
        action.egressInterface.vidx   = 4096;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, actionPtr->egressInterface.type= %d, actionPtr->egressInterface.vidx = %d",
                                         dev, action.egressInterface.type, action.egressInterface.vidx);

        /*
            1.17. Call with out of range actionPtr->vlanId[4096]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        action.egressInterface.type = CPSS_INTERFACE_TRUNK_E;
        action.egressInterface.vidx = 0;

        action.vlanId = 4096;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, actionPtr->vlanId = %d",
                                     dev, action.vlanId);

        /*
            1.18. Call with wrong enum values actionPtr->vlanPrecedence
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        action.vlanId         = 100;

        UTF_ENUMS_CHECK_MAC(cpssDxChTunnelTermEntrySet
                            (dev, tcamIndex, tunnelType, &config, &configMask, &action),
                            action.vlanPrecedence);

        /*
            1.19. Call with wrong enum values actionPtr->qosMode and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        action.vlanPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChTunnelTermEntrySet
                            (dev, tcamIndex, tunnelType, &config, &configMask, &action),
                            action.qosMode);

        /*
            1.20. Call with wrong enum values actionPtr->qosPrecedence
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        action.qosMode = CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChTunnelTermEntrySet
                            (dev, tcamIndex, tunnelType, &config, &configMask, &action),
                            action.qosPrecedence);

        /*
            1.21. Call with actionPtr{qosMode [CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E],
                                      out of range qosProfile [128]}
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        action.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        action.qosProfile    = 128;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, actionPtr->qosMode = %d, actionPtr->qosProfile = %d",
                                         dev, action.qosMode, action.qosProfile);

        /*
            1.22. Call with out of range actionPtr->defaultUP [8]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        action.qosProfile = 127;
        action.defaultUP  = 8;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, actionPtr->defaultUP = %d",
                                         dev, action.defaultUP);

        /*
            1.23. Call with actionPtr{policerEnable [GT_TRUE],
                                      out of range policerIndex [256]}
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        action.defaultUP     = 0;
        action.policerEnable = GT_TRUE;
        action.policerIndex  = 256;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, actionPtr->policerEnable = %d, actionPtr->policerIndex = %d",
                                         dev, action.policerEnable, action.policerIndex);

        /*
            1.24. Call with actionPtr{matchCounterEnable [GT_TRUE],
                                      matchCounterIndex [32]}
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        action.policerEnable = GT_FALSE;
        action.policerIndex  = 0;

        action.matchCounterEnable = GT_TRUE;
        action.matchCounterIndex  = 32;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, actionPtr->matchCounterEnable = %d, actionPtr->matchCounterIndex = %d",
                                         dev, action.matchCounterEnable, action.matchCounterIndex);

        /*
            1.25. Call with routerTunnelTermTcamIndex [10],
                            tunnelType [CPSS_TUNNEL_X_OVER_MPLS_E],
                            configPtr->mplsCfg{srcPortTrunk [0],
                                               srcIsTrunk [0],
                                               srcDev [0],
                                               vid [100],
                                               macDa [00:1A:FF:FF:FF:FF],
                                               label1[0],
                                               sBit1[0],
                                               exp1[0],
                                               label2[0],
                                               sBit2[0],
                                               exp2[0]},
                            configMaskPtr-> ipv4Cfg{0xFF,0xFF,...},
                            actionPtr{command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                      userDefinedCpuCode [CPSS_NET_FIRST_USER_DEFINED_E],
                                      passengerPacketType [CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E],
                                      egressInterface {type [CPSS_INTERFACE_TRUNK_E],
                                                       trunkId [0]},
                                      isTunnelStart [GT_TRUE],
                                      tunnelStartIdx [10],
                                      vlanId [100],
                                      vlanPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                      nestedVlanEnable [GT_FALSE],
                                      copyTtlFromTunnelHeader [GT_FALSE],
                                      qosMode [CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E],
                                      qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                      qosProfile [127],
                                      defaultUP [0],
                                      modifyUP [CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E],
                                      remapDSCP [GT_FALSE],
                                      modifyDSCP [CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E],
                                      mirrorToIngressAnalyzerEnable [GT_FALSE],
                                      policerEnable [GT_FALSE],
                                      policerIndex [0],
                                      matchCounterEnable [GT_FALSE],
                                      matchCounterIndex [0]}
            Expected: GT_OK.
        */
        tcamIndex  = 10;
        tunnelType = CPSS_TUNNEL_IP_OVER_MPLS_E;

        config.mplsCfg.srcPortTrunk = 0;
        config.mplsCfg.srcIsTrunk   = 0;
        config.mplsCfg.srcDev       = 0;
        config.mplsCfg.vid          = 100;

        config.mplsCfg.macDa.arEther[0] = 0x0;
        config.mplsCfg.macDa.arEther[1] = 0x1A;
        config.mplsCfg.macDa.arEther[2] = 0xFF;
        config.mplsCfg.macDa.arEther[3] = 0xFF;
        config.mplsCfg.macDa.arEther[4] = 0xFF;
        config.mplsCfg.macDa.arEther[5] = 0xFF;

        config.mplsCfg.label1 = 0;
        config.mplsCfg.sBit1  = 0;
        config.mplsCfg.exp1   = 0;
        config.mplsCfg.label2 = 0;
        config.mplsCfg.sBit2  = 0;
        config.mplsCfg.exp2   = 0;

        action.command             = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        action.userDefinedCpuCode  = CPSS_NET_FIRST_USER_DEFINED_E;
        action.passengerPacketType = CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E;
        action.isTunnelStart       = GT_TRUE;
        action.tunnelStartIdx      = 10;
        action.vlanId              = 100;
        action.vlanPrecedence      = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        action.nestedVlanEnable    = GT_FALSE;
        action.qosMode             = CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E;
        action.qosPrecedence       = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        action.qosProfile          = 127;
        action.defaultUP           = 0;
        action.modifyUP            = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
        action.remapDSCP           = GT_FALSE;
        action.modifyDSCP          = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        action.policerEnable       = GT_FALSE;
        action.policerIndex        = 0;
        action.matchCounterEnable  = GT_FALSE;
        action.matchCounterIndex   = 0;
        action.mirrorToIngressAnalyzerEnable = GT_FALSE;
        action.copyTtlFromTunnelHeader = GT_FALSE;

        action.egressInterface.type    = CPSS_INTERFACE_TRUNK_E;
        action.egressInterface.trunkId = 0;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(action.egressInterface.trunkId);

        configMask.mplsCfg.exp1 = 0x7;
        configMask.mplsCfg.exp2 = 0x7;
        configMask.mplsCfg.sBit1 = 0x1;
        configMask.mplsCfg.sBit2 = 0x1;
        configMask.mplsCfg.label1 = 0xfffff;
        configMask.mplsCfg.label2 = 0xfffff;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamIndex, tunnelType);

        /* 1.26. Call cpssDxChTunnelTermEntryGet with non-NULL pointers and tcamIndex [10].
        Expected: GT_OK and  valid [GT_TRUE], tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E],
        config, configMask and action the same as written - for important fields only:
        for action:
        command, userDefinedCpuCode, passengerPacketType, vlanId, vlanPrecedence,
        qosMode, qosPrecedence, qosProfile, defaultUP, mirrorToIngressAnalyzerEnable,
        policerEnable,  matchCounterEnable.*/

        st = cpssDxChTunnelTermEntryGet(dev, tcamIndex, &validGet, &tunnelTypeGet,
                                        &configGet, &configMaskGet, &actionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTunnelTermEntryGet: %d, %d", dev, tcamIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, validGet,
                                     "entry is treated as invalid: %d, %d", dev, tcamIndex);
        if(GT_TRUE == validGet)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(tunnelType, tunnelTypeGet,
                                         "get another tunnelType than was set: %d, %d", dev, tcamIndex);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&config.mplsCfg, (GT_VOID*)&configGet.mplsCfg, sizeof (config.mplsCfg)))
                      ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                                         "get another config than was set: %d, %d", dev, tcamIndex);

            /* check configMask fields */
            UTF_VERIFY_EQUAL2_STRING_MAC(configMask.mplsCfg.vid, configMaskGet.mplsCfg.vid,
                                         "get another configMask.mplsCfg.vid than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(configMask.mplsCfg.srcPortTrunk, configMaskGet.mplsCfg.srcPortTrunk,
                                         "get another configMask.mplsCfg.srcPortTrunk than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(configMask.mplsCfg.srcIsTrunk, configMaskGet.mplsCfg.srcIsTrunk,
                                         "get another configMask.mplsCfg.srcIsTrunk than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(configMask.mplsCfg.srcDev, configMaskGet.ipv4Cfg.srcDev,
                                         "get another configMask.mplsCfg.srcDev than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(configMask.mplsCfg.exp1, configMaskGet.mplsCfg.exp1,
                                         "get another configMask.mplsCfg.exp1 than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(configMask.mplsCfg.exp2, configMaskGet.mplsCfg.exp2,
                                         "get another configMask.mplsCfg.exp2 than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(configMask.mplsCfg.label1, configMaskGet.mplsCfg.label1,
                                         "get another configMask.mplsCfg.label1 than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(configMask.mplsCfg.label2, configMaskGet.mplsCfg.label2,
                                         "get another configMask.mplsCfg.label2 than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(configMask.mplsCfg.sBit1, configMaskGet.mplsCfg.sBit1,
                                         "get another configMask.mplsCfg.sBit1 than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(configMask.mplsCfg.sBit2, configMaskGet.mplsCfg.sBit2,
                                         "get another configMask.mplsCfg.sBit2 than was set: %d, %d", dev, tcamIndex);
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)(&(configMask.mplsCfg.macDa)), (GT_VOID*)(&(configMaskGet.mplsCfg.macDa)), sizeof (configMask.mplsCfg.macDa)))
                      ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                                         "get another macDa than was set: %d, %d", dev, tcamIndex);

            /* check action fields */
            UTF_VERIFY_EQUAL2_STRING_MAC(action.command, actionGet.command,
                                         "get another actionPtr->command than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.userDefinedCpuCode, actionGet.userDefinedCpuCode,
                                         "get another actionPtr-> than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.passengerPacketType, actionGet.passengerPacketType,
                                         "get another actionPtr->passengerPacketType than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.vlanId, actionGet.vlanId,
                                         "get another actionPtr->vlanId than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.vlanPrecedence, actionGet.vlanPrecedence,
                                         "get another actionPtr->vlanPrecedence than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.qosMode, actionGet.qosMode,
                                         "get another actionPtr->qosMode than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.qosPrecedence, actionGet.qosPrecedence,
                                         "get another actionPtr->qosPrecedence than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.qosProfile, actionGet.qosProfile,
                                         "get another actionPtr->qosProfile than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.defaultUP, actionGet.defaultUP,
                                         "get another actionPtr->defaultUP than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.mirrorToIngressAnalyzerEnable, actionGet.mirrorToIngressAnalyzerEnable,
                                         "get another actionPtr->mirrorToIngressAnalyzerEnable than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.policerEnable, actionGet.policerEnable,
                                         "get another actionPtr->policerEnable than was set: %d, %d", dev, tcamIndex);
            UTF_VERIFY_EQUAL2_STRING_MAC(action.matchCounterEnable, actionGet.matchCounterEnable,
                                         "get another actionPtr->matchCounterEnable than was set: %d, %d", dev, tcamIndex);
        }

        /*
            1.27. Call with configPtr->mplsCfg {out of range srcPortTrunk [64],
                                                srcIsTrunk [1]}
                            and other parameters from 1.27.
            Expected: GT_OK.
        */

        config.mplsCfg.srcIsTrunk   = 1;
        config.mplsCfg.srcPortTrunk = 64;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.srcIsTrunk = %d, configPtr->mplsCfg.srcPortTrunk = %d",
                                     dev, config.mplsCfg.srcIsTrunk, config.mplsCfg.srcPortTrunk);

        /*
            1.28. Call with configPtr->mplsCfg {out of range srcPortTrunk [128],
                                                srcIsTrunk [1]}
                            and other parameters from 1.27.
            Expected: NOT GT_OK.
        */
        config.mplsCfg.srcPortTrunk = 128;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.srcIsTrunk = %d, configPtr->mplsCfg.srcPortTrunk = %d",
                                         dev, config.mplsCfg.srcIsTrunk, config.mplsCfg.srcPortTrunk);

        /*
            1.29. Call with configPtr->mplsCfg {out of range srcPortTrunk [64],
                                                srcIsTrunk [0]}
                            and other parameters from 1.26.
            Expected: NOT GT_OK.
        */
        config.mplsCfg.srcIsTrunk   = 0;
        config.mplsCfg.srcPortTrunk = 64;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.srcIsTrunk = %d, configPtr->mplsCfg.srcPortTrunk = %d",
                                         dev, config.mplsCfg.srcIsTrunk, config.mplsCfg.srcPortTrunk);

        /*
            1.30. Call with configPtr->mplsCfg {out of range srcIsTrunk [10]}
                            and other parameters from 1.27.
            Expected: NOT GT_OK.
        */
        config.mplsCfg.srcPortTrunk = 0;
        config.mplsCfg.srcIsTrunk   = 10;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.srcIsTrunk = %d",
                                         dev, config.mplsCfg.srcIsTrunk);

        /*
            1.31. Call with configPtr->mplsCfg {out of range srcDev [32]}
                            and other parameters from 1.27.
            Expected: NOT GT_OK.
        */
        config.mplsCfg.srcIsTrunk = 0;
        config.mplsCfg.srcDev     = 32;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.srcDev = %d",
                                         dev, config.mplsCfg.srcDev);

        /*
            1.32. Call with configPtr->mplsCfg {out of range vid [4096]}
                            and other parameters from 1.27.
            Expected: GT_BAD_PARAM.
        */
        config.mplsCfg.srcDev = 0;
        config.mplsCfg.vid    = 4096;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, configPtr->mplsCfg.vid = %d",
                                     dev, config.mplsCfg.vid);

        /*
            1.33. Call with configPtr->mplsCfg {out of range label1 [1048576]}
                            and other parameters from 1.27.
            Expected: NOT GT_OK.
        */
        config.mplsCfg.vid    = 100;
        config.mplsCfg.label1 = 1048576;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.label1 = %d",
                                         dev, config.mplsCfg.label1);

        /*
            1.34. Call with configPtr->mplsCfg {out of range exp1 [8]}
                            and other parameters from 1.27.
            Expected: NOT GT_OK.
        */
        config.mplsCfg.label1 = 0;
        config.mplsCfg.exp1   = 8;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.exp1 = %d",
                                         dev, config.mplsCfg.exp1);

        /*
            1.35. Call with configPtr->mplsCfg {out of range label2 [1048576]}
                            and other parameters from 1.27.
            Expected: NOT GT_OK.
        */
        config.mplsCfg.exp1   = 0;
        config.mplsCfg.label2 = 1048576;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.label2 = %d",
                                         dev, config.mplsCfg.label2);

        /*
            1.36. Call with configPtr->mplsCfg {out of range exp2 [8]}
                            and other parameters from 1.27.
            Expected: NOT GT_OK.
        */
        config.mplsCfg.label2 = 0;
        config.mplsCfg.exp2   = 8;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.exp2 = %d",
                                         dev, config.mplsCfg.exp2);

        /*
            1.37. Call with actionPtr {isTunnelStart[GT_TRUE],
                            out of range tunnelStartIdx[1024]}
                            and other parameters from 1.27.
            Expected: NOT GT_OK.


        */
        config.mplsCfg.exp2 = 0;

        action.isTunnelStart  = GT_TRUE;
        action.tunnelStartIdx = 1024;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, actionPtr->tunnelStartIdx = %d",
                                         dev, action.tunnelStartIdx);

        /*
            1.38. Call with wrong enum values actionPtr->modifyUP and other parameters from 1.27.
            Expected: GT_BAD_PARAM.
        */
        action.tunnelStartIdx = 10;

        UTF_ENUMS_CHECK_MAC(cpssDxChTunnelTermEntrySet
                            (dev, tcamIndex, tunnelType, &config, &configMask, &action),
                            action.modifyUP);

        /*
            1.39. Call with tunnelType [CPSS_TUNNEL_ETHERNET_OVER_MPLS_E]
                            wrong enum values actionPtr->modifyDSCP
                            and other parameters from 1.27.
            Expected: GT_BAD_PARAM.
        */
        action.modifyUP   = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
        tunnelType        = CPSS_TUNNEL_ETHERNET_OVER_MPLS_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChTunnelTermEntrySet
                            (dev, tcamIndex, tunnelType, &config, &configMask, &action),
                            action.modifyDSCP);

        /*
            1.40. Call with configPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        tunnelType        = CPSS_TUNNEL_X_OVER_MPLS_E;
        action.modifyDSCP = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        NULL, &configMask, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configPtr = NULL", dev);

        /*
            1.41. Call with configMaskPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, NULL, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configMaskPtr = NULL", dev);

        /*
            1.42. Call with actionPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);

        /*
            1.43. Call with wrong enum values tunnelType and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTunnelTermEntrySet
                            (dev, tcamIndex, tunnelType, &config, &configMask, &action),
                            tunnelType);

        /*
            1.44. Call with out of range tunnelType [CPSS_TUNNEL_IP_OVER_X_E]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        tunnelType = CPSS_TUNNEL_IP_OVER_X_E;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d", dev, tunnelType);

        /*
            1.45. Call with out of range routerTunnelTermTcamIndex [1024]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        tunnelType = CPSS_TUNNEL_X_OVER_MPLS_E;
        tcamIndex  = 1024;

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routerTunnelTermTcamIndex = %d", dev, tcamIndex);

        /* 1.46. Call cpssDxChTunnelTermEntryInvalidate with routerTunnelTermTcamIndex [10].
        Expected: GT_OK.*/
        tcamIndex = 10;

        st = cpssDxChTunnelTermEntryInvalidate(dev, tcamIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTunnelTermEntryInvalidate: %d, %d", dev, tcamIndex);
    }

    tcamIndex  = 10;
    tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;

    config.ipv4Cfg.srcPortTrunk = 0;
    config.ipv4Cfg.srcIsTrunk   = 0;
    config.ipv4Cfg.srcDev       = 0;
    config.ipv4Cfg.vid          = 100;

    config.ipv4Cfg.macDa.arEther[0] = 0x0;
    config.ipv4Cfg.macDa.arEther[1] = 0x1A;
    config.ipv4Cfg.macDa.arEther[2] = 0xFF;
    config.ipv4Cfg.macDa.arEther[3] = 0xFF;
    config.ipv4Cfg.macDa.arEther[4] = 0xFF;
    config.ipv4Cfg.macDa.arEther[5] = 0xFF;

    config.ipv4Cfg.srcIp.arIP[0] = 10;
    config.ipv4Cfg.srcIp.arIP[1] = 15;
    config.ipv4Cfg.srcIp.arIP[2] = 1;
    config.ipv4Cfg.srcIp.arIP[3] = 250;

    config.ipv4Cfg.destIp.arIP[0] = 10;
    config.ipv4Cfg.destIp.arIP[1] = 15;
    config.ipv4Cfg.destIp.arIP[2] = 1;
    config.ipv4Cfg.destIp.arIP[3] = 255;

    cpssOsBzero((GT_VOID*) &configMask, sizeof(configMask));
    cpssOsMemSet((GT_VOID*) &(configMask), 0xFF, sizeof(configMask));

    action.command             = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
    action.userDefinedCpuCode  = CPSS_NET_FIRST_USER_DEFINED_E;
    action.passengerPacketType = CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E;
    action.isTunnelStart       = GT_FALSE;
    action.tunnelStartIdx      = 0;
    action.vlanId              = 100;
    action.vlanPrecedence      = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    action.nestedVlanEnable    = GT_FALSE;
    action.qosMode             = CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E;
    action.qosPrecedence       = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    action.qosProfile          = 127;
    action.defaultUP           = 0;
    action.modifyUP            = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
    action.remapDSCP           = GT_FALSE;
    action.modifyDSCP          = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
    action.policerEnable       = GT_FALSE;
    action.policerIndex        = 0;
    action.matchCounterEnable  = GT_FALSE;
    action.matchCounterIndex   = 0;
    action.mirrorToIngressAnalyzerEnable = GT_FALSE;
    action.copyTtlFromTunnelHeader = GT_FALSE;

    action.egressInterface.type    = CPSS_INTERFACE_TRUNK_E;
    action.egressInterface.trunkId = 0;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(action.egressInterface.trunkId);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                    &config, &configMask, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelTermEntryGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTunnelTermTcamIndex,
    OUT GT_BOOL                             *validPtr,
    OUT CPSS_TUNNEL_TYPE_ENT                *tunnelTypePtr,
    OUT CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configPtr,
    OUT CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    *configMaskPtr,
    OUT CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelTermEntryGet)
{
/*
    ITERATE_DEVICES (DxCh1, DxCh2)
    1.1. Call with routerTunnelTermTcamIndex [1],
                           tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E],
                           configPtr->ipv4Cfg{srcPortTrunk [0],
                                              srcIsTrunk [0],
                                              srcDev [0],
                                              vid [100],
                                              macDa [00:1A:FF:FF:FF:FF],
                                              srcIp->arIP[10.15.1.250],
                                              destIp->arIP[10.15.1.255]},
                            configMaskPtr-> ipv4Cfg{0xFF,0xFF,...},
                            actionPtr{command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                     userDefinedCpuCode [CPSS_NET_FIRST_USER_DEFINED_E],
                                     passengerPacketType [CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E],
                                     egressInterface { type [CPSS_INTERFACE_TRUNK_E],
                                                       trunkId [0]},
                                     isTunnelStart [GT_FALSE],
                                     tunnelStartIdx [0],
                                     vlanId [100],
                                     vlanPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                     nestedVlanEnable [GT_FALSE],
                                     copyTtlFromTunnelHeader [GT_FALSE],
                                     qosMode [CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E],
                                     qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                     qosProfile [127],
                                     defaultUP [0],
                                     modifyUP [CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E],
                                     remapDSCP [GT_FALSE],
                                     modifyDSCP [CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E],
                                     mirrorToIngressAnalyzerEnable [GT_FALSE],
                                     policerEnable [GT_FALSE],
                                     policerIndex [0],
                                     matchCounterEnable [GT_FALSE],
                                     matchCounterIndex [0]}
            Expected: GT_OK.
    1.2. Call with routerTunnelTermTcamIndex [1],
                   non-null validPtr,
                   non-null tunnelTypePtr,
                   non-null configPtr,
                   non-null configMaskPtr
                   and non-null actionPtr.
    Expected: GT_OK.
    1.3. Call with out of range routerTunnelTermTcamIndex [1024],
                   non-null validPtr,
                   non-null tunnelTypePtr,
                   non-null configPtr,
                   non-null configMaskPtr
                   and non-null actionPtr.
    Expected: NOT GT_OK.
    1.4. Call with routerTunnelTermTcamIndex [1],
                   validPtr [NULL],
                   non-null tunnelTypePtr,
                   non-null configPtr,
                   non-null configMaskPtr
                   and non-null actionPtr.
    Expected: GT_BAD_PTR.
    1.5. Call with routerTunnelTermTcamIndex [1],
                   non-null validPtr,
                   tunnelTypePtr [NULL],
                   non-null configPtr,
                   non-null configMaskPtr
                   and non-null actionPtr.
    Expected: GT_BAD_PTR.
    1.6. Call with routerTunnelTermTcamIndex [1],
                   non-null validPtr,
                   non-null tunnelTypePtr,
                   configPtr [NULL],
                   non-null configMaskPtr
                   and non-null actionPtr.
    Expected: GT_BAD_PTR.
    1.7. Call with routerTunnelTermTcamIndex [1],
                   non-null validPtr,
                   non-null tunnelTypePtr,
                   non-null configPtr,
                   configMaskPtr [NULL]
                   and non-null actionPtr.
    Expected: GT_BAD_PTR.
    1.8. Call with routerTunnelTermTcamIndex [1],
                   non-null validPtr,
                   non-null tunnelTypePtr,
                   non-null configPtr,
                   non-null configMaskPtr
                   and actionPtr [NULL].
    Expected: GT_BAD_PTR.
    1.9. Call cpssDxChTunnelTermEntryInvalidate with routerTunnelTermTcamIndex [1].
        Expected: GT_OK.
*/
    GT_STATUS                        st         = GT_OK;

    GT_U8                            dev;
    GT_U32                           tcamIndex  = 0;
    GT_BOOL                          valid      = GT_FALSE;
    CPSS_TUNNEL_TYPE_ENT             tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;
    CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT config;
    CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT configMask;
    CPSS_DXCH_TUNNEL_TERM_ACTION_STC action;


    cpssOsBzero((GT_VOID*) &config, sizeof(config));
    cpssOsBzero((GT_VOID*) &configMask, sizeof(configMask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with routerTunnelTermTcamIndex [1],
                           tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E],
                           configPtr->ipv4Cfg{srcPortTrunk [0],
                                              srcIsTrunk [0],
                                              srcDev [0],
                                              vid [100],
                                              macDa [00:1A:FF:FF:FF:FF],
                                              srcIp->arIP[10.15.1.250],
                                              destIp->arIP[10.15.1.255]},
                            configMaskPtr-> ipv4Cfg{0xFF,0xFF,...},
                            actionPtr{command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                     userDefinedCpuCode [CPSS_NET_FIRST_USER_DEFINED_E],
                                     passengerPacketType [CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E],
                                     egressInterface { type [CPSS_INTERFACE_TRUNK_E],
                                                       trunkId [0]},
                                     isTunnelStart [GT_FALSE],
                                     tunnelStartIdx [0],
                                     vlanId [100],
                                     vlanPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                     nestedVlanEnable [GT_FALSE],
                                     copyTtlFromTunnelHeader [GT_FALSE],
                                     qosMode [CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E],
                                     qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                     qosProfile [127],
                                     defaultUP [0],
                                     modifyUP [CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E],
                                     remapDSCP [GT_FALSE],
                                     modifyDSCP [CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E],
                                     mirrorToIngressAnalyzerEnable [GT_FALSE],
                                     policerEnable [GT_FALSE],
                                     policerIndex [0],
                                     matchCounterEnable [GT_FALSE],
                                     matchCounterIndex [0]}
            Expected: GT_OK.
        */
        tcamIndex  = 1;
        tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;

        config.ipv4Cfg.srcPortTrunk = 0;
        config.ipv4Cfg.srcIsTrunk = 0;
        config.ipv4Cfg.srcDev = 0;
        config.ipv4Cfg.vid = 100;

        config.ipv4Cfg.macDa.arEther[0] = 0x0;
        config.ipv4Cfg.macDa.arEther[1] = 0x1A;
        config.ipv4Cfg.macDa.arEther[2] = 0xFF;
        config.ipv4Cfg.macDa.arEther[3] = 0xFF;
        config.ipv4Cfg.macDa.arEther[4] = 0xFF;
        config.ipv4Cfg.macDa.arEther[5] = 0xFF;

        config.ipv4Cfg.srcIp.arIP[0] = 10;
        config.ipv4Cfg.srcIp.arIP[1] = 15;
        config.ipv4Cfg.srcIp.arIP[2] = 1;
        config.ipv4Cfg.srcIp.arIP[3] = 250;

        config.ipv4Cfg.destIp.arIP[0] = 10;
        config.ipv4Cfg.destIp.arIP[1] = 15;
        config.ipv4Cfg.destIp.arIP[2] = 1;
        config.ipv4Cfg.destIp.arIP[3] = 255;

        cpssOsBzero((GT_VOID*) &configMask, sizeof(configMask));
        cpssOsMemSet((GT_VOID*) &(configMask), 0xFF, sizeof(configMask));

        action.command             = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
        action.userDefinedCpuCode  = CPSS_NET_FIRST_USER_DEFINED_E;
        action.passengerPacketType = CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E;
        action.isTunnelStart       = GT_FALSE;
        action.tunnelStartIdx      = 0;
        action.vlanId              = 100;
        action.vlanPrecedence      = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        action.nestedVlanEnable    = GT_FALSE;
        action.qosMode             = CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E;
        action.qosPrecedence       = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
        action.qosProfile          = 127;
        action.defaultUP           = 0;
        action.modifyUP            = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
        action.remapDSCP           = GT_FALSE;
        action.modifyDSCP          = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        action.policerEnable       = GT_FALSE;
        action.policerIndex        = 0;
        action.matchCounterEnable  = GT_FALSE;
        action.matchCounterIndex   = 0;
        action.mirrorToIngressAnalyzerEnable = GT_FALSE;
        action.copyTtlFromTunnelHeader = GT_FALSE;

        action.egressInterface.type    = CPSS_INTERFACE_TRUNK_E;
        action.egressInterface.trunkId = 0;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(action.egressInterface.trunkId);

        st = cpssDxChTunnelTermEntrySet(dev, tcamIndex, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcamIndex, tunnelType);

        /*
            1.2. Call with routerTunnelTermTcamIndex [1],
                           non-null validPtr,
                           non-null tunnelTypePtr,
                           non-null configPtr,
                           non-null configMaskPtr
                           and non-null actionPtr.
            Expected: GT_OK.
        */
        tcamIndex = 1;

        st = cpssDxChTunnelTermEntryGet(dev, tcamIndex, &valid, &tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamIndex);

        /*
            1.3. Call with out of range routerTunnelTermTcamIndex [1024],
                           non-null validPtr,
                           non-null tunnelTypePtr,
                           non-null configPtr,
                           non-null configMaskPtr
                           and non-null actionPtr.
            Expected: NOT GT_OK.
        */
        tcamIndex = 1024;

        st = cpssDxChTunnelTermEntryGet(dev, tcamIndex, &valid, &tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamIndex);

        /*
            1.4. Call with routerTunnelTermTcamIndex [1],
                           validPtr [NULL],
                           non-null tunnelTypePtr,
                           non-null configPtr,
                           non-null configMaskPtr
                           and non-null actionPtr.
            Expected: GT_BAD_PTR.
        */
        tcamIndex = 1;

        st = cpssDxChTunnelTermEntryGet(dev, tcamIndex, NULL, &tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);

        /*
            1.5. Call with routerTunnelTermTcamIndex [1],
                           non-null validPtr,
                           tunnelTypePtr [NULL],
                           non-null configPtr,
                           non-null configMaskPtr
                           and non-null actionPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTunnelTermEntryGet(dev, tcamIndex, &valid, NULL,
                                        &config, &configMask, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tunnelTypePtr = NULL", dev);

        /*
            1.6. Call with routerTunnelTermTcamIndex [1],
                           non-null validPtr,
                           non-null tunnelTypePtr,
                           configPtr [NULL],
                           non-null configMaskPtr
                           and non-null actionPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTunnelTermEntryGet(dev, tcamIndex, &valid, &tunnelType,
                                        NULL, &configMask, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configPtr = NULL", dev);

        /*
            1.7. Call with routerTunnelTermTcamIndex [1],
                           non-null validPtr,
                           non-null tunnelTypePtr,
                           non-null configPtr,
                           configMaskPtr [NULL]
                           and non-null actionPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTunnelTermEntryGet(dev, tcamIndex, &valid, &tunnelType,
                                        &config, NULL, &action);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configMaskPtr = NULL", dev);

        /*
            1.8. Call with routerTunnelTermTcamIndex [1],
                           non-null validPtr,
                           non-null tunnelTypePtr,
                           non-null configPtr,
                           non-null configMaskPtr
                           and actionPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTunnelTermEntryGet(dev, tcamIndex, &valid, &tunnelType,
                                        &config, &configMask, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, actionPtr = NULL", dev);

        /* 1.9. Call cpssDxChTunnelTermEntryInvalidate with routerTunnelTermTcamIndex [1].
        Expected: GT_OK.  */

        st = cpssDxChTunnelTermEntryInvalidate(dev, tcamIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTunnelTermEntryInvalidate: %d, %d", dev, tcamIndex);
    }

    tcamIndex = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelTermEntryGet(dev, tcamIndex, &valid, &tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelTermEntryGet(dev, tcamIndex, &valid, &tunnelType,
                                    &config, &configMask, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelTermEntryInvalidate
(
    IN  GT_U8       devNum,
    IN  GT_U32      routerTunnelTermTcamIndex
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelTermEntryInvalidate)
{
/*
    ITERATE_DEVICES (DxCh1, DxCh2)
    1.1. Call with routerTunnelTermTcamIndex [100].
    Expected: GT_OK.
    1.2. Call with out of range routerTunnelTermTcamIndex [1024].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev;
    GT_U32    tcamIndex = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with routerTunnelTermTcamIndex [100].
            Expected: GT_OK.
        */
        tcamIndex = 100;

        st = cpssDxChTunnelTermEntryInvalidate(dev, tcamIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamIndex);

        /*
            1.2. Call with out of range routerTunnelTermTcamIndex [1024].
            Expected: NOT GT_OK.
        */
        tcamIndex = 1024;

        st = cpssDxChTunnelTermEntryInvalidate(dev, tcamIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcamIndex);
    }

    tcamIndex = 100;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelTermEntryInvalidate(dev, tcamIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelTermEntryInvalidate(dev, tcamIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelStartEgressFilteringSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelStartEgressFilteringSet)
{
/*
    ITERATE_DEVICES (DxCh1, DxCh2)
    1.1. Call with enable [GT_TRUE and GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssDxChTunnelStartEgressFilteringGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS st       = GT_OK;

    GT_U8     dev;
    GT_BOOL   state    = GT_TRUE;
    GT_BOOL   stateGet = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_TRUE and GT_FALSE].
            Expected: GT_OK.
        */

        /* Call function with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChTunnelStartEgressFilteringSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /* Call function with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChTunnelStartEgressFilteringSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChTunnelStartEgressFilteringGet.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChTunnelStartEgressFilteringGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                     "cpssDxChTunnelStartEgressFilteringGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                                     "get another enable than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelStartEgressFilteringSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelStartEgressFilteringSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelStartEgressFilteringGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelStartEgressFilteringGet)
{
/*
    ITERATE_DEVICES (DxCh1, DxCh2)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st       = GT_OK;

    GT_U8     dev;
    GT_BOOL   state    = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChTunnelStartEgressFilteringGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTunnelStartEgressFilteringGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelStartEgressFilteringGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelStartEgressFilteringGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTaggingSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChEthernetOverMplsTunnelStartTaggingSet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: DxCh2; DxCh3; xCat; Lion)
    1.1. Call with enable [GT_TRUE and GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssDxChEthernetOverMplsTunnelStartTaggingGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS st       = GT_OK;

    GT_U8     dev;
    GT_BOOL   state    = GT_TRUE;
    GT_BOOL   stateGet = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_TRUE and GT_FALSE].
            Expected: GT_OK.
        */

        /* Call function with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChEthernetOverMplsTunnelStartTaggingSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /* Call function with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChEthernetOverMplsTunnelStartTaggingSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChEthernetOverMplsTunnelStartTaggingGet.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChEthernetOverMplsTunnelStartTaggingGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                     "cpssDxChEthernetOverMplsTunnelStartTaggingGet: %d", dev);

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
        st = cpssDxChEthernetOverMplsTunnelStartTaggingSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChEthernetOverMplsTunnelStartTaggingSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTaggingGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChEthernetOverMplsTunnelStartTaggingGet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: DxCh2; DxCh3; xCat; Lion)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st       = GT_OK;

    GT_U8     dev;
    GT_BOOL   state    = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChEthernetOverMplsTunnelStartTaggingGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChEthernetOverMplsTunnelStartTaggingGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChEthernetOverMplsTunnelStartTaggingGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChEthernetOverMplsTunnelStartTaggingGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTagModeSet

(
    IN  GT_U8                                               devNum,
    IN  CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT tagMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChEthernetOverMplsTunnelStartTagModeSet)
{
/*
    ITERATE_DEVICE (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1. Call with tagMode [CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_VLAN_E /
                            CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_ENTRY_E]
    Expected: GT_OK.
    1.2. Call cpssDxChEthernetOverMplsTunnelStartTagModeGet with not NULL tagModePtr.
    Expected: GT_OK and the same tagModePtr as was set.
    1.3. Call with tagMode [wrong enum values]
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT tagMode    =
                            CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_VLAN_E;
    CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT tagModeGet =
                            CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_VLAN_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tagMode [CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_VLAN_E /
                                    CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_ENTRY_E]
            Expected: GT_OK.
        */
        /* iterate with CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_VLAN_E */
        tagMode = CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_VLAN_E;

        st = cpssDxChEthernetOverMplsTunnelStartTagModeSet(dev, tagMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tagMode);

        /*
            1.2. Call cpssDxChEthernetOverMplsTunnelStartTagModeGet with not NULL tagModePtr.
            Expected: GT_OK and the same tagModePtr as was set.
        */
        st = cpssDxChEthernetOverMplsTunnelStartTagModeGet(dev, &tagModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                    "cpssDxChEthernetOverMplsTunnelStartTagModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(tagMode, tagModeGet,
                    "get another tagMode than was set: %d", dev);

        /* iterate with CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_ENTRY_E */
        tagMode = CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_ENTRY_E;

        st = cpssDxChEthernetOverMplsTunnelStartTagModeSet(dev, tagMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tagMode);

        /*
            1.2. Call cpssDxChEthernetOverMplsTunnelStartTagModeGet with not NULL tagModePtr.
            Expected: GT_OK and the same tagModePtr as was set.
        */
        st = cpssDxChEthernetOverMplsTunnelStartTagModeGet(dev, &tagModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                    "cpssDxChEthernetOverMplsTunnelStartTagModeGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(tagMode, tagModeGet,
                    "get another tagMode than was set: %d", dev);

        /*
            1.3. Call with tagMode [wrong enum values]
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChEthernetOverMplsTunnelStartTagModeSet
                            (dev, tagMode),
                            tagMode);
    }

    tagMode = CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_ENTRY_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChEthernetOverMplsTunnelStartTagModeSet(dev, tagMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChEthernetOverMplsTunnelStartTagModeSet(dev, tagMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChEthernetOverMplsTunnelStartTagModeGet

(
    IN  GT_U8                                                devNum,
    OUT CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT *tagModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChEthernetOverMplsTunnelStartTagModeGet)
{
/*
    ITERATE_DEVICE (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1. Call with not NULL tagModePtr.
    Expected: GT_OK.
    1.2. Call tagModePtr [NULL].
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT tagMode =
                    CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_FROM_VLAN_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL tagModePtr.
            Expected: GT_OK.
        */
        st = cpssDxChEthernetOverMplsTunnelStartTagModeGet(dev, &tagMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call tagModePtr [NULL].
            Expected: GT_BAD_PTR
        */
        st = cpssDxChEthernetOverMplsTunnelStartTagModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tagModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChEthernetOverMplsTunnelStartTagModeGet(dev, &tagMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChEthernetOverMplsTunnelStartTagModeGet(dev, &tagMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelStartPassengerVlanTranslationEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelStartPassengerVlanTranslationEnableSet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1. Call with enable [GT_TRUE and GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssDxChTunnelStartPassengerVlanTranslationEnableGet.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL   enable    = GT_TRUE;
    GT_BOOL   enableGet = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_TRUE and GT_FALSE].
            Expected: GT_OK.
        */

        /* Call function with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssDxChTunnelStartPassengerVlanTranslationEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChTunnelStartPassengerVlanTranslationEnableGet.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChTunnelStartPassengerVlanTranslationEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChTunnelStartPassengerVlanTranslationEnableGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /* Call function with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssDxChTunnelStartPassengerVlanTranslationEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChTunnelStartPassengerVlanTranslationEnableGet.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChTunnelStartPassengerVlanTranslationEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChTunnelStartPassengerVlanTranslationEnableGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelStartPassengerVlanTranslationEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelStartPassengerVlanTranslationEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelStartPassengerVlanTranslationEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelStartPassengerVlanTranslationEnableGet)
{
/*
    ITERATE_DEVICES (APPLICABLE DEVICES: DxCh3; xCat; Lion)
    1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;
    GT_U8     dev;

    GT_BOOL   enable = GT_TRUE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChTunnelStartPassengerVlanTranslationEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTunnelStartPassengerVlanTranslationEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelStartPassengerVlanTranslationEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelStartPassengerVlanTranslationEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelTermExceptionCmdSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT exceptionType,
    IN  CPSS_PACKET_CMD_ENT                 command
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelTermExceptionCmdSet)
{
/*
    ITERATE_DEVICES (DxCh1, DxCh2)
    1.1. Call with exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E /
                                  CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E /
                                  CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E]
                   and command [CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                CPSS_PACKET_CMD_DROP_HARD_E]
    Expected: GT_OK.
    1.2. Call cpssDxChTunnelCtrlTtExceptionCmdGet with the same exceptionType.
    Expected: GT_OK and the same command.
    1.3. Call with command [CPSS_PACKET_CMD_FORWARD_E /
                            CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                            CPSS_PACKET_CMD_DROP_SOFT_E /
                            CPSS_PACKET_CMD_INVALID_E]
                   and exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E].
    Expected: NOT GT_OK.
    1.4. Call with wrong enum values exceptionType
                   and command [CPSS_PACKET_CMD_TRAP_TO_CPU_E.
    Expected: GT_BAD_PARAM.
    1.5. Call with exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E]
                   and wrong enum values command.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS  st = GT_OK;

    GT_U8                               dev;
    CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT exceptionType = CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E;
    CPSS_PACKET_CMD_ENT                 command       = CPSS_PACKET_CMD_FORWARD_E;
    CPSS_PACKET_CMD_ENT                 commandGet    = CPSS_PACKET_CMD_FORWARD_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E /
                                          CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E /
                                          CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E]
                           and command [CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                        CPSS_PACKET_CMD_DROP_HARD_E]
            Expected: GT_OK.
        */

        /*
            1.2. Call cpssDxChTunnelCtrlTtExceptionCmdGet with the same exceptionType.
            Expected: GT_OK and the same command.
        */

        /* 1.1. Call function with exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E] */
        /*                    command [CPSS_PACKET_CMD_TRAP_TO_CPU_E]                        */
        exceptionType = CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E;
        command       = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssDxChTunnelTermExceptionCmdSet(dev, exceptionType, command);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, exceptionType, command);

        /* 1.2. Call function with exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E] */
        /*                    command [CPSS_PACKET_CMD_TRAP_TO_CPU_E]                        */
        st = cpssDxChTunnelCtrlTtExceptionCmdGet(dev, exceptionType, &commandGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTunnelCtrlTtExceptionCmdGet: %d, %d",
                                     dev, exceptionType);
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet,
                                     "get another command than was set: %d", dev);

        /* 1.1. Call function with exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E] */
        /*                    command [CPSS_PACKET_CMD_DROP_HARD_E]                               */
        command = CPSS_PACKET_CMD_DROP_HARD_E;
        exceptionType = CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E;

        st = cpssDxChTunnelTermExceptionCmdSet(dev, exceptionType, command);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, exceptionType, command);

        /* 1.2. Call function with exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E] */
        /*                    command [CPSS_PACKET_CMD_DROP_HARD_E]                               */
        st = cpssDxChTunnelCtrlTtExceptionCmdGet(dev, exceptionType, &commandGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTunnelCtrlTtExceptionCmdGet: %d, %d",
                                     dev, exceptionType);
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet,
                                     "get another command than was set: %d", dev);

        /* 1.1. Call function with exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E] */
        /*                    command [CPSS_PACKET_CMD_TRAP_TO_CPU_E]                           */
        exceptionType = CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E;
        command       = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssDxChTunnelTermExceptionCmdSet(dev, exceptionType, command);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, exceptionType, command);

        /* 1.2. Call function with exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E] */
        /*                    command [CPSS_PACKET_CMD_TRAP_TO_CPU_E]                           */

        st = cpssDxChTunnelCtrlTtExceptionCmdGet(dev, exceptionType, &commandGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTunnelCtrlTtExceptionCmdGet: %d, %d",
                                     dev, exceptionType);
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet,
                                     "get another command than was set: %d", dev);

        /*
            1.3. Call with command [CPSS_PACKET_CMD_FORWARD_E /
                                    CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                                    CPSS_PACKET_CMD_DROP_SOFT_E /
                                    CPSS_PACKET_CMD_INVALID_E]
                           and exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E].
            Expected: NOT GT_OK.
        */
        exceptionType = CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E;

        command = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssDxChTunnelTermExceptionCmdSet(dev, exceptionType, command);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, command = %d", dev, command);

        command = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssDxChTunnelTermExceptionCmdSet(dev, exceptionType, command);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, command = %d", dev, command);

        command = CPSS_PACKET_CMD_DROP_SOFT_E;

        st = cpssDxChTunnelTermExceptionCmdSet(dev, exceptionType, command);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, command = %d", dev, command);

        /*
            1.4. Call with wrong enum values exceptionType
                           and command [CPSS_PACKET_CMD_TRAP_TO_CPU_E].
            Expected: GT_BAD_PARAM.
        */
        command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChTunnelTermExceptionCmdSet
                            (dev, exceptionType, command),
                            exceptionType);

        /*
            1.5. Call with exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E]
                           and wrong enum values command.
            Expected: GT_BAD_PARAM.
        */
        exceptionType = CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChTunnelTermExceptionCmdSet
                            (dev, exceptionType, command),
                            command);
    }

    exceptionType = CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E;
    command       = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelTermExceptionCmdSet(dev, exceptionType, command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelTermExceptionCmdSet(dev, exceptionType, command);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelCtrlTtExceptionCmdGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT exceptionType,
    OUT CPSS_PACKET_CMD_ENT                 *commandPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelCtrlTtExceptionCmdGet)
{
/*
    ITERATE_DEVICES (DxCh1, DxCh2)
    1.1. Call with exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E /
                                  CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E /
                                  CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E]
                   and non-null commandPtr.
    Expected: GT_OK.
    1.2. Call with exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E]
                   and commandPtr [NULL].
    Expected: GT_BAD_PTR.
    1.3. Call with wrong enum values exceptionType
                   and non-null commandPtr.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS  st = GT_OK;

    GT_U8                               dev;
    CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT exceptionType = CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E;
    CPSS_PACKET_CMD_ENT                 command       = CPSS_PACKET_CMD_FORWARD_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E /
                                          CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E /
                                          CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E]
                           and non-null commandPtr.
            Expected: GT_OK.
        */

        exceptionType = CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E;

        st = cpssDxChTunnelCtrlTtExceptionCmdGet(dev, exceptionType, &command);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, exceptionType);

        exceptionType = CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E;

        st = cpssDxChTunnelCtrlTtExceptionCmdGet(dev, exceptionType, &command);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, exceptionType);

        exceptionType = CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E;

        st = cpssDxChTunnelCtrlTtExceptionCmdGet(dev, exceptionType, &command);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, exceptionType);

        /*
            1.2. Call with exceptionType [CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E]
                           and commandPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        exceptionType = CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E;

        st = cpssDxChTunnelCtrlTtExceptionCmdGet(dev, exceptionType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, commandPtr = NULL", dev);

        /*
            1.3. Call with wrong enum values exceptionType and non-null commandPtr.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTunnelCtrlTtExceptionCmdGet
                            (dev, exceptionType, &command),
                            exceptionType);
    }

    exceptionType = CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelCtrlTtExceptionCmdGet(dev, exceptionType, &command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelCtrlTtExceptionCmdGet(dev, exceptionType, &command);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill TUNNEL_START table.
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelFillTunnelStartTable)
{
/*
    ITERATE_DEVICE (DxCh2, DxCh3)
    1.1. Get table Size.
         Call cpssDxChCfgTableNumEntriesGet with table [CPSS_DXCH_CFG_TABLE_ARP_TUNNEL_START_E]
                                                 and non-NULL numEntriesPtr.
    Expected: GT_OK.
    1.2. Fill all entries in TUNNEL_START table.
         Call cpssDxChTunnelStartEntrySet with routerArpTunnelStartLineIndex [0... numEntries-1],
                      tunnelType [CPSS_TUNNEL_X_OVER_IPV4_E]
                      and configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                                             vlanId [100],
                                             upMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                             up [0],
                                             dscpMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                             dscp [0],
                                             macDa [00:1A:FF:FF:FF:FF],
                                             dontFragmentFlag [GT_TRUE],
                                             ttl [0],
                                             autoTunnel [GT_TRUE],
                                             autoTunnelOffset [1],
                                             destIp->arIP [10.15.1.250],
                                             srcIp->arIP [10.15.1.255]}.
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssDxChTunnelStartEntrySet with routerArpTunnelStartLineIndex [numEntries]
         and other param from 1.2.
    Expected: GT_OK.
    1.4. Read all entries in TUNNEL_START table and compare with original.
         Call cpssDxChTunnelStartEntryGet with not NULL tunnelTypePtr and configPtr
    Expected: GT_OK and the sme params as was set.
    1.5. Try to read entry with index out of range.
         Call cpssDxChTunnelStartEntryGet with routerArpTunnelStartLineIndex [numEntries].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_TUNNEL_TYPE_ENT              tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;
    CPSS_DXCH_TUNNEL_START_CONFIG_UNT config;

    CPSS_TUNNEL_TYPE_ENT              tunnelTypeGet = CPSS_TUNNEL_IPV4_OVER_IPV4_E;
    CPSS_DXCH_TUNNEL_START_CONFIG_UNT configGet;


    cpssOsBzero((GT_VOID*) &config, sizeof(config));
    cpssOsBzero((GT_VOID*) &configGet, sizeof(configGet));

    /* Fill the entry for TUNNEL_START table */
    tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;

    config.ipv4Cfg.tagEnable    = GT_TRUE;
    config.ipv4Cfg.vlanId       = 100;
    config.ipv4Cfg.upMarkMode   = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
    config.ipv4Cfg.up           = 0;
    config.ipv4Cfg.dscpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
    config.ipv4Cfg.dscp         = 0;

    config.ipv4Cfg.macDa.arEther[0] = 0x0;
    config.ipv4Cfg.macDa.arEther[1] = 0x1A;
    config.ipv4Cfg.macDa.arEther[2] = 0xFF;
    config.ipv4Cfg.macDa.arEther[3] = 0xFF;
    config.ipv4Cfg.macDa.arEther[4] = 0xFF;
    config.ipv4Cfg.macDa.arEther[5] = 0xFF;

    config.ipv4Cfg.dontFragmentFlag = GT_TRUE;
    config.ipv4Cfg.ttl              = 0;
    config.ipv4Cfg.autoTunnel       = GT_TRUE;
    config.ipv4Cfg.autoTunnelOffset = 1;

    config.ipv4Cfg.destIp.arIP[0] = 10;
    config.ipv4Cfg.destIp.arIP[1] = 15;
    config.ipv4Cfg.destIp.arIP[2] = 1;
    config.ipv4Cfg.destIp.arIP[3] = 250;

    config.ipv4Cfg.srcIp.arIP[0] = 10;
    config.ipv4Cfg.srcIp.arIP[1] = 15;
    config.ipv4Cfg.srcIp.arIP[2] = 1;
    config.ipv4Cfg.srcIp.arIP[3] = 255;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        st = cpssDxChCfgTableNumEntriesGet(dev, CPSS_DXCH_CFG_TABLE_TUNNEL_START_E, &numEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChCfgTableNumEntriesGet: %d", dev);

        /* 1.2. Fill all entries in TUNNEL_START table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            config.ipv4Cfg.vlanId   = (GT_U16)(iTemp % 4095);
            config.ipv4Cfg.ttl      = iTemp % 255;
            config.ipv4Cfg.up       = iTemp % 7;
            config.ipv4Cfg.dscp     = iTemp % 63;

            st = cpssDxChTunnelStartEntrySet(dev, iTemp, tunnelType, &config);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "cpssDxChTunnelStartEntrySet: %d, %d, %d", dev, iTemp, tunnelType);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssDxChTunnelStartEntrySet(dev, numEntries, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                    "cpssDxChTunnelStartEntrySet: %d, %d, %d", dev, numEntries, tunnelType);

        /* 1.4. Read all entries in TUNNEL_START table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            config.ipv4Cfg.vlanId   = (GT_U16)(iTemp % 4095);
            config.ipv4Cfg.ttl      = iTemp % 255;
            config.ipv4Cfg.up       = iTemp % 7;
            config.ipv4Cfg.dscp     = iTemp % 63;

            st = cpssDxChTunnelStartEntryGet(dev, iTemp, &tunnelTypeGet, &configGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                        "cpssDxChTunnelStartEntryGet: %d, %d", dev, iTemp);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &config,
                                         (GT_VOID*) &configGet,
                                         sizeof(config) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                        "get anothre config than was set: %d", dev);

            UTF_VERIFY_EQUAL1_STRING_MAC(tunnelType, tunnelTypeGet,
                        "get anothre tunnelType than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssDxChTunnelStartEntryGet(dev, numEntries, &tunnelTypeGet, &configGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                        "cpssDxChTunnelStartEntryGet: %d, %d", dev, numEntries);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill tunnel termination table.
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelFillTunnelTermTable)
{
/*
    ITERATE_DEVICE (DxCh1, DxCh2)
    1.1. Get table Size.
         Call cpssDxChCfgTableNumEntriesGet with table [CPSS_DXCH_CFG_TABLE_ARP_TUNNEL_START_E]
                                                 and non-NULL numEntriesPtr.
    Expected: GT_OK.
    1.2. Fill all entries in tunnel termination table.
         Call cpssDxChTunnelTermEntrySet with routerTunnelTermTcamIndex [0... numEntries-1],
                       tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E],
                       configPtr->ipv4Cfg{srcPortTrunk [0],
                                          srcIsTrunk [0],
                                          srcDev [0],
                                          vid [100],
                                          macDa [00:1A:FF:FF:FF:FF],
                                          srcIp->arIP[10.15.1.250],
                                          destIp->arIP[10.15.1.255]},
                       configMaskPtr-> ipv4Cfg{0xFF,0xFF,...},
                       actionPtr{command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E],
                                 userDefinedCpuCode [CPSS_NET_FIRST_USER_DEFINED_E],
                                 passengerPacketType [CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E],
                                 egressInterface { type [CPSS_INTERFACE_TRUNK_E],
                                                   trunkId [0]},
                                 isTunnelStart [GT_FALSE],
                                 tunnelStartIdx [0],
                                 vlanId [100],
                                 vlanPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                 nestedVlanEnable [GT_FALSE],
                                 copyTtlFromTunnelHeader [GT_FALSE],
                                 qosMode [CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E],
                                 qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E],
                                 qosProfile [127],
                                 defaultUP [0],
                                 modifyUP [CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E],
                                 remapDSCP [GT_FALSE],
                                 modifyDSCP [CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E],
                                 mirrorToIngressAnalyzerEnable [GT_FALSE],
                                 policerEnable [GT_FALSE],
                                 policerIndex [0],
                                 matchCounterEnable [GT_FALSE],
                                 matchCounterIndex [0]}
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssDxChTunnelTermEntrySet with routerTunnelTermTcamIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in tunnel termination table and compare with original.
         Call cpssDxChTunnelTermEntryGet with non-NULL pointers and tcamIndex [0..numEntries-1].
    Expected: GT_OK and  valid [GT_TRUE], tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E],
              config, configMask and action the same as written - for important fields only
    1.5. Try to read entry with index out of range.
         Call cpssDxChTunnelTermEntryGet with non-NULL pointers and routerTunnelTermTcamIndex [numEntries].
    Expected: NOT GT_OK.
    1.6. Delete all entries in tunnel termination table.
         Call cpssDxChTunnelTermEntryInvalidate with routerTunnelTermTcamIndex as in 1.2.
    Expected: GT_OK.
    1.7. Try to delete entry with index out of range.
         Call cpssDxChTunnelTermEntryInvalidate with routerTunnelTermTcamIndex [numEntries].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    GT_U32                           tcamIndex  = 0;
    CPSS_TUNNEL_TYPE_ENT             tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;
    CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT config;
    CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT configMask;
    CPSS_DXCH_TUNNEL_TERM_ACTION_STC action;

    CPSS_TUNNEL_TYPE_ENT             tunnelTypeGet;

    CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT configGet;
    CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT configMaskGet;
    CPSS_DXCH_TUNNEL_TERM_ACTION_STC actionGet;
    GT_BOOL                          validGet = GT_FALSE;

    cpssOsBzero((GT_VOID*) &config, sizeof(config));
    cpssOsBzero((GT_VOID*) &configMask, sizeof(configMask));
    cpssOsBzero((GT_VOID*) &action, sizeof(action));
    cpssOsBzero((GT_VOID*) &configGet, sizeof(configGet));
    cpssOsBzero((GT_VOID*) &configMaskGet, sizeof(configMaskGet));
    cpssOsBzero((GT_VOID*) &actionGet, sizeof(actionGet));

    /* Fill the entry for tunnel termination table */
    tcamIndex  = 10;
    tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;

    config.ipv4Cfg.srcPortTrunk = 0;
    config.ipv4Cfg.srcIsTrunk = 0;
    config.ipv4Cfg.srcDev = 0;
    config.ipv4Cfg.vid = 100;

    config.ipv4Cfg.macDa.arEther[0] = 0x0;
    config.ipv4Cfg.macDa.arEther[1] = 0x1A;
    config.ipv4Cfg.macDa.arEther[2] = 0xFF;
    config.ipv4Cfg.macDa.arEther[3] = 0xFF;
    config.ipv4Cfg.macDa.arEther[4] = 0xFF;
    config.ipv4Cfg.macDa.arEther[5] = 0xFF;

    config.ipv4Cfg.srcIp.arIP[0] = 10;
    config.ipv4Cfg.srcIp.arIP[1] = 15;
    config.ipv4Cfg.srcIp.arIP[2] = 1;
    config.ipv4Cfg.srcIp.arIP[3] = 250;

    config.ipv4Cfg.destIp.arIP[0] = 10;
    config.ipv4Cfg.destIp.arIP[1] = 15;
    config.ipv4Cfg.destIp.arIP[2] = 1;
    config.ipv4Cfg.destIp.arIP[3] = 255;

    cpssOsBzero((GT_VOID*) &configMask, sizeof(configMask));
    cpssOsMemSet((GT_VOID*) &(configMask), 0xFF, sizeof(configMask));

    action.command             = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
    action.userDefinedCpuCode  = CPSS_NET_FIRST_USER_DEFINED_E;
    action.passengerPacketType = CPSS_TUNNEL_PASSENGER_PACKET_IPV4_E;
    action.isTunnelStart       = GT_FALSE;
    action.tunnelStartIdx      = 0;
    action.vlanId              = 100;
    action.vlanPrecedence      = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    action.nestedVlanEnable    = GT_FALSE;
    action.qosMode             = CPSS_DXCH_TUNNEL_QOS_UNTRUST_PKT_E;
    action.qosPrecedence       = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    action.qosProfile          = 127;
    action.defaultUP           = 0;
    action.modifyUP            = CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E;
    action.remapDSCP           = GT_FALSE;
    action.modifyDSCP          = CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
    action.policerEnable       = GT_FALSE;
    action.policerIndex        = 0;
    action.matchCounterEnable  = GT_FALSE;
    action.matchCounterIndex   = 0;
    action.mirrorToIngressAnalyzerEnable = GT_FALSE;
    action.copyTtlFromTunnelHeader = GT_FALSE;

    action.egressInterface.type    = CPSS_INTERFACE_TRUNK_E;
    action.egressInterface.trunkId = 0;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(action.egressInterface.trunkId);

    configMask.ipv4Cfg.srcPortTrunk = 0x7F;
    configMask.ipv4Cfg.srcIsTrunk = 1;
    configMask.ipv4Cfg.srcDev = 0x1F;
    configMask.ipv4Cfg.vid = 0xFFF;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        st = cpssDxChCfgTableNumEntriesGet(dev, CPSS_DXCH_CFG_TABLE_ROUTER_TCAM_E, &numEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChCfgTableNumEntriesGet: %d", dev);

        /* 1.2. Fill all entries in tunnel termination table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            action.vlanId       = (GT_U16)(iTemp % 4095);
            config.ipv4Cfg.vid  = (GT_U16)(iTemp % 4095);

            st = cpssDxChTunnelTermEntrySet(dev, iTemp, tunnelType,
                                            &config, &configMask, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                    "cpssDxChTunnelTermEntrySet: %d, %d, %d", dev, iTemp, tunnelType);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssDxChTunnelTermEntrySet(dev, numEntries, tunnelType,
                                        &config, &configMask, &action);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                    "cpssDxChTunnelTermEntrySet: %d, %d, %d", dev, numEntries, tunnelType);

        /* 1.4. Read all entries in tunnel termination table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            action.vlanId       = (GT_U16)(iTemp % 4095);
            config.ipv4Cfg.vid  = (GT_U16)(iTemp % 4095);

            st = cpssDxChTunnelTermEntryGet(dev, iTemp, &validGet, &tunnelTypeGet,
                                            &configGet, &configMaskGet, &actionGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                            "cpssDxChTunnelTermEntryGet: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, validGet,
                            "entry is treated as invalid: %d, %d", dev, iTemp);
            if((GT_OK == st) && (GT_TRUE == validGet))
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(tunnelType, tunnelTypeGet,
                            "get another tunnelType than was set: %d, %d", dev, iTemp);

                isEqual = (0 == cpssOsMemCmp((GT_VOID*)&config.ipv4Cfg,
                                             (GT_VOID*)&configGet.ipv4Cfg,
                                             sizeof (config.ipv4Cfg)))
                          ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                           "get another config than was set: %d, %d", dev, iTemp);

                 /* check configMask fields */
                UTF_VERIFY_EQUAL2_STRING_MAC(configMask.ipv4Cfg.vid, configMaskGet.ipv4Cfg.vid,
                    "get another configMask.ipv4Cfg.vid than was set: %d, %d", dev, tcamIndex);
                UTF_VERIFY_EQUAL2_STRING_MAC(configMask.ipv4Cfg.srcPortTrunk,
                                             configMaskGet.ipv4Cfg.srcPortTrunk,
                    "get another configMask.ipv4Cfg.srcPortTrunk than was set: %d, %d", dev, tcamIndex);
                UTF_VERIFY_EQUAL2_STRING_MAC(configMask.ipv4Cfg.srcIsTrunk,
                                             configMaskGet.ipv4Cfg.srcIsTrunk,
                    "get another configMask.ipv4Cfg.srcIsTrunk than was set: %d, %d", dev, tcamIndex);
                UTF_VERIFY_EQUAL2_STRING_MAC(configMask.ipv4Cfg.srcDev,
                                             configMaskGet.ipv4Cfg.srcDev,
                    "get another configMask.ipv4Cfg.srcDev than was set: %d, %d", dev, tcamIndex);
                isEqual = (0 == cpssOsMemCmp((GT_VOID*)(&(configMask.ipv4Cfg.macDa)),
                                             (GT_VOID*)(&(configMaskGet.ipv4Cfg.macDa)),
                                             sizeof (configMask.ipv4Cfg.macDa)))
                          ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                           "get another macDa than was set: %d, %d", dev, tcamIndex);
                isEqual = (0 == cpssOsMemCmp((GT_VOID*)(&(configMask.ipv4Cfg.srcIp)),
                                             (GT_VOID*)(&(configMaskGet.ipv4Cfg.srcIp)),
                                             sizeof (configMask.ipv4Cfg.srcIp)))
                          ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                           "get another srcIp than was set: %d, %d", dev, tcamIndex);
                isEqual = (0 == cpssOsMemCmp((GT_VOID*)(&(configMask.ipv4Cfg.destIp)),
                                             (GT_VOID*)(&(configMaskGet.ipv4Cfg.destIp)),
                                             sizeof (configMask.ipv4Cfg.destIp)))
                          ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                           "get another destIp than was set: %d, %d", dev, tcamIndex);

                /* check action fields */
                UTF_VERIFY_EQUAL2_STRING_MAC(action.command, actionGet.command,
                    "get another actionPtr->command than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(action.userDefinedCpuCode,
                                             actionGet.userDefinedCpuCode,
                    "get another actionPtr-> than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(action.passengerPacketType,
                                             actionGet.passengerPacketType,
                    "get another actionPtr->passengerPacketType than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(action.vlanId, actionGet.vlanId,
                    "get another actionPtr->vlanId than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(action.vlanPrecedence, actionGet.vlanPrecedence,
                    "get another actionPtr->vlanPrecedence than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(action.qosMode, actionGet.qosMode,
                    "get another actionPtr->qosMode than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(action.qosPrecedence, actionGet.qosPrecedence,
                    "get another actionPtr->qosPrecedence than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(action.qosProfile, actionGet.qosProfile,
                    "get another actionPtr->qosProfile than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(action.defaultUP, actionGet.defaultUP,
                    "get another actionPtr->defaultUP than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(action.mirrorToIngressAnalyzerEnable,
                                             actionGet.mirrorToIngressAnalyzerEnable,
                    "get another actionPtr->mirrorToIngressAnalyzerEnable than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(action.policerEnable, actionGet.policerEnable,
                    "get another actionPtr->policerEnable than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(action.matchCounterEnable, actionGet.matchCounterEnable,
                    "get another actionPtr->matchCounterEnable than was set: %d, %d", dev, iTemp);
            }
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssDxChTunnelTermEntryGet(dev, numEntries, &validGet, &tunnelTypeGet,
                                        &configGet, &configMaskGet, &actionGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                    "cpssDxChTunnelTermEntryGet: %d, %d", dev, numEntries);

        /* 1.4. Delete all entries in tunnel termination table. */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssDxChTunnelTermEntryInvalidate(dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                    "cpssDxChTunnelTermEntryInvalidate: %d, %d", dev, iTemp);
        }

        /* 1.7. Try to delete entry with index out of range. */
        st = cpssDxChTunnelTermEntryInvalidate(dev, numEntries);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                    "cpssDxChTunnelTermEntryInvalidate: %d, %d", dev, numEntries);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelStartPortGroupEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_GROUPS_BMP                  portGroupsBmp,
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *configPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelStartPortGroupEntrySet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (APPLICABLE DEVICES: DxCh2; DxCh3; xCat; Lion)

    1.1.1. Call with routerArpTunnelStartLineIndex [max - 1],
          tunnelType [CPSS_TUNNEL_X_OVER_IPV4_E /
                      CPSS_TUNNEL_X_OVER_GRE_IPV4_E]
          and configPtr->ipv4Cfg{tagEnable [GT_TRUE],
             vlanId [100],
             upMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
             up [0],
             dscpMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
             dscp [0],
             macDa [00:1A:FF:FF:FF:FF],
             dontFragmentFlag [GT_TRUE],
             ttl [0],
             autoTunnel [GT_TRUE],
             autoTunnelOffset [1],
             destIp->arIP [10.15.1.250],
             srcIp->arIP [10.15.1.255]}.
    Expected: GT_OK.
    1.1.2. Call cpssDxChTunnelStartPortGroupEntryGet with
    non-NULL tunnelTypePtr, configPtr
    and the same routerArpTunnelStartLineIndex as in 1.1.1.
    Expected: GT_OK and same value as written.
    1.1.2. Call cpssDxChTunnelStartPortGroupEntryGet with
    non-NULL tunnelTypePtr, configPtr
    and the same routerArpTunnelStartLineIndex as in 1.1.1.
    Expected: GT_OK and same value as written.
    1.1.3. Call with configPtr->ipv4Cfg{destIp->arIP [255.255.255.255],
                                      srcIp->arIP [255.255.255.251]}
                     and other parameters from 1.1.1.
    Expected: GT_OK.
    1.1.4. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                                        out of range vlanId [4096]}
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.5. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                                        wrong enum values upMarkMode }
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.6. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
               upMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
               out of range up [8]}
           and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.7. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
               dscpMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
               out of range dscp [64]}
           and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.8. Call with out of range configPtr->ipv4Cfg->ttl [256]
                     and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.9. Call with configPtr->ipv4Cfg->autoTunnelOffset [16]
                     and autoTunnel [GT_TRUE] (relevant for IPv6-over-IPv4 or
                                                            IPv6-over-GRE-IPv4)
                     and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.10. Call with routerArpTunnelStartLineIndex [10],
             tunnelType [CPSS_TUNNEL_X_OVER_MPLS_E]
             and configPtr->mplsCfg {tagEnable [GT_TRUE],
                     vlanId[100],
                     upMarkMode[CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                     up [7],
                     macDa [00:1A:FF:FF:FF:FF],
                     numLabels[2],
                     ttl [10],
                     label1 [100],
                     exp1MarkMode[CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                     exp1[7],
                     label2 [200],
                     exp2MarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                     exp2[7],
                     retainCRC [GT_TRUE]}
    Expected: GT_OK.
    1.1.11. Call cpssDxChTunnelStartPortGroupEntryGet with non-NULL
    tunnelTypePtr, configPtr  and the same routerArpTunnelStartLineIndex as in 1.1.10.
    Expected: GT_OK and same value as written.
    1.1.12. Call with configPtr->mplsCfg {tagEnable [GT_TRUE],
                                        out of range vlanId [4096]}
                    and other parameters from 1.10.
    Expected: GT_BAD_PARAM.
    1.1.13. Call with configPtr->mplsCfg {tagEnable [GT_TRUE],
                                          wrong enum values upMarkMode }
                      and other parameters from 1.10.
    Expected: GT_BAD_PARAM.
    1.1.14. Call with configPtr->mplsCfg {tagEnable [GT_TRUE],
                upMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                out of range up [8]}
            and other parameters from 1.19.
    Expected: NOT GT_OK.
    1.1.15. Call with out of range configPtr->mplsCfg {numLabels[0, 5]}
                      and other parameters from 1.10.
    Expected: NOT GT_OK.
    1.1.16. Call with out of range configPtr->mplsCfg->ttl [256]
                      and other parameters from 1.10.
    Expected: NOT GT_OK.
    1.1.17. Call with configPtr->mplsCfg->{numLabels [2],
            out of range label1 [1048576] (relevant only when number of labels is 2)}
                      and other parameters from 1.10.
    Expected: NOT GT_OK.
    1.1.18. Call with configPtr->mplsCfg->{numLabels [2],
            wrong enum values exp1MarkMode  (relevant only when number of labels is 2)}
                      and other parameters from 1.10.
    Expected: GT_BAD_PARAM.
    1.1.19. Call with configPtr->mplsCfg->{numLabels [2],
                exp1MarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                out of range exp1[8]
                (relevant only when number of labels is 2 and exp1MarkMode
                 is set according to entry <EXP1>)}
            and other parameters from 1.10.
    Expected: NOT GT_OK.
    1.1.20. Call with configPtr->mplsCfg->{out of range label2 [1048576]}
                      and other parameters from 1.10.
    Expected: NOT GT_OK.
    1.1.21. Call with configPtr->mplsCfg->{wrong enum values exp2MarkMode }
                      and other parameters from 1.10.
    Expected: GT_BAD_PARAM.
    1.1.22. Call with configPtr->mplsCfg->{exp2MarkMode
            [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
            out of range exp2[8]}
                      and other parameters from 1.10.
    Expected: GT_BAD_PARAM.
    1.1.23. Call with out of range routerArpTunnelStartLineIndex [max]
                      and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.24. Call with wrong enum values tunnelType
                      and other parameters from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.25. Call with out of range tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E /
                                               CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E /
                                               CPSS_TUNNEL_IPV6_OVER_IPV4_E /
                                               CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E /
                                               CPSS_TUNNEL_X_OVER_MPLS_E /
                                               CPSS_TUNNEL_ETHERNET_OVER_MPLS_E /
                                               CPSS_TUNNEL_IP_OVER_X_E]
                      and other parameters from 1.1.1.
    Expected: NOT GT_OK.
    1.1.26. Call with configPtr [NULL]
                      and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    GT_BOOL                             isEqual       = GT_FALSE;
    GT_U32                              lineIndex     = 0;
    GT_U32                              configSize    = 0;
    CPSS_TUNNEL_TYPE_ENT                tunnelType    = CPSS_TUNNEL_IPV4_OVER_IPV4_E;
    CPSS_TUNNEL_TYPE_ENT                tunnelTypeGet = CPSS_TUNNEL_IPV4_OVER_IPV4_E;
    CPSS_DXCH_TUNNEL_START_CONFIG_UNT   config;
    CPSS_DXCH_TUNNEL_START_CONFIG_UNT   configGet;


    cpssOsBzero((GT_VOID*) &config, sizeof(config));
    cpssOsBzero((GT_VOID*) &configGet, sizeof(configGet));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with routerArpTunnelStartLineIndex [max - 1],
                      tunnelType [CPSS_TUNNEL_X_OVER_IPV4_E /
                                  CPSS_TUNNEL_X_OVER_GRE_IPV4_E]
                      and configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                         vlanId [100],
                         upMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                         up [0],
                         dscpMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                         dscp [0],
                         macDa [00:1A:FF:FF:FF:FF],
                         dontFragmentFlag [GT_TRUE],
                         ttl [0],
                         autoTunnel [GT_TRUE],
                         autoTunnelOffset [1],
                         destIp->arIP [10.15.1.250],
                         srcIp->arIP [10.15.1.255]}.
                Expected: GT_OK.
            */

            /* Call with tunnelType [CPSS_TUNNEL_X_OVER_IPV4_E] */
            lineIndex  = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.tunnelStart - 1;
            tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;

            config.ipv4Cfg.tagEnable    = GT_TRUE;
            config.ipv4Cfg.vlanId       = 100;
            config.ipv4Cfg.upMarkMode   = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            config.ipv4Cfg.up           = 0;
            config.ipv4Cfg.dscpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            config.ipv4Cfg.dscp         = 0;
            config.ipv4Cfg.cfi          = 0;

            config.ipv4Cfg.macDa.arEther[0] = 0x0;
            config.ipv4Cfg.macDa.arEther[1] = 0x1A;
            config.ipv4Cfg.macDa.arEther[2] = 0xFF;
            config.ipv4Cfg.macDa.arEther[3] = 0xFF;
            config.ipv4Cfg.macDa.arEther[4] = 0xFF;
            config.ipv4Cfg.macDa.arEther[5] = 0xFF;

            config.ipv4Cfg.dontFragmentFlag = GT_TRUE;
            config.ipv4Cfg.ttl              = 0;
            config.ipv4Cfg.autoTunnel       = GT_TRUE;
            config.ipv4Cfg.autoTunnelOffset = 1;

            config.ipv4Cfg.destIp.arIP[0] = 10;
            config.ipv4Cfg.destIp.arIP[1] = 15;
            config.ipv4Cfg.destIp.arIP[2] = 1;
            config.ipv4Cfg.destIp.arIP[3] = 250;

            config.ipv4Cfg.srcIp.arIP[0] = 10;
            config.ipv4Cfg.srcIp.arIP[1] = 15;
            config.ipv4Cfg.srcIp.arIP[2] = 1;
            config.ipv4Cfg.srcIp.arIP[3] = 255;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
            }

            /*
                1.1.2. Call cpssDxChTunnelStartPortGroupEntryGet with
                non-NULL tunnelTypePtr, configPtr
                and the same routerArpTunnelStartLineIndex as in 1.1.1.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxChTunnelStartPortGroupEntryGet(dev, portGroupsBmp, lineIndex,
                                                      &tunnelTypeGet, &configGet);
            if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChTunnelStartPortGroupEntryGet: %d", dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChTunnelStartPortGroupEntryGet: %d", dev);

                /* validation values */
                UTF_VERIFY_EQUAL1_STRING_MAC(tunnelType, tunnelTypeGet,
                           "got another tunnelType then was set: %d", dev);

                /* calc size of IpV4 config */
                /* CFI bit relevant for DxCh3; xCat; Lion */
                configSize = sizeof(config.ipv4Cfg) - sizeof(config.ipv4Cfg.cfi) *
                     (CPSS_PP_FAMILY_CHEETAH2_E == PRV_CPSS_PP_MAC(dev)->devFamily);

                /* check if received config is equal to original */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &config.ipv4Cfg,
                                             (GT_VOID*) &configGet.ipv4Cfg,
                                             configSize)) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                           "got another config then was set: %d", dev);
            }

            /* Call with tunnelType [CPSS_TUNNEL_X_OVER_GRE_IPV4_E] */
            tunnelType = CPSS_TUNNEL_X_OVER_GRE_IPV4_E;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
            }

            /*
                1.1.2. Call cpssDxChTunnelStartPortGroupEntryGet with
                non-NULL tunnelTypePtr, configPtr
                and the same routerArpTunnelStartLineIndex as in 1.1.1.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxChTunnelStartPortGroupEntryGet(dev, portGroupsBmp, lineIndex,
                                                      &tunnelTypeGet, &configGet);
            if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChTunnelStartPortGroupEntryGet: %d", dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChTunnelStartPortGroupEntryGet: %d", dev);

                /* validation values */
                UTF_VERIFY_EQUAL1_STRING_MAC(tunnelType, tunnelTypeGet,
                           "got another tunnelType then was set: %d", dev);

                /* calc size of IpV4 config */
                /* CFI bit relevant for DxCh3; xCat; Lion */
                configSize = sizeof(config.ipv4Cfg) - sizeof(config.ipv4Cfg.cfi) *
                     (CPSS_PP_FAMILY_CHEETAH2_E == PRV_CPSS_PP_MAC(dev)->devFamily);

                /* check if received config is equal to original */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &config.ipv4Cfg,
                                             (GT_VOID*) &configGet.ipv4Cfg,
                                             configSize)) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                           "got another config then was set: %d", dev);
            }

            /*
                1.1.3. Call with configPtr->ipv4Cfg{destIp->arIP [255.255.255.255],
                                                  srcIp->arIP [255.255.255.251]}
                                 and other parameters from 1.1.1.
                Expected: GT_OK.
            */
            tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;

            config.ipv4Cfg.destIp.arIP[0] = 255;
            config.ipv4Cfg.destIp.arIP[1] = 255;
            config.ipv4Cfg.destIp.arIP[2] = 255;
            config.ipv4Cfg.destIp.arIP[3] = 255;

            config.ipv4Cfg.srcIp.arIP[0] = 255;
            config.ipv4Cfg.srcIp.arIP[1] = 255;
            config.ipv4Cfg.srcIp.arIP[2] = 255;
            config.ipv4Cfg.srcIp.arIP[3] = 251;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
            }

            /*
                1.1.4. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                                                    out of range vlanId [4096]}
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            config.ipv4Cfg.destIp.arIP[0] = 10;
            config.ipv4Cfg.destIp.arIP[1] = 15;
            config.ipv4Cfg.destIp.arIP[2] = 1;
            config.ipv4Cfg.destIp.arIP[3] = 250;

            config.ipv4Cfg.srcIp.arIP[0] = 10;
            config.ipv4Cfg.srcIp.arIP[1] = 15;
            config.ipv4Cfg.srcIp.arIP[2] = 1;
            config.ipv4Cfg.srcIp.arIP[3] = 255;

            config.ipv4Cfg.tagEnable = GT_TRUE;
            config.ipv4Cfg.vlanId    = 4096;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, configPtr->ipv4Cfg.vlanId = %d",
                                         dev, config.ipv4Cfg.vlanId);

            config.ipv4Cfg.vlanId = 100;

            /*
                1.1.5. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                                                    wrong enum values upMarkMode }
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            config.ipv4Cfg.vlanId    = 100;
            config.ipv4Cfg.tagEnable = GT_TRUE;

            if (!prvUtfIsPbrModeUsed() || (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_ENUMS_CHECK_MAC(cpssDxChTunnelStartPortGroupEntrySet
                                    (dev, portGroupsBmp, lineIndex, tunnelType, &config),
                                    config.ipv4Cfg.upMarkMode);
            }

            /*
                1.1.6. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                           upMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                           out of range up [8]}
                       and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            config.ipv4Cfg.tagEnable  = GT_TRUE;
            config.ipv4Cfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            config.ipv4Cfg.up         = 8;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                            "%d, ->up = %d", dev, config.ipv4Cfg.up);

            config.ipv4Cfg.up = 0;

            /*
                1.1.7. Call with configPtr->ipv4Cfg{tagEnable [GT_TRUE],
                           dscpMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                           out of range dscp [64]}
                       and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            config.ipv4Cfg.tagEnable    = GT_TRUE;
            config.ipv4Cfg.dscpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            config.ipv4Cfg.dscp         = 64;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                            "%d, ->dscp = %d", dev, config.ipv4Cfg.dscp);

            config.ipv4Cfg.dscp = 0;

            /*
                1.1.8. Call with out of range configPtr->ipv4Cfg->ttl [256]
                                 and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            config.ipv4Cfg.ttl = 256;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->ipv4Cfg.ttl = %d",
                                             dev, config.ipv4Cfg.ttl);

            config.ipv4Cfg.ttl = 0;

            /*
                1.1.9. Call with configPtr->ipv4Cfg->autoTunnelOffset [16]
                                 and autoTunnel [GT_TRUE]
                                 (relevant for IPv6-over-IPv4 or IPv6-over-GRE-IPv4)
                                 and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            config.ipv4Cfg.autoTunnel       = GT_TRUE;
            config.ipv4Cfg.autoTunnelOffset = 16;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ->ipv4Cfg.autoTunnelOffset = %d",
                                             dev, config.ipv4Cfg.autoTunnelOffset);

            /*
                1.1.10. Call with routerArpTunnelStartLineIndex [10],
                         tunnelType [CPSS_TUNNEL_X_OVER_MPLS_E]
                         and configPtr->mplsCfg {tagEnable [GT_TRUE],
                                 vlanId[100],
                                 upMarkMode[CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                 up [7],
                                 macDa [00:1A:FF:FF:FF:FF],
                                 numLabels[2],
                                 ttl [10],
                                 label1 [100],
                                 exp1MarkMode[CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                 exp1[7],
                                 label2 [200],
                                 exp2MarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                                 exp2[7],
                                 retainCRC [GT_TRUE]}
                Expected: GT_OK.
            */
            lineIndex  = 10;
            tunnelType = CPSS_TUNNEL_X_OVER_MPLS_E;

            config.mplsCfg.tagEnable  = GT_TRUE;
            config.mplsCfg.vlanId     = 100;
            config.mplsCfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            config.mplsCfg.up         = 7;
            config.mplsCfg.cfi        = 0;

            config.mplsCfg.macDa.arEther[0] = 0x0;
            config.mplsCfg.macDa.arEther[1] = 0x1A;
            config.mplsCfg.macDa.arEther[2] = 0xFF;
            config.mplsCfg.macDa.arEther[3] = 0xFF;
            config.mplsCfg.macDa.arEther[4] = 0xFF;
            config.mplsCfg.macDa.arEther[5] = 0xFF;

            config.mplsCfg.numLabels    = 2;
            config.mplsCfg.ttl          = 10;
            config.mplsCfg.label1       = 100;
            config.mplsCfg.exp1MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            config.mplsCfg.exp1         = 7;
            config.mplsCfg.label2       = 200;
            config.mplsCfg.exp2MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            config.mplsCfg.exp2         = 7;
            config.mplsCfg.retainCRC    = GT_FALSE;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, lineIndex, tunnelType);
            }

            /*
                1.1.11. Call cpssDxChTunnelStartPortGroupEntryGet with non-NULL
                tunnelTypePtr, configPtr  and the same routerArpTunnelStartLineIndex as in 1.1.10.
                Expected: GT_OK and same value as written.
            */
            st = cpssDxChTunnelStartPortGroupEntryGet(dev, portGroupsBmp, lineIndex,
                                                      &tunnelTypeGet, &configGet);
            if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChTunnelStartPortGroupEntryGet: %d", dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChTunnelStartPortGroupEntryGet: %d", dev);

                /* validation values */
                UTF_VERIFY_EQUAL1_STRING_MAC(tunnelType, tunnelTypeGet,
                           "got another tunnelType then was set: %d", dev);

                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &config.mplsCfg,
                                             (GT_VOID*) &configGet.mplsCfg,
                                             sizeof(config.mplsCfg))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                           "got another config then was set: %d", dev);
            }

            /*
                1.1.12. Call with configPtr->mplsCfg {tagEnable [GT_TRUE],
                                                    out of range vlanId [4096]}
                                and other parameters from 1.10.
                Expected: GT_BAD_PARAM.
            */
            config.mplsCfg.tagEnable = GT_TRUE;
            config.mplsCfg.vlanId    = 4096;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->mplsCfg.vlanId = %d",
                                         dev, config.mplsCfg.vlanId);

            config.mplsCfg.vlanId = 100;

            /*
                1.1.13. Call with configPtr->mplsCfg {tagEnable [GT_TRUE],
                                                      wrong enum values upMarkMode }
                                  and other parameters from 1.10.
                Expected: GT_BAD_PARAM.
            */
            config.mplsCfg.tagEnable  = GT_TRUE;

            if (!prvUtfIsPbrModeUsed())
            {
                UTF_ENUMS_CHECK_MAC(cpssDxChTunnelStartPortGroupEntrySet
                                    (dev, portGroupsBmp, lineIndex, tunnelType, &config),
                                    config.mplsCfg.upMarkMode);
            }

            /*
                1.1.14. Call with configPtr->mplsCfg {tagEnable [GT_TRUE],
                            upMarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                            out of range up [8]}
                        and other parameters from 1.19.
                Expected: NOT GT_OK.
            */
            config.mplsCfg.tagEnable  = GT_TRUE;
            config.mplsCfg.upMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            config.mplsCfg.up         = 8;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ->up = %d", dev, config.mplsCfg.up);

            config.mplsCfg.up = 0;

            /*
                1.1.15. Call with out of range configPtr->mplsCfg {numLabels[0, 5]}
                                  and other parameters from 1.10.
                Expected: NOT GT_OK.
            */
            /* Call with configPtr->mplsCfg.numLabels [0] */
            config.mplsCfg.numLabels = 0;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.numLabels = %d",
                                             dev, config.mplsCfg.numLabels);

            /* Call with configPtr->mplsCfg.numLabels [5] */
            config.mplsCfg.numLabels = 5;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.numLabels = %d",
                                             dev, config.mplsCfg.numLabels);

            config.mplsCfg.numLabels = 2;

            /*
                1.1.16. Call with out of range configPtr->mplsCfg->ttl [256]
                                  and other parameters from 1.10.
                Expected: NOT GT_OK.
            */
            config.mplsCfg.ttl = 256;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.ttl = %d",
                                             dev, config.mplsCfg.ttl);

            config.mplsCfg.ttl = 10;

            /*
                1.1.17. Call with configPtr->mplsCfg->{numLabels [2],
                        out of range label1 [1048576] (relevant only when number of labels is 2)}
                                  and other parameters from 1.10.
                Expected: NOT GT_OK.
            */
            config.mplsCfg.label1 = 1048576;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.label1 = %d",
                                             dev, config.mplsCfg.label1);

            config.mplsCfg.label1 = 0;

            /*
                1.1.18. Call with configPtr->mplsCfg->{numLabels [2],
                        wrong enum values exp1MarkMode  (relevant only when number of labels is 2)}
                                  and other parameters from 1.10.
                Expected: GT_BAD_PARAM.
            */
            config.mplsCfg.numLabels    = 2;
            config.mplsCfg.label1       = 100;

            if (!prvUtfIsPbrModeUsed() || (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_ENUMS_CHECK_MAC(cpssDxChTunnelStartPortGroupEntrySet
                                    (dev, portGroupsBmp, lineIndex,tunnelType, &config),
                                    config.mplsCfg.exp1MarkMode);
            }

            /*
                1.1.19. Call with configPtr->mplsCfg->{numLabels [2],
                            exp1MarkMode [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                            out of range exp1[8]
                            (relevant only when number of labels is 2 and
                             exp1MarkMode is set according to entry <EXP1>)}
                        and other parameters from 1.10.
                Expected: NOT GT_OK.
            */
            config.mplsCfg.numLabels    = 2;
            config.mplsCfg.exp1MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            config.mplsCfg.exp1         = 8;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                "%d, ->mplsCfg.exp1 = %d", dev, config.mplsCfg.exp1);

            config.mplsCfg.exp1 = 0;

            /*
                1.1.20. Call with configPtr->mplsCfg->{out of range label2 [1048576]}
                                  and other parameters from 1.10.
                Expected: NOT GT_OK.
            */
            config.mplsCfg.numLabels = 2;
            config.mplsCfg.label2    = 1048576;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.label2 = %d",
                                             dev, config.mplsCfg.label2);

            config.mplsCfg.label2 = 200;

            /*
                1.1.21. Call with configPtr->mplsCfg->{wrong enum values exp2MarkMode }
                                  and other parameters from 1.10.
                Expected: GT_BAD_PARAM.
            */
            if (!prvUtfIsPbrModeUsed() || (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_ENUMS_CHECK_MAC(cpssDxChTunnelStartPortGroupEntrySet
                                    (dev, portGroupsBmp, lineIndex, tunnelType, &config),
                                    config.mplsCfg.exp2MarkMode);
            }

            /*
                1.1.22. Call with configPtr->mplsCfg->{exp2MarkMode
                        [CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E],
                        out of range exp2[8]}
                                  and other parameters from 1.10.
                Expected: GT_BAD_PARAM.
            */
            config.mplsCfg.exp2MarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            config.mplsCfg.exp2         = 8;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, configPtr->mplsCfg.exp2 = %d",
                                             dev, config.mplsCfg.exp2);

            config.mplsCfg.exp2 = 0;

            /*
                1.1.23. Call with out of range routerArpTunnelStartLineIndex [max]
                                  and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */
            lineIndex  = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.tunnelStart;
            tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;

            config.ipv4Cfg.tagEnable    = GT_TRUE;
            config.ipv4Cfg.vlanId       = 100;
            config.ipv4Cfg.upMarkMode   = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            config.ipv4Cfg.up           = 0;
            config.ipv4Cfg.dscpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
            config.ipv4Cfg.dscp         = 0;
            config.ipv4Cfg.cfi          = 0;

            config.ipv4Cfg.macDa.arEther[0] = 0x0;
            config.ipv4Cfg.macDa.arEther[1] = 0x1A;
            config.ipv4Cfg.macDa.arEther[2] = 0xFF;
            config.ipv4Cfg.macDa.arEther[3] = 0xFF;
            config.ipv4Cfg.macDa.arEther[4] = 0xFF;
            config.ipv4Cfg.macDa.arEther[5] = 0xFF;

            config.ipv4Cfg.dontFragmentFlag = GT_TRUE;
            config.ipv4Cfg.ttl              = 0;
            config.ipv4Cfg.autoTunnel       = GT_TRUE;
            config.ipv4Cfg.autoTunnelOffset = 1;

            config.ipv4Cfg.destIp.arIP[0] = 10;
            config.ipv4Cfg.destIp.arIP[1] = 15;
            config.ipv4Cfg.destIp.arIP[2] = 1;
            config.ipv4Cfg.destIp.arIP[3] = 250;

            config.ipv4Cfg.srcIp.arIP[0] = 10;
            config.ipv4Cfg.srcIp.arIP[1] = 15;
            config.ipv4Cfg.srcIp.arIP[2] = 1;
            config.ipv4Cfg.srcIp.arIP[3] = 255;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lineIndex);

            /*
                1.1.24. Call with wrong enum values tunnelType
                                  and other parameters from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            lineIndex  = 10;

            if (!prvUtfIsPbrModeUsed() || (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_ENUMS_CHECK_MAC(cpssDxChTunnelStartPortGroupEntrySet
                                    (dev, portGroupsBmp, lineIndex, tunnelType, &config),
                                    tunnelType);
            }

            /*
                1.1.25. Call with out of range tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E /
                                                           CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E /
                                                           CPSS_TUNNEL_IPV6_OVER_IPV4_E /
                                                           CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E /
                                                           CPSS_TUNNEL_X_OVER_MPLS_E /
                                                           CPSS_TUNNEL_ETHERNET_OVER_MPLS_E /
                                                           CPSS_TUNNEL_IP_OVER_X_E]
                                  and other parameters from 1.1.1.
                Expected: NOT GT_OK.
            */

            /* Call with tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E] */
            tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d",
                                             dev, tunnelType);

            /* Call with tunnelType [CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E] */
            tunnelType = CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d",
                                             dev, tunnelType);

            /* Call with tunnelType [CPSS_TUNNEL_IPV6_OVER_IPV4_E] */
            tunnelType = CPSS_TUNNEL_IPV6_OVER_IPV4_E;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d",
                                             dev, tunnelType);

            /* Call with tunnelType [CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E] */
            tunnelType = CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d",
                                             dev, tunnelType);

            /* Call with tunnelType [CPSS_TUNNEL_X_OVER_MPLS_E] */
            tunnelType = CPSS_TUNNEL_X_OVER_MPLS_E;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d",
                                             dev, tunnelType);

            /* Call with tunnelType [CPSS_TUNNEL_ETHERNET_OVER_MPLS_E] */
            tunnelType = CPSS_TUNNEL_ETHERNET_OVER_MPLS_E;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d",
                                             dev, tunnelType);

            /* Call with tunnelType [CPSS_TUNNEL_IP_OVER_X_E] */
            tunnelType = CPSS_TUNNEL_IP_OVER_X_E;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d",
                                             dev, tunnelType);

            /*
                1.1.26. Call with configPtr [NULL]
                                  and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, NULL);
            if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, configPtr = NULL", dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configPtr = NULL", dev);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                      tunnelType, &config);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
                {
                    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
                }
                else
                {
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
                }
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                  tunnelType, &config);
        if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
        }
    }

    portGroupsBmp = 1;

    lineIndex  = 10;
    tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;

    config.ipv4Cfg.tagEnable    = GT_TRUE;
    config.ipv4Cfg.vlanId       = 100;
    config.ipv4Cfg.upMarkMode   = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
    config.ipv4Cfg.up           = 0;
    config.ipv4Cfg.dscpMarkMode = CPSS_DXCH_TUNNEL_START_QOS_MARK_FROM_ENTRY_E;
    config.ipv4Cfg.dscp         = 0;
    config.ipv4Cfg.cfi          = 0;

    config.ipv4Cfg.macDa.arEther[0] = 0x0;
    config.ipv4Cfg.macDa.arEther[1] = 0x1A;
    config.ipv4Cfg.macDa.arEther[2] = 0xFF;
    config.ipv4Cfg.macDa.arEther[3] = 0xFF;
    config.ipv4Cfg.macDa.arEther[4] = 0xFF;
    config.ipv4Cfg.macDa.arEther[5] = 0xFF;

    config.ipv4Cfg.dontFragmentFlag = GT_TRUE;
    config.ipv4Cfg.ttl              = 0;
    config.ipv4Cfg.autoTunnel       = GT_TRUE;
    config.ipv4Cfg.autoTunnelOffset = 1;

    config.ipv4Cfg.destIp.arIP[0] = 10;
    config.ipv4Cfg.destIp.arIP[1] = 15;
    config.ipv4Cfg.destIp.arIP[2] = 1;
    config.ipv4Cfg.destIp.arIP[3] = 250;

    config.ipv4Cfg.srcIp.arIP[0] = 10;
    config.ipv4Cfg.srcIp.arIP[1] = 15;
    config.ipv4Cfg.srcIp.arIP[2] = 1;
    config.ipv4Cfg.srcIp.arIP[3] = 255;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                                  tunnelType, &config);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelStartPortGroupEntrySet(dev, portGroupsBmp, lineIndex,
                                              tunnelType, &config);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelStartPortGroupEntryGet
(
    IN   GT_U8                              devNum,
    IN   GT_PORT_GROUPS_BMP                 portGroupsBmp,
    IN   GT_U32                             routerArpTunnelStartLineIndex,
    OUT  CPSS_TUNNEL_TYPE_ENT               *tunnelTypePtr,
    OUT  CPSS_DXCH_TUNNEL_START_CONFIG_UNT  *configPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelStartPortGroupEntryGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS(APPLICABLE DEVICES: DxCh2; DxCh3; xCat; Lion)
    1.1.1. Call with routerArpTunnelStartLineIndex[max - 1]
                     non-null tunnelTypePtr
                     and non-null configPtr.
    Expected: GT_OK.
    1.1.2. Call with out of range routerArpTunnelStartLineIndex [max],
                     non-null tunnelTypePtr
                     and non-null configPtr.
    Expected: NOT GT_OK.
    1.1.3. Call with routerArpTunnelStartLineIndex[max - 1],
                     tunnelTypePtr[NULL]
                     and non-null configPtr.
    Expected: GT_BAD_PTR.
    1.1.4. Call with routerArpTunnelStartLineIndex[max - 1],
                     non-null tunnelTypePtr
                     and configPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId   = 0;

    GT_U32                              lineIndex  = 0;
    CPSS_TUNNEL_TYPE_ENT                tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;
    CPSS_DXCH_TUNNEL_START_CONFIG_UNT   config;

    cpssOsBzero((GT_VOID*) &config, sizeof(config));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);

            /*
                1.1.1. Call with routerArpTunnelStartLineIndex[max - 1]
                                 non-null tunnelTypePtr
                                 and non-null configPtr.
                Expected: GT_OK.
            */
            lineIndex  = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.tunnelStart - 1;

            st = cpssDxChTunnelStartPortGroupEntryGet(dev, portGroupsBmp, lineIndex,
                                                      &tunnelType, &config);
            if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lineIndex);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lineIndex);
            }

            /*
                1.1.2. Call with out of range routerArpTunnelStartLineIndex [max],
                                 non-null tunnelTypePtr
                                 and non-null configPtr.
                Expected: NOT GT_OK.
            */
            lineIndex  = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.tunnelStart;

            st = cpssDxChTunnelStartPortGroupEntryGet(dev, portGroupsBmp, lineIndex,
                                                      &tunnelType, &config);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lineIndex);

            /*
                1.1.3. Call with routerArpTunnelStartLineIndex[max - 1],
                                 tunnelTypePtr[NULL]
                                 and non-null configPtr.
                Expected: GT_BAD_PTR.
            */
            lineIndex  = PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.tableSize.tunnelStart - 1;

            st = cpssDxChTunnelStartPortGroupEntryGet(dev, portGroupsBmp, lineIndex,
                                                      NULL, &config);
            if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, tunnelTypePtr = NULL", dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, tunnelTypePtr = NULL", dev);
            }

            /*
                1.1.4. Call with routerArpTunnelStartLineIndex[max - 1],
                                 non-null tunnelTypePtr
                                 and configPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTunnelStartPortGroupEntryGet(dev, portGroupsBmp, lineIndex,
                                                      &tunnelType, NULL);
            if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, configPtr = NULL", dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configPtr = NULL", dev);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChTunnelStartPortGroupEntryGet(dev, portGroupsBmp, lineIndex,
                                                      &tunnelType, &config);

            if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
            else
            {
                if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
                {
                    UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
                }
                else
                {
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
                }
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChTunnelStartPortGroupEntryGet(dev, portGroupsBmp, lineIndex,
                                                  &tunnelType, &config);
        if (prvUtfIsPbrModeUsed() && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
        }
    }

    portGroupsBmp = 1;

    lineIndex = 10;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E);

    /* go over all non active devices return GT_OK.
       for non multi core the API ignores the portGroupsBmp  */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelStartPortGroupEntryGet(dev, portGroupsBmp, lineIndex,
                                                  &tunnelType, &config);
        if (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        else
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelStartPortGroupEntryGet(dev, portGroupsBmp, lineIndex,
                                              &tunnelType, &config);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet
(
    IN  GT_U8   dev,
    OUT GT_U32  *additionToLengthPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet)
{
/*
    ITERATE_DEVICES(DxChXcat)
    1.1. Call with not null additionToLengthPtr.
    Expected: GT_OK.
    1.2. Call api with wrong additionToLengthPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U32  additionToLength;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E |
                                           UTF_CH3_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null additionToLengthPtr.
            Expected: GT_OK.
        */
        st = cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet(dev, &additionToLength);

        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }

        /*
            1.2. Call api with wrong additionToLengthPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet(dev, NULL);

        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, additionToLengthPtr = NULL", dev);
        }
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E |
                                           UTF_CH3_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet(dev, &additionToLength);

        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet(dev, &additionToLength);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet
(
    IN  GT_U8  dev,
    IN  GT_U32 additionToLength
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet)
{
/*
    ITERATE_DEVICES(DxChXcat)
    1.1. Call with additionToLength[0 / BIT_3 / BIT_6 - 1],
    Expected: GT_OK.
    1.2. Call cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong additionToLength [BIT_6 + 1].
    Expected: NOT GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8  dev;
    GT_U32 additionToLength = 0;
    GT_U32 additionToLengthGet = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E |
                                           UTF_CH3_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with additionToLength[0 / BIT_3 / BIT_6 - 1],
            Expected: GT_OK.
        */
        /* call with additionToLength[0] */
        additionToLength = 0;

        st = cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet(dev, additionToLength);

        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }

        /*
            1.2. Call cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet
                   with the same parameters.
            Expected: GT_OK and the same values than was set.
        */
        st = cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet(dev, &additionToLengthGet);

        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(additionToLength, additionToLengthGet,
                       "got another additionToLength then was set: %d", dev);
        }

        /* call with additionToLength[BIT_3] */
        additionToLength = BIT_3;

        st = cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet(dev, additionToLength);

        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }

        /* call get func again, 1.2. */
        st = cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet(dev, &additionToLengthGet);

        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(additionToLength, additionToLengthGet,
                       "got another additionToLength then was set: %d", dev);
        }

        /* call with additionToLength[BIT_6 - 1] */
        additionToLength = BIT_6 - 1;

        st = cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet(dev, additionToLength);

        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }

        /* call get func again, 1.2. */
        st = cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet(dev, &additionToLengthGet);

        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(additionToLength, additionToLengthGet,
                       "got another additionToLength then was set: %d", dev);
        }

        /*
            1.3. Call api with wrong additionToLength [BIT_6 + 1].
            Expected: NOT GT_OK.
        */
        additionToLength = BIT_6 + 1;

        st = cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet(dev, additionToLength);

        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }

        additionToLength = 0;
    }

    /* restore correct values */
    additionToLength = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E |
                                           UTF_CH3_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet(dev, additionToLength);

        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet(dev, additionToLength);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet
(
    IN  GT_U8    dev,
    IN  GT_U8    port,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS(DxChXcat)
    1.1.1. Call with not null enable pointer.
    Expected: GT_OK.
    1.1.2. Call api with wrong enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8    dev;
    GT_U8    port   = TUNNEL_VALID_PHY_PORT_CNS;
    GT_BOOL  enable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E |
                                           UTF_CH3_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not null enable pointer.
                Expected: GT_OK.
            */
            st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet(dev,
                                         port, &enable);
            if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
            }

            /*
                1.1.2. Call api with wrong enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet(dev,
                                         port, NULL);
            if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                       "%d, enablePtr = NULL", dev);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet(dev,
                                         port, &enable);
            if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet(dev,
                                     port, &enable);
        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet(dev,
                                     port, &enable);
        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
    }

    /* restore correct values */
    port = TUNNEL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E |
                                           UTF_CH3_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet(dev,
                                     port, &enable);
        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet(dev,
                                     port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
(
    IN  GT_U8   dev,
    IN  GT_U8   port,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS(DxChXcat)
    1.1.1. Call with enable[GT_TRUE / GT_FALSE],
    Expected: GT_OK.
    1.1.2. Call cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port = 0;
    GT_BOOL enable = GT_FALSE;
    GT_BOOL enableGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E |
                                           UTF_CH3_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable[GT_TRUE / GT_FALSE],
                Expected: GT_OK.
            */
            /* call with enable[GT_TRUE] */
            enable = GT_TRUE;

            st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet(dev,
                                         port, enable);
            if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
            }

            /*
                1.1.2. Call cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet(dev,
                                         port, &enableGet);
            if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet: %d ", dev);

                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                           "got another enable then was set: %d", dev);
            }

            /* call with enable[GT_FALSE] */
            enable = GT_FALSE;

            st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet(dev,
                                         port, enable);
            if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
            }

            /* call get func again, 1.1.2. */
            st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet(dev,
                                         port, &enableGet);
            if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
            }
            else
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet: %d ", dev);

                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                           "got another enable then was set: %d", dev);
            }
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet(dev,
                                         port, enable);
            if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
            }
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet(dev,
                                     port, enable);
        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet(dev,
                                     port, enable);
        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
    }

    /* restore correct values */
    port = TUNNEL_VALID_PHY_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E |
                                           UTF_CH3_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet(dev,
                                     port, enable);
        if (GT_TRUE != PRV_CPSS_DXCH_XCAT_A3_ONLY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        }
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet(dev,
                                     port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChTunnel suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChTunnel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelStartEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelStartEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpv4TunnelTermPortSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChIpv4TunnelTermPortGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChMplsTunnelTermPortSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChMplsTunnelTermPortGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelTermEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelTermEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelTermEntryInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelStartEgressFilteringSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelStartEgressFilteringGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChEthernetOverMplsTunnelStartTaggingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChEthernetOverMplsTunnelStartTaggingGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChEthernetOverMplsTunnelStartTagModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChEthernetOverMplsTunnelStartTagModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelStartPassengerVlanTranslationEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelStartPassengerVlanTranslationEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelTermExceptionCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelCtrlTtExceptionCmdGet)

    /* Tests for Table */
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelFillTunnelStartTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelFillTunnelTermTable)
    /* Ends of the tests for Table */

    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelStartPortGroupEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelStartPortGroupEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet)

UTF_SUIT_END_TESTS_MAC(cpssDxChTunnel)

