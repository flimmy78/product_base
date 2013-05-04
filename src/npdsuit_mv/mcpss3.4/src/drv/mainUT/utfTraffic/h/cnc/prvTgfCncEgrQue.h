/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfCncEgrQue.h
*
* DESCRIPTION:
*       Centralized Counters (Cnc)
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfCncEgrQue
#define __prvTgfCncEgrQue

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvTgfCncTestEgrQueConfigure
*
* DESCRIPTION:
*       This function configures Cnc EgrQue tests
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
GT_STATUS prvTgfCncTestEgrQueConfigure
(
    GT_VOID
);

/*******************************************************************************
* prvTgfCncTestEgrQueTrafficGenerateAndCheck
*
* DESCRIPTION:
*       Generate traffic:
*       Send to device's port given packet:
*       Check counters.
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
GT_VOID prvTgfCncTestEgrQueTrafficGenerateAndCheck
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfCncEgrQue */

