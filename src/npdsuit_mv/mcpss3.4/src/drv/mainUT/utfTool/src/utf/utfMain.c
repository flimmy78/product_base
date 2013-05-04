/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* utfMain.c
*
* DESCRIPTION:
*       Entry point for the UT (unit tests) engine
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpssCommon/cpssPresteraDefs.h>

#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsStr.h>
#include <gtOs/gtOsTimer.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>
#include <utf/private/prvUtfSuitsConfig.h>
#include <utf/private/prvUtfExtras.h>

#ifdef IMPL_TGF
    #include <common/tgfCommon.h>
#endif /* IMPL_TGF */

#define DEV_0_CNS    0

/* default device number */
GT_U8   prvTgfDevNum       = DEV_0_CNS;

/* flag to state the test uses port groups bmp */
GT_BOOL              usePortGroupsBmp = GT_FALSE;
/* bmp of port groups to use in test */
GT_PORT_GROUPS_BMP   currPortGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
/* number of port groups that we look for FDB */
GT_U32   prvTgfNumOfPortGroups = 0;

/* Max number of characters in test path */
#define UTF_MAX_TEST_PATH_LEN_CNS 100

/* Stack size for test's task */
#define UTF_TASK_STACK_SIZE_CNS 0x8000

/* Test's task priority */
#define UTF_TASK_PRIO_CNS 64

/* Test's result file name */
#define UTF_RESULT_FILENAME_CNS "result.txt"

/* Test's result of compare file name */
#define UTF_RESULT_OF_COMPARE_FILENAME_CNS "result_of_compare.txt"

/* Mark end of param string  */
#define UTF_END_OF_PARAM_VALUE_CNS 0xF 

/* Check given function for wrong enum values (from array) */
GT_U32  enumsIndex = 0;
GT_U32 tempParamValue = 0;


static FILE* fpResult = NULL;

/* array of buffers of 'general states' DB */
static struct {
    GT_CHAR buff[2048];/* the string to log when test fail */
    GT_BOOL valid;     /* is entry valid */
}prvUtfGeneralStateLogArr[PRV_UTF_GENERAL_STATE_INFO_NUM_CNS];

/*
 * Typedef: struct UTF_TEST_ENTRY_STC
 *
 * Description: Test case entry
 *              contains all runtime info about test case
 *
 * Fields:
 *          testFuncPtr     - pointer to test case function
 *          testNamePtr     - name of test case
 *          suiteNamePtr    - name of test suit
 *          errorCount      - number of errors for the test case
 *          testExecTimeSec - test running time
 *          testExecTimeNsec - test running time
 *          testType        - type of test
 */
typedef struct
{
    UTF_TEST_CASE_FUNC_PTR  testFuncPtr;
    const GT_CHAR*          testNamePtr;
    const GT_CHAR*          suitNamePtr;
    GT_U32                  errorCount;
    GT_U32                  testExecTimeSec;
    GT_U32                  testExecTimeNsec;
    UTF_TEST_TYPE_ENT       testType;
} UTF_TEST_ENTRY_STC;

/*
 * Typedef: struct UTF_CTX_STC
 *
 * Description: Main context for UTF
 *              there is can be only one statically
 *              allocated instance of this context
 *
 * Fields:
 *          testEntiresArray    - array of test entries
 *          testIndexArray      - array of random generated test's indexes
 *          usedTestNum         - number of used test entries in array
 *          currTestNum         - index of current test entry
 *          errorFlag           - indicates if it was error
 *          continueFlag        - define if UTF will continue test after first error
 */
typedef struct
{
    UTF_TEST_ENTRY_STC  testEntriesArray[UTF_MAX_NUM_OF_TESTS_CNS];
    GT_U32              testIndexArray[UTF_MAX_NUM_OF_TESTS_CNS];
    GT_U32              usedTestNum;
    GT_U32              currTestNum;
    GT_BOOL             errorFlag;
    GT_BOOL             continueFlag;
} UTF_CTX_STC;

/* single instance of UTF context */
static UTF_CTX_STC utfCtx;

/*
 * Typedef: struct UTF_TIMER_STC
 *
 * Description: contains timer info
 *
 * Fields:
 *          codeExecTimeSec  - code running time in seconds
 *          codeExecTimeNsec - code running time in nanoseconds
 */
typedef struct
{
    GT_U32              codeExecTimeSec;
    GT_U32              codeExecTimeNsec;
} UTF_TIMER_STC;

/* single instance of UTF timer struct */
static UTF_TIMER_STC utfTimer = {0, 0};

/* contains registration for all suits */
extern GT_STATUS utfAllSuitsDeclare(GT_VOID);

/* forward declaration for internal utf functions */
static GT_VOID utfSuitStatsShow(IN const GT_CHAR *suitNamePtr);
static GT_VOID utfFinalStatsShow(GT_VOID);
static GT_STATUS utfTestAdd(IN UTF_TEST_CASE_FUNC_PTR   tesFuncPtr,
                            IN const GT_CHAR            *testNamePtr,
                            IN const GT_CHAR            *suitNamePtr);
static GT_VOID utfPreTestRun(GT_VOID);
static GT_VOID utfPostTestRun(GT_VOID);
static GT_STATUS utfSuitsRun(IN const GT_CHAR  *testPathPtr,
                             IN GT_BOOL        continueFlag,
                             IN GT_BOOL        startFlag);
static GT_U32 utfTestsNumGet(IN const GT_CHAR  *suitNamePtr);


/* The printing mode that the UTF operates. */
static UTF_LOG_OUTPUT_ENT utfLogOutputSelectMode = UTF_LOG_OUTPUT_SERIAL_FINAL_E;

/* Semaphore for single test */
static CPSS_OS_SIG_SEM utfTestSemId = 0;

/* Task ID for single test run */
static CPSS_TASK utfTestTaskId = 0;

/* TimeOut for a single test */
static GT_U32 utfTestTimeOut = CPSS_OS_SEM_WAIT_FOREVER_CNS;

/* test type */
static UTF_TEST_TYPE_ENT utfTestType = UTF_TEST_TYPE_NONE_E;

/* test type bitmap to skip */
static GT_U32 utfSkipTestTypeBmp = 0;

/* start test flag */
static GT_BOOL utfStartFlag = GT_FALSE;

/* initial start time in sec */
static GT_U32  startTimeSec;

/* initial start time in nsec */
static GT_U32  startTimeNsec;

/* number of tests that is not checked for memory leakage */
#define NUM_MEM_LEAK_SKIPPED_TESTS 1

/* memLeakageSkippedTestArray - contains test names that is not checked for memory leakage */
static GT_CHAR *memLeakageSkippedTestArray[NUM_MEM_LEAK_SKIPPED_TESTS] =
                                        {"tgfBasicIpv42SharedVirtualRouter"};

/* test random run mode flag */
static GT_BOOL  isRandomRunMode = GT_FALSE;

/* force print of error info when the flag of utfCtx.continueFlag == GT_FALSE */
#define FORCE_PRINT_WHEN_NOT_CONTINUE_CNS
/*******************************************************************************
* utfSuitAdd
*
* DESCRIPTION:
*       This routine adds Suit.
*
* INPUTS:
*     suitPtr   -   (pointer to) a structure, where
*                   first 2 bytes define a pointer to a Suit name
*                   next blocks of 4 bytes define tests of this Suit, where
*                   first 2 bytes define Test name,
*                   last 2 bytes define pointer to test case function.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           -   Suit has been successully added.
*       GT_CREATE_ERROR -   it's impossible to add Suit because maximum count of
*                           tests has been already gained.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS utfSuitAdd
(
    IN const GT_VOID *suitPtr[]
)
{
    GT_STATUS   st = GT_OK;
    GT_U32      i = 0;
    const GT_CHAR  *suitNamePtr = suitPtr[i++];

    while ((GT_OK == st) && (suitPtr[i] != 0))
    {
        st = utfTestAdd((UTF_TEST_CASE_FUNC_PTR)suitPtr[i + 1], (const GT_CHAR*)suitPtr[i], suitNamePtr);
        i += 2;
    }
    return st;
}

/*******************************************************************************
* prvUtfGeneralStateLogDump
*
* DESCRIPTION:
*       dump the general state if exists.
*       this function dump to logger all that received from calling
*       PRV_UTF_GENERAL_STATE_INFO_MAC
*       the number of such messages defined in PRV_UTF_GENERAL_STATE_INFO_NUM_CNS
*
* INPUTS:
*       none
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       none
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_VOID prvUtfGeneralStateLogDump
(
    GT_VOID
)
{
    GT_U32  ii;
    GT_BOOL foundFirst = GT_FALSE;

    for(ii = 0 ; ii < PRV_UTF_GENERAL_STATE_INFO_NUM_CNS ; ii++)
    {
        if(prvUtfGeneralStateLogArr[ii].valid == GT_FALSE)
        {
            continue;
        }

        if(foundFirst == GT_FALSE)
        {
            foundFirst = GT_TRUE;
            PRV_UTF_LOG0_MAC("\n General state Dump: \n");
        }

        PRV_UTF_LOG2_MAC("%s%s",prvUtfGeneralStateLogArr[ii].buff,"\n");
    }

    if(foundFirst == GT_TRUE)
    {
        PRV_UTF_LOG0_MAC("General state Ended \n");
    }
}

/*******************************************************************************
* utfEqualVerify
*
* DESCRIPTION:
*       This routine is used for handling test failures (e != r) ,
*       error calculation.
*
* INPUTS:
*       e           -   expected value.
*       r           -   received value.
*       lineNum     -   number of line in test file where test failure
*                       has been found.
*       fileNamePtr -   (pointer to) name of test file where error has been
*                       found.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE     -   Pass condition was correct. Test may continue its flow.
*       GT_FALSE    -   Pass condition is invalid. Test is broken.
*
* COMMENTS:
*       Test cases must not call this function directly.
*       It is responsibility of UTF macro to continue test flow after failure
*       or not - is specified as an argument in utfRunTests routine.
*
*******************************************************************************/
GT_BOOL utfEqualVerify
(
    IN GT_U32   e,
    IN GT_U32   r,
    IN GT_32    lineNum,
    IN GT_CHAR  *fileNamePtr
)
{
#ifdef FORCE_PRINT_WHEN_NOT_CONTINUE_CNS
    UTF_LOG_OUTPUT_ENT origLogState = 0;
#endif /*FORCE_PRINT_WHEN_NOT_CONTINUE_CNS*/

    if (e != r)
    {
        utfCtx.errorFlag = GT_TRUE;
        utfCtx.testEntriesArray[utfCtx.currTestNum].errorCount++;

#ifdef FORCE_PRINT_WHEN_NOT_CONTINUE_CNS
        if(utfCtx.continueFlag == GT_FALSE)
        {
            /* we run until first fail , so let us have the printing regardless to
               global debug mode.
               because global debug mode maybe with HUGE amount of printings ... */
            origLogState = prvUtfLogPrintEnable(UTF_LOG_OUTPUT_SERIAL_ALL_E);
        }
#endif /*FORCE_PRINT_WHEN_NOT_CONTINUE_CNS*/

        PRV_UTF_LOG0_MAC("\n");
        PRV_UTF_LOG1_MAC("TEST %s FAILED\n", utfCtx.testEntriesArray[utfCtx.currTestNum].testNamePtr);
        PRV_UTF_LOG2_MAC("FAILURE! FileName: %s (line %d)\n", fileNamePtr, lineNum);
        PRV_UTF_LOG1_MAC("Expected: %d\n", e);
        PRV_UTF_LOG1_MAC("Received: %d\n", r);

        /* dump the general state if exists */
        prvUtfGeneralStateLogDump();

#ifdef FORCE_PRINT_WHEN_NOT_CONTINUE_CNS
        if(utfCtx.continueFlag == GT_FALSE)
        {
            /* restore original value */
            prvUtfLogPrintEnable(origLogState);
        }
#endif /*FORCE_PRINT_WHEN_NOT_CONTINUE_CNS*/

        return GT_FALSE;
    }
    return GT_TRUE;
}

/*******************************************************************************
* utfNotEqualVerify
*
* DESCRIPTION:
*       This routine is used for handling test failures (e == r), error
*       calculation.
*
* INPUTS:
*       e           -   expected value.
*       r           -   received value.
*       lineNum     -   number of line in test file where test failure
*                       has been found.
*       fileNamePtr -   (pointer to) name of test file where test failure
*                       has been found.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE     -   Pass condition was correct. Test may continue its flow.
*       GT_FALSE    -   Pass condition is invalid. Test is broken.
*
* COMMENTS:
*       Test cases must not call this function directly.
*       It is responsibility of UTF macro to continue test flow after failure
*       or not - is specified as an argument in utfRunTests routine.
*
*******************************************************************************/
GT_BOOL utfNotEqualVerify
(
    IN GT_U32   e,
    IN GT_U32   r,
    IN GT_32    lineNum,
    IN GT_CHAR  *fileNamePtr
)
{
    if (e == r)
    {
        utfCtx.errorFlag = GT_TRUE;
        utfCtx.testEntriesArray[utfCtx.currTestNum].errorCount++;

        PRV_UTF_LOG0_MAC("\n");
        PRV_UTF_LOG1_MAC("TEST %s FAILED\n", utfCtx.testEntriesArray[utfCtx.currTestNum].testNamePtr);
        PRV_UTF_LOG2_MAC("FAILURE! FileName: %s (line %d)\n", fileNamePtr, lineNum);
        PRV_UTF_LOG1_MAC("Expected to be not equal to: %d\n", e);
        PRV_UTF_LOG1_MAC("Received: %d\n", r);
        return GT_FALSE;
    }
    return GT_TRUE;
}

/*******************************************************************************
* utfFailureMsgLog
*
* DESCRIPTION:
*       This function is used for displaying failure information.
*
* INPUTS:
*       failureMsgPtr   -   (pointer to) failure message (format string).
*       argvPtr         -   (pointer to) vector of failure message arguments.
*       argc            -   number of failure message arguments.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE     -   Test may continue its flow.
*       GT_FALSE    -   Test must be interrupted.
*
* COMMENTS:
*        Test cases must not call this function directly.
*
*******************************************************************************/
GT_BOOL utfFailureMsgLog
(
    IN const GT_CHAR    *failureMsgPtr,
    IN const GT_VOID    *argvPtr[],
    IN GT_U32           argc
)
{
#ifdef FORCE_PRINT_WHEN_NOT_CONTINUE_CNS
    UTF_LOG_OUTPUT_ENT origLogState = 0;

    if(utfCtx.continueFlag == GT_FALSE)
    {
        /* we run until first fail , so let us have the printing regardless to
           global debug mode.
           because global debug mode maybe with HUGE amount of printings ... */
        origLogState = prvUtfLogPrintEnable(UTF_LOG_OUTPUT_SERIAL_ALL_E);
    }
#endif /*FORCE_PRINT_WHEN_NOT_CONTINUE_CNS*/

    PRV_UTF_LOG1_MAC("[UTF]: %s: ", utfCtx.testEntriesArray[utfCtx.currTestNum].testNamePtr);

    if (NULL == failureMsgPtr)
    {
        switch (argc)
        {
            case 0:
                PRV_UTF_LOG0_MAC("no parameters were defined\n");
                break;
            case 1:
                PRV_UTF_LOG1_MAC("%d\n", argvPtr[0]);
                break;
            case 2:
                PRV_UTF_LOG2_MAC("%d, %d\n", argvPtr[0], argvPtr[1]);
                break;
            case 3:
                PRV_UTF_LOG3_MAC("%d, %d, %d\n", argvPtr[0], argvPtr[1], argvPtr[2]);
                break;
            case 4:
                PRV_UTF_LOG4_MAC("%d, %d, %d, %d\n", argvPtr[0], argvPtr[1], argvPtr[2], argvPtr[3]);
                break;
            case 5:
                PRV_UTF_LOG5_MAC("%d, %d, %d, %d, %d\n", argvPtr[0], argvPtr[1], argvPtr[2], argvPtr[3], argvPtr[4]);
                break;
            case 6:
                PRV_UTF_LOG6_MAC("%d, %d, %d, %d, %d, %d\n", argvPtr[0], argvPtr[1], argvPtr[2], argvPtr[3], argvPtr[4], argvPtr[5]);
                break;
            case 7:
                PRV_UTF_LOG7_MAC("%d, %d, %d, %d, %d, %d, %d\n", argvPtr[0], argvPtr[1], argvPtr[2], argvPtr[3], argvPtr[4], argvPtr[5], argvPtr[6]);
                break;
            default:
                PRV_UTF_LOG1_MAC("too many arguments [%d]\n", argc);
                break;
        }
    }
    else
    {
        prvUtfLogMessage(failureMsgPtr, argvPtr, argc);
        PRV_UTF_LOG0_MAC("\n");
    }
    PRV_UTF_LOG0_MAC("\n");

#ifdef FORCE_PRINT_WHEN_NOT_CONTINUE_CNS
    if(utfCtx.continueFlag == GT_FALSE)
    {
        /* restore original value */
        prvUtfLogPrintEnable(origLogState);
    }
#endif /*FORCE_PRINT_WHEN_NOT_CONTINUE_CNS*/

    return utfCtx.continueFlag;
}

/*******************************************************************************
* utfTestsList
*
* DESCRIPTION:
*       This routine prints list of configured suits in case of NULL input argument
*       or print list of test cases inside given suit.
*
* INPUTS:
*       suitNamePtr -   (pointer to) suit name.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           -   Printing of suits was successful.
*       GT_NOT_FOUND    -   Given suit wasn't found in the UTF suit list.
*       GT_FAIL         -   General failure error. Should never happen.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS utfTestsList
(
    IN const GT_CHAR *suitNamePtr
)
{
    GT_U32 countIdx = 0;

    PRV_UTF_LOG0_MAC("\n");
    PRV_UTF_LOG0_MAC("[UTF]: utfTestsList: ***********************\n");

    if (NULL == suitNamePtr)
    {
        const GT_CHAR* lastSuitNamePtr = NULL;

        PRV_UTF_LOG0_MAC("[UTF]: suit name: [all]\n");
        PRV_UTF_LOG0_MAC("\n");

        for (countIdx = 0; countIdx < utfCtx.usedTestNum; ++countIdx)
        {
            const GT_CHAR *currSuitNamePtr = utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].suitNamePtr;

            /* check if current test needs to be skipped */
            if (utfSkipTestTypeBmp & (1 << utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].testType))
            {
                continue;
            }

            if (lastSuitNamePtr == 0 || (0 != cpssOsStrCmp(currSuitNamePtr, lastSuitNamePtr)))
            {
                PRV_UTF_LOG1_MAC("%s\n", currSuitNamePtr);
                lastSuitNamePtr = currSuitNamePtr;
            }
        }
    }
    else
    {
        PRV_UTF_LOG1_MAC("[UTF]: suit name: [%s]\n", suitNamePtr);
        PRV_UTF_LOG0_MAC("\n");

        for (countIdx = 0; countIdx < utfCtx.usedTestNum; countIdx++)
        {
            const GT_CHAR* currSuitNamePtr = utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].suitNamePtr;
            const GT_CHAR* currTestNamePtr = utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].testNamePtr;

            if (0 == cpssOsStrCmp(currSuitNamePtr, suitNamePtr))
            {
                PRV_UTF_LOG1_MAC("%s\n", currTestNamePtr);
            }
        }
    }

    PRV_UTF_LOG0_MAC("[UTF]: utfTestsList: ***********************\n");
    PRV_UTF_LOG0_MAC("\n");
    return GT_OK;
}

/*******************************************************************************
* utfTestsTypeList
*
* DESCRIPTION:
*       This routine prints all tests list of specific test type.
*
* INPUTS:
*       testType - (list of) type of tests, use 0 to set the end of list
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           -   Printing of suits was successful.
*       GT_NOT_FOUND    -   Given suit wasn't found in the UTF suit list.
*       GT_FAIL         -   General failure error. Should never happen.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS utfTestsTypeList
(
    IN UTF_TEST_TYPE_ENT    testType,
    IN ...
)
{
    GT_STATUS           st, st1  = GT_OK;
    va_list             argParam;
    GT_U32              index = 0;
    GT_U32              skipTestsBmp = 0;
    UTF_TEST_TYPE_ENT   testTypeArg = testType;


    /* init variable argument list */
    va_start(argParam, testType);

    /* go thru argument list */
    while (UTF_TEST_TYPE_NONE_E != testTypeArg)
    {
        /* set appropriate bit */
        skipTestsBmp |= 1 << testTypeArg;

        /* get next argument from list */
        testTypeArg = va_arg(argParam, UTF_TEST_TYPE_ENT);
    }

    /* free VA list pointer */
    va_end(argParam);

    /* disable not relevant tests */
    while (++index < UTF_TEST_TYPE_LAST_E)
    {
        /* check if appropriate bit is set */
        if (!((skipTestsBmp >> index) & 1))
        {
            /* skip not relevant package */
            st = utfSkipTests((UTF_TEST_TYPE_ENT) index);
            if (GT_OK != st)
            {
                PRV_UTF_LOG1_MAC("[UTF]: utfTestsTypeRun: failed [0x%X] to disable tests\n", st);

                return st;
            }
        }
    }

    /* print tests list */
    st = utfTestsList(0);
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfTestsTypeList: failed [0x%X] to list tests\n", st);

        st1 = st;
    }

    /* enable all tests */
    st = utfSkipTests(UTF_TEST_TYPE_NONE_E);
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfTestsTypeList: failed [0x%X] to enable tests\n", st);

        st1 = st;
    }

    return st1;
}

/*******************************************************************************
* utfTestsRun
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
GT_STATUS utfTestsRun
(
    IN const GT_CHAR *testPathPtr,
    IN GT_U32        numOfRuns,
    IN GT_BOOL       continueFlag
)
{
    GT_U32      i;
    GT_U32      suitExecTime;
    GT_STATUS   st = GT_OK;
    GT_STATUS   stSuits = GT_OK;

    /* Set the printing mode to print, in order to print the banner */
    prvUtfLogPrintEnable(UTF_LOG_OUTPUT_SERIAL_ALL_E);

    PRV_UTF_LOG0_MAC("\n");
    PRV_UTF_LOG0_MAC("[UTF]: utfTestsRun: ************************\n");

    if (NULL == testPathPtr)
    {
        PRV_UTF_LOG0_MAC("[UTF]: all suits\n");
    }
    else
    {
        PRV_UTF_LOG1_MAC("[UTF]: suite path [%s]\n", testPathPtr);
    }
    PRV_UTF_LOG1_MAC("[UTF]: number of loops [%d]\n", numOfRuns);
    PRV_UTF_LOG1_MAC("[UTF]: break after first failure [%d]\n", continueFlag);
    PRV_UTF_LOG1_MAC("[UTF]: random run mode [%d]\n", isRandomRunMode);
    PRV_UTF_LOG0_MAC("\n");

    /* check if number of cycle is zero */
    if (0 == numOfRuns)
    {
        st = GT_BAD_PARAM;
    }
    else
    {
        /* run requested suits in cycle */
        for (i = 0; (i < numOfRuns) && (GT_OK == st); i++)
        {
            PRV_UTF_LOG1_MAC("[UTF]: utfTestsRun: step [%d] **************\n", i);
            PRV_UTF_LOG0_MAC("\n");

            suitExecTime = cpssOsTime();
            st = utfSuitsRun(testPathPtr, continueFlag, utfStartFlag);
            suitExecTime = cpssOsTime() - suitExecTime;

            PRV_UTF_LOG1_MAC("[UTF]: suit running time [%d] seconds\n", suitExecTime);
            PRV_UTF_LOG0_MAC("\n");

            /* if some test fails during testing, error code is saved       */
            /* into another variable while st is restored to GT_OK          */
            /* because of it's not UTF error and can continue our testing.  */
            if (GT_UTF_TEST_FAILED == st)
            {
                stSuits = st;
                st = GT_OK;
            }
        }
    }

    PRV_UTF_LOG0_MAC("[UTF]: utfTestsRun: ************************\n");
    PRV_UTF_LOG0_MAC("\n");


    /* Test Run function may return two types of error codes.                   */
    /* First type is internal UTF error code, they have high priority and       */
    /* second type is special error code that indicates test failure,           */
    /* this error code can be used by upper layer to handle that tests fail.    */
    if (GT_OK == st)
    {
        st = stSuits;
    }
    return st;
}

/*******************************************************************************
* utfTestsTypeRun
*
* DESCRIPTION:
*       Runs all suits in case of NULL input path (testPathPtr),
*       or suit if suite name is specified as testPathPtr,
*       or specific test in the suit.
*
* INPUTS:
*       numOfRuns - defines how many times tests will be executed in loop
*       fContinue - defines will test be interrupted after first failure condition
*       testType   - (list of) type of tests, use 0 to set the end of list
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
GT_STATUS utfTestsTypeRun
(
    IN GT_U32               numOfRuns,
    IN GT_BOOL              fContinue,
    IN UTF_TEST_TYPE_ENT    testType,
    IN ...
)
{
    GT_STATUS           st, st1  = GT_OK;
    va_list             argParam;
    GT_U32              index = 0;
    GT_U32              skipTestsBmp = 0;
    UTF_TEST_TYPE_ENT   testTypeArg = testType;


    /* init variable argument list */
    va_start(argParam, testType);

    /* go thru argument list */
    while (UTF_TEST_TYPE_NONE_E != testTypeArg)
    {
        /* set appropriate bit */
        skipTestsBmp |= 1 << testTypeArg;

        /* get next argument from list */
        testTypeArg = va_arg(argParam, UTF_TEST_TYPE_ENT);
    }

    /* free VA list pointer */
    va_end(argParam);

    /* disable not relevant tests */
    while (++index < UTF_TEST_TYPE_LAST_E)
    {
        /* check if appropriate bit is set */
        if (!((skipTestsBmp >> index) & 1))
        {
            /* skip not relevant package */
            st = utfSkipTests((UTF_TEST_TYPE_ENT) index);
            if (GT_OK != st)
            {
                PRV_UTF_LOG1_MAC("[UTF]: utfTestsTypeRun: failed [0x%X] to disable tests\n", st);

                return st;
            }
        }
    }

    /* run tests */
    st = utfTestsRun(NULL, numOfRuns, fContinue);
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfTestsTypeRun: failed [0x%X] to run tests\n", st);

        st1 = st;
    }

    /* enable all tests */
    st = utfSkipTests(UTF_TEST_TYPE_NONE_E);
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfTestsTypeRun: failed [0x%X] to enable tests\n", st);

        st1 = st;
    }

    return st1;
}

/*******************************************************************************
* utfTestsStartRun
*
* DESCRIPTION:
*       Runs suits or specific tests from the start point.
*
* INPUTS:
*       testPathPtr -   (pointer to) name of start suite or test case to be run.
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
GT_STATUS utfTestsStartRun
(
    IN const GT_CHAR *testPathPtr,
    IN GT_U32        numOfRuns,
    IN GT_BOOL       fContinue
)
{
    GT_STATUS   st = GT_OK;


    /* enable start flag */
    utfStartFlag = GT_TRUE;

    /* run tests */
    st = utfTestsRun(testPathPtr, numOfRuns, fContinue);
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfTestsStartRun: failed [0x%X] to run tests\n", st);
    }

    /* disable start flag */
    utfStartFlag = GT_FALSE;

    return st;
}

/*******************************************************************************
* utfLogOutputSelect
*
* DESCRIPTION:
*       Switches logger output between serial interface and file.
*
* INPUTS:
*       eLogOutput  -   kind of log output
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           -   Output log interface was selected successfully.
*       GT_BAD_PARAM    -   Invalid output interface id was passed.
*       GT_FAIL         -   General failure error. Should never happen.
*
* COMMENTS:
*       By default output is done into serial interface.
*       Use this function if you want to have a log file.
*
*******************************************************************************/
GT_STATUS utfLogOutputSelect
(
    IN UTF_LOG_OUTPUT_ENT eLogOutput
)
{
    utfLogOutputSelectMode = eLogOutput;

    return GT_OK;
}

/*******************************************************************************
* utfLogOpen
*
* DESCRIPTION:
*       Opens logger with defined file name.
*
* INPUTS:
*       fileName  -   log output file name
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           -   logger has been successfully opened.
*       GT_BAD_PARAM    -   Invalid output interface id was passed.
*       GT_FAIL         -   General failure error. Should never happen.
*
* COMMENTS:
*       Available kinds of log output are serial interface and file.
*******************************************************************************/
GT_STATUS utfLogOpen
(
    IN const GT_CHAR* fileName
)
{
    return prvUtfLogOpen(fileName);
}

/*******************************************************************************
* utfLogClose
*
* DESCRIPTION:
*       This routine closes logger.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
*       GT_OK           -   logger has been successfully closed.
*       GT_FAIL         -   General failure error. Should never happen.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS utfLogClose
(
    GT_VOID
)
{
    return prvUtfLogClose();
}

/*******************************************************************************
* utfPreInitPhase
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
GT_STATUS utfPreInitPhase
(
    GT_VOID
)
{
    GT_STATUS   st = GT_OK;

    PRV_UTF_LOG0_MAC("\n");
    PRV_UTF_LOG0_MAC("[UTF]: utfInit: ************************\n");

    /* init extras, like active devices array, ports */
    st = prvUtfExtrasInit();
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfInit: failed [0x%X] init devices and ports\n", st);
    }

    return st;
}

/*******************************************************************************
* utfPostInitPhase
*
* DESCRIPTION:
*       This routine finish initializing UTF core. It must be called last.
*
* INPUTS:
*       st - return code from UTF initialize phase.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       It's good hint to insert this function inside some global configuration
*       function, as part of initialization phase.
*
*******************************************************************************/
GT_VOID utfPostInitPhase
(
    GT_STATUS st
)
{
    GT_U16      iter = 0;

    /* init index array */
    for (iter = 0; iter < utfCtx.usedTestNum; iter++)
    {
        utfCtx.testIndexArray[iter] = iter;
    }

    /* init flag */
    isRandomRunMode = GT_FALSE;

    if (GT_OK == st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfInit: OK, number of tests [%d]\n", utfCtx.usedTestNum);
        PRV_UTF_LOG0_MAC("Run utfHelp 0 for help about UTF usage\n");
    }
    else
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfInit: FAILED, declaration of suits fails: [%d]\n", st);
    }
    PRV_UTF_LOG0_MAC("[UTF]: utfInit: ************************\n");
    PRV_UTF_LOG0_MAC("\n");

    utfHelp(NULL);
}

/*******************************************************************************
* utfInit
*
* DESCRIPTION:
*       This routine initializes UTF core.
*       It must be called after utfPreInitPhase.
*
* INPUTS:
*       firstDevNum - device number of first device.
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
GT_STATUS utfInit
(
    GT_U8 firstDevNum
)
{
    GT_STATUS   st   = GT_OK;
    GT_U32      sec  = 0;
    GT_U32      nsec = 0;

    /* get initial start time */
    st = cpssOsTimeRT(&sec, &nsec);
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfInit: failed [0x%X] to get system time\n", st);
    }

    startTimeSec  = sec;
    startTimeNsec = nsec;

#ifdef IMPL_TGF
    /* initialize TGF configuration */
    st = prvTgfCommonInit(firstDevNum);
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfInit: failed [0x%X] to init TGF\n", st);
    }
#endif /* IMPL_TGF */

    return utfAllSuitsDeclare();
}

/*******************************************************************************
* utfHelp
*
* DESCRIPTION:
*       This routine log a help about the function, if function name is specified
*       as input argument. In case of NULL input argument this function displays
*       all available UTF functions.
*
* INPUTS:
*       utfFuncNamePtr - (pointer to) UTF-function name.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           -   Help logs information successfully.
*       GT_BAD_PARAM    -   Required function doesn't exist.
*       GT_FAIL         -   General failure error. Should never happen.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS utfHelp
(
    IN const GT_CHAR *utfFuncNamePtr
)
{
    PRV_UTF_LOG0_MAC("\n");
    PRV_UTF_LOG0_MAC("[UTF]: utfHelp: ****************************\n");

    if (NULL == utfFuncNamePtr)
    {
        PRV_UTF_LOG0_MAC("*****UTF functions:*****\n");
        PRV_UTF_LOG0_MAC("utfHelp       -   to log a help about the function.\n");
        PRV_UTF_LOG0_MAC("utfTestsList  -   to log list of all configured (available) suits and tests.\n");
        PRV_UTF_LOG0_MAC("utfTestsTypeList    - to log list of all configured suits by given UT type list (generic, enhanced, etc.)\n");
        PRV_UTF_LOG0_MAC("utfTestsRun   -   to run suits and tests\n");
        PRV_UTF_LOG0_MAC("utfTestsTypeRun     - to run all UT by given UT type list (generic, enhanced, etc.)\n");
        PRV_UTF_LOG0_MAC("utfLogOutputSelect  - to change output log to serial or file \n");
        PRV_UTF_LOG0_MAC("utfLogOpen          - to open logger with defined file name\n");
        PRV_UTF_LOG0_MAC("utfLogClose         - to close logger\n");
        PRV_UTF_LOG0_MAC("utfRandomRunModeSet - to set test's random run mode\n");
        PRV_UTF_LOG0_MAC("utfTestRunFirstSet  - to put specific test to be run first\n");
        PRV_UTF_LOG0_MAC("\n");

        PRV_UTF_LOG0_MAC("*****Examples:*****\n");
        PRV_UTF_LOG0_MAC("utfHelp 0                - to printout this help info.\n");
        PRV_UTF_LOG0_MAC("utfHelp \"utfTestsList\"   - to get detailed info about command \"utfTestsList\"\n");
        PRV_UTF_LOG0_MAC("utfTestsList 0           - to log list of all configured (available) suits.\n");
        PRV_UTF_LOG0_MAC("utfTestsList \"tgfBridge\" - to log list of test cases inside suit \"tgfBridge\".\n");
        PRV_UTF_LOG0_MAC("utfTestsTypeList 6, 0 - to log list of all Enhanced UTs\n");
        PRV_UTF_LOG0_MAC("utfTestsTypeList 1, 6, 0 - to log list of all Generic and Enhanced UTs\n");
        PRV_UTF_LOG0_MAC("utfTestsRun 0,1,1           - to run all suits once with no interrupts after first failure\n");
        PRV_UTF_LOG0_MAC("utfTestsRun \"tgfBridge\",1,1 - to run all tests from suit \"tgfBridge\" once\n");
        PRV_UTF_LOG0_MAC("utfTestsRun \"tgfBridge.tgfBasicDynamicLearning\",1,1 - to run test \"tgfBasicDynamicLearning\" from suit \"tgfBridge\" once\n");
        PRV_UTF_LOG0_MAC("utfTestsRun \".tgfBasicDynamicLearning\",1,1 - to run test \"tgfBasicDynamicLearning\" once\n");
        PRV_UTF_LOG0_MAC("utfTestsTypeRun 1, 1, 6, 0 - to run all Enhanced UT  \n");
        PRV_UTF_LOG0_MAC("utfTestsTypeRun 1, 1, 1, 6, 0 - to run all Generic and Enhanced UT  \n");
        PRV_UTF_LOG0_MAC("utfLogOutputSelect 0  - to change output log to serial \n");
        PRV_UTF_LOG0_MAC("utfLogOutputSelect 2  - to change output log to file\n");
        PRV_UTF_LOG0_MAC("utfLogOpen \"results.log\" - to set logger into file \"results.log\"\n");
        PRV_UTF_LOG0_MAC("utfLogClose - to close opened logger\n");
        PRV_UTF_LOG0_MAC("utfRandomRunModeSet 1, 5 - to enable random run mode with seed 5 for random generator\n");
        PRV_UTF_LOG0_MAC("utfTestRunFirstSet \"tgfBasicDynamicLearning\" - to run \"tgfBasicDynamicLearning\" test first\n");
        PRV_UTF_LOG0_MAC("\n");
        PRV_UTF_LOG0_MAC("Run utfHelp \"utfTestsList\" for example to get detailed info about command\n");
    }
    else
    {
        if (0 == cpssOsStrCmp("utfHelp", utfFuncNamePtr))
        {
            PRV_UTF_LOG0_MAC("utfHelp 0                     -   to log all available UTF functions. \n");
            PRV_UTF_LOG0_MAC("utfHelp <UTF-function name>   -   to log a help about the function.\n");
        }
        else if (0 == cpssOsStrCmp("utfTestsList", utfFuncNamePtr))
        {
            PRV_UTF_LOG0_MAC("utfTestsList 0                -   to log list of all configured (available) suits.\n");
            PRV_UTF_LOG0_MAC("utfTestsList <Suit name>      -   to log list of test cases inside given suit.\n");
        }
        else if (0 == cpssOsStrCmp("utfTestsRun", utfFuncNamePtr))
        {
            PRV_UTF_LOG0_MAC("utfTestsRun 0,1,1             -   to run all suits with defaults configuration.\n");
            PRV_UTF_LOG0_MAC("utfTestsRun <Name of suit or test case>, <Number of runs>, <Flag to continue after first failure> \n");
            PRV_UTF_LOG0_MAC("\n");
            PRV_UTF_LOG0_MAC("For example:\n");
            PRV_UTF_LOG0_MAC("utfTestsRun 0,1,1 - to run all suits once with no interrupts after first failure\n");
            PRV_UTF_LOG0_MAC("utfTestsRun \"TestSuit\",2,1 - to run all tests from suit \"TestSuit\" two times\n");
            PRV_UTF_LOG0_MAC("utfTestsRun \"TestSuit.TestOne\",1,1 - to run test \"TestOne\" from suit \"TestSuit\" once\n");

        }
        else if (0 == cpssOsStrCmp("utfLogOutputSelect", utfFuncNamePtr))
        {
            PRV_UTF_LOG0_MAC("utfLogOutputSelect <where to output> - to change output log to log, serial or file \n");
            PRV_UTF_LOG0_MAC("For example:\n");
            PRV_UTF_LOG0_MAC("utfLogOutputSelect 0 - to change output log to serial \n");
        }
        else if (0 == cpssOsStrCmp("utfTestsTypeRun", utfFuncNamePtr))
        {
            PRV_UTF_LOG0_MAC("utfTestsTypeRun <Number of runs>, <Continue flag>, <List of test types>, 0 \n");
            PRV_UTF_LOG0_MAC("For example:\n");
            PRV_UTF_LOG0_MAC("utfTestsTypeRun 1,1,\"TestType1\",\"TestType2\",0 - to run all UTs from packages \"TestType1\" and \"TestType2\"\n");
        }
        else
        {
            PRV_UTF_LOG1_MAC("Unknown command [%s]\n", utfFuncNamePtr);
        }
    }
    PRV_UTF_LOG0_MAC("[UTF]: utfHelp: ****************************\n");
    PRV_UTF_LOG0_MAC("\n");
    return GT_OK;
}

/******************************************************************************
* UTF private functions implementation
******************************************************************************/

/*******************************************************************************
* utfSuitStatsShow
*
* DESCRIPTION:
*     Displays final statistics about suit, is failed, number of errors
*     list of failed test cases for the suit
*
* INPUTS:
*     suitNamePtr   -   name of suit to show statistics
*
* OUTPUTS:
*     none
*
* RETURNS:
*     none
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_VOID utfSuitStatsShow
(
    const GT_CHAR *suitNamePtr
)
{
    GT_U32 countIdx;

    if (GT_TRUE == utfCtx.errorFlag)
    {
        PRV_UTF_LOG1_MAC("**** SUIT %s FAILED\n", suitNamePtr);

        for (countIdx = 0; countIdx < utfCtx.usedTestNum; countIdx++)
        {
            if ((0 == cpssOsStrCmp(suitNamePtr, utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].suitNamePtr)) &&
                (utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].errorCount > 0))
            {
                PRV_UTF_LOG2_MAC("TEST %s FAILED: %d errors\n",
                         utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].testNamePtr,
                         utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].errorCount);
            }
        }
    }
    else
    {
        PRV_UTF_LOG1_MAC("**** SUIT %s SUCCEEDED\n", suitNamePtr);
    }
    PRV_UTF_LOG0_MAC("\n");

    /* print total not skipped tests run */
    PRV_UTF_LOG1_MAC("[UTF]: number of tests run in current suit [%d]\n\n",
                     utfTestsNumGet(suitNamePtr) - prvUtfSkipTestsNumGet());

    /* reset skip tests counter */
    prvUtfSkipTestsNumReset();

    utfCtx.errorFlag = GT_FALSE;
}

/*******************************************************************************
* utfFinalStatsShow
*
* DESCRIPTION:
*     Displays final statistics about all suits, list of failed suits and
*     list of failed test cases for the suits
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     none
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_VOID utfFinalStatsShow
(
    GT_VOID
)
{
    GT_U32 countIdx;
    GT_BOOL isAllSuitesPassed = GT_TRUE;

    PRV_UTF_LOG0_MAC("******** UTF REPORT *********\n");

    for (countIdx = 0; countIdx < utfCtx.usedTestNum; countIdx++)
    {
        if (utfCtx.testEntriesArray[countIdx].errorCount > 0)
        {
            PRV_UTF_LOG3_MAC("%s.%s failed, number of errors %d\n",
                             utfCtx.testEntriesArray[countIdx].suitNamePtr,
                             utfCtx.testEntriesArray[countIdx].testNamePtr,
                             utfCtx.testEntriesArray[countIdx].errorCount);

            isAllSuitesPassed = GT_FALSE;
        }
    }
    if (GT_TRUE == isAllSuitesPassed)
    {
        PRV_UTF_LOG0_MAC("All tests were successful\n");
    }
    PRV_UTF_LOG0_MAC("\n");
}

/*******************************************************************************
* utfTestAdd
*
* DESCRIPTION:
*     Add test case to utf internal database
*
* INPUTS:
*     testFuncPtr   -   pointer to test case function
*     testNamePtr   -   name of the test case in the suit
*     suitNamePtr   -   name of the suit, unique for all suits
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK             - on success.
*     GT_CREATE_ERROR   - there is no more free slot for the test
*
* COMMENTS:
*     In case of GT_CREATE_ERROR you need to increase UTF_MAX_NUM_OF_TESTS_CNS
*
*******************************************************************************/
static GT_STATUS utfTestAdd
(
    IN UTF_TEST_CASE_FUNC_PTR   testFuncPtr,
    IN const GT_CHAR            *testNamePtr,
    IN const GT_CHAR            *suitNamePtr)
{
    if (utfCtx.usedTestNum >= UTF_MAX_NUM_OF_TESTS_CNS)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfTestAdd: failed add test [%s]\n", testNamePtr);
        return GT_CREATE_ERROR;
    }

    utfCtx.testEntriesArray[utfCtx.usedTestNum].testFuncPtr = testFuncPtr;
    utfCtx.testEntriesArray[utfCtx.usedTestNum].testNamePtr = testNamePtr;
    utfCtx.testEntriesArray[utfCtx.usedTestNum].suitNamePtr = suitNamePtr;
    utfCtx.testEntriesArray[utfCtx.usedTestNum].testType    = utfTestType;
    utfCtx.usedTestNum++;
    return GT_OK;
}

/*******************************************************************************
* utfPreTestRun
*
* DESCRIPTION:
*     This function is called before run each test case
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     none
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_VOID utfPreTestRun
(
    GT_VOID
)
{
    GT_STATUS   st   = GT_OK;
    GT_U32      sec  = 0;
    GT_U32      nsec = 0;
    GT_U32      ii;
    st = cpssOsTimeRT(&sec, &nsec);
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfPreTestRun: failed [0x%X] to get system time\n", st);
    }

    utfCtx.testEntriesArray[utfCtx.currTestNum].testExecTimeSec = sec;
    utfCtx.testEntriesArray[utfCtx.currTestNum].testExecTimeNsec = nsec;

    if (nsec < startTimeNsec)
    {
        nsec += 1000000000;
        sec--;
    }

    PRV_UTF_LOG2_MAC("[UTF]: test start after [%d.%09d] sec\n",
                     sec  - startTimeSec,
                     nsec - startTimeNsec);
    utfMemoryHeapCounterInit();

    /* by default -- all test must state explicitly if to use port groups bmp
       so before test starts ... clear the flag */
    usePortGroupsBmp = GT_FALSE;
    /* clear also the BMP */
    currPortGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    /* reset the 'General info' in the DB , meaning each test must state it's
       own 'General info' */
    for(ii = 0 ; ii < PRV_UTF_GENERAL_STATE_INFO_NUM_CNS; ii++)
    {
        prvUtfGeneralStateLogArr[ii].valid = GT_FALSE;
    }
}

/*******************************************************************************
* utfPostTestRun
*
* DESCRIPTION:
*     This function is called after test case execution
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     none
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_VOID utfPostTestRun
(
    GT_VOID
)
{
    GT_STATUS   st   = GT_OK;
    GT_U32      sec  = 0;
    GT_U32      nsec = 0;
    GT_U32      mem  = 0;
    st = cpssOsTimeRT(&sec, &nsec);
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfPostTestRun: failed [0x%X] to get system time\n", st);
    }

    if (nsec < utfCtx.testEntriesArray[utfCtx.currTestNum].testExecTimeNsec)
    {
        nsec += 1000000000;
        sec--;
    }
    utfCtx.testEntriesArray[utfCtx.currTestNum].testExecTimeNsec = nsec -
        utfCtx.testEntriesArray[utfCtx.currTestNum].testExecTimeNsec;
    utfCtx.testEntriesArray[utfCtx.currTestNum].testExecTimeSec = sec -
        utfCtx.testEntriesArray[utfCtx.currTestNum].testExecTimeSec;

    st = utfMemoryHeapLeakageStatusGet(&mem);
    if (GT_OK != st)
    {
        utfCtx.errorFlag = GT_TRUE;
        utfCtx.testEntriesArray[utfCtx.currTestNum].errorCount++;
        PRV_UTF_LOG1_MAC("[UTF]: Memory leak detected: %d bytes \n", mem);
    }
}

/*******************************************************************************
* utfTestNumRun
*
* DESCRIPTION:
*     This function is called to run test with defined number
*
* INPUTS:
*     testNum   -   test's number to run
*
* OUTPUTS:
*     none
*
* RETURNS:
*     none
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_VOID utfTestNumRun
(
    IN GT_U32   testNum
)
{
    GT_CHAR    suitTestName[2 * UTF_MAX_TEST_PATH_LEN_CNS + 1] = {0};


    /* create suitName.TestName for output log */
    if (GT_FALSE == isRandomRunMode)
    {
        cpssOsStrCpy(suitTestName, utfCtx.testEntriesArray[testNum].testNamePtr);
    }
    else
    {
        /* add suitName */
        cpssOsStrCpy(suitTestName, utfCtx.testEntriesArray[testNum].suitNamePtr);

        /* add separator dot */
        suitTestName[cpssOsStrlen(suitTestName)] = '.';

        /* add testName */
        cpssOsStrCpy(suitTestName + cpssOsStrlen(suitTestName),
                      utfCtx.testEntriesArray[testNum].testNamePtr);
    }

    PRV_UTF_LOG1_MAC("******** TEST %s STARTED\n", suitTestName);

    utfCtx.currTestNum = testNum;

    utfPrintKeepAlive();

    /* run unit test */
    utfPreTestRun();
    utfCtx.testEntriesArray[testNum].testFuncPtr();
    utfPostTestRun();

    /* check if the test finished succeeded */
    if (GT_FALSE == prvUtfSkipTestsFlagGet())
    {
    if (utfCtx.testEntriesArray[testNum].errorCount > 0)
    {
        PRV_UTF_LOG1_MAC("******** TEST %s FAILED\n", suitTestName);
    }
    else
    {
        PRV_UTF_LOG1_MAC("******** TEST %s SUCCEEDED\n", suitTestName);
    }
    }
    else
    {
        PRV_UTF_LOG1_MAC("******** TEST %s SKIPPED\n", suitTestName);

        /* restore skip test flag */
        prvUtfSkipTestsFlagReset();
    }

    PRV_UTF_LOG2_MAC("[UTF]: test running time [%d.%09d] sec\n",
            utfCtx.testEntriesArray[testNum].testExecTimeSec,
            utfCtx.testEntriesArray[testNum].testExecTimeNsec);

    PRV_UTF_LOG0_MAC("\n");
}

/*************************************************************************************
* utfLogResultRun
*
* DESCRIPTION:
*     This function executes result file logging.
*
* INPUTS:
*     paramArrayPtr - array of parameters, that passed to result.txt
*
* OUTPUTS:
*     paramDeltaComparePtr - array of parameters, with delta compare result
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
************************************************************************************/
GT_STATUS utfLogResultRun
(
    IN  UTF_RESULT_PARAM_STC         *paramArrayPtr,
    IN  GT_U8                         numberParam,
    OUT GT_BOOL                      *paramDeltaComparePtr
)
{
    GT_32       tempLogOutputValue;         /* this variable store main utfLogOutputSelectMode*/
    GT_U8       paramCounter;               /* counter of array parameters*/
    GT_BOOL     fileExists;                 /* true if file alredy exists, false if that is new file*/
    GT_U32      resultCmp;                  /* result of campare two parameters in persent*/
    UTF_RESULT_PARAM_STC *compareArrayPtr;  /* array of parameters from existing file*/
    GT_U32      st = GT_OK;                 /* functions status return*/
    GT_BOOL     memoryError = GT_FALSE;     /* memory error status */

    CPSS_NULL_PTR_CHECK_MAC(paramArrayPtr);
    CPSS_NULL_PTR_CHECK_MAC(paramDeltaComparePtr);

    compareArrayPtr = cpssOsMalloc(numberParam * sizeof(UTF_RESULT_PARAM_STC));

    /* Checking memory allocate error */
    if (NULL == compareArrayPtr)
    {
        memoryError = GT_TRUE;
    }

    /* Save old output mode value to use standart UTF output functions */
    tempLogOutputValue = utfLogOutputSelectMode;
    prvUtfLogPrintEnable(UTF_LOG_OUTPUT_FILE_E);

    /* Checking if file pointer already busy*/
    if (NULL != fpResult)
    {
        st = GT_ALREADY_EXIST;
        goto exit;
    }

    /* Open file for reading text*/
    fpResult = fopen(UTF_RESULT_FILENAME_CNS, "rt");

    /* Checking if file already exists*/
    if (NULL == fpResult)
    {
        fileExists = GT_FALSE;
    }
    else
    {
        fileExists = GT_TRUE;
        fclose(fpResult);
        fpResult = NULL;
    }

    /* Begin work with parameters */
    if (fileExists == GT_TRUE)
    {
        /* ------------------------PRINT OLD PARAMS----------------------------------*/
        /* Read all parameters from file to compareArrayPtr array */
        st = prvUtfLogReadParam(UTF_RESULT_FILENAME_CNS, numberParam, compareArrayPtr);
        /* Check if read completed successfully*/
        if (st != GT_OK)
        {
            goto exit;
        }

        /*Open file to write compare results */
        st = utfLogOpen(UTF_RESULT_OF_COMPARE_FILENAME_CNS);
        /* Check if file open completed successfully*/
        if (st != GT_OK)
        {
            goto exit;
        }

        PRV_UTF_LOG0_MAC("---------------------Old values:-------------------------\n");

        /* Printing all old parameters names and values to file and stdout*/
        for (paramCounter = 0; paramCounter < numberParam; paramCounter++)
        {
            if (osStrlen(compareArrayPtr[paramCounter].paramName) > 0)
            {
                PRV_UTF_LOG2_MAC("%s\t%9d\n", compareArrayPtr[paramCounter].paramName,
                                              compareArrayPtr[paramCounter].paramValue);
            }
        }

        PRV_UTF_LOG0_MAC("--------------------New values:-------------------------\n");

        /* Close result of compare file */
        st = utfLogClose();
        if (st != GT_OK)
        {
            goto exit;
        }
    }

    /* ---------------------------PRINT NEW PARAMS-----------------------------------*/
    /* Open results file to write new parameters name and values, old parameters will be lost */
    st = utfLogOpen(UTF_RESULT_FILENAME_CNS);
    if (st != GT_OK)
    {
         return st;
    }

    /* Iterate with all parameters */
    for (paramCounter = 0; paramCounter < numberParam; paramCounter++)
    {
        if (osStrlen(paramArrayPtr[paramCounter].paramName) > 0)
        {
            PRV_UTF_LOG2_MAC("%s\t%9d\n", paramArrayPtr[paramCounter].paramName,
                                          paramArrayPtr[paramCounter].paramValue);

            /* Initialize paramDeltaComparePtr array to return to the test*/
            paramDeltaComparePtr[paramCounter] = GT_TRUE;
        }
    }

    /*Close results file and check it well*/
    st = utfLogClose();
    if (st != GT_OK)
    {
        goto exit;
    }

    if (fileExists == GT_TRUE)
    {
        /* -----------------PRINT RESULT OF COMPARE---------------------------------*/
        st = utfLogOpen(UTF_RESULT_OF_COMPARE_FILENAME_CNS);
        if (st != GT_OK)
        {
            goto exit;
        }

        PRV_UTF_LOG0_MAC("-------------------Result of compare:-------------------\n");

        /* Iterate with all params */
        for (paramCounter = 0; paramCounter < numberParam; paramCounter++)
        {
            if (osStrlen(compareArrayPtr[paramCounter].paramName) > 0 &&
                0 == cpssOsMemCmp(compareArrayPtr[paramCounter].paramName,
                                  paramArrayPtr[paramCounter].paramName,
                                  sizeof(compareArrayPtr[paramCounter].paramName)))
            {
                /*----------------Checking if values equal---------------------------------------------*/
                if (compareArrayPtr[paramCounter].paramValue == paramArrayPtr[paramCounter].paramValue)
                {
                    PRV_UTF_LOG2_MAC("Parameter \"%s\" have the same value   %9d\n",
                                        compareArrayPtr[paramCounter].paramName,
                                        compareArrayPtr[paramCounter].paramValue);

                    paramDeltaComparePtr[paramCounter] = GT_TRUE;
                }

                /*----------------Checking if new value bigger-----------------------------------------*/
                if (compareArrayPtr[paramCounter].paramValue > paramArrayPtr[paramCounter].paramValue)
                {
                    if (0 != paramArrayPtr[paramCounter].paramValue)
                    {
                        resultCmp = (((compareArrayPtr[paramCounter].paramValue * 100)/
                                       paramArrayPtr[paramCounter].paramValue) - 100);
                        PRV_UTF_LOG3_MAC("Parameter \"%s\" is bigger by  \t%5d%% \t(current value is \t%d)\n",
                                            compareArrayPtr[paramCounter].paramName,
                                            resultCmp,
                                            paramArrayPtr[paramCounter].paramValue);

                        paramDeltaComparePtr[paramCounter] =
                            (resultCmp > paramArrayPtr[paramCounter].paramDelta) ? GT_FALSE : GT_TRUE;
                    }
                    else
                    {
                        PRV_UTF_LOG1_MAC("Error of compare, parameter \"%s\" equal 0.\n",
                                            paramArrayPtr[paramCounter].paramName);
                    }
                }

                /*----------------Checking if new value less------------------------------------------*/
                if (compareArrayPtr[paramCounter].paramValue < paramArrayPtr[paramCounter].paramValue)
                {
                    if (0 != paramArrayPtr[paramCounter].paramValue)
                    {
                        resultCmp = (((paramArrayPtr[paramCounter].paramValue * 100)/
                                       compareArrayPtr[paramCounter].paramValue) - 100);

                        PRV_UTF_LOG3_MAC("Parameter \"%s\" is smaller by \t%5d%% \t(current value is \t%d)\n",
                                            compareArrayPtr[paramCounter].paramName,
                                            resultCmp,
                                            paramArrayPtr[paramCounter].paramValue);

                        paramDeltaComparePtr[paramCounter] =
                            (resultCmp > paramArrayPtr[paramCounter].paramDelta) ? GT_FALSE : GT_TRUE;
                    }
                    else
                    {
                        PRV_UTF_LOG1_MAC("Error of compare, parameter \"%s\" equal 0.\n",
                                            paramArrayPtr[paramCounter].paramName);
                    }
                }
            }
        }

        st = utfLogClose();
    }

    /* Restore default output mode */
    prvUtfLogPrintEnable(tempLogOutputValue);

exit:
    if (GT_FALSE == memoryError)
    {
        cpssOsFree(compareArrayPtr);
    }

    return st;
}

/*************************************************************************************
* utfStartTimer
*
* DESCRIPTION:
*     This function starts UTF timer.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
************************************************************************************/
GT_STATUS utfStartTimer
(
    GT_VOID
)
{
    GT_STATUS   st   = GT_OK;

    GT_U32      sec  = 0;
    GT_U32      nsec = 0;

    st = cpssOsTimeRT(&sec, &nsec);
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfPreTestRun: failed [0x%X] to get system time\n", st);
    }

    utfTimer.codeExecTimeSec = sec;
    utfTimer.codeExecTimeNsec = nsec;

    return st;
}

/*************************************************************************************
* utfStopTimer
*
* DESCRIPTION:
*     This function stop UTF timer and return time measure in miliseconds.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     timeElapsedPtr - pointer to time elapsed value
*
* RETURNS:
*     GT_OK          - on success.
*     GT_FAIL        - on failure.
*
* COMMENTS:
*     none
*
************************************************************************************/
GT_STATUS utfStopTimer
(
    OUT GT_U32   *timeElapsedPtr
)
{
    GT_STATUS   st   = GT_OK;
    GT_U32      sec  = 0;
    GT_U32      nsec = 0;

    CPSS_NULL_PTR_CHECK_MAC(timeElapsedPtr);

    if (utfTimer.codeExecTimeSec == 0 && utfTimer.codeExecTimeNsec == 0)
    {
        return GT_FAIL;
    }

    st = cpssOsTimeRT(&sec, &nsec);
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfPostTestRun: failed [0x%X] to get system time\n", st);
    }

    if (nsec < utfTimer.codeExecTimeNsec)
    {
        nsec += 1000000000;
        sec--;
    }
    utfTimer.codeExecTimeNsec = nsec - utfTimer.codeExecTimeNsec;
    utfTimer.codeExecTimeSec  = sec  - utfTimer.codeExecTimeSec;

    *timeElapsedPtr = utfTimer.codeExecTimeSec * 1000 + utfTimer.codeExecTimeNsec / 1000000;

    utfTimer.codeExecTimeSec = 0;
    utfTimer.codeExecTimeNsec = 0;

    return st;
}

/*************************************************************************************
* utfMemoryHeapCounterInit
*
* DESCRIPTION:
*     This function initialize memory heap counter.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     none
*
* COMMENTS:
*     Should be called before utfMemoryHeapStatusGet()
*
************************************************************************************/
GT_VOID utfMemoryHeapCounterInit
(
    GT_VOID
)
{
    GT_U32 ii;
    GT_U32 res = 1;
    for (ii = 0; ii < NUM_MEM_LEAK_SKIPPED_TESTS; ii++)
    {
        res = cpssOsStrCmp(utfCtx.testEntriesArray[utfCtx.currTestNum].testNamePtr,
                            memLeakageSkippedTestArray[ii]);
        if (res == 0)
        {
            break;
        }
    }
    if (res != 0)
    {
        if (UTF_TEST_TYPE_TRAFFIC_E == utfCtx.testEntriesArray[utfCtx.currTestNum].testType)
        {
            /* for enhanced ut tests */
            /* init check for memory leaks */
            osMemStartHeapAllocationCounter();
        }
    }
}
/*************************************************************************************
* utfMemoryHeapLeakageStatusGet
*
* DESCRIPTION:
*     This function get memory heap leakage status.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     memPtr - (pointer to) memory leak size value
*
* RETURNS:
*     GT_OK      - on success (no memory leak).
*     GT_BAD_PTR - on null pointer
*     GT_FAIL    - on failure (memory leak detected).
*
* COMMENTS:
*     It is delta of current allocated bytes number and the value of allocation
*     counter set by previous utfMemoryHeapStatusInit() function
*
************************************************************************************/
GT_STATUS utfMemoryHeapLeakageStatusGet
(
    OUT GT_U32 *memPtr
)
{
    GT_U32 ii;
    GT_U32 res = 1;
    /* check for null-pointer */
    CPSS_NULL_PTR_CHECK_MAC(memPtr);
    for (ii = 0; ii < NUM_MEM_LEAK_SKIPPED_TESTS; ii++)
    {
        res = cpssOsStrCmp(utfCtx.testEntriesArray[utfCtx.currTestNum].testNamePtr,
                            memLeakageSkippedTestArray[ii]);
        if (res == 0)
        {
            break;
        }
    }
    if (res != 0)
    {
        if (UTF_TEST_TYPE_TRAFFIC_E == utfCtx.testEntriesArray[utfCtx.currTestNum].testType)
        {
            /* get current allocated bytes number */
            *memPtr = osMemGetHeapAllocationCounter();
            /*check if delta not equal zero (memory leak)*/
            return (*memPtr != 0) ? GT_FAIL : GT_OK;
        }
    }
    return GT_OK;
}
/*******************************************************************************
* utfTestTask
*
* DESCRIPTION:
*     This function is called in single task for run each test
*
* INPUTS:
*     paramPtr   -   pointer to input parameter
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK    -   on success
*     GT_FAIL  -   on error
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_STATUS __TASKCONV utfTestTask
(
    IN GT_VOID *paramPtr
)
{
    GT_STATUS   st = GT_OK;
    GT_U32      currTestNum = 0;

    /* get input parameter */
    currTestNum = *((GT_U32*) paramPtr);

    /* run UT */
    utfTestNumRun(currTestNum);

    /* signal semaphore */
    st = cpssOsSigSemSignal(utfTestSemId);
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfTestTask: failed [0x%X] to signal semaphore\n", st);
    }

    return st;
}

/*******************************************************************************
* utfSuitsRun
*
* DESCRIPTION:
*     This function executes all declared test cases for specific test suits.
*     Test suits to execute are defined by test path parameter.
*
* INPUTS:
*     testPathPtr   -   test path, may contain NULL for all suits, suit, test case
*     continueFlag  -   define test flow behaviour, should it be interrupted after
*                       first failure
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK                 - on success.
      GT_UTF_TEST_FAILED    -   Some test fails.
*     GT_FAIL               - on failure in case of invalid test path.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_STATUS utfSuitsRun
(
    IN const GT_CHAR *testPathPtr,
    IN GT_BOOL       continueFlag,
    IN GT_BOOL       startFlag
)
{
    GT_STATUS   st = GT_OK;
    GT_U32      countIdx = 0;
    GT_U32      sec  = 0;
    GT_U32      nsec = 0;
    GT_BOOL     lastSuitStarted = GT_FALSE;
    GT_BOOL     isSingleTest = GT_FALSE;
    GT_BOOL     isInvalidSuitName = GT_TRUE;
    GT_BOOL     isInvalidTestName = GT_TRUE;
    GT_BOOL     isSuitFound = GT_FALSE;
    GT_BOOL     isTestFound = GT_FALSE;

    GT_CHAR     lastSuitName[UTF_MAX_TEST_PATH_LEN_CNS + 1] = {0, };

    GT_CHAR     suitName[UTF_MAX_TEST_PATH_LEN_CNS + 1] = {0, };
    GT_CHAR     testName[UTF_MAX_TEST_PATH_LEN_CNS + 1] = {0, };

#if 0
    GT_CHAR     lastSuitName[UTF_MAX_TEST_PATH_LEN_CNS + 1];

    GT_CHAR     suitName[UTF_MAX_TEST_PATH_LEN_CNS + 1];
    GT_CHAR     testName[UTF_MAX_TEST_PATH_LEN_CNS + 1];

    /* !! NOTE - using explicit init by osMemSet to avoid
        implicit usage of memcpy by gcc (avoid to use libc directly) */
    osMemSet(lastSuitName, 0, sizeof(lastSuitName));
    osMemSet(suitName, 0, sizeof(suitName));
    osMemSet(testName, 0, sizeof(testName));
#endif

    /* Set to non printing if in mode NONE or FINAL */
    switch (utfLogOutputSelectMode)
    {
        case UTF_LOG_OUTPUT_SERIAL_ALL_E:
            prvUtfLogPrintEnable(UTF_LOG_OUTPUT_SERIAL_ALL_E);
            break;
        case UTF_LOG_OUTPUT_FILE_E:
            prvUtfLogPrintEnable(UTF_LOG_OUTPUT_FILE_E);
            break;
        default:
            prvUtfLogPrintEnable(UTF_LOG_OUTPUT_SERIAL_FINAL_E);
    }

    utfCtx.errorFlag = GT_FALSE;
    utfCtx.continueFlag = continueFlag;

    /* reset number of errors for all test enties */
    for (countIdx = 0; countIdx < UTF_MAX_NUM_OF_TESTS_CNS; countIdx++)
    {
        utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].errorCount = 0;
    }

    /* parse test path and extract suite name and test name */
    if (NULL == testPathPtr)
    {
        suitName[0] = '\0';
    }
   else
   {
        countIdx = 0;

        while ((testPathPtr[countIdx] != '.') &&
               (countIdx < UTF_MAX_TEST_PATH_LEN_CNS) &&
               (testPathPtr[countIdx] != '\0'))
        {
            suitName[countIdx] = testPathPtr[countIdx];
            countIdx++;
        }

        suitName[countIdx] = '\0';

        if (testPathPtr[countIdx] == '.')
        {
            GT_U32 suitIdx = 0;

            isSingleTest = GT_TRUE;

            while ((suitIdx < UTF_MAX_TEST_PATH_LEN_CNS) &&(testPathPtr[suitIdx] != '\0'))
            {
                testName[suitIdx] = testPathPtr[suitIdx + countIdx + 1];
                suitIdx++;
            }
            testName[suitIdx] = '\0';
        }
    }

    /* create semaphore */
    st = cpssOsSigSemBinCreate("utfTestSem", 0, &utfTestSemId);
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfSuitsRun: failed [0x%X] to create semaphore\n", st);
        return st;
    }

    /* go over all test entries and run all with defined suite and test names */
    for (countIdx = 0; countIdx < utfCtx.usedTestNum; countIdx++)
    {
        const GT_CHAR* currSuitNamePtr = utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].suitNamePtr;
        const GT_CHAR* currTestNamePtr = utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].testNamePtr;

        /* check is start suit or test is reached */
        isSuitFound = (startFlag && (0 == cpssOsStrCmp(currSuitNamePtr, suitName))) || isSuitFound;
        isTestFound = (((startFlag && (0 == cpssOsStrCmp(currTestNamePtr, testName))) || isTestFound));

        if (('\0' == suitName[0]) || (0 == cpssOsStrCmp(currSuitNamePtr, suitName)) || (GT_TRUE == isSuitFound))
        {
            isInvalidSuitName = GT_FALSE;

            if ((GT_TRUE == isSingleTest) && (0 != cpssOsStrCmp(currTestNamePtr, testName)) && (GT_FALSE == isTestFound))
            {
                continue;
            }

            isInvalidTestName = GT_FALSE;

            /* check if current test needs to be skipped */
            if (utfSkipTestTypeBmp & (1 << utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].testType))
            {
                continue;
            }

            if ((cpssOsStrCmp(currSuitNamePtr, lastSuitName) != 0) && (GT_FALSE == isRandomRunMode))
            {
                /* get current time */
                st = cpssOsTimeRT(&sec, &nsec);
                if (GT_OK != st)
                {
                    PRV_UTF_LOG1_MAC("[UTF]: utfSuitsRun: failed [0x%X] to get system time\n", st);
                }

                if (nsec < startTimeNsec)
                {
                    nsec += 1000000000;
                    sec--;
                }

                PRV_UTF_LOG1_MAC("**** SUIT %s STARTED\n", currSuitNamePtr);
                PRV_UTF_LOG1_MAC("[UTF]: number of tests [%d]\n", utfTestsNumGet(currSuitNamePtr));
                PRV_UTF_LOG2_MAC("[UTF]: suit start after [%d.%09d] sec\n", sec - startTimeSec, nsec - startTimeNsec);
                PRV_UTF_LOG0_MAC("\n");

                cpssOsStrCpy(lastSuitName, currSuitNamePtr);
                lastSuitStarted = GT_TRUE;
            }

            /* check if test timeout needed */
            if (CPSS_OS_SEM_WAIT_FOREVER_CNS == utfTestTimeOut)
            {
                /* run UT */
                utfTestNumRun(utfCtx.testIndexArray[countIdx]);
            }
            else
            {
                /* create task for single test */
                st = cpssOsTaskCreate("utfTestTask",
                                      UTF_TASK_PRIO_CNS,
                                      UTF_TASK_STACK_SIZE_CNS,
                                      (unsigned (__TASKCONV *) (GT_VOID*))utfTestTask,
                                      &(utfCtx.testIndexArray[countIdx]),
                                      &utfTestTaskId);
                if (GT_OK != st)
                {
                    PRV_UTF_LOG1_MAC("[UTF]: utfSuitsRun: failed [0x%X] to create task\n", st);
                    /* try to run the next test */
                    continue;
                }

                /* wait for semaphore signal */
                st = cpssOsSigSemWait(utfTestSemId, utfTestTimeOut);
                if (GT_TIMEOUT == st)
                {
                    utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].errorCount++;

                    PRV_UTF_LOG1_MAC("[UTF]: test timeout reached [%d] msec\n", utfTestTimeOut);
                    PRV_UTF_LOG1_MAC("******** TEST %s FAILED\n", utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].testNamePtr);
                    PRV_UTF_LOG0_MAC("\n");
                }

                /* wait for task end */
                cpssOsTimerWkAfter(10);

                /* delete test's task */
                st = cpssOsTaskDelete(utfTestTaskId);
                if (GT_OK != st)
                {
                    PRV_UTF_LOG1_MAC("[UTF]: utfSuitsRun: failed [0x%X] to delete task\n", st);
                }
            }
        }

        if (GT_TRUE == lastSuitStarted)
        {
            /* check if some test fails during testing */
            if (GT_TRUE == utfCtx.errorFlag)
            {
                st = GT_UTF_TEST_FAILED;
            }

            /* display final suit statistics */
            if (GT_FALSE == isRandomRunMode)
            {
            if (countIdx == (utfCtx.usedTestNum - 1))
            {
                utfSuitStatsShow(lastSuitName);
            }
                else if (cpssOsStrCmp(utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx + 1]].suitNamePtr, lastSuitName) != 0)
            {
                utfSuitStatsShow(lastSuitName);
                lastSuitName[0] = 0;
                lastSuitStarted = GT_FALSE;
            }
        }
    }
    }

    /* delete semaphore */
    st = cpssOsSigSemDelete(utfTestSemId);
    if (GT_OK != st)
    {
        PRV_UTF_LOG1_MAC("[UTF]: utfSuitsRun: failed [0x%X] to delete semaphore\n", st);
    }

    /* Set to printing mode if not in NONE printing mode,
       in order tp print the final result */
    if (UTF_LOG_OUTPUT_SERIAL_FINAL_E == utfLogOutputSelectMode)
    {
        prvUtfLogPrintEnable(UTF_LOG_OUTPUT_SERIAL_ALL_E);
    }

    /* show final statistics */
    if (GT_TRUE == isInvalidSuitName)
    {
        PRV_UTF_LOG0_MAC("******** SUIT ERROR ********\n");
        PRV_UTF_LOG1_MAC("Invalid suit name -- %s\n", suitName);
        PRV_UTF_LOG0_MAC("*****************************\n");
        PRV_UTF_LOG0_MAC("\n");
        return GT_FAIL;
    }
    if ((GT_TRUE == isInvalidTestName) && (GT_TRUE == isSingleTest))
    {
        PRV_UTF_LOG0_MAC("******** TEST ERROR ********\n");
        PRV_UTF_LOG1_MAC("Invalid test name -- %s\n", testName);
        PRV_UTF_LOG0_MAC("*****************************\n");
        PRV_UTF_LOG0_MAC("\n");
        return GT_FAIL;
    }

    /* display final report about all run suits */
    utfFinalStatsShow();
    return st;
}

/*******************************************************************************
* utfTestTimeOutSet
*
* DESCRIPTION:
*       This routine set timeout for single test.
*
* INPUTS:
*       testTimeOut     -   test timeout in milliseconds (0 to wait forever).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS utfTestTimeOutSet
(
    IN GT_U32   testTimeOut
)
{
    utfTestTimeOut = testTimeOut;

    return GT_OK;
}

/*******************************************************************************
* utfTestsNumGet
*
* DESCRIPTION:
*     Calculate total tests number in suit
*
* INPUTS:
*     suitNamePtr   -   name of the suit, unique for all suits
*
* OUTPUTS:
*     none
*
* RETURNS:
*     total tests number in suit
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32 utfTestsNumGet
(
    IN const GT_CHAR       *suitNamePtr
)
{
    GT_U32      countIdx   = 0;
    GT_U32      testsCount = 0;

    /* go over all test entries */
    for (countIdx = 0; countIdx < utfCtx.usedTestNum; countIdx++)
    {
        const GT_CHAR* currSuitNamePtr = utfCtx.testEntriesArray[utfCtx.testIndexArray[countIdx]].suitNamePtr;

        testsCount += (0 == cpssOsStrCmp(currSuitNamePtr, suitNamePtr));
    }

    return testsCount;
}

/*******************************************************************************
* utfDeclareTestType
*
* DESCRIPTION:
*     Declare global test type
*
* INPUTS:
*     testType   -   test type
*
* OUTPUTS:
*     none
*
* RETURNS:
*     none
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID utfDeclareTestType
(
    IN UTF_TEST_TYPE_ENT testType
)
{
    switch (testType)
    {
        case UTF_TEST_TYPE_GEN_E:
        case UTF_TEST_TYPE_CHX_E:
        case UTF_TEST_TYPE_SAL_E:
        case UTF_TEST_TYPE_EX_E:
        case UTF_TEST_TYPE_PM_E:
        case UTF_TEST_TYPE_TRAFFIC_E:
        case UTF_TEST_TYPE_TRAFFIC_FDB_NON_UNIFIED_E:
            utfTestType = testType;
            break;

        default:
            utfTestType = UTF_TEST_TYPE_NONE_E;
    }
}

/*******************************************************************************
* utfSkipTests
*
* DESCRIPTION:
*     Declare test type to skip
*
* INPUTS:
*     testType   -   test type
*
* OUTPUTS:
*     none
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS utfSkipTests
(
    IN UTF_TEST_TYPE_ENT testType
)
{
    switch (testType)
    {
        case UTF_TEST_TYPE_NONE_E:
            utfSkipTestTypeBmp = 0;
            break;

        case UTF_TEST_TYPE_GEN_E:
        case UTF_TEST_TYPE_CHX_E:
        case UTF_TEST_TYPE_SAL_E:
        case UTF_TEST_TYPE_EX_E:
        case UTF_TEST_TYPE_PM_E:
        case UTF_TEST_TYPE_TRAFFIC_E:
        case UTF_TEST_TYPE_TRAFFIC_FDB_NON_UNIFIED_E:
            utfSkipTestTypeBmp |= 1 << testType;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* utfDefineTests
*
* DESCRIPTION:
*     Declare test type to run.
*     every call to this API override the previous call.
*
* INPUTS:
*     testType   -   test type
*                    NOTE: value UTF_TEST_TYPE_NONE_E --> used here as 'ALL' (and not NONE)
* OUTPUTS:
*     none
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS utfDefineTests
(
    IN UTF_TEST_TYPE_ENT testType
)
{
    switch (testType)
    {
        case UTF_TEST_TYPE_NONE_E:
            /*NOTE: value UTF_TEST_TYPE_NONE_E --> used here as 'ALL' (and not NONE)*/
            utfSkipTestTypeBmp = 0xFFFFFFFF;/* allow all types */
            break;

        case UTF_TEST_TYPE_GEN_E:
        case UTF_TEST_TYPE_CHX_E:
        case UTF_TEST_TYPE_SAL_E:
        case UTF_TEST_TYPE_EX_E:
        case UTF_TEST_TYPE_PM_E:
        case UTF_TEST_TYPE_TRAFFIC_E:
        case UTF_TEST_TYPE_TRAFFIC_FDB_NON_UNIFIED_E:
            utfSkipTestTypeBmp = 0xFFFFFFFF & ~(1 << testType); /* only specific type */
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*************************************************************************************
* utfPrintKeepAlive
*
* DESCRIPTION:
*     This function print "." so user can see that test / test still running.
*     keep alive indication , that test is doing long processing , and between tests.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     none
*
* COMMENTS:
*     none
*
************************************************************************************/
GT_VOID utfPrintKeepAlive
(
    GT_VOID
)
{
    static GT_U32 counter = 0;
    /* put indication to the terminal , so we can see that multi-test are in
       progress */
    cpssOsPrintf(".");
    if(((++counter) & 0xf) == 0)
    {
        /* every 16 times do "\n" */
        cpssOsPrintf("\n");
    }
}

/*******************************************************************************
* utfGeneralStateMessageSave
*
* DESCRIPTION:
*       This function saves message about the general state.
*       Defines string with parameters to add to logger when test fail .
*       this string is about 'general state' of the test , like global parameters.
*       the caller can put into DB up to PRV_UTF_GENERAL_STATE_INFO_NUM_CNS such strings.
*       the DB is cleared at start of each test.
*       the DB is printed when test fail
*
* INPUTS:
*       index           - index in the DB
*       formatStringPtr -   (pointer to) format string.
*                         when NULL - the index in DB is cleared.
*       ...             -    string arguments.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       -   the info saved to DB
*       GT_BAD_PARAM -   the index >= PRV_UTF_GENERAL_STATE_INFO_NUM_CNS
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS utfGeneralStateMessageSave
(
    IN GT_U32           index,
    IN const GT_CHAR    *formatStringPtr,
    IN                  ...
)
{
    va_list args;

    if(index >= PRV_UTF_GENERAL_STATE_INFO_NUM_CNS)
    {
        return GT_BAD_PARAM;
    }

    if(formatStringPtr == NULL)
    {
        /* save to DB --> entry not valid */
        prvUtfGeneralStateLogArr[index].valid = GT_FALSE;
        return GT_OK;
    }

    /* save to DB */
    va_start(args, formatStringPtr);
    vsprintf(prvUtfGeneralStateLogArr[index].buff, formatStringPtr, args);
    va_end(args);

    /* entry is valid */
    prvUtfGeneralStateLogArr[index].valid = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* utfRandomRunModeSet
*
* DESCRIPTION:
*     This function enables test's random run mode
*
* INPUTS:
*     enable - enable\disable random run mode
*     seed   - random number seed (relevant only for random run mode)
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK - on success
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS utfRandomRunModeSet
(
    IN GT_BOOL  enable,
    IN GT_U32   seed
)
{
    GT_U32  iter      = 0;
    GT_U32  randIndex = 0; /* random array index */
    GT_U32  tmpVal    = 0;


    /* set random run mode flag */
    isRandomRunMode = enable;

    /* restore index array */
    for (iter = 0; iter < utfCtx.usedTestNum; iter++)
    {
        utfCtx.testIndexArray[iter] = iter;
    }

    /* mix up index array for random mode */
    if (GT_TRUE == isRandomRunMode)
    {
        /* set specific seed for random generator */
        cpssOsSrand(seed);

        /* generate random array indexes */
        for (iter = 0; iter < utfCtx.usedTestNum; iter++)
        {
            /* generate random index */
            randIndex = cpssOsRand() % utfCtx.usedTestNum;

            /* exchange array values */
            tmpVal = utfCtx.testIndexArray[randIndex];
            utfCtx.testIndexArray[randIndex] = utfCtx.testIndexArray[iter];
            utfCtx.testIndexArray[iter] = tmpVal;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* utfTestRunFirstSet
*
* DESCRIPTION:
*     This function put specific test to be run first
*
* INPUTS:
*     testNamePtr - test name to make it run first
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK        - on success
*     GT_BAD_PTR   - on null pointer
*     GT_NOT_FOUND - on test not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS utfTestRunFirstSet
(
    IN const GT_CHAR   *testNamePtr
)
{
    GT_U32  testIdx = 0;
    GT_U32  tmpVal  = 0;


    /* check test name */
    CPSS_NULL_PTR_CHECK_MAC(testNamePtr);

    /* fing test's index to run first */
    while (cpssOsStrCmp(utfCtx.testEntriesArray[utfCtx.testIndexArray[testIdx]].testNamePtr, testNamePtr) &&
           (++testIdx < utfCtx.usedTestNum));

    /* check if test found */
    if (testIdx == utfCtx.usedTestNum)
    {
        /* set wrong test's name */
        PRV_UTF_LOG1_MAC("[UTF]: utfTestRunFirstSet: invalid test name -- %s\n", testNamePtr);

        return GT_NOT_FOUND;
    }

    /* store test's index to run first */
    tmpVal = utfCtx.testIndexArray[testIdx];

    /* shift indexes in array */
    while (testIdx--)
    {
        utfCtx.testIndexArray[testIdx + 1] = utfCtx.testIndexArray[testIdx];
    }

    /* set test at the beginning of array */
    utfCtx.testIndexArray[0] = tmpVal;

    return GT_OK;
}

