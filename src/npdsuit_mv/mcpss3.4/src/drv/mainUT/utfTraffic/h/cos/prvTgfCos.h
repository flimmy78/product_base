/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfCos.h
*
* DESCRIPTION:
*       Class of services (Cos)
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfCos
#define __prvTgfCos

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvTgfCosTestCommonConfigure
*
* DESCRIPTION:
*       This function configures Cos all tests
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCosTestCommonConfigure
(
    GT_VOID
);

/*******************************************************************************
* prvTgfCosTestCommonReset
*
* DESCRIPTION:
*       This function resets configuration of Cos
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfCosTestCommonReset
(
    GT_VOID
);

/*******************************************************************************
* prvTgfCosExpTestTrafficGenerateAndCheck
*
* DESCRIPTION:
*       Generate traffic:
*       Send to device's port given packet:
*       Check amount of egressed packets with both TCs
*
* INPUTS:
*       None
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfCosExpTestTrafficGenerateAndCheck
(
    GT_VOID
);

/*******************************************************************************
* prvTgfCosVlanTagSelectTestTrafficGenerateAndCheck
*
* DESCRIPTION:
*       Generate traffic:
*       Send to device's port given packet:
*       Check amount of egressed packets with both TCs
*
* INPUTS:
*       None
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfCosVlanTagSelectTestTrafficGenerateAndCheck
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfCos */

