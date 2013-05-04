/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfBasicTests.h
*
* DESCRIPTION:
*       Basic tests for IPFIX declarations
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfBasicTestsh
#define __prvTgfBasicTestsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/ipfix/cpssDxChIpfix.h>

/* default VLAN0 Id */
#define PRV_TGF_VLANID_CNS  5

/* port number to send traffic to */
#define PRV_TGF_SEND_PORT_IDX_CNS     1

/*
 * Typedef: structure PRV_TGF_TEST_CONFIG_STC
 *
 * Description: Some test specific configuration parameters 
 *
 * Fields:
 *      ruleIndex   - pcl rule index
 *      ipfixIndex  - IPFIX entry index
 *      burstCount  - num of packets to send
 *      payloadLen  - each packet payload length
 *      samplingAction  - Command done on sampled packets.
 *      samplingMode    - Sampling base method.
 *      randomFlag      - Determines the random offset used for the 
 *                        reference value calculation in sampling.
 *      samplingWindow  - Window used for Reference value calculation. 
 *                        The units of this window are according to 
 *                        samplingMode.
 *      cpuSubCode      - 2 lsb bits of the cpu code for sampled packets.
 *                        Relevant only when samplingAction is Mirror.
 */
typedef struct
{ 
    GT_U32 ruleIndex;
    GT_U32 ipfixIndex;
    GT_U32 burstCount;
    GT_U32 payloadLen;
    PRV_TGF_IPFIX_SAMPLING_ACTION_ENT   samplingAction;
    PRV_TGF_IPFIX_SAMPLING_MODE_ENT     samplingMode;
    PRV_TGF_IPFIX_SAMPLING_DIST_ENT     randomFlag;
    GT_U64                              samplingWindow;
    GT_U32                              cpuSubCode;
}PRV_TGF_TEST_CONFIG_STC;

#ifndef PRV_TGF_TEST_DEFINITIONS

/* port bitmap VLAN members */
extern CPSS_PORTS_BMP_STC localPortsVlanMembers;

extern PRV_TGF_TEST_CONFIG_STC basicTestConf[5];

extern PRV_TGF_POLICER_STAGE_TYPE_ENT testedStage;

/******************************* Test packet **********************************/

/* L2 part of packet */
extern TGF_PACKET_L2_STC prvTgfPacketL2Part;

/* First VLAN_TAG part */
extern TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTag0Part;

/* DATA of packet */
extern GT_U8 prvTgfPayloadDataArr[];

/* PAYLOAD part */
extern TGF_PACKET_PAYLOAD_STC prvTgfPacketPayloadPart;

/* PARTS of packet */
extern TGF_PACKET_PART_STC prvTgfPacketPartArray[];

/* PACKET to send */
extern TGF_PACKET_STC prvTgfPacketInfo;

#endif /* PRV_TGF_TEST_DEFINITIONS */

/*******************************************************************************
* prvTgfIpfixStagesParamsSaveAndReset
*
* DESCRIPTION:
*       Save stages counting mode and enable metering state for later restore
*       and set stages to disable counting and metering. 
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
GT_VOID prvTgfIpfixStagesParamsSaveAndReset
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixStagesParamsRestore
*
* DESCRIPTION:
*       Restore stages counting mode and enable metering state. 
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
GT_VOID prvTgfIpfixStagesParamsRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixIngressTestInit
*
* DESCRIPTION:
*       Set test configuration:
*       Enable the Ingress Policy Engine set Pcl rule.  
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
GT_VOID prvTgfIpfixIngressTestInit
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixFirstIngressTestInit
*
* DESCRIPTION:
*       Set test configuration:
*       Enable the Ingress Policy Engine set Pcl rule.  
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
GT_VOID prvTgfIpfixFirstIngressTestInit
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixFirstUseAllIngressTestInit
*
* DESCRIPTION:
*       Set test configuration:
*       Enable the Ingress Policy Engine set Pcl rule.  
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
GT_VOID prvTgfIpfixFirstUseAllIngressTestInit
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixSecondUseAllIngressTestInit
*
* DESCRIPTION:
*       Set test configuration:
*       Enable the Ingress Policy Engine set Pcl rule.  
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
GT_VOID prvTgfIpfixSecondUseAllIngressTestInit
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixTimestampVerificationTestInit
*
* DESCRIPTION:
*       Set test configuration:
*       Enable the Ingress Policy Engine set Pcl rule.  
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
GT_VOID prvTgfIpfixTimestampVerificationTestInit
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixIngressTestTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic
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
GT_VOID prvTgfIpfixIngressTestTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixTimestampVerificationTestTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic
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
GT_VOID prvTgfIpfixTimestampVerificationTestTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixIngressRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfIpfixIngressTestRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixTrafficGenManager
*
* DESCRIPTION:
*       Perform the test traffic generation and checks for all port groups 
*       (unaware mode) and again for first core. 
*
* INPUTS:
*       initFuncPtr         - (pointer to) test initialization function.
*       trafficGenFuncPtr   - (pointer to) traffic generation and results check
*                             function.
*       restoreFuncPtr      - (pointer to) test restore configuration function.
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
GT_VOID prvTgfIpfixTrafficGenManager
(
    GT_VOID (*initFuncPtr)(GT_VOID),
    GT_VOID (*trafficGenFuncPtr)(GT_VOID),
    GT_VOID (*restoreFuncPtr)(GT_VOID)
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfBasicTestsh */
