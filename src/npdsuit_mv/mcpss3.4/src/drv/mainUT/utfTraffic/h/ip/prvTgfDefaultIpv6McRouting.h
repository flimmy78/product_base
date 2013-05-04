/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfDefaultIpv6McRouting.h
*
* DESCRIPTION:
*       Default IPV6 MC Routing
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfDefaultIpv6McRoutingh
#define __prvTgfDefaultIpv6McRoutingh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfDefaultIpv6McRoutingBaseConfigurationSet
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
GT_VOID prvTgfDefaultIpv6McRoutingBaseConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDefaultIpv6McRoutingDefaultLttRouteConfigurationSet
*
* DESCRIPTION:
*       Set default LTT Route Configuration
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
GT_VOID prvTgfDefaultIpv6McRoutingDefaultLttRouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDefaultIpv6McRoutingDefaultTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic to defaults
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
GT_VOID prvTgfDefaultIpv6McRoutingDefaultTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDefaultIpv6McRoutingAdditionalRouteConfigurationSet
*
* DESCRIPTION:
*       Set additional Route Configuration
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
GT_VOID prvTgfDefaultIpv6McRoutingAdditionalRouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDefaultIpv6McRoutingAdditionalTrafficGenerate
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
GT_VOID prvTgfDefaultIpv6McRoutingAdditionalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDefaultIpv6McRoutingConfigurationRestore
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
*       2. Restore Route Configuration
*       1. Restore Base Configuration
*
*******************************************************************************/
GT_VOID prvTgfDefaultIpv6McRoutingConfigurationRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfDefaultIpv6McRoutingh */

