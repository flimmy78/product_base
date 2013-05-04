/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* duneUtfMain.h
*
* DESCRIPTION:
*       Public header for test suits.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __duneUtfMainh
#define __duneUtfMainh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/extServices/os/gtOs/gtGenTypes.h>


/*******************************************************************************
* duneUtfPreInitPhase
*
* DESCRIPTION:
*       This routine start initializing UTF core. It must be called first.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           -   UTF initialization was successful.
*       GT_CREATE_ERROR -   UTF has no more free entries to register test cases.
*       GT_FAIL         -   General failure error. Should never happen.
*
* COMMENTS:
*       It's good hint to insert this function inside some global configuration
*       function, as part of initialization phase.
*
*******************************************************************************/
GT_STATUS duneUtfPreInitPhase
(
    GT_VOID
);

/*******************************************************************************
* duneUtfInit
*
* DESCRIPTION:
*       This routine initializes DUNE UTF core.
*       It must be called first.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           -   UTF initialization was successful.
*       GT_CREATE_ERROR -   UTF has no more free entries to register test cases.
*       GT_FAIL         -   General failure error. Should never happen.
*
* COMMENTS:
*       It's good hint to insert this function inside some global configuration
*       function, as part of initialization phase.
*
*******************************************************************************/
GT_STATUS duneUtfInit
(
    GT_VOID
);

/*******************************************************************************
* utfTestsDuneRun
*
* DESCRIPTION:
*       Runs all suits in case of NULL input path (testPathPtr),
*       or suit if suite name is specified as testPathPtr,
*       or specific test in the suit.
*
* INPUTS:
*       testPathPtr -   (pointer to) name of suite or test case to be run.
*                       In case of test case has format <suit.testcase>.
*       numOfRuns   -   defines how many times tests will be executed in loop.
*       fContinue   -   defines will test be interrupted after first failure condition.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               -   Running of tests was successful.
*       GT_NOT_FOUND        -   Test path wasn't found.
*       GT_BAD_PARAM        -   Bad number of cycles was passed.
        GT_UTF_TEST_FAILED  -   Some test fails.
*       GT_FAIL             -   General failure error. Should never happen.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS utfTestsDuneRun
(
    IN const GT_8   *testPathPtr,
    IN GT_U32       numOfRuns,
    IN GT_BOOL      continueFlag
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __duneUtfMainh */

