/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfFdbAging.h
*
* DESCRIPTION:
*       FDB Age Bit Da Refresh
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfFdbAging
#define __prvTgfFdbAging

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfFdbAgeBitDaRefreshConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Enable automatic aging.
*           Add to FDB table non-static entry with 
*                   MACADDR: 00:00:00:00:00:22 on port 23.
*           Change Aging time to 10 secunds (real aging will be after 
*                   2*10 second).
*           Success Criteria:
*               After 20 seconds packets captured on ports 8 and 18
*
*           Add to FDB table non-static entry with 
*                   MACADDR: 00:00:00:00:00:22 on port 23.
*           Enable Aged bit DA refresh.
*           Success Criteria:
*               After 20 seconds there will no packets captured on 
*               ports 8 and 18.
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
GT_VOID prvTgfFdbAgeBitDaRefreshConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfFdbAgeBitDaRefreshTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:00:11,
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
GT_VOID prvTgfFdbAgeBitDaRefreshTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfFdbAgeBitDaRefreshConfigurationRestore
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
GT_VOID prvTgfFdbAgeBitDaRefreshConfigurationRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfFdbAging */


