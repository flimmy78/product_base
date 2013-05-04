/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* utfMain.h
*
* DESCRIPTION:
*       Public header which must be included to all test suits .c files
*       Contains declaration of all UTF functionality, like listing, running,
*       and displaying statistics.
*       Defines macros to declare, start and finalize suit and test case.
*       Defines macros to check test failures.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __utfMainh
#define __utfMainh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _MSC_VER
    /* Disable warning messages for VC W4 warnings level */
    #pragma warning(disable: 4152)
#endif /* _MSC_VER */

#include <cpss/generic/cpssTypes.h>

/* defines */
/* This parameter defines how many test cases UTF will support.
 * Also the parameter impacts on the memory map size.
 */
#if defined(CHX_FAMILY) &&  defined(EXMXPM_FAMILY)
    #define UTF_MAX_NUM_OF_TESTS_CNS    2500
#else
    #define UTF_MAX_NUM_OF_TESTS_CNS    2000
#endif


/* Max Utf parameter name lenght */
#define UTF_MAX_PARAM_NAME_LEN_CNS  50

/* Max Utf value lenght (in Ansi format) */
#define UTF_MAX_VALUE_LEN_CNS       15

/* Max UTF result array size (maximum number of parameters passed to file)*/
#define UTF_RESULT_ARRAY_MAX_CNS    15

/* Max UTF result file size*/
#define UTF_MAX_LOG_SIZE_CNS      8196

/* Unit test fail  This error is returned by UTF to indicate that some test fails.*/
#define GT_UTF_TEST_FAILED          (CPSS_PRESTERA_ERROR_BASE_CNS + (0x10000))

#define UTF_ARGVP_ARR_1_MAC(argvp,arg1)   \
            const GT_VOID *argvp[1];                                          \
            argvp[0] = (GT_VOID*)((GT_U32)arg1);

#define UTF_ARGVP_ARR_2_MAC(argvp,arg1,arg2)   \
            const GT_VOID *argvp[2];                                          \
            argvp[0] = (GT_VOID*)((GT_U32)arg1);                              \
            argvp[1] = (GT_VOID*)((GT_U32)arg2);

#define UTF_ARGVP_ARR_3_MAC(argvp,arg1,arg2,arg3)   \
            const GT_VOID *argvp[3];                                          \
            argvp[0] = (GT_VOID*)((GT_U32)arg1);                              \
            argvp[1] = (GT_VOID*)((GT_U32)arg2);                              \
            argvp[2] = (GT_VOID*)((GT_U32)arg3);

#define UTF_ARGVP_ARR_4_MAC(argvp,arg1,arg2,arg3,arg4)   \
            const GT_VOID *argvp[4];                                          \
            argvp[0] = (GT_VOID*)((GT_U32)arg1);                              \
            argvp[1] = (GT_VOID*)((GT_U32)arg2);                              \
            argvp[2] = (GT_VOID*)((GT_U32)arg3);                              \
            argvp[3] = (GT_VOID*)((GT_U32)arg4);

#define UTF_ARGVP_ARR_5_MAC(argvp,arg1,arg2,arg3,arg4,arg5)   \
            const GT_VOID *argvp[5];                                          \
            argvp[0] = (GT_VOID*)((GT_U32)arg1);                              \
            argvp[1] = (GT_VOID*)((GT_U32)arg2);                              \
            argvp[2] = (GT_VOID*)((GT_U32)arg3);                              \
            argvp[3] = (GT_VOID*)((GT_U32)arg4);                              \
            argvp[4] = (GT_VOID*)((GT_U32)arg5);

#define UTF_ARGVP_ARR_6_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6)   \
            const GT_VOID *argvp[6];                                          \
            argvp[0] = (GT_VOID*)((GT_U32)arg1);                              \
            argvp[1] = (GT_VOID*)((GT_U32)arg2);                              \
            argvp[2] = (GT_VOID*)((GT_U32)arg3);                              \
            argvp[3] = (GT_VOID*)((GT_U32)arg4);                              \
            argvp[4] = (GT_VOID*)((GT_U32)arg5);                              \
            argvp[5] = (GT_VOID*)((GT_U32)arg6);

#define UTF_ARGVP_ARR_7_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6,arg7)   \
            const GT_VOID *argvp[7];                                          \
            argvp[0] = (GT_VOID*)((GT_U32)arg1);                              \
            argvp[1] = (GT_VOID*)((GT_U32)arg2);                              \
            argvp[2] = (GT_VOID*)((GT_U32)arg3);                              \
            argvp[3] = (GT_VOID*)((GT_U32)arg4);                              \
            argvp[4] = (GT_VOID*)((GT_U32)arg5);                              \
            argvp[5] = (GT_VOID*)((GT_U32)arg6);                              \
            argvp[6] = (GT_VOID*)((GT_U32)arg7);

#define UTF_ARGVP_ARR_8_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8)   \
            const GT_VOID *argvp[8];                                          \
            argvp[0] = (GT_VOID*)((GT_U32)arg1);                              \
            argvp[1] = (GT_VOID*)((GT_U32)arg2);                              \
            argvp[2] = (GT_VOID*)((GT_U32)arg3);                              \
            argvp[3] = (GT_VOID*)((GT_U32)arg4);                              \
            argvp[4] = (GT_VOID*)((GT_U32)arg5);                              \
            argvp[5] = (GT_VOID*)((GT_U32)arg6);                              \
            argvp[6] = (GT_VOID*)((GT_U32)arg7);                              \
            argvp[7] = (GT_VOID*)((GT_U32)arg8);

#define UTF_ARGVP_ARR_9_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9)\
            const GT_VOID *argvp[9];                                          \
            argvp[0] = (GT_VOID*)((GT_U32)arg1);                              \
            argvp[1] = (GT_VOID*)((GT_U32)arg2);                              \
            argvp[2] = (GT_VOID*)((GT_U32)arg3);                              \
            argvp[3] = (GT_VOID*)((GT_U32)arg4);                              \
            argvp[4] = (GT_VOID*)((GT_U32)arg5);                              \
            argvp[5] = (GT_VOID*)((GT_U32)arg6);                              \
            argvp[6] = (GT_VOID*)((GT_U32)arg7);                              \
            argvp[7] = (GT_VOID*)((GT_U32)arg8);                              \
            argvp[8] = (GT_VOID*)((GT_U32)arg9);


/* Check given function for wrong enum values (from array) */
extern GT_U32  enumsIndex;
extern GT_U32 tempParamValue;
#define UTF_ENUMS_CHECK_MAC(func, param)                                       \
    {                                                                          \
        enumsIndex = 0;                                                        \
        tempParamValue = param;                                                \
        for(enumsIndex = 0; enumsIndex < utfInvalidEnumArrSize; enumsIndex++)  \
        {                                                                      \
              param = utfInvalidEnumArr[enumsIndex];                           \
              st = func;                                                       \
              UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PARAM, st,                   \
                            "Fail on wrong enum value: %d", param);            \
        }                                                                      \
        param = tempParamValue;                                                \
    }


/* defines the number of messages that can be given as 'General info' see function
   utfGeneralStateMessageSave(...) */
#define PRV_UTF_GENERAL_STATE_INFO_NUM_CNS          10

/* typedefs */
/*
 * Typedef: enum UTF_LOG_OUTPUT_ENT
 *
 * Description: presents values of available kinds of log output
 *
 * Fields:
 *      UTF_LOG_OUTPUT_SERIAL_E - output to serial interface
 *      UTF_LOG_OUTPUT_FILE_E   - output to file
 *
 * Comments: serial and output to file are available
 *
 */
typedef enum
{
    UTF_LOG_OUTPUT_SERIAL_ALL_E = 0,
    UTF_LOG_OUTPUT_SERIAL_FINAL_E = 1,
    UTF_LOG_OUTPUT_FILE_E = 2
} UTF_LOG_OUTPUT_ENT;

/*
 * Typedef: enum UTF_TEST_TYPE_ENT
 *
 * Description: presents values of available types of tests
 *
 * Fields:
 *      UTF_TEST_TYPE_NONE_E    - no test type
 *      UTF_TEST_TYPE_GEN_E     - general test type
 *      UTF_TEST_TYPE_CHX_E     - CH test type
 *      UTF_TEST_TYPE_SAL_E     - Salsa test type
 *      UTF_TEST_TYPE_EX_E      - EX test type
 *      UTF_TEST_TYPE_PM_E      - PM test type
 *      UTF_TEST_TYPE_TRAFFIC_E - traffic test type
 *      UTF_TEST_TYPE_TRAFFIC_FDB_NON_UNIFIED_E - traffic test type when FDB is not unified
 *
 * Comments: serial and output to file are available
 *
 */
typedef enum
{
    UTF_TEST_TYPE_NONE_E = 0,
    UTF_TEST_TYPE_GEN_E,
    UTF_TEST_TYPE_CHX_E,
    UTF_TEST_TYPE_SAL_E,
    UTF_TEST_TYPE_EX_E,
    UTF_TEST_TYPE_PM_E,
    UTF_TEST_TYPE_TRAFFIC_E,
    UTF_TEST_TYPE_TRAFFIC_FDB_NON_UNIFIED_E,

    UTF_TEST_TYPE_LAST_E
} UTF_TEST_TYPE_ENT;

/*
 * Typedef: UTF_TEST_CASE_FUNC_PTR
 *
 * Description: pointer to test case funcion
 *
 * Comments: test case function should receive nothing (void) and shouldn't
 *      return anything (void)
 */
typedef GT_VOID (*UTF_TEST_CASE_FUNC_PTR)(GT_VOID);

/*
 * Typedef: struct UTF_RESULT_PARAM_STC
 *
 * Description: contain UTF result parameter, that passed from tests to result file
 *
 * Fields:
 *          paramName[UTF_MAX_PARAM_NAME_LEN] - parameter name (less then UTF_MAX_PARAM_NAME_LEN chars)
 *          paramValue                        - parameter value
 *          paramDelta                        - parameter Delta (in persents)
 */
typedef struct
{
    GT_CHAR             paramName[UTF_MAX_PARAM_NAME_LEN_CNS];
    GT_U32              paramValue;
    GT_U8               paramDelta;
} UTF_RESULT_PARAM_STC;

/* defines */
/*
 * Description: Defines begin of a singe suit.
 *
 * Parameters:
 *      x - suit name
 *
 * Comments: Name of suit must be unique between all the suits.
 */
#define UTF_SUIT_BEGIN_TESTS_MAC(x)         \
    GT_STATUS utfSuit_##x(GT_VOID) {        \
        GT_STATUS st = GT_OK;               \
        static const GT_VOID *s_##x[] = {   \
            #x,                             \

#define UTF_SUIT_PBR_BEGIN_TESTS_MAC(x)     \
           UTF_SUIT_BEGIN_TESTS_MAC(x)      \

/*
 * Description: Defines begin of a singe suit with preinit condition.
 *
 * Parameters:
 *      x - suit name
 *
 * Comments: Name of suit must be unique between all the suits.
 */
#define UTF_SUIT_WITH_PREINIT_CONDITION_BEGIN_TESTS_MAC(x)          \
    GT_STATUS utfSuit_##x(GT_VOID) {                                \
        GT_STATUS st = GT_OK;                                       \
        GT_U32 value = 0;                                           \
        static const GT_VOID *s_##x[] = {                           \
            #x,                                                     \

/*
 * Description: Finalize suit declaration.
 *
 * Parameters:
 *      x - suit name
 *
 * Comments: This macro must be placed after all declared test cases.
 */
#define UTF_SUIT_END_TESTS_MAC(x)           \
        0};                                 \
        st = utfSuitAdd(s_##x);             \
        return st;                          \
    }                                       \

/*
 * Description: Finalize suit declaration with preinit condition.
 *
 * Parameters:
 *      x - suit name
 *      y - preinit conditional string. The configuration value is retieved
 *          by this string, compared with expected result z and decision of
 *          including test into test suit or not is done
 *      z - expected value
 * Comments: This macro must be placed after all declared test cases.
 */
#define UTF_SUIT_WITH_PREINIT_CONDITION_END_TESTS_MAC(x,y,z)         \
        0};                                                          \
        if ((appDemoDbEntryGet(y, &value) == GT_OK)&&(value == z))   \
            st = utfSuitAdd(s_##x);                                  \
        return st;                                                   \
    }                                                                \

/*
 * Description: Finalize suit declaration for PBR mode.
 *
 * Parameters:
 *      x - suit name
 *      n - number of tests to including into the test suit in PBR mode.
 *          In other modes, all tests will be included.
 * Comments: This macro must be placed after all declared test cases.
 */
#define UTF_SUIT_PBR_END_TESTS_MAC(x,n)                              \
        0};                                                          \
        if (s_##x != NULL)                                           \
        {                                                            \
            PRV_TGF_IP_ROUTING_MODE_ENT routingMode;                 \
            prvTgfIpRoutingModeGet(&routingMode);                    \
            if (routingMode == PRV_TGF_IP_ROUTING_MODE_PCL_ACTION_E) \
                s_##x[1 + 2 * n] = NULL;     /* keep n tests */      \
            st = utfSuitAdd(s_##x);                                  \
        }                                                            \
        return st;                                                   \
    }                                                                \


/*
 * Description: Includes test case into test suit.
 *
 * Parameters:
 *      x - tested function name
 *
 * Comments: Name of test case must be unique inside one suit and
 *      can be not unique in different suits.
 */
#define UTF_SUIT_DECLARE_TEST_MAC(x) #x, x##UT,

/*
 * Description: Start test case implementation with this macro.
 *
 * Parameters:
 *      x - tested function name
 *
 * Comments: use this name later in test suit declaration.
 */
#define UTF_TEST_CASE_MAC(x) static GT_VOID x##UT(GT_VOID)

/*
 * Description: macro to call UT test for given test name
 *
 * Parameters:
 *      x - test name
 *
 * Comments: use this to call from one test to other test
 */
#define UTF_TEST_CALL_MAC(x)    x##UT()

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there is no any argument in output string.
 *
 * Parameters:
 *      e - expexted value
 *      r - received value
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_EQUAL0_PARAM_MAC(e, r)                                               \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            err = utfFailureMsgLog(NULL, NULL, 0);                                      \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there is only one argument in output string.
 *
 * Parameters:
 *      e -       expexted value
 *      r -       received value
 *      arg1 -    argument for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_EQUAL1_PARAM_MAC(e, r, arg1)                                         \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_1_MAC(argvp,arg1);                                            \
            err = utfFailureMsgLog(NULL, argvp, 1);                                     \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are two arguments in output string.
 *
 * Parameters:
 *      e -           expexted value
 *      r -           received value
 *      arg1, arg2 -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_EQUAL2_PARAM_MAC(e, r, arg1, arg2)                                   \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_2_MAC(argvp,arg1,arg2);                                       \
            err = utfFailureMsgLog(NULL, argvp, 2);                                     \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are three arguments in output string.
 *
 * Parameters:
 *      e -                   expexted value
 *      r -                   received value
 *      arg1, arg2, arg3 -    arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_EQUAL3_PARAM_MAC(e, r, arg1, arg2, arg3)                             \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_3_MAC(argvp,arg1,arg2,arg3);                                  \
            err = utfFailureMsgLog(NULL, argvp, 3);                                     \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are four arguments in output string.
 *
 * Parameters:
 *      e                       -    expexted value
 *      r                       -    received value
 *      arg1, arg2, arg3, arg4  -    arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_EQUAL4_PARAM_MAC(e, r, arg1, arg2, arg3, arg4)                       \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_4_MAC(argvp,arg1,arg2,arg3,arg4);                             \
            err = utfFailureMsgLog(NULL, argvp, 4);                                     \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are five arguments in output string.
 *
 * Parameters:
 *      e                               -    expexted value
 *      r                               -    received value
 *      arg1, arg2, arg3, arg4, arg5    -    arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_EQUAL5_PARAM_MAC(e, r, arg1, arg2, arg3, arg4, arg5)                 \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_5_MAC(argvp,arg1,arg2,arg3,arg4,arg5);                        \
            err = utfFailureMsgLog(NULL, argvp, 5);                                     \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are six arguments in output string.
 *
 * Parameters:
 *      e                                   -    expexted value
 *      r                                   -    received value
 *      arg1, arg2, arg3, arg4, arg5, arg6  -    arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_EQUAL6_PARAM_MAC(e, r, arg1, arg2, arg3, arg4, arg5, arg6)           \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_6_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6);                   \
            err = utfFailureMsgLog(NULL, argvp, 6);                                     \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)
/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are seven arguments in output string.
 *
 * Parameters:
 *      e                                   -    expexted value
 *      r                                   -    received value
 *      arg1, arg2, arg3, arg4, arg5, arg6, arg7  -    arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_EQUAL7_PARAM_MAC(e, r, arg1, arg2, arg3, arg4, arg5, arg6, arg7)     \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_7_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6,arg7);              \
            err = utfFailureMsgLog(NULL, argvp, 7);                                     \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there is no any argument in output string.
 *
 * Parameters:
 *      e -  expexted value
 *      r -  received value
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_NOT_EQUAL0_PARAM_MAC(e, r)                                           \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            err = utfFailureMsgLog(NULL, NULL, 0);                                      \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there is only one argument in output string.
 *
 * Parameters:
 *      e -       expexted value
 *      r -       received value
 *      arg1 -    argument for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(e, r, arg1)                                     \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_1_MAC(argvp,arg1);                        \
            err = utfFailureMsgLog(NULL, argvp, 1);                                     \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there are two arguments in output string.
 *
 * Parameters:
 *      e -           expexted value
 *      r -           received value
 *      arg1, arg2 -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(e, r, arg1, arg2)                               \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_2_MAC(argvp,arg1,arg2);                        \
            err = utfFailureMsgLog(NULL, argvp, 2);                                     \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there are three arguments in output string.
 *
 * Parameters:
 *      e -                   expexted value
 *      r -                   received value
 *      arg1, arg2, arg3 -    arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(e, r, arg1, arg2, arg3)                         \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_3_MAC(argvp,arg1,arg2,arg3);                        \
            err = utfFailureMsgLog(NULL, argvp, 3);                                     \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there are four arguments in output string.
 *
 * Parameters:
 *      e                       -    expexted value
 *      r                       -    received value
 *      arg1, arg2, arg3, arg4  -    arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(e, r, arg1, arg2, arg3, arg4)                   \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_4_MAC(argvp,arg1,arg2,arg3,arg4);                        \
            err = utfFailureMsgLog(NULL, argvp, 4);                                     \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there are five arguments in output string.
 *
 * Parameters:
 *      e                               -    expexted value
 *      r                               -    received value
 *      arg1, arg2, arg3, arg4, arg5    -    arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(e, r, arg1, arg2, arg3, arg4, arg5)             \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_5_MAC(argvp,arg1,arg2,arg3,arg4,arg5);                        \
            err = utfFailureMsgLog(NULL, argvp, 5);                                     \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there are six arguments in output string.
 *
 * Parameters:
 *      e                                   -    expexted value
 *      r                                   -    received value
 *      arg1, arg2, arg3, arg4, arg5, arg6  -    arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_NOT_EQUAL6_PARAM_MAC(e, r, arg1, arg2, arg3, arg4, arg5, arg6)       \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_6_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6);                   \
            err = utfFailureMsgLog(NULL, argvp, 6);                                     \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there are seven arguments in output string.
 *
 * Parameters:
 *      e                                   -    expexted value
 *      r                                   -    received value
 *      arg1, arg2, arg3, arg4, arg5, arg6, arg7  -    arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 *      Output string has format accordingly to "CPSS Unit Test SDD":[CPSS_UT_0130]
 */
#define UTF_VERIFY_NOT_EQUAL7_PARAM_MAC(e, r, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_7_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6,arg7);              \
            err = utfFailureMsgLog(NULL, argvp, 7);                                     \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there is no any argument in output string.
 *
 * Parameters:
 *      e -    expexted value
 *      r -    received value
 *      s -    format string for output
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_EQUAL0_STRING_MAC(e, r, s)                                           \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            err = utfFailureMsgLog(s, NULL, 0);                                         \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there is only one argument in output string.
 *
 * Parameters:
 *      e -    expexted value
 *      r -    received value
 *      s -    format string for output
 *      arg1 - argument for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_EQUAL1_STRING_MAC(e, r, s, arg1)                                     \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_1_MAC(argvp,arg1);                        \
            err = utfFailureMsgLog(s, argvp, 1);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are two arguments in output string.
 *
 * Parameters:
 *      e -           expexted value
 *      r -           received value
 *      s -           format string for output
 *      arg1, arg2 -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_EQUAL2_STRING_MAC(e, r, s, arg1, arg2)                               \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_2_MAC(argvp,arg1,arg2);                        \
            err = utfFailureMsgLog(s, argvp, 2);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are three arguments in output string.
 *
 * Parameters:
 *      e -                 expexted value
 *      r -                 received value
 *      s -                 format string for output
 *      arg1, arg2, arg3 -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_EQUAL3_STRING_MAC(e, r, s, arg1, arg2, arg3)                         \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_3_MAC(argvp,arg1,arg2,arg3);                        \
            err = utfFailureMsgLog(s, argvp, 3);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are four arguments in output string.
 *
 * Parameters:
 *      e                       -  expexted value
 *      r                       -  received value
 *      s                       -  format string for output
 *      arg1, arg2, arg3, arg4  -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_EQUAL4_STRING_MAC(e, r, s, arg1, arg2, arg3, arg4)                   \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_4_MAC(argvp,arg1,arg2,arg3,arg4);                        \
            err = utfFailureMsgLog(s, argvp, 4);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are five arguments in output string.
 *
 * Parameters:
 *      e                               -  expexted value
 *      r                               -  received value
 *      s                               -  format string for output
 *      arg1, arg2, arg3, arg4, arg5    -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_EQUAL5_STRING_MAC(e, r, s, arg1, arg2, arg3, arg4, arg5)             \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_5_MAC(argvp,arg1,arg2,arg3,arg4,arg5);                        \
            err = utfFailureMsgLog(s, argvp, 5);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are six arguments in output string.
 *
 * Parameters:
 *      e                                   -  expexted value
 *      r                                   -  received value
 *      s                                   -  format string for output
 *      arg1, arg2, arg3, arg4, arg5, arg6  -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_EQUAL6_STRING_MAC(e, r, s, arg1, arg2, arg3, arg4, arg5,arg6)        \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_6_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6);                   \
            err = utfFailureMsgLog(s, argvp, 6);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are seven arguments in output string.
 *
 * Parameters:
 *      e                                   -  expexted value
 *      r                                   -  received value
 *      s                                   -  format string for output
 *      arg1, arg2, arg3, arg4, arg5, arg6, arg7  -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_EQUAL7_STRING_MAC(e, r, s, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    do {                                                                                \
        GT_BOOL err = utfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_7_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6,arg7);              \
            err = utfFailureMsgLog(s, argvp, 7);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there is no any argument in output string.
 *
 * Parameters:
 *      e -    expexted value
 *      r -    received value
 *      s -    format string for output
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_NOT_EQUAL0_STRING_MAC(e, r, s)                                       \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            err = utfFailureMsgLog(s, NULL, 0);                                         \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there is only one argument in output string.
 *
 * Parameters:
 *      e -    expexted value
 *      r -    received value
 *      s -    format string for output
 *      arg1 - argument for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_NOT_EQUAL1_STRING_MAC(e, r, s, arg1)                                 \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_1_MAC(argvp,arg1);                        \
            err = utfFailureMsgLog(s, argvp, 1);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there are two arguments in output string.
 *
 * Parameters:
 *      e -           expexted value
 *      r -           received value
 *      s -           format string for output
 *      arg1, arg2 -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_NOT_EQUAL2_STRING_MAC(e, r, s, arg1, arg2)                           \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_2_MAC(argvp,arg1,arg2);                        \
            err = utfFailureMsgLog(s, argvp, 2);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there are three arguments in output string.
 *
 * Parameters:
 *      e -                 expexted value
 *      r -                 received value
 *      s -                 format string for output
 *      arg1, arg2, arg3 -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_NOT_EQUAL3_STRING_MAC(e, r, s, arg1, arg2, arg3)                     \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_3_MAC(argvp,arg1,arg2,arg3);                        \
            err = utfFailureMsgLog(s, argvp, 3);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there are four arguments in output string.
 *
 * Parameters:
 *      e                       -  expexted value
 *      r                       -  received value
 *      s                       -  format string for output
 *      arg1, arg2, arg3, arg4  -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_NOT_EQUAL4_STRING_MAC(e, r, s, arg1, arg2, arg3, arg4)               \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_4_MAC(argvp,arg1,arg2,arg3,arg4);                        \
            err = utfFailureMsgLog(s, argvp, 4);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there are five arguments in output string.
 *
 * Parameters:
 *      e                               -  expexted value
 *      r                               -  received value
 *      s                               -  format string for output
 *      arg1, arg2, arg3, arg4, arg5    -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_NOT_EQUAL5_STRING_MAC(e, r, s, arg1, arg2, arg3, arg4, arg5)         \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_5_MAC(argvp,arg1,arg2,arg3,arg4,arg5);                        \
            err = utfFailureMsgLog(s, argvp, 5);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there are six arguments in output string.
 *
 * Parameters:
 *      e                                   -  expexted value
 *      r                                   -  received value
 *      s                                   -  format string for output
 *      arg1, arg2, arg3, arg4, arg5, arg6  -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_NOT_EQUAL6_STRING_MAC(e, r, s, arg1, arg2, arg3, arg4, arg5, arg6)   \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_6_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6);                   \
            err = utfFailureMsgLog(s, argvp, 6);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is NOT equal to expected value.
 *      It is used when there are seven arguments in output string.
 *
 * Parameters:
 *      e                                   -  expexted value
 *      r                                   -  received value
 *      s                                   -  format string for output
 *      arg1, arg2, arg3, arg4, arg5, arg6, arg7  -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define UTF_VERIFY_NOT_EQUAL7_STRING_MAC(e, r, s, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    do {                                                                                \
        GT_BOOL err = utfNotEqualVerify(e, r, __LINE__, __FILE__);                      \
        if (GT_FALSE == err) {                                                          \
            UTF_ARGVP_ARR_7_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6,arg7);              \
            err = utfFailureMsgLog(s, argvp, 7);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)


/* default device number */
extern GT_U8    prvTgfDevNum; /* = 0; */
/* flag to state the test uses port groups bmp */
extern GT_BOOL              usePortGroupsBmp; /* default is  GT_FALSE*/;
/* bmp of port groups to use in test */
extern GT_PORT_GROUPS_BMP   currPortGroupsBmp /* default is CPSS_PORT_GROUP_UNAWARE_MODE_CNS*/;
/* number of port groups that we look for FDB */
extern GT_U32   prvTgfNumOfPortGroups /* = 0 */;


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
);

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
);

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
);

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
);

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
);

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
);

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
    IN GT_BOOL       fContinue
);

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
);

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
);

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
);

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
);

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
);

/*******************************************************************************
* prvUtfLogReadParam
*
* DESCRIPTION:
*       Read parameters from file to array.
*
* INPUTS:
*       fileName        - log output file name to check
*       numberParams    - number of parameters to read
*
* OUTPUTS:
*       arrayOfParamPtr - array of params from result file
*
* RETURNS:
*       GT_OK           - logger has been successfully opened.
*       GT_BAD_PARAM    - Invalid output interface id was passed.
*       GT_BAD_SIZE     - Invalid input file size.
*       GT_NO_RESOURCE  - Memory error.
*       GT_FAIL         - General failure error. Should never happen.
*
* COMMENTS: this functions used by  utfLogResultRun
*
*******************************************************************************/
GT_STATUS prvUtfLogReadParam
(
    IN  const GT_CHAR         *fileName,
    IN  GT_8                  numberParams,
    OUT UTF_RESULT_PARAM_STC *arrayOfParamPtr
);

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
);

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
);

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
);

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
);

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
);

/*******************************************************************************
* prvUtfLogPrintEnable
*
* DESCRIPTION:
*       This routine Enables\Disables printing.
*
* INPUTS:
*       printEnable   - If GT_TRUE print is enabled else disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       the previous settings
*
* COMMENTS:
*
*******************************************************************************/
UTF_LOG_OUTPUT_ENT prvUtfLogPrintEnable
(
    UTF_LOG_OUTPUT_ENT utfLogOutputSelectMode
);

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
);

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
);

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
);


/*************************************************************************************
* utfLogResultRun
*
* DESCRIPTION:
*     This function executes result file logging.
*
* INPUTS:
*     paramArrayPtr - array of parameters, that passed to file
*     numberParam   - number of parameters to read
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
GT_STATUS utfLogResultRun
(
    IN  UTF_RESULT_PARAM_STC         *paramArrayPtr,
    IN  GT_U8                         numberParam,
    OUT GT_BOOL                      *paramDeltaCompare
);

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
);

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
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
************************************************************************************/
GT_STATUS utfStopTimer
(
    OUT GT_U32   *timeElapsedPtr
);

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
*     Should be called before utfMemoryHeapLeakageStatusGet()
*
************************************************************************************/
GT_VOID utfMemoryHeapCounterInit
(
    GT_VOID
);

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
);
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
);

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
);

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
);

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
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __utfMainh */

