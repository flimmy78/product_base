/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmTunnelStartUT.c
*
* DESCRIPTION:
*       Unit tests for CPSS ExMxPm tunnel.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/exMxPm/exMxPmGen/tunnelStart/cpssExMxPmTunnelStart.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Invalid enum */
#define TS_INVALID_ENUM_CNS             0x5AAAAAA5

/* max valid index for TunnelStartEntry */
#ifdef PUMA_1 /* need to change in puma2 */
#define TUNNEL_START_MAX_LINE_INDEX     341
#else
#define TUNNEL_START_MAX_LINE_INDEX     4095
#endif

#define TS_INVALID_TUNNEL_ENUM_CNS      13

/* MPLS labels count (valid values: 0..PRV_CPSS_MPLS_LABEL_RANGE-1) */
#define PRV_CPSS_MPLS_LABEL_RANGE       1048576


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTunnelStartEgressFilteringEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTunnelStartEgressFilteringEnableSet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call with enable [GT_TRUE / GT_FALSE]. Expected: GT_OK.
1.2. Call cpssExMxPmTunnelStartEgressFilteringEnableGet with non-NULL enablePtr. Expected: GT_OK and the same enable.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   devNum;
    GT_BOOL                 enable;
    GT_BOOL                 enableRet;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* 1.1. Call with enable [GT_TRUE / GT_FALSE]. Expected: GT_OK. */

        enable = GT_FALSE;

        st = cpssExMxPmTunnelStartEgressFilteringEnableSet(devNum, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, devNum, enable);

        enable = GT_TRUE;

        st = cpssExMxPmTunnelStartEgressFilteringEnableSet(devNum, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, devNum, enable);

        /* 1.2. Call cpssExMxPmTunnelStartEgressFilteringEnableGet with
           non-NULL enablePtr. Expected: GT_OK and the same enable. */

        st = cpssExMxPmTunnelStartEgressFilteringEnableGet(devNum, &enableRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmTunnelStartEgressFilteringEnableGet: %d", devNum);

        if (GT_OK == st)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableRet,
             "cpssExMxPmTunnelStartEgressFilteringEnableGet: get another enable than was set: %d", devNum);
        }
    }

    /* set correct values*/
    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_FALSE))
    {
        st = cpssExMxPmTunnelStartEgressFilteringEnableSet(devNum, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, devNum);
    }

    /* 3. Call function with out of bound value for device id.*/
    devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTunnelStartEgressFilteringEnableSet(devNum, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, devNum);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTunnelStartEgressFilteringEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTunnelStartEgressFilteringEnableGet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call with non-null enablePtr. Expected: GT_OK.
1.2. Call with enablePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   devNum;
    GT_BOOL                 enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* 1.1. Call with non-null enablePtr. Expected: GT_OK. */

        st = cpssExMxPmTunnelStartEgressFilteringEnableGet(devNum, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, devNum);

        /* 1.2. Call with enablePtr [NULL]. Expected: GT_BAD_PTR. */

        st = cpssExMxPmTunnelStartEgressFilteringEnableGet(devNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", devNum);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_FALSE))
    {
        st = cpssExMxPmTunnelStartEgressFilteringEnableGet(devNum, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, devNum);
    }

    /* 3. Call function with out of bound value for device id.*/
    devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTunnelStartEgressFilteringEnableGet(devNum, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, devNum);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTunnelStartEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_EXMXPM_TUNNEL_START_CONFIG_UNT *configPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTunnelStartEntrySet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call with routerArpTunnelStartLineIndex [0 / 300 / TUNNEL_START_MAX_LINE_INDEX = 342], 
               tunnelType [CPSS_TUNNEL_X_OVER_IPV4_E], 
               valid configPtr { ipv4Cfg { tagEnable [GT_TRUE], 
                                           vlanId [100], 
                                           upMarkMode [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_ENTRY_E], 
                                           up [0], 
                                           dscpMarkMode [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_ENTRY_E], 
                                           dscp [0], 
                                           macDa [AA:BB:CC:00:00:11], 
                                           dontFragmentFlag [GT_TRUE], 
                                           ttl [0], 
                                           autoTunnel [GT_FALSE], 
                                           autoTunnelOffset [0], 
                                           ethType [CPSS_EXMXPM_TUNNEL_START_IPV4_GRE0_E], 
                                           destIp = {172.16.0.1}, 
                                           srcIp = {172.16.0.2}, 
                                           retainCrc [GT_FALSE]}}. 
Expected: GT_OK.
1.2. Call cpssExMxPmTunnelStartEntryGet with routerArpTunnelStartLineIndex [0], non-NULL configPtr, other params same as in 1.1. 
Expected: GT_OK and same tunnelType, config as written.
1.3. Call with out of range routerArpTunnelStartLineIndex [TUNNEL_START_MAX_LINE_INDEX + 1 = 1024], other params same as in 1.1. 
Expected: non GT_OK.
1.4. Call with out of range tunnelType [0x5AAAAA5], other params same as in 1.1. 
Expected: GT_BAD_PARAM.
1.5. Call with incorrect tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E / CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E / CPSS_TUNNEL_IPV6_OVER_IPV4_E / CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E / CPSS_TUNNEL_IP_OVER_MPLS_E / CPSS_TUNNEL_ETHERNET_OVER_MPLS_E / CPSS_TUNNEL_IP_OVER_X_E], other params same as in 1.1. 
Expected: GT_BAD_PARAM.
---- ipv4Cfg invalid but not relevant
1.6. Call with valid configPtr {ipv4Cfg.tagEnable [GT_FALSE], out of range ipv4Cfg.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] (not relevant)} and other parameters as in 1.1. 
Expected: GT_OK.
1.7. Call with valid configPtr {ipv4Cfg.tagEnable [GT_FALSE], out of range ipv4Cfg.upMarkMode [0x5AAAAAA5] (not relevant)} and other parameters as in 1.1. Expected: GT_OK.
1.8. Call with valid configPtr {ipv4Cfg.upMarkMode [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_INGRESS_PIPE_E], out of range ipv4Cfg.up [CPSS_USER_PRIORITY_RANGE_CNS = 8] (not relevant)} and other parameters same as in 1.1. Expected: GT_OK.
1.9. Call with valid configPtr {ipv4Cfg.dscpMarkMode [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_INGRESS_PIPE_E], out of range ipv4Cfg.dscp [CPSS_DSCP_RANGE_CNS = 64] (not relevant) } and other parameters same as in 1.1. Expected: GT_OK.
---- invalid/out of range  ipv4Cfg
1.10. Call with out of range ipv4Cfg.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.11. Call with out of range ipv4Cfg.upMarkMode [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.12. Call with out of range ipv4Cfg.up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters the same as in 1.1. Expected: non GT_OK.
1.13. Call with out of range ipv4Cfg.dscpMarkMode [0x5AAAAA5], other params same as in 1.1. Expected: GT_BAD_PARAM.
1.14. Call with out of range ipv4Cfg.dscp [CPSS_DSCP_RANGE_CNS = 64] and other parameters the same as in 1.1. Expected: non GT_OK.
1.15. Call with out of range ipv4Cfg.ttl [256], other params same as in 1.1. Expected: non GT_OK.

---tunnelType [CPSS_TUNNEL_X_OVER_GRE_IPV4_E]
1.16. Call with routerArpTunnelStartLineIndex [300], tunnelType [CPSS_TUNNEL_X_OVER_GRE_IPV4_E], other params same as in 1.1. Expected: GT_OK.

----tunnelType [CPSS_TUNNEL_X_OVER_MPLS_E]

1.17. Call with routerArpTunnelStartLineIndex [0], tunnelType [CPSS_TUNNEL_X_OVER_MPLS_E], valid configPtr { mplsCfg { tagEnable [GT_TRUE], vlanId [100], upMarkMode [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_ENTRY_E], up [4], macDa [AA:BB:CC:00:00:11], numLabels [2], ttl [200], label1 [0], exp1MarkMode [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_ENTRY_E], exp1 [0], label2 [0], exp2MarkMode [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_ENTRY_E], exp2 [0], retainCrc [GT_FALSE] } }. Expected: GT_OK.
1.18. Call cpssExMxPmTunnelStartEntryGet with non-NULL configPtr, other params same as in 1.17. Expected: GT_OK and same tunnelType, config as written.

--- mplsCfg invalid but not relevant
1.19. Call with valid configPtr {mplsCfg.tagEnable [GT_FALSE], out of range mplsCfg.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] (not relevant)} and other parameters as in 1.17. Expected: GT_OK.
1.20. Call with valid configPtr {mplsCfg.tagEnable [GT_FALSE], out of range mplsCfg.upMarkMode [0x5AAAAAA5] (not relevant)} and other parameters as in 1.17. Expected: GT_OK.
1.21. Call with valid configPtr {mplsCfg.upMarkMode [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_INGRESS_PIPE_E], out of range mplsCfg.up [CPSS_USER_PRIORITY_RANGE_CNS = 8] (not relevant)} and other parameters same as in 1.17. Expected: GT_OK.
1.22. Call with valid configPtr {mplsCfg.numLabels [1], out of range mplsCfg-> label1 [PRV_CPSS_MPLS_LABEL_RANGE = 1048576] (not relevant)} and other parameters same as in 1.17. Expected: GT_OK.
1.23. Call with valid configPtr {mplsCfg.numLabels [1], out of range mplsCfg.exp1MarkMode [0x5AAAAAA5] (not relevant)} and other parameters same as in 1.17. Expected: GT_OK.
1.24. Call with valid configPtr {mplsCfg.numLabels [1], out of range mplsCfg.exp1 [CPSS_EXP_RANGE_CNS = 8] (not relevant)} and other parameters same as in 1.17. Expected: GT_OK.
1.25. Call with valid configPtr {mplsCfg.exp2MarkMode [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_INGRESS_PIPE_E], out of range mplsCfg.exp2 [CPSS_EXP_RANGE_CNS = 8] (not relevant)} and other parameters same as in 1.17. Expected: GT_OK.

---- invalid/out of range mplsCfg
1.26. Call with out of range mplsCfg.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] and other parameters the same as in 1.17. Expected: GT_BAD_PARAM.
1.27. Call with out of range mplsCfg.upMarkMode [0x5AAAAAA5] and other parameters the same as in 1.17. Expected: GT_BAD_PARAM.
1.28. Call with out of range mplsCfg.up [CPSS_USER_PRIORITY_RANGE_CNS = 8] and other parameters the same as in 1.17. Expected: non GT_OK.
1.29. Call with out of range mplsCfg.ttl [256], other params same as in 1.17. Expected: non GT_OK.
1.30. Call with out of range mplsCfg.label1 [PRV_CPSS_MPLS_LABEL_RANGE =1048576] and other parameters the same as in 1.17. Expected: non GT_OK.
1.31. Call with out of range mplsCfg.label2 [PRV_CPSS_MPLS_LABEL_RANGE = 1048576] and other parameters the same as in 1.17. Expected: non GT_OK.
1.32. Call with out of range mplsCfg.exp1MarkMode [0x5AAAAAA5] and other parameters the same as in 1.17. Expected: GT_BAD_PARAM.
1.33. Call with out of range mplsCfg.exp2MarkMode [0x5AAAAAA5] and other parameters the same as in 1.17. Expected: GT_BAD_PARAM.
1.34. Call with out of range mplsCfg.exp1 [CPSS_EXP_RANGE_CNS = 8] and other parameters the same as in 1.17. Expected: non GT_OK.
1.35. Call with out of range mplsCfg.exp2 [CPSS_EXP_RANGE_CNS = 8] and other parameters the same as in 1.17. Expected: non GT_OK.
-----
1.36. Call with configPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   devNum;
    GT_U32                  routerArpTunnelStartLineIndex;
    CPSS_TUNNEL_TYPE_ENT                    tunnelType;
    CPSS_EXMXPM_TUNNEL_START_ENTRY_UNT      config;
    CPSS_EXMXPM_TUNNEL_START_ENTRY_UNT      configRet;
    CPSS_EXMXPM_TUNNEL_START_ENTRY_UNT      configIpv4ValidValues;  /* valid values for ipv4Cfg */
    CPSS_EXMXPM_TUNNEL_START_ENTRY_UNT      configMplsValidValues;  /* valid values for mplsCfg */
    GT_ETHERADDR                            mac1 = {{0xAA, 0xBB, 0xCC, 0, 0, 0x11}};
    GT_BOOL                                 isEqual;

    /* ipv4------ Set valid values for config.ipv4Cfg */
    cpssOsBzero((GT_VOID*) &configIpv4ValidValues, sizeof(configIpv4ValidValues));
    configIpv4ValidValues.ipv4Cfg.tagEnable = GT_TRUE;
    configIpv4ValidValues.ipv4Cfg.vlanId = 100;
    configIpv4ValidValues.ipv4Cfg.upMarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_ENTRY_E;
    configIpv4ValidValues.ipv4Cfg.up = 0;
    configIpv4ValidValues.ipv4Cfg.dscpMarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_ENTRY_E;
    configIpv4ValidValues.ipv4Cfg.dscp = 0;
    configIpv4ValidValues.ipv4Cfg.macDa = mac1;  /* [AA:BB:CC:00:00:11] */
    configIpv4ValidValues.ipv4Cfg.dontFragmentFlag = GT_TRUE;
    configIpv4ValidValues.ipv4Cfg.ttl = 0;
    configIpv4ValidValues.ipv4Cfg.autoTunnel = GT_FALSE;
    configIpv4ValidValues.ipv4Cfg.autoTunnelOffset = 0;
    configIpv4ValidValues.ipv4Cfg.ethType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;
    /* destIp = {172.16.0.1}; */
    configIpv4ValidValues.ipv4Cfg.destIp.arIP[0] = 172;
    configIpv4ValidValues.ipv4Cfg.destIp.arIP[1] = 16;
    configIpv4ValidValues.ipv4Cfg.destIp.arIP[2] = 0;
    configIpv4ValidValues.ipv4Cfg.destIp.arIP[3] = 1;
    /* srcIp = {172.16.0.2}; */
    configIpv4ValidValues.ipv4Cfg.srcIp.arIP[0] = 172;
    configIpv4ValidValues.ipv4Cfg.srcIp.arIP[1] = 16;
    configIpv4ValidValues.ipv4Cfg.srcIp.arIP[2] = 0;
    configIpv4ValidValues.ipv4Cfg.srcIp.arIP[3] = 2;
    configIpv4ValidValues.ipv4Cfg.retainCrc = GT_FALSE;

    /* Mpls------ Set valid values for config.mplsCfg */
    cpssOsBzero((GT_VOID*) &configMplsValidValues, sizeof(configMplsValidValues));
    configMplsValidValues.mplsCfg.tagEnable = GT_TRUE;
    configMplsValidValues.mplsCfg.vlanId = 100;
    configMplsValidValues.mplsCfg.upMarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_ENTRY_E;
    configMplsValidValues.mplsCfg.up = 4;
    configMplsValidValues.mplsCfg.macDa = mac1;   /* [AA:BB:CC:00:00:11] */
    configMplsValidValues.mplsCfg.numLabels = 2;
    configMplsValidValues.mplsCfg.ttl = 200;
    configMplsValidValues.mplsCfg.label1 = 0;
    configMplsValidValues.mplsCfg.exp1MarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_ENTRY_E;
    configMplsValidValues.mplsCfg.exp1 = 0;
    configMplsValidValues.mplsCfg.label2 = 0;
    configMplsValidValues.mplsCfg.exp2MarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_ENTRY_E;
    configMplsValidValues.mplsCfg.exp2 = 0;
    configMplsValidValues.mplsCfg.retainCrc = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /*1.1. Call with routerArpTunnelStartLineIndex [0 / 300 /
        TUNNEL_START_MAX_LINE_INDEX = 1023], tunnelType
        [CPSS_TUNNEL_X_OVER_IPV4_E], valid configPtr { ipv4Cfg { tagEnable
        [GT_TRUE], vlanId [100], upMarkMode
        [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_ENTRY_E], up [0], dscpMarkMode
        [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_ENTRY_E], dscp [0], macDa
        [AA:BB:CC:00:00:11], dontFragmentFlag [GT_TRUE], ttl [0], autoTunnel
        [GT_FALSE], autoTunnelOffset [0], ethType
        [CPSS_EXMXPM_TUNNEL_START_IPV4_GRE0_E], destIp = {172.16.0.1}, srcIp =
        {172.16.0.2}, retainCrc [GT_FALSE]}. Expected: GT_OK.*/

        routerArpTunnelStartLineIndex = 0;
        tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;
        /* copy ipv4cfg valid values to config */
        cpssOsMemCpy((GT_VOID*) &config, (GT_VOID*) &configIpv4ValidValues, sizeof(config));

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, devNum, routerArpTunnelStartLineIndex, tunnelType);

        routerArpTunnelStartLineIndex = 300;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, devNum, routerArpTunnelStartLineIndex, tunnelType);

        routerArpTunnelStartLineIndex = TUNNEL_START_MAX_LINE_INDEX;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, devNum, routerArpTunnelStartLineIndex, tunnelType);

        routerArpTunnelStartLineIndex = 0;  /* restore */

        /*1.2. Call cpssExMxPmTunnelStartEntryGet with
        routerArpTunnelStartLineIndex [0], non-NULL configPtr, other params same
        as in 1.1. Expected: GT_OK and same tunnelType, config as written.*/

        cpssOsBzero((GT_VOID*) &configRet, sizeof(configRet));

        st = cpssExMxPmTunnelStartEntryGet(devNum, routerArpTunnelStartLineIndex, tunnelType, &configRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmTunnelStartEntryGet: %d", devNum);

        if (GT_OK == st)
        {
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&config, (GT_VOID*)&configRet, sizeof (config)))
                  ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                             "cpssExMxPmTunnelStartEntryGet: get another config than was set: %d", devNum);
        }

        /*1.3. Call with out of range routerArpTunnelStartLineIndex
        [TUNNEL_START_MAX_LINE_INDEX + 1 = 1024], other params same as in 1.1.
        Expected: non GT_OK.*/

        routerArpTunnelStartLineIndex = TUNNEL_START_MAX_LINE_INDEX + 1;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routerArpTunnelStartLineIndex = %d", devNum, routerArpTunnelStartLineIndex);

        routerArpTunnelStartLineIndex = 0;

        /*1.4. Call with out of range tunnelType [0x5AAAAA5], other params same
        as in 1.1. Expected: GT_BAD_PARAM.*/

        tunnelType = TS_INVALID_ENUM_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tunnelType = %d", devNum, tunnelType);

        tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;

        /*1.5. Call with incorrect tunnelType [CPSS_TUNNEL_IPV4_OVER_IPV4_E /
        CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E / CPSS_TUNNEL_IPV6_OVER_IPV4_E /
        CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E / CPSS_TUNNEL_IP_OVER_MPLS_E /
        CPSS_TUNNEL_ETHERNET_OVER_MPLS_E / CPSS_TUNNEL_IP_OVER_X_E /
        CPSS_TUNNEL_INVALID_E], other params same as in 1.1. Expected:
        GT_BAD_PARAM.*/


        tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tunnelType = %d", devNum, tunnelType);

        tunnelType = CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tunnelType = %d", devNum, tunnelType);

        tunnelType = CPSS_TUNNEL_IPV6_OVER_IPV4_E;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tunnelType = %d", devNum, tunnelType);

        tunnelType = CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tunnelType = %d", devNum, tunnelType);

        tunnelType = CPSS_TUNNEL_IP_OVER_MPLS_E;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tunnelType = %d", devNum, tunnelType);

        tunnelType = CPSS_TUNNEL_ETHERNET_OVER_MPLS_E;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tunnelType = %d", devNum, tunnelType);

        tunnelType = CPSS_TUNNEL_IP_OVER_X_E;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tunnelType = %d", devNum, tunnelType);

        tunnelType = TS_INVALID_TUNNEL_ENUM_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tunnelType = %d", devNum, tunnelType);

        tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;  /* restore valid */


        /*---- ipv4Cfg invalid but not relevant*/

        /*1.6. Call with valid configPtr { ipv4Cfg.tagEnable [GT_FALSE], out of
        range ipv4Cfg.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] }
        and other parameters as in 1.1. Expected: GT_BAD_PARAM.*/
        config.ipv4Cfg.tagEnable = GT_FALSE;

        config.ipv4Cfg.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, config.ipv4Cfg.tagEnable = %d, config.ipv4Cfg.vlanId = %d",
                                     devNum, config.ipv4Cfg.tagEnable, config.ipv4Cfg.vlanId);

        config.ipv4Cfg.vlanId = configIpv4ValidValues.ipv4Cfg.vlanId;


        /*1.7. Call with valid configPtr {ipv4Cfg.tagEnable [GT_FALSE], out of
        range ipv4Cfg.upMarkMode [0x5AAAAAA5] } and other
        parameters as in 1.1. Expected: GT_BAD_PARAM.*/

        config.ipv4Cfg.upMarkMode = TS_INVALID_ENUM_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, config.ipv4Cfg.tagEnable = %d, config.ipv4Cfg.upMarkMode = %d",
                                     devNum, config.ipv4Cfg.tagEnable, config.ipv4Cfg.upMarkMode);

        config.ipv4Cfg.upMarkMode = configIpv4ValidValues.ipv4Cfg.upMarkMode;

        config.ipv4Cfg.tagEnable = configIpv4ValidValues.ipv4Cfg.tagEnable;

        /*1.8. Call with valid configPtr {ipv4Cfg.upMarkMode
        [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_INGRESS_PIPE_E], out of range
        ipv4Cfg.up [CPSS_USER_PRIORITY_RANGE_CNS = 8] } and other
        parameters same as in 1.1. Expected: GT_BAD_PARAM.*/

        config.ipv4Cfg.upMarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_INGRESS_PIPE_E;
        config.ipv4Cfg.up = TS_INVALID_ENUM_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, config.ipv4Cfg.upMarkMode = %d, config.ipv4Cfg.up = %d",
                                     devNum, config.ipv4Cfg.upMarkMode, config.ipv4Cfg.up);

        config.ipv4Cfg.up = configIpv4ValidValues.ipv4Cfg.up;
        config.ipv4Cfg.upMarkMode = configIpv4ValidValues.ipv4Cfg.upMarkMode;

        /*1.9. Call with valid configPtr {ipv4Cfg.dscpMarkMode
        [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_INGRESS_PIPE_E], out of range
        ipv4Cfg.dscp [CPSS_DSCP_RANGE_CNS = 64] } and other
        parameters same as in 1.1. Expected: GT_BAD_PARAM.*/

        config.ipv4Cfg.dscpMarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_INGRESS_PIPE_E;
        config.ipv4Cfg.dscp = TS_INVALID_ENUM_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, config.ipv4Cfg.dscpMarkMode = %d, config.ipv4Cfg.dscp = %d",
                                     devNum, config.ipv4Cfg.dscpMarkMode, config.ipv4Cfg.dscp);

        config.ipv4Cfg.dscp = configIpv4ValidValues.ipv4Cfg.dscp;
        config.ipv4Cfg.dscpMarkMode = configIpv4ValidValues.ipv4Cfg.dscpMarkMode;

        /*---- invalid/out of range ipv4Cfg*/

        /*1.10. Call with out of range config.ipv4Cfg.vlanId
        [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] and other parameters the same as in
        1.1. Expected: GT_BAD_PARAM.*/

        config.ipv4Cfg.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, config.ipv4Cfg.vlanId = %d", devNum, config.ipv4Cfg.vlanId);

        config.ipv4Cfg.vlanId = configIpv4ValidValues.ipv4Cfg.vlanId;


        /*1.11. Call with out of range config.ipv4Cfg.upMarkMode [0x5AAAAAA5] and other
        parameters the same as in 1.1. Expected: GT_BAD_PARAM.*/

        config.ipv4Cfg.upMarkMode = TS_INVALID_ENUM_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, config.ipv4Cfg.upMarkMode = %d", devNum, config.ipv4Cfg.upMarkMode);

        config.ipv4Cfg.upMarkMode = configIpv4ValidValues.ipv4Cfg.upMarkMode;

        /*1.12. Call with out of range config.ipv4Cfg.up [CPSS_USER_PRIORITY_RANGE_CNS
        = 8] and other parameters the same as in 1.1. Expected: non GT_OK.*/

        config.ipv4Cfg.up = CPSS_USER_PRIORITY_RANGE_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, config.ipv4Cfg.up = %d", devNum, config.ipv4Cfg.up);

        config.ipv4Cfg.up = configIpv4ValidValues.ipv4Cfg.up;

        /*1.13. Call with out of range config.ipv4Cfg.dscpMarkMode [0x5AAAAA5], other
        params same as in 1.1. Expected: GT_BAD_PARAM.*/

        config.ipv4Cfg.dscpMarkMode = TS_INVALID_ENUM_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, config.ipv4Cfg.dscpMarkMode = %d", devNum, config.ipv4Cfg.dscpMarkMode);

        config.ipv4Cfg.dscpMarkMode = configIpv4ValidValues.ipv4Cfg.dscpMarkMode;

        /*1.14. Call with out of range config.ipv4Cfg.dscp [CPSS_DSCP_RANGE_CNS = 64]
        and other parameters the same as in 1.1. Expected: non GT_OK.*/

        config.ipv4Cfg.dscp = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, config.ipv4Cfg.dscp = %d", devNum, config.ipv4Cfg.dscp);

        config.ipv4Cfg.dscp = configIpv4ValidValues.ipv4Cfg.dscp;


        /*1.15. Call with out of range config.ipv4Cfg.ttl [256], other params same as
        in 1.1. Expected: non GT_OK.*/

        config.ipv4Cfg.ttl = 256;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, config.ipv4Cfg.ttl = %d", devNum, config.ipv4Cfg.ttl);

        config.ipv4Cfg.ttl = configIpv4ValidValues.ipv4Cfg.ttl;


        /*---tunnelType [CPSS_TUNNEL_X_OVER_GRE_IPV4_E]*/

        /*1.16. Call with routerArpTunnelStartLineIndex [300], tunnelType
        [CPSS_TUNNEL_X_OVER_GRE_IPV4_E], other params same as in 1.1. Expected:
        GT_OK.*/

        routerArpTunnelStartLineIndex = 300;
        tunnelType = CPSS_TUNNEL_X_OVER_GRE_IPV4_E;
        /* copy ipv4cfg valid values to config */
        cpssOsMemCpy((GT_VOID*) &config, (GT_VOID*) &configIpv4ValidValues, sizeof(config));

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, devNum, routerArpTunnelStartLineIndex, tunnelType);


        /*----tunnelType [CPSS_TUNNEL_X_OVER_MPLS_E]*/

        /*1.17. Call with routerArpTunnelStartLineIndex [0], tunnelType
        [CPSS_TUNNEL_X_OVER_MPLS_E], valid configPtr { mplsCfg { tagEnable
        [GT_TRUE], vlanId [100], upMarkMode
        [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_ENTRY_E], up [4], macDa
        [AA:BB:CC:00:00:11], numLabels [2], ttl [200], label1 [0], exp1MarkMode
        [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_ENTRY_E], exp1 [0], label2 [0],
        exp2MarkMode [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_ENTRY_E], exp2 [0],
        retainCrc [GT_FALSE] } }. Expected: GT_OK.*/

        routerArpTunnelStartLineIndex = 0;
        tunnelType = CPSS_TUNNEL_X_OVER_MPLS_E;
        /* copy mplscfg valid values to config */
        cpssOsMemCpy((GT_VOID*) &config, (GT_VOID*) &configMplsValidValues, sizeof(config));

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, devNum, routerArpTunnelStartLineIndex, tunnelType);

        /*1.18. Call cpssExMxPmTunnelStartEntryGet with non-NULL configPtr,
        other params same as in 1.17. Expected: GT_OK and same tunnelType,
        config as written.*/

        cpssOsBzero((GT_VOID*) &configRet, sizeof(configRet));

        st = cpssExMxPmTunnelStartEntryGet(devNum, routerArpTunnelStartLineIndex, tunnelType, &configRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxPmTunnelStartEntryGet: %d", devNum);

        if (GT_OK == st)
        {
            isEqual = (0 == cpssOsMemCmp((GT_VOID*)&config, (GT_VOID*)&configRet, sizeof (config)))
                  ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                             "cpssExMxPmTunnelStartEntryGet: get another config than was set: %d", devNum);
        }

        /*--- mplsCfg invalid but not relevant*/

        /*1.19. Call with valid configPtr {mplsCfg.tagEnable [GT_FALSE], out of
        range config.mplsCfg.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] }
        and other parameters as in 1.17. Expected: GT_BAD_PARAM.*/

        config.mplsCfg.tagEnable = GT_FALSE;

        config.mplsCfg.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, config.mplsCfg.tagEnable = %d, config.mplsCfg.vlanId = %d",
                                      devNum, config.mplsCfg.tagEnable, config.mplsCfg.vlanId);

        config.mplsCfg.vlanId = configMplsValidValues.mplsCfg.vlanId;


        /*1.20. Call with valid configPtr {mplsCfg.tagEnable [GT_FALSE], out of
        range mplsCfg.upMarkMode [0x5AAAAAA5] } and other
        parameters as in 1.17. Expected: GT_BAD_PARAM.*/

        config.mplsCfg.upMarkMode = TS_INVALID_ENUM_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, config.mplsCfg.tagEnable = %d, config.mplsCfg.upMarkMode = %d",
                                      devNum, config.mplsCfg.tagEnable, config.mplsCfg.upMarkMode);

        config.mplsCfg.upMarkMode = configMplsValidValues.mplsCfg.upMarkMode;

        config.mplsCfg.tagEnable = configMplsValidValues.mplsCfg.tagEnable; /* restore */

        /*1.21. Call with valid configPtr {mplsCfg.upMarkMode
        [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_INGRESS_PIPE_E], out of range
        mplsCfg.up [CPSS_USER_PRIORITY_RANGE_CNS = 8] } and other
        parameters same as in 1.17. Expected: NOT GT_OK.*/

        config.mplsCfg.upMarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_INGRESS_PIPE_E;
        config.mplsCfg.up = CPSS_USER_PRIORITY_RANGE_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, config.mplsCfg.upMarkMode = %d, config.mplsCfg.up = %d",
                                         devNum, config.mplsCfg.upMarkMode, config.mplsCfg.up);

        config.mplsCfg.up = configMplsValidValues.mplsCfg.up;
        config.mplsCfg.upMarkMode = configMplsValidValues.mplsCfg.upMarkMode; /* restore */

        /*1.22. Call with valid configPtr {mplsCfg.numLabels [1], out of range
        mplsCfg-> label1 [PRV_CPSS_MPLS_LABEL_RANGE = 1048576] }
        and other parameters same as in 1.17. Expected: NOT GT_OK.*/

        config.mplsCfg.numLabels = 2;
        config.mplsCfg.label2 = PRV_CPSS_MPLS_LABEL_RANGE;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, config.mplsCfg.numLabels = %d, config.mplsCfg.label2 = %d",
                                         devNum, config.mplsCfg.numLabels, config.mplsCfg.label2);

        config.mplsCfg.label2 = configMplsValidValues.mplsCfg.label2;
        config.mplsCfg.numLabels = configMplsValidValues.mplsCfg.numLabels; /* restore */

        /*1.23. Call with valid configPtr {mplsCfg.numLabels [1], out of range
        mplsCfg.exp1MarkMode [0x5AAAAAA5] } and other parameters
        same as in 1.17. Expected: GT_BAD_PARAM.*/

        config.mplsCfg.numLabels = 1;
        config.mplsCfg.exp1MarkMode = TS_INVALID_ENUM_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, config.mplsCfg.numLabels = %d, config.mplsCfg.exp1MarkMode = %d",
                                      devNum, config.mplsCfg.numLabels, config.mplsCfg.exp1MarkMode);

        config.mplsCfg.exp1MarkMode = configMplsValidValues.mplsCfg.exp1MarkMode;
        config.mplsCfg.numLabels = configMplsValidValues.mplsCfg.numLabels; /* restore */

        /*1.24. Call with valid configPtr {mplsCfg.numLabels [1], out of range
        mplsCfg.exp1 [CPSS_EXP_RANGE_CNS = 8] (not relevant)} and other
        parameters same as in 1.17. Expected: GT_OK.*/

        config.mplsCfg.numLabels = 2;
        config.mplsCfg.exp2 = CPSS_EXP_RANGE_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, config.mplsCfg.numLabels = %d, config.mplsCfg.exp2 = %d",
                                      devNum, config.mplsCfg.numLabels, config.mplsCfg.exp2);

        config.mplsCfg.exp2 = configMplsValidValues.mplsCfg.exp2;
        config.mplsCfg.numLabels = configMplsValidValues.mplsCfg.numLabels; /* restore */

        /*1.25. Call with valid configPtr {mplsCfg.exp2MarkMode
        [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_INGRESS_PIPE_E], out of range
        mplsCfg.exp2 [CPSS_EXP_RANGE_CNS = 8] } and other
        parameters same as in 1.17. Expected: NOT GT_OK.*/

        config.mplsCfg.exp2MarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_INGRESS_PIPE_E;
        config.mplsCfg.exp2 = CPSS_EXP_RANGE_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, config.mplsCfg.exp2MarkMode = %d, config.mplsCfg.exp2 = %d",
                                         devNum, config.mplsCfg.exp2MarkMode, config.mplsCfg.exp2);

        config.mplsCfg.exp2 = configMplsValidValues.mplsCfg.exp2;
        config.mplsCfg.exp2MarkMode = configMplsValidValues.mplsCfg.exp2MarkMode; /* restore */

        /*---- invalid/out of range mplsCfg*/

        /*1.26. Call with out of range config.mplsCfg.vlanId
        [PRV_CPSS_MAX_NUM_VLANS_CNS = 4096] and other parameters the same as in
        1.17. Expected: GT_BAD_PARAM.*/

        config.mplsCfg.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, config.mplsCfg.vlanId = %d", devNum, config.mplsCfg.vlanId);

        config.mplsCfg.vlanId = configMplsValidValues.mplsCfg.vlanId;


        /*1.27. Call with out of range config.mplsCfg.upMarkMode [0x5AAAAAA5] and other
        parameters the same as in 1.17. Expected: GT_BAD_PARAM.*/

        config.mplsCfg.upMarkMode = TS_INVALID_ENUM_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, config.mplsCfg.upMarkMode = %d", devNum, config.mplsCfg.upMarkMode);

        config.mplsCfg.upMarkMode = configMplsValidValues.mplsCfg.upMarkMode;

        /*1.28. Call with out of range config.mplsCfg.up [CPSS_USER_PRIORITY_RANGE_CNS
        = 8] and other parameters the same as in 1.17. Expected: non GT_OK.*/

        config.mplsCfg.up = CPSS_USER_PRIORITY_RANGE_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, config.mplsCfg.up = %d", devNum, config.mplsCfg.up);

        config.mplsCfg.up = configMplsValidValues.mplsCfg.up;

        /*1.29. Call with out of range config.mplsCfg.ttl [256], other params same as
        in 1.17. Expected: non GT_OK.*/

        config.mplsCfg.ttl = 256;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, config.mplsCfg.ttl = %d", devNum, config.mplsCfg.ttl);

        config.mplsCfg.ttl = configMplsValidValues.mplsCfg.ttl;

        /*1.30. Call with out of range config.mplsCfg.label1 [PRV_CPSS_MPLS_LABEL_RANGE
        =1048576] and other parameters the same as in 1.17. Expected: non GT_OK.*/

        config.mplsCfg.label1 = PRV_CPSS_MPLS_LABEL_RANGE;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, config.mplsCfg.label1 = %d", devNum, config.mplsCfg.label1);

        config.mplsCfg.label1 = configMplsValidValues.mplsCfg.label1;

        /*1.31. Call with out of range config.mplsCfg.label2 [PRV_CPSS_MPLS_LABEL_RANGE
        =1048576] and other parameters the same as in 1.17. Expected: non GT_OK.*/

        config.mplsCfg.label2 = PRV_CPSS_MPLS_LABEL_RANGE;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, config.mplsCfg.label2 = %d", devNum, config.mplsCfg.label2);

        config.mplsCfg.label2 = configMplsValidValues.mplsCfg.label2;

        /*1.32. Call with out of range config.mplsCfg.exp1MarkMode [0x5AAAAAA5] and
        other parameters the same as in 1.17. Expected: GT_BAD_PARAM.*/

        config.mplsCfg.exp1MarkMode = TS_INVALID_ENUM_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, config.mplsCfg.exp1MarkMode = %d", devNum, config.mplsCfg.exp1MarkMode);

        config.mplsCfg.exp1MarkMode = configMplsValidValues.mplsCfg.exp1MarkMode;

        /*1.33. Call with out of range config.mplsCfg.exp2MarkMode [0x5AAAAAA5] and
        other parameters the same as in 1.17. Expected: GT_BAD_PARAM.*/

        config.mplsCfg.exp2MarkMode = TS_INVALID_ENUM_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, config.mplsCfg.exp2MarkMode = %d", devNum, config.mplsCfg.exp2MarkMode);

        config.mplsCfg.exp2MarkMode = configMplsValidValues.mplsCfg.exp2MarkMode;

        /*1.34. Call with out of range config.mplsCfg.exp1 [CPSS_EXP_RANGE_CNS = 8] and
        other parameters the same as in 1.17. Expected: non GT_OK.*/

        config.mplsCfg.exp1 = CPSS_EXP_RANGE_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, config.mplsCfg.exp1 = %d", devNum, config.mplsCfg.exp1);

        config.mplsCfg.exp1 = configMplsValidValues.mplsCfg.exp1;

        /*1.35. Call with out of range config.mplsCfg.exp2 [CPSS_EXP_RANGE_CNS = 8] and
        other parameters the same as in 1.17. Expected: non GT_OK.*/

        config.mplsCfg.exp2 = CPSS_EXP_RANGE_CNS;

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, config.mplsCfg.exp2 = %d", devNum, config.mplsCfg.exp2);

        config.mplsCfg.exp2 = configMplsValidValues.mplsCfg.exp2;

        /*-----*/

        /*1.36. Call with configPtr [NULL] and other parameters the same as in
        1.1. Expected: GT_BAD_PTR.*/

        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configPtr = NULL", devNum);
    }

    /* set correct values*/
    routerArpTunnelStartLineIndex = 0;
    tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;
    /* copy ipv4cfg valid values to config */
    cpssOsMemCpy((GT_VOID*) &config, (GT_VOID*) &configIpv4ValidValues, sizeof(config));

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_FALSE))
    {
        st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, devNum);
    }

    /* 3. Call function with out of bound value for device id.*/
    devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, devNum);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTunnelStartEntryGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    OUT CPSS_TUNNEL_TYPE_ENT                *tunnelTypePtr,
    OUT CPSS_EXMXPM_TUNNEL_START_CONFIG_UNT *configPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTunnelStartEntryGet)
{
/*
ITERATE_DEVICES (ExMxPm)
1.1. Call with routerArpTunnelStartLineIndex [0 / 300 / TUNNEL_START_MAX_LINE_INDEX = 1023], non-null tunnelTypePtr and non-null configPtr. Expected: GT_OK.
1.2. Call with out of range routerArpTunnelStartLineIndex [TUNNEL_START_MAX_LINE_INDEX + 1 = 1024] and other parameters from 1.1. Expected: non GT_OK.
1.3. Call with tunnelTypePtr [NULL] and other parameters from 1.1. Expected: GT_BAD_PTR.
1.4. Call with configPtr [NULL] and other parameters from 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   devNum;
    GT_U32                  routerArpTunnelStartLineIndex;
    CPSS_TUNNEL_TYPE_ENT    tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;
    CPSS_EXMXPM_TUNNEL_START_ENTRY_UNT  config;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /*1.1. Call with routerArpTunnelStartLineIndex [0 / 300 /
        TUNNEL_START_MAX_LINE_INDEX = 1023], non-null tunnelTypePtr and non-null
        configPtr. Expected: GT_OK.*/
        tunnelType = CPSS_TUNNEL_X_OVER_MPLS_E;
        routerArpTunnelStartLineIndex = 0;

        st = cpssExMxPmTunnelStartEntryGet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, devNum, routerArpTunnelStartLineIndex);

        routerArpTunnelStartLineIndex = 300;
        tunnelType = CPSS_TUNNEL_X_OVER_GRE_IPV4_E;

        st = cpssExMxPmTunnelStartEntryGet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, devNum, routerArpTunnelStartLineIndex);

        routerArpTunnelStartLineIndex = TUNNEL_START_MAX_LINE_INDEX;
        tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;

        st = cpssExMxPmTunnelStartEntryGet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, devNum, routerArpTunnelStartLineIndex);

        /*1.2. Call with out of range routerArpTunnelStartLineIndex
        [TUNNEL_START_MAX_LINE_INDEX + 1 = 1024] and other parameters from 1.1.
        Expected: non GT_OK.*/

        routerArpTunnelStartLineIndex = TUNNEL_START_MAX_LINE_INDEX + 1;

        st = cpssExMxPmTunnelStartEntryGet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, routerArpTunnelStartLineIndex = %d",
                                          devNum, routerArpTunnelStartLineIndex);

        routerArpTunnelStartLineIndex = 0;

        /*1.3. Call with tunnelTypePtr [NULL] and other parameters from 1.1.
        Expected: NOT GT_OK.*/

        st = cpssExMxPmTunnelStartEntryGet(devNum, routerArpTunnelStartLineIndex, TS_INVALID_TUNNEL_ENUM_CNS, &config);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, tunnelType = %d", devNum, TS_INVALID_TUNNEL_ENUM_CNS);

        /*1.4. Call with configPtr [NULL] and other parameters from 1.1.
        Expected: GT_BAD_PTR.*/

        st = cpssExMxPmTunnelStartEntryGet(devNum, routerArpTunnelStartLineIndex, tunnelType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, configPtr = NULL", devNum);
    }

    /* set correct values*/
    routerArpTunnelStartLineIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_FALSE))
    {
        st = cpssExMxPmTunnelStartEntryGet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, devNum);
    }

    /* 3. Call function with out of bound value for device id.*/
    devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTunnelStartEntryGet(devNum, routerArpTunnelStartLineIndex, tunnelType, &config);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, devNum);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTunnelStartGreEtherTypeSet
(
    IN  GT_U8                                devNum,
    IN  CPSS_TUNNEL_GRE_ETHER_TYPE_ENT       greType,
    IN  GT_U32                               ethType
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTunnelStartGreEtherTypeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with greType [CPSS_TUNNEL_GRE0_ETHER_TYPE_E / CPSS_TUNNEL_GRE1_ETHER_TYPE_E]
                   ethType [0 / 0xFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTunnelStartGreEtherTypeGet with the same greType
                                                   and non-NULL ethTypePtr.
    Expected: GT_OK and the same ethType.
    1.3. Call with out of range greType [0x5AAAAAA5] and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range ethType [0x10000] and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       devNum;

    CPSS_TUNNEL_GRE_ETHER_TYPE_ENT  greType    = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;
    GT_U32                          ethType    = 0;
    GT_U32                          ethTypeGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /*
            1.1. Call with greType [CPSS_TUNNEL_GRE0_ETHER_TYPE_E /
                                    CPSS_TUNNEL_GRE1_ETHER_TYPE_E]
                           ethType [0 / 0xFFFF].
            Expected: GT_OK.
        */

        /* Call with greType [CPSS_TUNNEL_GRE0_ETHER_TYPE_E] */
        greType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;
        ethType = 0;

        st = cpssExMxPmTunnelStartGreEtherTypeSet(devNum, greType, ethType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, devNum, greType, ethType);

        /*
            1.2. Call cpssExMxPmTunnelStartGreEtherTypeGet with the same greType
                                                           and non-NULL ethTypePtr.
            Expected: GT_OK and the same ethType.
        */
        st = cpssExMxPmTunnelStartGreEtherTypeGet(devNum, greType, &ethTypeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmTunnelStartGreEtherTypeGet: %d, %d", devNum, greType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ethType, ethTypeGet,
                       "get another ethType than was set: %d", devNum);

        /* Call with greType [CPSS_TUNNEL_GRE1_ETHER_TYPE_E] */
        greType = CPSS_TUNNEL_GRE1_ETHER_TYPE_E;
        ethType = 0xFFFF;

        st = cpssExMxPmTunnelStartGreEtherTypeSet(devNum, greType, ethType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, devNum, greType, ethType);

        /*
            1.2. Call cpssExMxPmTunnelStartGreEtherTypeGet with the same greType
                                                           and non-NULL ethTypePtr.
            Expected: GT_OK and the same ethType.
        */
        st = cpssExMxPmTunnelStartGreEtherTypeGet(devNum, greType, &ethTypeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmTunnelStartGreEtherTypeGet: %d, %d", devNum, greType);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(ethType, ethTypeGet,
                       "get another ethType than was set: %d", devNum);

        /*
            1.3. Call with out of range greType [0x5AAAAAA5]
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        greType = TS_INVALID_ENUM_CNS;

        st = cpssExMxPmTunnelStartGreEtherTypeSet(devNum, greType, ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, devNum, greType);

        /*
            1.4. Call with out of range ethType [0x10000] and other params from 1.1.
            Expected: NOT GT_OK.
        */
        ethType = 0x10000;

        st = cpssExMxPmTunnelStartGreEtherTypeSet(devNum, greType, ethType);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ethType = %d", devNum, ethType);
    }

    /* set correct values*/
    greType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;
    ethType = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_FALSE))
    {
        st = cpssExMxPmTunnelStartGreEtherTypeSet(devNum, greType, ethType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, devNum);
    }

    /* 3. Call function with out of bound value for device id.*/
    devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTunnelStartGreEtherTypeSet(devNum, greType, ethType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, devNum);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTunnelStartGreEtherTypeGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_TUNNEL_GRE_ETHER_TYPE_ENT              greType,
    OUT GT_U32                                      *ethTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTunnelStartGreEtherTypeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with greType [CPSS_TUNNEL_GRE0_ETHER_TYPE_E / CPSS_TUNNEL_GRE1_ETHER_TYPE_E]
                   non-NULL ethTypePtr.
    Expected: GT_OK.
    1.2. Call with out of range greType [0x5AAAAAA5]
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with null ethTypePtr [NULL]
                   and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       devNum;

    CPSS_TUNNEL_GRE_ETHER_TYPE_ENT  greType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;
    GT_U32                          ethType = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /*
            1.1. Call with greType [CPSS_TUNNEL_GRE0_ETHER_TYPE_E /
                                    CPSS_TUNNEL_GRE1_ETHER_TYPE_E]
                           non-NULL ethTypePtr.
            Expected: GT_OK.
        */

        /* Call with greType [CPSS_TUNNEL_GRE0_ETHER_TYPE_E] */
        greType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;

        st = cpssExMxPmTunnelStartGreEtherTypeGet(devNum, greType, &ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, devNum, greType);

        /* Call with greType [CPSS_TUNNEL_GRE1_ETHER_TYPE_E] */
        greType = CPSS_TUNNEL_GRE1_ETHER_TYPE_E;

        st = cpssExMxPmTunnelStartGreEtherTypeGet(devNum, greType, &ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, devNum, greType);

        /*
            1.2. Call with out of range greType [0x5AAAAAA5]
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        greType = TS_INVALID_ENUM_CNS;

        st = cpssExMxPmTunnelStartGreEtherTypeGet(devNum, greType, &ethType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, devNum, greType);

        greType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;

        /*
            1.3. Call with null ethTypePtr [NULL]
                           and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTunnelStartGreEtherTypeGet(devNum, greType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ethTypePtr = NULL", devNum);
    }

    /* set correct values*/
    greType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_FALSE))
    {
        st = cpssExMxPmTunnelStartGreEtherTypeGet(devNum, greType, &ethType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, devNum);
    }

    /* 3. Call function with out of bound value for device id.*/
    devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTunnelStartGreEtherTypeGet(devNum, greType, &ethType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, devNum);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTunnelStartMimITagEtherTypeSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  iTagEthType
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTunnelStartMimITagEtherTypeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with iTagEthType [0 / 0xFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTunnelStartMimITagEtherTypeGet with non-NULL iTagEthTypePtr.
    Expected: GT_OK and the same iTagEthType.
    1.3. Call with iTagEthType [0x10000].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       devNum;

    GT_U32      iTagEthType    = 0;
    GT_U32      iTagEthTypeGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /*
            1.1. Call with iTagEthType [0 / 0xFFFF].
            Expected: GT_OK.
        */

        /* Call with iTagEthType [0] */
        iTagEthType = 0;

        st = cpssExMxPmTunnelStartMimITagEtherTypeSet(devNum, iTagEthType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, devNum, iTagEthType);

        /*
            1.2. Call cpssExMxPmTunnelStartMimITagEtherTypeGet with non-NULL iTagEthTypePtr.
            Expected: GT_OK and the same iTagEthType.
        */
        st = cpssExMxPmTunnelStartMimITagEtherTypeGet(devNum, &iTagEthTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmTunnelStartMimITagEtherTypeGet: %d", devNum);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(iTagEthType, iTagEthTypeGet,
                       "get another iTagEthType than was set: %d", devNum);

        /* Call with iTagEthType [0xFFFF] */
        iTagEthType = 0xFFFF;

        st = cpssExMxPmTunnelStartMimITagEtherTypeSet(devNum, iTagEthType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, devNum, iTagEthType);

        /*
            1.2. Call cpssExMxPmTunnelStartMimITagEtherTypeGet with non-NULL iTagEthTypePtr.
            Expected: GT_OK and the same iTagEthType.
        */
        st = cpssExMxPmTunnelStartMimITagEtherTypeGet(devNum, &iTagEthTypeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmTunnelStartMimITagEtherTypeGet: %d", devNum);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(iTagEthType, iTagEthTypeGet,
                       "get another iTagEthType than was set: %d", devNum);

        /*
            1.3. Call with iTagEthType [0x10000].
            Expected: NOT GT_OK.
        */
        iTagEthType = 0x10000;

        st = cpssExMxPmTunnelStartMimITagEtherTypeSet(devNum, iTagEthType);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, devNum, iTagEthType);
    }

    /* set correct values*/
    iTagEthType = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_FALSE))
    {
        st = cpssExMxPmTunnelStartMimITagEtherTypeSet(devNum, iTagEthType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, devNum);
    }

    /* 3. Call function with out of bound value for device id.*/
    devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTunnelStartMimITagEtherTypeSet(devNum, iTagEthType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, devNum);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTunnelStartMimITagEtherTypeGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *iTagEthTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTunnelStartMimITagEtherTypeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non NULL iTagEthTypePtr.
    Expected: GT_OK.
    1.2. Call with null iTagEthTypePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       devNum;

    GT_U32      iTagEthType = 0;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /*
            1.1. Call with non NULL iTagEthTypePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmTunnelStartMimITagEtherTypeGet(devNum, &iTagEthType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, devNum);

        /*
            1.2. Call with null iTagEthTypePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTunnelStartMimITagEtherTypeGet(devNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, iTagEthTypePtr = NULL", devNum);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_FALSE))
    {
        st = cpssExMxPmTunnelStartMimITagEtherTypeGet(devNum, &iTagEthType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, devNum);
    }

    /* 3. Call function with out of bound value for device id.*/
    devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTunnelStartMimITagEtherTypeGet(devNum, &iTagEthType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, devNum);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill tunnel start table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmTunnelStartFillTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in tunnel start table.
         Call cpssExMxPmTunnelStartEntrySet with routerArpTunnelStartLineIndex [0 .. numEntries-1], tunnelType
                                                    [CPSS_TUNNEL_X_OVER_IPV4_E], valid configPtr { ipv4Cfg { tagEnable
                                                    [GT_TRUE], vlanId [100], upMarkMode
                                                    [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_ENTRY_E], up [0], dscpMarkMode
                                                    [CPSS_EXMXPM_TUNNEL_START_QOS_MARK_FROM_ENTRY_E], dscp [0], macDa
                                                    [AA:BB:CC:00:00:11], dontFragmentFlag [GT_TRUE], ttl [0], autoTunnel
                                                    [GT_FALSE], autoTunnelOffset [0], ethType
                                                    [CPSS_EXMXPM_TUNNEL_START_IPV4_GRE0_E], destIp = {172.16.0.1}, srcIp =
                                                    {172.16.0.2}, retainCrc [GT_FALSE]}. 
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmTunnelStartEntrySet with routerArpTunnelStartLineIndex [numEntries]
    Expected: NOT GT_OK.
    1.4. Read all entries in tunnel start table and compare with original.
         Call cpssExMxPmTunnelStartEntryGet with routerArpTunnelStartLineIndex [0], 
                                                    non-NULL configPtr, other params same as in 1.2. 
    Expected: GT_OK and same tunnelType, config as written.
    1.6. Try to read entry with index out of range.
         Call cpssExMxPmTunnelStartEntryGet with routerArpTunnelStartLineIndex [numEntries].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL              isEqual    = GT_FALSE;
    GT_U32               numEntries = 0;
    GT_U32               iTemp      = 0;
    CPSS_TUNNEL_TYPE_ENT tunnelType = CPSS_TUNNEL_IPV4_OVER_IPV4_E;

    CPSS_EXMXPM_TUNNEL_START_ENTRY_UNT      config;
    CPSS_EXMXPM_TUNNEL_START_ENTRY_UNT      configRet;
    GT_ETHERADDR                            mac1 = {{0xAA, 0xBB, 0xCC, 0, 0, 0x11}};


    cpssOsBzero((GT_VOID*) &config, sizeof(config));
    cpssOsBzero((GT_VOID*) &configRet, sizeof(configRet));

    /* Fill the entry for tunnel start table */
    tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;
    
    config.ipv4Cfg.tagEnable = GT_TRUE;
    config.ipv4Cfg.vlanId = 100;
    config.ipv4Cfg.upMarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_ENTRY_E;
    config.ipv4Cfg.up = 0;
    config.ipv4Cfg.dscpMarkMode = CPSS_EXMXPM_TUNNEL_START_MARK_FROM_ENTRY_E;
    config.ipv4Cfg.dscp = 0;
    config.ipv4Cfg.macDa = mac1;  /* [AA:BB:CC:00:00:11] */
    config.ipv4Cfg.dontFragmentFlag = GT_TRUE;
    config.ipv4Cfg.ttl = 0;
    config.ipv4Cfg.autoTunnel = GT_FALSE;
    config.ipv4Cfg.autoTunnelOffset = 0;
    config.ipv4Cfg.ethType = CPSS_TUNNEL_GRE0_ETHER_TYPE_E;
    /* destIp = {172.16.0.1}; */
    config.ipv4Cfg.destIp.arIP[0] = 172;
    config.ipv4Cfg.destIp.arIP[1] = 16;
    config.ipv4Cfg.destIp.arIP[2] = 0;
    config.ipv4Cfg.destIp.arIP[3] = 1;
    /* srcIp = {172.16.0.2}; */
    config.ipv4Cfg.srcIp.arIP[0] = 172;
    config.ipv4Cfg.srcIp.arIP[1] = 16;
    config.ipv4Cfg.srcIp.arIP[2] = 0;
    config.ipv4Cfg.srcIp.arIP[3] = 2;
    config.ipv4Cfg.retainCrc = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        if ((PRV_CPSS_EXMXPM_PP_MAC(dev)->moduleCfg.bridgeCfg.fdbMemoryMode)
            == CPSS_EXMXPM_PP_CONFIG_FDB_TABLE_MODE_EXTERNAL_E )
        {
            numEntries = 16384;
        }
        else
        {
            numEntries = 4096;
        }

        /* 1.2. Fill all entries in tunnel start table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            config.ipv4Cfg.vlanId = (GT_U16)(iTemp % 4095);
            config.ipv4Cfg.ttl    = iTemp % 255;

            st = cpssExMxPmTunnelStartEntrySet(dev, iTemp, tunnelType, &config);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmTunnelStartEntrySet: %d, %d, %d", dev, iTemp, tunnelType);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmTunnelStartEntrySet(dev, numEntries, tunnelType, &config);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmTunnelStartEntrySet: %d, %d, %d", dev, numEntries, tunnelType);

        /* 1.4. Read all entries in tunnel start table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            config.ipv4Cfg.vlanId = (GT_U16)(iTemp % 4095);
            config.ipv4Cfg.ttl    = iTemp % 255;

            st = cpssExMxPmTunnelStartEntryGet(dev, iTemp, tunnelType, &configRet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTunnelStartEntryGet: %d, %d", dev, iTemp);
    
            if (GT_OK == st)
            {
                isEqual = (0 == cpssOsMemCmp((GT_VOID*)&config.ipv4Cfg, (GT_VOID*)&configRet.ipv4Cfg, 
                                             sizeof (config.ipv4Cfg))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                 "cpssExMxPmTunnelStartEntryGet: get another config.ipv4Cfg than was set: %d", dev);
            }
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmTunnelStartEntryGet(dev, numEntries, tunnelType, &configRet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTunnelStartEntryGet: %d, %d", dev, numEntries);
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmTunnelStart suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmTunnelStart)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTunnelStartEgressFilteringEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTunnelStartEgressFilteringEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTunnelStartEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTunnelStartEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTunnelStartGreEtherTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTunnelStartGreEtherTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTunnelStartMimITagEtherTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTunnelStartMimITagEtherTypeGet)
    /* Test for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTunnelStartFillTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmTunnelStart)

