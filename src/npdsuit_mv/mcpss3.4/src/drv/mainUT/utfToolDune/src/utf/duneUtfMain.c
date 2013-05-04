#include <cpss/extServices/os/gtOs/gtGenTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>

#include <utf/duneUtfMain.h>
#include <utf/private/prvDuneUtfSuitsConfig.h>
#include <utf/private/prvDuneUtfExtras.h>


/* contains registration for all DUNE suits */
extern GT_STATUS utfAllDuneSuitsDeclare(GT_VOID);

/* Indicator whether DUNE UTF for has been initialized */
static GT_BOOL isDuneInitialized = GT_FALSE;


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
)
{
    GT_STATUS   st = GT_OK;

    PRV_UTF_LOG0_MAC("\n");
    PRV_UTF_LOG0_MAC("[UTF]: duneUtfInit: ************************\n");

    /* init extras, like active devices array, ports */
    st = prvDuneUtfExtrasInit();
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: duneUtfInit: failed [0x%X] init devices and ports\n", st);
    }

    return st;
}

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
)
{
    if (GT_FALSE == isDuneInitialized)
    {
        isDuneInitialized = GT_TRUE;
        return utfAllDuneSuitsDeclare();
    }

    return GT_ALREADY_EXIST;
}

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
)
{
    GT_STATUS         rc       = GT_OK;
    UTF_TEST_TYPE_ENT testType = UTF_TEST_TYPE_NONE_E;
    GT_U8             iter     = 0;

    /* disable UTs */
    for (iter = UTF_TEST_TYPE_NONE_E + 1; iter < UTF_TEST_TYPE_LAST_E; iter++)
    {
        testType = (UTF_TEST_TYPE_ENT) iter;

        rc = utfSkipTests(testType);
        if (GT_OK != rc)
    	{
    		PRV_UTF_LOG1_MAC("[UTF]: utfTestsDuneRun: failed [0x%X] to disable UT\n", rc);
    	}
    }
    
    /* run all suits */
    rc = utfTestsRun(testPathPtr, numOfRuns, continueFlag);

    /* enable all UTs */
    testType = UTF_TEST_TYPE_NONE_E;

    rc = utfSkipTests(testType);
    if (GT_OK != rc)
	{
		PRV_UTF_LOG1_MAC("[UTF]: utfTestsDuneRun: failed [0x%X] to enable UT\n", rc);
	}

    return rc;
}

