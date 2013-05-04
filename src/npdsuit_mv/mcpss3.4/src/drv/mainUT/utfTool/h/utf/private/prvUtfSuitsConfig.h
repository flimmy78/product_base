/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvUtfSuitsConfig.h
*
* DESCRIPTION:
*       Internal header which is included only by UTF code
*       and provides configuration for all suits.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
/* includes */
#include "prvUtfSuitsStart.h"

/* defines */
/* Provides more flexible switching between different configuration of suites */
#define PRV_UTF_SUITES_CONFIG_FILENAME_CNS "prvUtfSuitsConfig.h"

/* Uses macros to declare test-suite functions
 * and then implements utfDeclareAllSuits function
 * that calls test-suite functions.
 * Functions that are defined as arguments of UTF_MAIN_DECLARE_SUIT_MAC macros
 * will be declared and called.
 *
 * If you what add a Suit to the image please paste
 * UTF_MAIN_DECLARE_SUIT_MAC(name of Suit) here.
 */
UTF_MAIN_BEGIN_SUITS_MAC()
    /* declare test type for general suits */
    UTF_MAIN_DECLARE_TEST_TYPE_MAC(UTF_TEST_TYPE_GEN_E);

    /* Common UTs */
    UTF_MAIN_DECLARE_SUIT_MAC(cpssFormatConvert)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssBuffManagerPool)
    UTF_MAIN_DECLARE_SUIT_MAC(prvCpssMisc)


    /* Generic UTs */
    UTF_MAIN_DECLARE_SUIT_MAC(cpssGenPhyVct)

    /* finish declaration */
    UTF_MAIN_DECLARE_TEST_TYPE_MAC(UTF_TEST_TYPE_NONE_E);

    /* declare test type for Ex suits */
    UTF_MAIN_DECLARE_TEST_TYPE_MAC(UTF_TEST_TYPE_EX_E);
#if (defined EX_FAMILY) || (defined MX_FAMILY)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxBrgGen)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxBrgVlan)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxBrgFdb)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxBrgMc)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxBrgCount)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxBrgEgrFlt)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxBrgNestVlan)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxBrgPrvEdgeVlan)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxBrgStp)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPortBackEnd)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPortBufMg)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPortCtrl)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPortStat)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPortTx)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxMirror)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxStc)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxInlif)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxCosMap)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxTrunkHighLevel)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxTrunkLowLevel)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxCfgInit)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxL2Cos)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPcl)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxIpCtrl)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxVb)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxIp)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPolicer)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxIpFlow)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPolicy)
#endif

#ifdef TG_FAMILY
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxTgInlif)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxTgTrunk)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxTgPcl)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxTgBrgGen)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxTgIp)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxTgIpCtrl)
#endif /* #if (defined TG_FAMILY) */
    /* finish declaration */
    UTF_MAIN_DECLARE_TEST_TYPE_MAC(UTF_TEST_TYPE_NONE_E);

    /* declare test type for DxCh suits */
    UTF_MAIN_DECLARE_TEST_TYPE_MAC(UTF_TEST_TYPE_CHX_E);
#if (defined CHX_FAMILY)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChBrgCount)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChBrgEgrFlt)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChBrgFdb)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChBrgGen)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChBrgMc)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChBrgNestVlan)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChBrgPrvEdgeVlan)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChBrgSecurityBreach)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChBrgSrcId)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChBrgStp)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChBrgVlan)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChCnc)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChCfgInit)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChCos)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChCscd)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChCutThrough)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChDiag)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChIp)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChIpCtrl)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChIpFix)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChIpLpm)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChHwInit)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChHwInitLedCtrl)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChLogicalTargetMapping)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChMirror)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChStc)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChNetIf)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChNetIfMii)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChNst)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChNstPortIsolation)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChPcl)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChPhySmi)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChPhySmiPreInit)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChPolicer)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChPortBufMg)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChPortCn)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChPortPfc)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChPortSyncEther)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChPortCtrl)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChPortStat)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChPortTx)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChPtp)
/*    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChTcamManager)*/
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChTrunkHighLevel)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChTrunkLowLevel)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChTti)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChTunnel)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChVersion)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxChVnt)
#endif /* (defined CHX_FAMILY) */

#if (defined CH3_FAMILY)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxCh3Policer)
#endif /* (defined CH3_FAMILY) */

#if (defined CH3P_FAMILY)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxCh3pBrgCapwap)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxCh3pBrgMc)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxCh3pBrgVlan)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxCh3pCapwapGen)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxCh3pIp)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxCh3pTti)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxCh3pTunnel)
#endif /* (defined CH3P_FAMILY) */
    /* finish declaration */
    UTF_MAIN_DECLARE_TEST_TYPE_MAC(UTF_TEST_TYPE_NONE_E);

    /* declare test type for Salsa suits */
    UTF_MAIN_DECLARE_TEST_TYPE_MAC(UTF_TEST_TYPE_SAL_E);
#if (defined SAL_FAMILY)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalBrgEgrFlt)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalBrgFdb)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalBrgGen)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalBrgPrvEdgeVlan)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalBrgStp)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalBrgVlan)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalCfgInit)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalDscp2Cos)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalL2Cos)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalCscd)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalMirror)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalPortBufMg)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalPortCtrl)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalPortStat)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssDxSalPortTx)
#endif /* (defined SAL_FAMILY) */
    /* finish declaration */
    UTF_MAIN_DECLARE_TEST_TYPE_MAC(UTF_TEST_TYPE_NONE_E);

    /* declare test type for Puma suits */
    UTF_MAIN_DECLARE_TEST_TYPE_MAC(UTF_TEST_TYPE_PM_E);
#if (defined EXMXPM_FAMILY)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmBrgCount)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmBrgEgrFlt)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmBrgFdb)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmBrgFdbHash)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmBrgGen)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmBrgMc)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmBrgNestVlan)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmBrgPrvEdgeVlan)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmBrgSecurityBreach)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmBrgSrcId)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmBrgStp)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmBrgVlan)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmCfg)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmCounter)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmDit)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmExternalMemory)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmExternalTcam)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmIntTcam)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmInlif)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmIpCtrl)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmIpStat)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmIpTables)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmIpLpm)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmMirror)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmMpls)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmStc)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmNetIf)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmNstBrg)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmNstIp)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmNstPort)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmPcl)
#ifndef _WIN32
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmPhySmi)
#endif
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmPolicer)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmPortBufMg)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmPortCtrl)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmPortStat)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmPortTx)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmQos)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmSct)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmTrunkHighLevel)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmTrunkLowLevel)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmTti)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmTunnelStart)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmVb)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmVnt)

    /* UTs for Puma Fabric Adapter */
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmFabricConfig)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmFabricE2e)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmFabricHGLink)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmFabricPktReasm)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmFabricPktSegm)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmFabricSerdes)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmFabricVoq)
    UTF_MAIN_DECLARE_SUIT_MAC(cpssExMxPmFabricXbar)
#endif /* (defined EXMXPM_FAMILY) */
    /* finish declaration */
    UTF_MAIN_DECLARE_TEST_TYPE_MAC(UTF_TEST_TYPE_NONE_E);

#ifdef IMPL_FA
    UTF_MAIN_DECLARE_SUIT_MAC(gtFaApiGen)
    UTF_MAIN_DECLARE_SUIT_MAC(gtVoqApi)
    UTF_MAIN_DECLARE_SUIT_MAC(gtFaE2eApi)
    UTF_MAIN_DECLARE_SUIT_MAC(gtFaApiRxReassem)
    UTF_MAIN_DECLARE_SUIT_MAC(gtFaApiTxArbiter)
#endif /* IMPL_FA */

#ifdef IMPL_XBAR
    UTF_MAIN_DECLARE_SUIT_MAC(gtCpuMail)
    UTF_MAIN_DECLARE_SUIT_MAC(gtHyperGlink)
    UTF_MAIN_DECLARE_SUIT_MAC(gtXbar)
    UTF_MAIN_DECLARE_SUIT_MAC(gtXbarGpp)
    UTF_MAIN_DECLARE_SUIT_MAC(gtXbarLed)
#endif /* IMPL_XBAR */

    /* declare test type for Enhanced UT suits */
    UTF_MAIN_DECLARE_TEST_TYPE_MAC(UTF_TEST_TYPE_TRAFFIC_E);
#ifdef IMPL_TGF
    UTF_MAIN_DECLARE_SUIT_MAC(tgfTailDrop)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfCscd)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfCnc)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfCutThrough)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfPort)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfLogicalTarget)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfBridge)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfTunnel)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfIp)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfNst)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfPcl)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfIpLpmEngine)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfVnt)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfPolicer)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfMirror)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfTrunk)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfIpfix)
    UTF_MAIN_DECLARE_SUIT_MAC(tgfCos)
#endif /* IMPL_TGF */
    /* finish declaration */
    UTF_MAIN_DECLARE_TEST_TYPE_MAC(UTF_TEST_TYPE_NONE_E);

UTF_MAIN_END_SUITS_MAC()

/* includes */
#include "prvUtfSuitsEnd.h"

