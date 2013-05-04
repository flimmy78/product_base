/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTrunkUT.c
*
* DESCRIPTION:
*       Unit Tests for Trunk module.
*
*       Trunk module has two types of APIs: High-Level and Low-Level.
*       It's not recommended to mix both APIs so this test plan describes
*       two test suits. First suit is cpssDxChTrunkHighLevel which includes
*       all functions for High-Level mode and second suit is
*       cpssDxChTrunkLowLevel which includes all functions for Low-Level mode
*       and functions compliance with both modes.
*
*       For the cpssDxChTrunk HighLevel and LowLevel suits
*       the following assumptions are done:
*           1.  There is shadow DB for trunk's tables. Number of trunks is greater than zero.
*           2.  Trunk ids 2, 3 are in range for trunk ids.
*           3.  Virtual ports from 0 to 8 are valid for all tested device ids.
*           4.  Test cases should think about cleaning trunk's tables after testing.
*           5.  Test suits will be run separately after reset.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/* Maximum value for trun id    */
#define TRUNK_MAX_TRUNK_ID_CNS          128

/* Trunk database starts from index 1. This value is used as null index.    */
#define TRUNK_NULL_TRUNK_ID_CNS         0

/* Tested trunk id. All tests uses this trunk id for testing. It means that     */
/* test must clear this trunk id after testing.is completed.                    */
#define TRUNK_TESTED_TRUNK_ID_CNS       2

/* Tested virtual port id. All tests uses this port id for testing.     */
#define TRUNK_TESTED_VIRT_PORT_ID_CNS   8

#define TRUNK_HASH_MASK_CRC_NUM_ENTRIES_CNS 28
/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssDxChTrunkMembersSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_U32                   numOfEnabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    enabledMembersArray[],
    IN GT_U32                   numOfDisabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    disabledMembersArray[]
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkMembersSet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call with trunkId [2], numOfEnabledMembers [5], enabledMembersArray[0 / 1 / 2 / 3 / 4] {port [0 / 1 / 2 / 3 / 4], device [devNum]}, numOfDisabledMembers [3], disabledMembersArray [0 / 1 / 2] {port [5 / 6 / 7], device [devNum]}. Expected: GT_OK.
1.2. Try to set already written trunk members to another trunk id. Call with trunk id [3], numOfEnabledMembers [2], enabledMembersArray [0 / 1] {port [7 / 4], device[devNum]},  numOfDisabledMembers [2], disabledMembersArray [0 / 1] {port [6 / 2], device [devNum]}. Expected: NON GT_OK.
1.3. Try to set invalid device id and legal port id as trunk entry member. Call with trunk id [2], numOfEnabledMembers [1], enabledMembersArray[0] {port [0], device [PRV_CPSS_MAX_PP_DEVICES_CNS]}, numOfDisabledMembers [0]. Expected: NON GT_OK.
1.4. Try to set legal device id and invalid port id as trunk entry member. Call with trunk id [2], numOfEnabledMembers [1], enabledMembersArray[0] {port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], device [devNum]}, numOfDisabledMembers [0]. Expected: NON GT_OK.
1.5. Call cpssDxChTrunkDbEnabledMembersGet with trunkId [2], non-NULL numOfEnabledMembersPtr, and allocated enabledMembersArray for 8 members. Expected: GT_OK, numOfEnabledMembersPtr [5], and the same enabledMembersArray as written.
1.6. Call cpssDxChTrunkDbDisabledMembersGet with trunkId [2], non-NULL numOfDisabledMembersPtr, and allocated disabledMembersArray for 8 members. Expected: GT_OK, numOfDisabledMembersPtr [3], and the same disabledMembersArray as written.
1.7. Check out of range for sum [>CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS = 8] of enabled and disabled members. Call with trunkId [2], numOfEnabledMembers [6], enabledMembersArray [0 / 1 / 2 / 3 / 4 / 5] {port [0 / 1 / 2 / 3 / 4 / 5], device [devNum]}, numOfDisabledMembers [3], disabledMembersArray [tested dev id, virtual port id 6, 7, 8]. Expected: NON GT_OK.
1.8. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], numOfEnabledMembers [5], enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4], numOfDisabledMembers [3], disabledMembersArray [tested dev id, virtual port id 5, 6, 7]. Expected: NON GT_OK.
1.9. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], numOfEnabledMembers [5], enabledMembersArray [0 / 1 / 2 / 3 / 4] {port [0 / 1 / 2 / 3 / 4], device [devNum]}, numOfDisabledMembers [3], disabledMembersArray [0 / 1 / 2] {port [5 / 6 / 7], device [devNum]}. Expected: NON GT_OK.
1.10. Call with trunkId [2], numOfEnabledMembers [0], enabledMembersArray [NULL], numOfDisabledMembers [0], disabledMembersArray [NULL] to remove all members from the trunk. Expected: GT_OK.
1.11. Call with trunkId [2], numOfEnabledMembers [1], enabledMembersArray [NULL], numOfDisabledMembers [0], non-null  disabledMembersArray.
Expected: GT_BAD_PTR.
1.12. Call with trunkId [2], numOfEnabledMembers [0], non-null enabledMembersArray, numOfDisabledMembers [1], disabledMembersArray[NULL].
Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    GT_U32                  numOfEnabledMembers;
    CPSS_TRUNK_MEMBER_STC   enabledMembersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U32                  numOfDisabledMembers;
    CPSS_TRUNK_MEMBER_STC   disabledMembersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_32                   res;
    GT_U32                  numOfMembersWritten;
    CPSS_TRUNK_MEMBER_STC   membersArrayWritten[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U8                   hwDevNum;


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trunkId [2], numOfEnabledMembers [5],                                         */
        /* enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4],                          */
        /* numOfDisabledMembers [3], disabledMembersArray [tested dev id, virtual port id 5, 6, 7].     */
        /* Expected: GT_OK.                                                                             */
        trunkId = (GT_TRUNK_ID)(TRUNK_TESTED_TRUNK_ID_CNS + 3 * dev);
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(dev);

        numOfEnabledMembers = 5;
        enabledMembersArray[0].device = hwDevNum;
        enabledMembersArray[0].port = 0;
        enabledMembersArray[1].device = hwDevNum;
        enabledMembersArray[1].port = 1;
        enabledMembersArray[2].device = hwDevNum;
        enabledMembersArray[2].port = 2;
        enabledMembersArray[3].device = hwDevNum;
        enabledMembersArray[3].port = 3;
        enabledMembersArray[4].device = hwDevNum;
        enabledMembersArray[4].port = 4;

        numOfDisabledMembers = 3;
        disabledMembersArray[0].device = hwDevNum;
        disabledMembersArray[0].port = 5;
        disabledMembersArray[1].device = hwDevNum;
        disabledMembersArray[1].port = 6;
        disabledMembersArray[2].device = hwDevNum;
        disabledMembersArray[2].port = 7;

        st = cpssDxChTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.2. Try to set already written trunk members to another trunk id.                           */
        /* Call with trunk id [3], numOfEnabledMembers [2],                                             */
        /* enabledMembersArray [tested dev id, virtual port 7, 4],                                      */
        /* numOfDisabledMembers [2], disabledMembersArray [tested dev id, virtual port id 6, 2].        */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = (GT_TRUNK_ID)(TRUNK_TESTED_TRUNK_ID_CNS + 3 * dev + 1);
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        numOfEnabledMembers = 2;
        enabledMembersArray[0].device = hwDevNum;
        enabledMembersArray[0].port = 7;
        enabledMembersArray[1].device = hwDevNum;
        enabledMembersArray[1].port = 4;

        numOfDisabledMembers = 2;
        disabledMembersArray[0].device = hwDevNum;
        disabledMembersArray[0].port = 6;
        disabledMembersArray[1].device = hwDevNum;
        disabledMembersArray[1].port = 2;

        st = cpssDxChTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.3. Try to set invalid device id and legal port id as trunk entry member.                   */
        /* Call with trunk id [2], numOfEnabledMembers [1],                                             */
        /* enabledMembersArray [out of range device id, virtual port 8], numOfDisabledMembers [0].      */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = (GT_TRUNK_ID)(TRUNK_TESTED_TRUNK_ID_CNS + 3 * dev + 2);
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        numOfEnabledMembers = 1;
        enabledMembersArray[0].device = PRV_CPSS_MAX_PP_DEVICES_CNS;
        enabledMembersArray[0].port = 0;

        numOfDisabledMembers = 0;

        st = cpssDxChTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, enabledMembersArray[0]->device = %d",
                                         dev, enabledMembersArray[0].device);

        /* 1.4. Try to set legal device id and invalid port id as trunk entry member.                   */
        /* Call with trunk id [2], numOfEnabledMembers [1],                                             */
        /* enabledMembersArray [tested device id, out of range virtual port],                           */
        /* numOfDisabledMembers [0].                                                                    */
        /* Expected: NON GT_OK.                                                                         */
        numOfEnabledMembers = 1;
        enabledMembersArray[0].device = hwDevNum;
        enabledMembersArray[0].port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        numOfDisabledMembers = 0;

        st = cpssDxChTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, enabledMembersArray[0]->port = %d",
                                         dev, enabledMembersArray[0].port);

        /* 1.5. Call cpssDxChTrunkDbEnabledMembersGet with trunkId [2],                                 */
        /* non-NULL numOfEnabledMembersPtr, and allocated enabledMembersArray for 8 members.            */
        /* Expected: GT_OK, numOfEnabledMembersPtr [5], and the same enabledMembersArray as written.    */
        trunkId = (GT_TRUNK_ID)(TRUNK_TESTED_TRUNK_ID_CNS + 3 * dev);
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        numOfEnabledMembers = 5;
        enabledMembersArray[0].device = hwDevNum;
        enabledMembersArray[0].port = 0;
        enabledMembersArray[1].device = hwDevNum;
        enabledMembersArray[1].port = 1;
        enabledMembersArray[2].device = hwDevNum;
        enabledMembersArray[2].port = 2;
        enabledMembersArray[3].device = hwDevNum;
        enabledMembersArray[3].port = 3;
        enabledMembersArray[4].device = hwDevNum;
        enabledMembersArray[4].port = 4;

        numOfMembersWritten = CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
        st = cpssDxChTrunkDbEnabledMembersGet(dev, trunkId, &numOfMembersWritten, membersArrayWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTrunkDbEnabledMembersGet: %d, %d", dev, trunkId);

        UTF_VERIFY_EQUAL2_STRING_MAC(numOfEnabledMembers, numOfMembersWritten,
                   "get another numOfEnabledMembersPtr than was set: %d, %d", dev, trunkId);

        res = cpssOsMemCmp((const GT_VOID*)enabledMembersArray, (const GT_VOID*)membersArrayWritten,
                            numOfEnabledMembers * sizeof(enabledMembersArray[0]));
        UTF_VERIFY_EQUAL2_STRING_MAC(0, res,
                   "get another enabledMembersArray than was set: %d, %d", dev, trunkId);

        /* 1.6. Call cpssDxChTrunkDbDisabledMembersGet with trunkId [2],                                */
        /* non-NULL numOfDisabledMembersPtr, and allocated disabledMembersArray for 8 members.          */
        /* Expected: GT_OK, numOfDisabledMembersPtr [3], and the same disabledMembersArray as written.  */
        numOfDisabledMembers = 3;
        disabledMembersArray[0].device = hwDevNum;
        disabledMembersArray[0].port = 5;
        disabledMembersArray[1].device = hwDevNum;
        disabledMembersArray[1].port = 6;
        disabledMembersArray[2].device = hwDevNum;
        disabledMembersArray[2].port = 7;

        numOfMembersWritten = CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
        st = cpssDxChTrunkDbDisabledMembersGet(dev, trunkId, &numOfMembersWritten, membersArrayWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTrunkDbDisabledMembersGet: %d, %d", dev, trunkId);

        UTF_VERIFY_EQUAL2_STRING_MAC(numOfDisabledMembers, numOfMembersWritten,
                   "get another numOfDisabledMembersPtr than was set: %d, %d", dev, trunkId);;

        res = cpssOsMemCmp((const GT_VOID*)disabledMembersArray, (const GT_VOID*)membersArrayWritten,
                            numOfDisabledMembers * sizeof(disabledMembersArray[0]));
        UTF_VERIFY_EQUAL2_STRING_MAC(0, res,
                   "get another disabledMembersArray than was set: %d, %d", dev, trunkId);

        /* 1.7. Check out of range for sum [>CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS = 8]                     */
        /* of enabled and disabled members. Call with trunkId [2], numOfEnabledMembers [6],             */
        /* enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4, 5],                       */
        /*  numOfDisabledMembers [3], disabledMembersArray [tested dev id, virtual port id 6, 7, 8].    */
        /* Expected: NON GT_OK.                                                                         */
        numOfEnabledMembers = 6;
        enabledMembersArray[0].device = hwDevNum;
        enabledMembersArray[0].port = 0;
        enabledMembersArray[1].device = hwDevNum;
        enabledMembersArray[1].port = 1;
        enabledMembersArray[2].device = hwDevNum;
        enabledMembersArray[2].port = 2;
        enabledMembersArray[3].device = hwDevNum;
        enabledMembersArray[3].port = 3;
        enabledMembersArray[4].device = hwDevNum;
        enabledMembersArray[4].port = 4;
        enabledMembersArray[5].device = hwDevNum;
        enabledMembersArray[5].port = 5;

        numOfDisabledMembers = 3;
        disabledMembersArray[0].device = hwDevNum;
        disabledMembersArray[0].port = 6;
        disabledMembersArray[1].device = hwDevNum;
        disabledMembersArray[1].port = 7;
        disabledMembersArray[2].device = hwDevNum;
        disabledMembersArray[2].port = 8;

        st = cpssDxChTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.8. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], numOfEnabledMembers [5],               */
        /* enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4],                          */
        /* numOfDisabledMembers [3], disabledMembersArray [tested dev id, virtual port id 5, 6, 7].     */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        numOfEnabledMembers = 5;
        enabledMembersArray[0].device = hwDevNum;
        enabledMembersArray[0].port = 0;
        enabledMembersArray[1].device = hwDevNum;
        enabledMembersArray[1].port = 1;
        enabledMembersArray[2].device = hwDevNum;
        enabledMembersArray[2].port = 2;
        enabledMembersArray[3].device = hwDevNum;
        enabledMembersArray[3].port = 3;
        enabledMembersArray[4].device = hwDevNum;
        enabledMembersArray[4].port = 4;

        numOfDisabledMembers = 2;
        disabledMembersArray[0].device = hwDevNum;
        disabledMembersArray[0].port = 5;
        disabledMembersArray[1].device = hwDevNum;
        disabledMembersArray[1].port = 6;
        disabledMembersArray[2].device = hwDevNum;
        disabledMembersArray[2].port = 7;

        st = cpssDxChTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.9. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], numOfEnabledMembers [5],        */
        /* enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4],                          */
        /* numOfDisabledMembers [3], disabledMembersArray [tested dev id, virtual port id 5, 6, 7].     */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.10. Call cpssDxChTrunkMembersSet with trunkId [2], numOfEnabledMembers [0],                */
        /* enabledMembersArray [NULL], numOfDisabledMembers [0], disabledMembersArray [NULL]            */
        /* to remove all members from the trunk.                                                        */
        /* Expected: GT_OK.                                                                             */
        trunkId = (GT_TRUNK_ID)(TRUNK_TESTED_TRUNK_ID_CNS + 3 * dev);
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        numOfEnabledMembers = 0;
        numOfDisabledMembers = 0;

        st = cpssDxChTrunkMembersSet(dev, trunkId, numOfEnabledMembers, NULL,
                                     numOfDisabledMembers, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.11. Call with trunkId [2], numOfEnabledMembers [1],
           enabledMembersArray [NULL], numOfDisabledMembers [0], non-null
           disabledMembersArray. Expected: GT_BAD_PTR.  */

        numOfEnabledMembers = 1;
        numOfDisabledMembers = 0;

        st = cpssDxChTrunkMembersSet(dev, trunkId, numOfEnabledMembers, NULL,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, numOfEnabledMembers = %d, enabledMembersArray = NULL",
                                     dev, numOfEnabledMembers);

        /* 1.12. Call with trunkId [2], numOfEnabledMembers [0], non-null
           enabledMembersArray, numOfDisabledMembers [1], disabledMembersArray[NULL].
           Expected: GT_BAD_PTR.    */

        numOfEnabledMembers = 0;
        numOfDisabledMembers = 1;

        st = cpssDxChTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, numOfDisabledMembers = %d, disabledMembersArray = NULL",
                                     dev, numOfDisabledMembers);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    numOfEnabledMembers = 5;
    enabledMembersArray[0].device = 0;
    enabledMembersArray[0].port = 0;
    enabledMembersArray[1].device = 0;
    enabledMembersArray[1].port = 1;
    enabledMembersArray[2].device = 0;
    enabledMembersArray[2].port = 2;
    enabledMembersArray[3].device = 0;
    enabledMembersArray[3].port = 3;
    enabledMembersArray[4].device = 0;
    enabledMembersArray[4].port = 4;

    numOfDisabledMembers = 2;
    disabledMembersArray[0].device = 0;
    disabledMembersArray[0].port = 5;
    disabledMembersArray[1].device = 0;
    disabledMembersArray[1].port = 6;
    disabledMembersArray[2].device = 0;
    disabledMembersArray[2].port = 7;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                 numOfDisabledMembers, disabledMembersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssDxChTrunkMemberAdd
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkMemberAdd)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.2. Try to add the same trunk member again, accordingly to header the result must be OK. Call with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.3. Try to add already added trunk members to another trunk id. Call with trunk id [3], non-NULL memberPtr {port [8], device [devNum]}. Expected: NON GT_OK.
1.4. Try to add invalid device id and legal port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr {port [8], device [PRV_CPSS_MAX_PP_DEVICES_CNS]}. Expected: NON GT_OK.
1.5. Try to add legal device id and invalid port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr {port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], device [devNum]}. Expected: NON GT_OK.
1.6. Check for NULL handling. Call with trunk id [2], memberPtr [NULL]. Expected: GT_BAD_PTR.
1.7. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL memberPtr [tested dev, virtual port 8]. Expected: NON GT_OK.
1.8. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL memberPtr [tested dev, virtual port 8]. Expected: NON GT_OK.
1.9. Call cpssDxChTrunkMemberRemove with trunkId [2], memberPtr {port [8], device [devNum]} to remove added member from the trunk. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    CPSS_TRUNK_MEMBER_STC   member;
    GT_U8                   hwDevNum;


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].             */
        /* Expected: GT_OK.                                                                             */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(dev);

        member.device = hwDevNum;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.2. Try to add the same trunk member again, accordingly to header the result must be OK.    */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev, virtual port 8].                     */
        /* Expected: GT_OK.                                                                             */
        st = cpssDxChTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.3. Try to add already added trunk members to another trunk id.                             */
        /* Call with trunk id [3], non-NULL memberPtr [tested dev id, virtual port 8].                  */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS + 1;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                         dev, trunkId, member.device, member.port);

        /* 1.4. Try to add invalid device id and legal port id as trunk entry member.                   */
        /* Call with trunk id [2], non-NULL memberPtr [invalid dev id, virtual port 8].                 */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        member.device = PRV_CPSS_MAX_PP_DEVICES_CNS;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d",
                                         dev, trunkId, member.device);

        /* 1.5. Try to add legal device id and invalid port id as trunk entry member.                   */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev id, invalid port].                    */
        /* Expected: NON GT_OK.                                                                         */
        member.device = hwDevNum;
        member.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->port = %d",
                                         dev, trunkId, member.port);

        /* 1.6. Check for NULL handling. Call with trunk id [2], memberPtr [NULL].                      */
        /* Expected: GT_BAD_PTR.                                                                        */
        st = cpssDxChTrunkMemberAdd(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, memberPtr = NULL", dev, trunkId);

        /* 1.7. Call with null trunk id [NULL_TRUNK_ID_CNS = 0],                                        */
        /* non-NULL memberPtr [tested dev, virtual port 8].                                             */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.8. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],                                 */
        /* non-NULL memberPtr [tested dev, virtual port 8].                                             */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.9. Call cpssDxChTrunkMemberRemove with trunkId [2],                                        */
        /* memberPtr [tested dev id, virtual port 8] to remove added member from the trunk.             */
        /* Expected: GT_OK                                                                              */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChTrunkMemberRemove: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    member.device = 0;
    member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkMemberAdd(dev, trunkId, &member);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssDxChTrunkMemberRemove
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkMemberRemove)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Add member to tested trunk id before call remove function. Call cpssDxChTrunkMemberAdd with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.2. Try to remove invalid device id and legal port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr {port [8], device [PRV_CPSS_MAX_PP_DEVICES_CNS]}. Expected: GT_OK.
1.3. Try to remove legal device id and invalid port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr {port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], device [devNum]}. Expected: NON GT_OK.
1.4. Try to remove added member. Call with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.5. Try to remove already removed trunk member again, accordingly to header the result must be OK. Call with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.6. Check for NULL handling. Call with trunk id [2], memberPtr [NULL]. Expected: GT_BAD_PTR.
1.7. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL memberPtr [tested dev, virtual port 8]. Expected: NON GT_OK.
1.8. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL memberPtr [tested dev, virtual port 8]. Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    CPSS_TRUNK_MEMBER_STC   member;
    GT_U8                   hwDevNum;


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Add member to tested trunk id before call remove function.                                      */
        /* Call cpssDxChTrunkMemberAdd with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].   */
        /* Expected: GT_OK.                                                                                     */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(dev);

        member.device = hwDevNum;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChTrunkMemberAdd: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.2. Try to remove invalid device id and legal port id as trunk
           entry member. Call with trunk id [2], non-NULL memberPtr {port [8],
           device [PRV_CPSS_MAX_PP_DEVICES_CNS]}. Expected: GT_OK.
        */

        member.device = PRV_CPSS_MAX_PP_DEVICES_CNS;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d",
                                     dev, trunkId, member.device);

        /* 1.3. Try to remove legal device id and invalid port id as trunk entry member.                        */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev id, invalid port].                            */
        /* Expected: GT_OK.                                                                                 */
        member.device = hwDevNum;
        member.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->port = %d",
                                     dev, trunkId, member.port);

        /* 1.4. Try to remove added member.                                                                     */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].                          */
        /* Expected: GT_OK.                                                                                     */
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.5. Try to remove already removed trunk member again, accordingly to header the result must be OK.  */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev, virtual port 8].                             */
        /* Expected: GT_OK.                                                                                     */
        st = cpssDxChTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.6. Check for NULL handling. Call with trunk id [2], memberPtr [NULL].                              */
        /* Expected: GT_BAD_PTR.                                                                                */
        st = cpssDxChTrunkMemberRemove(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, memberPtr = NULL", dev, trunkId);

        /* 1.7. Call with null trunk id [NULL_TRUNK_ID_CNS = 0],                                                */
        /* non-NULL memberPtr [tested dev, virtual port 8].                                                     */
        /* Expected: NON GT_OK.                                                                                 */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        member.device = hwDevNum;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.8. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],                                         */
        /* non-NULL memberPtr [tested dev, virtual port 8].                                                     */
        /* Expected: NON GT_OK.                                                                                 */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    member.device = 0;
    member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkMemberRemove(dev, trunkId, &member);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssDxChTrunkMemberDisable
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkMemberDisable)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Add member to tested trunk before disable it. Call cpssDxChTrunkMemberAdd with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.2. Call with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.3. Try to disable already disabled trunk member, accordingly to header the result must be OK. Call with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.4. Try to disable invalid device id and legal port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr {port [8], device [PRV_CPSS_MAX_PP_DEVICES_CNS]} Expected: NON GT_OK.
1.5. Try to disable legal device id and invalid port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr {port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], device [devNum]}. Expected: NON GT_OK.
1.6. Remove member from trunk and try to disable it then. Call cpssDxChTrunkMemberRemove with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.7. Call with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: NON GT_OK.
1.8. Check for NULL handling. Call with trunk id [2], memberPtr [NULL]. Expected: GT_BAD_PTR.
1.9. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL memberPtr [tested dev, virtual port 8]. Expected: NON GT_OK.
1.10. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL memberPtr {port [8], device [devNum]}. Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    CPSS_TRUNK_MEMBER_STC   member;
    GT_U8                   hwDevNum;


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Add member to tested trunk before disable it.                                                       */
        /* Call cpssDxChTrunkMemberAdd with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].       */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(dev);

        member.device = hwDevNum;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChTrunkMemberAdd: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.2. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].                         */
        /* Expected: GT_OK.                                                                                         */
        st = cpssDxChTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.3. Try to disable already disabled trunk member, accordingly to header the result must be OK.          */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].                              */
        /* Expected: GT_OK.                                                                                         */
        st = cpssDxChTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.4. Try to disable invalid device id and legal port id as trunk entry member.                           */
        /* Call with trunk id [2], non-NULL memberPtr [invalid dev id, virtual port 8]                              */
        /* Expected: NON GT_OK.                                                                                     */
        member.device = PRV_CPSS_MAX_PP_DEVICES_CNS;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d",
                                         dev, trunkId, member.device);

        /* 1.5. Try to disable legal device id and invalid port id as trunk entry member.                           */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev id, invalid port].                                */
        /* Expected: NON GT_OK.                                                                                     */
        member.device = hwDevNum;
        member.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->port = %d",
                                         dev, trunkId, member.port);

        /* 1.6. Remove member from trunk and try to disable it then.                                                */
        /* Call cpssDxChTrunkMemberRemove with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].    */
        /* Expected: GT_OK.                                                                                         */
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChTrunkMemberRemove: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.7. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].                         */
        /* Expected: NON GT_OK.                                                                                     */
        st = cpssDxChTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.8. Check for NULL handling. Call with trunk id [2], memberPtr [NULL].                                  */
        /* Expected: GT_BAD_PTR.                                                                                    */
        st = cpssDxChTrunkMemberDisable(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, memberPtr = NULL", dev, trunkId);

        /* 1.9. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL memberPtr [tested dev, virtual port 8].   */
        /* Expected: NON GT_OK.                                                                                     */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.10. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],                                            */
        /* non-NULL memberPtr [tested dev, virtual port 8].                                                         */
        /* Expected: NON GT_OK.                                                                                     */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    member.device = 0;
    member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkMemberDisable(dev, trunkId, &member);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssDxChTrunkMemberEnable
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkMemberEnable)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Add member to tested trunk before enable it. Call cpssDxChTrunkMemberAdd with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.2. Call with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.3. Try to enable already enabled trunk member, accordingly to header the result must be OK. Call with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.4. Try to enable invalid device id and legal port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr {port [8], device [PRV_CPSS_MAX_PP_DEVICES_CNS]} Expected: NON GT_OK.
1.5. Try to enable legal device id and invalid port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr { PRV_CPSS_MAX_PP_PORTS_NUM_CNS, device [devNum]}. Expected: NON GT_OK.
1.6. Remove member from trunk and try to enable it then. Call cpssDxChTrunkMemberRemove with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.7. Call with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: NON GT_OK.
1.8. Check for NULL handling. Call with trunk id [2], memberPtr [NULL]. Expected: GT_BAD_PTR.
1.9. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL memberPtr {port [8], device [devNum]}. Expected: NON GT_OK.
1.10. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL memberPtr {port [8], device [devNum]}. Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    CPSS_TRUNK_MEMBER_STC   member;
    GT_U8                   hwDevNum;


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Add member to tested trunk before enable it.                                                        */
        /* Call cpssDxChTrunkMemberAdd with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].       */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(dev);

        member.device = hwDevNum;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChTrunkMemberAdd: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.2. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].                         */
        /* Expected: GT_OK.                                                                                         */
        st = cpssDxChTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.3. Try to enable already enabled trunk member, accordingly to header the result must be OK.            */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].                              */
        /* Expected: GT_OK.                                                                                         */
        st = cpssDxChTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.4. Try to enable invalid device id and legal port id as trunk entry member.                            */
        /* Call with trunk id [2], non-NULL memberPtr [invalid dev id, virtual port 8]                              */
        /* Expected: NON GT_OK.                                                                                     */
        member.device = PRV_CPSS_MAX_PP_DEVICES_CNS;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d",
                                         dev, trunkId, member.device);

        /* 1.5. Try to enable legal device id and invalid port id as trunk entry member.                            */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev id, invalid port].                                */
        /* Expected: NON GT_OK.                                                                                     */
        member.device = hwDevNum;
        member.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->port = %d",
                                         dev, trunkId, member.port);

        /* 1.6. Remove member from trunk and try to enable it then.                                                 */
        /* Call cpssDxChTrunkMemberRemove with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].    */
        /* Expected: GT_OK.                                                                                         */
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChTrunkMemberRemove: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.7. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].                         */
        /* Expected: NON GT_OK.                                                                                     */
        st = cpssDxChTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.8. Check for NULL handling. Call with trunk id [2], memberPtr [NULL].                                  */
        /* Expected: GT_BAD_PTR.                                                                                    */
        st = cpssDxChTrunkMemberEnable(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, memberPtr = NULL", dev, trunkId);

        /* 1.9. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL memberPtr [tested dev, virtual port 8].   */
        /* Expected: NON GT_OK.                                                                                     */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.10. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],                                            */
        /* non-NULL memberPtr [tested dev, virtual port 8].                                                         */
        /* Expected: NON GT_OK.                                                                                     */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    member.device = 0;
    member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkMemberEnable(dev, trunkId, &member);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low-Level Mode ? (in ExMx - High)

GT_STATUS cpssDxChTrunkNonTrunkPortsAdd
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *nonTrunkPortsBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkNonTrunkPortsAdd)
{
/*
ITERATE_DEVICES
1.1. Call with trunk id [2], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00] to add first eight virtual ports to the trunk's non-trunk entry. Expected: GT_OK.
1.2. Check for NULL handling. Call with trunk id [2], nonTrunkPortsBmpPtr [NULL]. Expected: GT_BAD_PTR.
1.3. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00]. Expected: GT_OK.
1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00]. Expected: NON GT_OK.
1.5. Call cpssDxChTrunkNonTrunkPortsRemove with trunkId [2], nonTrunkPortsBmp [0xFF, 0x00] to remove all non trunk members from the trunk  Expected: GT_OK.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPortsBmp;
    GT_U8               port;


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    cpssOsBzero((GT_VOID*)&nonTrunkPortsBmp, sizeof(nonTrunkPortsBmp));
    for (port = 0; port < CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS; port++)
    {
        CPSS_PORTS_BMP_PORT_SET_MAC(&nonTrunkPortsBmp, port);
    }

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trunk id [2], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00] to add first eight                */
        /* virtual ports to the trunk's non-trunk entry.                                                            */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, nonTrunkPortsBmp.ports[0], nonTrunkPortsBmp.ports[1]);

        /* 1.2. Check for NULL handling. Call with trunk id [2], nonTrunkPortsBmpPtr [NULL].                        */
        /* Expected: GT_BAD_PTR.                                                                                  */
        st = cpssDxChTrunkNonTrunkPortsAdd(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nonTrunkPortsBmpPtr = NULL", dev, trunkId);

        /* 1.3. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0],                      */
        /* non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00].                                                               */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00].  */
        /* Expected: NON GT_OK.                                                                                     */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.5. Call cpssDxChTrunkNonTrunkPortsRemove with trunkId [2], nonTrunkPortsBmp [0xFF, 0x00]               */
        /* to remove all non trunk members from the trunk                                                           */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChTrunkNonTrunkPortsRemove: %d, %d, %d, %d",
                                     dev, trunkId, nonTrunkPortsBmp.ports[0], nonTrunkPortsBmp.ports[1]);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low-Level Mode ? (in ExMx - High)

GT_STATUS cpssDxChTrunkNonTrunkPortsRemove
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *nonTrunkPortsBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkNonTrunkPortsRemove)
{
/*
ITERATE_DEVICES
1.1. Add first eight virtual ports to the trunk's non-trunk entry. Call cpssDxChTrunkNonTrunkPortsAdd with trunkId [2], nonTrunkPortsBmpPtr [0xFF, 0x00]. Expected: GT_OK.
1.2. Call with trunk id [2], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00] to remove first eight virtual ports from the trunk's non-trunk entry. Expected: GT_OK.
1.3. Check for NULL handling. Call with trunk id [2], nonTrunkPortsBmpPtr [NULL]. Expected: GT_BAD_PTR.
1.4. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00]. Expected: GT_OK.
1.5. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00]. Expected: NON GT_OK.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPortsBmp;
    GT_U8               port;


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    cpssOsBzero((GT_VOID*)&nonTrunkPortsBmp, sizeof(nonTrunkPortsBmp));
    for (port = 0; port < CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS; port++)
    {
        CPSS_PORTS_BMP_PORT_SET_MAC(&nonTrunkPortsBmp, port);
    }

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Add first eight virtual ports to the trunk's non-trunk entry.                                       */
        /* Call cpssDxChTrunkNonTrunkPortsAdd with trunkId [2], nonTrunkPortsBmpPtr [0xFF, 0x00].                   */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChTrunkNonTrunkPortsAdd: %d, %d, %d, %d",
                                     dev, trunkId, nonTrunkPortsBmp.ports[0], nonTrunkPortsBmp.ports[1]);

        /* 1.2. Call with trunk id [2], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00] to remove first eight             */
        /* virtual ports to the trunk's non-trunk entry.                                                            */
        /* Expected: GT_OK.                                                                                         */
        st = cpssDxChTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, nonTrunkPortsBmp.ports[0], nonTrunkPortsBmp.ports[1]);

        /* 1.3. Check for NULL handling. Call with trunk id [2], nonTrunkPortsBmpPtr [NULL].                        */
        /* Expected: GT_BAD_PTR.                                                                                  */
        st = cpssDxChTrunkNonTrunkPortsRemove(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nonTrunkPortsBmpPtr = NULL", dev, trunkId);

        /* 1.4. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0],                      */
        /* non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00].                                                               */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.5. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00].  */
        /* Expected: NON GT_OK.                                                                                     */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low-Level Mode

GT_STATUS cpssDxChTrunkPortTrunkIdSet
(
    IN GT_U8            devNum,
    IN GT_U8            portNum,
    IN GT_BOOL          memberOfTrunk,
    IN GT_TRUNK_ID      trunkId
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkPortTrunkIdSet)
{
/*
ITERATE_DEVICES_VIRT_PORTS (DxChx)
1.1.1. Call with memberOfTrunk [GT_TRUE], trunkId [2]. Expected: GT_OK.
1.1.2. Call cpssDxChTrunkPortTrunkIdGet with non-NULL memberOfTrunkPtr, and non-NULL trunkIdPtr. Expected: GT_OK, memberOfTrunkPtr [GT_TRUE] and trunkIdPtr [2].
1.1.3. Call with memberOfTrunk [GT_FALSE], trunkid [2] (but trunkid will be set to 0). Expected: GT_OK.
1.1.4. Call cpssDxChTrunkPortTrunkIdGet with non-NULL memberOfTrunkPtr, and non-NULL trunkIdPtr. Expected: GT_OK, memberOfTrunkPtr [GT_FALSE] and trunkIdPtr [0].
1.1.5. Call with null trunk id [NULL_TRUNK_ID_CNS = 0] and memberOfTrunk [GT_TRUE]. Expected: NON GT_OK.
1.1.6. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128] memberOfTrunk [GT_TRUE]. Expected: NON GT_OK.
1.1.7. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128] (this value relevant only when memberOfTrunk = GT_TRUE) memberOfTrunk [GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     memberOfTrunk;
    GT_TRUNK_ID trunkId;
    GT_BOOL         memberOfTrunkWritten;
    GT_TRUNK_ID     trunkIdWritten;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with memberOfTrunk [GT_TRUE], trunkId [2].
               Expected: GT_OK. */

            trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
            memberOfTrunk = GT_TRUE;

            st = cpssDxChTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, memberOfTrunk, trunkId);

            /* 1.1.2. Call cpssDxChTrunkPortTrunkIdGet with non-NULL
               memberOfTrunkPtr, and non-NULL trunkIdPtr. Expected: GT_OK,
               memberOfTrunkPtr [GT_TRUE] and trunkIdPtr [2].   */

            st = cpssDxChTrunkPortTrunkIdGet(dev, port, &memberOfTrunkWritten, &trunkIdWritten);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTrunkPortTrunkIdGet: %d, %d", dev, port);

            /* Verifuing values */
            UTF_VERIFY_EQUAL2_STRING_MAC(memberOfTrunk, memberOfTrunkWritten,
                   "get another memberOfTrunkPtr than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(trunkId, trunkIdWritten,
                   "get another trunkIdPtr than was set: %d, %d", dev, port);

            /* 1.1.3. Call with memberOfTrunk [GT_FALSE], trunkid [2]
               (but trunkid will be set to 0). Expected: GT_OK. */

            trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
            memberOfTrunk = GT_FALSE;

            st = cpssDxChTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, memberOfTrunk, trunkId);

            trunkId = 0;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

            /* 1.1.4. Call cpssDxChTrunkPortTrunkIdGet with non-NULL
               memberOfTrunkPtr, and non-NULL trunkIdPtr. Expected: GT_OK,
               memberOfTrunkPtr [GT_FALSE] and trunkIdPtr [0].  */

            st = cpssDxChTrunkPortTrunkIdGet(dev, port, &memberOfTrunkWritten, &trunkIdWritten);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTrunkPortTrunkIdGet: %d, %d", dev, port);

            /* Verifuing values */
            UTF_VERIFY_EQUAL2_STRING_MAC(memberOfTrunk, memberOfTrunkWritten,
                   "get another memberOfTrunkPtr than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(trunkId, trunkIdWritten,
                   "get another trunkIdPtr than was set: %d, %d", dev, port);

            /* 1.1.5. Call with null trunk id [NULL_TRUNK_ID_CNS = 0] and
               memberOfTrunk [GT_TRUE]. Expected: NON GT_OK.    */

            trunkId = TRUNK_NULL_TRUNK_ID_CNS;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
            memberOfTrunk = GT_TRUE;

            st = cpssDxChTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, memberOfTrunk, trunkId);

            /* 1.1.6. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128]
               memberOfTrunk [GT_TRUE]. Expected: NON GT_OK.    */

            trunkId = TRUNK_MAX_TRUNK_ID_CNS;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

            st = cpssDxChTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, memberOfTrunk, trunkId);

            /* 1.1.7. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128]
               (this value relevant only when memberOfTrunk = GT_TRUE)
                memberOfTrunk [GT_FALSE]. Expected: GT_OK.  */
            memberOfTrunk = GT_FALSE;

            st = cpssDxChTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, memberOfTrunk, trunkId);
        }

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
        memberOfTrunk = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
    memberOfTrunk = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkPortTrunkIdSet(dev, port, memberOfTrunk, trunkId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All Modes

GT_STATUS cpssDxChTrunkPortTrunkIdGet
(
    IN GT_U8            devNum,
    IN GT_U8            portNum,
    OUT GT_BOOL         *memberOfTrunkPtr,
    OUT GT_TRUNK_ID     *trunkIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkPortTrunkIdGet)
{
/*
ITERATE_DEVICES_VIRT_PORTS (DxChx)
1.1.1. Call with non-NULL memberOfTrunkPtr, and non-NULL trunkIdPtr. Expected: GT_OK.
1.1.2. Call with memberOfTrunkPtr [NULL], and non-NULL trunkIdPtr. Expected: GT_BAD_PTR.
1.1.3. Call with non-NULL memberOfTrunkPtr, and trunkIdPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           port;
    GT_BOOL         memberOfTrunk;
    GT_TRUNK_ID     trunkId;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-NULL memberOfTrunkPtr,
               and non-NULL trunkIdPtr. Expected: GT_OK.    */

            st = cpssDxChTrunkPortTrunkIdGet(dev, port, &memberOfTrunk, &trunkId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with memberOfTrunkPtr [NULL], and non-NULL
               trunkIdPtr. Expected: GT_BAD_PTR.    */

            st = cpssDxChTrunkPortTrunkIdGet(dev, port, NULL, &trunkId);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, memberOfTrunkPtr = NULL", dev, port);

            /* 1.1.3. Call with non-NULL memberOfTrunkPtr, and
               trunkIdPtr [NULL]. Expected: GT_BAD_PTR. */

            st = cpssDxChTrunkPortTrunkIdGet(dev, port, &memberOfTrunk, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, trunkIdPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChTrunkPortTrunkIdGet(dev, port, &memberOfTrunk, &trunkId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTrunkPortTrunkIdGet(dev, port, &memberOfTrunk, &trunkId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChTrunkPortTrunkIdGet(dev, port, &memberOfTrunk, &trunkId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkPortTrunkIdGet(dev, port, &memberOfTrunk, &trunkId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkPortTrunkIdGet(dev, port, &memberOfTrunk, &trunkId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low-Level Mode

GT_STATUS cpssDxChTrunkTableEntrySet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    IN  GT_U32                  numMembers,
    IN  CPSS_TRUNK_MEMBER_STC   membersArray[]
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkTableEntrySet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call with trunkId [2], numMembers [3], membersArray [0 / 1 / 2] {port [0 / 1 / 2], device [devNum]}. Expected: GT_OK.
1.2. Call with cpssDxChTrunkTableEntryGet with trunkId [2], non-NULL numMembersPtr, membersArray. Expected: GT_OK, numMembersPtr [3], and the same membersArray as was set.
1.3. Call with out of range numMembers [9]. Expected: NON GT_OK.
1.4. Call with null trunk id [NULL_TRUNK_ID_CNS = 0]. Expected: NON GT_OK.
1.5. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128]. Expected: NON GT_OK.
1.6. Call with trunkId [2], numMembers [3], membersArray [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    GT_U32                  numMembers;
    CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_32                   res;
    GT_U32                  numMembersWritten;
    CPSS_TRUNK_MEMBER_STC   membersArrayWritten[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U8                   hwDevNum;


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trunkId [2], numMembers [3], membersArray [tested dev id, virtual port 0, 1, 2].      */
        /* Expected: GT_OK.                                                                                     */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(dev);

        numMembers = 3;
        membersArray[0].device = hwDevNum;
        membersArray[0].port = 0;
        membersArray[1].device = hwDevNum;
        membersArray[1].port = 1;
        membersArray[2].device = hwDevNum;
        membersArray[2].port = 2;

        st = cpssDxChTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trunkId, numMembers);

        /* 1.2. Call with cpssDxChTrunkTableEntryGet with trunkId [2], non-NULL numMembersPtr, membersArray.    */
        /* Expected: GT_OK, numMembersPtr [3], and the same membersArray as was set.                            */
        st = cpssDxChTrunkTableEntryGet(dev, trunkId, &numMembersWritten, membersArrayWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTrunkTableEntryGet: %d, %d", dev, trunkId);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(numMembers, numMembersWritten,
                   "get another numMembersPtr than was set: %d, %d", dev, trunkId);

        res = cpssOsMemCmp((const GT_VOID*)membersArray, (const GT_VOID*)membersArrayWritten,
                            numMembers*sizeof(membersArray[0]));
        UTF_VERIFY_EQUAL2_STRING_MAC(0, res,
                   "get another membersArray than was set: %d, %d", dev, trunkId);

        /* 1.3. Call with out of range numMembers [9].                                                          */
        /* Expected: NON GT_OK.                                                                                 */
        numMembers = CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS + 1;

        st = cpssDxChTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, trunkId, numMembers);

        /* 1.4. Call with null trunk id [NULL_TRUNK_ID_CNS = 0].                                                */
        /* Expected: NON GT_OK.                                                                                 */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
        numMembers = 3;

        st = cpssDxChTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.5. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128].                                         */
        /* Expected: NON GT_OK.                                                                                 */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
        numMembers = 3;

        st = cpssDxChTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.6. Call with runkId [2], numMembers [3], membersArray [NULL].
           Expected: GT_BAD_PTR. */

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkTableEntrySet(dev, trunkId, numMembers, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, membersArray = NULL", dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    numMembers = 3;
    membersArray[0].device = dev;
    membersArray[0].port = 0;
    membersArray[1].device = dev;
    membersArray[1].port = 1;
    membersArray[2].device = dev;
    membersArray[2].port = 2;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All Modes

GT_STATUS cpssDxChTrunkTableEntryGet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    OUT GT_U32                  *numMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkTableEntryGet)
{
/*
ITERATE_DEVICES
1.1. Call with trunkId [2], not-NULL numMembers, and allocated membersArray. Expected: GT_OK.
1.2. Call with numMembersPtr [NULL]. Expected: GT_BAD_PTR.
1.3. Call with null trunk id [NULL_TRUNK_ID_CNS = 0]. Expected: NON GT_OK.
1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128]. Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    GT_U32                  numMembers;
    CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trunkId [2], not-NULL numMembers, and allocated membersArray.         */
        /* Expected: GT_OK.                                                                     */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.2. Call with numMembersPtr [NULL].                                                 */
        /* Expected: GT_BAD_PTR.                                                                */
        st = cpssDxChTrunkTableEntryGet(dev, trunkId, NULL, membersArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, numMembersPtr = NULL", dev, trunkId);

        /* 1.3. Call with null trunk id [NULL_TRUNK_ID_CNS = 0].                                */
        /* Expected: NON GT_OK.                                                                 */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128].                         */
        /* Expected: NON GT_OK.                                                                 */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low-Level Mode

GT_STATUS cpssDxChTrunkNonTrunkPortsEntrySet
(
    IN  GT_U8               devNum,
    IN  GT_TRUNK_ID         trunkId,
    IN  CPSS_PORTS_BMP_STC  *nonTrunkPortsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkNonTrunkPortsEntrySet)
{
/*
ITERATE_DEVICES
1.1. Call with trunkId [2], non-NULL nonTrunkPortsPtr [0xFF, 0x00]. Expected: GT_OK.
1.2. Call cpssDxChTrunkNonTrunkPortsEntryGet with trunkId [2], non-NULL nonTrunkPortsPtr. Expected: GT_OK and the same port bitmap as was written.
1.3. Check for NULL handling. Call with trunkId [2], nonTrunkPortsPtr [NULL]. Expected: GT_BAD_PTR.
1.4. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL nonTrunkPortsPtr [0xFF, 0x00]. Expected: GT_OK.
1.5. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL nonTrunkPortsPtr [0xFF, 0x00]. Expected: NON GT_OK.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPorts;
    GT_U8               port;


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    cpssOsBzero((GT_VOID*)&nonTrunkPorts, sizeof(nonTrunkPorts));
    for (port = 0; port < CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS; port++)
    {
        CPSS_PORTS_BMP_PORT_SET_MAC(&nonTrunkPorts, port);
    }

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        CPSS_PORTS_BMP_STC  nonTrunkPortsWritten;

        /* 1.1. Call with trunkId [2], non-NULL nonTrunkPortsPtr [0xFF, 0x00].                              */
        /* Expected: GT_OK.                                                                                 */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, nonTrunkPorts.ports[0], nonTrunkPorts.ports[1]);

        /* 1.2. Call cpssDxChTrunkNonTrunkPortsEntryGet with trunkId [2], non-NULL nonTrunkPortsPtr.        */
        /* Expected: GT_OK and the same port bitmap as was written.                                         */
        st = cpssDxChTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPortsWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTrunkNonTrunkPortsEntryGet: %d, %d", dev, trunkId);

        /* Verifying values */
        UTF_VERIFY_EQUAL2_STRING_MAC(nonTrunkPorts.ports[0], nonTrunkPortsWritten.ports[0],
                   "get another nonTrunkPortsPtr->ports[0] than was set: %d, %d", dev, trunkId);
        UTF_VERIFY_EQUAL2_STRING_MAC(nonTrunkPorts.ports[1], nonTrunkPortsWritten.ports[1],
                   "get another nonTrunkPortsPtr->ports[1] than was set: %d, %d", dev, trunkId);

        /* 1.3. Check for NULL handling. Call with trunkId [2], nonTrunkPortsPtr [NULL].                    */
        /* Expected: GT_BAD_PTR.                                                                            */
        st = cpssDxChTrunkNonTrunkPortsEntrySet(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nonTrunkPortsPtr = NULL", dev, trunkId);

        /* 1.4. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0],              */
        /* non-NULL nonTrunkPortsPtr [0xFF, 0x00].                                                          */
        /* Expected: GT_OK.                                                                                 */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.5. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],                                     */
        /* non-NULL nonTrunkPortsPtr [0xFF, 0x00].                                                          */
        /* Expected: NON GT_OK.                                                                             */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All Modes

GT_STATUS cpssDxChTrunkNonTrunkPortsEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_TRUNK_ID         trunkId,
    OUT  CPSS_PORTS_BMP_STC  *nonTrunkPortsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkNonTrunkPortsEntryGet)
{
/*
ITERATE_DEVICES
1.1. Call with trunkId [2], non-NULL nonTrunkPortsPtr. Expected: GT_OK.
1.2. Check for NULL handling. Call with trunk id [2], nonTrunkPortsPtr [NULL]. Expected: GT_BAD_PTR.
1.3. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL nonTrunkPortsPtr [0xFF, 0x00]. Expected: GT_OK.
1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL nonTrunkPortsPtr [0xFF, 0x00]. Expected: NON GT_OK.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPorts;


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trunkId [2], non-NULL nonTrunkPortsPtr.                               */
        /* Expected: GT_OK.                                                                     */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.2. Check for NULL handling. Call with trunk id [2], nonTrunkPortsPtr [NULL].       */
        /* Expected: GT_BAD_PTR.                                                                */
        st = cpssDxChTrunkNonTrunkPortsEntryGet(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, nonTrunkPortsPtr = NULL", st, dev);

        /* 1.3. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0],  */
        /* non-NULL nonTrunkPortsPtr [0xFF, 0x00].                                              */
        /* Expected: GT_OK.                                                                     */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],                         */
        /* non-NULL nonTrunkPortsPtr [0xFF, 0x00].                                              */
        /* Expected: NON GT_OK.                                                                 */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low-Level Mode

GT_STATUS cpssDxChTrunkDesignatedPortsEntrySet
(
    IN  GT_U8               devNum,
    IN  GT_U32              entryIndex,
    IN  CPSS_PORTS_BMP_STC  *designatedPortsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkDesignatedPortsEntrySet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call with entryIndex [3], non-NULL designatedPortsPtr [0xFF, 0x00]. Expected: GT_OK.
1.2. Call cpssDxChTrunkDesignatedPortsEntryGet with entryIndex [3], non-NULL designatedPortsPtr. Expected: GT_OK and the same port bitmap as was written.
1.3. Call with out of range entryIndex [8], non-NULL designatedPortsPtr [0xFF, 0x00]. Expected: NON GT_OK.
1.4. Check for NULL handling. Call with entryIndex [3], designatedPortsPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_U32              entryIndex;
    CPSS_PORTS_BMP_STC  designatedPorts;
    GT_U8               port;
    CPSS_PORTS_BMP_STC  designatedPortsWritten;


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    cpssOsBzero((GT_VOID*)&designatedPorts, sizeof(designatedPorts));
    for (port = 0; port < CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS; port++)
    {
        CPSS_PORTS_BMP_PORT_SET_MAC(&designatedPorts, port);
    }

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with entryIndex [3], non-NULL designatedPortsPtr [0xFF, 0x00].                             */
        /* Expected: GT_OK.                                                                                     */
        entryIndex = 3;

        st = cpssDxChTrunkDesignatedPortsEntrySet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryIndex, designatedPorts.ports[0], designatedPorts.ports[1]);

        /* 1.2. Call cpssDxChTrunkDesignatedPortsEntryGet with entryIndex [3], non-NULL designatedPortsPtr.     */
        /* Expected: GT_OK and the same port bitmap as was written.                                             */
        st = cpssDxChTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPortsWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTrunkDesignatedPortsEntryGet: %d, %d", dev, entryIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(designatedPorts.ports[0], designatedPortsWritten.ports[0],
                   "get another designatedPortsPtr->ports[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(designatedPorts.ports[1], designatedPortsWritten.ports[1],
                   "get another designatedPortsPtr->ports[1] than was set: %d", dev);

        /* 1.3. Call with out of range entryIndex [8], non-NULL designatedPortsPtr [0xFF, 0x00].               */
        /* Expected: NON GT_OK.                                                                                 */
        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            entryIndex = 8;
        }
        else
        {
            entryIndex = 64;
        }

        st = cpssDxChTrunkDesignatedPortsEntrySet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /* 1.4. Check for NULL handling. Call with entryIndex [3], designatedPortsPtr [NULL].                   */
        /* Expected: GT_BAD_PTR.                                                                                */
        st = cpssDxChTrunkDesignatedPortsEntrySet(dev, entryIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, designatedPortsPtr = NULL", dev);
    }

    entryIndex = 3;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkDesignatedPortsEntrySet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkDesignatedPortsEntrySet(dev, entryIndex, &designatedPorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All Modes

GT_STATUS cpssDxChTrunkDesignatedPortsEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_U32              entryIndex,
    OUT  CPSS_PORTS_BMP_STC  *designatedPortsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkDesignatedPortsEntryGet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call with entryIndex [3], non-NULL designatedPortsPtr. Expected: GT_OK.
1.2. Call with out of range entryIndex [8], non-NULL designatedPortsPtr. Expected: NON GT_OK.
1.3. Check for NULL handling. Call with entryIndex [3], designatedPortsPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_U32              entryIndex;
    CPSS_PORTS_BMP_STC  designatedPorts;


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with entryIndex [3], non-NULL designatedPortsPtr.                          */
        /* Expected: GT_OK.                                                                     */
        entryIndex = 3;

        st = cpssDxChTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /* 1.2. Call with out of range entryIndex [8], non-NULL designatedPortsPtr.            */
        /* Expected: NON GT_OK.                                                                 */
        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            entryIndex = 8;
        }
        else
        {
            entryIndex = 64;
        }

        st = cpssDxChTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /* 1.3. Check for NULL handling. Call with entryIndex [3], designatedPortsPtr [NULL].   */
        /* Expected: GT_BAD_PTR.                                                                */
        st = cpssDxChTrunkDesignatedPortsEntryGet(dev, entryIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, designatedPortsPtr = NULL", dev);
    }

    entryIndex = 3;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssDxChTrunkDbEnabledMembersGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    INOUT GT_U32                *numOfEnabledMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   enabledMembersArray[]
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkDbEnabledMembersGet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call with trunkId [2], non-NULL numOfEnabledMembers, and allocated enabledMembersArray for 8 members. Expected: GT_OK.
1.2. Check for NULL handling. Call with trunkId [2], numOfEnabledMembers [NULL], and allocated enabledMembersArray. Expected: GT_BAD_PTR.
1.3. Call with null trunk id [NULL_TRUNK_ID_CNS = 0]. Expected: NON GT_OK.
1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128]. Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    GT_U32                  numOfEnabledMembers;
    CPSS_TRUNK_MEMBER_STC   enabledMembersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trunkId [2], non-NULL numOfEnabledMembers,                            */
        /* and allocated enabledMembersArray for 8 members.                                     */
        /* Expected: GT_OK.                                                                     */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnabledMembers, enabledMembersArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.2. Check for NULL handling. Call with trunkId [2], numOfEnabledMembers [NULL],     */
        /* and allocated enabledMembersArray.                                                   */
        /* Expected: GT_BAD_PTR.                                                                */
        st = cpssDxChTrunkDbEnabledMembersGet(dev, trunkId, NULL, enabledMembersArray);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfEnabledMembers = NULL", dev);

        /* 1.3. Call with null trunk id [NULL_TRUNK_ID_CNS = 0].                                */
        /* Expected: NON GT_OK.                                                                 */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnabledMembers, enabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128].                         */
        /* Expected: NON GT_OK.                                                                 */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnabledMembers, enabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnabledMembers, enabledMembersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnabledMembers, enabledMembersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssDxChTrunkDbDisabledMembersGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    INOUT GT_U32                *numOfDisabledMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   disabledMembersArray[]
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkDbDisabledMembersGet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call with trunkId [2], non-NULL numOfDisabledMembersPtr, and allocated disabledMembersArray for 8 members. Expected: GT_OK.
1.2. Check for NULL handling. Call with trunkId [2], numOfDisabledMembersPtr [NULL], and allocated disabledMembersArray. Expected: GT_BAD_PTR.
1.3. Call with null trunk id [NULL_TRUNK_ID_CNS = 0]. Expected: NON GT_OK.
1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128]. Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    GT_U32                  numOfDisabledMembers;
    CPSS_TRUNK_MEMBER_STC   disabledMembersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trunkId [2], non-NULL numOfDisabledMembersPtr,                            */
        /* and allocated disabledMembersArray for 8 members.                                        */
        /* Expected: GT_OK.                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.2. Check for NULL handling. Call with trunkId [2], numOfDisabledMembersPtr [NULL],     */
        /* and allocated disabledMembersArray.                                                      */
        /* Expected: GT_BAD_PTR.                                                                    */
        st = cpssDxChTrunkDbDisabledMembersGet(dev, trunkId, NULL, disabledMembersArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, numOfDisabledMembersPtr = NULL", dev, trunkId);

        /* 1.3. Call with null trunk id [NULL_TRUNK_ID_CNS = 0].                                    */
        /* Expected: NON GT_OK.                                                                     */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128].                             */
        /* Expected: NON GT_OK.                                                                     */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisabledMembers, disabledMembersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssDxChTrunkDbIsMemberOfTrunk
(
    IN  GT_U8                   devNum,
    IN  CPSS_TRUNK_MEMBER_STC   *memberPtr,
    OUT GT_TRUNK_ID             *trunkIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkDbIsMemberOfTrunk)
{
/*
ITERATE_DEVICES (DxChx)
1.1. At first add some member to trunk. Call cpssDxChTrunkMemberAdd with trunkId [2], and non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.2. Call with non-NULL memberPtr {port [8], device [devNum]}., non-NULL trunkIdPtr. Expected: GT_OK and trunkIdPtr [2].
1.3. Accordingly to header trunkIdPtr may be NULL. Call with non-NULL memberPtr {port [8], device [devNum]}., trunkIdPtr [NULL]. Expected: GT_OK.
1.4. Remove member from trunk. Call cpssDxChTrunkMemberRemove with trunkId [2], non-NULL memberPtr {port [8], device [devNum]}. Expected: GT_OK.
1.5. Try to find already removed member. Call with non-NULL memberPtr {port [8], device [devNum]}, non-NULL trunkIdPtr. Expected: NOT GT_OK.
1.6. Check for NULL handling. Call with memberPtr [NULL], non-NULL trunkIdPtr. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_TRUNK_MEMBER_STC   member;
    GT_TRUNK_ID             trunkId;
    GT_U8                   hwDevNum;


    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_TRUNK_ID     trunkIdWritten;

        /* 1.1. At first add some member to trunk.                                                                  */
        /* Call cpssDxChTrunkMemberAdd with trunkId [2], and non-NULL memberPtr [tested dev id, virtual port 8].    */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(dev);

        member.device = hwDevNum;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChTrunkMemberAdd: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.2. Call with non-NULL memberPtr [tested dev id, virtual port 8], non-NULL trunkIdPtr.                  */
        /* Expected: GT_OK and trunkIdPtr [2].                                                                      */
        trunkIdWritten = 0;

        st = cpssDxChTrunkDbIsMemberOfTrunk(dev, &member, &trunkIdWritten);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, member.device, member.port);

        UTF_VERIFY_EQUAL1_STRING_MAC(trunkId, trunkIdWritten,
                   "get another trunkIdPtr than was set: %d", dev);

        /* 1.3. Accordingly to header trunkIdPtr may be NULL.                                                       */
        /* Call with non-NULL memberPtr [tested dev id, virtual port 8], trunkIdPtr [NULL].                         */
        /* Expected: GT_OK.                                                                                         */
        st = cpssDxChTrunkDbIsMemberOfTrunk(dev, &member, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, trunkIdPtr = NULL", dev);

        /* 1.4. Remove member from trunk.                                                                           */
        /* Call cpssDxChTrunkMemberRemove with trunkId [2], non-NULL memberPtr [tested dev id, virtual port 8].     */
        /* Expected: GT_OK.                                                                                         */
        st = cpssDxChTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChTrunkMemberRemove: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.5. Try to find already removed member.                                                                 */
        /* Call with non-NULL memberPtr [tested dev id, virtual port 8], non-NULL trunkIdPtr.                       */
        /* Expected: NON GT_OK.                                                                                     */
        trunkIdWritten = 0;

        st = cpssDxChTrunkDbIsMemberOfTrunk(dev, &member, &trunkIdWritten);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, member.device, member.port);

        /* 1.6. Check for NULL handling. Call with memberPtr [NULL], non-NULL trunkIdPtr.                           */
        /* Expected: GT_BAD_PTR.                                                                                    */
        st = cpssDxChTrunkDbIsMemberOfTrunk(dev, NULL, &trunkIdWritten);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, memberPtr = NULL", dev);
    }

    member.device = 0;
    member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkDbIsMemberOfTrunk(dev, &member, &trunkId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkDbIsMemberOfTrunk(dev, &member, &trunkId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssDxChTrunkDbTrunkTypeGet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    OUT CPSS_TRUNK_TYPE_ENT     *typePtr
);
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkDbTrunkTypeGet)
{
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_TRUNK_MEMBER_STC   member;
    GT_TRUNK_ID             trunkId;
    CPSS_TRUNK_TYPE_ENT     trunkType;
    GT_U8                   hwDevNum;
    CPSS_PORTS_BMP_STC      portsBmp;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /* 1.1. At first add some member to trunk.                                                                  */
        /* Call cpssDxChTrunkMemberAdd with trunkId [2], and non-NULL memberPtr [tested dev id, virtual port 8].    */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkDbTrunkTypeGet(dev,trunkId,&trunkType);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        if(trunkType == CPSS_TRUNK_TYPE_CASCADE_E)
        {
            st = cpssDxChTrunkCascadeTrunkPortsSet(dev, trunkId, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTrunkCascadeTrunkPortsSet: %d, %d ,NULL ",
                                         dev, trunkId );
        }
        else
        {
            /* this API should cause the trunk to be empty */
            st = cpssDxChTrunkMembersSet(dev, trunkId, 0,NULL,0,NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTrunkMembersSet: %d, %d ,0,NULL,0,NULL ",
                                         dev, trunkId );
        }

        st = cpssDxChTrunkDbTrunkTypeGet(dev,trunkId,&trunkType);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* trunk must be 'FREE' now */
        UTF_VERIFY_EQUAL0_PARAM_MAC(CPSS_TRUNK_TYPE_FREE_E, trunkType);

        hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(dev);

        member.device = hwDevNum;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChTrunkMemberAdd: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* trunk must be now 'Regular trunk'*/
        st = cpssDxChTrunkDbTrunkTypeGet(dev,trunkId,&trunkType);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        UTF_VERIFY_EQUAL0_PARAM_MAC(CPSS_TRUNK_TYPE_REGULAR_E, trunkType);

        /* 1.4. Remove member from trunk.                                                                           */
        /* Call cpssDxChTrunkMemberRemove with trunkId [2], non-NULL memberPtr [tested dev id, virtual port 8].     */
        /* Expected: GT_OK.                                                                                         */
        st = cpssDxChTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChTrunkMemberRemove: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* trunk must be now 'free' trunk */
        st = cpssDxChTrunkDbTrunkTypeGet(dev,trunkId,&trunkType);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        UTF_VERIFY_EQUAL0_PARAM_MAC(CPSS_TRUNK_TYPE_FREE_E, trunkType);

        /* set cascade trunk */
        portsBmp.ports[0] = portsBmp.ports[1] = 0;
        CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,TRUNK_TESTED_VIRT_PORT_ID_CNS);

        st = cpssDxChTrunkCascadeTrunkPortsSet(dev,trunkId, &portsBmp);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* trunk must be 'cascade' now */
        st = cpssDxChTrunkDbTrunkTypeGet(dev,trunkId,&trunkType);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        UTF_VERIFY_EQUAL0_PARAM_MAC(CPSS_TRUNK_TYPE_CASCADE_E, trunkType);

        st = cpssDxChTrunkCascadeTrunkPortsSet(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* trunk must be 'FREE' now */
        st = cpssDxChTrunkDbTrunkTypeGet(dev,trunkId,&trunkType);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        UTF_VERIFY_EQUAL0_PARAM_MAC(CPSS_TRUNK_TYPE_FREE_E, trunkType);

        /* Call with trunkIdPtr [NULL].                         */
        /* Expected: GT_BAD_PTR.                                                                                         */
        st = cpssDxChTrunkDbTrunkTypeGet(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkDbTrunkTypeGet(dev, trunkId, &trunkType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkDbTrunkTypeGet(dev, trunkId, &trunkType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/* --- different from ExMx */

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashDesignatedTableModeSet
(
    IN GT_U8   devNum,
    IN GT_BOOL useEntireTable
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashDesignatedTableModeSet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with mode [see CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT]. Expected: GT_OK.
1.2. Call cpssDxChTrunkHashDesignatedTableModeGet with non-NULL modePtr. Expected: GT_OK and same value as written.
*/
    GT_STATUS st              = GT_OK;
    GT_U8       dev;
    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT     modes[] ={
        CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_FIRST_INDEX_E,
        CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E,
        CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E,
        CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E};
    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT     modeRet,tmpMode = 0;
    GT_U32  numOfModes = sizeof(modes)/sizeof(modes[0]);
    GT_U32  ii;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        for(ii = 0 ; ii < numOfModes ; ii++)
        {
            /* 1.1. Call function with valid mode
               Expected: GT_OK.   */

            st = cpssDxChTrunkHashDesignatedTableModeSet(dev, modes[ii]);
            if(modes[ii] == CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E)
            {
                if(0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(dev))
                {
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, modes[ii]);
                    continue;
                }
                else
                {
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, modes[ii]);
                }
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, modes[ii]);
            }

            /* 1.2. Call cpssDxChTrunkHashDesignatedTableModeGet with non-NULL
               useEntireTablePtr. Expected: GT_OK and same value as written.    */

            st = cpssDxChTrunkHashDesignatedTableModeGet(dev, &modeRet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashDesignatedTableModeGet: %d", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL3_STRING_MAC(modeRet, modes[ii] ,
                       "in dev [%d] get another modeRet[%d] than was set[%d]",
                       dev ,modeRet, modes[ii]);
        }

        UTF_ENUMS_CHECK_MAC(cpssDxChTrunkHashDesignatedTableModeSet(dev, tmpMode),
                                                    tmpMode);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    tmpMode = CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E;
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashDesignatedTableModeSet(dev, tmpMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashDesignatedTableModeSet(dev, tmpMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashDesignatedTableModeGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *useEntireTablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashDesignatedTableModeGet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with non-NULL useEntireTablePtr. Expected: GT_OK.
1.2. Call function with useEntireTablePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st           = GT_OK;

    GT_U8     dev;
    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT     mode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL useEntireTablePtr.
           Expected: GT_OK. */

        st = cpssDxChTrunkHashDesignatedTableModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with useEntireTablePtr [NULL].
           Expected: GT_BAD_PTR.    */

        st = cpssDxChTrunkHashDesignatedTableModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, useEntireTablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashDesignatedTableModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashDesignatedTableModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashGlobalModeSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  hashMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashGlobalModeSet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with hashMode [CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E / CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E]. Expected: GT_OK.
1.2. Call cpssDxChTrunkHashGlobalModeGet with non-NULL hashModePtr. Expected: GT_OK and same value as written.
1.3. Call function with hashMode [wrong enum values]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st              = GT_OK;

    GT_U8       dev;
    CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  hashModes[] = {
                        CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E,
                        CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E,
                        CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E};
    CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  hashMode = CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E;
    CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  hashModeRet = CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E;
    GT_U32  numOfModes = sizeof(hashModes) / sizeof(hashModes[0]);
    GT_U32  ii;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with hashMode [CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E
           / CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E]. Expected: GT_OK.  */

        for(ii = 0 ; ii < numOfModes; ii++)
        {
            hashMode = hashModes[ii];

            st = cpssDxChTrunkHashGlobalModeSet(dev, hashMode);

            if(hashMode == CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E)
            {
                if(PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.featureInfo.trunkCrcHashSupported == GT_FALSE)
                {
                    /* the device nit support this option */
                    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, hashMode);
                    continue;
                }
            }

            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

            /* 1.2. Call cpssDxChTrunkHashGlobalModeGet with non-NULL
               hashModePtr. Expected: GT_OK and same value as written.  */

            st = cpssDxChTrunkHashGlobalModeGet(dev, &hashModeRet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashGlobalModeGet: %d", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(hashMode, hashModeRet,
                       "get another hashMode than was set: %d", dev);
        }

        /*
           1.3. Call function with hashMode [wrong enum values].
           Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTrunkHashGlobalModeSet
                            (dev, hashMode),
                            hashMode);
    }

    hashMode = CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashGlobalModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashGlobalModeSet(dev, hashMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashGlobalModeGet
(
    IN GT_U8    devNum,
    OUT CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  *hashModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashGlobalModeGet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with non-NULL hashModePtr. Expected: GT_OK.
1.2. Call function with hashModePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st           = GT_OK;

    GT_U8     dev;
    CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  hashMode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL hashModePtr.
           Expected: GT_OK. */

        st = cpssDxChTrunkHashGlobalModeGet(dev, &hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with hashModePtr [NULL].
           Expected: GT_BAD_PTR.    */

        st = cpssDxChTrunkHashGlobalModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, hashModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashGlobalModeGet(dev, &hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashGlobalModeGet(dev, &hashMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashIpAddMacModeSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashIpAddMacModeSet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with enable [GT_FALSE / GT_TRUE]. Expected: GT_OK.
1.2. Call cpssDxChTrunkHashIpAddMacModeGet with non-NULL enablePtr. Expected: GT_OK and same value as written.
*/
    GT_STATUS st              = GT_OK;

    GT_U8       dev;
    GT_BOOL     enable;
    GT_BOOL     enableRet;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable
           [GT_FALSE / GT_TRUE]. Expected: GT_OK.   */

        enable = GT_FALSE;

        st = cpssDxChTrunkHashIpAddMacModeSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_TRUE;

        st = cpssDxChTrunkHashIpAddMacModeSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* 1.2. Call cpssDxChTrunkHashIpAddMacModeGet with non-NULL
           enablePtr. Expected: GT_OK and same value as written.    */

        st = cpssDxChTrunkHashIpAddMacModeGet(dev, &enableRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashIpAddMacModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableRet,
                   "get another enable than was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashIpAddMacModeSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashIpAddMacModeSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashIpAddMacModeGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashIpAddMacModeGet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with non-NULL enablePtr. Expected: GT_OK.
1.2. Call function with enablePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st           = GT_OK;

    GT_U8     dev;
    GT_BOOL   enable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL enablePtr.
           Expected: GT_OK. */

        st = cpssDxChTrunkHashIpAddMacModeGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with enablePtr [NULL].
           Expected: GT_BAD_PTR.    */

        st = cpssDxChTrunkHashIpAddMacModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashIpAddMacModeGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashIpAddMacModeGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashIpModeSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashIpModeSet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with enable [GT_FALSE / GT_TRUE]. Expected: GT_OK.
1.2. Call cpssDxChTrunkHashIpModeGet with non-NULL enablePtr. Expected: GT_OK and same value as written.
*/
    GT_STATUS st              = GT_OK;

    GT_U8       dev;
    GT_BOOL     enable;
    GT_BOOL     enableRet;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable
           [GT_FALSE / GT_TRUE]. Expected: GT_OK.   */

        enable = GT_FALSE;

        st = cpssDxChTrunkHashIpModeSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_TRUE;

        st = cpssDxChTrunkHashIpModeSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* 1.2. Call cpssDxChTrunkHashIpModeGet with non-NULL
           enablePtr. Expected: GT_OK and same value as written.    */

        st = cpssDxChTrunkHashIpModeGet(dev, &enableRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashIpModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableRet,
                   "get another enable than was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashIpModeSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashIpModeSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashIpModeGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashIpModeGet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with non-NULL enablePtr. Expected: GT_OK.
1.2. Call function with enablePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st           = GT_OK;

    GT_U8     dev;
    GT_BOOL   enable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL enablePtr.
           Expected: GT_OK. */

        st = cpssDxChTrunkHashIpModeGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with enablePtr [NULL].
           Expected: GT_BAD_PTR.    */

        st = cpssDxChTrunkHashIpModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashIpModeGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashIpModeGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashL4ModeSet
(
    IN GT_U8   devNum,
    IN CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT hashMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashL4ModeSet)
{
/*
ITERATE_DEVICES (DxChx)
    1.1. Call function with hashMode [CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E /
                                      CPSS_DXCH_TRUNK_L4_LBH_SHORT_E].
    Expected: GT_OK.
    1.2. Call cpssDxChTrunkHashL4ModeGet with non-NULL hashModePtr.
    Expected: GT_OK and same value as written.
    1.3. Call function with hashMode [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st              = GT_OK;

    GT_U8       dev;
    CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT  hashMode = CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E;
    CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT  hashModeRet;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with hashMode [CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E /
                                              CPSS_DXCH_TRUNK_L4_LBH_SHORT_E].
            Expected: GT_OK.
        */
        hashMode = CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E;

        st = cpssDxChTrunkHashL4ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

        /* 1.2. Call cpssDxChTrunkHashL4ModeGet with non-NULL
           hashModePtr. Expected: GT_OK and same value as written.  */

        st = cpssDxChTrunkHashL4ModeGet(dev, &hashModeRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashL4ModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hashMode, hashModeRet,
                   "get another hashMode than was set: %d", dev);


        hashMode = CPSS_DXCH_TRUNK_L4_LBH_SHORT_E;

        st = cpssDxChTrunkHashL4ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

        /* 1.2. Call cpssDxChTrunkHashL4ModeGet with non-NULL
           hashModePtr. Expected: GT_OK and same value as written.  */

        st = cpssDxChTrunkHashL4ModeGet(dev, &hashModeRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashL4ModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hashMode, hashModeRet,
                   "get another hashMode than was set: %d", dev);

        /*
           1.3. Call function with hashMode [wrong enum values].
           Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTrunkHashL4ModeSet
                            (dev, hashMode),
                            hashMode);
    }

    hashMode = CPSS_DXCH_TRUNK_L4_LBH_SHORT_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashL4ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashL4ModeSet(dev, hashMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashL4ModeGet
(
    IN GT_U8   devNum,
    OUT CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT *hashModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashL4ModeGet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with non-NULL hashModePtr. Expected: GT_OK.
1.2. Call function with hashModePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st           = GT_OK;

    GT_U8     dev;
    CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT  hashMode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL hashModePtr.
           Expected: GT_OK. */

        st = cpssDxChTrunkHashL4ModeGet(dev, &hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with hashModePtr [NULL].
           Expected: GT_BAD_PTR.    */

        st = cpssDxChTrunkHashL4ModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, hashModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashL4ModeGet(dev, &hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashL4ModeGet(dev, &hashMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashIpv6ModeSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT   hashMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashIpv6ModeSet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with hashMode [CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_FLOW_E / CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_E]. Expected: GT_OK.
1.2. Call cpssDxChTrunkHashIpv6ModeGet with non-NULL hashModePtr. Expected: GT_OK and same value as written.
1.3. Call function with hashMode [wrong enum values]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st              = GT_OK;

    GT_U8       dev;
    CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT  hashMode;
    CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT  hashModeRet;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with hashMode [CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_FLOW_E
           / CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_E]. Expected: GT_OK.  */

        hashMode = CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_FLOW_E;

        st = cpssDxChTrunkHashIpv6ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

        hashMode = CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_E;

        st = cpssDxChTrunkHashIpv6ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hashMode);

        /* 1.2. Call cpssDxChTrunkHashIpv6ModeGet with non-NULL
           hashModePtr. Expected: GT_OK and same value as written.  */

        st = cpssDxChTrunkHashIpv6ModeGet(dev, &hashModeRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashIpv6ModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hashMode, hashModeRet,
                   "get another hashMode than was set: %d", dev);

        /*
           1.3. Call function with hashMode [wrong enum values].
           Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTrunkHashIpv6ModeSet
                            (dev, hashMode),
                            hashMode);
    }

    hashMode = CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashIpv6ModeSet(dev, hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashIpv6ModeSet(dev, hashMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashIpv6ModeGet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT   *hashModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashIpv6ModeGet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with non-NULL hashModePtr. Expected: GT_OK.
1.2. Call function with hashModePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st           = GT_OK;

    GT_U8     dev;
    CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT  hashMode;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL hashModePtr.
           Expected: GT_OK. */

        st = cpssDxChTrunkHashIpv6ModeGet(dev, &hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with hashModePtr [NULL].
           Expected: GT_BAD_PTR.    */

        st = cpssDxChTrunkHashIpv6ModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, hashModePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashIpv6ModeGet(dev, &hashMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashIpv6ModeGet(dev, &hashMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashIpShiftSet
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_BOOL                      isSrcIp,
    IN GT_U32                       shiftValue
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashIpShiftSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                           CPSS_IP_PROTOCOL_IPV6_E /
                                           CPSS_IP_PROTOCOL_IPV4V6_E],
                                  isSrcIp [GT_TRUE / GT_FALSE]
                           and shiftValue [0 / 3 / 15].
    Expected: GT_OK.
    1.2. Call cpssDxChTrunkHashIpShiftGet with the same parameters.
    Expected: GT_OK and the same value.
    1.3. Call with wrong protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                               isSrcIp [GT_TRUE],
                        and shiftValue [0].
    Expected: GT_BAD_PARAM.
    1.4. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E],
                         isSrcIp [GT_TRUE],
            and wrong shiftValue [4].
    Expected: NOT GT_OK.
    1.5. Call protocolStack [CPSS_IP_PROTOCOL_IPV6_E],
                         isSrcIp [GT_TRUE],
            and wrong shiftValue [16].
    Expected: NOT GT_OK.
    1.6. Call with wrong enum values protocolStack ,
                               isSrcIp [GT_TRUE],
                        and shiftValue [0].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                      dev;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                    isSrcIp       = GT_TRUE;
    GT_U32                     shiftValue    = 0;
    GT_U32                     shiftValueGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                   CPSS_IP_PROTOCOL_IPV6_E],
                                          isSrcIp [GT_TRUE / GT_FALSE],
                                   and shiftValue [0 / 3 / 15].
            Expected: GT_OK.
        */

        /* call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E],
         * isSrcIp [GT_TRUE], shiftValue [0]*/

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        isSrcIp       = GT_TRUE;
        shiftValue    = 0;

        st = cpssDxChTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChTrunkHashIpShiftGet with the same parameters.
            Expected: GT_OK and the same value.
        */
        st = cpssDxChTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashIpShiftGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(shiftValue, shiftValueGet,
                                     "get another shiftValue than was set: %d", dev);


        /* call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E],
         * isSrcIp [GT_FALSE], shiftValue [3]*/
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        isSrcIp       = GT_FALSE;
        shiftValue    = 3;

        st = cpssDxChTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChTrunkHashIpShiftGet with the same parameters.
            Expected: GT_OK and the same value.
        */
        st = cpssDxChTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashIpShiftGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(shiftValue, shiftValueGet,
                                     "get another shiftValue than was set: %d", dev);


        /* call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E],
         * isSrcIp [GT_FALSE], shiftValue [15]*/
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        shiftValue    = 15;

        st = cpssDxChTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChTrunkHashIpShiftGet with the same parameters.
            Expected: GT_OK and the same value.
        */
        st = cpssDxChTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashIpShiftGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(shiftValue, shiftValueGet,
                                     "get another shiftValue than was set: %d", dev);

        /*
            1.3. Call with wrong protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                       isSrcIp [GT_TRUE],
                                and shiftValue [0].
            Expected: GT_BAD_PARAM.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
        shiftValue    = 0;

        st = cpssDxChTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /*
            1.4. Call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E],
                                 isSrcIp [GT_TRUE],
                    and wrong shiftValue [4].
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        shiftValue    = 4;

        st = cpssDxChTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.5. Call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E],
                                 isSrcIp [GT_TRUE],
                    and wrong shiftValue [16].
            Expected: NOT GT_OK.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        shiftValue    = 16;

        st = cpssDxChTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
        UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.6. Call with wrong enum values protocolStack, isSrcIp [GT_TRUE],
                    and wrong shiftValue [0].
            Expected: GT_BAD_PARAM.
        */
        shiftValue = 0;

        UTF_ENUMS_CHECK_MAC(cpssDxChTrunkHashIpShiftSet
                            (dev, protocolStack, isSrcIp, shiftValue),
                            protocolStack);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashIpShiftSet(dev, protocolStack, isSrcIp, shiftValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashIpShiftGet
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_BOOL                      isSrcIp,
    OUT GT_U32                      *shiftValuePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashIpShiftGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                           CPSS_IP_PROTOCOL_IPV6_E /
                                           CPSS_IP_PROTOCOL_IPV4V6_E],
                                  isSrcIp [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call with wrong protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                  isSrcIp [GT_TRUE].
    Expected: GT_BAD_PARAM.
    1.3. Call with wrong enum values protocolStack ,
                               isSrcIp [GT_TRUE].
    Expected: GT_BAD_PARAM.
    1.4. Call with wrong shiftValuePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                      dev;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                    isSrcIp = GT_TRUE;
    GT_U32                     shiftValueGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with protocolStack [CPSS_IP_PROTOCOL_IPV4_E /
                                                   CPSS_IP_PROTOCOL_IPV6_E],
                                          isSrcIp [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* call with protocolStack [CPSS_IP_PROTOCOL_IPV4_E], isSrcIp [GT_TRUE]*/

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        isSrcIp       = GT_TRUE;

        st = cpssDxChTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValueGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* call with protocolStack [CPSS_IP_PROTOCOL_IPV6_E], isSrcIp [GT_FALSE]*/
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        isSrcIp       = GT_FALSE;

        st = cpssDxChTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValueGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong protocolStack [CPSS_IP_PROTOCOL_IPV4V6_E],
                                       isSrcIp [GT_TRUE].
            Expected: GT_BAD_PARAM.
        */
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
        isSrcIp       = GT_TRUE;

        st = cpssDxChTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValueGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /*
            1.3. Call with wrong enum values protocolStack, isSrcIp [GT_TRUE].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTrunkHashIpShiftGet
                            (dev, protocolStack, isSrcIp, &shiftValueGet),
                            protocolStack);

        /*
            1.4. Call with wrong shiftValuePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);

    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValueGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashIpShiftGet(dev, protocolStack, isSrcIp, &shiftValueGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashMaskSet
(
    IN GT_U8                          devNum,
    IN CPSS_DXCH_TRUNK_LBH_MASK_ENT   maskedField,
    IN GT_U8                          maskValue
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashMaskSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E /
                                         CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL0_E /
                                         CPSS_DXCH_TRUNK_LBH_MASK_IPV4_DIP_E /
                                         CPSS_DXCH_TRUNK_LBH_MASK_IPV6_FLOW_E /
                                         CPSS_DXCH_TRUNK_LBH_MASK_L4_SRC_PORT_E],
                              maskValue [0 / 0x2F /0x3F].
    Expected: GT_OK.
    1.2. Call cpssDxChTrunkHashMaskGet with the same parameters.
    Expected: GT_OK and the same value.
    1.3. Call with wrong enum values maskedField ,
                           maskValue [0].
    Expected: GT_BAD_PARAM.
    1.4. Call with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E],
          and wrong  maskValue [0x4F].
    Expected: GT_OUT_OF_RANGE.
*/
    GT_STATUS st = GT_OK;

    GT_U8                          dev;
    CPSS_DXCH_TRUNK_LBH_MASK_ENT   maskedField = CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E;
    GT_U8                          maskValue = 0;
    GT_U8                          maskValueGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E /
                                                 CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL0_E /
                                                 CPSS_DXCH_TRUNK_LBH_MASK_IPV4_DIP_E /
                                                 CPSS_DXCH_TRUNK_LBH_MASK_IPV6_FLOW_E /
                                                 CPSS_DXCH_TRUNK_LBH_MASK_L4_SRC_PORT_E],
                                      maskValue [0 / 0x2F /0x3F].
            Expected: GT_OK.
        */

        /* call with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E], maskValue [0]*/

        maskedField = CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E;
        maskValue   = 0;

        st = cpssDxChTrunkHashMaskSet(dev, maskedField, maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChTrunkHashMaskGet with the same parameters.
            Expected: GT_OK and the same value.
        */
        st = cpssDxChTrunkHashMaskGet(dev, maskedField, &maskValueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashMaskGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maskValue, maskValueGet,
                                     "get another maskValue than was set: %d", dev);


        /* call with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL0_E], maskValue [0x2F]*/
        maskedField = CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL0_E;
        maskValue   = 0x2F;

        st = cpssDxChTrunkHashMaskSet(dev, maskedField, maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChTrunkHashMaskGet with the same parameters.
            Expected: GT_OK and the same value.
        */
        st = cpssDxChTrunkHashMaskGet(dev, maskedField, &maskValueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashMaskGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maskValue, maskValueGet,
                                     "get another maskValue than was set: %d", dev);


        /* call with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_IPV4_DIP_E], maskValue [0x3F]*/
        maskedField = CPSS_DXCH_TRUNK_LBH_MASK_IPV4_DIP_E;
        maskValue   = 0x3F;

        st = cpssDxChTrunkHashMaskSet(dev, maskedField, maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChTrunkHashMaskGet with the same parameters.
            Expected: GT_OK and the same value.
        */
        st = cpssDxChTrunkHashMaskGet(dev, maskedField, &maskValueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashMaskGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maskValue, maskValueGet,
                                     "get another maskValue than was set: %d", dev);


        /* call with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_IPV6_FLOW_E], maskValue [0x3F]*/
        maskedField = CPSS_DXCH_TRUNK_LBH_MASK_IPV6_FLOW_E;

        st = cpssDxChTrunkHashMaskSet(dev, maskedField, maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChTrunkHashMaskGet with the same parameters.
            Expected: GT_OK and the same value.
        */
        st = cpssDxChTrunkHashMaskGet(dev, maskedField, &maskValueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashMaskGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maskValue, maskValueGet,
                                     "get another maskValue than was set: %d", dev);


        /* call with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_L4_SRC_PORT_E], maskValue [0x3F]*/
        maskedField = CPSS_DXCH_TRUNK_LBH_MASK_L4_SRC_PORT_E;

        st = cpssDxChTrunkHashMaskSet(dev, maskedField, maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssDxChTrunkHashMaskGet with the same parameters.
            Expected: GT_OK and the same value.
        */
        st = cpssDxChTrunkHashMaskGet(dev, maskedField, &maskValueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashMaskGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(maskValue, maskValueGet,
                                     "get another maskValue than was set: %d", dev);

        /*
            1.3. Call with wrong enum values maskedField, maskValue [0].
            Expected: GT_BAD_PARAM.
        */
        maskValue   = 0;

        UTF_ENUMS_CHECK_MAC(cpssDxChTrunkHashMaskSet
                            (dev, maskedField, maskValue),
                            maskedField);

        /*
            1.4. Call with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E],
                  and wrong  maskValue [0x4F].
            Expected: GT_OUT_OF_RANGE.
        */
        maskedField = CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E;
        maskValue   = 0x4F;

        st = cpssDxChTrunkHashMaskSet(dev, maskedField, maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OUT_OF_RANGE, st, dev);

        maskValue   = 0;
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashMaskSet(dev, maskedField, maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashMaskSet(dev, maskedField, maskValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashMaskGet
(
    IN GT_U8                          devNum,
    IN CPSS_DXCH_TRUNK_LBH_MASK_ENT   maskedField,
    OUT GT_U8                         *maskValuePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashMaskGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E /
                                         CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL0_E /
                                         CPSS_DXCH_TRUNK_LBH_MASK_IPV4_DIP_E /
                                         CPSS_DXCH_TRUNK_LBH_MASK_IPV6_FLOW_E /
                                         CPSS_DXCH_TRUNK_LBH_MASK_L4_SRC_PORT_E].
    Expected: GT_OK.
    1.2. Call with wrong enum values maskedField.
    Expected: GT_BAD_PARAM.
    1.4. Call with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E],
         and wrong  maskValuePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                          dev;
    CPSS_DXCH_TRUNK_LBH_MASK_ENT   maskedField = CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E;
    GT_U8                          maskValue;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E /
                                                 CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL0_E /
                                                 CPSS_DXCH_TRUNK_LBH_MASK_IPV4_DIP_E /
                                                 CPSS_DXCH_TRUNK_LBH_MASK_IPV6_FLOW_E /
                                                 CPSS_DXCH_TRUNK_LBH_MASK_L4_SRC_PORT_E].
            Expected: GT_OK.
        */

        /* call with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E]*/

        maskedField = CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E;

        st = cpssDxChTrunkHashMaskGet(dev, maskedField, &maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);


        /* call with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL0_E]*/
        maskedField = CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL0_E;

        st = cpssDxChTrunkHashMaskGet(dev, maskedField, &maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);


        /* call with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_IPV4_DIP_E]*/
        maskedField = CPSS_DXCH_TRUNK_LBH_MASK_IPV4_DIP_E;

        st = cpssDxChTrunkHashMaskGet(dev, maskedField, &maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);


        /* call with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_IPV6_FLOW_E]*/
        maskedField = CPSS_DXCH_TRUNK_LBH_MASK_IPV6_FLOW_E;

        st = cpssDxChTrunkHashMaskGet(dev, maskedField, &maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);


        /* call with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_L4_SRC_PORT_E]*/
        maskedField = CPSS_DXCH_TRUNK_LBH_MASK_L4_SRC_PORT_E;

        st = cpssDxChTrunkHashMaskGet(dev, maskedField, &maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with wrong enum values maskedField.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTrunkHashMaskGet
                            (dev, maskedField, &maskValue),
                            maskedField);

        /*
            1.3. Call with maskedField [CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E],
                  and wrong  maskValue [NULL].
            Expected: GT_BAD_PTR.
        */
        maskedField = CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E;

        st = cpssDxChTrunkHashMaskGet(dev, maskedField, NULL);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PTR, st, dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashMaskGet(dev, maskedField, &maskValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashMaskGet(dev, maskedField, &maskValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashMplsModeEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashMplsModeEnableSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChTrunkHashMplsModeEnableGet with non-NULL enablePtr.
    Expected: GT_OK and same value as written.
*/
    GT_STATUS st              = GT_OK;

    GT_U8       dev;
    GT_BOOL     enable;
    GT_BOOL     enableRet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable
           [GT_FALSE / GT_TRUE]. Expected: GT_OK.   */

        enable = GT_FALSE;

        st = cpssDxChTrunkHashMplsModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* 1.2. Call cpssDxChTrunkHashMplsModeEnableGet with non-NULL
           enablePtr. Expected: GT_OK and same value as written.    */

        st = cpssDxChTrunkHashMplsModeEnableGet(dev, &enableRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashMplsModeEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableRet,
                   "get another enable than was set: %d", dev);


        enable = GT_TRUE;

        st = cpssDxChTrunkHashMplsModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* 1.2. Call cpssDxChTrunkHashMplsModeEnableGet with non-NULL
           enablePtr. Expected: GT_OK and same value as written.    */

        st = cpssDxChTrunkHashMplsModeEnableGet(dev, &enableRet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkHashMplsModeEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableRet,
                   "get another enable than was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashMplsModeEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashMplsModeEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashMplsModeEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashMplsModeEnableGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call function with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev;
    GT_BOOL   enable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL enablePtr.
           Expected: GT_OK. */

        st = cpssDxChTrunkHashMplsModeEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with enablePtr [NULL].
           Expected: GT_BAD_PTR.    */

        st = cpssDxChTrunkHashMplsModeEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashMplsModeEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashMplsModeEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkInit
(
    IN  GT_U8       devNum,
    IN  GT_U8       maxNumberOfTrunks,
    IN  CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT trunkMembersMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkInit)
{
/*
    NOTE: the trunk was already initialized during the cpssInitSystem.
        with the max number of trunks that the device supports.
        So the cpss API of cpssDxChTrunkInit should not allow to call the API
        again unless the maxNumberOfTrunks has same value as given during
        cpssInitSystem.

        the DXCH devices supports different number of trunks , so the test
        should loop on all number of trunks possible 0..127 and check that only
        one of then return GT_OK , while all others return GT_BAD_STATE


    ITERATE_DEVICES (DxChx)
    1.1. Call function with maxNumberOfTrunks [1],
        and trunkMembersMode [CPSS_DXCH_TRUNK_MEMBERS_MODE_NATIVE_E /
                              CPSS_DXCH_TRUNK_MEMBERS_MODE_SOHO_EMULATION_E].
    Expected: see note above.
    1.2. Call function with wrong maxNumberOfTrunks [BIT_8].
    Expected: see note above.
    1.3. Call function with wrong enum values trunkMembersMode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8     dev;
    GT_U8     maxNumberOfTrunks = 1;
    CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT trunkMembersMode = 0;
    GT_U32  numberOfOk;
    GT_STATUS lastSt;
    GT_U32      ii;
    GT_U32      origInitNumberOfTrunks;/* the number of trunks that device was initialized with (during cpssInitSystem) */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with maxNumberOfTrunks [1],
                 and trunkMembersMode [CPSS_DXCH_TRUNK_MEMBERS_MODE_NATIVE_E /
                                       CPSS_DXCH_TRUNK_MEMBERS_MODE_SOHO_EMULATION_E].
            Expected: GT_OK.
        */

        numberOfOk = 0;
        origInitNumberOfTrunks = 0;

        for(ii = 0 ; ii < BIT_7; ii++)
        {
            maxNumberOfTrunks = (GT_U8)ii;

            trunkMembersMode = CPSS_DXCH_TRUNK_MEMBERS_MODE_NATIVE_E;

            st = cpssDxChTrunkInit(dev, maxNumberOfTrunks, trunkMembersMode);
            if(st == GT_OK)
            {
                numberOfOk ++;
                origInitNumberOfTrunks = maxNumberOfTrunks;
            }
            else
            {
                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_STATE, st, dev);
            }

            lastSt = st;

            trunkMembersMode = CPSS_DXCH_TRUNK_MEMBERS_MODE_SOHO_EMULATION_E;

            st = cpssDxChTrunkInit(dev, maxNumberOfTrunks, trunkMembersMode);
            UTF_VERIFY_EQUAL1_PARAM_MAC(lastSt, st, dev);
        }

        if(numberOfOk != 1)
        {
            /* only single GT_OK should have been received */
            st = GT_BAD_STATE;
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        }

        /*
            1.2. Call function with wrong maxNumberOfTrunks [BIT_7].
            Expected: GT_BAD_STATE.
        */
        maxNumberOfTrunks = BIT_7;

        st = cpssDxChTrunkInit(dev, maxNumberOfTrunks, trunkMembersMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_STATE, st, dev);

        maxNumberOfTrunks = (GT_U8)origInitNumberOfTrunks;

        /*
            1.3. Call with wrong enum values trunkMembersMode.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTrunkInit
                            (dev, maxNumberOfTrunks, trunkMembersMode),
                            trunkMembersMode);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkInit(dev, maxNumberOfTrunks, trunkMembersMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkInit(dev, maxNumberOfTrunks, trunkMembersMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkDesignatedMemberSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_BOOL                  enable,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkDesignatedMemberSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with trunk id [2], non-NULL memberPtr {port [8], device [devNum]}.
    Expected: GT_OK.
    1.2. Call cpssDxChTrunkDbDesignatedMemberGet with the same trunkId.
    Expected: GT_OK and the same values.
    1.3. Try to add the same trunk member again, accordingly
    to header the result must be OK. Call with trunk id [2],
    non-NULL memberPtr {port [8], device [devNum]}.
    Expected: GT_OK.
    1.4. Try to add already added trunk members to another trunk id.
    Call with trunk id [3], non-NULL memberPtr {port [8], device [devNum]}.
    Expected: NON GT_OK.
    1.5. Try to add invalid device id and legal port id
    as trunk entry member. Call with trunk id [2], non-NULL memberPtr {port [8],
    device [PRV_CPSS_MAX_PP_DEVICES_CNS]}.
    Expected: NON GT_OK.
    1.6. Try to add legal device id and invalid port id as trunk entry member.
    Call with trunk id [2], non-NULL memberPtr {port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
    device [devNum]}.
    Expected: NON GT_OK.
    1.7. Check for NULL handling. Call with trunk id [2], memberPtr [NULL].
    Expected: GT_BAD_PTR.
    1.8. Call with null trunk id [NULL_TRUNK_ID_CNS = 0],
    non-NULL memberPtr [tested dev, virtual port 8].
    Expected: NON GT_OK.
    1.9. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],
    non-NULL memberPtr [tested dev, virtual port 8].
    Expected: NON GT_OK.
    1.10. Call cpssDxChTrunkMemberRemove with trunkId [2],
    memberPtr {port [8], device [devNum]} to remove added member from the trunk.
    Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    GT_BOOL                 enable = GT_FALSE;
    GT_BOOL                 enableGet = GT_TRUE;
    CPSS_TRUNK_MEMBER_STC   member;
    CPSS_TRUNK_MEMBER_STC   memberGet;
    GT_U8                   hwDevNum;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        enable = GT_FALSE;

        hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(dev);

        member.device = hwDevNum;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkDesignatedMemberSet(dev, trunkId, enable, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                     dev, trunkId, member.device, member.port);
        if(GT_OK == st)
        {
            /*
                1.2. Call cpssDxChTrunkDbDesignatedMemberGet with the same trunkId.
                Expected: GT_OK and the same 'enable' value.
            */
            st = cpssDxChTrunkDbDesignatedMemberGet(dev, trunkId, &enableGet, &memberGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                                   "get another enable than was set: %d, %d", dev, trunkId);
        }

        /*
            1.1. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        enable = GT_TRUE;

        member.device = hwDevNum;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkDesignatedMemberSet(dev, trunkId, enable, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                     dev, trunkId, member.device, member.port);
        if(GT_OK == st)
        {
            /*
                1.2. Call cpssDxChTrunkDbDesignatedMemberGet with the same trunkId.
                Expected: GT_OK and the same values.
            */
            st = cpssDxChTrunkDbDesignatedMemberGet(dev, trunkId, &enableGet, &memberGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                                   "get another enable than was set: %d, %d", dev, trunkId);
            UTF_VERIFY_EQUAL2_STRING_MAC(member.device, memberGet.device,
                                   "get another member.device than was set: %d, %d", dev, trunkId);
            UTF_VERIFY_EQUAL2_STRING_MAC(member.port, memberGet.port,
                                   "get another member.port than was set: %d, %d", dev, trunkId);
        }

        /*
            1.3. Try to add the same trunk member again, accordingly to header the result must be OK.
            Call with trunk id [2], non-NULL memberPtr [tested dev, virtual port 8].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        st = cpssDxChTrunkDesignatedMemberSet(dev, trunkId, enable, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                     dev, trunkId, member.device, member.port);
        if(GT_OK == st)
        {
            /*
                1.2. Call cpssDxChTrunkDbDesignatedMemberGet with the same trunkId.
                Expected: GT_OK and the same values.
            */
            st = cpssDxChTrunkDbDesignatedMemberGet(dev, trunkId, &enableGet, &memberGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                                   "get another enable than was set: %d, %d", dev, trunkId);
            UTF_VERIFY_EQUAL2_STRING_MAC(member.device, memberGet.device,
                                   "get another member.device than was set: %d, %d", dev, trunkId);
            UTF_VERIFY_EQUAL2_STRING_MAC(member.port, memberGet.port,
                                   "get another member.port than was set: %d, %d", dev, trunkId);
        }

        /*
            1.4. Try to add already added trunk members to another trunk id.
            Call with trunk id [3], non-NULL memberPtr [tested dev id, virtual port 8].
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS + 1;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        enable = GT_TRUE;

        st = cpssDxChTrunkDesignatedMemberSet(dev, trunkId, enable, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d, memberPtr->port = %d",
                                         dev, trunkId, member.device, member.port);
        if(GT_OK == st)
        {
            /*
                1.2. Call cpssDxChTrunkDbDesignatedMemberGet with the same trunkId.
                Expected: GT_OK and the same values.
            */
            st = cpssDxChTrunkDbDesignatedMemberGet(dev, trunkId, &enableGet, &memberGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                                   "get another enable than was set: %d, %d", dev, trunkId);
            UTF_VERIFY_EQUAL2_STRING_MAC(member.device, memberGet.device,
                                   "get another member.device than was set: %d, %d", dev, trunkId);
            UTF_VERIFY_EQUAL2_STRING_MAC(member.port, memberGet.port,
                                   "get another member.port than was set: %d, %d", dev, trunkId);
        }

        /*
            1.5. Try to add invalid device id and legal port id as trunk entry member.
            Call with trunk id [2], non-NULL memberPtr [invalid dev id, virtual port 8].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        enable = GT_TRUE;

        member.device = PRV_CPSS_MAX_PP_DEVICES_CNS;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkDesignatedMemberSet(dev, trunkId, enable, &member);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->device = %d",
                                         dev, trunkId, member.device);

        /*
            1.6. Try to add legal device id and invalid port id as trunk entry member.
            Call with trunk id [2], non-NULL memberPtr [tested dev id, invalid port].
            Expected: NON GT_OK.
        */
        member.device = hwDevNum;
        member.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        enable = GT_TRUE;

        st = cpssDxChTrunkDesignatedMemberSet(dev, trunkId, enable, &member);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, memberPtr->port = %d",
                                         dev, trunkId, member.port);

        /*
            1.7. Check for NULL handling. Call with trunk id [2], memberPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        enable = GT_TRUE;

        st = cpssDxChTrunkDesignatedMemberSet(dev, trunkId, enable, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, memberPtr = NULL", dev, trunkId);

        /*
            1.8. Call with null trunk id [NULL_TRUNK_ID_CNS = 0],
            non-NULL memberPtr [tested dev, virtual port 8].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssDxChTrunkDesignatedMemberSet(dev, trunkId, enable, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.9. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],
            non-NULL memberPtr [tested dev, virtual port 8].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkDesignatedMemberSet(dev, trunkId, enable, &member);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.10. Call cpssDxChTrunkMemberRemove with trunkId [2],
            memberPtr [tested dev id, virtual port 8] to remove added member from the trunk.
            Expected: GT_OK
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssDxChTrunkMemberRemove: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    member.device = 0;
    member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. Call function for non active device and valid parameters.   */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkDesignatedMemberSet(dev, trunkId, enable, &member);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkDesignatedMemberSet(dev, trunkId, enable, &member);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkDbDesignatedMemberGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    OUT GT_BOOL                 *enablePtr,
    OUT CPSS_TRUNK_MEMBER_STC   *memberPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkDbDesignatedMemberGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with trunk id [2], non-NULL memberPtr and enablePtr.
    Expected: GT_OK.
    1.2. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],
    Expected: NON GT_OK.
    1.3. Check for NULL handling. Call with trunk id [2], enablePtr [NULL].
    Expected: GT_BAD_PTR.
    1.4. Check for NULL handling. Call with trunk id [2], memberPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    GT_BOOL                 enableGet = GT_TRUE;
    CPSS_TRUNK_MEMBER_STC   memberGet;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkDbDesignatedMemberGet(dev, trunkId, &enableGet, &memberGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.2. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkDbDesignatedMemberGet(dev, trunkId, &enableGet, &memberGet);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.3. Check for NULL handling. Call with trunk id [2], enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTrunkDbDesignatedMemberGet(dev, trunkId, NULL, &memberGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, trunkId);

        /*
            1.4. Check for NULL handling. Call with trunk id [2], memberPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTrunkDbDesignatedMemberGet(dev, trunkId, &enableGet, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, memberPtr = NULL", dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. Call function for non active device and valid parameters.   */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkDbDesignatedMemberGet(dev, trunkId, &enableGet, &memberGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkDbDesignatedMemberGet(dev, trunkId, &enableGet, &memberGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
    Test function to Fill trunk table.
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkFillTrunkTable)
{
/*
    ITERATE_DEVICE (DxChx)
    1.1. Get table Size. numEntries [PRV_CPSS_DEV_TRUNK_INFO_MAC(dev)->numTrunksSupportedHw + 1].
    1.2. Fill all entries in trunk table.
            Call cpssDxChTrunkTableEntrySet with trunkId [0... numEntries-1], numMembers [1], membersArray [0 / 1 / 2] {port [0 / 1 / 2], device [devNum]}.
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
            Call cpssDxChTrunkTableEntrySet with trunkId [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in trunk table and compare with original.
            Call with cpssDxChTrunkTableEntryGet with trunkId [0... numEntries-1], non-NULL numMembersPtr, membersArray.
    Expected: GT_OK, numMembersPtr [1], and the same membersArray as was set.
    1.5. Try to read entry with index out of range.
            Call with cpssDxChTrunkTableEntryGet with trunkId [numEntries], non-NULL numMembersPtr, membersArray.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;
    GT_U32      numEntries  = 0;
    GT_U32      iTemp       = 0;

    GT_U32                  numMembers = 0;
    CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U32                  numMembersWritten = 0;
    CPSS_TRUNK_MEMBER_STC   membersArrayWritten[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_TRUNK_ID             trunkId;
    GT_U8                   hwDevNum;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = PRV_CPSS_DEV_TRUNK_INFO_MAC(dev)->numTrunksSupportedHw + 1;

        if(prvUtfDxChTrunkWaNeeded == GT_TRUE)
        {
            /* only 30 trunks supported
                + 2 cascade trunks 126,127
                  if PRV_CPSS_DEV_TRUNK_INFO_MAC(dev)->numTrunksSupportedHw == 127
            */
            numEntries = 31;
        }

        hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(dev);

        /* Fill the entry for trunk table */
        numMembers = 1;
        membersArray[0].device = hwDevNum;
        membersArray[0].port = 0;
        membersArray[1].device = hwDevNum;
        membersArray[1].port = 1;
        membersArray[2].device = hwDevNum;
        membersArray[2].port = 2;

        /* 1.2. Fill all entries in trunk table */
        for(iTemp = 1; iTemp < numEntries; ++iTemp)
        {
            trunkId = (GT_TRUNK_ID)iTemp;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
            st = cpssDxChTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChTrunkTableEntrySet: %d, %d, %d", dev, iTemp, numMembers);
        }

        if(prvUtfDxChTrunkWaNeeded == GT_TRUE &&
           PRV_CPSS_DEV_TRUNK_INFO_MAC(dev)->numTrunksSupportedHw == 127)
        {
            /* only 30 trunks supported
                + 2 cascade trunks 126,127
                  if PRV_CPSS_DEV_TRUNK_INFO_MAC(dev)->numTrunksSupportedHw == 127
            */
            numEntries =
                PRV_CPSS_DEV_TRUNK_INFO_MAC(dev)->numTrunksSupportedHw + 1;
            /* 1.2. Fill all entries in trunk table */
            for(iTemp = 126; iTemp < numEntries; ++iTemp)
            {
                trunkId = (GT_TRUNK_ID)iTemp;
                CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
                st = cpssDxChTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChTrunkTableEntrySet: %d, %d, %d", dev, iTemp, numMembers);
            }
        }

        numEntries =
           PRV_CPSS_DEV_TRUNK_INFO_MAC(dev)->numTrunksSupportedHw + 1;
        trunkId = (GT_TRUNK_ID)numEntries;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        /* 1.3. Try to write entry with index out of range. */
        st = cpssDxChTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssDxChTrunkTableEntrySet: %d, %d, %d", dev, numEntries, numMembers);

        if(prvUtfDxChTrunkWaNeeded == GT_TRUE)
        {
            /* only 30 trunks supported
                + 2 cascade trunks 126,127
                  if PRV_CPSS_DEV_TRUNK_INFO_MAC(dev)->numTrunksSupportedHw == 127
            */
            numEntries = 31;
        }

        /* 1.4. Read all entries in trunk table and compare with original */
        for(iTemp = 1; iTemp < numEntries; ++iTemp)
        {
            trunkId = (GT_TRUNK_ID)iTemp;
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

            st = cpssDxChTrunkTableEntryGet(dev, trunkId, &numMembersWritten, membersArrayWritten);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTrunkTableEntryGet: %d, %d", dev, iTemp);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(numMembers, numMembersWritten,
                       "get another numMembersPtr than was set: %d, %d", dev, iTemp);

            st = cpssOsMemCmp((const GT_VOID*)membersArray, (const GT_VOID*)membersArrayWritten,
                                numMembers*sizeof(membersArray[0]));
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "get another membersArray than was set: %d, %d", dev, iTemp);
        }

        if(prvUtfDxChTrunkWaNeeded == GT_TRUE &&
           PRV_CPSS_DEV_TRUNK_INFO_MAC(dev)->numTrunksSupportedHw == 127)
        {
            /* only 30 trunks supported
                + 2 cascade trunks 126,127
                  if PRV_CPSS_DEV_TRUNK_INFO_MAC(dev)->numTrunksSupportedHw == 127
            */
            numEntries =
               PRV_CPSS_DEV_TRUNK_INFO_MAC(dev)->numTrunksSupportedHw + 1;

            /* 1.4. Read all entries in trunk table and compare with original */
            for(iTemp = 126; iTemp < numEntries; ++iTemp)
            {
                trunkId = (GT_TRUNK_ID)iTemp;
                CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

                st = cpssDxChTrunkTableEntryGet(dev, trunkId, &numMembersWritten, membersArrayWritten);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTrunkTableEntryGet: %d, %d", dev, iTemp);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(numMembers, numMembersWritten,
                           "get another numMembersPtr than was set: %d, %d", dev, iTemp);

                st = cpssOsMemCmp((const GT_VOID*)membersArray, (const GT_VOID*)membersArrayWritten,
                                    numMembers*sizeof(membersArray[0]));
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "get another membersArray than was set: %d, %d", dev, iTemp);
            }
        }

        numEntries =
           PRV_CPSS_DEV_TRUNK_INFO_MAC(dev)->numTrunksSupportedHw + 1;

        trunkId = (GT_TRUNK_ID)numEntries;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        /* 1.5. Try to read entry with index out of range. */
        st = cpssDxChTrunkTableEntryGet(dev, trunkId, &numMembersWritten, membersArrayWritten);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChTrunkTableEntryGet: %d, %d", dev, numEntries);
    }
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashCrcParametersSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT     crcMode,
    IN GT_U32                               crcSeed
);
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashCrcParametersSet)
{
    GT_STATUS st              = GT_OK;

    GT_U8       dev;
    CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT  crcModes[] = {
                        CPSS_DXCH_TRUNK_LBH_CRC_6_MODE_E,
                        CPSS_DXCH_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E};
    CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT  crcMode = CPSS_DXCH_TRUNK_LBH_CRC_6_MODE_E;
    CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT  crcModeRet = CPSS_DXCH_TRUNK_LBH_CRC_6_MODE_E;
    GT_U32              crcSeed,crcSeedRet;
    GT_U32  numOfModes = sizeof(crcModes) / sizeof(crcModes[0]);
    GT_U32  ii;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        for(ii = 0 ; ii < numOfModes; ii++)
        {
            crcMode = crcModes[ii];

            if(crcMode == CPSS_DXCH_TRUNK_LBH_CRC_6_MODE_E)
            {
                crcSeed = BIT_6 - 5;
            }
            else /* crc 16 */
            {
                crcSeed = BIT_16 - 5;
            }

            st = cpssDxChTrunkHashCrcParametersSet(dev, crcMode , crcSeed);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.2. Call cpssDxChTrunkHashGlobalModeGet with non-NULL
               hashModePtr. Expected: GT_OK and same value as written.  */

            st = cpssDxChTrunkHashCrcParametersGet(dev, &crcModeRet , &crcSeedRet);
            UTF_VERIFY_EQUAL0_PARAM_MAC(crcModeRet, crcMode);
            UTF_VERIFY_EQUAL0_PARAM_MAC(crcSeedRet, crcSeed);

            if(crcMode == CPSS_DXCH_TRUNK_LBH_CRC_6_MODE_E)
            {
                crcSeed = BIT_6;
            }
            else /* crc 16 */
            {
                crcSeed = BIT_16;
            }

            st = cpssDxChTrunkHashCrcParametersSet(dev, crcMode , crcSeed);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OUT_OF_RANGE, st);
        }

        crcSeed = 5;/* make sure no errors on the crcSeed value */
        /*
           1.3. Call function with hashMode [wrong enum values].
           Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChTrunkHashCrcParametersSet(dev, crcMode , crcSeed),
                            crcMode);
    }

    crcMode = crcModes[0];
    crcSeed = 5;/* make sure no errors on the crcSeed value */

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashCrcParametersSet(dev, crcMode , crcSeed);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashCrcParametersSet(dev, crcMode , crcSeed);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashCrcParametersGet
(
    IN GT_U8                                devNum,
    OUT CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT     *crcModePtr,
    OUT GT_U32                               *crcSeedPtr
);
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashCrcParametersGet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with non-NULL enablePtr. Expected: GT_OK.
1.2. Call function with  [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st           = GT_OK;
    GT_U8     dev;
    CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT   crcMode;
    GT_U32              crcSeed;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL enablePtr.
           Expected: GT_OK. */

        st = cpssDxChTrunkHashCrcParametersGet(dev, &crcMode , &crcSeed);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with [NULL].
           Expected: GT_BAD_PTR.    */

        st = cpssDxChTrunkHashCrcParametersGet(dev, NULL , &crcSeed);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

        st = cpssDxChTrunkHashCrcParametersGet(dev, &crcMode , NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

        st = cpssDxChTrunkHashCrcParametersGet(dev, NULL , NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashCrcParametersGet(dev, &crcMode , &crcSeed);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashCrcParametersGet(dev, &crcMode , &crcSeed);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkPortHashMaskInfoSet
(
    IN GT_U8     devNum,
    IN GT_U8     portNum,
    IN GT_BOOL   overrideEnable,
    IN GT_U32    index
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkPortHashMaskInfoSet)
{
    GT_STATUS  st    = GT_OK;

    GT_U8     dev;
    GT_U8     port;
    GT_BOOL   overrideEnable,overrideEnableRet;
    GT_U32    index,indexRet;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            overrideEnable = GT_FALSE;
            index = 0xFFFFFFFF;/* should be ignored */

            st = cpssDxChTrunkPortHashMaskInfoSet(dev, port, overrideEnable , index);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            st = cpssDxChTrunkPortHashMaskInfoGet(dev, port, &overrideEnableRet , &indexRet);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            UTF_VERIFY_EQUAL0_PARAM_MAC(overrideEnable, overrideEnableRet);

            overrideEnable = GT_TRUE;

            for(index = 0 ; index < BIT_4 ; index++)
            {
                st = cpssDxChTrunkPortHashMaskInfoSet(dev, port, overrideEnable , index);
                UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

                st = cpssDxChTrunkPortHashMaskInfoGet(dev, port, &overrideEnableRet , &indexRet);
                UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

                UTF_VERIFY_EQUAL0_PARAM_MAC(overrideEnable, overrideEnableRet);
                UTF_VERIFY_EQUAL0_PARAM_MAC(index, indexRet);
            }

            for(/* continue*/ ; index < BIT_6 ; index++)
            {
                st = cpssDxChTrunkPortHashMaskInfoSet(dev, port, overrideEnable , index);
                UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OUT_OF_RANGE, st);
            }
        }

        overrideEnable = GT_FALSE;
        index = 0xFFFFFFFF;/* should be ignored */

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChTrunkPortHashMaskInfoSet(dev, port, overrideEnable , index);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTrunkPortHashMaskInfoSet(dev, port, overrideEnable , index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChTrunkPortHashMaskInfoSet(dev, port, overrideEnable , index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    overrideEnable = GT_FALSE;
    index = 0xFFFFFFFF;/* should be ignored */
    port = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkPortHashMaskInfoSet(dev, port, overrideEnable , index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkPortHashMaskInfoSet(dev, port, overrideEnable , index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkPortHashMaskInfoGet
(
    IN GT_U8     devNum,
    IN GT_U8     portNum,
    OUT GT_BOOL  *overrideEnablePtr,
    OUT GT_U32   *indexPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkPortHashMaskInfoGet)
{
    GT_STATUS  st    = GT_OK;

    GT_U8     dev;
    GT_U8     port;
    GT_BOOL   overrideEnable;
    GT_U32    index;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not NULL .
                Expected: GT_OK.
            */
            st = cpssDxChTrunkPortHashMaskInfoGet(dev, port, &overrideEnable , &index);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with null [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChTrunkPortHashMaskInfoGet(dev, port, NULL , &index);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

            st = cpssDxChTrunkPortHashMaskInfoGet(dev, port, &overrideEnable , NULL);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

            st = cpssDxChTrunkPortHashMaskInfoGet(dev, port, NULL , NULL);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChTrunkPortHashMaskInfoGet(dev, port, &overrideEnable , &index);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChTrunkPortHashMaskInfoGet(dev, port, &overrideEnable , &index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChTrunkPortHashMaskInfoGet(dev, port, &overrideEnable , &index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkPortHashMaskInfoGet(dev, port, &overrideEnable , &index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkPortHashMaskInfoGet(dev, port, &overrideEnable , &index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashMaskCrcEntrySet
(
    IN GT_U8                          devNum,
    IN GT_U32                         index,
    IN CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC *entryPtr
);
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashMaskCrcEntrySet)
{
    GT_STATUS st              = GT_OK;

    GT_U8       dev;
    /* the DB about the fields in the CRC hash mask entry.
        index to this table is according to fields in
        CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC */
    struct FIELD_INFO_STCT{
        GT_U32  length;
        GT_U32  offset;
    } hashMaskCrcFieldsInfo[]=
    {
        {2 ,0 }, /* l4DstPortMaskBmp;    */
        {2 ,0 }, /* l4SrcPortMaskBmp;    */
        {3 ,0 }, /* ipv6FlowMaskBmp;     */
        {16,0 }, /* ipDipMaskBmp;        */
        {16,0 }, /* ipSipMaskBmp;        */
        {6 ,0 }, /* macDaMaskBmp;        */
        {6 ,0 }, /* macSaMaskBmp;        */
        {3 ,0 }, /* mplsLabel0MaskBmp;   */
        {3 ,0 }, /* mplsLabel1MaskBmp;   */
        {3 ,0 }, /* mplsLabel2MaskBmp;   */
        {1 ,0 }, /* localSrcPortMaskBmp; */
        {9 ,14}, /* udbsMaskBmp;         */
    };
    GT_U32  numOfFields = sizeof(hashMaskCrcFieldsInfo) / sizeof(hashMaskCrcFieldsInfo[0]);
    GT_U32  index;
    CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC hashMaskEntry , hashMaskEntryRet;
    GT_U32  *fieldPtr;
    GT_U32  bit;/*bit index*/
    GT_U32  field;/*field index*/

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        for(index = 0 ; index < TRUNK_HASH_MASK_CRC_NUM_ENTRIES_CNS ; index ++)
        {
            cpssOsMemSet(&hashMaskEntry,0,sizeof(hashMaskEntry));

            fieldPtr = (GT_U32*)(void*)(&hashMaskEntry);
            for(field = 0 ; field < numOfFields; field++,fieldPtr++)
            {
                *fieldPtr = 0;

                for(bit = 1 ; bit < (hashMaskCrcFieldsInfo[field].length - 1) ; bit+=2)
                {
                    /* set a bit */
                    U32_SET_FIELD_MAC((*fieldPtr) , bit , 1, 1);
                }

                /* set the MSB */
                U32_SET_FIELD_MAC((*fieldPtr) , (hashMaskCrcFieldsInfo[field].length - 1) , 1, 1);

                (*fieldPtr) <<= hashMaskCrcFieldsInfo[field].offset;

                st = cpssDxChTrunkHashMaskCrcEntrySet(dev, index , &hashMaskEntry);
                UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

                /* 1.2. Call cpssDxChTrunkHashGlobalModeGet with non-NULL
                   hashModePtr. Expected: GT_OK and same value as written.  */

                st = cpssDxChTrunkHashMaskCrcEntryGet(dev, index , &hashMaskEntryRet);
                UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

                if(cpssOsMemCmp(&hashMaskEntryRet,&hashMaskEntry,sizeof(hashMaskEntry)))
                {
                    UTF_VERIFY_EQUAL0_PARAM_MAC(1, 0);/* set and get has different values */
                }
            }

            fieldPtr = (GT_U32*)(void*)(&hashMaskEntry);
            for(field = 0 ; field < numOfFields; field++,fieldPtr++)
            {
                *fieldPtr = 1 + BIT_MASK_MAC(hashMaskCrcFieldsInfo[field].length);

                st = cpssDxChTrunkHashMaskCrcEntrySet(dev, index , &hashMaskEntry);
                UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OUT_OF_RANGE, st);
            }

            st = cpssDxChTrunkHashMaskCrcEntrySet(dev, index , NULL);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);

        }
    }

    cpssOsMemSet(&hashMaskEntry,0,sizeof(hashMaskEntry));
    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashMaskCrcEntrySet(dev, index , &hashMaskEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashMaskCrcEntrySet(dev, index , &hashMaskEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashMaskCrcEntryGet
(
    IN GT_U8                          devNum,
    IN GT_U32                         index,
    OUT CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashMaskCrcEntryGet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with non-NULL enablePtr. Expected: GT_OK.
1.2. Call function with  [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st           = GT_OK;
    GT_U8     dev;
    GT_U32    index;
    CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC hashMaskEntry;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL enablePtr.
           Expected: GT_OK. */

        for(index = 0 ; index < TRUNK_HASH_MASK_CRC_NUM_ENTRIES_CNS ; index ++)
        {
            st = cpssDxChTrunkHashMaskCrcEntryGet(dev, index , &hashMaskEntry);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* 1.2. Call function with [NULL].
               Expected: GT_BAD_PTR.    */

            st = cpssDxChTrunkHashMaskCrcEntryGet(dev, index , NULL);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);
        }

        index = TRUNK_HASH_MASK_CRC_NUM_ENTRIES_CNS;

        st = cpssDxChTrunkHashMaskCrcEntryGet(dev, index , &hashMaskEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashMaskCrcEntryGet(dev, index , &hashMaskEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashMaskCrcEntryGet(dev, index , &hashMaskEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashPearsonValueSet
(
    IN GT_U8                          devNum,
    IN GT_U32                         index,
    IN GT_U32                         value
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashPearsonValueSet)
{
    GT_STATUS st              = GT_OK;

    GT_U8       dev;
    GT_U32      index    = 0;
    GT_U32      value    = 0;
    GT_U32      valueRet = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        for(index = 0 ; index < 64; index++)
        {
            for(value = 0 ; value < 64; value++)
            {
                st = cpssDxChTrunkHashPearsonValueSet(dev, index , value);
                UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

                /* 1.2. Call cpssDxChTrunkHashPearsonValueGet with non-NULL
                   hashModePtr. Expected: GT_OK and same value as written.  */

                st = cpssDxChTrunkHashPearsonValueGet(dev, index , &valueRet);
                UTF_VERIFY_EQUAL0_PARAM_MAC(valueRet, value);
            }

            st = cpssDxChTrunkHashPearsonValueSet(dev, index , value);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OUT_OF_RANGE, st);
        }

        st = cpssDxChTrunkHashPearsonValueSet(dev, index , value);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PARAM, st);
    }

    index = 0;
    value = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashPearsonValueSet(dev, index , value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashPearsonValueSet(dev, index , value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkHashPearsonValueGet
(
    IN GT_U8                          devNum,
    IN GT_U32                         index,
    OUT GT_U32                        *valuePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkHashPearsonValueGet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with non-NULL enablePtr. Expected: GT_OK.
1.2. Call function with  [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st           = GT_OK;
    GT_U8     dev;
    GT_U32      index;
    GT_U32      value;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL enablePtr.
           Expected: GT_OK. */

        for(index = 0 ; index < 64; index++)
        {
            st = cpssDxChTrunkHashPearsonValueGet(dev, index , &value);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* 1.2. Call function with [NULL].
               Expected: GT_BAD_PTR.    */

            st = cpssDxChTrunkHashPearsonValueGet(dev, index , NULL);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_BAD_PTR, st);
        }
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkHashPearsonValueGet(dev, index , &value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkHashPearsonValueGet(dev, index , &value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkCascadeTrunkPortsSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *portsMembersPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkCascadeTrunkPortsSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with trunkId [2],
                   valid portsMembersPtr.
    Expected: GT_OK.
    1.2. Call cpssDxChTrunkCascadeTrunkPortsGet with the same trunkId.
    Expected: GT_OK and the same portsMembersPtr.
    1.3. Call with null trunkId [NULL_TRUNK_ID_CNS = 0],
                   and other valid parameters from 1.1.
    Expected: NON GT_OK.
    1.4. Call with out of range trunkId [TRUNK_MAX_TRUNK_ID_CNS],
                   and other valid parameters from 1.1.
    Expected: NON GT_OK.
    1.5. Call with NULL portsMembersPtr,
                   and other valid parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_TRUNK_ID         trunkId;
    GT_BOOL             isEqual         = GT_FALSE;
    CPSS_PORTS_BMP_STC  portsMembers    = {{0, 0}};
    CPSS_PORTS_BMP_STC  portsMembersGet = {{0, 0}};


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunkId [2],
                           valid portsMembersPtr.
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
        {
            portsMembers.ports[0] = 0xAAAA & PRV_CPSS_PP_MAC(dev)->existingPorts.ports[0];
            portsMembers.ports[1] = 0;
        }
        else
        {
            portsMembers.ports[0] = 0xFFFFFFFF & PRV_CPSS_PP_MAC(dev)->existingPorts.ports[0];
            portsMembers.ports[1] = 0xFFFFFFFF & PRV_CPSS_PP_MAC(dev)->existingPorts.ports[1];
        }

        st = cpssDxChTrunkCascadeTrunkPortsSet(dev, trunkId, &portsMembers);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.2. Call cpssDxChTrunkCascadeTrunkPortsGet with the same trunkId.
            Expected: GT_OK and the same portsMembersPtr.
        */
        st = cpssDxChTrunkCascadeTrunkPortsGet(dev, trunkId, &portsMembersGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxChTrunkCascadeTrunkPortsGet: %d", dev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*)portsMembers.ports,
                                     (GT_VOID*)portsMembersGet.ports,
                                     sizeof(portsMembers.ports))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another value than was set: %d", dev);

        /*
            1.3. Call with null trunkId [NULL_TRUNK_ID_CNS = 0],
                           and other valid parameters from 1.1.
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkCascadeTrunkPortsSet(dev, trunkId, &portsMembers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.4. Call with out of range trunkId [TRUNK_MAX_TRUNK_ID_CNS],
                           and other valid parameters from 1.1.
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkCascadeTrunkPortsSet(dev, trunkId, &portsMembers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
        /*
            1.5. Call with NULL portsMembersPtr,
                           and other valid parameters from 1.1.
            Expected: GT_OK.
        */
        st = cpssDxChTrunkCascadeTrunkPortsSet(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, portsMembersPtr = NULL", dev);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    portsMembers.ports[0] = 0;
    portsMembers.ports[1] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkCascadeTrunkPortsSet(dev, trunkId, &portsMembers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkCascadeTrunkPortsSet(dev, trunkId, &portsMembers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkDbMcLocalSwitchingEnableGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    OUT GT_BOOL                 *enablePtr,
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkDbMcLocalSwitchingEnableGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with trunk id [TRUNK_TESTED_TRUNK_ID_CNS].
    Expected: GT_OK.
    1.2. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128].
    Expected: NON GT_OK.
    1.3. Check for NULL handling. Call with trunk id [2], enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    GT_BOOL                 enableGet = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunk id [TRUNK_TESTED_TRUNK_ID_CNS].
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkDbMcLocalSwitchingEnableGet(dev, trunkId, &enableGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.2. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkDbMcLocalSwitchingEnableGet(dev, trunkId, &enableGet);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.3. Check for NULL handling. Call with trunk id [2], enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTrunkDbMcLocalSwitchingEnableGet(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, 
                            "%d, %d, enablePtr = NULL", dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. Call function for non active device and valid parameters.   */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkDbMcLocalSwitchingEnableGet(dev, trunkId, &enableGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkDbMcLocalSwitchingEnableGet(dev, trunkId, &enableGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkMcLocalSwitchingEnableSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_BOOL                  enable,
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkMcLocalSwitchingEnableSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with trunk id [2] and enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call with null trunk id [NULL_TRUNK_ID_CNS = 0].
    Expected: NON GT_OK.
    1.3. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],
    Expected: NON GT_OK.
*/
    GT_STATUS     st = GT_OK;
    GT_U8         dev;
    GT_TRUNK_ID   trunkId;
    GT_BOOL       enable = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunk id [2] and enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        /* call with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxChTrunkMcLocalSwitchingEnableSet(dev, trunkId, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* call with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssDxChTrunkMcLocalSwitchingEnableSet(dev, trunkId, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.2. Call with null trunk id [NULL_TRUNK_ID_CNS = 0].
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkMcLocalSwitchingEnableSet(dev, trunkId, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.3. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkMcLocalSwitchingEnableSet(dev, trunkId, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. Call function for non active device and valid parameters.   */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssDxChTrunkMcLocalSwitchingEnableSet(dev, trunkId, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkMcLocalSwitchingEnableSet(dev, trunkId, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChTrunkCascadeTrunkPortsGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    OUT CPSS_PORTS_BMP_STC       *portsMembersPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChTrunkCascadeTrunkPortsGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with trunkId [2],
                   valid portsMembersPtr.
    Expected: GT_OK.
    1.2. Call with null trunkId [NULL_TRUNK_ID_CNS = 0],
                   and other valid parameters from 1.1.
    Expected: NON GT_OK.
    1.3. Call with out of range trunkId [TRUNK_MAX_TRUNK_ID_CNS],
                   and other valid parameters from 1.1.
    Expected: NON GT_OK.
    1.4. Call with NULL portsMembersPtr,
                   and other valid parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  portsMembers = {{0, 0}};


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with trunkId [2],
                           valid portsMembersPtr.
            Expected: GT_OK.
        */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkCascadeTrunkPortsGet(dev, trunkId, &portsMembers);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.2. Call with null trunkId [NULL_TRUNK_ID_CNS = 0],
                           and other valid parameters from 1.1.
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkCascadeTrunkPortsGet(dev, trunkId, &portsMembers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /*
            1.3. Call with out of range trunkId [TRUNK_MAX_TRUNK_ID_CNS],
                           and other valid parameters from 1.1.
            Expected: NON GT_OK.
        */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        st = cpssDxChTrunkCascadeTrunkPortsGet(dev, trunkId, &portsMembers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
        /*
            1.4. Call with NULL portsMembersPtr,
                           and other valid parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChTrunkCascadeTrunkPortsGet(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsMembersPtr = NULL", dev);
    }

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChTrunkCascadeTrunkPortsGet(dev, trunkId, &portsMembers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChTrunkCascadeTrunkPortsGet(dev, trunkId, &portsMembers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChTrunk high-level suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChTrunkHighLevel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkMembersSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkMemberAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkMemberRemove)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkMemberDisable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkMemberEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkDbEnabledMembersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkDbDisabledMembersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkDbIsMemberOfTrunk)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkDbTrunkTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkDesignatedMemberSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkDbDesignatedMemberGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkCascadeTrunkPortsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkCascadeTrunkPortsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkDbMcLocalSwitchingEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkMcLocalSwitchingEnableSet)
UTF_SUIT_END_TESTS_MAC(cpssDxChTrunkHighLevel)

/*
 * Configuration of cpssDxChTrunk low-level suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChTrunkLowLevel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkNonTrunkPortsAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkNonTrunkPortsRemove)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkPortTrunkIdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkPortTrunkIdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkTableEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkTableEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkNonTrunkPortsEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkNonTrunkPortsEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkDesignatedPortsEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkDesignatedPortsEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashDesignatedTableModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashDesignatedTableModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashGlobalModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashGlobalModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashIpAddMacModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashIpAddMacModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashIpModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashIpModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashL4ModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashL4ModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashIpv6ModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashIpv6ModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashIpShiftSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashIpShiftGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashMaskSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashMaskGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashMplsModeEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashMplsModeEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkInit)

    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashCrcParametersSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashCrcParametersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkPortHashMaskInfoSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkPortHashMaskInfoGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashMaskCrcEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashMaskCrcEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashPearsonValueSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkHashPearsonValueGet)

    /* Test for Table */
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChTrunkFillTrunkTable)
UTF_SUIT_END_TESTS_MAC(cpssDxChTrunkLowLevel)

