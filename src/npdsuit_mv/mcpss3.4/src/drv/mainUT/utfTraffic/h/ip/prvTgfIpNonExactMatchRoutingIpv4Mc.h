/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfIpNonExactMatchRoutingIpv4Mc.h
*
* DESCRIPTION:
*       Non-exact match IPv4 MC prefix routing.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfIpNonExactMatchRoutingIpv4Mc
#define __prvTgfIpNonExactMatchRoutingIpv4Mc

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <common/tgfIpGen.h>

/*******************************************************************************
* prvTgfIpNonExactMatchRoutingIpv4McBaseConfigurationSet
*
* DESCRIPTION:
*       Set Base Configuration
*
* INPUTS:
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
GT_VOID prvTgfIpNonExactMatchRoutingIpv4McBaseConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpNonExactMatchRoutingIpv4McRouteConfigurationSet
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
GT_VOID prvTgfIpNonExactMatchRoutingIpv4McRouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpNonExactMatchRoutingIpv4McTrafficGenerate
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
GT_VOID prvTgfIpNonExactMatchRoutingIpv4McTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpNonExactMatchRoutingIpv4McConfigurationRestore
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
GT_VOID prvTgfIpNonExactMatchRoutingIpv4McConfigurationRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfIpNonExactMatchRoutingIpv4Mc */
