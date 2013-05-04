/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmIpStatUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmIpStat, that provides
*       The CPSS EXMXPM Ip Statistics APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpStat.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define IP_STAT_INVALID_ENUM_CNS    0x5AAAAAA5

/* Tests use this vlan id for testing VLAN functions */
#define IP_STAT_TESTED_VLAN_ID_CNS  100


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpCounterConfigSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  counterIndex,
    IN  CPSS_EXMXPM_IP_COUNTER_BIND_MODE_ENT    bindMode,
    IN  CPSS_EXMXPM_IP_COUNTER_SET_CFG_STC      *interfaceConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpCounterConfigSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with counterIndex [0],
                   bindMode [CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E]
                   and interfaceConfigPtr {portTrunkMode [CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E],
                                           packetType [CPSS_EXMXPM_IP_PACKET_TYPE_ALL_COUNTER_MODE_E],
                                           vlanMode [CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E]}.
    Expected: GT_OK.
    1.2. Call with counterIndex [1],
                   bindMode [CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E]
                   and interfaceConfigPtr {portTrunkMode [CPSS_EXMXPM_IP_PORT_COUNTER_MODE_E],
                                           portTrunk.devMode.devNum [dev],
                                           portTrunk.devMode.port [0],
                                           packetType [CPSS_EXMXPM_IP_PACKET_TYPE_IPV4_COUNTER_MODE_E],
                                           vlanMode [CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E],
                                           vlanId [100]}.
    Expected: GT_OK.
    1.3. Call with counterIndex [2],
                   bindMode [CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E]
                   and interfaceConfigPtr {portTrunkMode [CPSS_EXMXPM_IP_TRUNK_COUNTER_MODE_E],
                                           devNum [dev],
                                           portTrunk.trunk [2],
                                           packetType [CPSS_EXMXPM_IP_PACKET_TYPE_IPV6_COUNTER_MODE_E],
                                           vlanMode [CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E],
                                           vlanId [4095]}.
    Expected: GT_OK.
    1.4. Call with counterIndex [3],
                   bindMode [CPSS_EXMXPM_IP_COUNTER_BIND_TO_ROUTE_ENTRY_E]
                   and non-NULL interfaceConfigPtr.
    Expected: GT_OK.
    1.5. Call cpssExMxPmIpCounterConfigGet with the same counterIndex as in 1.1 - 1.4.
                                                and non-NULL bindModePtr and non-NULL interfaceConfigPtr.
    Expected: GT_OK and the same bindMode and interfaceConfigPtr.
    1.6. Call with out of range counterIndex [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range bindMode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with out of range interfaceConfigPtr->portTrunkMode [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.9. Call with out of range interfaceConfigPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.10. Call with out of range interfaceConfigPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                    and other parameters from 1.2.
    Expected: NOT GT_OK.
    1.11. Call with out of range interfaceConfigPtr->portTrunk.port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                    and other parameters from 1.2.
    Expected: NOT GT_OK.
    1.12. Call with out of range interfaceConfigPtr->portTrunk.trunk [256] (not relevant)
                    and other parameters from 1.2.
    Expected: GT_OK.
    1.13. Call with out of range interfaceConfigPtr->portTrunk.trunk [256]
                    and other parameters from 1.3.
    Expected: NOT GT_OK.
    1.14. Call with out of range interfaceConfigPtr->packetType [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.15. Call with interfaceConfigPtr->packetType [CPSS_EXMXPM_IP_PACKET_TYPE_INVALID_COUNTER_MODE_E] (invalid)
                    and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.16. Call with out of range interfaceConfigPtr->vlanMode [0x5AAAAAA5]
                    and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.17. Call with interfaceConfigPtr->vlanMode [CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E]
                    and out of range interfaceConfigPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                    and other parameters from 1.1.
    Expected: GT_OUT_OF_RANGE.
    1.18. Call with interfaceConfigPtr->vlanMode [CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E]
                    and out of range interfaceConfigPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.19. Call with interfaceConfigPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                  counterIndex = 0;
    CPSS_EXMXPM_IP_COUNTER_BIND_MODE_ENT    bindMode     = CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E;
    CPSS_EXMXPM_IP_COUNTER_SET_CFG_STC      config;
    CPSS_EXMXPM_IP_COUNTER_BIND_MODE_ENT    bindModeGet  = CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E;
    CPSS_EXMXPM_IP_COUNTER_SET_CFG_STC      configGet;


    cpssOsBzero((GT_VOID*) &config, sizeof(config));
    cpssOsBzero((GT_VOID*) &configGet, sizeof(configGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with counterIndex [0],
                           bindMode [CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E]
                           and interfaceConfigPtr {portTrunkMode [CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E],
                                                   packetType [CPSS_EXMXPM_IP_PACKET_TYPE_ALL_COUNTER_MODE_E],
                                                   vlanMode [CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E]}.
            Expected: GT_OK.
        */
        counterIndex = 0;
        bindMode     = CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E;

        config.portTrunkMode = CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E;
        config.packetType    = CPSS_EXMXPM_IP_PACKET_TYPE_ALL_COUNTER_MODE_E;
        config.vlanMode      = CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, counterIndex, bindMode);

        /*
            1.5. Call cpssExMxPmIpCounterConfigGet with the same counterIndex as in 1.1 - 1.4.
                                                        and non-NULL bindModePtr and non-NULL interfaceConfigPtr.
            Expected: GT_OK and the same bindMode and interfaceConfigPtr.
        */
        st = cpssExMxPmIpCounterConfigGet(dev, counterIndex, &bindModeGet, &configGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpCounterConfigGet: %d, %d", dev, counterIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(bindMode, bindModeGet,
                   "get another bindMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.portTrunkMode, configGet.portTrunkMode,
                   "get another interfaceConfigPtr->portTrunkMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.packetType, configGet.packetType,
                   "get another interfaceConfigPtr->packetType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.vlanMode, configGet.vlanMode,
                   "get another interfaceConfigPtr->vlanMode than was set: %d", dev);

        /*
            1.2. Call with counterIndex [1],
                           bindMode [CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E]
                           and interfaceConfigPtr {portTrunkMode [CPSS_EXMXPM_IP_PORT_COUNTER_MODE_E],
                                                   devNum [dev],
                                                   portTrunk.port [0],
                                                   packetType [CPSS_EXMXPM_IP_PACKET_TYPE_IPV4_COUNTER_MODE_E],
                                                   vlanMode [CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E],
                                                   vlanId [100]}.
            Expected: GT_OK.
        */
        counterIndex = 1;
        bindMode     = CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E;

        config.portTrunkMode  = CPSS_EXMXPM_IP_PORT_COUNTER_MODE_E;
        config.portTrunk.devPort.devNum = dev;
        config.portTrunk.devPort.port = 0;
        config.packetType     = CPSS_EXMXPM_IP_PACKET_TYPE_IPV4_COUNTER_MODE_E;
        config.vlanMode       = CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E;
        config.vlanId         = IP_STAT_TESTED_VLAN_ID_CNS;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, counterIndex, bindMode);

        /*
            1.5. Call cpssExMxPmIpCounterConfigGet with the same counterIndex as in 1.1 - 1.4.
                                                        and non-NULL bindModePtr and non-NULL interfaceConfigPtr.
            Expected: GT_OK and the same bindMode and interfaceConfigPtr.
        */
        st = cpssExMxPmIpCounterConfigGet(dev, counterIndex, &bindModeGet, &configGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpCounterConfigGet: %d, %d", dev, counterIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(bindMode, bindModeGet,
                   "get another bindMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.portTrunkMode, configGet.portTrunkMode,
                   "get another interfaceConfigPtr->portTrunkMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.portTrunk.devPort.devNum, configGet.portTrunk.devPort.devNum,
                   "get another interfaceConfigPtr->devNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.portTrunk.devPort.port, configGet.portTrunk.devPort.port,
                   "get another interfaceConfigPtr->portTrunk.port than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.packetType, configGet.packetType,
                   "get another interfaceConfigPtr->packetType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.vlanMode, configGet.vlanMode,
                   "get another interfaceConfigPtr->vlanMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.vlanId, configGet.vlanId,
                   "get another interfaceConfigPtr->vlanId than was set: %d", dev);

        /*
            1.3. Call with counterIndex [2],
                           bindMode [CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E]
                           and interfaceConfigPtr {portTrunkMode [CPSS_EXMXPM_IP_TRUNK_COUNTER_MODE_E],
                                                   devNum [dev],
                                                   portTrunk.trunk [2],
                                                   packetType [CPSS_EXMXPM_IP_PACKET_TYPE_IPV6_COUNTER_MODE_E],
                                                   vlanMode [CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E],
                                                   vlanId [4095]}.
            Expected: GT_OK.
        */
        counterIndex = 2;
        bindMode     = CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E;

        config.portTrunkMode   = CPSS_EXMXPM_IP_TRUNK_COUNTER_MODE_E;
        config.portTrunk.devPort.devNum          = dev;
        config.portTrunk.trunk = 2;
        config.packetType      = CPSS_EXMXPM_IP_PACKET_TYPE_IPV6_COUNTER_MODE_E;
        config.vlanMode        = CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E;
        config.vlanId          = 4095;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, counterIndex, bindMode);

        /*
            1.5. Call cpssExMxPmIpCounterConfigGet with the same counterIndex as in 1.1 - 1.4.
                                                        and non-NULL bindModePtr and non-NULL interfaceConfigPtr.
            Expected: GT_OK and the same bindMode and interfaceConfigPtr.
        */
        st = cpssExMxPmIpCounterConfigGet(dev, counterIndex, &bindModeGet, &configGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpCounterConfigGet: %d, %d", dev, counterIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(bindMode, bindModeGet,
                   "get another bindMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.portTrunkMode, configGet.portTrunkMode,
                   "get another interfaceConfigPtr->portTrunkMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.portTrunk.devPort.devNum, configGet.portTrunk.devPort.devNum,
                   "get another interfaceConfigPtr->devNum than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.portTrunk.trunk, configGet.portTrunk.trunk,
                   "get another interfaceConfigPtr->portTrunk.trunk than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.packetType, configGet.packetType,
                   "get another interfaceConfigPtr->packetType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.vlanMode, configGet.vlanMode,
                   "get another interfaceConfigPtr->vlanMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(config.vlanId, configGet.vlanId,
                   "get another interfaceConfigPtr->vlanId than was set: %d", dev);

        /*
            1.4. Call with counterIndex [3],
                           bindMode [CPSS_EXMXPM_IP_COUNTER_BIND_TO_ROUTE_ENTRY_E]
                           and non-NULL interfaceConfigPtr.
            Expected: GT_OK.
        */
        counterIndex = 3;
        bindMode     = CPSS_EXMXPM_IP_COUNTER_BIND_TO_ROUTE_ENTRY_E;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, counterIndex, bindMode);

        /*
            1.5. Call cpssExMxPmIpCounterConfigGet with the same counterIndex as in 1.1 - 1.4.
                                                        and non-NULL bindModePtr and non-NULL interfaceConfigPtr.
            Expected: GT_OK and the same bindMode and interfaceConfigPtr.
        */
        st = cpssExMxPmIpCounterConfigGet(dev, counterIndex, &bindModeGet, &configGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpCounterConfigGet: %d, %d", dev, counterIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(bindMode, bindModeGet,
                   "get another bindMode than was set: %d", dev);

        /*
            1.6. Call with out of range counterIndex [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        counterIndex = 4;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        counterIndex = 0;

        /*
            1.7. Call with out of range bindMode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        bindMode = IP_STAT_INVALID_ENUM_CNS;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, bindMode = %d", dev, bindMode);

        bindMode = CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E;

        /*
            1.8. Call with out of range interfaceConfigPtr->portTrunkMode [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        config.portTrunkMode = IP_STAT_INVALID_ENUM_CNS;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, interfaceConfigPtr->portTrunkMode = %d",
                                     dev, config.portTrunkMode);

        config.portTrunkMode = CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E;

        /*
            1.9. Call with out of range interfaceConfigPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        config.portTrunk.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, nterfaceConfigPtr->portTrunkMode = %d, interfaceConfigPtr->devNum = %d",
                                     dev, config.portTrunkMode, config.portTrunk.devPort.devNum);

        /*
            1.10. Call with out of range interfaceConfigPtr->devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                            and other parameters from 1.2.
            Expected: NOT GT_OK.
        */
        config.portTrunkMode = CPSS_EXMXPM_IP_PORT_COUNTER_MODE_E;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, nterfaceConfigPtr->portTrunkMode = %d, interfaceConfigPtr->devNum = %d",
                                         dev, config.portTrunkMode, config.portTrunk.devPort.devNum);

        config.portTrunk.devPort.devNum = dev;

        /*
            1.11. Call with out of range interfaceConfigPtr->portTrunk.port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                            and other parameters from 1.2.
            Expected: NOT GT_OK.
        */
        config.portTrunkMode = CPSS_EXMXPM_IP_PORT_COUNTER_MODE_E;
        config.portTrunk.devPort.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, nterfaceConfigPtr->portTrunkMode = %d, interfaceConfigPtr->portTrunk.port = %d",
                                         dev, config.portTrunkMode, config.portTrunk.devPort.port);

        config.portTrunk.devPort.port = 0;

        /*
            1.12. Call with out of range interfaceConfigPtr->portTrunk.trunk [256] (not relevant)
                            and other parameters from 1.2.
            Expected: GT_OK.
        */
        config.portTrunk.trunk = 256;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, nterfaceConfigPtr->portTrunkMode = %d, interfaceConfigPtr->portTrunk.trunk = %d",
                                     dev, config.portTrunkMode, config.portTrunk.trunk);

        /*
            1.13. Call with out of range interfaceConfigPtr->portTrunk.trunk [256]
                            and other parameters from 1.3.
            Expected: NOT GT_OK.
        */
        config.portTrunkMode = CPSS_EXMXPM_IP_TRUNK_COUNTER_MODE_E;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, nterfaceConfigPtr->portTrunkMode = %d, interfaceConfigPtr->portTrunk.trunk = %d",
                                         dev, config.portTrunkMode, config.portTrunk.trunk);

        config.portTrunk.trunk = 2;

        /*
            1.14. Call with out of range interfaceConfigPtr->packetType [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        config.portTrunkMode = CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E;
        config.packetType    = IP_STAT_INVALID_ENUM_CNS;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nterfaceConfigPtr->packetType = %d",
                                     dev, config.packetType);

        config.packetType = CPSS_EXMXPM_IP_PACKET_TYPE_ALL_COUNTER_MODE_E;

        /*
            1.16. Call with out of range interfaceConfigPtr->vlanMode [0x5AAAAAA5]
                            and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        config.vlanMode = IP_STAT_INVALID_ENUM_CNS;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nterfaceConfigPtr->vlanMode = %d",
                                     dev, config.vlanMode);

        config.vlanMode = CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E;

        /*
            1.17. Call with interfaceConfigPtr->vlanMode [CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E]
                            and out of range interfaceConfigPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                            and other parameters from 1.1.
            Expected: GT_OUT_OF_RANGE.
        */
        config.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, nterfaceConfigPtr->vlanMode = %d, interfaceConfigPtr->vlanId= %d",
                                     dev, config.vlanMode, config.vlanId);

        /*
            1.18. Call with interfaceConfigPtr->vlanMode [CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E]
                            and out of range interfaceConfigPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        config.vlanMode = CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E;

        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, nterfaceConfigPtr->vlanMode = %d, interfaceConfigPtr->vlanId= %d",
                                     dev, config.vlanMode, config.vlanId);

        config.vlanId = IP_STAT_TESTED_VLAN_ID_CNS;

        /*
            1.19. Call with interfaceConfigPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, interfaceConfigPtr = NULL", dev);
    }

    counterIndex = 0;
    bindMode     = CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E;

    config.portTrunkMode = CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E;
    config.packetType    = CPSS_EXMXPM_IP_PACKET_TYPE_ALL_COUNTER_MODE_E;
    config.vlanMode      = CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpCounterConfigSet(dev, counterIndex, bindMode, &config);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpCounterConfigGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  counterIndex,
    OUT CPSS_EXMXPM_IP_COUNTER_BIND_MODE_ENT    *bindModePtr,
    OUT CPSS_EXMXPM_IP_COUNTER_SET_CFG_STC      *interfaceConfigPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpCounterConfigGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with counterIndex [0 / 3],
                   non-NULL bindModePtr
                   and non-NULL interfaceConfigPtr.
    Expected: GT_OK.
    1.2. Call with out of range counterIndex [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with bindModePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with interfaceConfigPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                                  counterIndex = 0;
    CPSS_EXMXPM_IP_COUNTER_BIND_MODE_ENT    bindMode     = CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E;
    CPSS_EXMXPM_IP_COUNTER_SET_CFG_STC      config;


    cpssOsBzero((GT_VOID*) &config, sizeof(config));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with counterIndex [0 / 3],
                           non-NULL bindModePtr
                           and non-NULL interfaceConfigPtr.
            Expected: GT_OK.
        */

        /* Call with counterIndex [0] */
        counterIndex = 0;

        st = cpssExMxPmIpCounterConfigGet(dev, counterIndex, &bindMode, &config);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /* Call with counterIndex [3] */
        counterIndex = 3;

        st = cpssExMxPmIpCounterConfigGet(dev, counterIndex, &bindMode, &config);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /*
            1.2. Call with out of range counterIndex [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        counterIndex = 4;

        st = cpssExMxPmIpCounterConfigGet(dev, counterIndex, &bindMode, &config);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        counterIndex = 0;

        /*
            1.3. Call with bindModePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpCounterConfigGet(dev, counterIndex, NULL, &config);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, bindModePtr = NULL", dev);

        /*
            1.4. Call with interfaceConfigPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpCounterConfigGet(dev, counterIndex, &bindMode, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, interfaceConfigPtr = NULL", dev);
    }

    counterIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpCounterConfigGet(dev, counterIndex, &bindMode, &config);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpCounterConfigGet(dev, counterIndex, &bindMode, &config);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpCounterSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          counterIndex,
    IN  CPSS_EXMXPM_IP_COUNTER_SET_STC  *countersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpCounterSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with counterIndex [0 / 3]
                   and countersPtr {inUcPkts [0 / 0xFF],
                                    inMcPkts [0 / 0xFF],
                                    inUcNonRoutedExceptionPkts [0 / 0xFF],
                                    inUcNonRoutedNonExceptionPkts [0 / 0xFF],
                                    inMcNonRoutedExceptionPkts [0 / 0xFF],
                                    inMcNonRoutedNonExceptionPkts [0 / 0xFF],
                                    inUcTrappedMirrorPkts [0 / 0xFF],
                                    inMcTrappedMirrorPkts [0 / 0xFF],
                                    inMcRfpFailPkts [0 / 0xFF],
                                    outUcPkts [0 / 0xFF]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpCounterGet with same counterIndex and non-NULL countersPtr.
    Expected: GT_OK and the same countersPtr.
    1.3. Call with out of range counterIndex [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with countersPtr->inUcPkts [0xFFFFFFFF] (no any constraints)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.5. Call with countersPtr->inMcPkts [0xFFFFFFFF] (no any constraints)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.6. Call with countersPtr->inUcNonRoutedExceptionPkts [0xFFFFFFFF] (no any constraints)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.7. Call with countersPtr->inUcNonRoutedNonExceptionPkts [0xFFFFFFFF] (no any constraints)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.8. Call with countersPtr->inMcNonRoutedExceptionPkts [0xFFFFFFFF] (no any constraints)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.9. Call with countersPtr->inMcNonRoutedNonExceptionPkts [0xFFFFFFFF] (no any constraints)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.10. Call with countersPtr->inUcTrappedMirrorPkts [0xFFFFFFFF] (no any constraints)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.11. Call with countersPtr->inMcTrappedMirrorPkts [0xFFFFFFFF] (no any constraints)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.12. Call with countersPtr->inMcRfpFailPkts [0xFFFFFFFF] (no any constraints)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.13. Call with countersPtr->outUcPkts [0xFFFFFFFF] (no any constraints)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.14. Call with countersPtr [NULL]
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                          counterIndex = 0;
    CPSS_EXMXPM_IP_COUNTER_SET_STC  counters;
    CPSS_EXMXPM_IP_COUNTER_SET_STC  countersGet;


    cpssOsBzero((GT_VOID*) &counters, sizeof(counters));
    cpssOsBzero((GT_VOID*) &countersGet, sizeof(countersGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with counterIndex [0 / 3]
                           and countersPtr {inUcPkts [0 / 0xFF],
                                            inMcPkts [0 / 0xFF],
                                            inUcNonRoutedExceptionPkts [0 / 0xFF],
                                            inUcNonRoutedNonExceptionPkts [0 / 0xFF],
                                            inMcNonRoutedExceptionPkts [0 / 0xFF],
                                            inMcNonRoutedNonExceptionPkts [0 / 0xFF],
                                            inUcTrappedMirrorPkts [0 / 0xFF],
                                            inMcTrappedMirrorPkts [0 / 0xFF],
                                            inMcRfpFailPkts [0 / 0xFF],
                                            outUcPkts [0 / 0xFF]}.
            Expected: GT_OK.
        */

        /* Call with counterIndex [0] */
        counterIndex = 0;

        counters.inUcPkts                      = 0;
        counters.inMcPkts                      = 0;
        counters.inUcNonRoutedExceptionPkts    = 0;
        counters.inUcNonRoutedNonExceptionPkts = 0;
        counters.inMcNonRoutedExceptionPkts    = 0;
        counters.inMcNonRoutedNonExceptionPkts = 0;
        counters.inUcTrappedMirrorPkts         = 0;
        counters.inMcTrappedMirrorPkts         = 0;
        counters.inMcRfpFailPkts               = 0;
        counters.outUcPkts                     = 0;

        st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /*
            1.2. Call cpssExMxPmIpCounterGet with same counterIndex and non-NULL countersPtr.
            Expected: GT_OK and the same countersPtr.
        */
        st = cpssExMxPmIpCounterGet(dev, counterIndex, &countersGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpCounterGet: %d, %d", dev, counterIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inUcPkts, countersGet.inUcPkts,
                   "get another countersPtr->inUcPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcPkts, countersGet.inMcPkts,
                   "get another countersPtr->inMcPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inUcNonRoutedExceptionPkts, countersGet.inUcNonRoutedExceptionPkts,
                   "get another countersPtr->inUcNonRoutedExceptionPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inUcNonRoutedNonExceptionPkts, countersGet.inUcNonRoutedNonExceptionPkts,
                   "get another countersPtr->inUcNonRoutedNonExceptionPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcNonRoutedExceptionPkts, countersGet.inMcNonRoutedExceptionPkts,
                   "get another countersPtr->inMcNonRoutedExceptionPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcNonRoutedNonExceptionPkts, countersGet.inMcNonRoutedNonExceptionPkts,
                   "get another countersPtr->inMcNonRoutedNonExceptionPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inUcTrappedMirrorPkts, countersGet.inUcTrappedMirrorPkts,
                   "get another countersPtr->inUcTrappedMirrorPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcTrappedMirrorPkts, countersGet.inMcTrappedMirrorPkts,
                   "get another countersPtr->inMcTrappedMirrorPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcRfpFailPkts, countersGet.inMcRfpFailPkts,
                   "get another countersPtr->inMcRfpFailPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.outUcPkts, countersGet.outUcPkts,
                   "get another countersPtr->outUcPkts than was set: %d", dev);

        /* Call with counterIndex [3] */
        counterIndex = 3;

        counters.inUcPkts                      = 0xFF;
        counters.inMcPkts                      = 0xFF;
        counters.inUcNonRoutedExceptionPkts    = 0xFF;
        counters.inUcNonRoutedNonExceptionPkts = 0xFF;
        counters.inMcNonRoutedExceptionPkts    = 0xFF;
        counters.inMcNonRoutedNonExceptionPkts = 0xFF;
        counters.inUcTrappedMirrorPkts         = 0xFF;
        counters.inMcTrappedMirrorPkts         = 0xFF;
        counters.inMcRfpFailPkts               = 0xFF;
        counters.outUcPkts                     = 0xFF;

        st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /*
            1.2. Call cpssExMxPmIpCounterGet with same counterIndex and non-NULL countersPtr.
            Expected: GT_OK and the same countersPtr.
        */
        st = cpssExMxPmIpCounterGet(dev, counterIndex, &countersGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmIpCounterGet: %d, %d", dev, counterIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inUcPkts, countersGet.inUcPkts,
                   "get another countersPtr->inUcPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcPkts, countersGet.inMcPkts,
                   "get another countersPtr->inMcPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inUcNonRoutedExceptionPkts, countersGet.inUcNonRoutedExceptionPkts,
                   "get another countersPtr->inUcNonRoutedExceptionPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inUcNonRoutedNonExceptionPkts, countersGet.inUcNonRoutedNonExceptionPkts,
                   "get another countersPtr->inUcNonRoutedNonExceptionPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcNonRoutedExceptionPkts, countersGet.inMcNonRoutedExceptionPkts,
                   "get another countersPtr->inMcNonRoutedExceptionPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcNonRoutedNonExceptionPkts, countersGet.inMcNonRoutedNonExceptionPkts,
                   "get another countersPtr->inMcNonRoutedNonExceptionPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inUcTrappedMirrorPkts, countersGet.inUcTrappedMirrorPkts,
                   "get another countersPtr->inUcTrappedMirrorPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcTrappedMirrorPkts, countersGet.inMcTrappedMirrorPkts,
                   "get another countersPtr->inMcTrappedMirrorPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcRfpFailPkts, countersGet.inMcRfpFailPkts,
                   "get another countersPtr->inMcRfpFailPkts than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(counters.outUcPkts, countersGet.outUcPkts,
                   "get another countersPtr->outUcPkts than was set: %d", dev);

        /*
            1.3. Call with out of range counterIndex [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        counterIndex = 4;

        st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        counterIndex = 0;

        /*
            1.4. Call with countersPtr->inUcPkts [0xFFFFFFFF] (no any constraints)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        counters.inUcPkts = 0xFFFFFFFF;

        st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, countersPtr->inUcPkts = %d",
                                     dev, counters.inUcPkts);

        counters.inUcPkts = 0;

        /*
            1.5. Call with countersPtr->inMcPkts [0xFFFFFFFF] (no any constraints)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        counters.inMcPkts = 0xFFFFFFFF;

        st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, countersPtr->inMcPkts = %d",
                                     dev, counters.inMcPkts);

        counters.inMcPkts = 0;

        /*
            1.6. Call with countersPtr->inUcNonRoutedExceptionPkts [0xFFFFFFFF] (no any constraints)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        counters.inUcNonRoutedExceptionPkts = 0xFFFFFFFF;

        st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, countersPtr->inUcNonRoutedExceptionPkts = %d",
                                     dev, counters.inUcNonRoutedExceptionPkts);

        counters.inUcNonRoutedExceptionPkts = 0;

        /*
            1.7. Call with countersPtr->inUcNonRoutedNonExceptionPkts [0xFFFFFFFF] (no any constraints)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        counters.inUcNonRoutedNonExceptionPkts = 0xFFFFFFFF;

        st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, countersPtr->inUcNonRoutedNonExceptionPkts = %d",
                                     dev, counters.inUcNonRoutedNonExceptionPkts);

        counters.inUcNonRoutedNonExceptionPkts = 0;

        /*
            1.8. Call with countersPtr->inMcNonRoutedExceptionPkts [0xFFFFFFFF] (no any constraints)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        counters.inMcNonRoutedExceptionPkts = 0xFFFFFFFF;

        st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, countersPtr->inMcNonRoutedExceptionPkts = %d",
                                     dev, counters.inMcNonRoutedExceptionPkts);

        counters.inMcNonRoutedExceptionPkts = 0;

        /*
            1.9. Call with countersPtr->inMcNonRoutedNonExceptionPkts [0xFFFFFFFF] (no any constraints)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        counters.inMcNonRoutedNonExceptionPkts = 0xFFFFFFFF;

        st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, countersPtr->inMcNonRoutedNonExceptionPkts = %d",
                                     dev, counters.inMcNonRoutedNonExceptionPkts);

        counters.inMcNonRoutedNonExceptionPkts = 0;

        /*
            1.10. Call with countersPtr->inUcTrappedMirrorPkts [0xFFFFFFFF] (no any constraints)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        counters.inUcTrappedMirrorPkts = 0xFFFFFFFF;

        st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, countersPtr->inUcTrappedMirrorPkts = %d",
                                     dev, counters.inUcTrappedMirrorPkts);

        counters.inUcTrappedMirrorPkts = 0;

        /*
            1.11. Call with countersPtr->inMcTrappedMirrorPkts [0xFFFFFFFF] (no any constraints)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        counters.inMcTrappedMirrorPkts = 0xFFFFFFFF;

        st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, countersPtr->inMcTrappedMirrorPkts = %d",
                                     dev, counters.inMcTrappedMirrorPkts);

        counters.inMcTrappedMirrorPkts = 0;

        /*
            1.12. Call with countersPtr->inMcRfpFailPkts [0xFFFFFFFF] (no any constraints)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        counters.inMcRfpFailPkts = 0xFFFFFFFF;

        st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, countersPtr->inMcRfpFailPkts = %d",
                                     dev, counters.inMcRfpFailPkts);

        counters.inMcRfpFailPkts = 0;

        /*
            1.13. Call with countersPtr->outUcPkts [0xFFFFFFFF] (no any constraints)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        counters.outUcPkts = 0xFFFFFFFF;

        st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, countersPtr->outUcPkts = %d",
                                     dev, counters.outUcPkts);

        counters.outUcPkts = 0;

        /*
            1.14. Call with countersPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpCounterSet(dev, counterIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, countersPtr = NULL", dev);
    }

    counterIndex = 0;

    counters.inUcPkts                      = 0;
    counters.inMcPkts                      = 0;
    counters.inUcNonRoutedExceptionPkts    = 0;
    counters.inUcNonRoutedNonExceptionPkts = 0;
    counters.inMcNonRoutedExceptionPkts    = 0;
    counters.inMcNonRoutedNonExceptionPkts = 0;
    counters.inUcTrappedMirrorPkts         = 0;
    counters.inMcTrappedMirrorPkts         = 0;
    counters.inMcRfpFailPkts               = 0;
    counters.outUcPkts                     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpCounterSet(dev, counterIndex, &counters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpCounterGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          counterIndex,
    OUT CPSS_EXMXPM_IP_COUNTER_SET_STC  *countersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpCounterGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with counterIndex [0 / 3]
                   and non-NULL countersPtr.
    Expected: GT_OK.
    1.2. Call with out of range counterIndex [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with countersPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                          counterIndex = 0;
    CPSS_EXMXPM_IP_COUNTER_SET_STC  counters;


    cpssOsBzero((GT_VOID*) &counters, sizeof(counters));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with counterIndex [0 / 3]
                           and non-NULL countersPtr.
            Expected: GT_OK.
        */

        /* Call with counterIndex [0] */
        counterIndex = 0;

        st = cpssExMxPmIpCounterGet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /* Call with counterIndex [3] */
        counterIndex = 3;

        st = cpssExMxPmIpCounterGet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        /*
            1.2. Call with out of range counterIndex [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        counterIndex = 4;

        st = cpssExMxPmIpCounterGet(dev, counterIndex, &counters);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, counterIndex);

        counterIndex = 0;

        /*
            1.3. Call with countersPtr [NULL]
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpCounterGet(dev, counterIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, countersPtr = NULL", dev);
    }

    counterIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpCounterGet(dev, counterIndex, &counters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpCounterGet(dev, counterIndex, &counters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpDropCounterModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMXPM_IP_DROP_COUNTER_MODE_ENT    mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpDropCounterModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mode [CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_ALL_E /
                         CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_UC_RPF_FAIL_E /
                         CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_OPTION_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpDropCounterModeGet with non-NULL modePtr.
    Expected: GT_OK and the same mode.
    1.3. Call with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_IP_DROP_COUNTER_MODE_ENT mode    = CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_ALL_E;
    CPSS_EXMXPM_IP_DROP_COUNTER_MODE_ENT modeGet = CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_ALL_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_ALL_E /
                                 CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_UC_RPF_FAIL_E /
                                 CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_OPTION_E].
            Expected: GT_OK.
        */

        /* Call with mode [CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_ALL_E] */
        mode = CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_ALL_E;

        st = cpssExMxPmIpDropCounterModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmIpDropCounterModeGet with non-NULL modePtr.
            Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmIpDropCounterModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpDropCounterModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_UC_RPF_FAIL_E] */
        mode = CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_UC_RPF_FAIL_E;

        st = cpssExMxPmIpDropCounterModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmIpDropCounterModeGet with non-NULL modePtr.
            Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmIpDropCounterModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpDropCounterModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_OPTION_E] */
        mode = CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_OPTION_E;

        st = cpssExMxPmIpDropCounterModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmIpDropCounterModeGet with non-NULL modePtr.
            Expected: GT_OK and the same mode.
        */
        st = cpssExMxPmIpDropCounterModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpDropCounterModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /*
            1.3. Call function with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = IP_STAT_INVALID_ENUM_CNS;

        st = cpssExMxPmIpDropCounterModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_ALL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpDropCounterModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpDropCounterModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpDropCounterModeGet
(
    IN  GT_U8                                   devNum,
    OUT CPSS_EXMXPM_IP_DROP_COUNTER_MODE_ENT    *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpDropCounterModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null modePtr.
    Expected: GT_OK.
    1.2. Call with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_IP_DROP_COUNTER_MODE_ENT mode = CPSS_EXMXPM_IP_DROP_COUNTER_COUNT_ALL_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null modePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmIpDropCounterModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpDropCounterModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpDropCounterModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpDropCounterModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpDropCounterSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  value
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpDropCounterSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with value [0 / 100].
    Expected: GT_OK.
    1.2. Call cpssExMxPmIpDropCounterGet with non-NULL valuePtr.
    Expected: GT_OK and the same value.
    1.3. Call with value [0xFFFFFFFF] (no any constraints).
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      value    = 0;
    GT_U32      valueGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with value [0 / 100].
            Expected: GT_OK.
        */

        /* Call with value [0] */
        value = 0;

        st = cpssExMxPmIpDropCounterSet(dev, value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, value);

        /*
            1.2. Call cpssExMxPmIpDropCounterGet with non-NULL valuePtr.
            Expected: GT_OK and the same value.
        */
        st = cpssExMxPmIpDropCounterGet(dev, &valueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpDropCounterGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(value, valueGet,
                       "get another value than was set: %d", dev);

        /* Call with value [100] */
        value = 100;

        st = cpssExMxPmIpDropCounterSet(dev, value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, value);

        /*
            1.2. Call cpssExMxPmIpDropCounterGet with non-NULL valuePtr.
            Expected: GT_OK and the same value.
        */
        st = cpssExMxPmIpDropCounterGet(dev, &valueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmIpDropCounterGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(value, valueGet,
                       "get another value than was set: %d", dev);

        /*
            1.3. Call with value [0xFFFFFFFF] (no any constraints).
            Expected: GT_OK.
        */
        value = 0xFFFFFFFF;

        st = cpssExMxPmIpDropCounterSet(dev, value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, value);
    }

    value = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpDropCounterSet(dev, value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpDropCounterSet(dev, value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmIpDropCounterGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *valuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmIpDropCounterGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null valuePtr.
    Expected: GT_OK.
    1.2. Call with valuePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      value = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null valuePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmIpDropCounterGet(dev, &value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null valuePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmIpDropCounterGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, valuePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmIpDropCounterGet(dev, &value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmIpDropCounterGet(dev, &value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill CounterConfig table.
*/                
UTF_TEST_CASE_MAC(cpssExMxPmIpFillCounterConfigTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in CounterConfig table.
         Call cpssExMxPmIpCounterConfigSet with counterIndex [0 .. numEntries - 1],
                                                bindMode [CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E]
                                                and interfaceConfigPtr {portTrunkMode [CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E],
                                                                        packetType [CPSS_EXMXPM_IP_PACKET_TYPE_ALL_COUNTER_MODE_E],
                                                                        vlanMode [CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E]}.
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmIpCounterConfigSet with counterIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in CounterConfig table and compare with original.
         Call cpssExMxPmIpCounterConfigGet with the same counterIndex and non-NULL pointers.
    Expected: GT_OK and the same params as was set.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmIpCounterConfigGet with counterIndex [numEntries] and non-NULL arpMacAddrPtr.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_EXMXPM_IP_COUNTER_BIND_MODE_ENT    bindMode     = CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E;
    CPSS_EXMXPM_IP_COUNTER_SET_CFG_STC      config;
    CPSS_EXMXPM_IP_COUNTER_BIND_MODE_ENT    bindModeGet  = CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E;
    CPSS_EXMXPM_IP_COUNTER_SET_CFG_STC      configGet;


    cpssOsBzero((GT_VOID*) &config, sizeof(config));
    cpssOsBzero((GT_VOID*) &configGet, sizeof(configGet));

    /* Fill the entry for CounterConfig table */
    bindMode     = CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E;

    config.portTrunkMode = CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E;
    config.packetType    = CPSS_EXMXPM_IP_PACKET_TYPE_ALL_COUNTER_MODE_E;
    config.vlanMode      = CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E;
    config.vlanId        = 100;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 4;

        /* 1.2. Fill all entries in CounterConfig table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            config.vlanId = (GT_U16)(iTemp % 4095);

            st = cpssExMxPmIpCounterConfigSet(dev, iTemp, bindMode, &config);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmIpCounterConfigSet: %d, %d, %d", dev, iTemp, bindMode);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmIpCounterConfigSet(dev, numEntries, bindMode, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpCounterConfigSet: %d, %d", dev, numEntries);

        /* 1.4. Read all entries in CounterConfig table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            config.vlanId = (GT_U16)(iTemp % 4095);

            st = cpssExMxPmIpCounterConfigGet(dev, iTemp, &bindModeGet, &configGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpCounterConfigGet: %d, %d", 
                                         dev, iTemp);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(bindMode, bindModeGet,
                       "get another bindMode than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(config.portTrunkMode, configGet.portTrunkMode,
                       "get another interfaceConfigPtr->portTrunkMode than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(config.packetType, configGet.packetType,
                       "get another interfaceConfigPtr->packetType than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(config.vlanMode, configGet.vlanMode,
                       "get another interfaceConfigPtr->vlanMode than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(config.vlanId, configGet.vlanId,
                       "get another interfaceConfigPtr->vlanId than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmIpCounterConfigGet(dev, numEntries, &bindModeGet, &configGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpCounterConfigGet: %d, %d", dev, numEntries);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Counter table.
*/                
UTF_TEST_CASE_MAC(cpssExMxPmIpFillCounterTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in Counter table.
         Call cpssExMxPmIpCounterSet with counterIndex [0 ..  numEntries-1]
                                       and countersPtr {inUcPkts [0 / 0xFF],
                                                        inMcPkts [0 / 0xFF],
                                                        inUcNonRoutedExceptionPkts [0 / 0xFF],
                                                        inUcNonRoutedNonExceptionPkts [0 / 0xFF],
                                                        inMcNonRoutedExceptionPkts [0 / 0xFF],
                                                        inMcNonRoutedNonExceptionPkts [0 / 0xFF],
                                                        inUcTrappedMirrorPkts [0 / 0xFF],
                                                        inMcTrappedMirrorPkts [0 / 0xFF],
                                                        inMcRfpFailPkts [0 / 0xFF],
                                                        outUcPkts [0 / 0xFF]}.
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmIpCounterSet with counterIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in Counter table and compare with original.
         Call cpssExMxPmIpCounterGet with the same counterIndex and non-NULL countersPtr.
    Expected: GT_OK and the same counters as was set.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmIpCounterGet with counterIndex [numEntries] and non-NULL arpMacAddrPtr.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_EXMXPM_IP_COUNTER_SET_STC  counters;
    CPSS_EXMXPM_IP_COUNTER_SET_STC  countersGet;


    cpssOsBzero((GT_VOID*) &counters, sizeof(counters));
    cpssOsBzero((GT_VOID*) &countersGet, sizeof(countersGet));

    /* Fill the entry for Counter table */
    counters.inUcPkts                      = 0;
    counters.inMcPkts                      = 0;
    counters.inUcNonRoutedExceptionPkts    = 0;
    counters.inUcNonRoutedNonExceptionPkts = 0;
    counters.inMcNonRoutedExceptionPkts    = 0;
    counters.inMcNonRoutedNonExceptionPkts = 0;
    counters.inUcTrappedMirrorPkts         = 0;
    counters.inMcTrappedMirrorPkts         = 0;
    counters.inMcRfpFailPkts               = 0;
    counters.outUcPkts                     = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 4;

        /* 1.2. Fill all entries in Counter table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            counters.inUcPkts              = (GT_U32)(iTemp % 255);
            counters.inMcPkts              = (GT_U32)(iTemp % 255);
            counters.inUcTrappedMirrorPkts = (GT_U32)(iTemp % 255);
            counters.inMcTrappedMirrorPkts = (GT_U32)(iTemp % 255);

            st = cpssExMxPmIpCounterSet(dev, iTemp, &counters);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpCounterSet: %d, %d, %d", dev, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmIpCounterSet(dev, numEntries, &counters);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpCounterSet: %d, %d", dev, numEntries);

        /* 1.4. Read all entries in Counter table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            counters.inUcPkts              = (GT_U32)(iTemp % 255);
            counters.inMcPkts              = (GT_U32)(iTemp % 255);
            counters.inUcTrappedMirrorPkts = (GT_U32)(iTemp % 255);
            counters.inMcTrappedMirrorPkts = (GT_U32)(iTemp % 255);

            st = cpssExMxPmIpCounterGet(dev, iTemp, &countersGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpCounterGet: %d, %d", dev, iTemp);
    
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(counters.inUcPkts, countersGet.inUcPkts,
                       "get another countersPtr->inUcPkts than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcPkts, countersGet.inMcPkts,
                       "get another countersPtr->inMcPkts than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(counters.inUcNonRoutedExceptionPkts, countersGet.inUcNonRoutedExceptionPkts,
                       "get another countersPtr->inUcNonRoutedExceptionPkts than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(counters.inUcNonRoutedNonExceptionPkts, countersGet.inUcNonRoutedNonExceptionPkts,
                       "get another countersPtr->inUcNonRoutedNonExceptionPkts than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcNonRoutedExceptionPkts, countersGet.inMcNonRoutedExceptionPkts,
                       "get another countersPtr->inMcNonRoutedExceptionPkts than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcNonRoutedNonExceptionPkts, countersGet.inMcNonRoutedNonExceptionPkts,
                       "get another countersPtr->inMcNonRoutedNonExceptionPkts than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(counters.inUcTrappedMirrorPkts, countersGet.inUcTrappedMirrorPkts,
                       "get another countersPtr->inUcTrappedMirrorPkts than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcTrappedMirrorPkts, countersGet.inMcTrappedMirrorPkts,
                       "get another countersPtr->inMcTrappedMirrorPkts than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(counters.inMcRfpFailPkts, countersGet.inMcRfpFailPkts,
                       "get another countersPtr->inMcRfpFailPkts than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(counters.outUcPkts, countersGet.outUcPkts,
                       "get another countersPtr->outUcPkts than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmIpCounterGet(dev, numEntries, &countersGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmIpCounterGet: %d, %d", dev, numEntries);
    }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmIpStat suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmIpStat)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpCounterConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpCounterConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpCounterSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpDropCounterModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpDropCounterModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpDropCounterSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpDropCounterGet)
    /* Tests for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpFillCounterConfigTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmIpFillCounterTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmIpStat)

