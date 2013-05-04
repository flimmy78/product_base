/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTunnelTermIpv4overIpv4Trap.h
*
* DESCRIPTION:
*       Tunnel Term: Ipv4 over Ipv4 - Action Trap
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfTunnelTermIpv4overIpv4Traph
#define __prvTgfTunnelTermIpv4overIpv4Traph

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfTunnelTermIpv4overIpv4TrapBaseConfigSet
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
GT_VOID prvTgfTunnelTermIpv4overIpv4TrapBaseConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermIpv4overIpv4TrapIpConfigSet
*
* DESCRIPTION:
*       Set IP Configuration
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
GT_VOID prvTgfTunnelTermIpv4overIpv4TrapIpConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermIpv4overIpv4TrapTcamConfigSet
*
* DESCRIPTION:
*       Set TCAM Configuration
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
GT_VOID prvTgfTunnelTermIpv4overIpv4TrapTcamConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermIpv4overIpv4TrapTrafficGenerate
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
GT_VOID prvTgfTunnelTermIpv4overIpv4TrapTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermIpv4overIpv4TrapConfigRestore
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
GT_VOID prvTgfTunnelTermIpv4overIpv4TrapConfigRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfTunnelTermIpv4overIpv4Traph */

