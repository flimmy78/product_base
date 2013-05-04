/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfComplicatedIpv4McRouting.h
*
* DESCRIPTION:
*       Complicated IPV4 MC Routing
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfComplicatedIpv4McRoutingh
#define __prvTgfComplicatedIpv4McRoutingh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfComplicatedIpv4McRoutingBaseConfigurationSet
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
GT_VOID prvTgfComplicatedIpv4McRoutingBaseConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfComplicatedIpv4McRoutingDefaultLttRouteConfigurationSet
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
GT_VOID prvTgfComplicatedIpv4McRoutingDefaultLttRouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfComplicatedIpv4McRoutingDefaultTrafficGenerate
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
GT_VOID prvTgfComplicatedIpv4McRoutingDefaultTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfComplicatedIpv4McRoutingAdditionalRouteConfigurationSet
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
GT_VOID prvTgfComplicatedIpv4McRoutingAdditionalRouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfComplicatedIpv4McRoutingAdditionalTrafficGenerate
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
GT_VOID prvTgfComplicatedIpv4McRoutingAdditionalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfComplicatedIpv4McRoutingConfigurationRestore
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
GT_VOID prvTgfComplicatedIpv4McRoutingConfigurationRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfComplicatedIpv4McRoutingh */

