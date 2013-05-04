/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTunnelTermEtherOverIpv4GreRedirectToEgress.h
*
* DESCRIPTION:
*       Tunnel term Ethernet over Ipv4 GRE redirect to egress
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/


/*******************************************************************************
* prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressBridgeConfigSet
*
* DESCRIPTION:
*       Set Bridge Configuration
*
* INPUTS:
*       isTaggedVlansUsed - to mark vlans tagged or not
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
GT_VOID prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressBridgeConfigSet
(
    IN GT_BOOL isTaggedVlansUsed
);

/*******************************************************************************
* prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressTtiConfigSet
*
* DESCRIPTION:
*       Set TTI test settings:
*           - Enable port 0 for Eth lookup
*           - Set Eth key lookup MAC mode to Mac DA
*           - Set TTI rule action
*           - Set TTI rule
*
* INPUTS:
*       None
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
GT_VOID prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressTtiConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressTrafficGenerateTagged
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:11,
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
GT_VOID prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressTrafficGenerateTagged
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressTrafficGenerateUntagged
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:34:02,
*               macSa = 00:00:00:00:00:11,
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
GT_VOID prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressTrafficGenerateUntagged
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressConfigurationRestore
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
GT_VOID prvTgfTunnelTermEtherOverIpv4GreRedirectToEgressConfigurationRestore
(
    GT_VOID
);

