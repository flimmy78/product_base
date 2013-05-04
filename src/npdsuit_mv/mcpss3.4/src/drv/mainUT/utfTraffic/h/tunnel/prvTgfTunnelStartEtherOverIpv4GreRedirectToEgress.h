/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTunnelStartEtherOverIpv4GreRedirectToEgress.h
*
* DESCRIPTION:
*       Tunnel start Ethernet over Ipv4 GRE redirect to egress
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/


/*******************************************************************************
* prvTgfTunnelStartEtherOverIpv4GreRedirectToEgressBridgeConfigSet
*
* DESCRIPTION:
*       Set Bridge Configuration
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
GT_VOID prvTgfTunnelStartEtherOverIpv4GreRedirectToEgressBridgeConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelStartEtherOverIpv4GreRedirectToEgressTunnelConfigSet
*
* DESCRIPTION:
*       Set Tunnel Configuration
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
GT_VOID prvTgfTunnelStartEtherOverIpv4GreRedirectToEgressTunnelConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelStartEtherOverIpv4GreRedirectToEgressPclConfigSet
*
* DESCRIPTION:
*       Set Pcl Configuration
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
GT_VOID prvTgfTunnelStartEtherOverIpv4GreRedirectToEgressPclConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelStartEtherOverIpv4GreRedirectToEgressTrafficGenerate
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
GT_VOID prvTgfTunnelStartEtherOverIpv4GreRedirectToEgressTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelStartEtherOverIpv4GreRedirectToEgressConfigurationRestore
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
GT_VOID prvTgfTunnelStartEtherOverIpv4GreRedirectToEgressConfigurationRestore
(
    GT_VOID
);

