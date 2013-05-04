/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfVlanTag1RemIfZero.h
*
* DESCRIPTION:
*       VLAN Tag1 Removal If Zero UT.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfVlanTag1RemIfZero
#define __prvTgfVlanTag1RemIfZero

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfVlanTag1RemIfZeroConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*
*       Generate Traffic:
*
*       Success Criteria:
*
*       Additional Configuration:
*
*       Generate Traffic:
*
*       Success Criteria:
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
GT_VOID prvTgfVlanTag1RemIfZeroConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVlanTag1RemIfZeroTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
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
GT_VOID prvTgfVlanTag1RemIfZeroTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVlanTag1RemIfZeroConfigurationRestore
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
GT_VOID prvTgfVlanTag1RemIfZeroConfigurationRestore
(
    GT_VOID
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfVlanTag1RemIfZero */


