/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmDitUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmDit, that provides
*       The CPSS EXMXPM Dit APIs.
*       Dit entry can hold dual Ip Mc(in internal memory) or single Ip/Mpls or
*       single Vpls (in internal or external memory). These APIs handle
*       writing/reading different kinds of entry types.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/dit/cpssExMxPmDitTypes.h>
#include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpTypes.h>
#include <cpss/exMxPm/exMxPmGen/dit/cpssExMxPmDit.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Invalid enum */
#define DIT_INVALID_ENUM_CNS            0x5AAAAAA5

#define DIT_TESTED_VLAN_ID_CNS          100

/* dit module pointer */
#define PRV_CPSS_EXMXPM_DIT_MODULE_MAC(devNum)           \
                            (&(PRV_CPSS_EXMXPM_DEV_MODULE_CFG_MAC(devNum)->ditCfg))


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitIpMcPairWrite
(
    IN  GT_U8                                           devNum,
    IN  GT_U32                                          ditPairEntryIndex,
    IN  CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_MODE_ENT  ditPairWriteForm,
    IN  CPSS_EXMXPM_DIT_IP_MC_PAIR_STC                 *ditPairEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitIpMcPairWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ditPairEntryIndex [0],
                   ditPairWriteForm [CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E]
                   and ditPairEntryPtr->firstDitNode {ditRpfFailCommand [CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                                                      isTunnelStart [GT_FALSE],
                                                      nextHopIf {type [CPSS_INTERFACE_VID_E],
                                                                 vlanId [100]},
                                                      nextHopVlanId [100],
                                                      ttlHopLimitThreshold [255],
                                                      excludeSourceVlanIf [GT_TRUE]}.
    Expected: GT_OK.
    1.2. Call with ditPairEntryIndex [100],
                   ditPairWriteForm [CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_SECOND_DIT_NEXT_POINTER_ONLY_E],
                   ditPairEntryPtr->secondDitNode {ditRpfFailCommand [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E],
                                                   isTunnelStart [GT_TRUE],
                                                   nextHopTunnelPointer [0x3FFF],
                                                   nextHopIf {type [CPSS_INTERFACE_TRUNK_E],
                                                              trunkId [2]},
                                                   nextHopVlanId [100],
                                                   ttlHopLimitThreshold [0],
                                                   excludeSourceVlanIf [GT_TRUE]}
                   and ditPairEntryPtr->ditNextPointer [1000].
    Expected: GT_OK.
    1.3. Call with ditPairEntryIndex [274],
                   ditPairWriteForm [CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_WHOLE_E],
                   ditPairEntryPtr->firstDitNode {ditRpfFailCommand [CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                                                  isTunnelStart [GT_FALSE],
                                                  nextHopIf {type [CPSS_INTERFACE_VID_E], vlanId [100]},
                                                  nextHopVlanId [100],
                                                  ttlHopLimitThreshold [255],
                                                  excludeSourceVlanIf [GT_TRUE]},
                   ditPairEntryPtr->secondDitNode {ditRpfFailCommand [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E],
                                                   isTunnelStart [GT_TRUE],
                                                   nextHopTunnelPointer [0x3FFF],
                                                   nextHopIf {type [CPSS_INTERFACE_TRUNK_E], trunkId [2]},
                                                   nextHopVlanId [100],
                                                   ttlHopLimitThreshold [0],
                                                   excludeSourceVlanIf [GT_TRUE]}
                   and ditPairEntryPtr->ditNextPointer [0xFFFFF].
    Expected: GT_OK.
    1.4. Call cpssExMxPmDitIpMcPairRead with the same ditPairEntryIndex as in 1.1, 1.2, 1.3
                                           and non-NULL ditPairEntryPtr.
    Expected: GT_OK and the same ditPairEntryPtr.
    1.5. Call with out of range ditPairEntryIndex [3072]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range ditPairWriteForm [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with out of range ditPairEntryPtr->firstDitNode.ditRpfFailCommand [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with ditPairEntryPtr->firstDitNode.ditRpfFailCommand [CPSS_PACKET_CMD_FORWARD_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.10. Call with ditPairEntryPtr->firstDitNode.isTunnelStart [GT_TRUE],
                    out of range ditPairEntryPtr->firstDitNode.nextHopTunnelPointer [16384]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.11. Call with ditPairEntryPtr->firstDitNode.isTunnelStartt [GT_FALSE],
                    out of range ditPairEntryPtr->firstDitNode.nextHopTunnelPointer [16384] (not relevant)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.12. Call with ditPairEntryPtr->firstDitNode.isTunnelStart [GT_FALSE],
                    out of range ditPairEntryPtr->firstDitNode.nextHopIf.type [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.13. Call with ditPairEntryPtr->firstDitNode.isTunnelStart [GT_TRUE],
                    out of range ditPairEntryPtr->firstDitNode.nextHopIf.type [0x5AAAAAA5] (not relevant)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.14. Call with ditPairEntryPtr->firstDitNode.isTunnelStart [GT_FALSE],
                    ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_VID_E],
                    out of range ditPairEntryPtr->firstDitNode.nextHopIf.vidx [PRV_CPSS_MAX_NUM_VLANS_CNS] (out of range)
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.15. Call with ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_PORT_E],
                    out of range ditPairEntryPtr->firstDitNode.nextHopIf.vidx [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.16. Call with ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_PORT_E],
                    out of range ditPairEntryPtr->firstDitNode.nextHopIf.devPort.portNum [64] (out of ramge)
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.17. Call with ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_PORT_E],
                    out of range ditPairEntryPtr->firstDitNode.nextHopIf.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of ramge)
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.18. Call with ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_TRUNK_E],
                    out of range ditPairEntryPtr->firstDitNode.nextHopIf.devPort.portNum [64] (not relevant)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.19. Call with ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_TRUNK_E],
                    out of range ditPairEntryPtr->firstDitNode.nextHopIf.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.20. Call with ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_TRUNK_E],
                    out of range ditPairEntryPtr->firstDitNode.nextHopIf.trunkId [256] (out of ramge)
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.21. Call with ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_VID_E],
                    out of range ditPairEntryPtr->firstDitNode.nextHopIf.trunkId [256] (not relevant)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.22. Call with out of range ditPairEntryPtr->firstDitNode.nextHopVlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.23. Call with out of range ditPairEntryPtr->firstDitNode.ttlHopLimitThreshold [256]
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.24. Call with out of range ditPairEntryPtr->ditNextPointer [0x100000]
                    and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.25. Call with ditPairEntryPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                          ditPairEntryIndex = 0;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_MODE_ENT  ditPairWriteForm  = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_STC                  ditPairEntry;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_STC                  ditPairEntryGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ditPairEntryIndex [0],
                           ditPairWriteForm [CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E]
                           and ditPairEntryPtr->firstDitNode {ditRpfFailCommand [CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                                                              isTunnelStart [GT_FALSE],
                                                              nextHopIf {type [CPSS_INTERFACE_VIDX_E],
                                                                         vlanId [100]},
                                                              nextHopVlanId [100],
                                                              ttlHopLimitThreshold [255],
                                                              excludeSourceVlanIf [GT_TRUE]}.
            Expected: GT_OK.
        */
        ditPairEntryIndex = 0;
        ditPairWriteForm  = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E;

        ditPairEntry.firstDitNode.ditRpfFailCommand    = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
        ditPairEntry.firstDitNode.isTunnelStart        = GT_FALSE;
        ditPairEntry.firstDitNode.nextHopTunnelPointer = 0x3FFF;
        ditPairEntry.firstDitNode.nextHopIf.type       = CPSS_INTERFACE_VIDX_E;
        ditPairEntry.firstDitNode.nextHopIf.vidx       = DIT_TESTED_VLAN_ID_CNS;
        ditPairEntry.firstDitNode.nextHopVlanId        = DIT_TESTED_VLAN_ID_CNS;
        ditPairEntry.firstDitNode.ttlHopLimitThreshold = 255;
        ditPairEntry.firstDitNode.excludeSourceVlanIf  = GT_TRUE;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ditPairEntryIndex, ditPairWriteForm);

        /*
            1.4. Call cpssExMxPmDitIpMcPairRead with the same ditPairEntryIndex as in 1.1, 1.2, 1.3
                                                   and non-NULL ditPairEntryPtr.
            Expected: GT_OK and the same ditPairEntryPtr.
        */
        st = cpssExMxPmDitIpMcPairRead(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntryGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmDitIpMcPairRead: %d, %d, %d", dev, ditPairEntryIndex, ditPairWriteForm);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.ditRpfFailCommand,
                                     ditPairEntryGet.firstDitNode.ditRpfFailCommand,
                   "get another ditPairEntryPtr->firstDitNode.ditRpfFailCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.isTunnelStart,
                                     ditPairEntryGet.firstDitNode.isTunnelStart,
                   "get another ditPairEntryPtr->firstDitNode.isTunnelStart than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.nextHopIf.type,
                                     ditPairEntryGet.firstDitNode.nextHopIf.type,
                   "get another ditPairEntryPtr->firstDitNode.nextHopIf.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.nextHopIf.vidx,
                                     ditPairEntryGet.firstDitNode.nextHopIf.vidx,
                   "get another ditPairEntryPtr->firstDitNode.nextHopIf.vidx than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.nextHopVlanId,
                                     ditPairEntryGet.firstDitNode.nextHopVlanId,
                   "get another ditPairEntryPtr->firstDitNode.nextHopVlanId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.ttlHopLimitThreshold,
                                     ditPairEntryGet.firstDitNode.ttlHopLimitThreshold,
                   "get another ditPairEntryPtr->firstDitNode.ttlHopLimitThreshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.excludeSourceVlanIf,
                                     ditPairEntryGet.firstDitNode.excludeSourceVlanIf,
                   "get another ditPairEntryPtr->firstDitNode.excludeSourceVlanIf than was set: %d", dev);

        /*
            1.2. Call with ditPairEntryIndex [100],
                           ditPairWriteForm [CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_SECOND_DIT_NEXT_POINTER_ONLY_E],
                           ditPairEntryPtr->secondDitNode {ditRpfFailCommand [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E],
                                                           isTunnelStart [GT_TRUE],
                                                           nextHopTunnelPointer [0x3FFF],
                                                           nextHopIf {type [CPSS_INTERFACE_TRUNK_E],
                                                                      trunkId [2]},
                                                           nextHopVlanId [100],
                                                           ttlHopLimitThreshold [0],
                                                           excludeSourceVlanIf [GT_TRUE]}
                           and ditPairEntryPtr->ditNextPointer [1000].
            Expected: GT_OK.
        */
        ditPairEntryIndex = 100;
        ditPairWriteForm  = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_SECOND_DIT_NEXT_POINTER_ONLY_E;

        ditPairEntry.secondDitNode.ditRpfFailCommand    = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
        ditPairEntry.secondDitNode.isTunnelStart        = GT_TRUE;
        ditPairEntry.secondDitNode.nextHopTunnelPointer = 0x3FFF;
        ditPairEntry.secondDitNode.nextHopIf.type       = CPSS_INTERFACE_TRUNK_E;
        ditPairEntry.secondDitNode.nextHopIf.trunkId    = 2;
        ditPairEntry.secondDitNode.nextHopVlanId        = DIT_TESTED_VLAN_ID_CNS;
        ditPairEntry.secondDitNode.ttlHopLimitThreshold = 0;
        ditPairEntry.secondDitNode.excludeSourceVlanIf  = GT_TRUE;
        ditPairEntry.secondDitNode.isLast               = GT_TRUE;

        ditPairEntry.ditNextPointer = 0;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ditPairEntryIndex, ditPairWriteForm);

        /*
            1.4. Call cpssExMxPmDitIpMcPairRead with the same ditPairEntryIndex as in 1.1, 1.2, 1.3
                                                   and non-NULL ditPairEntryPtr.
            Expected: GT_OK and the same ditPairEntryPtr.
        */
        st = cpssExMxPmDitIpMcPairRead(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntryGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmDitIpMcPairRead: %d, %d, %d", dev, ditPairEntryIndex, ditPairEntryIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.ditRpfFailCommand,
                                     ditPairEntryGet.secondDitNode.ditRpfFailCommand,
                   "get another ditPairEntryPtr->secondDitNode.ditRpfFailCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.isTunnelStart,
                                     ditPairEntryGet.secondDitNode.isTunnelStart,
                   "get another ditPairEntryPtr->secondDitNode.isTunnelStart than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.nextHopTunnelPointer,
                                     ditPairEntryGet.secondDitNode.nextHopTunnelPointer,
                   "get another ditPairEntryPtr->secondDitNode.nextHopTunnelPointer than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.nextHopIf.type,
                                     ditPairEntryGet.secondDitNode.nextHopIf.type,
                   "get another ditPairEntryPtr->secondDitNode.nextHopIf.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.nextHopIf.trunkId,
                                     ditPairEntryGet.secondDitNode.nextHopIf.trunkId,
                   "get another ditPairEntryPtr->secondDitNode.nextHopIf.trunkId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.nextHopVlanId,
                                     ditPairEntryGet.secondDitNode.nextHopVlanId,
                   "get another ditPairEntryPtr->secondDitNode.nextHopVlanId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.ttlHopLimitThreshold,
                                     ditPairEntryGet.secondDitNode.ttlHopLimitThreshold,
                   "get another ditPairEntryPtr->secondDitNode.ttlHopLimitThreshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.excludeSourceVlanIf,
                                     ditPairEntryGet.secondDitNode.excludeSourceVlanIf,
                   "get another ditPairEntryPtr->secondDitNode.excludeSourceVlanIf than was set: %d", dev);

        /*
            1.3. Call with ditPairEntryIndex [274],
                           ditPairWriteForm [CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_WHOLE_E],
                           ditPairEntryPtr->firstDitNode {ditRpfFailCommand [CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                                                          isTunnelStart [GT_FALSE],
                                                          nextHopIf {type [CPSS_INTERFACE_VID_E], vlanId [100]},
                                                          nextHopVlanId [100],
                                                          ttlHopLimitThreshold [255],
                                                          excludeSourceVlanIf [GT_TRUE]},
                           ditPairEntryPtr->secondDitNode {ditRpfFailCommand [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E],
                                                           isTunnelStart [GT_TRUE],
                                                           nextHopTunnelPointer [0x3FFF],
                                                           nextHopIf {type [CPSS_INTERFACE_TRUNK_E], trunkId [2]},
                                                           nextHopVlanId [100],
                                                           ttlHopLimitThreshold [0],
                                                           excludeSourceVlanIf [GT_TRUE]}
                           and ditPairEntryPtr->ditNextPointer [0xFFFFF].
            Expected: GT_OK.
        */
        ditPairEntryIndex = 274;
        ditPairWriteForm  = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_WHOLE_E;

        ditPairEntry.firstDitNode.ditRpfFailCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
        ditPairEntry.firstDitNode.isTunnelStart     = GT_FALSE;
        ditPairEntry.firstDitNode.nextHopIf.type    = CPSS_INTERFACE_VIDX_E;
        ditPairEntry.firstDitNode.nextHopIf.vidx    = DIT_TESTED_VLAN_ID_CNS;
        ditPairEntry.firstDitNode.nextHopVlanId     = DIT_TESTED_VLAN_ID_CNS;
        ditPairEntry.firstDitNode.ttlHopLimitThreshold = 255;
        ditPairEntry.firstDitNode.excludeSourceVlanIf  = GT_TRUE;
        ditPairEntry.firstDitNode.isLast               = GT_FALSE;

        ditPairEntry.secondDitNode.ditRpfFailCommand    = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
        ditPairEntry.secondDitNode.isTunnelStart        = GT_TRUE;
        ditPairEntry.secondDitNode.nextHopTunnelPointer = 0x3FFF;
        ditPairEntry.secondDitNode.nextHopIf.type       = CPSS_INTERFACE_TRUNK_E;
        ditPairEntry.secondDitNode.nextHopIf.trunkId    = 2;
        ditPairEntry.secondDitNode.nextHopVlanId        = DIT_TESTED_VLAN_ID_CNS;
        ditPairEntry.secondDitNode.ttlHopLimitThreshold = 0;
        ditPairEntry.secondDitNode.excludeSourceVlanIf  = GT_TRUE;

        ditPairEntry.ditNextPointer = 0xFFFFF;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ditPairEntryIndex, ditPairWriteForm);

        /*
            1.4. Call cpssExMxPmDitIpMcPairRead with the same ditPairEntryIndex as in 1.1, 1.2, 1.3
                                                   and non-NULL ditPairEntryPtr.
            Expected: GT_OK and the same ditPairEntryPtr.
        */
        st = cpssExMxPmDitIpMcPairRead(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntryGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmDitIpMcPairRead: %d, %d, %d", dev, ditPairEntryIndex, ditPairEntryIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.ditRpfFailCommand,
                                     ditPairEntryGet.firstDitNode.ditRpfFailCommand,
                   "get another ditPairEntryPtr->firstDitNode.ditRpfFailCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.isTunnelStart,
                                     ditPairEntryGet.firstDitNode.isTunnelStart,
                   "get another ditPairEntryPtr->firstDitNode.isTunnelStart than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.nextHopIf.type,
                                     ditPairEntryGet.firstDitNode.nextHopIf.type,
                   "get another ditPairEntryPtr->firstDitNode.nextHopIf.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.nextHopIf.vidx,
                                     ditPairEntryGet.firstDitNode.nextHopIf.vidx,
                   "get another ditPairEntryPtr->firstDitNode.nextHopIf.vidx than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.nextHopVlanId,
                                     ditPairEntryGet.firstDitNode.nextHopVlanId,
                   "get another ditPairEntryPtr->firstDitNode.nextHopVlanId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.ttlHopLimitThreshold,
                                     ditPairEntryGet.firstDitNode.ttlHopLimitThreshold,
                   "get another ditPairEntryPtr->firstDitNode.ttlHopLimitThreshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.excludeSourceVlanIf,
                                     ditPairEntryGet.firstDitNode.excludeSourceVlanIf,
                   "get another ditPairEntryPtr->firstDitNode.excludeSourceVlanIf than was set: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.ditRpfFailCommand,
                                     ditPairEntryGet.secondDitNode.ditRpfFailCommand,
                   "get another ditPairEntryPtr->secondDitNode.ditRpfFailCommand than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.isTunnelStart,
                                     ditPairEntryGet.secondDitNode.isTunnelStart,
                   "get another ditPairEntryPtr->secondDitNode.isTunnelStart than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.nextHopTunnelPointer,
                                     ditPairEntryGet.secondDitNode.nextHopTunnelPointer,
                   "get another ditPairEntryPtr->secondDitNode.nextHopTunnelPointer than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.nextHopIf.type,
                                     ditPairEntryGet.secondDitNode.nextHopIf.type,
                   "get another ditPairEntryPtr->secondDitNode.nextHopIf.type than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.nextHopIf.trunkId,
                                     ditPairEntryGet.secondDitNode.nextHopIf.trunkId,
                   "get another ditPairEntryPtr->secondDitNode.nextHopIf.trunkId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.nextHopVlanId,
                                     ditPairEntryGet.secondDitNode.nextHopVlanId,
                   "get another ditPairEntryPtr->secondDitNode.nextHopVlanId than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.ttlHopLimitThreshold,
                                     ditPairEntryGet.secondDitNode.ttlHopLimitThreshold,
                   "get another ditPairEntryPtr->secondDitNode.ttlHopLimitThreshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.secondDitNode.excludeSourceVlanIf,
                                     ditPairEntryGet.secondDitNode.excludeSourceVlanIf,
                   "get another ditPairEntryPtr->secondDitNode.excludeSourceVlanIf than was set: %d", dev);

        /*
            1.5. Call with out of range ditPairEntryIndex [3072]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ditPairEntryIndex = 3072;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ditPairEntryIndex);

        ditPairEntryIndex = 0;

        /*
            1.7. Call with out of range ditPairWriteForm [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ditPairWriteForm = DIT_INVALID_ENUM_CNS;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditPairWriteForm = %d", dev, ditPairWriteForm);

        ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E;

        /*
            1.8. Call with out of range ditPairEntryPtr->firstDitNode.ditRpfFailCommand [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ditPairEntry.firstDitNode.ditRpfFailCommand = DIT_INVALID_ENUM_CNS;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditPairEntryPtr->firstDitNode.ditRpfFailCommand = %d",
                                     dev, ditPairEntry.firstDitNode.ditRpfFailCommand);

        /*
            1.9. Call with ditPairEntryPtr->firstDitNode.ditRpfFailCommand [CPSS_PACKET_CMD_FORWARD_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ditPairEntry.firstDitNode.ditRpfFailCommand = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditPairEntryPtr->firstDitNode.ditRpfFailCommand = %d",
                                         dev, ditPairEntry.firstDitNode.ditRpfFailCommand);

        ditPairEntry.firstDitNode.ditRpfFailCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        /*
            1.10. Call with ditPairEntryPtr->firstDitNode.isTunnelStartout [GT_TRUE],
                            out of range ditPairEntryPtr->firstDitNode.nextHopTunnelPointer [16384]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ditPairEntry.firstDitNode.isTunnelStart        = GT_TRUE;
        ditPairEntry.firstDitNode.nextHopTunnelPointer = 16384;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->firstDitNode.isTunnelStart = %d, ->firstDitNode.nextHopTunnelPointer = %d",
                                         dev, ditPairEntry.firstDitNode.isTunnelStart, ditPairEntry.firstDitNode.nextHopTunnelPointer);

        /*
            1.11. Call with ditPairEntryPtr->firstDitNode.isTunnelStartout [GT_FALSE],
                            out of range ditPairEntryPtr->firstDitNode.nextHopTunnelPointer [16384] (not relevant)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        ditPairEntry.firstDitNode.isTunnelStart = GT_FALSE;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->firstDitNode.isTunnelStart = %d, ->firstDitNode.nextHopTunnelPointer = %d",
                                     dev, ditPairEntry.firstDitNode.isTunnelStart, ditPairEntry.firstDitNode.nextHopTunnelPointer);

        ditPairEntry.firstDitNode.nextHopTunnelPointer = 0;

        /*
            1.12. Call with ditPairEntryPtr->firstDitNode.isTunnelStartout [GT_FALSE],
                            out of range ditPairEntryPtr->firstDitNode.nextHopIf.type [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ditPairEntry.firstDitNode.isTunnelStart  = GT_FALSE;
        ditPairEntry.firstDitNode.nextHopIf.type = DIT_INVALID_ENUM_CNS;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->firstDitNode.isTunnelStart = %d, ->firstDitNode.nextHopIf.type = %d",
                                     dev, ditPairEntry.firstDitNode.isTunnelStart, ditPairEntry.firstDitNode.nextHopIf.type);

        /*
            1.13. Call with ditPairEntryPtr->firstDitNode.isTunnelStartout [GT_TRUE],
                            out of range ditPairEntryPtr->firstDitNode.nextHopIf.type [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ditPairEntry.firstDitNode.isTunnelStart = GT_TRUE;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->firstDitNode.isTunnelStart = %d, ->firstDitNode.nextHopIf.type = %d",
                                     dev, ditPairEntry.firstDitNode.isTunnelStart, ditPairEntry.firstDitNode.nextHopIf.type);

        ditPairEntry.firstDitNode.nextHopIf.type = CPSS_INTERFACE_VID_E;

        /*
            1.14. Call with ditPairEntryPtr->firstDitNode.isTunnelStart [GT_FALSE],
                            ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_VID_E],
                            out of range ditPairEntryPtr->firstDitNode.nextHopIf.vidx [PRV_CPSS_MAX_NUM_VLANS_CNS]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ditPairEntry.firstDitNode.isTunnelStart  = GT_FALSE;
        ditPairEntry.firstDitNode.nextHopIf.type = CPSS_INTERFACE_VID_E;
        ditPairEntry.firstDitNode.nextHopIf.vidx = 4096;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, ->firstDitNode.isTunnelStart = %d, ->firstDitNode.nextHopIf.vidx = %d",
                                     dev, ditPairEntry.firstDitNode.isTunnelStart, ditPairEntry.firstDitNode.nextHopIf.vidx);

        ditPairEntry.firstDitNode.nextHopIf.vidx = DIT_TESTED_VLAN_ID_CNS;

        /*
            1.15. Call with ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_PORT_E],
                            out of range ditPairEntryPtr->firstDitNode.nextHopIf.vidx [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        ditPairEntry.firstDitNode.nextHopIf.type = CPSS_INTERFACE_PORT_E;
        ditPairEntry.firstDitNode.nextHopIf.devPort.devNum = 0;
        ditPairEntry.firstDitNode.nextHopIf.devPort.portNum = 0;
        ditPairEntry.firstDitNode.nextHopIf.vidx = 4096;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->firstDitNode.nextHopIf.type = %d, ->firstDitNode.nextHopIf.vidx = %d",
                                     dev, ditPairEntry.firstDitNode.nextHopIf.type, ditPairEntry.firstDitNode.nextHopIf.vidx);

        ditPairEntry.firstDitNode.nextHopIf.vidx = DIT_TESTED_VLAN_ID_CNS;

        /*
            1.16. Call with ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_PORT_E],
                            out of range ditPairEntryPtr->firstDitNode.nextHopIf.devPort.portNum [64] (out of ramge)
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ditPairEntry.firstDitNode.nextHopIf.type = CPSS_INTERFACE_PORT_E;
        ditPairEntry.firstDitNode.nextHopIf.devPort.portNum = 64;
        ditPairEntry.firstDitNode.nextHopIf.devPort.devNum = dev;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->firstDitNode.nextHopIf.type = %d, ->firstDitNode.nextHopIf.devPort.portNum = %d",
                                         dev, ditPairEntry.firstDitNode.nextHopIf.type, ditPairEntry.firstDitNode.nextHopIf.devPort.portNum);

        ditPairEntry.firstDitNode.nextHopIf.devPort.portNum = 0;

        /*
            1.17. Call with ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_PORT_E],
                            out of range ditPairEntryPtr->firstDitNode.nextHopIf.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (out of ramge)
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ditPairEntry.firstDitNode.nextHopIf.type = CPSS_INTERFACE_PORT_E;
        ditPairEntry.firstDitNode.nextHopIf.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->firstDitNode.nextHopIf.type = %d, ->firstDitNode.nextHopIf.devPort.devNum = %d",
                                         dev, ditPairEntry.firstDitNode.nextHopIf.type, ditPairEntry.firstDitNode.nextHopIf.devPort.devNum);

        ditPairEntry.firstDitNode.nextHopIf.devPort.devNum = dev;

        /*
            1.18. Call with ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_TRUNK_E],
                            out of range ditPairEntryPtr->firstDitNode.nextHopIf.devPort.portNum [64] (not relevant)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        ditPairEntry.firstDitNode.nextHopIf.type = CPSS_INTERFACE_TRUNK_E;
        ditPairEntry.firstDitNode.nextHopIf.trunkId = 0;
        ditPairEntry.firstDitNode.nextHopIf.devPort.portNum = 64;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->firstDitNode.nextHopIf.type = %d, ->firstDitNode.nextHopIf.devPort.portNum = %d",
                                     dev, ditPairEntry.firstDitNode.nextHopIf.type, ditPairEntry.firstDitNode.nextHopIf.devPort.portNum);

        ditPairEntry.firstDitNode.nextHopIf.devPort.portNum = 0;

        /*
            1.19. Call with ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_TRUNK_E],
                            out of range ditPairEntryPtr->firstDitNode.nextHopIf.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        ditPairEntry.firstDitNode.nextHopIf.type = CPSS_INTERFACE_TRUNK_E;
        ditPairEntry.firstDitNode.nextHopIf.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->firstDitNode.nextHopIf.type = %d, ->firstDitNode.nextHopIf.devPort.devNum = %d",
                                     dev, ditPairEntry.firstDitNode.nextHopIf.type, ditPairEntry.firstDitNode.nextHopIf.devPort.devNum);

        ditPairEntry.firstDitNode.nextHopIf.devPort.devNum = dev;

        /*
            1.20. Call with ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_TRUNK_E],
                            out of range ditPairEntryPtr->firstDitNode.nextHopIf.trunkId [256] (out of ramge)
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ditPairEntry.firstDitNode.nextHopIf.type = CPSS_INTERFACE_TRUNK_E;
        ditPairEntry.firstDitNode.nextHopIf.trunkId = 256;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->firstDitNode.nextHopIf.type = %d, ->firstDitNode.nextHopIf.trunkId = %d",
                                         dev, ditPairEntry.firstDitNode.nextHopIf.type, ditPairEntry.firstDitNode.nextHopIf.trunkId);

        ditPairEntry.firstDitNode.nextHopIf.trunkId = 0;

        /*
            1.21. Call with ditPairEntryPtr->firstDitNode.nextHopIf.type [CPSS_INTERFACE_VID_E],
                            out of range ditPairEntryPtr->firstDitNode.nextHopIf.trunkId [256] (not relevant)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        /* interface type of VID is not supported */
        /*ditPairEntry.firstDitNode.nextHopIf.type = CPSS_INTERFACE_VID_E;
        ditPairEntry.firstDitNode.nextHopIf.trunkId = 256;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->firstDitNode.nextHopIf.type = %d, ->firstDitNode.nextHopIf.trunkId = %d",
                                     dev, ditPairEntry.firstDitNode.nextHopIf.type, ditPairEntry.firstDitNode.nextHopIf.trunkId);

        ditPairEntry.firstDitNode.nextHopIf.trunkId = 0;*/

        /*
            1.22. Call with out of range ditPairEntryPtr->firstDitNode.nextHopVlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ditPairEntry.firstDitNode.nextHopVlanId = 4096;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ->firstDitNode.nextHopVlanId = %d",
                                     dev, ditPairEntry.firstDitNode.nextHopVlanId);

        ditPairEntry.firstDitNode.nextHopVlanId = DIT_TESTED_VLAN_ID_CNS;

        /*
            1.23. Call with out of range ditPairEntryPtr->firstDitNode.ttlHopLimitThreshold [256]
                            and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ditPairEntry.firstDitNode.ttlHopLimitThreshold = 256;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ->firstDitNode.ttlHopLimitThreshold = %d",
                                         dev, ditPairEntry.firstDitNode.ttlHopLimitThreshold);

        ditPairEntry.firstDitNode.ttlHopLimitThreshold = 255;

        /*
            1.24. Call with out of range ditPairEntryPtr->ditNextPointer [0x100000]
                            and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_WHOLE_E;
        ditPairEntry.ditNextPointer = 0x100000;

        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditPairEntryPtr->ditNextPointer = %d",
                                         dev, ditPairEntry.ditNextPointer);

        ditPairEntry.ditNextPointer = 0xFFFFF;

        /*
            1.25. Call with ditPairEntryPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ditPairEntryPtr = NULL", dev);
    }

    ditPairEntryIndex = 0;
    ditPairWriteForm  = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E;

    ditPairEntry.firstDitNode.ditRpfFailCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    ditPairEntry.firstDitNode.isTunnelStart     = GT_FALSE;
    ditPairEntry.firstDitNode.nextHopIf.type    = CPSS_INTERFACE_VID_E;
    ditPairEntry.firstDitNode.nextHopIf.vidx    = DIT_TESTED_VLAN_ID_CNS;
    ditPairEntry.firstDitNode.nextHopVlanId     = DIT_TESTED_VLAN_ID_CNS;
    ditPairEntry.firstDitNode.ttlHopLimitThreshold = 255;
    ditPairEntry.firstDitNode.excludeSourceVlanIf  = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitIpMcPairWrite(dev, ditPairEntryIndex, ditPairWriteForm, &ditPairEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitIpMcPairRead
(
    IN  GT_U8                                           devNum,
    IN  GT_U32                                          ditPairEntryIndex,
    IN  CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_MODE_ENT  ditPairReadForm,
    OUT CPSS_EXMXPM_DIT_IP_MC_PAIR_STC                  *ditPairEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitIpMcPairRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ditPairEntryIndex [0 / 274],
                   ditPairWriteForm [CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E /
                                     CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_SECOND_DIT_NEXT_POINTER_ONLY_E],
                   and non-NULL ditPairEntryPtr.
    Expected: GT_OK.
    1.2. Call with out of range ditPairEntryIndex [3072]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range ditPairWriteForm [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with ditPairEntryPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                          ditPairEntryIndex = 0;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_MODE_ENT  ditPairReadForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_STC                  ditPairEntry;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ditPairEntryIndex [0 / 274],
                           ditPairReadForm [CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E /
                                            CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_SECOND_DIT_NEXT_POINTER_ONLY_E],
                           and non-NULL ditPairEntryPtr.
            Expected: GT_OK.
        */

        /* Call with ditPairEntryIndex [0] */
        ditPairEntryIndex = 0;
        ditPairReadForm  = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E;

        st = cpssExMxPmDitIpMcPairRead(dev, ditPairEntryIndex, ditPairReadForm, &ditPairEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ditPairEntryIndex, ditPairReadForm);

        /* Call with ditPairEntryIndex [274] */
        ditPairEntryIndex = 274;
        ditPairReadForm  = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_SECOND_DIT_NEXT_POINTER_ONLY_E;

        st = cpssExMxPmDitIpMcPairRead(dev, ditPairEntryIndex, ditPairReadForm, &ditPairEntry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ditPairEntryIndex, ditPairReadForm);

        /*
            1.2. Call with out of range ditPairEntryIndex [3072]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ditPairEntryIndex = 3072;

        st = cpssExMxPmDitIpMcPairRead(dev, ditPairEntryIndex, ditPairReadForm, &ditPairEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ditPairEntryIndex);

        ditPairEntryIndex = 0;

        /*
            1.3. Call with out of range ditPairWriteForm [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ditPairReadForm = DIT_INVALID_ENUM_CNS;

        st = cpssExMxPmDitIpMcPairRead(dev, ditPairEntryIndex, ditPairReadForm, &ditPairEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditPairWriteForm = %d", dev, ditPairReadForm);

        ditPairReadForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_SECOND_DIT_NEXT_POINTER_ONLY_E;

        /*
            1.4. Call with ditPairEntryPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitIpMcPairRead(dev, ditPairEntryIndex, ditPairReadForm, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ditPairEntryPtr = NULL", dev);
    }

    ditPairEntryIndex = 0;
    ditPairReadForm   = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitIpMcPairRead(dev, ditPairEntryIndex, ditPairReadForm, &ditPairEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitIpMcPairRead(dev, ditPairEntryIndex, ditPairReadForm, &ditPairEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitIpMcPairLastSet
(
    IN  GT_U8                                                   devNum,
    IN  GT_U32                                                  ditPairEntryIndex,
    IN  CPSS_EXMXPM_DIT_IP_MC_PAIR_LAST_FIELD_WRITE_MODE_ENT    ditPairWriteForm,
    IN  GT_U32                                                  isLast
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitIpMcPairLastSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ditPairEntryIndex [0 / 274],
                   ditPairWriteForm [CPSS_EXMXPM_DIT_IP_MC_PAIR_WRITE_FIRST_DIT_LAST_FIELD_E /
                                     CPSS_EXMXPM_DIT_IP_MC_PAIR_WRITE_SECOND_DIT_LAST_FIELD_E]
                   and isLast [0 / 1].
    Expected: GT_OK.
    1.2. Call with out of range ditPairEntryIndex [3072]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range ditPairWriteForm [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range isLast [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                                ditPairEntryIndex = 0;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_LAST_FIELD_WRITE_MODE_ENT  ditPairWriteForm  = CPSS_EXMXPM_DIT_IP_MC_PAIR_WRITE_FIRST_DIT_LAST_FIELD_E;
    GT_U32                                                isLast            = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ditPairEntryIndex [0 / 274],
                           ditPairWriteForm [CPSS_EXMXPM_DIT_IP_MC_PAIR_WRITE_FIRST_DIT_LAST_FIELD_E /
                                             CPSS_EXMXPM_DIT_IP_MC_PAIR_WRITE_SECOND_DIT_LAST_FIELD_E]
                           and isLast [0 / 1].
            Expected: GT_OK.
        */

        /* Call with ditPairEntryIndex [0] */
        ditPairEntryIndex = 0;
        ditPairWriteForm  = CPSS_EXMXPM_DIT_IP_MC_PAIR_WRITE_FIRST_DIT_LAST_FIELD_E;
        isLast            = 0;

        st = cpssExMxPmDitIpMcPairLastSet(dev, ditPairEntryIndex, ditPairWriteForm, isLast);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ditPairEntryIndex, ditPairWriteForm, isLast);

        /* Call with ditPairEntryIndex [274] */
        ditPairEntryIndex = 274;
        ditPairWriteForm  = CPSS_EXMXPM_DIT_IP_MC_PAIR_WRITE_SECOND_DIT_LAST_FIELD_E;
        isLast            = 1;

        st = cpssExMxPmDitIpMcPairLastSet(dev, ditPairEntryIndex, ditPairWriteForm, isLast);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ditPairEntryIndex, ditPairWriteForm, isLast);

        /*
            1.2. Call with out of range ditPairEntryIndex [3072]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        ditPairEntryIndex = 3072;

        st = cpssExMxPmDitIpMcPairLastSet(dev, ditPairEntryIndex, ditPairWriteForm, isLast);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ditPairEntryIndex);

        ditPairEntryIndex = 0;

        /*
            1.3. Call with out of range ditPairWriteForm [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ditPairWriteForm = DIT_INVALID_ENUM_CNS;

        st = cpssExMxPmDitIpMcPairLastSet(dev, ditPairEntryIndex, ditPairWriteForm, isLast);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditPairWriteForm = %d", dev, ditPairWriteForm);

        ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_WRITE_FIRST_DIT_LAST_FIELD_E;

        /*
            1.4. Call with out of range isLast [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        isLast = 2;

        st = cpssExMxPmDitIpMcPairLastSet(dev, ditPairEntryIndex, ditPairWriteForm, isLast);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, isLast = %d", dev, isLast);
    }

    ditPairEntryIndex = 0;
    ditPairWriteForm  = CPSS_EXMXPM_DIT_IP_MC_PAIR_WRITE_FIRST_DIT_LAST_FIELD_E;
    isLast            = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitIpMcPairLastSet(dev, ditPairEntryIndex, ditPairWriteForm, isLast);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitIpMcPairLastSet(dev, ditPairEntryIndex, ditPairWriteForm, isLast);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitIpMplsEntryWrite
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  ditEntryIndex,
    IN  CPSS_EXMXPM_DIT_IP_MPLS_SELECTION_ENT   ditIpMpls,
    IN  CPSS_UNICAST_MULTICAST_ENT              ditType,
    IN  CPSS_EXMXPM_DIT_IP_MPLS_STC             *ditEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitIpMplsEntryWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with  ditEntryIndex[0 / 1],
                    ditIpMpls[CPSS_EXMXPM_DIT_IP_SELECTION_E / CPSS_EXMXPM_DIT_MPLS_SELECTION_E],
                    ditType[CPSS_UNICAST_E / CPSS_MULTICAST_E],
                    ditEntryPtr{
                                ditRpfFailCommand [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E / CPSS_PACKET_CMD_BRIDGE_E],
                                outLif { type [CPSS_INTERFACE_PORT_E],
                                          devPort->devNum[dev],
                                          devPort->portNum[0]}}.}
                                         outlifPointer{arpPtr[0]} },
                                vlanId [100 / 4095],
                                ttlHopLimitThreshold [0],
                                excludeSourceVlanIf [GT_FALSE / GT_TRUE],
                                mplsLabel [0 / 100],
                                exp [0 / 7]
                                qosExpCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E / CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_ENTRY_E],
                                mplsCommand [0 / 1],
                                ttlIndex [0 / 7],
                                isVidSet [GT_FALSE / GT_TRUE],
                                srcInterface{type[CPSS_INTERFACE_PORT_E],
                                             devPort {devNum [devNum],
                                                      portNum [0]},
                                             trunkId [0],
                                             vidx [0],
                                             vlanId [100]},
                                srcInterfaceCompareMode [CPSS_EXMXPM_DIT_SOURCE_INTERFACE_COMPARE_MODE_VID_E / CPSS_EXMXPM_DIT_SOURCE_INTERFACE_COMPARE_MODE_PORT_E],
                                isLast[GT_FALSE / GT_TRUE] }
    Expected: GT_OK.
    1.2. Call cpssExMxPmDitIpMplsEntryRead with not NULL ditEntryPtr and other params from 1.1.
    Expected: GT_OK and the same params as was set.
    1.3. Call with ditIpMpls [0x5AAAAAA5] out of range, and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with ditEntryPtr->ditRpfFailCommand [0x5AAAAAA5] out of range, and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with ditEntryPtr->ditRpfFailCommand [CPSS_PACKET_CMD_FORWARD_E] not support, and other params from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with ditEntryPtr->outLif.type [0x5AAAAAA5] out of range, and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_PORT_E]
                    ditEntryPtr->outLif.devPort.portNum [64] out of range,
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_PORT_E]
                    ditEntryPtr->outLif.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] out of range,
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.10. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_TRUNK_E]
                    ditEntryPtr->outLif.devPort.portNum [64] (not relevant),
                    and other params from 1.1.
    Expected: GT_OK.
    1.11. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_TRUNK_E]
                    ditEntryPtr->outLif.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant),
                    and other params from 1.1.
    Expected: GT_OK.
    1.12. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_TRUNK_E]
                    ditEntryPtr->outLif.trunkId [256] out of range,
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.13. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_VIDX_E]
                    ditEntryPtr->outLif.trunkId [256] not relevant
                    and other params from 1.1.
    Expected: GT_OK.
    1.14. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_VIDX_E]
                    ditEntryPtr->outLif.vidx [4096] out of range
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.15. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_PORT_E]
                    ditEntryPtr->outLif.vidx [4096] not relevant
                    and other params from 1.1.
    Expected: GT_OK.
    1.16. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_VID_E /
                                             CPSS_INTERFACE_DEVICE_E /
                                             CPSS_INTERFACE_FABRIC_VIDX_E] not supported
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.17. Call with ditEntryPtr->vlanId [4096] out of range, and other params from 1.1.
    Expected: NOT GT_OK.
    1.18. Call with ditEntryPtr->exp [8] out of range, and other params from 1.1.
    Expected: NOT GT_OK.
    1.19. Call with ditEntryPtr->qosExpCommand [0x5AAAAAA5] out of range, and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.20. Call with ditEntryPtr->mplsCommand[1], ditEntryPtr->ttlIndex [8] out of range, and other params from 1.1.
    Expected: NOT GT_OK.
    1.21. Call with ditEntryPtr->mplsCommand[2] out of range, and other params from 1.1.
    Expected: NOT GT_OK.
    1.22. Call with ditEntryPtr->srcInterface.type [0x5AAAAAA5] out of range, and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.23. Call with ditEntryPtr->srcInterfaceCompareMode [0x5AAAAAA5] out of range, and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.24. Call with ditEntryPtr [NULL], and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_BOOL     isEqual = GT_FALSE;

    GT_U32                                  ditEntryIndex = 0;
    CPSS_EXMXPM_DIT_IP_MPLS_SELECTION_ENT   ditIpMpls     = CPSS_EXMXPM_DIT_IP_SELECTION_E;
    CPSS_UNICAST_MULTICAST_ENT              ditType       = CPSS_MULTICAST_E;
    CPSS_EXMXPM_DIT_IP_MPLS_STC             ditEntry;
    CPSS_EXMXPM_DIT_IP_MPLS_STC             ditEntryGet;


    cpssOsBzero((GT_VOID*) &ditEntry, sizeof(ditEntry));
    cpssOsBzero((GT_VOID*) &ditEntryGet, sizeof(ditEntryGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if (PRV_CPSS_EXMXPM_DIT_MODULE_MAC(dev)->ipMcMode != CPSS_EXMXPM_PP_CONFIG_DIT_IP_MC_DUAL_MODE_E)
        {
            /*
                1.1. Call with  ditEntryIndex[0 / 1],
                                ditIpMpls[CPSS_EXMXPM_DIT_IP_SELECTION_E / CPSS_EXMXPM_DIT_MPLS_SELECTION_E],
                                ditType[CPSS_UNICAST_E / CPSS_MULTICAST_E],
                                ditEntryPtr{
                                            ditRpfFailCommand [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E / CPSS_PACKET_CMD_BRIDGE_E],
                                            outLif { {outlifType{CPSS_EXMXPM_OUTLIF_TYPE_LL_E}
                                                     interfaceInfo{{type [CPSS_INTERFACE_PORT_E],
                                                                          devPort->devNum[dev],
                                                                          devPort->portNum[0]}}.}
                                                     outlifPointer{arpPtr[0]} },
                                            vlanId [100 / 4095],
                                            ttlHopLimitThreshold [0],
                                            excludeSourceVlanIf [GT_FALSE / GT_TRUE],
                                            mplsLabel [0 / 100],
                                            exp [0 / 7]
                                            qosExpCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E / CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_ENTRY_E],
                                            mplsCommand [0 / 1],
                                            ttlIndex [0 / 7],
                                            isVidSet [GT_FALSE / GT_TRUE],
                                            srcInterface{type[CPSS_INTERFACE_PORT_E],
                                                         devPort {devNum [devNum],
                                                                  portNum [0]},
                                                         trunkId [0],
                                                         vidx [0],
                                                         vlanId [100]},
                                            srcInterfaceCompareMode [CPSS_EXMXPM_DIT_SOURCE_INTERFACE_COMPARE_MODE_VID_E / CPSS_EXMXPM_DIT_SOURCE_INTERFACE_COMPARE_MODE_PORT_E],
                                            isLast[GT_FALSE / GT_TRUE] }
                Expected: GT_OK.
            */
            /* iterate with ditEntryIndex = 0 */
            ditEntryIndex = 0;
            ditIpMpls = CPSS_EXMXPM_DIT_IP_SELECTION_E;
            ditType   = CPSS_UNICAST_E;
            ditEntry.ditRpfFailCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;
            ditEntry.outLif.devPort.devNum = dev;
            ditEntry.outLif.devPort.portNum = 0;
            ditEntry.vlanId = 100;
            ditEntry.ttlHopLimitThreshold = 0;
            ditEntry.excludeSourceVlanIf = GT_FALSE;
            ditEntry.mplsLabel = 0;
            ditEntry.exp = 0;
            ditEntry.qosExpCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E;
            ditEntry.mplsCommand = 0;
            ditEntry.ttlIndex = 0;
            ditEntry.isVidSet = GT_FALSE;
            ditEntry.srcInterface.type = CPSS_INTERFACE_PORT_E;
            ditEntry.srcInterface.devPort.devNum = dev;
            ditEntry.srcInterface.devPort.portNum = 0;
            ditEntry.srcInterfaceCompareMode = CPSS_EXMXPM_DIT_SOURCE_INTERFACE_COMPARE_MODE_VID_E;
            ditEntry.isLast = GT_FALSE;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ditEntryIndex, ditIpMpls, ditType);

            /*
                1.2. Call cpssExMxPmDitIpMplsEntryRead with not NULL ditEntryPtr and other params from 1.1.
                Expected: GT_OK and the same params as was set.
            */
            st = cpssExMxPmDitIpMplsEntryRead(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntryGet);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmDitIpMplsEntryRead: %d, %d, %d, %d",
                                        dev, ditEntryIndex, ditIpMpls, ditType);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &ditEntry, (GT_VOID*) &ditEntryGet, sizeof(ditEntry) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another ditEntry than was set: %d", dev);

            /* iterate with ditEntryIndex = 0 */
            ditEntryIndex = 1;
            ditIpMpls = CPSS_EXMXPM_DIT_MPLS_SELECTION_E;
            ditType   = CPSS_MULTICAST_E;
            ditEntry.ditRpfFailCommand = CPSS_PACKET_CMD_BRIDGE_E;
            ditEntry.vlanId = 4095;
            ditEntry.excludeSourceVlanIf = GT_TRUE;
            ditEntry.mplsLabel = 100;
            ditEntry.exp = 7;
            ditEntry.qosExpCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_ENTRY_E;
            ditEntry.mplsCommand = 1;
            ditEntry.ttlIndex = 7;
            ditEntry.isVidSet = GT_TRUE;
            ditEntry.srcInterfaceCompareMode = CPSS_EXMXPM_DIT_SOURCE_INTERFACE_COMPARE_MODE_PORT_E;
            ditEntry.isLast = GT_TRUE;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ditEntryIndex, ditIpMpls, ditType);

            /*
                1.2. Call cpssExMxPmDitIpMplsEntryRead with not NULL ditEntryPtr and other params from 1.1.
                Expected: GT_OK and the same params as was set.
            */
            st = cpssExMxPmDitIpMplsEntryRead(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntryGet);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmDitIpMplsEntryRead: %d, %d, %d, %d",
                                        dev, ditEntryIndex, ditIpMpls, ditType);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &ditEntry, (GT_VOID*) &ditEntryGet, sizeof(ditEntry) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another ditEntry than was set: %d", dev);

            /*
                1.3. Call with ditIpMpls [0x5AAAAAA5] out of range, and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            ditIpMpls = DIT_INVALID_ENUM_CNS;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditIpMpls = %d", dev, ditIpMpls);

            ditIpMpls = CPSS_EXMXPM_DIT_MPLS_SELECTION_E;

            /*
                1.5. Call with ditEntryPtr->ditRpfFailCommand [0x5AAAAAA5] out of range, and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            ditEntry.ditRpfFailCommand = DIT_INVALID_ENUM_CNS;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditEntryPtr->ditRpfFailCommand = %d",
                                         dev, ditEntry.ditRpfFailCommand);

            ditEntry.ditRpfFailCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;

            /*
                1.6. Call with ditEntryPtr->ditRpfFailCommand [CPSS_PACKET_CMD_FORWARD_E] not support, and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.ditRpfFailCommand = CPSS_PACKET_CMD_FORWARD_E;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->ditRpfFailCommand = %d",
                                         dev, ditEntry.ditRpfFailCommand);

            ditEntry.ditRpfFailCommand = CPSS_PACKET_CMD_BRIDGE_E;

            /*
                1.7. Call with ditEntryPtr->outLif.type [0x5AAAAAA5] out of range, and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            ditEntry.outLif.type = DIT_INVALID_ENUM_CNS;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditEntryPtr->outLif.type = %d",
                                         dev, ditEntry.outLif.type);

            ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;

            /*
                1.8. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_PORT_E]
                                ditEntryPtr->outLif.devPort.portNum [64] out of range,
                                and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;
            ditEntry.outLif.devPort.portNum = 64;
            ditEntry.outLif.devPort.devNum = dev;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.devPort.portNum = %d",
                                             dev, ditEntry.outLif.type, ditEntry.outLif.devPort.portNum);

            ditEntry.outLif.devPort.portNum = 0;

            /*
                1.9. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_PORT_E]
                                ditEntryPtr->outLif.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] out of range,
                                and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;
            ditEntry.outLif.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.devPort.devNum = %d",
                                             dev, ditEntry.outLif.type, ditEntry.outLif.devPort.devNum);

            ditEntry.outLif.devPort.devNum = 0;

            /*
                1.10. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_TRUNK_E]
                                ditEntryPtr->outLif.devPort.portNum [64] (not relevant),
                                and other params from 1.1.
                Expected: GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_TRUNK_E;
            ditEntry.outLif.devPort.portNum = 64;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.devPort.portNum = %d",
                                         dev, ditEntry.outLif.type, ditEntry.outLif.devPort.portNum);

            ditEntry.outLif.devPort.portNum = 0;

            /*
                1.11. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_TRUNK_E]
                                ditEntryPtr->outLif.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant),
                                and other params from 1.1.
                Expected: GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_TRUNK_E;
            ditEntry.outLif.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.devPort.devNum = %d",
                                         dev, ditEntry.outLif.type, ditEntry.outLif.devPort.devNum);

            ditEntry.outLif.devPort.devNum = 0;

            /*
                1.12. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_TRUNK_E]
                                ditEntryPtr->outLif.trunkId [256] out of range,
                                and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_TRUNK_E;
            ditEntry.outLif.trunkId = 256;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.trunkId = %d",
                                             dev, ditEntry.outLif.type, ditEntry.outLif.trunkId);

            ditEntry.outLif.trunkId = 0;

            /*
                1.13. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_VIDX_E]
                                ditEntryPtr->outLif.trunkId [256] not relevant
                                and other params from 1.1.
                Expected: GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_VIDX_E;
            ditEntry.outLif.trunkId = 256;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.trunkId = %d",
                                         dev, ditEntry.outLif.type, ditEntry.outLif.trunkId);

            ditEntry.outLif.trunkId = 0;

            /*
                1.14. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_VIDX_E]
                                ditEntryPtr->outLif.vidx [4096] out of tange
                                and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_VIDX_E;
            ditEntry.outLif.vidx = 4096;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.vidx = %d",
                                             dev, ditEntry.outLif.type, ditEntry.outLif.vidx);

            ditEntry.outLif.vidx = 0;

            /*
                1.15. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_PORT_E]
                                ditEntryPtr->outLif.vidx [4096] not relevant
                                and other params from 1.1.
                Expected: GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;
            ditEntry.outLif.vidx = 4096;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.vidx = %d",
                                         dev, ditEntry.outLif.type, ditEntry.outLif.vidx);

            ditEntry.outLif.vidx = 0;

            /*
                1.16. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_VID_E /
                                                         CPSS_INTERFACE_DEVICE_E /
                                                         CPSS_INTERFACE_FABRIC_VIDX_E] not supported
                                and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_VID_E;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d",
                                             dev, ditEntry.outLif.type);

            ditEntry.outLif.type = CPSS_INTERFACE_DEVICE_E;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d",
                                             dev, ditEntry.outLif.type);

            ditEntry.outLif.type = CPSS_INTERFACE_FABRIC_VIDX_E;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d",
                                             dev, ditEntry.outLif.type);

            ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;

            /*
                1.17. Call with ditEntryPtr->vlanId [4096] out of range, and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.vlanId = 4096;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->vlanId = %d",
                                         dev, ditEntry.vlanId);

            ditEntry.vlanId = 4095;

            /*
                1.18. Call with ditEntryPtr->exp [8] out of range, and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.exp = 8;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->exp = %d",
                                         dev, ditEntry.exp);

            ditEntry.exp = 7;

            /*
                1.19. Call with ditEntryPtr->qosExpCommand [0x5AAAAAA5] out of range, and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            ditEntry.qosExpCommand = DIT_INVALID_ENUM_CNS;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditEntryPtr->qosExpCommand = %d",
                                         dev, ditEntry.qosExpCommand);

            ditEntry.qosExpCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E;

            /*
                1.20. Call with ditEntryPtr->mplsCommand[1], ditEntryPtr->ttlIndex [8] out of range, and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.mplsCommand = 1;
            ditEntry.ttlIndex    = 8;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->mplsCommand = %d, ditEntryPtr->ttlIndex = %d",
                                         dev, ditEntry.mplsCommand, ditEntry.ttlIndex);

            ditEntry.ttlIndex = 7;

            /*
                1.21. Call with ditEntryPtr->mplsCommand[2] out of range, and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.mplsCommand = 2;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->mplsCommand = %d",
                                         dev, ditEntry.mplsCommand);

            ditEntry.mplsCommand = 1;

            /*
                1.22. Call with ditEntryPtr->srcInterface.type [0x5AAAAAA5] out of range, and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            ditEntry.srcInterface.type = DIT_INVALID_ENUM_CNS;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditEntryPtr->srcInterface.type = %d",
                                         dev, ditEntry.srcInterface.type);

            ditEntry.srcInterface.type = CPSS_INTERFACE_PORT_E;

            /*
                1.23. Call with ditEntryPtr->srcInterfaceCompareMode [0x5AAAAAA5] out of range, and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            ditEntry.srcInterfaceCompareMode = DIT_INVALID_ENUM_CNS;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditEntryPtr->srcInterfaceCompareMode = %d",
                                         dev, ditEntry.srcInterfaceCompareMode);

            ditEntry.srcInterfaceCompareMode = CPSS_EXMXPM_DIT_SOURCE_INTERFACE_COMPARE_MODE_VID_E;

            /*
                1.24. Call with ditEntryPtr [NULL], and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ditEntryPtr = NULL", dev);
        }
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitIpMplsEntryRead
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  ditEntryIndex,
    IN  CPSS_EXMXPM_DIT_IP_MPLS_SELECTION_ENT   ditIpMpls,
    IN  CPSS_UNICAST_MULTICAST_ENT              ditType,
    OUT CPSS_EXMXPM_DIT_IP_MPLS_STC             *ditEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitIpMplsEntryRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with  ditEntryIndex[0],
                    ditIpMpls[CPSS_EXMXPM_DIT_IP_SELECTION_E],
                    ditType[CPSS_UNICAST_E],
                    not NULL ditEntryPtr.
    Expected: GT_OK.
    1.2. Call with ditIpMpls [0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with ditType [0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with ditEntryPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                                  ditEntryIndex = 0;
    CPSS_EXMXPM_DIT_IP_MPLS_SELECTION_ENT   ditIpMpls     = CPSS_EXMXPM_DIT_IP_SELECTION_E;
    CPSS_UNICAST_MULTICAST_ENT              ditType       = CPSS_MULTICAST_E;
    CPSS_EXMXPM_DIT_IP_MPLS_STC             ditEntry;


    cpssOsBzero((GT_VOID*) &ditEntry, sizeof(ditEntry));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if (PRV_CPSS_EXMXPM_DIT_MODULE_MAC(dev)->ipMcMode != CPSS_EXMXPM_PP_CONFIG_DIT_IP_MC_DUAL_MODE_E)
        {
            /*
                1.1. Call with  ditEntryIndex[0],
                                ditIpMpls[CPSS_EXMXPM_DIT_IP_SELECTION_E],
                                ditType[CPSS_UNICAST_E],
                                not NULL ditEntryPtr.
                Expected: GT_OK.
            */
            ditEntryIndex = 0;
            ditIpMpls = CPSS_EXMXPM_DIT_IP_SELECTION_E;
            ditType   = CPSS_UNICAST_E;

            st = cpssExMxPmDitIpMplsEntryRead(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ditEntryIndex, ditIpMpls, ditType);

            /*
                1.2. Call with ditIpMpls [0x5AAAAAA5] and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            ditIpMpls = DIT_INVALID_ENUM_CNS;

            st = cpssExMxPmDitIpMplsEntryRead(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditIpMpls = %d", dev, ditIpMpls);

            ditIpMpls = CPSS_EXMXPM_DIT_IP_SELECTION_E;

            /*
                1.3. Call with ditType [0x5AAAAAA5] and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            ditType = DIT_INVALID_ENUM_CNS;

            st = cpssExMxPmDitIpMplsEntryRead(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditType = %d", dev, ditType);

            ditType = CPSS_MULTICAST_E;

            /*
                1.4. Call with ditEntryPtr [NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmDitIpMplsEntryRead(dev, ditEntryIndex, ditIpMpls, ditType, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ditEntryPtr = NULL", dev);
        }
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitIpMplsEntryRead(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitIpMplsEntryRead(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitIpMplsEntryLastSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  ditEntryIndex,
    IN  CPSS_EXMXPM_DIT_IP_MPLS_SELECTION_ENT   ditIpMpls,
    IN  CPSS_UNICAST_MULTICAST_ENT              ditType,
    IN  GT_U32                                  isLast
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitIpMplsEntryLastSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmDitIpMplsEntryWrite with
                    ditEntryIndex[0],
                    ditIpMpls[CPSS_EXMXPM_DIT_IP_SELECTION_E],
                    ditType[CPSS_UNICAST_E],
                    ditEntryPtr{
                                ditRpfFailCommand [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E],
                                outLif { {outlifType{CPSS_EXMXPM_OUTLIF_TYPE_LL_E}
                                         interfaceInfo{{type [CPSS_INTERFACE_PORT_E],
                                                              devPort->devNum[dev],
                                                              devPort->portNum[0]}}.}
                                         outlifPointer{arpPtr[0]} },
                                vlanId [100],
                                ttlHopLimitThreshold [0],
                                excludeSourceVlanIf [GT_FALSE],
                                mplsLabel [0],
                                exp [0]
                                qosExpCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E],
                                mplsCommand [0],
                                ttlIndex [0],
                                isVidSet [GT_FALSE],
                                srcInterface{type[CPSS_INTERFACE_PORT_E],
                                             devPort {devNum [devNum],
                                                      portNum [0]},
                                             trunkId [0],
                                             vidx [0],
                                             vlanId [100]},
                                srcInterfaceCompareMode [CPSS_EXMXPM_DIT_SOURCE_INTERFACE_COMPARE_MODE_VID_E],
                                isLast[GT_FALSE] }
    Expected: GT_OK.
    1.2. Call with  ditEntryIndex [0],
                    ditIpMpls [CPSS_EXMXPM_DIT_IP_SELECTION_E],
                    ditType [CPSS_UNICAST_E],
                    isLast [0]
    Expected: GT_OK.
    1.3. Call with ditIpMpls [0x5AAAAAA5] and other params from 1.2.
    Expected: GT_BAD_PARAM.
    1.5. Call with isLast [2] and other params from 1.2.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                                  ditEntryIndex = 0;
    CPSS_EXMXPM_DIT_IP_MPLS_SELECTION_ENT   ditIpMpls     = CPSS_EXMXPM_DIT_IP_SELECTION_E;
    CPSS_UNICAST_MULTICAST_ENT              ditType       = CPSS_MULTICAST_E;
    CPSS_EXMXPM_DIT_IP_MPLS_STC             ditEntry;
    GT_U32                                  isLast = 0;


    cpssOsBzero((GT_VOID*) &ditEntry, sizeof(ditEntry));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if (PRV_CPSS_EXMXPM_DIT_MODULE_MAC(dev)->ipMcMode != CPSS_EXMXPM_PP_CONFIG_DIT_IP_MC_DUAL_MODE_E)
        {
            /*
                1.1. Call with  ditEntryIndex[0],
                                ditIpMpls[CPSS_EXMXPM_DIT_IP_SELECTION_E],
                                ditType[CPSS_UNICAST_E],
                                ditEntryPtr{
                                            ditRpfFailCommand [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E],
                                            outLif { {outlifType{CPSS_EXMXPM_OUTLIF_TYPE_LL_E}
                                                     interfaceInfo{{type [CPSS_INTERFACE_PORT_E],
                                                                          devPort->devNum[dev],
                                                                          devPort->portNum[0]}}.}
                                                     outlifPointer{arpPtr[0]} },
                                            vlanId [100],
                                            ttlHopLimitThreshold [0],
                                            excludeSourceVlanIf [GT_FALSE],
                                            mplsLabel [0],
                                            exp [0]
                                            qosExpCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E],
                                            mplsCommand [0],
                                            ttlIndex [0],
                                            isVidSet [GT_FALSE],
                                            srcInterface{type[CPSS_INTERFACE_PORT_E],
                                                         devPort {devNum [devNum],
                                                                  portNum [0]},
                                                         trunkId [0],
                                                         vidx [0],
                                                         vlanId [100]},
                                            srcInterfaceCompareMode [CPSS_EXMXPM_DIT_SOURCE_INTERFACE_COMPARE_MODE_VID_E],
                                            isLast[GT_FALSE] }
                Expected: GT_OK.
            */
            ditEntryIndex = 0;
            ditIpMpls = CPSS_EXMXPM_DIT_IP_SELECTION_E;
            ditType   = CPSS_UNICAST_E;
            ditEntry.ditRpfFailCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;
            ditEntry.outLif.devPort.devNum = dev;
            ditEntry.outLif.devPort.portNum = 0;
            ditEntry.vlanId = 100;
            ditEntry.ttlHopLimitThreshold = 0;
            ditEntry.excludeSourceVlanIf = GT_FALSE;
            ditEntry.mplsLabel = 0;
            ditEntry.exp = 0;
            ditEntry.qosExpCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E;
            ditEntry.mplsCommand = 0;
            ditEntry.ttlIndex = 0;
            ditEntry.isVidSet = GT_FALSE;
            ditEntry.srcInterface.type = CPSS_INTERFACE_PORT_E;
            ditEntry.srcInterface.devPort.devNum = dev;
            ditEntry.srcInterface.devPort.portNum = 0;
            ditEntry.srcInterfaceCompareMode = CPSS_EXMXPM_DIT_SOURCE_INTERFACE_COMPARE_MODE_VID_E;
            ditEntry.isLast = GT_FALSE;

            st = cpssExMxPmDitIpMplsEntryWrite(dev, ditEntryIndex, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ditEntryIndex, ditIpMpls, ditType);

            /*
                1.2. Call with  ditEntryIndex [0],
                                ditIpMpls [CPSS_EXMXPM_DIT_IP_SELECTION_E],
                                ditType [CPSS_UNICAST_E],
                                isLast [0]
                Expected: GT_OK.
            */
            ditEntryIndex = 0;
            ditIpMpls = CPSS_EXMXPM_DIT_IP_SELECTION_E;
            ditType = CPSS_UNICAST_E;
            isLast  = 0;

            st = cpssExMxPmDitIpMplsEntryLastSet(dev, ditEntryIndex, ditIpMpls, ditType, isLast);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ditEntryIndex, ditIpMpls, ditType, isLast);

            /*
                1.3. Call with ditIpMpls [0x5AAAAAA5] and other params from 1.2.
                Expected: GT_BAD_PARAM.
            */
            ditIpMpls = DIT_INVALID_ENUM_CNS;

            st = cpssExMxPmDitIpMplsEntryLastSet(dev, ditEntryIndex, ditIpMpls, ditType, isLast);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditIpMpls = %d", dev, ditIpMpls);

            ditIpMpls = CPSS_EXMXPM_DIT_IP_SELECTION_E;

            /*
                1.5. Call with isLast [2] and other params from 1.2.
                Expected: NOT GT_OK.
            */
            isLast = 2;

            st = cpssExMxPmDitIpMplsEntryLastSet(dev, ditEntryIndex, ditIpMpls, ditType, isLast);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, isLast = %d", dev, isLast);
        }
    }

    ditEntryIndex = 0;
    ditIpMpls = CPSS_EXMXPM_DIT_IP_SELECTION_E;
    ditType = CPSS_UNICAST_E;
    isLast  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitIpMplsEntryLastSet(dev, ditEntryIndex, ditIpMpls, ditType, isLast);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitIpMplsEntryLastSet(dev, ditEntryIndex, ditIpMpls, ditType, isLast);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitTtlTableEntrySet
(
    IN  GT_U8          devNum,
    IN  GT_U32         index,
    IN  GT_U32         value
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitTtlTableEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 7], value [0 / 255]
    Expected: GT_OK.
    1.2. Call cpssExMxPmDitTtlTableEntryGet with not NULL valuePtr
    Expected: GT_OK and the same value as was set.
    1.3. Call with index[8] out of range and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with value[256] out of range and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32         index    = 0;
    GT_U32         value    = 0;
    GT_U32         valueGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 7], value [0 / 255]
            Expected: GT_OK.
        */
        /* iterate with index = 0 */
        index = 0;
        value = 0;

        st = cpssExMxPmDitTtlTableEntrySet(dev, index, value);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, value);

        /*
            1.2. Call cpssExMxPmDitTtlTableEntryGet with not NULL valuePtr
            Expected: GT_OK and the same value as was set.
        */
        st = cpssExMxPmDitTtlTableEntryGet(dev, index, &valueGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmDitTtlTableEntryGet: %d, %d", dev, index);
        UTF_VERIFY_EQUAL1_STRING_MAC(value, valueGet, "got another value than was set: %d, %d", dev);

        /* iterate with index = 7 */
        index = 7;
        value = 255;

        st = cpssExMxPmDitTtlTableEntrySet(dev, index, value);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, value);

        /*
            1.2. Call cpssExMxPmDitTtlTableEntryGet with not NULL valuePtr
            Expected: GT_OK and the same value as was set.
        */
        st = cpssExMxPmDitTtlTableEntryGet(dev, index, &valueGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmDitTtlTableEntryGet: %d, %d", dev, index);
        UTF_VERIFY_EQUAL1_STRING_MAC(value, valueGet, "got another value than was set: %d, %d", dev);

        /*
            1.3. Call with index[8] out of range and other params from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmDitTtlTableEntrySet(dev, index, value);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.4. Call with value[256] out of range and other params from 1.1.
            Expected: NOT GT_OK.
        */
        value = 256;

        st = cpssExMxPmDitTtlTableEntrySet(dev, index, value);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, value = %d", dev, value);
    }

    index = 0;
    value = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitTtlTableEntrySet(dev, index, value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitTtlTableEntrySet(dev, index, value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitTtlTableEntryGet
(
    IN  GT_U8          devNum,
    IN  GT_U32         index,
    OUT GT_U32         *valuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitTtlTableEntryGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with index [0 / 7], and not NULL valuePtr
    Expected: GT_OK.
    1.3. Call with index[8] out of range and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with value[NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      index    = 0;
    GT_U32      value    = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with index [0 / 7], and not NULL valuePtr
            Expected: GT_OK.
        */
        index = 0;

        st = cpssExMxPmDitTtlTableEntryGet(dev, index, &value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 7;

        st = cpssExMxPmDitTtlTableEntryGet(dev, index, &value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.3. Call with index[8] out of range and other params from 1.1.
            Expected: NOT GT_OK.
        */
        index = 8;

        st = cpssExMxPmDitTtlTableEntryGet(dev, index, &value);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 7;

        /*
            1.4. Call with value[NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitTtlTableEntryGet(dev, index, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, valuePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitTtlTableEntryGet(dev, index, &value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitTtlTableEntryGet(dev, index, &value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitVplsEntryWrite
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ditEntryIndex,
    IN  CPSS_UNICAST_MULTICAST_ENT    ditType,
    IN  CPSS_EXMXPM_DIT_VPLS_STC      *ditEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitVplsEntryWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with  ditEntryIndex[0 / 1],
                    ditType[CPSS_UNICAST_E / CPSS_MULTICAST_E],
                    ditEntryPtr{outLif { type [CPSS_INTERFACE_PORT_E],
                                          devPort->devNum[dev],
                                          devPort->portNum[0]},
                                vlanId [100 / 4095],
                                ttl [0 / 100],
                                vlanTagCommand [CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_TAGGED_FROM_DIT_E / CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_UNTAGGED_E],
                                label [0 / 100],
                                exp [0 / 7],
                                qosExpCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E / CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_ENTRY_E],
                                up [0 / 7],
                                mplsCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E / CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_ENTRY_E],
                                vplsUnknownUcEnable [GT_FALSE / GT_TRUE],
                                vplsUnregisteredMcEnable [GT_FALSE / GT_TRUE],
                                pwId [0 / 100],
                                isLast[GT_FALSE / GT_TRUE] }
    Expected: GT_OK.
    1.2. Call cpssExMxPmDitVplsEntryRead with not NULL ditEntryPtr and other params from 1.1.
    Expected: GT_OK and the same ditEntry as was set.
    1.3. Call with ditEntryPtr->outLif.type [0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_PORT_E]
                    ditEntryPtr->outLif.devPort.portNum [64] out of range,
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_PORT_E]
                    ditEntryPtr->outLif.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] out of range,
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_TRUNK_E]
                    ditEntryPtr->outLif.devPort.portNum [64] (not relevant),
                    and other params from 1.1.
    Expected: GT_OK.
    1.7. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_TRUNK_E]
                    ditEntryPtr->outLif.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant),
                    and other params from 1.1.
    Expected: GT_OK.
    1.8. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_TRUNK_E]
                    ditEntryPtr->outLif.trunkId [256] out of range,
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.9. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_VIDX_E]
                    ditEntryPtr->outLif.trunkId [256] not relevant
                    and other params from 1.1.
    Expected: GT_OK.
    1.10. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_VIDX_E]
                    ditEntryPtr->outLif.vidx [4096] out of range
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.11. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_PORT_E]
                    ditEntryPtr->outLif.vidx [4096] not relevant
                    and other params from 1.1.
    Expected: GT_OK.
    1.12. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_VID_E /
                                             CPSS_INTERFACE_DEVICE_E /
                                             CPSS_INTERFACE_FABRIC_VIDX_E] not supported
                    and other params from 1.1.
    Expected: NOT GT_OK.
    1.13. Call with ditEntryPtr->vlanId [4096] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.14. Call with ditEntryPtr->vlanTagCommand [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.15. Call with ditEntryPtr->exp [8] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.16. Call with ditEntryPtr->up [8] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.17. Call with ditEntryPtr->qosExpCommand [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.18. Call with ditEntryPtr->mplsCommand [0x5AAAAAA5] (out of range) and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.19. Call with ditEntryPtr->pwId [0x80] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.20. Call with ditEntryPtr->label [0x100000] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.21. Call with ditEntryPtr->ttl [0x100] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.22. Call with ditEntryPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_BOOL     isEqual = GT_FALSE;

    GT_U32                        ditEntryIndex;
    CPSS_UNICAST_MULTICAST_ENT    ditType;
    CPSS_EXMXPM_DIT_VPLS_STC      ditEntry;
    CPSS_EXMXPM_DIT_VPLS_STC      ditEntryGet;


    cpssOsBzero((GT_VOID*) &ditEntry, sizeof(ditEntry));
    cpssOsBzero((GT_VOID*) &ditEntryGet, sizeof(ditEntryGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if (PRV_CPSS_EXMXPM_DIT_MODULE_MAC(dev)->ipMcMode != CPSS_EXMXPM_PP_CONFIG_DIT_IP_MC_DUAL_MODE_E)
        {
            /*
                1.1. Call with  ditEntryIndex[0 / 1],
                                ditType[CPSS_UNICAST_E / CPSS_MULTICAST_E],
                                ditEntryPtr{outLif { type [CPSS_INTERFACE_PORT_E],
                                                      devPort->devNum[dev],
                                                      devPort->portNum[0]},
                                            vlanId [100 / 4095],
                                            ttl [0 / 100],
                                            vlanTagCommand [CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_TAGGED_FROM_DIT_E / CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_UNTAGGED_E],
                                            label [0 / 100],
                                            exp [0 / 7],
                                            qosExpCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E / CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_ENTRY_E],
                                            up [0 / 7],
                                            mplsCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E / CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_ENTRY_E],
                                            vplsUnknownUcEnable [GT_FALSE / GT_TRUE],
                                            vplsUnregisteredMcEnable [GT_FALSE / GT_TRUE],
                                            pwId [0 / 100],
                                            isLast[GT_FALSE / GT_TRUE] }
                Expected: GT_OK.
            */
            /* iterate with ditEntryIndex = 0 */
            ditEntryIndex = 0;
            ditType       = CPSS_UNICAST_E;
            ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;
            ditEntry.outLif.devPort.devNum  = dev;
            ditEntry.outLif.devPort.portNum = 0;
            ditEntry.vlanId = 100;
            ditEntry.ttl = 0;
            ditEntry.vlanTagCommand = CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_TAGGED_FROM_DIT_E;
            ditEntry.label = 0;
            ditEntry.exp   = 0;
            ditEntry.qosExpCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E;
            ditEntry.up = 0;
            ditEntry.mplsCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E;
            ditEntry.vplsUnknownUcEnable = GT_FALSE;
            ditEntry.vplsUnregisteredMcEnable = GT_FALSE;
            ditEntry.pwId = 0;
            ditEntry.isLast = GT_FALSE;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ditEntryIndex, ditType);

            /*
                1.2. Call cpssExMxPmDitVplsEntryRead with not NULL ditEntryPtr and other params from 1.1.
                Expected: GT_OK and the same ditEntry as was set.
            */
            st = cpssExMxPmDitVplsEntryRead(dev, ditEntryIndex, ditType, &ditEntryGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmDitVplsEntryRead: %d, %d, %d",
                                        dev, ditEntryIndex, ditType);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &ditEntry, (GT_VOID*) &ditEntryGet, sizeof(ditEntry) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another ditEntry then was set: %d", dev);

            /* iterate with ditEntryIndex = 1 */
            ditEntryIndex = 1;
            ditType       = CPSS_MULTICAST_E;
            ditEntry.vlanId = 4095;
            ditEntry.ttl = 100;
            ditEntry.vlanTagCommand = CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_UNTAGGED_E;
            ditEntry.label = 100;
            ditEntry.exp   = 7;
            ditEntry.qosExpCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_ENTRY_E;
            ditEntry.up = 7;
            ditEntry.mplsCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_ENTRY_E;
            ditEntry.vplsUnknownUcEnable = GT_TRUE;
            ditEntry.vplsUnregisteredMcEnable = GT_TRUE;
            ditEntry.pwId = 100;
            ditEntry.isLast = GT_TRUE;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ditEntryIndex, ditType);

            /*
                1.2. Call cpssExMxPmDitVplsEntryRead with not NULL ditEntryPtr and other params from 1.1.
                Expected: GT_OK and the same ditEntry as was set.
            */
            st = cpssExMxPmDitVplsEntryRead(dev, ditEntryIndex, ditType, &ditEntryGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmDitVplsEntryRead: %d, %d, %d",
                                        dev, ditEntryIndex, ditType);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &ditEntry, (GT_VOID*) &ditEntryGet, sizeof(ditEntry) )) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another ditEntry then was set: %d", dev);

            /*
                1.3. Call with ditEntryPtr->outLif.type [0x5AAAAAA5]out of range,
                                and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            ditEntry.outLif.type = DIT_INVALID_ENUM_CNS;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d",
                                             dev, ditEntry.outLif.type);

            ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;

            /*
                1.4. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_PORT_E]
                                ditEntryPtr->outLif.devPort.portNum [64] out of range,
                                and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;
            ditEntry.outLif.devPort.portNum = 64;
            ditEntry.outLif.devPort.devNum = dev;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.devPort.portNum = %d",
                                             dev, ditEntry.outLif.type, ditEntry.outLif.devPort.portNum);

            ditEntry.outLif.devPort.portNum = 0;

            /*
                1.5. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_PORT_E]
                                ditEntryPtr->outLif.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] out of range,
                                and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;
            ditEntry.outLif.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.devPort.devNum = %d",
                                             dev, ditEntry.outLif.type, ditEntry.outLif.devPort.devNum);

            ditEntry.outLif.devPort.devNum = 0;

            /*
                1.6. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_TRUNK_E]
                                ditEntryPtr->outLif.devPort.portNum [64] (not relevant),
                                and other params from 1.1.
                Expected: GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_TRUNK_E;
            ditEntry.outLif.devPort.portNum = 64;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.devPort.portNum = %d",
                                         dev, ditEntry.outLif.type, ditEntry.outLif.devPort.portNum);

            ditEntry.outLif.devPort.portNum = 0;

            /*
                1.7. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_TRUNK_E]
                                ditEntryPtr->outLif.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant),
                                and other params from 1.1.
                Expected: GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_TRUNK_E;
            ditEntry.outLif.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.devPort.devNum = %d",
                                         dev, ditEntry.outLif.type, ditEntry.outLif.devPort.devNum);

            ditEntry.outLif.devPort.devNum = 0;

            /*
                1.8. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_TRUNK_E]
                                ditEntryPtr->outLif.trunkId [256] out of range,
                                and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_TRUNK_E;
            ditEntry.outLif.trunkId = 256;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.trunkId = %d",
                                             dev, ditEntry.outLif.type, ditEntry.outLif.trunkId);

            ditEntry.outLif.trunkId = 0;

            /*
                1.9. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_VIDX_E]
                                ditEntryPtr->outLif.trunkId [256] not relevant
                                and other params from 1.1.
                Expected: GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_VIDX_E;
            ditEntry.outLif.trunkId = 256;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.trunkId = %d",
                                         dev, ditEntry.outLif.type, ditEntry.outLif.trunkId);

            ditEntry.outLif.trunkId = 0;

            /*
                1.10. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_VIDX_E]
                                ditEntryPtr->outLif.vidx [4096] out of tange
                                and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_VIDX_E;
            ditEntry.outLif.vidx = 4096;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.vidx = %d",
                                             dev, ditEntry.outLif.type, ditEntry.outLif.vidx);

            ditEntry.outLif.vidx = 0;

            /*
                1.11. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_PORT_E]
                                ditEntryPtr->outLif.vidx [4096] not relevant
                                and other params from 1.1.
                Expected: GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;
            ditEntry.outLif.vidx = 4096;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d, ditEntryPtr->outLif.vidx = %d",
                                         dev, ditEntry.outLif.type, ditEntry.outLif.vidx);

            ditEntry.outLif.vidx = 0;

            /*
                1.12. Call with ditEntryPtr->outLif.type [CPSS_INTERFACE_VID_E /
                                                         CPSS_INTERFACE_DEVICE_E /
                                                         CPSS_INTERFACE_FABRIC_VIDX_E] not supported
                                and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.outLif.type = CPSS_INTERFACE_VID_E;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d",
                                             dev, ditEntry.outLif.type);

            ditEntry.outLif.type = CPSS_INTERFACE_DEVICE_E;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d",
                                             dev, ditEntry.outLif.type);

            ditEntry.outLif.type = CPSS_INTERFACE_FABRIC_VIDX_E;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->outLif.type = %d",
                                             dev, ditEntry.outLif.type);

            ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;

            /*
                1.13. Call with ditEntryPtr->vlanId [4096] and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.vlanId = 4096;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->vlanId = %d",
                                         dev, ditEntry.vlanId);

            ditEntry.vlanId = 100;

            /*
                1.14. Call with ditEntryPtr->vlanTagCommand [0x5AAAAAA5] and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            ditEntry.vlanTagCommand = DIT_INVALID_ENUM_CNS;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditEntryPtr->vlanTagCommand = %d",
                                         dev, ditEntry.vlanTagCommand);

            ditEntry.vlanTagCommand = CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_UNTAGGED_E;

            /*
                1.15. Call with ditEntryPtr->exp [8] and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.exp = 8;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->exp = %d",
                                         dev, ditEntry.exp);

            ditEntry.exp = 7;

            /*
                1.16. Call with ditEntryPtr->up [8] and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.up = 8;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->up = %d",
                                         dev, ditEntry.up);

            ditEntry.up = 7;

            /*
                1.17. Call with ditEntryPtr->qosExpCommand [0x5AAAAAA5] and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            ditEntry.qosExpCommand = DIT_INVALID_ENUM_CNS;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditEntryPtr->qosExpCommand = %d",
                                         dev, ditEntry.qosExpCommand);

            ditEntry.qosExpCommand = CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_UNTAGGED_E;

            /*
                1.18. Call with ditEntryPtr->mplsCommand [0x5AAAAAA5] and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            ditEntry.mplsCommand = DIT_INVALID_ENUM_CNS;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ditEntryPtr->mplsCommand = %d",
                                         dev, ditEntry.mplsCommand);

            ditEntry.mplsCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_ENTRY_E;

            /*
                1.19. Call with ditEntryPtr->pwId [0x80] (out of range) and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.pwId = 0x80;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->pwId = %d",
                                         dev, ditEntry.pwId);

            ditEntry.pwId = 0;

            /*
                1.20. Call with ditEntryPtr->label [0x100000] (out of range) and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.label = 0x100000;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->label = %d",
                                         dev, ditEntry.label);

            ditEntry.label = 0;

            /*
                1.21. Call with ditEntryPtr->ttl [0x100] (out of range) and other params from 1.1.
                Expected: NOT GT_OK.
            */
            ditEntry.ttl = 0x100;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ditEntryPtr->ttl = %d",
                                         dev, ditEntry.ttl);

            ditEntry.ttl = 0;

            /*
                1.22. Call with ditEntryPtr [NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ditEntryPtr = NULL", dev);
        }
    }

    ditEntryIndex = 0;
    ditType       = CPSS_UNICAST_E;
    ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;
    ditEntry.outLif.devPort.devNum  = dev;
    ditEntry.outLif.devPort.portNum = 0;
    ditEntry.vlanId = 100;
    ditEntry.ttl = 0;
    ditEntry.vlanTagCommand = CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_TAGGED_FROM_DIT_E;
    ditEntry.label = 0;
    ditEntry.exp   = 0;
    ditEntry.qosExpCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E;
    ditEntry.up = 0;
    ditEntry.mplsCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E;
    ditEntry.vplsUnknownUcEnable = GT_FALSE;
    ditEntry.vplsUnregisteredMcEnable = GT_FALSE;
    ditEntry.pwId = 0;
    ditEntry.isLast = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitVplsEntryRead
(
    IN  GT_U8                        devNum,
    IN  GT_U32                       ditEntryIndex,
    IN  CPSS_UNICAST_MULTICAST_ENT   ditType,
    OUT CPSS_EXMXPM_DIT_VPLS_STC     *ditEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitVplsEntryRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with  ditEntryIndex[0],
                    ditType[CPSS_UNICAST_E],
                    not NULL ditEntryPtr
    Expectyed: GT_OK.
    1.2. Call with ditType[0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with ditEntryPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32                       ditEntryIndex = 0;
    CPSS_UNICAST_MULTICAST_ENT   ditType       = CPSS_MULTICAST_E;
    CPSS_EXMXPM_DIT_VPLS_STC     ditEntry;


    cpssOsBzero((GT_VOID*) &ditEntry, sizeof(ditEntry));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if (PRV_CPSS_EXMXPM_DIT_MODULE_MAC(dev)->ipMcMode != CPSS_EXMXPM_PP_CONFIG_DIT_IP_MC_DUAL_MODE_E)
        {
            /*
                1.1. Call with  ditEntryIndex[0],
                                ditType[CPSS_UNICAST_E],
                                not NULL ditEntryPtr
                Expectyed: GT_OK.
            */
            ditEntryIndex = 0;
            ditType       = CPSS_UNICAST_E;

            st = cpssExMxPmDitVplsEntryRead(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ditEntryIndex, ditType);

            /*
                1.3. Call with ditEntryPtr [NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmDitVplsEntryRead(dev, ditEntryIndex, ditType, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ditEntryPtr = NULL", dev);
        }
    }

    ditEntryIndex = 0;
    ditType       = CPSS_UNICAST_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitVplsEntryRead(dev, ditEntryIndex, ditType, &ditEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitVplsEntryRead(dev, ditEntryIndex, ditType, &ditEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitVplsEntryLastSet
(
    IN  GT_U8                            devNum,
    IN  GT_U32                           ditEntryIndex,
    IN  CPSS_UNICAST_MULTICAST_ENT       ditType,
    IN  GT_U32                           isLast
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitVplsEntryLastSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmDitVplsEntryLastSet with
                    ditEntryIndex[0 / 1],
                    ditType[CPSS_UNICAST_E / CPSS_MULTICAST_E],
                    ditEntryPtr{
                                outLif { {outlifType{CPSS_EXMXPM_OUTLIF_TYPE_LL_E}
                                         interfaceInfo{{type [CPSS_INTERFACE_PORT_E],
                                                              devPort->devNum[dev],
                                                              devPort->portNum[0]}}.}
                                         outlifPointer{arpPtr[0]} },
                                vlanId [100 / 4095],
                                ttl [0 / 100],
                                vlanTagCommand [CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_TAGGED_FROM_DIT_E / CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_UNTAGGED_E],
                                label [0 / 100],
                                exp [0 / 7],
                                qosExpCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E / CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_ENTRY_E],
                                up [0 / 7],
                                mplsCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E / CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_ENTRY_E],
                                vplsUnknownUcEnable [GT_FALSE / GT_TRUE],
                                vplsUnregisteredMcEnable [GT_FALSE / GT_TRUE],
                                pwId [0 / 100],
                                isLast[GT_FALSE / GT_TRUE] }
    Expected: GT_OK.
    1.2. Call with  ditEntryIndex [0],
                    ditType [CPSS_UNICAST_E],
                    isLast [0]
    Expected: GT_OK.
    1.4. Call with isLast [2] and other params from 1.2.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;


    GT_U32                           ditEntryIndex  = 0;
    CPSS_UNICAST_MULTICAST_ENT       ditType        = CPSS_UNICAST_E;
    GT_U32                           isLast         = 0;
    CPSS_EXMXPM_DIT_VPLS_STC         ditEntry;


    cpssOsBzero((GT_VOID*) &ditEntry, sizeof(ditEntry));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if (PRV_CPSS_EXMXPM_DIT_MODULE_MAC(dev)->ipMcMode != CPSS_EXMXPM_PP_CONFIG_DIT_IP_MC_DUAL_MODE_E)
        {
            /*
                1.1. Call cpssExMxPmDitVplsEntryLastSet with
                                ditEntryIndex[0],
                                ditType[CPSS_UNICAST_E],
                                ditEntryPtr{
                                            outLif { {outlifType{CPSS_EXMXPM_OUTLIF_TYPE_LL_E}
                                                     interfaceInfo{{type [CPSS_INTERFACE_PORT_E],
                                                                          devPort->devNum[dev],
                                                                          devPort->portNum[0]}}.}
                                                     outlifPointer{arpPtr[0]} },
                                            vlanId [100],
                                            ttl [0],
                                            vlanTagCommand [CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_TAGGED_FROM_DIT_E],
                                            label [0],
                                            exp [0],
                                            qosExpCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E],
                                            up [0],
                                            mplsCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E],
                                            vplsUnknownUcEnable [GT_FALSE],
                                            vplsUnregisteredMcEnable [GT_FALSE],
                                            pwId [0],
                                            isLast[GT_FALSE] }
                Expected: GT_OK.
            */
            ditEntryIndex = 0;
            ditType       = CPSS_UNICAST_E;
            ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;
            ditEntry.outLif.devPort.devNum  = dev;
            ditEntry.outLif.devPort.portNum = 0;
            ditEntry.vlanId = 100;
            ditEntry.ttl = 0;
            ditEntry.vlanTagCommand = CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_TAGGED_FROM_DIT_E;
            ditEntry.label = 0;
            ditEntry.exp   = 0;
            ditEntry.qosExpCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E;
            ditEntry.up = 0;
            ditEntry.mplsCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E;
            ditEntry.vplsUnknownUcEnable = GT_FALSE;
            ditEntry.vplsUnregisteredMcEnable = GT_FALSE;
            ditEntry.pwId = 0;
            ditEntry.isLast = GT_FALSE;

            st = cpssExMxPmDitVplsEntryWrite(dev, ditEntryIndex, ditType, &ditEntry);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ditEntryIndex, ditType);

            /*
                1.2. Call with  ditEntryIndex [0],
                                ditType [CPSS_UNICAST_E],
                                isLast [0]
                Expected: GT_OK.
            */
            ditEntryIndex = 0;
            ditType = CPSS_UNICAST_E;
            isLast  = 0;

            st = cpssExMxPmDitVplsEntryLastSet(dev, ditEntryIndex, ditType, isLast);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ditEntryIndex, ditType, isLast);

            /*
                1.4. Call with isLast [2] and other params from 1.2.
                Expected: NOT GT_OK.
            */
            isLast = 2;

            st = cpssExMxPmDitVplsEntryLastSet(dev, ditEntryIndex, ditType, isLast);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, isLast = %d", dev, isLast);
        }
    }

    ditEntryIndex = 0;
    ditType = CPSS_UNICAST_E;
    isLast  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitVplsEntryLastSet(dev, ditEntryIndex, ditType, isLast);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitVplsEntryLastSet(dev, ditEntryIndex, ditType, isLast);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

#if 0
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitCounterConfigSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  counterIndex,
    IN  CPSS_UNICAST_MULTICAST_ENT              type,
    IN  CPSS_EXMXPM_DIT_COUNTER_SET_CFG_STC     *counterCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitCounterConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with counterIndex [0],
                   type[CPSS_UNICAST_E],
                   and counterCfgPtr {portTrunkMode [CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E],
                                      packetType [CPSS_EXMXPM_IP_PACKET_TYPE_ALL_COUNTER_MODE_E],
                                      vlanMode [CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E]}.
    Expected: GT_OK.
    1.2. Call with counterIndex [1]
                   type[CPSS_UNICAST_E],
                   and counterCfgPtr {portTrunkMode [CPSS_EXMXPM_IP_PORT_COUNTER_MODE_E],
                                      devNum [dev],
                                      portTrunk.port [0],
                                      packetType [CPSS_EXMXPM_IP_PACKET_TYPE_IPV4_COUNTER_MODE_E],
                                      vlanMode [CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E],
                                      vlanId [100]}.
    Expected: GT_OK.
    1.3. Call with counterIndex [0]
                   type[CPSS_UNICAST_E],
                   and counterCfgPtr {portTrunkMode [CPSS_EXMXPM_IP_TRUNK_COUNTER_MODE_E],
                                      portTrunk.trunk [2],
                                      packetType [CPSS_EXMXPM_IP_PACKET_TYPE_IPV6_COUNTER_MODE_E],
                                      vlanMode [CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E],
                                      vlanId [4095]}.
    Expected: GT_OK.
    1.4. Call cpssExMxPmDitCounterConfigGet with counterIndex [0 / 1] and non-NULL counterCfgPtr.
    Expected: GT_OK and the same counterCfgPtr.
    1.5. Call with out of range counterIndex [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with out of range counterCfgPtr->portTrunkMode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.10. Call with out of range counterCfgPtr->portTrunk.port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                   and other parameters from 1.2.
    Expected: NOT GT_OK.
    1.11. Call with out of range counterCfgPtr->portTrunk.trunk [256] (not relevant)
                    and other parameters from 1.2.
    Expected: GT_OK.
    1.12. Call with out of range counterCfgPtr->portTrunk.trunk [256]
                    and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.13. Call with out of range counterCfgPtr->packetType [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.14. Call with out of range counterCfgPtr->vlanMode [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.16. Call with counterCfgPtr->vlanMode [CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E]
                    and out of range interfaceConfigPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.17. Call with counterCfgPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                              counterIndex = 0;
    CPSS_UNICAST_MULTICAST_ENT          type;
    CPSS_EXMXPM_DIT_COUNTER_SET_CFG_STC counterCfg;
    CPSS_EXMXPM_DIT_COUNTER_SET_CFG_STC counterCfgGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with counterIndex [0]
                           type[CPSS_UNICAST_E],
                           and counterCfgPtr {portTrunkMode [CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E],
                                              packetType [CPSS_EXMXPM_DIT_PACKET_TYPE_ALL_COUNTER_MODE_E],
                                              vlanMode [CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E]}.
            Expected: GT_OK.
        */
        counterIndex = 0;
        type = CPSS_UNICAST_E;
        counterCfg.portTrunkMode = CPSS_EXMXPM_DIT_DISREGARD_PORT_TRUNK_COUNTER_MODE_E;
        counterCfg.packetType    = CPSS_EXMXPM_DIT_PACKET_TYPE_ALL_COUNTER_MODE_E;
        counterCfg.vlanMode      = CPSS_EXMXPM_DIT_DISREGARD_VLAN_COUNTER_MODE_E;

        st = cpssExMxPmDitCounterConfigSet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /*
            1.4. Call cpssExMxPmDitCounterConfigGet with counterIndex [0 / 1] and non-NULL counterCfgPtr.
            Expected: GT_OK and the same counterCfgPtr.
        */
        st = cpssExMxPmDitCounterConfigGet(dev, counterIndex, type, &counterCfgGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmDitCounterConfigGet: %d, %d, %d", dev, counterIndex, type);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(counterCfg.portTrunkMode, counterCfgGet.portTrunkMode,
                   "get another counterCfgPtr->portTrunkMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterCfg.packetType, counterCfgGet.packetType,
                   "get another counterCfgPtr->packetType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterCfg.vlanMode, counterCfgGet.vlanMode,
                   "get another counterCfgPtr->vlanMode than was set: %d", dev);

        /*
            1.2. Call with counterIndex [1]
                           type[CPSS_UNICAST_E],
                           and counterCfgPtr {portTrunkMode [CPSS_EXMXPM_IP_PORT_COUNTER_MODE_E],
                                              devNum [dev],
                                              portTrunk.port [0],
                                              packetType [CPSS_EXMXPM_IP_PACKET_TYPE_IPV4_COUNTER_MODE_E],
                                              vlanMode [CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E],
                                              vlanId [100]}.
            Expected: GT_OK.
        */
        counterIndex = 1;
        type = CPSS_UNICAST_E;
        counterCfg.portTrunkMode  = CPSS_EXMXPM_DIT_PORT_COUNTER_MODE_E;
        counterCfg.portTrunk.devPort.devNum = dev;
        counterCfg.portTrunk.devPort.port   = 0;
        counterCfg.packetType     = CPSS_EXMXPM_DIT_PACKET_TYPE_IPV4_COUNTER_MODE_E;
        counterCfg.vlanMode       = CPSS_EXMXPM_DIT_USE_VLAN_COUNTER_MODE_E;
        counterCfg.vlanId         = DIT_TESTED_VLAN_ID_CNS;

        st = cpssExMxPmDitCounterConfigSet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /*
            1.4. Call cpssExMxPmDitCounterConfigGet with counterIndex [0 / 1] and non-NULL counterCfgPtr.
            Expected: GT_OK and the same counterCfgPtr.
        */
        st = cpssExMxPmDitCounterConfigGet(dev, counterIndex, type, &counterCfgGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmDitCounterConfigGet: %d, %d, %d", dev, counterIndex, type);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(counterCfg.portTrunkMode, counterCfgGet.portTrunkMode,
                   "get another counterCfgPtr->portTrunkMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterCfg.portTrunk.devPort.devNum, counterCfgGet.portTrunk.devPort.devNum,
                   "get another counterCfgPtr->.portTrunk.devPort.devNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterCfg.portTrunk.devPort.port, counterCfgGet.portTrunk.devPort.port,
                   "get another counterCfgPtr->portTrunk.devPort.port than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterCfg.packetType, counterCfgGet.packetType,
                   "get another counterCfgPtr->packetType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterCfg.vlanMode, counterCfgGet.vlanMode,
                   "get another counterCfgPtr->vlanMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterCfg.vlanId, counterCfgGet.vlanId,
                   "get another counterCfgPtr->vlanId than was set: %d", dev);

        /*
            1.3. Call with counterIndex [0]
                           type[CPSS_UNICAST_E],
                           and counterCfgPtr {portTrunkMode [CPSS_EXMXPM_IP_TRUNK_COUNTER_MODE_E],
                                              portTrunk.trunk [2],
                                              packetType [CPSS_EXMXPM_DIT_PACKET_TYPE_IPV6_COUNTER_MODE_E],
                                              vlanMode [CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E],
                                              vlanId [4095]}.
            Expected: GT_OK.
        */
        counterIndex = 0;
        type = CPSS_UNICAST_E;
        counterCfg.portTrunkMode   = CPSS_EXMXPM_DIT_TRUNK_COUNTER_MODE_E;
        counterCfg.portTrunk.trunk = 2;
        counterCfg.packetType      = CPSS_EXMXPM_DIT_PACKET_TYPE_IPV6_COUNTER_MODE_E;
        counterCfg.vlanMode        = CPSS_EXMXPM_DIT_USE_VLAN_COUNTER_MODE_E;
        counterCfg.vlanId          = 4095;

        st = cpssExMxPmDitCounterConfigSet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /*
            1.4. Call cpssExMxPmDitCounterConfigGet with counterIndex [0 / 1] and non-NULL counterCfgPtr.
            Expected: GT_OK and the same counterCfgPtr.
        */
        st = cpssExMxPmDitCounterConfigGet(dev, counterIndex, type, &counterCfgGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssExMxPmDitCounterConfigGet: %d, %d, %d", dev, counterIndex, type);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(counterCfg.portTrunkMode, counterCfgGet.portTrunkMode,
                   "get another counterCfgPtr->portTrunkMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterCfg.portTrunk.trunk, counterCfgGet.portTrunk.trunk,
                   "get another counterCfgPtr->portTrunk.trunk than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterCfg.packetType, counterCfgGet.packetType,
                   "get another counterCfgPtr->packetType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterCfg.vlanMode, counterCfgGet.vlanMode,
                   "get another counterCfgPtr->vlanMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counterCfg.vlanId, counterCfgGet.vlanId,
                   "get another counterCfgPtr->vlanId than was set: %d", dev);

        /*
            1.5. Call with out of range counterIndex [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        counterIndex = 2;

        st = cpssExMxPmDitCounterConfigSet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        counterIndex = 0;

        /*
            1.6. Call with out of range counterCfgPtr->portTrunkMode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        counterCfg.portTrunkMode = DIT_INVALID_ENUM_CNS;

        st = cpssExMxPmDitCounterConfigSet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, counterCfgPtr->portTrunkMode = %d",
                                     dev, counterCfg.portTrunkMode);

        counterCfg.portTrunkMode = CPSS_EXMXPM_DIT_DISREGARD_PORT_TRUNK_COUNTER_MODE_E;

        /*
            1.10. Call with out of range counterCfgPtr->portTrunk.port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                            and other parameters from 1.2.
            Expected: NOT GT_OK.
        */
        counterCfg.portTrunkMode          = CPSS_EXMXPM_DIT_PORT_COUNTER_MODE_E;
        counterCfg.portTrunk.devPort.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmDitCounterConfigSet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, counterCfgPtr->portTrunkMode = %d, counterCfgPtr->portTrunk.devPort.port = %d",
                                         dev, counterCfg.portTrunkMode, counterCfg.portTrunk.devPort.port);

        counterCfg.portTrunk.devPort.port = 0;

        /*
            1.11. Call with out of range counterCfgPtr->portTrunk.trunk [256] (not relevant)
                            and other parameters from 1.2.
            Expected: GT_OK.
        */
        counterCfg.portTrunk.trunk = 256;

        st = cpssExMxPmDitCounterConfigSet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, counterCfgPtr->portTrunkMode = %d, counterCfgPtr->portTrunk.trunk = %d",
                                     dev, counterCfg.portTrunkMode, counterCfg.portTrunk.trunk);

        /*
            1.12. Call with out of range counterCfgPtr->portTrunk.trunk [256]
                            and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        counterCfg.portTrunkMode = CPSS_EXMXPM_DIT_TRUNK_COUNTER_MODE_E;

        st = cpssExMxPmDitCounterConfigSet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, counterCfgPtr->portTrunkMode = %d, counterCfgPtr->portTrunk.trunk = %d",
                                         dev, counterCfg.portTrunkMode, counterCfg.portTrunk.trunk);

        counterCfg.portTrunk.trunk = 2;

        /*
            1.13. Call with out of range counterCfgPtr->packetType [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        counterCfg.portTrunkMode = CPSS_EXMXPM_DIT_DISREGARD_PORT_TRUNK_COUNTER_MODE_E;
        counterCfg.packetType    = DIT_INVALID_ENUM_CNS;

        st = cpssExMxPmDitCounterConfigSet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, counterCfgPtr->packetType = %d",
                                     dev, counterCfg.packetType);

        counterCfg.packetType = CPSS_EXMXPM_DIT_PACKET_TYPE_ALL_COUNTER_MODE_E;

        /*
            1.14. Call with out of range counterCfgPtr->vlanMode [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        counterCfg.vlanMode = DIT_INVALID_ENUM_CNS;

        st = cpssExMxPmDitCounterConfigSet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, counterCfgPtr->vlanMode = %d",
                                     dev, counterCfg.vlanMode);

        counterCfg.vlanMode = CPSS_EXMXPM_DIT_USE_VLAN_COUNTER_MODE_E;

        /*
            1.16. Call with counterCfgPtr->vlanMode [CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E]
                            and out of range counterCfgPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        counterCfg.vlanMode = CPSS_EXMXPM_DIT_DISREGARD_VLAN_COUNTER_MODE_E;

        st = cpssExMxPmDitCounterConfigSet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, counterCfgPtr->vlanMode = %d, counterCfgPtr->vlanId= %d",
                                     dev, counterCfg.vlanMode, counterCfg.vlanId);

        counterCfg.vlanId = DIT_TESTED_VLAN_ID_CNS;

        /*
            1.17. Call with counterCfgPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitCounterConfigSet(dev, counterIndex, type, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterCfgPtr = NULL", dev);
    }

    counterIndex = 0;
    type = CPSS_UNICAST_E;
    counterCfg.portTrunkMode = CPSS_EXMXPM_DIT_DISREGARD_PORT_TRUNK_COUNTER_MODE_E;
    counterCfg.packetType    = CPSS_EXMXPM_DIT_PACKET_TYPE_ALL_COUNTER_MODE_E;
    counterCfg.vlanMode      = CPSS_EXMXPM_DIT_DISREGARD_VLAN_COUNTER_MODE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitCounterConfigSet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitCounterConfigSet(dev, counterIndex, type, &counterCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitCounterConfigGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  counterIndex,
    IN  CPSS_UNICAST_MULTICAST_ENT              type,
    OUT CPSS_EXMXPM_DIT_COUNTER_SET_CFG_STC     *counterCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitCounterConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with counterIndex [0 / 1]
                   type[CPSS_UNICAST_E],
                   and non-NULL counterCfgPtr.
    Expected: GT_OK.
    1.2. Call with out of range counterIndex [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range type [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with counterCfgPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                               counterIndex = 0;
    CPSS_EXMXPM_DIT_COUNTER_SET_CFG_STC  counterCfg;
    CPSS_UNICAST_MULTICAST_ENT           type;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with counterIndex [0 / 1],
                           type[CPSS_UNICAST_E],
                           and non-NULL counterCfgPtr.
            Expected: GT_OK.
        */

        /* Call with counterIndex [0] */
        counterIndex = 0;
        type = CPSS_UNICAST_E;

        st = cpssExMxPmDitCounterConfigGet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /* Call with counterIndex [1] */
        counterIndex = 1;

        st = cpssExMxPmDitCounterConfigGet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /*
            1.2. Call with out of range counterIndex [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        counterIndex = 2;

        st = cpssExMxPmDitCounterConfigGet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        counterIndex = 0;

        /*
            1.3. Call with out of range type [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        type = DIT_INVALID_ENUM_CNS;

        st = cpssExMxPmDitCounterConfigGet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, type = %d", dev, type);

        type = CPSS_MULTICAST_E;

        /*
            1.4. Call with counterCfgPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitCounterConfigGet(dev, counterIndex, type, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterCfgPtr = NULL", dev);
    }

    counterIndex = 0;
    type = CPSS_UNICAST_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitCounterConfigGet(dev, counterIndex, type, &counterCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitCounterConfigGet(dev, counterIndex, type, &counterCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
#endif

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitCounterGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          counterIndex,
    IN  CPSS_UNICAST_MULTICAST_ENT      type,
    OUT GT_U32                          *valuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitCounterGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with counterIndex [0 / 1]
                   type [CPSS_UNICAST_E / CPSS_MULTICAST_E],
                   and non-null valuePtr.
    Expected: GT_OK.
    1.2. Call with out of range counterIndex [2]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range type [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with valuePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      counterIndex = 0;
    GT_U32      value        = 0;

    CPSS_UNICAST_MULTICAST_ENT  type = CPSS_UNICAST_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with counterIndex [0 / 1]
                           type [CPSS_UNICAST_E / CPSS_MULTICAST_E],
                           and non-null valuePtr.
            Expected: GT_OK.
        */

        /* Call with counterIndex [0] */
        counterIndex = 0;
        type         = CPSS_UNICAST_E;

        st = cpssExMxPmDitCounterGet(dev, counterIndex, type, &value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /* Call with counterIndex [1] */
        counterIndex = 1;
        type         = CPSS_MULTICAST_E;

        st = cpssExMxPmDitCounterGet(dev, counterIndex, type, &value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /*
            1.2. Call with out of range counterIndex [2]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        counterIndex = 2;

        st = cpssExMxPmDitCounterGet(dev, counterIndex, type, &value);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        counterIndex = 0;

        /*
            1.3. Call with out of range type [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        type = DIT_INVALID_ENUM_CNS;

        st = cpssExMxPmDitCounterGet(dev, counterIndex, type, &value);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, type = %d", dev, type);

        type = CPSS_MULTICAST_E;

        /*
            1.4. Call with valuePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitCounterGet(dev, counterIndex, type, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, valuePtr = NULL", dev);
    }

    counterIndex = 0;
    type = CPSS_MULTICAST_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitCounterGet(dev, counterIndex, type, &value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitCounterGet(dev, counterIndex, type, &value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitBasedBridgeEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitBasedBridgeEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_TRUE / GT_FALSE]
    Expected: GT_OK
    1.2. Call cpssExMxPmDitBasedBridgeEnableGet with not NULL enablePtr
    Expected: GT_OK and the same params as was set.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_TRUE / GT_FALSE]
            Expected: GT_OK
        */
        enable = GT_TRUE;

        st = cpssExMxPmDitBasedBridgeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmDitBasedBridgeEnableGet with not NULL enablePtr
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmDitBasedBridgeEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmDitBasedBridgeEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got anothe enable than was set: %d", dev);

        enable = GT_FALSE;

        st = cpssExMxPmDitBasedBridgeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmDitBasedBridgeEnableGet with not NULL enablePtr
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmDitBasedBridgeEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmDitBasedBridgeEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got anothe enable than was set: %d", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitBasedBridgeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitBasedBridgeEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitBasedBridgeEnableGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitBasedBridgeEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL enablePtr
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL enablePtr
            Expected: GT_OK.
        */
        st = cpssExMxPmDitBasedBridgeEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitBasedBridgeEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitBasedBridgeEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitBasedBridgeEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitMultiTargetMcQueuePriorityConfigSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  queue,
    IN  GT_U32  priority
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitMultiTargetMcQueuePriorityConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with queue [0 / 3], priority [0 / 3].
    Expected: GT_OK.
    1.2. Call cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet with not NULL priorityPtr
             and other params from 1.1.
    Expected: GT_OK and the same params as was set
    1.3. Call with queue[4] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with priority[4] and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32  queue       = 0;
    GT_U32  priority    = 0;
    GT_U32  priorityGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with queue [0 / 3], priority [0 / 3].
            Expected: GT_OK.
        */
        /* iterate with queue = 0 */
        queue    = 0;
        priority = 0;

        st = cpssExMxPmDitMultiTargetMcQueuePriorityConfigSet(dev, queue, priority);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, priority);

        /*
            1.2. Call cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet with not NULL priorityPtr
                     and other params from 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet(dev, queue, &priorityGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet: %d, %d",
                                    dev, queue);

        UTF_VERIFY_EQUAL1_STRING_MAC(priority, priorityGet, "got another priority than was set: %d, %d", dev);

        /* iterate with queue = 3 */
        queue    = 3;
        priority = 3;

        st = cpssExMxPmDitMultiTargetMcQueuePriorityConfigSet(dev, queue, priority);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, queue, priority);

        /*
            1.2. Call cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet with not NULL priorityPtr
                     and other params from 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet(dev, queue, &priorityGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet: %d, %d",
                                    dev, queue);

        UTF_VERIFY_EQUAL1_STRING_MAC(priority, priorityGet, "got another priority than was set: %d, %d", dev);

        /*
            1.3. Call with queue[4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        queue = 4;

        st = cpssExMxPmDitMultiTargetMcQueuePriorityConfigSet(dev, queue, priority);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, queue);

        queue = 3;

        /*
            1.4. Call with priority[4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        priority = 4;

        st = cpssExMxPmDitMultiTargetMcQueuePriorityConfigSet(dev, queue, priority);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, "%d, priority = %d", dev, priority);
    }

    queue    = 0;
    priority = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitMultiTargetMcQueuePriorityConfigSet(dev, queue, priority);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitMultiTargetMcQueuePriorityConfigSet(dev, queue, priority);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  queue,
    OUT GT_U32  *priorityPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with queue [0], not NULL priorityPtr.
    Expected: GT_OK.
    1.2. Call with queue[4] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with priority[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32  queue       = 0;
    GT_U32  priority    = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with queue [0], not NULL priorityPtr.
            Expected: GT_OK.
        */
        queue = 0;

        st = cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet(dev, queue, &priority);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, queue);

        /*
            1.2. Call with queue[4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        queue = 4;

        st = cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet(dev, queue, &priority);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, queue);

        queue = 0;

        /*
            1.3. Call with priority[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet(dev, queue, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, priorityPtr = NULL", dev);
    }

    queue    = 0;


    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet(dev, queue, &priority);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet(dev, queue, &priority);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitMcTc2IpTvQueueConfigSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  fullThreshold,
    IN  GT_U32  xoffThreshold,
    IN  GT_U32  xonThreshold
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitMcTc2IpTvQueueConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with fullThreshold[0 / 10], xoffThreshold[0 / 20], xonThreshold[0 / 30].
    Expected: GT_OK.
    1.2. Call cpssExMxPmDitMcTc2IpTvQueueConfigGet with not NULL pointers.
    Expected: GT_OK and the same params as was set.
    1.3. Call with fullThreshold [0x400] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with xoffThreshold [0x400] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with xonThreshold [0x400] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32  fullThreshold = 0;
    GT_U32  xoffThreshold = 0;
    GT_U32  xonThreshold = 0;

    GT_U32  fullThresholdGet = 0;
    GT_U32  xoffThresholdGet = 0;
    GT_U32  xonThresholdGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with fullThreshold[0 / 10], xoffThreshold[0 / 20], xonThreshold[0 / 30].
            Expected: GT_OK.
        */
        fullThreshold = 0;
        xoffThreshold = 0;
        xonThreshold = 0;

        st = cpssExMxPmDitMcTc2IpTvQueueConfigSet(dev, fullThreshold, xoffThreshold, xonThreshold);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fullThreshold, xoffThreshold, xonThreshold);

        /*
            1.2. Call cpssExMxPmDitMcTc2IpTvQueueConfigGet with not NULL pointers.
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmDitMcTc2IpTvQueueConfigGet(dev, &fullThresholdGet, &xoffThresholdGet, &xonThresholdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmDitMcTc2IpTvQueueConfigGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(fullThreshold, fullThresholdGet, "got another fullThreshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xoffThreshold, xoffThresholdGet, "got another xoffThreshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xonThreshold, xonThresholdGet, "got another xonThreshold than was set: %d", dev);

        fullThreshold = 10;
        xoffThreshold = 20;
        xonThreshold  = 30;

        st = cpssExMxPmDitMcTc2IpTvQueueConfigSet(dev, fullThreshold, xoffThreshold, xonThreshold);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fullThreshold, xoffThreshold, xonThreshold);

        /*
            1.2. Call cpssExMxPmDitMcTc2IpTvQueueConfigGet with not NULL pointers.
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmDitMcTc2IpTvQueueConfigGet(dev, &fullThresholdGet, &xoffThresholdGet, &xonThresholdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmDitMcTc2IpTvQueueConfigGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(fullThreshold, fullThresholdGet, "got another fullThreshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xoffThreshold, xoffThresholdGet, "got another xoffThreshold than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(xonThreshold, xonThresholdGet, "got another xonThreshold than was set: %d", dev);

        /*
            1.3. Call with fullThreshold [0x400] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.
        */
        fullThreshold = 0x400;

        st = cpssExMxPmDitMcTc2IpTvQueueConfigSet(dev, fullThreshold, xoffThreshold, xonThreshold);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fullThreshold);

        fullThreshold = 10;

        /*
            1.4. Call with xoffThreshold [0x400] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.
        */
        xoffThreshold = 0x400;

        st = cpssExMxPmDitMcTc2IpTvQueueConfigSet(dev, fullThreshold, xoffThreshold, xonThreshold);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, xoffThreshold = %d", dev, xoffThreshold);

        xoffThreshold = 10;

        /*
            1.5. Call with xonThreshold [0x400] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.
        */
        xonThreshold = 0x400;

        st = cpssExMxPmDitMcTc2IpTvQueueConfigSet(dev, fullThreshold, xoffThreshold, xonThreshold);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, xonThreshold = %d", dev, xonThreshold);
    }

    fullThreshold = 0;
    xoffThreshold = 0;
    xonThreshold = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitMcTc2IpTvQueueConfigSet(dev, fullThreshold, xoffThreshold, xonThreshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitMcTc2IpTvQueueConfigSet(dev, fullThreshold, xoffThreshold, xonThreshold);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitMcTc2IpTvQueueConfigGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *fullThresholdPtr,
    OUT GT_U32  *xoffThresholdPtr,
    OUT GT_U32  *xonThresholdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitMcTc2IpTvQueueConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL pointers
    Expected: GT_OK.
    1.2. Call with fullThresholdPtr [NULL]
    Expected: GT_BAD_PTR.
    1.3. Call with xoffThresholdPtr [NULL]
    Expected: GT_BAD_PTR.
    1.4. Call with xonThresholdPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32  fullThreshold = 0;
    GT_U32  xoffThreshold = 0;
    GT_U32  xonThreshold = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL pointers
            Expected: GT_OK.
        */
        st = cpssExMxPmDitMcTc2IpTvQueueConfigGet(dev, &fullThreshold, &xoffThreshold, &xonThreshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with fullThresholdPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitMcTc2IpTvQueueConfigGet(dev, NULL, &xoffThreshold, &xonThreshold);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, fullThresholdPtr = NULL", dev);

        /*
            1.3. Call with xoffThresholdPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitMcTc2IpTvQueueConfigGet(dev, &fullThreshold, NULL, &xonThreshold);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, xoffThresholdPtr = NULL", dev);

        /*
            1.4. Call with xonThresholdPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitMcTc2IpTvQueueConfigGet(dev, &fullThreshold, &xoffThreshold, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, xonThresholdPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitMcTc2IpTvQueueConfigGet(dev, &fullThreshold, &xoffThreshold, &xonThreshold);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitMcTc2IpTvQueueConfigGet(dev, &fullThreshold, &xoffThreshold, &xonThreshold);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitMcTc2IpTvQueueIpTvXonStatusGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *isQueueEmptyPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitMcTc2IpTvQueueIpTvXonStatusGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL isQueueEmptyPtr
    Expected: GT_OK.
    1.2. Call with isQueueEmptyPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL     isQueueEmpty = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL isQueueEmptyPtr
            Expected: GT_OK.
        */
        st = cpssExMxPmDitMcTc2IpTvQueueIpTvXonStatusGet(dev, &isQueueEmpty);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with isQueueEmptyPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitMcTc2IpTvQueueIpTvXonStatusGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, isQueueEmptyPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitMcTc2IpTvQueueIpTvXonStatusGet(dev, &isQueueEmpty);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitMcTc2IpTvQueueIpTvXonStatusGet(dev, &isQueueEmpty);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitMultiTargetTCQueueConfigSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  analyzerMcQueue,
    IN  GT_U32  mcRpfFailTcQueue,
    IN  GT_U32  controlMcTcQueue
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitMultiTargetTCQueueConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with analyzerMcQueue [0 / 3], mcRpfFailTcQueue [0 / 3], controlMcTcQueue[0 / 3].
    Expected: GT_OK.
    1.2. Call cpssExMxPmDitMultiTargetTCQueueConfigGet
              with not NULL analyzerMcQueuePtr, mcRpfFailTcQueuePtr, controlMcTcQueuePtr.
              and other params from 1.1.
    Expected: GT_OK and the same params as was set.
    1.3. Call with analyzerMcQueue[4] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with mcRpfFailTcQueue[4] and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with controlMcTcQueue[4] and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32  analyzerMcQueue  = 0;
    GT_U32  mcRpfFailTcQueue = 0;
    GT_U32  controlMcTcQueue = 0;
    GT_U32  analyzerMcQueueGet  = 0;
    GT_U32  mcRpfFailTcQueueGet = 0;
    GT_U32  controlMcTcQueueGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with analyzerMcQueue [0 / 3], mcRpfFailTcQueue [0 / 3], controlMcTcQueue[0 / 3].
            Expected: GT_OK.
        */
        /* iterate with analyzerMcQueue = 0 */
        analyzerMcQueue  = 0;
        mcRpfFailTcQueue = 0;
        controlMcTcQueue = 0;

        st = cpssExMxPmDitMultiTargetTCQueueConfigSet(dev, analyzerMcQueue, mcRpfFailTcQueue, controlMcTcQueue);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, analyzerMcQueue, mcRpfFailTcQueue, controlMcTcQueue);

        /*
            1.2. Call cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet with not NULL priorityPtr
                     and other params from 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxPmDitMultiTargetTCQueueConfigGet(dev, &analyzerMcQueueGet, &mcRpfFailTcQueueGet, &controlMcTcQueueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmDitMultiTargetTCQueueConfigGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(analyzerMcQueue, analyzerMcQueueGet, "got another analyzerMcQueue than was set: %d, %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mcRpfFailTcQueue, mcRpfFailTcQueueGet, "got another mcRpfFailTcQueue than was set: %d, %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(controlMcTcQueue, controlMcTcQueueGet, "got another controlMcTcQueue than was set: %d, %d", dev);

        /* iterate with analyzerMcQueue = 3 */
        analyzerMcQueue  = 3;
        mcRpfFailTcQueue = 3;
        controlMcTcQueue = 3;

        st = cpssExMxPmDitMultiTargetTCQueueConfigSet(dev, analyzerMcQueue, mcRpfFailTcQueue, controlMcTcQueue);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, analyzerMcQueue, mcRpfFailTcQueue, controlMcTcQueue);

        /*
            1.2. Call cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet with not NULL priorityPtr
                     and other params from 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxPmDitMultiTargetTCQueueConfigGet(dev, &analyzerMcQueueGet, &mcRpfFailTcQueueGet, &controlMcTcQueueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmDitMultiTargetTCQueueConfigGet: %d", dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(analyzerMcQueue, analyzerMcQueueGet, "got another analyzerMcQueue than was set: %d, %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mcRpfFailTcQueue, mcRpfFailTcQueueGet, "got another mcRpfFailTcQueue than was set: %d, %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(controlMcTcQueue, controlMcTcQueueGet, "got another controlMcTcQueue than was set: %d, %d", dev);

        /*
            1.3. Call with analyzerMcQueue[4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        analyzerMcQueue = 4;

        st = cpssExMxPmDitMultiTargetTCQueueConfigSet(dev, analyzerMcQueue, mcRpfFailTcQueue, controlMcTcQueue);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, analyzerMcQueue);

        analyzerMcQueue = 3;

        /*
            1.4. Call with mcRpfFailTcQueue[4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        mcRpfFailTcQueue = 4;

        st = cpssExMxPmDitMultiTargetTCQueueConfigSet(dev, analyzerMcQueue, mcRpfFailTcQueue, controlMcTcQueue);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, mcRpfFailTcQueue = %d", dev, mcRpfFailTcQueue);

        mcRpfFailTcQueue = 3;
        /*
            1.5. Call with controlMcTcQueue[4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        controlMcTcQueue = 4;

        st = cpssExMxPmDitMultiTargetTCQueueConfigSet(dev, analyzerMcQueue, mcRpfFailTcQueue, controlMcTcQueue);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, controlMcTcQueue = %d", dev, controlMcTcQueue);
    }

    analyzerMcQueue  = 0;
    mcRpfFailTcQueue = 0;
    controlMcTcQueue = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitMultiTargetTCQueueConfigSet(dev, analyzerMcQueue, mcRpfFailTcQueue, controlMcTcQueue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitMultiTargetTCQueueConfigSet(dev, analyzerMcQueue, mcRpfFailTcQueue, controlMcTcQueue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitMultiTargetTCQueueConfigGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *analyzerMcQueuePtr,
    OUT GT_U32  *mcRpfFailTcQueuePtr,
    OUT GT_U32  *controlMcTcQueuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitMultiTargetTCQueueConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL analyzerMcQueuePtr, mcRpfFailTcQueuePtr, controlMcTcQueuePtr.
    Expected: GT_OK.
    1.2. Call with analyzerMcQueuePtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with mcRpfFailTcQueuePtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with controlMcTcQueuePtr[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32  analyzerMcQueue  = 0;
    GT_U32  mcRpfFailTcQueue = 0;
    GT_U32  controlMcTcQueue = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL analyzerMcQueuePtr, mcRpfFailTcQueuePtr, controlMcTcQueuePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmDitMultiTargetTCQueueConfigGet(dev, &analyzerMcQueue, &mcRpfFailTcQueue, &controlMcTcQueue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with analyzerMcQueuePtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitMultiTargetTCQueueConfigGet(dev, NULL, &mcRpfFailTcQueue, &controlMcTcQueue);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, analyzerMcQueuePtrPtr = NULL", dev);

        /*
            1.3. Call with mcRpfFailTcQueuePtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitMultiTargetTCQueueConfigGet(dev, &analyzerMcQueue, NULL, &controlMcTcQueue);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mcRpfFailTcQueuePtr = NULL", dev);

        /*
            1.4. Call with controlMcTcQueuePtr[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitMultiTargetTCQueueConfigGet(dev, &analyzerMcQueue, &mcRpfFailTcQueue, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, controlMcTcQueuePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitMultiTargetTCQueueConfigGet(dev, &analyzerMcQueue, &mcRpfFailTcQueue, &controlMcTcQueue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitMultiTargetTCQueueConfigGet(dev, &analyzerMcQueue, &mcRpfFailTcQueue, &controlMcTcQueue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  tcQueue,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcQueue [0 / 3], enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet with not NULL enablePtr
             and other params from 1.1.
    Expected: GT_OK and the same enable as was set
    1.3. Call with tcQueue[4] and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32  tcQueue   = 0;
    GT_BOOL enable    = GT_FALSE;
    GT_BOOL enableGet = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcQueue [0 / 3], enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* iterate with tcQueue = 0 */
        tcQueue = 0;
        enable  = GT_TRUE;

        st = cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableSet(dev, tcQueue, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcQueue, enable);

        /*
            1.2. Call cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet with not NULL enablePtr
                     and other params from 1.1.
            Expected: GT_OK and the same enable as was set
        */
        st = cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet(dev, tcQueue, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet: %d, %d",
                                    dev, tcQueue);

        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d, %d", dev);

        /* iterate with tcQueue = 3 */
        tcQueue = 3;
        enable  = GT_TRUE;

        st = cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableSet(dev, tcQueue, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcQueue, enable);

        /*
            1.2. Call cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet with not NULL priorityPtr
                     and other params from 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet(dev, tcQueue, &enableGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet: %d, %d",
                                    dev, tcQueue);

        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d, %d", dev);

        /*
            1.3. Call with queue[4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        tcQueue = 4;

        st = cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableSet(dev, tcQueue, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcQueue);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableSet(dev, tcQueue, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableSet(dev, tcQueue, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  tcQueue,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcQueue [3], not NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with tcQueue[4] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with enablePtr[NULL] and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32  tcQueue   = 0;
    GT_BOOL enable    = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcQueue [3], not NULL enablePtr.
            Expected: GT_OK.
        */
        tcQueue = 3;

        st = cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet(dev, tcQueue, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcQueue);

        /*
            1.2. Call with tcQueue[4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        tcQueue = 4;

        st = cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet(dev, tcQueue, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcQueue);

        tcQueue = 3;

        /*
            1.3. Call with enablePtr[NULL] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        st = cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet(dev, tcQueue, NULL);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, "%d, enablePtr = NULL", dev);
    }

    tcQueue = 3;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet(dev, tcQueue, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet(dev, tcQueue, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitMultiTargetTCQueueWeightSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  tcQueue,
    IN  GT_U32  weight
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitMultiTargetTCQueueWeightSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcQueue [0 / 3], weight [0 / 255].
    Expected: GT_OK.
    1.2. Call cpssExMxPmDitMultiTargetTCQueueWeightGet with not NULL enablePtr
             and other params from 1.1.
    Expected: GT_OK and the same enable as was set
    1.3. Call with tcQueue[4] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with weight[256] and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32  tcQueue   = 0;
    GT_U32  weight    = 0;
    GT_U32  weightGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcQueue [0 / 3], weight [0 / 255].
            Expected: GT_OK.
        */
        /* iterate with tcQueue = 0 */
        tcQueue = 0;
        weight  = 0;

        st = cpssExMxPmDitMultiTargetTCQueueWeightSet(dev, tcQueue, weight);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcQueue, weight);

        /*
            1.2. Call cpssExMxPmDitMultiTargetTCQueueWeightGet with not NULL enablePtr
                     and other params from 1.1.
            Expected: GT_OK and the same enable as was set
        */
        st = cpssExMxPmDitMultiTargetTCQueueWeightGet(dev, tcQueue, &weightGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmDitMultiTargetTCQueueWeightGet: %d, %d", dev, tcQueue);

        /* iterate with tcQueue = 3 */
        tcQueue = 3;
        weight  = 255;

        st = cpssExMxPmDitMultiTargetTCQueueWeightSet(dev, tcQueue, weight);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, tcQueue, weight);

        /*
            1.2. Call cpssExMxPmDitMultiTargetTCQueueWeightGet with not NULL enablePtr
                     and other params from 1.1.
            Expected: GT_OK and the same enable as was set
        */
        st = cpssExMxPmDitMultiTargetTCQueueWeightGet(dev, tcQueue, &weightGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmDitMultiTargetTCQueueWeightGet: %d, %d", dev, tcQueue);

        /*
            1.3. Call with tcQueue[4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        tcQueue = 4;

        st = cpssExMxPmDitMultiTargetTCQueueWeightSet(dev, tcQueue, weight);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcQueue);

        tcQueue = 3;

        /*
            1.4. Call with weight[256] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        weight = 256;

        st = cpssExMxPmDitMultiTargetTCQueueWeightSet(dev, tcQueue, weight);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, weight = %d", dev, weight);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitMultiTargetTCQueueWeightSet(dev, tcQueue, weight);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitMultiTargetTCQueueWeightSet(dev, tcQueue, weight);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitMultiTargetTCQueueWeightGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  tcQueue,
    OUT GT_U32  *weightPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitMultiTargetTCQueueWeightGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with tcQueue [0 / 3], not NULL weightPtr.
    Expected: GT_OK.
    1.2. Call with tcQueue[4] and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with weight[NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32  tcQueue   = 0;
    GT_U32  weight    = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tcQueue [0 / 3], not NULL weightPtr.
            Expected: GT_OK.
        */
        tcQueue = 0;

        st = cpssExMxPmDitMultiTargetTCQueueWeightGet(dev, tcQueue, &weight);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcQueue);

        tcQueue = 3;

        st = cpssExMxPmDitMultiTargetTCQueueWeightGet(dev, tcQueue, &weight);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcQueue);

        /*
            1.2. Call with tcQueue[4] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        tcQueue = 4;

        st = cpssExMxPmDitMultiTargetTCQueueWeightGet(dev, tcQueue, &weight);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tcQueue);

        tcQueue = 0;

        /*
            1.3. Call with weight[NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitMultiTargetTCQueueWeightGet(dev, tcQueue, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, weightPtr = NULL", dev);
    }

    tcQueue = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitMultiTargetTCQueueWeightGet(dev, tcQueue, &weight);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitMultiTargetTCQueueWeightGet(dev, tcQueue, &weight);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitRateShaperSet
(
    IN   GT_U8      devNum,
    IN   GT_BOOL    enable,
    IN   GT_U32     windowSize,
    IN   GT_U32     csuAccessRate,
    IN   GT_U32     mcAccessRate
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitRateShaperSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable[GT_FALSE / GT_TRUE], windowSize[0 / 100], csuAccessRate[0 / 7], mcAccessRate[0 / 127].
    Expected: GT_OK.
    1.2. Call cpssExMxPmDitRateShaperGet with not NULL pointers and other params from 1.1.
    Expected: GT_OK and the same params from 1.1.
    1.3. Call with csuAccessRate[8] and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with mcAccessRate[128] and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with windowSize[65536] and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL    enable = GT_FALSE;
    GT_U32     windowSize = 0;
    GT_U32     csuAccessRate = 0;
    GT_U32     mcAccessRate = 0;

    GT_BOOL    enableGet = GT_FALSE;
    GT_U32     windowSizeGet = 0;
    GT_U32     csuAccessRateGet = 0;
    GT_U32     mcAccessRateGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable[GT_FALSE / GT_TRUE], windowSize[0 / 100], csuAccessRate[0 / 7], mcAccessRate[0 / 127].
            Expected: GT_OK.
        */
        /* iterate with enable = GT_FALSE */
        enable        = GT_FALSE;
        windowSize    = 0;
        csuAccessRate = 0;
        mcAccessRate  = 0;

        st = cpssExMxPmDitRateShaperSet(dev, enable, windowSize, csuAccessRate, mcAccessRate);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, enable, windowSize, csuAccessRate, mcAccessRate);

        /*
            1.2. Call cpssExMxPmDitRateShaperGet with not NULL pointers and other params from 1.1.
            Expected: GT_OK and the same params from 1.1.
        */
        st = cpssExMxPmDitRateShaperGet(dev, &enableGet, &windowSizeGet, &csuAccessRateGet, &mcAccessRateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmDitRateShaperGet: %d" ,dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d" ,dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(windowSize, windowSizeGet, "got another windowSize than was set: %d" ,dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(csuAccessRate, csuAccessRateGet, "got another csuAccessRate than was set: %d" ,dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mcAccessRate, mcAccessRateGet, "got another mcAccessRate than was set: %d" ,dev);

        /* iterate with enable = GT_TRUE */
        enable        = GT_TRUE;
        windowSize    = 100;
        csuAccessRate = 7;
        mcAccessRate  = 127;

        st = cpssExMxPmDitRateShaperSet(dev, enable, windowSize, csuAccessRate, mcAccessRate);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, enable, windowSize, csuAccessRate, mcAccessRate);

        /*
            1.2. Call cpssExMxPmDitRateShaperGet with not NULL pointers and other params from 1.1.
            Expected: GT_OK and the same params from 1.1.
        */
        st = cpssExMxPmDitRateShaperGet(dev, &enableGet, &windowSizeGet, &csuAccessRateGet, &mcAccessRateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmDitRateShaperGet: %d" ,dev);

        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "got another enable than was set: %d" ,dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(windowSize, windowSizeGet, "got another windowSize than was set: %d" ,dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(csuAccessRate, csuAccessRateGet, "got another csuAccessRate than was set: %d" ,dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(mcAccessRate, mcAccessRateGet, "got another mcAccessRate than was set: %d" ,dev);

        /*
            1.3. Call with csuAccessRate[8] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        csuAccessRate = 8;

        st = cpssExMxPmDitRateShaperSet(dev, enable, windowSize, csuAccessRate, mcAccessRate);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, csuAccessRate = %d", dev, csuAccessRate);

        csuAccessRate = 7;

        /*
            1.4. Call with mcAccessRate[128] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        mcAccessRate = 128;

        st = cpssExMxPmDitRateShaperSet(dev, enable, windowSize, csuAccessRate, mcAccessRate);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, mcAccessRate = %d", dev, mcAccessRate);

        mcAccessRate = 0;

        /*
            1.5. Call with windowSize[65536] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        windowSize = 65536;

        st = cpssExMxPmDitRateShaperSet(dev, enable, windowSize, csuAccessRate, mcAccessRate);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, windowSize = %d", dev, windowSize);
    }

    enable        = GT_FALSE;
    windowSize    = 0;
    csuAccessRate = 0;
    mcAccessRate  = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitRateShaperSet(dev, enable, windowSize, csuAccessRate, mcAccessRate);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitRateShaperSet(dev, enable, windowSize, csuAccessRate, mcAccessRate);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitRateShaperGet
(
    IN   GT_U8      devNum,
    OUT  GT_BOOL   *enablePtr,
    OUT  GT_U32    *windowSizePtr,
    OUT  GT_U32    *csuAccessRatePtr,
    OUT  GT_U32    *mcAccessRatePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitRateShaperGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL enablePtr, windowSizePtr, csuAccessRatePtr, mcAccessRatePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with windowSizePtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with csuAccessRatePtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with mcAccessRatePtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL    enable = GT_FALSE;
    GT_U32     windowSize = 0;
    GT_U32     csuAccessRate = 0;
    GT_U32     mcAccessRate = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL enablePtr, windowSizePtr, csuAccessRatePtr, mcAccessRatePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmDitRateShaperGet(dev, &enable, &windowSize, &csuAccessRate, &mcAccessRate);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitRateShaperGet(dev, NULL, &windowSize, &csuAccessRate, &mcAccessRate);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);

        /*
            1.3. Call with windowSizePtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitRateShaperGet(dev, &enable, NULL, &csuAccessRate, &mcAccessRate);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, windowSizePtr = NULL", dev);

        /*
            1.4. Call with csuAccessRatePtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitRateShaperGet(dev, &enable, &windowSize, NULL, &mcAccessRate);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, csuAccessRatePtr = NULL", dev);

        /*
            1.5. Call with mcAccessRatePtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitRateShaperGet(dev, &enable, &windowSize, &csuAccessRate, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mcAccessRatePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitRateShaperGet(dev, &enable, &windowSize, &csuAccessRate, &mcAccessRate);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitRateShaperGet(dev, &enable, &windowSize, &csuAccessRate, &mcAccessRate);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitMultiTargetUcSchedModeSet
(
    IN   GT_U8                                  devNum,
    IN   GT_BOOL                                ucTableAccessStrictEnable,
    IN   CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_ENT    schedulerMtu,
    IN   GT_BOOL                                ucStrictPriorityEnable,
    IN   GT_U32                                 ucWeight,
    IN   GT_U32                                 mcWeight
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitMultiTargetUcSchedModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with ucTableAccessStrictEnable[GT_FALSE / GT_TRUE],
                   schedulerMtu[CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_2K_E / CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_8K_E],
                   ucStrictPriorityEnable[GT_FALSE / GT_TRUE],
                   ucWeight[0 / 255],
                   mcWeight[0 / 255]
    Expected: GT_OK.
    1.2. Call cpssExMxPmDitMultiTargetUcSchedModeGet with not null pointers
    Expected: GT_OK and the same params as was set.
    1.3. Call with ucTableAccessStrictEnable[0x5AAAAAA5] and othe rparams from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with ucWeight [256] and othe rparams from 1.1.
    Expected: NOT GT_OK.
    1.5. Call with mcWeight [256] and othe rparams from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with schedulerMtu[0x5AAAAAA5] and othe rparams from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL                                ucTableAccessStrictEnable = GT_FALSE;
    CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_ENT    schedulerMtu = CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_2K_E;
    GT_BOOL                                ucStrictPriorityEnable = GT_FALSE;
    GT_U32                                 ucWeight = 0;
    GT_U32                                 mcWeight = 0;

    GT_BOOL                                ucTableAccessStrictEnableGet = GT_FALSE;
    CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_ENT    schedulerMtuGet = CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_2K_E;
    GT_BOOL                                ucStrictPriorityEnableGet = GT_FALSE;
    GT_U32                                 ucWeightGet = 0;
    GT_U32                                 mcWeightGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ucTableAccessStrictEnable[GT_FALSE / GT_TRUE],
                           schedulerMtu[CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_2K_E / CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_8K_E],
                           ucStrictPriorityEnable[GT_FALSE / GT_TRUE],
                           ucWeight[0 / 255],
                           mcWeight[0 / 255]
            Expected: GT_OK.
        */
        /* iterate with ucTableAccessStrictEnable = GT_FALSE */
        ucTableAccessStrictEnable = GT_FALSE;
        schedulerMtu = CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_2K_E;
        ucStrictPriorityEnable = GT_FALSE;
        ucWeight = 0;
        mcWeight = 0;

        st = cpssExMxPmDitMultiTargetUcSchedModeSet(dev, ucTableAccessStrictEnable,
                                                    schedulerMtu, ucStrictPriorityEnable,
                                                    ucWeight, mcWeight);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, ucTableAccessStrictEnable,
                                    schedulerMtu, ucStrictPriorityEnable,
                                    ucWeight, mcWeight);

        /*
            1.2. Call cpssExMxPmDitMultiTargetUcSchedModeGet with not null pointers
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmDitMultiTargetUcSchedModeGet(dev, &ucTableAccessStrictEnableGet,
                                                    &schedulerMtuGet, &ucStrictPriorityEnableGet,
                                                    &ucWeightGet, &mcWeightGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmDitMultiTargetUcSchedModeGet: %d", dev);

        /* iterate with ucTableAccessStrictEnable = GT_TRUE */
        ucTableAccessStrictEnable = GT_TRUE;
        schedulerMtu = CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_8K_E;
        ucStrictPriorityEnable = GT_TRUE;
        ucWeight = 255;
        mcWeight = 255;

        st = cpssExMxPmDitMultiTargetUcSchedModeSet(dev, ucTableAccessStrictEnable,
                                                    schedulerMtu, ucStrictPriorityEnable,
                                                    ucWeight, mcWeight);
        UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, ucTableAccessStrictEnable,
                                    schedulerMtu, ucStrictPriorityEnable,
                                    ucWeight, mcWeight);

        /*
            1.2. Call cpssExMxPmDitMultiTargetUcSchedModeGet with not null pointers
            Expected: GT_OK and the same params as was set.
        */
        st = cpssExMxPmDitMultiTargetUcSchedModeGet(dev, &ucTableAccessStrictEnableGet,
                                                    &schedulerMtuGet, &ucStrictPriorityEnableGet,
                                                    &ucWeightGet, &mcWeightGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmDitMultiTargetUcSchedModeGet: %d", dev);

        /*
            1.3. Call with ucTableAccessStrictEnable[0x5AAAAAA5] and othe rparams from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ucTableAccessStrictEnable = DIT_INVALID_ENUM_CNS;

        st = cpssExMxPmDitMultiTargetUcSchedModeSet(dev, ucTableAccessStrictEnable,
                                                    schedulerMtu, ucStrictPriorityEnable,
                                                    ucWeight, mcWeight);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ucTableAccessStrictEnable = %d", dev, ucTableAccessStrictEnable);

        ucTableAccessStrictEnable = GT_TRUE;

        /*
            1.4. Call with ucWeight [256] and othe rparams from 1.1.
            Expected: NOT GT_OK.
        */
        ucWeight = 256;

        st = cpssExMxPmDitMultiTargetUcSchedModeSet(dev, ucTableAccessStrictEnable,
                                                    schedulerMtu, ucStrictPriorityEnable,
                                                    ucWeight, mcWeight);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ucWeight = %d", dev, ucWeight);

        ucWeight = 255;

        /*
            1.5. Call with mcWeight [256] and othe rparams from 1.1.
            Expected: NOT GT_OK.
        */
        mcWeight = 256;

        st = cpssExMxPmDitMultiTargetUcSchedModeSet(dev, ucTableAccessStrictEnable,
                                                    schedulerMtu, ucStrictPriorityEnable,
                                                    ucWeight, mcWeight);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, mcWeight = %d", dev, mcWeight);

        mcWeight = 0;

        /*
            1.6. Call with schedulerMtu[0x5AAAAAA5] and othe rparams from 1.1.
            Expected: GT_BAD_PARAM.
        */
        schedulerMtu = DIT_INVALID_ENUM_CNS;

        st = cpssExMxPmDitMultiTargetUcSchedModeSet(dev, ucTableAccessStrictEnable,
                                                    schedulerMtu, ucStrictPriorityEnable,
                                                    ucWeight, mcWeight);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, schedulerMtu = %d", dev, schedulerMtu);

        schedulerMtu = CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_2K_E;

    }

    ucTableAccessStrictEnable = GT_FALSE;
    schedulerMtu = CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_2K_E;
    ucStrictPriorityEnable = GT_FALSE;
    ucWeight = 0;
    mcWeight = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitMultiTargetUcSchedModeSet(dev, ucTableAccessStrictEnable,
                                                    schedulerMtu, ucStrictPriorityEnable,
                                                    ucWeight, mcWeight);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitMultiTargetUcSchedModeSet(dev, ucTableAccessStrictEnable,
                                                    schedulerMtu, ucStrictPriorityEnable,
                                                    ucWeight, mcWeight);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitMultiTargetUcSchedModeGet
(
    IN   GT_U8                                  devNum,
    OUT  GT_BOOL                                *ucTableAccessStrictEnablePtr,
    OUT  CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_ENT    *schedulerMtuPtr,
    OUT  GT_BOOL                                *ucStrictPriorityEnablePtr,
    OUT  GT_U32                                 *ucWeightPtr,
    OUT  GT_U32                                 *mcWeightPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitMultiTargetUcSchedModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL pointers
    Expected: GT_OK.
    1.2. Call with ucTableAccessStrictEnablePtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call with schedulerMtuPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with ucStrictPriorityEnablePtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call with ucWeightPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call with mcWeightPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_BOOL                                ucTableAccessStrictEnable = GT_FALSE;
    CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_ENT    schedulerMtu = CPSS_EXMXPM_DIT_MT_UC_SCHED_MTU_2K_E;
    GT_BOOL                                ucStrictPriorityEnable = GT_FALSE;
    GT_U32                                 ucWeight = 0;
    GT_U32                                 mcWeight = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL pointers
            Expected: GT_OK.
        */
        st = cpssExMxPmDitMultiTargetUcSchedModeGet(dev, &ucTableAccessStrictEnable,
                                                    &schedulerMtu, &ucStrictPriorityEnable,
                                                    &ucWeight, &mcWeight);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with ucTableAccessStrictEnablePtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitMultiTargetUcSchedModeGet(dev, NULL,
                                                    &schedulerMtu, &ucStrictPriorityEnable,
                                                    &ucWeight, &mcWeight);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ucTableAccessStrictEnablePtr = NULL", dev);

        /*

            1.3. Call with schedulerMtuPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitMultiTargetUcSchedModeGet(dev, &ucTableAccessStrictEnable,
                                                    NULL, &ucStrictPriorityEnable,
                                                    &ucWeight, &mcWeight);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, schedulerMtuPtr = NULL", dev);

        /*

            1.4. Call with ucStrictPriorityEnablePtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitMultiTargetUcSchedModeGet(dev, &ucTableAccessStrictEnable,
                                                    &schedulerMtu, NULL,
                                                    &ucWeight, &mcWeight);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ucStrictPriorityEnablePtr = NULL", dev);

        /*

            1.5. Call with ucWeightPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitMultiTargetUcSchedModeGet(dev, &ucTableAccessStrictEnable,
                                                    &schedulerMtu, &ucStrictPriorityEnable,
                                                    NULL, &mcWeight);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ucWeightPtr = NULL", dev);

        /*

            1.6. Call with mcWeightPtr [NULL] and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitMultiTargetUcSchedModeGet(dev, &ucTableAccessStrictEnable,
                                                    &schedulerMtu, &ucStrictPriorityEnable,
                                                    &ucWeight, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mcWeightPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitMultiTargetUcSchedModeGet(dev, &ucTableAccessStrictEnable,
                                            &schedulerMtu, &ucStrictPriorityEnable,
                                            &ucWeight, &mcWeight);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitMultiTargetUcSchedModeGet(dev, &ucTableAccessStrictEnable,
                                            &schedulerMtu, &ucStrictPriorityEnable,
                                            &ucWeight, &mcWeight);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitVpwsTargetTaggedModeSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_MODE_ENT vpwsTargetTaggedMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitVpwsTargetTaggedModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with vpwsTargetTaggedMode [CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_VLAN_TABLE_MODE_E /
                                         CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_MODE_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmDitVpwsTargetTaggedModeGet with not NULL vpwsTargetTaggedModePtr.
    Expected: GT_OK and the same values as written.
    1.3. Call with out of range vpwsTargetTaggedMode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_MODE_ENT vpwsTargetTaggedMode    = CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_VLAN_TABLE_MODE_E;
    CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_MODE_ENT vpwsTargetTaggedModeGet = CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_VLAN_TABLE_MODE_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vpwsTargetTaggedMode [CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_VLAN_TABLE_MODE_E /
                                                 CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_MODE_E].
            Expected: GT_OK.
        */

        /* call with vpwsTargetTaggedMode = CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_VLAN_TABLE_MODE_E */
        vpwsTargetTaggedMode = CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_VLAN_TABLE_MODE_E;

        st = cpssExMxPmDitVpwsTargetTaggedModeSet(dev, vpwsTargetTaggedMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vpwsTargetTaggedMode);

        /*
            1.2. Call cpssExMxPmDitVpwsTargetTaggedModeGet with not NULL vpwsTargetTaggedModePtr.
            Expected: GT_OK and the same values as written.
        */
        st = cpssExMxPmDitVpwsTargetTaggedModeGet(dev, &vpwsTargetTaggedModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(vpwsTargetTaggedMode, vpwsTargetTaggedModeGet,
                   "got another vpwsTargetTaggedMode than was set: %d, %d", dev);

        /* call with vpwsTargetTaggedMode = CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_MODE_E */
        vpwsTargetTaggedMode = CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_MODE_E;

        st = cpssExMxPmDitVpwsTargetTaggedModeSet(dev, vpwsTargetTaggedMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vpwsTargetTaggedMode);

        /*
            1.2. Call cpssExMxPmDitVpwsTargetTaggedModeGet with not NULL vpwsTargetTaggedModePtr.
            Expected: GT_OK and the same values as written.
        */
        st = cpssExMxPmDitVpwsTargetTaggedModeGet(dev, &vpwsTargetTaggedModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(vpwsTargetTaggedMode, vpwsTargetTaggedModeGet,
                   "got another vpwsTargetTaggedMode than was set: %d, %d", dev);

        /*
            1.3. Call with out of range vpwsTargetTaggedMode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        vpwsTargetTaggedMode = DIT_INVALID_ENUM_CNS;

        st = cpssExMxPmDitVpwsTargetTaggedModeSet(dev, vpwsTargetTaggedMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vpwsTargetTaggedMode);
    }

    vpwsTargetTaggedMode = CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_VLAN_TABLE_MODE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitVpwsTargetTaggedModeSet(dev, vpwsTargetTaggedMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitVpwsTargetTaggedModeSet(dev, vpwsTargetTaggedMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitVpwsTargetTaggedModeGet
(
    IN  GT_U8                                       devNum,
    OUT CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_MODE_ENT *vpwsTargetTaggedModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitVpwsTargetTaggedModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL vpwsTargetTaggedModePtr.
    Expected: GT_OK.
    1.2. Call with vpwsTargetTaggedModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_MODE_ENT vpwsTargetTaggedMode = CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_VLAN_TABLE_MODE_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL vpwsTargetTaggedModePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmDitVpwsTargetTaggedModeGet(dev, &vpwsTargetTaggedMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with vpwsTargetTaggedModePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitVpwsTargetTaggedModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vpwsTargetTaggedModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitVpwsTargetTaggedModeGet(dev, &vpwsTargetTaggedMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitVpwsTargetTaggedModeGet(dev, &vpwsTargetTaggedMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitVpwsTargetTaggedControlSet
(
    IN  GT_U8                                           devNum,
    IN  CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_ENT  vpwsTargetTaggedControl
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitVpwsTargetTaggedControlSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with vpwsTargetTaggedControl [CPSS_EXMXPM_DIT_VPWS_TARGET_UNTAGGED_CONTROL_E /
                                            CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_E /
                                            CPSS_EXMXPM_DIT_VPWS_TARGET_COPY_SRC_TAG_CONTROL_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmDitVpwsTargetTaggedControlGet with not NULL vpwsTargetTaggedControlPtr.
    Expected: GT_OK and the same values as written.
    1.3. Call with out of range vpwsTargetTaggedControl [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_ENT  vpwsTargetTaggedControl    = CPSS_EXMXPM_DIT_VPWS_TARGET_UNTAGGED_CONTROL_E;
    CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_ENT  vpwsTargetTaggedControlGet = CPSS_EXMXPM_DIT_VPWS_TARGET_UNTAGGED_CONTROL_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vpwsTargetTaggedControl [CPSS_EXMXPM_DIT_VPWS_TARGET_UNTAGGED_CONTROL_E /
                                                    CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_E /
                                                    CPSS_EXMXPM_DIT_VPWS_TARGET_COPY_SRC_TAG_CONTROL_E].
            Expected: GT_OK.
        */

        /* call with vpwsTargetTaggedControl = CPSS_EXMXPM_DIT_VPWS_TARGET_UNTAGGED_CONTROL_E */
        vpwsTargetTaggedControl = CPSS_EXMXPM_DIT_VPWS_TARGET_UNTAGGED_CONTROL_E;

        st = cpssExMxPmDitVpwsTargetTaggedControlSet(dev, vpwsTargetTaggedControl);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vpwsTargetTaggedControl);

        /*
            1.2. Call cpssExMxPmDitVpwsTargetTaggedControlGet with not NULL vpwsTargetTaggedControlPtr.
            Expected: GT_OK and the same values as written.
        */
        st = cpssExMxPmDitVpwsTargetTaggedControlGet(dev, &vpwsTargetTaggedControlGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmDitVpwsTargetTaggedControlGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(vpwsTargetTaggedControl, vpwsTargetTaggedControlGet,
                   "got another vpwsTargetTaggedControl than was set: %d, %d", dev);

        /* call with vpwsTargetTaggedControl = CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_E */
        vpwsTargetTaggedControl = CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_E;

        st = cpssExMxPmDitVpwsTargetTaggedControlSet(dev, vpwsTargetTaggedControl);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vpwsTargetTaggedControl);

        /*
            1.2. Call cpssExMxPmDitVpwsTargetTaggedControlGet with not NULL vpwsTargetTaggedControlPtr.
            Expected: GT_OK and the same values as written.
        */
        st = cpssExMxPmDitVpwsTargetTaggedControlGet(dev, &vpwsTargetTaggedControlGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmDitVpwsTargetTaggedControlGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(vpwsTargetTaggedControl, vpwsTargetTaggedControlGet,
                   "got another vpwsTargetTaggedControl than was set: %d, %d", dev);

        /* call with vpwsTargetTaggedControl = CPSS_EXMXPM_DIT_VPWS_TARGET_COPY_SRC_TAG_CONTROL_E */
        vpwsTargetTaggedControl = CPSS_EXMXPM_DIT_VPWS_TARGET_COPY_SRC_TAG_CONTROL_E;

        st = cpssExMxPmDitVpwsTargetTaggedControlSet(dev, vpwsTargetTaggedControl);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vpwsTargetTaggedControl);

        /*
            1.2. Call cpssExMxPmDitVpwsTargetTaggedControlGet with not NULL vpwsTargetTaggedControlPtr.
            Expected: GT_OK and the same values as written.
        */
        st = cpssExMxPmDitVpwsTargetTaggedControlGet(dev, &vpwsTargetTaggedControlGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmDitVpwsTargetTaggedControlGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(vpwsTargetTaggedControl, vpwsTargetTaggedControlGet,
                   "got another vpwsTargetTaggedControl than was set: %d, %d", dev);

        /*
            1.3. Call with out of range vpwsTargetTaggedControl [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        vpwsTargetTaggedControl = DIT_INVALID_ENUM_CNS;

        st = cpssExMxPmDitVpwsTargetTaggedControlSet(dev, vpwsTargetTaggedControl);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vpwsTargetTaggedControl);
    }

    vpwsTargetTaggedControl = CPSS_EXMXPM_DIT_VPWS_TARGET_UNTAGGED_CONTROL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitVpwsTargetTaggedControlSet(dev, vpwsTargetTaggedControl);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitVpwsTargetTaggedControlSet(dev, vpwsTargetTaggedControl);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitVpwsTargetTaggedControlGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_ENT  *vpwsTargetTaggedControlPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitVpwsTargetTaggedControlGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with not NULL vpwsTargetTaggedControlPtr.
    Expected: GT_OK.
    1.2. Call with vpwsTargetTaggedControlPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_DIT_VPWS_TARGET_TAGGED_CONTROL_ENT  vpwsTargetTaggedControl = CPSS_EXMXPM_DIT_VPWS_TARGET_UNTAGGED_CONTROL_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not NULL vpwsTargetTaggedControlPtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmDitVpwsTargetTaggedControlGet(dev, &vpwsTargetTaggedControl);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with vpwsTargetTaggedControlPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitVpwsTargetTaggedControlGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vpwsTargetTaggedControlPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitVpwsTargetTaggedControlGet(dev, &vpwsTargetTaggedControl);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitVpwsTargetTaggedControlGet(dev, &vpwsTargetTaggedControl);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitVplsUnknownUnicastFloodEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitVplsUnknownUnicastFloodEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmDitVplsUnknownUnicastFloodEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssExMxPmDitVplsUnknownUnicastFloodEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmDitVplsUnknownUnicastFloodEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmDitVplsUnknownUnicastFloodEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmDitVplsUnknownUnicastFloodEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxPmDitVplsUnknownUnicastFloodEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmDitVplsUnknownUnicastFloodEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmDitVplsUnknownUnicastFloodEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmDitVplsUnknownUnicastFloodEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitVplsUnknownUnicastFloodEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitVplsUnknownUnicastFloodEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmDitVplsUnknownUnicastFloodEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitVplsUnknownUnicastFloodEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmDitVplsUnknownUnicastFloodEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmDitVplsUnknownUnicastFloodEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmDitVplsUnknownUnicastFloodEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmDitVplsUnknownUnicastFloodEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill DIT Ip Mc pair table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitFillIpMcPairTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in DIT Ip Mc pair table.
         Call cpssExMxPmDitIpMcPairWrite with ditPairEntryIndex [0..numEntries - 1],
                                       ditPairWriteForm [CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E]
                                       and ditPairEntryPtr->firstDitNode {ditRpfFailCommand [CPSS_PACKET_CMD_TRAP_TO_CPU_E],
                                                                          isTunnelStart [GT_FALSE],
                                                                          nextHopIf {type [CPSS_INTERFACE_VID_E],
                                                                                     vlanId [100]},
                                                                          nextHopVlanId [100],
                                                                          ttlHopLimitThreshold [255],
                                                                          excludeSourceVlanIf [GT_TRUE]}.
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmDitIpMcPairWrite with ditPairEntryIndex [numEntries], and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in DIT Ip Mc pair table and compare with original.
         Call cpssExMxPmDitIpMcPairRead with the same ditPairEntryIndex as in 1.2,
                                           and non-NULL ditPairEntryPtr.
    Expected: GT_OK and the same ditPairEntryPtr.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmDitIpMcPairRead with ditPairEntryIndex [numEntries],
                                           and non-NULL ditPairEntryPtr.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_MODE_ENT  ditPairWriteForm  = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_STC                  ditPairEntry;
    CPSS_EXMXPM_DIT_IP_MC_PAIR_STC                  ditPairEntryGet;


    cpssOsBzero((GT_VOID*) &ditPairEntry, sizeof(ditPairEntry));
    cpssOsBzero((GT_VOID*) &ditPairEntryGet, sizeof(ditPairEntryGet));

    /* Fill the entry for DIT Ip Mc pair table */
    ditPairWriteForm = CPSS_EXMXPM_DIT_IP_MC_PAIR_READ_WRITE_FIRST_DIT_ONLY_E;

    ditPairEntry.firstDitNode.ditRpfFailCommand    = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    ditPairEntry.firstDitNode.isTunnelStart        = GT_FALSE;
    ditPairEntry.firstDitNode.nextHopTunnelPointer = 0x3FFF;
    ditPairEntry.firstDitNode.nextHopIf.type       = CPSS_INTERFACE_VIDX_E;
    ditPairEntry.firstDitNode.nextHopIf.vidx       = DIT_TESTED_VLAN_ID_CNS;
    ditPairEntry.firstDitNode.nextHopVlanId        = DIT_TESTED_VLAN_ID_CNS;
    ditPairEntry.firstDitNode.ttlHopLimitThreshold = 255;
    ditPairEntry.firstDitNode.excludeSourceVlanIf  = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 275;

        /* 1.2. Fill all entries in DIT Ip Mc pair table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            ditPairEntry.firstDitNode.nextHopTunnelPointer = iTemp % 0x4000;
            ditPairEntry.firstDitNode.nextHopIf.vidx       = (GT_U16)(iTemp % 4095);

            st = cpssExMxPmDitIpMcPairWrite(dev, iTemp, ditPairWriteForm, &ditPairEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmDitIpMcPairWrite: %d, %d, %d", dev, iTemp, ditPairWriteForm);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmDitIpMcPairWrite(dev, numEntries, ditPairWriteForm, &ditPairEntry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmDitIpMcPairWrite: %d, %d, %d", dev, iTemp, ditPairWriteForm);

        /* 1.4. Read all entries in DIT Ip Mc pair table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            ditPairEntry.firstDitNode.nextHopTunnelPointer = iTemp % 0x4000;
            ditPairEntry.firstDitNode.nextHopIf.vidx       = (GT_U16)(iTemp % 4095);

            st = cpssExMxPmDitIpMcPairRead(dev, iTemp, ditPairWriteForm, &ditPairEntryGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                           "cpssExMxPmDitIpMcPairRead: %d, %d, %d", dev, iTemp, ditPairWriteForm);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.ditRpfFailCommand,
                                         ditPairEntryGet.firstDitNode.ditRpfFailCommand,
                       "get another ditPairEntryPtr->firstDitNode.ditRpfFailCommand than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.isTunnelStart,
                                         ditPairEntryGet.firstDitNode.isTunnelStart,
                       "get another ditPairEntryPtr->firstDitNode.isTunnelStart than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.nextHopIf.type,
                                         ditPairEntryGet.firstDitNode.nextHopIf.type,
                       "get another ditPairEntryPtr->firstDitNode.nextHopIf.type than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.nextHopIf.vidx,
                                         ditPairEntryGet.firstDitNode.nextHopIf.vidx,
                       "get another ditPairEntryPtr->firstDitNode.nextHopIf.vidx than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.nextHopVlanId,
                                         ditPairEntryGet.firstDitNode.nextHopVlanId,
                       "get another ditPairEntryPtr->firstDitNode.nextHopVlanId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.ttlHopLimitThreshold,
                                         ditPairEntryGet.firstDitNode.ttlHopLimitThreshold,
                       "get another ditPairEntryPtr->firstDitNode.ttlHopLimitThreshold than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ditPairEntry.firstDitNode.excludeSourceVlanIf,
                                         ditPairEntryGet.firstDitNode.excludeSourceVlanIf,
                       "get another ditPairEntryPtr->firstDitNode.excludeSourceVlanIf than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmDitIpMcPairRead(dev, numEntries, ditPairWriteForm, &ditPairEntryGet);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmDitIpMcPairRead: %d, %d, %d", dev, iTemp, ditPairWriteForm);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill DIT Ip/Mpls tAble.
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitFillIpMplsTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in DIT Ip/Mpls table.
         Call cpssExMxPmDitIpMplsEntryWrite with ditEntryIndex[0..numEntries - 1],
                            ditIpMpls[CPSS_EXMXPM_DIT_IP_SELECTION_E],
                            ditType[CPSS_UNICAST_E],
                            ditEntryPtr{
                                        ditRpfFailCommand [CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E],
                                        outLif { {outlifType{CPSS_EXMXPM_OUTLIF_TYPE_LL_E}
                                                 interfaceInfo{{type [CPSS_INTERFACE_PORT_E],
                                                                      devPort->devNum[dev],
                                                                      devPort->portNum[0]}}.}
                                                 outlifPointer{arpPtr[0]} },
                                        vlanId [100],
                                        ttlHopLimitThreshold [0],
                                        excludeSourceVlanIf [GT_FALSE],
                                        mplsLabel [0],
                                        exp [0]
                                        qosExpCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E],
                                        mplsCommand [0],
                                        ttlIndex [0],
                                        isVidSet [GT_FALSE],
                                        srcInterface{type[CPSS_INTERFACE_PORT_E],
                                                     devPort {devNum [devNum],
                                                              portNum [0]},
                                                     trunkId [0],
                                                     vidx [0],
                                                     vlanId [100]},
                                        srcInterfaceCompareMode [CPSS_EXMXPM_DIT_SOURCE_INTERFACE_COMPARE_MODE_VID_E],
                                        isLast[GT_FALSE] }
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmDitIpMplsEntryWrite with ditEntryIndex[numEntries], and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in DIT Ip/Mpls table and compare with original.
         Call cpssExMxPmDitIpMplsEntryRead with not NULL ditEntryPtr and other params from 1.2.
    Expected: GT_OK and the same params as was set.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmDitIpMplsEntryRead with not NULL ditEntryPtr and ditEntryIndex[numEntries],
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_EXMXPM_DIT_IP_MPLS_SELECTION_ENT   ditIpMpls     = CPSS_EXMXPM_DIT_IP_SELECTION_E;
    CPSS_UNICAST_MULTICAST_ENT              ditType       = CPSS_MULTICAST_E;
    CPSS_EXMXPM_DIT_IP_MPLS_STC             ditEntry;
    CPSS_EXMXPM_DIT_IP_MPLS_STC             ditEntryGet;


    cpssOsBzero((GT_VOID*) &ditEntry, sizeof(ditEntry));
    cpssOsBzero((GT_VOID*) &ditEntryGet, sizeof(ditEntryGet));

    /* Fill the entry for DIT Ip/Mpls table */
    ditIpMpls = CPSS_EXMXPM_DIT_IP_SELECTION_E;
    ditType   = CPSS_UNICAST_E;
    ditEntry.ditRpfFailCommand = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
    ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;
    ditEntry.vlanId = 100;
    ditEntry.ttlHopLimitThreshold = 0;
    ditEntry.excludeSourceVlanIf = GT_FALSE;
    ditEntry.mplsLabel = 0;
    ditEntry.exp = 1;
    ditEntry.qosExpCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E;
    ditEntry.mplsCommand = 0;
    ditEntry.ttlIndex = 2;
    ditEntry.isVidSet = GT_FALSE;
    ditEntry.srcInterface.type = CPSS_INTERFACE_PORT_E;
    ditEntry.srcInterfaceCompareMode = CPSS_EXMXPM_DIT_SOURCE_INTERFACE_COMPARE_MODE_VID_E;
    ditEntry.isLast = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if (PRV_CPSS_EXMXPM_DIT_MODULE_MAC(dev)->ipMcMode != CPSS_EXMXPM_PP_CONFIG_DIT_IP_MC_DUAL_MODE_E)
        {
            ditEntry.outLif.devPort.devNum = dev;
            ditEntry.outLif.devPort.portNum = 0;
            ditEntry.srcInterface.devPort.devNum = dev;
            ditEntry.srcInterface.devPort.portNum = 0;

            /* 1.1. Get table Size */
            if (ditType == CPSS_UNICAST_E)
            {
                numEntries = PRV_CPSS_EXMXPM_DIT_MODULE_MAC(dev)->ucMplsEntriesNum;
            }
            else
            {
                numEntries = PRV_CPSS_EXMXPM_DIT_MODULE_MAC(dev)->mcMplsEntriesNum;
            }

            /* 1.2. Fill all entries in DIT Ip/Mpls table */
            for(iTemp = 0; iTemp < numEntries; ++iTemp)
            {
                /* make every entry unique */
                ditEntry.vlanId     = (GT_U16)(iTemp % 4095);
                ditEntry.exp        = iTemp % 7;
                ditEntry.ttlIndex   = iTemp % 7;

                st = cpssExMxPmDitIpMplsEntryWrite(dev, iTemp, ditIpMpls, ditType, &ditEntry);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmDitIpMplsEntryWrite: %d, %d, %d", dev, iTemp, ditIpMpls);
            }

            /* 1.3. Try to write entry with index out of range. */
            st = cpssExMxPmDitIpMplsEntryWrite(dev, numEntries, ditIpMpls, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmDitIpMplsEntryWrite: %d, %d, %d", dev, iTemp, ditIpMpls);

            /* 1.4. Read all entries in DIT Ip/Mpls table and compare with original */
            for(iTemp = 0; iTemp < numEntries; ++iTemp)
            {
                /* restore unique entry before compare */
                ditEntry.vlanId     = (GT_U16)(iTemp % 4095);
                ditEntry.exp        = iTemp % 7;
                ditEntry.ttlIndex   = iTemp % 7;

                st = cpssExMxPmDitIpMplsEntryRead(dev, iTemp, ditIpMpls, ditType, &ditEntryGet);
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmDitIpMplsEntryRead: %d, %d, %d, %d",
                                            dev, iTemp, ditIpMpls, ditType);

                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &ditEntry, (GT_VOID*) &ditEntryGet, sizeof(ditEntry) )) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another ditEntry than was set: %d", dev);
            }

            /* 1.5. Try to read entry with index out of range. */
            st = cpssExMxPmDitIpMplsEntryRead(dev, numEntries, ditIpMpls, ditType, &ditEntryGet);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmDitIpMplsEntryRead: %d, %d, %d, %d",
                                             dev, iTemp, ditIpMpls, ditType);
        }
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Ttl Value  table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitFillTtlTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in Ttl Value table.
         Call cpssExMxPmDitTtlTableEntrySet with index [0..numEntries - 1], value [0]
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmDitTtlTableEntrySet with index [numEntries], and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in Ttl Value table and compare with original.
         Call cpssExMxPmDitTtlTableEntryGet with not NULL valuePtr
    Expected: GT_OK and the same value as was set.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmDitTtlTableEntryGet with index [numEntries].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;
    GT_U32      value      = 0;
    GT_U32      valueGet   = 0;


    /* Fill the entry for Ttl Value table */
    value = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 8;

        /* 1.2. Fill all entries in Ttl Value table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            value = iTemp % 255;

            st = cpssExMxPmDitTtlTableEntrySet(dev, iTemp, value);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmDitTtlTableEntrySet: %d, %d, %d", dev, iTemp, value);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmDitTtlTableEntrySet(dev, numEntries, value);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmDitTtlTableEntrySet: %d, %d, %d", dev, iTemp, value);

        /* 1.4. Read all entries in Ttl Value table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            value = iTemp % 255;

            st = cpssExMxPmDitTtlTableEntryGet(dev, iTemp, &valueGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmDitTtlTableEntryGet: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL1_STRING_MAC(value, valueGet, "got another value than was set: %d, %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmDitTtlTableEntryGet(dev, numEntries, &valueGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmDitTtlTableEntryGet: %d, %d", dev, iTemp);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill DitVpls table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmDitFillVplsTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in DitVpls table.
         Call cpssExMxPmDitVplsEntryWrite with ditEntryIndex[0..numEntries - 1],
                                                ditType[CPSS_UNICAST_E],
                                                ditEntryPtr{
                                                            outLif { {outlifType{CPSS_EXMXPM_OUTLIF_TYPE_LL_E}
                                                                     interfaceInfo{{type [CPSS_INTERFACE_PORT_E],
                                                                                          devPort->devNum[dev],
                                                                                          devPort->portNum[0]}}.}
                                                                     outlifPointer{arpPtr[0]} },
                                                            vlanId [100],
                                                            ttl [0],
                                                            vlanTagCommand [CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_TAGGED_FROM_DIT_E],
                                                            label [0],
                                                            exp [0],
                                                            qosExpCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E],
                                                            up [0],
                                                            mplsCommand [CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E],
                                                            vplsUnknownUcEnable [GT_FALSE],
                                                            vplsUnregisteredMcEnable [GT_FALSE],
                                                            pwId [0],
                                                            isLast[GT_FALSE] }
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmDitVplsEntryWrite with ditEntryIndex[numEntries], and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in DitVpls table and compare with original.
         Call cpssExMxPmDitVplsEntryRead with not NULL ditEntryPtr and other params from 1.2.
    Expected: GT_OK and the same ditEntry as was set.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmDitVplsEntryRead with ditEntryIndex[numEntries],
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_UNICAST_MULTICAST_ENT    ditType = CPSS_UNICAST_E;
    CPSS_EXMXPM_DIT_VPLS_STC      ditEntry;
    CPSS_EXMXPM_DIT_VPLS_STC      ditEntryGet;


    cpssOsBzero((GT_VOID*) &ditEntry, sizeof(ditEntry));
    cpssOsBzero((GT_VOID*) &ditEntryGet, sizeof(ditEntryGet));

    /* Fill the entry for DitVpls table */
    ditType       = CPSS_UNICAST_E;
    ditEntry.outLif.type = CPSS_INTERFACE_PORT_E;
    ditEntry.vlanId = 100;
    ditEntry.ttl = 0;
    ditEntry.vlanTagCommand = CPSS_EXMXPM_DIT_VLAN_TAG_COMMAND_TAGGED_FROM_DIT_E;
    ditEntry.label = 0;
    ditEntry.exp   = 0;
    ditEntry.qosExpCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E;
    ditEntry.up = 0;
    ditEntry.mplsCommand = CPSS_EXMXPM_DIT_QOS_COMMAND_FROM_DESCRIPTOR_E;
    ditEntry.vplsUnknownUcEnable = GT_FALSE;
    ditEntry.vplsUnregisteredMcEnable = GT_FALSE;
    ditEntry.pwId = 0;
    ditEntry.isLast = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if (PRV_CPSS_EXMXPM_DIT_MODULE_MAC(dev)->ipMcMode != CPSS_EXMXPM_PP_CONFIG_DIT_IP_MC_DUAL_MODE_E)
        {
            ditEntry.outLif.devPort.devNum  = dev;
            ditEntry.outLif.devPort.portNum = 0;

            /* 1.1. Get table Size */
            if( ditType == CPSS_UNICAST_E)
            {
                numEntries = PRV_CPSS_EXMXPM_DIT_MODULE_MAC(dev)->ucOtherEntriesNum;
            }
            else
            {
                numEntries = PRV_CPSS_EXMXPM_DIT_MODULE_MAC(dev)->mcOtherEntriesNum;
            }

            /* 1.2. Fill all entries in DitVpls table */
            for(iTemp = 0; iTemp < numEntries; ++iTemp)
            {
                /* make every entry unique */
                ditEntry.vlanId = (GT_U16)(iTemp % 4095);
                ditEntry.ttl    = iTemp % 255;
                ditEntry.exp    = iTemp % 7;
                ditEntry.up     = iTemp % 7;
                ditEntry.pwId   = iTemp % 127;

                st = cpssExMxPmDitVplsEntryWrite(dev, iTemp, ditType, &ditEntry);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmDitVplsEntryWrite: %d, %d, %d", dev, iTemp, ditType);
            }

            /* 1.3. Try to write entry with index out of range. */
            st = cpssExMxPmDitVplsEntryWrite(dev, numEntries, ditType, &ditEntry);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmDitVplsEntryWrite: %d, %d, %d", dev, iTemp, ditType);

            /* 1.4. Read all entries in DitVpls table and compare with original */
            for(iTemp = 0; iTemp < numEntries; ++iTemp)
            {
                /* restore unique entry before compare */
                ditEntry.vlanId = (GT_U16)(iTemp % 4095);
                ditEntry.ttl    = iTemp % 255;
                ditEntry.exp    = iTemp % 7;
                ditEntry.up     = iTemp % 7;
                ditEntry.pwId   = iTemp % 127;

                st = cpssExMxPmDitVplsEntryRead(dev, iTemp, ditType, &ditEntryGet);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmDitVplsEntryRead: %d, %d, %d",
                                            dev, iTemp, ditType);

                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &ditEntry, (GT_VOID*) &ditEntryGet, sizeof(ditEntry) )) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual, "got another ditEntry then was set: %d", dev);
            }

            /* 1.5. Try to read entry with index out of range. */
            st = cpssExMxPmDitVplsEntryRead(dev, numEntries, ditType, &ditEntryGet);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmDitVplsEntryRead: %d, %d, %d",
                                        dev, iTemp, ditType);
        }
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmDit suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmDit)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitIpMcPairWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitIpMcPairRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitIpMcPairLastSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitIpMplsEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitIpMplsEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitIpMplsEntryLastSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitTtlTableEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitTtlTableEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitVplsEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitVplsEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitVplsEntryLastSet)
    /* this configuration is not supported due to erratum */
    /* UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitCounterConfigSet) */
    /* UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitCounterConfigGet) */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitBasedBridgeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitBasedBridgeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitMultiTargetMcQueuePriorityConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitMultiTargetMcQueuePriorityConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitMcTc2IpTvQueueConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitMcTc2IpTvQueueConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitMcTc2IpTvQueueIpTvXonStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitMultiTargetTCQueueConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitMultiTargetTCQueueConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitMultiTargetTCQueueStrictPriorityEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitMultiTargetTCQueueWeightSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitMultiTargetTCQueueWeightGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitRateShaperSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitRateShaperGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitMultiTargetUcSchedModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitMultiTargetUcSchedModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitVpwsTargetTaggedModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitVpwsTargetTaggedModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitVpwsTargetTaggedControlSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitVpwsTargetTaggedControlGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitVplsUnknownUnicastFloodEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitVplsUnknownUnicastFloodEnableGet)
    /* Tests for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitFillIpMcPairTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitFillIpMplsTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitFillTtlTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmDitFillVplsTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmDit)
