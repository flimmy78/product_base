/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfDefaultIpv4McRouting.h
*
* DESCRIPTION:
*       Default IPV4 MC Routing
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfDefaultIpv4McRoutingh
#define __prvTgfDefaultIpv4McRoutingh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfDefaultIpv4McRoutingBaseConfigurationSet
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
GT_VOID prvTgfDefaultIpv4McRoutingBaseConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDefaultIpv4McRoutingDefaultLttRouteConfigurationSet
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
GT_VOID prvTgfDefaultIpv4McRoutingDefaultLttRouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDefaultIpv4McRoutingDefaultTrafficGenerate
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
GT_VOID prvTgfDefaultIpv4McRoutingDefaultTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDefaultIpv4McRoutingAdditionalRouteConfigurationSet
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
GT_VOID prvTgfDefaultIpv4McRoutingAdditionalRouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDefaultIpv4McRoutingAdditionalTrafficGenerate
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
GT_VOID prvTgfDefaultIpv4McRoutingAdditionalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDefaultIpv4McRoutingConfigurationRestore
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
GT_VOID prvTgfDefaultIpv4McRoutingConfigurationRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfDefaultIpv4McRoutingh */

