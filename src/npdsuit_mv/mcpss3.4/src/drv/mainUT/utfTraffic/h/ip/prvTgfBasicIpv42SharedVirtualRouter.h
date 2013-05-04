/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfBasicIpv42SharedVirtualRouter.h
*
* DESCRIPTION:
*       Basic IPV4 UC 2 shared virtual Router
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfBasicIpv42SharedVirtualRouterh
#define __prvTgfBasicIpv42SharedVirtualRouterh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfBasicIpv42SharedVirtualRouterBaseConfigSet
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
GT_VOID prvTgfBasicIpv42SharedVirtualRouterBaseConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBasicIpv42SharedVirtualRouterRouteConfigSet
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
GT_VOID prvTgfBasicIpv42SharedVirtualRouterRouteConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBasicIpv42SharedVirtualRouterUcIpConfigSet
*
* DESCRIPTION:
*       Set UC IP Configuration
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
GT_VOID prvTgfBasicIpv42SharedVirtualRouterUcIpConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBasicIpv42NonSharedVirtualRouterMcIpConfigSet
*
* DESCRIPTION:
*       Set MC IP Configuration
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
GT_VOID prvTgfBasicIpv42NonSharedVirtualRouterMcIpConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBasicIpv42SharedVirtualRouterEnable
*
* DESCRIPTION:
*       Enable routing mechanism
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
GT_VOID prvTgfBasicIpv42SharedVirtualRouterEnable
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBasicIpv42SharedVirtualRouterTrafficGenerate
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
GT_VOID prvTgfBasicIpv42SharedVirtualRouterTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBasicIpv4Uc2SharedVRConfigRestore
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
GT_VOID prvTgfBasicIpv4Uc2SharedVRConfigRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfBasicIpv42SharedVirtualRouterh */

