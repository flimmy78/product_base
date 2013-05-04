/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfIpNonExactMatchRoutingIpv6Mc.h
*
* DESCRIPTION:
*       Non-exact match IPv6 MC prefix routing.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfIpNonExactMatchRoutingIpv6Mc
#define __prvTgfIpNonExactMatchRoutingIpv6Mc

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <common/tgfIpGen.h>

/*******************************************************************************
* prvTgfIpNonExactMatchRoutingIpv6McBaseConfigurationSet
*
* DESCRIPTION:
*       Set Base Configuration
*
* INPUTS:
*       protocol  - protocol to be tested (IPv4/IPv6)
*       routeType - routing type (unicast/multicast)
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
GT_VOID prvTgfIpNonExactMatchRoutingIpv6McBaseConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpNonExactMatchRoutingIpv6McRouteConfigurationSet
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
GT_VOID prvTgfIpNonExactMatchRoutingIpv6McRouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpNonExactMatchRoutingIpv6McTrafficGenerate
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
GT_VOID prvTgfIpNonExactMatchRoutingIpv6McTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpNonExactMatchRoutingIpv6McConfigurationRestore
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
*       1. Restore Route Configuration
*       2. Restore Base Configuration
*
*******************************************************************************/
GT_VOID prvTgfIpNonExactMatchRoutingIpv6McConfigurationRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfIpNonExactMatchRoutingIpv6Mc */
