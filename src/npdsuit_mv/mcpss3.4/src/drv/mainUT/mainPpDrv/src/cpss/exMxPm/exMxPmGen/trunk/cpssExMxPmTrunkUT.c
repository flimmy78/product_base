/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmTrunkUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmTrunk, that provides
*       API definitions for 802.3ad Link Aggregation (Trunk) facility
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/trunk/cpssExMxPmTrunk.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for virtual port id */
#define TRUNK_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define TRUNK_INVALID_ENUM_CNS    0x5AAAAAA5

/* Maximum value for trun id */
#define TRUNK_MAX_TRUNK_ID_CNS    256

/* Tested trunk id */
#define TRUNK_TESTED_TRUNK_ID_CNS 2

/* This value is used as null index. */
#define TRUNK_NULL_TRUNK_ID_CNS   0


/*----------------------------------------------------------------------------*/
/*
High Level mode

GT_STATUS cpssExMxPmTrunkMembersSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_U32                   numOfEnabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    enabledMembersArray[],
    IN GT_U32                   numOfDisabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    disabledMembersArray[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkMembersSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with trunkId [2],
                   numOfEnabledMembers [5],
                   enabledMembersArray [0 / 1 / 2 / 3 / 4] {port [0 / 1 / 2 / 3 / 4], device [devNum]},
                   numOfDisabledMembers [7],
                   disabledMembersArray [0 / ... / 7] {port [5 / ... / 11], device [devNum]}.
    Expected: GT_OK.
    1.2. Call with trunkId [3],
                   numOfEnabledMembers [2],
                   enabledMembersArray [0 / 1] (already added) {port [7 / 4], device [devNum]},
                   numOfDisabledMembers [2],
                   disabledMembersArray [0 / 1] {port [6 / 2], device [devNum]}.
    Expected: NON GT_OK.
    1.3. Call with trunkId [2],
                   numOfEnabledMembers [1],
                   enabledMembersArray[0] {port [0], out of range device [PRV_CPSS_MAX_PP_DEVICES_CNS]},
                   numOfDisabledMembers [0].
    Expected: NON GT_OK.
    1.4. Call with trunkId [2],
                   numOfEnabledMembers [1],
                   enabledMembersArray[0] {out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], device [devNum]},
                   numOfDisabledMembers [0].
    Expected: NON GT_OK.
    1.5. Call cpssExMxPmTrunkDbEnabledMembersGet with trunkId [2],
                                                      non-NULL numOfEnabledMembersPtr,
                                                      and allocated enabledMembersArray for 12 members.
    Expected: GT_OK, numOfEnabledMembersPtr [5], and the same enabledMembersArray as written.
    1.6. Call cpssExMxPmTrunkDbDisabledMembersGet with trunkId [2],
                                                       non-NULL numOfDisabledMembersPtr,
                                                       and allocated disabledMembersArray for 12 members.
    Expected: GT_OK, numOfDisabledMembersPtr [7], and the same disabledMembersArray as written.
    1.7. Check out of range for sum [>CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS = 12] of enabled and disabled members.
         Call with trunkId [2],
                   numOfEnabledMembers [6],
                   enabledMembersArray [0 / 1 / 2 / 3 / 4 / 5] {port [0 / 1 / 2 / 3 / 4 / 5], device [devNum]},
                   numOfDisabledMembers [7],
                   disabledMembersArray [tested dev id, virtual port id 6, 7, 8, 9, 10, 11, 12].
    Expected: NON GT_OK.
    1.8. Call with null trunkId [NULL_TRUNK_ID_CNS = 0],
                   numOfEnabledMembers [5],
                   enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4],
                   numOfDisabledMembers [7],
                   disabledMembersArray [tested dev id, virtual port id 5 ... 11].
    Expected: NON GT_OK.
    1.9. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                   numOfEnabledMembers [5],
                   enabledMembersArray [0 / 1 / 2 / 3 / 4] {port [0 / 1 / 2 / 3 / 4], device [devNum]},
                   numOfDisabledMembers [7],
                   disabledMembersArray [0 /  / 6] {port [5 / .. / 11], device [devNum]}.
    Expected: NON GT_OK.
    1.10. Call with trunkId [2],
                    numOfEnabledMembers [0],
                    enabledMembersArray [NULL],
                    numOfDisabledMembers [0],
                    disabledMembersArray [NULL] to remove all members from the trunk.
    Expected: GT_OK.
    1.11. Call with trunkId [2],
                    numOfEnabledMembers [1],
                    enabledMembersArray [NULL],
                    numOfDisabledMembers [0],
                    non-null  disabledMembersArray.
    Expected: GT_BAD_PTR.
    1.12. Call with trunkId [2],
                    numOfEnabledMembers [0],
                    non-null enabledMembersArray,
                    numOfDisabledMembers [1],
                    disabledMembersArray[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_TRUNK_ID             trunkId              = 0;
    GT_U32                  numOfEnabledMembers  = 0;
    CPSS_TRUNK_MEMBER_STC   enabledMembersArray [CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U32                  numOfDisabledMembers = 0;
    CPSS_TRUNK_MEMBER_STC   disabledMembersArray[CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_BOOL                 isEqual              = GT_FALSE;
    GT_U32                  numOfMembersWritten  = 0;
    CPSS_TRUNK_MEMBER_STC   membersArrayWritten [CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunkId [2],
                           numOfEnabledMembers [5],
                           enabledMembersArray [0 / 1 / 2 / 3 / 4] {port [0 / 1 / 2 / 3 / 4], device [devNum]},
                           numOfDisabledMembers [7],
                           disabledMembersArray [0 / ... / 7] {port [5 / ... / 11], device [devNum]}.
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        numOfEnabledMembers = 5;

        enabledMembersArray[0].device = dev;
        enabledMembersArray[0].port   = 0;
        enabledMembersArray[1].device = dev;
        enabledMembersArray[1].port   = 1;
        enabledMembersArray[2].device = dev;
        enabledMembersArray[2].port   = 2;
        enabledMembersArray[3].device = dev;
        enabledMembersArray[3].port   = 3;
        enabledMembersArray[4].device = dev;
        enabledMembersArray[4].port   = 4;

        numOfDisabledMembers = 7;

        disabledMembersArray[0].device = dev;
        disabledMembersArray[0].port   = 5;
        disabledMembersArray[1].device = dev;
        disabledMembersArray[1].port   = 6;
        disabledMembersArray[2].device = dev;
        disabledMembersArray[2].port   = 7;
        disabledMembersArray[3].device = dev;
        disabledMembersArray[3].port   = 8;
        disabledMembersArray[4].device = dev;
        disabledMembersArray[4].port   = 9;
        disabledMembersArray[5].device = dev;
        disabledMembersArray[5].port   = 10;
        disabledMembersArray[6].device = dev;
        disabledMembersArray[6].port   = 11;

        st = cpssExMxPmTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                       numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, numOfEnabledMembers = %d, numOfDisabledMembers = %d",
                                     dev, trunkId, numOfEnabledMembers, numOfDisabledMembers);

        /*
            1.2. Call with trunkId [3],
                           numOfEnabledMembers [2],
                           enabledMembersArray [0 / 1] (already added) {port [7 / 4], device [devNum]},
                           numOfDisabledMembers [2],
                           disabledMembersArray [0 / 1] {port [6 / 2], device [devNum]}.
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS + 1;

        numOfEnabledMembers = 2;
        enabledMembersArray[0].device = dev;
        enabledMembersArray[0].port   = 7;
        enabledMembersArray[1].device = dev;
        enabledMembersArray[1].port   = 4;

        numOfDisabledMembers = 2;

        disabledMembersArray[0].device = dev;
        disabledMembersArray[0].port   = 6;
        disabledMembersArray[1].device = dev;
        disabledMembersArray[1].port   = 2;

        st = cpssExMxPmTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                       numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.3. Call with trunkId [2],
                           numOfEnabledMembers [1],
                           enabledMembersArray[0] {port [0], out of range device [PRV_CPSS_MAX_PP_DEVICES_CNS]},
                           numOfDisabledMembers [0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        numOfEnabledMembers = 1;
        enabledMembersArray[0].device = PRV_CPSS_MAX_PP_DEVICES_CNS;
        enabledMembersArray[0].port   = 0;

        numOfDisabledMembers = 0;

        st = cpssExMxPmTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                       numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, enabledMembersArray[0]->device = %d",
                                         dev, enabledMembersArray[0].device);

        /*
            1.4. Call with trunkId [2],
                           numOfEnabledMembers [1],
                           enabledMembersArray[0] {out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], device [devNum]},
                           numOfDisabledMembers [0].
            Expected: NON GT_OK.
        */
        numOfEnabledMembers = 1;
        enabledMembersArray[0].device = dev;
        enabledMembersArray[0].port   = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        numOfDisabledMembers = 0;

        st = cpssExMxPmTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                       numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, enabledMembersArray[0]->port = %d",
                                         dev, enabledMembersArray[0].port);

        /*
            1.5. Call cpssExMxPmTrunkDbEnabledMembersGet with trunkId [2],
                                                              non-NULL numOfEnabledMembersPtr,
                                                              and allocated enabledMembersArray for 12 members.
            Expected: GT_OK, numOfEnabledMembersPtr [5], and the same enabledMembersArray as written.
        */
        numOfEnabledMembers = 5;

        enabledMembersArray[0].device = dev;
        enabledMembersArray[0].port   = 0;
        enabledMembersArray[1].device = dev;
        enabledMembersArray[1].port   = 1;
        enabledMembersArray[2].device = dev;
        enabledMembersArray[2].port   = 2;
        enabledMembersArray[3].device = dev;
        enabledMembersArray[3].port   = 3;
        enabledMembersArray[4].device = dev;
        enabledMembersArray[4].port   = 4;

        /* must initialize to the max number of members we wish to receive */
        numOfMembersWritten = CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS;

        st = cpssExMxPmTrunkDbEnabledMembersGet(dev, trunkId, &numOfMembersWritten, membersArrayWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmTrunkDbEnabledMembersGet: %d, %d", dev, trunkId);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(numOfEnabledMembers, numOfMembersWritten,
                   "get another numOfEnabledMembers than was set: %d, %d", dev, trunkId);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) enabledMembersArray,
                                     (GT_VOID*) membersArrayWritten,
                                     numOfEnabledMembers * sizeof(enabledMembersArray[0]))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                   "get another enabledMembersArray than was set: %d, %d", dev, trunkId);

        /*
            1.6. Call cpssExMxPmTrunkDbDisabledMembersGet with trunkId [2],
                                                               non-NULL numOfDisabledMembersPtr,
                                                               and allocated disabledMembersArray for 12 members.
            Expected: GT_OK, numOfDisabledMembersPtr [7], and the same disabledMembersArray as written.
        */
        numOfDisabledMembers = 7;

        disabledMembersArray[0].device = dev;
        disabledMembersArray[0].port   = 5;
        disabledMembersArray[1].device = dev;
        disabledMembersArray[1].port   = 6;
        disabledMembersArray[2].device = dev;
        disabledMembersArray[2].port   = 7;
        disabledMembersArray[3].device = dev;
        disabledMembersArray[3].port   = 8;
        disabledMembersArray[4].device = dev;
        disabledMembersArray[4].port   = 9;
        disabledMembersArray[5].device = dev;
        disabledMembersArray[5].port   = 10;
        disabledMembersArray[6].device = dev;
        disabledMembersArray[6].port   = 11;

        /* must initialize to the max number of members we wish to receive */
        numOfMembersWritten = CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS;

        st = cpssExMxPmTrunkDbDisabledMembersGet(dev, trunkId, &numOfMembersWritten, membersArrayWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmTrunkDbDisabledMembersGet: %d, %d", dev, trunkId);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(numOfDisabledMembers, numOfMembersWritten,
                   "get another numOfDisabledMembers than was set: %d, %d", dev, trunkId);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) disabledMembersArray,
                                     (GT_VOID*) membersArrayWritten,
                                     numOfDisabledMembers * sizeof(disabledMembersArray[0]))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                   "get another disabledMembersArray than was set: %d, %d", dev, trunkId);

        /*
            1.7. Check out of range for sum [>CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS = 12] of enabled and disabled members.
                 Call with trunkId [2],
                           numOfEnabledMembers [6],
                           enabledMembersArray [0 / 1 / 2 / 3 / 4 / 5] {port [0 / 1 / 2 / 3 / 4 / 5], device [devNum]},
                           numOfDisabledMembers [7],
                           disabledMembersArray [tested dev id, virtual port id 6, 7, 8, 9, 10, 11, 12].
            Expected: NON GT_OK.
        */
        numOfEnabledMembers = 6;

        enabledMembersArray[0].device = dev;
        enabledMembersArray[0].port   = 0;
        enabledMembersArray[1].device = dev;
        enabledMembersArray[1].port   = 1;
        enabledMembersArray[2].device = dev;
        enabledMembersArray[2].port   = 2;
        enabledMembersArray[3].device = dev;
        enabledMembersArray[3].port   = 3;
        enabledMembersArray[4].device = dev;
        enabledMembersArray[4].port   = 4;
        enabledMembersArray[5].device = dev;
        enabledMembersArray[5].port   = 5;

        numOfDisabledMembers = 7;

        disabledMembersArray[0].device = dev;
        disabledMembersArray[0].port   = 6;
        disabledMembersArray[1].device = dev;
        disabledMembersArray[1].port   = 7;
        disabledMembersArray[2].device = dev;
        disabledMembersArray[2].port   = 8;
        disabledMembersArray[3].device = dev;
        disabledMembersArray[3].port   = 9;
        disabledMembersArray[4].device = dev;
        disabledMembersArray[4].port   = 10;
        disabledMembersArray[5].device = dev;
        disabledMembersArray[5].port   = 11;
        disabledMembersArray[6].device = dev;
        disabledMembersArray[6].port   = 12;

        st = cpssExMxPmTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                       numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, numOfEnabledMembers + numOfDisabledMembers = %d",
                                         dev, trunkId, numOfEnabledMembers + numOfDisabledMembers);

        /*
            1.8. Call with null trunkId [NULL_TRUNK_ID_CNS = 0],
                           numOfEnabledMembers [5],
                           enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4],
                           numOfDisabledMembers [7],
                           disabledMembersArray [tested dev id, virtual port id 5 ... 11].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        numOfEnabledMembers = 5;

        enabledMembersArray[0].device = dev;
        enabledMembersArray[0].port   = 0;
        enabledMembersArray[1].device = dev;
        enabledMembersArray[1].port   = 1;
        enabledMembersArray[2].device = dev;
        enabledMembersArray[2].port   = 2;
        enabledMembersArray[3].device = dev;
        enabledMembersArray[3].port   = 3;
        enabledMembersArray[4].device = dev;
        enabledMembersArray[4].port   = 4;

        numOfDisabledMembers = 7;

        disabledMembersArray[0].device = dev;
        disabledMembersArray[0].port   = 5;
        disabledMembersArray[1].device = dev;
        disabledMembersArray[1].port   = 6;
        disabledMembersArray[2].device = dev;
        disabledMembersArray[2].port   = 7;
        disabledMembersArray[3].device = dev;
        disabledMembersArray[3].port   = 8;
        disabledMembersArray[4].device = dev;
        disabledMembersArray[4].port   = 9;
        disabledMembersArray[5].device = dev;
        disabledMembersArray[5].port   = 10;
        disabledMembersArray[6].device = dev;
        disabledMembersArray[6].port   = 11;

        st = cpssExMxPmTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                       numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.9. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                           numOfEnabledMembers [5],
                           enabledMembersArray [0 / 1 / 2 / 3 / 4] {port [0 / 1 / 2 / 3 / 4], device [devNum]},
                           numOfDisabledMembers [7],
                           disabledMembersArray [0 /  / 6] {port [5 / .. / 11], device [devNum]}.
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                       numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.10. Call with trunkId [2],
                            numOfEnabledMembers [0],
                            enabledMembersArray [NULL],
                            numOfDisabledMembers [0],
                            disabledMembersArray [NULL] to remove all members from the trunk.
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        numOfEnabledMembers  = 0;
        numOfDisabledMembers = 0;

        st = cpssExMxPmTrunkMembersSet(dev, trunkId, numOfEnabledMembers, NULL,
                                       numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.11. Call with trunkId [2],
                            numOfEnabledMembers [1],
                            enabledMembersArray [NULL],
                            numOfDisabledMembers [0],
                            non-null  disabledMembersArray.
            Expected: GT_BAD_PTR.
        */

        numOfEnabledMembers  = 1;
        numOfDisabledMembers = 0;

        st = cpssExMxPmTrunkMembersSet(dev, trunkId, numOfEnabledMembers, NULL,
                                       numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, numOfEnabledMembers = %d, enabledMembersArray = NULL",
                                     dev, numOfEnabledMembers);

        /*
            1.12. Call with trunkId [2],
                            numOfEnabledMembers [0],
                            non-null enabledMembersArray,
                            numOfDisabledMembers [1],
                            disabledMembersArray[NULL].
            Expected: GT_BAD_PTR.
        */

        numOfEnabledMembers  = 0;
        numOfDisabledMembers = 1;

        st = cpssExMxPmTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                       numOfDisabledMembers, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, numOfDisabledMembers = %d, disabledMembersArray = NULL",
                                     dev, numOfDisabledMembers);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    numOfEnabledMembers = 5;

    enabledMembersArray[0].device = dev;
    enabledMembersArray[0].port   = 0;
    enabledMembersArray[1].device = dev;
    enabledMembersArray[1].port   = 1;
    enabledMembersArray[2].device = dev;
    enabledMembersArray[2].port   = 2;
    enabledMembersArray[3].device = dev;
    enabledMembersArray[3].port   = 3;
    enabledMembersArray[4].device = dev;
    enabledMembersArray[4].port   = 4;

    numOfDisabledMembers = 7;

    disabledMembersArray[0].device = dev;
    disabledMembersArray[0].port   = 5;
    disabledMembersArray[1].device = dev;
    disabledMembersArray[1].port   = 6;
    disabledMembersArray[2].device = dev;
    disabledMembersArray[2].port   = 7;
    disabledMembersArray[3].device = dev;
    disabledMembersArray[3].port   = 8;
    disabledMembersArray[4].device = dev;
    disabledMembersArray[4].port   = 9;
    disabledMembersArray[5].device = dev;
    disabledMembersArray[5].port   = 10;
    disabledMembersArray[6].device = dev;
    disabledMembersArray[6].port   = 11;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                       numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                       numOfDisabledMembers, disabledMembersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High Level mode

GT_STATUS cpssExMxPmTrunkMemberAdd
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkMemberAdd)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with trunkId [2],
                   memberPtr {port [0], device [devNum]}.
    Expected: GT_OK.
    1.2. Call with trunkId [2]
                   and memberPtr {port [0], device [devNum]} (already added).
    Expected: GT_OK.
    1.3. Call with trunkId [3],
                   memberPtr {port [0], device [devNum]} (already added to another trunkId).
    Expected: NON GT_OK.
    1.4. Call with trunkId [2],
                   non-NULL memberPtr {port [0], out of range device [PRV_CPSS_MAX_PP_DEVICES_CNS]}.
    Expected: NON GT_OK.
    1.5. Call with trunkId [2],
                   non-NULL memberPtr {out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], device [devNum]}.
    Expected: NON GT_OK.
    1.6. Call with null trunkId [NULL_TRUNK_ID_CNS = 0],
                   non-NULL memberPtr [tested dev, virtual port 0].
    Expected: NON GT_OK.
    1.7. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                   memberPtr [tested dev, virtual port 0].
    Expected: NON GT_OK.
    1.8. Call with trunkId [2],
                   memberPtr [NULL].
    Expected: GT_BAD_PTR.
    1.9. Call cpssExMxPmTrunkMemberRemove with trunkId [2],
                                               memberPtr {port [0], device [devNum]} to remove added member from the trunk.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_TRUNK_ID             trunkId = 0;
    CPSS_TRUNK_MEMBER_STC   member;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunkId [2],
                           memberPtr {port [0], device [devNum]}.
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        member.device = dev;
        member.port   = TRUNK_VALID_VIRT_PORT_CNS;

        st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                     dev, trunkId, member.device, member.port);

        /*
            1.2. Call with trunkId [2]
                           and memberPtr {port [0], device [devNum]} (already added).
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                     dev, trunkId, member.device, member.port);

        /*
            1.3. Call with trunkId [3],
                           memberPtr {port [0], device [devNum]} (already added to another trunkId).
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS + 1;

        st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                         dev, trunkId, member.device, member.port);

        /*
            1.4. Call with trunkId [2],
                           non-NULL memberPtr {port [0], out of range device [PRV_CPSS_MAX_PP_DEVICES_CNS]}.
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        member.device = PRV_CPSS_MAX_PP_DEVICES_CNS;
        member.port   = TRUNK_VALID_VIRT_PORT_CNS;

        st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d",
                                         dev, trunkId, member.device);

        member.device = dev;

        /*
            1.5. Call with trunkId [2],
                           non-NULL memberPtr {out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], device [devNum]}.
            Expected: NON GT_OK.
        */
        member.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->port = %d",
                                         dev, trunkId, member.port);

        member.port = TRUNK_VALID_VIRT_PORT_CNS;

        /*
            1.6. Call with null trunkId [NULL_TRUNK_ID_CNS = 0],
                           non-NULL memberPtr [tested dev, virtual port 0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.7. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                           memberPtr [tested dev, virtual port 0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        /*
            1.8. Call with trunkId [2],
                           memberPtr [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssExMxPmTrunkMemberAdd(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, memberPtr = NULL", dev, trunkId);

        /*
            1.9. Call cpssExMxPmTrunkMemberRemove with trunkId [2],
                                                       memberPtr {port [0], device [devNum]} to remove added member from the trunk.
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkMemberRemove: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    member.device = 0;
    member.port   = TRUNK_VALID_VIRT_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High Level mode

GT_STATUS cpssExMxPmTrunkMemberRemove
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkMemberRemove)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmTrunkMemberAdd with trunkId [2],
                                            non-NULL memberPtr {port [0], device [devNum]} to add new member.
    Expected: GT_OK.
    1.2. Call with trunkId [2],
                   non-NULL memberPtr {port [0], device [devNum]}.
    Expected: GT_OK.
    1.3. Call with trunkId [2],
                   non-NULL memberPtr {port [0], device [devNum]} (already removed).
    Expected: GT_OK.
    1.4. Call with trunkId [2],
                   non-NULL memberPtr {port [0], out of range device [PRV_CPSS_MAX_PP_DEVICES_CNS]}.
    Expected: NOT GT_OK.
    1.5. Call with trunkId [2],
                   non-NULL memberPtr {out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], device [devNum]}.
    Expected: NON GT_OK.
    1.6. Call with null trunkId [NULL_TRUNK_ID_CNS = 0],
                   non-NULL memberPtr [tested dev, virtual port 0].
    Expected: NON GT_OK.
    1.7. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                   non-NULL memberPtr [tested dev, virtual port 0].
    Expected: NON GT_OK.
    1.8. Check for NULL handling.
         Call with trunkId [2],
                   memberPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_TRUNK_ID             trunkId = 0;
    CPSS_TRUNK_MEMBER_STC   member;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxPmTrunkMemberAdd with trunkId [2],
                                                    non-NULL memberPtr {port [0], device [devNum]} to add new member.
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        member.device = dev;
        member.port   = TRUNK_VALID_VIRT_PORT_CNS;

        st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkMemberAdd: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /*
            1.2. Call with trunkId [2],
                           non-NULL memberPtr {port [0], device [devNum]}.
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                     dev, trunkId, member.device, member.port);

        /*
            1.3. Call with trunkId [2],
                           non-NULL memberPtr {port [0], device [devNum]} (already removed).
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                     dev, trunkId, member.device, member.port);

        /*
            1.4. Call with trunkId [2],
                           non-NULL memberPtr {port [0], out of range device [PRV_CPSS_MAX_PP_DEVICES_CNS]}.
            Expected: GT_OK.
        */
        member.device = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d",
                                         dev, trunkId, member.device);

        member.device = dev;

        /*
            1.5. Call with trunkId [2],
                           non-NULL memberPtr {out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], device [devNum]}.
            Expected: GT_OK.
        */
        member.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->port = %d",
                                         dev, trunkId, member.port);

        member.port = TRUNK_VALID_VIRT_PORT_CNS;

        /*
            1.6. Call with null trunkId [NULL_TRUNK_ID_CNS = 0],
                           non-NULL memberPtr [tested dev, virtual port 0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.7. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                           non-NULL memberPtr [tested dev, virtual port 0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        /*
            1.8. Check for NULL handling.
                 Call with trunkId [2],
                           memberPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkMemberRemove(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, memberPtr = NULL", dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    member.device = dev;
    member.port   = TRUNK_VALID_VIRT_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High Level mode

GT_STATUS cpssExMxPmTrunkMemberDisable
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkMemberDisable)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmTrunkMemberAdd with trunkId [2],
                                            non-NULL memberPtr {port [0], device [devNum]}.
    Expected: GT_OK.
    1.2. Call with trunkId [2],
                   non-NULL memberPtr {port [0], device [devNum]}.
    Expected: GT_OK.
    1.3. Call with trunkId [2],
                   non-NULL memberPtr {port [0], device [devNum]} (already disabled).
    Expected: GT_OK.
    1.4. Call with trunkId [2],
                   non-NULL memberPtr {port [0], out of range device [PRV_CPSS_MAX_PP_DEVICES_CNS]}
    Expected: NON GT_OK.
    1.5. Call with trunkId [2],
                   non-NULL memberPtr {out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], device [devNum]}.
    Expected: NON GT_OK.
    1.6. Call cpssExMxPmTrunkMemberRemove with trunkId [2],
                                               non-NULL memberPtr {port [0], device [devNum]}.
    Expected: GT_OK.
    1.7. Call with trunkId [2],
                   non-NULL memberPtr {port [0], device [devNum]} (removed member).
    Expected: NON GT_OK.
    1.8. Call with null trunkId [NULL_TRUNK_ID_CNS = 0],
                   non-NULL memberPtr [tested dev, virtual port 0].
    Expected: NON GT_OK.
    1.9. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                   non-NULL memberPtr {port [0], device [devNum]}.
    Expected: NON GT_OK.
    1.10. Call with trunkId [2], memberPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_TRUNK_ID             trunkId = 0;
    CPSS_TRUNK_MEMBER_STC   member;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxPmTrunkMemberAdd with trunkId [2],
                                                    non-NULL memberPtr {port [0], device [devNum]}.
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        member.device = dev;
        member.port   = TRUNK_VALID_VIRT_PORT_CNS;

        st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkMemberAdd: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /*
            1.2. Call with trunkId [2],
                           non-NULL memberPtr {port [0], device [devNum]}.
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                     dev, trunkId, member.device, member.port);

        /*
            1.3. Call with trunkId [2],
                           non-NULL memberPtr {port [0], device [devNum]} (already disabled).
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                     dev, trunkId, member.device, member.port);

        /*
            1.4. Call with trunkId [2],
                           non-NULL memberPtr {port [0], out of range device [PRV_CPSS_MAX_PP_DEVICES_CNS]}
            Expected: NON GT_OK.
        */
        member.device = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d",
                                         dev, trunkId, member.device);

        member.device = dev;

        /*
            1.5. Call with trunkId [2],
                           non-NULL memberPtr {out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], device [devNum]}.
            Expected: NON GT_OK.
        */
        member.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->port = %d",
                                         dev, trunkId, member.port);

        member.port = TRUNK_VALID_VIRT_PORT_CNS;

        /*
            1.6. Call cpssExMxPmTrunkMemberRemove with trunkId [2],
                                                       non-NULL memberPtr {port [0], device [devNum]}.
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkMemberRemove: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /*
            1.7. Call with trunkId [2],
                           non-NULL memberPtr {port [0], device [devNum]} (removed member).
            Expected: NON GT_OK.
        */
        st = cpssExMxPmTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                         dev, trunkId, member.device, member.port);

        /*
            1.8. Call with null trunkId [NULL_TRUNK_ID_CNS = 0],
                           non-NULL memberPtr [tested dev, virtual port 0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.9. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                           non-NULL memberPtr {port [0], device [devNum]}.
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        /*
            1.10. Call with trunkId [2],
                            memberPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkMemberDisable(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, memberPtr = NULL", dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    member.device = 0;
    member.port   = TRUNK_VALID_VIRT_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkMemberDisable(dev, trunkId, &member);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High Level mode

GT_STATUS cpssExMxPmTrunkMemberEnable
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkMemberEnable)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmTrunkMemberAdd with trunkId [2],
                                            non-NULL memberPtr {port [0], device [devNum]}.
    Expected: GT_OK.
    1.2. Call with trunkId [2],
                   non-NULL memberPtr {port [0], device [devNum]}.
    Expected: GT_OK.
    1.3. Call with trunkId [2],
                   non-NULL memberPtr {port [0], device [devNum]} (already enabled).
    Expected: GT_OK.
    1.4. Call with trunkId [2],
                   non-NULL memberPtr {port [0], out of range device [PRV_CPSS_MAX_PP_DEVICES_CNS]}.
    Expected: NON GT_OK.
    1.5. Call with trunkId [2],
                   non-NULL memberPtr {out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], device [devNum]}.
    Expected: NON GT_OK.
    1.6. Call cpssExMxPmTrunkMemberRemove with trunkId [2],
                                               non-NULL memberPtr {port [0], device [devNum]}.
    Expected: GT_OK.
    1.7. Call with trunkId [2],
                   non-NULL memberPtr {port [0], device [devNum]} (removed member).
    Expected: NON GT_OK.
    1.8. Call with trunkId [NULL_TRUNK_ID_CNS = 0],
                   non-NULL memberPtr {port [0], device [devNum]}.
    Expected: NON GT_OK.
    1.9. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                   non-NULL memberPtr {port [0], device [devNum]}.
    Expected: NON GT_OK.
    1.10. Call with trunkId [2],
                    memberPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_TRUNK_ID             trunkId = 0;
    CPSS_TRUNK_MEMBER_STC   member;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxPmTrunkMemberAdd with trunkId [2],
                                                    non-NULL memberPtr {port [0], device [devNum]}.
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        member.device = dev;
        member.port   = TRUNK_VALID_VIRT_PORT_CNS;

        st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkMemberAdd: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /*
            1.2. Call with trunkId [2],
                           non-NULL memberPtr {port [0], device [devNum]}.
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                     dev, trunkId, member.device, member.port);

        /*
            1.3. Call with trunkId [2],
                           non-NULL memberPtr {port [0], device [devNum]} (already enabled).
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                     dev, trunkId, member.device, member.port);

        /*
            1.4. Call with trunkId [2],
                           non-NULL memberPtr {port [0], out of range device [PRV_CPSS_MAX_PP_DEVICES_CNS]}
            Expected: NON GT_OK.
        */
        member.device = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxPmTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d",
                                         dev, trunkId, member.device);

        member.device = dev;

        /*
            1.5. Call with trunkId [2],
                           non-NULL memberPtr {out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], device [devNum]}.
            Expected: NON GT_OK.
        */
        member.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPmTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->port = %d",
                                         dev, trunkId, member.port);

        member.port = TRUNK_VALID_VIRT_PORT_CNS;

        /*
            1.6. Call cpssExMxPmTrunkMemberRemove with trunkId [2],
                                                       non-NULL memberPtr {port [0], device [devNum]}.
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkMemberRemove: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /*
            1.7. Call with trunkId [2],
                           non-NULL memberPtr {port [0], device [devNum]} (removed member).
            Expected: NON GT_OK.
        */
        st = cpssExMxPmTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                         dev, trunkId, member.device, member.port);

        /*
            1.8. Call with null trunkId [NULL_TRUNK_ID_CNS = 0],
                           non-NULL memberPtr [tested dev, virtual port 0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.9. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                           non-NULL memberPtr {port [0], device [devNum]}.
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        /*
            1.10. Call with trunkId [2],
                            memberPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkMemberEnable(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, memberPtr = NULL", dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    member.device = 0;
    member.port   = TRUNK_VALID_VIRT_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkMemberEnable(dev, trunkId, &member);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low Level mode

GT_STATUS cpssExMxPmTrunkNonTrunkPortsAdd
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *nonTrunkPortsBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkNonTrunkPortsAdd)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with trunkId [2],
                   nonTrunkPortsBmpPtr->ports [3, 0].
    Expected: GT_OK.
    1.2. Call with trunkId [NULL_TRUNK_ID_CNS = 0],
                   non-NULL nonTrunkPortsBmpPtr [3, 0].
    Expected: GT_OK.
    1.3. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                   non-NULL nonTrunkPortsBmpPtr [3, 0].
    Expected: NON GT_OK.
    1.4. Call with trunkId [2],
                   nonTrunkPortsBmpPtr [NULL].
    Expected: GT_BAD_PTR.
    1.5. Call cpssExMxPmTrunkNonTrunkPortsRemove with trunkId [2],
                                                 nonTrunkPortsBmp [3, 0] to remove all non trunk members from the trunk.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_TRUNK_ID         trunkId = 0;
    CPSS_PORTS_BMP_STC  nonTrunkPortsBmp;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunkId [2],
                           nonTrunkPortsBmpPtr->ports [3, 0].
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        nonTrunkPortsBmp.ports[0] = 3;
        nonTrunkPortsBmp.ports[1] = 0;

        st = cpssExMxPmTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.2. Call with trunkId [NULL_TRUNK_ID_CNS = 0],
                           non-NULL nonTrunkPortsBmpPtr [3, 0].
            Expected: GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.3. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                           non-NULL nonTrunkPortsBmpPtr [3, 0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        /*
            1.4. Call with trunkId [2],
                           nonTrunkPortsBmpPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkNonTrunkPortsAdd(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nonTrunkPortsBmpPtr = NULL", dev, trunkId);

        /*
            1.5. Call cpssExMxPmTrunkNonTrunkPortsRemove with trunkId [2],
                                                               nonTrunkPortsBmp [3, 0] to remove all non trunk members from the trunk.
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkNonTrunkPortsRemove: %d, %d",
                                     dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    nonTrunkPortsBmp.ports[0] = 3;
    nonTrunkPortsBmp.ports[1] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low Level mode

GT_STATUS cpssExMxPmTrunkNonTrunkPortsRemove
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *nonTrunkPortsBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkNonTrunkPortsRemove)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmTrunkNonTrunkPortsAdd with trunkId [2],
                                                   nonTrunkPortsBmpPtr [3, 0].
    Expected: GT_OK.
    1.2. Call with trunkId [2],
                   non-NULL nonTrunkPortsBmpPtr [3, 0].
    Expected: GT_OK.
    1.3. Call with trunkId [NULL_TRUNK_ID_CNS = 0],
                   non-NULL nonTrunkPortsBmpPtr [3, 0].
    Expected: GT_OK.
    1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                   non-NULL nonTrunkPortsBmpPtr [3, 0].
    Expected: NON GT_OK.
    1.5. Call with trunkId [2],
                   nonTrunkPortsBmpPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_TRUNK_ID         trunkId = 0;
    CPSS_PORTS_BMP_STC  nonTrunkPortsBmp;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxPmTrunkNonTrunkPortsAdd with trunkId [2],
                                                           nonTrunkPortsBmpPtr [3, 0].
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        nonTrunkPortsBmp.ports[0] = 3;
        nonTrunkPortsBmp.ports[1] = 0;

        st = cpssExMxPmTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkNonTrunkPortsAdd: %d, %d",
                                     dev, trunkId);
        /*
            1.2. Call with trunkId [2],
                           nonTrunkPortsBmpPtr->ports [3, 0].
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.3. Call with trunkId [NULL_TRUNK_ID_CNS = 0],
                           non-NULL nonTrunkPortsBmpPtr [3, 0].
            Expected: GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                           non-NULL nonTrunkPortsBmpPtr [3, 0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        /*
            1.5. Call with trunkId [2],
                           nonTrunkPortsBmpPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkNonTrunkPortsRemove(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nonTrunkPortsBmpPtr = NULL", dev, trunkId);


    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    nonTrunkPortsBmp.ports[0] = 3;
    nonTrunkPortsBmp.ports[1] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low Level mode

GT_STATUS cpssExMxPmTrunkPortTrunkIdSet
(
    IN GT_U8            devNum,
    IN GT_U8            portNum,
    IN GT_BOOL          memberOfTrunk,
    IN GT_TRUNK_ID      trunkId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkPortTrunkIdSet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with memberOfTrunk [GT_TRUE],
                     trunkId [2].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmTrunkPortTrunkIdGet with non-NULL memberOfTrunkPtr,
                                              and non-NULL trunkIdPtr.
    Expected: GT_OK, memberOfTrunkPtr [GT_TRUE] and trunkIdPtr [2].
    1.1.3. Call with memberOfTrunk [GT_FALSE],
                     trunkId [2] (but trunkid will be set to 0).
    Expected: GT_OK.
    1.1.4. Call cpssExMxPmTrunkPortTrunkIdGet with non-NULL memberOfTrunkPtr,
                                              and non-NULL trunkIdPtr.
    Expected: GT_OK, memberOfTrunkPtr [GT_FALSE] and trunkIdPtr [0].
    1.1.5. Call with trunkId [NULL_TRUNK_ID_CNS = 0]
                     and memberOfTrunk [GT_TRUE].
    Expected: NON GT_OK.
    1.1.6. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256]
                     memberOfTrunk [GT_TRUE].
    Expected: NON GT_OK.
    1.1.7. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256] (not relevant)
                     memberOfTrunk [GT_FALSE].
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = TRUNK_VALID_VIRT_PORT_CNS;

    GT_BOOL     memberOfTrunk    = GT_FALSE;
    GT_TRUNK_ID trunkId          = 0;
    GT_BOOL     memberOfTrunkGet = GT_FALSE;
    GT_TRUNK_ID trunkIdGet       = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with memberOfTrunk [GT_TRUE],
                                 trunkId [2].
                Expected: GT_OK.
            */
            trunkId       = TRUNK_TESTED_TRUNK_ID_CNS;
            memberOfTrunk = GT_TRUE;

            st = cpssExMxPmTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, memberOfTrunk, trunkId);

            /*
                1.1.2. Call cpssExMxPmTrunkPortTrunkIdGet with non-NULL memberOfTrunkPtr,
                                                          and non-NULL trunkIdPtr.
                Expected: GT_OK, memberOfTrunkPtr [GT_TRUE] and trunkIdPtr [2].
            */
            st = cpssExMxPmTrunkPortTrunkIdGet(dev, port, &memberOfTrunkGet, &trunkIdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkPortTrunkIdGet: %d, %d", dev, port);

            /* Verifuing values */
            UTF_VERIFY_EQUAL2_STRING_MAC(memberOfTrunk, memberOfTrunkGet,
                   "get another memberOfTrunk than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(trunkId, trunkIdGet,
                   "get another trunkId than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with memberOfTrunk [GT_FALSE],
                                 trunkId [2] (but trunkid will be set to 0).
                Expected: GT_OK.
            */
            memberOfTrunk = GT_FALSE;

            st = cpssExMxPmTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, memberOfTrunk, trunkId);

            /*
                1.1.4. Call cpssExMxPmTrunkPortTrunkIdGet with non-NULL memberOfTrunkPtr,
                                                          and non-NULL trunkIdPtr.
                Expected: GT_OK, memberOfTrunkPtr [GT_FALSE] and trunkIdPtr [0].
            */
            st = cpssExMxPmTrunkPortTrunkIdGet(dev, port, &memberOfTrunkGet, &trunkIdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkPortTrunkIdGet: %d, %d", dev, port);

            /* Verifuing values */
            trunkId = 0;

            UTF_VERIFY_EQUAL2_STRING_MAC(memberOfTrunk, memberOfTrunkGet,
                   "get another memberOfTrunkPtr than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(trunkId, trunkIdGet,
                   "get another trunkIdPtr than was set: %d, %d", dev, port);

            /*
                1.1.5. Call with trunkId [NULL_TRUNK_ID_CNS = 0]
                                 and memberOfTrunk [GT_TRUE].
                Expected: NON GT_OK.
            */
            trunkId       = TRUNK_NULL_TRUNK_ID_CNS;
            memberOfTrunk = GT_TRUE;

            st = cpssExMxPmTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, memberOfTrunk, trunkId);

            /*
                1.1.6. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256]
                                 memberOfTrunk [GT_TRUE].
                Expected: NON GT_OK.
            */
            trunkId = TRUNK_MAX_TRUNK_ID_CNS;

            st = cpssExMxPmTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, memberOfTrunk, trunkId);

            /*
                1.1.7. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256] (not relevant)
                                 memberOfTrunk [GT_FALSE].
                Expected: GT_OK.
            */
            memberOfTrunk = GT_FALSE;

            st = cpssExMxPmTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, memberOfTrunk, trunkId);
        }

        trunkId       = TRUNK_TESTED_TRUNK_ID_CNS;
        memberOfTrunk = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    trunkId       = TRUNK_TESTED_TRUNK_ID_CNS;
    memberOfTrunk = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TRUNK_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkPortTrunkIdGet
(
    IN GT_U8            devNum,
    IN GT_U8            portNum,
    OUT GT_BOOL         *memberOfTrunkPtr,
    OUT GT_TRUNK_ID     *trunkIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkPortTrunkIdGet)
{
/*
    ITERATE_DEVICES_VIRT_PORTS (ExMxPm)
    1.1.1. Call with non-NULL memberOfTrunkPtr,
                     and non-NULL trunkIdPtr.
    Expected: GT_OK.
    1.1.2. Call with memberOfTrunkPtr [NULL],
                     and non-NULL trunkIdPtr.
    Expected: GT_BAD_PTR.
    1.1.3. Call with non-NULL memberOfTrunkPtr,
                     and trunkIdPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = TRUNK_VALID_VIRT_PORT_CNS;

    GT_BOOL     memberOfTrunk = GT_FALSE;
    GT_TRUNK_ID trunkId       = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL memberOfTrunkPtr,
                                 and non-NULL trunkIdPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmTrunkPortTrunkIdGet(dev, port, &memberOfTrunk, &trunkId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with memberOfTrunkPtr [NULL],
                                 and non-NULL trunkIdPtr.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmTrunkPortTrunkIdGet(dev, port, NULL, &trunkId);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, memberOfTrunkPtr = NULL", dev, port);

            /*
                1.1.3. Call with non-NULL memberOfTrunkPtr,
                                 and trunkIdPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmTrunkPortTrunkIdGet(dev, port, &memberOfTrunk, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, trunkIdPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmTrunkPortTrunkIdGet(dev, port, &memberOfTrunk, &trunkId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmTrunkPortTrunkIdGet(dev, port, &memberOfTrunk, &trunkId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmTrunkPortTrunkIdGet(dev, port, &memberOfTrunk, &trunkId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = TRUNK_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkPortTrunkIdGet(dev, port, &memberOfTrunk, &trunkId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkPortTrunkIdGet(dev, port, &memberOfTrunk, &trunkId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low level mode

GT_STATUS cpssExMxPmTrunkTableEntrySet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    IN  GT_U32                  numMembers,
    IN  CPSS_TRUNK_MEMBER_STC   membersArray[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkTableEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with trunkId [2],
                   numMembers [3],
                   membersArray [0 / 1 / 2] {port [0 / 1 / 2], device [devNum]}.
    Expected: GT_OK.
    1.2. Call with cpssExMxPmTrunkTableEntryGet with trunkId [2], non-NULL numMembersPtr, non-NULL membersArray.
    Expected: GT_OK, numMembersPtr [3], and the same membersArray as was set.
    1.3. Call with out of range numMembers [13].
    Expected: NON GT_OK.
    1.4. Call with trunkId [NULL_TRUNK_ID_CNS = 0].
    Expected: NON GT_OK.
    1.5. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256].
    Expected: NON GT_OK.
    1.6. Call with trunkId [2],
                   numMembers [3],
                   membersArray [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_TRUNK_ID             trunkId       = 0;
    GT_U32                  numMembers    = 0;
    CPSS_TRUNK_MEMBER_STC   membersArray   [CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_BOOL                 isEqual       = GT_FALSE;
    GT_U32                  numMembersGet = 0;
    CPSS_TRUNK_MEMBER_STC   membersArrayGet[CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunkId [2],
                           numMembers [3],
                           membersArray [0 / 1 / 2] {port [0 / 1 / 2], device [devNum]}.
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        numMembers = 3;

        membersArray[0].device = dev;
        membersArray[0].port   = 0;
        membersArray[1].device = dev;
        membersArray[1].port   = 1;
        membersArray[2].device = dev;
        membersArray[2].port   = 2;

        st = cpssExMxPmTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trunkId, numMembers);

        /*
            1.2. Call with cpssExMxPmTrunkTableEntryGet with trunkId [2], non-NULL numMembersPtr, non-NULL membersArray.
            Expected: GT_OK, numMembersPtr [3], and the same membersArray as was set.
        */
        st = cpssExMxPmTrunkTableEntryGet(dev, trunkId, &numMembersGet, membersArrayGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmTrunkTableEntryGet: %d, %d", dev, trunkId);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(numMembers, numMembersGet,
                   "get another numMembers than was set: %d, %d", dev, trunkId);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) membersArray,
                                     (GT_VOID*) membersArrayGet,
                                     numMembers * sizeof(membersArray[0]))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                   "get another membersArray than was set: %d, %d", dev, trunkId);

        /*
            1.3. Call with out of range numMembers [13].
            Expected: NON GT_OK.
        */
        numMembers = CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS + 1;

        st = cpssExMxPmTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, trunkId, numMembers);

        numMembers = 3;

        /*
            1.4. Call with trunkId [NULL_TRUNK_ID_CNS = 0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.5. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        /*
            1.6. Call with trunkId [2],
                           numMembers [3],
                           membersArray [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkTableEntrySet(dev, trunkId, numMembers, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, membersArray = NULL", dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    numMembers = 3;

    membersArray[0].device = dev;
    membersArray[0].port   = 0;
    membersArray[1].device = dev;
    membersArray[1].port   = 1;
    membersArray[2].device = dev;
    membersArray[2].port   = 2;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkTableEntryGet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    OUT GT_U32                  *numMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkTableEntryGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with trunkId [2],
                   not-NULL numMembers,
                   and allocated membersArray.
    Expected: GT_OK.
    1.2. Call with trunk id [NULL_TRUNK_ID_CNS = 0].
    Expected: NON GT_OK.
    1.3. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256].
    Expected: NON GT_OK.
    1.4. Call with numMembersPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_TRUNK_ID             trunkId;
    GT_U32                  numMembers;
    CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunkId [2],
                           not-NULL numMembers,
                           and allocated membersArray.
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.2. Call with trunk id [NULL_TRUNK_ID_CNS = 0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.3. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        /*
            1.4. Call with numMembersPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkTableEntryGet(dev, trunkId, NULL, membersArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, numMembersPtr = NULL", dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low level mode

GT_STATUS cpssExMxPmTrunkNonTrunkPortsEntrySet
(
    IN  GT_U8               devNum,
    IN  GT_TRUNK_ID         trunkId,
    IN  CPSS_PORTS_BMP_STC  *nonTrunkPortsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkNonTrunkPortsEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with trunkId [2],
                   non-NULL nonTrunkPortsPtr [3, 0].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTrunkNonTrunkPortsEntryGet with trunkId [2], non-NULL nonTrunkPortsPtr.
    Expected: GT_OK and the same port bitmap as was written.
    1.3. Call with trunkId [NULL_TRUNK_ID_CNS = 0],
                   non-NULL nonTrunkPortsPtr [3, 0].
    Expected: GT_OK.
    1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                   non-NULL nonTrunkPortsPtr [3, 0].
    Expected: NON GT_OK.
    1.5. Call with trunkId [2],
                   nonTrunkPortsPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_TRUNK_ID         trunkId = 0;
    CPSS_PORTS_BMP_STC  nonTrunkPorts;
    CPSS_PORTS_BMP_STC  nonTrunkPortsGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunkId [2],
                           non-NULL nonTrunkPortsPtr [3, 0].
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        nonTrunkPorts.ports[0] = 3;
        nonTrunkPorts.ports[1] = 0;

        st = cpssExMxPmTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.2. Call cpssExMxPmTrunkNonTrunkPortsEntryGet with trunkId [2], non-NULL nonTrunkPortsPtr.
            Expected: GT_OK and the same port bitmap as was written.
        */
        st = cpssExMxPmTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPortsGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmTrunkNonTrunkPortsEntryGet: %d, %d", dev, trunkId);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(nonTrunkPorts.ports[0], nonTrunkPortsGet.ports[0],
                   "get another nonTrunkPortsPtr->ports[0] than was set: %d, %d", dev, trunkId);
        UTF_VERIFY_EQUAL2_STRING_MAC(nonTrunkPorts.ports[1], nonTrunkPortsGet.ports[1],
                   "get another nonTrunkPortsPtr->ports[1] than was set: %d, %d", dev, trunkId);

        /*
            1.3. Call with trunkId [NULL_TRUNK_ID_CNS = 0],
                           non-NULL nonTrunkPortsPtr [3, 0].
            Expected: GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                           non-NULL nonTrunkPortsPtr [3, 0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        /*
            1.5. Call with trunkId [2],
                           nonTrunkPortsPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkNonTrunkPortsEntrySet(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nonTrunkPortsPtr = NULL", dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkNonTrunkPortsEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_TRUNK_ID         trunkId,
    OUT  CPSS_PORTS_BMP_STC  *nonTrunkPortsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkNonTrunkPortsEntryGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with trunkId [2],
                   non-NULL nonTrunkPortsPtr.
    Expected: GT_OK.
    1.2. Call with trunkId [NULL_TRUNK_ID_CNS = 0],
                   non-NULL nonTrunkPortsPtr.
    Expected: GT_OK.
    1.3. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                   non-NULL nonTrunkPortsPtr.
    Expected: NON GT_OK.
    1.4. Call with trunkId [2],
                   nonTrunkPortsPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_TRUNK_ID         trunkId = 0;
    CPSS_PORTS_BMP_STC  nonTrunkPorts;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunkId [2],
                           non-NULL nonTrunkPortsPtr.
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.2. Call with trunkId [NULL_TRUNK_ID_CNS = 0],
                           non-NULL nonTrunkPortsPtr.
            Expected: GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.3. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256],
                           non-NULL nonTrunkPortsPtr.
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxPmTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        /*
            1.4. Call with trunkId [2],
                           nonTrunkPortsPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkNonTrunkPortsEntryGet(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nonTrunkPortsPtr = NULL", dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmTrunkDesignatedPortsEntrySet
(
    IN  GT_U8               devNum,
    IN  GT_U32              entryIndex,
    IN  CPSS_PORTS_BMP_STC  *designatedPortsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkDesignatedPortsEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIndex [0 / 15],
                   non-NULL designatedPortsPtr [0, 0 / 3, 0].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTrunkDesignatedPortsEntryGet with same entryIndex, non-NULL designatedPortsPtr.
    Expected: GT_OK and the same port bitmap as was written.
    1.3. Call with out of range entryIndex [16],
                   non-NULL designatedPortsPtr [3, 0].
    Expected: NON GT_OK.
    1.4. Call with entryIndex [3],
                   designatedPortsPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32              entryIndex = 0;
    CPSS_PORTS_BMP_STC  designatedPorts;
    CPSS_PORTS_BMP_STC  designatedPortsGet;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryIndex [0 / 15],
                           non-NULL designatedPortsPtr [0, 0 / 3, 0].
            Expected: GT_OK.
        */

        /* Call with entryIndex [0] */
        entryIndex = 0;

        designatedPorts.ports[0] = 0;
        designatedPorts.ports[1] = 0;

        st = cpssExMxPmTrunkDesignatedPortsEntrySet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call cpssExMxPmTrunkDesignatedPortsEntryGet with same entryIndex, non-NULL designatedPortsPtr.
            Expected: GT_OK and the same port bitmap as was written.
        */
        st = cpssExMxPmTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPortsGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmTrunkDesignatedPortsEntryGet: %d, %d", dev, entryIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(designatedPorts.ports[0], designatedPortsGet.ports[0],
                   "get another designatedPortsPtr->ports[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(designatedPorts.ports[1], designatedPortsGet.ports[1],
                   "get another designatedPortsPtr->ports[1] than was set: %d", dev);

        /* Call with entryIndex [15] */
        entryIndex = 15;

        designatedPorts.ports[0] = 3;
        designatedPorts.ports[1] = 0;

        st = cpssExMxPmTrunkDesignatedPortsEntrySet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.2. Call cpssExMxPmTrunkDesignatedPortsEntryGet with same entryIndex, non-NULL designatedPortsPtr.
            Expected: GT_OK and the same port bitmap as was written.
        */
        st = cpssExMxPmTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPortsGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmTrunkDesignatedPortsEntryGet: %d, %d", dev, entryIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(designatedPorts.ports[0], designatedPortsGet.ports[0],
                   "get another designatedPortsPtr->ports[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(designatedPorts.ports[1], designatedPortsGet.ports[1],
                   "get another designatedPortsPtr->ports[1] than was set: %d", dev);

        /*
            1.3. Call with out of range entryIndex [16],
                           non-NULL designatedPortsPtr [3, 0].
            Expected: NON GT_OK.
        */
        entryIndex = 16;

        st = cpssExMxPmTrunkDesignatedPortsEntrySet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        entryIndex = 3;

        /*
            1.4. Call with entryIndex [3],
                           designatedPortsPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkDesignatedPortsEntrySet(dev, entryIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, designatedPortsPtr = NULL", dev);
    }

    entryIndex = 0;

    designatedPorts.ports[0] = 0;
    designatedPorts.ports[1] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkDesignatedPortsEntrySet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkDesignatedPortsEntrySet(dev, entryIndex, &designatedPorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkDesignatedPortsEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_U32              entryIndex,
    OUT  CPSS_PORTS_BMP_STC  *designatedPortsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkDesignatedPortsEntryGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with entryIndex [0 / 15],
                   non-NULL designatedPortsPtr.
    Expected: GT_OK.
    1.2. Call with out of range entryIndex [16],
                   non-NULL designatedPortsPtr.
    Expected: NON GT_OK.
    1.3. Call with entryIndex [3],
                   designatedPortsPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32              entryIndex = 0;
    CPSS_PORTS_BMP_STC  designatedPorts;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with entryIndex [0 / 15],
                           non-NULL designatedPortsPtr.
            Expected: GT_OK.
        */

        /* Call with entryIndex [0] */
        entryIndex = 0;

        st = cpssExMxPmTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /* Call with entryIndex [15] */
        entryIndex = 15;

        st = cpssExMxPmTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /*
            1.3. Call with out of range entryIndex [16],
                           non-NULL designatedPortsPtr.
            Expected: NON GT_OK.
        */
        entryIndex = 16;

        st = cpssExMxPmTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        entryIndex = 3;

        /*
            1.4. Call with entryIndex [3],
                           designatedPortsPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkDesignatedPortsEntryGet(dev, entryIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, designatedPortsPtr = NULL", dev);
    }

    entryIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High level mode

GT_STATUS cpssExMxPmTrunkDbEnabledMembersGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    INOUT GT_U32                *numOfEnabledMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   enabledMembersArray[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkDbEnabledMembersGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with trunkId [2],
                   non-NULL numOfEnabledMembers [12],
                   and allocated enabledMembersArray for 12 members.
    Expected: GT_OK.
    1.2. Call with trunk id [NULL_TRUNK_ID_CNS = 0].
    Expected: NON GT_OK.
    1.3. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256].
    Expected: NON GT_OK.
    1.4. Call with trunkId [2],
                   numOfEnabledMembers [NULL],
                   and allocated enabledMembersArray.
    Expected: GT_BAD_PTR.
    1.5. Call with trunkId [2],
                   enabledMembersArray [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_TRUNK_ID                 trunkId        = 0;
    GT_U32                      numOfEnMembers = 0;
    OUT CPSS_TRUNK_MEMBER_STC   enabledMembersArray[CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunkId [2],
                           non-NULL numOfEnabledMembers [12],
                           and allocated enabledMembersArray for 12 members.
            Expected: GT_OK.
        */
        trunkId        = TRUNK_TESTED_TRUNK_ID_CNS;
        /* must initialize to the max number of members we wish to receive */
        numOfEnMembers = CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS;

        st = cpssExMxPmTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnMembers, enabledMembersArray);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trunkId, numOfEnMembers);

        /*
            1.2. Call with trunk id [NULL_TRUNK_ID_CNS = 0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        /* must initialize to the max number of members we wish to receive */
        numOfEnMembers = CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
        st = cpssExMxPmTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnMembers, enabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.3. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        /* must initialize to the max number of members we wish to receive */
        numOfEnMembers = CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
        st = cpssExMxPmTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnMembers, enabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        /*
            1.5. Call with trunkId [2],
                           numOfEnabledMembers [NULL],
                           and allocated enabledMembersArray.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkDbEnabledMembersGet(dev, trunkId, NULL, enabledMembersArray);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfEnabledMembers = NULL", dev);

        /*
            1.6. Call with trunkId [2],
                           enabledMembersArray [NULL].
            Expected: GT_BAD_PTR.
        */
        numOfEnMembers = 1;

        st = cpssExMxPmTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnMembers, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enabledMembersArray = NULL", dev);
    }

    trunkId        = TRUNK_TESTED_TRUNK_ID_CNS;
    numOfEnMembers = 8;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* must initialize to the max number of members we wish to receive */
        numOfEnMembers = CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
        st = cpssExMxPmTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnMembers, enabledMembersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    /* must initialize to the max number of members we wish to receive */
    numOfEnMembers = CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
    st = cpssExMxPmTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnMembers, enabledMembersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High level mode

GT_STATUS cpssExMxPmTrunkDbDisabledMembersGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    INOUT GT_U32                *numOfDisabledMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   disabledMembersArray[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkDbDisabledMembersGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with trunkId [2],
                   non-NULL numOfDisabledMembersPtr [12],
                   and allocated disabledMembersArray for 12 members.
    Expected: GT_OK.
    1.2. Call with trunk id [NULL_TRUNK_ID_CNS = 0].
    Expected: NON GT_OK.
    1.3. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256].
    Expected: NON GT_OK.
    1.4. Call with trunkId [2],
                   numOfDisabledMembersPtr [NULL],
                   and allocated disabledMembersArray.
    Expected: GT_BAD_PTR.
    1.5. Call with trunkId [2],
                   disabledMembersArray [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_TRUNK_ID                 trunkId         = 0;
    GT_U32                      numOfDisMembers = 0;
    OUT CPSS_TRUNK_MEMBER_STC   disabledMembersArray[CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunkId [2],
                           non-NULL numOfDisabledMembersPtr [12],
                           and allocated disabledMembersArray for 12 members.
            Expected: GT_OK.
        */
        trunkId         = TRUNK_TESTED_TRUNK_ID_CNS;
        /* must initialize to the max number of members we wish to receive */
        numOfDisMembers = CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS;

        st = cpssExMxPmTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trunkId, numOfDisMembers);

        /*
            1.2. Call with trunk id [NULL_TRUNK_ID_CNS = 0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        /* must initialize to the max number of members we wish to receive */
        numOfDisMembers = CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS;

        st = cpssExMxPmTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.3. Call with out of range trunkId [GT_TRUNK_MAX_ID = 256].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        /* must initialize to the max number of members we wish to receive */
        numOfDisMembers = CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS;

        st = cpssExMxPmTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        /*
            1.4. Call with trunkId [2],
                           numOfDisabledMembersPtr [NULL],
                           and allocated disabledMembersArray.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkDbDisabledMembersGet(dev, trunkId, NULL, disabledMembersArray);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfDisabledMembersPtr = NULL", dev);

        /*
            1.5. Call with trunkId [2],
                           disabledMembersArray [NULL].
            Expected: GT_BAD_PTR.
        */
        numOfDisMembers = 1;

        st = cpssExMxPmTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisMembers, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, disabledMembersArray = NULL", dev);
    }

    trunkId         = TRUNK_TESTED_TRUNK_ID_CNS;
    numOfDisMembers = 8;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* must initialize to the max number of members we wish to receive */
        numOfDisMembers = CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
        st = cpssExMxPmTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    /* must initialize to the max number of members we wish to receive */
    numOfDisMembers = CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
    st = cpssExMxPmTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisMembers, disabledMembersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High level mode

GT_STATUS cpssExMxPmTrunkDbIsMemberOfTrunk
(
    IN  GT_U8                   devNum,
    IN  CPSS_TRUNK_MEMBER_STC   *memberPtr,
    OUT GT_TRUNK_ID             *trunkIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkDbIsMemberOfTrunk)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmTrunkMemberAdd with trunkId [2],
                                            and non-NULL memberPtr {port [0], device [devNum]}.
    Expected: GT_OK.
    1.2. Call with memberPtr {port [0], device [devNum]},
                   non-NULL trunkIdPtr.
    Expected: GT_OK and trunkIdPtr [2].
    1.3. Call with non-NULL memberPtr {port [0], device [devNum]},
                   trunkIdPtr [NULL].
    Expected: GT_OK.
    1.4. Call cpssExMxPmTrunkMemberRemove with trunkId [2],
                                               non-NULL memberPtr {port [0], device [devNum]}.
    Expected: GT_OK.
    1.5. Call with non-NULL memberPtr {port [0], device [devNum]} (removed member),
                   non-NULL trunkIdPtr.
    Expected: NOT GT_OK.
    1.6. Call with memberPtr [NULL],
                   non-NULL trunkIdPtr.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_TRUNK_MEMBER_STC   member;
    GT_TRUNK_ID             trunkId    = 0;
    GT_TRUNK_ID             trunkIdGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxPmTrunkMemberAdd with trunkId [2],
                                                    and non-NULL memberPtr {port [0], device [devNum]}.
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        member.device = dev;
        member.port   = TRUNK_VALID_VIRT_PORT_CNS;

        st = cpssExMxPmTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmTrunkMemberAdd: %d, %d", dev, trunkId);

        /*
            1.2. Call with memberPtr {port [0], device [devNum]},
                           non-NULL trunkIdPtr.
            Expected: GT_OK and trunkIdPtr [2].
        */
        st = cpssExMxPmTrunkDbIsMemberOfTrunk(dev, &member, &trunkIdGet);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, member.device, member.port);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(trunkId, trunkIdGet,
                   "get another trunkId than was set: %d", dev);

        /*
            1.3. Call with non-NULL memberPtr {port [0], device [devNum]},
                           trunkIdPtr [NULL].
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkDbIsMemberOfTrunk(dev, &member, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, trunkIdPtr = NULL", dev);

        /*
            1.4. Call cpssExMxPmTrunkMemberRemove with trunkId [2],
                                                       non-NULL memberPtr {port [0], device [devNum]}.
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmTrunkMemberRemove: %d, %d", dev, trunkId);

        /*
            1.5. Call with non-NULL memberPtr {port [0], device [devNum]} (removed member),
                           non-NULL trunkIdPtr.
            Expected: NOT GT_OK.
        */
        st = cpssExMxPmTrunkDbIsMemberOfTrunk(dev, &member, &trunkIdGet);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, member.device, member.port);

        /*
            1.6. Call with memberPtr [NULL],
                           non-NULL trunkIdPtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkDbIsMemberOfTrunk(dev, NULL, &trunkIdGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, memberPtr = NULL", dev);
    }

    member.device = 0;
    member.port   = TRUNK_VALID_VIRT_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkDbIsMemberOfTrunk(dev, &member, &trunkIdGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkDbIsMemberOfTrunk(dev, &member, &trunkIdGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashDesignatedTableModeSet
(
    IN GT_U8   devNum,
    IN CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_ENT mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashDesignatedTableModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with mode [CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_NO_LB_E /
                         CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_VIDX_VID_SRC_INTERFACE_E /
                         CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_VIDX_VID_SRC_INTERFACE_FLOWID_DITPTR_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTrunkHashDesignatedTableModeGet with non-NULL modePtr.
    Expected: GT_OK and same value as written.
    1.3. Call with out of range mode [0x5AAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_ENT mode    = CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_NO_LB_E;
    CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_ENT modeGet = CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_NO_LB_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_NO_LB_E /
                                 CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_VIDX_VID_SRC_INTERFACE_E /
                                 CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_VIDX_VID_SRC_INTERFACE_FLOWID_DITPTR_E].
            Expected: GT_OK.
        */

        /* Call with mode [CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_NO_LB_E] */
        mode = CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_NO_LB_E;

        st = cpssExMxPmTrunkHashDesignatedTableModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmTrunkHashDesignatedTableModeGet with non-NULL modePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashDesignatedTableModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashDesignatedTableModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_VIDX_VID_SRC_INTERFACE_E] */
        mode = CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_VIDX_VID_SRC_INTERFACE_E;

        st = cpssExMxPmTrunkHashDesignatedTableModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmTrunkHashDesignatedTableModeGet with non-NULL modePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashDesignatedTableModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashDesignatedTableModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /* Call with mode [CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_VIDX_VID_SRC_INTERFACE_FLOWID_DITPTR_E] */
        mode = CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_VIDX_VID_SRC_INTERFACE_FLOWID_DITPTR_E;

        st = cpssExMxPmTrunkHashDesignatedTableModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssExMxPmTrunkHashDesignatedTableModeGet with non-NULL modePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashDesignatedTableModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashDesignatedTableModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d", dev);

        /*
            1.3. Call with out of range mode [0x5AAAAA5].
            Expected: GT_BAD_PARAM.
        */
        mode = TRUNK_INVALID_ENUM_CNS;

        st = cpssExMxPmTrunkHashDesignatedTableModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_NO_LB_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashDesignatedTableModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashDesignatedTableModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashDesignatedTableModeGet
(
    IN GT_U8   devNum,
    OUT CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_ENT *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashDesignatedTableModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL modePtr.
    Expected: GT_OK.
    1.2. Call with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_ENT   mode = CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_NO_LB_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL modePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkHashDesignatedTableModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkHashDesignatedTableModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashDesignatedTableModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashDesignatedTableModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashGlobalModeSet
(
    IN GT_U8    devNum,
    IN CPSS_EXMXPM_TRUNK_LBH_GLOBAL_MODE_ENT  hashMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashGlobalModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E /
                             CPSS_EXMXPM_TRUNK_LBH_INGRESS_PORT_E /
                             CPSS_EXMXPM_TRUNK_LBH_PERFECT_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTrunkHashGlobalModeGet with non-NULL hashModePtr.
    Expected: GT_OK and same value as written.
    1.3. Call with out of range hashMode [0x5AAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TRUNK_LBH_GLOBAL_MODE_ENT  hashMode    = CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E;
    CPSS_EXMXPM_TRUNK_LBH_GLOBAL_MODE_ENT  hashModeGet = CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E /
                                     CPSS_EXMXPM_TRUNK_LBH_INGRESS_PORT_E /
                                     CPSS_EXMXPM_TRUNK_LBH_PERFECT_E].
            Expected: GT_OK.
        */

        /* Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E] */
        hashMode = CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E;

        st = cpssExMxPmTrunkHashGlobalModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

        /*
            1.2. Call cpssExMxPmTrunkHashGlobalModeGet with non-NULL hashModePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashGlobalModeGet(dev, &hashModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashGlobalModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hashMode, hashModeGet,
                       "get another hashMode than was set: %d", dev);

        /* Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_INGRESS_PORT_E] */
        hashMode = CPSS_EXMXPM_TRUNK_LBH_INGRESS_PORT_E;

        st = cpssExMxPmTrunkHashGlobalModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

        /*
            1.2. Call cpssExMxPmTrunkHashGlobalModeGet with non-NULL hashModePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashGlobalModeGet(dev, &hashModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashGlobalModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hashMode, hashModeGet,
                       "get another hashMode than was set: %d", dev);

        /* Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_PERFECT_E] */
        hashMode = CPSS_EXMXPM_TRUNK_LBH_PERFECT_E;

        st = cpssExMxPmTrunkHashGlobalModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

        /*
            1.2. Call cpssExMxPmTrunkHashGlobalModeGet with non-NULL hashModePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashGlobalModeGet(dev, &hashModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashGlobalModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hashMode, hashModeGet,
                       "get another hashMode than was set: %d", dev);

        /*
            1.3. Call with out of range hashMode [0x5AAAAA5].
            Expected: GT_BAD_PARAM.
        */
        hashMode = TRUNK_INVALID_ENUM_CNS;

        st = cpssExMxPmTrunkHashGlobalModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, hashMode);
    }

    hashMode = CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashGlobalModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashGlobalModeSet(dev, hashMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashGlobalModeGet
(
    IN GT_U8    devNum,
    OUT CPSS_EXMXPM_TRUNK_LBH_GLOBAL_MODE_ENT  *hashModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashGlobalModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL hashModePtr.
    Expected: GT_OK.
    1.2. Call with hashModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TRUNK_LBH_GLOBAL_MODE_ENT   hashMode = CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL hashModePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkHashGlobalModeGet(dev, &hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null hashModePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkHashDesignatedTableModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, hashModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashGlobalModeGet(dev, &hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashGlobalModeGet(dev, &hashMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashAddMacModeEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashAddMacModeEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTrunkHashAddMacModeEnableGet with non-NULL enablePtr.
    Expected: GT_OK and same value as written.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssExMxPmTrunkHashAddMacModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmTrunkHashAddMacModeEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmTrunkHashAddMacModeEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashAddMacModeEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxPmTrunkHashAddMacModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmTrunkHashAddMacModeEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmTrunkHashAddMacModeEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashAddMacModeEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashAddMacModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashAddMacModeEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashAddMacModeEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashAddMacModeEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkHashAddMacModeEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkHashAddMacModeEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashAddMacModeEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashAddMacModeEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashIpModeEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashIpModeEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTrunkHashIpModeEnableGet with non-NULL enablePtr.
    Expected: GT_OK and same value as written.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssExMxPmTrunkHashIpModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmTrunkHashIpModeEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmTrunkHashIpModeEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashIpModeEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxPmTrunkHashIpModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmTrunkHashIpModeEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmTrunkHashIpModeEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashIpModeEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashIpModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashIpModeEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashIpModeEnableGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashIpModeEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkHashIpModeEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkHashIpModeEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashIpModeEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashIpModeEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashL4ModeSet
(
    IN GT_U8   devNum,
    IN CPSS_EXMXPM_TRUNK_LBH_L4_MODE_ENT hashMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashL4ModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_L4_DISABLED_E /
                             CPSS_EXMXPM_TRUNK_LBH_L4_LONG_E /
                             CPSS_EXMXPM_TRUNK_LBH_L4_SHORT_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTrunkHashL4ModeGet with non-NULL hashModePtr.
    Expected: GT_OK and same value as written.
    1.3. Call with out of range hashMode [0x5AAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TRUNK_LBH_L4_MODE_ENT  hashMode    = CPSS_EXMXPM_TRUNK_LBH_L4_DISABLED_E;
    CPSS_EXMXPM_TRUNK_LBH_L4_MODE_ENT  hashModeGet = CPSS_EXMXPM_TRUNK_LBH_L4_DISABLED_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_L4_DISABLED_E /
                                     CPSS_EXMXPM_TRUNK_LBH_L4_LONG_E /
                                     CPSS_EXMXPM_TRUNK_LBH_L4_SHORT_E].
            Expected: GT_OK.
        */

        /* Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_L4_DISABLED_E] */
        hashMode = CPSS_EXMXPM_TRUNK_LBH_L4_DISABLED_E;

        st = cpssExMxPmTrunkHashL4ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

        /*
            1.2. Call cpssExMxPmTrunkHashL4ModeGet with non-NULL hashModePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashL4ModeGet(dev, &hashModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashL4ModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hashMode, hashModeGet,
                       "get another hashMode than was set: %d", dev);

        /* Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_L4_LONG_E] */
        hashMode = CPSS_EXMXPM_TRUNK_LBH_L4_LONG_E;

        st = cpssExMxPmTrunkHashL4ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

        /*
            1.2. Call cpssExMxPmTrunkHashL4ModeGet with non-NULL hashModePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashL4ModeGet(dev, &hashModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashL4ModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hashMode, hashModeGet,
                       "get another hashMode than was set: %d", dev);

        /* Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_L4_SHORT_E] */
        hashMode = CPSS_EXMXPM_TRUNK_LBH_L4_SHORT_E;

        st = cpssExMxPmTrunkHashL4ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

        /*
            1.2. Call cpssExMxPmTrunkHashL4ModeGet with non-NULL hashModePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashL4ModeGet(dev, &hashModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashL4ModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hashMode, hashModeGet,
                       "get another hashMode than was set: %d", dev);

        /*
            1.3. Call with out of range hashMode [0x5AAAAA5].
            Expected: GT_BAD_PARAM.
        */
        hashMode = TRUNK_INVALID_ENUM_CNS;

        st = cpssExMxPmTrunkHashL4ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, hashMode);
    }

    hashMode = CPSS_EXMXPM_TRUNK_LBH_L4_DISABLED_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashL4ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashL4ModeSet(dev, hashMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashL4ModeGet
(
    IN GT_U8   devNum,
    OUT CPSS_EXMXPM_TRUNK_LBH_L4_MODE_ENT *hashModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashL4ModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL hashModePtr.
    Expected: GT_OK.
    1.2. Call with hashModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TRUNK_LBH_L4_MODE_ENT   hashMode = CPSS_EXMXPM_TRUNK_LBH_L4_DISABLED_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL hashModePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkHashL4ModeGet(dev, &hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null hashModePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkHashL4ModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, hashModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashL4ModeGet(dev, &hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashL4ModeGet(dev, &hashMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashIpv6ModeSet
(
    IN GT_U8                                devNum,
    IN CPSS_EXMXPM_TRUNK_LBH_IPV6_MODE_ENT  hashMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashIpv6ModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_IPV6_LSB_SIP_DIP_FLOW_E /
                             CPSS_EXMXPM_TRUNK_LBH_IPV6_MSB_SIP_DIP_FLOW_E /
                             CPSS_EXMXPM_TRUNK_LBH_IPV6_MSB_LSB_SIP_DIP_FLOW_E /
                             CPSS_EXMXPM_TRUNK_LBH_IPV6_LSB_SIP_DIP_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTrunkHashIpv6ModeGet with non-NULL hashModePtr.
    Expected: GT_OK and same value as written.
    1.3. Call with out of range hashMode [0x5AAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TRUNK_LBH_IPV6_MODE_ENT  hashMode    = CPSS_EXMXPM_TRUNK_LBH_IPV6_LSB_SIP_DIP_FLOW_E;
    CPSS_EXMXPM_TRUNK_LBH_IPV6_MODE_ENT  hashModeGet = CPSS_EXMXPM_TRUNK_LBH_IPV6_LSB_SIP_DIP_FLOW_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_IPV6_LSB_SIP_DIP_FLOW_E /
                                     CPSS_EXMXPM_TRUNK_LBH_IPV6_MSB_SIP_DIP_FLOW_E /
                                     CPSS_EXMXPM_TRUNK_LBH_IPV6_MSB_LSB_SIP_DIP_FLOW_E /
                                     CPSS_EXMXPM_TRUNK_LBH_IPV6_LSB_SIP_DIP_E].
            Expected: GT_OK.
        */

        /* Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_IPV6_LSB_SIP_DIP_FLOW_E] */
        hashMode = CPSS_EXMXPM_TRUNK_LBH_IPV6_LSB_SIP_DIP_FLOW_E;

        st = cpssExMxPmTrunkHashIpv6ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

        /*
            1.2. Call cpssExMxPmTrunkHashIpv6ModeGet with non-NULL hashModePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashIpv6ModeGet(dev, &hashModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashIpv6ModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hashMode, hashModeGet,
                       "get another hashMode than was set: %d", dev);

        /* Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_IPV6_MSB_SIP_DIP_FLOW_E] */
        hashMode = CPSS_EXMXPM_TRUNK_LBH_IPV6_MSB_SIP_DIP_FLOW_E;

        st = cpssExMxPmTrunkHashIpv6ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

        /*
            1.2. Call cpssExMxPmTrunkHashIpv6ModeGet with non-NULL hashModePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashIpv6ModeGet(dev, &hashModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashIpv6ModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hashMode, hashModeGet,
                       "get another hashMode than was set: %d", dev);

        /* Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_IPV6_MSB_LSB_SIP_DIP_FLOW_E] */
        hashMode = CPSS_EXMXPM_TRUNK_LBH_IPV6_MSB_LSB_SIP_DIP_FLOW_E;

        st = cpssExMxPmTrunkHashIpv6ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

        /*
            1.2. Call cpssExMxPmTrunkHashIpv6ModeGet with non-NULL hashModePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashIpv6ModeGet(dev, &hashModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashIpv6ModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hashMode, hashModeGet,
                       "get another hashMode than was set: %d", dev);

        /* Call with hashMode [CPSS_EXMXPM_TRUNK_LBH_IPV6_LSB_SIP_DIP_E] */
        hashMode = CPSS_EXMXPM_TRUNK_LBH_IPV6_LSB_SIP_DIP_E;

        st = cpssExMxPmTrunkHashIpv6ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

        /*
            1.2. Call cpssExMxPmTrunkHashIpv6ModeGet with non-NULL hashModePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashIpv6ModeGet(dev, &hashModeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashIpv6ModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hashMode, hashModeGet,
                       "get another hashMode than was set: %d", dev);

        /*
            1.3. Call with out of range hashMode [0x5AAAAA5].
            Expected: GT_BAD_PARAM.
        */
        hashMode = TRUNK_INVALID_ENUM_CNS;

        st = cpssExMxPmTrunkHashIpv6ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, hashMode);
    }

    hashMode = CPSS_EXMXPM_TRUNK_LBH_IPV6_LSB_SIP_DIP_FLOW_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashIpv6ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashIpv6ModeSet(dev, hashMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashIpv6ModeGet
(
    IN GT_U8                                devNum,
    OUT CPSS_EXMXPM_TRUNK_LBH_IPV6_MODE_ENT  *hashModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashIpv6ModeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL hashModePtr.
    Expected: GT_OK.
    1.2. Call with hashModePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TRUNK_LBH_IPV6_MODE_ENT   hashMode = CPSS_EXMXPM_TRUNK_LBH_IPV6_LSB_SIP_DIP_FLOW_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL hashModePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkHashIpv6ModeGet(dev, &hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null hashModePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkHashIpv6ModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, hashModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashIpv6ModeGet(dev, &hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashIpv6ModeGet(dev, &hashMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashMplsModeEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashMplsModeEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTrunkHashMplsModeEnableGet with non-NULL enablePtr.
    Expected: GT_OK and same value as written.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssExMxPmTrunkHashMplsModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmTrunkHashMplsModeEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmTrunkHashMplsModeEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashMplsModeEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxPmTrunkHashMplsModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmTrunkHashMplsModeEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmTrunkHashMplsModeEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashMplsModeEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashMplsModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashMplsModeEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashMplsModeEnableGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashMplsModeEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmTrunkHashMplsModeEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkHashMplsModeEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashMplsModeEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashMplsModeEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashIpShiftSet
(
    IN GT_U8    devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_BOOL  isSrcIp,
    IN GT_U32   shiftValue
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashIpShiftSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E],
                   isSrcIp [GT_FALSE / GT_TRUE]
                   and shiftValue [3 / 15].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTrunkHashIpShiftGet with the same protocolStack, isSrcIp and non-NULL shiftValuePtr.
    Expected: GT_OK and same value as written.
    1.3. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range protocolStack [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E],
                   out of range shiftValue [4]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.6. Call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E],
                   out of range shiftValue [16]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_IP_PROTOCOL_STACK_ENT  protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                     isSrcIp       = GT_FALSE;
    GT_U32                      shiftValue    = 0;
    GT_U32                      shiftValueGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E],
                           isSrcIp [GT_FALSE / GT_TRUE]
                           and shiftValue [3 / 15].
            Expected: GT_OK.
        */

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        isSrcIp       = GT_FALSE;
        shiftValue    = 3;

        st = cpssExMxPmTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, protocolStack, isSrcIp, shiftValue);

        /*
            1.2. Call cpssExMxPmTrunkHashIpShiftGet with the same protocolStack, isSrcIp and non-NULL shiftValuePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValueGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashIpShiftGet: %d, %d, %d", dev, protocolStack, isSrcIp);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(shiftValue, shiftValueGet,
                       "get another shiftValue than was set: %d", dev);

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        isSrcIp       = GT_TRUE;
        shiftValue    = 15;

        st = cpssExMxPmTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, protocolStack, isSrcIp, shiftValue);

        /*
            1.2. Call cpssExMxPmTrunkHashIpShiftGet with the same protocolStack, isSrcIp and non-NULL shiftValuePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValueGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashIpShiftGet: %d, %d, %d", dev, protocolStack, isSrcIp);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(shiftValue, shiftValueGet,
                       "get another shiftValue than was set: %d", dev);

        /*
            1.3. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxPmTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocolStack);

        /*
            1.4. Call with out of range protocolStack [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolStack = TRUNK_INVALID_ENUM_CNS;

        st = cpssExMxPmTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, protocolStack);

        /*
            1.5. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E],
                           out of range shiftValue [4]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        shiftValue    = 4;

        st = cpssExMxPmTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, shiftValue = %d", dev, protocolStack, shiftValue);

        /*
            1.6. Call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E],
                           out of range shiftValue [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        shiftValue    = 16;

        st = cpssExMxPmTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, shiftValue = %d", dev, protocolStack, shiftValue);
    }

    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    isSrcIp       = GT_FALSE;
    shiftValue    = 3;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashIpShiftGet
(
    IN GT_U8    devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_BOOL  isSrcIp,
    OUT GT_U32   *shiftValuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashIpShiftGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                  CPSS_IP_PROTOCOL_IPV6_E],
                   isSrcIp [GT_FALSE / GT_TRUE]
                   and non-NULL shiftValuePtr.
    Expected: GT_OK.
    1.2. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with out of range protocolStack [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with shiftValuePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_IP_PROTOCOL_STACK_ENT  protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                     isSrcIp       = GT_FALSE;
    GT_U32                      shiftValue    = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                          CPSS_IP_PROTOCOL_IPV6_E],
                           isSrcIp [GT_FALSE / GT_TRUE]
                           and non-NULL shiftValuePtr.
            Expected: GT_OK.
        */

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        isSrcIp       = GT_FALSE;

        st = cpssExMxPmTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, isSrcIp);

        /* Call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E] */
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        isSrcIp       = GT_TRUE;
        shiftValue    = 15;

        st = cpssExMxPmTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, protocolStack, isSrcIp);

        /*
            1.2. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E] (not supported)
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;

        st = cpssExMxPmTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValue);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, protocolStack);

        /*
            1.3. Call with out of range protocolStack [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        protocolStack = TRUNK_INVALID_ENUM_CNS;

        st = cpssExMxPmTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, protocolStack);

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.4. Call with shiftValuePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, shiftValuePtr = NULL", dev);
    }

    protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    isSrcIp       = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashMaskSet
(
    IN GT_U8    devNum,
    IN CPSS_EXMXPM_TRUNK_LBH_MASK_ENT   maskedField,
    IN GT_U8    maskValue
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashMaskSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with maskedField [CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E /
                                CPSS_EXMXPM_TRUNK_LBH_MASK_IPV4_SIP_E  /
                                CPSS_EXMXPM_TRUNK_LBH_MASK_L4_DST_PORT_E]
                   and maskValue[0 / 10 / 0x3F].
    Expected: GT_OK.
    1.2. Call cpssExMxPmTrunkHashMaskGet with the same maskedField and non-NULL maskValuePtr.
    Expected: GT_OK and same value as written.
    1.3. Call with out of range maskedField [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call with out of range maskValue [64]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TRUNK_LBH_MASK_ENT  maskedField  = CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E;
    GT_U8                           maskValue    = 0;
    GT_U8                           maskValueGet = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with maskedField [CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E /
                                        CPSS_EXMXPM_TRUNK_LBH_MASK_IPV4_SIP_E  /
                                        CPSS_EXMXPM_TRUNK_LBH_MASK_L4_DST_PORT_E]
                           and maskValue[0 / 10 / 0x3F].
            Expected: GT_OK.
        */

        /* Call with maskedField [CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E] */
        maskedField = CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E;
        maskValue   = 0;

        st = cpssExMxPmTrunkHashMaskSet(dev, maskedField, maskValue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, maskedField, maskValue);

        /*
            1.2. Call cpssExMxPmTrunkHashMaskGet with the same maskedField and non-NULL maskValuePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashMaskGet(dev, maskedField, &maskValueGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashMaskGet: %d, %d", dev, maskedField);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maskValue, maskValueGet,
                       "get another maskValue than was set: %d", dev);

        /* Call with maskedField [CPSS_EXMXPM_TRUNK_LBH_MASK_IPV4_SIP_E] */
        maskedField = CPSS_EXMXPM_TRUNK_LBH_MASK_IPV4_SIP_E;
        maskValue   = 10;

        st = cpssExMxPmTrunkHashMaskSet(dev, maskedField, maskValue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, maskedField, maskValue);

        /*
            1.2. Call cpssExMxPmTrunkHashMaskGet with the same maskedField and non-NULL maskValuePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashMaskGet(dev, maskedField, &maskValueGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashMaskGet: %d, %d", dev, maskedField);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maskValue, maskValueGet,
                       "get another maskValue than was set: %d", dev);

        /* Call with maskedField [CPSS_EXMXPM_TRUNK_LBH_MASK_L4_DST_PORT_E] */
        maskedField = CPSS_EXMXPM_TRUNK_LBH_MASK_L4_DST_PORT_E;
        maskValue   = 0x3F;

        st = cpssExMxPmTrunkHashMaskSet(dev, maskedField, maskValue);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, maskedField, maskValue);

        /*
            1.2. Call cpssExMxPmTrunkHashMaskGet with the same maskedField and non-NULL maskValuePtr.
            Expected: GT_OK and same value as written.
        */
        st = cpssExMxPmTrunkHashMaskGet(dev, maskedField, &maskValueGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmTrunkHashMaskGet: %d, %d", dev, maskedField);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maskValue, maskValueGet,
                       "get another maskValue than was set: %d", dev);

        /*
            1.3. Call with out of range maskedField [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        maskedField = TRUNK_INVALID_ENUM_CNS;

        st = cpssExMxPmTrunkHashMaskSet(dev, maskedField, maskValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, maskedField);

        maskedField = CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E;

        /*
            1.4. Call with out of range maskValue [64]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        maskValue = 64;

        st = cpssExMxPmTrunkHashMaskSet(dev, maskedField, maskValue);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, maskValue = %d", dev, maskValue);
    }

    maskedField = CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E;
    maskValue   = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashMaskSet(dev, maskedField, maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashMaskSet(dev, maskedField, maskValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All modes

GT_STATUS cpssExMxPmTrunkHashMaskGet
(
    IN GT_U8    devNum,
    IN CPSS_EXMXPM_TRUNK_LBH_MASK_ENT   maskedField,
    OUT GT_U8    *maskValuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkHashMaskGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with maskedField [CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E /
                                CPSS_EXMXPM_TRUNK_LBH_MASK_IPV4_SIP_E /
                                CPSS_EXMXPM_TRUNK_LBH_MASK_L4_DST_PORT_E]
                   and non-NULL maskValuePtr.
    Expected: GT_OK.
    1.2. Call with out of range maskedField [0x5AAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call with maskValuePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_EXMXPM_TRUNK_LBH_MASK_ENT  maskedField = CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E;
    GT_U8                           maskValue   = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with maskedField [CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E /
                                        CPSS_EXMXPM_TRUNK_LBH_MASK_IPV4_SIP_E /
                                        CPSS_EXMXPM_TRUNK_LBH_MASK_L4_DST_PORT_E]
                           and non-NULL maskValuePtr.
            Expected: GT_OK.
        */

        /* Call with maskedField [CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E] */
        maskedField = CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E;

        st = cpssExMxPmTrunkHashMaskGet(dev, maskedField, &maskValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, maskedField);

        /* Call with maskedField [CPSS_EXMXPM_TRUNK_LBH_MASK_IPV4_SIP_E] */
        maskedField = CPSS_EXMXPM_TRUNK_LBH_MASK_IPV4_SIP_E;

        st = cpssExMxPmTrunkHashMaskGet(dev, maskedField, &maskValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, maskedField);

        /* Call with maskedField [CPSS_EXMXPM_TRUNK_LBH_MASK_L4_DST_PORT_E] */
        maskedField = CPSS_EXMXPM_TRUNK_LBH_MASK_L4_DST_PORT_E;

        st = cpssExMxPmTrunkHashMaskGet(dev, maskedField, &maskValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, maskedField);

        /*
            1.2. Call with out of range maskedField [0x5AAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        maskedField = TRUNK_INVALID_ENUM_CNS;

        st = cpssExMxPmTrunkHashMaskGet(dev, maskedField, &maskValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, maskedField);

        maskedField = CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E;

        /*
            1.3. Call with maskValuePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmTrunkHashMaskGet(dev, maskedField, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskValuePtr = NULL", dev);
    }

    maskedField = CPSS_EXMXPM_TRUNK_LBH_MASK_MAC_DA_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmTrunkHashMaskGet(dev, maskedField, &maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmTrunkHashMaskGet(dev, maskedField, &maskValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Trunk table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkFillTrunkTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in Trunk table.
         Call cpssExMxPmTrunkTableEntrySet with trunkId [0..numEntries - 1],
                           numMembers [3],
                           membersArray [0 / 1 / 2] {port [0 / 1 / 2], device [devNum]}.
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmTrunkTableEntrySet with trunkId [numEntries],
    Expected: NOT GT_OK.
    1.4. Read all entries in Trunk table and compare with original.
         Call with cpssExMxPmTrunkTableEntryGet with trunkId form 1.2. , non-NULL numMembersPtr, non-NULL membersArray.
    Expected: GT_OK, numMembersPtr [3], and the same membersArray as was set.
    1.5. Try to read entry with index out of range.
         Call with cpssExMxPmTrunkTableEntryGet with trunkId [numEntries].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual    = GT_FALSE;
    GT_TRUNK_ID numEntries = 0;
    GT_TRUNK_ID iTemp      = 0;

    GT_U32                  numMembers    = 0;
    CPSS_TRUNK_MEMBER_STC   membersArray   [CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U32                  numMembersGet = 0;
    CPSS_TRUNK_MEMBER_STC   membersArrayGet[CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS];


    /* Fill the entry for Trunk table */
    numMembers = 3;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        membersArray[0].device = dev;
        membersArray[0].port   = 0;
        membersArray[1].device = dev;
        membersArray[1].port   = 1;
        membersArray[2].device = dev;
        membersArray[2].port   = 2;

        /* 1.1. Get table Size */
        numEntries = (GT_TRUNK_ID)(PRV_CPSS_DEV_TRUNK_INFO_MAC(dev)->numTrunksSupportedHw + 1);

        /* 1.2. Fill all entries in Trunk table */
        for(iTemp = 1; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmTrunkTableEntrySet(dev, iTemp, numMembers, membersArray);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkTableEntrySet: %d, %d, %d", dev, iTemp, numMembers);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmTrunkTableEntrySet(dev, numEntries, numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkTableEntrySet: %d, %d, %d", dev, iTemp, numMembers);

        /* 1.4. Read all entries in Trunk table and compare with original */
        for(iTemp = 1; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmTrunkTableEntryGet(dev, iTemp, &numMembersGet, membersArrayGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkTableEntryGet: %d, %d", dev, iTemp);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(numMembers, numMembersGet,
                       "get another numMembers than was set: %d, %d", dev, iTemp);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) membersArray,
                                         (GT_VOID*) membersArrayGet,
                                         numMembers * sizeof(membersArray[0]))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                       "get another membersArray than was set: %d, %d", dev, iTemp);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmTrunkTableEntryGet(dev, numEntries, &numMembersGet, membersArrayGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkTableEntryGet: %d, %d", dev, iTemp);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill designated trunk table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmTrunkFillDesignatedTrunkTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in designated trunk table.
            Call cpssExMxPmTrunkDesignatedPortsEntrySet with entryIndex [0..numEntries - 1],
                           non-NULL designatedPortsPtr [0, 0].
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmTrunkDesignatedPortsEntrySet with entryIndex [numEntries],
    Expected: NOT GT_OK.
    1.4. Read all entries in designated trunk table and compare with original.
         Call cpssExMxPmTrunkDesignatedPortsEntryGet with same entryIndex, non-NULL designatedPortsPtr.
    Expected: GT_OK and the same port bitmap as was written.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmTrunkDesignatedPortsEntryGet with entryIndex [numEntries].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_PORTS_BMP_STC  designatedPorts;
    CPSS_PORTS_BMP_STC  designatedPortsGet;


    /* Fill the entry for designated trunk table */
    designatedPorts.ports[0] = 0;
    designatedPorts.ports[1] = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = PRV_CPSS_DEV_TRUNK_INFO_MAC(dev)->numOfDesignatedTrunkEntriesHw;

        /* 1.2. Fill all entries in designated trunk table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmTrunkDesignatedPortsEntrySet(dev, iTemp, &designatedPorts);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkDesignatedPortsEntrySet: %d, %d", dev, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmTrunkDesignatedPortsEntrySet(dev, numEntries, &designatedPorts);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkDesignatedPortsEntrySet: %d, %d", dev, iTemp);

        /* 1.4. Read all entries in designated trunk table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmTrunkDesignatedPortsEntryGet(dev, iTemp, &designatedPortsGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkDesignatedPortsEntryGet: %d, %d", dev, iTemp);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(designatedPorts.ports[0], designatedPortsGet.ports[0],
                       "get another designatedPortsPtr->ports[0] than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(designatedPorts.ports[1], designatedPortsGet.ports[1],
                       "get another designatedPortsPtr->ports[1] than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmTrunkDesignatedPortsEntryGet(dev, numEntries, &designatedPortsGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmTrunkDesignatedPortsEntryGet: %d, %d", dev, iTemp);
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmTrunk high-level suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmTrunkHighLevel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkMembersSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkMemberAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkMemberRemove)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkMemberDisable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkMemberEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkPortTrunkIdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkTableEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkNonTrunkPortsEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkDesignatedPortsEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkDbEnabledMembersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkDbDisabledMembersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkDbIsMemberOfTrunk)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashDesignatedTableModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashDesignatedTableModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashGlobalModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashGlobalModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashAddMacModeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashAddMacModeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashIpModeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashIpModeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashL4ModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashL4ModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashIpv6ModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashIpv6ModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashMplsModeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashMplsModeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashIpShiftSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashIpShiftGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashMaskSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashMaskGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmTrunkHighLevel)

/*
 * Configuration of cpssExMxPmTrunk low-level suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmTrunkLowLevel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkNonTrunkPortsAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkNonTrunkPortsRemove)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkPortTrunkIdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkPortTrunkIdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkTableEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkTableEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkNonTrunkPortsEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkNonTrunkPortsEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkDesignatedPortsEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkDesignatedPortsEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashDesignatedTableModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashDesignatedTableModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashGlobalModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashGlobalModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashAddMacModeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashAddMacModeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashIpModeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashIpModeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashL4ModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashL4ModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashIpv6ModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashIpv6ModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashMplsModeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashMplsModeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashIpShiftSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashIpShiftGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashMaskSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkHashMaskGet)
    /* Test for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkFillTrunkTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmTrunkFillDesignatedTrunkTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmTrunkLowLevel)

