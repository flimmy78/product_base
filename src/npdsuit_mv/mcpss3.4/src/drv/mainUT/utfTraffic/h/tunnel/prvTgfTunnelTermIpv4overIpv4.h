/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTunnelTermIpv4overIpv4.h
*
* DESCRIPTION:
*       Tunnel Term: Ipv4 over Ipv4 - Basic
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfTunnelTermIpv4overIpv4h
#define __prvTgfTunnelTermIpv4overIpv4h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfTunnelTermIpv4overIpv4BaseConfigurationSet
* 
* DESCRIPTION:
*       Set Base Configuration
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
GT_VOID prvTgfTunnelTermIpv4overIpv4BaseConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermIpv4overIpv4RouteConfigurationSet
*
* DESCRIPTION:
*       Set Route Configuration
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
GT_VOID prvTgfTunnelTermIpv4overIpv4RouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermIpv4overIpv4TtiConfigurationSet
*
* DESCRIPTION:
*       Set TTI Configuration
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
GT_VOID prvTgfTunnelTermIpv4overIpv4TtiConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermRuleValidStatusSet
*
* DESCRIPTION:
*       Set TTI Rule Valid Status
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
GT_VOID prvTgfTunnelTermRuleValidStatusSet
(
    GT_BOOL   validStatus
);

/*******************************************************************************
* prvTgfTunnelTermIpv4overIpv4TrafficGenerate
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
GT_VOID prvTgfTunnelTermIpv4overIpv4TrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermIpv4overIpv4TrafficGenerateExpectNoTraffic
*
* DESCRIPTION:
*       Generate traffic - expect no traffic
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
GT_VOID prvTgfTunnelTermIpv4overIpv4TrafficGenerateExpectNoTraffic
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermIpv4overIpv4ConfigurationRestore
*
* DESCRIPTION:
*       Restore configuration
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
*       3. Restore TTI Configuration
*       2. Restore Route Configuration
*       1. Restore Base Configuration
*
*******************************************************************************/
GT_VOID prvTgfTunnelTermIpv4overIpv4ConfigurationRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfTunnelTermIpv4overIpv4h */

