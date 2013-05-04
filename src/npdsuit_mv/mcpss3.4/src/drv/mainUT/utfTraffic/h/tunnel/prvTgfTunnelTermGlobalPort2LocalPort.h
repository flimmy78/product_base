/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTunnelTermGlobalPort2LocalPort.h
*
* DESCRIPTION:
*       Tunnel Term: Global Port 2 Local Port - Basic
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfTunnelTermGlobalPort2LocalPorth
#define __prvTgfTunnelTermGlobalPort2LocalPorth

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* port1 number to send traffic to */
#define PRV_TGF_SEND_PORT1_NUM_CNS        2  /* local port 3, port group id 0 */

/* port2 number to send traffic to */
#define PRV_TGF_SEND_PORT2_NUM_CNS        18 /* local port 3, port group id 1 */

/* port3 number to send traffic to */
#define PRV_TGF_SEND_PORT3_NUM_CNS        34 /* local port 3, port group id 2 */

/* port4 number to send traffic to */
#define PRV_TGF_SEND_PORT4_NUM_CNS        50 /* local port 3, port group id 3 */

/* nextHop port number to receive traffic from */
#define PRV_TGF_NEXTHOPE_PORT_NUM_CNS     22
/*******************************************************************************
* prvTgfTunnelTermGlobalPort2LocalPortBaseConfigurationSet
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
GT_VOID prvTgfTunnelTermGlobalPort2LocalPortBaseConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermGlobalPort2LocalPortRouteConfigurationSet
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
GT_VOID prvTgfTunnelTermGlobalPort2LocalPortRouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermGlobalPort2LocalPortTtiConfigurationSet
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
GT_VOID prvTgfTunnelTermGlobalPort2LocalPortTtiConfigurationSet
(
    GT_U32      portTestNum,
    GT_U32      portConfigSet
);

/*******************************************************************************
* prvTgfTunnelTermGlobalPort2LocalPortTtiConfiguration2Set
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
*       sending Basic IPv4 Tunnel Termination - to Specific Port Group 1, local port 3
*******************************************************************************/
GT_VOID prvTgfTunnelTermGlobalPort2LocalPortTtiConfiguration2Set
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermGlobalPort2LocalPortTtiConfiguration3Set
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
*       sending Basic IPv4 Tunnel Termination - Port Groups 0,2 , local port 3
*******************************************************************************/
GT_VOID prvTgfTunnelTermGlobalPort2LocalPortTtiConfiguration3Set
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermGlobalPort2LocalPortRuleValidStatusSet
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
GT_VOID prvTgfTunnelTermGlobalPort2LocalPortRuleValidStatusSet
(
    GT_BOOL   validStatus
);

/*******************************************************************************
* prvTgfTunnelTermGlobalPort2LocalPortTrafficGenerate
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
GT_VOID prvTgfTunnelTermGlobalPort2LocalPortTrafficGenerate
(
    GT_U32 testNum,
    GT_U32 iteration,
    GT_U32 transmission
);

/*******************************************************************************
* prvTgfTunnelTermGlobalPort2LocalPortConfigurationRestore
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
GT_VOID prvTgfTunnelTermGlobalPort2LocalPortConfigurationRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfTunnelTermGlobalPort2LocalPorth */

