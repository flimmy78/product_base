/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssBuffManagerPoolUT.c
*
* DESCRIPTION:
*       Unit tests for Miscellaneous operations for CPSS.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpssCommon/private/prvCpssMisc.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

#define MEM_ARRAY_SIZE_CNS  20
#define VALUES_ARRAY_SIZE_CNS    6

/*----------------------------------------------------------------------------*/
/*
GT_STATUS  prvCpssFieldValueSet
(
    IN GT_U32                  *startMemPtr,
    IN GT_U32                  startBit,
    IN GT_U32                  numBits,
    IN GT_U32                  value
);
*/
UTF_TEST_CASE_MAC(prvCpssFieldValueSet)
{
    GT_STATUS   st;/*return value*/
    static GT_U32   memArray[MEM_ARRAY_SIZE_CNS];/* array of words */
    GT_U32  ii; /* iterator */
    GT_U32  indexMax;/* max index in array memArray[] */
    GT_U32  startIndex;/* start index of filed */
    GT_U32  numOfBits; /*number of bits in the filed */
    GT_U32  valuesArray[VALUES_ARRAY_SIZE_CNS] = {/* value to write to field */
            0xFFFFFFFF , 0xA5A5A5A5 , 0x5A5A5A5A , 0x12345678 , 0x87654321 , 0 };
    GT_U32  value;/* actual value*/
    GT_U32  expectedValue;/* expected value*/

    indexMax = 32 * (MEM_ARRAY_SIZE_CNS - 1);

    for(startIndex = 0; startIndex < indexMax ; startIndex++)
    {
        for(numOfBits = 1 ; numOfBits <= 32 ; numOfBits++)
        {
            for(ii = 0; ii < VALUES_ARRAY_SIZE_CNS ; ii ++)
            {
                st = prvCpssFieldValueSet(memArray,startIndex,numOfBits,valuesArray[ii]);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, startIndex, numOfBits, valuesArray[ii]);

                st = prvCpssFieldValueGet(memArray,startIndex,numOfBits,&value);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, startIndex, numOfBits);

                expectedValue =  numOfBits < 32 ?
                                 U32_GET_FIELD_MAC(valuesArray[ii],0,numOfBits) :
                                 valuesArray[ii];
                /* compare the 'set' with the 'get' */
                UTF_VERIFY_EQUAL2_PARAM_MAC(expectedValue, value, startIndex, numOfBits);
            }
        }
    }

    startIndex = 54;
    /*****************************************/
    /* check for 'errors' -- on the set API  */
    /*****************************************/
    numOfBits = 0;
    value = valuesArray[0];
    st = prvCpssFieldValueSet(memArray,startIndex,numOfBits,value);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PARAM, st);

    numOfBits = 33;
    value = valuesArray[0];
    st = prvCpssFieldValueSet(memArray,startIndex,numOfBits,value);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PARAM, st);

    numOfBits = 0;
    st = prvCpssFieldValueSet(NULL,startIndex,numOfBits,value);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);


    /*****************************************/
    /* check for 'errors' -- on the set API  */
    /*****************************************/

    numOfBits = 0;
    st = prvCpssFieldValueGet(memArray,startIndex,numOfBits,&value);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PARAM, st);

    numOfBits = 33;
    st = prvCpssFieldValueGet(memArray,startIndex,numOfBits,&value);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PARAM, st);

    numOfBits = 0;
    st = prvCpssFieldValueGet(memArray,startIndex,numOfBits,NULL);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

    numOfBits = 0;
    st = prvCpssFieldValueGet(NULL,startIndex,numOfBits,&value);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of prvCpssMisc suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(prvCpssMisc)
    UTF_SUIT_DECLARE_TEST_MAC(prvCpssFieldValueSet)
UTF_SUIT_END_TESTS_MAC(prvCpssMisc)
