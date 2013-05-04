/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *2
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTunnelTermEtherType.h
*
* DESCRIPTION:
*       Tunnel Term: Ethertype- Basic
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#ifndef __prvTgfTunnelTermEtherTypeh
#define __prvTgfTunnelTermEtherTypeh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvTgfTunnelTermEtherTypeBaseConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set index of the rule in the TCAM to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - set MAC mask as disable
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
GT_VOID prvTgfTunnelTermEtherTypeBaseConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermEtherTypeVrfIdAssignConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set index of the rule in the TCAM to 1
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - set MAC mask as disable
*         - set TTI rule action - change VRF ID
*
* INPUTS:
*       sendPortNum - port number to send packet
*       vrfId       - VRF ID to set
*       l2DataPtr   - L2 data of packet
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
GT_VOID prvTgfTunnelTermEtherTypeVrfIdAssignConfigurationSet
(
    IN  GT_U8                         sendPortNum,
    IN  GT_U32                        vrfId,
    IN  TGF_PACKET_L2_STC            *l2DataPtr
);

/*******************************************************************************
* prvTgfTunnelTermEtherTypeTrafficGenerate
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
GT_VOID prvTgfTunnelTermEtherTypeTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermEtherTypeTrafficGenerate
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
GT_VOID prvTgfTunnelTermEtherTypeDifferentTrafficGenerate
(
    GT_VOID
);


/*******************************************************************************
* prvTgfTunnelTermEtherTypeTrafficGenerateExpectNoTraffic
*
* DESCRIPTION:
*       Generate traffic - Expect No Traffic
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
GT_VOID prvTgfTunnelTermEtherTypeTrafficGenerateExpectNoTraffic
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermEtherTypeConfigurationRestore
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
GT_VOID prvTgfTunnelTermEtherTypeConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermEtherTypeVrfIdAssignConfigRestore
*
* DESCRIPTION:
*       Restore test configuration
*
* INPUTS:
*       sendPortNum - port number to send packets
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
GT_VOID prvTgfTunnelTermEtherTypeVrfIdAssignConfigRestore
(
    IN GT_U8  sendPortNum
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __prvTgfTunnelTermEtherTypeh */
