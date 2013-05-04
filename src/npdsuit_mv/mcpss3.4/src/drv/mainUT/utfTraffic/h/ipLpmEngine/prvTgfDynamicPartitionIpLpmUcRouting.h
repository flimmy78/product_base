/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfDynamicPartitionIpLpmUcRouting.h
*
* DESCRIPTION:
*       IP LPM Engine
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfDynamicPartitionIpLpmUcRoutingh
#define __prvTgfDynamicPartitionIpLpmUcRoutingh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushDownUcBaseConfigurationSet
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
GT_VOID prvTgfDynamicPartitionIpLpmPushDownUcBaseConfigurationSet
(
    GT_VOID
);
/*******************************************************************************
* prvTgfDynamicPartitionIpLpmDefragUcBaseConfigurationSet
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
GT_VOID prvTgfDynamicPartitionIpLpmDefragUcBaseConfigurationSet
(
    GT_VOID
);



/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushUpUcBaseConfigurationSet
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
GT_VOID prvTgfDynamicPartitionIpLpmPushUpUcBaseConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushUpUcRouteConfigurationSet
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
GT_VOID prvTgfDynamicPartitionIpLpmPushUpUcRouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushDownUcRouteConfigurationSet
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
GT_VOID prvTgfDynamicPartitionIpLpmPushDownUcRouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDynamicPartitionIpLpmDefragUcRouteConfigurationSet
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
GT_VOID prvTgfDynamicPartitionIpLpmDefragUcRouteConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushUpUcRoutingTrafficGenerate
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
GT_VOID prvTgfDynamicPartitionIpLpmPushUpUcRoutingTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushDownUcRoutingTrafficGenerate
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
GT_VOID prvTgfDynamicPartitionIpLpmPushDownUcRoutingTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDynamicPartitionIpLpmDefragUcRoutingTrafficGenerate
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
GT_VOID prvTgfDynamicPartitionIpLpmDefragUcRoutingTrafficGenerate
(
    GT_VOID
);


/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushUpUcRoutingConfigurationRestore
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
GT_VOID prvTgfDynamicPartitionIpLpmPushUpUcRoutingConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDynamicPartitionIpLpmPushDownUcRoutingConfigurationRestore
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
GT_VOID prvTgfDynamicPartitionIpLpmPushDownUcRoutingConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfDynamicPartitionIpLpmDefragUcRoutingConfigurationRestore
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
GT_VOID prvTgfDynamicPartitionIpLpmDefragUcRoutingConfigurationRestore
(
    GT_VOID
);







#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfDynamicPartitionIpLpmUcRoutingh */

