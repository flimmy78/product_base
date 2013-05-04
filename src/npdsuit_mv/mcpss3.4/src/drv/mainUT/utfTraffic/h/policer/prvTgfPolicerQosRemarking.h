/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPolicerQosRemarking.h
*
* DESCRIPTION:
*       Policer VLAN Counting UT.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfPolicerQosRemarkingh
#define __prvTgfPolicerQosRemarkingh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <common/tgfPolicerGen.h>

/*******************************************************************************
* prvTgfPolicerQosRemarkingVlanConfigurationSet
*
* DESCRIPTION:
*       Set configuration
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
GT_VOID prvTgfPolicerQosRemarkingVlanConfigurationSet
(
    GT_VOID    
);

/*******************************************************************************
* prvTgfPolicerQosRemarkingConfigurationRestore
*
* DESCRIPTION:
*       Restore configuration
*
* INPUTS:
*       policerStage - Policer stage
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
GT_VOID prvTgfPolicerQosRemarkingConfigurationRestore
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT policerStage
);

/*******************************************************************************
* prvTgfPolicerIngressQosRemarkingByEntryTrafficGenerate
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
GT_VOID prvTgfPolicerIngressQosRemarkingByEntryTrafficGenerate
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT policerStage
);

/*******************************************************************************
* prvTgfPolicerIngressQosRemarkingByTableTrafficGenerate
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
GT_VOID prvTgfPolicerIngressQosRemarkingByTableTrafficGenerate
(
    IN PRV_TGF_POLICER_STAGE_TYPE_ENT policerStage
);

/*******************************************************************************
* prvTgfPolicerEgressQosRemarkingTrafficGenerate
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
GT_VOID prvTgfPolicerEgressQosRemarkingTrafficGenerate
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfPolicerQosRemarkingh */


