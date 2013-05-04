/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTunnelTermPortGroupIpv4overIpv4.h
*
* DESCRIPTION:
*       Tunnel Term: Ipv4 over Ipv4 Port Group - Basic
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfTunnelTermPortGroupIpv4overIpv4h
#define __prvTgfTunnelTermPortGroupIpv4overIpv4h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* port number to send traffic from port group 0 */
#define PRV_TGF_SEND_PORT_GROUP0_PORT_NUM_CNS   4       /* port 4 in port group 0 */

/* port number to send traffic from port group 1 */
#define PRV_TGF_SEND_PORT_GROUP1_PORT_NUM_CNS   20      /* port 4 in port group 1 */

/* port number to send traffic from port group 2 */
#define PRV_TGF_SEND_PORT_GROUP2_PORT_NUM_CNS   36      /* port 4 in port group 2 */

/* port number to send traffic from port group 3 */
#define PRV_TGF_SEND_PORT_GROUP3_PORT_NUM_CNS   52      /* port 4 in port group 3 */

/* port number to send traffic from */
GT_U8 prvTgfSendPort;
/*******************************************************************************
* prvTgfTunnelTermPortGroupIpv4overIpv4BaseConfigurationSet
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
GT_VOID prvTgfTunnelTermPortGroupIpv4overIpv4BaseConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermPortGroupIpv4overIpv4RouteConfigurationSet
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
GT_VOID prvTgfTunnelTermPortGroupIpv4overIpv4RouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermPortGroupIpv4overIpv4TtiConfigurationSet
*
* DESCRIPTION:
*       Set TTI Configuration
*
* INPUTS:
*       portGroupsBmp     - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfTunnelTermPortGroupIpv4overIpv4TtiConfigurationSet
(
    IN  GT_PORT_GROUPS_BMP     portGroupsBmp
);

/*******************************************************************************
* prvTgfTunnelTermPortGroupRuleValidStatusSet
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
GT_VOID prvTgfTunnelTermPortGroupRuleValidStatusSet
(
    GT_BOOL   validStatus
);

/*******************************************************************************
* prvTgfTunnelTermPortGroupIpv4overIpv4TrafficGenerate
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
GT_VOID prvTgfTunnelTermPortGroupIpv4overIpv4TrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermPortGroupIpv4overIpv4TrafficGenerateExpectNoTraffic
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
GT_VOID prvTgfTunnelTermPortGroupIpv4overIpv4TrafficGenerateExpectNoTraffic
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermPortGroupIpv4overIpv4ConfigurationRestore
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
GT_VOID prvTgfTunnelTermPortGroupIpv4overIpv4ConfigurationRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfTunnelTermPortGroupIpv4overIpv4h */

