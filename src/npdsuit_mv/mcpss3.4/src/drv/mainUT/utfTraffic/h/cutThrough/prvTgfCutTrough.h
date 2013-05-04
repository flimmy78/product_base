/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfCutThrough.h
*
* DESCRIPTION:
*       CPSS Cut-Through
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfCutThrough
#define __prvTgfCutThrough

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvTgfCutThroughBypassRouterAndPolicerBaseConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Create 2 Vlan entries -- (port[0], port[1]) and (port[2], port[3]);
*         - Enable Cut-Through;
*         - Set MRU value for a VLAN MRU profile;
*         - Create FDB entry.
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
GT_VOID prvTgfCutThroughBypassRouterAndPolicerBaseConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfCutThroughBypassRouterAndPolicerRouteConfigurationSet
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
GT_VOID prvTgfCutThroughBypassRouterAndPolicerRouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfCutThroughBypassRouterAndPolicerTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           - Send traffic;
*           - Check routing mechanism is working;
*           - Enable bypassing the Router and Ingress Policer engines;
*           - Send traffic;
*           - Check that routing mechanism is ignored.
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
GT_VOID prvTgfCutThroughBypassRouterAndPolicerTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfCutThroughBypassRouterAndPolicerConfigurationRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfCutThroughBypassRouterAndPolicerConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfCutThroughDefaultPacketLengthConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Create Vlan entry with all ports;
*         - Enable Cut-Through;
*         - Set MRU profile index for a VLAN;
*         - Set MRU value for a VLAN MRU profile;
*         - Set Default packet length to for Cut-Through mode.
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
GT_VOID prvTgfCutThroughDefaultPacketLengthConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfCutThroughDefaultPacketLengthTrafficGenerate
*
* DESCRIPTION:
*         - Send packet;
*         - Check that all ports received the packet;
*         - Set Default packet length to be > MRU value;
*         - Send packet;
*         - Check that no port received the packet.
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
GT_VOID prvTgfCutThroughDefaultPacketLengthTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfCutThroughDefaultPacketLengthConfigurationRestore
*
* DESCRIPTION:
*       Restore test configuration
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
GT_VOID prvTgfCutThroughDefaultPacketLengthConfigurationRestore
(
    GT_VOID
);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfCutThrough */

