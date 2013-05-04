/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
*  cpssExMxIpFlowUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxIpFlow.c, that provides 
*       CPSS EXMX IP Flow API.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/ipFlow/cpssExMxIpFlow.h>

#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */
#define IP_FLOW_OUT_OF_ENUM_CNS     0x5AAAAAA5 

/*----------------------------------------------------------------------------*/
/*
cpssExMxIpFlowEntrySet  
(
    IN GT_U8                                dev,
    IN CPSS_EXMX_IP_FLOW_ENTRY_TYPE_ENT     entryType,
    IN GT_U32                               entryIndex,
    IN GT_U16                               portProt,
    IN CPSS_EXMX_POLICY_ACTION_STC          *actionPtr 
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpFlowEntrySet)
{
/*
ITERATE_DEVICES (ExMx)
1.1.  Call function with entryType [CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E 
/ CPSS_EXMX_IP_FLOW_ENTRY_TYPE_UDP_E / CPSS_EXMX_IP_FLOW_ENTRY_TYPE_IP_E], 
entryIndex [0/ 2/ 7], portProt  [0/ 10/ 0xFFFF] and non-NULL actionPtr.
 [actionPtr.pktCmd [CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E / 
CPSS_EXMX_POLICY_ACTION_PKT_TRAP_TO_CPU_E / CPSS_EXMX_POLICY_ACTION_PKT_DROP_E; 
   actionPtr.mirror.mirrorToCpu [GT_TRUE/ GT_FALSE], actionPtr.mirror.mirrorToRxAnalyzerPort
    [GT_TRUE/ GT_FALSE]; 
actionPtr.qos. markCmd [CPSS_EXMX_POLICY_ACTION_MARK_NONE_E /
CPSS_EXMX_POLICY_ACTION_MARK_BY_ENTRY_E (correct only with 
actionPtr.policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E)
 /  CPSS_EXMX_POLICY_ACTION_MARK_BY_MAPPING_ENTRY_DSCP_E / 
 CPSS_EXMX_POLICY_ACTION_MARK_BY_MAPPING_PKT_DSCP_E], actionPtr.qos.markDscp
  [GT_TRUE / GT_FALSE],  actionPtr.qos.dscp [0/ 63], actionPtr.qos. markTc 
  [GT_TRUE / GT_FALSE], actionPtr.qos.tc [0/ 7], actionPtr.qos.markDp 
  [GT_TRUE / GT_FALSE], actionPtr.qos.dp [0/ 3] actionPtr.qos.markUp [GT_TRUE 
  / GT_FALSE], actionPtr.qos.up [0/ 7]; 
  actionPtr.redirect.redirectCmd [CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E /  
  CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_OUT_IF_E / CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_ROUTER_E / 
   CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_MPLS_E], actionPtr.redirect.redirectVid [100], 
   actionPtr.redirect.redirectData.outInterface.type [CPSS_INTERFACE_PORT_E = 0 /
    CPSS_INTERFACE_TRUNK_E / CPSS_INTERFACE_VIDX_E / CPSS_INTERFACE_VID_E], 
actionPtr.redirect.redirectData.outInterface.devPort.devNum [0],
actionPtr.redirect.redirectData.outInterface.devPort.portNum [0], 
   actionPtr.redirect.redirectData.outInterface.trunkId [0/ 31], 
   actionPtr.redirect.redirectData.outInterface. vidx [100] 
actionPtr.redirect.redirectData.outInterface.vlanId [100],
 actionPtr.redirect. redirectData.routeEntryPointer [50],
 actionPtr.redirect. redirectData.mplsNhlfePointer [50]; 
actionPtr.policer.policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E 
/ CPSS_EXMX_POLICY_ACTION_POLICER_METER_ONLY_E / CPSS_EXMX_POLICY_ACTION_POLICER_COUNTER_ONLY_E
 / CPSS_EXMX_POLICY_ACTION_POLICER_METER_AND_COUNTER_E],  actionPtr.policer.policerId
  [0/ maxNumOfPolicerEntries - 1]; actionPtr.fa.setLbi [GT_TRUE / GT_FALSE], 
  actionPtr.fa.lbi [0/ 2], actionPtr.fa.flowId [0];
actionPtr.vlan.modifyVlan [CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E / 
CPSS_EXMX_POLICY_ACTION_VLAN_CMD_MODIFY_E], actionPtr.vlan.nestedVlan [GT_TRUE / GT_FALSE],
 actionPtr.vlan.vlanId [100/ 4095] 
Expected: GT_OK.
1.2. Call cpssExMxIpFlowEntryGet with entryType, entryIndex - same as in 1.1. , 
non-NULL portProt  and non-NULL actionPtr. Expected: GT_OK and: 
For 98EX1x6 - all values same as written;
For others - only this values same as written: portProt, action [pktCmd; mirror. mirrorToCpu; 
qos.markCmd; policer].
1.3. Call function with out of range entryType [0x5AAAAAA5] and other parameters the same 
as in 1.1. Expected: GT_BAD_PARAM.
1.4. Call function with out of range entryIndex [8] and other parameters the same as in 1.1. 
Expected: non GT_OK.
1.5. Call function with out of range actionPtr.pktCmd [0x5AAAAAA5] and other parameters the 
same as in 1.1. Expected: GT_BAD_PARAM.
1.6. Call function with out of range actionPtr.qos. markCmd [0x5AAAAAA5] and other parameters 
the same as in 1.1. Expected: GT_BAD_PARAM.
1.7. Call function with out of range actionPtr.qos.dscp [64] and other parameters the same
 as in 1.1. Expected: NON GT_OK.
1.8. Call function with out of range actionPtr.qos.tc [8] and other parameters the same as
 in 1.1. Expected: NON GT_OK.
1.9. Call function with out of range actionPtr.qos.dp [4] and other parameters the same as
 in 1.1. Expected: NON GT_OK.
1.10. Call function with out of range actionPtr.qos.up [8] and other parameters the same 
as in 1.1. Expected: NON GT_OK.
1.11. Call function with out of range actionPtr.redirect.redirectCmd [0x5AAAAAA5] and 
other parameters the same as in 1.1. Expected: GT_BAD_PARAM for 98EX1x5 (Twist D) 
and 98EX1x6 (Tiger) and GT_OK for other devices.
1.12. Call function with out of range actionPtr.redirect.redirectVid [4096] and other
 parameters the same as in 1.1. Expected: NON GT_OK for 98EX1x5 (Twist D) and 
 98EX1x6 (Tiger) and GT_OK for other devices.
1.13. Call function with out of range actionPtr.redirect.redirectData.outInterface.type
 [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM for 
 98EX1x5 (Twist D) and 98EX1x6 (Tiger) and GT_OK for other devices.
1.14. Call function with out of range actionPtr.redirect.redirectData.outInterface.trunkId 
[0x1F] and other parameters the same as in 1.1. Expected: NON GT_OK  for 98EX1x5 
(Twist D) and 98EX1x6 (Tiger) and GT_OK for other devices.
1.15. Call function with out of range actionPtr.redirect.redirectData.outInterface.vidx
 [4096] and other parameters the same as in 1.1. Expected: NON GT_OK for 98EX1x5 (Twist D)
  and 98EX1x6 (Tiger) and GT_OK for other devices.
1.16. Call function with out of range actionPtr.redirect.redirectData.outInterface.vlanId 
[4096] and other parameters the same as in 1.1. Expected: NON GT_OK for 98EX1x5 
(Twist D) and 98EX1x6 (Tiger) and GT_OK for other devices.
1.17. Call function with out of range actionPtr.policer.policerMode [0x5AAAAAA5] and other 
parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.18. Call function with out of range actionPtr.policer.policerId [maxNumOfPolicerEntries] 
and other parameters the same as in 1.1. Expected: NON GT_OK 
1.19. Call function with out of range  actionPtr.fa.lbi [0x3] and other parameters the 
same as in 1.1. Expected: NON GT_OK for 98EX1x5 (Twist D) and 98EX1x6 (Tiger) and
 GT_OK for other devices.
1.20. Call function with out of range actionPtr.vlan.modifyVlan [0x5AAAAAA5] and other 
parameters the same as in 1.1. Expected: GT_BAD_PARAM for 98EX1x6 (Tiger) and
 GT_OK for other devices.
1.21. Call function with out of range actionPtr.vlan.vlanId [4096]  and other parameters 
the same as in 1.1. Expected: NON GT_OK 98EX1x6 (Tiger) and
 GT_OK for other devices.
1.22. Call function with actionPtr [NULL] and other parameters the same as in 1.1. 
Expected: GT_BAD_PTR.
1.23. Check that Policer and CoS Marking Command Mark by entry are mutual exclusive for 
98MX6x8, 98MX6x5, 98EX1x5 devices. Call with actionPtr [ actionPtr.qos. markCmd 
= CPSS_EXMX_POLICY_ACTION_MARK_BY_ENTRY_E;  actionPtr.policer.policerMode =   
CPSS_EXMX_POLICY_ACTION_POLICER_METER_ONLY_E]. Expected: non GT_OK for 98MX6x8, 98MX6x5, 
98EX1x5, and GT_OK for 98EX1x6.
*/
    GT_STATUS                           st = GT_OK;
    GT_U8                               dev;
    CPSS_EXMX_IP_FLOW_ENTRY_TYPE_ENT    entryType;
    GT_U32                              entryIndex;
    GT_U16                              portProt;
    GT_U16                              retPortProt;
    CPSS_EXMX_POLICY_ACTION_STC         action;
    CPSS_EXMX_POLICY_ACTION_STC         retAction;
    CPSS_PP_FAMILY_TYPE_ENT             family;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfDeviceFamilyGet(dev, &family);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /*1.1.  Call function with entryType [CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E  
        / CPSS_EXMX_IP_FLOW_ENTRY_TYPE_UDP_E / CPSS_EXMX_IP_FLOW_ENTRY_TYPE_IP_E], 
        entryIndex [0/ 2/ 7], portProt  [0/ 10/ 0xFFFF] and non-NULL actionPtr.
        [actionPtr.pktCmd [CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E / 
        CPSS_EXMX_POLICY_ACTION_PKT_TRAP_TO_CPU_E / CPSS_EXMX_POLICY_ACTION_PKT_DROP_E; 
        actionPtr.mirror.mirrorToCpu [GT_TRUE/ GT_FALSE], actionPtr.mirror.mirrorToRxAnalyzerPort
        [GT_TRUE/ GT_FALSE]; 
        actionPtr.qos.markCmd [CPSS_EXMX_POLICY_ACTION_MARK_NONE_E / 
        CPSS_EXMX_POLICY_ACTION_MARK_BY_ENTRY_E (correct only with 
        actionPtr.policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E)
        /  CPSS_EXMX_POLICY_ACTION_MARK_BY_MAPPING_ENTRY_DSCP_E / 
        CPSS_EXMX_POLICY_ACTION_MARK_BY_MAPPING_PKT_DSCP_E], actionPtr.qos.markDscp
        [GT_TRUE / GT_FALSE],  actionPtr.qos.dscp [0/ 63], actionPtr.qos.markTc 
        [GT_TRUE / GT_FALSE], actionPtr.qos.tc [0/ 7], actionPtr.qos.markDp 
        [GT_TRUE / GT_FALSE], actionPtr.qos.dp [0/ 3] actionPtr.qos.markUp [GT_TRUE 
        / GT_FALSE], actionPtr.qos.up [0/ 7]; 
        actionPtr.redirect.redirectCmd [CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E /  
        CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_OUT_IF_E / CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_ROUTER_E / 
        CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_MPLS_E], actionPtr.redirect.redirectVid [100], 
        actionPtr.redirect.redirectData.outInterface.type [CPSS_INTERFACE_PORT_E = 0 /
        CPSS_INTERFACE_TRUNK_E / CPSS_INTERFACE_VIDX_E / CPSS_INTERFACE_VID_E], 
        actionPtr.redirect.redirectData.outInterface.devPort.devNum [0],
        actionPtr.redirect.redirectData.outInterface.devPort.portNum [0], 
        actionPtr.redirect.redirectData.outInterface.trunkId [0/ 31], 
        actionPtr.redirect.redirectData.outInterface.vidx [100] 
        actionPtr.redirect.redirectData.outInterface.vlanId [100],
        actionPtr.redirect.redirectData.routeEntryPointer [50],
        actionPtr.redirect.redirectData.mplsNhlfePointer [50] ; 
        actionPtr.policer.policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E 
        / CPSS_EXMX_POLICY_ACTION_POLICER_METER_ONLY_E / CPSS_EXMX_POLICY_ACTION_POLICER_COUNTER_ONLY_E
        / CPSS_EXMX_POLICY_ACTION_POLICER_METER_AND_COUNTER_E],  actionPtr.policer.policerId
        [0/ maxNumOfPolicerEntries - 1]; actionPtr.fa.setLbi [GT_TRUE / GT_FALSE], 
        actionPtr.fa.lbi [0/ 2], actionPtr.fa.flowId [0];
        actionPtr.vlan.modifyVlan [CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E / 
        CPSS_EXMX_POLICY_ACTION_VLAN_CMD_MODIFY_E], actionPtr.vlan.nestedVlan [GT_TRUE / GT_FALSE],
        actionPtr.vlan.vlanId [100/ 4095]
        Expected: GT_OK. */
        /* 1.2. Call cpssExMxIpFlowEntryGet with entryType, entryIndex - same as in 1.1. , 
        non-NULL portProt  and non-NULL actionPtr. Expected: GT_OK and: 
        For 98EX1x6 - all values same as written;
        For others - only these values same as written: portProt, action [pktCmd; mirror. mirrorToCpu; 
        qos.markCmd; policer.policerMode]. */

        entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E;
        entryIndex = 0;
        portProt = 0;

        cpssOsBzero((GT_VOID*)&action, sizeof(action));
        action.pktCmd = CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E;
        action.mirror.mirrorToCpu = GT_TRUE;
        action.mirror.mirrorToRxAnalyzerPort = GT_TRUE;
        action.qos.markCmd = CPSS_EXMX_POLICY_ACTION_MARK_NONE_E; 
        action.redirect.redirectCmd = CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E; 
        action.redirect.redirectVid = 100;
        action.policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_METER_ONLY_E; 
        action.policer.policerId = 2;
        action.fa.setLbi = GT_TRUE;
        action.fa.lbi = 1;
        action.fa.flowId = 2;
        action.vlan.modifyVlan = CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E;
        action.vlan.nestedVlan = GT_TRUE;
        action.vlan.vlanId = 100;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);

        st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, &retAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxIpFlowEntryGet: %d, %d, %d",
                 dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(portProt, retPortProt,
            "got another portProt than was set: %d, %d, %d", dev, entryType, entryIndex);

        UTF_VERIFY_EQUAL3_STRING_MAC(action.pktCmd, retAction.pktCmd,
            "got another action.pktCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.mirror.mirrorToCpu, retAction.mirror.mirrorToCpu,
          "got another action.mirror.mirrorToCpu than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.qos.markCmd, retAction.qos.markCmd,
          "got another action.qos.markCmd than was set: %d, %d, %d", dev, entryType, entryIndex);

        UTF_VERIFY_EQUAL3_STRING_MAC(action.policer.policerMode, retAction.policer.policerMode,
          "got another action.policer.policerMode than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(action.policer.policerId, retAction.policer.policerId,
          "got another action.policer.policerId than was set: %d, %d, %d, policerMode = %d", 
                                     dev, entryType, entryIndex, action.policer.policerMode);

        if((CPSS_PP_FAMILY_TWISTD_E == family) || (CPSS_PP_FAMILY_TIGER_E == family))
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(action.fa.setLbi, retAction.fa.setLbi,
              "got another action.fa.setLbi than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL4_STRING_MAC(action.fa.lbi, retAction.fa.lbi,
              "got another action.fa.lbi than was set: %d, %d, %d, setLbi = %d", dev, entryType, entryIndex, action.fa.setLbi);

            UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.redirectCmd, retAction.redirect.redirectCmd,
              "got another action.redirect.redirectCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
        }
        if(CPSS_PP_FAMILY_TIGER_E == family)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(action.fa.flowId, retAction.fa.flowId,
              "got another action.fa.flowId than was set: %d, %d, %d", dev, entryType, entryIndex);

            UTF_VERIFY_EQUAL3_STRING_MAC(action.vlan.modifyVlan, retAction.vlan.modifyVlan,
              "got another action.vlan.modifyVlan than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.vlan.nestedVlan, retAction.vlan.nestedVlan,
              "got another action.vlan.nestedVlan than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL4_STRING_MAC(action.vlan.vlanId, retAction.vlan.vlanId,
              "got another action.vlan.vlanId than was set: %d, %d, %d, vlan.nestedVlan = %d", dev, entryType, entryIndex, action.vlan.nestedVlan);
        }

        entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_UDP_E;
        entryIndex = 2;
        portProt = 10;

        cpssOsBzero((GT_VOID*)&action, sizeof(action));
        action.pktCmd = CPSS_EXMX_POLICY_ACTION_PKT_TRAP_TO_CPU_E;
        action.mirror.mirrorToCpu = GT_FALSE;
        action.mirror.mirrorToRxAnalyzerPort = GT_FALSE;
        action.qos. markCmd = CPSS_EXMX_POLICY_ACTION_MARK_BY_ENTRY_E;
        action.qos.markDscp = GT_TRUE;
        action.qos.dscp = 1;
        action.qos.markTc = GT_TRUE;
        action.qos.tc = 2;
        action.qos.markDp = GT_TRUE;
        action.qos.dp = 3;
        action.qos.markUp = GT_TRUE;
        action.qos.up = 4;
        action.redirect.redirectCmd = CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_OUT_IF_E;
        action.redirect.redirectVid = 100;
        action.redirect.redirectData.outInterface.type = CPSS_INTERFACE_PORT_E;
        action.redirect.redirectData.outInterface.devPort.devNum = 1;
        action.redirect.redirectData.outInterface.devPort.portNum = 2;
        action.policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
        action.fa.setLbi = GT_FALSE;
        action.fa.flowId = 0;
        action.vlan.modifyVlan = CPSS_EXMX_POLICY_ACTION_VLAN_CMD_MODIFY_E;
        action.vlan.nestedVlan = GT_FALSE;
        action.vlan.vlanId = 4095;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);

        st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, &retAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxIpFlowEntryGet: %d, %d, %d",
                 dev, entryType, entryIndex);

        UTF_VERIFY_EQUAL3_STRING_MAC(portProt, retPortProt,
            "got another portProt than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.pktCmd, retAction.pktCmd,
            "got another action.pktCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.mirror.mirrorToCpu, retAction.mirror.mirrorToCpu,
          "got another action.mirror.mirrorToCpu than was set: %d, %d, %d", dev, entryType, entryIndex);

        UTF_VERIFY_EQUAL3_STRING_MAC(action.qos.markCmd, retAction.qos.markCmd,
          "got another action.qos.markCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(action.qos.dscp, retAction.qos.dscp,
          "got another action.qos.dscp than was set: %d, %d, %d, qos.markCmd = %d", dev, entryType, entryIndex, action.qos.markCmd);
        UTF_VERIFY_EQUAL4_STRING_MAC(action.qos.tc, retAction.qos.tc,
          "got another action.qos.tc than was set: %d, %d, %d, qos.markCmd = %d", dev, entryType, entryIndex, action.qos.markCmd);
        UTF_VERIFY_EQUAL4_STRING_MAC(action.qos.dp, retAction.qos.dp,
          "got another action.qos.dp than was set: %d, %d, %d, qos.markCmd = %d", dev, entryType, entryIndex, action.qos.markCmd);
        UTF_VERIFY_EQUAL4_STRING_MAC(action.qos.up, retAction.qos.up,
          "got another action.qos.up than was set: %d, %d, %d, qos.markCmd = %d", dev, entryType, entryIndex, action.qos.markCmd);

        UTF_VERIFY_EQUAL3_STRING_MAC(action.policer.policerMode, retAction.policer.policerMode,
          "got another action.policer.policerMode than was set: %d, %d, %d", dev, entryType, entryIndex);

        if((CPSS_PP_FAMILY_TWISTD_E == family) || (CPSS_PP_FAMILY_TIGER_E == family))
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(action.fa.setLbi, retAction.fa.setLbi,
              "got another action.fa.setLbi than was set: %d, %d, %d", dev, entryType, entryIndex);

            UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.redirectCmd, retAction.redirect.redirectCmd,
              "got another action.redirect.redirectCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.redirectVid, retAction.redirect.redirectVid,
              "got another action.redirect.redirectVid than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL4_STRING_MAC(action.redirect.redirectData.outInterface.type, retAction.redirect.redirectData.outInterface.type,
              "got another action.redirect.redirectData.outInterface.type than was set: %d, %d, %d, redirectCmd = %d",
                                         dev, entryType, entryIndex, action.redirect.redirectCmd);
            UTF_VERIFY_EQUAL5_STRING_MAC(action.redirect.redirectData.outInterface.devPort.devNum, retAction.redirect.redirectData.outInterface.devPort.devNum,
              "got another action.redirect.redirectData.outInterface.devPort.devNum than was set: %d, %d, %d, redirectCmd = %d, redirectData.outInterface.type = %d",
                                         dev, entryType, entryIndex, action.redirect.redirectCmd, action.redirect.redirectData.outInterface.type);
            UTF_VERIFY_EQUAL5_STRING_MAC(action.redirect.redirectData.outInterface.devPort.portNum, retAction.redirect.redirectData.outInterface.devPort.portNum,
              "got another action.redirect.redirectData.outInterface.devPort.portNum than was set: %d, %d, %d, redirectCmd = %d, redirectData.outInterface.type = %d",
                                         dev, entryType, entryIndex, action.redirect.redirectCmd, action.redirect.redirectData.outInterface.type);
        }
        if(CPSS_PP_FAMILY_TIGER_E == family)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(action.fa.flowId, retAction.fa.flowId,
              "got another action.fa.flowId than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.vlan.modifyVlan, retAction.vlan.modifyVlan,
              "got another action.vlan.modifyVlan than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.vlan.nestedVlan, retAction.vlan.nestedVlan,
              "got another action.vlan.nestedVlan than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL4_STRING_MAC(action.vlan.vlanId, retAction.vlan.vlanId,
              "got another action.vlan.vlanId than was set: %d, %d, %d, vlan.modifyVlan = %d", dev, entryType, entryIndex, action.vlan.modifyVlan);
        }

        entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_IP_E;
        entryIndex = 7;
        portProt = 0xFFFF;

        cpssOsBzero((GT_VOID*)&action, sizeof(action));
        action.pktCmd = CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E;
        action.mirror.mirrorToCpu = GT_TRUE;
        action.mirror.mirrorToRxAnalyzerPort = GT_FALSE;
        action.qos.markCmd = CPSS_EXMX_POLICY_ACTION_MARK_BY_MAPPING_ENTRY_DSCP_E;
        action.qos.markDscp = GT_TRUE;
        action.qos.dscp = 63;
        action.qos.markTc = GT_FALSE;
        action.qos.markDp = GT_FALSE;
        action.qos.markUp = GT_FALSE;
        action.redirect.redirectCmd = CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_ROUTER_E;
        action.redirect.redirectVid = 500;
        action.redirect.redirectData.routeEntryPointer = 50;
        action.policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
        action.fa.setLbi = GT_FALSE;
        action.fa.flowId = 0;
        action.vlan.modifyVlan = CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E;
        action.vlan.nestedVlan = GT_FALSE;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);

        st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, &retAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxIpFlowEntryGet: %d, %d, %d",
                 dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(portProt, retPortProt,
            "got another portProt than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.pktCmd, retAction.pktCmd,
            "got another action.pktCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.mirror.mirrorToCpu, retAction.mirror.mirrorToCpu,
          "got another action.mirror.mirrorToCpu than was set: %d, %d, %d", dev, entryType, entryIndex);

        UTF_VERIFY_EQUAL3_STRING_MAC(action.qos.markCmd, retAction.qos.markCmd,
          "got another action.qos.markCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(action.qos.dscp, retAction.qos.dscp,
          "got another action.qos.dscp than was set: %d, %d, %d, qos.markCmd = %d", dev, entryType, entryIndex, action.qos.markCmd);

        UTF_VERIFY_EQUAL3_STRING_MAC(action.policer.policerMode, retAction.policer.policerMode,
          "got another action.policer.policerMode than was set: %d, %d, %d", dev, entryType, entryIndex);

        if((CPSS_PP_FAMILY_TWISTD_E == family) || (CPSS_PP_FAMILY_TIGER_E == family))
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(action.fa.setLbi, retAction.fa.setLbi,
              "got another action.fa.setLbi than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.redirectCmd, retAction.redirect.redirectCmd,
              "got another action.redirect.redirectCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.redirectVid, retAction.redirect.redirectVid,
              "got another action.redirect.redirectVid than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL4_STRING_MAC(action.redirect.redirectData.routeEntryPointer, retAction.redirect.redirectData.routeEntryPointer,
              "got another action.redirect.redirectData.routeEntryPointer than was set: %d, %d, %d, redirectCmd = %d",
                                         dev, entryType, entryIndex, action.redirect.redirectCmd);
        }
        if(CPSS_PP_FAMILY_TIGER_E == family)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(action.fa.flowId, retAction.fa.flowId,
              "got another action.fa.flowId than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.vlan.modifyVlan, retAction.vlan.modifyVlan,
              "got another action.vlan.modifyVlan than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.vlan.nestedVlan, retAction.vlan.nestedVlan,
              "got another action.vlan.nestedVlan than was set: %d, %d, %d", dev, entryType, entryIndex);
        }

        entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E;
        entryIndex = 7;
        portProt = 15;

        cpssOsBzero((GT_VOID*)&action, sizeof(action));
        action.pktCmd = CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E;
        action.mirror.mirrorToCpu = GT_FALSE;
        action.mirror.mirrorToRxAnalyzerPort = GT_TRUE;
        action.qos.markCmd = CPSS_EXMX_POLICY_ACTION_MARK_BY_MAPPING_PKT_DSCP_E;
        action.qos.markDscp = GT_FALSE;
        action.qos.markTc = GT_FALSE;
        action.qos.markDp = GT_FALSE;
        action.qos.markUp = GT_FALSE;
        action.redirect.redirectCmd = CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_MPLS_E;
        action.redirect.redirectVid = 256;
        action.redirect.redirectData.mplsNhlfePointer = 50;
        action.policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
        action.fa.setLbi = GT_FALSE;
        action.fa.flowId = 0;
        action.vlan.modifyVlan = CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E;
        action.vlan.nestedVlan = GT_FALSE;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        if(CPSS_PP_FAMILY_TIGER_E == family)
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);

            st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, &retAction);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxIpFlowEntryGet: %d, %d, %d",
                     dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(portProt, retPortProt,
                "got another portProt than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.pktCmd, retAction.pktCmd,
                "got another action.pktCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.mirror.mirrorToCpu, retAction.mirror.mirrorToCpu,
              "got another action.mirror.mirrorToCpu than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.qos.markCmd, retAction.qos.markCmd,
              "got another action.qos.markCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.policer.policerMode, retAction.policer.policerMode,
              "got another action.policer.policerMode than was set: %d, %d, %d", dev, entryType, entryIndex);
            
            UTF_VERIFY_EQUAL3_STRING_MAC(action.fa.flowId, retAction.fa.flowId,
              "got another action.fa.flowId than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.vlan.modifyVlan, retAction.vlan.modifyVlan,
              "got another action.vlan.modifyVlan than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.vlan.nestedVlan, retAction.vlan.nestedVlan,
              "got another action.vlan.nestedVlan than was set: %d, %d, %d", dev, entryType, entryIndex);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);
        }

        entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E;
        entryIndex = 7;
        portProt = 15;

        cpssOsBzero((GT_VOID*)&action, sizeof(action));
        action.pktCmd = CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E;
        action.mirror.mirrorToCpu = GT_FALSE;
        action.mirror.mirrorToRxAnalyzerPort = GT_TRUE;
        action.qos.markCmd = CPSS_EXMX_POLICY_ACTION_MARK_NONE_E;
        action.qos.markDscp = GT_FALSE;
        action.qos.markTc = GT_FALSE;
        action.qos.markDp = GT_FALSE;
        action.qos.markUp = GT_FALSE;
        action.redirect.redirectCmd = CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_OUT_IF_E;
        action.redirect.redirectVid = 100;
        action.redirect.redirectData.outInterface.type = CPSS_INTERFACE_TRUNK_E;
        action.redirect.redirectData.outInterface.trunkId = 10;
        action.policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_COUNTER_ONLY_E;
        prvUtfExMxMaxNumOfPolicerEntriesGet(dev, &(action.policer.policerId));
        action.policer.policerId = action.policer.policerId - 1;
        action.fa.setLbi = GT_FALSE;
        action.fa.flowId = 0;
        action.vlan.modifyVlan = CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E;
        action.vlan.nestedVlan = GT_FALSE;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);

        st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, &retAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxIpFlowEntryGet: %d, %d, %d",
                 dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(portProt, retPortProt,
            "got another portProt than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.pktCmd, retAction.pktCmd,
            "got another action.pktCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.mirror.mirrorToCpu, retAction.mirror.mirrorToCpu,
          "got another action.mirror.mirrorToCpu than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.qos.markCmd, retAction.qos.markCmd,
          "got another action.qos.markCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.policer.policerMode, retAction.policer.policerMode,
          "got another action.policer.policerMode than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(action.policer.policerId, retAction.policer.policerId,
          "got another action.policer.policerId than was set: %d, %d, %d, policerMode = %d", 
                                     dev, entryType, entryIndex, action.policer.policerMode);

        if((CPSS_PP_FAMILY_TWISTD_E == family) || (CPSS_PP_FAMILY_TIGER_E == family))
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(action.fa.setLbi, retAction.fa.setLbi,
              "got another action.fa.setLbi than was set: %d, %d, %d", dev, entryType, entryIndex);

            UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.redirectCmd, retAction.redirect.redirectCmd,
              "got another action.redirect.redirectCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.redirectVid, retAction.redirect.redirectVid,
              "got another action.redirect.redirectVid than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL4_STRING_MAC(action.redirect.redirectData.outInterface.type, retAction.redirect.redirectData.outInterface.type,
              "got another action.redirect.redirectData.outInterface.type than was set: %d, %d, %d, redirectCmd = %d",
                                         dev, entryType, entryIndex, action.redirect.redirectCmd);
            UTF_VERIFY_EQUAL5_STRING_MAC(action.redirect.redirectData.outInterface.trunkId, retAction.redirect.redirectData.outInterface.trunkId,
              "got another action.redirect.redirectData.outInterface.trunkId than was set: %d, %d, %d, redirectCmd = %d, redirectData.outInterface.type = %d",
                                         dev, entryType, entryIndex, action.redirect.redirectCmd, action.redirect.redirectData.outInterface.type);
        }
        if(CPSS_PP_FAMILY_TIGER_E == family)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(action.fa.flowId, retAction.fa.flowId,
              "got another action.fa.flowId than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.vlan.modifyVlan, retAction.vlan.modifyVlan,
              "got another action.vlan.modifyVlan than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.vlan.nestedVlan, retAction.vlan.nestedVlan,
              "got another action.vlan.nestedVlan than was set: %d, %d, %d", dev, entryType, entryIndex);
        }

        entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E;
        entryIndex = 7;
        portProt = 15;

        cpssOsBzero((GT_VOID*)&action, sizeof(action));
        action.pktCmd = CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E;
        action.mirror.mirrorToCpu = GT_FALSE;
        action.mirror.mirrorToRxAnalyzerPort = GT_TRUE;
        action.qos.markCmd = CPSS_EXMX_POLICY_ACTION_MARK_NONE_E;
        action.qos.markDscp = GT_FALSE;
        action.qos.markTc = GT_FALSE;
        action.qos.markDp = GT_FALSE;
        action.qos.markUp = GT_FALSE;
        action.redirect.redirectCmd = CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_OUT_IF_E;
        action.redirect.redirectVid = 100;
        action.redirect.redirectData.outInterface.type = CPSS_INTERFACE_VIDX_E;
        action.redirect.redirectData.outInterface.vidx = 100;
        action.policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_METER_AND_COUNTER_E;
        action.policer.policerId = 20;
        action.fa.setLbi = GT_FALSE;
        action.fa.flowId = 0;
        action.vlan.modifyVlan = CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E;
        action.vlan.nestedVlan = GT_FALSE;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);

        st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, &retAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxIpFlowEntryGet: %d, %d, %d",
                 dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(portProt, retPortProt,
            "got another portProt than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.pktCmd, retAction.pktCmd,
            "got another action.pktCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.mirror.mirrorToCpu, retAction.mirror.mirrorToCpu,
          "got another action.mirror.mirrorToCpu than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.qos.markCmd, retAction.qos.markCmd,
          "got another action.qos.markCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.policer.policerMode, retAction.policer.policerMode,
          "got another action.policer.policerMode than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL4_STRING_MAC(action.policer.policerId, retAction.policer.policerId,
          "got another action.policer.policerId than was set: %d, %d, %d, policerMode = %d", 
                                     dev, entryType, entryIndex, action.policer.policerMode);

        if((CPSS_PP_FAMILY_TWISTD_E == family) || (CPSS_PP_FAMILY_TIGER_E == family))
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(action.fa.setLbi, retAction.fa.setLbi,
              "got another action.fa.setLbi than was set: %d, %d, %d", dev, entryType, entryIndex);

            UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.redirectCmd, retAction.redirect.redirectCmd,
              "got another action.redirect.redirectCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.redirectVid, retAction.redirect.redirectVid,
              "got another action.redirect.redirectVid than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL4_STRING_MAC(action.redirect.redirectData.outInterface.type, retAction.redirect.redirectData.outInterface.type,
              "got another action.redirect.redirectData.outInterface.type than was set: %d, %d, %d, redirectCmd = %d",
                                         dev, entryType, entryIndex, action.redirect.redirectCmd);
            UTF_VERIFY_EQUAL5_STRING_MAC(action.redirect.redirectData.outInterface.vidx, retAction.redirect.redirectData.outInterface.vidx,
              "got another action.redirect.redirectData.outInterface.vidx than was set: %d, %d, %d, redirectCmd = %d, redirectData.outInterface.type = %d",
                                         dev, entryType, entryIndex, action.redirect.redirectCmd, action.redirect.redirectData.outInterface.type);
        }
        if(CPSS_PP_FAMILY_TIGER_E == family)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(action.fa.flowId, retAction.fa.flowId,
              "got another action.fa.flowId than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.vlan.modifyVlan, retAction.vlan.modifyVlan,
              "got another action.vlan.modifyVlan than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.vlan.nestedVlan, retAction.vlan.nestedVlan,
              "got another action.vlan.nestedVlan than was set: %d, %d, %d", dev, entryType, entryIndex);
        }

        entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E;
        entryIndex = 7;
        portProt = 15;

        cpssOsBzero((GT_VOID*)&action, sizeof(action));
        action.pktCmd = CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E;
        action.mirror.mirrorToCpu = GT_FALSE;
        action.mirror.mirrorToRxAnalyzerPort = GT_TRUE;
        action.qos.markCmd = CPSS_EXMX_POLICY_ACTION_MARK_NONE_E;
        action.qos.markDscp = GT_FALSE;
        action.qos.markTc = GT_FALSE;
        action.qos.markDp = GT_FALSE;
        action.qos.markUp = GT_FALSE;
        action.redirect.redirectCmd = CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_OUT_IF_E;
        action.redirect.redirectVid = 100;
        action.redirect.redirectData.outInterface.type = CPSS_INTERFACE_VID_E;
        action.redirect.redirectData.outInterface.vlanId = 100;
        action.policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
        action.fa.setLbi = GT_FALSE;
        action.fa.flowId = 0;
        action.vlan.modifyVlan = CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E;
        action.vlan.nestedVlan = GT_FALSE;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);

        st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, &retAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxIpFlowEntryGet: %d, %d, %d",
                 dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(portProt, retPortProt,
            "got another portProt than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.pktCmd, retAction.pktCmd,
            "got another action.pktCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.mirror.mirrorToCpu, retAction.mirror.mirrorToCpu,
          "got another action.mirror.mirrorToCpu than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.qos.markCmd, retAction.qos.markCmd,
          "got another action.qos.markCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(action.policer.policerMode, retAction.policer.policerMode,
          "got another action.policer.policerMode than was set: %d, %d, %d", dev, entryType, entryIndex);
        if((CPSS_PP_FAMILY_TWISTD_E == family) || (CPSS_PP_FAMILY_TIGER_E == family))
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(action.fa.setLbi, retAction.fa.setLbi,
              "got another action.fa.setLbi than was set: %d, %d, %d", dev, entryType, entryIndex);

            UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.redirectCmd, retAction.redirect.redirectCmd,
              "got another action.redirect.redirectCmd than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.redirect.redirectVid, retAction.redirect.redirectVid,
              "got another action.redirect.redirectVid than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL4_STRING_MAC(action.redirect.redirectData.outInterface.type, retAction.redirect.redirectData.outInterface.type,
              "got another action.redirect.redirectData.outInterface.type than was set: %d, %d, %d, redirectCmd = %d",
                                         dev, entryType, entryIndex, action.redirect.redirectCmd);
            UTF_VERIFY_EQUAL5_STRING_MAC(action.redirect.redirectData.outInterface.vlanId, retAction.redirect.redirectData.outInterface.vlanId,
              "got another action.redirect.redirectData.outInterface.vlanId than was set: %d, %d, %d, redirectCmd = %d, redirectData.outInterface.type = %d",
                                         dev, entryType, entryIndex, action.redirect.redirectCmd, action.redirect.redirectData.outInterface.type);
        }
        if(CPSS_PP_FAMILY_TIGER_E == family)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(action.fa.flowId, retAction.fa.flowId,
              "got another action.fa.flowId than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.vlan.modifyVlan, retAction.vlan.modifyVlan,
              "got another action.vlan.modifyVlan than was set: %d, %d, %d", dev, entryType, entryIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(action.vlan.nestedVlan, retAction.vlan.nestedVlan,
              "got another action.vlan.nestedVlan than was set: %d, %d, %d", dev, entryType, entryIndex);
        }

        /* 1.3. Call function with out of range entryType [0x5AAAAAA5] and other parameters the same 
        as in 1.1. Expected: GT_BAD_PARAM. */
        entryType = IP_FLOW_OUT_OF_ENUM_CNS;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, entryType, entryIndex, portProt);

        /* restore */
        entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E; 

        /* 1.4. Call function with out of range entryIndex [8] and other parameters the same as in 1.1. 
        Expected: non GT_OK. */
        entryIndex = 8;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);

        /* restore */
        entryIndex = 0;

        /* 1.5. Call function with out of range actionPtr.pktCmd [0x5AAAAAA5] and other parameters the 
        same as in 1.1. Expected: GT_BAD_PARAM. */
        action.pktCmd = IP_FLOW_OUT_OF_ENUM_CNS;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, %d, %d, actionPtr->pktCmd=%d",
                                     dev, entryType, entryIndex, portProt, action.pktCmd);

        /* restore */
        action.pktCmd = CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E;

        /* 1.6. Call function with out of range actionPtr.qos.markCmd [0x5AAAAAA5] and other parameters 
        the same as in 1.1. Expected: GT_BAD_PARAM. */
        action.qos.markCmd = IP_FLOW_OUT_OF_ENUM_CNS;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, %d, %d, actionPtr->qos.markCmd=%d",
                                     dev, entryType, entryIndex, portProt, action.qos.markCmd);

        /* restore */
        action.qos.markCmd = CPSS_EXMX_POLICY_ACTION_MARK_BY_ENTRY_E;

        /* 1.7. Call function with out of range actionPtr.qos.dscp [64] and other parameters the same
        as in 1.1. Expected: NON GT_OK  */
        action.qos.dscp = CPSS_DSCP_RANGE_CNS;
        action.qos.markDscp = GT_TRUE;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_NOT_EQUAL7_STRING_MAC(GT_OK, st,"%d, %d, %d, %d, actionPtr->qos.[dscp=%d, markCmd = %d, markDscp=%d]",
            dev, entryType, entryIndex, portProt, action.qos.dscp, action.qos.markCmd, action.qos.markDscp);

        /* restore */
        action.qos.dscp = 0;
        action.qos.markDscp = GT_FALSE;

        /* 1.8. Call function with out of range actionPtr.qos.tc [8] and other parameters the same as
        in 1.1. Expected: NON GT_OK */

        action.qos.tc = CPSS_TC_RANGE_CNS;
        action.qos.markTc = GT_TRUE;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_NOT_EQUAL7_STRING_MAC(GT_OK, st, "%d, %d, %d, %d, actionPtr->qos[tc=%d, markCmd = %d, markTc=%d]",
            dev, entryType, entryIndex, portProt, action.qos.tc, action.qos.markCmd, action.qos. markTc);

        /* restore */
        action.qos.tc = 0;
        action.qos. markTc = GT_FALSE;

        /* 1.9. Call function with out of range actionPtr.qos.dp [CPSS_DP_RANGE_CNS + 1= 4] and other parameters the same as
        in 1.1. Expected: NON GT_OK  */
        action.qos.dp = CPSS_DP_RANGE_CNS + 1;
        action.qos.markDp = GT_TRUE;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_NOT_EQUAL7_STRING_MAC(GT_OK, st, "%d, %d, %d, %d, actionPtr->qos[dp=%d, markCmd = %d, markDp=%d]",
            dev, entryType, entryIndex, portProt, action.qos.dp, action.qos.markCmd, action.qos.markDp);

        /* restore */
        action.qos.dp = 0;
        action.qos.markDp = GT_FALSE;

        /* 1.10. Call function with out of range actionPtr.qos.up [8] and other parameters the same 
        as in 1.1. Expected: NON GT_OK */
        action.qos.up = CPSS_USER_PRIORITY_RANGE_CNS;
        action.qos.markUp = GT_TRUE;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_NOT_EQUAL7_STRING_MAC(GT_OK, st, "%d, %d, %d, %d, actionPtr->qos [up = %d, markCmd = %d, markUp = %d]",
            dev, entryType, entryIndex, portProt, action.qos.up, action.qos.markCmd, action.qos.markUp);

        /* restore */
        action.qos.up = 0;
        action.qos.markUp = GT_FALSE;

        /* 1.11. Call function with out of range actionPtr.redirect.redirectCmd [0x5AAAAAA5] and 
        other parameters the same as in 1.1. Expected: GT_BAD_PARAM for 98EX1x5 (Twist D) 
        and 98EX1x6 (Tiger) and GT_OK for other devices. */
        action.redirect.redirectCmd = IP_FLOW_OUT_OF_ENUM_CNS;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);

        if ((CPSS_PP_FAMILY_TWISTD_E == family) || (CPSS_PP_FAMILY_TIGER_E == family))
        {
            UTF_VERIFY_EQUAL5_STRING_MAC(GT_BAD_PARAM, st, 
            "Tiger or TwistD device: %d, %d, %d, %d, actionPtr->redirect.redirectCmd=%d",
            dev, entryType, entryIndex, portProt, action.redirect.redirectCmd);
        }
        else
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);
        }

        /* restore */
        action.redirect.redirectCmd = CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_OUT_IF_E;

        /* 1.12. Call function with out of range actionPtr.redirect.redirectVid [4096] and other
        parameters the same as in 1.1. Expected: NON GT_OK for 98EX1x5 (Twist D) and 
        98EX1x6 (Tiger) and GT_OK for other devices. */
        action.redirect.redirectVid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);

        if ((CPSS_PP_FAMILY_TWISTD_E == family) || (CPSS_PP_FAMILY_TIGER_E == family))
        {
            UTF_VERIFY_NOT_EQUAL6_STRING_MAC(GT_OK, st, 
            "Tiger or TwistD device: %d, %d, %d, %d, actionPtr->redirect.redirectVid=%d, actionPtr->redirect.redirectCmd=%d", 
            dev, entryType, entryIndex, portProt, action.redirect.redirectVid, action.redirect.redirectCmd);
        }
        else
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);
        }

        /* restore */
        action.redirect.redirectVid = 100;

        /* 1.13. Call function with out of range actionPtr.redirect.redirectData.outInterface.type
        [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM for 
        98EX1x5 (Twist D) and 98EX1x6 (Tiger) and GT_OK for other devices. */
        action.redirect.redirectData.outInterface.type = IP_FLOW_OUT_OF_ENUM_CNS;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);

        if ((CPSS_PP_FAMILY_TWISTD_E == family) || (CPSS_PP_FAMILY_TIGER_E == family))
        {
            UTF_VERIFY_EQUAL6_STRING_MAC(GT_BAD_PARAM, st,
            "Tiger or TwistD device: %d, %d, %d, %d, actionPtr->redirect.redirectData.outInterface.type=%d, actionPtr->redirect.redirectCmd=%d",
            dev, entryType, entryIndex, portProt, action.redirect.redirectData.outInterface.type, action.redirect.redirectCmd);
        }
        else
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);
        }

        /* restore */

        /* 1.14. Call function with out of range actionPtr.redirect.redirectData.outInterface.trunkId 
        [32] and other parameters the same as in 1.1. Expected: NON GT_OK  for 98EX1x5 
        (Twist D) and 98EX1x6 (Tiger) and GT_OK for other devices. */
        action.redirect.redirectData.outInterface.type = CPSS_INTERFACE_TRUNK_E;
        action.redirect.redirectData.outInterface.trunkId = 32;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);

        if (CPSS_PP_FAMILY_TWISTD_E == family || CPSS_PP_FAMILY_TIGER_E == family) 
        {
            UTF_VERIFY_NOT_EQUAL6_STRING_MAC(GT_OK, st, 
            "Tiger or TwistD device: %d, %d, %d, %d, actionPtr->redirect.redirectData.outInterface.trunkId=%d, actionPtr->redirect.redirectCmd=%d",
            dev, entryType, entryIndex, portProt, action.redirect.redirectData.outInterface.trunkId, action.redirect.redirectCmd);
        }
        else
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);
        }

        /* restore */
        action.redirect.redirectData.outInterface.trunkId = 0;

        /* 1.15. Call function with out of range actionPtr.redirect.redirectData.outInterface.vidx
        [4096] and other parameters the same as in 1.1. Expected: NON GT_OK for 98EX1x5 (Twist D)
        and 98EX1x6 (Tiger) and GT_OK for other devices. */
        action.redirect.redirectData.outInterface.vidx = PRV_CPSS_MAX_NUM_VLANS_CNS;
        action.redirect.redirectData.outInterface.type = CPSS_INTERFACE_VIDX_E;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);

        if ((CPSS_PP_FAMILY_TWISTD_E == family) || (CPSS_PP_FAMILY_TIGER_E == family))
        {
            UTF_VERIFY_NOT_EQUAL6_STRING_MAC(GT_OK, st, 
            "Tiger or TwistD device: %d, %d, %d, %d, actionPtr->redirect.redirectData.outInterface.vidx=%d, actionPtr->redirect.redirectCmd=%d",
            dev, entryType, entryIndex, portProt, action.redirect.redirectData.outInterface.vidx, action.redirect.redirectCmd);
        }
        else
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);
        }

        /* restore */
        action.redirect.redirectData.outInterface.vidx = 100;

        /* 1.16. Call function with out of range actionPtr.redirect.redirectData.outInterface.vlanId 
        [4096] and other parameters the same as in 1.1. Expected: NON GT_OK for 98EX1x5 
        (Twist D) and 98EX1x6 (Tiger) and GT_OK for other devices. */
        action.redirect.redirectData.outInterface.type = CPSS_INTERFACE_VID_E;
        action.redirect.redirectData.outInterface.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);

        if (CPSS_PP_FAMILY_TWISTD_E == family || CPSS_PP_FAMILY_TIGER_E == family) 
        {
            UTF_VERIFY_NOT_EQUAL6_STRING_MAC(GT_OK, st, 
            "Tiger or TwistD device: %d, %d, %d, %d, actionPtr->redirect.redirectData.outInterface.vlanId=%d, actionPtr->redirect.redirectCmd=%d",
            dev, entryType, entryIndex, portProt, action.redirect.redirectData.outInterface.vlanId, action.redirect.redirectCmd);
        }
        else
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);
        }

        /* restore */
        action.redirect.redirectData.outInterface.vlanId = 100;

        /* 1.17. Call function with out of range actionPtr.policer.policerMode [0x5AAAAAA5] and other 
        parameters the same as in 1.1. Expected: GT_BAD_PARAM. */
        action.policer.policerMode = IP_FLOW_OUT_OF_ENUM_CNS;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_BAD_PARAM, st, 
        "%d, %d, %d, %d, actionPtr->policer.policerMode=%d",
         dev, entryType, entryIndex, portProt, action.policer.policerMode);

        /* restore */
        action.qos.markCmd = CPSS_EXMX_POLICY_ACTION_MARK_NONE_E;
        action.policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_METER_ONLY_E;

        /* 1.18. Call function with out of range actionPtr.policer.policerId [maxNumOfPolicerEntries] 
        and other parameters the same as in 1.1. Expected: NON GT_OK  */
        prvUtfExMxMaxNumOfPolicerEntriesGet(dev, &action.policer.policerId);

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_NOT_EQUAL6_STRING_MAC(GT_OK, st, 
        "%d, %d, %d, %d, actionPtr->policer.policerId=%d, policerMode = %d",
        dev, entryType, entryIndex, portProt, action.policer.policerId, action.policer.policerMode);

        /* restore */
        action.policer.policerId = 0;

        /* 1.19. Call function with out of range  actionPtr.fa.lbi [0x3] and other parameters the 
        same as in 1.1. Expected: NON GT_OK for 98EX1x5 (Twist D) and 98EX1x6 (Tiger) and
        GT_OK for other devices. */
        action.fa.setLbi = GT_TRUE;
        action.fa.lbi = 0x3;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        
        if ((CPSS_PP_FAMILY_TWISTD_E == family) || (CPSS_PP_FAMILY_TIGER_E == family))
        {
            UTF_VERIFY_NOT_EQUAL6_STRING_MAC(GT_OK, st, 
            "Tiger or TwistD device: %d, %d, %d, %d, actionPtr->fa.setLbi=%d, actionPtr->fa.lbi=%d",
            dev, entryType, entryIndex, portProt, action.fa.setLbi, action.fa.lbi);
        }
        else
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);
        }

        /* restore */
        action.fa.setLbi = GT_FALSE;
        action.fa.lbi = 0;

        /* 1.20. Call function with out of range actionPtr.vlan.modifyVlan [0x5AAAAAA5] and other 
        parameters the same as in 1.1. Expected: GT_BAD_PARAM for 98EX1x6 (Tiger) and
        GT_OK for other devices. */
        action.vlan.modifyVlan = IP_FLOW_OUT_OF_ENUM_CNS;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);

        if (CPSS_PP_FAMILY_TIGER_E == family) 
        {
            UTF_VERIFY_NOT_EQUAL5_STRING_MAC(GT_OK, st, 
            "Tiger or TwistD device: %d, %d, %d, %d, actionPtr->vlan.modifyVlan=%d",
            dev, entryType, entryIndex, portProt, action.vlan.modifyVlan);
        }
        else
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);
        }

        /* restore */
        action.vlan.modifyVlan = CPSS_EXMX_POLICY_ACTION_VLAN_CMD_MODIFY_E;

        /* 1.21. Call function with out of range actionPtr.vlan.vlanId [4096]  and other parameters 
        the same as in 1.1. Expected: NON GT_OK 98EX1x6 (Tiger) and
        GT_OK for other devices. */
        action.vlan.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);

        if (CPSS_PP_FAMILY_TIGER_E == family) 
        {
            UTF_VERIFY_NOT_EQUAL6_STRING_MAC(GT_OK, st, 
            "Tiger or TwistD device: %d, %d, %d, %d, actionPtr->vlan.modifyVlan=%d, actionPtr->vlan.vlanId=%d",
            dev, entryType, entryIndex, portProt, action.vlan.modifyVlan, action.vlan.vlanId);
        }
        else
        {
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex, portProt);
        }

        /* restore */
        action.vlan.vlanId = 100;

        /* 1.22. Call function with actionPtr [NULL] and other parameters the same as in 1.1. 
        Expected: GT_BAD_PTR. */

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, NULL);

        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, %d, NULL", dev, entryType, entryIndex, portProt);

        /* 1.23. Check that Policer and CoS Marking Command Mark by entry are mutual exclusive for 
        98MX6x8, 98MX6x5, 98EX1x5 devices. Call with actionPtr [ actionPtr.qos. markCmd 
        = CPSS_EXMX_POLICY_ACTION_MARK_BY_ENTRY_E;  actionPtr.policer.policerMode =   
        CPSS_EXMX_POLICY_ACTION_POLICER_METER_ONLY_E]. Expected: NON GT_OK */

        action.qos.markCmd = CPSS_EXMX_POLICY_ACTION_MARK_BY_ENTRY_E;
        action.policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_METER_ONLY_E;

        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_NOT_EQUAL6_STRING_MAC(GT_OK, st,
        "%d, %d, %d, %d, actionPtr->qos.markCmd=%d, actionPtr->policer.policerMode=%d",
        dev, entryType, entryIndex, portProt, action.qos.markCmd, action.policer.policerMode);
    }

    entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E;
    entryIndex = 7;
    portProt = 15;

    cpssOsBzero((GT_VOID*)&action, sizeof(action));
    action.pktCmd = CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E;
    action.mirror.mirrorToCpu = GT_FALSE;
    action.mirror.mirrorToRxAnalyzerPort = GT_TRUE;
    action.qos.markCmd = CPSS_EXMX_POLICY_ACTION_MARK_NONE_E;
    action.qos.markDscp = GT_FALSE;
    action.qos. markTc = GT_FALSE;
    action.qos.markDp = GT_FALSE;
    action.qos.markUp = GT_FALSE;
    action.redirect.redirectCmd = CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_OUT_IF_E;
    action.redirect.redirectVid = 100;
    action.redirect.redirectData.outInterface.type = CPSS_INTERFACE_VID_E;
    action.redirect.redirectData.outInterface.vlanId = 100;
    action.policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
    action.fa.setLbi = GT_FALSE;
    action.fa.flowId = 0;
    action.vlan.modifyVlan = CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E;
    action.vlan.nestedVlan = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpFlowEntrySet(dev, entryType, entryIndex, portProt, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssExMxIpFlowEntryGet
(       
    IN  GT_U8                               dev,
    IN  CPSS_EXMX_IP_FLOW_ENTRY_TYPE_ENT    entryType,
    IN  GT_U32                              entryIndex,
    OUT GT_U16                              *portProtPtr,
    OUT CPSS_EXMX_POLICY_ACTION_STC         *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpFlowEntryGet)
{
/*
ITERATE_DEVICES (ExMx)
1.1. Call function with with entryType [CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E / 
CPSS_EXMX_IP_FLOW_ENTRY_TYPE_UDP_E / CPSS_EXMX_IP_FLOW_ENTRY_TYPE_IP_E], 
entryIndex [0/ 2/ 7], non-NULL portProtPtr anf non-NULL actionPtr. Expected: GT_OK.
1.2. Call function with out of range entryType [0x5AAAAAA5] and other parameters
 the same as in 1.1. Expected: GT_BAD_PARAM.
1.3. Call function with out of range entryIndex [8] and other parameters the
 same as in 1.1. Expected: NON GT_OK.
1.4. Call function with portProtPtr [NULL] and other parameters the same as 
in 1.1. Expected: GT_BAD_PTR.
1.5. Call function with actionPtr [NULL] and other parameters the same as in 
1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS                           st = GT_OK;
    GT_U8                               dev;
    CPSS_EXMX_IP_FLOW_ENTRY_TYPE_ENT    entryType;
    GT_U32                              entryIndex;
    GT_U16                              retPortProt;
    CPSS_EXMX_POLICY_ACTION_STC         retAction;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with with entryType [CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E / 
        CPSS_EXMX_IP_FLOW_ENTRY_TYPE_UDP_E / CPSS_EXMX_IP_FLOW_ENTRY_TYPE_IP_E], 
        entryIndex [0/ 2/ 7], non-NULL portProtPtr anf non-NULL actionPtr. Expected: GT_OK. */
        entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E;
        entryIndex = 0;

        st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, &retAction);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex);

        entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_UDP_E;
        entryIndex = 2;

        st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, &retAction);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex);

        entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_IP_E;
        entryIndex = 7;

        st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, &retAction);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryType, entryIndex);

        /* 1.2. Call function with out of range entryType [0x5AAAAAA5] and other parameters
        the same as in 1.1. Expected: GT_BAD_PARAM. */
        entryType = IP_FLOW_OUT_OF_ENUM_CNS;
        entryIndex = 0;

        st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, &retAction);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, entryType, entryIndex);

        /* 1.3. Call function with out of range entryIndex [8] and other parameters the
        same as in 1.1. Expected: NON GT_OK. */
        entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E;
        entryIndex = 8;

        st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, &retAction);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, entryIndex=%d", dev, entryType, entryIndex);

        /* 1.4. Call function with portProtPtr [NULL] and other parameters the same as 
        in 1.1. Expected: GT_BAD_PTR. */
        entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E;
        entryIndex = 0;

        st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, NULL, &retAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, portProtPtr=NULL", dev, entryType, entryIndex);

        /* 1.5. Call function with actionPtr [NULL] and other parameters the same as in 
        1.1. Expected: GT_BAD_PTR. */
        entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E;
        entryIndex = 0;

        st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, NULL);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, actionPtr=NULL", dev, entryType, entryIndex);

    }
    entryType = CPSS_EXMX_IP_FLOW_ENTRY_TYPE_TCP_E;
    entryIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, &retAction);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpFlowEntryGet(dev, entryType, entryIndex, &retPortProt, &retAction);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxIpFlow suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxIpFlow)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpFlowEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpFlowEntryGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxIpFlow)
